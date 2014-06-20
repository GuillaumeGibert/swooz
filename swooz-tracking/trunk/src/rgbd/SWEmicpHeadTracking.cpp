
/**
 * \file SWEmicpHeadTracking.cpp
 * \brief Defines SWEmicpHeadTracking class
 * \author Florian Lance
 * \date 09/12/13
 */

#include "rgbd/SWEmicpHeadTracking.h"
#include "SWTrackingDevice.h"
#include "interface/SWConvQtOpencv.h"
#include "cloud/SWImageProcessing.h"
#include "moc_SWEmicpHeadTracking.cpp"


// OPENCV
#include "opencvUtility.h"

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace swExcept;


// ########################### SWEmicpHeadTrackingWorker

SWEmicpHeadTrackingWorker::SWEmicpHeadTrackingWorker() : m_oCaptureHeadMotion(swCloud::SWCaptureHeadMotion(20,20)),
    m_bIsRGBDDeviceInitialized(true), m_bVerbose(false), m_bDoWork(true), m_i32Fps(100), m_pCurrentFaceRect(NULL), m_pCurrentNoseRect(NULL),
    m_pCurrentRigidMotion(NULL), m_pCurrCloud(NULL),m_pReferenceCloud(NULL), m_bWorkStopped(true)
{        
    // set yarp port name
        std::string l_sDeviceName   = "rgbd";
        std::string l_sLibraryName  = "emicp";
        m_sHeadTrackingPortName     =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/head";
        m_oHeadTrackingPort.open(m_sHeadTrackingPortName.c_str());

    // init rgbd device
        if(m_oKinectThread.init(0) == -1)
        {
            m_bIsRGBDDeviceInitialized = false;
        }

        if(m_bIsRGBDDeviceInitialized)
        {
            try
            {
                m_oKinectThread.startListening();
            }
            catch(const kinectInitError &e)
            {
                std::cerr << "-ERROR : " << e.what() << std::endl;
                m_bIsRGBDDeviceInitialized = false;
            }

            while(!m_oKinectThread.isDataAvailable())
            {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            }

            std::cout << "RGBD data is ready. " << std::endl;

            cv::Mat l_oRGB   = m_oKinectThread.bgrImage();
            cv::Mat l_oCloud = m_oKinectThread.cloudMap();
            m_CKinectParams = swDevice::SWKinectParams(m_oKinectThread.captureMode(), cv::Size(l_oRGB.cols, l_oCloud.rows));
            m_oKinectThread.setRecalibration(true, m_CKinectParams.m_i32OffsetRgbX, m_CKinectParams.m_i32OffsetRgbY);
        }
//        else
//        {
//            emit leaveProgram();
//        }

}

SWEmicpHeadTrackingWorker::~SWEmicpHeadTrackingWorker()
{
    m_oKinectThread.stopListening();
    deleteAndNullify(m_pReferenceCloud);
    deleteAndNullify(m_pCurrCloud);
    deleteAndNullify(m_pCurrentFaceRect);
    deleteAndNullify(m_pCurrentNoseRect);
    deleteAndNullify(m_pCurrentRigidMotion);
}

void SWEmicpHeadTrackingWorker::updateInterfaceParameters(cdouble dTemplateCoeffReduc, cdouble dTargetCoeffReduc, cdouble dScoreComputingReduc,
                               cint i32K, cdouble dKTransSmooth, cdouble dKRotSmooth,
                               cdouble dP2, cdouble dINF, cdouble dFactor, cdouble dD02)
{
    m_oParametersMutex.lockForWrite();
        m_oCaptureHeadMotion.setParameters(dTemplateCoeffReduc,dTargetCoeffReduc, dScoreComputingReduc,i32K,dKTransSmooth,dKRotSmooth,dP2, dINF, dFactor, dD02);
    m_oParametersMutex.unlock();
}

void SWEmicpHeadTrackingWorker::doWork()
{
    if(!m_bIsRGBDDeviceInitialized)
    {
        stopWork();
        return;
    }

    bool l_bContinueLoop = true;
    m_bDoWork     = true;
    m_bWorkStopped= false;

    while(l_bContinueLoop)
    {
        // start timer
            clock_t l_oFirstTime = clock();

        // wait (necessary to get the events)
            QTime l_oDieTime = QTime::currentTime().addMSecs(3);
            while( QTime::currentTime() < l_oDieTime)
            {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 3);
            }

        // tracking
            cv::Mat l_oBGR   = m_oKinectThread.bgrImage();
            cv::Mat l_oCloud = m_oKinectThread.cloudMap();

        // resize the rgb mat
            if(m_CKinectParams.m_oOriginalSize != m_CKinectParams.m_oVideoSize)
            {
                resize(l_oBGR, l_oBGR, m_CKinectParams.m_oVideoSize);
            }

        // check if the loop must be stopped
            m_oLoopMutex.lockForRead();
                l_bContinueLoop = m_bDoWork;
            m_oLoopMutex.unlock();

        // launch head motion computing
            cv::Mat l_oRGBDetect;
            swCloud::SWRigidMotion l_oRigidMotion;

            // DEBUG
//                qDebug() << "launch head motion computing : " << (float)(clock() - l_oFirstTime) / CLOCKS_PER_SEC;

            m_oParametersMutex.lockForRead();
                int l_i32Res = m_oCaptureHeadMotion.computeHeadMotion(l_oRigidMotion, l_oBGR, l_oCloud, l_oRGBDetect);
            m_oParametersMutex.unlock();

            // DEBUG
//                qDebug() << "end head motion computing : " << (float)(clock() - l_oFirstTime) / CLOCKS_PER_SEC;

            if(l_i32Res == -1)
            {
                std::cerr << "ERROR : Capture head motion, invalid result, neutral rigid motion used. " << std::endl;
            }

            m_oCurrentRigidMotion = l_oRigidMotion;

        // send yarp data
            // head bottle
            Bottle &l_oHeadBottle = m_oHeadTrackingPort.prepare();
            l_oHeadBottle.clear();

                // device lib id
                l_oHeadBottle.addInt(swTracking::EMICP_LIB); //head : EMICP_LIB id / get(0).asInt()

                // head translation
                l_oHeadBottle.addDouble(m_oCurrentRigidMotion.m_aFTranslation[0]); //head: Tx / get(1).asDouble()
                l_oHeadBottle.addDouble(m_oCurrentRigidMotion.m_aFTranslation[1]); //head: Ty / get(2).asDouble()
                l_oHeadBottle.addDouble(m_oCurrentRigidMotion.m_aFTranslation[2]); //head: Tz / get(3).asDouble()

                // head rotation
                l_oHeadBottle.addDouble(m_oCurrentRigidMotion.m_aFRotAngles[0]); //head: Rx / get(4).asDouble()
                l_oHeadBottle.addDouble(m_oCurrentRigidMotion.m_aFRotAngles[1]); //head: Ry / get(5).asDouble()
                l_oHeadBottle.addDouble(m_oCurrentRigidMotion.m_aFRotAngles[2]); //head: Rz / get(6).asDouble()

                if(m_bVerbose)
                {
                    std::cout << "TR : " << l_oHeadBottle.get(1).asDouble() << " " << l_oHeadBottle.get(2).asDouble() << " " << l_oHeadBottle.get(3).asDouble() << std::endl;
                    std::cout << "RO : " << l_oHeadBottle.get(3).asDouble() << " " << l_oHeadBottle.get(4).asDouble() << " " << l_oHeadBottle.get(5).asDouble() << std::endl << std::endl;
                }

            m_oHeadTrackingPort.write();


        // compute total delay between the getting of the kinect data and the send of the bottle conainting the rigid motion
            float l_fDelay = (float)(clock() - l_oFirstTime) / CLOCKS_PER_SEC;

            // display
            if(l_i32Res == 0)
            {
                deleteAndNullify(m_pReferenceCloud);
                m_pReferenceCloud = new swCloud::SWCloud();
                m_pReferenceCloud->copy(m_oCaptureHeadMotion.debugFaceCloudRef());
                m_pReferenceCloud->setUnicolor(0,0,255);
                m_pReferenceCloud->reduce(0.5f);
            }

            if(l_i32Res == 1)
            {
                deleteAndNullify(m_pCurrCloud);
                m_pCurrCloud = new swCloud::SWCloud();
                m_pCurrCloud->copy(m_oCaptureHeadMotion.debugTransformedFaceCloud());
                m_pCurrCloud->setUnicolor(0,255,0);
                m_pCurrCloud->reduce(0.5f);
                (*m_pCurrCloud) += (*m_pReferenceCloud);
            }

            // retrieve rectangles
                cv::Rect l_oFaceRect, l_oNoseRect;
                m_oCaptureHeadMotion.getRect(l_oFaceRect, l_oNoseRect);

                deleteAndNullify(m_pCurrentFaceRect);
                deleteAndNullify(m_pCurrentNoseRect);
                m_pCurrentFaceRect = new cv::Rect(l_oFaceRect);
                m_pCurrentNoseRect = new cv::Rect(l_oNoseRect);

            // send the cloud and the rectangles to the interface for displaying
                emit sendFaceRect(m_pCurrentFaceRect);
                emit sendNoseRect(m_pCurrentNoseRect);
                emit sendCloudToDisplay(m_pCurrCloud);

                deleteAndNullify(m_pCurrentRigidMotion);

                m_pCurrentRigidMotion = new swCloud::SWRigidMotion(m_oCurrentRigidMotion);
                emit sendRigidMotion(m_pCurrentRigidMotion);

            // send the delay to be displayed in a widget
                emit sendDelay(l_fDelay);
    }

    m_oCaptureHeadMotion.reset();
    m_bWorkStopped = true;
}

void SWEmicpHeadTrackingWorker::stopWork()
{
    m_oLoopMutex.lockForWrite();
        m_bDoWork = false;
    m_oLoopMutex.unlock();
}


void SWEmicpHeadTrackingWorker::clean()
{
    m_oHeadTrackingPort.interrupt();

    m_oKinectThread.stopListening();
    m_oHeadTrackingPort.close();

    Network::fini();
}


void SWEmicpHeadTrackingWorker::configure(yarp::os::ResourceFinder &pRF)
{
    if(m_bVerbose)
    {
        std::cout << "Configure Emicp head tracking module. " << std::endl;
    }

    // ...
}

// ########################### SWEmicpHeadTrackingInterface

SWEmicpHeadTrackingInterface::SWEmicpHeadTrackingInterface() : m_uiMainWindow(new Ui::SWUI_WEmicpHeadTracking()), m_oTimer(new QBasicTimer), m_fDelay(0.f)
{
    // init Qt interface
        // add Qt meta types
        qRegisterMetaType<cv::Mat>("cv::Rect*");
        qRegisterMetaType<swCloud::SWRigidMotion*>("SWRigidMotion");
        qRegisterMetaType<swCloud::SWCloud*>("SWCloud");

        // init main widget
            m_uiMainWindow->setupUi(this);
            this->setWindowTitle(QString("SWoOz : EMICP head tracking"));

        // init widgets
            m_pDisplayImageWidget = new SWDisplayImageWidget();

            QGLFormat l_glFormat;
            l_glFormat.setVersion( 4, 3 );
            l_glFormat.setProfile(  QGLFormat::CompatibilityProfile);
            l_glFormat.setSampleBuffers( true );
            QGLContext *l_glContext = new QGLContext(l_glFormat);
            m_pGLCloudWidget        = new SWGLCloudWidget(l_glContext, this);
            m_pGLCloudWidget->setCamera(QVector3D(0.f, 0.0f, 0.6f), QVector3D(0.f, 0.0f,  1.f), QVector3D(0.f, 1.f,  0.f), false);

            std::vector<std::string> l_aSRotationsLabel;
            l_aSRotationsLabel.push_back("rX");
            l_aSRotationsLabel.push_back("rY");
            l_aSRotationsLabel.push_back("rZ");
            std::vector<std::string> l_aSTranslationsLabel;
            l_aSTranslationsLabel.push_back("tX");
            l_aSTranslationsLabel.push_back("tY");
            l_aSTranslationsLabel.push_back("tZ");
            m_pDisplayHistoRotWidget   = new SWDisplayCurvesWidget(this, l_aSRotationsLabel,    QSize(600, 250), 2.f, 100, 30, 2);
            m_pDisplayHistoTransWidget = new SWDisplayCurvesWidget(this, l_aSTranslationsLabel, QSize(600, 250), 200.f, 100, 30, 2);

            m_uiMainWindow->hlVideo->addWidget(m_pDisplayImageWidget);
            m_uiMainWindow->hlClouds->addWidget(m_pGLCloudWidget);
            m_uiMainWindow->hlRotations->addWidget(m_pDisplayHistoRotWidget);
            m_uiMainWindow->hlTranslations->addWidget(m_pDisplayHistoTransWidget);

            std::vector<float> l_aSRotationsInitValues(3, 0.f);
            std::vector<float> l_aSTranslationsInitValues(3, 0.f);
            m_pDisplayHistoRotWidget->setNewValues(l_aSRotationsInitValues);
            m_pDisplayHistoTransWidget->setNewValues(l_aSTranslationsInitValues);

        // init worker
            m_pWTracking = new SWEmicpHeadTrackingWorker();

        // init connections
            QObject::connect(m_uiMainWindow->pbStart,SIGNAL(clicked()),      m_pWTracking, SLOT(doWork()));
            QObject::connect(m_uiMainWindow->pbStop, SIGNAL(clicked()),      m_pWTracking, SLOT(stopWork()));
            QObject::connect(this,                   SIGNAL(stopModule()),   m_pWTracking, SLOT(stopWork()));
            QObject::connect(this,                   SIGNAL(cleanModule()),  m_pWTracking, SLOT(clean()));
            QObject::connect(m_pWTracking,           SIGNAL(leaveProgram()), this,         SLOT(close()));
            QObject::connect(m_pWTracking,           SIGNAL(sendCloudToDisplay(swCloud::SWCloud*)), this, SLOT(updateCloudDisplay(swCloud::SWCloud*)));
            QObject::connect(m_pWTracking,           SIGNAL(sendFaceRect(cv::Rect*)), this, SLOT(updateFaceRectangle(cv::Rect*)));
            QObject::connect(m_pWTracking,           SIGNAL(sendNoseRect(cv::Rect*)), this, SLOT(updateNoseRectangle(cv::Rect*)));
            QObject::connect(m_pWTracking,           SIGNAL(sendRigidMotion(swCloud::SWRigidMotion*)), this, SLOT(updateHistogramDisplay(swCloud::SWRigidMotion*)));
            QObject::connect(m_pWTracking,           SIGNAL(sendDelay(float)),        this, SLOT(updateDelay(float)));

            QObject::connect(m_uiMainWindow->dsbTemplateCloudReduction,     SIGNAL(valueChanged(double)), this, SLOT(updateInterfaceValues(double)));
            QObject::connect(m_uiMainWindow->dsbTargetCloudReduction,       SIGNAL(valueChanged(double)), this, SLOT(updateInterfaceValues(double)));
            QObject::connect(m_uiMainWindow->dsbScoreComputingReduction,    SIGNAL(valueChanged(double)), this, SLOT(updateInterfaceValues(double)));
            QObject::connect(m_uiMainWindow->dsbP2,                         SIGNAL(valueChanged(double)), this, SLOT(updateInterfaceValues(double)));
            QObject::connect(m_uiMainWindow->dsbINF,                        SIGNAL(valueChanged(double)), this, SLOT(updateInterfaceValues(double)));
            QObject::connect(m_uiMainWindow->dsbFactor,                     SIGNAL(valueChanged(double)), this, SLOT(updateInterfaceValues(double)));
            QObject::connect(m_uiMainWindow->dsbD02,                        SIGNAL(valueChanged(double)), this, SLOT(updateInterfaceValues(double)));
            QObject::connect(m_uiMainWindow->sbKSmooth,                     SIGNAL(valueChanged(int)),    this, SLOT(updateInterfaceValues(int)));
            QObject::connect(m_uiMainWindow->dsbKTransSmoothConst,          SIGNAL(valueChanged(double)), this, SLOT(updateInterfaceValues(double)));
            QObject::connect(m_uiMainWindow->dsbKRotSmoothConst,            SIGNAL(valueChanged(double)), this, SLOT(updateInterfaceValues(double)));
            QObject::connect(m_uiMainWindow->cbSmoothRigidMotion,           SIGNAL(stateChanged(int)),    this, SLOT(updateInterfaceValues(int)));

            updateInterfaceValues(0);

        // init thread
            m_pWTracking->moveToThread(&m_TTracking);
            m_TTracking.start();

        // init kinect thread used by the display widget
            if(m_oKinectThread.init(0) != -1)
            {
                try
                {
                    m_oKinectThread.startListening();

                    while(!m_oKinectThread.isDataAvailable())
                    {
                        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                    }

                    cv::Mat l_oRGB   = m_oKinectThread.bgrImage();
                    cv::Mat l_oCloud = m_oKinectThread.cloudMap();
                    m_CKinectParams = swDevice::SWKinectParams(m_oKinectThread.captureMode(), cv::Size(l_oRGB.cols, l_oCloud.rows));
                    m_oKinectThread.setRecalibration(true, m_CKinectParams.m_i32OffsetRgbX, m_CKinectParams.m_i32OffsetRgbY);
                }
                catch(const kinectInitError &e)
                {
                    std::cerr << "-ERROR : " << e.what() << std::endl;
                }

                // launch timer for updating the display widget
                    m_oTimer->start(1000/30, this);
            }
            else
            {
                std::cerr << "Leave program, please check if a kinect/xtion is plugged.  " << std::endl;
                QTimer::singleShot(0, this, SLOT(close()));
            }

}

SWEmicpHeadTrackingInterface::~SWEmicpHeadTrackingInterface()
{
    m_TTracking.quit();
    m_TTracking.wait();

    deleteAndNullify(m_pWTracking);
}

void SWEmicpHeadTrackingInterface::closeEvent(QCloseEvent *event)
{
    emit stopModule();

    QTime l_oDieTime = QTime::currentTime().addMSecs(1000);
    while( QTime::currentTime() < l_oDieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    }

    emit cleanModule();
}

void SWEmicpHeadTrackingInterface::configure(ResourceFinder &oRf)
{
    m_pWTracking->configure(oRf);
}

void SWEmicpHeadTrackingInterface::timerEvent(QTimerEvent *e)
{
    // parameter "e" not used in the body of the function
    Q_UNUSED(e);
    updateImageDisplay();
}

void SWEmicpHeadTrackingInterface::updateCloudDisplay(swCloud::SWCloud *pCloud)
{
    if(pCloud)
    {
        if(pCloud->size() > 0)
        {
            m_pGLCloudWidget->setCloud(pCloud);
        }
    }
}

void SWEmicpHeadTrackingInterface::updateFaceRectangle(cv::Rect *pFaceRect)
{
    m_oMutex.lockForWrite();
        m_oFaceRect = cv::Rect(*pFaceRect);
    m_oMutex.unlock();
}

void SWEmicpHeadTrackingInterface::updateNoseRectangle(cv::Rect *pNoseRect)
{
    m_oMutex.lockForWrite();
        m_oNoseRect = cv::Rect(*pNoseRect);
    m_oMutex.unlock();
}

void SWEmicpHeadTrackingInterface::updateHistogramDisplay(swCloud::SWRigidMotion *pRigidMotion)
{
    std::vector<float> l_aRotNewValues(3),l_aTransNewValues(3);
    for(int ii = 0; ii < 3; ++ii)
    {
        l_aRotNewValues[ii]   = pRigidMotion->m_aFRotAngles[ii];
        l_aTransNewValues[ii] = pRigidMotion->m_aFTranslation[ii];
    }

    m_pDisplayHistoRotWidget->setNewValues(l_aRotNewValues);
    m_pDisplayHistoTransWidget->setNewValues(l_aTransNewValues);
}

void SWEmicpHeadTrackingInterface::updateImageDisplay()
{
    // get the current image from the kinect
    cv::Mat l_oRgb = m_oKinectThread.bgrImage();
    std::string l_sDelay("D ");
    std::ostringstream l_osDelay;


    // apply a filter on the zones where detection could fail

    for(int ii = 0; ii < l_oRgb.rows/4; ++ii)
    {
        for(int jj = 0; jj < l_oRgb.cols; ++jj)
        {
//            cv::Vec3b l_oColValueUp   = l_oRgb.at<cv::Vec3b>(ii,jj);
//            cv::Vec3b l_oColValueDown = l_oRgb.at<cv::Vec3b>(l_oRgb.rows -1 - ii,jj);

//            for(int kk = 0; kk < 3; ++kk)
//            {
//                if(l_oColValueUp[kk] < 155)
//                {
//                    l_oColValueUp[kk] += 100;
//                }
//                else
//                {
//                    l_oColValueUp[kk] = 255;
//                }

//                if(l_oColValueDown[kk] < 155)
//                {
//                    l_oColValueDown[kk] += 100;
//                }
//                else
//                {
//                    l_oColValueDown[kk] = 255;
//                }
//            }

//            l_oRgb.at<cv::Vec3b>(ii,jj)                   = l_oColValueUp;
//            l_oRgb.at<cv::Vec3b>(l_oRgb.rows - 1 - ii,jj) = l_oColValueDown;

            l_oRgb.at<cv::Vec3b>(ii,jj)                   = cv::Vec3b(0,0,0);
            l_oRgb.at<cv::Vec3b>(l_oRgb.rows - 1 - ii,jj) = cv::Vec3b(0,0,0);
        }
    }


    // apply rectangles used for the rigid motion computing on the cv mat image
    m_oMutex.lockForRead();
        cv::Rect l_oFaceRectangle = m_oFaceRect;
        cv::Rect l_oNoseRectangle = m_oNoseRect;
        l_osDelay  << m_fDelay;
    m_oMutex.unlock();            

    if(swUtil::isInside(l_oFaceRectangle, l_oRgb))
    {
        cv::rectangle(l_oRgb, cv::Point(l_oFaceRectangle.x, l_oFaceRectangle.y),
            cv::Point(l_oFaceRectangle.x+l_oFaceRectangle.width, l_oFaceRectangle.y+l_oFaceRectangle.height), RED,1);
    }

    if(swUtil::isInside(l_oNoseRectangle,l_oRgb))
    {
        cv::rectangle(l_oRgb, cv::Point(l_oNoseRectangle.x, l_oNoseRectangle.y),
                cv::Point(l_oNoseRectangle.x + l_oNoseRectangle.width, l_oNoseRectangle.y + l_oNoseRectangle.height), GREEN,1);
    }

    l_sDelay += l_osDelay.str();
    if(l_sDelay.size()==6)
    {
        l_sDelay += std::string("0");
    }
    swUtil::displayTextDownRight(l_oRgb, l_sDelay);

    m_pDisplayImageWidget->refreshDisplay(swConv::mat2QImage(l_oRgb));
}


void SWEmicpHeadTrackingInterface::updateInterfaceValues(int i32Value)
{
    updateInterfaceValues(0.);
}

void SWEmicpHeadTrackingInterface::updateInterfaceValues(double dValue)
{
    // alignement params
        // reduction
        double l_dTemplateCoeffReduction  = m_uiMainWindow->dsbTemplateCloudReduction->value();
        double l_dTargetCoeffReduction    = m_uiMainWindow->dsbTargetCloudReduction->value();
        double l_dScoreComputingReduction = m_uiMainWindow->dsbScoreComputingReduction->value();
        // smoothing
        int l_i32K              = (m_uiMainWindow->cbSmoothRigidMotion->checkState() == Qt::Checked) ? m_uiMainWindow->sbKSmooth->value() : 0;
        double l_dKTransSmooth  = m_uiMainWindow->dsbKTransSmoothConst->value();
        double l_dKRotSmooth    = m_uiMainWindow->dsbKRotSmoothConst->value();

        // emicp
        double l_dP2     = m_uiMainWindow->dsbP2->value();
        double l_dINF    = m_uiMainWindow->dsbINF->value();
        double l_dFactor = m_uiMainWindow->dsbFactor->value();
        double l_dD02    = m_uiMainWindow->dsbD02->value();        

        m_pWTracking->updateInterfaceParameters(l_dTemplateCoeffReduction,l_dTargetCoeffReduction,l_dScoreComputingReduction,
                                                l_i32K, l_dKTransSmooth, l_dKRotSmooth,
                                                l_dP2, l_dINF, l_dFactor, l_dD02
                                                );
}

void SWEmicpHeadTrackingInterface::updateDelay(float fDelay)
{
    m_oMutex.lockForWrite();
        m_fDelay = fDelay;
    m_oMutex.unlock();
}


int main(int argc, char* argv[])
{
    // initialize yarp network
        Network l_oYarp;
        if (!l_oYarp.checkNetwork())
        {
            std::cerr << "-ERROR: Problem connecting to YARP server" << std::endl;
            return -1;
        }

    // create module
        QApplication l_oApp(argc, argv);
        SWEmicpHeadTrackingInterface l_oEmicpHeadTrackingInterface;
        l_oEmicpHeadTrackingInterface.resize(QSize(1200,950));
        l_oEmicpHeadTrackingInterface.move(50,50);
        l_oEmicpHeadTrackingInterface.show();

    // prepare and configure the resource finder
//        ResourceFinder rf;
//            rf.setVerbose(true);
//            rf.setDefaultConfigFile("emicpHeadTracking.ini");
//            rf.setDefaultContext("swtracking/conf");
//            rf.configure("ICUB_ROOT", argc, argv);

    // configure the module
//        std::cout << "Configuring the Emicp Head tracking module..."<< std::endl;
//            l_oEmicpHeadTrackingInterface.configure(rf);

    return l_oApp.exec();
}

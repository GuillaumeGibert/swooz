
/**
 * \file SWCreateAvatarInterface.cpp
 * \author LANCE Florian
 * \date 21/01/13
 * \brief definition of SWCreateAvatarInterface
 */

#include "interface/SWCreateAvatarInterface.h"
#include "interface/SWConvQtOpencv.h"

#include "cloud/SWConvCloud.h"

#include "moc_SWCreateAvatarInterface.cpp"

#include "interface/SWConvQtOpencv.h"
#include "cloud/SWImageProcessing.h"


SWCreateAvatarInterface::SWCreateAvatarInterface(QWidget *oQWRelative) : QMainWindow(oQWRelative), m_uiCreateAvatar(new Ui::SWUI_WCreateAvatar),
//  , m_bFaceRectInitialise(false), m_bDetectionStopped(false)
  m_oTimer(new QBasicTimer)
{
    m_pCloudToDisplay = NULL;
    m_bWorkStarted = false;

    // init rects
        m_oFaceRect.width = 0;
        m_oNoseRect.width = 0;

	// init main widget
    m_uiCreateAvatar->setupUi(this);
	this->setWindowTitle(QString("SWoOz : Create avatar"));

	// init opengl context
    QGLFormat l_glFormat;
    l_glFormat.setVersion( 4, 3 );
    l_glFormat.setProfile(QGLFormat::CompatibilityProfile);
    l_glFormat.setSampleBuffers(true);
    QGLContext *l_glContext1 = new QGLContext(l_glFormat);
    QGLContext *l_glContext2 = new QGLContext(l_glFormat);
		
	// add widgets
	//	display widgets
    //      rgb
    m_WRGBDisplay =  new SWDisplayImageWidget(this);
    m_uiCreateAvatar->hlRGB->addWidget(m_WRGBDisplay);
    //      radial proj
    m_WRadialProjDisplay =  new SWDisplayImageWidget(this);
    m_uiCreateAvatar->hlRadial->addWidget(m_WRadialProjDisplay);

    //	cloud gl widget
    m_WCloudGL = new SWGLCloudWidget(l_glContext1, this);
//    m_WCloudGL->setMaximumSize(640,480);
    m_uiCreateAvatar->hlCloud->addWidget(m_WCloudGL);
    // mesh gl widget
    m_WMeshGL = new SWGLMeshWidget(l_glContext2, this);
    m_uiCreateAvatar->hlMesh->addWidget(m_WMeshGL);

	// init workers
    m_WCreateAvatar = new SWCreateAvatarWorker(0);

    // set connections
    //      start work
        QObject::connect(m_uiCreateAvatar->pbStart, SIGNAL(clicked()), this, SLOT(startWork()));
        QObject::connect(this, SIGNAL(startWorkSignal()), m_WCreateAvatar, SLOT(doWork()));

    //      stop work
        QObject::connect(m_uiCreateAvatar->pbStop, SIGNAL(clicked()), m_WCreateAvatar, SLOT(stopWork()));
        QObject::connect(this, SIGNAL(leaveProgram()), m_WCreateAvatar, SLOT(stopWork()));
        QObject::connect(m_WCreateAvatar, SIGNAL(stopWorkSignal()), SLOT(switchDetect()));

    //      rectangles
        QObject::connect(m_WCreateAvatar, SIGNAL(sendFaceRect(cv::Rect*)), this, SLOT(updateFaceRectangle(cv::Rect*)));
        QObject::connect(m_WCreateAvatar, SIGNAL(sendNoseRect(cv::Rect*)), this, SLOT(updateNoseRectangle(cv::Rect*)));

    //      stasmm points
        QObject::connect(m_WCreateAvatar, SIGNAL(sendStasmPoints(std::vector<cv::Point2i>)), this, SLOT(updateStasmPoints(std::vector<cv::Point2i>)));

    //      cloud
        QObject::connect(m_WCreateAvatar, SIGNAL(sendCloud(swCloud::SWCloud*)), m_WCloudGL, SLOT(setCloud(swCloud::SWCloud*)));

    //      mesh
        QObject::connect(m_WCreateAvatar, SIGNAL(sendMesh(swMesh::SWMesh*)), m_WMeshGL, SLOT(setMesh(swMesh::SWMesh*)));
        QObject::connect(m_uiCreateAvatar->pbSaveAvatar, SIGNAL(clicked()), this,   SLOT(saveMeshFile()));
        QObject::connect(this,  SIGNAL(saveMeshFileSignal(QString)),  m_WCreateAvatar, SLOT(saveMeshFile(QString)));

    //      ui
        QObject::connect(m_WCreateAvatar, SIGNAL(switchTab(int)),m_uiCreateAvatar->tw3D,   SLOT(setCurrentIndex(int)));
        QObject::connect(m_WCreateAvatar, SIGNAL(enableInterfaceSignal()), this,   SLOT(enableInterface()));

    //      display
        QObject::connect(m_WCreateAvatar, SIGNAL(sendMat(cv::Mat*)), this,      SLOT(updateRadialProjDisplay(cv::Mat*)));
        QObject::connect(m_WCreateAvatar, SIGNAL(sendTexture(cv::Mat*)), this,  SLOT(udpdateTexture3DDisplay(cv::Mat*)));

    //      parameters
        QObject::connect(m_uiCreateAvatar->sbWidth,     SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setRadialWidth(const int)));
        QObject::connect(m_uiCreateAvatar->sbHeight,    SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setRadialHeight(const int)));
        QObject::connect(m_uiCreateAvatar->sbColor,     SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setBilateralColor(const int)));
        QObject::connect(m_uiCreateAvatar->sbSpace,     SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setBilateralSpace(const int)));
        QObject::connect(m_uiCreateAvatar->sbDiameter,  SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setBilateralDiameter(const int)));
        QObject::connect(m_uiCreateAvatar->sbClouds,    SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setCloudNumberValue(const int)));
        QObject::connect(m_uiCreateAvatar->sbXOffset,   SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setCalibrationXValue(const int)));
        QObject::connect(m_uiCreateAvatar->sbYOffset,   SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setCalibrationYValue(const int)));
        QObject::connect(m_uiCreateAvatar->dsbRadius,   SIGNAL(valueChanged(double)),m_WCreateAvatar, SLOT(setCylinderRadius(double)));
        QObject::connect(m_uiCreateAvatar->cbBilateralFilter,SIGNAL(toggled(bool)) ,m_WCreateAvatar,  SLOT(setUseBilateralFilter(bool)));
        QObject::connect(m_uiCreateAvatar->cbSTASM,     SIGNAL(toggled(bool))      ,m_WCreateAvatar,  SLOT(setUseStasm(bool)));
        QObject::connect(m_uiCreateAvatar->sbErode,     SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setErode(int)));
        QObject::connect(m_uiCreateAvatar->sbDilate,    SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setDilate(int)));
        QObject::connect(m_uiCreateAvatar->cbDisplayLines,   SIGNAL(toggled(bool)), m_WMeshGL,        SLOT(setMeshLinesRender(const bool)));
        QObject::connect(m_uiCreateAvatar->cbApplyTexture,   SIGNAL(toggled(bool)), m_WMeshGL,        SLOT(applyTexture(bool)));

    // init worker parameters values
        m_WCreateAvatar->setRadialWidth(m_uiCreateAvatar->sbWidth->value());
        m_WCreateAvatar->setRadialHeight(m_uiCreateAvatar->sbHeight->value());
        m_WCreateAvatar->setBilateralColor(m_uiCreateAvatar->sbColor->value());
        m_WCreateAvatar->setBilateralSpace(m_uiCreateAvatar->sbSpace->value());
        m_WCreateAvatar->setBilateralDiameter(m_uiCreateAvatar->sbDiameter->value());
        m_WCreateAvatar->setCloudNumberValue(m_uiCreateAvatar->sbClouds->value());
        m_WCreateAvatar->setCalibrationXValue(m_uiCreateAvatar->sbXOffset->value());
        m_WCreateAvatar->setCalibrationYValue(m_uiCreateAvatar->sbYOffset->value());

    // init and start create avatar thread
        m_WCreateAvatar->moveToThread(&m_TCreateAvatar);
        m_TCreateAvatar.start();

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
            catch(const swExcept::kinectInitError &e)
            {
                std::cerr << "-ERROR : " << e.what() << std::endl;
            }
        }

    // init face detection
        m_CFaceDetectPtr = SWFaceDetectionPtr(new swDetect::SWFaceDetection(cv::Size(80,80)));
        m_oLastRectFace.width = 0;

    // launch timer for updating the display widget
        m_oTimer->start(1000/30, this);
}


SWCreateAvatarInterface::~SWCreateAvatarInterface()
{
    // stop threads
        m_oKinectThread.stopListening();
        m_TCreateAvatar.quit();
        m_TCreateAvatar.wait();

    // delete worker and ui
        deleteAndNullify(m_WCreateAvatar);
        deleteAndNullify(m_uiCreateAvatar);

    // delete others
        deleteAndNullify(m_pCloudToDisplay);
}


void SWCreateAvatarInterface::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    updateDisplay();
}

void SWCreateAvatarInterface::closeEvent(QCloseEvent *event)
{
    emit leaveProgram();
}

void SWCreateAvatarInterface::startWork()
{
    m_uiCreateAvatar->tw3D->setCurrentIndex(0);
    m_uiCreateAvatar->pbSaveAvatar->setEnabled(false);

    m_oMutex.lockForWrite();
        m_bWorkStarted = true;
    m_oMutex.unlock();

    emit startWorkSignal();
}

void SWCreateAvatarInterface::switchDetect()
{
    m_oMutex.lockForWrite();
        m_bWorkStarted = false;
    m_oMutex.unlock();

    // reset rects
        m_oFaceRect.width = 0;
        m_oNoseRect.width = 0;

    m_uiCreateAvatar->pbStart->setEnabled(true);
    m_uiCreateAvatar->pbStop->setEnabled(false);
}

void SWCreateAvatarInterface::updateDisplay()
{
    // get the current image from the kinect
        cv::Mat l_oBGR   = m_oKinectThread.bgrImage();
        cv::Mat l_oCloud = m_oKinectThread.cloudMap();
        cv::Mat l_oBGRForeGround = swImage::swUtil::removeBackground(l_oBGR, l_oCloud);

    m_oMutex.lockForRead();
        bool l_bDoDetect = !m_bWorkStarted;
        cv::Rect l_oFaceRectangle = m_oFaceRect;
        cv::Rect l_oNoseRectangle = m_oNoseRect;
    m_oMutex.unlock();

    if(l_bDoDetect)
    {
        // detect
            m_CFaceDetectPtr->detect(l_oBGRForeGround);

        // cloud
            deleteAndNullify(m_pCloudToDisplay);
            m_pCloudToDisplay = new swCloud::SWCloud();

            if(m_CFaceDetectPtr->faceRect().width > 0)
            {
                cv::Mat l_oFaceCloudMat = l_oCloud(m_CFaceDetectPtr->faceRect());
                swCloud::convCloudMat2SWCloud(l_oFaceCloudMat, l_oBGR(m_CFaceDetectPtr->faceRect()), *m_pCloudToDisplay,0.f,1.2f);

                if(m_pCloudToDisplay->size() > 0)
                {
                    m_WCloudGL->setCloud(m_pCloudToDisplay);
                }
            }

        // display image
            m_CFaceDetectPtr->displayFace(l_oBGR, cv::Scalar(0,255,0));
    }
    else if(l_oFaceRectangle.width > 0)
    {
        if(swUtil::isInside(l_oFaceRectangle, l_oBGR))
        {
            cv::rectangle(l_oBGR, cv::Point(l_oFaceRectangle.x, l_oFaceRectangle.y),
                cv::Point(l_oFaceRectangle.x+l_oFaceRectangle.width, l_oFaceRectangle.y+l_oFaceRectangle.height), RED,1);
        }

        if(swUtil::isInside(l_oNoseRectangle,l_oBGR))
        {
            cv::rectangle(l_oBGR, cv::Point(l_oNoseRectangle.x, l_oNoseRectangle.y),
                    cv::Point(l_oNoseRectangle.x + l_oNoseRectangle.width, l_oNoseRectangle.y + l_oNoseRectangle.height), RED,1);
        }

        // display feature points
            for(uint ii = 0; ii < m_vP2IStasmPoints.size(); ++ii)
            {
                cv::circle(l_oBGR, m_vP2IStasmPoints[ii], 1, BLUE);
            }
    }

//    swUtil::displayFPS(l_oBGR);
    // update display image widget
        m_WRGBDisplay->refreshDisplay(swConv::mat2QImage(l_oBGR));
}

void SWCreateAvatarInterface::updateRadialProjDisplay(cv::Mat *pRadialProj)
{
    cv::Mat l_oRadialGray(pRadialProj->rows, pRadialProj->cols, CV_8UC3);
    for(int ii = 0; ii < l_oRadialGray.rows * l_oRadialGray.cols; ++ii)
    {
        cv::Vec3b l_oRGB;
        l_oRGB[0] = (int)(pRadialProj->at<float>(ii));
        l_oRGB[1] = (int)(pRadialProj->at<float>(ii));
        l_oRGB[2] = (int)(pRadialProj->at<float>(ii));
        l_oRadialGray.at<cv::Vec3b>(ii) = l_oRGB;
    }

    m_WRadialProjDisplay->refreshDisplay(swConv::mat2QImage(l_oRadialGray));
}

void SWCreateAvatarInterface::udpdateTexture3DDisplay(cv::Mat *pTexture)
{
    m_WMeshGL->setTexture(swConv::mat2QImage(*pTexture));
}

void SWCreateAvatarInterface::updateStasmPoints(std::vector<cv::Point2i> vP2IStasmPoints)
{
    m_vP2IStasmPoints = vP2IStasmPoints;
}

void SWCreateAvatarInterface::updateFaceRectangle(cv::Rect *pFaceRect)
{
    m_oMutex.lockForWrite();
        m_oFaceRect = cv::Rect(*pFaceRect);
    m_oMutex.unlock();
}

void SWCreateAvatarInterface::updateNoseRectangle(cv::Rect *pNoseRect)
{
    m_oMutex.lockForWrite();
        m_oNoseRect = cv::Rect(*pNoseRect);
    m_oMutex.unlock();
}

void SWCreateAvatarInterface::saveMeshFile()
{
    QString l_sPathFileToSave = QFileDialog::getSaveFileName(this, "Save mesh file", QString(), "Mesh file (*.obj)");
    emit saveMeshFileSignal(l_sPathFileToSave);
}

void SWCreateAvatarInterface::enableInterface()
{
    m_uiCreateAvatar->pbSaveAvatar->setEnabled(true);
}


int main(int argc, char* argv[])
{
    QApplication l_oApp(argc, argv);
    SWCreateAvatarInterface l_oQtInterface;
    l_oQtInterface.show();

    return l_oApp.exec();
}

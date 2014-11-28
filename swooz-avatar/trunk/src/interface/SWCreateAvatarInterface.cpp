
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
    m_oTimer(new QBasicTimer), m_bGLFullScreen(false)
{
    m_bWorkStarted = false;
    m_bResetKinect = false;

    // init rects
        m_oFaceRect.width = 0;
        m_oNoseRect.width = 0;

	// init main widget
    m_uiCreateAvatar->setupUi(this);
	this->setWindowTitle(QString("SWoOz : Create avatar"));
    this->setWindowIcon(QIcon(QString("../data/images/logos/icon_swooz_avatar.png")));

	// init opengl context
    QHBoxLayout *l_pGLContainerLayout = new QHBoxLayout();
    l_pGLContainerLayout->layout()->setContentsMargins(0,0,0,0);
    m_pGLContainer = new QWidget();
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
    m_WRadialProjDisplay =  new SWDisplayImageWidget(this, false, true);
    m_uiCreateAvatar->hlRadial->addWidget(m_WRadialProjDisplay);
    //      face texture
    m_WFaceTextureDisplay =  new SWDisplayImageWidget(this);
    m_uiCreateAvatar->hlFaceTexture->addWidget(m_WFaceTextureDisplay);

    //	cloud gl widget
    m_WCloudGL = new SWGLCloudWidget(l_glContext1, this);
    m_WCloudGL->setCameraMode(SWQtCamera::TRACKBALL_CAMERA);
//    m_WCloudGL->setMaximumSize(640,480);
    m_uiCreateAvatar->hlCloud->addWidget(m_WCloudGL);
    // mesh gl widget
    m_WMeshGL = new SWGLMeshWidget(l_glContext2, this, "../data/shaders/createAvatarMesh.vert", "../data/shaders/createAvatarMesh.frag");  
    m_WMeshGL->setCameraMode(SWQtCamera::TRACKBALL_CAMERA);


    m_pGLContainer->setLayout(l_pGLContainerLayout);
    l_pGLContainerLayout->addWidget(m_WMeshGL);
    m_uiCreateAvatar->hlMesh->addWidget(m_pGLContainer);

	// init workers
    m_WCreateAvatar = new SWCreateAvatarWorker(&m_oKinectThread);

    // set connections
    //      start work
        QObject::connect(m_uiCreateAvatar->pbStart, SIGNAL(clicked()), this, SLOT(startWork()));
        QObject::connect(this, SIGNAL(startWorkSignal()), m_WCreateAvatar, SLOT(doWork()));

    //      stop work
        QObject::connect(m_uiCreateAvatar->pbStop, SIGNAL(clicked()), m_WCreateAvatar, SLOT(stopWork()));
        QObject::connect(this, SIGNAL(leaveProgram()), m_WCreateAvatar, SLOT(stopWork()));
        QObject::connect(m_WCreateAvatar, SIGNAL(stopWorkSignal()), SLOT(switchDetect()));

    //      reconstruct
        QObject::connect(m_uiCreateAvatar->pbReconstruct, SIGNAL(clicked()), m_WCreateAvatar, SLOT(reconstruct()));

    //      reset selected contours
        QObject::connect(m_uiCreateAvatar->pbResetSelectedContours, SIGNAL(clicked()), m_WCreateAvatar,      SLOT(resetDeletedPointsRadialProj()));
        QObject::connect(m_uiCreateAvatar->pbResetSelectedContours, SIGNAL(clicked()), m_WRadialProjDisplay, SLOT(resetSelectedPoints()));

    //      rectangles
        QObject::connect(m_WCreateAvatar, SIGNAL(sendFaceRect(cv::Rect*)), this, SLOT(updateFaceRectangle(cv::Rect*)));
        QObject::connect(m_WCreateAvatar, SIGNAL(sendNoseRect(cv::Rect*)), this, SLOT(updateNoseRectangle(cv::Rect*)));

    //      stasmm points
        QObject::connect(m_WCreateAvatar, SIGNAL(sendStasmPoints(std::vector<cv::Point2i>)), this, SLOT(updateStasmPoints(std::vector<cv::Point2i>)));

    //      cloud
        QObject::connect(m_WCreateAvatar, SIGNAL(sendCloud(swCloud::SWCloud*, bool)), m_WCloudGL, SLOT(setCloud(swCloud::SWCloud*, bool)));

    //      mesh
        QObject::connect(m_WCreateAvatar, SIGNAL(sendMesh(swMesh::SWMesh*,bool)), m_WMeshGL, SLOT(setMesh(swMesh::SWMesh*,bool)));
        QObject::connect(m_uiCreateAvatar->pbSaveAvatar, SIGNAL(clicked()), this,   SLOT(saveMeshFile()));
        QObject::connect(this,  SIGNAL(saveMeshFileSignal(QString)),  m_WCreateAvatar, SLOT(saveMeshFile(QString)));

    //      ui
        QObject::connect(m_WCreateAvatar, SIGNAL(switchTab(int)),m_uiCreateAvatar->tw3D,   SLOT(setCurrentIndex(int)));
        QObject::connect(m_WCreateAvatar, SIGNAL(enableInterfaceSignal()), this,   SLOT(enableInterface()));

    //      display
        QObject::connect(m_WCreateAvatar, SIGNAL(sendMat(cv::Mat*)),     this,  SLOT(updateRadialProjDisplay(cv::Mat*)));
        QObject::connect(m_WCreateAvatar, SIGNAL(sendTexture(cv::Mat*)), this,  SLOT(udpdateTexture3DDisplay(cv::Mat*)));        
        QObject::connect(m_WCreateAvatar, SIGNAL(sendTexture(cv::Mat*)), this,  SLOT(updateFaceTextureDisplay(cv::Mat*)));

    //      parameters
    //          projection
        QObject::connect(m_uiCreateAvatar->sbWidth,     SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setRadialWidth(const int)));
        QObject::connect(m_uiCreateAvatar->sbHeight,    SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setRadialHeight(const int)));
        QObject::connect(m_uiCreateAvatar->dsbRadius,   SIGNAL(valueChanged(double)),m_WCreateAvatar, SLOT(setCylinderRadius(double)));
        QObject::connect(m_uiCreateAvatar->sbErode,     SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setErode(int)));
        QObject::connect(m_uiCreateAvatar->sbDilate,    SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setDilate(int)));
        QObject::connect(m_uiCreateAvatar->sbExpandValue,SIGNAL(valueChanged(int)), m_WCreateAvatar,  SLOT(setExpandValue(int)));
        QObject::connect(m_uiCreateAvatar->sbExpandConnex,SIGNAL(valueChanged(int)), m_WCreateAvatar,  SLOT(setExpandConnex(int)));
        QObject::connect(m_uiCreateAvatar->sbEraseValue,SIGNAL(valueChanged(int)), m_WCreateAvatar,  SLOT(setEraseValue(int)));
        QObject::connect(m_uiCreateAvatar->sbEraseConnex,SIGNAL(valueChanged(int)), m_WCreateAvatar,  SLOT(setEraseConnex(int)));
    //          filter
        QObject::connect(m_uiCreateAvatar->sbColor,     SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setBilateralColor(const int)));
        QObject::connect(m_uiCreateAvatar->sbSpace,     SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setBilateralSpace(const int)));
        QObject::connect(m_uiCreateAvatar->sbDiameter,  SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setBilateralDiameter(const int)));
        QObject::connect(m_uiCreateAvatar->sbClouds,    SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setCloudNumberValue(const int)));
        QObject::connect(m_uiCreateAvatar->cbBilateralFilter,SIGNAL(toggled(bool)) ,m_WCreateAvatar,  SLOT(setUseBilateralFilter(bool)));
    //          rgbd device
        QObject::connect(m_uiCreateAvatar->sbXOffset,   SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setCalibrationXValue(const int)));
        QObject::connect(m_uiCreateAvatar->sbYOffset,   SIGNAL(valueChanged(int)),  m_WCreateAvatar,  SLOT(setCalibrationYValue(const int)));
        QObject::connect(m_uiCreateAvatar->dsbFaceDepth,SIGNAL(valueChanged(double)),  m_WCloudGL,      SLOT(setDepthRect(const double)));
        QObject::connect(m_uiCreateAvatar->dsbFaceDepth,SIGNAL(valueChanged(double)),  m_WCreateAvatar, SLOT(setDepthCloud(const double)));
    //          misc
        QObject::connect(m_uiCreateAvatar->cbSTASM,     SIGNAL(toggled(bool))      ,m_WCreateAvatar,  SLOT(setUseStasm(bool)));
    //          display
        QObject::connect(m_uiCreateAvatar->cbDisplayLines,   SIGNAL(toggled(bool)), m_WMeshGL,        SLOT(setMeshLinesRender(const bool)));
        QObject::connect(m_uiCreateAvatar->cbApplyTexture,   SIGNAL(toggled(bool)), m_WMeshGL,        SLOT(applyTexture(bool)));
        QObject::connect(m_uiCreateAvatar->dsbFOV,      SIGNAL(valueChanged(double)),m_WMeshGL,       SLOT(setFOV(const double)));
        QObject::connect(m_uiCreateAvatar->dsbFOV,      SIGNAL(valueChanged(double)),m_WCloudGL,      SLOT(setFOV(const double)));
        QObject::connect(m_WCreateAvatar,               SIGNAL(sendNumCloud(int)),  this,             SLOT(setNumCloud(const int)));

    //      reset kinect
        QObject::connect(m_uiCreateAvatar->sbDeviceMode,SIGNAL(valueChanged(int)),  this,             SLOT(resetKinect(int)));
        QObject::connect(m_WCreateAvatar,   SIGNAL(endResetKinect()),   this,       SLOT(releaseKinectMutex()));

    //      others
        QObject::connect(m_WRadialProjDisplay, SIGNAL(clickPoint(QPoint, QSize, bool)), m_WCreateAvatar,   SLOT(addPointToDeleteRadialProj(QPoint, QSize, bool)));

        // fullscreen
        QObject::connect(m_WMeshGL, SIGNAL(enableFullScreen()), this, SLOT(enableGLFullScreen()));
        QObject::connect(m_WMeshGL, SIGNAL(disableFullScreen()), this, SLOT(disableGLFullScreen()));

    // init worker parameters values
        // radial projection width / height
            m_WCreateAvatar->setRadialWidth(m_uiCreateAvatar->sbWidth->value());
            m_WCreateAvatar->setRadialHeight(m_uiCreateAvatar->sbHeight->value());
        // spatial filter
            m_WCreateAvatar->setBilateralColor(m_uiCreateAvatar->sbColor->value());
            m_WCreateAvatar->setBilateralSpace(m_uiCreateAvatar->sbSpace->value());
            m_WCreateAvatar->setBilateralDiameter(m_uiCreateAvatar->sbDiameter->value());
        // temporal filter
            m_WCreateAvatar->setCloudNumberValue(m_uiCreateAvatar->sbClouds->value());
        // calibration
            m_WCreateAvatar->setCalibrationXValue(m_uiCreateAvatar->sbXOffset->value());
            m_WCreateAvatar->setCalibrationYValue(m_uiCreateAvatar->sbYOffset->value());
        // expand / erase
            m_WCreateAvatar->setExpandValue(m_uiCreateAvatar->sbExpandValue->value());
            m_WCreateAvatar->setExpandConnex(m_uiCreateAvatar->sbExpandConnex->value());
            m_WCreateAvatar->setEraseValue(m_uiCreateAvatar->sbEraseValue->value());
            m_WCreateAvatar->setEraseConnex(m_uiCreateAvatar->sbEraseConnex->value());
        // dilate / erode
            m_WCreateAvatar->setDilate(m_uiCreateAvatar->sbDilate->value());
            m_WCreateAvatar->setErode(m_uiCreateAvatar->sbErode->value());
        // stasm
            m_WCreateAvatar->setUseStasm(m_uiCreateAvatar->cbSTASM->isChecked());

        // cloud
            m_WCloudGL->setDepthRect(m_uiCreateAvatar->dsbFaceDepth->value());
            m_WCreateAvatar->setDepthCloud(m_uiCreateAvatar->dsbFaceDepth->value());


    // desactive debug interface components
            m_uiCreateAvatar->laPixelDiameter->setVisible(false);
            m_uiCreateAvatar->sbDiameter->setVisible(false);
            m_uiCreateAvatar->laCynlinderRadius->setVisible(false);
            m_uiCreateAvatar->dsbRadius->setVisible(false);
            m_uiCreateAvatar->laFOV->setVisible(false);
            m_uiCreateAvatar->dsbFOV->setVisible(false);


    // init and start create avatar thread
        m_WCreateAvatar->moveToThread(&m_TCreateAvatar);
        m_TCreateAvatar.start();

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
}

void SWCreateAvatarInterface::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);

    m_oResetKinectMutex.lockForRead();
        bool l_bDoDisplay  = !m_bResetKinect;        
    m_oResetKinectMutex.unlock();

    if(l_bDoDisplay)
    {
        updateDisplay();
    }
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
    if(!m_oKinectThread.isDataAvailable())
    {
        qWarning() << "Kinect data not available, display disactivated. ";
        return;
    }

    // get the current image from the kinect
        cv::Mat l_oBGR, l_oCloud;
        l_oBGR   = m_oKinectThread.bgrImage().clone();
        l_oCloud = m_oKinectThread.cloudMap().clone();

    // check size mat
        if(l_oBGR.rows != l_oCloud.rows)
        {
           cv::resize(l_oBGR, l_oBGR, cv::Size(l_oCloud.cols,l_oCloud.rows));
        }

    // remove background
        cv::Mat l_oBGRForeGround = swImage::swUtil::removeBackground(l_oBGR, l_oCloud);

    m_oMutex.lockForRead();
        bool l_bDoDetect = !m_bWorkStarted;
        cv::Rect l_oFaceRectangle = m_oFaceRect;
        cv::Rect l_oNoseRectangle = m_oNoseRect;
    m_oMutex.unlock();

    if(l_bDoDetect)
    {
        // detect
            m_CFaceDetectPtr->detectFace(l_oBGRForeGround);

        // cloud
            swCloud::SWCloud *l_pCloudToDisplay = new swCloud::SWCloud();

            if(m_CFaceDetectPtr->faceRect().width > 0)
            {
                cv::Mat l_oFaceCloudMat = l_oCloud(m_CFaceDetectPtr->faceRect());
                swCloud::convCloudMat2SWCloud(l_oFaceCloudMat, l_oBGR(m_CFaceDetectPtr->faceRect()), *l_pCloudToDisplay,0.f,1.2f);

                if(l_pCloudToDisplay->size() > 0)
                {
                    m_WCloudGL->setCloud(l_pCloudToDisplay);
                }
                else
                {
                    deleteAndNullify(l_pCloudToDisplay);
                }
            }
            else
            {
                deleteAndNullify(l_pCloudToDisplay);
            }

        // display image
            m_CFaceDetectPtr->displayFace(l_oBGR, cv::Scalar(0,255,0));
    }
    else if(l_oFaceRectangle.width > 0)
    {
        // display face rectangle
            if(swUtil::isInside(l_oFaceRectangle, l_oBGR))
            {
                cv::rectangle(l_oBGR, cv::Point(l_oFaceRectangle.x, l_oFaceRectangle.y),
                    cv::Point(l_oFaceRectangle.x+l_oFaceRectangle.width, l_oFaceRectangle.y+l_oFaceRectangle.height), RED,1);
            }

        // display nose rectangle
            if(swUtil::isInside(l_oNoseRectangle,l_oBGR))
            {
                cv::rectangle(l_oBGR, cv::Point(l_oNoseRectangle.x, l_oNoseRectangle.y),
                        cv::Point(l_oNoseRectangle.x + l_oNoseRectangle.width, l_oNoseRectangle.y + l_oNoseRectangle.height), RED,1);
            }

        // display stasm feature points
            for(uint ii = 0; ii < m_vP2IStasmPoints.size(); ++ii)
            {
                cv::circle(l_oBGR, m_vP2IStasmPoints[ii], 1, BLUE);
            }

        // display num cloud
            std::ostringstream l_osNumCloud, l_osTotalCloud;
            l_osNumCloud << m_i32NumCloud;
            l_osTotalCloud << m_uiCreateAvatar->sbClouds->value();
            swUtil::displayTextDownRight(l_oBGR, std::string(l_osNumCloud.str() + " / " + l_osTotalCloud.str() ));
    }

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

void SWCreateAvatarInterface::updateFaceTextureDisplay(cv::Mat *pFaceTexture)
{
    m_WFaceTextureDisplay->refreshDisplay(swConv::mat2QImage(*pFaceTexture));
}

void SWCreateAvatarInterface::udpdateTexture3DDisplay(cv::Mat *pTexture)
{
    m_WMeshGL->setTexture(swConv::mat2QImage(*pTexture));
}

void SWCreateAvatarInterface::resetKinect(const int i32VideoMode)
{
    m_oResetKinectMutex.lockForWrite();
        m_bResetKinect = true;
    m_oResetKinectMutex.unlock();

    m_WCreateAvatar->resetKinect(i32VideoMode);

}

void SWCreateAvatarInterface::releaseKinectMutex()
{
    m_oResetKinectMutex.lockForWrite();
        m_bResetKinect = false;
        m_oResetKinectMutex.unlock();
}

void SWCreateAvatarInterface::setNumCloud(const int i32NumCloud)
{
    m_oMutex.lockForWrite();
        m_i32NumCloud = i32NumCloud;
    m_oMutex.unlock();
}


void SWCreateAvatarInterface::updateStasmPoints(std::vector<cv::Point2i> vP2IStasmPoints)
{
    m_oMutex.lockForWrite();
        m_vP2IStasmPoints = vP2IStasmPoints;
    m_oMutex.unlock();
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
    m_uiCreateAvatar->pbReconstruct->setEnabled(true);
}


void SWCreateAvatarInterface::enableGLFullScreen()
{
    if(!m_bGLFullScreen)
    {
        m_pGLContainer->setParent(0);
        m_pGLContainer->showFullScreen();
        m_bGLFullScreen = true;
    }
}

void SWCreateAvatarInterface::disableGLFullScreen()
{
    if(m_bGLFullScreen)
    {
       m_uiCreateAvatar->hlMesh->addWidget(m_pGLContainer);
        m_bGLFullScreen = false;
    }
}

int main(int argc, char* argv[])
{
    QApplication l_oApp(argc, argv);
    SWCreateAvatarInterface l_oQtInterface;
    l_oQtInterface.show();

    return l_oApp.exec();
}

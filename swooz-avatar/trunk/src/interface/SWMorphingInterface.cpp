
/**
 * \file SWMorphingInterface.cpp
 * \brief Defines SWMorphingInterface
 * \author Florian Lance
 * \date 12/11/13
 */

#include "interface/SWMorphingInterface.h"

#include "moc_SWMorphingInterface.cpp"

#include <QtGui>

#include "interface/QtWidgets/SWGLOptimalStepNonRigidICP.h"


#include <cloud/SWAlignClouds.h>


#include "gpuMat/gpuMatUtility.h"

#include <time.h>

SWMorphingInterface::SWMorphingInterface() : m_uiMorphing(new Ui::SWUI_Morphing)
{      
    m_bTemplateDefined  = false;
    m_bTargetDefined    = false;
    m_bGLFullScreen     = false;
    m_pathTargetTexture = "";

    m_bSetLandmarksManually = false;

    // init main widget
    m_uiMorphing->setupUi(this);
    this->setWindowTitle(QString("SWoOz : Morphing"));

    // set default values
    m_i32RotXDefaultValue = m_i32RotYDefaultValue = m_i32RotZDefaultValue = 1800;
    m_i32TransXDefaultValue = m_i32TransYDefaultValue = m_i32TransZDefaultValue = 500;
    m_i32ScalingDefaultValue = 0;

    m_dStartAlphaDefaultValue = 1.5;//0.5;
    m_dMinAlphaDefaultValue   = 0.8;//0.1;
    m_dDiffMaxDefaultValue    = 0.2;//15.0;
    m_dBetaDefaultValue       = 1.0;//0.1;
    m_dGamaDefaultValue       = 3.2;//100.0;
    m_dCoeffValueDefaultValue = 0.95;//0.8;
    m_dAngleMaxDefaultValue   = 50.0;

    // parameters
        // spinbox
            m_uiMorphing->dsbFactor->setMinimum(0.0);       m_uiMorphing->dsbFactor->setMaximum(1.0);
            m_uiMorphing->dsbStartAlpha->setMinimum(0.0);   m_uiMorphing->dsbStartAlpha->setMaximum(1000.0);
            m_uiMorphing->dsbMinAlpha->setMinimum(0.0);     m_uiMorphing->dsbMinAlpha->setMaximum(100.0);
            m_uiMorphing->dsbDiffMax->setMinimum(0.0);      m_uiMorphing->dsbDiffMax->setMaximum(500.0);
            m_uiMorphing->dsbAngleMax->setMinimum(0.0);     m_uiMorphing->dsbAngleMax->setMaximum(180.0);
            m_uiMorphing->dsbAngleMax->setDecimals(2);
            m_uiMorphing->dsbBeta->setMinimum(0.0);         m_uiMorphing->dsbBeta->setMaximum(150.0);
            m_uiMorphing->dsbGama->setMinimum(0.0);         m_uiMorphing->dsbGama->setMaximum(10000.0);

        // checkbox
            m_uiMorphing->cbTemplateMesh->setChecked(true); m_uiMorphing->cbTargetMesh->setChecked(true);
            m_uiMorphing->cbCorr->setChecked(true);
            m_uiMorphing->cbLandmarks->setChecked(true);
            m_uiMorphing->cbUseLandmarks->setChecked(true);
            m_uiMorphing->rbStasm->setChecked(true);

    // middle container
        QHBoxLayout *l_pGLContainerLayout = new QHBoxLayout();
        l_pGLContainerLayout->layout()->setContentsMargins(0,0,0,0);
        m_pGLContainer = new QWidget();
        QGLFormat l_glFormat;
        l_glFormat.setVersion( 4, 3 );
        l_glFormat.setProfile(  QGLFormat::CompatibilityProfile);
        l_glFormat.setSampleBuffers( true );
        QGLContext *l_glContext = new QGLContext(l_glFormat);
        m_pGLOSNRICP = new SWGLOptimalStepNonRigidICP(l_glContext, m_pGLContainer);
        m_pGLOSNRICP->setCameraMode(SWQtCamera::TRACKBALL_CAMERA);
        QVector3D l_oEye(0,0,-1),l_oLookAt(0,0,0);
        m_pGLOSNRICP->setCamera(l_oEye,l_oLookAt);
        m_pGLOSNRICP->setCameraInitial(l_oEye,l_oLookAt, QVector3D(0,1,0));

        l_pGLContainerLayout->addWidget(m_pGLOSNRICP);
        m_pGLContainer->setLayout(l_pGLContainerLayout);
        m_uiMorphing->glScene->addWidget(m_pGLContainer);


    // init worker
        m_pWMorphing = new SWMorphingWorker(m_pGLOSNRICP);

        // m_pWMorphing
        QObject::connect(m_pWMorphing,  SIGNAL(updateSceneDisplaySignal()),     m_pGLOSNRICP,               SLOT(refreshDisplay()));
        QObject::connect(m_pWMorphing,  SIGNAL(endMorphingSignal()),            this,                       SLOT(unlockInterface()));
        QObject::connect(m_pWMorphing,  SIGNAL(endMorphingSignal()),            m_pGLOSNRICP,               SLOT(updateDisplayTextEndMorphing()));

        // m_pGLOSNRICP
        QObject::connect(m_pGLOSNRICP,  SIGNAL(sendSourceMeshInfos(QString)), m_uiMorphing->tbInfosTemplate,SLOT(setText(QString)));
        QObject::connect(m_pGLOSNRICP,  SIGNAL(sendTargetMeshInfos(QString)), m_uiMorphing->tbInfosTarget,  SLOT(setText(QString)));
        QObject::connect(this,          SIGNAL(sendInfoTemplateClickedPoints(QString)), m_uiMorphing->tbTemplateLandmarks, SLOT(setText(QString)));
        QObject::connect(this,          SIGNAL(sendInfoTargetClickedPoints(QString)), m_uiMorphing->tbTargetLandmarks, SLOT(setText(QString)));

        // m_uiMorphing
            // pushbuttons
        QObject::connect(m_uiMorphing->pbSetTargetTexture,  SIGNAL(clicked()),      this,                   SLOT(setTargetTexture()));
        QObject::connect(m_uiMorphing->pbStart,             SIGNAL(clicked()),      m_pWMorphing,           SLOT(startMorphing()));
        QObject::connect(m_uiMorphing->pbStart,             SIGNAL(clicked()),      this,                   SLOT(setLandmarksUsage()));
        QObject::connect(m_uiMorphing->pbStart,             SIGNAL(clicked()),      this,                   SLOT(lockInterface()));
        QObject::connect(m_uiMorphing->pbStop,              SIGNAL(clicked()),      m_pWMorphing,           SLOT(stopMorphing()));
        QObject::connect(m_uiMorphing->pbStop,              SIGNAL(clicked(bool)),  m_uiMorphing->pbStop,   SLOT(setEnabled(bool)));
        QObject::connect(m_uiMorphing->pbSaveMorphedMesh,   SIGNAL(clicked()),      this,                   SLOT(saveMeshFile()));
        QObject::connect(m_uiMorphing->pbSetTemplate,       SIGNAL(clicked()),      this,                   SLOT(setTemplateMeshPath()));
        QObject::connect(m_uiMorphing->pbSetTarget,         SIGNAL(clicked()),      this,                   SLOT(setTargetMeshPath()));
        QObject::connect(m_uiMorphing->pbResetMorphing,     SIGNAL(clicked()),      m_pGLOSNRICP,           SLOT(resetMorphing()));
        QObject::connect(m_uiMorphing->pbCorr,              SIGNAL(clicked()),      m_pGLOSNRICP,           SLOT(computeDistWAndCorr()));
        QObject::connect(m_uiMorphing->pbResetParameters,   SIGNAL(clicked()),      this,                   SLOT(resetAllParameters()));
        QObject::connect(m_uiMorphing->pbSetLandManually,   SIGNAL(clicked()),      this,                   SLOT(setLandmarksManually()));
        QObject::connect(m_uiMorphing->pbEraseLand,   SIGNAL(clicked()),      this,                         SLOT(eraseManuallyLandmarks()));
            // combobox
        QObject::connect(m_uiMorphing->cbUseLandmarks,      SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setUseLandMarks(bool)));
        QObject::connect(m_uiMorphing->cbTemplateCloud,     SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setCloudSDisplay(bool)));
        QObject::connect(m_uiMorphing->cbTargetCloud,       SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setCloudTDisplay(bool)));
        QObject::connect(m_uiMorphing->cbTemplateMesh,      SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setMeshSDisplay(bool)));
        QObject::connect(m_uiMorphing->cbTargetMesh,        SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setMeshTDisplay(bool)));
        QObject::connect(m_uiMorphing->cbTemplateTriNormal, SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setTrianglesNormalsSDisplay(bool)));
        QObject::connect(m_uiMorphing->cbTargetTriNormal,   SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setTrianglesNormalsTDisplay(bool)));
        QObject::connect(m_uiMorphing->cbTemplateVertNormal,SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setVerticesNormalsSDisplay(bool)));
        QObject::connect(m_uiMorphing->cbTargetVertNormal,  SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setVerticesNormalsTDisplay(bool)));
        QObject::connect(m_uiMorphing->cbLandmarks,         SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setLandMarksDisplay(bool)));
        QObject::connect(m_uiMorphing->cbCorr,              SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setCorrDisplay(bool)));
        QObject::connect(m_uiMorphing->cbTemplateFillMesh,  SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setFillSDisplay(bool)));
        QObject::connect(m_uiMorphing->cbTargetFillMesh,    SIGNAL(clicked(bool)),  m_pGLOSNRICP, SLOT(setFillTDisplay(bool)));
            // horizontal sliders
        QObject::connect(m_uiMorphing->hsRotX,              SIGNAL(valueChanged(int)),  m_pGLOSNRICP, SLOT(setRotTargetX(int)));
        QObject::connect(m_uiMorphing->hsRotY,              SIGNAL(valueChanged(int)),  m_pGLOSNRICP, SLOT(setRotTargetY(int)));
        QObject::connect(m_uiMorphing->hsRotZ,              SIGNAL(valueChanged(int)),  m_pGLOSNRICP, SLOT(setRotTargetZ(int)));
        QObject::connect(m_uiMorphing->hsTransX,            SIGNAL(valueChanged(int)),  m_pGLOSNRICP, SLOT(setTransTargetX(int)));
        QObject::connect(m_uiMorphing->hsTransY,            SIGNAL(valueChanged(int)),  m_pGLOSNRICP, SLOT(setTransTargetY(int)));
        QObject::connect(m_uiMorphing->hsTransZ,            SIGNAL(valueChanged(int)),  m_pGLOSNRICP, SLOT(setTransTargetZ(int)));
        QObject::connect(m_uiMorphing->hsScale,             SIGNAL(valueChanged(int)),  m_pGLOSNRICP, SLOT(setTargetScaling(int)));
            // double spinboxes
        QObject::connect(m_uiMorphing->dsbFactor,           SIGNAL(valueChanged(double)),m_pGLOSNRICP,SLOT(setCoeffAlpha(double)));
        QObject::connect(m_uiMorphing->dsbStartAlpha,       SIGNAL(valueChanged(double)),m_pGLOSNRICP,SLOT(setStartAlpha(double)));
        QObject::connect(m_uiMorphing->dsbMinAlpha,         SIGNAL(valueChanged(double)),m_pGLOSNRICP,SLOT(setMinAlpha(double)));
        QObject::connect(m_uiMorphing->dsbDiffMax,          SIGNAL(valueChanged(double)),m_pGLOSNRICP,SLOT(setDiffMax(double)));
        QObject::connect(m_uiMorphing->dsbBeta,             SIGNAL(valueChanged(double)),m_pGLOSNRICP,SLOT(setBeta(double)));
        QObject::connect(m_uiMorphing->dsbGama,             SIGNAL(valueChanged(double)),m_pGLOSNRICP,SLOT(setGama(double)));
        QObject::connect(m_uiMorphing->dsbAngleMax,         SIGNAL(valueChanged(double)),m_pGLOSNRICP,SLOT(setAngleMax(double)));        


        // fullscreen
        QObject::connect(m_pGLOSNRICP, SIGNAL(enableFullScreen()), this, SLOT(enableGLFullScreen()));
        QObject::connect(m_pGLOSNRICP, SIGNAL(disableFullScreen()), this, SLOT(disableGLFullScreen()));        

        // landmarks manually
        QObject::connect(m_pGLOSNRICP, SIGNAL(enterKeyPressed()), this, SLOT(landmarksSetManuallyEscape()));
        QObject::connect(m_pGLOSNRICP, SIGNAL(spaceKeyPressed()), this, SLOT(landmarksSetManuallyNextPoint()));
        QObject::connect(m_pGLOSNRICP, SIGNAL(rayClick()), m_pGLOSNRICP, SLOT(updatePointMeshClicked()));
        QObject::connect(m_uiMorphing->rbStasm, SIGNAL(clicked()), this, SLOT(setLandmarksUsage()));
        QObject::connect(m_uiMorphing->rbManual, SIGNAL(clicked()), this, SLOT(setLandmarksUsage()));

        QObject::connect(this, SIGNAL(updateTargetTexture(QString)), m_pGLOSNRICP, SLOT(updateTargetTexture(QString)));


        // this
        QObject::connect(this,                              SIGNAL(saveMeshFileSignal(QString)),m_pWMorphing,SLOT(saveMeshFile(QString)));

        resetAllParameters();

    // init thread
        m_pWMorphing->moveToThread(&m_TMorphing);
        m_TMorphing.start();
}

SWMorphingInterface::~SWMorphingInterface()
{
    m_TMorphing.quit();
    m_TMorphing.wait();

    deleteAndNullify(m_pWMorphing);
}


void SWMorphingInterface::resetAllParameters()
{
    // set default values
    m_uiMorphing->dsbFactor->setValue(m_dCoeffValueDefaultValue);
    m_uiMorphing->hsRotX->setValue(m_i32RotXDefaultValue);
    m_uiMorphing->hsRotY->setValue(m_i32RotYDefaultValue);
    m_uiMorphing->hsRotZ->setValue(m_i32RotZDefaultValue);
    m_uiMorphing->hsScale->setValue(m_i32ScalingDefaultValue);
    m_uiMorphing->hsTransX->setValue(m_i32TransXDefaultValue);
    m_uiMorphing->hsTransY->setValue(m_i32TransYDefaultValue);
    m_uiMorphing->hsTransZ->setValue(m_i32TransZDefaultValue);
    m_uiMorphing->dsbStartAlpha->setValue(m_dStartAlphaDefaultValue);
    m_uiMorphing->dsbMinAlpha->setValue(m_dMinAlphaDefaultValue);
    m_uiMorphing->dsbDiffMax->setValue(m_dDiffMaxDefaultValue);
    m_uiMorphing->dsbBeta->setValue(m_dBetaDefaultValue);
    m_uiMorphing->dsbGama->setValue(m_dGamaDefaultValue);
    m_uiMorphing->dsbAngleMax->setValue(m_dAngleMaxDefaultValue);

    m_pGLOSNRICP->setRotTargetX(m_i32RotXDefaultValue); m_pGLOSNRICP->setRotTargetY(m_i32RotYDefaultValue);
    m_pGLOSNRICP->setRotTargetZ(m_i32RotZDefaultValue);
    m_pGLOSNRICP->setTransTargetX(m_i32TransXDefaultValue); m_pGLOSNRICP->setTransTargetY(m_i32TransYDefaultValue);
    m_pGLOSNRICP->setTransTargetZ(m_i32TransZDefaultValue);
    m_pGLOSNRICP->setTargetScaling(m_i32ScalingDefaultValue);
    m_pGLOSNRICP->setCoeffAlpha(m_dCoeffValueDefaultValue);
}

void SWMorphingInterface::saveMeshFile()
{
    QString l_sPathFileToSave = QFileDialog::getSaveFileName(this, "Save mesh file", QString(), "Mesh file (*.obj)");
    emit saveMeshFileSignal(l_sPathFileToSave);
}

void SWMorphingInterface::setTemplateMeshPath()
{
    QString l_sPathTemplateMesh = QFileDialog::getOpenFileName(this, "Load template mesh", QString(), "Mesh file (*.obj)");;
    m_uiMorphing->lePathTemplate->setText(l_sPathTemplateMesh);
    m_pGLOSNRICP->setSourceMesh(l_sPathTemplateMesh);   
    m_bTemplateDefined = true;

    if(m_bTemplateDefined && m_bTargetDefined)
    {
        m_uiMorphing->pbStart->setEnabled(true);
        m_uiMorphing->pbSaveMorphedMesh->setEnabled(true);
        m_uiMorphing->pbCorr->setEnabled(true);
        m_uiMorphing->pbSetLandManually->setEnabled(true);
    }
}

void SWMorphingInterface::setTargetMeshPath()
{
    QString l_sPathTargetMesh = QFileDialog::getOpenFileName(this, "Load target mesh", QString(), "Mesh file (*.obj)");;
    m_uiMorphing->lePathTarget->setText(l_sPathTargetMesh);
    m_pGLOSNRICP->setTargetMesh(l_sPathTargetMesh);
    m_bTargetDefined = true;

    if(m_bTemplateDefined && m_bTargetDefined)
    {
        m_uiMorphing->pbStart->setEnabled(true);
        m_uiMorphing->pbSaveMorphedMesh->setEnabled(true);
        m_uiMorphing->pbCorr->setEnabled(true);
        m_uiMorphing->pbSetLandManually->setEnabled(true);
        m_uiMorphing->pbEraseLand->setEnabled(true);
    }
}

int main(int argc, char* argv[])
{
    QApplication l_oApp(argc, argv);
    SWMorphingInterface l_oMorphingInterface;
    l_oMorphingInterface.resize(1800, 900);
    l_oMorphingInterface.move(50,50);
    l_oMorphingInterface.show();

    return l_oApp.exec();
}

void SWMorphingInterface::unlockInterface()
{
    m_uiMorphing->pbStart->setEnabled(true);
    m_uiMorphing->pbSaveMorphedMesh->setEnabled(true);
    m_uiMorphing->pbResetParameters->setEnabled(true);
    m_uiMorphing->pbCorr->setEnabled(true);
    m_uiMorphing->pbResetMorphing->setEnabled(true);
    m_uiMorphing->pbSetTemplate->setEnabled(true);
    m_uiMorphing->pbSetTarget->setEnabled(true);
    m_uiMorphing->dsbFactor->setEnabled(true);
    m_uiMorphing->cbUseLandmarks->setEnabled(true);
    m_uiMorphing->hsRotX->setEnabled(true);
    m_uiMorphing->hsRotY->setEnabled(true);
    m_uiMorphing->hsRotZ->setEnabled(true);
    m_uiMorphing->hsTransX->setEnabled(true);
    m_uiMorphing->hsTransY->setEnabled(true);
    m_uiMorphing->hsTransZ->setEnabled(true);
    m_uiMorphing->hsScale->setEnabled(true);
    m_uiMorphing->pbSetLandManually->setEnabled(true);
    m_uiMorphing->pbEraseLand->setEnabled(true);

    // expert
        m_uiMorphing->dsbStartAlpha->setEnabled(true);
        m_uiMorphing->dsbMinAlpha->setEnabled(true);
        m_uiMorphing->dsbDiffMax->setEnabled(true);
        m_uiMorphing->dsbBeta->setEnabled(true);
        m_uiMorphing->dsbGama->setEnabled(true);
        m_uiMorphing->dsbAngleMax->setEnabled(true);

    m_uiMorphing->pbStop->setEnabled(false);
}

void SWMorphingInterface::lockInterface()
{
    m_uiMorphing->pbStart->setDisabled(true);
    m_uiMorphing->pbSaveMorphedMesh->setDisabled(true);
    m_uiMorphing->pbResetParameters->setDisabled(true);
    m_uiMorphing->pbCorr->setDisabled(true);
    m_uiMorphing->pbResetMorphing->setDisabled(true);
    m_uiMorphing->pbSetTemplate->setDisabled(true);
    m_uiMorphing->pbSetTarget->setDisabled(true);
    m_uiMorphing->dsbFactor->setDisabled(true);
    m_uiMorphing->cbUseLandmarks->setDisabled(true);
    m_uiMorphing->hsRotX->setDisabled(true);
    m_uiMorphing->hsRotY->setDisabled(true);
    m_uiMorphing->hsRotZ->setDisabled(true);
    m_uiMorphing->hsTransX->setDisabled(true);
    m_uiMorphing->hsTransY->setDisabled(true);
    m_uiMorphing->hsTransZ->setDisabled(true);
    m_uiMorphing->hsScale->setDisabled(true);
    m_uiMorphing->pbSetLandManually->setDisabled(true);
    m_uiMorphing->pbEraseLand->setDisabled(true);

    // expert
        m_uiMorphing->dsbStartAlpha->setDisabled(true);
        m_uiMorphing->dsbMinAlpha->setDisabled(true);
        m_uiMorphing->dsbDiffMax->setDisabled(true);
        m_uiMorphing->dsbBeta->setDisabled(true);
        m_uiMorphing->dsbGama->setDisabled(true);
        m_uiMorphing->dsbAngleMax->setDisabled(true);

    m_uiMorphing->pbStop->setDisabled(false);
}


void SWMorphingInterface::enableGLFullScreen()
{
    if(!m_bGLFullScreen)
    {
        m_pGLContainer->setParent(0);
        m_pGLContainer->showFullScreen();
        m_bGLFullScreen = true;
    }
}

void SWMorphingInterface::disableGLFullScreen()
{
    if(m_bGLFullScreen)
    {
        m_uiMorphing->glScene->addWidget(m_pGLContainer);
        m_bGLFullScreen = false;
    }
}


void SWMorphingInterface::setLandmarksManually()
{
    lockInterfaceForLandmarksSetting();
    m_bSetLandmarksManually = true;
    m_bSetLandmarksTemplate = true;
    setLandmarks3DDisplayTemplate();
    m_pGLOSNRICP->setTemplateMeshPointSelection(true);

    m_templateMeshClickedPointsInfo = "";
    m_targetMeshClickedPointsInfo = "";

    m_uiMorphing->tbTemplateLandmarks->setText(m_templateMeshClickedPointsInfo);
    m_uiMorphing->tbTargetLandmarks->setText(m_targetMeshClickedPointsInfo);
}

void SWMorphingInterface::setLandmarks3DDisplayTemplate()
{
    m_pGLOSNRICP->setInfo3DDisplay("Select a point on the template mesh (space key for validating) ");

    m_uiMorphing->cbTemplateCloud->setChecked(true);
    m_uiMorphing->cbTemplateMesh->setChecked(true);
    m_uiMorphing->cbTemplateFillMesh->setChecked(true);
    m_uiMorphing->cbTargetMesh->setChecked(true);

    m_uiMorphing->cbTemplateTriNormal->setChecked(false);
    m_uiMorphing->cbTemplateVertNormal->setChecked(false);
    m_uiMorphing->cbTargetCloud->setChecked(false);
    m_uiMorphing->cbTargetTriNormal->setChecked(false);
    m_uiMorphing->cbTargetVertNormal->setChecked(false);
    m_uiMorphing->cbTargetFillMesh->setChecked(false);
    m_uiMorphing->cbCorr->setChecked(false);
    m_uiMorphing->cbLandmarks->setChecked(false);

    m_pGLOSNRICP->setCloudSDisplay(true);
    m_pGLOSNRICP->setMeshSDisplay(true);
    m_pGLOSNRICP->setFillSDisplay(true);
    m_pGLOSNRICP->setMeshTDisplay(true);

    m_pGLOSNRICP->setLandMarksDisplay(false);
    m_pGLOSNRICP->setCloudTDisplay(false);
    m_pGLOSNRICP->setTrianglesNormalsSDisplay(false);
    m_pGLOSNRICP->setTrianglesNormalsTDisplay(false);
    m_pGLOSNRICP->setVerticesNormalsSDisplay(false);
    m_pGLOSNRICP->setVerticesNormalsTDisplay(false);
    m_pGLOSNRICP->setCorrDisplay(false);
    m_pGLOSNRICP->setFillTDisplay(false);
}


void SWMorphingInterface::setLandmarks3DDisplayTarget()
{
    m_pGLOSNRICP->setInfo3DDisplay("Select a corresponding point on the target mesh (space key for validating) ");

    m_uiMorphing->cbTargetCloud->setChecked(true);
    m_uiMorphing->cbTargetMesh->setChecked(true);
    m_uiMorphing->cbTargetFillMesh->setChecked(true);
    m_uiMorphing->cbTemplateMesh->setChecked(true);

    m_uiMorphing->cbTemplateTriNormal->setChecked(false);
    m_uiMorphing->cbTemplateVertNormal->setChecked(false);
    m_uiMorphing->cbTemplateFillMesh->setChecked(false);
    m_uiMorphing->cbTemplateCloud->setChecked(false);
    m_uiMorphing->cbTemplateMesh->setChecked(false);
    m_uiMorphing->cbTargetTriNormal->setChecked(false);
    m_uiMorphing->cbTargetVertNormal->setChecked(false);
    m_uiMorphing->cbCorr->setChecked(false);
    m_uiMorphing->cbLandmarks->setChecked(false);

    m_pGLOSNRICP->setCloudTDisplay(true);
    m_pGLOSNRICP->setMeshTDisplay(true);
    m_pGLOSNRICP->setFillTDisplay(true);
    m_pGLOSNRICP->setMeshSDisplay(true);

    m_pGLOSNRICP->setLandMarksDisplay(false);
    m_pGLOSNRICP->setCloudSDisplay(false);
    m_pGLOSNRICP->setTrianglesNormalsSDisplay(false);
    m_pGLOSNRICP->setTrianglesNormalsTDisplay(false);
    m_pGLOSNRICP->setVerticesNormalsSDisplay(false);
    m_pGLOSNRICP->setVerticesNormalsTDisplay(false);
    m_pGLOSNRICP->setCorrDisplay(false);
    m_pGLOSNRICP->setFillSDisplay(false);
}

void SWMorphingInterface::landmarksSetManuallyNextPoint()
{
    //
    if(m_bSetLandmarksTemplate)
    {
        int l_idClickedTemplateMeshPoint;
        QVector3D l_clickedTemplateMeshPoint;
        m_pGLOSNRICP->stockCurrentClickedMeshPoint(true, l_idClickedTemplateMeshPoint,l_clickedTemplateMeshPoint);
        qDebug() << "template point selected : " << l_clickedTemplateMeshPoint << " id -> " << l_idClickedTemplateMeshPoint;
        m_bSetLandmarksTemplate = false;
        m_pGLOSNRICP->setTemplateMeshPointSelection(false);

        // add selected point to the text display
        m_templateMeshClickedPointsInfo += QString("Id : " + QString::number(l_idClickedTemplateMeshPoint) + "\n v : " +
                                                   QString::number(l_clickedTemplateMeshPoint.x()) + " " + QString::number(l_clickedTemplateMeshPoint.y())
                                                   + " " + QString::number(l_clickedTemplateMeshPoint.z()) + "\n\n");

        emit sendInfoTemplateClickedPoints(m_templateMeshClickedPointsInfo);
        setLandmarks3DDisplayTarget();
    }
    else
    {
        int l_idClickedTargetMeshPoint;
        QVector3D l_clickedTargetMeshPoint;
        m_pGLOSNRICP->stockCurrentClickedMeshPoint(false, l_idClickedTargetMeshPoint,l_clickedTargetMeshPoint);
        qDebug() << "target point selected : " << l_clickedTargetMeshPoint << " id -> " << l_idClickedTargetMeshPoint;
        m_bSetLandmarksTemplate = true;
        m_pGLOSNRICP->setTemplateMeshPointSelection(true);

        // add selected point to the text display
        m_targetMeshClickedPointsInfo += QString("Id : " + QString::number(l_idClickedTargetMeshPoint) + "\n v : " +
                                                   QString::number(l_clickedTargetMeshPoint.x()) + " " + QString::number(l_clickedTargetMeshPoint.y())
                                                 + " " + QString::number(l_clickedTargetMeshPoint.z()) + "\n\n");

        emit sendInfoTargetClickedPoints(m_targetMeshClickedPointsInfo);
        setLandmarks3DDisplayTemplate();
    }
}


void SWMorphingInterface::eraseManuallyLandmarks()
{
    m_templateMeshClickedPointsInfo = "";
    m_targetMeshClickedPointsInfo = "";

    m_uiMorphing->tbTemplateLandmarks->setText(m_templateMeshClickedPointsInfo);
    m_uiMorphing->tbTargetLandmarks->setText(m_targetMeshClickedPointsInfo);

    m_pGLOSNRICP->eraseManuallyLandmarks();

    m_pGLOSNRICP->setInfo3DDisplay("Landmarks erased. Set new landmarks manually or use STASM. ");
}

void SWMorphingInterface::setLandmarksUsage()
{
    if(m_uiMorphing->rbStasm->isChecked())
    {
        m_pGLOSNRICP->setLandMarksDisplay(true);
        m_pGLOSNRICP->updateLandmarksWithSTASM();
        m_pGLOSNRICP->setInfo3DDisplay("STASM landmarks");
    }
    else
    {
        m_pGLOSNRICP->setLandMarksDisplay(false);
        m_pGLOSNRICP->updateLandmarksWithManualSelection();
        m_pGLOSNRICP->setInfo3DDisplay("Manual landmarks. ");
    }
}

void SWMorphingInterface::lockInterfaceForLandmarksSetting()
{
    lockInterface();
    m_uiMorphing->pbStop->setDisabled(true);
}

void SWMorphingInterface::unlockInterfaceForLandmarksSetting()
{
    unlockInterface();
    m_uiMorphing->pbStop->setDisabled(false);
}

void SWMorphingInterface::landmarksSetManuallyEscape()
{
    if(m_bSetLandmarksManually && m_bSetLandmarksTemplate)
    {
        unlockInterfaceForLandmarksSetting();
        m_bSetLandmarksManually = false;

        // force landmarks update
            setLandmarksUsage();

            m_pGLOSNRICP->setInfo3DDisplay("Manual landmarks set. ");
    }
    else if(m_bSetLandmarksManually)
    {
        std::cout << "Select a point on the target mesh before ending landmark manual selection. " << std::endl;
    }
}


void SWMorphingInterface::setTargetTexture()
{
    // retrieve obj path
        m_pathTargetTexture = QFileDialog::getOpenFileName(this, "Load texture", QString(), "Texture file (*.png)");
        emit updateTargetTexture(m_pathTargetTexture);

        m_pGLOSNRICP->setInfo3DDisplay("Texture loaded on the taget mesh. ");
}


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


SWMorphingInterface::SWMorphingInterface()
{
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

    QGridLayout *l_pMainGridLayout = new QGridLayout();
    QWidget *l_pCentralWidget = new QWidget();
    setCentralWidget(l_pCentralWidget);

    QWidget *l_pWLeftContainer = new QWidget(), *l_pWMiddleContainer = new QWidget(), *l_pWRightContainer = new QWidget();
    l_pWMiddleContainer->setMinimumWidth(800);
    l_pWMiddleContainer->setMinimumHeight(600);
    l_pMainGridLayout->addWidget(l_pWLeftContainer,   0, 0, 0, 2);
    l_pMainGridLayout->addWidget(l_pWMiddleContainer, 0, 2, 0, 6);
    l_pMainGridLayout->addWidget(l_pWRightContainer,  0, 8, 0, 2);
    l_pCentralWidget->setLayout(l_pMainGridLayout);


    // left container
        QGridLayout *l_pLeftContainerLayout = new QGridLayout();
        QWidget *l_pWMeshChoiceContainer = new QWidget(), *l_pWParametersContainer = new QWidget();
        l_pLeftContainerLayout->addWidget(l_pWParametersContainer,0,0,1,1, Qt::AlignCenter);
        l_pLeftContainerLayout->addWidget(l_pWMeshChoiceContainer,2,0,1,1, Qt::AlignCenter);
        l_pWLeftContainer->setLayout(l_pLeftContainerLayout);

        // parameters
            // qtabWidget
                m_pUi_WParamsDispl = new Ui::SWUI_WParamsDisplay();
                m_pUi_WParamsDispl->setupUi(l_pWParametersContainer);
                m_pUi_WParamsDispl->twParamsDisplay->setCurrentIndex(0);
            // spinbox
                m_pUi_WParamsDispl->dsbStartAlpha->setMinimum(0.0);     m_pUi_WParamsDispl->dsbStartAlpha->setMaximum(1000.0);
                m_pUi_WParamsDispl->dsbMinAlpha->setMinimum(0.0);       m_pUi_WParamsDispl->dsbMinAlpha->setMaximum(100.0);
                m_pUi_WParamsDispl->dsbDiffMax->setMinimum(0.0);        m_pUi_WParamsDispl->dsbDiffMax->setMaximum(500.0);
                m_pUi_WParamsDispl->dsbAngleMax->setMinimum(0.0);       m_pUi_WParamsDispl->dsbAngleMax->setMaximum(180.0);
                m_pUi_WParamsDispl->dsbAngleMax->setDecimals(2);
                m_pUi_WParamsDispl->dsbBeta->setMinimum(0.0);           m_pUi_WParamsDispl->dsbBeta->setMaximum(150.0);
                m_pUi_WParamsDispl->dsbCoeffAlpha->setMinimum(0.0);     m_pUi_WParamsDispl->dsbCoeffAlpha->setMaximum(1.0);
                m_pUi_WParamsDispl->dsbGama->setMinimum(0.0);           m_pUi_WParamsDispl->dsbGama->setMaximum(10000.0);

            // checkbox
                m_pUi_WParamsDispl->cbDisplaySourceMesh->setChecked(true); m_pUi_WParamsDispl->cbDisplayTargetMesh->setChecked(true);
                m_pUi_WParamsDispl->cbCorr->setChecked(true);
                m_pUi_WParamsDispl->cbLandMarksPoints->setChecked(true);
                m_pUi_WParamsDispl->cbUseLandMarks->setChecked(false);

        // mesh choice
            // qTabWidget
                m_pUi_WMeshCorr = new Ui::SWUI_WMeshCorr();
                m_pUi_WMeshCorr->setupUi(l_pWMeshChoiceContainer);
                m_pUi_WMeshCorr->twMeshCorr->setCurrentIndex(0);

            // create list items
                std::vector<QString> l_vQSSourceMeshItems, l_vQSTargetMeshItems;
                std::vector<QString> l_vQSSourceCorrItems, l_vQSTargetCorrItems;
                QString l_sBaseMeshes("../data/meshes/"), l_sBaseCorr("../data/meshes/stasmCorr/");
                // source meshes
                    l_vQSSourceMeshItems.push_back(l_sBaseMeshes + "generic_headCleaned.obj");
                    l_vQSSourceMeshItems.push_back(l_sBaseMeshes + "generic_faceOnly.obj");
                    l_vQSSourceMeshItems.push_back(l_sBaseMeshes + "generic_head_openmouth2.obj");
                // target meshes
                    l_vQSTargetMeshItems.push_back(l_sBaseMeshes + "avatars/target.obj");
                    l_vQSTargetMeshItems.push_back(l_sBaseMeshes + "generic_headCleaned.obj");
                    l_vQSTargetMeshItems.push_back(l_sBaseMeshes + "neutralFaceShift_florian3.obj");
                    l_vQSTargetMeshItems.push_back(l_sBaseMeshes + "target_openMouth.obj");
                // source stasm corr
                    l_vQSSourceCorrItems.push_back(l_sBaseCorr + "genericMeshstasmLandMarks.txt");
                    l_vQSSourceCorrItems.push_back(l_sBaseCorr + "faceshiftMeshstasmLandMarks.txt");
                // target stasm corr
                    l_vQSTargetCorrItems.push_back(l_sBaseCorr + "faceshiftMeshstasmLandMarks.txt");
                    l_vQSTargetCorrItems.push_back(l_sBaseCorr + "target_corr.txt");
                    l_vQSTargetCorrItems.push_back(l_sBaseCorr + "target_openMouth.txt");

            // fill qlistWidget
                for(uint ii = 0; ii < l_vQSSourceMeshItems.size(); ++ii)
                {
                    m_pUi_WMeshCorr->lWSourceMeshes->addItem(l_vQSSourceMeshItems[ii]);
                }
                for(uint ii = 0; ii < l_vQSTargetMeshItems.size(); ++ii)
                {
                    m_pUi_WMeshCorr->lWTargetMeshes->addItem(l_vQSTargetMeshItems[ii]);
                }
                for(uint ii = 0; ii < l_vQSSourceCorrItems.size(); ++ii)
                {
                    m_pUi_WMeshCorr->lWSourceCorr->addItem(l_vQSSourceCorrItems[ii]);
                }
                for(uint ii = 0; ii < l_vQSTargetCorrItems.size(); ++ii)
                {
                    m_pUi_WMeshCorr->lWTargetCorr->addItem(l_vQSTargetCorrItems[ii]);
                }                              

    // right container
        QGridLayout *l_pRightContainerLayout = new QGridLayout();
        QWidget *l_pActionsContainer = new QWidget(), *l_pTransfoContainer = new QWidget();
        l_pRightContainerLayout->addWidget(l_pTransfoContainer, 0, 0, 2, 2, Qt::AlignCenter);
        l_pRightContainerLayout->addWidget(l_pActionsContainer, 2, 0, 1, 1, Qt::AlignCenter);
        l_pWRightContainer->setLayout(l_pRightContainerLayout);

        // transformations
            m_pUi_WTTransf = new Ui::SWUI_WTargetTransfo();
            m_pUi_WTTransf->setupUi(l_pTransfoContainer);
            // translations
                m_pUi_WTTransf->hsXTr->setMinimum(0); m_pUi_WTTransf->hsXTr->setMaximum(1000);
                m_pUi_WTTransf->hsYTr->setMinimum(0); m_pUi_WTTransf->hsYTr->setMaximum(1000);
                m_pUi_WTTransf->hsZTr->setMinimum(0); m_pUi_WTTransf->hsZTr->setMaximum(1000);
            // rotations
                m_pUi_WTTransf->hsXRot->setMinimum(0); m_pUi_WTTransf->hsXRot->setMaximum(3600);
                m_pUi_WTTransf->hsYRot->setMinimum(0); m_pUi_WTTransf->hsYRot->setMaximum(3600);
                m_pUi_WTTransf->hsZRot->setMinimum(0); m_pUi_WTTransf->hsZRot->setMaximum(3600);
            // scale
                m_pUi_WTTransf->hsScale->setMinimum(0); m_pUi_WTTransf->hsScale->setMaximum(1000);

        // actions
            QVBoxLayout *l_pActionsLayout = new QVBoxLayout();

            // pushbutton
                m_pPBStart = new QPushButton("Start"), m_pPBReset = new QPushButton("Reset");
                m_pPBStop = new QPushButton("Stop"); m_pPBStop->setEnabled(false);
                m_pPBSaveMesh = new QPushButton("Save current source mesh");
                m_pPBComputeCorr = new QPushButton("Compute corresondances");
                m_pPBResetParameters = new QPushButton("Reset parameters");
                m_pPBStart->setMinimumWidth(150); m_pPBReset->setMinimumWidth(150); m_pPBComputeCorr->setMinimumWidth(150);
                m_pPBStop->setMinimumWidth(150); m_pPBSaveMesh->setMinimumWidth(150); m_pPBResetParameters->setMinimumWidth(150);
                l_pActionsLayout->addWidget(m_pPBStart); l_pActionsLayout->addWidget(m_pPBStop);
                l_pActionsLayout->addWidget(m_pPBReset); l_pActionsLayout->addWidget(m_pPBComputeCorr);
                l_pActionsLayout->addWidget(m_pPBSaveMesh); l_pActionsLayout->addWidget(m_pPBResetParameters);

            l_pActionsContainer->setLayout(l_pActionsLayout);

    // middle container
        QHBoxLayout *l_pMiddleContainerLayout = new QHBoxLayout();
        QGLFormat l_glFormat;
        l_glFormat.setVersion( 4, 3 );
        l_glFormat.setProfile(  QGLFormat::CompatibilityProfile);
        l_glFormat.setSampleBuffers( true );
        QGLContext *l_glContext = new QGLContext(l_glFormat);
        m_pGLOSNRICP = new SWGLOptimalStepNonRigidICP(l_glContext, l_pWMiddleContainer);
        m_pGLOSNRICP->setMinimumWidth(800);
        m_pGLOSNRICP->setMinimumHeight(600);
        l_pMiddleContainerLayout->addWidget(m_pGLOSNRICP);
        l_pWMiddleContainer->setLayout(l_pMiddleContainerLayout);


    // init worker
        m_pWMorphing = new SWMorphingWorker(m_pGLOSNRICP);
        QObject::connect(m_pPBStart,            SIGNAL(clicked()),                      m_pWMorphing, SLOT(startMorphing()));
        QObject::connect(m_pPBStop,             SIGNAL(clicked()),                      m_pWMorphing, SLOT(stopMorphing()));
        QObject::connect(m_pPBSaveMesh,         SIGNAL(clicked()),                      this,         SLOT(saveMeshFile()));
        QObject::connect(m_pWMorphing,          SIGNAL(updateSceneDisplaySignal()),     m_pGLOSNRICP, SLOT(refreshDisplay()));

        QObject::connect(m_pWMorphing,          SIGNAL(startMorphingSignal()),          this,         SLOT(lockInterface()));
        QObject::connect(m_pWMorphing,          SIGNAL(endMorphingSignal()),            this,         SLOT(unlockInterface()));
        QObject::connect(this,                  SIGNAL(saveMeshFileSignal(QString)),    m_pWMorphing, SLOT(saveMeshFile(QString)));

        QObject::connect(m_pPBReset,                        SIGNAL(clicked()),                  m_pGLOSNRICP, SLOT(resetMorphing()));
        QObject::connect(m_pUi_WMeshCorr->pbSetMeshes,      SIGNAL(clicked()),                  m_pGLOSNRICP, SLOT(resetMorphingWithNewMeshes()));
        QObject::connect(m_pUi_WMeshCorr->pbSetStamCorr,    SIGNAL(clicked()),                  m_pGLOSNRICP, SLOT(resetMorphingWithNewCorr()));
        QObject::connect(m_pPBComputeCorr,                  SIGNAL(clicked()),                  m_pGLOSNRICP, SLOT(computeDistWAndCorr()));
        QObject::connect(m_pPBResetParameters,              SIGNAL(clicked()),                  this,         SLOT(resetAllParameters()));

        QObject::connect(m_pUi_WParamsDispl->cbDisplaySourceMesh,       SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setMeshSDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbDisplayTargetMesh,       SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setMeshTDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbUseLandMarks,            SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setUseLandMarks(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbCloudSourceMesh,         SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setCloudSDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbCloudTargetMesh,         SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setCloudTDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbTriNormalsSourceMesh,    SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setTrianglesNormalsSDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbTriNormalsTargetMesh,    SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setTrianglesNormalsTDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbVertNormalsSourceMesh,   SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setVerticesNormalsSDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbVertNormalsTargetMesh,   SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setVerticesNormalsTDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbLandMarksPoints,         SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setLandMarksDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbCorr,                    SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setCorrDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbFillSourceMesh,          SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setFillSDisplay(bool)));
        QObject::connect(m_pUi_WParamsDispl->cbFillTargetMesh,          SIGNAL(clicked(bool)),    m_pGLOSNRICP, SLOT(setFillTDisplay(bool)));

        QObject::connect(m_pUi_WTTransf->hsXRot,    SIGNAL(valueChanged(int)),              m_pGLOSNRICP, SLOT(setRotTargetX(int)));
        QObject::connect(m_pUi_WTTransf->hsYRot,    SIGNAL(valueChanged(int)),              m_pGLOSNRICP, SLOT(setRotTargetY(int)));
        QObject::connect(m_pUi_WTTransf->hsZRot,    SIGNAL(valueChanged(int)),              m_pGLOSNRICP, SLOT(setRotTargetZ(int)));
        QObject::connect(m_pUi_WTTransf->hsXTr,     SIGNAL(valueChanged(int)),              m_pGLOSNRICP, SLOT(setTransTargetX(int)));
        QObject::connect(m_pUi_WTTransf->hsYTr,     SIGNAL(valueChanged(int)),              m_pGLOSNRICP, SLOT(setTransTargetY(int)));
        QObject::connect(m_pUi_WTTransf->hsZTr,     SIGNAL(valueChanged(int)),              m_pGLOSNRICP, SLOT(setTransTargetZ(int)));
        QObject::connect(m_pUi_WTTransf->hsScale,   SIGNAL(valueChanged(int)),              m_pGLOSNRICP, SLOT(setTargetScaling(int)));

        QObject::connect(m_pUi_WParamsDispl->dsbStartAlpha,      SIGNAL(valueChanged(double)),           m_pGLOSNRICP, SLOT(setStartAlpha(double)));
        QObject::connect(m_pUi_WParamsDispl->dsbMinAlpha,        SIGNAL(valueChanged(double)),           m_pGLOSNRICP, SLOT(setMinAlpha(double)));
        QObject::connect(m_pUi_WParamsDispl->dsbDiffMax,         SIGNAL(valueChanged(double)),           m_pGLOSNRICP, SLOT(setDiffMax(double)));
        QObject::connect(m_pUi_WParamsDispl->dsbBeta,            SIGNAL(valueChanged(double)),           m_pGLOSNRICP, SLOT(setBeta(double)));
        QObject::connect(m_pUi_WParamsDispl->dsbCoeffAlpha,      SIGNAL(valueChanged(double)),           m_pGLOSNRICP, SLOT(setCoeffAlpha(double)));
        QObject::connect(m_pUi_WParamsDispl->dsbAngleMax,        SIGNAL(valueChanged(double)),           m_pGLOSNRICP, SLOT(setAngleMax(double)));
        QObject::connect(m_pUi_WParamsDispl->dsbGama,            SIGNAL(valueChanged(double)),           m_pGLOSNRICP, SLOT(setGama(double)));

        QObject::connect(m_pUi_WMeshCorr->lWSourceMeshes,     SIGNAL(currentTextChanged(QString)),    m_pGLOSNRICP, SLOT(setSourceMesh(QString)));
        QObject::connect(m_pUi_WMeshCorr->lWTargetMeshes,     SIGNAL(currentTextChanged(QString)),    m_pGLOSNRICP, SLOT(setTargetMesh(QString)));
        QObject::connect(m_pUi_WMeshCorr->lWSourceCorr,       SIGNAL(currentTextChanged(QString)),    m_pGLOSNRICP, SLOT(setSourceCorr(QString)));
        QObject::connect(m_pUi_WMeshCorr->lWTargetCorr,       SIGNAL(currentTextChanged(QString)),    m_pGLOSNRICP, SLOT(setTargetCorr(QString)));

        m_pGLOSNRICP->setSourceMesh(m_pUi_WMeshCorr->lWSourceMeshes->item(0)->text());
        m_pGLOSNRICP->setTargetMesh(m_pUi_WMeshCorr->lWTargetMeshes->item(0)->text());
        m_pGLOSNRICP->setSourceCorr(m_pUi_WMeshCorr->lWSourceCorr->item(0)->text());
        m_pGLOSNRICP->setTargetCorr(m_pUi_WMeshCorr->lWTargetCorr->item(0)->text());

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
    m_pUi_WParamsDispl->dsbStartAlpha->setValue(m_dStartAlphaDefaultValue);
    m_pUi_WParamsDispl->dsbMinAlpha->setValue(m_dMinAlphaDefaultValue);
    m_pUi_WParamsDispl->dsbDiffMax->setValue(m_dDiffMaxDefaultValue);
    m_pUi_WParamsDispl->dsbAngleMax->setValue(m_dAngleMaxDefaultValue);
    m_pUi_WParamsDispl->dsbBeta->setValue(m_dBetaDefaultValue);
    m_pUi_WParamsDispl->dsbCoeffAlpha->setValue(m_dCoeffValueDefaultValue);
    m_pUi_WParamsDispl->dsbGama->setValue(m_dGamaDefaultValue);
    m_pUi_WTTransf->hsXTr->setValue(m_i32TransXDefaultValue);
    m_pUi_WTTransf->hsYTr->setValue(m_i32TransYDefaultValue);
    m_pUi_WTTransf->hsZTr->setValue(m_i32TransZDefaultValue);
    m_pUi_WTTransf->hsXRot->setValue(m_i32RotXDefaultValue);
    m_pUi_WTTransf->hsYRot->setValue(m_i32RotYDefaultValue);
    m_pUi_WTTransf->hsZRot->setValue(m_i32RotZDefaultValue);
    m_pUi_WTTransf->hsScale->setValue(m_i32ScalingDefaultValue);

    m_pGLOSNRICP->setRotTargetX(m_i32RotXDefaultValue); m_pGLOSNRICP->setRotTargetY(m_i32RotYDefaultValue);
    m_pGLOSNRICP->setRotTargetZ(m_i32RotZDefaultValue);
    m_pGLOSNRICP->setTransTargetX(m_i32TransXDefaultValue); m_pGLOSNRICP->setTransTargetY(m_i32TransYDefaultValue);
    m_pGLOSNRICP->setTransTargetZ(m_i32TransZDefaultValue);
    m_pGLOSNRICP->setTargetScaling(m_i32ScalingDefaultValue);
    m_pGLOSNRICP->setStartAlpha(m_dStartAlphaDefaultValue); m_pGLOSNRICP->setMinAlpha(m_dMinAlphaDefaultValue);
    m_pGLOSNRICP->setDiffMax(m_dDiffMaxDefaultValue); m_pGLOSNRICP->setBeta(m_dBetaDefaultValue);
    m_pGLOSNRICP->setCoeffAlpha(m_dCoeffValueDefaultValue); m_pGLOSNRICP->setAngleMax(m_dAngleMaxDefaultValue);
    m_pGLOSNRICP->setGama(m_dGamaDefaultValue);

}

void SWMorphingInterface::lockInterface()
{
    m_pPBStart->setEnabled(false);
    m_pPBReset->setEnabled(false);
    m_pPBSaveMesh->setEnabled(false);
    m_pPBComputeCorr->setEnabled(false);
    m_pPBResetParameters->setEnabled(false);
    m_pUi_WMeshCorr->pbSetMeshes->setEnabled(false);
    m_pUi_WMeshCorr->pbSetStamCorr->setEnabled(false);
    m_pUi_WTTransf->gbTargetMeshTr->setEnabled(false);
    m_pUi_WTTransf->gbTargetMeshRot->setEnabled(false);
    m_pUi_WTTransf->gbTargetMeshScale->setEnabled(false);

    m_pPBStop->setEnabled(true);    
}

void SWMorphingInterface::unlockInterface()
{
    m_pPBStart->setEnabled(true);
    m_pPBReset->setEnabled(true);
    m_pPBComputeCorr->setEnabled(true);
    m_pPBSaveMesh->setEnabled(true);
    m_pPBResetParameters->setEnabled(true);
    m_pUi_WMeshCorr->pbSetMeshes->setEnabled(true);
    m_pUi_WMeshCorr->pbSetStamCorr->setEnabled(true);
    m_pUi_WTTransf->gbTargetMeshTr->setEnabled(true);
    m_pUi_WTTransf->gbTargetMeshRot->setEnabled(true);
    m_pUi_WTTransf->gbTargetMeshScale->setEnabled(true);

    m_pPBStop->setEnabled(false);        
}

void SWMorphingInterface::saveMeshFile()
{
    QString l_sPathFileToSave = QFileDialog::getSaveFileName(this, "Save mesh file", QString(), "Mesh file (*.obj)");
    emit saveMeshFileSignal(l_sPathFileToSave);
}

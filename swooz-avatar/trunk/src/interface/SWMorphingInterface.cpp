
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

//    float m[3][3] = {{2,-1, 0},
//                    {-1, 2,-1},
//                     {0,-1, 2}};

//    cv::Mat M = cv::Mat(3, 3, CV_32FC1, m);
//    std::cout << "M : " << std::endl << M << std::endl;

//    cv::Mat L,U, LU;
////    swUtil::swCuda::solve(M,L,U, LU);
//    swUtil::swCuda::solve2(M,LU);
////    std::cout << "L : " << std::endl << L << std::endl;
////    std::cout << "U : " << std::endl << U << std::endl;
//    std::cout << "LU : " << std::endl << LU << std::endl;


//    cv::Mat mat(1000,1000, CV_32FC1, cv::Scalar(0.f));
//    for(int ii = 0; ii < mat.rows*mat.cols; ++ii)
//        if(rand()%100 == 0)
//            mat.at<float>(ii) = rand()%100;


//    cv::Mat X;
//    swUtil::swCuda::resolveAX_B(M, X);

//      std::cout << "X : " << std::endl << X << std::endl;


    m_bTemplateDefined  = false;
    m_bTargetDefined    = false;

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

    // middle container
        QHBoxLayout *l_pGLContainerLayout = new QHBoxLayout();
        QWidget *l_pGLContainer = new QWidget();
        QGLFormat l_glFormat;
        l_glFormat.setVersion( 4, 3 );
        l_glFormat.setProfile(  QGLFormat::CompatibilityProfile);
        l_glFormat.setSampleBuffers( true );
        QGLContext *l_glContext = new QGLContext(l_glFormat);
        m_pGLOSNRICP = new SWGLOptimalStepNonRigidICP(l_glContext, l_pGLContainer);
        l_pGLContainerLayout->addWidget(m_pGLOSNRICP);
        l_pGLContainer->setLayout(l_pGLContainerLayout);
        m_uiMorphing->glScene->addWidget(l_pGLContainer);


    // init worker
        m_pWMorphing = new SWMorphingWorker(m_pGLOSNRICP);

        // m_pWMorphing
        QObject::connect(m_pWMorphing,  SIGNAL(updateSceneDisplaySignal()),     m_pGLOSNRICP,               SLOT(refreshDisplay()));
        QObject::connect(m_pWMorphing,  SIGNAL(endMorphingSignal()),            this,                       SLOT(unlockInterface()));

        // m_pGLOSNRICP
        QObject::connect(m_pGLOSNRICP,  SIGNAL(sendSourceMeshInfos(QString)), m_uiMorphing->tbInfosTemplate,SLOT(setText(QString)));
        QObject::connect(m_pGLOSNRICP,  SIGNAL(sendTargetMeshInfos(QString)), m_uiMorphing->tbInfosTarget,  SLOT(setText(QString)));

        // m_uiMorphing
            // pushbuttons
        QObject::connect(m_uiMorphing->pbStart,             SIGNAL(clicked()),      m_pWMorphing,           SLOT(startMorphing()));
        QObject::connect(m_uiMorphing->pbStart,             SIGNAL(clicked()),      this,                   SLOT(lockInterface()));
        QObject::connect(m_uiMorphing->pbStop,              SIGNAL(clicked()),      m_pWMorphing,           SLOT(stopMorphing()));
        QObject::connect(m_uiMorphing->pbStop,              SIGNAL(clicked(bool)),  m_uiMorphing->pbStop,   SLOT(setEnabled(bool)));
        QObject::connect(m_uiMorphing->pbSaveMorphedMesh,   SIGNAL(clicked()),      this,                   SLOT(saveMeshFile()));
        QObject::connect(m_uiMorphing->pbSetTemplate,       SIGNAL(clicked()),      this,                   SLOT(setTemplateMeshPath()));
        QObject::connect(m_uiMorphing->pbSetTarget,         SIGNAL(clicked()),      this,                   SLOT(setTargetMeshPath()));
        QObject::connect(m_uiMorphing->pbResetMorphing,     SIGNAL(clicked()),      m_pGLOSNRICP,           SLOT(resetMorphing()));
        QObject::connect(m_uiMorphing->pbCorr,              SIGNAL(clicked()),      m_pGLOSNRICP,           SLOT(computeDistWAndCorr()));
        QObject::connect(m_uiMorphing->pbResetParameters,   SIGNAL(clicked()),      this,                   SLOT(resetAllParameters()));
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

//    delete[] test;
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

    // expert
        m_uiMorphing->dsbStartAlpha->setDisabled(true);
        m_uiMorphing->dsbMinAlpha->setDisabled(true);
        m_uiMorphing->dsbDiffMax->setDisabled(true);
        m_uiMorphing->dsbBeta->setDisabled(true);
        m_uiMorphing->dsbGama->setDisabled(true);
        m_uiMorphing->dsbAngleMax->setDisabled(true);

    m_uiMorphing->pbStop->setDisabled(false);
}
//    {
//        clock_t m_oProgramTime = clock();

//        int l_i32Size1 = 10000;
//        int l_i32Size2 = 10000;
//        int l_fI32Size1[] = {l_i32Size1, l_i32Size2};
//        int l_fI32Size2[] = {l_i32Size2, l_i32Size1};
//    //    cv::SparseMat_<double> sm1(2, l_fI32Size1);
//    //    cv::SparseMat_<double> sm2(2, l_fI32Size2);
//    //    cv::SparseMat_<double> sm3;
//        cv::Mat m1(l_i32Size1,l_i32Size2, CV_32FC1);
//        cv::Mat m2(l_i32Size2,l_i32Size1, CV_32FC1);
//        cv::Mat m3, m4;

//        for(int ii = 0; ii < l_i32Size1;++ii)
//        {
//            for(int jj = 0; jj < l_i32Size2; ++jj)
//            {
//                int v1 = rand()%100;
//                int v2 = rand()%100;

//                m1.at<float>(ii,jj) = v1;
//                m2.at<float>(jj,ii) = v2;

//    //            sm1.ref(ii,jj) = v1;
//    //            sm2.ref(jj,ii) = v2;

//            }
//        }

//        m_oProgramTime = clock();
//    //    swUtil::swCuda::blockMatrixMultiplication(sm1, sm2, sm3);
//        swUtil::swCuda::blockMatrixMultiplication(m1, m2, m4, 4);
//        std::cout << "cuda : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << std::endl;


//    //    m_oProgramTime = clock();
//    //    m3 = m1 * m2;
//    //    std::cout << "opencv : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << std::endl;
//    //    std::cout << m3 << " ";

//        int error = 0;

//    //    for(int ii = 0; ii < l_i32Size1;++ii)
//    //    {
//    //        for(int jj = 0; jj < l_i32Size1; ++jj)
//    //        {

//    //            if(rand()%10000 == 0)
//    //            {
//    //                std::cout << m3.at<float>(ii,jj) << " " << m4.at<float>(ii,jj) << " | ";
//    //            }

//    //            if(m3.at<float>(ii,jj)* m3.at<float>(ii,jj) - m4.at<float>(ii,jj)* m4.at<float>(ii,jj) > 0.01f)
//    //            {
//    //                error++;
//    //            }
//    //        }
//    //    }
//        std::cout << "errors : " << error << std::endl;

//    //    int width  = 1000;
//    //    int height = 1000;
//    //    cv::Mat l_oM1(height,width, CV_32FC1), l_oM2(width,height, CV_32FC1);

//    //    for(int ii = 0; ii < width*height;++ii)
//    //    {
//    //        l_oM1.at<float>(ii) = rand()%100;
//    //        l_oM2.at<float>(ii) = rand()%100;
//    //    }


//    //    m_oProgramTime = clock();
//    //    cv::Mat l_oRes;
//    //    swUtil::swCuda::matrixMultiplication(l_oM1, l_oM2, l_oRes);
//    //    std::cout << "cuda : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << std::endl;

//    //    m_oProgramTime = clock();
//    //    cv::Mat l_oRes2 = l_oM1 * l_oM2;
//    //    std::cout << "opencv : " << (float)(clock() - m_oProgramTime) / CLOCKS_PER_SEC << std::endl;

//    //    std::cout << "Res :" << std::endl;
//    //    for(int ii = 0; ii < height*height;++ii)
//    //    {
//    //        if(rand()%10000 == 0)
//    //        {
//    //            if(l_oRes.at<float>(ii)*l_oRes.at<float>(ii) - l_oRes2.at<float>(ii)*l_oRes2.at<float>(ii) > 0.001f)
//    //            {
//    //                std::cerr << "Error computing : " << l_oRes.at<float>(ii) << " " << l_oRes2.at<float>(ii) << " " << ii << std::endl;
//    //                break;
//    //            }
//    //        }
//    //    }
//    }

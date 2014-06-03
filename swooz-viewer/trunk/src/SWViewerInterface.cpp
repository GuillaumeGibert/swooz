
/**
 * \file SWViewerInterface.cpp
 * \brief Defines SWViewerInterface
 * \author Florian Lance
 * \date 02/06/14
 */

#include "SWViewerInterface.h"

#include "moc_SWViewerInterface.cpp"

SWViewerInterface::SWViewerInterface() : m_uiViewer(new Ui::SWUI_Viewer)
{
    // init main widget
    m_uiViewer->setupUi(this);
    this->setWindowTitle(QString("SWoOz : Viewer"));

    // set default values
    // ...

    // parameters
        // spinbox
        // ...

        // checkbox
        // ...

    // middle container
        QHBoxLayout *l_pGLContainerLayout = new QHBoxLayout();
        QWidget *l_pGLContainer = new QWidget();
        QGLFormat l_glFormat;
        l_glFormat.setVersion( 4, 3 );
        l_glFormat.setProfile(  QGLFormat::CompatibilityProfile);
        l_glFormat.setSampleBuffers( true );
        QGLContext *l_glContext = new QGLContext(l_glFormat);
        m_pGLMesh = new SWGLMeshWidget(l_glContext, l_pGLContainer, "../data/shaders/meshViewer.vert", "../data/shaders/meshViewer.frag");
        l_pGLContainerLayout->addWidget(m_pGLMesh);
        l_pGLContainer->setLayout(l_pGLContainerLayout);
        m_uiViewer->glScene->addWidget(l_pGLContainer);


    // init worker
        //m_pWViewer = new SWViewerWorker(...);



    // init connections
         QObject::connect(m_uiViewer->pbLoadMesh, SIGNAL(clicked()), this, SLOT(loadMesh()));



}

SWViewerInterface::~SWViewerInterface()
{
}

void SWViewerInterface::loadMesh()
{
    QString l_sPathMesh = QFileDialog::getOpenFileName(this, "Load mesh", QString(), "Mesh file (*.obj)");;
    //m_uiViewer->lePathTemplate->setText(l_sPathTemplateMesh);
    //m_pGLOSNRICP->setSourceMesh(l_sPathTemplateMesh);

    swMesh::SWMesh l_oMesh(l_sPathMesh.toUtf8().constData());
    m_pGLMesh->setMesh(&l_oMesh);

//    m_bTemplateDefined = true;

//    if(m_bTemplateDefined && m_bTargetDefined)
//    {
//        m_uiMorphing->pbStart->setEnabled(true);
//        m_uiMorphing->pbSaveMorphedMesh->setEnabled(true);
//        m_uiMorphing->pbCorr->setEnabled(true);
//    }
}

int main(int argc, char* argv[])
{
    QApplication l_oApp(argc, argv);
    SWViewerInterface l_oViewerInterface;
    l_oViewerInterface.resize(1800, 900);
    l_oViewerInterface.move(50,50);
    l_oViewerInterface.show();

    return l_oApp.exec();
}

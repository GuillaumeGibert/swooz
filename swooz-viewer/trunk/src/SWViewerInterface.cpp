
/**
 * \file SWViewerInterface.cpp
 * \brief Defines SWViewerInterface
 * \author Florian Lance
 * \date 02/06/14
 */

#include "SWViewerInterface.h"

#include "moc_SWViewerInterface.cpp"

#include "interface/QtWidgets/SWGLMultiObjectWidget.h"

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
        m_pGLMultiObject = new SWGLMultiObjectWidget(l_glContext, l_pGLContainer);//, "../data/shaders/meshViewer.vert", "../data/shaders/meshViewer.frag");
        l_pGLContainerLayout->addWidget(m_pGLMultiObject);
        l_pGLContainer->setLayout(l_pGLContainerLayout);
        m_uiViewer->glScene->addWidget(l_pGLContainer);


    // init worker
        //m_pWViewer = new SWViewerWorker(...);



    // init connections
         QObject::connect(m_uiViewer->pbLoadCloud, SIGNAL(clicked()), this, SLOT(loadCloud()));
         QObject::connect(m_uiViewer->pbLoadMesh, SIGNAL(clicked()), this, SLOT(loadMesh()));



}

SWViewerInterface::~SWViewerInterface()
{
}

void SWViewerInterface::loadCloud()
{
    QString l_sPathCloud = QFileDialog::getOpenFileName(this, "Load cloud", QString(), "Mesh file (*.obj)");;
    m_pGLMultiObject->addCloud(l_sPathCloud);
}

void SWViewerInterface::loadMesh()
{
    QString l_sPathMesh = QFileDialog::getOpenFileName(this, "Load mesh", QString(), "Mesh file (*.obj)");;
    m_pGLMultiObject->addMesh(l_sPathMesh);
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

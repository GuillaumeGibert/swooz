
/**
 * \file SWViewerInterface.cpp
 * \brief Defines SWViewerInterface
 * \author Florian Lance
 * \date 02/06/14
 */

#include "SWViewerInterface.h"

#include "moc_SWViewerInterface.cpp"

#include "interface/QtWidgets/SWGLMultiObjectWidget.h"

#include <QCheckBox>

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
         QObject::connect(m_uiViewer->pbDeleteCloud, SIGNAL(clicked()), this, SLOT(deleteCloud()));
         QObject::connect(m_uiViewer->pbDeleteMesh, SIGNAL(clicked()), this, SLOT(deleteMesh()));

         QObject::connect(m_uiViewer->lwClouds, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(updateCloudInterfaceParameters(QListWidgetItem *)));
         QObject::connect(m_uiViewer->lwMeshes, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(updateMeshInterfaceParameters(QListWidgetItem *)));

        // update interface
            QObject::connect(m_uiViewer->dsbRX, SIGNAL(editingFinished()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->dsbRY, SIGNAL(editingFinished()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->dsbRZ, SIGNAL(editingFinished()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->dsbTrX, SIGNAL(editingFinished()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->dsbTrY, SIGNAL(editingFinished()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->dsbTrZ, SIGNAL(editingFinished()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->dsbScaling, SIGNAL(editingFinished()), this, SLOT(updateParameters()));

            QObject::connect(m_uiViewer->cbDisplayLines, SIGNAL(clicked()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->cbVisible, SIGNAL(clicked()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->rbDisplayOriginalColor, SIGNAL(clicked()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->rbDisplayTexture, SIGNAL(clicked()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->rbDisplayUnicolor, SIGNAL(clicked()), this, SLOT(updateParameters()));

            QObject::connect(m_uiViewer->sbColorB, SIGNAL(editingFinished()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->sbColorG, SIGNAL(editingFinished()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->sbColorR, SIGNAL(editingFinished()), this, SLOT(updateParameters()));
}

SWViewerInterface::~SWViewerInterface()
{}

void SWViewerInterface::loadCloud()
{
    // retrieve obj path
        QString l_sPathCloud = QFileDialog::getOpenFileName(this, "Load cloud", QString(), "Mesh file (*.obj)");

    // add list item and cloud
        m_uiViewer->lwClouds->addItem(l_sPathCloud);
        m_pGLMultiObject->addCloud(l_sPathCloud);

    // set the current row with the last added cloud
        m_uiViewer->lwClouds->setCurrentRow(m_uiViewer->lwClouds->count()-1);

    // unlock cloud delete button
        m_uiViewer->pbDeleteCloud->setEnabled(true);

    // update interface with new cloud parameters
        updateCloudInterfaceParameters(NULL);
}

void SWViewerInterface::loadMesh()
{
    // retrieve obj path
        QString l_sPathMesh = QFileDialog::getOpenFileName(this, "Load mesh", QString(), "Mesh file (*.obj)");

    // add list item and mesh
        m_uiViewer->lwMeshes->addItem(l_sPathMesh);
        m_pGLMultiObject->addMesh(l_sPathMesh);

    // set the current row with the last added mesh
        m_uiViewer->lwMeshes->setCurrentRow(m_uiViewer->lwMeshes->count()-1);

    // unlock mesh delete button
        m_uiViewer->pbDeleteMesh->setEnabled(true);

    // update interface with new mesh parameters
        updateMeshInterfaceParameters(NULL);
}


void SWViewerInterface::deleteCloud()
{
    // retrieve item index to delete
        int l_i32IndexCloud = m_uiViewer->lwClouds->currentRow();

    // delete cloud and item
        if(l_i32IndexCloud >= 0)
        {
            m_pGLMultiObject->removeCloud(l_i32IndexCloud);
            delete m_uiViewer->lwClouds->takeItem(l_i32IndexCloud);
        }

    // lock delete cloud button
        if(m_uiViewer->lwClouds->count() == 0)
        {
            m_uiViewer->pbDeleteCloud->setEnabled(false);
        }

    // lock
        m_uiViewer->rbDisplayTexture->setEnabled(false);
        m_uiViewer->pbSetTexture->setEnabled(false);
        m_uiViewer->cbDisplayLines->setEnabled(false);
        m_uiViewer->cbVisible->setEnabled(false);
        m_uiViewer->rbDisplayOriginalColor->setEnabled(false);
        m_uiViewer->rbDisplayUnicolor->setEnabled(false);
        m_uiViewer->dsbRX->setEnabled(false);
        m_uiViewer->dsbRY->setEnabled(false);
        m_uiViewer->dsbRZ->setEnabled(false);
        m_uiViewer->dsbTrX->setEnabled(false);
        m_uiViewer->dsbTrY->setEnabled(false);
        m_uiViewer->dsbTrZ->setEnabled(false);
        m_uiViewer->dsbScaling->setEnabled(false);
        m_uiViewer->sbColorR->setEnabled(false);
        m_uiViewer->sbColorG->setEnabled(false);
        m_uiViewer->sbColorB->setEnabled(false);

    // reset text
        m_uiViewer->leNameItem->setText(QString("..."));
        m_uiViewer->leNameItem->deselect();
}

void SWViewerInterface::deleteMesh()
{
    // retrieve item index to delete
        int l_i32IndexMesh = m_uiViewer->lwMeshes->currentRow();

    // delete mesh and item
        if(l_i32IndexMesh >= 0)
        {
            m_pGLMultiObject->removeMesh(l_i32IndexMesh);
            delete m_uiViewer->lwMeshes->takeItem(l_i32IndexMesh);
        }

    // lock delete cloud button
        if(m_uiViewer->lwMeshes->count() == 0)
        {
            m_uiViewer->pbDeleteMesh->setEnabled(false);
        }

    // lock
        m_uiViewer->rbDisplayTexture->setEnabled(false);
        m_uiViewer->pbSetTexture->setEnabled(false);
        m_uiViewer->cbDisplayLines->setEnabled(false);
        m_uiViewer->cbVisible->setEnabled(false);
        m_uiViewer->rbDisplayOriginalColor->setEnabled(false);
        m_uiViewer->rbDisplayUnicolor->setEnabled(false);
        m_uiViewer->dsbRX->setEnabled(false);
        m_uiViewer->dsbRY->setEnabled(false);
        m_uiViewer->dsbRZ->setEnabled(false);
        m_uiViewer->dsbTrX->setEnabled(false);
        m_uiViewer->dsbTrY->setEnabled(false);
        m_uiViewer->dsbTrZ->setEnabled(false);
        m_uiViewer->dsbScaling->setEnabled(false);
        m_uiViewer->sbColorR->setEnabled(false);
        m_uiViewer->sbColorG->setEnabled(false);
        m_uiViewer->sbColorB->setEnabled(false);

    // reset text
        m_uiViewer->leNameItem->setText(QString("..."));
        m_uiViewer->leNameItem->deselect();
}

void SWViewerInterface::updateParameters()
{

    SWGLObjectParameters l_oParams;
    l_oParams.m_bVisible       = (m_uiViewer->cbVisible->checkState() == Qt::Checked);
    l_oParams.m_bDisplayLines  = (m_uiViewer->cbDisplayLines->checkState() == Qt::Checked);
    l_oParams.m_vTranslation   = QVector3D(m_uiViewer->dsbTrX->value(), m_uiViewer->dsbTrY->value(), m_uiViewer->dsbTrZ->value());
    l_oParams.m_vRotation      = QVector3D(m_uiViewer->dsbRX->value(), m_uiViewer->dsbRY->value(), m_uiViewer->dsbRZ->value());
    l_oParams.m_dScaling       = m_uiViewer->dsbScaling->value();
    l_oParams.m_vUnicolor      = QVector3D(m_uiViewer->sbColorR->value(), m_uiViewer->sbColorG->value(), m_uiViewer->sbColorB->value());

    GLObjectDisplayMode l_oDisplayMode;
    if(m_uiViewer->rbDisplayOriginalColor->isChecked())
    {
        l_oDisplayMode = GLO_ORIGINAL_COLOR;
    }
    else if(m_uiViewer->rbDisplayUnicolor->isChecked())
    {
        l_oDisplayMode = GLO_UNI_COLOR;
    }
    else
    {
        l_oDisplayMode = GLO_TEXTURE;
    }

    l_oParams.displayMode = l_oDisplayMode;

    int l_i32Index;

    if(m_bIsCloudLastSelection)
    {
        l_i32Index = m_uiViewer->lwClouds->currentRow();

        if(l_i32Index >= 0)
        {
            m_pGLMultiObject->setCloudParameters(static_cast<uint>(l_i32Index), l_oParams);
        }
    }
    else
    {
        l_i32Index = m_uiViewer->lwMeshes->currentRow();

        if(l_i32Index >= 0)
        {
            m_pGLMultiObject->setMeshParameters(static_cast<uint>(l_i32Index), l_oParams);
        }
    }
}

void SWViewerInterface::updateCloudInterfaceParameters(QListWidgetItem *)
{
    // set the type of the last selected item
        m_bIsCloudLastSelection = true;

    // update current item name
        m_uiViewer->leNameItem->setText(QString("Cloud : ") + m_uiViewer->lwClouds->currentItem()->text());

    // lock
        m_uiViewer->rbDisplayTexture->setEnabled(false);
        m_uiViewer->pbSetTexture->setEnabled(false);

    // unlock
        m_uiViewer->cbDisplayLines->setEnabled(false);
        m_uiViewer->cbVisible->setEnabled(true);
        m_uiViewer->rbDisplayOriginalColor->setEnabled(true);
        m_uiViewer->rbDisplayUnicolor->setEnabled(true);
        m_uiViewer->dsbRX->setEnabled(true);
        m_uiViewer->dsbRY->setEnabled(true);
        m_uiViewer->dsbRZ->setEnabled(true);
        m_uiViewer->dsbTrX->setEnabled(true);
        m_uiViewer->dsbTrY->setEnabled(true);
        m_uiViewer->dsbTrZ->setEnabled(true);
        m_uiViewer->dsbScaling->setEnabled(true);
        m_uiViewer->sbColorR->setEnabled(true);
        m_uiViewer->sbColorG->setEnabled(true);
        m_uiViewer->sbColorB->setEnabled(true);

    updateInterfaceParameters();
}

void SWViewerInterface::updateMeshInterfaceParameters(QListWidgetItem *)
{
    // set the type of the last selected item
        m_bIsCloudLastSelection = false;

    // update current item name
        m_uiViewer->leNameItem->setText(QString("Mesh : ") + m_uiViewer->lwMeshes->currentItem()->text());

    // unlock
        m_uiViewer->rbDisplayTexture->setEnabled(true);
        m_uiViewer->pbSetTexture->setEnabled(true);
        m_uiViewer->cbDisplayLines->setEnabled(true);
        m_uiViewer->cbVisible->setEnabled(true);
        m_uiViewer->rbDisplayOriginalColor->setEnabled(true);
        m_uiViewer->rbDisplayUnicolor->setEnabled(true);
        m_uiViewer->dsbRX->setEnabled(true);
        m_uiViewer->dsbRY->setEnabled(true);
        m_uiViewer->dsbRZ->setEnabled(true);
        m_uiViewer->dsbTrX->setEnabled(true);
        m_uiViewer->dsbTrY->setEnabled(true);
        m_uiViewer->dsbTrZ->setEnabled(true);
        m_uiViewer->dsbScaling->setEnabled(true);
        m_uiViewer->sbColorR->setEnabled(true);
        m_uiViewer->sbColorG->setEnabled(true);
        m_uiViewer->sbColorB->setEnabled(true);

    updateInterfaceParameters();
}

void SWViewerInterface::updateInterfaceParameters()
{
    SWGLObjectParameters l_oParams;
    int l_i32Index;

    if(m_bIsCloudLastSelection)
    {
        l_i32Index = m_uiViewer->lwClouds->currentRow();

        if(l_i32Index >= 0)
        {
            m_pGLMultiObject->cloudParameters(l_i32Index, l_oParams);
        }
        else // no clouds found
        {
            return;
        }
    }
    else
    {
        l_i32Index = m_uiViewer->lwMeshes->currentRow();

        if(l_i32Index >= 0)
        {
            m_pGLMultiObject->meshParameters(l_i32Index, l_oParams);
        }
        else // no meshes found
        {
            return;
        }
    }


    qDebug() << "updateInterfaceParameters " << l_i32Index << " " << m_bIsCloudLastSelection;



    if(l_oParams.displayMode == GLO_UNI_COLOR)
    {
        m_uiViewer->rbDisplayUnicolor->setChecked(true);
    }
    else if(l_oParams.displayMode == GLO_ORIGINAL_COLOR)
    {
        m_uiViewer->rbDisplayOriginalColor->setChecked(true);
    }
    else
    {
        m_uiViewer->rbDisplayTexture->setChecked(true);
    }

    m_uiViewer->cbVisible->setChecked(l_oParams.m_bVisible);
    m_uiViewer->cbDisplayLines->setChecked(l_oParams.m_bDisplayLines);

    m_uiViewer->dsbTrX->setValue(l_oParams.m_vTranslation.x());
    m_uiViewer->dsbTrY->setValue(l_oParams.m_vTranslation.y());
    m_uiViewer->dsbTrZ->setValue(l_oParams.m_vTranslation.z());

    m_uiViewer->dsbRX->setValue(l_oParams.m_vRotation.x());
    m_uiViewer->dsbRY->setValue(l_oParams.m_vRotation.y());
    m_uiViewer->dsbRZ->setValue(l_oParams.m_vRotation.z());

    m_uiViewer->dsbScaling->setValue(l_oParams.m_dScaling);

    m_uiViewer->sbColorR->setValue(l_oParams.m_vUnicolor.x());
    m_uiViewer->sbColorG->setValue(l_oParams.m_vUnicolor.y());
    m_uiViewer->sbColorB->setValue(l_oParams.m_vUnicolor.z());
}

//void SWViewerInterface::timerEvent(QTimerEvent *e)
//{
//    updateParameters();
//}

int main(int argc, char* argv[])
{
    QApplication l_oApp(argc, argv);
    SWViewerInterface l_oViewerInterface;
    l_oViewerInterface.resize(1800, 900);
    l_oViewerInterface.move(50,50);
    l_oViewerInterface.show();

    return l_oApp.exec();
}

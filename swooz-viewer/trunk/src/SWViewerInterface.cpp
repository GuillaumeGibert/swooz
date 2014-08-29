
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
#include <time.h>


SWViewerInterface::SWViewerInterface() : m_uiViewer(new Ui::SWUI_Viewer), m_bDesactiveUpdateParameters(false), m_bGLFullScreen(false)
{
    // init main widget
    m_uiViewer->setupUi(this);
    this->setWindowTitle(QString("SWoOz : Viewer"));

    // middle container
        QHBoxLayout *l_pGLContainerLayout = new QHBoxLayout();
        m_pGLContainer = new QWidget();
        QGLFormat l_glFormat;
        l_glFormat.setVersion( 4, 3 );
        l_glFormat.setProfile(  QGLFormat::CompatibilityProfile);
        l_glFormat.setSampleBuffers( true );
        QGLContext *l_glContext = new QGLContext(l_glFormat);
        m_pGLMultiObject = new SWGLMultiObjectWidget(l_glContext, m_pGLContainer);
        l_pGLContainerLayout->addWidget(m_pGLMultiObject);
        l_pGLContainerLayout->layout()->setContentsMargins(0,0,0,0);
        m_pGLContainer->setLayout(l_pGLContainerLayout);
        m_uiViewer->glScene->addWidget(m_pGLContainer);

    // init worker
        m_pWViewer = new SWViewerWorker();

    // init connections
        QObject::connect(m_uiViewer->pbLoadCloud, SIGNAL(clicked()), this, SLOT(loadCloud()));
        QObject::connect(m_uiViewer->pbLoadMesh, SIGNAL(clicked()), this, SLOT(loadMesh()));
        QObject::connect(m_uiViewer->pbDeleteCloud, SIGNAL(clicked()), this, SLOT(deleteCloud()));
        QObject::connect(m_uiViewer->pbDeleteMesh, SIGNAL(clicked()), this, SLOT(deleteMesh()));
        QObject::connect(m_uiViewer->pbSetTexture, SIGNAL(clicked()), this, SLOT(setTexture()));

        QObject::connect(m_uiViewer->lwClouds, SIGNAL(currentRowChanged(int)), this, SLOT(updateCloudInterfaceParameters()));
        QObject::connect(m_uiViewer->lwMeshes, SIGNAL(currentRowChanged(int)), this, SLOT(updateMeshInterfaceParameters()));
        QObject::connect(m_uiViewer->lwClouds, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateCloudInterfaceParameters(QListWidgetItem*)));
        QObject::connect(m_uiViewer->lwMeshes, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateMeshInterfaceParameters(QListWidgetItem*)));

        QObject::connect(m_uiViewer->lwClouds, SIGNAL(currentRowChanged(int)), m_pWViewer, SLOT(updateCloudAnimationPath(int)));
        QObject::connect(m_uiViewer->lwMeshes, SIGNAL(currentRowChanged(int)), m_pWViewer, SLOT(updateMeshAnimationPath(int)));
        QObject::connect(this, SIGNAL(cloudCurrentRowChanged(int)), m_pWViewer, SLOT(updateCloudAnimationPath(int)));
        QObject::connect(this, SIGNAL(meshCurrentRowChanged(int)), m_pWViewer, SLOT(updateMeshAnimationPath(int)));
        QObject::connect(m_uiViewer->lwClouds, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateCloudAnimationPath(QListWidgetItem*)));
        QObject::connect(m_uiViewer->lwMeshes, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateMeshAnimationPath(QListWidgetItem*)));


        // update interface
            QObject::connect(m_uiViewer->dsbRX, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbRY, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbRZ, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbTrX, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbTrY, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbTrZ, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbScaling, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));

            QObject::connect(m_uiViewer->cbDisplayLines, SIGNAL(clicked()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->cbVisible, SIGNAL(clicked()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->rbDisplayOriginalColor, SIGNAL(clicked()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->rbDisplayTexture, SIGNAL(clicked()), this, SLOT(updateParameters()));
            QObject::connect(m_uiViewer->rbDisplayUnicolor, SIGNAL(clicked()), this, SLOT(updateParameters()));

            QObject::connect(m_uiViewer->sbColorB, SIGNAL(valueChanged(int)), this, SLOT(updateParameters(int)));
            QObject::connect(m_uiViewer->sbColorG, SIGNAL(valueChanged(int)), this, SLOT(updateParameters(int)));
            QObject::connect(m_uiViewer->sbColorR, SIGNAL(valueChanged(int)), this, SLOT(updateParameters(int)));

            QObject::connect(m_uiViewer->dsbLightX, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbLightY, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbLightZ, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));

            QObject::connect(m_uiViewer->leTexturePath, SIGNAL(textChanged(QString)), this, SLOT(updateParameters(QString)));

            QObject::connect(m_uiViewer->dsbAmbiantLight1, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbAmbiantLight2, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbAmbiantLight3, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbDiffusLight1, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbDiffusLight2, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbDiffusLight3, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbSpecularLight1, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbSpecularLight2, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbSpecularLight3, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbAmbiantK, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbDiffusK, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbSpecularK, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
            QObject::connect(m_uiViewer->dsbSpecularP, SIGNAL(valueChanged(double)), this, SLOT(updateParameters(double)));
        // push buttons
            QObject::connect(m_uiViewer->pbSetCamera, SIGNAL(clicked()), this, SLOT(setCameraToCurrentItem()));
            QObject::connect(m_uiViewer->pbResetCamera, SIGNAL(clicked()), m_pGLMultiObject, SLOT(resetCamera()));
            QObject::connect(m_uiViewer->pbLaunchAllAnim, SIGNAL(clicked()), m_pWViewer, SLOT(startLoop()));
            QObject::connect(m_uiViewer->pbSetModFile, SIGNAL(clicked()), this, SLOT(loadModFile()));
            QObject::connect(m_uiViewer->pbSetSeqFile, SIGNAL(clicked()), this, SLOT(loadSeqFile()));
            QObject::connect(m_uiViewer->pbSetMeshCorr, SIGNAL(clicked()), this, SLOT(loadMeshCorrFile()));

        // fullscreen
            QObject::connect(m_pGLMultiObject, SIGNAL(enableFullScreen()), this, SLOT(enableGLFullScreen()));                        
            QObject::connect(m_pGLMultiObject, SIGNAL(disableFullScreen()), this, SLOT(disableGLFullScreen()));

        // worker
            QObject::connect(this,  SIGNAL(stopLoop()), m_pWViewer, SLOT(stopLoop()));
            QObject::connect(this, SIGNAL(setModFilePath(bool,int,QString)), m_pWViewer, SLOT(setModFile(bool,int,QString)));
            QObject::connect(this, SIGNAL(setSeqFilePath(bool,int,QString)), m_pWViewer, SLOT(setSeqFile(bool,int,QString)));
            QObject::connect(this, SIGNAL(setCorrFilePath(bool,int,QString)), m_pWViewer, SLOT(setCorrFilePath(bool,int,QString)));
            QObject::connect(m_pWViewer, SIGNAL(sendAnimationPathFile(QString,QString,QString)), this, SLOT(updateAnimationPathFileDisplay(QString,QString,QString)));
            QObject::connect(this, SIGNAL(deleteAnimation(bool,int)), m_pWViewer, SLOT(deleteAnimation(bool,int)));
            QObject::connect(this, SIGNAL(addAnimation(bool)), m_pWViewer, SLOT(addAnimation(bool)));
            QObject::connect(m_pWViewer, SIGNAL(sendOffsetAnimation(SWAnimationSendDataPtr)),m_pGLMultiObject, SLOT(setAnimationOffset(SWAnimationSendDataPtr)),Qt::DirectConnection);
//            QObject::connect(m_pWViewer, SIGNAL(sendOffsetAnimation(SWAnimationSendDataPtr)),m_pGLMultiObject, SLOT(setAnimationOffset(SWAnimationSendDataPtr)));
            QObject::connect(m_pWViewer, SIGNAL(startAnimation(bool,int)), m_pGLMultiObject, SLOT(beginAnimation(bool,int)));

            QObject::connect(m_pWViewer, SIGNAL(drawSceneSignal()), m_pGLMultiObject, SLOT(updateGL()));

    // init thread
        m_pWViewer->moveToThread(&m_TViewer);
        m_TViewer.start();
}


SWViewerInterface::~SWViewerInterface()
{
    m_TViewer.quit();
    m_TViewer.wait();

    deleteAndNullify(m_pWViewer);
}


void SWViewerInterface::closeEvent(QCloseEvent *event)
{
    emit stopLoop();

    QTime l_oDieTime = QTime::currentTime().addMSecs(200);
    while( QTime::currentTime() < l_oDieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}


void SWViewerInterface::loadModFile()
{
    // retrieve obj path
        QString l_sPathMod = QFileDialog::getOpenFileName(this, "Load mod file", QString(), "Mod file (*.mod)");
        if(l_sPathMod == "")
        {
            return;
        }

    // retrieve item index to delete
        int l_i32Index;

        if(m_bIsCloudLastSelection)
        {
            l_i32Index = m_uiViewer->lwClouds->currentRow();
        }
        else
        {
            l_i32Index = m_uiViewer->lwMeshes->currentRow();
        }

        if(l_i32Index != -1)
        {
            emit setModFilePath(m_bIsCloudLastSelection, l_i32Index, l_sPathMod);
        }
        else
        {
            std::cerr << "Load an object before loading a mod file. " << std::endl;
        }
}

void SWViewerInterface::loadSeqFile()
{
    // retrieve obj path
        QString l_sPathSeq = QFileDialog::getOpenFileName(this, "Load seq file", QString(), "Seq file (*.seq)");
        if(l_sPathSeq == "")
        {
            return;
        }

        int l_i32Index;

        if(m_bIsCloudLastSelection)
        {
            l_i32Index = m_uiViewer->lwClouds->currentRow();
        }
        else
        {
            l_i32Index = m_uiViewer->lwMeshes->currentRow();
        }

        if(l_i32Index != -1)
        {
            emit setSeqFilePath(m_bIsCloudLastSelection, l_i32Index, l_sPathSeq);
        }
        else
        {
            std::cerr << "Load an object before loading a seq file. " << std::endl;
        }
}

void SWViewerInterface::loadMeshCorrFile()
{
    // retrieve obj path
        QString l_sPathObjCorr = QFileDialog::getOpenFileName(this, "Load corr file", QString(), "Seq file (*.obj)");
        if(l_sPathObjCorr == "")
        {
            return;
        }

    // retrieve item index to delete
        int l_i32Index;

        if(m_bIsCloudLastSelection)
        {
            l_i32Index = m_uiViewer->lwClouds->currentRow();
        }
        else
        {
            l_i32Index = m_uiViewer->lwMeshes->currentRow();
        }

        if(l_i32Index != -1)
        {
            emit setCorrFilePath(m_bIsCloudLastSelection, l_i32Index, l_sPathObjCorr);
        }
        else
        {
            std::cerr << "Load an object before loading a seq file. " << std::endl;
        }
}

void SWViewerInterface::updateAnimationPathFileDisplay(QString modFilePath, QString seqFilePath, QString corrFilePath)
{
    m_uiViewer->leMod->setText(modFilePath);
    m_uiViewer->leSeq->setText(seqFilePath);
    m_uiViewer->leMeshCorr->setText(corrFilePath);
}

void SWViewerInterface::loadCloud()
{
    // retrieve obj path
        QString l_sPathCloud = QFileDialog::getOpenFileName(this, "Load cloud", QString(), "Mesh file (*.obj)");

        if(l_sPathCloud == "")
        {
            return;
        }

    // add list item and cloud
        emit addAnimation(true);
        m_uiViewer->lwClouds->addItem(l_sPathCloud);
        m_pGLMultiObject->addCloud(l_sPathCloud);        

    // set the current row with the last added cloud
        m_uiViewer->lwClouds->setCurrentRow(m_uiViewer->lwClouds->count()-1);

    // unlock cloud delete button
        m_uiViewer->pbDeleteCloud->setEnabled(true);

    // update interface with new cloud parameters
        updateCloudInterfaceParameters();
}

void SWViewerInterface::loadMesh()
{
    // retrieve obj path
        QString l_sPathMesh = QFileDialog::getOpenFileName(this, "Load mesh", QString(), "Mesh file (*.obj)");

        if(l_sPathMesh == "")
        {
            return;
        }

    // add list item and mesh
        emit addAnimation(false);
        m_uiViewer->lwMeshes->addItem(l_sPathMesh);
        m_pGLMultiObject->addMesh(l_sPathMesh);

    // set the current row with the last added mesh
        m_uiViewer->lwMeshes->setCurrentRow(m_uiViewer->lwMeshes->count()-1);

    // unlock mesh delete button
        m_uiViewer->pbDeleteMesh->setEnabled(true);

    // update interface with new mesh parameters
        updateMeshInterfaceParameters();
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

            emit deleteAnimation(true, l_i32IndexCloud);
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
        m_uiViewer->dsbLightX->setEnabled(false);
        m_uiViewer->dsbLightY->setEnabled(false);
        m_uiViewer->dsbLightZ->setEnabled(false);

    // reset text
        m_uiViewer->leNameItem->setText(QString("..."));
        m_uiViewer->lePtNumber->setText(QString("..."));
        m_uiViewer->leTriNumber->setText(QString("..."));
        m_uiViewer->leNameItem->deselect();

    // update interface with new cloud parameters
        updateCloudInterfaceParameters();
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

            emit deleteAnimation(false, l_i32IndexMesh);
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
        m_uiViewer->dsbLightX->setEnabled(false);
        m_uiViewer->dsbLightY->setEnabled(false);
        m_uiViewer->dsbLightZ->setEnabled(false);

    // reset text
        m_uiViewer->leNameItem->setText(QString("..."));
        m_uiViewer->lePtNumber->setText(QString("..."));
        m_uiViewer->leTriNumber->setText(QString("..."));
        m_uiViewer->leNameItem->deselect();

    // update interface with new mesh parameters
        updateMeshInterfaceParameters();
}

void SWViewerInterface::updateParameters(int i32Inused)
{
    updateParameters();
}

void SWViewerInterface::updateParameters(double dInused)
{
    updateParameters();
}

void SWViewerInterface::updateParameters(QString sInused)
{
    updateParameters();
}

void SWViewerInterface::updateParameters()
{
    if(m_bDesactiveUpdateParameters)
    {
        return;
    }

    SWGLObjectParameters l_oParams;
    l_oParams.m_bVisible       = (m_uiViewer->cbVisible->checkState() == Qt::Checked);
    l_oParams.m_bDisplayLines  = (m_uiViewer->cbDisplayLines->checkState() == Qt::Checked);
    l_oParams.m_vTranslation   = QVector3D(m_uiViewer->dsbTrX->value(), m_uiViewer->dsbTrY->value(), m_uiViewer->dsbTrZ->value());
    l_oParams.m_vRotation      = QVector3D(m_uiViewer->dsbRX->value(), m_uiViewer->dsbRY->value(), m_uiViewer->dsbRZ->value());
    l_oParams.m_dScaling       = m_uiViewer->dsbScaling->value();
    l_oParams.m_vUnicolor      = QVector3D(m_uiViewer->sbColorR->value(), m_uiViewer->sbColorG->value(), m_uiViewer->sbColorB->value());
    l_oParams.m_sTexturePath   = QString(m_uiViewer->leTexturePath->text());

    // lights
        l_oParams.m_vSourceLight   = QVector3D(m_uiViewer->dsbLightX->value(),m_uiViewer->dsbLightY->value(),m_uiViewer->dsbLightZ->value());
        l_oParams.m_vAmbiantLight  = QVector3D(m_uiViewer->dsbAmbiantLight1->value(),m_uiViewer->dsbAmbiantLight2->value(),m_uiViewer->dsbAmbiantLight3->value());
        l_oParams.m_vDiffusLight   = QVector3D(m_uiViewer->dsbDiffusLight1->value(),m_uiViewer->dsbDiffusLight2->value(),m_uiViewer->dsbDiffusLight3->value());
        l_oParams.m_vSpecularLight = QVector3D(m_uiViewer->dsbSpecularLight1->value(),m_uiViewer->dsbSpecularLight2->value(),m_uiViewer->dsbSpecularLight3->value());
        l_oParams.m_dAmbiantK = m_uiViewer->dsbAmbiantK->value();
        l_oParams.m_dDiffusK = m_uiViewer->dsbDiffusK->value();
        l_oParams.m_dSpecularK = m_uiViewer->dsbSpecularK->value();
        l_oParams.m_dSpecularP = m_uiViewer->dsbSpecularP->value();

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
    updateCloudInterfaceParameters();
}

void SWViewerInterface::updateMeshInterfaceParameters(QListWidgetItem *)
{
    updateMeshInterfaceParameters();
}

void SWViewerInterface::updateCloudAnimationPath(QListWidgetItem *)
{
    int index = m_uiViewer->lwClouds->currentRow();
    emit cloudCurrentRowChanged(index);
}

void SWViewerInterface::updateMeshAnimationPath(QListWidgetItem *)
{
    int index = m_uiViewer->lwMeshes->currentRow();
    emit meshCurrentRowChanged(index);
}

void SWViewerInterface::updateCloudInterfaceParameters()
{
    if(m_uiViewer->lwClouds->currentRow() == -1 || m_uiViewer->lwClouds->currentRow() >= m_pGLMultiObject->m_cloudsInfos.size())
    {
        return;
    }

    // set the type of the last selected item
        m_bIsCloudLastSelection = true;

    // update current item infos
        QString l_sName = m_uiViewer->lwClouds->currentItem()->text();
        m_uiViewer->leNameItem->setText(QString("Cloud : ") + l_sName.right(l_sName.size() -  l_sName.lastIndexOf('/')-1));
        m_uiViewer->lePtNumber->setText(QString::number(m_pGLMultiObject->m_cloudsInfos[m_uiViewer->lwClouds->currentRow()].m_points));
        m_uiViewer->leTriNumber->setText(QString::number(m_pGLMultiObject->m_cloudsInfos[m_uiViewer->lwClouds->currentRow()].m_triangles));

    // lock
        m_uiViewer->rbDisplayTexture->setEnabled(false);
        m_uiViewer->pbSetTexture->setEnabled(false);
        m_uiViewer->leTexturePath->setEnabled(false);
        m_uiViewer->dsbLightX->setEnabled(false);
        m_uiViewer->dsbLightY->setEnabled(false);
        m_uiViewer->dsbLightZ->setEnabled(false);        
        m_uiViewer->dsbAmbiantLight1->setEnabled(false);
        m_uiViewer->dsbAmbiantLight2->setEnabled(false);
        m_uiViewer->dsbAmbiantLight3->setEnabled(false);
        m_uiViewer->dsbDiffusLight1->setEnabled(false);
        m_uiViewer->dsbDiffusLight2->setEnabled(false);
        m_uiViewer->dsbDiffusLight3->setEnabled(false);
        m_uiViewer->dsbSpecularLight1->setEnabled(false);
        m_uiViewer->dsbSpecularLight2->setEnabled(false);
        m_uiViewer->dsbSpecularLight3->setEnabled(false);
        m_uiViewer->dsbAmbiantK->setEnabled(false);
        m_uiViewer->dsbDiffusK->setEnabled(false);
        m_uiViewer->dsbSpecularK->setEnabled(false);
        m_uiViewer->dsbSpecularP->setEnabled(false);

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

void SWViewerInterface::updateMeshInterfaceParameters()
{
    if(m_uiViewer->lwMeshes->currentRow() == -1 || m_uiViewer->lwMeshes->currentRow() >= m_pGLMultiObject->m_meshesInfos.size())
    {
        return;
    }
    // set the type of the last selected item
        m_bIsCloudLastSelection = false;

    // update current item infos
        QString l_sName = m_uiViewer->lwMeshes->currentItem()->text();
        m_uiViewer->leNameItem->setText(QString("Mesh : ") + l_sName.right(l_sName.size() - l_sName.lastIndexOf('/')-1));
        m_uiViewer->lePtNumber->setText(QString::number(m_pGLMultiObject->m_meshesInfos[m_uiViewer->lwMeshes->currentRow()].m_points));
        m_uiViewer->leTriNumber->setText(QString::number(m_pGLMultiObject->m_meshesInfos[m_uiViewer->lwMeshes->currentRow()].m_triangles));

    // unlock
        m_uiViewer->rbDisplayTexture->setEnabled(true);
        m_uiViewer->pbSetTexture->setEnabled(true);
        m_uiViewer->leTexturePath->setEnabled(true);
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
        m_uiViewer->dsbLightX->setEnabled(true);
        m_uiViewer->dsbLightY->setEnabled(true);
        m_uiViewer->dsbLightZ->setEnabled(true);
        m_uiViewer->dsbAmbiantLight1->setEnabled(true);
        m_uiViewer->dsbAmbiantLight2->setEnabled(true);
        m_uiViewer->dsbAmbiantLight3->setEnabled(true);
        m_uiViewer->dsbDiffusLight1->setEnabled(true);
        m_uiViewer->dsbDiffusLight2->setEnabled(true);
        m_uiViewer->dsbDiffusLight3->setEnabled(true);
        m_uiViewer->dsbSpecularLight1->setEnabled(true);
        m_uiViewer->dsbSpecularLight2->setEnabled(true);
        m_uiViewer->dsbSpecularLight3->setEnabled(true);
        m_uiViewer->dsbAmbiantK->setEnabled(true);
        m_uiViewer->dsbDiffusK->setEnabled(true);
        m_uiViewer->dsbSpecularK->setEnabled(true);
        m_uiViewer->dsbSpecularP->setEnabled(true);

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

    m_bDesactiveUpdateParameters = true;
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

        m_uiViewer->leTexturePath->setText(l_oParams.m_sTexturePath);

        m_uiViewer->dsbLightX->setValue(l_oParams.m_vSourceLight.x());
        m_uiViewer->dsbLightY->setValue(l_oParams.m_vSourceLight.y());
        m_uiViewer->dsbLightZ->setValue(l_oParams.m_vSourceLight.z());

        m_uiViewer->dsbAmbiantLight1->setValue(l_oParams.m_vAmbiantLight.x());
        m_uiViewer->dsbAmbiantLight2->setValue(l_oParams.m_vAmbiantLight.y());
        m_uiViewer->dsbAmbiantLight3->setValue(l_oParams.m_vAmbiantLight.z());
        m_uiViewer->dsbDiffusLight1->setValue(l_oParams.m_vDiffusLight.x());
        m_uiViewer->dsbDiffusLight2->setValue(l_oParams.m_vDiffusLight.y());
        m_uiViewer->dsbDiffusLight3->setValue(l_oParams.m_vDiffusLight.z());
        m_uiViewer->dsbSpecularLight1->setValue(l_oParams.m_vSpecularLight.x());
        m_uiViewer->dsbSpecularLight2->setValue(l_oParams.m_vSpecularLight.y());
        m_uiViewer->dsbSpecularLight3->setValue(l_oParams.m_vSpecularLight.z());
        m_uiViewer->dsbAmbiantK->setValue(l_oParams.m_dAmbiantK);
        m_uiViewer->dsbDiffusK->setValue(l_oParams.m_dDiffusK);
        m_uiViewer->dsbSpecularK->setValue(l_oParams.m_dSpecularK);
        m_uiViewer->dsbSpecularP->setValue(l_oParams.m_dSpecularP);
    m_bDesactiveUpdateParameters = false;
}

void SWViewerInterface::setTexture()
{
    // retrieve obj path
        QString l_sPathTexture = QFileDialog::getOpenFileName(this, "Load texture", QString(), "Texture file (*.png)");
        m_uiViewer->leTexturePath->setText(l_sPathTexture);
}

void SWViewerInterface::setCameraToCurrentItem()
{
    if(m_bIsCloudLastSelection)
    {
        m_pGLMultiObject->setCameraItem(true, m_uiViewer->lwClouds->currentRow());
    }
    else
    {
        m_pGLMultiObject->setCameraItem(false, m_uiViewer->lwMeshes->currentRow());
    }
}

void SWViewerInterface::enableGLFullScreen()
{
    if(!m_bGLFullScreen)
    {
        m_pGLContainer->setParent(0);
        m_pGLContainer->showFullScreen();
        m_bGLFullScreen = true;
    }
}

void SWViewerInterface::disableGLFullScreen()
{
    if(m_bGLFullScreen)
    {
        m_uiViewer->glScene->addWidget(m_pGLContainer);
        m_bGLFullScreen = false;
    }
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

SWViewerWorker::SWViewerWorker() : m_i32LoopPeriod(1000/60)
{
    m_bDoLoop = true;

    qRegisterMetaType<swCloud::SWCloud*>("SWCloud");
    qRegisterMetaType<QVector<float> >("QVector<float>");
    qRegisterMetaType<SWAnimationSendDataPtr>("SWAnimationSendDataPtr");
}



void SWViewerWorker::startLoop()
{
    bool l_bDoLoop;
    m_oLoopMutex.lockForRead();
        l_bDoLoop = m_bDoLoop;
    m_oLoopMutex.unlock();

    QTime l_oStartTime;
    l_oStartTime.start();

    int l_numLine = 0;

    QVector<bool> l_currentAnimCloud(m_vCloudsAnimation.size(),true);
    QVector<bool> l_currentAnimMesh(m_vMeshesAnimation.size(),true);

    clock_t l_oProgramTime = clock();

    while(l_bDoLoop)
    {
         //control the time of the loop
            while(l_oStartTime.elapsed() < m_i32LoopPeriod)
            {
                QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, m_i32LoopPeriod - l_oStartTime.elapsed());
            }
            l_oStartTime.restart();


            bool l_cloudAnimStillRunning = false;
            for(int ii = 0; ii < l_currentAnimCloud.size(); ++ii)
            {
                if(l_currentAnimCloud[ii] == true)
                {
                    l_cloudAnimStillRunning = true;
                    break;
                }
            }
            bool l_meshAnimStillRunning = false;
            for(int ii = 0; ii < l_currentAnimMesh.size(); ++ii)
            {
                if(l_currentAnimMesh[ii] == true)
                {
                    l_meshAnimStillRunning = true;
                    break;
                }
            }

            if(!l_cloudAnimStillRunning && !l_meshAnimStillRunning)
            {
                l_bDoLoop = false;
                break;
            }

        // ...
            for(int ii = 0; ii < m_vCloudsAnimation.size(); ++ii)
            {
                if(!l_currentAnimCloud[ii])
                {
                    continue;
                }

                SWAnimationSendDataPtr dataToSend = SWAnimationSendDataPtr(new SWAnimationSendData());
                dataToSend->m_index = ii;
                dataToSend->m_isCloud = true;

                if(!m_vCloudsAnimation[ii].retrieveTransfosToApply(l_numLine, dataToSend->m_animationOffsetsX,
                                    dataToSend->m_animationOffsetsY,dataToSend->m_animationOffsetsZ,dataToSend->m_animationRigidMotion))
                {
                    l_currentAnimCloud[ii] = false;
                }
                else
                {
                    if(l_numLine == 0)
                    {
                        emit startAnimation(true, ii);
                    }
                    emit sendOffsetAnimation(dataToSend);
                }                
            }
            for(int ii = 0; ii < m_vMeshesAnimation.size(); ++ii)
            {
                if(!l_currentAnimMesh[ii])
                {
                    continue;
                }

                SWAnimationSendDataPtr dataToSend = SWAnimationSendDataPtr(new SWAnimationSendData());
                dataToSend->m_index = ii;
                dataToSend->m_isCloud = false;

                if(!m_vMeshesAnimation[ii].retrieveTransfosToApply(l_numLine, dataToSend->m_animationOffsetsX,
                                    dataToSend->m_animationOffsetsY,dataToSend->m_animationOffsetsZ,dataToSend->m_animationRigidMotion))
                {
                    l_currentAnimMesh[ii] = false;
                }
                else
                {
                    if(l_numLine == 0)
                    {
                        emit startAnimation(false, ii);
                    }

                    emit sendOffsetAnimation(dataToSend);
                }
            }

        // check end of the loop
            m_oLoopMutex.lockForRead();
                l_bDoLoop = m_bDoLoop;
            m_oLoopMutex.unlock();

            ++l_numLine;

            emit drawSceneSignal();
    }
}

void SWViewerWorker::stopLoop()
{
    m_oLoopMutex.lockForWrite();
        m_bDoLoop = false;
    m_oLoopMutex.unlock();
}



void SWViewerWorker::updateCloudAnimationPath(int indexCloud)
{
    if(indexCloud  < m_vCloudsAnimation.size() && indexCloud > -1)
    {
        sendAnimationPathFile(m_cloudModFileName[indexCloud], m_cloudSeqFileName[indexCloud], m_cloudCorrFileName[indexCloud]);
    }
    else
    {
        sendAnimationPathFile(QString("..."), QString("..."), QString("..."));
    }
}

void SWViewerWorker::updateMeshAnimationPath(int indexMesh)
{
    if(indexMesh  < m_vMeshesAnimation.size() && indexMesh > -1)
    {
        sendAnimationPathFile(m_meshModFileName[indexMesh], m_meshSeqFileName[indexMesh], m_meshCorrFileName[indexMesh]);
    }
    else
    {
        sendAnimationPathFile(QString("..."), QString("..."), QString("..."));
    }
}


void SWViewerWorker::setModFile(bool isCloud, int indexObject, QString pathFile)
{
    swAnimation::SWMod l_mod;
    l_mod.loadModFile(pathFile);

    if(isCloud)
    {
        if(indexObject < m_vCloudsAnimation.size())
        {
            // modify an animation
            m_vCloudsAnimation[indexObject].setMod(l_mod);
            m_vCloudsAnimation[indexObject].constructCorrId();
            m_cloudModFileName[indexObject] = pathFile;
            updateCloudAnimationPath(indexObject);
        }
    }
    else
    {
        if(indexObject < m_vMeshesAnimation.size())
        {
            // modify an animation
            m_vMeshesAnimation[indexObject].setMod(l_mod);
            m_vMeshesAnimation[indexObject].constructCorrId();
            m_meshModFileName[indexObject] = pathFile;
            updateMeshAnimationPath(indexObject);
        }
    }
}

void SWViewerWorker::addAnimation(bool isCloud)
{
    swAnimation::SWAnimation l_animation;
    if(isCloud)
    {
        m_cloudSeqFileName.push_back("...");
        m_cloudModFileName.push_back("...");
        m_cloudCorrFileName.push_back("...");
        m_vCloudsAnimation.push_back(l_animation);
    }
    else
    {
        m_meshSeqFileName.push_back("...");
        m_meshModFileName.push_back("...");
        m_meshCorrFileName.push_back("...");
        m_vMeshesAnimation.push_back(l_animation);
    }
}

void SWViewerWorker::setSeqFile(bool isCloud, int indexObject, QString pathFile)
{
    swAnimation::SWSeq l_seq;
    l_seq.loadSeqFile(pathFile);

    if(isCloud)
    {
        if(indexObject < m_vCloudsAnimation.size())
        {
            // modify an animation
            m_vCloudsAnimation[indexObject].setSeq(l_seq);
            m_cloudSeqFileName[indexObject] = pathFile;
            updateCloudAnimationPath(indexObject);
        }
    }
    else
    {
        if(indexObject < m_vMeshesAnimation.size())
        {
            // modify an animation
            m_vMeshesAnimation[indexObject].setSeq(l_seq);
            m_meshSeqFileName[indexObject] = pathFile;
            updateMeshAnimationPath(indexObject);
        }
    }
}

void SWViewerWorker::setCorrFilePath(bool isCloud, int indexObject, QString pathFile)
{
    if(isCloud)
    {
        if(indexObject < m_vCloudsAnimation.size())
        {
            m_vCloudsAnimation[indexObject].setCloudCorr(pathFile);
            m_cloudCorrFileName[indexObject] = pathFile;
            m_vCloudsAnimation[indexObject].constructCorrId();
            updateCloudAnimationPath(indexObject);
        }
    }
    else
    {
        if(indexObject < m_vMeshesAnimation.size())
        {
            m_vMeshesAnimation[indexObject].setCloudCorr(pathFile);
            m_meshCorrFileName[indexObject] = pathFile;
            m_vMeshesAnimation[indexObject].constructCorrId();
            updateMeshAnimationPath(indexObject);
        }
    }
}

void SWViewerWorker::deleteAnimation(bool isCLoud, int indexObject)
{
    if(isCLoud)
    {
        if(indexObject < m_vCloudsAnimation.size())
        {
            m_vCloudsAnimation.removeAt(indexObject);
            m_cloudModFileName.removeAt(indexObject);
            m_cloudSeqFileName.removeAt(indexObject);
            m_cloudCorrFileName.removeAt(indexObject);
        }
    }
    else
    {
        if(indexObject < m_vMeshesAnimation.size())
        {
            m_vMeshesAnimation.removeAt(indexObject);
            m_meshModFileName.removeAt(indexObject);
            m_meshSeqFileName.removeAt(indexObject);
            m_meshCorrFileName.removeAt(indexObject);
        }
    }
}




/**
 * \file SWGLMultiObjectWidget.cpp
 * \brief Defines SWGLMultiObjectWidget
 * \author Florian Lance
 * \date 03/06/14
 */


#include "interface/QtWidgets/SWGLMultiObjectWidget.h"
#include <iostream>

#include "moc_SWGLMultiObjectWidget.cpp"

#include "interface/SWGLUtility.h"

#include <QGLFunctions>

SWGLMultiObjectWidget::SWGLMultiObjectWidget(QGLContext *context, QWidget* parent) :
    SWGLWidget(context, parent)
{}

SWGLMultiObjectWidget::~SWGLMultiObjectWidget()
{}

void SWGLMultiObjectWidget::setCloudParameters(cuint ui32Index, const SWGLObjectParameters &oParams)
{

    m_vCloudsParameters[ui32Index]->m_parametersMutex.lockForWrite();
        m_vCloudsParameters[ui32Index]->m_bCloud = oParams.m_bCloud;
        m_vCloudsParameters[ui32Index]->m_bVisible = oParams.m_bVisible;
        m_vCloudsParameters[ui32Index]->m_bDisplayLines = oParams.m_bDisplayLines;
        m_vCloudsParameters[ui32Index]->displayMode = oParams.displayMode;
        m_vCloudsParameters[ui32Index]->m_dScaling = oParams.m_dScaling;
        m_vCloudsParameters[ui32Index]->m_vUnicolor = oParams.m_vUnicolor;
        m_vCloudsParameters[ui32Index]->m_vTranslation = oParams.m_vTranslation;
        m_vCloudsParameters[ui32Index]->m_vRotation = oParams.m_vRotation;
        m_vCloudsParameters[ui32Index]->m_sTexturePath = oParams.m_sTexturePath;
    m_vCloudsParameters[ui32Index]->m_parametersMutex.unlock();

    updateGL();
}

void SWGLMultiObjectWidget::setMeshParameters(cuint ui32Index, const SWGLObjectParameters &oParams)
{
    m_vMeshesParameters[ui32Index]->m_parametersMutex.lockForWrite();
        m_vMeshesParameters[ui32Index]->m_bCloud = oParams.m_bCloud;
        m_vMeshesParameters[ui32Index]->m_bVisible = oParams.m_bVisible;
        m_vMeshesParameters[ui32Index]->m_bDisplayLines = oParams.m_bDisplayLines;
        m_vMeshesParameters[ui32Index]->displayMode = oParams.displayMode;
        m_vMeshesParameters[ui32Index]->m_dScaling = oParams.m_dScaling;
        m_vMeshesParameters[ui32Index]->m_vUnicolor = oParams.m_vUnicolor;
        m_vMeshesParameters[ui32Index]->m_vTranslation = oParams.m_vTranslation;
        m_vMeshesParameters[ui32Index]->m_vRotation = oParams.m_vRotation;
        m_vMeshesParameters[ui32Index]->m_sTexturePath = oParams.m_sTexturePath;
    m_vMeshesParameters[ui32Index]->m_parametersMutex.unlock();

    updateGL();
}

void SWGLMultiObjectWidget::cloudParameters(cuint ui32Index, SWGLObjectParameters &oParams)
{
    m_vCloudsParameters[ui32Index]->m_parametersMutex.lockForRead();
        oParams.m_bCloud        = m_vCloudsParameters[ui32Index]->m_bCloud;
        oParams.m_bVisible      = m_vCloudsParameters[ui32Index]->m_bVisible;
        oParams.m_bDisplayLines = m_vCloudsParameters[ui32Index]->m_bDisplayLines;
        oParams.displayMode     = m_vCloudsParameters[ui32Index]->displayMode;
        oParams.m_dScaling      = m_vCloudsParameters[ui32Index]->m_dScaling;
        oParams.m_vUnicolor     = m_vCloudsParameters[ui32Index]->m_vUnicolor;
        oParams.m_vTranslation  = m_vCloudsParameters[ui32Index]->m_vTranslation;
        oParams.m_vRotation     = m_vCloudsParameters[ui32Index]->m_vRotation;
        oParams.m_sTexturePath  = m_vCloudsParameters[ui32Index]->m_sTexturePath;
    m_vCloudsParameters[ui32Index]->m_parametersMutex.unlock();
}

void SWGLMultiObjectWidget::meshParameters(cuint ui32Index, SWGLObjectParameters &oParams)
{
    m_vMeshesParameters[ui32Index]->m_parametersMutex.lockForRead();
        oParams.m_bCloud        = m_vMeshesParameters[ui32Index]->m_bCloud;
        oParams.m_bVisible      = m_vMeshesParameters[ui32Index]->m_bVisible;
        oParams.m_bDisplayLines = m_vMeshesParameters[ui32Index]->m_bDisplayLines;
        oParams.displayMode     = m_vMeshesParameters[ui32Index]->displayMode;
        oParams.m_dScaling      = m_vMeshesParameters[ui32Index]->m_dScaling;
        oParams.m_vUnicolor     = m_vMeshesParameters[ui32Index]->m_vUnicolor;
        oParams.m_vTranslation  = m_vMeshesParameters[ui32Index]->m_vTranslation;
        oParams.m_vRotation     = m_vMeshesParameters[ui32Index]->m_vRotation;
        oParams.m_sTexturePath  = m_vMeshesParameters[ui32Index]->m_sTexturePath;
    m_vMeshesParameters[ui32Index]->m_parametersMutex.unlock();
}

void SWGLMultiObjectWidget::initializeGL()
{
    // set perspective
        m_rZNear = 0.01;
        m_rZFar  = 100.0;
        m_rFOV   = 40.0;

    // set background
        qglClearColor(QColor(49, 53, 70));

    initShaders("../data/shaders/cloudViewer.vert", "../data/shaders/cloudViewer.frag", m_oShaderCloud);
    initShaders("../data/shaders/meshViewer.vert",  "../data/shaders/meshViewer.frag",  m_oShaderMesh);

    // enable depth buffer
        glEnable(GL_DEPTH_TEST);
}

void SWGLMultiObjectWidget::paintGL()
{   
    // set the size point
        glPointSize(m_glFSizePoint);

    // clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // create texture
//        if(m_bBindTexture)
//        {
//            m_bBindTexture = false;
//            m_textureLocation = bindTexture(m_oTexture);
//        }

    // calculate model view transformation
        QMatrix4x4 l_oViewMatrix;
        l_oViewMatrix.setToIdentity();

    // set camera vue
        l_oViewMatrix.lookAt( m_pCamera->eyePosition(), m_pCamera->viewDirection(), m_pCamera->up());

    // comput MVP matrix
        QMatrix4x4 l_oModelMatrix;
        l_oModelMatrix.setToIdentity();
        m_oMVPMatrix = l_oModelMatrix  * m_oProjectionMatrix * l_oViewMatrix;

    drawScene();    
}

void SWGLMultiObjectWidget::addCloud(const QString &sPathCloud)
{
    makeCurrent();

    SWCloudPtr l_pCloud = SWCloudPtr(new swCloud::SWCloud());
    l_pCloud->loadObj(sPathCloud.toUtf8().constData());

    SWGLObjectParametersPtr l_pCloudParam = SWGLObjectParametersPtr(new SWGLObjectParameters);
    l_pCloudParam->m_bCloud = true;
    l_pCloudParam->m_bVisible = true;
    l_pCloudParam->m_bDisplayLines   = false;
    l_pCloudParam->displayMode = GLO_ORIGINAL_COLOR;
    l_pCloudParam->m_dScaling = 1.;
    l_pCloudParam->m_vRotation = QVector3D(0.,0.,0.);
    l_pCloudParam->m_vRotation = QVector3D(0.,0.,0.);
    l_pCloudParam->m_sTexturePath = QString("...");
    l_pCloudParam->m_vUnicolor = QVector3D(255.,0.,0.);

    // init buffers
        QGLBufferPtr l_indexBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_vertexBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_colorBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_normalBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_textureBuffer = QGLBufferPtr(new QGLBuffer());
        initIndexBuffer(*l_indexBuffer);
        initVertexBuffer(*l_vertexBuffer);
        initVertexBuffer(*l_normalBuffer);
        initVertexBuffer(*l_textureBuffer);
        initVertexBuffer(*l_colorBuffer);

    // set usage pattern
        l_indexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_vertexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_normalBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_textureBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_colorBuffer->setUsagePattern(QGLBuffer::StaticDraw);

    m_pListCloudsMutex.lockForWrite();
        m_vClouds.push_back(l_pCloud);
        m_vCloudsParameters.push_back(l_pCloudParam);
        m_vCloudsVertexBuffer.push_back(l_vertexBuffer);
        m_vCloudsIndexBuffer.push_back(l_indexBuffer);
        m_vCloudsNormalBuffer.push_back(l_normalBuffer);
        m_vCloudsTextureBuffer.push_back(l_textureBuffer);
        m_vCloudsColorBuffer.push_back(l_colorBuffer);

        m_vCloudsBufferToUpdate.push_back(true);

    m_pListCloudsMutex.unlock();

    updateGL();
}

void SWGLMultiObjectWidget::addMesh(const QString &sPathMesh)
{
    makeCurrent();

    SWMeshPtr l_pMesh = SWMeshPtr(new swMesh::SWMesh(sPathMesh.toUtf8().constData()));

    SWGLObjectParametersPtr l_pMeshesParam = SWGLObjectParametersPtr(new SWGLObjectParameters);
    l_pMeshesParam->m_bCloud = false;
    l_pMeshesParam->m_bVisible = true;
    l_pMeshesParam->m_bDisplayLines   = false;
    l_pMeshesParam->displayMode = GLO_ORIGINAL_COLOR;
    l_pMeshesParam->m_dScaling = 1.;
    l_pMeshesParam->m_vRotation = QVector3D(0.,0.,0.);
    l_pMeshesParam->m_vRotation = QVector3D(0.,0.,0.);
    l_pMeshesParam->m_sTexturePath = QString("...");
    l_pMeshesParam->m_vUnicolor = QVector3D(255.,0.,0.);

    // init buffers
        QGLBufferPtr l_indexBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_vertexBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_colorBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_normalBuffer = QGLBufferPtr(new QGLBuffer());
        QGLBufferPtr l_textureBuffer = QGLBufferPtr(new QGLBuffer());
        initIndexBuffer(*l_indexBuffer);
        initVertexBuffer(*l_vertexBuffer);
        initVertexBuffer(*l_normalBuffer);
        initVertexBuffer(*l_textureBuffer);
        initVertexBuffer(*l_colorBuffer);

    // set usage pattern
        l_indexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_vertexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_normalBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_textureBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        l_colorBuffer->setUsagePattern(QGLBuffer::StaticDraw);

    // allocate buffers
        float  *l_aFVertexBuffer   = l_pMesh->vertexBuffer();
        float  *l_aFColorBuffer    = l_pMesh->cloud()->colorBuffer();
        uint32 *l_aUI32IndexBuffer = l_pMesh->indexVertexTriangleBuffer();
        float  *l_aFNormalBuffer   = l_pMesh->normalBuffer();
        float  *l_aFTextureBuffer  = l_pMesh->textureBuffer();
            allocateBuffer(*l_vertexBuffer,  l_aFVertexBuffer,     l_pMesh->pointsNumber() *  3 * sizeof(float) );
            allocateBuffer(*l_indexBuffer,   l_aUI32IndexBuffer,   l_pMesh->trianglesNumber() * 3* sizeof(GLuint) );
            allocateBuffer(*l_colorBuffer,   l_aFColorBuffer,      l_pMesh->pointsNumber() *  3 * sizeof(float));
            allocateBuffer(*l_normalBuffer,  l_aFNormalBuffer,     l_pMesh->pointsNumber() *  3 * sizeof(float));
            allocateBuffer(*l_textureBuffer, l_aFTextureBuffer,    l_pMesh->pointsNumber() *  2 * sizeof(float) );
        deleteAndNullifyArray(l_aFVertexBuffer);
        deleteAndNullifyArray(l_aUI32IndexBuffer);
        deleteAndNullifyArray(l_aFColorBuffer);
        deleteAndNullifyArray(l_aFNormalBuffer);
        deleteAndNullifyArray(l_aFTextureBuffer);

    m_oParamMutex.lockForWrite();
        m_vMeshes.push_back(l_pMesh);
        m_vMeshesParameters.push_back(l_pMeshesParam);
        m_vMeshesVertexBuffer.push_back(l_vertexBuffer);
        m_vMeshesIndexBuffer.push_back(l_indexBuffer);
        m_vMeshesNormalBuffer.push_back(l_normalBuffer);
        m_vMeshesTextureBuffer.push_back(l_textureBuffer);
        m_vMeshesColorBuffer.push_back(l_colorBuffer);

        m_vCloudsBufferToUpdate.push_back(true);

    m_oParamMutex.unlock();

    updateGL();
}

void SWGLMultiObjectWidget::removeCloud(cuint ui32Index)
{
    makeCurrent();

    m_pListCloudsMutex.lockForWrite();

    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    if(ui32Index < static_cast<uint>(m_vClouds.size()))
    {        
        m_vClouds.removeAt(ui32Index);
        m_vCloudsParameters.removeAt(ui32Index);

        m_vBuffersToDelete.push_back(m_vCloudsVertexBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vCloudsIndexBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vCloudsColorBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vCloudsTextureBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vCloudsNormalBuffer[ui32Index]);

        m_vCloudsVertexBuffer.removeAt(ui32Index);
        m_vCloudsNormalBuffer.removeAt(ui32Index);
        m_vCloudsIndexBuffer.removeAt(ui32Index);
        m_vCloudsTextureBuffer.removeAt(ui32Index);
        m_vCloudsColorBuffer.removeAt(ui32Index);
        m_vCloudsBufferToUpdate.removeAt(ui32Index);

        m_pListCloudsMutex.unlock();
    }
    else
    {
        m_pListCloudsMutex.unlock();
        std::cerr << "-ERROR : bad index removeCloud " << std::endl;
        return;
    }

    updateGL();
}

void SWGLMultiObjectWidget::removeMesh(cuint ui32Index)
{    
    makeCurrent();

    m_pListMeshesMutex.lockForWrite();

    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    if(ui32Index < static_cast<uint>(m_vMeshes.size()))
    {
        m_vMeshes.removeAt(ui32Index);
        m_vMeshesParameters.removeAt(ui32Index);

        m_vBuffersToDelete.push_back(m_vMeshesVertexBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vMeshesNormalBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vMeshesIndexBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vMeshesTextureBuffer[ui32Index]);
        m_vBuffersToDelete.push_back(m_vMeshesColorBuffer[ui32Index]);

        m_vMeshesVertexBuffer.removeAt(ui32Index);
        m_vMeshesNormalBuffer.removeAt(ui32Index);
        m_vMeshesIndexBuffer.removeAt(ui32Index);
        m_vMeshesTextureBuffer.removeAt(ui32Index);
        m_vMeshesColorBuffer.removeAt(ui32Index);        

        m_pListMeshesMutex.unlock();
    }
    else
    {
        m_pListMeshesMutex.unlock();
        std::cerr << "-ERROR : bad index removeMesh " << std::endl;
        return;
    }

    updateGL();
}

void SWGLMultiObjectWidget::setTexture(cuint ui32Index, const QString &sTexturePath)
{
//    m_oParamMutex.lockForWrite();
//        m_textureLocation = bindTexture(QImage(sTexturePath));
//        m_bBindTexture = true;
    //    m_oParamMutex.unlock();
}

void SWGLMultiObjectWidget::applyTexture(cuint ui32Index, const bool bApplyTexture)
{
    //    m_oParamMutex.lockForWrite();
    //        m_bApplyTexture = bApplyTexture;
    //    m_oParamMutex.unlock();
    //    updateGL();
}


void SWGLMultiObjectWidget::drawClouds()
{
    // bind shader for clouds
    m_oShaderCloud.bind();
        checkGlError(true);

    // retrieve meshes number
    int l_i32CloudsNumber = m_vClouds.size();

    // display clouds
        for(int ii = 0; ii < l_i32CloudsNumber; ++ii)
        {                       
            // release buffers
                QGLBuffer::release(QGLBuffer::VertexBuffer);
                QGLBuffer::release(QGLBuffer::IndexBuffer);

            // retrieve data
                m_vCloudsParameters[ii]->m_parametersMutex.lockForRead();
                    bool l_bVisible = m_vCloudsParameters[ii]->m_bVisible;
                    QVector3D l_vTranslation = m_vCloudsParameters[ii]->m_vTranslation;
                    QVector3D l_vRotation = m_vCloudsParameters[ii]->m_vRotation;
                    QVector3D l_vUnicolor = m_vCloudsParameters[ii]->m_vUnicolor;
                    GLObjectDisplayMode l_oDisplayMode = m_vCloudsParameters[ii]->displayMode;
                m_vCloudsParameters[ii]->m_parametersMutex.unlock();

            // check visibility
                if(!l_bVisible)
                {
                    continue;
                }

            // apply transformations² // TODO : apply rotate to a defined axis
                swCloud::SWRigidMotion l_oTransfo;
                l_oTransfo.m_aFTranslation[0] = l_vTranslation.x();
                l_oTransfo.m_aFTranslation[1] = l_vTranslation.y();
                l_oTransfo.m_aFTranslation[2] = l_vTranslation.z();
                l_oTransfo.m_aFRotAngles[0]   = l_vRotation.x();
                l_oTransfo.m_aFRotAngles[1]   = l_vRotation.y();
                l_oTransfo.m_aFRotAngles[2]   = l_vRotation.z();

                l_oTransfo.computeRotationMatrix();

//                l_oCloud.transform(l_oTransfo.m_aFRotation, l_oTransfo.m_aFTranslation);

            // uniform
                m_oShaderCloud.setUniformValue("displayMode", l_oDisplayMode);
                m_oShaderCloud.setUniformValue("uniColor", l_vUnicolor.x()/255., l_vUnicolor.y()/255., l_vUnicolor.z()/255.);
                m_oShaderCloud.setUniformValue("mvpMatrix", m_oMVPMatrix);

            if(m_vCloudsBufferToUpdate[ii])
            {
                // allocate buffers
                    float  *l_aFVertexBuffer   = m_vClouds[ii]->vertexBuffer();
                    float  *l_aFColorBuffer    = m_vClouds[ii]->colorBuffer();
                    uint32 *l_aUI32IndexBuffer = m_vClouds[ii]->indexBuffer();
                        allocateBuffer(*m_vCloudsVertexBuffer[ii], l_aFVertexBuffer,   m_vClouds[ii]->size() *  3 * sizeof(float) );
                        allocateBuffer(*m_vCloudsIndexBuffer[ii], l_aUI32IndexBuffer, m_vClouds[ii]->size() * sizeof(GLuint) );
                        allocateBuffer(*m_vCloudsColorBuffer[ii], l_aFColorBuffer,    m_vClouds[ii]->size() *  3 * sizeof(float));
                    deleteAndNullifyArray(l_aFVertexBuffer);
                    deleteAndNullifyArray(l_aUI32IndexBuffer);
                    deleteAndNullifyArray(l_aFColorBuffer);

                m_vCloudsBufferToUpdate[ii] = false;
            }


            // draw
                drawBufferWithColor(*m_vCloudsIndexBuffer[ii], *m_vCloudsVertexBuffer[ii],
                                    *m_vCloudsColorBuffer[ii], m_oShaderCloud, GL_POINTS);
        }

    m_oShaderCloud.release();
        checkGlError(true);
}

void SWGLMultiObjectWidget::drawMeshes()
{   
    // bind shader for meshes
    m_oShaderMesh.bind();
        checkGlError(true);

    // retrieve meshes number
    int l_i32MeshesNumber = m_vMeshes.size();

    // display meshes
        for(int ii = 0; ii < l_i32MeshesNumber; ++ii)
        {
            // release buffers
                QGLBuffer::release(QGLBuffer::VertexBuffer);
                QGLBuffer::release(QGLBuffer::IndexBuffer);

            // retrieve data
                m_vMeshesParameters[ii]->m_parametersMutex.lockForRead();
                    bool l_bVisible = m_vMeshesParameters[ii]->m_bVisible;
                    bool l_bDisplayLines = m_vMeshesParameters[ii]->m_bDisplayLines;
                    QVector3D l_vTranslation = m_vMeshesParameters[ii]->m_vTranslation;
                    QVector3D l_vRotation = m_vMeshesParameters[ii]->m_vRotation;
                    QVector3D l_vUnicolor = m_vMeshesParameters[ii]->m_vUnicolor;
                    GLObjectDisplayMode l_oDisplayMode = m_vMeshesParameters[ii]->displayMode;
                m_vMeshesParameters[ii]->m_parametersMutex.unlock();

            // check visibility
                if(!l_bVisible)
                {
                    continue;
                }

            // set polygon mode
                if(l_bDisplayLines)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                else
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

            // apply transformations² // TODO : apply rotate to a defined axis
                swCloud::SWRigidMotion l_oTransfo;
                l_oTransfo.m_aFTranslation[0] = l_vTranslation.x();
                l_oTransfo.m_aFTranslation[1] = l_vTranslation.y();
                l_oTransfo.m_aFTranslation[2] = l_vTranslation.z();
                l_oTransfo.m_aFRotAngles[0]   = l_vRotation.x();
                l_oTransfo.m_aFRotAngles[1]   = l_vRotation.y();
                l_oTransfo.m_aFRotAngles[2]   = l_vRotation.z();

                l_oTransfo.computeRotationMatrix();

                // ...

            // uniform
                m_oShaderMesh.setUniformValue("displayMode", l_oDisplayMode);
                m_oShaderMesh.setUniformValue("uniColor", l_vUnicolor.x()/255., l_vUnicolor.y()/255., l_vUnicolor.z()/255.);
                m_oShaderMesh.setUniformValue("mvpMatrix", m_oMVPMatrix);

            // draw
                    if(l_oDisplayMode == GLO_ORIGINAL_COLOR)
                    {
                        drawBufferWithColor(*m_vMeshesIndexBuffer[ii], *m_vMeshesVertexBuffer[ii],
                                            *m_vMeshesColorBuffer[ii], *m_vMeshesNormalBuffer[ii], m_oShaderMesh, GL_TRIANGLES);
                    }
                    else if(l_oDisplayMode == GLO_TEXTURE)
                    {
                        glEnable(GL_TEXTURE_2D);
                        drawBufferWithTexture(*m_vMeshesIndexBuffer[ii], *m_vMeshesVertexBuffer[ii],
                                              *m_vMeshesTextureBuffer[ii],*m_vMeshesNormalBuffer[ii], m_oShaderMesh, GL_TRIANGLES);
                    }
                    else if(l_oDisplayMode == GLO_UNI_COLOR)
                    {
                        drawBuffer(*m_vMeshesIndexBuffer[ii], *m_vMeshesVertexBuffer[ii],
                                   *m_vMeshesNormalBuffer[ii], m_oShaderMesh, GL_TRIANGLES);
                    }
        }

        m_oShaderMesh.release();

    glDisable(GL_TEXTURE_2D);
}

void SWGLMultiObjectWidget::drawScene()
{   
    drawAxes(m_oShaderCloud, m_oMVPMatrix, 0.02f);

    m_pListMeshesMutex.lockForRead();
        drawMeshes();
    m_pListMeshesMutex.unlock();

    m_pListCloudsMutex.lockForRead();
        drawClouds();
    m_pListCloudsMutex.unlock();
}



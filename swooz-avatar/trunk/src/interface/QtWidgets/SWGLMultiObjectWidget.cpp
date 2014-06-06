


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
    SWGLWidget(context, parent),
    m_vertexBuffer(QGLBuffer::VertexBuffer), m_indexBuffer(QGLBuffer::IndexBuffer),  m_normalBuffer(QGLBuffer::VertexBuffer), m_textureBuffer(QGLBuffer::VertexBuffer),
    m_colorBuffer(QGLBuffer::VertexBuffer)
{}

SWGLMultiObjectWidget::~SWGLMultiObjectWidget()
{}

void SWGLMultiObjectWidget::initBuffers()
{
    // create the buffer
        m_vertexBuffer.create();
        m_indexBuffer.create();
        m_normalBuffer.create();
        m_textureBuffer.create();
        m_colorBuffer.create();

    // define the usage pattern
//        m_vertexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
//        m_indexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
//        m_normalBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
//        m_textureBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
}

void SWGLMultiObjectWidget::setCloudParameters(cuint ui32Index, const SWGLObjectParameters &oParams)
{
    m_oParamMutex.lockForWrite();
        m_vCloudsParameters[ui32Index] = oParams;
    m_oParamMutex.unlock();

    updateGL();
}

void SWGLMultiObjectWidget::setMeshParameters(cuint ui32Index, const SWGLObjectParameters &oParams)
{
    m_oParamMutex.lockForWrite();
        m_vMeshesParameters[ui32Index] = oParams;
    m_oParamMutex.unlock();

    updateGL();
}

void SWGLMultiObjectWidget::cloudParameters(cuint ui32Index, SWGLObjectParameters &oParams)
{
    m_oParamMutex.lockForRead();
        oParams = m_vCloudsParameters[ui32Index];
    m_oParamMutex.unlock();
}

void SWGLMultiObjectWidget::meshParameters(cuint ui32Index, SWGLObjectParameters &oParams)
{
    m_oParamMutex.lockForRead();
        oParams = m_vMeshesParameters[ui32Index];
    m_oParamMutex.unlock();
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

    // enable texture
        glEnable(GL_TEXTURE_2D);

    initBuffers();
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
    SWCloudPtr l_pCloud( new swCloud::SWCloud());
    l_pCloud->loadObj(sPathCloud.toUtf8().constData());

    SWGLObjectParameters l_oCloudParam;
    l_oCloudParam.m_bCloud = true;
    l_oCloudParam.m_bVisible = true;
    l_oCloudParam.m_bDisplayLines   = false;
    l_oCloudParam.displayMode = GLO_ORIGINAL_COLOR;
    l_oCloudParam.m_dScaling = 1.;
    l_oCloudParam.m_vRotation = QVector3D(0.,0.,0.);
    l_oCloudParam.m_vRotation = QVector3D(0.,0.,0.);
    l_oCloudParam.m_sTexturePath = QString("...");
    l_oCloudParam.m_vUnicolor = QVector3D(255.,0.,0.);   

    m_oParamMutex.lockForWrite();
        m_vClouds.push_back(l_pCloud);
        m_vCloudsParameters.push_back(l_oCloudParam);
    m_oParamMutex.unlock();

    updateGL();
}

void SWGLMultiObjectWidget::addMesh(const QString &sPathMesh)
{
    SWMeshPtr l_pMesh(new swMesh::SWMesh(sPathMesh.toUtf8().constData()));

    SWGLObjectParameters l_oMeshesParam;
    l_oMeshesParam.m_bCloud = false;
    l_oMeshesParam.m_bVisible = true;
    l_oMeshesParam.m_bDisplayLines   = false;
    l_oMeshesParam.displayMode = GLO_ORIGINAL_COLOR;
    l_oMeshesParam.m_dScaling = 1.;
    l_oMeshesParam.m_vRotation = QVector3D(0.,0.,0.);
    l_oMeshesParam.m_vRotation = QVector3D(0.,0.,0.);
    l_oMeshesParam.m_sTexturePath = QString("...");
    l_oMeshesParam.m_vUnicolor = QVector3D(255.,0.,0.);

    m_oParamMutex.lockForWrite();
        m_vMeshes.push_back(l_pMesh);
        m_vMeshesParameters.push_back(l_oMeshesParam);
    m_oParamMutex.unlock();

    updateGL();
}

void SWGLMultiObjectWidget::removeCloud(cuint ui32Index)
{
    if(ui32Index < static_cast<uint>(m_vClouds.size()))
    {
        m_oParamMutex.lockForWrite();
            m_vClouds.removeAt(ui32Index);
            m_vCloudsParameters.removeAt(ui32Index);
        m_oParamMutex.unlock();

        updateGL();
    }
    else
    {
        std::cerr << "-ERROR : bad index removeCloud " << std::endl;
    }
}

void SWGLMultiObjectWidget::removeMesh(cuint ui32Index)
{
    if(ui32Index < static_cast<uint>(m_vMeshes.size()))
    {
        m_oParamMutex.lockForWrite();
            m_vMeshes.removeAt(ui32Index);
            m_vMeshesParameters.removeAt(ui32Index);
        m_oParamMutex.unlock();

        updateGL();
    }
    else
    {
        std::cerr << "-ERROR : bad index removeMesh " << std::endl;
    }
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
}

//void SWGLMeshWidget::applyTexture(const bool bApplyTexture)
//{
//    m_oParamMutex.lockForWrite();
//        m_bApplyTexture = bApplyTexture;
//    m_oParamMutex.unlock();
//    updateGL();
//}

void SWGLMultiObjectWidget::setMeshLinesRender(cuint ui32Index, const bool bRenderLines)
{
//    m_oParamMutex.lockForWrite();
//        m_bLinesRender = bRenderLines;
//    m_oParamMutex.unlock();
//    updateGL();
}

//void SWGLMeshWidget::setMesh(swMesh::SWMesh *pMesh)
//{
//    if(pMesh)
//    {
//        if(pMesh->trianglesNumber() < 1)
//        {
//            return;
//        }
//    }
//    else
//    {
//        return;
//    }

//    if(m_bInitCamWithCloudPosition)
//    {
//        swCloud::SWCloudBBox l_oBBox = pMesh->cloud()->bBox();
//        QVector3D l_oEye,l_oLookAt;
//        l_oEye.setX((l_oBBox.m_fMaxX + l_oBBox.m_fMinX)/2);
//        l_oEye.setY((l_oBBox.m_fMaxY + l_oBBox.m_fMinY)/2);
//        l_oEye.setZ((l_oBBox.m_fMaxZ + l_oBBox.m_fMinZ)/2);

//        l_oLookAt = l_oEye;
//        l_oEye.setZ(l_oEye.z() - 0.25f);
//        l_oLookAt.setZ(l_oLookAt.z() + 1.f);

//        resetCamera(l_oEye,l_oLookAt);

//        m_bInitCamWithCloudPosition = false;
//    }

//    deleteAndNullify(m_pMesh);
//    m_pMesh = new swMesh::SWMesh(*pMesh);
//    updateGL();
//}


//void SWGLMeshWidget::setMesh(swMesh::SWMesh &oMesh)
//{
//    m_pMesh = &oMesh;
//    updateGL();
//}


void SWGLMultiObjectWidget::drawClouds()
{
    // bind shader for clouds
        if(!m_oShaderCloud.bind())
        {
            throw swExcept::swShaderGLError();
        }

    m_oParamMutex.lockForRead();
        int l_i32CloudsNumber = m_vClouds.size();
    m_oParamMutex.unlock();

    // display clouds
        for(int ii = 0; ii < l_i32CloudsNumber; ++ii)
        {
            // release buffers
                QGLBuffer::release(QGLBuffer::VertexBuffer);
                QGLBuffer::release(QGLBuffer::IndexBuffer);

            // retrieve data
                m_oParamMutex.lockForRead();
                    swCloud::SWCloud l_oCloud;
                    l_oCloud.copy(*m_vClouds[ii]);
                    SWGLObjectParameters l_oParam = m_vCloudsParameters[ii];
                m_oParamMutex.unlock();

            // check visibility
                if(!l_oParam.m_bVisible)
                {
                    continue;
                }

            // apply transformations² // TODO : apply rotate to a defined axis
                swCloud::SWRigidMotion l_oTransfo;
                l_oTransfo.m_aFTranslation[0] = l_oParam.m_vTranslation.x();
                l_oTransfo.m_aFTranslation[1] = l_oParam.m_vTranslation.y();
                l_oTransfo.m_aFTranslation[2] = l_oParam.m_vTranslation.z();
                l_oTransfo.m_aFRotAngles[0] = l_oParam.m_vRotation.x();
                l_oTransfo.m_aFRotAngles[1] = l_oParam.m_vRotation.y();
                l_oTransfo.m_aFRotAngles[2] = l_oParam.m_vRotation.z();

                l_oTransfo.computeRotationMatrix();
                l_oCloud.transform(l_oTransfo.m_aFRotation, l_oTransfo.m_aFTranslation);


            // init buffers
                float  *l_aFVertexBuffer   = l_oCloud.vertexBuffer();
                uint32 *l_aUI32IndexBuffer = l_oCloud.indexBuffer();
                float  *l_aCloudC = NULL;

                allocateBuffer(m_vertexBuffer,  l_aFVertexBuffer,   l_oCloud.size() *  3 * sizeof(float) );
                allocateBuffer(m_indexBuffer,   l_aUI32IndexBuffer, l_oCloud.size() * sizeof(GLuint) );

            // apply display mode
                if(l_oParam.displayMode == GLO_ORIGINAL_COLOR)
                {
                    l_aCloudC   = l_oCloud.colorBuffer();
                    allocateBuffer(m_colorBuffer, l_aCloudC, l_oCloud.size() *  3 * sizeof(float));
                }
                else if(l_oParam.displayMode == GLO_UNI_COLOR)
                {
                    m_oShaderCloud.setUniformValue("uniColor", l_oParam.m_vUnicolor.x()/255., l_oParam.m_vUnicolor.y()/255., l_oParam.m_vUnicolor.z()/255.);
                }

            // uniform
                m_oShaderCloud.setUniformValue("displayMode", l_oParam.displayMode);
                m_oShaderCloud.setUniformValue("mvpMatrix", m_oMVPMatrix);

            // draw
                GLenum l_glError = drawBufferWithColor(m_indexBuffer, m_vertexBuffer, m_colorBuffer, m_oShaderCloud, GL_POINTS);

            // check errors
                if(l_glError)
                {
                    qWarning() << "SWGLMultiObjectWidget -> DrawScene GLError : " << l_glError;
                }

            // clean
                deleteAndNullifyArray(l_aFVertexBuffer);
                deleteAndNullifyArray(l_aUI32IndexBuffer);
                deleteAndNullifyArray(l_aCloudC);
        }
}

void SWGLMultiObjectWidget::drawMeshes()
{
    // bind shader for meshes
        if(!m_oShaderMesh.bind())
        {
            throw swExcept::swShaderGLError();
        }

    // retrieve meshes numbers
        m_oParamMutex.lockForRead();
            int l_i32MeshesNumber = m_vMeshes.size();
        m_oParamMutex.unlock();

    // display meshes
        for(int ii = 0; ii < l_i32MeshesNumber; ++ii)
        {
            // release buffers
                QGLBuffer::release(QGLBuffer::VertexBuffer);
                QGLBuffer::release(QGLBuffer::IndexBuffer);

            // retrieve data
                m_oParamMutex.lockForRead();
                    swMesh::SWMesh l_oMesh = (*m_vMeshes.at(ii));
                    SWGLObjectParameters l_oParam = m_vMeshesParameters[ii];
                m_oParamMutex.unlock();

            // check visibility
                if(!l_oParam.m_bVisible)
                {
                    continue;
                }

            // set polygon mode
                if(l_oParam.m_bDisplayLines)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                else
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

            // init buffers
                float  *l_aFVertexBuffer   = l_oMesh.vertexBuffer();
                uint32 *l_aUI32IndexBuffer = l_oMesh.indexVertexTriangleBuffer();
                float  *l_aFNormalBuffer   = l_oMesh.normalBuffer();
                float  *l_aFTextureBuffer  = NULL;
                float  *l_aFColorBuffer    = NULL;
                allocateBuffer(m_vertexBuffer,  l_aFVertexBuffer,   l_oMesh.pointsNumber() *     3 * sizeof(float) );
                allocateBuffer(m_indexBuffer,   l_aUI32IndexBuffer, l_oMesh.trianglesNumber() *  3 * sizeof(GLuint) );
                allocateBuffer(m_normalBuffer,  l_aFNormalBuffer,   l_oMesh.pointsNumber() *     3 * sizeof(float) );


            // apply display mode
                if(l_oParam.displayMode == GLO_ORIGINAL_COLOR)
                {
                    l_aFColorBuffer   = l_oMesh.cloud()->colorBuffer();
                    allocateBuffer(m_colorBuffer, l_aFColorBuffer, l_oMesh.pointsNumber() *  3 * sizeof(float));
                }
                else if(l_oParam.displayMode == GLO_UNI_COLOR)
                {
                    m_oShaderMesh.setUniformValue("uniColor", l_oParam.m_vUnicolor.x()/255., l_oParam.m_vUnicolor.y()/255., l_oParam.m_vUnicolor.z()/255.);
                }
                else if(l_oParam.displayMode == GLO_TEXTURE)
                {
                    l_aFTextureBuffer  = l_oMesh.textureBuffer();
                    allocateBuffer(m_textureBuffer, l_aFTextureBuffer,  l_oMesh.pointsNumber() *  2 * sizeof(float) );
                }

            // uniform
                m_oShaderMesh.setUniformValue("displayMode", l_oParam.displayMode);
                m_oShaderMesh.setUniformValue("mvpMatrix", m_oMVPMatrix);

            // draw
                GLenum l_glError;

                if(l_oParam.displayMode == GLO_ORIGINAL_COLOR)
                {
                    l_glError = drawBufferWithColor(m_indexBuffer, m_vertexBuffer, m_colorBuffer, m_normalBuffer, m_oShaderMesh, GL_TRIANGLES);
                }
                else if(l_oParam.displayMode == GLO_TEXTURE)
                {
                    l_glError = drawBufferWithTexture(m_indexBuffer, m_vertexBuffer, m_textureBuffer, m_normalBuffer, m_oShaderMesh, GL_TRIANGLES);
                }
                else if(l_oParam.displayMode == GLO_UNI_COLOR)
                {
                    l_glError = drawBuffer(m_indexBuffer, m_vertexBuffer, m_normalBuffer, m_oShaderMesh, GL_TRIANGLES);
                }

            // check errors
                if(l_glError)
                {
                    qWarning() << "SWGLMultiObjectWidget -> DrawScene GLError : " << l_glError;
                }

            // clean
                deleteAndNullifyArray(l_aFVertexBuffer);
                deleteAndNullifyArray(l_aUI32IndexBuffer);
                deleteAndNullifyArray(l_aFNormalBuffer);
                deleteAndNullifyArray(l_aFColorBuffer);
                deleteAndNullifyArray(l_aFTextureBuffer);
        }
}

void SWGLMultiObjectWidget::drawScene()
{
    drawAxes(m_oShaderCloud, m_oMVPMatrix, 0.02f);

    drawClouds();

    drawMeshes();
}



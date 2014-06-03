


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
    m_vertexBuffer(QGLBuffer::VertexBuffer), m_indexBuffer(QGLBuffer::IndexBuffer),  m_normalBuffer(QGLBuffer::VertexBuffer), m_textureBuffer(QGLBuffer::VertexBuffer)
{

}

SWGLMultiObjectWidget::~SWGLMultiObjectWidget()
{

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
    initShaders("../data/shaders/meshViewer.vert", "../data/shaders/meshViewer.frag", m_oShaderMesh);


    // enable depth buffer
        glEnable(GL_DEPTH_TEST);

    // enable texture
        glEnable(GL_TEXTURE_2D);

    initBuffers();
}


void SWGLMultiObjectWidget::paintGL()
{
    qDebug() << "paintGL ";

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

    drawAxes(m_oShaderMesh, m_oMVPMatrix, 0.02f);
}

void SWGLMultiObjectWidget::addCloud(const QString &sPathCloud)
{
    SWCloudPtr l_pCloud( new swCloud::SWCloud());
    l_pCloud->loadObj(sPathCloud.toUtf8().constData());
    m_vClouds.push_back(l_pCloud);
}

void SWGLMultiObjectWidget::addMesh(const QString &sPathMesh)
{
//    SWMeshPtr l_pMesh( new swMesh::SWMesh(sPathMesh.toUtf8().constData()));
//    qDebug() << " M : " << l_pMesh->pointsNumber();
    m_vMeshes.push_back(SWMeshPtr(new swMesh::SWMesh(sPathMesh.toUtf8().constData())));
    m_vBMeshLinesRender.push_back(false);
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


void SWGLMultiObjectWidget::initBuffers()
{
    // create the buffer
        m_vertexBuffer.create();
        m_indexBuffer.create();
        m_normalBuffer.create();
        m_textureBuffer.create();

    // define the usage pattern
//        m_vertexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
//        m_indexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
//        m_normalBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
//        m_textureBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
}

void SWGLMultiObjectWidget::drawScene()
{
    // bind shader for clouds
        if(!m_oShaderCloud.bind())
        {
            throw swExcept::swShaderGLError();
        }

    // display clouds
        for(int ii = 0; ii < m_vClouds.size(); ++ii)
        {
            std::cout << ii << " ";
            QGLBuffer::release(QGLBuffer::VertexBuffer);
            QGLBuffer::release(QGLBuffer::IndexBuffer);

            swCloud::SWCloud *l_pCloud  = &(*m_vClouds[ii]);

            float  *l_aFVertexBuffer   = l_pCloud->vertexBuffer();
            uint32 *l_aUI32IndexBuffer = l_pCloud->indexBuffer();

            allocateBuffer(m_vertexBuffer,  l_aFVertexBuffer,   l_pCloud->size() *  3 * sizeof(float) );
            allocateBuffer(m_indexBuffer,   l_aUI32IndexBuffer, l_pCloud->size() * sizeof(GLuint) );

            m_oShaderCloud.setUniformValue("mvpMatrix", m_oMVPMatrix);

            drawBuffer(m_indexBuffer, m_vertexBuffer, m_oShaderCloud, GL_POINTS);

            delete[] l_aFVertexBuffer;
            delete[] l_aUI32IndexBuffer;
        }

    // bin shader for meshes
        if(!m_oShaderMesh.bind())
        {
            throw swExcept::swShaderGLError();
        }

    // display meshes
        for(int ii = 0; ii < m_vMeshes.size(); ++ii)
        {
            std::cout << ii << " " << m_vMeshes.size() << " ";

            if(m_vBMeshLinesRender[ii])
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            QGLBuffer::release(QGLBuffer::VertexBuffer);
            QGLBuffer::release(QGLBuffer::IndexBuffer);

            swMesh::SWMesh *l_pMesh = &(*m_vMeshes.at(ii));

            qDebug() << " hhhhhhhhhhhhh ";

            float  *l_aFVertexBuffer   = l_pMesh->vertexBuffer();
            uint32 *l_aUI32IndexBuffer = l_pMesh->indexVertexTriangleBuffer();
            float  *l_aFNormalBuffer   = l_pMesh->normalBuffer();

            allocateBuffer(m_vertexBuffer,  l_aFVertexBuffer,   l_pMesh->pointsNumber() *     3 * sizeof(float) );
            allocateBuffer(m_indexBuffer,   l_aUI32IndexBuffer, l_pMesh->trianglesNumber() *  3 * sizeof(GLuint) );
            allocateBuffer(m_normalBuffer,  l_aFNormalBuffer,   l_pMesh->pointsNumber() *     3 * sizeof(float) );

            m_oShaderMesh.setUniformValue("mvpMatrix", m_oMVPMatrix);
            m_oShaderMesh.setUniformValue("applyTexture", false);

            drawBuffer(m_indexBuffer, m_vertexBuffer, m_normalBuffer, m_oShaderMesh, GL_TRIANGLES);

            delete[] l_aFVertexBuffer;
            delete[] l_aUI32IndexBuffer;
            delete[] l_aFNormalBuffer;
        }
}

void SWGLMultiObjectWidget::removeCloud(cuint ui32Index)
{
    if(ui32Index < static_cast<uint>(m_vClouds.size()))
    {
        m_vClouds.removeAt(ui32Index);
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
        m_vMeshes.removeAt(ui32Index);
        m_vBMeshLinesRender.removeAt(ui32Index);
    }
    else
    {
        std::cerr << "-ERROR : bad index removeMesh " << std::endl;
    }
}



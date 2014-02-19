

/**
 * \file SWGLMeshWidget.cpp
 * \brief Defines SWGLMeshWidget
 * \author Florian Lance
 * \date 11/07/13
 */


#include "SWGLMeshWidget.h"
#include <iostream>

#include "moc_SWGLMeshWidget.cpp"

#include "interface/SWGLUtility.h"


SWGLMeshWidget::SWGLMeshWidget(QGLContext *context, QWidget* parent, const QString &sVertexShaderPath, const QString &sFragmentShaderPath) :
    SWGLWidget(context, parent), m_sVertexShaderPath(sVertexShaderPath), m_sFragmentShaderPath(sFragmentShaderPath), m_pMesh(NULL),
    m_vertexBuffer(QGLBuffer::VertexBuffer), m_indexBuffer(QGLBuffer::IndexBuffer),  m_normalBuffer(QGLBuffer::VertexBuffer), m_textureBuffer(QGLBuffer::VertexBuffer),
    m_bInitCamWithCloudPosition(true), m_bLinesRender(false), m_bApplyTexture(false)
{}

SWGLMeshWidget::~SWGLMeshWidget()
{}

void SWGLMeshWidget::initializeGL()
{
    // set background
        qglClearColor(QColor(49, 53, 70));

    // init shaders
        initShaders("../data/shaders/cloudAvatar.vert", "../data/shaders/cloudAvatar.frag", m_oShaderLines, false);
        initShaders("../data/shaders/meshAvatar.vert", "../data/shaders/meshAvatar.frag", m_oShaderMesh,  true);

    // enable depth buffer
        glEnable(GL_DEPTH_TEST);

    // enable texture
        glEnable(GL_TEXTURE_2D);

    // init buffers
        initMeshBuffers();
}


void SWGLMeshWidget::paintGL()
{
    // set the size point
        glPointSize(m_glFSizePoint);

    // clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // activate texture
        if(m_bApplyTexture)
        {
            qDebug() << "bind : " << m_textureLocation;
            glBindTexture(GL_TEXTURE_2D, m_textureLocation);
        }

    // calculate model view transformation
        QMatrix4x4 l_oViewMatrix;
        l_oViewMatrix.setToIdentity();

    // set camera vue
        l_oViewMatrix.lookAt( m_pCamera->eyePosition(), m_pCamera->viewDirection(), m_pCamera->up());

    // comput MVP matrix
        QMatrix4x4 l_oModelMatrix;
        l_oModelMatrix.setToIdentity();
        m_oMVPMatrix = l_oModelMatrix  * m_oProjectionMatrix * l_oViewMatrix;

    // draw
        if(m_pMesh)
        {
            drawMesh();
        }

        drawAxes(m_oShaderMesh, m_oMVPMatrix, 0.02f);
}

void SWGLMeshWidget::setTexture(const QImage &oTexture)
{
    m_oParamMutex.lockForWrite();
        m_oTexture = oTexture;
        m_textureLocation = bindTexture(m_oTexture);
    m_oParamMutex.unlock();
}

void SWGLMeshWidget::setTexture(const QString &sTexturePath)
{
    m_oParamMutex.lockForWrite();
        m_textureLocation = bindTexture(QImage(sTexturePath));
    m_oParamMutex.unlock();
}

void SWGLMeshWidget::applyTexture(const bool bApplyTexture)
{
    m_oParamMutex.lockForWrite();
        m_bApplyTexture = bApplyTexture;
        qDebug() << "m_bApplyTexture : " << m_bApplyTexture;
    m_oParamMutex.unlock();
    updateGL();
}

void SWGLMeshWidget::setMeshLinesRender(const bool bRenderLines)
{
    m_oParamMutex.lockForWrite();
        m_bLinesRender = bRenderLines;
    m_oParamMutex.unlock();
    updateGL();
}

void SWGLMeshWidget::setMesh(swMesh::SWMesh *pMesh)
{
    if(pMesh)
    {
        if(pMesh->trianglesNumber() < 1)
        {
            return;
        }
    }
    else
    {
        return;
    }

    if(m_bInitCamWithCloudPosition)
    {
        swCloud::SWCloudBBox l_oBBox = pMesh->cloud()->bBox();
        QVector3D l_oEye,l_oLookAt;
        l_oEye.setX((l_oBBox.m_fMaxX + l_oBBox.m_fMinX)/2);
        l_oEye.setY((l_oBBox.m_fMaxY + l_oBBox.m_fMinY)/2);
        l_oEye.setZ((l_oBBox.m_fMaxZ + l_oBBox.m_fMinZ)/2);

        l_oLookAt = l_oEye;
        l_oEye.setZ(l_oEye.z() - 0.25f);
        l_oLookAt.setZ(l_oLookAt.z() + 1.f);

        resetCamera(l_oEye,l_oLookAt);

        m_bInitCamWithCloudPosition = false;
    }

    deleteAndNullify(m_pMesh);
    m_pMesh = new swMesh::SWMesh(*pMesh);
    updateGL();
}


void SWGLMeshWidget::setMesh(swMesh::SWMesh &oMesh)
{
    m_pMesh = &oMesh;
    updateGL();
}


void SWGLMeshWidget::initMeshBuffers()
{
    // create the buffer
        m_vertexBuffer.create();
        m_indexBuffer.create();
        m_normalBuffer.create();
        m_textureBuffer.create();

    // define the usage pattern
        m_vertexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
        m_indexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
        m_normalBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
        m_textureBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
}

void SWGLMeshWidget::drawMesh()
{
    // bind shader
    if(!m_oShaderMesh.bind())
    {
         throw swExcept::swShaderGLError();
    }

    // set mode
    m_oParamMutex.lockForRead();
    if(m_bLinesRender)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    m_oParamMutex.unlock();


    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    float  *l_aFVertexBuffer   = m_pMesh->vertexBuffer();
    uint32 *l_aUI32IndexBuffer = m_pMesh->indexVertexTriangleBuffer();
    float  *l_aFNormalBuffer   = m_pMesh->normalBuffer();
    float  *l_aFTextureBuffer  = m_pMesh->textureBuffer();

    // allocate QGL buffers
    allocateBuffer(m_vertexBuffer,  l_aFVertexBuffer,   m_pMesh->pointsNumber() *     3 * sizeof(float) );
    allocateBuffer(m_indexBuffer,   l_aUI32IndexBuffer, m_pMesh->trianglesNumber() *  3 * sizeof(GLuint) );
    allocateBuffer(m_normalBuffer,  l_aFNormalBuffer,   m_pMesh->pointsNumber() *     3 * sizeof(float) );
    allocateBuffer(m_textureBuffer, l_aFTextureBuffer,  m_pMesh->pointsNumber() *     2 * sizeof(float) );

    m_oShaderMesh.setUniformValue("mvpMatrix", m_oMVPMatrix);

    qDebug() << "m_textureBuffer " << m_textureBuffer.size();
    drawBufferWithTexture(m_indexBuffer, m_vertexBuffer, m_textureBuffer, m_normalBuffer, m_oShaderMesh, GL_TRIANGLES);

    delete[] l_aFVertexBuffer;
    delete[] l_aUI32IndexBuffer;
    delete[] l_aFNormalBuffer;
    delete[] l_aFTextureBuffer;
}

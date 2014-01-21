

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
    m_vertexBuffer(QGLBuffer::VertexBuffer), m_indexBuffer(QGLBuffer::IndexBuffer),  m_normalBuffer(QGLBuffer::VertexBuffer), m_textureBuffer(QGLBuffer::VertexBuffer)
{}

SWGLMeshWidget::~SWGLMeshWidget()
{}

void SWGLMeshWidget::initializeGL()
{
    // qglClearColor(Qt::black);
    qglClearColor(QColor(49, 53, 70));

    // init shaders
    initShaders("../data/shaders/cloudAvatar.vert", "../data/shaders/cloudAvatar.frag", m_oShaderLines,     false);
    initShaders(m_sVertexShaderPath, m_sFragmentShaderPath, m_oShaderMesh, true);

    qWarning() << m_sVertexShaderPath << " " << m_sFragmentShaderPath;
//    std::cout << "m_sVertexShaderPath : " << m_sVertexShaderPath << " " << m_sFragmentShaderPath << std::endl;

    // enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // enable back face culling
    glEnable(GL_CULL_FACE);

    // init geometry
    initMeshBuffers();

    // init texture
    QImage l_oTexture("../data/textures/avatars/stel_skin3.jpg");
    qDebug() << "Size texture loaded : " << l_oTexture.size();
    m_textureLocation = bindTexture(l_oTexture);


    // using QBasicTimer because its faster that QTimer
//    m_oTimer->start(5, this);
}


void SWGLMeshWidget::paintGL()
{
    // set the size point
    glPointSize(m_glFSizePoint);

    // clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // activate texture
    glBindTexture(GL_TEXTURE_2D, m_textureLocation);

    // calculate model view transformation
    QMatrix4x4 l_oViewMatrix;
    l_oViewMatrix.setToIdentity();

    // set camera vue
    l_oViewMatrix.lookAt( m_pCamera->eyePosition(), m_pCamera->viewDirection(), m_pCamera->up());

    // comput MVP matrix
    QMatrix4x4 l_oModelMatrix;
    l_oModelMatrix.setToIdentity();
    m_oMVPMatrix = l_oModelMatrix  * m_oProjectionMatrix * l_oViewMatrix;

    if(m_pMesh)
    {
        drawMesh();
    }

    drawAxes(m_oShaderLines, m_oMVPMatrix, 0.02f);
}


void SWGLMeshWidget::setTexture(const QString &sTexturePath)
{
    m_textureLocation = bindTexture(QImage(sTexturePath));
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
        // TODO : throw ...
    }

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
    drawBufferWithTexture(m_indexBuffer, m_vertexBuffer, m_textureBuffer, m_normalBuffer, m_oShaderMesh, GL_TRIANGLES); //

    delete[] l_aFVertexBuffer;
    delete[] l_aUI32IndexBuffer;
    delete[] l_aFNormalBuffer;
    delete[] l_aFTextureBuffer;
}


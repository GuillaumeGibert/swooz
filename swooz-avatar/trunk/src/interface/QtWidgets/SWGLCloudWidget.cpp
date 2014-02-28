

/**
 * \file SWGLCloudWidget.cpp
 * \brief Defines SWGLCloudWidget
 * \author Florian Lance
 * \date 11/07/13
 */

#include "SWGLCloudWidget.h"

#include "moc_SWGLCloudWidget.cpp"

#include "interface/SWGLUtility.h"

using namespace swExcept;

SWGLCloudWidget::SWGLCloudWidget(QGLContext *context, QWidget* parent) :
    SWGLWidget(context, parent), m_bInitCamWithCloudPosition(true),
    m_vertexBuffer(QGLBuffer::VertexBuffer), m_colorBuffer(QGLBuffer::VertexBuffer), m_indexBuffer(QGLBuffer::IndexBuffer), m_pCloud(NULL)
{
    m_fDefaultOpacity = 1.f;   
    m_fDepthRect = -1.f;
}

SWGLCloudWidget::~SWGLCloudWidget()
{
    deleteAndNullify(m_pCloud);
}

void SWGLCloudWidget::initializeGL()
{
    // set perspective
        m_rZNear = 0.01;
        m_rZFar  = 100.0;
        m_rFOV   = 40.0;

    // set background
        qglClearColor(QColor(49, 53, 70));

    // init shaders
        initShaders("../data/shaders/cloudAvatar.vert", "../data/shaders/cloudAvatar.frag", m_oShader,    false);

    // set options
    //  depth buffer
        glEnable(GL_DEPTH_TEST);
    //  transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //  line smoothing
        glEnable(GL_LINE_SMOOTH);
    //  back face culling
        glDisable(GL_CULL_FACE);

    // init QGL buffers
        initCloudBuffers();

    // start timer
        m_oTimer->start(1000/30, this);
}

void SWGLCloudWidget::paintGL()
{
    // clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // calculate model view transformation
    QMatrix4x4 l_oViewMatrix;
    l_oViewMatrix.setToIdentity();

    // set camera vue
    l_oViewMatrix.lookAt( m_pCamera->eyePosition(), m_pCamera->viewDirection(), m_pCamera->up());

    // comput MVP matrix
    QMatrix4x4 l_oModelMatrix;
    l_oModelMatrix.setToIdentity();
    m_oMVPMatrix = l_oModelMatrix  * m_oProjectionMatrix * l_oViewMatrix;

    // draw geometry
    try
    {
        if(m_pCloud)
        {
            if(m_pCloud->size() > 0)
            {
                drawCloud(m_oShader, *m_pCloud, m_glFSizePoint, m_oMVPMatrix);

                swCloud::SWCloudBBox l_oBBox = m_pCloud->bBox();
                drawDepthRect(m_oShader, m_oMVPMatrix, l_oBBox.m_fMinZ + m_fDepthRect);
            }
        }
    }
    catch(const openglError &e)
    {
        qWarning() << "drawCloud : " << e.what();
    }
}

void SWGLCloudWidget::setCloud(swCloud::SWCloud *oCloud)
{
    if(oCloud)
    {
        if(oCloud->size() == 0)
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
        swCloud::SWCloudBBox l_oBBox = oCloud->bBox();
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


    deleteAndNullify(m_pCloud);
    m_pCloud = new swCloud::SWCloud(*oCloud);

    if(oCloud->size() > 0 && m_fDepthRect > 0.f)
    {
        m_oCloudBBox = oCloud->bBox();
        m_oCloudBBox.m_fMaxZ =  m_oCloudBBox.m_fMinZ + m_fDepthRect;
        m_oCloudBBox.m_fMinZ -= 0.1f;

        m_pCloud->keepOnlyPointInsideBBox(m_oCloudBBox);
    }

    updateGL();
}

void SWGLCloudWidget::initCloudBuffers()
{
    // create the buffer
    m_vertexBuffer.create();
    m_indexBuffer.create();
    m_colorBuffer.create();
}


void SWGLCloudWidget::drawDepthRect(QGLShaderProgram &oShader, QMatrix4x4 &mvpMatrix, cfloat fDepth)
{
    // bind shader
    if(!oShader.bind())
    {
        throw swShaderGLError();
    }

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    int l_i32SizeIndex      = sizeof(GLuint);
    int l_i32SizeVertex     = sizeof(float) * 3;
    uint32 *l_aDepthRectI   = new uint32[6];
    float  *l_aDepthRectV   = new float[3*4];

    // defines points    
    l_aDepthRectV[2] = l_aDepthRectV[5] = l_aDepthRectV[8] = l_aDepthRectV[11] = fDepth;
    l_aDepthRectV[0] = m_oCloudBBox.m_fMinX; l_aDepthRectV[3] = m_oCloudBBox.m_fMinX; l_aDepthRectV[6] = m_oCloudBBox.m_fMaxX; l_aDepthRectV[9]  = m_oCloudBBox.m_fMaxX;
    l_aDepthRectV[1] = m_oCloudBBox.m_fMaxY; l_aDepthRectV[4] = m_oCloudBBox.m_fMinY; l_aDepthRectV[7] = m_oCloudBBox.m_fMinY; l_aDepthRectV[10] = m_oCloudBBox.m_fMaxY;
    // define index
    l_aDepthRectI[0] = 0;
    l_aDepthRectI[1] = 1;
    l_aDepthRectI[2] = 2;
    l_aDepthRectI[3] = 2;
    l_aDepthRectI[4] = 3;
    l_aDepthRectI[5] = 0;

    // allocate
    allocateBuffer(m_indexBuffer,  l_aDepthRectI, 6 * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aDepthRectV, 4 * l_i32SizeVertex);

    // set uniform values parameters
    oShader.setUniformValue("uniColor", 22, 39, 51);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", 0.8f);

    // draw primitives
    GLenum l_glError = drawBufferWithColor(m_indexBuffer, m_vertexBuffer, m_colorBuffer, oShader, GL_TRIANGLES);

    delete[] l_aDepthRectI;
    delete[] l_aDepthRectV;

    if(l_glError)
    {
        qWarning() << "drawDepthRect GLError : " << l_glError;
    }
}

void SWGLCloudWidget::setDepthRect(const double dDepth)
{
    m_fDepthRect = (float)dDepth;
}

void SWGLCloudWidget::drawCloud(QGLShaderProgram &oShader, const swCloud::SWCloud &oCloud, cfloat fSizePoint, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    // bind shader
    if(!oShader.bind())
    {
        throw swShaderGLError();
    }

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    int l_i32SizeIndex      = sizeof(GLuint);
    int l_i32SizeVertex     = sizeof(float) * 3;
    uint32 *l_aCloudI       = oCloud.indexBuffer();
    float  *l_aCloudV       = oCloud.vertexBuffer();
    float  *l_aCloudC       = oCloud.colorBuffer();

    allocateBuffer(m_indexBuffer,  l_aCloudI, oCloud.size() * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aCloudV, oCloud.size() * l_i32SizeVertex);
    allocateBuffer(m_colorBuffer  ,l_aCloudC, oCloud.size() * l_i32SizeVertex);

    // set size of the points
    glPointSize(fSizePoint);

    // set uniform values parameters
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    GLenum l_glError = drawBufferWithColor(m_indexBuffer, m_vertexBuffer, m_colorBuffer, oShader, GL_POINTS);

    delete[] l_aCloudI;
    delete[] l_aCloudV;
    delete[] l_aCloudC;

    if(l_glError)
    {
        qWarning() << "drawCloud GLError : " << l_glError;
    }
}

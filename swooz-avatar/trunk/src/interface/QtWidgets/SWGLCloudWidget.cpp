

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
    SWGLWidget(context, parent), m_bApplyRigidMotion(false), m_bInitCamWithCloudPosition(true),
    m_vertexBuffer(QGLBuffer::VertexBuffer), m_colorBuffer(QGLBuffer::VertexBuffer), m_indexBuffer(QGLBuffer::IndexBuffer), m_pCloud(NULL)
{
    // init rigid motion parameters
    m_oRigidMotion.setToIdentity();

    m_fDefaultOpacity = 1.f;
}

SWGLCloudWidget::~SWGLCloudWidget()
{
    deleteAndNullify(m_pCloud);
}

void SWGLCloudWidget::initializeGL()
{

    // qglClearColor(Qt::black);
    qglClearColor(QColor(49, 53, 70));

    // init shaders
    initShaders("../data/shaders/cloudAvatar.vert", "../data/shaders/cloudAvatar.frag", m_oShader,    false);

// enable depth buffer
    glEnable(GL_DEPTH_TEST);

// set transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);

    // enable back face culling
    glDisable(GL_CULL_FACE);

    // init QGL buffers
    initCloudBuffers();

    // using QBasicTimer because its faster that QTimer
    m_oTimer->start(1000/30, this);
}

void SWGLCloudWidget::paintGL()
{
    // set the size point
    glPointSize(m_glFSizePoint);

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

    if(m_bApplyRigidMotion)
    {
        m_oMVPMatrix *= m_oRigidMotion;
    }

    // draw geometry
    try
    {
        if(m_pCloud)
        {
            drawCloud(m_oShader, *m_pCloud, 3.f, m_oMVPMatrix);
        }
    }
    catch(const openglError &e)
    {
        qWarning() << "drawCloud : " << e.what();
    }
}

void SWGLCloudWidget::setCloudAndRigidMotion(const swCloud::SWRigidMotion &oRigidMotion, swCloud::SWCloud *oCloud)
{
    QMatrix4x4 l_oQMatRigidMotion;

    l_oQMatRigidMotion.setToIdentity();

    l_oQMatRigidMotion(0,0) = oRigidMotion.m_aFRotation[0];
    l_oQMatRigidMotion(0,1) = oRigidMotion.m_aFRotation[1];
    l_oQMatRigidMotion(0,2) = oRigidMotion.m_aFRotation[2];
    l_oQMatRigidMotion(0,3) = oRigidMotion.m_aFTranslation[0];

    l_oQMatRigidMotion(1,0) = oRigidMotion.m_aFRotation[3];
    l_oQMatRigidMotion(1,1) = oRigidMotion.m_aFRotation[4];
    l_oQMatRigidMotion(1,2) = oRigidMotion.m_aFRotation[5];
    l_oQMatRigidMotion(1,3) = oRigidMotion.m_aFTranslation[1];

    l_oQMatRigidMotion(2,0) = oRigidMotion.m_aFRotation[6];
    l_oQMatRigidMotion(2,1) = oRigidMotion.m_aFRotation[7];
    l_oQMatRigidMotion(2,2) = oRigidMotion.m_aFRotation[8];
    l_oQMatRigidMotion(2,3) = oRigidMotion.m_aFTranslation[2];

    l_oQMatRigidMotion(3,0) = 0;
    l_oQMatRigidMotion(3,1) = 0;
    l_oQMatRigidMotion(3,2) = 0;
    l_oQMatRigidMotion(3,3) = 1;

//    m_bApplyRigidMotion = true;

    m_oRigidMotion    = l_oQMatRigidMotion;
    m_pCloud          = oCloud;

    updateGL();
}

void SWGLCloudWidget::setCloud(swCloud::SWCloud *oCloud)
{
    if(m_bInitCamWithCloudPosition)
    {
        swCloud::SWCloudBBox l_oBBox = oCloud->bBox();
        QVector3D l_oEye,l_oLookAt;
        l_oEye.setX((l_oBBox.m_fMaxX + l_oBBox.m_fMinX)/2);
        l_oEye.setY((l_oBBox.m_fMaxY + l_oBBox.m_fMinY)/2);
        l_oEye.setZ((l_oBBox.m_fMaxZ + l_oBBox.m_fMinZ)/2);

        l_oLookAt = l_oEye;
        l_oEye.setZ(l_oEye.z() - 0.15f);
        l_oLookAt.setZ(l_oLookAt.z() + 1.f);

        resetCamera(l_oEye,l_oLookAt);

        m_bInitCamWithCloudPosition = false;
    }


    deleteAndNullify(m_pCloud);
    m_pCloud = new swCloud::SWCloud(*oCloud);
    updateGL();
}

void SWGLCloudWidget::initCloudBuffers()
{
    // create the buffer
    m_vertexBuffer.create();
    m_indexBuffer.create();
    m_colorBuffer.create();

    // define the usage pattern (DynamicDraw : The data will be modified repeatedly and used many times for drawing operations. )
//    m_vertexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
//    m_indexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
//    m_colorBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
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

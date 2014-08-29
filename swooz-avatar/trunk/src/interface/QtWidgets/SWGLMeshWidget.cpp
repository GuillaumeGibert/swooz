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

#include <QGLFunctions>

SWGLMeshWidget::SWGLMeshWidget(QGLContext *context, QWidget* parent, const QString &sVertexShaderPath, const QString &sFragmentShaderPath) :
    SWGLWidget(context, parent), m_sVertexShaderPath(sVertexShaderPath), m_sFragmentShaderPath(sFragmentShaderPath), m_pMesh(NULL),
    m_bLinesRender(false), m_bApplyTexture(false), m_bNewMesh(false)
{}


SWGLMeshWidget::~SWGLMeshWidget()
{}

void SWGLMeshWidget::initializeGL()
{
    // set perspective
        m_rZNear = 0.01;
        m_rZFar = 100.0;
        m_rFOV = 40.0;

    // set background
        qglClearColor(QColor(49, 53, 70));

    // init shaders
        if(m_sVertexShaderPath != "" && m_sFragmentShaderPath != "")
        {
            initShaders(m_sVertexShaderPath, m_sFragmentShaderPath, m_oShaderMesh, true);
        }
        else
        {
            initShaders("../data/shaders/meshAvatar.vert", "../data/shaders/meshAvatar.frag", m_oShaderMesh, true);
        }

        initShaders("../data/shaders/cloudAvatar.vert", "../data/shaders/cloudAvatar.frag", m_oShaderLines, false);


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

    // create texture
        if(m_bBindTexture)
        {
            m_bBindTexture = false;
            m_textureLocation = bindTexture(m_oTexture);
        }

    // calculate model view transformation
        QMatrix4x4 l_oViewMatrix;
        l_oViewMatrix.setToIdentity();

    // set camera vue
        l_oViewMatrix.lookAt( m_pCamera->eyePosition(), m_pCamera->lookAt(), m_pCamera->up());

    // comput MVP matrix
        QMatrix4x4 l_oModelMatrix;
        l_oModelMatrix.setToIdentity();
        m_oMVPMatrix = l_oModelMatrix * m_oProjectionMatrix * l_oViewMatrix;

    // draw
        if(m_pMesh)
        {
            drawMesh();
        }

    drawAxes(m_oShaderLines, m_oMVPMatrix, 0.02f);
}

void SWGLMeshWidget::setTexture(const QImage &oTexture)
{
    m_oParamMutex.lockForWrite();
        m_oTexture = oTexture;
        m_bBindTexture = true;
    m_oParamMutex.unlock();
}

void SWGLMeshWidget::setTexture(const QString &sTexturePath)
{
    m_oParamMutex.lockForWrite();
        m_textureLocation = bindTexture(QImage(sTexturePath));
        m_bBindTexture = true;
    m_oParamMutex.unlock();
}

void SWGLMeshWidget::applyTexture(const bool bApplyTexture)
{
    m_oParamMutex.lockForWrite();
        m_bApplyTexture = bApplyTexture;
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

void SWGLMeshWidget::setMesh(swMesh::SWMesh *pMesh, bool bResetCamera)
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

    if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA && bResetCamera)
    {
        std::vector<float> l_v3FMean = pMesh->cloud()->meanPoint();
        QVector3D l_oEye,l_oLookAt;
        l_oLookAt.setX(l_v3FMean[0]);
        l_oLookAt.setY(l_v3FMean[1]);
        l_oLookAt.setZ(l_v3FMean[2]);
        l_oEye = l_oLookAt;
        l_oEye.setZ(l_oEye.z() + 0.25f);

        setCamera(l_oEye,l_oLookAt);
        setCameraInitial(l_oEye,l_oLookAt, QVector3D(0,1,0));
    }
    else if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA && bResetCamera)
    {
        swCloud::SWCloudBBox l_oBBox = pMesh->cloud()->bBox();
        QVector3D l_oEye,l_oLookAt;
        l_oEye.setX((l_oBBox.m_fMaxX + l_oBBox.m_fMinX)/2);
        l_oEye.setY((l_oBBox.m_fMaxY + l_oBBox.m_fMinY)/2);
        l_oEye.setZ((l_oBBox.m_fMaxZ + l_oBBox.m_fMinZ)/2);
        l_oLookAt = l_oEye;
        l_oEye.setZ(l_oEye.z() + 0.25f);
        l_oLookAt.setZ(l_oLookAt.z() + 1.f);

        setCamera(l_oEye,l_oLookAt);
        setCameraInitial(l_oEye,l_oLookAt, QVector3D(0,1,0));
    }

    deleteAndNullify(m_pMesh);
    m_pMesh = new swMesh::SWMesh(*pMesh);
    m_bNewMesh = true;

    updateGL();
}

void SWGLMeshWidget::initMeshBuffers()
{
    // init
        initIndexBuffer(m_indexBufferMesh);
        initVertexBuffer(m_vertexBufferMesh);
        initVertexBuffer(m_normalBufferMesh);
        initVertexBuffer(m_textureBufferMesh);

    // define the usage pattern
        m_vertexBufferMesh.setUsagePattern(QGLBuffer::StaticDraw);
        m_indexBufferMesh.setUsagePattern(QGLBuffer::StaticDraw);
        m_normalBufferMesh.setUsagePattern(QGLBuffer::StaticDraw);
        m_textureBufferMesh.setUsagePattern(QGLBuffer::StaticDraw);
}

void SWGLMeshWidget::drawMesh()
{
    m_oShaderMesh.bind();
        checkGlError();

    // bind texture
        if(m_bApplyTexture)
        {
            glBindTexture(GL_TEXTURE_2D, m_textureLocation);
                checkGlError();
        }

    // set mode
    m_oParamMutex.lockForRead();
        if(m_bLinesRender)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                checkGlError();
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                checkGlError();
        }
    m_oParamMutex.unlock();

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    if(m_bNewMesh)
    {
        float *l_aFVertexBuffer = m_pMesh->vertexBuffer();
        uint32 *l_aUI32IndexBuffer = m_pMesh->indexVertexTriangleBuffer();
        float *l_aFNormalBuffer = m_pMesh->normalBuffer();
        float *l_aFTextureBuffer = m_pMesh->textureBuffer();

        // allocate QGL buffers
        allocateBuffer(m_vertexBufferMesh, l_aFVertexBuffer, m_pMesh->pointsNumber() * 3 * sizeof(float) );
        allocateBuffer(m_indexBufferMesh, l_aUI32IndexBuffer, m_pMesh->trianglesNumber() * 3 * sizeof(GLuint) );
        allocateBuffer(m_normalBufferMesh, l_aFNormalBuffer, m_pMesh->pointsNumber() * 3 * sizeof(float) );
        allocateBuffer(m_textureBufferMesh, l_aFTextureBuffer, m_pMesh->pointsNumber() * 2 * sizeof(float) );

        deleteAndNullifyArray(l_aFVertexBuffer);
        deleteAndNullifyArray(l_aUI32IndexBuffer);
        deleteAndNullifyArray(l_aFNormalBuffer);
        deleteAndNullifyArray(l_aFTextureBuffer);

        m_bNewMesh = false;
    }

    m_oShaderMesh.setUniformValue("mvpMatrix", m_oMVPMatrix);

    int l_i32DisplayMode = 0;

    if(m_bApplyTexture)
    {
        l_i32DisplayMode = 2;
    }

    m_oShaderMesh.setUniformValue("displayMode", l_i32DisplayMode);

    m_oShaderMesh.setUniformValue("viewDirection", -m_pCamera->lookAt());


    drawBufferWithTexture(m_indexBufferMesh, m_vertexBufferMesh, m_textureBufferMesh, m_normalBufferMesh, m_oShaderMesh, GL_TRIANGLES);

    m_oShaderMesh.release();
        checkGlError();
}

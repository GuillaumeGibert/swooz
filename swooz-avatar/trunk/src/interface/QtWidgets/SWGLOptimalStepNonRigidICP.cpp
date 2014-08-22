



/**
 * \file SWGLOptimalStepNonRigidICP.cpp
 * \brief Defines SWGLOptimalStepNonRigidICP
 * \author Florian Lance
 * \date 26/09/13
 */

#include "SWGLOptimalStepNonRigidICP.h"
#include "geometryUtility.h"

#include "moc_SWGLOptimalStepNonRigidICP.cpp"



using namespace std;
using namespace swMesh;
using namespace swExcept;

#include "time.h"

#include <QtGui>


SWGLOptimalStepNonRigidICP::SWGLOptimalStepNonRigidICP(QGLContext *context, QWidget* parent) : SWGLWidget(context, parent),
     m_vertexBuffer(QGLBuffer::VertexBuffer), m_indexBuffer(QGLBuffer::IndexBuffer), m_colorBuffer(QGLBuffer::VertexBuffer),
     m_normalBuffer(QGLBuffer::VertexBuffer), m_textureBuffer(QGLBuffer::VertexBuffer)
{
    // init manual landmarks parameters
        m_idClickedPointOnSourceMesh = -1;
        m_idClickedPointOnTargetMesh = -1;
        m_targetTextureLocation = -1;
        m_bTemplateMeshPointSelection = true;

        m_infoDisplay3D = tr("No meshes loaded. Set the template mesh and the target mesh. ");

        m_clickedPointOnSourceMesh = QVector3D(-1000,-1000,-1000);
        m_clickedPointOnTargetMesh = QVector3D(-1000,-1000,-1000);

    // set default parameters values
        // display
            m_bMeshSDisplay = m_bMeshTDisplay = m_bCorrDisplay = m_bDisplayLandMarks = true;
            m_bPointsTDisplay = m_bPointsSDisplay = m_bVerticesNormalsSDisplay = m_bTrianglesNormalsSDisplay = m_bVerticesNormalsTDisplay  = m_bTrianglesNormalsTDisplay = false;
            m_bFillS = m_bFillT = false;

        // OptimalStepNonRigidICP
            m_bUseLandMarks = true;

        // translations
            m_fXTransTarget = m_fYTransTarget = m_fZTransTarget = 0.f;
        // rotations
            m_fXRotTarget = m_fYRotTarget = m_fZRotTarget = 0.f;
        // scaling
            m_fTargetScaling = 1.f;//0.f;

        // paths files
            m_sPathSourceMesh   = "";
            m_sPathTargetMesh   = "";
            m_sPathStasmSource  = "";
            m_sPathStasmTarget  = "";

    // set parameters values
        // display
            m_fDefaultOpacity         = 1.f;
            m_fOpacitySourceMeshLines = 1.f;
            m_fOpacityTargetMeshLines = 0.3f;
            m_fOpacitySourceMesh      = 0.5f;
            m_fOpacityTargetMesh      = 0.5f;

    // init mutex
        m_pParamMutex       = new QMutex();
        m_pSourceMeshMutex  = new QReadWriteLock();
        m_pTargetMeshMutex  = new QReadWriteLock();
        m_pUMutex           = new QReadWriteLock();
        m_pWMutex           = new QReadWriteLock();

    // udpate display
//        updateGL();
        setAttribute(Qt::WA_OpaquePaintEvent);
        setAutoFillBackground(false);
        update();
}


SWGLOptimalStepNonRigidICP::~SWGLOptimalStepNonRigidICP()
{
    deleteAndNullify(m_pParamMutex);
    deleteAndNullify(m_pSourceMeshMutex);
    deleteAndNullify(m_pTargetMeshMutex);
    deleteAndNullify(m_pUMutex);
    deleteAndNullify(m_pWMutex);
}

double SWGLOptimalStepNonRigidICP::startAlpha() const
{
    QMutexLocker l_oParamLocker(m_pParamMutex);
    return m_dStartAlpha;
}

double SWGLOptimalStepNonRigidICP::minAlpha() const
{
    QMutexLocker l_oParamLocker(m_pParamMutex);
    return m_dMinAlpha;
}

double SWGLOptimalStepNonRigidICP::diffMax() const
{
    QMutexLocker l_oParamLocker(m_pParamMutex);
    return m_dDiffMax;
}

double SWGLOptimalStepNonRigidICP::coeffAlpha() const
{
    QMutexLocker l_oParamLocker(m_pParamMutex);
    return m_dCoeffAlpha;
}

void SWGLOptimalStepNonRigidICP::alignWithNose(swMesh::SWMesh &oSourceMesh, swMesh::SWMesh &oTargetMesh)
{
    swCloud::SWCloud *l_oSource = oSourceMesh.cloud(), *l_oTarget = oTargetMesh.cloud();

    std::vector<float> l_A3FSourceMeanPoint = l_oSource->meanPoint();
    vector<float> l_A3FTargetMeanPoint = l_oTarget->meanPoint();

    l_A3FSourceMeanPoint[2] -= 10.f;
    l_A3FTargetMeanPoint[2] -= 10.f;
    int idCloseSource = l_oSource->idNearestPoint(l_A3FSourceMeanPoint);
    int idCloseTarget = l_oTarget->idNearestPoint(l_A3FTargetMeanPoint);

    std::vector<float> l_A3FOffset;
    l_A3FOffset.push_back(l_oSource->coord(0)[idCloseSource] - l_oTarget->coord(0)[idCloseTarget]);
    l_A3FOffset.push_back(l_oSource->coord(1)[idCloseSource] - l_oTarget->coord(1)[idCloseTarget]);
    l_A3FOffset.push_back(l_oSource->coord(2)[idCloseSource] - l_oTarget->coord(2)[idCloseTarget]);

    (*l_oTarget) += l_A3FOffset;
}


void SWGLOptimalStepNonRigidICP::initResolve()
{
    m_pSourceMeshMutex->lockForWrite();
        m_pOSNRICP->updateSourceMeshNormals();
    m_pSourceMeshMutex->unlock();

    m_pSourceMeshMutex->lockForRead();
    m_pTargetMeshMutex->lockForRead();
        m_pUMutex->lockForWrite();
            m_pOSNRICP->computeCorrespondences();
        m_pUMutex->unlock();
        m_pWMutex->lockForWrite();
            m_pOSNRICP->computeDistanceWeights();
        m_pWMutex->unlock();
    m_pSourceMeshMutex->unlock();
    m_pTargetMeshMutex->unlock();
}


double SWGLOptimalStepNonRigidICP::morph(cdouble dAlpha)
{
//    clock_t l_oProgramTime = clock();
//    qDebug() << "morph -> " << dAlpha;

    if(!m_pOSNRICP)
    {
        qWarning() << "Set meshes before starting morphing. ";
        return -1.0;
    }

    // init current params
    m_pParamMutex->lock();
    double l_dBeta        = m_dBeta;
    double l_dGama        = m_dGama;
    double l_dUseLandmarks= m_bUseLandMarks;

//    qDebug() << "Start alpha : " << m_dStartAlpha << "\nAlpha : " << dAlpha << "\nBeta : " << m_dBeta << "\nGama : " << m_dGama << "\nUse landmarks : " << m_bUseLandMarks;
//    qDebug() << "Min Alpha : " << m_dMinAlpha << "\nCoeff : " << m_dCoeffAlpha;

    m_pParamMutex->unlock();

    initResolve();
    double l_dDiff;

    m_infoDisplay3D = QString("Morphing in progress... (Alpha  : " + QString::number(dAlpha) + " -> " + QString::number(m_dMinAlpha) +  " Beta : " + QString::number(l_dBeta) + " Gama : " + QString::number(l_dGama) + ")");
    update();

    try
    {
        l_dDiff = m_pOSNRICP->resolve(static_cast<float>(dAlpha), static_cast<float>(l_dBeta), static_cast<float>(l_dGama), l_dUseLandmarks);
    }
    catch (const cv::Exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

//    qDebug() << " end morph ->  " << dAlpha << " time : " << ((float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC);

    return l_dDiff;
}

void SWGLOptimalStepNonRigidICP::saveCurrentMeshToObj(const QString &sPath)
{           
    int l_i32SeparatorsNb = 0;
    QString l_sName(sPath);
    for(int ii = 0; ii < sPath.size(); ++ii)
    {
        if(sPath[ii] == '/' || sPath[ii] == '\\')
        {
            ++l_i32SeparatorsNb;
        }
    }

    int ii = 0;
    while(ii < l_i32SeparatorsNb)
    {
        if(l_sName[0] == '/' || l_sName[0] == '\\')
        {
            ++ii;
        }

        l_sName.remove(0, 1);
    }

    QString l_sNameOBJ(l_sName);
    l_sName.remove(l_sName.size()-3, l_sName.size());
    l_sName.append("mtl");
    QString l_sNameMTL(l_sName);

    QString l_sPath(sPath);
    l_sPath.remove(l_sPath.size() - l_sNameOBJ.size(), l_sNameOBJ.size());

    qDebug() << "Path : " << l_sPath.toUtf8().constData();
    qDebug() << "Save OBJ file : " << l_sNameOBJ.toUtf8().constData();
    qDebug() << "Save Material file : " << l_sNameMTL.toUtf8().constData();

    m_pOSNRICP->m_oSourceMesh.saveToObj(l_sPath.toUtf8().constData(), l_sNameOBJ.toUtf8().constData(), l_sNameMTL.toUtf8().constData());
}

void SWGLOptimalStepNonRigidICP::setCloudSDisplay(bool bVal)
{
    m_bPointsSDisplay = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setCloudTDisplay(bool bVal)
{
    m_bPointsTDisplay = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setMeshSDisplay(bool bVal)
{
    m_bMeshSDisplay = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setMeshTDisplay(bool bVal)
{
    m_bMeshTDisplay = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setUseLandMarks(bool bVal)
{
    m_bUseLandMarks = bVal;
}

void SWGLOptimalStepNonRigidICP::setVerticesNormalsSDisplay(bool bVal)
{
    m_bVerticesNormalsSDisplay = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setVerticesNormalsTDisplay(bool bVal)
{
    m_bVerticesNormalsTDisplay = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setTrianglesNormalsSDisplay(bool bVal)
{
    m_bTrianglesNormalsSDisplay = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setTrianglesNormalsTDisplay(bool bVal)
{
    m_bTrianglesNormalsTDisplay = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setLandMarksDisplay(bool bVal)
{
    m_bDisplayLandMarks = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setCorrDisplay(bool bVal)
{
    m_bCorrDisplay = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setFillSDisplay(bool bVal)
{
    m_bFillS = bVal;
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::setFillTDisplay(bool bVal)
{
    m_bFillT = bVal;
//    updateGL();
    update();
}


void SWGLOptimalStepNonRigidICP::setAngleMax(double dVal) // TODO : complete
{
    if(m_pOSNRICP)
    {
//        m_pOSNRICP->m_dAngleMax = dVal;
        m_pOSNRICP->m_fAngleMax = static_cast<float>(dVal);
    }
}

void SWGLOptimalStepNonRigidICP::setRotTargetX(int i32Val)
{
    m_fXRotTarget = (i32Val-1800) * 0.1f;
    transformTarget();
}

void SWGLOptimalStepNonRigidICP::setRotTargetY(int i32Val)
{
    m_fYRotTarget = (i32Val-1800) * 0.1f;
    transformTarget();
}

void SWGLOptimalStepNonRigidICP::setRotTargetZ(int i32Val)
{
    m_fZRotTarget = (i32Val-1800) * 0.1f;
    transformTarget();
}

void SWGLOptimalStepNonRigidICP::setTransTargetX(int i32Val)
{
    m_fXTransTarget = (i32Val - 500) * 0.001f;
    transformTarget();
}

void SWGLOptimalStepNonRigidICP::setTransTargetY(int i32Val)
{
    m_fYTransTarget = (i32Val - 500) * 0.001f;
    transformTarget();
}

void SWGLOptimalStepNonRigidICP::setTransTargetZ(int i32Val)
{
    m_fZTransTarget = (i32Val - 500) * 0.001f;
    transformTarget();
}

void SWGLOptimalStepNonRigidICP::setTargetScaling(int i32Val)
{
    m_fTargetScaling = 1.f - i32Val / 1000.f;
    transformTarget();
}

void SWGLOptimalStepNonRigidICP::setStartAlpha(double dVal)
{
    QMutexLocker l_oParamLocker(m_pParamMutex);
    m_dStartAlpha = dVal;
}

void SWGLOptimalStepNonRigidICP::setMinAlpha(double dVal)
{
    QMutexLocker l_oParamLocker(m_pParamMutex);
    m_dMinAlpha = dVal;
}

void SWGLOptimalStepNonRigidICP::setDiffMax(double dVal)
{
    QMutexLocker l_oParamLocker(m_pParamMutex);
    m_dDiffMax = dVal;
}

void SWGLOptimalStepNonRigidICP::setBeta(double dVal)
{
    QMutexLocker l_oParamLocker(m_pParamMutex);
    m_dBeta = dVal;
}

void SWGLOptimalStepNonRigidICP::setGama(double dVal)
{
    QMutexLocker l_oParamLocker(m_pParamMutex);
    m_dGama = dVal;
}

void SWGLOptimalStepNonRigidICP::setCoeffAlpha(double dVal)
{
    QMutexLocker l_oParamLocker(m_pParamMutex);
    m_dCoeffAlpha = dVal;
}

void SWGLOptimalStepNonRigidICP::computeDistWAndCorr()
{
    m_pOSNRICP->computeCorrespondences();
    m_pOSNRICP->computeDistanceWeights();
    update();
}

void SWGLOptimalStepNonRigidICP::transformTarget(cbool bUpdateDisplay)
{
    if(m_pOSNRICP)
    {
        m_pTargetMeshMutex->lockForWrite();

            m_pOSNRICP->m_oTargetMesh.cloud()->copy(*m_pOSNRICP->m_oOriginalTargetMesh.cloud());

            if(m_fTargetScaling != 1.f)
            {
                m_pOSNRICP->m_oTargetMesh.scale(m_fTargetScaling);
            }

            swCloud::SWRigidMotion l_oRigidMotion(m_fXRotTarget, m_fYRotTarget, m_fZRotTarget);
            l_oRigidMotion.m_aFTranslation[0] = m_fXTransTarget;
            l_oRigidMotion.m_aFTranslation[1] = m_fYTransTarget;
            l_oRigidMotion.m_aFTranslation[2] = m_fZTransTarget;

            m_pOSNRICP->m_oTargetMesh.cloud()->transform(l_oRigidMotion.m_aFRotation, l_oRigidMotion.m_aFTranslation);
            m_pOSNRICP->m_oTargetMesh.updateNonOrientedTrianglesNormals();
            m_pOSNRICP->m_oTargetMesh.updateNonOrientedVerticesNormals();

        m_pTargetMeshMutex->unlock();

        m_targetCloudBuffer.m_bUpdate = true;
        m_targetMeshBuffer.m_bUpdate = true;
        m_targetVerticesNormalesBuffer.m_bUpdate = true;
        m_targetTrianglesNormalesBuffer.m_bUpdate = true;

        if(bUpdateDisplay)
        {
            update();
        }
    }
}

void SWGLOptimalStepNonRigidICP::initializeGL()
{
    // qglClearColor(Qt::black);
        qglClearColor(QColor(49, 53, 70));

    // init shaders
        initShaders("../data/shaders/cloudAvatar.vert", "../data/shaders/cloudAvatar.frag", m_oShaderPoints,    false);
        initShaders("../data/shaders/cloudAvatar.vert", "../data/shaders/cloudAvatar.frag", m_oShaderLines,     false);
        initShaders("../data/shaders/meshViewer.vert", "../data/shaders/meshViewer.frag",   m_oShaderMesh, false);

    // enable depth buffer
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);

    // enable blending
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        initIndexBuffer(m_indexBuffer);
        initVertexBuffer(m_vertexBuffer);
        initVertexBuffer(m_colorBuffer);
        initVertexBuffer(m_normalBuffer);
        initVertexBuffer(m_textureBuffer);


        initBufferList(m_templateCloudBuffer);
        initBufferList(m_templateMeshBuffer);
        initBufferList(m_templateVerticesNormalesBuffer);
        initBufferList(m_templateTrianglesNormalesBuffer);

        initBufferList(m_targetCloudBuffer);
        initBufferList(m_targetMeshBuffer);
        initBufferList(m_targetVerticesNormalesBuffer);
        initBufferList(m_targetTrianglesNormalesBuffer);

        initBufferList(m_landmarksManualBuffer);
}


void SWGLOptimalStepNonRigidICP::initBufferList(SWGLBufferList &oBuffer)
{
    oBuffer.m_bUpdate = false;
    initIndexBuffer(oBuffer.m_indexBuffer);
    initVertexBuffer(oBuffer.m_vertexBuffer);
    initVertexBuffer(oBuffer.m_colorBuffer);
    initVertexBuffer(oBuffer.m_normalBuffer);
    initVertexBuffer(oBuffer.m_textureBuffer);
}

void SWGLOptimalStepNonRigidICP::paintGL()
{
    glEnable(GL_DEPTH_TEST);
    // set the size point
        glPointSize(m_glFSizePoint);

    // clear color and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // calculate model view transformation
        QMatrix4x4 l_oViewMatrix;
        l_oViewMatrix.setToIdentity();

    // set camera vue
        l_oViewMatrix.lookAt( m_pCamera->eyePosition(), m_pCamera->lookAt(), m_pCamera->up());

    // comput MVP matrix
        QMatrix4x4 l_oModelMatrix;
        l_oModelMatrix.setToIdentity();
        m_oMVPMatrix = l_oModelMatrix  * m_oProjectionMatrix * l_oViewMatrix;

        drawScene();

}


void SWGLOptimalStepNonRigidICP::paintEvent(QPaintEvent *paintEvent)
{
    paintGL();

        // set mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            checkGlError(true);

    QPainter p;
    p.begin(this);
    p.setRenderHint(QPainter::Antialiasing);
    QFont font=p.font() ;
    font.setPointSize(18);
    p.setFont(font);
    p.setPen(QColor(255,0,0));
    p.drawText(100, height()-50, m_infoDisplay3D);
    p.end();
}


void SWGLOptimalStepNonRigidICP::drawScene()
{
    // draw axe
        drawAxes(m_oShaderLines, m_oMVPMatrix, 0.02f);

    if(!m_pOSNRICP.get())
    {
        return;
    }

    // draw cloud
        // source
        if(m_bPointsSDisplay)
        {
            m_pSourceMeshMutex->lockForRead();
                drawSourceCloud(m_oShaderPoints, *m_pOSNRICP->m_oSourceMesh.cloud(), 6.f, m_oMVPMatrix);
            m_pSourceMeshMutex->unlock();
        }
        // target
        if(m_bPointsTDisplay)
        {
            m_pTargetMeshMutex->lockForRead();
                drawCloud(m_oShaderPoints, *m_pOSNRICP->m_oTargetMesh.cloud(), 3.f, m_oMVPMatrix, 1.f, 0.f, 0.f);
            m_pTargetMeshMutex->unlock();
        }


    // draw corr lines
        if(m_bCorrDisplay)
        {
            m_pSourceMeshMutex->lockForRead();
            m_pTargetMeshMutex->lockForRead();
            m_pUMutex->lockForRead();
                drawCorrLines(m_oShaderLines, *m_pOSNRICP->m_oSourceMesh.cloud(), *m_pOSNRICP->m_oTargetMesh.cloud(), m_pOSNRICP->m_u, m_oMVPMatrix, 1.0f, 1.0f, 0.0f);
            m_pSourceMeshMutex->unlock();
            m_pTargetMeshMutex->unlock();
            m_pUMutex->unlock();
        }

    // draw source mesh
        if(m_bMeshSDisplay)
        {
            m_pSourceMeshMutex->lockForRead();
                drawMesh(m_oShaderMesh, m_templateMeshBuffer, m_pOSNRICP->m_oSourceMesh, m_oMVPMatrix, m_bFillS, GLO_UNI_COLOR, QVector3D(100,100,200));
            m_pSourceMeshMutex->unlock();
        }

    // draw target mesh
        if(m_bMeshTDisplay)
        {
            m_pTargetMeshMutex->lockForRead();
                if(m_targetTextureLocation != -1)
                {
                    drawMesh(m_oShaderMesh, m_targetMeshBuffer, m_pOSNRICP->m_oTargetMesh, m_oMVPMatrix, m_bFillT, GLO_TEXTURE, QVector3D(200,100,100),m_targetTextureLocation);
                }
                else
                {
                    drawMesh(m_oShaderMesh, m_targetMeshBuffer, m_pOSNRICP->m_oTargetMesh, m_oMVPMatrix, m_bFillT, GLO_UNI_COLOR, QVector3D(200,100,100));
                }
            m_pTargetMeshMutex->unlock();
        }

    // draw vertices normals
        // source
        if(m_bVerticesNormalsSDisplay)
        {
            m_pSourceMeshMutex->lockForRead();
                drawMeshVerticesNormals(m_oShaderLines, m_templateVerticesNormalesBuffer, m_pOSNRICP->m_oSourceMesh, m_oMVPMatrix, 0.5f, 0.5f, 0.5f);
            m_pSourceMeshMutex->unlock();
        }
        // target
        if(m_bVerticesNormalsTDisplay)
        {
            m_pTargetMeshMutex->lockForRead();
                drawMeshVerticesNormals(m_oShaderLines, m_targetVerticesNormalesBuffer, m_pOSNRICP->m_oTargetMesh, m_oMVPMatrix, 0.5f, 0.5f, 0.5f);
            m_pTargetMeshMutex->unlock();
        }
    // draw triangles normals
        // source
        if(m_bTrianglesNormalsSDisplay)
        {
            m_pSourceMeshMutex->lockForRead();
                drawMeshTrianglesNormals(m_oShaderLines, m_templateTrianglesNormalesBuffer, m_pOSNRICP->m_oSourceMesh, m_oMVPMatrix, 0.5f, 0.5f, 0.5f);
            m_pSourceMeshMutex->unlock();
        }
        // target
        if(m_bTrianglesNormalsTDisplay)
        {
            m_pTargetMeshMutex->lockForRead();
                drawMeshTrianglesNormals(m_oShaderLines, m_targetTrianglesNormalesBuffer, m_pOSNRICP->m_oTargetMesh, m_oMVPMatrix, 0.5f, 0.5f, 0.5f);
            m_pTargetMeshMutex->unlock();
        }

    // draw landmarks
        if(m_bDisplayLandMarks)
        {
            m_pSourceMeshMutex->lockForRead();
            m_pTargetMeshMutex->lockForRead();
                drawLandMarksPoints(m_oShaderLines, m_pOSNRICP->m_l, *m_pOSNRICP->m_oSourceMesh.cloud(), *m_pOSNRICP->m_oTargetMesh.cloud(), m_oMVPMatrix);
                drawLandMarksCorr(m_oShaderLines, m_pOSNRICP->m_l, *m_pOSNRICP->m_oSourceMesh.cloud(), *m_pOSNRICP->m_oTargetMesh.cloud(), m_oMVPMatrix, 0.2f, 1.0f, 0.0f);
            m_pSourceMeshMutex->unlock();
            m_pTargetMeshMutex->unlock();
        }


    // draw ray
//        m_oShaderLines.bind();
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//        QGLBuffer::release(QGLBuffer::VertexBuffer);
//        QGLBuffer::release(QGLBuffer::IndexBuffer);

//        uint32 *l_aRayI = new uint32[2];
//        l_aRayI[0] = 0;
//        l_aRayI[1] = 1;
//        float  *l_aRayV = new float[2* 3];
//        l_aRayV[0] = m_rayStart.x();
//        l_aRayV[1] = m_rayStart.y();
//        l_aRayV[2] = m_rayStart.z();
//        l_aRayV[3] = m_rayEnd.x();
//        l_aRayV[4] = m_rayEnd.y();
//        l_aRayV[5] = m_rayEnd.z();

//        int l_i32SizeIndex      = sizeof(GLuint);
//        int l_i32SizeVertex     = sizeof(float) * 3;
//        allocateBuffer(m_indexBuffer,  l_aRayI, 2 * l_i32SizeIndex);
//        allocateBuffer(m_vertexBuffer, l_aRayV, 2 * l_i32SizeVertex);

//        // set shaders parameters
//        m_oShaderLines.setUniformValue("displayMode", 1);
//        m_oShaderLines.setUniformValue("uniColor", 1.f, 0.f, 0.f);
//        m_oShaderLines.setUniformValue("mvpMatrix", m_oMVPMatrix);

//        // draw primitives
//        drawBuffer(m_indexBuffer, m_vertexBuffer, m_oShaderLines, GL_LINES);

//        delete[] l_aRayI;
//        delete[] l_aRayV;

//        m_oShaderLines.release();

    // drawManualLandmarks

//        m_clickedPointOnTargetMesh

    if(m_pSourceMesh.get() && m_pTargetMesh.get())
    {
        std::vector<float> l_vX,l_vY,l_vZ;
        m_pSourceMeshMutex->lockForRead();
            for(int ii = 0; ii < m_idClickedPointsOnSourceMesh.size(); ++ii)
            {
                l_vX.push_back(m_pOSNRICP->m_oSourceMesh.cloud()->coord(0)[m_idClickedPointsOnSourceMesh[ii]]);
                l_vY.push_back(m_pOSNRICP->m_oSourceMesh.cloud()->coord(1)[m_idClickedPointsOnSourceMesh[ii]]);
                l_vZ.push_back(m_pOSNRICP->m_oSourceMesh.cloud()->coord(2)[m_idClickedPointsOnSourceMesh[ii]]);
            }
        m_pSourceMeshMutex->unlock();

        if(l_vX.size() > 0)
        {
            swCloud::SWCloud l_landmarksMTemplate(l_vX,l_vY,l_vZ);
            drawPoints(m_oShaderPoints, l_landmarksMTemplate, 25.f, m_oMVPMatrix, 0.5f, 1.f, 0.5f);
        }

        l_vX.clear(); l_vY.clear(); l_vZ.clear();

        m_pTargetMeshMutex->lockForRead();
            for(int ii = 0; ii < m_idClickedPointsOnTargetMesh.size(); ++ii)
            {
                l_vX.push_back(m_pOSNRICP->m_oTargetMesh.cloud()->coord(0)[m_idClickedPointsOnTargetMesh[ii]]);
                l_vY.push_back(m_pOSNRICP->m_oTargetMesh.cloud()->coord(1)[m_idClickedPointsOnTargetMesh[ii]]);
                l_vZ.push_back(m_pOSNRICP->m_oTargetMesh.cloud()->coord(2)[m_idClickedPointsOnTargetMesh[ii]]);
            }
        m_pTargetMeshMutex->unlock();

        if(l_vX.size() > 0)
        {
            swCloud::SWCloud l_landmarksMTarget(l_vX,l_vY,l_vZ);
            drawPoints(m_oShaderPoints, l_landmarksMTarget, 25.f, m_oMVPMatrix, 0.5f, 0.5f, 1.f);
        }

        l_vX.clear(); l_vY.clear(); l_vZ.clear();
        if(m_bTemplateMeshPointSelection)
        {
            l_vX.push_back(m_clickedPointOnSourceMesh.x());
            l_vY.push_back(m_clickedPointOnSourceMesh.y());
            l_vZ.push_back(m_clickedPointOnSourceMesh.z());
        }
        else
        {
            l_vX.push_back(m_clickedPointOnTargetMesh.x());
            l_vY.push_back(m_clickedPointOnTargetMesh.y());
            l_vZ.push_back(m_clickedPointOnTargetMesh.z());
        }

        if(l_vX[0] != -1000)
        {
            swCloud::SWCloud l_landmarksCurrent(l_vX,l_vY,l_vZ);
            drawPoints(m_oShaderPoints, l_landmarksCurrent, 50.f, m_oMVPMatrix, 1.f, 1.f, 1.f);
        }
    }
}



void SWGLOptimalStepNonRigidICP::drawMeshTrianglesNormals(QGLShaderProgram &oShader, SWGLBufferList &oBuffers, const SWMesh &oMesh, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    float l_fCoeffLenghtNormal = 0.01f;

    if(!oMesh.isTrianglesNormals())
    {
        cerr << "drawMeshTrianglesNormals : No triangles normals in the mesh" << endl;
        return;
    }

    oShader.bind();
        checkGlError();

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    if(oBuffers.m_bUpdate)
    {
        int l_i32SizeIndex      = sizeof(GLuint);
        int l_i32SizeVertex     = sizeof(float) * 3;
        uint32 *l_aNormalI      = new uint32[oMesh.trianglesNumber()*2];
        float  *l_aNormalV      = new float[oMesh.trianglesNumber()*2 * 3];

        for(uint ii = 0; ii < oMesh.trianglesNumber(); ++ii)
        {
            // fill index buffer
                l_aNormalI[2*ii]   = 2*ii;
                l_aNormalI[2*ii+1] = 2*ii+1;

            // fill vertex buffer
                std::vector<float> l_vTriCenterPoint;
                oMesh.triangleCenterPoint(l_vTriCenterPoint,ii);
                l_aNormalV[6*ii]   = l_vTriCenterPoint[0];
                l_aNormalV[6*ii+1] = l_vTriCenterPoint[1];
                l_aNormalV[6*ii+2] = l_vTriCenterPoint[2];

                std::vector<float> l_vNormal;
                oMesh.triangleNormal(l_vNormal, ii);
                l_aNormalV[6*ii+3] = l_vTriCenterPoint[0] + l_fCoeffLenghtNormal * l_vNormal[0];
                l_aNormalV[6*ii+4] = l_vTriCenterPoint[1] + l_fCoeffLenghtNormal * l_vNormal[1];
                l_aNormalV[6*ii+5] = l_vTriCenterPoint[2] + l_fCoeffLenghtNormal * l_vNormal[2];
        }

        allocateBuffer(oBuffers.m_indexBuffer,  l_aNormalI, oMesh.trianglesNumber() * 2 * l_i32SizeIndex);
        allocateBuffer(oBuffers.m_vertexBuffer, l_aNormalV, oMesh.trianglesNumber() * 2 * l_i32SizeVertex);

        deleteAndNullifyArray(l_aNormalI);
        deleteAndNullifyArray(l_aNormalV);

        oBuffers.m_bUpdate = false;
    }

    // set shaders parameters
    oShader.setUniformValue("displayMode", 1);
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    drawBuffer(oBuffers.m_indexBuffer, oBuffers.m_vertexBuffer, oShader, GL_LINES);

    oShader.release();
        checkGlError();
}


void SWGLOptimalStepNonRigidICP::drawLandMarksPoints(QGLShaderProgram &oShader, const std::map<uint,uint> mLandMarkCorr, const swCloud::SWCloud &oSource, const swCloud::SWCloud &oTarget, QMatrix4x4 &mvpMatrix)
{
    if(mLandMarkCorr.size() == 0)
    {
        return;
    }

    oShader.bind();
        checkGlError();

    glPointSize(15.f);

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    int l_i32SizeIndex      = sizeof(GLuint);
    int l_i32SizeVertex     = sizeof(float) * 3;

    uint32 *l_aPointI = new uint32[mLandMarkCorr.size()];
    float  *l_aPointV = new float[mLandMarkCorr.size()* 3];

    uint ii = 0;
    for (std::map<uint,uint>::const_iterator it = mLandMarkCorr.cbegin(); it != mLandMarkCorr.cend(); ++it, ++ii)
    {
        // fill index buffer
        l_aPointI[ii]   = ii;

        uint l_ui32TemplateId = it->first;
        l_aPointV[3*ii]   = oSource.coord(0)[l_ui32TemplateId];
        l_aPointV[3*ii+1] = oSource.coord(1)[l_ui32TemplateId];
        l_aPointV[3*ii+2] = oSource.coord(2)[l_ui32TemplateId];
    }

    allocateBuffer(m_indexBuffer,  l_aPointI, static_cast<int>(mLandMarkCorr.size())  * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aPointV, static_cast<int>(mLandMarkCorr.size())  * l_i32SizeVertex);

    // set shaders parameters
    oShader.setUniformValue("displayMode", 1);
    oShader.setUniformValue("uniColor", 1.f, 0.f, 0.f);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    drawBuffer(m_indexBuffer, m_vertexBuffer, oShader, GL_POINTS);

    ii = 0;
    for (std::map<uint,uint>::const_iterator it = mLandMarkCorr.cbegin(); it != mLandMarkCorr.cend(); ++it, ++ii)
    {
        uint l_ui32TargetId = it->second;
        l_aPointV[3*ii]   = oTarget.coord(0)[l_ui32TargetId];
        l_aPointV[3*ii+1] = oTarget.coord(1)[l_ui32TargetId];
        l_aPointV[3*ii+2] = oTarget.coord(2)[l_ui32TargetId];
    }

    allocateBuffer(m_vertexBuffer, l_aPointV, static_cast<int>(mLandMarkCorr.size())  * l_i32SizeVertex);

    // set shaders parameters
//    oShader.setUniformValue("displayMode", 1);
    oShader.setUniformValue("uniColor", 0.0f, 1.f, 0.f);
//    oShader.setUniformValue("mvpMatrix", mvpMatrix);
//    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
   drawBuffer(m_indexBuffer, m_vertexBuffer, oShader, GL_POINTS);

    delete[] l_aPointI;
    delete[] l_aPointV;

    oShader.release();
        checkGlError();
}


void SWGLOptimalStepNonRigidICP::drawLandMarksCorr(QGLShaderProgram &oShader, const std::map<uint,uint> mLandMarkCorr, const swCloud::SWCloud &oSource, const swCloud::SWCloud &oTarget,
                                                   QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    if(mLandMarkCorr.size() == 0)
    {
        return;
    }

    oShader.bind();
        checkGlError();

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    int l_i32SizeIndex      = sizeof(GLuint);
    int l_i32SizeVertex     = sizeof(float) * 3;

    uint32 *l_aCorrLineI    = new uint32[mLandMarkCorr.size()*2];
    float  *l_aCorrLineV    = new float[mLandMarkCorr.size() *2 * 3];

    uint ii = 0;
    for (std::map<uint,uint>::const_iterator it = mLandMarkCorr.cbegin(); it != mLandMarkCorr.cend(); ++it, ++ii)
    {
        // fill index buffer
        l_aCorrLineI[2*ii]   = 2*ii;
        l_aCorrLineI[2*ii+1] = 2*ii+1;

        uint l_ui32TemplateId = it->first;
        uint l_ui32TargetId   = it->second;
//        cout << "f s : " << l_ui32TemplateId << " " << l_ui32TargetId << endl;

        l_aCorrLineV[6*ii]   = oSource.coord(0)[l_ui32TemplateId];
        l_aCorrLineV[6*ii+1] = oSource.coord(1)[l_ui32TemplateId];
        l_aCorrLineV[6*ii+2] = oSource.coord(2)[l_ui32TemplateId];

        l_aCorrLineV[6*ii+3] = oTarget.coord(0)[l_ui32TargetId];
        l_aCorrLineV[6*ii+4] = oTarget.coord(1)[l_ui32TargetId];
        l_aCorrLineV[6*ii+5] = oTarget.coord(2)[l_ui32TargetId];
    }
//    cout << endl;

    allocateBuffer(m_indexBuffer,  l_aCorrLineI, static_cast<int>(mLandMarkCorr.size()) * 2 * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aCorrLineV, static_cast<int>(mLandMarkCorr.size()) * 2 * l_i32SizeVertex);

    // set shaders parameters        
    oShader.setUniformValue("displayMode", 1);
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    drawBuffer(m_indexBuffer, m_vertexBuffer, oShader, GL_LINES);

    deleteAndNullifyArray(l_aCorrLineI);
    deleteAndNullifyArray(l_aCorrLineV);

    oShader.release();
        checkGlError();
}

void SWGLOptimalStepNonRigidICP::drawCorrLines(QGLShaderProgram &oShader, const swCloud::SWCloud &oSource, const swCloud::SWCloud &oTarget,
                                               const std::vector<uint> &vU, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    oShader.bind();
        checkGlError();

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    int l_i32SizeIndex      = sizeof(GLuint);
    int l_i32SizeVertex     = sizeof(float) * 3;

    uint32 *l_aLinesCorrI      = new uint32[oSource.size()*2];
    float  *l_aLinesCorrV      = new float[oSource.size()*2 * 3];
    float  *l_aLinesCorrC      = new float[oSource.size()*2 * 3];

    float l_fMaxDist = 0.f;
    std::vector<float> l_vDist;

    for(uint ii = 0; ii < oSource.size(); ++ii)
    {
        // fill index buffer
        l_aLinesCorrI[2*ii]   = 2*ii;
        l_aLinesCorrI[2*ii+1] = 2*ii+1;

        // fill vertex buffer
        l_aLinesCorrV[6*ii]   = oSource.coord(0)[ii];
        l_aLinesCorrV[6*ii+1] = oSource.coord(1)[ii];
        l_aLinesCorrV[6*ii+2] = oSource.coord(2)[ii];
        l_aLinesCorrV[6*ii+3] = oTarget.coord(0)[vU[ii]];
        l_aLinesCorrV[6*ii+4] = oTarget.coord(1)[vU[ii]];
        l_aLinesCorrV[6*ii+5] = oTarget.coord(2)[vU[ii]];

        // compute distances
        std::vector<float> l_vP1, l_vP2;
        l_vP1.push_back(l_aLinesCorrV[6*ii]);
        l_vP1.push_back(l_aLinesCorrV[6*ii+1]);
        l_vP1.push_back(l_aLinesCorrV[6*ii+2]);

        l_vP2.push_back(l_aLinesCorrV[6*ii+3]);
        l_vP2.push_back(l_aLinesCorrV[6*ii+4]);
        l_vP2.push_back(l_aLinesCorrV[6*ii+5]);

        l_vDist.push_back(swUtil::norm(swUtil::vec(l_vP1, l_vP2)));
        if(l_vDist.back() > l_fMaxDist)
        {
            l_fMaxDist = l_vDist.back();
        }
    }

    for(uint ii = 0; ii < oSource.size(); ++ii)
    {
        // fill color buffer
        float l_fGreen = 1.f - (2*l_vDist[ii])/l_fMaxDist;
        if(l_fGreen > 1.f)
        {
            l_fGreen = 1.f;
        }

        float l_fCol[3] = {l_vDist[ii]/l_fMaxDist, l_fGreen, 0.f};

        l_aLinesCorrC[6*ii]   = l_fCol[0];
        l_aLinesCorrC[6*ii+1] = l_fCol[1];
        l_aLinesCorrC[6*ii+2] = l_fCol[2];

        l_aLinesCorrC[6*ii+3] = l_fCol[0];
        l_aLinesCorrC[6*ii+4] = l_fCol[1];
        l_aLinesCorrC[6*ii+5] = l_fCol[2];
    }

    allocateBuffer(m_indexBuffer, l_aLinesCorrI, oSource.size() * 2 * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer,l_aLinesCorrV, oSource.size() * 2 * l_i32SizeVertex);
    allocateBuffer(m_colorBuffer, l_aLinesCorrC, oSource.size() * 2 * l_i32SizeVertex);

    // set shaders parameters
    oShader.setUniformValue("displayMode", 0);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    drawBufferWithColor(m_indexBuffer, m_vertexBuffer, m_colorBuffer, oShader, GL_LINES);

    deleteAndNullifyArray(l_aLinesCorrI);
    deleteAndNullifyArray(l_aLinesCorrV);
    deleteAndNullifyArray(l_aLinesCorrC);

    oShader.release();
        checkGlError();
}

void SWGLOptimalStepNonRigidICP::drawPoints(QGLShaderProgram &oShader, const swCloud::SWCloud &oCloud, cfloat fSizePoint, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    // bind shader
    oShader.bind();
        checkGlError();

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        checkGlError();

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers

    int l_i32SizeIndex      = sizeof(GLuint);
    int l_i32SizeVertex     = sizeof(float) * 3;
    uint32 *l_aCloudI       = oCloud.indexBuffer();
    float  *l_aCloudV       = oCloud.vertexBuffer();

    allocateBuffer(m_landmarksManualBuffer.m_indexBuffer,  l_aCloudI, oCloud.size() * l_i32SizeIndex);
    allocateBuffer(m_landmarksManualBuffer.m_vertexBuffer, l_aCloudV, oCloud.size() * l_i32SizeVertex);

    deleteAndNullifyArray(l_aCloudI);
    deleteAndNullifyArray(l_aCloudV);

    // set size of the points
    glPointSize(fSizePoint);

    // set uniform values parameters
    oShader.setUniformValue("displayMode", 1);
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    drawBuffer(m_landmarksManualBuffer.m_indexBuffer, m_landmarksManualBuffer.m_vertexBuffer, oShader, GL_POINTS);

    oShader.release();
        checkGlError();
}


void SWGLOptimalStepNonRigidICP::drawCloud(QGLShaderProgram &oShader, const swCloud::SWCloud &oCloud, cfloat fSizePoint, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    // bind shader
    oShader.bind();
        checkGlError();

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        checkGlError();

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    if(m_targetCloudBuffer.m_bUpdate)
    {
        int l_i32SizeIndex      = sizeof(GLuint);
        int l_i32SizeVertex     = sizeof(float) * 3;
        uint32 *l_aCloudI       = oCloud.indexBuffer();
        float  *l_aCloudV       = oCloud.vertexBuffer();

        allocateBuffer(m_targetCloudBuffer.m_indexBuffer,  l_aCloudI, oCloud.size() * l_i32SizeIndex);
        allocateBuffer(m_targetCloudBuffer.m_vertexBuffer, l_aCloudV, oCloud.size() * l_i32SizeVertex);

        deleteAndNullifyArray(l_aCloudI);
        deleteAndNullifyArray(l_aCloudV);

        m_targetCloudBuffer.m_bUpdate = false;
    }

    // set size of the points
    glPointSize(fSizePoint);

    // set uniform values parameters
    oShader.setUniformValue("displayMode", 1);
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    drawBuffer(m_targetCloudBuffer.m_indexBuffer, m_targetCloudBuffer.m_vertexBuffer, oShader, GL_POINTS);

    oShader.release();
        checkGlError();
}

void SWGLOptimalStepNonRigidICP::drawSourceCloud(QGLShaderProgram &oShader, const swCloud::SWCloud &oCloud, cfloat fSizePoint, QMatrix4x4 &mvpMatrix)
{
    oShader.bind();
        checkGlError();

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        checkGlError();

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    if(m_templateCloudBuffer.m_bUpdate)
    {
        int l_i32SizeIndex      = sizeof(GLuint);
        int l_i32SizeVertex     = sizeof(float) * 3;
        uint32 *l_aCloudI       = oCloud.indexBuffer();
        float  *l_aCloudV       = oCloud.vertexBuffer();
        float *l_aCloudC        = new float[3*oCloud.size()];

        // init color point buffer based on weights values (w[i] -> red, w[i] -> white)
            for(uint ii = 0; ii < oCloud.size(); ++ii)
            {
                float l_fCol[3] = {1.f, 0.f, 0.f};

                if(m_pOSNRICP->m_w[ii] > 0.0)
                {
                    l_fCol[0] = 1.f;
                    l_fCol[1] = 1.f;
                    l_fCol[2] = 1.f;
                }

                if(ii == m_idClickedPointOnSourceMesh)
                {
                    l_fCol[0] = 0.f;
                    l_fCol[1] = 1.f;
                    l_fCol[2] = 0.f;
                }

                l_aCloudC[3*ii]   = l_fCol[0];
                l_aCloudC[3*ii+1] = l_fCol[1];
                l_aCloudC[3*ii+2] = l_fCol[2];
            }

        allocateBuffer(m_templateCloudBuffer.m_indexBuffer,  l_aCloudI, oCloud.size() * l_i32SizeIndex);
        allocateBuffer(m_templateCloudBuffer.m_vertexBuffer, l_aCloudV, oCloud.size() * l_i32SizeVertex);
        allocateBuffer(m_templateCloudBuffer.m_colorBuffer,  l_aCloudC, oCloud.size() * l_i32SizeVertex);

        deleteAndNullifyArray(l_aCloudI);
        deleteAndNullifyArray(l_aCloudV);
        deleteAndNullifyArray(l_aCloudC);

        m_templateCloudBuffer.m_bUpdate = false;
    }

    // set size of the points
    glPointSize(fSizePoint);

    // set uniform values parameters
    oShader.setUniformValue("displayMode", 0);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    drawBufferWithColor(m_templateCloudBuffer.m_indexBuffer, m_templateCloudBuffer.m_vertexBuffer,
                        m_templateCloudBuffer.m_colorBuffer, oShader, GL_POINTS);

    oShader.release();
        checkGlError();
}

void SWGLOptimalStepNonRigidICP::drawMeshVerticesNormals(QGLShaderProgram &oShader, SWGLBufferList &oBuffers, const swMesh::SWMesh &oMesh, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    float l_fCoeffLenghtNormal = 0.01f;

    if(!oMesh.isVerticesNormals())
    {
        cerr << "drawMeshVerticesNormals : No vertices normals in the mesh" << endl;
        return;
    }

    oShader.bind();
        checkGlError();

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    if(oBuffers.m_bUpdate)
    {
        int l_i32SizeIndex      = sizeof(GLuint);
        int l_i32SizeVertex     = sizeof(float) * 3;

        uint32 *l_aNormalI      = new uint32[oMesh.pointsNumber()*2];
        float  *l_aNormalV      = new float[oMesh.pointsNumber()*2 * 3];

        for(uint ii = 0; ii < oMesh.pointsNumber(); ++ii)
        {
            // fill index buffer
                l_aNormalI[2*ii]   = 2*ii;
                l_aNormalI[2*ii+1] = 2*ii+1;

            // fill vertex buffer
                std::vector<float> l_vPoint;
                oMesh.point(l_vPoint, ii);
                l_aNormalV[6*ii]   = l_vPoint[0];
                l_aNormalV[6*ii+1] = l_vPoint[1];
                l_aNormalV[6*ii+2] = l_vPoint[2];

                std::vector<float> l_vNormal;
                oMesh.vertexNormal(l_vNormal, ii);
                l_aNormalV[6*ii+3] = l_vPoint[0] + l_fCoeffLenghtNormal * l_vNormal[0];
                l_aNormalV[6*ii+4] = l_vPoint[1] + l_fCoeffLenghtNormal * l_vNormal[1];
                l_aNormalV[6*ii+5] = l_vPoint[2] + l_fCoeffLenghtNormal * l_vNormal[2];
        }

        allocateBuffer(oBuffers.m_indexBuffer,  l_aNormalI, oMesh.pointsNumber() * 2 * l_i32SizeIndex);
        allocateBuffer(oBuffers.m_vertexBuffer, l_aNormalV, oMesh.pointsNumber() * 2 * l_i32SizeVertex);

        deleteAndNullifyArray(l_aNormalI);
        deleteAndNullifyArray(l_aNormalV);

        oBuffers.m_bUpdate = false;
    }

    // set shaders parameters
    oShader.setUniformValue("displayMode", 1);
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    drawBuffer(oBuffers.m_indexBuffer, oBuffers.m_vertexBuffer, oShader, GL_LINES);

    oShader.release();
        checkGlError();
}



void SWGLOptimalStepNonRigidICP::resetMorphing()
{
    if(m_pSourceMesh.get() && m_pTargetMesh.get())
    {
        m_pOSNRICP.reset();

        QFileInfo l_oFile1(m_sPathStasmSource), l_oFile2(m_sPathStasmTarget);
        if(l_oFile1.exists() && l_oFile2.exists())
        {
            m_pOSNRICP = SWOptimalStepNonRigidICPPtr(new SWOptimalStepNonRigidICP(*m_pSourceMesh.get(), *m_pTargetMesh.get(),
                                                    m_sPathStasmSource.toUtf8().constData(), m_sPathStasmTarget.toUtf8().constData()));
        }
        else
        {
            m_pOSNRICP = SWOptimalStepNonRigidICPPtr(new SWOptimalStepNonRigidICP(*m_pSourceMesh.get(), *m_pTargetMesh.get()));
        }

        // apply transformation
            transformTarget(false);

        // init morphing
            m_pOSNRICP->computeCorrespondences();
            m_pOSNRICP->computeDistanceWeights();

        // set buffer update to true
            m_templateCloudBuffer.m_bUpdate = true;
            m_templateMeshBuffer.m_bUpdate = true;
            m_templateVerticesNormalesBuffer.m_bUpdate = true;
            m_templateTrianglesNormalesBuffer.m_bUpdate = true;

            m_targetCloudBuffer.m_bUpdate = true;
            m_targetMeshBuffer.m_bUpdate = true;
            m_targetVerticesNormalesBuffer.m_bUpdate = true;
            m_targetTrianglesNormalesBuffer.m_bUpdate = true;


            if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
            {
                std::vector<float> l_v3FMean = m_pOSNRICP->m_oSourceMesh.cloud()->meanPoint();
                QVector3D l_oEye,l_oLookAt;
                l_oLookAt.setX(l_v3FMean[0]);
                l_oLookAt.setY(l_v3FMean[1]);
                l_oLookAt.setZ(l_v3FMean[2]);
                l_oEye = l_oLookAt;
                l_oEye.setZ(l_oEye.z() - 0.25f);

                setCamera(l_oEye,l_oLookAt);
                setCameraInitial(l_oEye,l_oLookAt, QVector3D(0,1,0));
            }
//            updateGL();
            update();
    }
}

void SWGLOptimalStepNonRigidICP::resetMorphingWithNewCorr()
{
    // delete data
        m_pOSNRICP.reset();

    // reset the morphing
        resetMorphing();
}

void SWGLOptimalStepNonRigidICP::refreshDisplay(bool bBufferUpdate)
{
    if(bBufferUpdate)
    {
        bufferUpdate();
    }
//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::resetMorphingWithNewMeshes()
{
    if(m_sPathSourceMesh == "" || m_sPathTargetMesh == "")
    {
        std::cerr << "Defines meshes before reset. " << std::endl;
        return;
    }

    m_infoDisplay3D = QString("Meshes set, the morphing can start.");

    // align source and target meshes with the nose
        alignWithNose(*m_pSourceMesh.get(), *m_pTargetMesh.get());

    // reset the morphing
        resetMorphing();

    // set buffers to update
        bufferUpdate();
}


void SWGLOptimalStepNonRigidICP::bufferUpdate()
{
    // set buffers to update
        m_templateCloudBuffer.m_bUpdate = true;
        m_templateMeshBuffer.m_bUpdate = true;
        m_templateVerticesNormalesBuffer.m_bUpdate = true;
        m_templateTrianglesNormalesBuffer.m_bUpdate = true;

        m_targetCloudBuffer.m_bUpdate = true;
        m_targetMeshBuffer.m_bUpdate = true;
        m_targetVerticesNormalesBuffer.m_bUpdate = true;
        m_targetTrianglesNormalesBuffer.m_bUpdate = true;

}

void SWGLOptimalStepNonRigidICP::setSourceMesh(const QString &sPathSource)
{
    // set the source mesh
        m_sPathSourceMesh = sPathSource.toUtf8().constData(); // BUG : Qt version not compiled with std support, so toStdString crash, use toUtf8 instead
        m_pOSNRICP.reset();
        m_pSourceMesh.reset();
        m_pSourceMesh = SWMeshPtr(new swMesh::SWMesh(m_sPathSourceMesh));

        std::vector<float> l_normal0;
        if(!m_pSourceMesh->vertexNormal(l_normal0,0))
        {
            // the mesh doesn't have valid normals, they will be recomputed
            m_pSourceMesh->updateNonOrientedTrianglesNormals();
            m_pSourceMesh->updateNonOrientedVerticesNormals();
        }


    // check if the mesh is too huge
        if(m_pSourceMesh->pointsNumber() > 10000)
        {
            m_pSourceMesh.reset();

            std::cerr << "Template mesh too huge for morphing computing. " << std::endl;

            return;
        }

    // set the source stasm file path
        m_sPathStasmSource = sPathSource;
        for(int ii = 0; ii < 3; ++ii)
        {
            m_sPathStasmSource.remove(m_sPathStasmSource.size()-1,1);
        }
        m_sPathStasmSource += "stasm";

    // send meshes infos to the interface
        QString l_sSourceMeshInfos(getInfoMesh(*m_pSourceMesh));

    // check stasm file existence
        QFileInfo l_oFileInfo(m_sPathStasmSource);

        if(l_oFileInfo.exists())
        {
             l_sSourceMeshInfos += QString("STASM path : ") + m_sPathStasmSource;
        }
        else
        {
            l_sSourceMeshInfos  += QString("STASM path : file not found");
        }

        emit sendSourceMeshInfos(l_sSourceMeshInfos);

    // launch the reset if source mesh and target mesh are defined
        if(m_pSourceMesh.get() && m_pTargetMesh.get())
        {
            resetMorphingWithNewMeshes();
        }

        if(m_pSourceMesh.get() && !m_pTargetMesh.get())
        {
            m_infoDisplay3D = tr("Template mesh set, now load the target mesh. ");
            update();
        }
}

void SWGLOptimalStepNonRigidICP::setTargetMesh(const QString &sPathTarget)
{
    // set the target mesh
        m_sPathTargetMesh = sPathTarget.toUtf8().constData(); // BUG : Qt version not compiled with std support, so toStdString crash, use toUtf8 instead
        m_pOSNRICP.reset();
        m_pTargetMesh.reset();
        m_pTargetMesh = SWMeshPtr(new swMesh::SWMesh(m_sPathTargetMesh));

        std::vector<float> l_normal0;
        if(!m_pTargetMesh->vertexNormal(l_normal0,0))
        {
            // the mesh doesn't have valid normals, they will be recomputed
            m_pTargetMesh->updateNonOrientedTrianglesNormals();
            m_pTargetMesh->updateNonOrientedVerticesNormals();
        }

    // set the target stasm file path
        m_sPathStasmTarget = sPathTarget;
        for(int ii = 0; ii < 3; ++ii)
        {
            m_sPathStasmTarget.remove(m_sPathStasmTarget.size()-1,1);
        }
        m_sPathStasmTarget += "stasm";

        QString l_sTargetMeshInfos(getInfoMesh(*m_pTargetMesh));


    // check stasm file existence
        QFileInfo l_oFileInfo(m_sPathStasmTarget);

        if(l_oFileInfo.exists())
        {
            l_sTargetMeshInfos += QString("STASM path : ") + m_sPathStasmTarget;
        }
        else
        {
            l_sTargetMeshInfos += QString("STASM path : file not found");
        }

    // send meshes infos to the interface
        emit sendTargetMeshInfos(l_sTargetMeshInfos);

// launch the reset if source mesh and target mesh are defined
    if(m_pSourceMesh.get() && m_pTargetMesh.get())
    {
        resetMorphingWithNewMeshes();
    }

    if(!m_pSourceMesh.get() && m_pTargetMesh.get())
    {
        m_infoDisplay3D = tr("Target mesh set, now load the template mesh. ");
        update();
    }
}


QString SWGLOptimalStepNonRigidICP::getInfoMesh(const swMesh::SWMesh &oMesh)
{
    QString l_sInfo(QString("Points     : ") + QString::number(oMesh.pointsNumber()) + QString("\n"));
    l_sInfo +=      QString("Triangles : ") + QString::number(oMesh.trianglesNumber()) + QString("\n");
    l_sInfo +=      QString("Edges     : ") + QString::number(oMesh.edgesNumber()) + QString("\n");
    return l_sInfo;
}

void SWGLOptimalStepNonRigidICP::setTemplateMeshPointSelection(const bool templateMesh)
{
    m_bTemplateMeshPointSelection = templateMesh;
}

void SWGLOptimalStepNonRigidICP::updatePointMeshClicked()
{
    if(!m_pOSNRICP.get())
    {
        return;
    }

    std::vector<float> l_rayStart;
    l_rayStart.push_back(m_rayStart.x());
    l_rayStart.push_back(m_rayStart.y());
    l_rayStart.push_back(m_rayStart.z());
    std::vector<float> l_rayEnd;
    l_rayEnd.push_back(m_rayEnd.x());
    l_rayEnd.push_back(m_rayEnd.y());
    l_rayEnd.push_back(m_rayEnd.z());


    std::vector<std::vector<float> > l_vIntersectionPoints;

    // update current selected point
    //      on the template mesh
    if(m_bTemplateMeshPointSelection)
    {

        for(uint ii = 0; ii < m_pOSNRICP->m_oSourceMesh.trianglesNumber(); ++ii)
        {
            std::vector<float> pt1,pt2,pt3;
            m_pOSNRICP->m_oSourceMesh.trianglePoints(pt1,pt2,pt3,ii);


            std::vector<float> l_intersectionPoint;
            if(swUtil::segmentTriangleIntersect(l_rayStart,l_rayEnd,pt1,pt2,pt3,l_intersectionPoint) == 1)
            {
                l_vIntersectionPoints.push_back(l_intersectionPoint);
            }
        }
    }
    //      on the target mesh
    else
    {
        for(uint ii = 0; ii < m_pOSNRICP->m_oTargetMesh.trianglesNumber(); ++ii)
        {
            std::vector<float> pt1,pt2,pt3;
            m_pOSNRICP->m_oTargetMesh.trianglePoints(pt1,pt2,pt3,ii);


            std::vector<float> l_intersectionPoint;
            if(swUtil::segmentTriangleIntersect(l_rayStart,l_rayEnd,pt1,pt2,pt3,l_intersectionPoint) == 1)
            {
                l_vIntersectionPoints.push_back(l_intersectionPoint);
            }
        }
    }

    double l_distMin = DBL_MAX;
    int l_idMin = -1;
    for(uint ii = 0; ii < l_vIntersectionPoints.size(); ++ii)
    {
        QVector3D l_intersectionPoint(l_vIntersectionPoints[ii][0],l_vIntersectionPoints[ii][1],l_vIntersectionPoints[ii][2]);

        double l_length = (l_intersectionPoint - m_pCamera->eyePosition()).length();

        qDebug() << "lenght : " << l_length;

        if(l_length < l_distMin)
        {
            l_distMin = l_length;
            l_idMin = ii;
        }
    }

    qDebug() << "l_idMin : " << l_idMin;


    if(m_bTemplateMeshPointSelection)
    {
        if(l_idMin != -1)
        {

            m_idClickedPointOnSourceMesh = m_pOSNRICP->m_oSourceMesh.cloud()->idNearestPoint(l_vIntersectionPoints[l_idMin]);
            m_clickedPointOnSourceMesh.setX(m_pOSNRICP->m_oSourceMesh.cloud()->coord(0)[m_idClickedPointOnSourceMesh]);
            m_clickedPointOnSourceMesh.setY(m_pOSNRICP->m_oSourceMesh.cloud()->coord(1)[m_idClickedPointOnSourceMesh]);
            m_clickedPointOnSourceMesh.setZ(m_pOSNRICP->m_oSourceMesh.cloud()->coord(2)[m_idClickedPointOnSourceMesh]);

            qDebug() << "Clicked point on source mesh : " << m_clickedPointOnSourceMesh;
        }
    }
    else
    {
        if(l_idMin != -1)
        {
            m_idClickedPointOnTargetMesh = m_pOSNRICP->m_oTargetMesh.cloud()->idNearestPoint(l_vIntersectionPoints[l_idMin]);
            m_clickedPointOnTargetMesh.setX(m_pOSNRICP->m_oTargetMesh.cloud()->coord(0)[m_idClickedPointOnTargetMesh]);
            m_clickedPointOnTargetMesh.setY(m_pOSNRICP->m_oTargetMesh.cloud()->coord(1)[m_idClickedPointOnTargetMesh]);
            m_clickedPointOnTargetMesh.setZ(m_pOSNRICP->m_oTargetMesh.cloud()->coord(2)[m_idClickedPointOnTargetMesh]);

            qDebug() << "Clicked point on target mesh : " << m_clickedPointOnTargetMesh;
        }
    }

//    updateGL();

    update();
}

void SWGLOptimalStepNonRigidICP::stockCurrentClickedMeshPoint(const bool templateMesh, int &clickedPointId, QVector3D &clickedPoint)
{
    if(templateMesh)
    {
        m_idClickedPointsOnSourceMesh.push_back(m_idClickedPointOnSourceMesh);
        m_clickedPointsOnSourceMesh.push_back(m_clickedPointOnSourceMesh);
        clickedPointId = m_idClickedPointOnSourceMesh;
        clickedPoint = m_clickedPointOnSourceMesh;
    }
    else
    {
        m_idClickedPointsOnTargetMesh.push_back(m_idClickedPointOnTargetMesh);
        m_clickedPointsOnTargetMesh.push_back(m_clickedPointOnTargetMesh);
        clickedPointId = m_idClickedPointOnTargetMesh;
        clickedPoint = m_clickedPointOnTargetMesh;
    }
}

void SWGLOptimalStepNonRigidICP::eraseManuallyLandmarks()
{
    m_clickedPointsOnSourceMesh.clear();
    m_clickedPointsOnTargetMesh.clear();

    m_idClickedPointsOnSourceMesh.clear();
    m_idClickedPointsOnTargetMesh.clear();

    m_clickedPointOnSourceMesh = QVector3D(-1000,-1000,-1000);
    m_clickedPointOnTargetMesh = QVector3D(-1000,-1000,-1000);

//    updateGL();
    update();
}

void SWGLOptimalStepNonRigidICP::updateLandmarksWithSTASM()
{
    if(m_pSourceMesh.get() && m_pTargetMesh.get())
    {
        m_pOSNRICP->updateLandmarksWithSTASM();
    }
}

void SWGLOptimalStepNonRigidICP::updateLandmarksWithManualSelection()
{
    if(m_pSourceMesh.get() && m_pTargetMesh.get())
    {
        std::vector<int> l_idSource,l_idTarget;
        for(int ii = 0; ii < m_idClickedPointsOnSourceMesh.size(); ++ii)
        {
            l_idSource.push_back(m_idClickedPointsOnSourceMesh[ii]);
            l_idTarget.push_back(m_idClickedPointsOnTargetMesh[ii]);
        }

        m_pOSNRICP->updateLandmarksWithManualSelection(l_idSource,l_idTarget);
    }
}

void SWGLOptimalStepNonRigidICP::updateTargetTexture(QString pathTargetTexture)
{
    qDebug() << pathTargetTexture;
    m_targetTexture = QImage(pathTargetTexture);
    m_targetTextureLocation = bindTexture(m_targetTexture);
    m_targetMeshBuffer.m_bUpdate = true;

    update();
    //    updateGL();
}

void SWGLOptimalStepNonRigidICP::updateDisplayTextEndMorphing()
{
    m_infoDisplay3D = QString("Morphing done.");
    update();
}

void SWGLOptimalStepNonRigidICP::setInfo3DDisplay(QString textToDisplay)
{
    m_infoDisplay3D = textToDisplay;
    update();
}


void SWGLOptimalStepNonRigidICP::drawMesh(QGLShaderProgram &shader, SWGLBufferList &buffers, const SWMesh &mesh, const QMatrix4x4 &mvpMatrix, const bool fillMesh,
                                          const GLObjectDisplayMode displayMode, const QVector3D unicolor, const int textureLocation)
{
    // bind shader for meshes
    shader.bind();
        checkGlError(true);

        std::vector<float> l_coordText;
        bool l_textureCoordinatesExist = mesh.textureCoordinate(0,l_coordText);

    // release buffers
        QGLBuffer::release(QGLBuffer::VertexBuffer);
        QGLBuffer::release(QGLBuffer::IndexBuffer);

    // set polygon mode
        if(fillMesh)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

    // uniform
        // camera
            shader.setUniformValue("viewDirection", -m_pCamera->lookAt());
        // transformations
            shader.setUniformValue("mvpMatrix", mvpMatrix);
        // color / texture
            shader.setUniformValue("displayMode", displayMode);
            shader.setUniformValue("uniColor", unicolor.x()/255., unicolor.y()/255., unicolor.z()/255.);
        // lights
            std::vector<float> l_lookAt;
            shader.setUniformValue("lSourcePos" , QVector3D(0,0,-0.8f));
            shader.setUniformValue("kSpecular" , 0.1f);

            if(buffers.m_bUpdate)
            {
                // allocate buffers
                    float  *l_vertexBuffer   = mesh.vertexBuffer();
                    float  *l_colorBuffer    = mesh.colorBuffer();
                    uint32 *l_indexBuffer    = mesh.indexVertexTriangleBuffer();
                    float  *l_normalBuffer   = mesh.normalBuffer();
                    float  *l_textureBuffer  = NULL;

                    if(l_textureCoordinatesExist && displayMode == GLO_TEXTURE)
                    {
                        l_textureBuffer = mesh.textureBuffer();
                        allocateBuffer(buffers.m_textureBuffer, l_textureBuffer, mesh.pointsNumber() * 2 * sizeof(float));
                    }

                    allocateBuffer(buffers.m_vertexBuffer,  l_vertexBuffer, mesh.pointsNumber() *  3 * sizeof(float) );
                    allocateBuffer(buffers.m_indexBuffer,   l_indexBuffer,  mesh.trianglesNumber() * 3* sizeof(GLuint) );
                    allocateBuffer(buffers.m_colorBuffer,   l_colorBuffer,  mesh.pointsNumber() *  3 * sizeof(float));
                    allocateBuffer(buffers.m_normalBuffer,  l_normalBuffer, mesh.pointsNumber() *  3 * sizeof(float));

                    deleteAndNullifyArray(l_vertexBuffer);
                    deleteAndNullifyArray(l_colorBuffer);
                    deleteAndNullifyArray(l_indexBuffer);
                    deleteAndNullifyArray(l_normalBuffer);
                    deleteAndNullifyArray(l_textureBuffer);

                buffers.m_bUpdate = false;
            }

            // draw
                if(displayMode == GLO_ORIGINAL_COLOR)
                {
                    drawBufferWithColor(buffers.m_indexBuffer, buffers.m_vertexBuffer,
                                        buffers.m_colorBuffer, buffers.m_normalBuffer, shader, GL_TRIANGLES);
                }
                else if(l_textureCoordinatesExist && displayMode == GLO_TEXTURE)
                {
                    glEnable(GL_TEXTURE_2D);

                    // bind texture
                        glBindTexture(GL_TEXTURE_2D, textureLocation);

                    drawBufferWithTexture(buffers.m_indexBuffer, buffers.m_vertexBuffer,
                                          buffers.m_textureBuffer,buffers.m_normalBuffer, shader, GL_TRIANGLES);

                    glDisable(GL_TEXTURE_2D);
                }
                else if(displayMode == GLO_UNI_COLOR || !l_textureCoordinatesExist)
                {
                    drawBuffer(buffers.m_indexBuffer, buffers.m_vertexBuffer,
                               buffers.m_normalBuffer, shader, GL_TRIANGLES);
                }

        shader.release();
}

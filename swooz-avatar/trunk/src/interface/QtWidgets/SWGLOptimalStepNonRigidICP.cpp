



/**
 * \file SWGLOptimalStepNonRigidICP.cpp
 * \brief Defines SWGLOptimalStepNonRigidICP
 * \author Florian Lance
 * \date 26/09/13
 */

#include "SWGLOptimalStepNonRigidICP.h"
#include "geometryUtility.h"

#include "moc_SWGLOptimalStepNonRigidICP.cpp"


#include "interface/SWGLUtility.h"


using namespace std;
using namespace swMesh;
using namespace swExcept;

#include "time.h"


SWGLOptimalStepNonRigidICP::SWGLOptimalStepNonRigidICP(QGLContext *context, QWidget* parent) : SWGLWidget(context, parent),
     m_vertexBuffer(QGLBuffer::VertexBuffer), m_indexBuffer(QGLBuffer::IndexBuffer), m_colorBuffer(QGLBuffer::VertexBuffer),
     m_normalBuffer(QGLBuffer::VertexBuffer), m_textureBuffer(QGLBuffer::VertexBuffer)
{       
    // set default parameters values
        // display
            m_bMeshSDisplay = m_bMeshTDisplay = m_bCorrDisplay = m_bDisplayLandMarks = true;
            m_bPointsTDisplay = m_bPointsSDisplay = false;
            m_bVerticesNormalsSDisplay = m_bTrianglesNormalsSDisplay = m_bVerticesNormalsTDisplay  = m_bTrianglesNormalsTDisplay = false;
        // translations
            m_fXTransTarget = m_fYTransTarget = m_fZTransTarget = 0.f;
        // rotations
            m_fXRotTarget = m_fYRotTarget = m_fZRotTarget = 0.f;
        // scaling
            m_fTargetScaling = 0.f;
        // OptimalStepNonRigidICP
            m_dStartAlpha   = 1.0;
            m_dMinAlpha     = 0.2;
            m_dDiffMax      = 100.0;
            m_dBeta         = 0.4;
            m_dCoeffAlpha   = 0.8;
            m_dGama         = 100.0;
            m_bUseLandMarks = false;
        // paths files
            m_sPathSourceMesh = "";
            m_sPathTargetMesh = "";

    // set parameters values
        // display
            m_fDefaultOpacity         = 1.f;
            m_fOpacitySourceMeshLines = 1.f;
            m_fOpacityTargetMeshLines = 0.3f;                                    

        // paths files
            m_sPathStasmFileSource = ""; //../data/meshes/avatars/genericMeshstasmLandMarks.txt";
            m_sPathStasmFileTarget = ""; //"../data/meshes/avatars/faceshiftMeshstasmLandMarks.txt";

    // init mutex
        m_pParamMutex       = new QMutex();
        m_pSourceMeshMutex  = new QReadWriteLock();
        m_pTargetMeshMutex  = new QReadWriteLock();
        m_pUMutex           = new QReadWriteLock();
        m_pWMutex           = new QReadWriteLock();

    // udpate display
        updateGL();
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

    vector<float> l_A3FSourceMeanPoint = l_oSource->meanPoint();
    vector<float> l_A3FTargetMeanPoint = l_oTarget->meanPoint();

    l_A3FSourceMeanPoint[2] -= 10.f;
    l_A3FTargetMeanPoint[2] -= 10.f;
    int idCloseSource = l_oSource->idNearestPoint(l_A3FSourceMeanPoint);
    int idCloseTarget = l_oTarget->idNearestPoint(l_A3FTargetMeanPoint);

    vector<float> l_A3FOffset;
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

    m_pUMutex->lockForWrite();
    m_pSourceMeshMutex->lockForRead();
    m_pTargetMeshMutex->lockForRead();
        m_pOSNRICP->computeCorrespondences();
    m_pUMutex->unlock();
    m_pWMutex->lockForWrite();
        m_pOSNRICP->computeDistanceWeights();
    m_pWMutex->unlock();
    m_pTargetMeshMutex->unlock();

    m_pUMutex->lockForRead();
    m_pWMutex->lockForRead();
        m_pOSNRICP->copyDataForResolving();
    m_pSourceMeshMutex->unlock();
    m_pUMutex->unlock();
    m_pWMutex->unlock();
}


double SWGLOptimalStepNonRigidICP::morph(cdouble dAlpha)
{
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
    m_pParamMutex->unlock();

    initResolve();
    double l_dDiff = m_pOSNRICP->resolve(dAlpha, l_dBeta, l_dGama, l_dUseLandmarks);

    m_pSourceMeshMutex->lockForWrite();
        m_pOSNRICP->updateSourceMeshWithMorphModification();
    m_pSourceMeshMutex->unlock();

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

    qDebug() << "Path : " << l_sPath;
    qDebug() << "Save OBJ file : " << l_sNameOBJ;
    qDebug() << "Save Material file : " << l_sNameMTL;

    m_pOSNRICP->m_oSourceMesh.saveToObj(sPath.toStdString(), l_sNameOBJ.toStdString(), l_sNameMTL.toStdString());
//    m_pOSNRICP->m_oSourceMesh.saveToObj(sPath.toStdString());
}

void SWGLOptimalStepNonRigidICP::setCloudSDisplay(bool bVal)
{
    m_bPointsSDisplay = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setCloudTDisplay(bool bVal)
{
    m_bPointsTDisplay = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setMeshSDisplay(bool bVal)
{
    m_bMeshSDisplay = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setMeshTDisplay(bool bVal)
{
    m_bMeshTDisplay = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setUseLandMarks(bool bVal)
{
    m_bUseLandMarks = bVal;
}

void SWGLOptimalStepNonRigidICP::setVerticesNormalsSDisplay(bool bVal)
{
    m_bVerticesNormalsSDisplay = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setVerticesNormalsTDisplay(bool bVal)
{
    m_bVerticesNormalsTDisplay = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setTrianglesNormalsSDisplay(bool bVal)
{
    m_bTrianglesNormalsSDisplay = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setTrianglesNormalsTDisplay(bool bVal)
{
    m_bTrianglesNormalsTDisplay = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setLandMarksDisplay(bool bVal)
{
    m_bDisplayLandMarks = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setCorrDisplay(bool bVal)
{
    m_bCorrDisplay = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setFillSDisplay(bool bVal)
{
    m_bFillS = bVal;
    updateGL();
}

void SWGLOptimalStepNonRigidICP::setFillTDisplay(bool bVal)
{
    m_bFillT = bVal;
    updateGL();
}


void SWGLOptimalStepNonRigidICP::setAngleMax(double dVal) // TODO : complete
{
    if(m_pOSNRICP)
    {
        m_pOSNRICP->m_dAngleMax = dVal;
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
    updateGL();
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

        m_pTargetMeshMutex->unlock();

        if(bUpdateDisplay)
        {
            updateGL();
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
        initShaders("../data/shaders/meshAvatar.vert", "../data/shaders/meshAvatar.frag",   m_oShaderTriangles, false);

    // enable depth buffer
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);

    // set transparency
        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // do not use, make the mesh invisible( todo : check why)

        // enable texture
//            glEnable(GL_TEXTURE_2D);

    // init buffers
        m_indexBuffer.create();
        m_vertexBuffer.create();
        m_colorBuffer.create();
        m_normalBuffer.create();
        m_textureBuffer.create();
//        m_indexBuffer.setUsagePattern( QGLBuffer::DynamicDraw);
//        m_vertexBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
//        m_colorBuffer.setUsagePattern( QGLBuffer::DynamicDraw);
//        m_normalBuffer.setUsagePattern( QGLBuffer::DynamicDraw);
//        m_textureBuffer.setUsagePattern( QGLBuffer::DynamicDraw);
}

void SWGLOptimalStepNonRigidICP::paintGL()
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

    // draw the scene
        drawScene();
}


void SWGLOptimalStepNonRigidICP::drawScene()
{
    makeCurrent();

    // draw axe
        drawAxes(m_oShaderLines, m_oMVPMatrix, 0.02f);

    if(!m_pOSNRICP)
    {
        return;
    }    

    // draw cloud
        // source
        if(m_bPointsSDisplay)
        {
            try
            {
                m_pSourceMeshMutex->lockForRead();
                    drawSourceCloud(m_oShaderPoints, *m_pOSNRICP->m_oSourceMesh.cloud(), 6.f, m_oMVPMatrix);
                m_pSourceMeshMutex->unlock();
            }
            catch(const openglError &e)
            {
                qWarning() << "drawSourceCloud : " << e.what();
            }
        }
        // target
        if(m_bPointsTDisplay)
        {
            try
            {
                m_pTargetMeshMutex->lockForRead();
                    drawCloud(m_oShaderPoints, *m_pOSNRICP->m_oTargetMesh.cloud(), 3.f, m_oMVPMatrix, 1.f, 0.f, 0.f);
                m_pTargetMeshMutex->unlock();
            }
            catch(const openglError &e)
            {
                qWarning() << "drawCloud : " << e.what();
            }
        }


    // draw corr lines
        if(m_bCorrDisplay)
        {
            try
            {
                m_pSourceMeshMutex->lockForRead();
                m_pTargetMeshMutex->lockForRead();
                m_pUMutex->lockForRead();
                    drawCorrLines(m_oShaderLines, *m_pOSNRICP->m_oSourceMesh.cloud(), *m_pOSNRICP->m_oTargetMesh.cloud(), m_pOSNRICP->m_u, m_oMVPMatrix, 1.0f, 1.0f, 0.0f);
                m_pSourceMeshMutex->unlock();
                m_pTargetMeshMutex->unlock();
                m_pUMutex->unlock();
            }
            catch(const openglError &e)
            {
                qWarning() << "drawCorrLines : " << e.what();
            }
        }

    // draw source mesh
        if(m_bMeshSDisplay)
        {          
            try
            {
                m_pSourceMeshMutex->lockForRead();

                if(m_bFillS)
                {
                    drawMeshTriangles(m_oShaderTriangles, m_pOSNRICP->m_oSourceMesh, m_oMVPMatrix, 0.5f, 0.5f, 1.f, m_fOpacitySourceMeshLines);
                }
                else
                {
                    drawMeshLines(m_oShaderLines, m_pOSNRICP->m_oSourceMesh, m_oMVPMatrix, 0.5f, 0.5f, 1.f, m_fOpacitySourceMeshLines);
                }

                m_pSourceMeshMutex->unlock();
            }
            catch(const openglError &e)
            {
                qWarning() << "drawMeshLines source : " << e.what();
            }
        }

    // draw target mesh
        if(m_bMeshTDisplay)
        {
            try
            {

                m_pTargetMeshMutex->lockForRead();

                if(m_bFillT)
                {
                    drawMeshTriangles(m_oShaderTriangles, m_pOSNRICP->m_oTargetMesh, m_oMVPMatrix, 0.5f, 1.0f, 0.5f, m_fOpacitySourceMeshLines);
                }
                else
                {
                    drawMeshLines(m_oShaderLines, m_pOSNRICP->m_oTargetMesh, m_oMVPMatrix, 0.5f, 1.0f, 0.5f, m_fOpacityTargetMeshLines);
                }

                m_pTargetMeshMutex->unlock();
            }
            catch(const openglError &e)
            {
                qWarning() << "drawMeshLines target : " << e.what();
            }
        }                

    // draw vertices normals
        // source
        if(m_bVerticesNormalsSDisplay)
        {
            try
            {
                m_pSourceMeshMutex->lockForRead();
                    drawMeshVerticesNormals(m_oShaderLines, m_pOSNRICP->m_oSourceMesh, m_oMVPMatrix, 0.5f, 0.5f, 0.5f);
                m_pSourceMeshMutex->unlock();
            }
            catch(const openglError &e)
            {
                qWarning() << "drawMeshVerticesNormals : sourceMesh " << e.what();
            }
        }
        // target
        if(m_bVerticesNormalsTDisplay)
        {
            try
            {
                m_pTargetMeshMutex->lockForRead();
                    drawMeshVerticesNormals(m_oShaderLines, m_pOSNRICP->m_oTargetMesh, m_oMVPMatrix, 0.5f, 0.5f, 0.5f);
                m_pTargetMeshMutex->unlock();
            }
            catch(const openglError &e)
            {
                qWarning() << "drawMeshVerticesNormals : targetMesh " << e.what();
            }
        }
    // draw triangles normals
        // source
        if(m_bTrianglesNormalsSDisplay)
        {
            try
            {
                m_pSourceMeshMutex->lockForRead();
                    drawMeshTrianglesNormals(m_oShaderLines, m_pOSNRICP->m_oSourceMesh, m_oMVPMatrix, 0.5f, 0.5f, 0.5f);
                m_pSourceMeshMutex->unlock();
            }
            catch(const openglError &e)
            {
                qWarning() << "drawMeshTrianglesNormals : sourceMesh " << e.what();
            }
        }
        // target

        if(m_bTrianglesNormalsTDisplay)
        {
            try
            {
                m_pTargetMeshMutex->lockForRead();
                    drawMeshTrianglesNormals(m_oShaderLines, m_pOSNRICP->m_oTargetMesh, m_oMVPMatrix, 0.5f, 0.5f, 0.5f);
                m_pTargetMeshMutex->unlock();
            }
            catch(const openglError &e)
            {
                qWarning() << "drawMeshTrianglesNormals : targetMesh " << e.what();
            }
        }

    // draw landmarks
        if(m_bDisplayLandMarks)
        {
            try
            {
                m_pSourceMeshMutex->lockForRead();
                m_pTargetMeshMutex->lockForRead();
                    drawLandMarksPoints(m_oShaderLines, m_pOSNRICP->m_l, *m_pOSNRICP->m_oSourceMesh.cloud(), *m_pOSNRICP->m_oTargetMesh.cloud(), m_oMVPMatrix);
                    drawLandMarksCorr(m_oShaderLines, m_pOSNRICP->m_l, *m_pOSNRICP->m_oSourceMesh.cloud(), *m_pOSNRICP->m_oTargetMesh.cloud(), m_oMVPMatrix, 0.2f, 1.0f, 0.0f);
                m_pSourceMeshMutex->unlock();
                m_pTargetMeshMutex->unlock();
            }
            catch(const openglError &e)
            {
                qWarning() << "drawLandMarks : " << e.what();
            }

        }
}

void SWGLOptimalStepNonRigidICP::drawLandMarksPoints(QGLShaderProgram &oShader, const std::map<uint,uint> mLandMarkCorr, const swCloud::SWCloud &oSource, const swCloud::SWCloud &oTarget, QMatrix4x4 &mvpMatrix)
{
    if(mLandMarkCorr.size() == 0)
    {
        return;
    }

    // bind shader
    if(!oShader.bind())
    {
        throw swShaderGLError();
    }

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

    allocateBuffer(m_indexBuffer,  l_aPointI, mLandMarkCorr.size()  * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aPointV, mLandMarkCorr.size()  * l_i32SizeVertex);

    // set shaders parameters
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

    allocateBuffer(m_vertexBuffer, l_aPointV, mLandMarkCorr.size()  * l_i32SizeVertex);

    // set shaders parameters
    oShader.setUniformValue("uniColor", 0.0f, 1.f, 0.f);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    GLenum l_glError = drawBuffer(m_indexBuffer, m_vertexBuffer, oShader, GL_POINTS);

    delete[] l_aPointI;
    delete[] l_aPointV;

    if(l_glError)
    {
        qWarning() << "drawLandMarksPoints GLError : " << l_glError;
    }
}


void SWGLOptimalStepNonRigidICP::drawLandMarksCorr(QGLShaderProgram &oShader, const std::map<uint,uint> mLandMarkCorr, const swCloud::SWCloud &oSource, const swCloud::SWCloud &oTarget,
                                                   QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    if(mLandMarkCorr.size() == 0)
    {
        return;
    }

    // bind shader
    if(!oShader.bind())
    {
        throw swShaderGLError();
    }

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

    allocateBuffer(m_indexBuffer,  l_aCorrLineI, mLandMarkCorr.size() * 2 * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aCorrLineV, mLandMarkCorr.size() * 2 * l_i32SizeVertex);

    // set shaders parameters
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    GLenum l_glError = drawBuffer(m_indexBuffer, m_vertexBuffer, oShader, GL_LINES);

    delete[] l_aCorrLineI;
    delete[] l_aCorrLineV;

    if(l_glError)
    {
        qWarning() << "drawLandMarksCorr GLError : " << l_glError;
    }
}

void SWGLOptimalStepNonRigidICP::drawCorrLines(QGLShaderProgram &oShader, const swCloud::SWCloud &oSource, const swCloud::SWCloud &oTarget,
                                               const std::vector<uint> &vU, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    // bind shader
    if(!oShader.bind())
    {
        throw swShaderGLError();
    }

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    int l_i32SizeIndex      = sizeof(GLuint);
    int l_i32SizeVertex     = sizeof(float) * 3;

    uint32 *l_aNormalI      = new uint32[oSource.size()*2];
    float  *l_aNormalV      = new float[oSource.size()*2 * 3];

    for(uint ii = 0; ii < oSource.size(); ++ii)
    {
        // fill index buffer
        l_aNormalI[2*ii]   = 2*ii;
        l_aNormalI[2*ii+1] = 2*ii+1;

        // fill vertex buffer
        l_aNormalV[6*ii]   = oSource.coord(0)[ii];
        l_aNormalV[6*ii+1] = oSource.coord(1)[ii];
        l_aNormalV[6*ii+2] = oSource.coord(2)[ii];

        l_aNormalV[6*ii+3] = oTarget.coord(0)[vU[ii]];
        l_aNormalV[6*ii+4] = oTarget.coord(1)[vU[ii]];
        l_aNormalV[6*ii+5] = oTarget.coord(2)[vU[ii]];
    }

    allocateBuffer(m_indexBuffer, l_aNormalI, oSource.size() * 2 * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aNormalV, oSource.size() * 2 * l_i32SizeVertex);

    // set shaders parameters
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    GLenum l_glError = drawBuffer(m_indexBuffer, m_vertexBuffer, oShader, GL_LINES);

    delete[] l_aNormalI;
    delete[] l_aNormalV;

    if(l_glError)
    {
        qWarning() << "drawCorrLines GLError : " << l_glError;
    }
}

void SWGLOptimalStepNonRigidICP::drawCloud(QGLShaderProgram &oShader, const swCloud::SWCloud &oCloud, cfloat fSizePoint, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
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

    allocateBuffer(m_indexBuffer,  l_aCloudI, oCloud.size() * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aCloudV, oCloud.size() * l_i32SizeVertex);

    // set size of the points
    glPointSize(fSizePoint);

    // set uniform values parameters
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    GLenum l_glError = drawBuffer(m_indexBuffer, m_vertexBuffer, oShader, GL_POINTS);

    delete[] l_aCloudI;
    delete[] l_aCloudV;

    if(l_glError)
    {
        qWarning() << "drawCloud GLError : " << l_glError;
    }
}

void SWGLOptimalStepNonRigidICP::drawSourceCloud(QGLShaderProgram &oShader, const swCloud::SWCloud &oCloud, cfloat fSizePoint, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
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

            l_aCloudC[3*ii]   = l_fCol[0];
            l_aCloudC[3*ii+1] = l_fCol[1];
            l_aCloudC[3*ii+2] = l_fCol[2];
        }

    allocateBuffer(m_indexBuffer,  l_aCloudI, oCloud.size() * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aCloudV, oCloud.size() * l_i32SizeVertex);
    allocateBuffer(m_colorBuffer,  l_aCloudC, oCloud.size() * l_i32SizeVertex);

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
        qWarning() << "drawSourceCloud GLError : " << l_glError;
    }
}

void SWGLOptimalStepNonRigidICP::drawMeshTriangles(QGLShaderProgram &oShader, swMesh::SWMesh &oMesh, QMatrix4x4 &mvpMatrix,
                          cfloat fR, cfloat fG, cfloat fB, cfloat fOpacity)
{
    // bind shader
    if(!oShader.bind())
    {
        throw swExcept::swShaderGLError();
    }

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    float  *l_aFVertexBuffer   = oMesh.vertexBuffer();
    uint32 *l_aUI32IndexBuffer = oMesh.indexVertexTriangleBuffer();
    float  *l_aFNormalBuffer   = oMesh.normalBuffer();
    float  *l_aFTextureBuffer  = oMesh.textureBuffer();

    // allocate QGL buffers
    allocateBuffer(m_vertexBuffer,  l_aFVertexBuffer,   oMesh.pointsNumber() *     3 * sizeof(float) );
    allocateBuffer(m_indexBuffer,   l_aUI32IndexBuffer, oMesh.trianglesNumber() *  3 * sizeof(GLuint) );
    allocateBuffer(m_normalBuffer,  l_aFNormalBuffer,   oMesh.pointsNumber() *     3 * sizeof(float) );
    allocateBuffer(m_textureBuffer, l_aFTextureBuffer,  oMesh.pointsNumber() *     2 * sizeof(float) );

    // set uniform values parameters
    oShader.setUniformValue("mvpMatrix",    mvpMatrix);
    oShader.setUniformValue("opacity",      fOpacity);
    oShader.setUniformValue("applyTexture", false);

    // draw primitives
//    GLenum l_glError = drawBuffer(m_indexBuffer, m_vertexBuffer, m_normalBuffer, oShader, GL_TRIANGLES);
    GLenum l_glError =drawBufferWithTexture(m_indexBuffer, m_vertexBuffer, m_textureBuffer, m_normalBuffer, oShader, GL_TRIANGLES);

    delete[] l_aFVertexBuffer;
    delete[] l_aUI32IndexBuffer;
    delete[] l_aFNormalBuffer;
    delete[] l_aFTextureBuffer;

    if(l_glError)
    {
        qWarning() << "drawMeshTriangles GLError : " << l_glError;
    }
}


void SWGLOptimalStepNonRigidICP::drawMeshLines(QGLShaderProgram &oShader, SWMesh &oMesh, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b, cfloat fOpacity)
{
    // bind shader
    if(!oShader.bind())
    {
        throw swShaderGLError();
    }

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
    int l_i32SizeFaceIndex  = sizeof(GLuint) * 3;
    int l_i32SizeVertex     = sizeof(float) * 3;
    uint32 *l_aFaceI        = oMesh.indexVertexTriangleBuffer();
    float  *l_aCloudV       = oMesh.cloud()->vertexBuffer();

    allocateBuffer(m_indexBuffer,  l_aFaceI,  oMesh.trianglesNumber() * l_i32SizeFaceIndex);
    allocateBuffer(m_vertexBuffer, l_aCloudV, oMesh.cloud()->size()   * l_i32SizeVertex);

    // set uniform values parameters
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", fOpacity);

    // draw primitives
    GLenum l_glError =  drawBuffer(m_indexBuffer, m_vertexBuffer, oShader, GL_TRIANGLES);

    delete[] l_aFaceI;
    delete[] l_aCloudV;

    if(l_glError)
    {
        qWarning() << "drawMeshLines GLError : " << l_glError;
    }
}


void SWGLOptimalStepNonRigidICP::drawMeshVerticesNormals(QGLShaderProgram &oShader, const swMesh::SWMesh &oMesh, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    float l_fCoeffLenghtNormal = 0.01f;

    // bind shader
    if(!oShader.bind())
    {
        throw swShaderGLError();
        return;
    }

    if(!oMesh.isVerticesNormals())
    {
        cerr << "drawMeshVerticesNormals : No vertices normals in the mesh" << endl;
        return;
    }

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
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

    allocateBuffer(m_indexBuffer,  l_aNormalI, oMesh.pointsNumber() * 2 * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aNormalV, oMesh.pointsNumber() * 2 * l_i32SizeVertex);

    // set shaders parameters
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    GLenum l_glError = drawBuffer(m_indexBuffer, m_vertexBuffer, oShader, GL_LINES);

    delete[] l_aNormalI;
    delete[] l_aNormalV;

    if(l_glError)
    {
        qWarning() << "drawMeshVerticesNormals GLError : " << l_glError;
    }
}

void SWGLOptimalStepNonRigidICP::drawMeshTrianglesNormals(QGLShaderProgram &oShader, const SWMesh &oMesh, QMatrix4x4 &mvpMatrix, cfloat r, cfloat g, cfloat b)
{
    float l_fCoeffLenghtNormal = 0.01f;

    // bind shader
    if(!oShader.bind())
    {
        throw swShaderGLError();
    }

    if(!oMesh.isTrianglesNormals())
    {
        cerr << "drawMeshTrianglesNormals : No triangles normals in the mesh" << endl;
        return;
    }

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // release buffers
    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // allocate buffers
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

    allocateBuffer(m_indexBuffer,  l_aNormalI, oMesh.trianglesNumber() * 2 * l_i32SizeIndex);
    allocateBuffer(m_vertexBuffer, l_aNormalV, oMesh.trianglesNumber() * 2 * l_i32SizeVertex);

    // set shaders parameters
    oShader.setUniformValue("uniColor", r, g, b);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("opacity", m_fDefaultOpacity);

    // draw primitives
    GLenum l_glError = drawBuffer(m_indexBuffer, m_vertexBuffer, oShader, GL_LINES);

    delete[] l_aNormalI;
    delete[] l_aNormalV;

    if(l_glError)
    {
        qWarning() << "drawMeshTrianglesNormals GLError : " << l_glError;
    }
}





void SWGLOptimalStepNonRigidICP::resetMorphing()
{
    if(m_pSourceMesh && m_pTargetMesh)
    {
        m_pOSNRICP.reset();
        m_pOSNRICP = SWOptimalStepNonRigidICPPtr(new SWOptimalStepNonRigidICP(*m_pSourceMesh.get(), *m_pTargetMesh.get(), m_sPathStasmFileSource, m_sPathStasmFileTarget));

        // apply transformation
            transformTarget(false);

        // init morphing
            m_pOSNRICP->computeCorrespondences();
            m_pOSNRICP->computeDistanceWeights();
            updateGL();
    }
}

void SWGLOptimalStepNonRigidICP::resetMorphingWithNewCorr()
{
    // delete data
        m_pOSNRICP.reset();

    // reset the morphing
        resetMorphing();
}

void SWGLOptimalStepNonRigidICP::refreshDisplay()
{
    updateGL();
}

void SWGLOptimalStepNonRigidICP::resetMorphingWithNewMeshes()
{
    // delete data
        m_pOSNRICP.reset();
        m_pSourceMesh.reset();
        m_pTargetMesh.reset();

    // load new meshes
        m_pSourceMesh = SWMeshPtr(new swMesh::SWMesh(m_sPathSourceMesh));
        m_pTargetMesh = SWMeshPtr(new swMesh::SWMesh(m_sPathTargetMesh));

    // find scaling value
        swCloud::SWCloudBBox l_oSourceCloudBBox = m_pSourceMesh->cloud()->bBox();
        swCloud::SWCloudBBox l_oTargetCloudBBox = m_pTargetMesh->cloud()->bBox();
        vector<float> l_vPt1,l_vPt2;
        l_vPt1.push_back(l_oSourceCloudBBox.m_fMinX);
        l_vPt1.push_back(l_oSourceCloudBBox.m_fMinY);
        l_vPt1.push_back(l_oSourceCloudBBox.m_fMinZ);
        l_vPt2.push_back(l_oSourceCloudBBox.m_fMaxX);
        l_vPt2.push_back(l_oSourceCloudBBox.m_fMaxY);
        l_vPt2.push_back(l_oSourceCloudBBox.m_fMaxZ);
        float l_fD1 = swUtil::norm(swUtil::vec(l_vPt1,l_vPt2));

        l_vPt1.clear(); l_vPt2.clear();
        l_vPt1.push_back(l_oTargetCloudBBox.m_fMinX);
        l_vPt1.push_back(l_oTargetCloudBBox.m_fMinY);
        l_vPt1.push_back(l_oTargetCloudBBox.m_fMinZ);
        l_vPt2.push_back(l_oTargetCloudBBox.m_fMaxX);
        l_vPt2.push_back(l_oTargetCloudBBox.m_fMaxY);
        l_vPt2.push_back(l_oTargetCloudBBox.m_fMaxZ);
        float l_fD2 = swUtil::norm(swUtil::vec(l_vPt1,l_vPt2));

    // apply scaling value
        if(l_fD2/l_fD1 > 1000)
        {
             m_pTargetMesh->scale(0.001f);
        }

    // align source and target meshes with the nose
        alignWithNose(*m_pSourceMesh.get(), *m_pTargetMesh.get());

    // reset the morphing
        resetMorphing();
}

void SWGLOptimalStepNonRigidICP::setSourceMesh(const QString &sPathSource)
{
    m_sPathSourceMesh = sPathSource.toStdString();
}

void SWGLOptimalStepNonRigidICP::setTargetMesh(const QString &sPathTarget)
{
    m_sPathTargetMesh = sPathTarget.toStdString();
}

void SWGLOptimalStepNonRigidICP::setSourceCorr(const QString &sPathSource)
{
    m_sPathStasmFileSource = sPathSource.toStdString();
}

void SWGLOptimalStepNonRigidICP::setTargetCorr(const QString &sPathTarget)
{
    m_sPathStasmFileTarget = sPathTarget.toStdString();
}



// SWOOZ
#include "SWGLWidget.h"
#include "interface/SWGLUtility.h"

// Std
#include <iostream>

// Qt
#include <QFileInfo>
#include <QTransform>

// MOC
#include "moc_SWGLWidget.cpp"


SWGLWidget::SWGLWidget(  QGLContext *oContext, QWidget* oParent) :
    QGLWidget( oContext, oParent ), m_glContext(oContext),  m_oTimer(new QBasicTimer)
{
    // miscellanous
        m_bVerbose = false;

    // default perspective
        m_rZNear = 0.01;
        m_rZFar  = 1000.0;
        m_rFOV   = 60.0;

	// init camera parameters
        m_bIsClickedMouse = false;
        m_bMidClick       = false;
        m_pCamera         = new SWQtCamera(QVector3D(0.f, 0.f, -0.15f), QVector3D(0.f, 0.f,  1000.f), QVector3D(0.f, 1.f,  0.f));
	
	// init gl parameters
        if(m_bVerbose)
        {
            qDebug() << "OpenGl " << QGLWidget::format().majorVersion() << "," << QGLWidget::format().minorVersion();
        }

    // init point size
        m_glFSizePoint  = 2.0f;
	
	// init qt parameters
        setFocusPolicy( Qt::StrongFocus ); // set strong focus policy to be able to get the key events
}

SWGLWidget::~SWGLWidget()
{
    deleteAndNullify(m_oTimer);
    deleteAndNullify(m_pCamera);
}

void SWGLWidget::resetCamera()
{
    m_pCamera->reset();
//    updateGL();
    update();
}


void SWGLWidget::initTextures()
{
    m_cubeMapTextureNegX = QImage("../data/images/textures/cubeMap/citadella/negx.bmp");
    m_cubeMapTextureNegY = QImage("../data/images/textures/cubeMap/citadella/negy.bmp");
    m_cubeMapTextureNegZ = QImage("../data/images/textures/cubeMap/citadella/negz.bmp");
    m_cubeMapTexturePosX = QImage("../data/images/textures/cubeMap/citadella/posx.bmp");
    m_cubeMapTexturePosY = QImage("../data/images/textures/cubeMap/citadella/posy.bmp");
    m_cubeMapTexturePosZ = QImage("../data/images/textures/cubeMap/citadella/posz.bmp");

    m_cubeMapTextureNegXLocation = bindTexture(m_cubeMapTextureNegX);
    checkGlError(true);
    m_cubeMapTexturePosXLocation = bindTexture(m_cubeMapTexturePosX);
    checkGlError(true);
    m_cubeMapTexturePosZLocation = bindTexture(m_cubeMapTexturePosZ);
    checkGlError(true);
    m_cubeMapTextureNegZLocation = bindTexture(m_cubeMapTextureNegZ);
    checkGlError(true);
    m_cubeMapTexturePosYLocation = bindTexture(m_cubeMapTexturePosY);
    checkGlError(true);
    m_cubeMapTextureNegYLocation = bindTexture(m_cubeMapTextureNegY);
    checkGlError(true);
}


void SWGLWidget::setCamera(const QVector3D &oEyePosition, const QVector3D &oLookAt, const QVector3D &oUp, cbool bUpdateGL)
{
    m_pCamera->set(oEyePosition, oLookAt, oUp);

    if(bUpdateGL)
    {
//        updateGL();
        update();
    }
}

void SWGLWidget::setCameraInitial(const QVector3D &oEyePositionInitial, const QVector3D &oLookAtInitial, const QVector3D &oUpInitial)
{
    m_pCamera->setInitial(oEyePositionInitial, oLookAtInitial, oUpInitial);
}

void SWGLWidget::setCameraMode(const SWQtCamera::CameraMode oCameraMode)
{    
    m_pCamera->setCameraMode(oCameraMode);
}

void SWGLWidget::setFOV(const double dFOV)
{
    m_oParamMutex.lockForWrite();
        if(dFOV > 10 && dFOV < 120)
        {
            m_rFOV  = dFOV;
        }
    m_oParamMutex.unlock();


    resizeGL(this->width(), this->height());
}


void SWGLWidget::setPerspective(const qreal rFOV, const qreal rZNear, const qreal rZFar)
{
    m_oParamMutex.lockForWrite();
        if(rFOV > 10 && rFOV < 120)
        {
            m_rFOV  = rFOV;
        }
        if(rZNear > 0 && rZNear < rZFar)
        {
            m_rZNear = rZNear;
            m_rZFar  = rZFar;
        }
    m_oParamMutex.unlock();


    resizeGL(this->width(), this->height());
}

void SWGLWidget::initShaders( const QString& vertexShaderPath, const QString& fragmentShaderPath, QGLShaderProgram &oShader, cbool bBindShader)
{
	// load & compile vertex shader
    if (!oShader.addShaderFromSourceFile( QGLShader::Vertex, vertexShaderPath))
	{
        qWarning() << oShader.log();
	}

	// load & compile fragment shader
    if ( !oShader.addShaderFromSourceFile( QGLShader::Fragment, fragmentShaderPath ))
	{
        qWarning() << oShader.log();
	}

	// link shader pipeline
    if ( !oShader.link() )
	{
        qWarning() << "Could not link shader program : " << oShader.log();
	}
	
    if(bBindShader)
    {
        // bind shader pipeline
        if (!oShader.bind())
        {
            qWarning() << "Could not bind shader program : " << oShader.log();
        }
    }
}

void SWGLWidget::mousePressEvent(QMouseEvent *e)
{
	m_bIsClickedMouse = true;	   
    m_bMidClick       = (e->button() == Qt::MidButton);   
    m_oCursorLastClickPosition = e->pos();

    // compute click ray
    {
        QVector3D point(m_oCursorLastClickPosition.x(),m_oSize.height()-m_oCursorLastClickPosition.y(), 0.0);
        QVector3D unProjected;
        glhUnProject (point, m_oMVPMatrix, unProjected);
        m_rayStart = unProjected;
        point.setZ(1.0);
        glhUnProject (point, m_oMVPMatrix, unProjected);
        m_rayEnd =  unProjected;
    }

    emit rayClick();

//    updateGL();
    update();
}

void SWGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
	m_bIsClickedMouse = false;
    m_bMidClick       = false;
}

void SWGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(m_bIsClickedMouse)
	{
		QPoint l_oMiddle(m_oSize.width()/2, m_oSize.height()/2);
        QPoint l_oDiff = l_oMiddle - QPoint(e->x(), e->y());

        if(!m_bMidClick)
        {
            m_oCurrentRotation = QVector3D(-l_oDiff.y(), l_oDiff.x(), 0);
            m_pCamera->rotate(m_oCurrentRotation.x(), m_oCurrentRotation.y(), m_oCurrentRotation.z(), m_oCurrentRotation.length()/300);
        }
        else
        {
            if(e->x() < l_oMiddle.x())
            {
                if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
                {
                    m_pCamera->moveLeft(0.00003f * (l_oMiddle.x() - e->x()));
                }
                else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
                {
                    m_pCamera->moveLeft(0.003f * (l_oMiddle.x() - e->x()));
                }
            }
            else if(e->x() > l_oMiddle.x())
            {
                if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
                {
                    m_pCamera->moveRight(0.00003f * (e->x() - l_oMiddle.x()));
                }
                else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
                {
                    m_pCamera->moveRight(0.003f * (e->x() - l_oMiddle.x()));
                }
            }

            if(e->y() < l_oMiddle.y())
            {
                if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
                {
                    m_pCamera->moveUp(0.000015f * (l_oMiddle.y() - e->y()));
                }
                else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
                {
                    m_pCamera->moveUp(0.003f * (l_oMiddle.y() - e->y()));
                }
            }
            else if(e->y() > l_oMiddle.y())
            {
                if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
                {
                    m_pCamera->moveDown(0.000015f * (e->y() - l_oMiddle.y()));
                }
                else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
                {
                    m_pCamera->moveDown(0.003f * (e->y() - l_oMiddle.y()));
                }
            }
        }

//        updateGL();
        update();
	}		
}

void SWGLWidget::wheelEvent(QWheelEvent *e)
{	   
    if(e->delta() > 0)
    {
        if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
        {
            m_pCamera->moveForeward( e->delta()/120 * 0.02f);
        }
        else if (m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
        {
            m_pCamera->moveForeward( e->delta()/120 * 0.05f);
        }
    }
    else
    {
        if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
        {
            m_pCamera->moveBackward(-e->delta()/120 * 0.02f);
        }
        else if (m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
        {
            m_pCamera->moveBackward(-e->delta()/120 * 0.05f);
        }
    }

//	updateGL();
    update();
}

void SWGLWidget::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
        case Qt::Key_Escape:
            emit disableFullScreen();
        break;
        case Qt::Key_F:
            emit enableFullScreen();
        break;
        case Qt::Key_Enter:
            emit enterKeyPressed();
        break;
        case Qt::Key_Return:
            emit enterKeyPressed();
        break;
        case Qt::Key_Space:
            emit spaceKeyPressed();
        break;
		case Qt::Key_Plus:  // increase gl point size
			m_glFSizePoint < 10 ? m_glFSizePoint += 1 : m_glFSizePoint = 10;		
		break;
		case Qt::Key_Minus: // decrease gl point size
			m_glFSizePoint > 1  ? m_glFSizePoint -= 1 : m_glFSizePoint = 1;		
		break;	
		case Qt::Key_R:	    // reset camera view 
            m_pCamera->reset();
        break;
        case Qt::Key_Left:
            if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
            {
                m_pCamera->moveLeft(0.05f);
            }
            else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
            {
                m_pCamera->moveLeft(3.f);
            }
        break;
        case Qt::Key_Right:
            if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
            {
                m_pCamera->moveRight(0.05f);
            }
            else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
            {
                m_pCamera->moveRight(3.f);
            }
        break;
        case Qt::Key_Up:
            if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
            {
                m_pCamera->moveForeward(0.05f);
            }
            else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
            {
                m_pCamera->moveForeward(0.1f);
            }
        break;
        case Qt::Key_Down:
            if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
            {
                m_pCamera->moveBackward(0.06f);
            }
            else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
            {
                m_pCamera->moveBackward(0.1f);
            }
        break;
		case Qt::Key_Z:
            if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
            {
                m_pCamera->rotateY(-3.f);
            }
            else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
            {
                m_pCamera->moveUp(3.f);
            }
		break;		
		case Qt::Key_Q:
            if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
            {
                m_pCamera->rotateX(3.f);
            }
            else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
            {
                m_pCamera->moveLeft(3.f);
            }
		break;		
		case Qt::Key_S:
            if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
            {
                m_pCamera->rotateY(3.f);
            }
            else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
            {
                m_pCamera->moveDown(3.f);
            }
		break;		
		case Qt::Key_D:
            if(m_pCamera->cameraMode() == SWQtCamera::FPS_CAMERA)
            {
                m_pCamera->rotateX(-3.f);
            }
            else if(m_pCamera->cameraMode() == SWQtCamera::TRACKBALL_CAMERA)
            {
                m_pCamera->moveRight(3.f);
            }
		break;				
		
	}

//	updateGL();
    update();
}

void SWGLWidget::timerEvent(QTimerEvent *e)
{
	// parameter "e" not used in the body of the function
	Q_UNUSED(e);
	
    if(m_bIsClickedMouse && !m_bMidClick)
    {
        m_pCamera->rotate(m_oCurrentRotation.x(), m_oCurrentRotation.y(), m_oCurrentRotation.z(), m_oCurrentRotation.length()/300);
    }

//    updateGL();
    update();
}

void SWGLWidget::resizeGL( int i32Width, int i32Height )
{
    m_oParamMutex.lockForRead();
        qreal l_rFOV = m_rFOV;
        qreal l_rZNear = m_rZNear;
        qreal l_rZFar = m_rZFar;
    m_oParamMutex.unlock();


	// set OpenGL viewport to cover whole widget
	glViewport(0, 0, i32Width, i32Height);

	m_oSize = QSize(i32Width, i32Height);
	
	// calculate aspect ratio
	qreal aspect = (qreal)i32Width / ((qreal)i32Height?i32Height:1);
	
	// reset projection
    m_oProjectionMatrix.setToIdentity();

	// set perspective projection        
    m_oProjectionMatrix.perspective(l_rFOV, aspect, l_rZNear, l_rZFar);
//    void ortho(qreal left, qreal right, qreal bottom, qreal top, qreal nearPlane, qreal farPlane);
//    void frustum(qreal left, qreal right, qreal bottom, qreal top, qreal nearPlane, qreal farPlane);

//    updateGL();
    update();
}


void  SWGLWidget::drawAxes(QGLShaderProgram &oShader, QMatrix4x4 &mvpMatrix, cfloat fScale, const QVector3D &oOrigine)
{
    oShader.bind();
        checkGlError(true);

    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        checkGlError(true);

    // init buffers
    QGLBuffer l_vertexBuffer, l_indexBuffer;
    initIndexBuffer(l_indexBuffer);
    initVertexBuffer(l_vertexBuffer);

    float l_vertexBufferA[] = {oOrigine.x(), oOrigine.y(), oOrigine.z(),
                              oOrigine.x(), oOrigine.y(), oOrigine.z()+1.f*fScale,
                              oOrigine.x(), oOrigine.y()+1.f*fScale, oOrigine.z(),
                              oOrigine.x()+1.f*fScale, oOrigine.y(), oOrigine.z()};

    uint l_indexBufferLine1[] = {0,1};
    uint l_indexBufferLine2[] = {0,2};
    uint l_indexBufferLine3[] = {0,3};

    // allocate QGL buffers
    allocateBuffer(l_vertexBuffer, l_vertexBufferA, 4 *  3 * sizeof(float) );

    // set mvp matrix uniform value
    oShader.setUniformValue("applyTransformation", false);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("displayMode", 1);

    // set color uniform value for the current line
    oShader.setUniformValue("uniColor", 1.f, 0.f, 0.f);

    allocateBuffer(l_indexBuffer, l_indexBufferLine1, 2 * sizeof(GLuint) );
    drawBuffer(l_indexBuffer, l_vertexBuffer, oShader, GL_LINES);

    oShader.setUniformValue("uniColor", 0.f, 1.f, 0.f);
    allocateBuffer(l_indexBuffer, l_indexBufferLine2, 2 * sizeof(GLuint) );
    drawBuffer(l_indexBuffer, l_vertexBuffer, oShader, GL_LINES);

    // set color uniform value for the current line
    oShader.setUniformValue("uniColor", 0.f, 0.f, 1.f);
    allocateBuffer(l_indexBuffer, l_indexBufferLine3, 2 * sizeof(GLuint) );
    drawBuffer(l_indexBuffer, l_vertexBuffer, oShader, GL_LINES);

    oShader.release();
        checkGlError(true);
}

void SWGLWidget::drawCubeMap(QGLShaderProgram &oShader, QMatrix4x4 &mvpMatrix)
{
    oShader.bind();
        checkGlError(true);

    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // enable texture
    glEnable(GL_TEXTURE_2D);
        checkGlError(true);

    glDepthMask (GL_FALSE);
        checkGlError(true);

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        checkGlError(true);

    float d = 1000.f;
    float l_vertexFace1[] = {-d,d,-d, d,d,-d, d,-d,-d ,-d,-d,-d};
    float l_vertexFace2[] = {d,d,-d, d,d,d, d,-d,d ,d,-d,-d};
    float l_vertexFace3[] = {d,d,d, -d,d,d, -d,-d,d ,d,-d,d};
    float l_vertexFace4[] = {-d,d,d, -d,d,-d, -d,-d,-d ,-d,-d,d};
    float l_vertexFace5[] = {-d,d,-d, -d,d,d, d,d,d ,d,d,-d};
    float l_vertexFace6[] = {d,-d,-d,  d,-d,d, -d,-d,d ,-d,-d,-d};

    uint32 l_indexFace[] = {0,1,2, 0,2,3};
    float l_textureFace[] = {0.f,1.f, 1.f,1.f, 1.f,0.f, 0.f,0.f};

    oShader.setUniformValue("applyTransformation", false);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("displayMode", 2);

    allocateBuffer(m_textureBufferCubeMap, l_textureFace, 6 * sizeof(float));
    allocateBuffer(m_indexBufferCubeMap, l_indexFace, 6 * sizeof(GLuint) );

    qDebug() << m_cubeMapTextureNegXLocation << " " << m_cubeMapTexturePosZLocation << " " << m_cubeMapTexturePosXLocation << " " << m_cubeMapTextureNegZLocation;
    allocateBuffer(m_vertexBufferCubeMap, l_vertexFace1, 8 *  3 * sizeof(float) );
    glBindTexture(GL_TEXTURE_2D, m_cubeMapTextureNegXLocation);
        checkGlError(true);
    drawBufferWithTexture(m_indexBufferCubeMap, m_vertexBufferCubeMap, m_textureBufferCubeMap, oShader, GL_TRIANGLES);

    allocateBuffer(m_vertexBufferCubeMap, l_vertexFace2, 8 *  3 * sizeof(float) );
    glBindTexture(GL_TEXTURE_2D, m_cubeMapTexturePosZLocation);
        checkGlError(true);
    drawBufferWithTexture(m_indexBufferCubeMap, m_vertexBufferCubeMap, m_textureBufferCubeMap, oShader, GL_TRIANGLES);

    allocateBuffer(m_vertexBufferCubeMap, l_vertexFace3, 8 *  3 * sizeof(float) );
    glBindTexture(GL_TEXTURE_2D, m_cubeMapTexturePosXLocation);
        checkGlError(true);
    drawBufferWithTexture(m_indexBufferCubeMap, m_vertexBufferCubeMap, m_textureBufferCubeMap, oShader, GL_TRIANGLES);

    allocateBuffer(m_vertexBufferCubeMap, l_vertexFace4, 8 *  3 * sizeof(float) );
    glBindTexture(GL_TEXTURE_2D, m_cubeMapTextureNegZLocation);
        checkGlError(true);
    drawBufferWithTexture(m_indexBufferCubeMap, m_vertexBufferCubeMap, m_textureBufferCubeMap, oShader, GL_TRIANGLES);

    allocateBuffer(m_vertexBufferCubeMap, l_vertexFace5, 8 *  3 * sizeof(float) );
    glBindTexture(GL_TEXTURE_2D, m_cubeMapTexturePosYLocation);
        checkGlError(true);
    drawBufferWithTexture(m_indexBufferCubeMap, m_vertexBufferCubeMap, m_textureBufferCubeMap, oShader, GL_TRIANGLES);

    allocateBuffer(m_vertexBufferCubeMap, l_vertexFace6, 8 *  3 * sizeof(float) );
    glBindTexture(GL_TEXTURE_2D, m_cubeMapTextureNegYLocation);
        checkGlError(true);
    drawBufferWithTexture(m_indexBufferCubeMap, m_vertexBufferCubeMap, m_textureBufferCubeMap, oShader, GL_TRIANGLES);

    oShader.release();
        checkGlError(true);

    // enable texture
    glDisable(GL_TEXTURE_2D);
        checkGlError(true);

    glDepthMask (GL_TRUE);
        checkGlError(true);
}

void SWGLWidget::glhUnProject(const QVector3D &point, const QMatrix4x4 &mvpMatrix, QVector3D &pointUnprojected)
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    QMatrix4x4 inv = mvpMatrix.inverted();
    QVector4D in,out;
    in.setX((point.x()-viewport[0])/ viewport[2]*2.0-1.0);
    in.setY((point.y()-viewport[1])/ viewport[3]*2.0-1.0);
    in.setZ(2.0*point.z()-1.0);
    in.setW(1.0);

   out = inv * in;
   out.setW(1.0/out.w());

   pointUnprojected.setX(out.x()*out.w());
   pointUnprojected.setY(out.y()*out.w());
   pointUnprojected.setZ(out.z()*out.w());
}

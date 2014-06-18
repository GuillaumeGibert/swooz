
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
        m_rZFar  = 100.0;
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

void SWGLWidget::resetCamera(const QVector3D &oEyePosition, const QVector3D &oLookAt, const QVector3D &oUp)
{
    deleteAndNullify(m_pCamera);

    m_pCamera = new SWQtCamera(oEyePosition, oLookAt, oUp);
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
                m_pCamera->moveLeft(0.0001f * (l_oMiddle.x() - e->x()));
            }
            else if(e->x() > l_oMiddle.x())
            {
                m_pCamera->moveRight(0.0001f * (e->x() - l_oMiddle.x()));
            }

            if(e->y() < l_oMiddle.y())
            {
                m_pCamera->moveUp(0.0001f * (l_oMiddle.y() - e->y()));
            }
            else if(e->y() > l_oMiddle.y())
            {
                m_pCamera->moveDown(0.0001f * (e->y() - l_oMiddle.y()));
            }
        }

        updateGL();
	}		
}

void SWGLWidget::wheelEvent(QWheelEvent *e)
{	   
    if(e->delta() > 0)
    {
        m_pCamera->moveForeward( e->delta()/120 * 0.02f);
    }
    else
    {
        m_pCamera->moveBackward(-e->delta()/120 * 0.02f);
    }

	updateGL();
}

void SWGLWidget::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
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
            m_pCamera->moveLeft(0.05f);
        break;
        case Qt::Key_Right:
            m_pCamera->moveRight(0.05f);
        break;
        case Qt::Key_Up:
            m_pCamera->moveForeward(0.05f);
        break;
        case Qt::Key_Down:
            m_pCamera->moveBackward(0.05f);
        break;
		case Qt::Key_Z:
            m_pCamera->rotateY(-3.f);
		break;		
		case Qt::Key_Q:
            m_pCamera->rotateX(3.f);
		break;		
		case Qt::Key_S:
            m_pCamera->rotateY(3.f);
		break;		
		case Qt::Key_D:
            m_pCamera->rotateX(-3.f);
		break;				
		
	}

	updateGL();
}

void SWGLWidget::timerEvent(QTimerEvent *e)
{
	// parameter "e" not used in the body of the function
	Q_UNUSED(e);
	
    if(m_bIsClickedMouse && !m_bMidClick)
    {
        m_pCamera->rotate(m_oCurrentRotation.x(), m_oCurrentRotation.y(), m_oCurrentRotation.z(), m_oCurrentRotation.length()/300);
    }

    updateGL();
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

    updateGL();
}

void  SWGLWidget::drawAxes(QGLShaderProgram &oShader, QMatrix4x4 &mvpMatrix, cfloat fScale, const QVector3D &oOrigine)
{
    oShader.bind();
        checkGlError();

    QGLBuffer::release(QGLBuffer::VertexBuffer);
    QGLBuffer::release(QGLBuffer::IndexBuffer);

    // set mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        checkGlError();

    // init buffers
    QGLBuffer l_vertexBuffer, l_indexBuffer;
    initIndexBuffer(l_indexBuffer);
    initVertexBuffer(l_vertexBuffer);

    float  *l_aFVertexBuffer   = new float[12];
    l_aFVertexBuffer[0] = oOrigine.x();
    l_aFVertexBuffer[1] = oOrigine.y();
    l_aFVertexBuffer[2] = oOrigine.z();

    l_aFVertexBuffer[3] = oOrigine.x();
    l_aFVertexBuffer[4] = oOrigine.y();
    l_aFVertexBuffer[5] = oOrigine.z() +1.f*fScale;

    l_aFVertexBuffer[6] = oOrigine.x();
    l_aFVertexBuffer[7] = oOrigine.y() +1.f*fScale;
    l_aFVertexBuffer[8] = oOrigine.z();

    l_aFVertexBuffer[9] = oOrigine.x() +1.f*fScale;
    l_aFVertexBuffer[10]= oOrigine.y();
    l_aFVertexBuffer[11]= oOrigine.z();

    uint32 *l_aUI32IndexBuffer = new uint[2];
    l_aUI32IndexBuffer[0] = 0;
    l_aUI32IndexBuffer[1] = 1;

    // allocate QGL buffers
    allocateBuffer(l_vertexBuffer, l_aFVertexBuffer, 4 *  3 * sizeof(float) );
    allocateBuffer(l_indexBuffer, l_aUI32IndexBuffer, 2 * sizeof(GLuint) );
    deleteAndNullifyArray(l_aFVertexBuffer);
    deleteAndNullifyArray(l_aUI32IndexBuffer);

    // set mvp matrix uniform value
    oShader.setUniformValue("applyTransformation", false);
    oShader.setUniformValue("mvpMatrix", mvpMatrix);
    oShader.setUniformValue("displayMode", 1);

    // set color uniform value for the current line
    oShader.setUniformValue("uniColor", 1.f, 0.f, 0.f);

    drawBuffer(l_indexBuffer, l_vertexBuffer, oShader, GL_LINES);

    l_aUI32IndexBuffer = new uint[6];
    l_aUI32IndexBuffer[0] = 0;
    l_aUI32IndexBuffer[1] = 2;

    // allocate QGL buffers
    allocateBuffer(l_indexBuffer, l_aUI32IndexBuffer, 2 * sizeof(GLuint) );
    deleteAndNullifyArray(l_aUI32IndexBuffer);

    // set color uniform value for the current line
    oShader.setUniformValue("uniColor", 0.f, 1.f, 0.f);
    drawBuffer(l_indexBuffer, l_vertexBuffer, oShader, GL_LINES);

    l_aUI32IndexBuffer = new uint[6];
    l_aUI32IndexBuffer[0] = 0;
    l_aUI32IndexBuffer[1] = 3;

    // allocate QGL buffers
    allocateBuffer(l_indexBuffer, l_aUI32IndexBuffer, 2 * sizeof(GLuint) );
    deleteAndNullifyArray(l_aUI32IndexBuffer);

    // set color uniform value for the current line
    oShader.setUniformValue("uniColor", 0.f, 0.f, 1.f);
    drawBuffer(l_indexBuffer, l_vertexBuffer, oShader, GL_LINES);

    oShader.release();
        checkGlError();
}

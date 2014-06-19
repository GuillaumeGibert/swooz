
/**
 * \file SWGLWidget.h
 * \brief Defines SWGLWidget
 * \author Florian Lance
 * \date 08/03/13
 */

#ifndef _SWGLWIDGET_
#define _SWGLWIDGET_

// Swooz
#include "swExceptions.h"
#include "interface/SWQtCamera.h"

// Qt
#include <QtGui>
#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QGLWidget>
#include <QGLContext>
#include <QGLBuffer>


/**
 * \class SWGLWidget
 * \brief Base class for rendering opengl in Qt widget.
 * \author Florian Lance
 * \date 08/03/13
 */
class SWGLWidget : public QGLWidget
{
	Q_OBJECT
	
	public:
		
		/**
		 * \brief constructor of SWGLWidget
         * \param [in] oContext : Opengl rendering context.
         * \param [in] oParent  : parent widget.
		 */	
        SWGLWidget(QGLContext *oContext, QWidget* oParent = 0);

		/**
		 * \brief destructor of SWGLWidget
		 */		
		~SWGLWidget();

        /**
         * \brief Set camera view.
         * \param [in] oEyePosition : eye position
         * \param [in] oLookAt      : view direction
         * \param [in] oUp          : up direction
         */
        void setCamera(const QVector3D &oEyePosition, const QVector3D &oLookAt, const QVector3D &oUp = QVector3D(0.f, 1.f,  0.f));


        /**
         * @brief setPerspective
         * @param [in] rFOV   : ...
         * @param [in] rZNear : ...
         * @param [in] rZFar  : ...
         */
        void setPerspective(const qreal rFOV, const qreal rZNear, const qreal rZFar);


    public slots :

        /**
         * @brief setFOV
         * @param [in] dFOV   : ...
         */
        void setFOV(const double dFOV);


        /**
         * @brief Reset camera view.
         */
        void resetCamera();


	protected:
		
		/**
		 * \brief Initialize the OpenGl scene
		 */	
        virtual void initializeGL() = 0;
		
		/**
		 * \brief Draw the OpenGl scene
		 */		
        virtual void paintGL() = 0;
	
        /**
         * \brief Resize the OpenGl window
         * \param [in] w : width value
         * \param [in] h : height value
         */
        virtual void resizeGL( int w, int h );

		/**
		 * \brief Initialize the parameters shaders
		 * \param [in] vertexShaderPath   : vertex shader path
		 * \param [in] fragmentShaderPath : fragment shader path
         * \param [in,out] oShader        : shader program to init
         * \param [in] bBindShader        : bind the shader ?
		 */	
        void initShaders( const QString& vertexShaderPath, const QString& fragmentShaderPath,
                          QGLShaderProgram &oShader, cbool bBindShader = true);

		/**
		 * \brief Mouse press event, retrieve the current mouse position when the mouse is pressed.
		 * \param [in] e : qt mouse event
		 */
        virtual void mousePressEvent(QMouseEvent *e);

		/**
		 * \brief Mouse release event, compute the rotation axis and the angular speed.
		 * \param [in] e : qt mouse event
		 */	
        virtual void mouseReleaseEvent(QMouseEvent *e);
	
		/**
		 * \brief Mouse move event
		 * \param [in] e : qt mouse event
		 */		
        virtual void mouseMoveEvent(QMouseEvent *e);
		
		/**
		 * \brief Timer event
		 * \param [in] e : qt timer event
		 */			
        virtual void timerEvent(QTimerEvent *e);
		
		/**
		 * \brief Mouse wheel event
		 * \param [in] e : qt wheel event
		 */			
        virtual void wheelEvent(QWheelEvent *e);
	
		/**
		 * \brief Key press event
		 * \param [in] e : qt key event
		 */		
        virtual void keyPressEvent(QKeyEvent *e);

        /**
         * \brief Draw 3D scene axes. (ijk) // TODO : details axes
         * \param [in,out] oShader  : shader program
         * \param [in] mvpMatrix    : movel-view-projection matrix
         * \param [in] fScale       : scale value of the axes
         * \param [in] oOrigine     : origin point of the axes
         */
        void drawAxes(QGLShaderProgram &oShader, QMatrix4x4 &mvpMatrix, cfloat fScale = 1.f, const QVector3D &oOrigine = QVector3D(0.f,0.f,0.f));


    protected :

        bool m_bVerbose;                /**< display more info */
        bool m_bIsClickedMouse;         /**< is the mouse clicked ? */
        bool m_bMidClick;               /**< is mouse midle button clicked ? */

        GLfloat m_glFSizePoint;         /**< size of the gl points */

        qreal m_rZNear;                 /**< near perspective */
        qreal m_rZFar;                  /**< far pespective */
        qreal m_rFOV;                   /**< field of view perspective */

        QSize	  m_oSize;              /**< size of the OpenGl window */
        QVector3D m_oCurrentRotation;   /**< current rotation of the camera */
        QMatrix4x4 m_oProjectionMatrix;	/**< projection 4x4 matrix */
        SWQtCamera *m_pCamera;          /**< camera of the scene */
	        	
        QBasicTimer *m_oTimer;          /**< qt timer */

        QGLContext *m_glContext;        /**< opengl context */


        QReadWriteLock m_oParamMutex; /**< ... */
};
     
#endif

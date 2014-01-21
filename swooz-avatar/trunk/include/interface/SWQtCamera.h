


/**
 * \file SWQtCamera.h
 * \brief Defines SWQtCamera
 * \author Florian Lance
 * \date 11/04/13
 */


#ifndef _SWQTCAMERA_
#define _SWQTCAMERA_

#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>

#include "commonTypes.h"

/**
 * \class SWQtCamera
 * \brief A Qt OpenGl free fly camera using Quaternion
 * \author Florian Lance
 * \date 11/04/13
 */
class SWQtCamera // ############################################################################## TODO : fix camera rotation
{	
	public:
		
		/**
		 * \brief constructor of SWQtCamera
		 * \param [in] oEyePosition     : initial position of the eye
		 * \param [in] oLookAt 		: view direction
		 * \param [in] oUp 		: up vector for the view direction
		 */	
		SWQtCamera(const QVector3D &oEyePosition, const QVector3D &oLookAt, const QVector3D &oUp);
		
		/**
		 * \brief destructor of SWQtCamera
		 */	
		~SWQtCamera(); 	

		/**
		 * \brief Return eye position vector
		 * \return eye position vector
		 */		
		QVector3D eyePosition() const;
		
		/**
		 * \brief Return view direction vector
		 * \return view direction vector
		 */				
		QVector3D viewDirection() const;
		
		/**
		 * \brief Return up vector
		 * \return up vector
		 */				
		QVector3D up() const;	
	
        /**
         * \brief Apply a rotation on the input vector axe.
         * \param [in] fX : x vector coordinate
         * \param [in] fY : y vector coordinate
         * \param [in] fZ : z vector coordinate
         * \param [in] fAmount : angle in degrees
         */
        void rotate(cfloat fX, cfloat fY, cfloat fZ, cfloat fAmount);

		/**
		 * \brief Apply a rotation on the X axis
		 * \param [in] fAmount : angle in degrees
		 */		
		void rotateX(cfloat fAmount);
		
		/**
		 * \brief Apply a rotation on the Y axis
		 * \param [in] fAmount : angle in degrees
		 */		
		void rotateY(cfloat fAmount);		
	
		/**
		 * \brief Move on the view direction
		 * \param [in] fAmount : distance in meters
		 */		
		void moveForeward(cfloat fAmount);
		
		/**
		 * \brief Move on the opposite view direction
		 * \param [in] fAmount : distance in meters
		 */		
		void moveBackward(cfloat fAmount);		
		
		/**
		 * \brief Move on the left relative to the view direction
		 * \param [in] fAmount : distance in meters
		 */		
		void moveLeft(cfloat fAmount);	

		/**
		 * \brief Move on the right relative to the view direction
		 * \param [in] fAmount : distance in meters
		 */		
		void moveRight(cfloat fAmount);	

        /**
         * \brief Move up relative to the view direction
         * \param [in] fAmount : distance in meters
         */
        void moveUp(cfloat fAmount);

        /**
         * \brief Move down relative to the view direction
         * \param [in] fAmount : distance in meters
         */
        void moveDown(cfloat fAmount);
						
		/**
		 * \brief Reset the camera with the constructor values
		 */				
		void reset();
		
		QVector3D  m_oEyePosition;	  /**< eye position */		
		QVector3D  m_oLookAt;		  /**< view direction */		
		QVector3D  m_oUp;		  /**< up vector relative to the view direction */	

	private:

		
		QVector3D  m_oLeft;		  /**< left vector relative to the view direction */	
		QVector3D  m_oForeward;		  /**< difference vector between the eye position and the view direction */

		QVector3D  m_oInitialEyePosition; /**< initial eye position */
		QVector3D  m_oInitialLookAt;	  /**< initial view direction */
		QVector3D  m_oInitialUp;	  /**< initial up vector */
};


#endif

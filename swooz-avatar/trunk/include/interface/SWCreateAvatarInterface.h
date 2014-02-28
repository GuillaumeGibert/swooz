
/**
 * \file SWCreateAvatarInterface.h
 * \brief Defines SWCreateAvatarInterface
 * \author Florian Lance
 * \date 21/01/13
 */

#ifndef _SWCREATEAVATARINTERFACE_
#define _SWCREATEAVATARINTERFACE_

#include <QtGui>

#include "../genUI/SWUI_WCreateAvatar.h"
#include "QtWorkers/SWCreateAvatarWorker.h"

#include "SWDisplayImageWidget.h"
#include "SWGLCloudWidget.h"
#include "SWGLMeshWidget.h"


namespace Ui {
    class SWCreateAvatarInterface;
}

/**
 * \class SWCreateAvatarInterface
 * \brief  Qt interface for SWCreateAvatar
 * \author Florian Lance
 * \date 21/01/13
 */
class SWCreateAvatarInterface : public QMainWindow
{
	Q_OBJECT
	
	public :
		
		/**
         * \brief default constructor of SWCreateAvatarInterface
		 */		
        explicit SWCreateAvatarInterface(QWidget *oQWRelative = 0);
	
		/**
         * \brief destructor of SWCreateAvatarInterface
		 */		
        ~SWCreateAvatarInterface();
	
        /**
         * \brief Timer event
         * \param [in] e : qt timer event
         */
        void timerEvent(QTimerEvent *e);

        /**
         * @brief closeEvent
         * @param [in] event : close event
         */
        void closeEvent(QCloseEvent *event);

        /**
         * @brief Update the display
         */
        void updateDisplay();


	
	public slots:

        /**
         * @brief startWork
         */
        void startWork();

        /**
         * @brief switchDetect
         */
        void switchDetect();

        /**
         * @brief saveMeshFile
         */
        void saveMeshFile();

        /**
         * @brief enableInterface
         */
        void enableInterface();

        /**
         * @brief Update the face rectangle to be displayed
         * @param [in] pFaceRect : face rectangle pointer
         */
        void updateFaceRectangle(cv::Rect *pFaceRect);

        /**
         * @brief Update the nose rectangle to be displayed
         * @param [in] pNoseRect : nose rectangle pointer
         */
        void updateNoseRectangle(cv::Rect *pNoseRect);

        /**
         * @brief updateRadialProjDisplay
         * @param pRadialProj
         */
        void updateRadialProjDisplay(cv::Mat *pRadialProj);

        /**
         * @brief updateStasmPoints
         * @param vP2IStasmPoints
         */
        void updateStasmPoints(std::vector<cv::Point2i> vP2IStasmPoints);

        /**
         * @brief udpdateTexture3DDisplay
         * @param pTexture
         */
        void udpdateTexture3DDisplay(cv::Mat *pTexture);


        /**
         * @brief updateFaceTextureDisplay
         * @param pFaceTexture
         */
        void updateFaceTextureDisplay(cv::Mat *pFaceTexture);


        /**
         * @brief resetKinect
         * @param i32VideoMode
         */
        void resetKinect(const int i32VideoMode);


        /**
         * @brief releaseKinectMutex
         */
        void releaseKinectMutex();

        /**
         * @brief setNumCloud
         */
        void setNumCloud(const int);


	signals:

        /**
         * @brief startWorkSignal
         */
        void startWorkSignal();
	
        /**
         * @brief saveMeshFileSignal
         */
        void saveMeshFileSignal(QString);

        /**
         * @brief leaveProgram
         */
        void leaveProgram();
		
		
	private :
	
		// widgets & ui
        Ui::SWUI_WCreateAvatar *m_uiCreateAvatar;   /**< qt main window */
        SWDisplayImageWidget *m_WRGBDisplay;        /**< qt rgb display widget */
        SWDisplayImageWidget *m_WRadialProjDisplay; /**< qt radial proj display widget */
        SWDisplayImageWidget *m_WFaceTextureDisplay;/**< qt face texture display widget */
        SWGLCloudWidget *m_WCloudGL;                /**< qt opengl cloud widget */
        SWGLMeshWidget  *m_WMeshGL;                 /**< qt opengl mesh widget */
	
		// threads & workers
        SWCreateAvatarWorker  *m_WCreateAvatar; /**< create avatar worker */
        QThread                m_TCreateAvatar; /**< create avatar thread */

        // timer
        QBasicTimer *m_oTimer;                  /**< qt timer */


        //
        bool m_bWorkStarted;
        bool m_bFaceRectInitialise;             /**< is the face rectangle initialized ? */
        bool m_bDetectionStopped;               /**< is the detection work must be stopped ? */

        // mutex
        QReadWriteLock m_oMutex;                /**< mutex */

        // stasm
        std::vector<cv::Point2i> m_vP2IStasmPoints; /**< last stsams points computed */

        // cloud
        int m_i32NumCloud;                      /**< current cloud number */
        swCloud::SWCloud *m_pCloudToDisplay;    /**< cloud to be displayed in the gl cloud widget */

        // detection
        cv::Rect m_oFaceRect;                   /**< face detection rectangle */
        cv::Rect m_oNoseRect;                   /**< nose detection rectangle */
        cv::Rect m_oLastRectFace;               /**< last face rectangle computed */
        cv::Mat  m_oFaceDetect;                 /**< rgb image containing the detections rectangles */
        SWFaceDetectionPtr m_CFaceDetectPtr;    /**< detect face pointer */

        // kinect
        bool m_bResetKinect;                         /**< ... */
        QReadWriteLock m_oResetKinectMutex;          /**< mutex */
        swDevice::SWKinect_thread   m_oKinectThread; /**< rgbd device */
};




#endif

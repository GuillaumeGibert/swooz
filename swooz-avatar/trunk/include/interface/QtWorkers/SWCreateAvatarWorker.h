
/**
 * \file SWCreateAvatarWorker.h
 * \brief Defines SWCreateAvatarWorker
 * \author Florian Lance
 * \date 08/02/13
 */

#ifndef _SWCREATEAVATARWORKER_
#define _SWCREATEAVATARWORKER_

#include "devices/rgbd/SWKinect_thread.h"
#include "SWCreateAvatar.h"
#include "cloud/SWCloud.h"


#include <QObject>
#include <QReadWriteLock>

#include <QMatrix4x4>

#include <boost/iostreams/device/mapped_file.hpp>




typedef boost::shared_ptr<SWCreateAvatar> SWCreateAvatarPtr;	/**< boost shared pointer for SWCreateAvatar */
typedef boost::shared_ptr<swDevice::SWKinect_thread> SWKinect_threadPtr;	/**< boost shared pointer for SWKinect */


/**
 * \class SWCreateAvatarWorker
 * \brief  ...
 * \author Florian Lance
 * \date 06/02/13
 */
class SWCreateAvatarWorker : public QObject
{	
	Q_OBJECT
	
	public :
		
		/**
		 * \brief current worker mode enum
		 */		
        enum WorkMode{ DETECT, ADD_CLOUD};
	
		/**
         * \brief constructor of SWCreateAvatarWorker
		 * \param [in] ui8VideoMode : kinect video mode
		 */	
        SWCreateAvatarWorker(const uint8 ui8VideoMode = CV_CAP_OPENNI_SXGA_30HZ);
	
		/**
         * \brief destructor of SWCreateAvatarWorker
		 */	
        ~SWCreateAvatarWorker();
						
	
	public slots:
	
        /**
         * \brief ...
         */
        void doWork();
		
        /**
         * \brief ...
         */
        void stopWork();

        /**
         * @brief saveMeshFile
         */
        void saveMeshFile(QString sPath);
	
        /**
         * \brief Set the number of clouds to use in the avatar creation.
         * \param [in] i32NumberOfClouds : number of cloud
         */
        void setCloudNumberValue(const int i32NumberOfClouds);

        /**
         * \brief Set the X value offset for the rgb video input.
         * \param [in] i32CalibrationX : x offset
         */
        void setCalibrationXValue(const int i32CalibrationX);

        /**
         * \brief Set the Y value offset for the rgb video input.
         * \param [in] i32CalibrationY : y offset
         */
        void setCalibrationYValue(const int i32CalibrationY);
		
        /**
         * @brief setRadialWidth
         * @param i32Val
         */
        void setRadialWidth(const int i32Val);

        /**
         * @brief setRadialHeight
         * @param i32Val
         */
        void setRadialHeight(const int i32Val);

        /**
         * @brief setBilateralDiameter
         * @param i32Val
         */
        void setBilateralDiameter(const int i32Val);

        /**
         * @brief setBilateralColor
         * @param i32Val
         */
        void setBilateralColor(const int i32Val);

        /**
         * @brief setBilateralSpace
         * @param i32Val
         */
        void setBilateralSpace(const int i32Val);

        /**
         * @brief setCylinderRadius
         * @param dRadius
         */
        void setCylinderRadius(const double dRadius);

        /**
         * @brief setUseBilateralFilter
         * @param bUseFilter
         */
        void setUseBilateralFilter(const bool bUseFilter);

        /**
         * @brief setUseStasm
         * @param bUseStasm
         */
        void setUseStasm(const bool bUseStasm);

        /**
         * @brief setErode
         * @param i32Erode
         */
        void setErode(const int i32Erode);

        /**
         * @brief setDilate
         * @param i32Dilate
         */
        void setDilate(const int i32Dilate);
		
	signals:

        /**
         * \brief ...
         */
        void sendCloud(swCloud::SWCloud*);

        /**
         * @brief sendMesh
         */
        void sendMesh(swMesh::SWMesh*);

        /**
         * @brief sendMat
         */
        void sendMat(cv::Mat*);

        /**
         * @brief sendTexture
         */
        void sendTexture(cv::Mat*);

        /**
         * @brief switchTab
         */
        void switchTab(int);
		
        /**
         * @brief enableInterfaceSignal
         */
        void enableInterfaceSignal();

        /**
         * \brief ...
         */
        void stopWorkSignal();

        /**
         * @brief Signal for sending the face rectangle
         */
        void sendFaceRect(cv::Rect*);

        /**
         * @brief Signal for sending the nose rectangle
         */
        void sendNoseRect(cv::Rect*);

        /**
         * @brief sendStasmPoints
         */
        void sendStasmPoints(std::vector<cv::Point2i>);
		
	private :
		

        bool m_bDoWork;                             /**< ... */
        bool m_bWorkStopped;                        /**< ... */
        bool m_bResetKinectParams;                  /**< ... */
        int  m_i32NumberOfClouds;                   /**< ... */

        // parameters
        int  m_i32CurrentCloudNumber;               /**< ... */
        int  m_i32CalibrationX;                     /**< ... */
        int  m_i32CalibrationY;                     /**< ... */

        // detection
        cv::Rect *m_pCurrentFaceRect;               /**< current face rectangle */
        cv::Rect *m_pCurrentNoseRect;               /**< current nose rectangle */

        // mat
        cv::Mat *m_pRadialProjectionToDisplay;       /**< filtered radial projection mat to be displayed */
        cv::Mat *m_pTextureToDisplay;               /**< ... */

        // cloud
        swCloud::SWCloud *m_pCloudToDisplay;        /**< cloud to be displayed */

        // mesh
        swMesh::SWMesh *m_pMeshToDisplay;           /**< mesh to be displayed */

        // mutex
        QReadWriteLock      m_oLoopMutex;           /**< loop mutex */
        QReadWriteLock      m_oParametersMutex;     /**< parameters mutex */

        // kinect
        swDevice::SWKinectParams m_CKinectParams;   /**< kinect video params */
		SWKinect_threadPtr 	m_CKinectTPtr;  		/**< kinect class pointer */		

        // avatar
        SWCreateAvatarPtr 	m_CAvatarPtr;           /**< create avatar class pointer */
};

#endif



/**
 * \brief Save all the rgb and cloud data from a kinect, the length of the sequence must be under 3min.
 */
//		void saveKinectData();

/**
 * \brief Load the previoulsy saved kinect data and change the current data source in the main loop to use it.
 */
//		void loadKinectData();

/**
 * \brief Switch between SAVE_KINECT_DATA and DETECT_AND_DISPLAY mode.
 */
//		void switchModeSaveKinectData();

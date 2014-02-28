
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


typedef boost::shared_ptr<SWCreateAvatar> SWCreateAvatarPtr;	/**< boost shared pointer for SWCreateAvatar */


/**
 * \class SWCreateAvatarWorker
 * \brief  Worker thread for the create avatar module.
 * \author Florian Lance
 * \date 06/02/13
 */
class SWCreateAvatarWorker : public QObject
{	
	Q_OBJECT
	
	public :
		
        /**
         * @brief SWCreateAvatarWorker
         * @param [in] pRGBDDeviceThread : pointer of the kinect thread allocated in the main interface
         */
        SWCreateAvatarWorker(swDevice::SWKinect_thread *pRGBDDeviceThread);
	
		/**
         * \brief destructor of SWCreateAvatarWorker
		 */	
        ~SWCreateAvatarWorker();
						
	
	public slots:
	
        /**
         * @brief doWork
         */
        void doWork();
		
        /**
         * @brief stopWork
         */
        void stopWork();


        /**
         * @brief reconstruct
         */
        void reconstruct();

        /**
         * @brief saveMeshFile
         * @param [in] sPath : path for saving the mesh
         */
        void saveMeshFile(QString sPath);
	
        /**
         * \brief SsetCloudNumberValue
         */
        void setCloudNumberValue(const int);

        /**
         * \brief setCalibrationXValue
         */
        void setCalibrationXValue(const int);

        /**
         * \brief setCalibrationYValue
         */
        void setCalibrationYValue(const int);
		
        /**
         * @brief setRadialWidth
         */
        void setRadialWidth(const int);

        /**
         * @brief setRadialHeight
         */
        void setRadialHeight(const int);

        /**
         * @brief setBilateralDiameter
         */
        void setBilateralDiameter(const int);

        /**
         * @brief setBilateralColor
         */
        void setBilateralColor(const int);

        /**
         * @brief setBilateralSpace
         */
        void setBilateralSpace(const int);

        /**
         * @brief setCylinderRadius
         */
        void setCylinderRadius(const double);

        /**
         * @brief setUseBilateralFilter
         */
        void setUseBilateralFilter(const bool);

        /**
         * @brief setUseStasm
         */
        void setUseStasm(const bool);

        /**
         * @brief setErode
         */
        void setErode(const int);

        /**
         * @brief setDilate
         */
        void setDilate(const int);

        /**
         * @brief resetKinect
         */
        void resetKinect(const int);

        /**
         * @brief setExpandValue
         */
        void setExpandValue(const int);

        /**
         * @brief setExpandConnex
         */
        void setExpandConnex(const int);

        /**
         * @brief setEraseValue
         */
        void setEraseValue(const int);

        /**
         * @brief setEraseConnex
         */
        void setEraseConnex(const int);

        /**
         * @brief setDepthCloud
         */
        void setDepthCloud(const double);


        /**
         * @brief addPointToDeleteRadialProj
         */
        void addPointToDeleteRadialProj(const QPoint, const QSize);

        /**
         * @brief resetDeletedPointsRadialProj
         */
        void resetDeletedPointsRadialProj();


        /**
         * @brief setWidthRectRatio
         */
        void setWidthRectRatio(const int);

        /**
         * @brief setHeightRectRatio
         */
        void setHeightRectRatio(const int);

		
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

        /**
         * @brief endResetKinect
         */
        void endResetKinect();

        /**
         * @brief sendNumCloud
         */
        void sendNumCloud(int);
		
	private :
		

        bool m_bDoWork;                             /**< ... */
        bool m_bInitKinect;                         /**< ... */
        int  m_i32NumberOfClouds;                   /**< ... */

        // parameters
        int  m_i32CurrentCloudNumber;               /**< ... */
        int  m_i32CalibrationX;                     /**< ... */
        int  m_i32CalibrationY;                     /**< ... */

        // detection
        cv::Rect *m_pCurrentFaceRect;               /**< current face rectangle */
        cv::Rect *m_pCurrentNoseRect;               /**< current nose rectangle */

        // mat
        cv::Mat *m_pRadialProjectionToDisplay;      /**< filtered radial projection mat to be displayed */
        cv::Mat *m_pFaceTexture;                    /**< face mesh texture */

        // cloud
        swCloud::SWCloud *m_pCloudToDisplay;        /**< cloud to be displayed */

        // mesh
        swMesh::SWMesh *m_pFaceMeshResult;          /**< face mesh result */

        // mutex
        QReadWriteLock      m_oLoopMutex;           /**< loop mutex */
        QReadWriteLock      m_oParametersMutex;     /**< parameters mutex */

        // kinect
        swDevice::SWKinect_thread *m_pRGBDDeviceThread; /**< kinect class pointer defined in createAvatar interface */

        // avatar
        SWCreateAvatarPtr 	m_CAvatarPtr;           /**< create avatar class pointer */
};

#endif

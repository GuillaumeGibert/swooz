
/**
 * \file SWKinect_thread.h
 * \brief Defines SWKinect_thread
 * \author Florian Lance
 * \date 17/06/13
 */

#ifndef _SWKINECT_THREAD_
#define _SWKINECT_THREAD_

#include "SWKinect.h"
#include "devices/SWDevice_thread.h"

namespace swDevice
{
	/**
	 * \class SWKinect_thread
	 * \brief A threaded kinect module.
	 */	
	class SWKinect_thread : public SWDevice_thread
	{
		public:
			
			/**
			 * \brief Default SWKinect_thread constructor.
             * \param [in] bVerbose : display informations
			 */	
            SWKinect_thread(bool bVerbose = true);
		
			/**
			 * \brief SWKinect_thread destructor.
			 */		
			virtual ~SWKinect_thread(void);
		
			/**
			 * \brief Init the kinect video module.
			 * \param [in] i32CaptureMode : kinect video mode to use
			 * \return 0 if the device contains image generator, else -1
			 */		
			virtual int init(cint i32CaptureMode = 1);

			/**
			 * \brief Start listening kinect data.
             * \throw kinectInitError
			 */	
			virtual void startListening();
		
			/**
			 * \brief Stop listening kinect data.
			 */
			virtual void stopListening();
		
			/**
			 * \brief Set the recalibration between the rgb data and the depth data.
			 * \param [in] bRecalib      : enable or disable the recalibration
			 * \param [in] i32XCalibrate : set the X offset
			 * \param [in] i32YCalibrate : set the Y offset
			 */				
			void setRecalibration(cbool bRecalib, cint i32XCalibrate, cint i32YCalibrate);
			
			/**
			 * \brief Safe accessor for kinect disparityMap.
			 * \return cv mat disparityMap
			 */		
			cv::Mat disparityMap();	
			
			/**
			 * \brief Safe accessor for kinect cloudMap.
			 * \return cv mat cloudMap
			 */		
			cv::Mat cloudMap();
			
			/**
			 * \brief Safe accessor for kinect bgrImage.
			 * \return cv mat bgrImage
			 */		
			cv::Mat bgrImage();
			
			/**
			 * \brief Safe accessor for kinect depthMap.
			 * \return cv mat depthMap
			 */		
			cv::Mat depthMap();
			
			/**
			 * \brief Safe accessor for kinect grayImage.
			 * \return cv mat grayImage
			 */		
			cv::Mat grayImage();
		
			/**
			 * \brief Indicates if kinect data is available.
			 * \return true if data available, else return false.
			 */		
			bool isDataAvailable();			
			
			/**
			 * \brief Get the size frame of the current kinect video mode.
			 * Warning : may not necessary return the correct size with devices like the ASUS xtion pro, 
			 * in order to be certain to get the good value, retrieve the size from the SWKinect cv::Mat after a grab. 
			 * \return the cv size value
			 */	
			cv::Size sizeFrame();
		
			/**
			 * \brief Get the number of frame per second for the current kinect video mode
			 * Warning : may not necessary return the correct fps with devices like the ASUS xtion pro.
			 * \return the fps number
			 */	
			int fps();		
			
			/**
			 * \brief Return kinect module capture video mode.
			 * \return the video mode integer
			 */				
			int captureMode();
		
		private :

			cv::Mat m_oDisparityMap; /**< Disparity map CV_CAP_OPENNI_DISPARITY_MAP   : Disparity in pixels (CV_8UC1) */
			cv::Mat m_oCloudMap; 	 /**< Cloud map     CV_CAP_OPENNI_POINT_CLOUD_MAP : XYZ in meters       (CV_32FC3)*/
			cv::Mat m_oBgrImage;	 /**< BGR image	    CV_CAP_OPENNI_BGR_IMAGE       : rgb color pixels    (CV_8UC3) */
			cv::Mat m_oDepthMap;	 /**< Depth map	    CV_CAP_OPENNI_DEPTH_MAP       : Depth values in mm  (CV_16UC1)*/
			cv::Mat m_oGrayImage;	 /**< Gray image    CV_CAP_OPENNI_GRAY_IMAGE	  : gray image		(CV_8UC1) */		
			
			/**
			 * \brief Work thread.
			 */		
			void doWork();
		
			bool m_bInitialized;	/**< is the module initialized ? */
			bool m_bDataAvailable;	/**< is the data available ? */
		
			SWKinect m_oKinect; 	/**< kinect module */
	};
};

#endif

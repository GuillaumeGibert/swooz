
/**
 * \file SWKinect.h
 * \brief Defines SWKinect
 * \author Guillaume Gibert / Florian Lance
 * \date 01/10/12
 */

#ifndef _SWKINECT_
#define _SWKINECT_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "commonTypes.h"

#include <iostream>

namespace swDevice
{
	/**
	 * \class SWKinect
	 * \brief A hight level interface for getting data from a kinect/xtion device.
	 */	
    class SWKinect
	{
		public:

            cv::VideoCapture m_oCapture;    /**< openni video capture */
		
            bool m_bVerbose;                /**< display informations ? */
            bool m_bKinectInitalized;       /**< is the kinect initialized ? */
            bool m_bRecalibrate;            /**< is a recalibration must be done ? */
            int  m_i32XCalibrate;           /**< the X offset that must be applied to the rgb data to fit the depth data */
            int  m_i32YCalibrate;           /**< the Y offset that must be applied to the rgb data to fit the depth data */
            int  m_i32CaptureMode;          /**< kinect capture video mode */
			
			cv::Mat disparityMap; 	/**< Disparity map CV_CAP_OPENNI_DISPARITY_MAP   : Disparity in pixels (CV_8UC1) */
            cv::Mat cloudMap;       /**< Cloud map     CV_CAP_OPENNI_POINT_CLOUD_MAP : XYZ in meters       (CV_32FC3)*/
            cv::Mat bgrImage;       /**< BGR image	   CV_CAP_OPENNI_BGR_IMAGE       : rgb color pixels    (CV_8UC3) */
            cv::Mat depthMap;       /**< Depth map	   CV_CAP_OPENNI_DEPTH_MAP       : Depth values in mm  (CV_16UC1)*/
            cv::Mat grayImage;      /**< Gray image	   CV_CAP_OPENNI_GRAY_IMAGE      : ...                 (...) */
		
			/**
             * \brief SWKinect constructor.
             * \param [in] bVerbose : display informations
			 */	
            SWKinect(bool bVerbose = true);
		
			/**
			 * \brief SWKinect destructor.
			 */		
			virtual ~SWKinect(void);
		
			/**
			 * \brief Init the kinect video module.
			 * \param [in] i32CaptureMode : kinect video mode to use
			 * \return 0 if the device contains image generator, else -1
             * \throw kinectInitError
			 */		
			int init(cint i32CaptureMode = 0);
		
			/**
			 * \brief Set the recalibration between the rgb data and the depth data.
			 * \param [in] bRecalib      : enable or disable the recalibration
			 * \param [in] i32XCalibrate : set the X offset
			 * \param [in] i32YCalibrate : set the Y offset
			 */		
			void setRecalibration(cbool bRecalib, cint i32XCalibrate, cint i32YCalibrate);
		
			/**
             * \brief Grab a new frame from the device, init must have been called before.
			 * \return 0 if the new frame is correctly grabbed, else return -1
             * \throw kinectInitError, kinectGrabError
			 */			
			int grab(void);
			
			/**
			 * \brief Recalibrate the rgb data with the calibration parameters.
			 */
			void recalibrate();
			
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
			
            // TODO Coment normalize stuff
			int minDepthValue;
			int maxDepthValue ;
			cv::Mat normalizedDepthMap;
			void normalizeDepthImage(void);		
	};
}

#endif

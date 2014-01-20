
/**
 * \file SWKinectParams.h
 * \brief Defines SWKinectParams
 * \author Florian Lance
 * \date 26/02/13
 */

#ifndef _SWKINECTPARAMS_
#define _SWKINECTPARAMS_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

#include "commonTypes.h"

namespace swDevice
{

	/**
	 * \class SWKinectParams
	 * \brief Useful kinect parameters for image computing with opencv.
	 * \author Florian Lance
	 * \date 26/02/13
	 */
    class SWKinectParams // TODO : put hard values in an ini file
	{	
		public :
			
			/**
			 * \brief constructor of SWKinectParams	
			*/
			SWKinectParams() : m_i32CaptureMode(0), m_i32Fps(30), m_i32OffsetRgbX(0), m_i32OffsetRgbY(0), m_i32OffsetHDTextureX(0), m_i32OffsetHDTextureY(0),
					   m_fWidthRatio(1.f), m_fHeightRatio(1.f),
					   m_oOriginalSize( cv::Size(640, 480)), m_oVideoSize(cv::Size(640, 480)), m_oTextureSize(cv::Size(640, 480))
			{}
				
			/**
			 * \brief constructor of SWKinectParams		
			 * \param [in] ui8VideoMode : video mode capture used for the kinect device
			 * \param [in] oFrameSize   : size of the original kinect video frame, must be cv::Size(oKinect.bgrImage.cols, oKinect.bgrImage.rows) after a  oKinect.grab(), 
			 * with oKinect the SWKinect implementation.
			 */		
			SWKinectParams(cuint8 ui8VideoMode, const cv::Size &oFrameSize)
			{
				// set video params
				m_i32CaptureMode = ui8VideoMode;
				
				// init sizes 
				m_oOriginalSize  = oFrameSize; // original size of the video
				
				if(oFrameSize.width < 640)
				{
					m_oVideoSize = oFrameSize; // if (640,480) video size not available
				}
				else
				{
					m_oVideoSize = cv::Size(640, 480); // basic video size used for the computing, always take this one if available
				}
					 
				m_oTextureSize  = oFrameSize; // always take the bigger size available for the texture
				
				// init fps and offset values
				
				m_i32OffsetHDTextureX = 0;
				m_i32OffsetHDTextureY = 0;
				
				if(m_i32CaptureMode == 0) // kinect or xtion videoMode 0 -> VGA_30HZ (640-480)
				{
					m_i32Fps = 30;				
					m_i32OffsetRgbX = 1;  // xtion // kinect ?
					m_i32OffsetRgbY = 0; // xtion // kinect ?
				}
				else if(m_i32CaptureMode == 1 && oFrameSize == cv::Size(1280, 1024)) // kinect video mode 1 -> SXGA_15HZ (1280-1024)
				{
					m_i32Fps = 15;				
					m_i32OffsetRgbX = 0; // ?
					m_i32OffsetRgbY = 0; // ?
					m_i32OffsetHDTextureX = 0; // ?
					m_i32OffsetHDTextureY = 0; // ?
				}
				else if(m_i32CaptureMode == 1 && oFrameSize == cv::Size(640, 480))   // bugged xtion video mode 1 -> VGA_30HZ/SXGA_30HZ ? (640-480)
				{
					m_i32Fps = 30; 				
					m_i32OffsetRgbX = 1;
					m_i32OffsetRgbY = 0;
				}
				else if(m_i32CaptureMode == 2 && oFrameSize == cv::Size(640, 480))   // kinect video mode 2 -> SXGA_30HZ (640-480)
				{
					m_i32Fps = 30;				
					m_i32OffsetRgbX = 0; // ?
					m_i32OffsetRgbY = 0; // ?				
				}
				else if(m_i32CaptureMode == 2 && oFrameSize == cv::Size(1280, 1024)) // bugged xtion video mode 2 ->  SXGA_15HZ ? (1280-1024)
				{
					m_i32Fps = 15;				
					m_i32OffsetRgbX = 0;
					m_i32OffsetRgbY = 8;
					m_i32OffsetHDTextureX = 4;
					m_i32OffsetHDTextureY = -1;
				}
                else	// TODO others mode
				{
					m_i32Fps = 30; // 60 ?				
					m_i32OffsetRgbX = 0; // ?
					m_i32OffsetRgbY = 0; // ?
				}

				// init size ratio
				m_fWidthRatio  = 1.f * m_oOriginalSize.width  / m_oVideoSize.width;
				m_fHeightRatio = 1.f * m_oOriginalSize.height / m_oVideoSize.height;						
			}
			
			/**
			 * \brief recopy constructor of SWKinectParams		
			 * \param [in] oKinectParams : the parameters to recopy
			 */		
			SWKinectParams(const SWKinectParams &oKinectParams) : m_i32CaptureMode(oKinectParams.m_i32CaptureMode), m_i32Fps(oKinectParams.m_i32Fps), 
									      m_i32OffsetRgbX(oKinectParams.m_i32OffsetRgbX), m_i32OffsetRgbY(oKinectParams.m_i32OffsetRgbY),
									      m_i32OffsetHDTextureX(oKinectParams.m_i32OffsetHDTextureX), m_i32OffsetHDTextureY(oKinectParams.m_i32OffsetHDTextureY),
									      m_fWidthRatio(oKinectParams.m_fWidthRatio), m_fHeightRatio(oKinectParams.m_fHeightRatio),
									      m_oOriginalSize(oKinectParams.m_oOriginalSize), m_oVideoSize(oKinectParams.m_oVideoSize), 
									      m_oTextureSize(oKinectParams.m_oTextureSize)
			{}		
					
			/**
			 * \brief destructor of SWKinectParams		
			 */		
			~SWKinectParams()
			{}	
										
			void display()
			{
				std::cout << "SWKinectParams" <<  std::endl;
				std::cout << "Video capture mode : " << m_i32CaptureMode<< std::endl;
				std::cout << "Original video size           : " << m_oOriginalSize << std::endl;
				std::cout << "Video size used for computing : " << m_oVideoSize    << std::endl;
				std::cout << "Image size used for texturing : " << m_oTextureSize  << std::endl;
				std::cout << "Video fps : " << m_i32Fps        << std::endl;
				std::cout << "Width ratio between orignal and used video size : " << m_fWidthRatio   << std::endl;
				std::cout << "Height ration --------------------------------- : " << m_fHeightRatio  << std::endl;
				
			}

			int m_i32CaptureMode; 	  /**< capture video mode :
						       0 -> CV_CAP_OPENNI_VGA_30HZ
						       1 -> CV_CAP_OPENNI_SXGA_15HZ
						       2 -> CV_CAP_OPENNI_SXGA_30HZ
						       3 -> CV_CAP_OPENNI_QVGA_30HZ
						       4 -> CV_CAP_OPENNI_QVGA_60HZ */					  			
				
			int m_i32Fps;		  /**< video fps */
			
			int m_i32OffsetRgbX;	  /**< x offset bewteen rgb and depth data */
			int m_i32OffsetRgbY;	  /**< y offset bewteen rgb and depth data  */
			
			int m_i32OffsetHDTextureX;/**< x offset used to recalibrate (1280-1024) rgb data with (640,480) depth data */
			int m_i32OffsetHDTextureY;/**< y offset used to recalibrate (1280-1024) rgb data with (640,480) depth data */
			
			float    m_fWidthRatio;	  /**< ratio between m_oOriginalSize width and m_oVideoSize width, used for scaling rectangles bewteen theses two sizes */
			float    m_fHeightRatio;  /**< ratio between m_oOriginalSize height and m_oVideoSize height, used for scaling rectangles bewteen theses two sizes */
			
			cv::Size m_oOriginalSize; /**< base video size */
			cv::Size m_oVideoSize; 	  /**< wanted size for the video to be used for the computing, must be <= m_oOriginalSize */
			cv::Size m_oTextureSize;  /**< wanted size for the video images to be used for the avatar texture, must be <= m_oOriginalSize */				
	};
};

#endif

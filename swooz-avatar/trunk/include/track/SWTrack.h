
/**
 * \file SWTrack.h
 * \brief defines SWTrack 
 * \author Florian Lance
 * \date 29/11/12
 */

#ifndef _SWTRACK_
#define _SWTRACK_


#include "stdafx.h"
#include "commonTypes.h"

#include "track/SWTrackFlow.h"
#include "detect/SWFaceDetection.h"


typedef boost::shared_ptr<swTrack::SWTrackFlow>         SWTrackFlowPtr;	     /**< boost shared pointer for SWTrackFlow */
typedef boost::shared_ptr<swDetect::SWFaceDetection> 	SWFaceDetectionPtr;  /**< boost shared pointer for SWFaceDetection */

namespace swTrack
{
	/**
	 * \class SWTrack
	 * \brief Tracking using the optical flow to follow faces
	 * \author Florian Lance
	 * \date 05/12/12	 
	 */
	class SWTrack
	{
		public:
			
			/**
			 * \brief default constructor of SWTrack
			 */	
			SWTrack();	
		
			/**
			 * \brief track the face
			 * \param [in] oPreRgbFrame  : previous rgb mat image
			 * \param [in] oCurrRgbFrame : current rgb mat image
			 * \param [in,out] oFaceRect  : the result rectangle of the tracking
			 * \return true if the tracking is successfull, else return false
			 */		
			bool trackFace(const cv::Mat &oPreRgbFrame, const cv::Mat &oCurrRgbFrame, cv::Rect &oFaceRect);
		
			/**
			 * \brief display the features points of the optical flow
			 * \param [in,out] oInputMat : input rgb mat, the feature points will be drawed on it.
			 */		
			void displayFeaturesPoints(cv::Mat &oInputMat);
			
		private:
		
			bool m_bTracked; 				/**< is the face tracked ? */
		
			uint m_ui32NumFrame; 				/**< number of the current frame */
			uint m_ui32NumberOfFramesBeforeFaceDetection; 	/**< number of frame between two face detections */
		
			cv::Rect m_oFaceRect; 				/**< face rectangle */
		
			std::vector<cv::Point2f> m_vP2fPrePoints; 	/**< vector of the previous feature points */
			std::vector<cv::Point2f> m_vP2fPoints;    	/**< vector of the current feature points */
		
			SWTrackFlowPtr m_CTrackFlowPtr; 		/**< TrackFlow pointer */
			SWFaceDetectionPtr m_CFaceDetectionPtr;  	/**< FaceDetection pointer */	
	};
}

#endif


/**
 * \file SWHaarCascade.h
 * \brief definition of SWHaarCascade
 * \author Florian Lance
 * \date 04/12/12
 */

#ifndef _SWHAARCASCADE_
#define _SWHAARCASCADE_

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#undef max
#undef min
//#include "opencv2/gpu/gpu.hpp"


#include "stdafx.h"
#include "commonTypes.h"
#include "SWExceptions.h"

#include <stdexcept> 

namespace swDetect
{

	/**
	 * \class SWHaarCascade
	 * \brief This class detects a specific object int a cv::Mat image using the haar cascade algorithm.
	 * \author Florian Lance
	 * \date 04/12/12
	 */
	class SWHaarCascade
	{	
		public:
		
			// ############################################# CONSTRUCTORS / DESTRUCTORS
		
			/**
			 * \brief constructor of SWHaarCascade
			 * \param [in] sCascadeFile 	      : the file cascade name of the object to detect
             * \param [in] uiMin32Neighbours      :  Parameter specifying how many neighbors each candidate rectangle should have to retain it.
			 * \param [in] oMinDetectSize 	      : min size of a detection
			 * \param [in] oMaxDetectSize 	      : max size of a detection
             * \param [in] bUseGPU                : use the GPU for computing
			 */
            SWHaarCascade(const std::string &sCascadeFile, const uint uiMin32Neighbours = 3,
                      const cv::Size oMinDetectSize = cv::Size(80,80), const cv::Size oMaxDetectSize = cv::Size(0,0), bool bUseGPU = false); // oMaxDetectSize = cv::Size(0,0)
		
			// ############################################# METHODS
			
			/**
			 * \brief launch the haar cascade detection in the input rgb image
			 * \param [in] oRgbImg    : input rgb image
			 * \param [in,out] oRects : array of rectangles, will contain the haar cascade detection result
			 * \return true if the detection is successfull, else return false
			 */			
			bool detect(const cv::Mat& oRgbImg, std::vector<cv::Rect> &oRects);			
		
		private:
			
			/**
			 * \brief launch the haar cascade detection CPU computing in the input rgb image
			 * \param [in] oRgbImg    : input rgb image
			 * \param [in,out] oRects : array of rectangles, will contain the haar cascade detection result
			 * \return true if the detection is successfull, else return false
			 */				
//			bool detectGPU(const cv::Mat& oRgbImg, std::vector<cv::Rect> &oRects);
			
			/**
			 * \brief launch the haar cascade detection GPU computing in the input rgb image
			 * \param [in] oRgbImg    : input rgb image
			 * \param [in,out] oRects : array of rectangles, will contain the haar cascade detection result
			 * \return true if the detection is successfull, else return false
			 */				
			bool detectCPU(const cv::Mat& oRgbImg, std::vector<cv::Rect> &oRects);

		private:

			bool m_bUseGPU; 			/**< use the gpu for computing */
			bool m_initFile; 			/**< indicates if the cascade classifier has been correctly initialized */
				
            uint m_ui32MinNeighbours;  	/**< Parameter specifying how many neighbors each candidate rectangle should have to retain it. */
		
			float m_fPyramidScale;			/**< ... */
			
			cv::Size m_oMinDetectSize; 		/**< min size of a detection */
			cv::Size m_oMaxDetectSize; 		/**< max size of a detection */
		
			cv::CascadeClassifier m_oCascade; 	/**< cascade classifier */
//			cv::gpu::CascadeClassifier_GPU m_oCascadeGpu; /**< gpu cascade classifier */
		
	};
}

#endif

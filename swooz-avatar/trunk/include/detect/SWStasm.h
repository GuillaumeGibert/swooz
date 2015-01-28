
/*******************************************************************************
**                                                                            **
**  SWoOz is a software platform written in C++ used for behavioral           **
**  experiments based on interactions between people and robots               **
**  or 3D avatars.                                                            **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
** *****************************************************************************
**          Authors: Guillaume Gibert, Florian Lance                          **
**  Website/Contact: http://swooz.free.fr/                                    **
**       Repository: https://github.com/GuillaumeGibert/swooz                 **
********************************************************************************/

/**
 * \file SWStasm.h
 * \brief definition of SWStasm
 * \author Florian Lance
 * \date 18/02/13
 */

#ifndef _SWSTASM_
#define _SWSTASM_

#undef _XLength_error

// stasm
#include "stasm/stasm.hpp"

// opencv
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// swooz
#include "commonTypes.h"
#include "SWExceptions.h"
#include "cloud/SWCloud.h"

namespace swDetect
{

	/**
	 * \class SWStasm
	 * \brief Stasm wrapper.
	 * \author Florian Lance
	 * \date 18/02/13
	 */
	class SWStasm
	{
		public :
			
			// ############################################# CONSTRUCTORS / DESTRUCTORS
		
			/**
			 * \brief constructor of SWStasm
			 * \param [in] sConfFile0Path : stasm config file 0 path
			 * \param [in] sConfFile1Path : stasm config file 1 path			  
			 */	
            SWStasm(const std::string &sConfFile0Path = "../data/stasm/mu-68-1d.conf", const std::string &sConfFile1Path = "../data/stasm/mu-76-2d.conf");
			
			/**
			 * \brief destructor of SWStasm
			 */	
			virtual ~SWStasm(void);		
		
			// ############################################# METHODS
		
			/**
			 * \brief Detect landmarks on the face, using stasm algorithm.
			 * \param [in]     oCurrRgbMat         : input current rgb mat
			 * \param [in]     oCurrFaceRect       : input current face rectangle
			 * \return  false if the operation failed, true else
			 */		
			bool launchAsmSearch(const cv::Mat &oCurrRgbMat,  cv::Rect &oCurrFaceRect);
										

			/**
			 * \brief Reset stasm params, but not the config files.
			 */				
			void resetParams();

			/**
			 * \brief Display stasm features points on the input image
			 * \param [in,out] oOutputDetectRgbImg : image to update with detected stasm features points
			 * \param [in]     oColor  	       : color used
			 */				
			void display(cv::Mat& oOutputDetectRgbImg, const cv::Scalar &oColor) const;
			

            void featuresPoints(std::vector<cv::Point2i> &oFeaturesPoints); // TODO : doc

			/**
			 * \brief ...
			 * \param [in]  oCloudMap       : ...
			 * \param [out] vP3FStasmPoints : ...
			 */							
			void compute3DPoints(const cv::Mat& oCloudMap, std::vector<cv::Point3f> &vP3FStasmPoints);
						
			/**
			 * \brief ...
			 * \param [in]  oCloudMap  : ...
			 * \param [out] oStasmMask : ...
			 */				
			void setStasmMask(const cv::Mat &oCloudMap, cv::Mat &oStasmMask);

			
//			cv::Rect computeUpperFaceRect();
			
			/**
			 * \brief get the right eye brow points
			 * \return the vector of right eye brow cv point
			 */					
			std::vector<cv::Point2i> rightEyeBrowPoints();
						
			std::vector<cv::Point2i> m_oFeaturesPoints;   /**< array of detected features points */

		private :				

			bool m_bInitDone;  	         /**< is the initialization done */
			bool m_bInitFirstShape;		 /**< a shape has been computed */
		
			int m_i32InitializedModels;      /**< number of models initialized */
	
			std::string m_sConfFile0;    	 /**< path of the first stasm conf file */
			std::string m_sConfFile1;    	 /**< path of the second stasm conf file  */
		
			SHAPE 	  m_oInitShape;		 /**< precedent computed shape used for the next computing */
			SHAPE 	  m_oCombinedShape;	 /**< ... */
		
			ASM_MODEL m_oModels[2];	         /**< asm models loaded */
		
		
			// ############################################# METHODS
		
			/**
			 * \brief Adapt and convert the face rectangle to stasm Det_params
			 * \param [in]  oCurrAugmentedRectFace : the expanded face rectangle 
			 * \param [in]  oCurrRectFace 	       : the face rectangle
			 * \param [out] oDetParams    	       : stasm rectangle
			 */		
			void convRectToStasmDetParams(const cv::Rect &oCurrAugmentedRectFace, const cv::Rect &oCurrRectFace, DET_PARAMS &oDetParams);
			
			/**
			 * \brief Convert a rgb cv::Mat to a rgb stasm image.
			 * \param [in]  oCurrRgbMat : input rgb mat
			 * \param [out] oStasmImage : the rgb stasm image result
			 */			
			void convMatToStasmImage(const cv::Mat &oCurrRgbMat, ImageT<RGB_TRIPLE> &oStasmImage);			
		
	};
}

#endif

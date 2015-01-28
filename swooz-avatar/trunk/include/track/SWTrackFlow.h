
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
 * \file SWTrackFlow.h
 * \brief defines SWTrackFlow 
 * \author Florian Lance
 * \date 29/11/12 
 */

#ifndef _SWTRACKFLOW_
#define _SWTRACKFLOW_


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include "SWExceptions.h"

namespace swTrack
{
	/**
	 * \class SWTrackFlow
	 * \author LANCE Florian
	 * \date 29/11/12
	 * \brief Tracking class for cv::Mat image
	 */
	class SWTrackFlow
	{
		public:
			
			/**
			 * \brief default constructor of SWTrackFlow
			 * \param [in] oWinSize       : size of the window for the computing of the optical flow
			 * \param [in] i32MaxLvl      : based maximal pyramid level number for the computing of the optical flow
			 */	
			SWTrackFlow(cv::Size oWinSize = cv::Size(20,20), int i32MaxLvl = 5);		
			
			/**
			 * \brief Launch the tracking
			 * \param [in] oPreGray       : previous gray image 
			 * \param [in] oGray          : current gray image 
			 * \param [in] vP2fPrePoints  : previous features points
			 * \param [in,out] vP2fPoints : current features points
			 */	
			bool track(const cv::Mat &oPreGray, const cv::Mat &oGray, 
				   const std::vector<cv::Point2f> &vP2fPrePoints, const std::vector<cv::Point2f> &vP2fPoints);
			
		private:
			
			int m_i32Maxlvl;      /**< based maximal pyramid level number for the computing of the optical flow */
			cv::Size m_oWinSize;  /**< size of the pyramid window for the computing of the optical flow */
			
			cv::Mat m_oPyrImg;    /**<  previous image */
			cv::Mat m_oPrePyrImg; /**<  current image */
	};
}



#endif

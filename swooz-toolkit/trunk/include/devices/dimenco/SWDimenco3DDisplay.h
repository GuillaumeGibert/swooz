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

#ifndef _SWDIMENCO3DDISPLAY_
#define _sWDIMENCO3DDISPLAY_

#include "targetver.h"
#include <tchar.h>

#include "commonTypes.h"
#include "opencvUtility.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Windows.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <fstream>

/**
 * @brief The SWDimenco3DDisplay class
 */
class SWDimenco3DDisplay
{
	public:

        /**
         * @brief SWDimenco3DDisplay destructor
         */
		virtual ~SWDimenco3DDisplay(void);

	public: 
		// display infos
        int displaySize;            /**< ... */
        int displayImgWidth;        /**< ... */
        int displayImgHeight;       /**< ... */

		// Dimenco display constants to convert depth into disparity
		// using the following equation: 
		// D(Z) = M * (1-(vz/(Z-Zd+vz))) + C
		// D: disparity [0,255]
		// Z: depth [0,1]
        double Zd;                  /**< depth of display plane */
        double vz;                  /**< view distance in coordinate units */
        double M;                   /**< linear function multiplier */
        double C;                   /**< linear function constant */
	
		// rgb image infos
        int originalRGBImgWidth;    /**< ... */
        int originalRGBImgHeight;   /**< ... */
        cv::Mat m_rgbImg;           /**< ... */
	
		// depth image infos
        int originalDepthImgWidth;  /**< ... */
        int originalDepthImgHeight; /**< ... */
        cv::Mat m_depthImg;         /**< ... */
		
        /**
         * @brief init
         * @param msize
         * @return
         */
		int init(int msize);

        /**
         * @brief refresh
         * @param rgbImg
         * @param depthImg
         */
		void refresh(const cv::Mat& rgbImg, const cv::Mat& depthImg);

        /**
         * @brief depth2disparity
         * @param depthImg
         * @param disparityImg
         */
		void depth2disparity(const cv::Mat& depthImg, cv::Mat& disparityImg);

        /**
         * @brief padImageBorder
         * @param inputImg
         * @param outputImg
         */
		void padImageBorder( const cv::Mat& inputImg, cv::Mat& outputImg );

        /**
         * @brief concatRGBDImages
         * @param pRgbImg
         * @param pDisparityImg
         * @param rgbdImg
         */
		void concatRGBDImages( const cv::Mat& pRgbImg, const cv::Mat& pDisparityImg, cv::Mat& rgbdImg ); 

        /**
         * @brief addZeroLines
         * @param inputImg
         * @param outputImg
         */
		void addZeroLines(const cv::Mat& inputImg, cv::Mat& outputImg);

        /**
         * @brief add3DHeader
         * @param img
         */
		void add3DHeader( cv::Mat& img );

        /**
         * @brief embedBlueBit7B
         * @param data
         * @param hd
         * @param len
         */
		void embedBlueBit7B(unsigned char *data, unsigned char *hd, int len);
};

#endif

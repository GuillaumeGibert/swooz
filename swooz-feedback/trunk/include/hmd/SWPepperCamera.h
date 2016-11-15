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
 * \file SWPepperCamera.h
 * \brief Defines SWPepperCamera class
 * \author Guillaume Gibert
 * \date 14/11/16
 */


#ifndef _SW_PEPPER_CAMERA_
#define _SW_PEPPER_CAMERA_

#define BOOST_ALL_DYN_LINK

#include "opencvUtility.h"
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <string>

#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>

using namespace AL;

/**
 * \class SWPepperCamera
 * \brief ...
 */
class SWPepperCamera
{
    public:

        /**
         * @brief SWPepperCamera
         */
        SWPepperCamera(){m_fullScreen = false;}

        /**
         * @brief open
         * @param displayImgWidth
         * @param displayImgHeight
	 * @param sRobotAddress
         * @param pepperPort
	* @param sPepperVideoMode
         * @return
         */
        bool open(int displayImgWidth, int displayImgHeight, std::string sRobotAddress, int pepperPort, std::string sPepperVideoMode, int fps);

        /**
         * @brief close
         * @return
         */
	bool close();

        /**
         * @brief loop
         */
        bool loop();

        /**
         * @brief interrupt
         * @return
         */
	bool interrupt();

    private :

        // display infos
        bool m_fullScreen;          /**< is fullscreen display ? */
        int m_displayImgWidth;      /**< width image display */
        int m_displayImgHeight;     /**< height image display */
    
	int m_i32ImageWidth;
	int m_i32ImageHeight;
	int m_i32PepperVideoMode;
    
        cv::Mat m_displayImage;
	std::string m_sPepperVideoMode;
	std::string clientName;
	ALVideoDeviceProxy* camProxy;
    
};

   
#endif



   

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
 * \file SWSonyHMZT3W.h
 * \brief Defines SWSonyHMZT3W class
 * \author Guillaume Gibert
 * \date 17/07/14
 */


#ifndef _SWSONYHMZT3W_
#define _SWSONYHMZT3W_

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "opencvUtility.h"
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

/**
 * \class SWSonyHMZT3W
 * \brief ...
 */
class SWSonyHMZT3W
{
    public:

        /**
         * @brief SWSonyHMZT3W
         */
        SWSonyHMZT3W(){m_fullScreen = false; m_eyeToDisplay = 0;}

        /**
         * @brief open
         * @param displayImgWidth
         * @param displayImgHeight
         * @return
         */
        bool open(int displayImgWidth, int displayImgHeight);

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
        int m_eyeToDisplay;         /**< eyes to diplay : 0 -> the both alternately, 1 -> left, 2 -> right */

        cv::Mat m_displayImage;


        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > m_leftEyeImagePort;  // make a port for reading left eye images
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > m_rightEyeImagePort; // make a port for reading right eye images
};

   
#endif



   

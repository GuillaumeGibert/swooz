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
 * \file display_kinect_main.cpp
 * \author Florian Lance
 * \date 02-05-2014
 * \brief An example program for displaying kinect data using SWoOZ platform.
 */

#include <iostream>
#include "devices/rgbd/SWKinect.h"

int main()
{
    // create an opencv window
    cvNamedWindow("rgb_kinect", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
    cvNamedWindow("cloud_map_kinect", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);

    cvMoveWindow("rgb_kinect",200,200);
    cvMoveWindow("cloud_map_kinect",200+640,200);

    swDevice::SWKinect kinectDevice;

    // init the kinect device
    if(kinectDevice.init() == -1)
    {
        std::cerr << "Error initializing kinect device. " << std::endl;
        return -1;
    }

    char key = ' ';

    // set the display loop
    while(key != 'q')
    {
        // grab new kinect frame
        kinectDevice.grab();

        // display the kinect rgb image in the opencv window
        cv::imshow("rgb_kinect",kinectDevice.bgrImage);

        // display the kinect cloud map in the opencv window
        cv::imshow("cloud_map_kinect",kinectDevice.cloudMap);

        // wait key event for escaping the loop
        key = cv::waitKey(5);
    }

    // destroy windows
    cvDestroyWindow("rgb_kinect");
    cvDestroyWindow("cloud_map_kinect");

    return 0;
}

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
 * \file display_kinect_thread_main.cpp
 * \author Florian Lance
 * \date 02-05-2014
 * \brief An example program for displaying kinect data in a thread using SWoOZ platform.
 */

#include <iostream>
#include "devices/rgbd/SWKinect_thread.h"

int main()
{
    // create an opencv window
    cvNamedWindow("rgb_kinect", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
    cvNamedWindow("cloud_map_kinect", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);

    cvMoveWindow("rgb_kinect",200,200);
    cvMoveWindow("cloud_map_kinect",200+640,200);

    swDevice::SWKinect_thread kinectDeviceT;


    // init the kinect device
    if(kinectDeviceT.init(0) == -1)
    {
        std::cerr << "Error initializing kinect device. " << std::endl;
        return -1;
    }

    // start listening the kinect device
    kinectDeviceT.startListening();

    while(!kinectDeviceT.isDataAvailable())
    {
        cv::waitKey(5);
    }

    char key = ' ';

    // set the display loop
    while(key != 'q')
    {
        // display the kinect rgb image in the opencv window
        cv::imshow("rgb_kinect" ,kinectDeviceT.bgrImage());

        // display the kinect cloud map in the opencv window
        cv::imshow("cloud_map_kinect", kinectDeviceT.cloudMap());

        // wait key event for escaping the loop
        key = cv::waitKey(5);
    }

    // stop listening kinect device    
    kinectDeviceT.stopListening();

    // destroy windows
    cvDestroyWindow("rgb_kinect");
    cvDestroyWindow("cloud_map_kinect");

    return 0;
}

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
 * \file kinect_data_saver_main.cpp
 * \author Florian Lance
 * \date 02-05-2014
 * \brief An example program for saving kinect data using SWoOZ platform.
 */


#include <iostream>
#include <time.h>
#include "devices/rgbd/SWKinect_thread.h"
#include "devices/rgbd/SWSaveKinectData.h"

#include "boost/filesystem.hpp"

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

    std::string path("./kinect_save/data_");
    boost::filesystem::path dir("./kinect_save");
    boost::filesystem::create_directory(dir);

    double maxLength = 60.0; // maximum length of the saving
    double maxSize   = 20.0; // maximum size in Go
    double fps       = 30.0; // fps

    swDevice::SWSaveKinectData dataSaver(path, maxLength, maxSize);

    bool saveVideoData = true;
    bool saveCloudData = true;
    dataSaver.start(saveVideoData, saveCloudData);

    char key = ' ';

    while(key != 'q')
    {
        clock_t time = clock();

        // display the kinect rgb image in the opencv window
        cv::imshow("rgb_kinect",kinectDeviceT.bgrImage());

        // display the kinect cloud map in the opencv window
        cv::imshow("cloud_map_kinect",kinectDeviceT.cloudMap());

        // save current kinect frame
        if(!dataSaver.save(kinectDeviceT.bgrImage(), kinectDeviceT.cloudMap()))
        {
            break;
        }

        // check time
        double elapsedTime = ((float)(clock() - time) / CLOCKS_PER_SEC);

        // wait for key events
        if(elapsedTime < 1000.0 / fps)
        {
            key = cv::waitKey(static_cast<int>(1000.0 / fps - elapsedTime));
        }
        else
        {
            key = cv::waitKey(5);
        }
    }

    // stop listening kinect data
    kinectDeviceT.stopListening();

    // stop the saving and create the data files
    dataSaver.stop();

    // destroy windows
    cvDestroyWindow("rgb_kinect");
    cvDestroyWindow("cloud_map_kinect");

    return 0;
}

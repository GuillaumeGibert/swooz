

/**
 * \file kinect_data_saver_main.cpp
 * \author Florian Lance
 * \date 02-05-2014
 * \brief An example program for saving kinect data using SWoOZ platform.
 */


#include <iostream>
#include <time.h>
#include "devices/rgbd/SWKinect.h"
#include "devices/rgbd/SWSaveKinectData.h"

#include "boost/filesystem.hpp"

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

        // grab new kinect frame
        kinectDevice.grab();

        // display the kinect rgb image in the opencv window
        cv::imshow("rgb_kinect",kinectDevice.bgrImage);

        // display the kinect cloud map in the opencv window
        cv::imshow("cloud_map_kinect",kinectDevice.cloudMap);

        // save current kinect frame
        if(!dataSaver.save(kinectDevice.bgrImage, kinectDevice.cloudMap))
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

    // stop the saving and create the data files
    dataSaver.stop();

    // destroy windows
    cvDestroyWindow("rgb_kinect");
    cvDestroyWindow("cloud_map_kinect");

    return 0;
}

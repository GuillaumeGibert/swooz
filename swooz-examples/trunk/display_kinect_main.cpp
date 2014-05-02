

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




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

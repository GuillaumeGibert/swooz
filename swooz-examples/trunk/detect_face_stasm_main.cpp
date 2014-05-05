



/**
 * \file detect_face_stasm_main.cpp
 * \author Florian Lance
 * \date 05-05-2014
 * \brief ...
 */

#include <iostream>
#include "devices/rgbd/SWKinect_thread.h"
//#include "detect/SWStasm.h"
#include "detect/SWFaceDetection.h"
#include "opencvUtility.h"

int main()
{
    // create an opencv window
    cvNamedWindow("rgb_kinect_haar1", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
    cvNamedWindow("rgb_kinect_haar2", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
    cvNamedWindow("rgb_kinect_haar3", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);

    cvMoveWindow("rgb_kinect_haar1",200,200);
    cvMoveWindow("rgb_kinect_haar2",200+640,200);
    cvMoveWindow("rgb_kinect_haar3",200,200+480);

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

    // init haar cascade detection
    swDetect::SWFaceDetection haarDetect1(cv::Size(80,80), false, "../data/classifier/haarcascade_frontalface_alt.xml");
    swDetect::SWFaceDetection haarDetect2(cv::Size(80,80), false, "../data/classifier/haarcascade_frontalface_alt2.xml");
    swDetect::SWFaceDetection haarDetect3(cv::Size(80,80), false, "../data/classifier/haarcascade_frontalface_alt_tree.xml");

    char key = ' ';

    // set the display loop
    while(key != 'q')
    {
        cv::Mat rgb = kinectDeviceT.bgrImage();

        haarDetect1.detect(rgb);
        haarDetect2.detect(rgb);
        haarDetect3.detect(rgb);

        cv::Mat displayHaar1 = rgb.clone();
        cv::Mat displayHaar2 = rgb.clone();
        cv::Mat displayHaar3 = rgb.clone();

        haarDetect1.displayFace(displayHaar1, RED);
        haarDetect2.displayFace(displayHaar2, GREEN);
        haarDetect3.displayFace(displayHaar3, BLUE);

        cv::imshow("rgb_kinect_haar1", displayHaar1);
        cv::imshow("rgb_kinect_haar2", displayHaar2);
        cv::imshow("rgb_kinect_haar3", displayHaar3);

        // wait key event for escaping the loop
        key = cv::waitKey(5);
    }

    // stop listening kinect device
    kinectDeviceT.stopListening();

    // destroy windows
    cvDestroyWindow("rgb_kinect_haar1");
    cvDestroyWindow("rgb_kinect_haar2");
    cvDestroyWindow("rgb_kinect_haar3");

    return 0;
}

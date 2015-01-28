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

//face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );


/**
 * \file detect_face_stasm_main.cpp
 * \author Florian Lance
 * \date 05-05-2014
 * \brief ...
 */

#include <iostream>
#include "devices/rgbd/SWKinect_thread.h"
//#include "detect/SWStasm.h"
//#include "detect/SWFaceDetection.h"
#include "detect/SWFaceDetection_thread.h"
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
    swDetect::SWFaceDetection_thread haarDetect1(cv::Size(80,80), false, "../data/classifier/haarcascade_frontalface_alt.xml");
    swDetect::SWFaceDetection_thread haarDetect2(cv::Size(0,0), cv::Size(100,100), false, "../data/classifier/haarcascade_mcs_nose.xml");
    swDetect::SWFaceDetection_thread haarDetect3(cv::Size(0,0), cv::Size(100,100), false, "../data/classifier/haarcascade_nose.xml");


    haarDetect1.startDetection();
    haarDetect2.startDetection();
    haarDetect3.startDetection();


    char key = ' ';

    // set the display loop
    while(key != 'q')
    {
        cv::Mat rgb = kinectDeviceT.bgrImage();

        haarDetect1.setNewRGB(rgb);
        cv::Rect faceRect = haarDetect1.getLastRect();

        if(faceRect.width > 0)
        {
            haarDetect2.setNewRGB(rgb(faceRect));
            haarDetect3.setNewRGB(rgb(faceRect));
        }
        else
        {
            haarDetect2.setNewRGB(rgb);
            haarDetect3.setNewRGB(rgb);
        }
//        haarDetect3.setNewRGB(rgb);

        cv::Mat displayHaar1 = rgb.clone();


        cv::Mat displayHaar2, displayHaar3;

        if(faceRect.width > 0)
        {
            displayHaar2 = rgb.clone()(faceRect);
            displayHaar3 = rgb.clone()(faceRect);
        }
        else
        {
            displayHaar2 = rgb.clone();
            displayHaar3 = rgb.clone();
        }

//        cv::Mat displayHaar3 = rgb.clone();

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

    haarDetect1.stopDetection();
    haarDetect2.stopDetection();
    haarDetect3.stopDetection();

    // destroy windows
    cvDestroyWindow("rgb_kinect_haar1");
    cvDestroyWindow("rgb_kinect_haar2");
    cvDestroyWindow("rgb_kinect_haar3");

    return 0;
}

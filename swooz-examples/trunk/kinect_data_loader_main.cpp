

/**
 * \file kinect_data_loader_main.cpp
 * \author Florian Lance
 * \date 02-05-2014
 * \brief An example program for loading kinect data using SWoOZ platform.
 */

#include <iostream>
#include <time.h>
#include "devices/rgbd/SWLoadKinectData.h"

int main()
{

    // create an opencv window
    cvNamedWindow("rgb_kinect", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
    cvNamedWindow("cloud_map_kinect", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
    // move opencv windows
    cvMoveWindow("rgb_kinect",200,200);
    cvMoveWindow("cloud_map_kinect",200+640,200);

    std::string path("./kinect_save/data_");

    swDevice::SWLoadKinectData dataLoader(path);

    bool saveVideoData = true;
    bool saveCloudData = true;
    double fps       = 30.0; // fps

    dataLoader.start();

    char key = ' ';
    while(key != 'q')
    {
        clock_t time = clock();

        cv::Mat rgb, cloudMap;

        // grab new kinect frame
        if(saveVideoData)
        {
            if(!dataLoader.grabVideo(rgb))
            {
                break;
            }
        }
        if(saveCloudData)
        {
            if(!dataLoader.grabCloud(cloudMap))
            {
                break;
            }
        }

        // display the kinect loaded rgb image in the opencv window
        cv::imshow("rgb_kinect",rgb);

        // display the kinect loaded cloud map in the opencv window
        cv::imshow("cloud_map_kinect",cloudMap);

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

    // stop the loading
    dataLoader.stop();

    // destroy windows
    cvDestroyWindow("rgb_kinect");
    cvDestroyWindow("cloud_map_kinect");

    return 0;
}


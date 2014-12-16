

/**
 * \file SWKinectRFModule.cpp
 * \brief Defines SWKinectRFModule
 * \author  Florian Lance
 * \date 16/12/14
 */


#include "devices/rgbd/SWKinectRFModule.h"

using namespace yarp::sig;
using namespace yarp::os;

bool SWKinectRFModule::configure()
{
    m_kinectThread.init(0);
    m_kinectThread.startListening();

    m_rgb.open("/tracking/image:o");
    m_depth.open("/tracking/depth:o");

    return true;
}


bool SWKinectRFModule::updateModule()
{
    cv::Mat rgb   = m_kinectThread.bgrImage();
    cv::Mat depth = m_kinectThread.depthMap();

    ImageOf<PixelRgb>    l_rgb;
    l_rgb = m_rgb.prepare();

    l_rgb.resize(rgb.rows, rgb.cols);
    for(int ii = 0; ii < rgb.rows; ++ii)
    {
        for(int jj = 0; jj < rgb.cols; ++jj)
        {
            PixelRgb pix;
            cv::Vec<uchar, 3> vec = rgb.at<cv::Vec<uchar,3> >(ii,jj);
            pix.r = vec[0];
            pix.g = vec[1];
            pix.b = vec[2];
            l_rgb.pixel(ii,jj) = pix;
        }
    }

    ImageOf<PixelRgbInt> l_depth;
    l_depth = m_depth.prepare();

    l_depth.resize(depth.rows, depth.cols);
    for(int ii = 0; ii < depth.rows; ++ii)
    {
        for(int jj = 0; jj < depth.cols; ++jj)
        {
            PixelRgbInt pix;
            cv::Vec<ushort, 1> vec = depth.at<cv::Vec<ushort,1> >(ii,jj);
            pix.r = static_cast<int>(vec[0]);
            pix.g = static_cast<int>(vec[1]);
            pix.b = static_cast<int>(vec[2]);
            l_depth.pixel(ii,jj) = pix;
        }
    }

    m_rgb.write();
    m_depth.write();

    return true;
}


double SWKinectRFModule::getPeriod()
{
    return 30.0/1000.0;
}

bool SWKinectRFModule::interruptModule()
{

    m_rgb.interrupt();
    m_depth.interrupt();
    std::cout << "--> Interrupting the Kinect Tracking module..." << std::endl;
    return true;
}

bool SWKinectRFModule::close()
{
    m_kinectThread.stopListening();

    m_rgb.close();
    m_depth.close();

    return true;
}

int main(int argc, char* argv[])
{
    /* initialize yarp network */
    yarp::os::Network yarp;
    if (!yarp.checkNetwork())
    {
        std::cout << "-ERROR: Problem connecting to YARP server" << std::endl;
        return -1;
    }

    /* create your module */
    SWKinectRFModule  l_kinectModule;


    /* configure the module */
    std::cout << "Configuring the kinect module..."<< std::endl;
    if (l_kinectModule.configure())
    {
        /* run the module */
        std::cout << "Starting the kinect module..." << std::endl;
        l_kinectModule.runModule();
    }
    else
    {
        std::cout << "Failed to configure the kinect module!"<< std::endl;
    }
    return 0;
}

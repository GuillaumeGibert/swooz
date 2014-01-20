

/**
 * \file SWKinect_thread.cpp
 * \brief Defines SWKinect_thread
 * \author Florian Lance
 * \date 20/06/13
 */

#include "devices/rgbd/SWKinect_thread.h"

#include "SWExceptions.h"

using namespace cv;
using namespace std;
using namespace swDevice;
using namespace swExcept;

SWKinect_thread::SWKinect_thread(bool bVerbose) : m_oKinect(SWKinect(bVerbose)),m_bInitialized(false), m_bDataAvailable(false)
{}

SWKinect_thread::~SWKinect_thread(void)
{
	if(m_bListening)
	{
		stopListening();
	}
}

int SWKinect_thread::init(cint i32CaptureMode)
{
    try
    {
        m_oKinect.init(i32CaptureMode);
    }
    catch( const swKinectError &e)
    {
        cerr << "SWKinect_thread::init " << e.what() << endl;
        m_bInitialized = false;
        return -1;
    }

    m_bInitialized = true;

    return 0;
}

void SWKinect_thread::setRecalibration(cbool bRecalib, cint i32XCalibrate, cint i32YCalibrate)
{
	boost::lock_guard<boost::mutex> lock(m_oMutex);
	m_oKinect.setRecalibration(bRecalib, i32XCalibrate, i32YCalibrate);
}

void SWKinect_thread::startListening()
{
	if(m_bInitialized)
	{
		m_bListening 	   = true;
		m_pListeningThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&SWKinect_thread::doWork, this)));	
		
		int l_i32WaitTimeOut = 0;

		while(!isDataAvailable())
		{			
			boost::this_thread::sleep(boost::posix_time::milliseconds(10));

			if(l_i32WaitTimeOut % 10 == 0)
			{
                if(m_oKinect.m_bVerbose)
                    std::cout << "Waiting  for kinect data... " << std::endl;
			}
			if(l_i32WaitTimeOut > 500)
			{
				std::cerr << "Timeout... " << std::endl;
				throw kinectDataTimeOutError();
			}

			++l_i32WaitTimeOut;
		}

        if(m_oKinect.m_bVerbose)
            std::cout << "Kinect data available. " << std::endl;
	}
	else
	{        
		std::cerr << "Kinect module not initialized, the thread can't be started. " << std::endl;
		throw kinectInitError();		
	}
}

void SWKinect_thread::stopListening()
{
	if(m_bListening)
	{
		m_bListening 	 = false;
		m_bDataAvailable = false;
		m_pListeningThread->join();		
	}
}

void SWKinect_thread::doWork()
{
	while(m_bListening)
	{			
		if(m_oKinect.grab() != -1)
		{
			boost::lock_guard<boost::mutex> lock(m_oMutex);
			m_oDisparityMap 	= m_oKinect.disparityMap.clone();
            m_oCloudMap         = m_oKinect.cloudMap.clone();
            m_oBgrImage         = m_oKinect.bgrImage.clone();
            m_oDepthMap         = m_oKinect.depthMap.clone();
			m_oGrayImage		= m_oKinect.grayImage.clone();	
			m_bDataAvailable	= true;
		}
	}
}

bool SWKinect_thread::isDataAvailable()
{
	boost::lock_guard<boost::mutex> lock(m_oMutex);
	return m_bDataAvailable;
}

cv::Mat SWKinect_thread::cloudMap()
{
	{
		boost::lock_guard<boost::mutex> lock(m_oMutex);

		if(m_bDataAvailable)
		{
            return m_oCloudMap;
		}
	}
	
	return cv::Mat(640, 480, CV_32FC3);
}

cv::Mat SWKinect_thread::bgrImage()
{
	{
		boost::lock_guard<boost::mutex> lock(m_oMutex);
	
		if(m_bDataAvailable)
		{
            return m_oBgrImage;
		}
	}
	
	return cv::Mat(m_oKinect.sizeFrame(), CV_8UC3);
}

cv::Mat SWKinect_thread::depthMap()
{
	{
		boost::lock_guard<boost::mutex> lock(m_oMutex);
	
		if(m_bDataAvailable)
		{
            return m_oDepthMap;
		}
	}
	
	return cv::Mat(640, 480, CV_16UC1);
}

cv::Mat SWKinect_thread::disparityMap()
{
	{
		boost::lock_guard<boost::mutex> lock(m_oMutex);
	
		if(m_bDataAvailable)
		{
            return m_oDisparityMap;
		}
	}
	
	return cv::Mat(640, 480, CV_8UC1);
}

cv::Mat SWKinect_thread::grayImage()
{
	{
		boost::lock_guard<boost::mutex> lock(m_oMutex);
	
		if(m_bDataAvailable)
		{
            return m_oGrayImage;
		}
	}
	
	return cv::Mat(m_oKinect.sizeFrame(), CV_8UC1);
}


cv::Size SWKinect_thread::sizeFrame()
{
	return cv::Size((int)m_oKinect.m_oCapture.get(CV_CAP_PROP_FRAME_WIDTH), (int)m_oKinect.m_oCapture.get(CV_CAP_PROP_FRAME_HEIGHT));
}

int SWKinect_thread::fps()
{
	return (int)m_oKinect.m_oCapture.get(CV_CAP_PROP_FPS);
}

int SWKinect_thread::captureMode()
{
	return m_oKinect.m_i32CaptureMode;
}

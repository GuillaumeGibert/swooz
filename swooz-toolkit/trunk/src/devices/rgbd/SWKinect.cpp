
/**
 * \file SWKinect.cpp
 * \brief Defines SWFastrak_thread
 * \author Florian Lance
 * \date 20/06/13
 */

#include "devices/rgbd/SWKinect.h"
#include "SWExceptions.h"

using namespace cv;
using namespace std;
using namespace swDevice;
using namespace swExcept;

SWKinect::SWKinect(bool bVerbose) : m_bVerbose(bVerbose)
{
	// this should be adjusted through a config file
	minDepthValue = 400; // in mm
	maxDepthValue = 1670; // in mm

	minDepthValue = 800; // in mm
	maxDepthValue = 3350; // in mm
	
    m_bKinectInitalized = false;
    m_bRecalibrate      = false;//true;
	m_i32XCalibrate = 0;//4;
	m_i32YCalibrate = 0;//-7;
}

SWKinect::~SWKinect(void)
{}

int SWKinect::init(cint i32CaptureMode)
{
	m_i32CaptureMode = i32CaptureMode;
	
    if(m_bVerbose)
        cout << "Device opening ..." << endl;

    m_oCapture.open( CV_CAP_OPENNI );
	
    if( !m_oCapture.isOpened() )
	{
		cerr << "-ERROR: Cannot open a capture object!" << endl;
        throw kinectInitError();
		return -1;
	}
	
	// checks if the depth and rgb image are calibrated
    if(m_oCapture.get( CV_CAP_PROP_OPENNI_REGISTRATION ) == 0)
    {
        m_oCapture.set(CV_CAP_PROP_OPENNI_REGISTRATION,1);
    }
	
	// set the image output mode
    if(m_i32CaptureMode > 4 || m_i32CaptureMode < 0)
    {
        cerr << "Kinect, invalid capture mode. Parameter set to 0. " << endl;
        m_i32CaptureMode = 0;
    }

	if     (m_i32CaptureMode == 0)
	{
        if(m_bVerbose)
            cout << "Kinect capture mode set to CV_CAP_OPENNI_VGA_30HZ  " << endl;
        m_oCapture.set( CV_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CV_CAP_OPENNI_VGA_30HZ );
	}
	else if(m_i32CaptureMode == 1)
	{
        if(m_bVerbose)
            cout << "Kinect capture mode set to CV_CAP_OPENNI_SXGA_15HZ  " << endl;
        m_oCapture.set( CV_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CV_CAP_OPENNI_SXGA_15HZ );
	}
	else if(m_i32CaptureMode == 2)
	{
        if(m_bVerbose)
            cout << "Kinect capture mode set to CV_CAP_OPENNI_SXGA_30HZ  " << endl;
        m_oCapture.set( CV_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CV_CAP_OPENNI_SXGA_30HZ );
	}
	else if(m_i32CaptureMode == 3)
	{
        if(m_bVerbose)
            cout << "Kinect capture mode set to CV_CAP_OPENNI_QVGA_30HZ  " << endl;
        m_oCapture.set( CV_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CV_CAP_OPENNI_QVGA_30HZ );
	}
	else if(m_i32CaptureMode == 4)
	{
        if(m_bVerbose)
            cout << "Kinect capture mode set to CV_CAP_OPENNI_QVGA_60HZ  " << endl;
        m_oCapture.set( CV_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CV_CAP_OPENNI_QVGA_60HZ );
	}
	
    if(m_bVerbose)
        cout << "\nDepth generator output mode:" << endl <<
            "FRAME_WIDTH      " << m_oCapture.get( CV_CAP_PROP_FRAME_WIDTH ) << endl <<
            "FRAME_HEIGHT     " << m_oCapture.get( CV_CAP_PROP_FRAME_HEIGHT ) << endl <<
            "FRAME_MAX_DEPTH  " << m_oCapture.get( CV_CAP_PROP_OPENNI_FRAME_MAX_DEPTH ) << " mm" << endl <<
            "FPS              " << m_oCapture.get( CV_CAP_PROP_FPS ) << endl <<
            "REGISTRATION     " << m_oCapture.get( CV_CAP_PROP_OPENNI_REGISTRATION ) << endl;
	
    if( m_oCapture.get( CV_CAP_OPENNI_IMAGE_GENERATOR_PRESENT ) )
	{        
        if(m_bVerbose)
            cout <<
                "\nImage generator output mode:" << endl <<
                "FRAME_WIDTH   " << m_oCapture.get( CV_CAP_OPENNI_IMAGE_GENERATOR+CV_CAP_PROP_FRAME_WIDTH ) << endl <<
                "FRAME_HEIGHT  " << m_oCapture.get( CV_CAP_OPENNI_IMAGE_GENERATOR+CV_CAP_PROP_FRAME_HEIGHT ) << endl <<
                "FPS           " << m_oCapture.get( CV_CAP_OPENNI_IMAGE_GENERATOR+CV_CAP_PROP_FPS ) << endl;

        m_bKinectInitalized = true;

		return 0;
	}
	else
	{
        cerr << "-ERROR: Device doesn't contain image generator." << endl;
        throw kinectInitError();
		return -1;
	}
}

void SWKinect::setRecalibration(cbool bRecalib, cint i32XCalibrate, cint i32YCalibrate)
{
	m_bRecalibrate  = bRecalib;
	m_i32XCalibrate = i32XCalibrate;
	m_i32YCalibrate	= i32YCalibrate;
}

int SWKinect::grab(void)
{
    if(!m_bKinectInitalized)
    {
        cerr << "-ERROR: grab, SWKinect. init must be called before. " << endl;
        throw kinectInitError();
        return -1;
    }

    if( !m_oCapture.grab() )
    {
        cerr << "-ERROR: Cannot grab images!" << endl;
        throw kinectGrabError();
        return -1;
    }
    else
    {
		// CV_CAP_OPENNI_DEPTH_MAP 	
		// CV_CAP_OPENNI_POINT_CLOUD_MAP 	
		// CV_CAP_OPENNI_DISPARITY_MAP 	
		// CV_CAP_OPENNI_DISPARITY_MAP_32F 	
		// CV_CAP_OPENNI_VALID_DEPTH_MASK 	
		// CV_CAP_OPENNI_BGR_IMAGE 	
		// CV_CAP_OPENNI_GRAY_IMAGE 		
		
		// Depth Image
        m_oCapture.retrieve(depthMap,     CV_CAP_OPENNI_DEPTH_MAP);
		// Disparity Image
        m_oCapture.retrieve(disparityMap, CV_CAP_OPENNI_DISPARITY_MAP); // CV_CAP_OPENNI_DISPARITY_MAP_32F
		// RGB Image
        m_oCapture.retrieve(bgrImage,     CV_CAP_OPENNI_BGR_IMAGE);
		// Cloud map
        m_oCapture.retrieve(cloudMap,     CV_CAP_OPENNI_POINT_CLOUD_MAP);

		// gray image
        m_oCapture.retrieve(grayImage,    CV_CAP_OPENNI_GRAY_IMAGE);
		
		if(m_bRecalibrate && (m_i32XCalibrate != 0 || m_i32YCalibrate != 0))
		{
			recalibrate();
		}
	}


    // create normalized depth map (for dimenco)
    normalizeDepthImage();
	
	return 0;
}

void SWKinect::recalibrate()
{
	cv::Scalar l_oBlack(0,0,0), l_oCurrColor;
	cv::Mat l_oNewBgrImage(cv::Size(bgrImage.size()), CV_8UC3, l_oBlack);
	
	for(int ii = 0; ii < l_oNewBgrImage.rows; ++ii)
	{
		for(int jj = 0; jj < l_oNewBgrImage.cols; ++jj)
		{		
			int l_i32CurrII;
			int l_i32CurrJJ;
			bool l_bPaintBlack = false;			

			if(m_i32XCalibrate < 0)
			{
				if( jj < l_oNewBgrImage.cols + m_i32XCalibrate-1)
				{
					l_i32CurrJJ = jj - m_i32XCalibrate;					
				}
				else
				{
					l_bPaintBlack = true;
				}
			}
			else
			{
				if( jj > m_i32XCalibrate)
				{
					l_i32CurrJJ = jj - m_i32XCalibrate;
				}	
				else
				{
					l_bPaintBlack = true;
				}				
			}
			
			if(m_i32YCalibrate < 0)
			{
				if( ii < l_oNewBgrImage.rows + m_i32YCalibrate-1)
				{
					l_i32CurrII = ii - m_i32YCalibrate;				
				}
				else
				{
					l_bPaintBlack = true;
				}				
			}
			else
			{
				if( ii > m_i32YCalibrate)
				{
					l_i32CurrII = ii - m_i32YCalibrate;
				}		
				else
				{
					l_bPaintBlack = true;
				}				
			}			
			
			if(!l_bPaintBlack)
			{					
				l_oNewBgrImage.at<cv::Vec3b> (ii,jj) = bgrImage.at<cv::Vec3b>(l_i32CurrII,l_i32CurrJJ);					
			}			
		}
	}
	
	
	bgrImage = l_oNewBgrImage.clone();
}

cv::Size SWKinect::sizeFrame()
{
    return cv::Size((int)m_oCapture.get(CV_CAP_PROP_FRAME_WIDTH), (int)m_oCapture.get(CV_CAP_PROP_FRAME_HEIGHT));
}

int SWKinect::fps()
{
    return (int)m_oCapture.get(CV_CAP_PROP_FPS);
}

void SWKinect::normalizeDepthImage(void)
{
	normalizedDepthMap = Mat(depthMap.rows, depthMap.cols, CV_32FC1);
	
	for (int l_row=0; l_row<depthMap.rows; l_row++)
	{
		for (int l_col =0; l_col<depthMap.cols; l_col++)
		{
			if (depthMap.at<unsigned short>(l_row,l_col) <= minDepthValue)
			{
				normalizedDepthMap.at<float>(l_row,l_col) = 0.0f;
			}
			if (depthMap.at<unsigned short>(l_row,l_col) >= maxDepthValue)
			{
				normalizedDepthMap.at<float>(l_row,l_col) = 1.0f;
			}
			if (depthMap.at<unsigned short>(l_row,l_col) < maxDepthValue && depthMap.at<unsigned short>(l_row,l_col) > minDepthValue)
			{
				normalizedDepthMap.at<float>(l_row,l_col) = ((float)depthMap.at<unsigned short>(l_row,l_col) - minDepthValue) / (maxDepthValue-minDepthValue);
			}
		}
	}
//    std::cout << "normalizedDepthMap.at<float>(l_row,l_col) = " <<normalizedDepthMap.at<float>(depthMap.rows/2,depthMap.cols/2) <<std::endl;
}

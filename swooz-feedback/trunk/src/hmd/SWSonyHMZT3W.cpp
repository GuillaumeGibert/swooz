
/**
 * \file SWSonyHMZT3W.cpp
 * \brief Defines SWSonyHMZT3W class
 * \author Guillaume Gibert
 * \date 17/07/14
 */


#include "hmd/SWSonyHMZT3W.h"

using namespace yarp::sig;
using namespace yarp::os;

bool SWSonyHMZT3W::loop()
{
	ImageOf<PixelRgb> *inputImage1 = NULL;
	ImageOf<PixelRgb> *inputImage2 = NULL;
	
    if(m_eyeToDisplay == 0)
    {
	inputImage1 = m_rightEyeImagePort.read(false);
        inputImage2 = inputImage1;
    }
    else if(m_eyeToDisplay == 1)
    {
        inputImage1 = m_leftEyeImagePort.read(false);
        inputImage2 = inputImage1;
    }
    else
    {
	inputImage1 = m_leftEyeImagePort.read(false);
        inputImage2 = m_rightEyeImagePort.read(false);
    }

    if (inputImage1!=NULL && inputImage2!=NULL)
    {
        // creates opencv image to store the input images
	cv::Mat inBgrImg(cv::Size(inputImage1->width()+inputImage2->width(), inputImage1->height()), CV_8UC3, cv::Scalar::all(0));
        cv::Mat inBgrImg1(cv::Size(inputImage1->width(), inputImage1->height()), CV_8UC3, cv::Scalar::all(0));
	cv::Mat inBgrImg2(cv::Size(inputImage2->width(), inputImage2->height()), CV_8UC3, cv::Scalar::all(0));

        // fills the first image
        for (int x=0; x<inputImage1->width(); x++)
        {
            for (int y=0; y<inputImage1->height(); y++)
            {
                PixelRgb& pixel = inputImage1->pixel(x,y);

                inBgrImg1.data[inBgrImg1.step[0]*y + inBgrImg1.step[1]* x + 0] = pixel.b;
                inBgrImg1.data[inBgrImg1.step[0]*y + inBgrImg1.step[1]* x + 1] = pixel.g;
                inBgrImg1.data[inBgrImg1.step[0]*y + inBgrImg1.step[1]* x + 2] = pixel.r;
            }
        }
	
	// fills the second image
        for (int x=0; x<inputImage2->width(); x++)
        {
            for (int y=0; y<inputImage2->height(); y++)
            {
                PixelRgb& pixel = inputImage2->pixel(x,y);

                inBgrImg2.data[inBgrImg2.step[0]*y + inBgrImg2.step[1]* x + 0] = pixel.b;
                inBgrImg2.data[inBgrImg2.step[0]*y + inBgrImg2.step[1]* x + 1] = pixel.g;
                inBgrImg2.data[inBgrImg2.step[0]*y + inBgrImg2.step[1]* x + 2] = pixel.r;
            }
        }

	// concats the 2 images
	cv::hconcat(inBgrImg1, inBgrImg2, inBgrImg);
	
        // resizes to the output size
        cv::resize(inBgrImg, m_diplayImage, m_diplayImage.size(),0,0,CV_INTER_LINEAR);
    }



    // test 3D
//    m_diplayImage.resize();


    // display current output
    cv::imshow("SonyHMZT3W", m_diplayImage);
    char l_key = cv::waitKey(1);

    if(l_key == 'q')
    {
        std::cout << "Leave program. " << std::endl;
        return false;
    }
    else if(l_key == 'f')
    {
        if(!m_fullScreen)
        {
            std::cout << "Enable fullscreen. " << std::endl;
            cv::destroyWindow("SonyHMZT3W");
            cv::namedWindow("SonyHMZT3W", CV_WINDOW_NORMAL);
            cv::setWindowProperty("SonyHMZT3W", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
            m_fullScreen = true;
        }
        else
        {
            std::cout << "Disable fullscreen. " << std::endl;
            cv::destroyWindow("SonyHMZT3W");
            cv::namedWindow("SonyHMZT3W", CV_WINDOW_AUTOSIZE);
            m_fullScreen = false;
        }
    }
    else if(l_key == 'e')
    {
        if(m_eyeToDisplay == 0)
        {
            std::cout << "Display only left eye. " << std::endl;
            m_eyeToDisplay = 1;
        }
        else if(m_eyeToDisplay == 1)
        {
            std::cout << "Display left and right eye alternately. " << std::endl;
            m_eyeToDisplay = 2;
        }
        else
        {
            std::cout << "Display only right eye. " << std::endl;
            m_eyeToDisplay = 0;
        }
    }

    return true;

}


bool SWSonyHMZT3W::open(int displayImgWidth, int displayImgHeight)
{
    bool ret=true;
    ret=m_leftEyeImagePort.open("/hmd/left/in");  // give the port a name
    ret = ret && m_rightEyeImagePort.open("/hmd/right/in");  // give the port a name

    // Filled from ini file
    m_displayImgWidth  = displayImgWidth;
    m_displayImgHeight = displayImgHeight;
    m_diplayImage = cv::Mat(cv::Size(m_displayImgWidth, m_displayImgHeight), CV_8UC3, cv::Scalar::all(0));

    // creates a full screen cv window
    cv::namedWindow("SonyHMZT3W", CV_NORMAL);
    cv::setWindowProperty("SonyHMZT3W", CV_WINDOW_AUTOSIZE, CV_WINDOW_AUTOSIZE);
    cv::waitKey(1);

    return ret;
}


bool SWSonyHMZT3W::close()
{
    m_leftEyeImagePort.close();
    m_rightEyeImagePort.close();

    cv::destroyWindow("SonyHMZT3W");

    return true;
}


bool SWSonyHMZT3W::interrupt()
{
    m_leftEyeImagePort.interrupt();
    m_rightEyeImagePort.interrupt();

    return true;
}


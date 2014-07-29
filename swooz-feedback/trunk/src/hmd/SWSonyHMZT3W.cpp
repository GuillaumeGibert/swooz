
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
    ImageOf<PixelRgb> *inputImage = NULL;

    if(m_eyeToDisplay == 0)
    {
        if (m_leftRightImg) //left image
        {
            inputImage= m_leftEyeImagePort.read(false);
            m_leftRightImg = false;

        }
        else //right image
        {
            inputImage= m_rightEyeImagePort.read(false);
            m_leftRightImg = true;
        }
    }
    else if(m_eyeToDisplay == 1)
    {
        inputImage= m_leftEyeImagePort.read(false);
    }
    else
    {
        inputImage= m_rightEyeImagePort.read(false);
    }

    if (inputImage!=NULL)
    {
        // creates opencv image to store the input images
        cv::Mat inBgrImg(cv::Size(inputImage->width(), inputImage->height()), CV_8UC3, cv::Scalar::all(0));

        // fills the left image
        for (int x=0; x<inputImage->width(); x++)
        {
            for (int y=0; y<inputImage->height(); y++)
            {
                PixelRgb& pixel = inputImage->pixel(x,y);

                inBgrImg.data[inBgrImg.step[0]*y + inBgrImg.step[1]* x + 0] = pixel.b;
                inBgrImg.data[inBgrImg.step[0]*y + inBgrImg.step[1]* x + 1] = pixel.g;
                inBgrImg.data[inBgrImg.step[0]*y + inBgrImg.step[1]* x + 2] = pixel.r;
            }
        }

        // resizes to the output size
        cv::resize(inBgrImg, m_diplayImage, m_diplayImage.size(),0,0,CV_INTER_LINEAR);
    }

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
            std::cout << "Display only right eye. " << std::endl;
            m_eyeToDisplay = 2;
        }
        else
        {
            std::cout << "Display left and right eye alternately. " << std::endl;
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

    // starts with left eye
    m_leftRightImg = true;

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


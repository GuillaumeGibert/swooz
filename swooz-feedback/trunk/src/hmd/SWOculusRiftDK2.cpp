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

/**
 * \file SWOculusRiftDK2.cpp
 * \brief Defines SWOculusRiftDK2 class
 * \author Guillaume Gibert
 * \date 04/06/2015
 */


#include "hmd/SWOculusRiftDK2.h"

using namespace yarp::sig;
using namespace yarp::os;

bool SWOculusRiftDK2::loop()
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


//    if(inputImage1 != NULL)
//    {
//        if(inputImage1->getRowSize() == 0)
//        {
//            return true;
//        }
//    }
//    if(inputImage2 != NULL)
//    {
//        if(inputImage2->getRowSize() == 0)
//        {
//            return true;
//        }
//    }

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
        cv::resize(inBgrImg, m_displayImage, m_displayImage.size(),0,0,CV_INTER_LINEAR);
    }



    // test 3D
//    m_diplayImage.resize();

    // display current output
//    cv::imshow("SWOculusRiftDK2", m_displayImage);
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
            cv::destroyWindow("SWOculusRiftDK2");
            cv::namedWindow("SWOculusRiftDK2", CV_WINDOW_NORMAL);
            cv::setWindowProperty("SWOculusRiftDK2", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
            m_fullScreen = true;
        }
        else
        {
            std::cout << "Disable fullscreen. " << std::endl;
            cv::destroyWindow("SWOculusRiftDK2");
            cv::namedWindow("SWOculusRiftDK2", CV_WINDOW_AUTOSIZE);
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


bool SWOculusRiftDK2::open(int displayImgWidth, int displayImgHeight)
{
    bool ret=true;
    ret=m_leftEyeImagePort.open("/hmd/left/in");  // give the port a name
    ret = ret && m_rightEyeImagePort.open("/hmd/right/in");  // give the port a name

    // Filled from ini file
    m_displayImgWidth  = displayImgWidth;
    m_displayImgHeight = displayImgHeight;
    m_displayImage = cv::Mat(cv::Size(m_displayImgWidth, m_displayImgHeight), CV_8UC3, cv::Scalar::all(0));

    // creates a full screen cv window
    cv::namedWindow("SWOculusRiftDK2", CV_NORMAL);
    cv::setWindowProperty("SWOculusRiftDK2", CV_WINDOW_AUTOSIZE, CV_WINDOW_AUTOSIZE);
    cv::waitKey(1);

    return ret;
}


bool SWOculusRiftDK2::close()
{
    m_leftEyeImagePort.close();
    m_rightEyeImagePort.close();

    cv::destroyWindow("SWOculusRiftDK2");

    return true;
}


bool SWOculusRiftDK2::interrupt()
{
    m_leftEyeImagePort.interrupt();
    m_rightEyeImagePort.interrupt();

    return true;
}


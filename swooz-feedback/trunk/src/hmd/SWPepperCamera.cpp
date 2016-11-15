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
 * \file SWPepperCamera.cpp
 * \brief Defines SWPepperCamera class
 * \author Guillaume Gibert
 * \date 14/11/16
 */


#include "hmd/SWPepperCamera.h"


bool SWPepperCamera::loop()
{
	/** Create an cv::Mat header to wrap into an opencv image.*/
	cv::Mat imgHeader = cv::Mat(cv::Size(m_i32ImageWidth, m_i32ImageHeight), CV_8UC3);
	
	/** Retrieve an image from the camera.
	* The image is returned in the form of a container object, with the
	* following fields:
	* 0 = width
	* 1 = height
	* 2 = number of layers
	* 3 = colors space index (see alvisiondefinitions.h)
	* 4 = time stamp (seconds)
	* 5 = time stamp (micro seconds)
	* 6 = image buffer (size of width * height * number of layers)
	*/
	ALValue img = camProxy->getImageRemote(clientName);

	/** Access the image buffer (6th field) and assign it to the opencv image
	* container. */
	imgHeader.data = (uchar*) img[6].GetBinary();

	/** Tells to ALVideoDevice that it can give back the image buffer to the
	* driver. Optional after a getImageRemote but MANDATORY after a getImageLocal.*/
	camProxy->releaseImage(clientName);
	
	if (imgHeader.cols != 0 && imgHeader.rows != 0)
	{
		// creates opencv image to store the input images
		cv::Mat inBgrImg(cv::Size(imgHeader.cols+imgHeader.cols, imgHeader.rows), CV_8UC3, cv::Scalar::all(0));

		// concats the 2 images
		cv::hconcat(imgHeader, imgHeader, inBgrImg);

		// resizes to the output size
		cv::resize(inBgrImg, m_displayImage, m_displayImage.size(),0,0,CV_INTER_LINEAR);
	}


	// display current output
	cv::imshow("PepperCamera", m_displayImage);
	
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
		    cv::destroyWindow("PepperCamera");
		    cv::namedWindow("PepperCamera", CV_WINDOW_NORMAL);
		    cv::setWindowProperty("PepperCamera", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		    m_fullScreen = true;
		}
		else
		{
		    std::cout << "Disable fullscreen. " << std::endl;
		    cv::destroyWindow("PepperCamera");
		    cv::namedWindow("PepperCamera", CV_WINDOW_AUTOSIZE);
		    m_fullScreen = false;
		}
	}
	

	return true;
}


bool SWPepperCamera::open(int displayImgWidth, int displayImgHeight, std::string sRobotAddress, int pepperPort, std::string sPepperVideoMode, int fps)
{
	std::cout << displayImgWidth << ", " << displayImgHeight << ", " << sRobotAddress << ", " << pepperPort << ", " << sPepperVideoMode << ", " << fps << std::endl;
	if (!sPepperVideoMode.compare("kQQQQVGA"))
	{
		m_i32ImageWidth = 40;
		m_i32ImageHeight = 30;
		m_i32PepperVideoMode = AL::kQQQQVGA;
	}
	else if (!sPepperVideoMode.compare("kQQQVGA"))
	{
		m_i32ImageWidth = 80;
		m_i32ImageHeight = 60;
		m_i32PepperVideoMode = AL::kQQQVGA;
	}	
	else if (!sPepperVideoMode.compare("kQQVGA"))
	{
		m_i32ImageWidth = 160;
		m_i32ImageHeight = 120;
		m_i32PepperVideoMode = AL::kQQVGA;
	}
	else if (!sPepperVideoMode.compare("kQVGA"))
	{
		m_i32ImageWidth = 320;
		m_i32ImageHeight = 240;
		m_i32PepperVideoMode = AL::kQVGA;
	}
	else if (!sPepperVideoMode.compare("kVGA"))
	{
		m_i32ImageWidth = 640;
		m_i32ImageHeight = 480;
		m_i32PepperVideoMode = AL::kVGA;
	}
	else if (!sPepperVideoMode.compare("k4VGA"))
	{
		m_i32ImageWidth = 1280;
		m_i32ImageHeight = 960;
		m_i32PepperVideoMode = AL::k4VGA;
	}
	else if (!sPepperVideoMode.compare("k16VGA"))
	{
		m_i32ImageWidth = 2560;
		m_i32ImageHeight = 1920;
		m_i32PepperVideoMode = AL::k16VGA;
	}
	else
	{
		std::cerr << "[ERROR]> Video mode not supported!" << std::endl;
		return false;
	}
	
	
	try
	{
		/** Create a proxy to ALVideoDevice on the robot.*/
		camProxy = new ALVideoDeviceProxy(sRobotAddress, pepperPort);
	
		/** Subscribe a client image requiring sPepperVideoMode and BGR colorspace.*/
		clientName = camProxy->subscribe("test", m_i32PepperVideoMode, kBGRColorSpace, fps);
		
	}
	catch (const AL::ALError& e)
	{
		std::cerr << "Caught exception " << e.what() << std::endl;
		return false;
	}
	
	
	// Filled from ini file
	
	m_displayImgWidth  = displayImgWidth;
	m_displayImgHeight = displayImgHeight;
	m_displayImage = cv::Mat(cv::Size(m_displayImgWidth, m_displayImgHeight), CV_8UC3, cv::Scalar::all(0));

	// creates a full screen cv window
	cv::namedWindow("PepperCamera", CV_NORMAL);
	cv::setWindowProperty("PepperCamera", CV_WINDOW_AUTOSIZE, CV_WINDOW_AUTOSIZE);
	cv::waitKey(10);

	return true;
}


bool SWPepperCamera::close()
{
	/** Cleanup.*/
	camProxy->unsubscribe(clientName);
	if (NULL != camProxy)
	{
		delete camProxy;
		camProxy = NULL;
	}
	cv::destroyWindow("PepperCamera");

	return true;
}


bool SWPepperCamera::interrupt()
{
	return true;
}


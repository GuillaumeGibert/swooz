#include "hmd\SWSonyHMZT3W.h"

using namespace yarp::sig;
using namespace yarp::os;

void SWSonyHMZT3W::loop()
{
	ImageOf<PixelRgb> *inputImage;
	
	if (left_rightImg) //left image
	{
		inputImage= leftEyeImagePort.read();  // read an image
		left_rightImg = false;
		
	}
	else //right image
	{
		inputImage= rightEyeImagePort.read();  // read an image
		left_rightImg = true;
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
		
		// creates opencv image for output	
		cv::Mat displayImg(cv::Size(displayImgWidth, displayImgHeight), CV_8UC3, cv::Scalar::all(0));
		
		// resizes to the output size
		cv::resize(inBgrImg, displayImg, displayImg.size(),0,0,CV_INTER_LINEAR);
		
		//~ std::cout << "image size = (" <<  displayImg.rows << ", " <<  displayImg.cols << " )" << std::endl;
		
		cv::imshow("SonyHMZT3W", displayImg);
		cv::waitKey(1);
	}
		
}


bool SWSonyHMZT3W::open(int displayImgWidth, int displayImgHeight)
{
	bool ret=true;
	ret=leftEyeImagePort.open("/hmd/left/in");  // give the port a name
	ret = ret && rightEyeImagePort.open("/hmd/right/in");  // give the port a name
	
	// Filled from ini file
	this->displayImgWidth = displayImgWidth;
	this->displayImgHeight = displayImgHeight;
	
	// starts with left eye
	left_rightImg = true;
	
	// creates a full screen cv window
	cv::namedWindow("SonyHMZT3W", CV_WINDOW_AUTOSIZE);
	cv::waitKey(1);
	
	return ret;
}


bool SWSonyHMZT3W::close()
{
	leftEyeImagePort.close();
	rightEyeImagePort.close();
	
	cvDestroyWindow("SonyHMZT3W");
	
	return true;
}


bool SWSonyHMZT3W::interrupt()
{
	leftEyeImagePort.interrupt();
	rightEyeImagePort.interrupt();
	
	return true;
}


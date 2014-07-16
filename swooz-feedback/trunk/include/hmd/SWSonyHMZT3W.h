#ifndef _SWSONYHMZT3W_
#define _SWSONYHMZT3W_

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "opencvUtility.h"
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


class SWSonyHMZT3W
{
	yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > leftEyeImagePort;  // make a port for reading images
	yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > rightEyeImagePort;
	
	// display infos
        int displayImgWidth;        /**< ... */
        int displayImgHeight;       /**< ... */
	bool left_rightImg;
	
	public:

		SWSonyHMZT3W()
		{
		// constructor
		}

		bool open(int displayImgWidth, int displayImgHeight);

		bool close();

		void loop(); 

		bool interrupt();

};

   
#endif



   
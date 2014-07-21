
/**
 * \file SWSonyHMZT3W.h
 * \brief Defines SWSonyHMZT3W class
 * \author Guillaume Gibert
 * \date 17/07/14
 */


#ifndef _SWSONYHMZT3W_
#define _SWSONYHMZT3W_

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "opencvUtility.h"
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

/**
 * \class SWSonyHMZT3W
 * \brief ...
 */
class SWSonyHMZT3W
{
    public:

        /**
         * @brief SWSonyHMZT3W
         */
        SWSonyHMZT3W(){}

        /**
         * @brief open
         * @param displayImgWidth
         * @param displayImgHeight
         * @return
         */
        bool open(int displayImgWidth, int displayImgHeight);

        /**
         * @brief close
         * @return
         */
		bool close();

        /**
         * @brief loop
         */
		void loop(); 

        /**
         * @brief interrupt
         * @return
         */
		bool interrupt();

    private :

        // display infos
        bool left_rightImg;         /**< ... */
        int displayImgWidth;        /**< ... */
        int displayImgHeight;       /**< ... */


        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > leftEyeImagePort;  // make a port for reading images
        yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > rightEyeImagePort;
};

   
#endif



   

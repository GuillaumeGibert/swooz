
#ifndef _SWDIMENCO3DDISPLAY_
#define _sWDIMENCO3DDISPLAY_

#include "targetver.h"
#include <tchar.h>

#include "commonTypes.h"
#include "opencvUtility.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Windows.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <fstream>

/**
 * @brief The SWDimenco3DDisplay class
 */
class SWDimenco3DDisplay
{
	public:

        /**
         * @brief SWDimenco3DDisplay destructor
         */
		virtual ~SWDimenco3DDisplay(void);

	public: 
		// display infos
        int displaySize;            /**< ... */
        int displayImgWidth;        /**< ... */
        int displayImgHeight;       /**< ... */

		// Dimenco display constants to convert depth into disparity
		// using the following equation: 
		// D(Z) = M * (1-(vz/(Z-Zd+vz))) + C
		// D: disparity [0,255]
		// Z: depth [0,1]
        double Zd;                  /**< depth of display plane */
        double vz;                  /**< view distance in coordinate units */
        double M;                   /**< linear function multiplier */
        double C;                   /**< linear function constant */
	
		// rgb image infos
        int originalRGBImgWidth;    /**< ... */
        int originalRGBImgHeight;   /**< ... */
        cv::Mat m_rgbImg;           /**< ... */
	
		// depth image infos
        int originalDepthImgWidth;  /**< ... */
        int originalDepthImgHeight; /**< ... */
        cv::Mat m_depthImg;         /**< ... */
		
        /**
         * @brief init
         * @param msize
         * @return
         */
		int init(int msize);

        /**
         * @brief refresh
         * @param rgbImg
         * @param depthImg
         */
		void refresh(const cv::Mat& rgbImg, const cv::Mat& depthImg);

        /**
         * @brief depth2disparity
         * @param depthImg
         * @param disparityImg
         */
		void depth2disparity(const cv::Mat& depthImg, cv::Mat& disparityImg);

        /**
         * @brief padImageBorder
         * @param inputImg
         * @param outputImg
         */
		void padImageBorder( const cv::Mat& inputImg, cv::Mat& outputImg );

        /**
         * @brief concatRGBDImages
         * @param pRgbImg
         * @param pDisparityImg
         * @param rgbdImg
         */
		void concatRGBDImages( const cv::Mat& pRgbImg, const cv::Mat& pDisparityImg, cv::Mat& rgbdImg ); 

        /**
         * @brief addZeroLines
         * @param inputImg
         * @param outputImg
         */
		void addZeroLines(const cv::Mat& inputImg, cv::Mat& outputImg);

        /**
         * @brief add3DHeader
         * @param img
         */
		void add3DHeader( cv::Mat& img );

        /**
         * @brief embedBlueBit7B
         * @param data
         * @param hd
         * @param len
         */
		void embedBlueBit7B(unsigned char *data, unsigned char *hd, int len);
};

#endif

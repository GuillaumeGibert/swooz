
/**
 * \file SWConvQtOpencv.h
 * \author LANCE Florian
 * \date 18/01/13
 * \brief ...
 */


#ifndef _SWCONVQTOPENCV_
#define _SWCONVQTOPENCV_

#include "commonTypes.h"
#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <QImage>

namespace swConv
{	
	/**
     * \brief Convert opencv mat3b to Qt QImage
     * \param [in] oMat : mat to be converted
     * \return  qimage result
	 */		
	static QImage mat2QImage(const cv::Mat3b &oMat)
	{
		QImage l_oQImage(oMat.cols, oMat.rows, QImage::Format_ARGB32);
						
		for (int jj = 0; jj < oMat.rows; ++jj) 
		{					
			const cv::Vec3b *l_oMatRow = oMat[jj];	
			
			QRgb *l_oQIRow = (QRgb*)l_oQImage.scanLine(jj);
			
			for (int ii = 0; ii < oMat.cols; ++ii) 
			{
				l_oQIRow[ii] = qRgba(l_oMatRow[ii][2], l_oMatRow[ii][1], l_oMatRow[ii][0], 255);
			}
		}

		return l_oQImage;
	}


    /**
     * \brief Convert opencv mat3f to Qt QImage
     * \param [in] oMat : mat to be converted
     * \return  qimage result
     */
//    static QImage mat3f2QImage(const cv::Mat3f &oMat)
//    {
//        QImage l_oQImage(oMat.cols, oMat.rows, QImage::Format_ARGB32);

////        float l_fMin = FLT_MAX;
////        float l_fMax = FLT_MIN;

//        for (int ii = 0; ii < oMat.rows; ++ii)
//        {
//            for(int jj = 0; jj < oMat.cols; ++jj)
//            {
//                uint l_ui32Val = oMat.at<float>(ii,jj)*oMat.at<float>(ii,jj);
//                QColor l_oColor;

//                l_oColor.setRgb(l_ui32Val,l_ui32Val,l_ui32Val);

//                l_oQImage.setColor(ii*oMat.cols + jj, l_oColor.rgb());

////QColor::setRgb


////                std::cout << l_ui32Pixel << " ";
////                if(oMat.at<float>(ii,jj) > 0.f)
////                {
////                    l_oQImage.setPixel(jj, ii, 0);
////                }
////                else if(oMat.at<float>(ii,jj) < -2.f)
////                {
////                    l_oQImage.setPixel(jj, ii, 16777215);
////                }
////                else
////                {
////                    uint l_ui32Val   = oMat.at<float>(ii,jj) * (-128);
////                    uint l_ui32Pixel = l_ui32Val +  (l_ui32Val << 8) + (l_ui32Val  << 16);
////                    l_oQImage.setPixel(jj, ii, l_ui32Pixel);
////                }
//            }
//        }

//        l_fMax += l_fMin;

//        for (int ii = 0; ii < oMat.rows; ++ii)
//        {
//            for(int jj = 0; jj < oMat.cols; ++jj)
//            {
//                uint l_ui32Val = (uint)(255 * oMat.at<float>(ii,jj) / l_fMax);
//                uint l_ui32Pixel = l_ui32Val +  (l_ui32Val << 8) + (l_ui32Val  << 16);
//                l_oQImage.setPixel(jj, ii, l_ui32Pixel);
//            }
//        }

//        return l_oQImage;
//    }


	/**
     * \brief Convert Qt QImage to opencv mat3b
     * \param [in]oMat : QImage to be converted
     * \return  mat3b result
	 */			
	static cv::Mat qImage2Mat(const QImage &oQImage)
	{
		cv::Mat l_oMat1(oQImage.height(), oQImage.width(), CV_8UC4, (uchar*)oQImage.bits(), oQImage.bytesPerLine());		
		cv::Mat l_oMat2 = cv::Mat(l_oMat1.rows, l_oMat1.cols, CV_8UC3 );

		int l_i32FromTo[] = { 0,0,  1,1,  2,2 };
		cv::mixChannels( &l_oMat1, 1, &l_oMat2, 1, l_i32FromTo, 3 );

		return l_oMat2; 						
	}
	
}


#endif

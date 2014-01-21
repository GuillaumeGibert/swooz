
/**
 * \file SWImageProcessing.h
 * \brief Contains utility functions for processing images.
 * \author Florian Lance
 * \date 29/01/13
 */

#ifndef _SWIMAGEPROCESSING_
#define _SWIMAGEPROCESSING_

#include "opencv2/imgproc/imgproc.hpp"
#include "cloud/SWCloud.h"

namespace swImage
{
	namespace swUtil
	{		
        /**
         * @brief return a rgb mat image with the background colored
         * @param [in] oRgb      : input RGB mat image
         * @param [in] oDepth    : input Depth mat image
         * @param [in] fDepthMin : minimum depth value before a point is considered as a member of the background
         * @param [in] i32DilatationBackground : dilation value used to fill holes
         * @param [in] v3bColor  : color used for background pixels
         * @return a new mat rgb image
         */
        static cv::Mat removeBackground(const cv::Mat &oRgb, const cv::Mat &oDepth, cfloat fDepthMin = 1.1, cint i32DilatationBackground = 3, const cv::Vec3b v3bColor = cv::Vec3b(122,122,122))
        {
            cv::Mat l_oFore = oRgb.clone();
            cv::Mat l_oMask(oRgb.rows, oRgb.cols, CV_32F, cv::Scalar(1.f));

            if(oDepth.depth() == CV_16U && oDepth.channels() == 1)
            {
                for(int ii = 0; ii < oDepth.rows * oDepth.cols ; ++ii)
                {
                    if(oDepth.at<unsigned short >(ii) > fDepthMin*1000 ||
                       oDepth.at<unsigned short >(ii) == 0)
                    {
                        l_oMask.at<float>(ii) = 0.f;
                    }
                }
            }
            else
            {
                for(int ii = 0; ii < oDepth.rows * oDepth.cols ; ++ii)
                {
                    if(oDepth.at<cv::Vec3f>(ii)[2] > fDepthMin ||
                       oDepth.at<cv::Vec3f>(ii)[2] == 0)
                    {
                        l_oMask.at<float>(ii) = 0.f;
                    }
                }
            }

            cv::dilate(l_oMask, l_oMask, cv::Mat(), cv::Point(-1,-1), i32DilatationBackground);

            for(int ii = 0; ii < l_oMask.rows * l_oMask.cols; ++ii)
            {
                if(l_oMask.at<float>(ii) < 1.f)
                {
                    l_oFore.at<cv::Vec3b>(ii) = v3bColor;
                }
            }


            return l_oFore;
        }


				
		/**
         * \brief  Scale an opencv rectangle by two value
         * \param  [in] oInputRect : input rectangle
         * \param  [in] ratioX     : x scale ratio value
         * \param  [in] ratioY     : y scale ratio value
         * \return the scaled rectangle
		 */		
		static cv::Rect scaleRect(const cv::Rect oInputRect, cfloat ratioX, cfloat ratioY)
		{
			cv::Rect l_oRectToScale = oInputRect;
			l_oRectToScale.x      = (int)(l_oRectToScale.x * ratioX);
			l_oRectToScale.width  = (int)(l_oRectToScale.width * ratioX);
			l_oRectToScale.y      = (int)(l_oRectToScale.y * ratioY);
			l_oRectToScale.height = (int)(l_oRectToScale.height * ratioY);
			
			return l_oRectToScale;
		}
		
		/**
		 * \brief  ...
		 * \param  [in]  oInputRect 	: ...
		 * \param  [in]  oInputCloudMat : ...
		 * \param  [out] bBox     	: ...
		 * \param  [in]  ui32VideoMode  : ...
		 * \return ...
		 */		
		static void computeSizeCloudRect(const cv::Rect &oInputRect, const cv::Mat &oInputCloudMat, swCloud::SWCloudBBox &bBox, cint ui32VideoMode)
		{
			// init sub rect
			cv::Rect l_oRect = oInputRect;
			
			if(ui32VideoMode == 1) // CV_CAP_OPENNI_SXGA_15HZ
			{
				// l_oRect.y += 22;
			}
						
			l_oRect.x += l_oRect.width/3;
			l_oRect.width -= 2*l_oRect.width/3;
			l_oRect.y += l_oRect.height/3;
			l_oRect.height -= 2*l_oRect.height/3;	

			// init the cloud of the face mat 
			cv::Mat l_oFaceCloudMat = oInputCloudMat.clone();
			l_oFaceCloudMat 	= l_oFaceCloudMat(l_oRect);
			
			// init cloud coordinates arrays
			std::vector<float> l_vFX, l_vFY, l_vFZ;

			for(int ii = 0; ii < l_oFaceCloudMat.rows; ++ii)
			{
				for(int jj = 0; jj < l_oFaceCloudMat.cols; ++jj)
				{
					if(l_oFaceCloudMat.at<cv::Vec3f>(ii,jj) != cv::Vec3f(0,0,0) )
					{
						l_vFX.push_back(l_oFaceCloudMat.at<cv::Vec3f>(ii,jj)[0]);
						l_vFY.push_back(l_oFaceCloudMat.at<cv::Vec3f>(ii,jj)[1]);							
						l_vFZ.push_back(l_oFaceCloudMat.at<cv::Vec3f>(ii,jj)[2]);
					}				
				}
			}
			
			swCloud::SWCloud     l_oCloudBorder(l_vFX, l_vFY, l_vFZ);
			swCloud::SWCloudBBox l_oBBoxRectCloud = l_oCloudBorder.bBox();
			
//			std::cout << "l_oBBoxRectCloud " << std::endl;
//			std::cout << " -> " <<3*(l_oBBoxRectCloud.m_fMaxX - l_oBBoxRectCloud.m_fMinX) << " " << 3*(l_oBBoxRectCloud.m_fMaxY - l_oBBoxRectCloud.m_fMinY ) << std::endl;
			
			float l_fDiffX = l_oBBoxRectCloud.m_fMaxX - l_oBBoxRectCloud.m_fMinX;
			float l_fDiffY = l_oBBoxRectCloud.m_fMaxY - l_oBBoxRectCloud.m_fMinY;
			
			bBox 	     = l_oBBoxRectCloud;
			bBox.m_fMinX-= l_fDiffX;
			bBox.m_fMaxX+= l_fDiffX;
			bBox.m_fMinY-= l_fDiffY;
			bBox.m_fMaxY+= l_fDiffY;

			swCloud::SWCloud l_oCloudBBox1, l_oCloudBBox2;
			l_oCloudBorder.cloudBBox(l_oCloudBBox1,1000);
			swCloud::SWCloud::cloudBBox(bBox ,l_oCloudBBox2,1000);
			
			l_oCloudBorder += l_oCloudBBox1;
			l_oCloudBorder += l_oCloudBBox2;
		}						
		


		/**
		 * \brief  Encode the input vec3f and the input index in a vec2i with some restrictions :
		 * 	   float data must be : x > -0.5f && x < 0.5f  && y > -0.5f && y < 0.5f && z > 0.5f && z < 1.5f 
		 * \param  [out] oVec2i   : result vec2i
		 * \param  [in]  oP3f     : input vec3f to be encoded
		 * \param  [in]  i32Index : index of the current vec3f
		 */			
		static void encodeFloatCloudCoord(cv::Vec2i &oVec2i, const cv::Point3f &oP3f, cint i32Index)
		{
			// float data must be : x > -0.5f && x < 0.5f  && y > -0.5f && y < 0.5f && z > 0.5f && z < 1.5f 			
			
			// (x,y,z) -> ((fx,fy,fz) + 0.5) * 10000
			// l_i32V1 : 0  x3  x2  x1  y14 y13 y12 y11 | y10 y9  y8  y7  y6  y5  y4  y3 | y2  y1  z14 z13 z12 z11 z10 z9  | z8  z7  z6  z5  z4  z3  z2  z1 | 
			// l_i32V2 : 0  i20 i19 i18 i17 i16 i15 i14 | i13 i12 i11 i10 i9  i8  i7  i6 | i5  i4  i3  i2  i1  x14 x13 x12 | x11 x10 x9  x8  x7  x6  x5  x4 | 			
			// (x,z,z) -> 14 bits : 0 - 16 383
			   // i    -> 20 bits : 0 - 1 048 575
			
			int l_i32V1 = 0, l_i32V2 = 0;			
			int l_i32X = (int) ((oP3f.x + 0.5f) * 10000); // x -> 0 - 15 000 
			int l_i32Y = (int) ((oP3f.y + 0.5f) * 10000); // y -> 0 - 15 000 
			int l_i32Z = (int) ((oP3f.z + 0.5f) * 10000); // z -> 0 - 15 000 
			int l_i32Index = i32Index;
			
			l_i32X = l_i32X & 16383;
			l_i32Y = l_i32Y & 16383;
			l_i32Z = l_i32Z & 16383;
			l_i32Index = l_i32Index & 1048575;
			
			l_i32V1  = l_i32Z;
			l_i32V1 += l_i32Y << 14;
			l_i32V1 += (l_i32X & 7) << 28; 
			
			l_i32V2 += (l_i32X >> 3) & 2047;
			l_i32V2 += l_i32Index << 11;
			
			oVec2i[0] = l_i32V1;
			oVec2i[1] = l_i32V2;				
		}
		
		/**
		 * \brief  Decode the input vec2i to get the initial vec3f and the index.
		 * 	   It's must be used with encodeFloatCloudCoord
		 * \param  [out] oP3f     : result decoded vec3f
		 * \param  [in]  i32Index : result decoded index
		 * \param  [in]  oVec2i   : input vec2i
		 */			
		static void decodeIntCloudCoord(cv::Point3f &oP3f, int &i32Index, const cv::Vec2i &oVec2i)
		{
			// (x,y,z) -> ((fx,fy,fz) + 0.5) * 10000
			// l_i32V1 : 0  x3  x2  x1  y14 y13 y12 y11 | y10 y9  y8  y7  y6  y5  y4  y3 | y2  y1  z14 z13 z12 z11 z10 z9  | z8  z7  z6  z5  z4  z3  z2  z1 | 
			// l_i32V2 : 0  i20 i19 i18 i17 i16 i15 i14 | i13 i12 i11 i10 i9  i8  i7  i6 | i5  i4  i3  i2  i1  x14 x13 x12 | x11 x10 x9  x8  x7  x6  x5  x4 | 			
			// (x,z,z) -> 14 bits : 0 - 16 383
			   // i    -> 20 bits : 0 - 1 048 575				

			int l_i32V1 = oVec2i[0];
			int l_i32V2 = oVec2i[1];
			
			float l_fZ = (float)(l_i32V1 & 16383);
			float l_fY = (float)((l_i32V1 >> 14) & 16383);
			float l_fX = (float)((l_i32V1 >> 28) & 7);
			l_fX 	  += (l_i32V2 & 2047) << 3;
			
			i32Index = (l_i32V2 >> 11) & 1048575;
			
			l_fZ = (l_fZ * 0.0001f) - 0.5f;
			l_fY = (l_fY * 0.0001f) - 0.5f;
			l_fX = (l_fX * 0.0001f) - 0.5f;
			
			oP3f.x = l_fX;
			oP3f.y = l_fY;
			oP3f.z = l_fZ;	
		}				

	
    }
}


		// static void encodeFloatCloudCoord(cv::Vec<uint16, 3> &oEncodedPoint, const cv::Point3f &oP3f)
		// {		
			// float data must be : x > -3.f && x < 3.f  && y > -3f && y < 3f && z > 0.5f && z < 6.5f 
			
			// x <- (fx + 3) * 10000 | y <- (fy + 3) * 10000 | z <- (fz - 0.5) * 10000 |  =====>  (x,y,z) >= 0 && (x,y,z) <= 60000
			// oEncodedPoint[0] : x16 x15 x14 x13 x12 x11 x10 x9 | x8 x7 x6 x5 x4 x3 x2 x1 | 
			// oEncodedPoint[1] : y16 y15 y14 y13 y12 y11 y10 y9 | y8 y7 y6 y5 y4 y3 y2 y1 |
			// oEncodedPoint[2] : z16 z15 z14 z13 z12 z11 z10 z9 | z8 z7 z6 z5 z4 z3 z2 z1 |
			// (x,z,z) -> 16 bits : 0 - 65 535

			// oEncodedPoint[0] = 0;
			// oEncodedPoint[1] = 0;
			// oEncodedPoint[2] = 0;
			
			// int l_i32X = (int) ((oP3f.x + 3.f ) * 10000); // x -> 0 - 60 000
			// int l_i32Y = (int) ((oP3f.y + 3.f ) * 10000); // y -> 0 - 60 000
			// int l_i32Z = (int) ((oP3f.z - 0.5f) * 10000); // z -> 0 - 60 000			
			
			// oEncodedPoint[0] += l_i32X & 65535;
			// oEncodedPoint[1] += l_i32Y & 65535;
			// oEncodedPoint[2] += l_i32Z & 65535;			
		// }
		
		// static void decodeFloatCloudCoord(cv::Point3f &oP3f, cv::Vec< uint16, 3>  &oEncodedPoint)
		// {
			// x <- (fx + 3) * 10000 | y <- (fy + 3) * 10000 | z <- (fz - 0.5) * 10000 |  =====>  (x,y,z) >= 0 && (x,y,z) <= 60000
			// oEncodedPoint[0] : x16 x15 x14 x13 x12 x11 x10 x9 | x8 x7 x6 x5 x4 x3 x2 x1 | 
			// oEncodedPoint[1] : y16 y15 y14 y13 y12 y11 y10 y9 | y8 y7 y6 y5 y4 y3 y2 y1 |
			// oEncodedPoint[2] : z16 z15 z14 z13 z12 z11 z10 z9 | z8 z7 z6 z5 z4 z3 z2 z1 |
			// (x,z,z) -> 16 bits : 0 - 65 535

			// oP3f.x = (oEncodedPoint[0]*0.0001f) - 3.f;
			// oP3f.y = (oEncodedPoint[1]*0.0001f) - 3.f;
			// oP3f.z = (oEncodedPoint[2]*0.0001f) + 0.5f;									
		// }	


		// static void encodeFloatCloudCoord(float &fEncodedPoint, const cv::Point3f &oP3f)
		// {
			
			// float l_fX = (float)((int)(oP3f.x * 10000));
			// float l_fY = (float)((int) (oP3f.y * 10000) * 10000);
			// float l_fZ = (float)(((int) (oP3f.z * 10000)) * 10000 * 10000);
			
			// std::cout << "l_fX " << l_fX << " l_fY " << l_fY << " l_fZ " << l_fZ << std::endl;
			// fEncodedPoint = l_fX + l_fY + l_fZ;			
		// }
		
		// static void decodeFloatCloudCoord(cv::Point3f &oP3f, const float fEncodedPoint)
		// {
			// float l_fEncodedPoint;
			// std::cout << "fEncodedPoint " << fEncodedPoint << std::endl;
			
			// l_fEncodedPoint = fEncodedPoint / 10000;
			// oP3f.x = modf(fEncodedPoint, &l_fEncodedPoint);
			
			// l_fEncodedPoint = l_fEncodedPoint / 10000;
			// oP3f.y = modf(l_fEncodedPoint, &oP3f.z);
			
			// oP3f.z *= 0.0001f;
			
			// std::cout << "oP3f.x " << oP3f.x << " oP3f.y " << oP3f.y << " oP3f.z " << oP3f.z << std::endl << std::endl;
			
			// float l_fEncodedPoint 
			
			// float l_fEncodedPoint = fEncodedPoint / 10000;
			// std::cout << "l_fEncodedPoint " << l_fEncodedPoint << std::endl;
			
			// oP3f.x = modf((float)l_fEncodedPoint, &l_fEncodedPoint);
			// std::cout << "l_fEncodedPoint " << l_fEncodedPoint << " oP3f.x " << oP3f.x  << std::endl << std::endl;
			
			// double param, fractpart, intpart;

		  // param = 3.14159265;
		  // fractpart = modf (param , &intpart);
		  // printf ("%lf = %lf + %lf \n", param, intpart, fractpart);
					
		// }


		// static void decodeFloatCloudCoord(cv::Point3f &oP3f, const SEncodedCloudPoint &oEncodedPoint)
		// {
			// x <- (fx + 3) * 10000 | y <- (fy + 3) * 10000 | z <- (fz - 0.5) * 10000 |  =====>  (x,y,z) >= 0 && (x,y,z) <= 60000
			// m_ui16X : x16 x15 x14 x13 x12 x11 x10 x9 | x8 x7 x6 x5 x4 x3 x2 x1 | 
			// m_ui16Y : y16 y15 y14 y13 y12 y11 y10 y9 | y8 y7 y6 y5 y4 y3 y2 y1 |
			// m_ui16Z : z16 z15 z14 z13 z12 z11 z10 z9 | z8 z7 z6 z5 z4 z3 z2 z1 |
			// (x,z,z) -> 16 bits : 0 - 65 535

			// oP3f.x = (oEncodedPoint.m_ui16X*0.0001f) - 3.f;
			// oP3f.y = (oEncodedPoint.m_ui16Y*0.0001f) - 3.f;
			// oP3f.z = (oEncodedPoint.m_ui16Z*0.0001f) + 0.5f;									
		// }	

		// static void encodeFloatCloudCoord(SEncodedCloudPoint &oEncodedPoint, const cv::Point3f &oP3f)
		// {		
			// float data must be : x > -3.f && x < 3.f  && y > -3f && y < 3f && z > 0.5f && z < 6.5f 
			
			// x <- (fx + 3) * 10000 | y <- (fy + 3) * 10000 | z <- (fz - 0.5) * 10000 |  =====>  (x,y,z) >= 0 && (x,y,z) <= 60000
			// m_ui16X : x16 x15 x14 x13 x12 x11 x10 x9 | x8 x7 x6 x5 x4 x3 x2 x1 | 
			// m_ui16Y : y16 y15 y14 y13 y12 y11 y10 y9 | y8 y7 y6 y5 y4 y3 y2 y1 |
			// m_ui16Z : z16 z15 z14 z13 z12 z11 z10 z9 | z8 z7 z6 z5 z4 z3 z2 z1 |
			// (x,z,z) -> 16 bits : 0 - 65 535

			// oEncodedPoint.m_ui16X = 0;
			// oEncodedPoint.m_ui16Y = 0;
			// oEncodedPoint.m_ui16Z = 0;
			
			// int l_i32X = (int) ((oP3f.x + 3.f ) * 10000); // x -> 0 - 60 000
			// int l_i32Y = (int) ((oP3f.y + 3.f ) * 10000); // y -> 0 - 60 000
			// int l_i32Z = (int) ((oP3f.z - 0.5f) * 10000); // z -> 0 - 60 000			
			
			// oEncodedPoint.m_ui16X += l_i32X & 65535;
			// oEncodedPoint.m_ui16Y += l_i32Y & 65535;
			// oEncodedPoint.m_ui16Z += l_i32Z & 65535;			
		// }
		// struct SEncodedCloudPoint
		// {
			// uint16 m_ui16X; // 0 - 65 535
			// uint16 m_ui16Y;
			// uint16 m_ui16Z;			
		// };

		// /**
		 // * \brief  Encode the input vec3f and the input index in a vec2i with some restrictions :
		 // * 	   float data must be : x > -0.5f && x < 0.5f  && y > -0.5f && y < 0.5f && z > 0.5f && z < 1.5f 
		 // * \param  [out] oVec2i   : result vec2i
		 // * \param  [in]  oP3f     : input vec3f to be encoded
		 // * \param  [in]  i32Index : index of the current vec3f
		 // */			
		// static void encodeFloatCloudCoord(cv::Vec2i &oVec2i, const cv::Point3f &oP3f, cint i32Index)
		// {
			// float data must be : x > -0.5f && x < 0.5f  && y > -0.5f && y < 0.5f && z > 0.5f && z < 1.5f 			
			
			// first method : 
			// (x,y,z) -> (fx,fy,fz) * 1000 + 2000  // mult 1000 for mm conversion, add 2000 to cancel the minus
			// l_i32V1 : 0  x5  x4  x3  x2  x1  y13 y12 | y11 y10 y9  y8  y7  y6  y5  y4  | y3  y2  y1  z13 z12 z11 z10 z9  | z8  z7  z6  z5  z4  z3  z2  z1 | 
			// l_i32V2 : 0  i23 i22 i21 i20 i19 i18 i17 | i16 i15 i14 i13 i12 i11 i10 i9  | i8  i7  i6  i5  i4  i3  i2  i1  | x13 x12 x11 x10 x9  x8  x7  x6 | 
			// (x,z,z) -> 13 bits : 0 - 8 191			
			   // i    -> 23 bits : 0 - 8 388 607
					
			// int l_i32V1 = 0, l_i32V2 = 0;			
			// int l_i32X = (int) (oP3f.x * 1000 + 2000);
			// int l_i32Y = (int) (oP3f.y * 1000 + 2000);
			// int l_i32Z = (int) (oP3f.z * 1000 + 2000);
			// int l_i32Index = i32Index;
			
			// l_i32X = l_i32X & 8191;
			// l_i32Y = l_i32Y & 8191;
			// l_i32Z = l_i32Z & 8191;
			// l_i32Index = l_i32Index & 8388607;
			
			// l_i32V1  = l_i32Z;
			// l_i32V1 += l_i32Y << 13;
			// l_i32V1 += (l_i32X & 31) << 26; 
			
			// l_i32V2 += (l_i32X >> 5) & 255;
			// l_i32V2 += l_i32Index << 8;
			
			// oVec2i[0] = l_i32V1;
			// oVec2i[1] = l_i32V2;	


			// second method : 
			// (x,y,z) -> ((fx,fy,fz) + 0.5) * 10000
			// l_i32V1 : 0  x3  x2  x1  y14 y13 y12 y11 | y10 y9  y8  y7  y6  y5  y4  y3 | y2  y1  z14 z13 z12 z11 z10 z9  | z8  z7  z6  z5  z4  z3  z2  z1 | 
			// l_i32V2 : 0  i20 i19 i18 i17 i16 i15 i14 | i13 i12 i11 i10 i9  i8  i7  i6 | i5  i4  i3  i2  i1  x14 x13 x12 | x11 x10 x9  x8  x7  x6  x5  x4 | 			
			// (x,z,z) -> 14 bits : 0 - 16 383
			//    i    -> 20 bits : 0 - 1 048 575
			
			// int l_i32V1 = 0, l_i32V2 = 0;			
			// int l_i32X = (int) ((oP3f.x + 0.5f) * 10000); // x -> 0 - 15 000 
			// int l_i32Y = (int) ((oP3f.y + 0.5f) * 10000); // y -> 0 - 15 000 
			// int l_i32Z = (int) ((oP3f.z + 0.5f) * 10000); // z -> 0 - 15 000 
			// int l_i32Index = i32Index;
			
			// l_i32X = l_i32X & 16383;
			// l_i32Y = l_i32Y & 16383;
			// l_i32Z = l_i32Z & 16383;
			// l_i32Index = l_i32Index & 1048575;
			
			// l_i32V1  = l_i32Z;
			// l_i32V1 += l_i32Y << 14;
			// l_i32V1 += (l_i32X & 7) << 28; 
			
			// l_i32V2 += (l_i32X >> 3) & 2047;
			// l_i32V2 += l_i32Index << 11;
			
			// oVec2i[0] = l_i32V1;
			// oVec2i[1] = l_i32V2;				
		// }
		
		// /**
		 // * \brief  Decode the input vec2i to get the initial vec3f and the index.
		 // * 	   It's must be used with encodeFloatCloudCoord
		 // * \param  [out] oP3f     : result decoded vec3f
		 // * \param  [in]  i32Index : result decoded index
		 // * \param  [in]  oVec2i   : input vec2i
		 // */			
		// static void decodeIntCloudCoord(cv::Point3f &oP3f, int &i32Index, const cv::Vec2i &oVec2i)
		// {
			// first method : 
			// (x,y,z) -> (fx,fy,fz) * 1000 + 2000  // mult 1000 for mm conversion, add 2000 to cancel the minus
			// l_i32V1 : 0  x5  x4  x3  x2  x1  y13 y12 | y11 y10 y9  y8  y7  y6  y5  y4  | y3  y2  y1  z13 z12 z11 z10 z9  | z8  z7  z6  z5  z4  z3  z2  z1 | 
			// l_i32V2 : 0  i23 i22 i21 i20 i19 i18 i17 | i16 i15 i14 i13 i12 i11 i10 i9  | i8  i7  i6  i5  i4  i3  i2  i1  | x13 x12 x11 x10 x9  x8  x7  x6 | 
			// (x,z,z) -> 13 bits : 0 - 8 191			
			//    i    -> 23 bits : 0 - 8 388 607		
			
			// int l_i32V1 = oVec2i[0];
			// int l_i32V2 = oVec2i[1];
			
			// float l_fZ = l_i32V1 & 8191;
			// float l_fY = (l_i32V1 >> 13) & 8191;
			// float l_fX = (l_i32V1 >> 26) & 31;
			// l_fX 	  += (l_i32V2 & 255) << 5;
			
			// i32Index = (l_i32V2 >> 8) & 8388607;
			
			// l_fZ = (l_fZ - 2000)* 0.001f;
			// l_fY = (l_fY - 2000)* 0.001f;
			// l_fX = (l_fX - 2000)* 0.001f;
			
			// oP3f.x = l_fX;
			// oP3f.y = l_fY;
			// oP3f.z = l_fZ;			
					
					
			// second method : 
			// (x,y,z) -> ((fx,fy,fz) + 0.5) * 10000
			// l_i32V1 : 0  x3  x2  x1  y14 y13 y12 y11 | y10 y9  y8  y7  y6  y5  y4  y3 | y2  y1  z14 z13 z12 z11 z10 z9  | z8  z7  z6  z5  z4  z3  z2  z1 | 
			// l_i32V2 : 0  i20 i19 i18 i17 i16 i15 i14 | i13 i12 i11 i10 i9  i8  i7  i6 | i5  i4  i3  i2  i1  x14 x13 x12 | x11 x10 x9  x8  x7  x6  x5  x4 | 			
			// (x,z,z) -> 14 bits : 0 - 16 383
			//    i    -> 20 bits : 0 - 1 048 575				

			// int l_i32V1 = oVec2i[0];
			// int l_i32V2 = oVec2i[1];
			
			// float l_fZ = (float)(l_i32V1 & 16383);
			// float l_fY = (float)((l_i32V1 >> 14) & 16383);
			// float l_fX = (float)((l_i32V1 >> 28) & 7);
			// l_fX 	  += (l_i32V2 & 2047) << 3;
			
			// i32Index = (l_i32V2 >> 11) & 1048575;
			
			// l_fZ = (l_fZ * 0.0001f) - 0.5f;
			// l_fY = (l_fY * 0.0001f) - 0.5f;
			// l_fX = (l_fX * 0.0001f) - 0.5f;
			
			// oP3f.x = l_fX;
			// oP3f.y = l_fY;
			// oP3f.z = l_fZ;	
		// }
		
		
#endif 

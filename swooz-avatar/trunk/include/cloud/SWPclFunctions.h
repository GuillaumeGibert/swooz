
/**
 * \file SWPclFunctions.h
 * \brief Contains utility functions for pcl
 * \author Florian Lance
 * \date 16/05/13
 */

#ifndef _SWPCLFUNCTIONS_
#define _SWPCLFUNCTIONS_

#include "cloud/SWMaskCloud.h"
#include "opencv2/imgproc/imgproc.hpp"

#undef max
#undef min
#include <pcl/point_cloud.h>

namespace swCloud
{
	namespace swUtil
	{
		/**
		 * \brief Convert a SWCloud to a Pcl PointCloud<PointXYZRGB>
		 * \param [in] oCloudPoint   : input SW cloud point
		 * \param [in,out] oCloudPcl : output pcl cloud point
		 * \param [in] fMinDist      : minimum depth of the points to keep
		 * \param [in] fDepth        : fMinDist + fDepth will be the maximum depth of the points to keep
		 */	
		static bool convSWCloud2PclCloud(const SWCloud &oCloudPoint, pcl::PointCloud<pcl::PointXYZRGB> &oCloudPcl,
						 cfloat fMinDist = 0.f, cfloat fDepth = 10.f)
		{
			if(fDepth < 0.f)
			{
				std::cerr << "Error convSWCloud2PclCloud : bad dist parameters. " << std::endl;
				return false;
			}	
			
			pcl::PointXYZRGB l_oPointToAdd;
			pcl::PointCloud<pcl::PointXYZRGB> l_oCloudPoint;
			
			float *l_fX = oCloudPoint.coord(0);
			float *l_fY = oCloudPoint.coord(1);
			float *l_fZ = oCloudPoint.coord(2);
			
			uint8 *l_ui8R = oCloudPoint.color(0);
			uint8 *l_ui8G = oCloudPoint.color(1);
			uint8 *l_ui8B = oCloudPoint.color(2);
			
			uint32 l_ui32RGB;
			
			for(uint ii = 0; ii < oCloudPoint.size(); ++ii)
			{
				l_oPointToAdd.x = l_fX[ii];
				l_oPointToAdd.y = -l_fY[ii];
				l_oPointToAdd.z = l_fZ[ii];
				
				l_ui32RGB = ((uint32)l_ui8R[ii] << 16 | (uint32)l_ui8G[ii] << 8 | (uint32)l_ui8B[ii]);
				l_oPointToAdd.rgb = *reinterpret_cast<float*>(&l_ui32RGB);

				if(l_oPointToAdd.z > fMinDist && l_oPointToAdd.z < fDepth + fMinDist )
				{	
					l_oCloudPoint.push_back(l_oPointToAdd);	
				}
			}
			
			oCloudPcl = l_oCloudPoint;
			
			return true;
		}	

		/**
		 * \brief Convert a pcl cloud to a SWCloud
		 * \param [in] oCloudPcl       : input pcl cloud point
		 * \param [in,out] oCloudPoint : output SWCloud
		 */			
		static bool convPclCloud2SWCloud(const pcl::PointCloud<pcl::PointXYZRGB> &oCloudPcl, SWCloud &oCloudPoint)
		{
			uint l_ui32NumberOfPoints = oCloudPcl.size();
			uint32 l_ui32Rgb;
			float l_value;

			float *l_aFCoords   = new float[l_ui32NumberOfPoints * 3];
			uint8 *l_aUi8Colors = new uint8[l_ui32NumberOfPoints * 3];
			
			float *l_fX = &(*l_aFCoords);
			float *l_fY = &l_fX[l_ui32NumberOfPoints];
			float *l_fZ = &l_fY[l_ui32NumberOfPoints];
			
			uint8 *l_ui8R = &(*l_aUi8Colors);
			uint8 *l_ui8G = &l_ui8R[l_ui32NumberOfPoints];
			uint8 *l_ui8B = &l_ui8G[l_ui32NumberOfPoints];
			
			for(uint ii = 0; ii < l_ui32NumberOfPoints; ++ii)
			{
				l_fX[ii] =  oCloudPcl[ii].x;
				l_fY[ii] = -oCloudPcl[ii].y;
				l_fZ[ii] =  oCloudPcl[ii].z;
				
				l_value = oCloudPcl[ii].rgb;
				l_ui32Rgb =  *reinterpret_cast<uint32*>(&l_value);
				
				l_ui8R[ii] = l_ui32Rgb >> 16;
				l_ui8G[ii] = (l_ui32Rgb >> 8) & 255;
				l_ui8B[ii] = l_ui32Rgb & 255;
			}		
			
			oCloudPoint.set(l_ui32NumberOfPoints, l_aFCoords, l_aUi8Colors);
			
			return true;
		}			
    }
}


#endif

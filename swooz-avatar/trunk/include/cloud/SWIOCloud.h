
/**
 * \file SWIOCloud.h
 * \brief Contains utility for loading and saving clouds.
 * \author Florian Lance
 * \date 29/01/13
 */

#ifndef _SWIOCLOUD_
#define _SWIOCLOUD_

#include "opencv2/imgproc/imgproc.hpp"
#include "cloud/SWCloud.h"

// #undef max
// #undef min
// #include <pcl/point_cloud.h>
// #include <pcl/io/pcd_io.h>


namespace swCloud
{
	namespace swUtil
	{
		static void writeEncodedCloudMappedFiles(const cv::Mat &oMat, int &i32NumPoint, int* i32DataPoints, int &i32NumIndex, int* i32DataIndex)
		{
			int l_i32Size = oMat.rows*oMat.cols;
			
			for(int ii = 0; ii < 2*l_i32Size; ii+=2)
			{
				i32DataPoints[2*i32NumPoint + ii]    = oMat.at<cv::Vec2i>(ii)[0];
				i32DataPoints[2*i32NumPoint + ii + 1]= oMat.at<cv::Vec2i>(ii)[1];				
			}
			
			i32DataIndex[i32NumIndex] = l_i32Size;
			
			i32NumPoint += l_i32Size;
			++i32NumIndex;
		}
			
		static void loadEncodedCloudMappedFiles(cv::Mat &oMat, int &i32NumPoint, int* i32DataPoints, int &i32NumIndex, int* i32DataIndex)
		{
			int l_i32Size = i32DataIndex[i32NumIndex];
			// std::cout << i32NumIndex << " size -> " << l_i32Size << std::endl;
			
			std::vector<cv::Vec2i> l_vV2iCurrentCloudPoint;	
			
			for(int ii = 0; ii < l_i32Size; ++ii)
			{
				cv::Vec2i l_oVec(i32DataPoints[i32NumPoint + ii], i32DataPoints[2 * i32NumPoint + ii]);
				l_vV2iCurrentCloudPoint.push_back(l_oVec);
				
			}			
			
			i32NumPoint += l_i32Size;
			++i32NumIndex;			
			
			oMat = cv::Mat(l_vV2iCurrentCloudPoint);
			// std::cout << i32NumIndex << " " << i32NumPoint << std::endl;
		}
		
			
		// static bool writeEncodedCloudMatInFile(const cv::Mat &oMat, cuint i32Index, ofstream &oFile)
		// {			
			// oFile << "cloud_" << i32Index << endl;
			
			// for(uint ii = 0; ii < oMat.rows*oMat.cols; ++ii)
			// {
				// oFile << oMat.at<cv::Vec2i>(ii)[0]  << " " << oMat.at<cv::Vec2i>(ii)[1] << " | ";
				// oFile
			// }			
			
			// return true;
		// }
		
		
		
		/**
		 * \brief Save a SWCloud to PCD file
		 * \param  [in] oCloudPointToSave  : input SWCloud to save
		 * \param  [in,out] sPath 	  : path string of the saved cloud
		 * \return true if the save is successful, else return false
		 */	
		/*static bool saveSWCloudToPCDFile(const SWCloud &oCloudPointToSave, const std::string &sPath)
		{
			pcl::PointCloud<pcl::PointXYZRGB> l_oCloudPcl;
			
			if(convSWCloud2PclCloud(oCloudPointToSave, l_oCloudPcl))
			{
				std::cout << "save cloud to path " << sPath << std::endl;
				int l_i32Binary = 0;
				return pcl::io::savePCDFile(sPath, l_oCloudPcl, l_i32Binary);
			}
			else
			{
				std::cerr << "Error saveSWCloudToPCD : pcl conversion impossible. " << std::endl;
				return false;
			}
		}*/
		
		
		/**
		 * \brief Load a PCD file to init a SWCloud
		 * \param  [in,out] oCloudPoint : SWCloud to init
		 * \param  [in] sPath           : path string of the PCD cloud to load
		 * \return true if the loading of the cloud is successful, else return false
		 */		
		/*static bool loadPCDFileToSWCloud(SWCloud &oCloudPoint, const std::string &sPath)
		{
			pcl::PointCloud<pcl::PointXYZRGB> l_oCloudPcl;
			
			if(pcl::io::loadPCDFile(sPath, l_oCloudPcl) != -1)
			{
				return convPclCloud2SWCloud(l_oCloudPcl, oCloudPoint);
			}
			else
			{
				std::cerr << "Error loadPCDFileToSWCloud : fail loading PCD cloud, path : " << sPath << std::endl;
				return false;			
			}
		}*/
		
		/**
		 * \brief Load an OBJ file to init a SWCloud
		 * \param  [in,out] oCloudPoint : SWCloud to init
		 * \param  [in] sPath           : path string of the OBJ file to load
		 * \return true if the loading of the cloud is successful, else return false
		 */			
		/*static bool loadOBJFileToSWCloud(SWCloud &oCloudPoint, const std::string &sPath)
		{
			std::ifstream l_oFlow(sPath);
			std::vector<float> l_vFX, l_vFY, l_vFZ;

			if(l_oFlow)
			{
				bool l_bEnd = false;
				std::string l_sWord;
				float l_fPX, l_fPY, l_fPZ;				
				
				char l_cCurrenChar;
				l_oFlow.get(l_cCurrenChar);
	
				while(l_cCurrenChar == '#') // skip comments
				{					
					getline(l_oFlow, l_sWord);
					l_oFlow.get(l_cCurrenChar);
				}
				
				while(!l_bEnd) // add points
				{
					if(l_cCurrenChar == 'v')
					{
						l_sWord = l_cCurrenChar;
						l_cCurrenChar = ' ';
					}
					else
					{
						l_oFlow >> l_sWord;
					}
					
					if(l_sWord != "v")
					{
						l_bEnd = true;
					}
					else
					{					
						l_oFlow >> l_fPX;
						l_oFlow >> l_fPY;
						l_oFlow >> l_fPZ;
						
						l_vFX.push_back(l_fPX/1000);
						l_vFY.push_back(l_fPY/1000);
						l_vFZ.push_back(-l_fPZ/1000);
					}
				}
				
				SWCloud l_oCloudPoint(l_vFX, l_vFY, l_vFZ);
				oCloudPoint.copy(l_oCloudPoint);
				
				return true;
			}
			else
			{
				std::cerr << "Error loadOBJFileToSWCloud : fail loading obj, path : " << sPath << std::endl;
				return false;
			}
		}*/	

    }
}

#endif

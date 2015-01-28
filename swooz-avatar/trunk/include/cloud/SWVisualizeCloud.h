
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
 * \file SWVisualizeCloud.h
 * \brief defines SWVisualizeCloud 
 * \author Florian Lance
 * \date 07/12/12
 */

#ifndef _SWVISUALIZECLOUD_
#define _SWVISUALIZECLOUD_

#undef max
#undef min
#include "pcl/visualization/cloud_viewer.h"
#include "cloud/SWCloud.h"


namespace swCloud
{
	typedef pcl::PointCloud<pcl::PointXYZRGB>::Ptr  CloudPointPtr;			 /**< boost shared pointer for CloudPoint */
	typedef pcl::PointCloud<pcl::PointXYZRGB>  CloudPoint;				 /**< pcl cloud point redefinition */
	
	typedef boost::shared_ptr<pcl::visualization::CloudViewer> CloudViewerPtr;	 /**< boost shared pointer for CloudViewer */	
	
	/**
	 * \class SWVisualizeCloud
	 * \brief Visualize clouds points with pcl
	 * \author Florian Lance
	 * \date 07/12/12	 
	 */
	class SWVisualizeCloud
	{
	
		public:
			
			/**
			 * \brief constructor of SWVisualizeCloud, display the input cloud in the viewer
			 * \param [in] oInputCloudPoint : input cloud to display
			 * \param [in] bLoop : if true,  display in a loop
			 *		       if false, the display is destroyed at the same time than the class
			 */
			SWVisualizeCloud(const CloudPoint &oInputCloudPoint, cbool bLoop = true);
		
			/**
			 * \brief constructor of SWVisualizeCloud, display the two input cloud in the viewer
			 * \param [in] oInputCloudPoint1 :  input cloud 1 to display
			 * \param [in] oInputCloudPoint2 :  input cloud 2 to display
			 * \param [in] bLoop : if true,  display in a loop
			 *		       if false, the display is destroyed at the same time than the class
			 */		
			SWVisualizeCloud(const CloudPoint &oInputCloudPoint1, const CloudPoint &oInputCloudPoint2, cbool bLoop = true);
		
			/**
			 * \brief constructor of SWVisualizeCloud, display the input cloud in the viewer
			 * \param [in] oCloudPoint : input cloud 1 to display
			 * \param [in] bLoop : if true,  display in a loop
			 *		       if false, the display is destroyed at the same time than the class
			 */		
			SWVisualizeCloud(const swCloud::SWCloud &oCloudPoint, cbool bLoop = true);
			
			/**
			 * \brief constructor of SWVisualizeCloud, display the two input cloud in the viewer
			 * \param [in] oCloudPoint1 : input cloud 1 to display
			 * \param [in] oCloudPoint2 : input cloud 2 to display
			 * \param [in] bLoop : if true,  display in a loop
			 *		       if false, the display is destroyed at the same time than the class
			 */					
			SWVisualizeCloud(const SWCloud &oCloudPoint1, const SWCloud &oCloudPoint2, cbool bLoop = true);

			/**
			 * \brief destructor of SWVisualizeCloud
			 */		
			~SWVisualizeCloud(void);
			
		private:
		
			/**
			 * \brief Init the viewer
			 * \param [in] bLoop : if true,  display in a loop
			 *		       if false, the display is destroyed at the same time than the class
			 */				
			void initViewer(cbool bLoop);
		
		private:

			CloudPointPtr m_oCloudPointPtr;   /**< cloud point pointer */	
			CloudViewerPtr m_oCloudViewerPtr; /**< cloud viewer pointer */
	};
}

#endif

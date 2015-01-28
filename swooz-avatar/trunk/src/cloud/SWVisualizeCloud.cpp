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
 * \file SWVisualizeCloud.cpp
 * \author LANCE Florian
 * \date 07/12/12
 * \brief defines SWVisualizeCloud
 */

#include "cloud/SWVisualizeCloud.h"
#include "cloud/SWPclFunctions.h"


using namespace std;
using namespace swCloud;
using namespace swUtil;

SWVisualizeCloud::SWVisualizeCloud(const CloudPoint &oInputCloudPoint, cbool bLoop)
{
	// init point cloud
	m_oCloudPointPtr =  CloudPointPtr(new CloudPoint(oInputCloudPoint));
	
	initViewer(bLoop);
}

SWVisualizeCloud::SWVisualizeCloud(const CloudPoint &oInputCloudPoint1, const CloudPoint &oInputCloudPoint2,  cbool bLoop)
{
	// init point cloud
	CloudPoint l_oTempCloud = oInputCloudPoint2;
	l_oTempCloud += oInputCloudPoint1;
	
	m_oCloudPointPtr =  CloudPointPtr(new CloudPoint(l_oTempCloud));
	
	initViewer(bLoop);
}


SWVisualizeCloud::SWVisualizeCloud(const SWCloud &oCloudPoint, cbool bLoop)
{
	// conv cloud
	CloudPoint l_oCloudPcl;
    convSWCloud2PclCloud(oCloudPoint, l_oCloudPcl);
	
	m_oCloudPointPtr =  CloudPointPtr(new CloudPoint(l_oCloudPcl));
	
	initViewer(bLoop);
}

SWVisualizeCloud::SWVisualizeCloud(const SWCloud &oCloudPoint1, const SWCloud &oCloudPoint2, cbool bLoop)
{
	SWCloud l_oCloudPoint;
	l_oCloudPoint.copy(oCloudPoint1);
	l_oCloudPoint += oCloudPoint2;
	
	// conv cloud
	CloudPoint l_oCloudPcl;
    convSWCloud2PclCloud(l_oCloudPoint, l_oCloudPcl);
	
	m_oCloudPointPtr =  CloudPointPtr(new CloudPoint(l_oCloudPcl));
	
	initViewer(bLoop);
}


SWVisualizeCloud::~SWVisualizeCloud(void)
{

}


void SWVisualizeCloud::initViewer(cbool bLoop)
{
	m_oCloudViewerPtr = CloudViewerPtr(new pcl::visualization::CloudViewer("Cloud viewer"));
	m_oCloudViewerPtr->showCloud(m_oCloudPointPtr);
	
	if(bLoop)
	{
		// cloudPoint visualization loop
		while (!m_oCloudViewerPtr->wasStopped ())
		{
			
		}
	}		
}

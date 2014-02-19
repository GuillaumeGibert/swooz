
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


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
 * \file SWTrack.cpp
 * \brief defines SWTrack 
 * \author Florian Lance
 * \date 05/12/12
 */

#include "track/SWTrack.h"

#include "opencvUtility.h"

using namespace cv;
using namespace std;
using namespace swTrack;
using namespace swExcept;

SWTrack::SWTrack()
{
    m_CFaceDetectionPtr = SWFaceDetectionPtr(new swDetect::SWFaceDetection(cv::Size(80,80)));
	
	m_CTrackFlowPtr = SWTrackFlowPtr(new SWTrackFlow());
	
	m_ui32NumFrame = 0;
	m_ui32NumberOfFramesBeforeFaceDetection	= 15;
}


bool SWTrack::trackFace(const Mat &oPreRgbFrame, const Mat &oCurrRgbFrame, Rect &oFaceRect)
{	
	m_bTracked = false;
	
	if(oPreRgbFrame.rows * oPreRgbFrame.cols != oPreRgbFrame.rows * oPreRgbFrame.cols)
	{
		cerr << "Previous frame and current frame mush have the same size, the tracking can't be done. " << endl;
		return false;
	}
	
	try
	{	
		// retrieve the mat contening the face detected with haar cascade
		// if(m_ui32NumFrame % m_ui32NumberOfFramesBeforeFaceDetection == 0 && m_ui32NumFrame != 0)
		// {
            if(m_CFaceDetectionPtr->detectFace(oCurrRgbFrame)) // if face detection is successfull
			{
				m_oFaceRect = m_CFaceDetectionPtr->faceRect();

				oFaceRect = m_oFaceRect;
				
				m_ui32NumFrame++;
				
				return true;
			}
			else
			{
				
				oFaceRect = m_oFaceRect;
				
				m_ui32NumFrame++;
				
				return false;
			}
		// }
		// else
		// {
			// m_oFaceRect = oFaceRect;
		// }	

		m_ui32NumFrame++;

		// gray mat for computing the feature points
		Mat l_oGrayPrevImg (oPreRgbFrame.size(), IPL_DEPTH_8U, 1);
		Mat l_oGrayCurrImg (oPreRgbFrame.size(), IPL_DEPTH_8U, 1);	
		
		// conv rgb images to gray
		cvtColor( oPreRgbFrame , l_oGrayPrevImg, CV_BGR2GRAY );
		cvtColor( oCurrRgbFrame, l_oGrayCurrImg, CV_BGR2GRAY );	
		
		// init feature mat
		Mat l_oFeaturesMat = l_oGrayPrevImg(m_oFaceRect);
		
		// reset feature points vectors
		m_vP2fPrePoints.clear();
		m_vP2fPoints.clear();
		
		// save good features in m_vP2fPrePoints
		try
		{
			goodFeaturesToTrack(l_oFeaturesMat,m_vP2fPrePoints, 30, 0.01, 2);	
			// goodFeaturesToTrack(l_oFeaturesMat,m_vP2fPrePoints, 50, 0.01, 4);
		}
		catch(const cv::Exception &e) 
		{
			cerr << e.what() << endl;
			throw opticalFeaturesError();
		}
		
		// init the second vector
		for(uint ii = 0; ii < m_vP2fPrePoints.size(); ++ii)
		{
			m_vP2fPrePoints[ii].x += m_oFaceRect.x;
			m_vP2fPrePoints[ii].y += m_oFaceRect.y;
			
			m_vP2fPoints.push_back(Point2f(0.f,0.f));
		}

		// launch the tracking
		m_CTrackFlowPtr->track(l_oGrayPrevImg, l_oGrayCurrImg, m_vP2fPrePoints, m_vP2fPoints);

		float l_fDiffX = 0,l_fDiffY =0;
		uint l_ui32NbOfMovedPoints = 0;	
		
		// distances smaller than l_i32MinDist are not taken into account
		int l_i32MinDist = 3,l_i32OffsetX,l_i32OffsetY;		
		
		for(uint ii = 0; ii < m_vP2fPoints.size(); ++ii)
		{
			l_i32OffsetX = (int)(m_vP2fPoints[ii].x - m_vP2fPrePoints[ii].x);
			if(l_i32OffsetX < 0)
			{
				l_i32OffsetX  *= -1;
			}
			l_i32OffsetY = (int)(m_vP2fPoints[ii].y - m_vP2fPrePoints[ii].y);
			
			if(l_i32OffsetY < 0)
			{
				l_i32OffsetY  *= -1;
			}		
			
			if(l_i32OffsetX > l_i32MinDist || l_i32OffsetY > l_i32MinDist )
			{
				l_fDiffX += m_vP2fPoints[ii].x - m_vP2fPrePoints[ii].x;
				l_fDiffY += m_vP2fPoints[ii].y - m_vP2fPrePoints[ii].y;
				l_ui32NbOfMovedPoints++;
			}
		}	
		
		if(l_ui32NbOfMovedPoints!= 0)
		{
			l_fDiffX/= l_ui32NbOfMovedPoints;
			l_fDiffY/= l_ui32NbOfMovedPoints;
		}
		
		// the offset is applied on the rectangle
		if(m_oFaceRect.x + l_fDiffX >0 &&  m_oFaceRect.x + l_fDiffX < oCurrRgbFrame.cols)
		{
			if(l_fDiffX > 3)
			{
				l_fDiffX = 3;
			}
			
			oFaceRect.x += (int)l_fDiffX;
		}
		
		if(m_oFaceRect.y + l_fDiffY >0 &&  m_oFaceRect.y + l_fDiffY < oCurrRgbFrame.rows)
		{
			if(l_fDiffY > 3)
			{
				l_fDiffY = 3;		
			}
			
			oFaceRect.y += (int)l_fDiffY;
		}
		
		m_bTracked = true;
	}
	catch(const opticalFlowError)
	{
		return false;
	}
	
	return true;
}

void SWTrack::displayFeaturesPoints(Mat &oInputMat)
{
	if(m_bTracked)
	{
		// draw features points of the previous frame
		for(uint ii = 0; ii < m_vP2fPrePoints.size(); ++ii)
		{
			circle(oInputMat, m_vP2fPrePoints[ii] ,1, RED);	
		}	
		
		// draw features points of the current frame
		for(uint ii = 0; ii < m_vP2fPoints.size(); ++ii)
		{
			circle(oInputMat, m_vP2fPoints[ii] ,1, BLUE);	
		}		
	}
}

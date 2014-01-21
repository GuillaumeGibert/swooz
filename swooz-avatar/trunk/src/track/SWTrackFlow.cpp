
/**
 * \file SWTrackFlow.cpp
 * \brief defines SWTrackFlow 
 * \author Florian Lance
 * \date 29/11/12
 */

#include <iostream>

#include "track/SWTrackFlow.h"

using namespace cv;
using namespace std;
using namespace swTrack;
using namespace swExcept;


SWTrackFlow::SWTrackFlow(Size oWinSize, int i32MaxLvl) : m_oWinSize(oWinSize), m_i32Maxlvl(i32MaxLvl)
{}

bool SWTrackFlow::track(const Mat &oPreGray, const Mat &oGray,  const vector<Point2f> &vP2fPrePoints, const vector<Point2f> &vP2fPoints)
{
	vector<uchar> l_vU8Status;
	vector<float> l_vFError;
	
	try
	{
		calcOpticalFlowPyrLK(oPreGray, oGray, vP2fPrePoints, vP2fPoints, l_vU8Status, l_vFError, m_oWinSize, m_i32Maxlvl);
	}
	catch( const cv::Exception &e)
	{
		cerr << e.what() << endl;
		throw opticalComputeError();
		
		return false;
	}
	
	return true;
}

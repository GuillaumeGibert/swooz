
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

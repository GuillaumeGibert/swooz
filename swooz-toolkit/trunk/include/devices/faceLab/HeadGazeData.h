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
 * \file HeadGazeData.h
 * \brief Defines Head gaz data structure
 */


#ifndef _HEAD_GAZE_DATA_
#define _HEAD_GAZE_DATA_

#include <vector>

struct Point2D
{
	double x;
	double y;
};

struct Point3D
{
	double x;
	double y;
	double z;
};

struct HeadGazeData
{
	Point3D headTranslation;
	Point3D headRotation;
	
	Point3D rightMouthCorner; 	/**< 4 */
	Point3D leftMouthCorner;  	/**< 5 */
	
    Point3D outerLip0;          /**< 100 */
    Point3D outerLip1;          /**< 101 */
    Point3D outerLip2;          /**< 102 */
    Point3D outerLip3;          /**< 103 */
    Point3D outerLip4;          /**< 104 */
    Point3D outerLip5;          /**< 105 */
		
    Point3D innerLip0;          /**< 200 */
    Point3D innerLip1;          /**< 201 */
    Point3D innerLip2;          /**< 202 */
    Point3D innerLip3;          /**< 203 */
    Point3D innerLip4;          /**< 204 */
    Point3D innerLip5;          /**< 205 */
    Point3D innerLip6;          /**< 206 */
    Point3D innerLip7;          /**< 207 */
		
	Point3D rightEyebrow0; 		/**< 300 */
	Point3D rightEyebrow1; 		/**< 301 */
	Point3D rightEyebrow2; 		/**< 302 */
		
	Point3D leftEyebrow0; 		/**< 400 */
	Point3D leftEyebrow1; 		/**< 401 */
	Point3D leftEyebrow2; 		/**< 402 */
		
    Point3D noseCenter;         /**< 500 */
	
	Point3D leftEyeCenter;		/**< 700 */
	Point3D rightEyeCenter;		/**< 600 */
	
	double leftPupilDiameter;
	Point2D leftGazeRotation;
	sm::eod::GazeQualityLevel leftEyeValidity;
	double leftEyeClosure;
	double rightPupilDiameter;
	Point2D rightGazeRotation;
	sm::eod::GazeQualityLevel rightEyeValidity;
	double rightEyeClosure;
};

//enum GazeQualityLevel
//{
//   NO_TRACKING = 0,        //!< The head is not tracked, no gaze data available.
//   HEAD_DIRECTION,         //!< The eyes could not be tracked, gaze is computed from head orientation only.
//   NON_IR_GAZE,            //!< The gaze is computed from the head pose and the iris location.
//   IR_GAZE,                //!< The gaze is computed from the infrared reflections and the iris location.
//   NUM_GAZE_QUALITY_LEVELS //!< The number of gaze quality levels defined.
//};

/**
 * @brief conv3DPointsToVector
 * @param p
 * @return
 */
static std::vector<double> conv3DPointsToVector(const Point3D &p)
{
	double l_a3DPoint[] = {p.x, p.y, p.z};
	return std::vector<double>(l_a3DPoint, l_a3DPoint + 3);
}

#endif

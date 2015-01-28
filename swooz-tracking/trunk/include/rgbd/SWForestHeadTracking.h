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

#ifndef FOREST_HEAD_TRACKING_H
#define FOREST_HEAD_TRACKING_H

#include <stdio.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <math.h>

////////////////////////////////
// Forest
#include "forest/CRForestEstimator.h"
////////////////////////////////

////////////////////////////////
// GLUT
#include "forest/freeglut.h"
#include "forest/gl_camera.hpp"
////////////////////////////////

////////////////////////////////
// OpenNI
#undef WIN32_LEAN_AND_MEAN
#include <XnOS.h>
#include <XnCppWrapper.h>
#include <XnCodecIDs.h>
using namespace xn;
////////////////////////////////

///////////////////////////////
// YARP
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/Drivers.h>
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
////////////////////////////////////


#define PATH_SEP "/"


using namespace std;
using namespace cv;







#endif


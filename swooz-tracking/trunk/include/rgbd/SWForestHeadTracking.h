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


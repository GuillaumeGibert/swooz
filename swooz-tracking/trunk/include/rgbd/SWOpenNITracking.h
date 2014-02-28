
/**
 * \file OpenNITracking.h
 * \brief Defines OpenNITracking class
 * \author Emmanuel Body
 * \date 07/01/13
 */

#ifndef _SWOPENNITRACKING_
#define _SWOPENNITRACKING_



// Kinect Skeleton (OpenNi)
#include "devices/rgbd/SWKinectSkeleton.h"

// YARP
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

// stdlib
#include <string>
#include <vector>
#include <iostream>

// BOOST
#include <boost/scoped_ptr.hpp>

/**
 * \class SWOpenNITracking
 * \brief This module sends OpenNI upper body skeleton data on a multitude of differents ports
 */
class SWOpenNITracking : public yarp::os::RFModule
{
public:
	SWOpenNITracking();

	bool configure(yarp::os::ResourceFinder &rf);

	bool isOpenNIInitialized() const;

	void initOpenNi();

	bool updateModule();

	double getPeriod();

	bool interruptModule();

private:

	bool m_IsOpenNiInitialized;

	// port names
	std::string m_HeadTrackingPortName;
	std::string m_TorsoTrackingPortName;
	std::string m_LeftArmTrackingPortName;
	std::string m_RightArmTrackingPortName;


	// buffered ports
	yarp::os::BufferedPort<yarp::os::Bottle> m_HeadTrackingPort;
	yarp::os::BufferedPort<yarp::os::Bottle> m_TorsoTrackingPort;
	yarp::os::BufferedPort<yarp::os::Bottle> m_LeftArmTrackingPort;
	yarp::os::BufferedPort<yarp::os::Bottle> m_RightArmTrackingPort;

	boost::scoped_ptr<swDevice::SWKinectSkeleton> m_KinectSkeleton;
};

#endif

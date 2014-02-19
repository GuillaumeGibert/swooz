/**
 * \file OpenNITracking.cpp
 * \brief Defines OpenNITracking class
 * \author Emmanuel Body
 * \date 07/01/13
 */

#include "rgbd/SWOpenNITracking.h"
#include "SWTrackingDevice.h"

// SWOOZ Toolkit
#include "geometryUtility.h"


SWOpenNITracking::SWOpenNITracking()
	:m_IsOpenNiInitialized(true)
{
	std::string l_DeviceName = "rgbd";
    std::string l_LibraryName = "openni";

	m_HeadTrackingPortName		= "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/head";
	m_TorsoTrackingPortName		= "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/torso";
	m_LeftArmTrackingPortName	= "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/left_arm";
	m_RightArmTrackingPortName	= "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/right_arm";

	m_HeadTrackingPort		.open(m_HeadTrackingPortName	.c_str());
	m_TorsoTrackingPort		.open(m_TorsoTrackingPortName	.c_str());
	m_LeftArmTrackingPort	.open(m_LeftArmTrackingPortName	.c_str());
	m_RightArmTrackingPort	.open(m_RightArmTrackingPortName.c_str());

    initOpenNi();
}

void SWOpenNITracking::initOpenNi()
{
	m_KinectSkeleton.reset(swDevice::SWKinectSkeleton::getInstance());
	if (m_KinectSkeleton->init() == 0)
	{
		m_KinectSkeleton->selectProfile(swDevice::SWKinectSkeleton::UpperBody);
		std::cout << "OpenNI initiated" << std::endl;
	}
	else
	{
		std::cout << "Failed OpenNI initialisation" << std::endl;
		m_IsOpenNiInitialized = false;
	}
}

bool SWOpenNITracking::isOpenNIInitialized() const
{
	return m_IsOpenNiInitialized;
}

double SWOpenNITracking::getPeriod()
{
	// module periodicity (seconds), called implicitly by myModule
	return 0.001;
}

bool SWOpenNITracking::interruptModule() {
	m_HeadTrackingPort		.interrupt();
	m_TorsoTrackingPort		.interrupt();
	m_LeftArmTrackingPort	.interrupt();
	m_RightArmTrackingPort	.interrupt();
	return true;
}

bool SWOpenNITracking::configure(yarp::os::ResourceFinder & rf)
{
	// nothing
	return true;
}


bool SWOpenNITracking::updateModule()
{
	if (!m_IsOpenNiInitialized)
	{
		return false;
	}
	std::vector<swDevice::SWKinectSkeleton::Coordinates> l_values;
	if (m_KinectSkeleton->grab(l_values) == 0)
	{
		swDevice::SWKinectSkeleton::Coordinates l_pointTorso		= l_values[0];
		swDevice::SWKinectSkeleton::Coordinates l_pointNeck			= l_values[1];
		swDevice::SWKinectSkeleton::Coordinates l_pointHead			= l_values[2];
		swDevice::SWKinectSkeleton::Coordinates l_pointLShoulder	= l_values[3];
		swDevice::SWKinectSkeleton::Coordinates l_pointRShoulder	= l_values[4];
		swDevice::SWKinectSkeleton::Coordinates l_pointLElbow		= l_values[5];
		swDevice::SWKinectSkeleton::Coordinates l_pointRElbow		= l_values[6];
		swDevice::SWKinectSkeleton::Coordinates l_pointLHand		= l_values[7];
		swDevice::SWKinectSkeleton::Coordinates l_pointRHand		= l_values[8];


		yarp::os::Bottle & l_HeadBottle		= m_HeadTrackingPort.prepare();
		l_HeadBottle.clear();
			l_HeadBottle.addInt(swTracking::OPENNI_LIB);
			l_HeadBottle.addDouble(l_pointNeck.X);			l_HeadBottle.addDouble(l_pointNeck.Y);			l_HeadBottle.addDouble(l_pointNeck.Z);
			l_HeadBottle.addDouble(l_pointHead.X);			l_HeadBottle.addDouble(l_pointHead.Y);			l_HeadBottle.addDouble(l_pointHead.Z);
			l_HeadBottle.addDouble(l_pointLShoulder.X);		l_HeadBottle.addDouble(l_pointLShoulder.Y);		l_HeadBottle.addDouble(l_pointLShoulder.Z);
			l_HeadBottle.addDouble(l_pointRShoulder.X);		l_HeadBottle.addDouble(l_pointRShoulder.Y);		l_HeadBottle.addDouble(l_pointRShoulder.Z);
		m_HeadTrackingPort.write();

		yarp::os::Bottle & l_TorsoBottle 	= m_TorsoTrackingPort.prepare();
		l_TorsoBottle.clear();
			l_TorsoBottle.addInt(swTracking::OPENNI_LIB);
			l_TorsoBottle.addDouble(l_pointTorso.X);		l_TorsoBottle.addDouble(l_pointTorso.Y);		l_TorsoBottle.addDouble(l_pointTorso.Z);
			l_TorsoBottle.addDouble(l_pointNeck.X);			l_TorsoBottle.addDouble(l_pointNeck.Y);			l_TorsoBottle.addDouble(l_pointNeck.Z);
			l_TorsoBottle.addDouble(l_pointLShoulder.X);	l_TorsoBottle.addDouble(l_pointLShoulder.Y);	l_TorsoBottle.addDouble(l_pointLShoulder.Z);
			l_TorsoBottle.addDouble(l_pointRShoulder.X);	l_TorsoBottle.addDouble(l_pointRShoulder.Y);	l_TorsoBottle.addDouble(l_pointRShoulder.Z);
		m_TorsoTrackingPort.write();

		yarp::os::Bottle & l_LeftArmBottle 	= m_LeftArmTrackingPort.prepare();
		l_LeftArmBottle.clear();
			l_LeftArmBottle.addInt(swTracking::OPENNI_LIB);
			l_LeftArmBottle.addDouble(l_pointTorso.X);		l_LeftArmBottle.addDouble(l_pointTorso.Y);		l_LeftArmBottle.addDouble(l_pointTorso.Z);
			l_LeftArmBottle.addDouble(l_pointNeck.X);		l_LeftArmBottle.addDouble(l_pointNeck.Y);		l_LeftArmBottle.addDouble(l_pointNeck.Z);
			l_LeftArmBottle.addDouble(l_pointLShoulder.X);	l_LeftArmBottle.addDouble(l_pointLShoulder.Y);	l_LeftArmBottle.addDouble(l_pointLShoulder.Z);
			l_LeftArmBottle.addDouble(l_pointLElbow.X);		l_LeftArmBottle.addDouble(l_pointLElbow.Y);		l_LeftArmBottle.addDouble(l_pointLElbow.Z);
			l_LeftArmBottle.addDouble(l_pointLHand.X);		l_LeftArmBottle.addDouble(l_pointLHand.Y);		l_LeftArmBottle.addDouble(l_pointLHand.Z);
		m_LeftArmTrackingPort.write();

		yarp::os::Bottle & l_RightArmBottle = m_RightArmTrackingPort.prepare();
		l_RightArmBottle.clear();
			l_RightArmBottle.addInt(swTracking::OPENNI_LIB);
			l_RightArmBottle.addDouble(l_pointTorso.X);		l_RightArmBottle.addDouble(l_pointTorso.Y);		l_RightArmBottle.addDouble(l_pointTorso.Z);
			l_RightArmBottle.addDouble(l_pointNeck.X);		l_RightArmBottle.addDouble(l_pointNeck.Y);		l_RightArmBottle.addDouble(l_pointNeck.Z);
			l_RightArmBottle.addDouble(l_pointRShoulder.X);	l_RightArmBottle.addDouble(l_pointRShoulder.Y);	l_RightArmBottle.addDouble(l_pointRShoulder.Z);
			l_RightArmBottle.addDouble(l_pointRElbow.X);	l_RightArmBottle.addDouble(l_pointRElbow.Y);	l_RightArmBottle.addDouble(l_pointRElbow.Z);
			l_RightArmBottle.addDouble(l_pointRHand.X);		l_RightArmBottle.addDouble(l_pointRHand.Y);		l_RightArmBottle.addDouble(l_pointRHand.Z);
		m_RightArmTrackingPort.write();
	}


	return true; // RF Module OK
}


int main(int argc, char **argv)
{

    yarp::os::Network netwotk;
    if (!netwotk.checkNetwork())
    {
        std::cerr << "/!\\ Problem connecting to YARP server." << std::endl;
        return -1;
    }

    SWOpenNITracking l_OpenNITracking;

    std::cout << "Starting the OpenNI Skeleton capture module.." << std::endl;

    l_OpenNITracking.runModule();

    std::cout<< "Ending OpenNI Skeleton capture module..." << std::endl;
}

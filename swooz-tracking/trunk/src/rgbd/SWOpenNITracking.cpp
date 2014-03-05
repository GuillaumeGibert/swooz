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
    :m_IsOpenNiInitialized(false)
{
    std::string l_DeviceName  = "rgbd";
    std::string l_LibraryName = "openni";

    m_sHeadTrackingPortName		= "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/head";
    m_sTorsoTrackingPortName    = "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/torso";
    m_sLeftArmTrackingPortName	= "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/left_arm";
    m_sRightArmTrackingPortName	= "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/right_arm";

    m_oHeadTrackingPort.open(m_sHeadTrackingPortName.c_str());
    m_oTorsoTrackingPort.open(m_sTorsoTrackingPortName.c_str());
    m_oLeftArmTrackingPort.open(m_sLeftArmTrackingPortName.c_str());
    m_oRightArmTrackingPort.open(m_sRightArmTrackingPortName.c_str());

    initOpenNi();
}

SWOpenNITracking::~SWOpenNITracking()
{
    m_oHeadTrackingPort.close();
    m_oTorsoTrackingPort.close();
    m_oLeftArmTrackingPort.close();
    m_oRightArmTrackingPort.close();
}

void SWOpenNITracking::initOpenNi()
{
    m_oKinectSkeleton.reset(swDevice::SWKinectSkeleton::getInstance());

    if (m_oKinectSkeleton->init() == 0)
	{
        m_oKinectSkeleton->selectProfile(swDevice::SWKinectSkeleton::UpperBody);
        m_IsOpenNiInitialized = true;

        std::cout << "OpenNI skeleton initialized. " << std::endl;
	}
	else
	{
        std::cerr << "-ERROR : OpenNI initialisation failure. " << std::endl;
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
    m_oHeadTrackingPort		.interrupt();
    m_oTorsoTrackingPort    .interrupt();
    m_oLeftArmTrackingPort	.interrupt();
    m_oRightArmTrackingPort	.interrupt();
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
    if (m_oKinectSkeleton->grab(l_values) == 0)
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


        yarp::os::Bottle & l_HeadBottle		= m_oHeadTrackingPort.prepare();
		l_HeadBottle.clear();
			l_HeadBottle.addInt(swTracking::OPENNI_LIB);
			l_HeadBottle.addDouble(l_pointNeck.X);			l_HeadBottle.addDouble(l_pointNeck.Y);			l_HeadBottle.addDouble(l_pointNeck.Z);
			l_HeadBottle.addDouble(l_pointHead.X);			l_HeadBottle.addDouble(l_pointHead.Y);			l_HeadBottle.addDouble(l_pointHead.Z);
			l_HeadBottle.addDouble(l_pointLShoulder.X);		l_HeadBottle.addDouble(l_pointLShoulder.Y);		l_HeadBottle.addDouble(l_pointLShoulder.Z);
			l_HeadBottle.addDouble(l_pointRShoulder.X);		l_HeadBottle.addDouble(l_pointRShoulder.Y);		l_HeadBottle.addDouble(l_pointRShoulder.Z);
        m_oHeadTrackingPort.write();

        yarp::os::Bottle & l_TorsoBottle 	= m_oTorsoTrackingPort.prepare();
		l_TorsoBottle.clear();
			l_TorsoBottle.addInt(swTracking::OPENNI_LIB);
			l_TorsoBottle.addDouble(l_pointTorso.X);		l_TorsoBottle.addDouble(l_pointTorso.Y);		l_TorsoBottle.addDouble(l_pointTorso.Z);
			l_TorsoBottle.addDouble(l_pointNeck.X);			l_TorsoBottle.addDouble(l_pointNeck.Y);			l_TorsoBottle.addDouble(l_pointNeck.Z);
			l_TorsoBottle.addDouble(l_pointLShoulder.X);	l_TorsoBottle.addDouble(l_pointLShoulder.Y);	l_TorsoBottle.addDouble(l_pointLShoulder.Z);
			l_TorsoBottle.addDouble(l_pointRShoulder.X);	l_TorsoBottle.addDouble(l_pointRShoulder.Y);	l_TorsoBottle.addDouble(l_pointRShoulder.Z);
        m_oTorsoTrackingPort.write();

        yarp::os::Bottle & l_LeftArmBottle 	= m_oLeftArmTrackingPort.prepare();
		l_LeftArmBottle.clear();
			l_LeftArmBottle.addInt(swTracking::OPENNI_LIB);
			l_LeftArmBottle.addDouble(l_pointTorso.X);		l_LeftArmBottle.addDouble(l_pointTorso.Y);		l_LeftArmBottle.addDouble(l_pointTorso.Z);
			l_LeftArmBottle.addDouble(l_pointNeck.X);		l_LeftArmBottle.addDouble(l_pointNeck.Y);		l_LeftArmBottle.addDouble(l_pointNeck.Z);
			l_LeftArmBottle.addDouble(l_pointLShoulder.X);	l_LeftArmBottle.addDouble(l_pointLShoulder.Y);	l_LeftArmBottle.addDouble(l_pointLShoulder.Z);
			l_LeftArmBottle.addDouble(l_pointLElbow.X);		l_LeftArmBottle.addDouble(l_pointLElbow.Y);		l_LeftArmBottle.addDouble(l_pointLElbow.Z);
			l_LeftArmBottle.addDouble(l_pointLHand.X);		l_LeftArmBottle.addDouble(l_pointLHand.Y);		l_LeftArmBottle.addDouble(l_pointLHand.Z);
        m_oLeftArmTrackingPort.write();

        yarp::os::Bottle & l_RightArmBottle = m_oRightArmTrackingPort.prepare();
		l_RightArmBottle.clear();
			l_RightArmBottle.addInt(swTracking::OPENNI_LIB);
			l_RightArmBottle.addDouble(l_pointTorso.X);		l_RightArmBottle.addDouble(l_pointTorso.Y);		l_RightArmBottle.addDouble(l_pointTorso.Z);
			l_RightArmBottle.addDouble(l_pointNeck.X);		l_RightArmBottle.addDouble(l_pointNeck.Y);		l_RightArmBottle.addDouble(l_pointNeck.Z);
			l_RightArmBottle.addDouble(l_pointRShoulder.X);	l_RightArmBottle.addDouble(l_pointRShoulder.Y);	l_RightArmBottle.addDouble(l_pointRShoulder.Z);
			l_RightArmBottle.addDouble(l_pointRElbow.X);	l_RightArmBottle.addDouble(l_pointRElbow.Y);	l_RightArmBottle.addDouble(l_pointRElbow.Z);
			l_RightArmBottle.addDouble(l_pointRHand.X);		l_RightArmBottle.addDouble(l_pointRHand.Y);		l_RightArmBottle.addDouble(l_pointRHand.Z);
        m_oRightArmTrackingPort.write();
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

    std::cout << "Starting the OpenNI Skeleton capture module.." << std::endl;
    SWOpenNITracking l_OpenNITracking;

    // prepare and configure the resource finder
    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("openniTracking.ini");
    rf.setDefaultContext("swtracking/conf");
    rf.configure("YARP_DIR", argc, argv);

    if(l_OpenNITracking.configure(rf))
    {
         l_OpenNITracking.runModule();
    }

    std::cout << "Ending OpenNI Skeleton capture module..." << std::endl;
}

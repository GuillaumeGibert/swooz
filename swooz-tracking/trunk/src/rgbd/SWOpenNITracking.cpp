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
 * \file OpenNITracking.cpp
 * \brief Defines OpenNITracking class
 * \author Emmanuel Body
 * \date 07/01/13
 */

#include "rgbd/SWOpenNITracking.h"
#include "SWTrackingDevice.h"

// SWOOZ Toolkit
#include "geometryUtility.h"
#include "SWExceptions.h"


SWOpenNITracking::SWOpenNITracking()
    :m_IsOpenNiInitialized(false)
{
    std::string l_DeviceName  = "rgbd";
    std::string l_LibraryName = "openni";

    m_sHeadTrackingPortName		= "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/head";
    m_sTorsoTrackingPortName    = "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/torso";
    m_sLeftArmTrackingPortName	= "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/left_arm";
    m_sRightArmTrackingPortName	= "/tracking/" + l_DeviceName + "/"+ l_LibraryName + "/right_arm";

	m_siCubEncodersTrackingPortName = "/tracking/openni/icub/encoders";
	
    m_oHeadTrackingPort.open(m_sHeadTrackingPortName.c_str());
    m_oTorsoTrackingPort.open(m_sTorsoTrackingPortName.c_str());
    m_oLeftArmTrackingPort.open(m_sLeftArmTrackingPortName.c_str());
    m_oRightArmTrackingPort.open(m_sRightArmTrackingPortName.c_str());
    m_oAllJointsTrackingPort.open("/tracking/joints:o");
	
	m_oiCubEncodersTrackingPort.open(m_siCubEncodersTrackingPortName.c_str());
	

    initOpenNi();
}


SWOpenNITracking::~SWOpenNITracking()
{
    m_oHeadTrackingPort.close();
    m_oTorsoTrackingPort.close();
    m_oLeftArmTrackingPort.close();
    m_oRightArmTrackingPort.close();
    m_oAllJointsTrackingPort.close();
	
	m_oiCubEncodersTrackingPort.close();
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
    return 1/30.;
}

bool SWOpenNITracking::interruptModule() {
    m_oHeadTrackingPort		.interrupt();
    m_oTorsoTrackingPort    .interrupt();
    m_oLeftArmTrackingPort	.interrupt();
    m_oRightArmTrackingPort	.interrupt();
    m_oAllJointsTrackingPort	.interrupt();
	
	m_oiCubEncodersTrackingPort.interrupt();
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
		swDevice::SWKinectSkeleton::Coordinates l_pointNeck		= l_values[1];
		swDevice::SWKinectSkeleton::Coordinates l_pointHead		= l_values[2];
		swDevice::SWKinectSkeleton::Coordinates l_pointLShoulder		= l_values[3];
		swDevice::SWKinectSkeleton::Coordinates l_pointRShoulder		= l_values[4];
		swDevice::SWKinectSkeleton::Coordinates l_pointLElbow		= l_values[5];
		swDevice::SWKinectSkeleton::Coordinates l_pointRElbow		= l_values[6];
		swDevice::SWKinectSkeleton::Coordinates l_pointLHand		= l_values[7];
		swDevice::SWKinectSkeleton::Coordinates l_pointRHand		= l_values[8];
		swDevice::SWKinectSkeleton::Coordinates l_pointLHip			= l_values[9];
		swDevice::SWKinectSkeleton::Coordinates l_pointRHip			= l_values[10];


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


        yarp::os::Bottle &l_allJointsBottle = m_oAllJointsTrackingPort.prepare();
        l_allJointsBottle.clear();
            l_HeadBottle.addDouble(l_pointNeck.X);			l_HeadBottle.addDouble(l_pointNeck.Y);			l_HeadBottle.addDouble(l_pointNeck.Z);
            l_HeadBottle.addDouble(l_pointHead.X);			l_HeadBottle.addDouble(l_pointHead.Y);			l_HeadBottle.addDouble(l_pointHead.Z);
            l_HeadBottle.addDouble(l_pointLShoulder.X);		l_HeadBottle.addDouble(l_pointLShoulder.Y);		l_HeadBottle.addDouble(l_pointLShoulder.Z);
            l_HeadBottle.addDouble(l_pointRShoulder.X);		l_HeadBottle.addDouble(l_pointRShoulder.Y);		l_HeadBottle.addDouble(l_pointRShoulder.Z);
            l_TorsoBottle.addDouble(l_pointTorso.X);		l_TorsoBottle.addDouble(l_pointTorso.Y);		l_TorsoBottle.addDouble(l_pointTorso.Z);
            l_LeftArmBottle.addDouble(l_pointLElbow.X);		l_LeftArmBottle.addDouble(l_pointLElbow.Y);		l_LeftArmBottle.addDouble(l_pointLElbow.Z);
            l_RightArmBottle.addDouble(l_pointRElbow.X);	l_RightArmBottle.addDouble(l_pointRElbow.Y);	l_RightArmBottle.addDouble(l_pointRElbow.Z);
            l_LeftArmBottle.addDouble(l_pointLHand.X);		l_LeftArmBottle.addDouble(l_pointLHand.Y);		l_LeftArmBottle.addDouble(l_pointLHand.Z);
            l_RightArmBottle.addDouble(l_pointRHand.X);		l_RightArmBottle.addDouble(l_pointRHand.Y);		l_RightArmBottle.addDouble(l_pointRHand.Z);
        m_oAllJointsTrackingPort.write();
	
	
	yarp::os::Bottle &l_iCubEncodersBottle = m_oiCubEncodersTrackingPort.prepare();
        l_iCubEncodersBottle.clear();
	//head
	std::vector<double> l_vPointNeck(3), l_vPointHead(3), l_vPointTorso(3), l_vPointLShoulder(3), l_vPointRShoulder(3), l_vPointLElbow(3), l_vPointRElbow(3), l_vPointLHand(3), l_vPointRHand(3);
		l_vPointNeck[0] = l_pointNeck.X;
		l_vPointNeck[1] = l_pointNeck.Y;
		l_vPointNeck[2] = l_pointNeck.Z;
		l_vPointHead[0] = l_pointHead.X;
		l_vPointHead[1] = l_pointHead.Y;
		l_vPointHead[2] = l_pointHead.Z;
		l_vPointLShoulder[0] = l_pointLShoulder.X;
		l_vPointLShoulder[1] = l_pointLShoulder.Y;
		l_vPointLShoulder[2] = l_pointLShoulder.Z;
		l_vPointRShoulder[0] = l_pointRShoulder.X;
		l_vPointRShoulder[1] = l_pointRShoulder.Y;
		l_vPointRShoulder[2] = l_pointRShoulder.Z;

		std::vector<double> l_vecClavicles  = swUtil::vec(l_vPointLShoulder,	l_vPointRShoulder);
		std::vector<double> l_vecHead       = swUtil::vec(l_vPointNeck,		l_vPointHead);
		std::vector<double> l_rpyHead = swUtil::computeRollPitchYaw(l_vecHead, l_vecClavicles);
		
	l_iCubEncodersBottle.addDouble(-l_rpyHead[1]);
	l_iCubEncodersBottle.addDouble(-l_rpyHead[0]);
	l_iCubEncodersBottle.addDouble(0);
	//
	// torso
	yarp::sig::Vector l_vTorsoJoints;
        l_vTorsoJoints.resize(3);
        l_vTorsoJoints = 0.;
	
	l_vPointTorso[0] = l_pointTorso.X;
	l_vPointTorso[1] = l_pointTorso.Y;
        l_vPointTorso[2] = l_pointTorso.Z;
	std::vector<double> l_vecTorso      = swUtil::vec(l_vPointTorso, l_vPointNeck);
	std::vector<double> l_rpyTorso      = swUtil::computeRollPitchYaw(l_vecTorso, l_vecClavicles);
	
	 l_vTorsoJoints[0] = -l_rpyTorso[2];
	l_vTorsoJoints[1] = l_rpyTorso[0];
	//~ l_vTorsoJoints[2] = l_rpyTorso[1];
			
	std::vector<double> l_pointXaxis(3), l_pointYaxis(3), l_pointZaxis(3), l_pointOrigin(3);
	l_pointOrigin[0] = 0.0; l_pointOrigin[1] = 0.0; l_pointOrigin[2] = 0.0; 
	l_pointXaxis[0] = 1.0; l_pointXaxis[1] = 0.0; l_pointXaxis[2] = 0.0; 
	l_pointYaxis[0] = 0.0; l_pointYaxis[1] = 1.0; l_pointYaxis[2] = 0.0; 
	l_pointZaxis[0] = 0.0; l_pointZaxis[1] = 0.0; l_pointZaxis[2] = 1.0; 
	std::vector<double> l_vecXAxis  = swUtil::vec(l_pointOrigin, l_pointXaxis);
	std::vector<double> l_vecYAxis  = swUtil::vec(l_pointOrigin, l_pointYaxis);
	std::vector<double> l_vecZAxis  = swUtil::vec(l_pointOrigin, l_pointZaxis);
				
	if (l_vecClavicles[2]>0)
	{
		l_vTorsoJoints[0] = swUtil::vectorAngle(l_vecClavicles, l_vecXAxis);
	}
	else
	{
		l_vTorsoJoints[0] = -swUtil::vectorAngle(l_vecClavicles, l_vecXAxis);
	}
	
	if (l_vecTorso[0]>0)
	{
		l_vTorsoJoints[1] = -swUtil::vectorAngle(l_vecTorso, l_vecYAxis);
	}
	else
	{
		l_vTorsoJoints[1] = swUtil::vectorAngle(l_vecTorso, l_vecYAxis);
	}
	
	l_vTorsoJoints[2] = swUtil::vectorAngle(l_vecTorso, l_vecZAxis) - 90;
	
	l_iCubEncodersBottle.addDouble(l_vTorsoJoints[0]);
	l_iCubEncodersBottle.addDouble(l_vTorsoJoints[1]);
	l_iCubEncodersBottle.addDouble(l_vTorsoJoints[2]);
	//
	// left arm
	yarp::sig::Vector l_vLArmJoints;
        l_vLArmJoints.resize(15);
        l_vLArmJoints = 0.;
	
	l_vPointLElbow[0] = l_pointLElbow.X;
	l_vPointLElbow[1] = l_pointLElbow.Y;
	l_vPointLElbow[2] = l_pointLElbow.Z;
	
	l_vPointLHand[0] = l_pointLHand.X;
	l_vPointLHand[1] = l_pointLHand.Y;
	l_vPointLHand[2] = l_pointLHand.Z;
	
	l_vecTorso      = swUtil::vec(l_vPointNeck, l_vPointTorso);
	std::vector<double> l_vecLForearm = swUtil::vec(l_vPointLElbow, l_vPointLHand);
	std::vector<double> l_vecLShoulder = swUtil::vec(l_vPointLShoulder, l_vPointNeck);
	std::vector<double> l_vecLArm = swUtil::vec( l_vPointLShoulder, l_vPointLElbow);

	std::vector<double> l_rpyLShoulder = swUtil::computeRollPitchYaw(l_vecLArm, l_vecTorso);
	std::vector<double> l_rpyLElbow = swUtil::computeRollPitchYaw(l_vecLForearm, l_vecLArm);
	std::vector<double> l_vecLArm_roll(3);
	l_vecLArm_roll[0] = 0; l_vecLArm_roll[1] = l_vecLArm[1]; l_vecLArm_roll[2] = l_vecLArm[2]; 
	l_vLArmJoints[0] = -swUtil::vectorAngle(l_vecTorso, l_vecLArm_roll);
	
	std::vector<double> l_vecLArm_pitch(3);
	l_vecLArm_pitch[0] =  l_vecLArm[0]; l_vecLArm_pitch[1] = l_vecLArm[1]; l_vecLArm_pitch[2] = 0; 
	l_vLArmJoints[1] = swUtil::vectorAngle(l_vecTorso, l_vecLArm_pitch);
	
	l_vLArmJoints[3] = swUtil::vectorAngle(l_vecLForearm, l_vecLArm);
	
	for (int l_enc=0; l_enc < 15; l_enc++)
		l_iCubEncodersBottle.addDouble(l_vLArmJoints[l_enc]);
	//
	// right arm
	yarp::sig::Vector l_vRArmJoints;
        l_vRArmJoints.resize(15);
        l_vRArmJoints = 0.;
	
	l_vPointRElbow[0] = l_pointRElbow.X;
	l_vPointRElbow[1] = l_pointRElbow.Y;
	l_vPointRElbow[2] = l_pointRElbow.Z;
	
	l_vPointRHand[0] = l_pointRHand.X;
	l_vPointRHand[1] = l_pointRHand.Y;
	l_vPointRHand[2] = l_pointRHand.Z;
	
	l_vecTorso      = swUtil::vec(l_vPointNeck, l_vPointTorso);
	std::vector<double> l_vecRForearm = swUtil::vec(l_vPointRElbow, l_vPointRHand);
	std::vector<double> l_vecRShoulder = swUtil::vec(l_vPointRShoulder, l_vPointNeck);
	std::vector<double> l_vecRArm = swUtil::vec( l_vPointRShoulder, l_vPointRElbow);

	std::vector<double> l_rpyRShoulder = swUtil::computeRollPitchYaw(l_vecRArm, l_vecTorso);
	std::vector<double> l_rpyRElbow = swUtil::computeRollPitchYaw(l_vecRForearm, l_vecRArm);
	std::vector<double> l_vecRArm_roll(3);
	l_vecRArm_roll[0] = 0; l_vecRArm_roll[1] = l_vecRArm[1]; l_vecRArm_roll[2] = l_vecRArm[2]; 
	l_vRArmJoints[0] = -swUtil::vectorAngle(l_vecTorso, l_vecRArm_roll);
	
	std::vector<double> l_vecRArm_pitch(3);
	l_vecRArm_pitch[0] =  l_vecRArm[0]; l_vecRArm_pitch[1] = l_vecRArm[1]; l_vecRArm_pitch[2] = 0; 
	l_vRArmJoints[1] = swUtil::vectorAngle(l_vecTorso, l_vecRArm_pitch);
	
	l_vRArmJoints[3] = swUtil::vectorAngle(l_vecRForearm, l_vecRArm);
	
	for (int l_enc=0; l_enc < 15; l_enc++)
		l_iCubEncodersBottle.addDouble(l_vRArmJoints[l_enc]);
	//
	m_oiCubEncodersTrackingPort.write();
	
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
//    yarp::os::ResourceFinder rf;
//    rf.setVerbose(true);
//    rf.setDefaultConfigFile("openniTracking.ini");
//    rf.setDefaultContext("swtracking/conf");
//    rf.configure("YARP_DIR", argc, argv);

//    if(l_OpenNITracking.configure(rf))
//    {
         l_OpenNITracking.runModule();
//    }

    std::cout << "Ending OpenNI Skeleton capture module..." << std::endl;
}

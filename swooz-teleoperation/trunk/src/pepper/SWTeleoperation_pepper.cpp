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
 * \file SWTeleoperation_pepper.cpp
 * \author Guillaume Gibert
 * \date 14/11/2016
 * \brief ...
 */

#include "pepper/SWTeleoperation_pepper.h"

#include "SWTrackingDevice.h"

#include <sstream>
#include <vector>
#include <cmath>
#include <iostream>


#include "opencv2/core/core.hpp"
#include "opencvUtility.h"
#include "geometryUtility.h"

SWTeleoperation_pepper::SWTeleoperation_pepper() :  m_i32HeadTimeLastBottle(0)
{    
	m_bHeadActivatedDefault     = true;
	m_bLeftArmActivatedDefault  = false;
	m_bRightArmActivatedDefault = false;

	double l_aDHeadMinJointDefault[]     = {-2.0, -0.5};
	double l_aDHeadMaxJointDefault[]     = { 2.0,  0.5};
	double l_aDLeftArmMinJointDefault[]  = {-2.0857, 0.0087, -2.0857, -1.5620, -1.8239, 0.0};
	double l_aDLeftArmMaxJointDefault[]  = { 2.0857,  1.5620,   2.0857,  -0.0087,  1.8239, 1.0};
	double l_aDRightArmMinJointDefault[] = {-2.0857, -1.5620,  -2.0857,  0.0087, -1.8239, 0.0};
	double l_aDRightArmMaxJointDefault[] = { 2.0857,  -0.0087,  2.0857,  1.5620,  1.8239, 1.0};

	m_vHeadMinJointDefault      = std::vector<double>(l_aDHeadMinJointDefault, l_aDHeadMinJointDefault + sizeof(l_aDHeadMinJointDefault) / sizeof(double));
	m_vHeadMaxJointDefault      = std::vector<double>(l_aDHeadMaxJointDefault, l_aDHeadMaxJointDefault + sizeof(l_aDHeadMaxJointDefault) / sizeof(double));
	m_vLeftArmMinJointDefault   = std::vector<double>(l_aDLeftArmMinJointDefault, l_aDLeftArmMinJointDefault + sizeof(l_aDLeftArmMinJointDefault) / sizeof(double));
	m_vLeftArmMaxJointDefault   = std::vector<double>(l_aDLeftArmMaxJointDefault, l_aDLeftArmMaxJointDefault + sizeof(l_aDLeftArmMaxJointDefault) / sizeof(double));
	m_vRightArmMinJointDefault  = std::vector<double>(l_aDRightArmMinJointDefault, l_aDRightArmMinJointDefault + sizeof(l_aDRightArmMinJointDefault) / sizeof(double));
	m_vRightArmMaxJointDefault  = std::vector<double>(l_aDRightArmMaxJointDefault, l_aDRightArmMaxJointDefault + sizeof(l_aDRightArmMaxJointDefault) / sizeof(double));

	m_vHeadMinJoint     = std::vector<double>(m_vHeadMinJointDefault.size());
	m_vHeadMaxJoint     = std::vector<double>(m_vHeadMaxJointDefault.size());
	m_vLeftArmMinJoint  = std::vector<double>(m_vLeftArmMinJointDefault.size());
	m_vLeftArmMaxJoint  = std::vector<double>(m_vLeftArmMaxJointDefault.size());
	m_vRightArmMinJoint = std::vector<double>(m_vRightArmMinJointDefault.size());
	m_vRightArmMaxJoint = std::vector<double>(m_vRightArmMaxJointDefault.size());

	m_aHeadAngles.arraySetSize(2);
	m_aLArmAngles.arraySetSize(6);
	m_aRArmAngles.arraySetSize(6);
}


bool SWTeleoperation_pepper::configure(ResourceFinder &oRf)
{
	// gets the module name which will form the stem of all module port names
        m_sModuleName   = oRf.check("name", Value("teleoperation_pepper"), "Teleoperation/pepper Module name (string)").asString();
        setName(m_sModuleName.c_str());
        m_sRobotAddress = oRf.check("IP", Value("169.254.108.110"), "IP Adress of the Pepper Robot").asString();

	// acceleration/speeds values for pepper
        m_dJointVelocityValue     = oRf.check("jointVelocityValue",  yarp::os::Value(0.1),  "Joint Velocity Value (float)").asDouble();

	// robot parts to control
        m_bHeadActivated    = oRf.check("headActivated",yarp::os::Value(m_bHeadActivatedDefault), "Head activated (int)").asInt() != 0;
        m_bLeftArmActivated = oRf.check("leftArmHandActivated",yarp::os::Value(m_bLeftArmActivatedDefault), "Left arm activated (int)"). asInt() != 0;
        m_bRightArmActivated= oRf.check("rightArmHandActivated",yarp::os::Value(m_bRightArmActivatedDefault), "Right arm activated (int)"). asInt() != 0;

        m_startPosture  = oRf.check("startPosture", yarp::os::Value(0), "Start posture (int)").asInt();
        m_endPosture    = oRf.check("endPosture", yarp::os::Value(0), "End posture (int)").asInt();

        for(uint ii = 0; ii < m_vLeftArmMinJointDefault.size(); ++ii)
        {
		std::ostringstream l_os;
		l_os << ii;

		std::string l_sHeadMinJoint("headMinValueJoint" + l_os.str());
		std::string l_sHeadMaxJoint("headMaxValueJoint" + l_os.str());
		std::string l_sLeftArmMinJoint("leftArmMinValueJoint" + l_os.str());
		std::string l_sLeftArmMaxJoint("leftArmMaxValueJoint" + l_os.str());
		std::string l_sRightArmMinJoint("rightArmMinValueJoint" + l_os.str());
		std::string l_sRightArmMaxJoint("rightArmMaxValueJoint" + l_os.str());

		std::string l_sHeadMinJointInfo("Head minimum joint " + l_os.str() + " Value (double)");
		std::string l_sHeadMaxJointInfo("Head maximum joint " + l_os.str() + " Value (double)");
		std::string l_sLeftArmMinJointInfo("Left arm minimum joint " + l_os.str() + " Value (double)");
		std::string l_sLeftArmMaxJointInfo("Left arm maximum joint " + l_os.str() + " Value (double)");
		std::string l_sRightArmMinJointInfo("Right arm minimum joint " + l_os.str() + " Value (double)");
		std::string l_sRightArmMaxJointInfo("Right arm maximum joint " + l_os.str() + " Value (double)");

		// head
		if(ii < 2)
		{
			m_vHeadMinJoint[ii]     = oRf.check(l_sHeadMinJoint.c_str(), m_vHeadMinJointDefault[ii], l_sHeadMinJointInfo.c_str()).asDouble();
			m_vHeadMaxJoint[ii]     = oRf.check(l_sHeadMaxJoint.c_str(), m_vHeadMaxJointDefault[ii], l_sHeadMaxJointInfo.c_str()).asDouble();
		}

		// left arm
		m_vLeftArmMinJoint[ii]      = oRf.check(l_sLeftArmMinJoint.c_str(), m_vLeftArmMinJointDefault[ii], l_sLeftArmMinJointInfo.c_str()).asDouble();
		m_vLeftArmMaxJoint[ii]      = oRf.check(l_sLeftArmMaxJoint.c_str(), m_vLeftArmMaxJointDefault[ii], l_sLeftArmMaxJointInfo.c_str()).asDouble();
		// right arm
		m_vRightArmMinJoint[ii]     = oRf.check(l_sRightArmMinJoint.c_str(), m_vRightArmMinJointDefault[ii], l_sRightArmMinJointInfo.c_str()).asDouble();
		m_vRightArmMaxJoint[ii]     = oRf.check(l_sRightArmMaxJoint.c_str(), m_vRightArmMaxJointDefault[ii], l_sRightArmMaxJointInfo.c_str()).asDouble();
	}

	// miscellaneous
        m_i32Fps                    = oRf.check("fps",              yarp::os::Value(100),  "Frame per second (int)").asInt();
        m_i32HeadTimeoutReset       = oRf.check("headTimeoutReset", yarp::os::Value(3000), "Head gaze timeout reset pepper (int)").asInt();
	m_i32LArmTimeoutReset       = oRf.check("leftArmHandTimeOutReset", yarp::os::Value(3000), "Left arm timeout reset pepper (int)").asInt();
	m_i32RArmTimeoutReset       = oRf.check("rightArmHandTimeOutReset", yarp::os::Value(3000), "Right arm timeout reset pepper (int)").asInt();
	
	// init ports
        std::string l_sHeadTrackerPortName  = "/teleoperation/pepper/head";
        std::string l_sLeftArmTrackerPortName = "/teleoperation/pepper/left_arm/hand_fingers";
        std::string l_sRightArmTrackerPortName= "/teleoperation/pepper/right_arm/hand_fingers";

        if(!m_oHeadTrackerPort.open(l_sHeadTrackerPortName.c_str()) ||
            !m_oLeftArmTrackerPort.open(l_sLeftArmTrackerPortName.c_str()) ||
            !m_oRightArmTrackerPort.open(l_sRightArmTrackerPortName.c_str()))
        {
		std::cerr << "-ERROR: Unable to open ports." << std::endl;
		interruptModule();
		return false;
        }

        try
        {
		m_oRobotMotionProxy = new ALMotionProxy(m_sRobotAddress);
        }
        catch (const AL::ALError& e)
        {
		std::cerr << "-ERROR: " << e.what() << std::endl;
		return false;
        }


        // set starting posture
        ALRobotPostureProxy l_oRobotRobotPosture=  ALRobotPostureProxy(m_sRobotAddress);
        try
        {
		if(m_startPosture == 0)
		{
			l_oRobotRobotPosture.goToPosture("Stand", 1.0);
		}
		else if(m_startPosture == 1)
		{
			l_oRobotRobotPosture.goToPosture("StandZero", 1.0);
		}
		else if(m_startPosture == 2)
		{
			l_oRobotRobotPosture.goToPosture("Crouch", 1.0);
		}
        }
        catch (const AL::ALError& e)
        {
            std::cerr << "-ERROR: " << e.what() << std::endl;
            return false;
        }

        std::cout << "end start posture " << std::endl;

	//~ std::cout << "stiffness activaded (Head, leftArm, rightArm): " << m_bHeadActivated << " " <<  m_bLeftArmActivated << " " << m_bRightArmActivated << std::endl;

        // set stiffnesses
        //~ if(m_bHeadActivated)
        //~ {
		//~ m_oRobotMotionProxy->setStiffnesses("Head", 1.0f);
        //~ }
        //~ if(m_bLeftArmActivated)
        //~ {
		//~ m_oRobotMotionProxy->setStiffnesses("LArm", 1.0f);
        //~ }
        //~ if(m_bRightArmActivated)
        //~ {
		//~ m_oRobotMotionProxy->setStiffnesses("RArm", 1.0f);
        //~ }

        //~ std::cout << "end stiffness activaded (Head, leftArm, rightArm): " << m_bHeadActivated << " " << m_bLeftArmActivated << " " << m_bRightArmActivated << std::endl;

	return true;
}


bool SWTeleoperation_pepper::interruptModule()
{
	m_oHeadTrackerPort.interrupt();
	m_oLeftArmTrackerPort.interrupt();
	m_oRightArmTrackerPort.interrupt();
	
	std::cout << "--Interrupting the pepper Teleoperation module..." << std::endl;
	return true;
}

void SWTeleoperation_pepper::resetHeadPosition()
{
	if(!m_bHeadActivated)
	{
		return;
	}

	m_aHeadAngles[0] = 0.f;
	m_aHeadAngles[1] = 0.f;

	try
	{
		m_oRobotMotionProxy->setStiffnesses("Head",1.0f);
			m_oRobotMotionProxy->setAngles(AL::ALValue("Head"), m_aHeadAngles, static_cast<float>(m_dJointVelocityValue));
		m_oRobotMotionProxy->setStiffnesses("Head",0.0f);
	}
	catch (const AL::ALError& e)
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
	}
}


void SWTeleoperation_pepper::resetLeftArmPosition()
{
	if(!m_bLeftArmActivated)
	{
		return;
	}

	// TO CHECK
	//~ m_aLArmAngles[0] = 0.0f;
	//~ m_aLArmAngles[1] = 0.f;
	//~ m_aLArmAngles[2] = 0.f;
	//~ m_aLArmAngles[3] = 0.f;
	m_aLArmAngles[4] = 0.f;
	m_aLArmAngles[5] = 0.f;
	
	try 
	{
		//~ m_oRobotMotionProxy->setAngles(AL::ALValue("LArm"), m_aLArmAngles, static_cast<float>(m_dJointVelocityValue));
		
		//~ m_oRobotMotionProxy->setStiffnesses("LHand",1.0f);
			m_oRobotMotionProxy->setAngles(AL::ALValue("LHand"), m_aLArmAngles[5], static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("LHand",0.0f);
		
		
		//~ m_oRobotMotionProxy->setStiffnesses("LWristYaw",1.0f);
			m_oRobotMotionProxy->setAngles(AL::ALValue("LWristYaw"), m_aLArmAngles[4], static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("LWristYaw",0.0f);
		
	}
	catch (const AL::ALError& e) 
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
	}

}

void SWTeleoperation_pepper::resetRightArmPosition()
{
	if(!m_bRightArmActivated)
	{
		return;
	}

	//  TO CHECK
	//~ m_aRArmAngles[0] = 0.0f;
	//~ m_aRArmAngles[1] = 0.f;
	//~ m_aRArmAngles[2] = 0.f;
	//~ m_aRArmAngles[3] = 0.f;
	m_aRArmAngles[4] = 0.f;
	m_aRArmAngles[5] = 0.f;
  
	try 
	{
		//~ m_oRobotMotionProxy->setStiffnesses("RArm",1.0f);
		//~ m_oRobotMotionProxy->setAngles(AL::ALValue("RArm"), m_aRArmAngles, static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("RHand",1.0f);
			m_oRobotMotionProxy->setAngles(AL::ALValue("RHand"), m_aRArmAngles[5], static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("RHand",0.0f);
		
		//~ m_oRobotMotionProxy->setStiffnesses("RWristYaw",1.0f);
			m_oRobotMotionProxy->setAngles(AL::ALValue("RWristYaw"), m_aRArmAngles[4], static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("RWristYaw",0.0f);
	}
	catch (const AL::ALError& e) 
	{
		std::cerr << "Caught exception: " << e.what() << std::endl;
	}

}

bool SWTeleoperation_pepper::close()
{
	resetHeadPosition();
	resetLeftArmPosition();
	resetRightArmPosition();

	// close ports
	m_oHeadTrackerPort.close();
	m_oLeftArmTrackerPort.close();
	m_oRightArmTrackerPort.close();
	
	// set stiffnesses
	//~ if(m_bHeadActivated)
	//~ {
		//~ m_oRobotMotionProxy->setStiffnesses("Head", 0.0f);
	//~ }
	//~ if(m_bLeftArmActivated)
	//~ {
		//~ m_oRobotMotionProxy->setStiffnesses("LArm", 0.0f);
	//~ }
	//~ if(m_bRightArmActivated)
	//~ {
		//~ m_oRobotMotionProxy->setStiffnesses("RArm", 0.0f);
	//~ }

	// set ending posture
	ALRobotPostureProxy l_oRobotRobotPosture=  ALRobotPostureProxy(m_sRobotAddress);
	try
	{
		if(m_endPosture == 0)
		{
			l_oRobotRobotPosture.goToPosture("Stand", 1.0);
		}
		else if(m_endPosture == 1)
		{
			l_oRobotRobotPosture.goToPosture("StandZero", 1.0);
		}
		else if(m_endPosture == 2)
		{
			l_oRobotRobotPosture.goToPosture("Crouch", 1.0);
		}
	}
	catch (const AL::ALError& e)
	{
		std::cerr << "-ERROR: " << e.what() << std::endl;
		deleteAndNullify(m_oRobotMotionProxy);
		return false;
	}

	deleteAndNullify(m_oRobotMotionProxy);

	return true;
}

void SWTeleoperation_pepper::computeHandAngles(yarp::os::Bottle* handBottle,std::vector<double> &vHandAngles, bool leftArm)
{
	vHandAngles = std::vector<double>(4,0.);

	// retrieve leap data
        std::vector<double> l_vArmDirection(3,0.), l_vHandDirection(3,0.),l_vHandDirectionE(3,0.), l_vHandPalmCoord(3,0.), l_vHandPalmNormal(3,0.), l_vHandPalmNormalE(3,0.);
        for(int ii = 0; ii < 3; ++ii)
        {
		l_vArmDirection[ii]     = handBottle->get(1 + ii).asDouble();
		l_vHandDirection[ii]    = handBottle->get(4 + ii).asDouble();
		l_vHandDirectionE[ii]   = handBottle->get(7 + ii).asDouble();
		l_vHandPalmCoord[ii]    = handBottle->get(10 + ii).asDouble();
		l_vHandPalmNormal[ii]   = handBottle->get(13 + ii).asDouble();
		l_vHandPalmNormalE[ii]  = handBottle->get(16 + ii).asDouble();
        }

	// convert to vec3D
        cv::Vec3d l_vecHandPalmNormal(l_vHandPalmNormal[0], l_vHandPalmNormal[1], l_vHandPalmNormal[2]);
        cv::Vec3d l_vecHandPalmCoord(l_vHandPalmCoord[0], l_vHandPalmCoord[1], l_vHandPalmCoord[2]);
        cv::Vec3d l_vecHandDirection(l_vHandDirection[0], l_vHandDirection[1], l_vHandDirection[2]);
        cv::Vec3d l_vecArmDirection(l_vArmDirection[0], l_vArmDirection[1], l_vArmDirection[2]);

	// normalize vectors
        l_vecHandPalmNormal = cv::normalize(l_vecHandPalmNormal);
        l_vecArmDirection   = cv::normalize(l_vecArmDirection);
        l_vecHandDirection  = cv::normalize(l_vecHandDirection);

	// convert to mat
        cv::Mat l_matHandDirection(l_vecHandDirection);
        cv::Mat l_matHandPalmNormal(l_vecHandPalmNormal);
        cv::Mat l_matArmDirection(l_vecArmDirection);

	// check hand palm orientation
        bool l_bHandPalmUp = false;
        if(swUtil::rad2Deg(acos(cv::Vec3d(0.,-1.,0.).dot(l_vecHandPalmNormal))) > 90.)
        {
		l_bHandPalmUp = true;
        }

	// compute transformation for aligning arm to z axis
        cv::Mat l_matTransfo;
        cv::Vec3d l_vecAxis(0.,0.,-1.);
        swUtil::rodriguesRotation(l_vecArmDirection, l_vecAxis, l_matTransfo);
	// apply transformation to the arm and the hand
        cv::Mat l_matTransfoHandDirection = l_matTransfo * l_matHandDirection;
        cv::Mat l_matTransfoHandNormal    = l_matTransfo * l_matHandPalmNormal;
        cv::Mat l_matTransfoArmDirection  = l_matTransfo * l_matArmDirection;

	// compute transformation for aligning palm normal to Y axis
        if(!l_bHandPalmUp)
        {
		l_vecAxis = cv::Vec3d(0.,-1.,0.);
        }
        else
        {
		l_vecAxis = cv::Vec3d(0.,1.,0.);
        }

        cv::Vec3d l_vecTransfoHandNormal(l_matTransfoHandNormal);
        swUtil::rodriguesRotation(l_vecTransfoHandNormal, l_vecAxis, l_matTransfo);

        cv::Mat l_matTransfoHandDirection2 = l_matTransfo * l_matTransfoHandDirection;

	// compute angle for wrist yaw
        double l_dot  = l_matTransfoHandDirection2.dot(l_matTransfoArmDirection);
        double l_angle = swUtil::rad2Deg(acos(l_dot/(cv::norm(l_matTransfoHandDirection2)* cv::norm(l_matTransfoArmDirection))));
        cv::Mat l_matCross = l_matTransfoHandDirection2.cross(l_matTransfoArmDirection);

        double l_dCrossY = l_matCross.at<double>(1);

        if(!leftArm)
        {
		l_dCrossY *= -1;
        }

        if(l_dCrossY > 0.)
        {
		if(!l_bHandPalmUp)
		{
			l_angle = -l_angle;
		}
        }
        else
        {
		if(l_bHandPalmUp)
		{
			l_angle = -l_angle;
		}
        }

        // set joint value
        vHandAngles[3] = l_angle;

	// compute angle for wrist ptich
        cv::Vec3d l_vecTransfoHandDirection(l_matTransfoHandDirection);
        cv::Vec3d l_vecTransfoHandRight = l_vecTransfoHandNormal.cross(l_vecTransfoHandDirection);

        bool l_bHandPalmLeft = true;
        if(swUtil::rad2Deg(acos(cv::Vec3d(-1.,0.,0.).dot(l_vecHandPalmNormal))) > 90.)
        {
		l_bHandPalmLeft = false;
        }

        // compute transformation for aligning palm normal to X axis
        if(l_bHandPalmLeft)
        {
		l_vecAxis = cv::Vec3d(0.,-1.,0.);
        }
        else
        {
		l_vecAxis = cv::Vec3d(0.,1.,0.);
        }

        swUtil::rodriguesRotation(l_vecTransfoHandRight, l_vecAxis, l_matTransfo);
        l_matTransfoHandDirection2 = l_matTransfo * l_matTransfoHandDirection;

        l_dot  = l_matTransfoHandDirection2.dot(l_matTransfoArmDirection);
        l_angle = swUtil::rad2Deg(acos(l_dot/(cv::norm(l_matTransfoHandDirection2)* cv::norm(l_matTransfoArmDirection))));

        l_matCross = l_matTransfoHandDirection2.cross(l_matTransfoArmDirection);

        if(!leftArm)
        {
		l_dCrossY *= -1;
        }


        if(l_dCrossY > 0.)
        {
		if(l_bHandPalmLeft)
		{
			l_angle = -l_angle;
		}
		else
		{
			l_angle = 0.0;
		}
	}
        else
        {
		if(l_bHandPalmLeft)
		{
			l_angle = 0.0;
		}
		else
		{
			l_angle = -l_angle;
		}
        }

        // set joint value
        vHandAngles[2] = l_angle;

        if(!leftArm)
        {
		vHandAngles[1] = (swUtil::rad2Deg(l_vHandPalmNormalE[1]) + 90.0);
        }
        else
        {
		vHandAngles[1] = -(swUtil::rad2Deg(l_vHandPalmNormalE[1]) - 90.0);
        }

        double l_dAngle = swUtil::rad2Deg(acos(cv::normalize(cv::Vec3d(0.0,l_vArmDirection[1],l_vArmDirection[2])).dot(cv::Vec3d(0.0,1.0,0.0))));
        l_dAngle *= -1.0;
        l_dAngle += 140.0;
        vHandAngles[0] = l_dAngle;
}

void SWTeleoperation_pepper::computeFingerAngles(yarp::os::Bottle *handFingersBottle, std::vector<double> &vFingerAngles)
{
	// arm joint 0 hand_finger
	// arm joint 1 thumb_oppose
	// arm joint 2 thumb_proximal
	// arm joint 3 thumb_distal
	// arm joint 4 index_proximal
	// arm joint 5 index_distal
	// arm joint 6 middle_proximal
	// arm joint 7 middle_distal
	// arm joint 8 pinky

	// init res angles
        vFingerAngles = std::vector<double>(9,0.);

	// retrieve leap data
        std::vector<cv::Vec3d> l_vecThumbDirections(3,    cv::Vec3d(0.,0.,0.));
        std::vector<cv::Vec3d> l_vecIndexDirections(4,    cv::Vec3d(0.,0.,0.));
        std::vector<cv::Vec3d> l_vecMiddleDirections(4,   cv::Vec3d(0.,0.,0.));
        std::vector<cv::Vec3d> l_vecRingDirections(4,     cv::Vec3d(0.,0.,0.));
        std::vector<cv::Vec3d> l_vecPinkyDirections(4,    cv::Vec3d(0.,0.,0.));
        cv::Vec3d l_vecHandNormal    = cv::normalize(cv::Vec3d(handFingersBottle->get(13).asDouble(), handFingersBottle->get(14).asDouble(), handFingersBottle->get(15).asDouble()));
        cv::Vec3d l_vecHandDirection = cv::normalize(cv::Vec3d(handFingersBottle->get(4).asDouble(), handFingersBottle->get(5).asDouble(), handFingersBottle->get(6).asDouble()));

        for(int ii = 0; ii < 4; ++ii)
        {
		for(int jj = 0; jj < 3; ++jj)
		{
			if(ii < 3)
			{
				l_vecThumbDirections[ii][jj] = handFingersBottle->get(19 + ii * 3 + jj).asDouble();
			}

			l_vecIndexDirections[ii][jj] = handFingersBottle->get(28 + ii * 3 + jj).asDouble();
			l_vecMiddleDirections[ii][jj] = handFingersBottle->get(40 + ii * 3 + jj).asDouble();
			l_vecRingDirections[ii][jj] = handFingersBottle->get(52 + ii * 3 + jj).asDouble();
			l_vecPinkyDirections[ii][jj] = handFingersBottle->get(64 + ii * 3 + jj).asDouble();
		}

		if(l_vecThumbDirections[ii][0] != 0 && l_vecThumbDirections[ii][1] != 0 && l_vecThumbDirections[ii][2] != 0)
		{
			l_vecThumbDirections[ii]    = cv::normalize(l_vecThumbDirections[ii]);
		}
		if(l_vecIndexDirections[ii][0] != 0 && l_vecIndexDirections[ii][1] != 0 && l_vecIndexDirections[ii][2] != 0)
		{
			l_vecIndexDirections[ii]    = cv::normalize(l_vecIndexDirections[ii]);
		}
		if(l_vecMiddleDirections[ii][0] != 0 && l_vecMiddleDirections[ii][1] != 0 && l_vecMiddleDirections[ii][2] != 0)
		{
			l_vecMiddleDirections[ii]   = cv::normalize(l_vecMiddleDirections[ii]);
		}
		if(l_vecRingDirections[ii][0] != 0 && l_vecRingDirections[ii][1] != 0 && l_vecRingDirections[ii][2] != 0)
		{
			l_vecRingDirections[ii]     = cv::normalize(l_vecRingDirections[ii]);
		}
		if(l_vecPinkyDirections[ii][0] != 0 && l_vecPinkyDirections[ii][1] != 0 && l_vecPinkyDirections[ii][2] != 0)
		{
			l_vecPinkyDirections[ii]    = cv::normalize(l_vecPinkyDirections[ii]);
		}
        }


        std::vector<cv::Mat> l_vMatThumbDirectionsTransfo(3,    cv::Mat(cv::Vec3d(0.,0.,0.)));
        std::vector<cv::Mat> l_vMatIndexDirectionsTransfo(4,    cv::Mat(cv::Vec3d(0.,0.,0.)));
        std::vector<cv::Mat> l_vMatMiddleDirectionsTransfo(4,   cv::Mat(cv::Vec3d(0.,0.,0.)));
        std::vector<cv::Mat> l_vMatRingDirectionsTransfo(4,     cv::Mat(cv::Vec3d(0.,0.,0.)));
        std::vector<cv::Mat> l_vMatPinkyDirectionsTransfo(4,    cv::Mat(cv::Vec3d(0.,0.,0.)));
        cv::Mat l_matHandDirectionTransfo(cv::Vec3d(0.,0.,0.));


	// compute transformation for aligning palm normal to Y axis
        cv::Vec3d l_vecAxis;

        bool l_bHandPalmUp = false;
        if(swUtil::rad2Deg(acos(cv::Vec3d(0.,-1.,0.).dot(l_vecHandNormal))) > 90.)
        {
		l_bHandPalmUp = true;
        }
        bool l_bHandPalmLeft = true;
        if(swUtil::rad2Deg(acos(cv::Vec3d(-1.,0.,0.).dot(l_vecHandNormal))) > 90.)
        {
		l_bHandPalmLeft = false;
        }

        if(!l_bHandPalmUp)
        {
		l_vecAxis = cv::Vec3d(0.,-1.,0.);
        }
        else
        {
		l_vecAxis = cv::Vec3d(0.,1.,0.);
        }


        cv::Mat l_matTransfo;
        swUtil::rodriguesRotation(l_vecHandNormal, l_vecAxis, l_matTransfo);

        for(int ii = 0; ii < 4; ++ii)
        {
		if(ii < 3)
		{
			l_vMatThumbDirectionsTransfo[ii] = l_matTransfo * cv::Mat(l_vecThumbDirections[ii]);
		}

		l_vMatIndexDirectionsTransfo[ii]     = l_matTransfo * cv::Mat(l_vecIndexDirections[ii]);
		l_vMatMiddleDirectionsTransfo[ii]    = l_matTransfo * cv::Mat(l_vecMiddleDirections[ii]);
		l_vMatRingDirectionsTransfo[ii]      = l_matTransfo * cv::Mat(l_vecRingDirections[ii]);
		l_vMatPinkyDirectionsTransfo[ii]     = l_matTransfo * cv::Mat(l_vecPinkyDirections[ii]);
        }

        l_matHandDirectionTransfo = l_matTransfo * cv::Mat(l_vecHandDirection);


	// compute fingers interval (hand_finger)
        // ... better not (hight risk of breaking)

	// compute thumbs angles
        // thumb metacarpal-> index metarcapal (thumb_oppose)
	cv::Vec3d l_vecTemp1(cv::normalize(cv::Vec3d(l_vMatThumbDirectionsTransfo[0]))); // TODO :...
	cv::Vec3d l_vecTemp2(cv::normalize(cv::Vec3d(l_vMatIndexDirectionsTransfo[0])));
	double l_dDot = l_vecTemp1.dot(l_vecTemp2);
	double l_dAngle = swUtil::rad2Deg(acos(l_dDot));
	vFingerAngles[2] = 90.0 - l_dAngle;

        // proximal->intermediate (thumb_proximal)
	l_vecTemp1 = cv::normalize(l_vecThumbDirections[0]);
	l_vecTemp2 = cv::normalize(l_vecThumbDirections[1]);
	l_dDot = l_vecTemp1.dot(l_vecTemp2);
	cv::Vec3d l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle = swUtil::rad2Deg(acos(l_dDot));
	//            vFingerAngles[1] += l_dAngle;

        // intermediate->distal (thumb_distal)
	l_vecTemp1 = cv::normalize(l_vecThumbDirections[1]);
	l_vecTemp2 = cv::normalize(l_vecThumbDirections[2]);
	l_dDot     = l_vecTemp1.dot(l_vecTemp2);
	l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle = swUtil::rad2Deg(acos(l_dDot));
	vFingerAngles[3] += l_dAngle;

	// compute index angles
        // metacarpal->proximal (index_proximal)
	l_vecTemp1 = cv::normalize(l_vecIndexDirections[0]);
	l_vecTemp2 = cv::normalize(l_vecIndexDirections[1]);
	l_dDot     = l_vecTemp1.dot(l_vecTemp2);
	l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle = swUtil::rad2Deg(acos(l_dDot));

	if(!l_bHandPalmLeft)
	{
		if(l_vecCross[1] < 0.)
		{
			vFingerAngles[4] += l_dAngle;
		}
	}
	else
	{
		if(l_vecCross[1] >= 0.)
		{
			vFingerAngles[4] += l_dAngle;
		}
	}

        // proximal->intermediate + intermediate->distal (index_distal)
	l_vecTemp1 = cv::normalize(l_vecIndexDirections[1]);
	l_vecTemp2 = cv::normalize(l_vecIndexDirections[2]);
	l_dDot     = l_vecTemp1.dot(l_vecTemp2);
	l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

	if(!l_bHandPalmLeft)
        {
		if(l_vecCross[1] < 0.)
                {
			vFingerAngles[5] += l_dAngle;
                }
	}
        else
	{
                if(l_vecCross[1] >= 0.)
                {
			vFingerAngles[5] += l_dAngle;
                }
	}

	l_vecTemp1 = cv::normalize(l_vecIndexDirections[2]);
	l_vecTemp2 = cv::normalize(l_vecIndexDirections[3]);
	l_dDot     = l_vecTemp1.dot(l_vecTemp2);
	l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

	if(!l_bHandPalmLeft)
        {
		if(l_vecCross[1] < 0.)
                {
			vFingerAngles[5] += l_dAngle;
                }
	}
        else
        {
                if(l_vecCross[1] >= 0.)
                {
			vFingerAngles[5] += l_dAngle;
                }
	}

	// compute middle angles
        // metacarpal->proximal (middle_proximal)
	l_vecTemp1 = cv::normalize(l_vecMiddleDirections[0]);
	l_vecTemp2 = cv::normalize(l_vecMiddleDirections[1]);
	l_dDot     = l_vecTemp1.dot(l_vecTemp2);
	l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

	if(!l_bHandPalmLeft)
	{
		if(l_vecCross[1] < 0.)
		{
		    vFingerAngles[6] += l_dAngle;
		}
	}
	else
	{
		if(l_vecCross[1] >= 0.)
		{
		    vFingerAngles[6] += l_dAngle;
		}
	}

        // proximal->intermediate + intermediate->distal (middle_distal)
	l_vecTemp1 = cv::normalize(l_vecMiddleDirections[1]);
	l_vecTemp2 = cv::normalize(l_vecMiddleDirections[2]);
	l_dDot     = l_vecTemp1.dot(l_vecTemp2);
	l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

	if(!l_bHandPalmLeft)
	{
		if(l_vecCross[1] < 0.)
		{
		    vFingerAngles[7] += l_dAngle;
		}
	}
	else
	{
		if(l_vecCross[1] >= 0.)
		{
		    vFingerAngles[7] += l_dAngle;
		}
	}

	l_vecTemp1 = cv::normalize(l_vecMiddleDirections[2]);
	l_vecTemp2 = cv::normalize(l_vecMiddleDirections[3]);
	l_dDot     = l_vecTemp1.dot(l_vecTemp2);
	l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

	if(!l_bHandPalmLeft)
	{
		if(l_vecCross[1] < 0.)
		{
			vFingerAngles[7] += l_dAngle;
		}
	}
	else
	{
		if(l_vecCross[1] >= 0.)
		{
			vFingerAngles[7] += l_dAngle;
		}
	}

	// compute ring + pinky angles
        // metacarpal->proximal + proximal->intermediate + intermediate->distal (pinky)
	l_vecTemp1 = cv::normalize(l_vecPinkyDirections[0]);
	l_vecTemp2 = cv::normalize(l_vecPinkyDirections[1]);
	l_dDot     = l_vecTemp1.dot(l_vecTemp2);
	l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

	if(!l_bHandPalmLeft)
	{
		if(l_vecCross[1] < 0.)
		{
		    vFingerAngles[8] += l_dAngle;
		}
	}
	else
	{
		if(l_vecCross[1] >= 0.)
		{
		    vFingerAngles[8] += l_dAngle;
		}
	}

	l_vecTemp1 = cv::normalize(l_vecPinkyDirections[1]);
	l_vecTemp2 = cv::normalize(l_vecPinkyDirections[2]);
	l_dDot     = l_vecTemp1.dot(l_vecTemp2);
	l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

	if(!l_bHandPalmLeft)
	{
		if(l_vecCross[1] < 0.)
		{
		    vFingerAngles[8] += l_dAngle;
		}
	}
	else
	{
		if(l_vecCross[1] >= 0.)
		{
		    vFingerAngles[8] += l_dAngle;
		}
	}

	l_vecTemp1 = cv::normalize(l_vecPinkyDirections[2]);
	l_vecTemp2 = cv::normalize(l_vecPinkyDirections[3]);
	l_dDot     = l_vecTemp1.dot(l_vecTemp2);
	l_vecCross = l_vecTemp1.cross(l_vecTemp2);
	l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

	if(!l_bHandPalmLeft)
	{
		if(l_vecCross[1] < 0.)
		{
			vFingerAngles[8] += l_dAngle;
		}
	}
	else
	{
		if(l_vecCross[1] >= 0.)
		{
			vFingerAngles[8] += l_dAngle;
		}
	}
}


bool SWTeleoperation_pepper::updateModule()
{
	AL::ALValue l_headNames = AL::ALValue::array("HeadYaw", "HeadPitch");
	AL::ALValue l_headAngles = AL::ALValue::array(0.f,0.f);

	AL::ALValue l_rightArmNames = AL::ALValue::array("RShoulderPitch", "RShoulderRoll", "RElbowYaw", "RElbowRoll", "RWristYaw", "RHand");
	AL::ALValue l_rightArmAngles = AL::ALValue::array(0.f,0.f,0.f, 0.f,0.f,0.f);

	AL::ALValue l_leftArmNames = AL::ALValue::array("LShoulderPitch", "LShoulderRoll", "LElbowYaw", "LElbowRoll", "LWristYaw", "LHand");
	AL::ALValue l_lefttArmAngles = AL::ALValue::array(0.f,0.f,0.f, 0.f,0.f,0.f);


	Bottle *l_pHeadTarget = NULL,  *l_pLeftArmTarget = NULL, *l_pRightArmTarget = NULL;

	bool l_bHeadCapture = false,  l_bLeftArmCapture = false, l_bRightArmCapture = false;

	// read head commands
	if(m_bHeadActivated)
	{
		l_pHeadTarget = m_oHeadTrackerPort.read(false);

		if(l_pHeadTarget)
		{
			int l_deviceId = l_pHeadTarget->get(0).asInt();
			switch(l_deviceId)
			{
				case swTracking::FOREST_LIB :
				{                
					l_headAngles[0] = swUtil::deg2rad(-l_pHeadTarget->get(2).asDouble());  // HeadYaw -5?
					l_headAngles[1] = swUtil::deg2rad(l_pHeadTarget->get(1).asDouble() );  // HeadPitch  -5?
				}
				break;
				case swTracking::FASTRAK_LIB :
				{
					l_headAngles[0] = -(l_pHeadTarget->get(1).asDouble()-90.0); //yaw
					l_headAngles[1] = -l_pHeadTarget->get(2).asDouble(); //pitch
					
					m_aHeadAngles[0] = swUtil::deg2rad(-(l_pHeadTarget->get(1).asDouble()-90.0)); //yaw
					m_aHeadAngles[1] =  swUtil::deg2rad(-l_pHeadTarget->get(2).asDouble()); //pitch
				}
				break;
			}

			m_i32HeadTimeLastBottle = 0;
			l_bHeadCapture = true;
		}
		else
		{
			m_i32HeadTimeLastBottle += 1000/m_i32Fps;

			if(m_i32HeadTimeLastBottle > m_i32HeadTimeoutReset)
			{
				m_i32HeadTimeLastBottle = 0;
				resetHeadPosition();
			}
		}
	}

	//read left arm
	if(m_bLeftArmActivated)
	{
		l_pLeftArmTarget = m_oLeftArmTrackerPort.read(false);
		
		if(l_pLeftArmTarget)
		{
			int l_deviceId = l_pLeftArmTarget->get(0).asInt();
			switch(l_deviceId)
			{
				case swTracking::LEAP_LIB :
				{
					std::vector<double> l_vHandAngles;
					computeHandAngles(l_pLeftArmTarget, l_vHandAngles, true);
					
					std::vector<double> l_vFingerAngles;
					computeFingerAngles(l_pLeftArmTarget, l_vFingerAngles);
					// wrist
					m_aLArmAngles[4] = swUtil::deg2rad(l_vHandAngles[1]-90);
					
					double l_dHandClosure =  l_vFingerAngles[4] +l_vFingerAngles[5] + l_vFingerAngles[6] + l_vFingerAngles[7] + l_vFingerAngles[8];
					if (l_dHandClosure> 500.0)
						l_dHandClosure = 500.0;
					if (l_dHandClosure < 30.0)
						l_dHandClosure = 0.0;
					// hand closure
					m_aLArmAngles[5] = 1-l_dHandClosure/500.0;
					
					
					
					std::cout << "wrist, hand = " << m_aLArmAngles[4] << ", " << m_aLArmAngles[5] << std::endl;
					
					//~ std::cout << "l_vFingerAngles = ";
					//~ for (int l_i32vectorSize=0; l_i32vectorSize< l_vFingerAngles.size(); l_i32vectorSize++)
					//~ {
						//~ std::cout << l_vFingerAngles[l_i32vectorSize];
						//~ std::cout << ";  ";
						
					//~ }
					//~ std::cout << std::endl;
					
					
				}
				break;
			}
			
			m_i32LArmTimeLastBottle = 0;
			l_bLeftArmCapture = true;
		}
		else
		{
			m_i32LArmTimeLastBottle += 1000/m_i32Fps;

			if(m_i32LArmTimeLastBottle > m_i32LArmTimeoutReset)
			{
				m_i32LArmTimeLastBottle = 0;
				resetLeftArmPosition();
			}
		}
	}

	//read right arm
	if(m_bRightArmActivated)
	{
		l_pRightArmTarget = m_oRightArmTrackerPort.read(false);
		
		if(l_pRightArmTarget)
		{
			int l_deviceId = l_pRightArmTarget->get(0).asInt();
			switch(l_deviceId)
			{
				case swTracking::LEAP_LIB :
				{
					std::vector<double> l_vHandAngles;
					computeHandAngles(l_pRightArmTarget, l_vHandAngles, false);
					
					std::vector<double> l_vFingerAngles;
					computeFingerAngles(l_pRightArmTarget, l_vFingerAngles);
					
					
					m_aRArmAngles[4] = swUtil::deg2rad(l_vHandAngles[1]-90);
					double l_dHandClosure =  l_vFingerAngles[4] +l_vFingerAngles[5] + l_vFingerAngles[6] + l_vFingerAngles[7] + l_vFingerAngles[8];
					if (l_dHandClosure> 500.0)
						l_dHandClosure = 500.0;
					if (l_dHandClosure < 30.0)
						l_dHandClosure = 0.0;
					
					m_aRArmAngles[5] = 1-l_dHandClosure/500.0;
					
					//~ std::cout << "wrist, hand = " << m_aRArmAngles[4] << ", " << m_aRArmAngles[5] << std::endl;
				}
				break;
			}
			
			m_i32RArmTimeLastBottle = 0;
			l_bRightArmCapture = true;
		}
		else
		{
			m_i32RArmTimeLastBottle += 1000/m_i32Fps;

			if(m_i32RArmTimeLastBottle > m_i32RArmTimeoutReset)
			{
				m_i32RArmTimeLastBottle = 0;
				resetRightArmPosition();
			}
		}
	}

	//check each joint value to ensure it is in the right range, if not crop to the max/min values
	for(int ii = 0; ii < 6; ++ii)
	{
		if(ii < 2)
		{
			if(m_aHeadAngles[ii].getUnionValue().asFloat > static_cast<float>(m_vHeadMaxJoint[ii]))
			{
				m_aHeadAngles[ii] = static_cast<float>(m_vHeadMaxJoint[ii]);
			}
			if(m_aHeadAngles[ii].getUnionValue().asFloat < static_cast<float>(m_vHeadMinJoint[ii]))
			{
				m_aHeadAngles[ii] = static_cast<float>(m_vHeadMinJoint[ii]);
			}
		}

		if(m_aLArmAngles[ii].getUnionValue().asFloat > static_cast<float>(m_vLeftArmMaxJoint[ii]))
		{
			m_aLArmAngles[ii] = static_cast<float>(m_vLeftArmMaxJoint[ii]);
		}
		if(m_aLArmAngles[ii].getUnionValue().asFloat < static_cast<float>(m_vLeftArmMinJoint[ii]))
		{
			m_aLArmAngles[ii] = static_cast<float>(m_vLeftArmMinJoint[ii]);
		}
		if(m_aRArmAngles[ii].getUnionValue().asFloat > static_cast<float>(m_vRightArmMaxJoint[ii]))
		{
			m_aRArmAngles[ii] = static_cast<float>(m_vRightArmMaxJoint[ii]);
		}
		if(m_aRArmAngles[ii].getUnionValue().asFloat < static_cast<float>(m_vRightArmMinJoint[ii]))
		{
			m_aRArmAngles[ii] = static_cast<float>(m_vRightArmMinJoint[ii]);
		}
	}

	

	if (l_bHeadCapture)
	{
		//~ m_oRobotMotionProxy->setStiffnesses("Head",1.0f);
			m_oRobotMotionProxy->setAngles(AL::ALValue("Head"), m_aHeadAngles, static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("Head",0.0f);
	}

	if (l_bLeftArmCapture)
	{
		//~ m_oRobotMotionProxy->setAngles(AL::ALValue("LArm"), m_aLArmAngles, static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("LHand",1.0f);
			AL::ALValue lHandClosure = m_aLArmAngles[5];
			m_oRobotMotionProxy->setAngles(AL::ALValue("LHand"), lHandClosure, static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("LHand",0.0f);
			
		//~ m_oRobotMotionProxy->setStiffnesses("LWristYaw",1.0f);
			AL::ALValue lWristYaw = m_aLArmAngles[4];
			m_oRobotMotionProxy->setAngles(AL::ALValue("LWristYaw"), lWristYaw, static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("LWristYaw",0.0f);
	}

	if (l_bRightArmCapture)
	{       
		//~ m_oRobotMotionProxy->setAngles(AL::ALValue("RArm"), m_aRArmAngles, static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("RHand",1.0f);
			m_oRobotMotionProxy->setAngles(AL::ALValue("RHand"), m_aRArmAngles[5], static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("RHand",0.0f);
		
		//~ m_oRobotMotionProxy->setStiffnesses("RWristYaw",1.0f);
			m_oRobotMotionProxy->setAngles(AL::ALValue("RWristYaw"), m_aRArmAngles[4], static_cast<float>(m_dJointVelocityValue));
		//~ m_oRobotMotionProxy->setStiffnesses("RWristYaw",0.0f);
	}

	return true;
}


double SWTeleoperation_pepper::getPeriod()
{
	return 1./m_i32Fps;
}

int main(int argc, char* argv[])
{
	// initialize yarp network
	yarp::os::Network yarp;
	if (!yarp.checkNetwork())
	{
		std::cerr << "-ERROR: Problem connecting to YARP server" << std::endl;
		return -1;
	}

	// create your module
	SWTeleoperation_pepper l_oTeleoperation_pepper;

	// prepare and configure the resource finder
	yarp::os::ResourceFinder l_oRf;
		l_oRf.setVerbose(true);
		l_oRf.setDefaultConfigFile("teleoperation_pepper.ini");
		l_oRf.setDefaultContext("swooz-teleoperation/conf");
		l_oRf.configure("PEPPER_ROOT", argc, argv);

	// configure the module
	std::cout << "Configuring the pepper Teleoperation module..."<< std::endl;
	if (l_oTeleoperation_pepper.configure(l_oRf))
	{
		// run the module
		std::cout << "Starting the pepper Teleoperation module..." << std::endl;
		l_oTeleoperation_pepper.runModule();
	}
	else
	{
		std::cerr << "Failed to configure the pepper Teleoperation module!"<< std::endl;
	}

	return 0;
}

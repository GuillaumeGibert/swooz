/**
 * \file SWTeleoperation_reeti.cpp
 * \author Guillaume Gibert
 * \date 16/03/2015
 * \brief ...
 */

#include "reeti/SWTeleoperation_reeti.h"

#include "SWTrackingDevice.h"

#include <sstream>
#include <vector>
#include <cmath>
#include <iostream>


#include "opencv2/core/core.hpp"
#include "opencvUtility.h"
#include "geometryUtility.h"

SWTeleoperation_reeti::SWTeleoperation_reeti() :  m_i32HeadTimeLastBottle(0)
{    
	m_bHeadActivatedDefault     = true;
	m_bFaceActivatedDefault	= false;
	m_bGazeActivatedDefault     = false;
	m_bLEDSActivatedDefault     = false;   
}

SWTeleoperation_reeti::~SWTeleoperation_reeti()
{}

bool SWTeleoperation_reeti::configure(ResourceFinder &oRf)
{
	// gets the module name which will form the stem of all module port names
        m_sModuleName   = oRf.check("name", Value("teleoperation_reeti"), "Teleoperation/Reeti Module name (string)").asString();
	m_sRobotName    = oRf.check("robot",Value("reeti"),  "Robot name (string)").asString();
        setName(m_sModuleName.c_str());
        m_sRobotAddress = oRf.check("IP", Value("127.0.0.1"), "IP Adress of the reeti Robot").asString();
	m_i32RobotPort = oRf.check("Port", yarp::os::Value(54001), "IP Port of the reeti Robot").asInt();

	// robot parts to control
        m_bHeadActivated    = oRf.check("headActivated",yarp::os::Value(m_bHeadActivatedDefault), "Head activated (int)").asInt() != 0;
	m_bFaceActivated    = oRf.check("faceActivated",yarp::os::Value(m_bFaceActivatedDefault), "Face activated (int)").asInt() != 0;
	m_bGazeActivated    = oRf.check("gazeActivated",yarp::os::Value(m_bGazeActivatedDefault), "Gaze activated (int)").asInt() != 0;
        m_bLEDSActivated    = oRf.check("LEDSActivated",yarp::os::Value(m_bLEDSActivatedDefault), "LEDS activated (int)"). asInt() != 0;
       
	// neckRotat
	m_dNeckRotatMinValueJoint = oRf.check("neckRotaMinValueJoint",  yarp::os::Value(0.0f), "Neck Rotat minimum Joint Value (float)").asDouble();
	m_dNeckRotatMaxValueJoint = oRf.check("neckRotaMaxValueJoint",  yarp::os::Value(100.0f), "Neck Rotat maximum Joint Value (float)").asDouble();
	m_dNeckRotatNeuValueJoint = oRf.check("neckRotaNeuValueJoint",  yarp::os::Value(50.0f), "Neck Rotat neutral Joint Value (float)").asDouble();
	// neckTilt
	m_dNeckTiltMinValueJoint = oRf.check("neckTiltMinValueJoint",  yarp::os::Value(0.0f), "Neck Tilt minimum Joint Value (float)").asDouble();
	m_dNeckTiltMaxValueJoint = oRf.check("neckTiltMaxValueJoint",  yarp::os::Value(100.0f), "Neck Tilt maximum Joint Value (float)").asDouble();
	m_dNeckTiltNeuValueJoint = oRf.check("neckTiltNeuValueJoint",  yarp::os::Value(55.0f), "Neck Tilt neutral Joint Value (float)").asDouble();
        // neckPan
	m_dNeckPanMinValueJoint = oRf.check("neckPanMinValueJoint",  yarp::os::Value(0.0f), "Neck Pan minimum Joint Value (float)").asDouble();
	m_dNeckPanMaxValueJoint = oRf.check("neckPanMaxValueJoint",  yarp::os::Value(100.0f), "Neck Pan maximum Joint Value (float)").asDouble();
	m_dNeckPanNeuValueJoint = oRf.check("neckPanNeuValueJoint",  yarp::os::Value(50.0f), "Neck Pan neutral Joint Value (float)").asDouble();
	// leftLC
	m_dLeftLCMinValueJoint = oRf.check("leftLCMinValueJoint",  yarp::os::Value(0.0f), "Left LC minimum Joint Value (float)").asDouble();
	m_dLeftLCMaxValueJoint = oRf.check("leftLCMaxValueJoint",  yarp::os::Value(100.0f), "Left LC maximum Joint Value (float)").asDouble();
	m_dLeftLCNeuValueJoint = oRf.check("leftLCNeuValueJoint",  yarp::os::Value(43.8f), "Left LC neutral Joint Value (float)").asDouble();
	// rightLC
	m_dRightLCMinValueJoint = oRf.check("rightLCMinValueJoint",  yarp::os::Value(0.0f), "Right LC minimum Joint Value (float)").asDouble();
	m_dRightLCMaxValueJoint = oRf.check("rightLCMaxValueJoint",  yarp::os::Value(100.0f), "Right LC maximum Joint Value (float)").asDouble();
	m_dRightLCNeuValueJoint = oRf.check("rightLCNeuValueJoint",  yarp::os::Value(56.1f), "Right LC neutral Joint Value (float)").asDouble();
	// topLip
	m_dTopLipMinValueJoint = oRf.check("topLipMinValueJoint",  yarp::os::Value(0.0f), "Top Lip minimum Joint Value (float)").asDouble();
	m_dTopLipMaxValueJoint = oRf.check("topLipMaxValueJoint",  yarp::os::Value(100.0f), "Top Lip maximum Joint Value (float)").asDouble();
	m_dTopLipNeuValueJoint = oRf.check("topLipNeuValueJoint",  yarp::os::Value(50.0f), "Top Lip neutral Joint Value (float)").asDouble();
	// bottomLip
	m_dBottomLipMinValueJoint = oRf.check("bottomLipMinValueJoint",  yarp::os::Value(0.0f), "Bottom Lip minimum Joint Value (float)").asDouble();
	m_dBottomLipMaxValueJoint = oRf.check("bottomLipMaxValueJoint",  yarp::os::Value(100.0f), "Bottom Lip maximum Joint Value (float)").asDouble();
	m_dBottomLipNeuValueJoint = oRf.check("bottomLipNeuValueJoint",  yarp::os::Value(81.0f), "Bottom Lip neutral Joint Value (float)").asDouble();
	// rightEar
	m_dRightEarMinValueJoint = oRf.check("rightEarMinValueJoint",  yarp::os::Value(0.0f), "Right Ear minimum Joint Value (float)").asDouble();
	m_dRightEarMaxValueJoint = oRf.check("rightEarMaxValueJoint",  yarp::os::Value(100.0f), "Right Ear maximum Joint Value (float)").asDouble();
	m_dRightEarNeuValueJoint = oRf.check("rightEarNeuValueJoint",  yarp::os::Value(80.0f), "Right Ear neutral Joint Value (float)").asDouble();
	// leftEar
	m_dLeftEarMinValueJoint = oRf.check("lefttEarMinValueJoint",  yarp::os::Value(0.0f), "Left Ear minimum Joint Value (float)").asDouble();
	m_dLeftEarMaxValueJoint = oRf.check("leftEarMaxValueJoint",  yarp::os::Value(100.0f), "Left Ear maximum Joint Value (float)").asDouble();
	m_dLeftEarNeuValueJoint = oRf.check("leftEarNeuValueJoint",  yarp::os::Value(80.0f), "Left Ear neutral Joint Value (float)").asDouble();
	// rightEyeTilt
	m_dRightEyeTiltMinValueJoint = oRf.check("rightEyeTiltMinValueJoint",  yarp::os::Value(0.0f), "Right Eye Tilt minimum Joint Value (float)").asDouble();
	m_dRightEyeTiltMaxValueJoint = oRf.check("rightEyeTiltMaxValueJoint",  yarp::os::Value(100.0f), "Right Eye Tilt maximum Joint Value (float)").asDouble();
	m_dRightEyeTiltNeuValueJoint = oRf.check("rightEyeTiltNeuValueJoint",  yarp::os::Value(60.0f), "Right Eye Tilt neutral Joint Value (float)").asDouble();
	// leftEyeTilt
	m_dLeftEyeTiltMinValueJoint = oRf.check("lefttEyeTiltMinValueJoint",  yarp::os::Value(0.0f), "Left Eye Tilt minimum Joint Value (float)").asDouble();
	m_dLeftEyeTiltMaxValueJoint = oRf.check("leftEyeTiltMaxValueJoint",  yarp::os::Value(100.0f), "Left Eye Tilt maximum Joint Value (float)").asDouble();
	m_dLeftEyeTiltNeuValueJoint = oRf.check("leftEyeTiltNeuValueJoint",  yarp::os::Value(40.0f), "Left Eye Tilt neutral Joint Value (float)").asDouble();
	// rightEyePan
	m_dRightEyePanMinValueJoint = oRf.check("rightEyePanMinValueJoint",  yarp::os::Value(0.0f), "Right Eye Pan minimum Joint Value (float)").asDouble();
	m_dRightEyePanMaxValueJoint = oRf.check("rightEyePanMaxValueJoint",  yarp::os::Value(100.0f), "Right Eye Pan maximum Joint Value (float)").asDouble();
	m_dRightEyePanNeuValueJoint = oRf.check("rightEyePanNeuValueJoint",  yarp::os::Value(42.5f), "Right Eye Pan neutral Joint Value (float)").asDouble();
	// leftEyePan
	m_dLeftEyePanMinValueJoint = oRf.check("lefttEyePanMinValueJoint",  yarp::os::Value(0.0f), "Left Eye Pan minimum Joint Value (float)").asDouble();
	m_dLeftEyePanMaxValueJoint = oRf.check("leftEyePanMaxValueJoint",  yarp::os::Value(100.0f), "Left Eye Pan maximum Joint Value (float)").asDouble();
	m_dLeftEyePanNeuValueJoint = oRf.check("leftEyePanNeuValueJoint",  yarp::os::Value(42.5f), "Left Eye Pan neutral Joint Value (float)").asDouble();
	// rightEyeLid
	m_dRightEyeLidMinValueJoint = oRf.check("rightEyeLidMinValueJoint",  yarp::os::Value(0.0f), "Right Eye Lid minimum Joint Value (float)").asDouble();
	m_dRightEyeLidMaxValueJoint = oRf.check("rightEyeLidMaxValueJoint",  yarp::os::Value(100.0f), "Right Eye Lid maximum Joint Value (float)").asDouble();
	m_dRightEyeLidNeuValueJoint = oRf.check("rightEyeLidNeuValueJoint",  yarp::os::Value(90.0f), "Right Eye Lid neutral Joint Value (float)").asDouble();
	// leftEyeLid
	m_dLeftEyeLidMinValueJoint = oRf.check("lefttEyeLidMinValueJoint",  yarp::os::Value(0.0f), "Left Eye Lid minimum Joint Value (float)").asDouble();
	m_dLeftEyeLidMaxValueJoint = oRf.check("leftEyeLidMaxValueJoint",  yarp::os::Value(100.0f), "Left Eye Lid maximum Joint Value (float)").asDouble();
	m_dLeftEyeLidNeuValueJoint = oRf.check("leftEyeLidNeuValueJoint",  yarp::os::Value(90.0f), "Left Eye Lid neutral Joint Value (float)").asDouble();
	
	// miscellaneous
        m_i32Fps                    	= oRf.check("fps",              yarp::os::Value(30),  "Frame per second (int)").asInt();
        m_i32HeadTimeoutReset      	= oRf.check("headTimeoutReset", yarp::os::Value(3000), "Head gaze timeout reset Reeti (int)").asInt();

	// init ports
        std::string l_sHeadTrackerPortName  = "/teleoperation/" + m_sRobotName + "/head";
	std::string l_sGazeTrackerPortName  = "/teleoperation/" + m_sRobotName + "/gaze";
        std::string l_sFaceTrackerPortName  = "/teleoperation/" + m_sRobotName + "/face";
	     

        if(	!m_oHeadTrackerPort.open(l_sHeadTrackerPortName.c_str()) ||
		!m_oFaceTrackerPort.open(l_sFaceTrackerPortName.c_str()) ||
		!m_oGazeTrackerPort.open(l_sGazeTrackerPortName.c_str())
	)
        {
            std::cerr << "-ERROR: Unable to open ports." << std::endl;
            interruptModule();
            return false;
        }

	// creates an urbi client to send commands to the Reeti robot
	client=new UClient(m_sRobotAddress, m_i32RobotPort);
     
	return true;
}


bool SWTeleoperation_reeti::interruptModule()
{
	m_oHeadTrackerPort.interrupt();
	m_oFaceTrackerPort.interrupt();
	m_oGazeTrackerPort.interrupt();

	std::cout << "--Interrupting the reeti Teleoperation module..." << std::endl;
	return true;
}

void SWTeleoperation_reeti::resetPosition()
{
	if(m_bHeadActivated)
	{
		string cmd = "Global.servo.neckRotat(" + m_dNeckRotatNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.neckTilt(" + m_dNeckTiltNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.neckPan(" + m_dNeckPanNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
	}
	
	if(m_bFaceActivated)
	{
		string cmd = "Global.servo.LeftLC(" + m_dLeftLCNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.rightLC(" + m_dRightLCNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.topLip(" + m_dTopLipNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.bottomLip(" + m_dBottomLipNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
	}
	
	if(m_bGazeActivated)
	{
		string cmd = "Global.servo.rightEyeTilt(" + m_dRightEyeTiltNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.leftEyeTilt(" + m_dLeftEyeTiltNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.rightEyePan(" + m_dRightEyePanNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.leftEyePan(" + m_dLeftEyePanNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.rightEyeLid(" + m_dRightEyeLidNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.leftEyeLid(" + m_dLeftEyeLidNeuValueJoint + ");"; 
		client->send("%s", cmd.c_str());
	}
	
	// TODO: to add switch off the led
}


bool SWTeleoperation_reeti::close()
{
	resetPosition();

	// close yarp ports
	m_oHeadTrackerPort.close();
	m_oFaceTrackerPort.close();
	m_oGazeTrackerPort.close();
  
	// close urbi client
	client->close();
	
	std::cout << "--Closing the reeti Teleoperation module..." << std::endl;
	return true;
}


bool SWTeleoperation_reeti::updateModule()
{
	// init command vector
        yarp::sig::Vector l_vHeadJoints;
        l_vHeadJoints.resize(3); // Rotation, Pan, Tilt
        l_vHeadJoints = 0.;
	yarp::sig::Vector l_vGazeJoints;
        l_vGazeJoints.resize(6); // rightEyeX, Y, leftEyeX, Y, rightEyeLid, leftEyeLid
        l_vHeadJoints = 0.;
	
	// defines bottles
	Bottle *l_pHeadTarget = NULL, *l_pFaceTarget = NULL, *l_pGazeTarget = NULL;
	
	// defines joint values
	double l_dNeckRotatValueJoint, l_dNeckTiltValueJoint, l_dNeckPanValueJoint;
	double l_dLeftLCValueJoint, l_dRightLCValueJoint, l_dTopLipValueJoint, l_dBottomLipValueJoint;
	double l_dRightEarValueJoint, l_dLeftEarValueJoint;
	double l_dRightEyeTiltValueJoint, l_dLeftEyeTiltValueJoint, l_dRightEyePanValueJoint, l_dLeftEyePanValueJoint;
	double l_dRightEyeLidValueJoint, l_dLeftEyeLidValueJoint;
	
	// read head commands
	if(m_bHeadActivated)
	{
		l_pHeadTarget = m_oHeadTrackerPort.read(false);

		if(l_pHeadTarget)
		{
			int l_deviceId = l_pHeadTarget->get(0).asInt();
			
			switch(l_deviceId)
			{
				case swTracking::DUMMY_LIB :
				{
					l_vHeadJoints[0] = l_pHeadTarget->get(1).asDouble()+50;
					l_vHeadJoints[1] = l_pHeadTarget->get(2).asDouble()+50;
					l_vHeadJoints[2] = l_pHeadTarget->get(3).asDouble()+50;
				}
				break;
				case swTracking::FASTRAK_LIB :
				{
					l_vHeadJoints[0] = l_pHeadTarget->get(2).asDouble()+50;
					l_vHeadJoints[1] = l_pHeadTarget->get(3).asDouble()+50;
					l_vHeadJoints[2] = -(l_pHeadTarget->get(1).asDouble()-90.0)+50;
				}
				break;
				case swTracking::FOREST_LIB :
				{
					l_vHeadJoints[0] = -l_pHeadTarget->get(1).asDouble()+50; //head rotation "yes" 
					l_vHeadJoints[1] = -l_pHeadTarget->get(3).asDouble()+50; //head rotation
					l_vHeadJoints[2] = -l_pHeadTarget->get(2).asDouble()+50; //head rotation "no"
				}
				break;
				case swTracking::COREDATA_LIB :
				{
					l_vHeadJoints[0] =  swUtil::rad2Deg(l_pHeadTarget->get(4).asDouble())+50; //head rotation "yes" 
					l_vHeadJoints[1] = -swUtil::rad2Deg(l_pHeadTarget->get(6).asDouble())+50; //head rotation
					l_vHeadJoints[2] =  swUtil::rad2Deg(l_pHeadTarget->get(5).asDouble())+50; //head rotation "no"
				}
				break;
				case swTracking::EMICP_LIB :
				{
					l_vHeadJoints[0] = -l_pHeadTarget->get(4).asDouble()+50; 
					l_vHeadJoints[1] = -l_pHeadTarget->get(6).asDouble()+50; 
					l_vHeadJoints[2] = -l_pHeadTarget->get(5).asDouble()+50;
				}
				break;
				case swTracking::FACESHIFT_LIB :
				{
					l_vHeadJoints[0] = -swUtil::rad2Deg(l_pHeadTarget->get(4).asDouble())+50; 
					l_vHeadJoints[1] = swUtil::rad2Deg(l_pHeadTarget->get(6).asDouble())+50;
					l_vHeadJoints[2] = swUtil::rad2Deg(l_pHeadTarget->get(5).asDouble())+50;
				}
				break;
				case swTracking::OPENNI_LIB :
				{
					std::vector<double> l_pointNeck(3), l_pointHead(3), l_pointLShoulder(3), l_pointRShoulder(3);
					l_pointNeck[0] = l_pHeadTarget->get(1).asDouble();
					l_pointNeck[1] = l_pHeadTarget->get(2).asDouble();
					l_pointNeck[2] = l_pHeadTarget->get(3).asDouble();
					l_pointHead[0] = l_pHeadTarget->get(4).asDouble();
					l_pointHead[1] = l_pHeadTarget->get(5).asDouble();
					l_pointHead[2] = l_pHeadTarget->get(6).asDouble();
					l_pointLShoulder[0] = l_pHeadTarget->get(7).asDouble();
					l_pointLShoulder[1] = l_pHeadTarget->get(8).asDouble();
					l_pointLShoulder[2] = l_pHeadTarget->get(9).asDouble();
					l_pointRShoulder[0] = l_pHeadTarget->get(10).asDouble();
					l_pointRShoulder[1] = l_pHeadTarget->get(11).asDouble();
					l_pointRShoulder[2] = l_pHeadTarget->get(12).asDouble();

					std::vector<double> l_vecClavicles  = swUtil::vec(l_pointLShoulder,	l_pointRShoulder);
					std::vector<double> l_vecHead       = swUtil::vec(l_pointNeck,		l_pointHead);
					std::vector<double> l_rpyHead = swUtil::computeRollPitchYaw(l_vecHead, l_vecClavicles);

					l_vHeadJoints[0] = -l_rpyHead[1]+50;
					l_vHeadJoints[1] = -l_rpyHead[0]+50;
					l_vHeadJoints[2] =  l_rpyHead[2]+50;
				}
				break;
			}
			
			// fills the joint variables
			l_dNeckRotatValueJoint = l_vHeadJoints[2];
			l_dNeckTiltValueJoint = l_vHeadJoints[0];
			l_dNeckPanValueJoint = l_vHeadJoints[1];

			m_i32HeadTimeLastBottle = 0;
		}
		else  // manage timeout and reset position
		{
			m_i32HeadTimeLastBottle += 1000/m_i32Fps;

			if(m_i32HeadTimeLastBottle > m_i32HeadTimeoutReset)
			{
				m_i32HeadTimeLastBottle = 0;
				resetPosition();
			}
		}
	}

   
	if(m_bGazeActivated)
	{
		l_pGazeTarget = m_oGazeTrackerPort.read(false);

		if(l_pGazeTarget)
		{
		    int l_deviceId = l_pGazeTarget->get(0).asInt();

			switch(l_deviceId)
			{
				case swTracking::FACESHIFT_LIB :
				{
					//~ l_vGazeJoints[3] = -(l_pGazeTarget->get(1).asDouble() + l_pGazeTarget->get(3).asDouble())*0.5; // up/down eye [-35; +15]
					//~ l_vGazeJoints[4] = -(l_pGazeTarget->get(2).asDouble() + l_pGazeTarget->get(4).asDouble())*0.5; // version angle [-50; 52] = (L+R)/2
					//~ l_vGazeJoints[5] =  -l_pGazeTarget->get(2).asDouble() + l_pGazeTarget->get(4).asDouble();     // vergence angle [0 90] = R-L
					
					l_vGazeJoints[0] =  l_pGazeTarget->get(2).asDouble() +50; //rightEyeX
					l_vGazeJoints[1] =  l_pGazeTarget->get(1).asDouble() +50; //rightEyeY
					l_vGazeJoints[2] =  l_pGazeTarget->get(4).asDouble() +50; //leftEyeX
					l_vGazeJoints[3] =  l_pGazeTarget->get(3).asDouble() +50; //leftEyeY
					// TODO add the blink information
					l_vGazeJoints[4] =  l_pGazeTarget->get(5).asDouble(); //rightEyeLid
					l_vGazeJoints[5] =  l_pGazeTarget->get(6).asDouble(); //leftEyeLid
					
				}
				break;
				case swTracking::DUMMY_LIB :
				{
					// eye position
					l_vGazeJoints[0] =  l_pGazeTarget->get(1).asDouble(); //rightEyeX
					l_vGazeJoints[1] =  l_pGazeTarget->get(2).asDouble(); //rightEyeY
					l_vGazeJoints[2] =  l_pGazeTarget->get(3).asDouble(); //leftEyeX
					l_vGazeJoints[3] =  l_pGazeTarget->get(4).asDouble(); //leftEyeY
					l_vGazeJoints[4] =  l_pGazeTarget->get(5).asDouble(); //rightEyeLid
					l_vGazeJoints[5] =  l_pGazeTarget->get(6).asDouble(); //leftEyeLid
				}
				break;
				case swTracking::TOBII_LIB :
				{
					// eye position
					int l_i32ScreenHeight = l_pGazeTarget->get(27).asInt();
					int l_i32ScreenWidth  = l_pGazeTarget->get(28).asInt();
					int l_i32DistanceInterEye  = l_pGazeTarget->get(29).asInt();

					double l_leftValidity = l_pGazeTarget->get(1).asDouble();
					double l_rightValidity = l_pGazeTarget->get(2).asDouble();

					std::vector<double> l_vRightEyePosition3D;
					l_vRightEyePosition3D.push_back(l_pGazeTarget->get(8).asDouble());
					l_vRightEyePosition3D.push_back(l_pGazeTarget->get(9).asDouble());
					l_vRightEyePosition3D.push_back(l_pGazeTarget->get(10).asDouble());
					std::vector<double> l_vRightGazePoint2D;
					l_vRightGazePoint2D.push_back(l_pGazeTarget->get(25).asDouble());
					l_vRightGazePoint2D.push_back(l_pGazeTarget->get(26).asDouble());

					std::vector<double> l_vLeftEyePosition3D;
					l_vLeftEyePosition3D.push_back(l_pGazeTarget->get(5).asDouble());
					l_vLeftEyePosition3D.push_back(l_pGazeTarget->get(6).asDouble());
					l_vLeftEyePosition3D.push_back(l_pGazeTarget->get(7).asDouble());
					std::vector<double> l_vLeftGazePoint2D;
					l_vLeftGazePoint2D.push_back(l_pGazeTarget->get(23).asDouble());
					l_vLeftGazePoint2D.push_back(l_pGazeTarget->get(24).asDouble());


					bool l_blink = (l_leftValidity + l_rightValidity) == 8;

					double l_subjectDistance, l_rightEyeRotationY, l_leftEyeRotationY, l_rightEyeRotationX, l_leftEyeRotationX;

					// TODO : simon code, check
					if(l_leftValidity == 4) //Right eye only is valid
					{
						l_subjectDistance = l_vRightEyePosition3D[2];

						l_rightEyeRotationY = (atan((l_vRightGazePoint2D[1] -0.5)*l_i32ScreenHeight/l_subjectDistance))*180/PI;
						l_leftEyeRotationY  = l_rightEyeRotationY;  // we hypothesize that both eyes are going up/down synchronously
						l_vGazeJoints[3] = (l_leftEyeRotationY + l_rightEyeRotationY)/2;

						l_rightEyeRotationX = (atan(((l_vRightGazePoint2D[0] -0.5)*l_i32ScreenWidth - l_i32DistanceInterEye/2)/l_subjectDistance))*180/PI;
						l_leftEyeRotationX = l_rightEyeRotationX;
						l_vGazeJoints[4] = (l_leftEyeRotationX + l_rightEyeRotationX)/2;
						l_vGazeJoints[5] = -l_leftEyeRotationX + l_rightEyeRotationX;
					}
					else if(l_rightValidity == 4) // left eye is valid
					{
						l_subjectDistance = l_vLeftEyePosition3D[2];

						l_leftEyeRotationY = (atan(l_vLeftGazePoint2D[1] -0.5) * l_i32ScreenHeight/l_subjectDistance)*180/PI;
						l_rightEyeRotationY = l_leftEyeRotationY; // we hypothesize that both eyes are going up/down synchronously
						l_vGazeJoints[3] = (l_leftEyeRotationY + l_rightEyeRotationY)/2;

						l_rightEyeRotationX = (atan(((l_vLeftGazePoint2D[0]-.5)*l_i32ScreenWidth-l_i32DistanceInterEye/2)/l_subjectDistance))*180/PI;
						l_leftEyeRotationX = (atan(((l_vLeftGazePoint2D[0]-.5)*l_i32ScreenWidth+l_i32DistanceInterEye/2)/l_subjectDistance))*180/PI;
						l_vGazeJoints[4] = (l_leftEyeRotationX + l_rightEyeRotationX)/2;
						l_vGazeJoints[5] = -l_leftEyeRotationX + l_rightEyeRotationX;
					}
					else //Both eyes are valid - use the average of values
					{
						l_subjectDistance = (l_vLeftEyePosition3D[2] +  l_vRightEyePosition3D[2])/2;

						l_leftEyeRotationY = (atan((l_vLeftGazePoint2D[1]-.5)*l_i32ScreenHeight/l_subjectDistance))*180/PI;
						l_rightEyeRotationY = (atan((l_vRightGazePoint2D[1]-.5)*l_i32ScreenHeight/l_subjectDistance))*180/PI;
						l_vGazeJoints[3] = (l_leftEyeRotationY + l_rightEyeRotationY)/2;

						l_rightEyeRotationX = (atan(((l_vRightGazePoint2D[0]-.5)*l_i32ScreenWidth-l_i32DistanceInterEye/2)/l_subjectDistance))*180/PI;
						l_leftEyeRotationX = (atan(((l_vLeftGazePoint2D[0]-.5)*l_i32ScreenWidth+l_i32DistanceInterEye/2)/l_subjectDistance))*180/PI;
						l_vGazeJoints[4] = (l_leftEyeRotationX + l_rightEyeRotationX)/2;
						l_vGazeJoints[5] = -l_leftEyeRotationX + l_rightEyeRotationX;
					}
				}
				break;
				case swTracking::COREDATA_LIB :
				{
					// eye position
					l_vGazeJoints[3] = swUtil::rad2Deg( (l_pGazeTarget->get(9) .asDouble() + l_pGazeTarget->get(14).asDouble())/2.); // up/down eye [-35; +15]
					l_vGazeJoints[4] = swUtil::rad2Deg(-(l_pGazeTarget->get(10).asDouble() + l_pGazeTarget->get(15).asDouble())/2.); // version angle [-50; 52] = (L+R)/2
					l_vGazeJoints[5] = swUtil::rad2Deg( -l_pGazeTarget->get(10).asDouble() + l_pGazeTarget->get(15).asDouble());     // vergence angle [0 90] = R-L

					// eye closure
					double l_dLeftEyeClosure = l_pGazeTarget->get(8).asDouble(), l_dRightEyeClosure = l_pGazeTarget->get(13).asDouble();
					
					
				}
				break;
			}

                m_dGazeTimeLastBottle = -1.;
                m_pVelocityController->enableGaze(true);
		}
	}

	
	if(m_bFaceActivated)
	{
		l_pFaceTarget = m_oFaceTrackerPort.read(false);

		if(l_pFaceTarget)
		{
			int l_deviceId = l_pFaceTarget->get(0).asInt();
			switch(l_deviceId)
			{
				case swTracking::OPENNI_LIB :
				{
					
				}break;
				case swTracking::FOREST_LIB :
				{                
					
				} break;
			}

			m_i32DFaceTimeLastBottle = 0;
			l_bFaceCapture = true;
		}
		else
		{
			m_i32FaceTimeLastBottle += 1000/m_i32Fps;

			if(m_i32FaceTimeLastBottle > m_i32FaceTimeoutReset)
			{
				m_i32FaceTimeLastBottle = 0;
				resetPosition();
			}
		}
	}
	

	//checks each joint value to ensure it is in the right range, if not crop to the max/min values
	// head
	if (l_dNeckRotatValueJoint < m_dNeckRotatMinValueJoint)
		l_dNeckRotatValueJoint = m_dNeckRotatMinValueJoint;
	if (l_dNeckRotatValueJoint > m_dNeckRotatMaxValueJoint)
		l_dNeckRotatValueJoint = m_dNeckRotatMaxValueJoint;
	
	if (l_dNeckTiltValueJoint < m_dNeckTiltMinValueJoint)
		l_dNeckTiltValueJoint = m_dNeckTiltMinValueJoint;
	if (l_dNeckTiltValueJoint > m_dNeckTiltMaxValueJoint)
		l_dNeckTiltValueJoint = m_dNeckTiltMaxValueJoint;
	
	if (l_dNeckPanValueJoint < m_dNeckPanMinValueJoint)
		l_dNeckPanValueJoint = m_dNeckPanMinValueJoint;
	if (l_dNeckPanValueJoint > m_dNeckPanMaxValueJoint)
		l_dNeckPanValueJoint = m_dNeckPanMaxValueJoint;
	//face
	if (l_dLeftLCValueJoint < m_dLeftLCMinValueJoint)
		l_dLeftLCValueJoint = m_dLeftLCMinValueJoint;
	if (l_dLeftLCValueJoint > m_dLeftLCMaxValueJoint)
		l_dLeftLCValueJoint = m_dLeftLCMaxValueJoint;
	
	if (l_dRightLCValueJoint < m_dRightLCMinValueJoint)
		l_dRightLCValueJoint = m_dRightLCMinValueJoint;
	if (l_dRightLCValueJoint > m_dRightLCMaxValueJoint)
		l_dRightLCValueJoint = m_dRightLCMaxValueJoint;
	
	if (l_dTopLipValueJoint < m_dTopLipMinValueJoint)
		l_dTopLipValueJoint = m_dTopLipMinValueJoint;
	if (l_dTopLipValueJoint > m_dTopLipMaxValueJoint)
		l_dTopLipValueJoint = m_dTopLipMaxValueJoint;
	
	if (l_dBottomLipValueJoint < m_dBottomLipMinValueJoint)
		l_dBottomLipValueJoint = m_dBottomLipMinValueJoint;
	if (l_dBottomLipValueJoint > m_dBottomLipMaxValueJoint)
		l_dBottomLipValueJoint = m_dBottomLipMaxValueJoint;
	//gaze
	if (l_dRightEyeTiltValueJoint < m_dRightEyeTiltMinValueJoint)
		l_dRightEyeTiltValueJoint = m_dRightEyeTiltMinValueJoint;
	if (l_dRightEyeTiltValueJoint > m_dRightEyeTiltMaxValueJoint)
		l_dRightEyeTiltValueJoint = m_dRightEyeTiltMaxValueJoint;
	
	if (l_dLeftEyeTiltValueJoint < m_dLeftEyeTiltMinValueJoint)
		l_dLeftEyeTiltValueJoint = m_dLeftEyeTiltMinValueJoint;
	if (l_dLeftEyeTiltValueJoint > m_dLeftEyeTiltMaxValueJoint)
		l_dLeftEyeTiltValueJoint = m_dLeftEyeTiltMaxValueJoint;
	
	if (l_dRightEyePanValueJoint < m_dRightEyePanMinValueJoint)
		l_dRightEyePanValueJoint = m_dRightEyePanMinValueJoint;
	if (l_dRightEyePanValueJoint > m_dRightEyePanMaxValueJoint)
		l_dRightEyePanValueJoint = m_dRightEyePanMaxValueJoint;
	
	if (l_dLeftEyePanValueJoint < m_dLeftEyePanMinValueJoint)
		l_dLeftEyePanValueJoint = m_dLeftEyePanMinValueJoint;
	if (l_dLeftEyePanValueJoint > m_dLeftEyePanMaxValueJoint)
		l_dLeftEyePanValueJoint = m_dLeftEyePanMaxValueJoint;
   
	if (l_dRightEyeLidValueJoint < m_dRightEyeLidMinValueJoint)
		l_dRightEyeLidValueJoint = m_dRightEyeLidMinValueJoint;
	if (l_dRightEyeLidValueJoint > m_dRightEyeLidMaxValueJoint)
		l_dRightEyeLidValueJoint = m_dRightEyeLidMaxValueJoint;
	
	if (l_dLeftEyeLidValueJoint < m_dLeftEyeLidMinValueJoint)
		l_dLeftEyeLidValueJoint = m_dLeftEyeLidMinValueJoint;
	if (l_dLeftEyeLidValueJoint > m_dLeftEyeLidMaxValueJoint)
		l_dLeftEyeLidValueJoint = m_dLeftEyeLidMaxValueJoint;

	
	// sends the command to the robot
	// head
	if (l_bHeadCapture)
	{
		string cmd = "Global.servo.neckRotat(" + l_dNeckRotatValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.neckTilt(" + l_dNeckTiltValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.neckPan(" + l_dNeckPanValueJoint + ");"; 
		client->send("%s", cmd.c_str());
	}
	// face
	if(m_bFaceActivated)
	{
		string cmd = "Global.servo.LeftLC(" + l_dLeftLCValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.rightLC(" + l_dRightLCValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.topLip(" + l_dTopLipValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.bottomLip(" + l_dBottomLipValueJoint + ");"; 
		client->send("%s", cmd.c_str());
	}
	// gaze
	if(m_bGazeActivated)
	{
		string cmd = "Global.servo.rightEyeTilt(" + l_dRightEyeTiltValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.leftEyeTilt(" + l_dLeftEyeTiltValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.rightEyePan(" + l_dRightEyePanValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.leftEyePan(" + l_dLeftEyePanValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.rightEyeLid(" + l_dRightEyeLidValueJoint + ");"; 
		client->send("%s", cmd.c_str());
		
		string cmd = "Global.servo.leftEyeLid(" + l_dLeftEyeLidValueJoint + ");"; 
		client->send("%s", cmd.c_str());
	}

	return true;
}


double SWTeleoperation_reeti::getPeriod()
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
	SWTeleoperation_reeti l_oTeleoperation_reeti;

	// prepare and configure the resource finder
	yarp::os::ResourceFinder l_oRf;
	l_oRf.setVerbose(true);
	l_oRf.setDefaultConfigFile("teleoperation_reeti.ini");
	l_oRf.setDefaultContext("swooz-teleoperation/conf");
	l_oRf.configure("REETI_ROOT", argc, argv);

	// configure the module
	std::cout << "Configuring the reeti Teleoperation module..."<< std::endl;
	if (l_oTeleoperation_reeti.configure(l_oRf))
	{
		// run the module
		std::cout << "Starting the reeti Teleoperation module..." << std::endl;
		l_oTeleoperation_reeti.runModule();
	}
	else
	{
		std::cerr << "Failed to configure the reeti Teleoperation module!"<< std::endl;
	}

	return 0;
}

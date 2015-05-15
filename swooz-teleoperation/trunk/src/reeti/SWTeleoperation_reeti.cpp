/**
 * \file SWTeleoperation_reeti.cpp
 * \author Guillaume Gibert
 * \date 16/03/2015
 * \brief ...
 */

#include "reeti/SWTeleoperation_reeti.h"

#include "SWTrackingDevice.h"


#include <cmath>
#include <iostream>

#include "geometryUtility.h"


//using namespace std;
using namespace urbi;

SWTeleoperation_reeti::SWTeleoperation_reeti() :  m_i32HeadTimeLastBottle(0)
{    
	m_bHeadActivatedDefault     	= true;
	m_bFaceActivatedDefault		= true;
	m_bGazeActivatedDefault     	= true;
	m_bLEDSActivatedDefault     	= false;   
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
	m_dNeckRotatCoeffValueJoint = oRf.check("neckRotaCoeffValueJoint",  yarp::os::Value(1.0f), "Neck Rotat coefficient Joint Value (float)").asDouble();
	// neckTilt
	m_dNeckTiltMinValueJoint = oRf.check("neckTiltMinValueJoint",  yarp::os::Value(0.0f), "Neck Tilt minimum Joint Value (float)").asDouble();
	m_dNeckTiltMaxValueJoint = oRf.check("neckTiltMaxValueJoint",  yarp::os::Value(100.0f), "Neck Tilt maximum Joint Value (float)").asDouble();
	m_dNeckTiltNeuValueJoint = oRf.check("neckTiltNeuValueJoint",  yarp::os::Value(50.0f), "Neck Tilt neutral Joint Value (float)").asDouble();
	m_dNeckTiltCoeffValueJoint = oRf.check("neckTiltCoeffValueJoint",  yarp::os::Value(4.0f), "Neck Tilt coeffticient Joint Value (float)").asDouble();
        // neckPan
	m_dNeckPanMinValueJoint = oRf.check("neckPanMinValueJoint",  yarp::os::Value(0.0f), "Neck Pan minimum Joint Value (float)").asDouble();
	m_dNeckPanMaxValueJoint = oRf.check("neckPanMaxValueJoint",  yarp::os::Value(100.0f), "Neck Pan maximum Joint Value (float)").asDouble();
	m_dNeckPanNeuValueJoint = oRf.check("neckPanNeuValueJoint",  yarp::os::Value(50.0f), "Neck Pan neutral Joint Value (float)").asDouble();
	m_dNeckPanCoeffValueJoint = oRf.check("neckPanCoeffValueJoint",  yarp::os::Value(3.3f), "Neck Pan coefficient Joint Value (float)").asDouble();
	// leftLC
	m_dLeftLCMinValueJoint = oRf.check("leftLCMinValueJoint",  yarp::os::Value(0.0f), "Left LC minimum Joint Value (float)").asDouble();
	m_dLeftLCMaxValueJoint = oRf.check("leftLCMaxValueJoint",  yarp::os::Value(100.0f), "Left LC maximum Joint Value (float)").asDouble();
	m_dLeftLCNeuValueJoint = oRf.check("leftLCNeuValueJoint",  yarp::os::Value(50.0f), "Left LC neutral Joint Value (float)").asDouble();
	// rightLC
	m_dRightLCMinValueJoint = oRf.check("rightLCMinValueJoint",  yarp::os::Value(0.0f), "Right LC minimum Joint Value (float)").asDouble();
	m_dRightLCMaxValueJoint = oRf.check("rightLCMaxValueJoint",  yarp::os::Value(100.0f), "Right LC maximum Joint Value (float)").asDouble();
	m_dRightLCNeuValueJoint = oRf.check("rightLCNeuValueJoint",  yarp::os::Value(50.0f), "Right LC neutral Joint Value (float)").asDouble();
	// topLip
	m_dTopLipMinValueJoint = oRf.check("topLipMinValueJoint",  yarp::os::Value(0.0f), "Top Lip minimum Joint Value (float)").asDouble();
	m_dTopLipMaxValueJoint = oRf.check("topLipMaxValueJoint",  yarp::os::Value(100.0f), "Top Lip maximum Joint Value (float)").asDouble();
	m_dTopLipNeuValueJoint = oRf.check("topLipNeuValueJoint",  yarp::os::Value(50.0f), "Top Lip neutral Joint Value (float)").asDouble();
	// bottomLip
	m_dBottomLipMinValueJoint = oRf.check("bottomLipMinValueJoint",  yarp::os::Value(0.0f), "Bottom Lip minimum Joint Value (float)").asDouble();
	m_dBottomLipMaxValueJoint = oRf.check("bottomLipMaxValueJoint",  yarp::os::Value(100.0f), "Bottom Lip maximum Joint Value (float)").asDouble();
	m_dBottomLipNeuValueJoint = oRf.check("bottomLipNeuValueJoint",  yarp::os::Value(80.0f), "Bottom Lip neutral Joint Value (float)").asDouble();
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
	m_dRightEyeTiltNeuValueJoint = oRf.check("rightEyeTiltNeuValueJoint",  yarp::os::Value(50.0f), "Right Eye Tilt neutral Joint Value (float)").asDouble();
	m_dRightEyeTiltCoeffValueJoint = oRf.check("rightEyeTiltCoeffValueJoint",  yarp::os::Value(1.3f), "Right Eye Tilt coefficient Joint Value (float)").asDouble();
	// leftEyeTilt
	m_dLeftEyeTiltMinValueJoint = oRf.check("lefttEyeTiltMinValueJoint",  yarp::os::Value(0.0f), "Left Eye Tilt minimum Joint Value (float)").asDouble();
	m_dLeftEyeTiltMaxValueJoint = oRf.check("leftEyeTiltMaxValueJoint",  yarp::os::Value(100.0f), "Left Eye Tilt maximum Joint Value (float)").asDouble();
	m_dLeftEyeTiltNeuValueJoint = oRf.check("leftEyeTiltNeuValueJoint",  yarp::os::Value(50.0f), "Left Eye Tilt neutral Joint Value (float)").asDouble();
	m_dLeftEyeTiltCoeffValueJoint = oRf.check("leftEyeTiltCoeffValueJoint",  yarp::os::Value(1.3f), "Left Eye Tilt coefficient Joint Value (float)").asDouble();
	// rightEyePan
	m_dRightEyePanMinValueJoint = oRf.check("rightEyePanMinValueJoint",  yarp::os::Value(0.0f), "Right Eye Pan minimum Joint Value (float)").asDouble();
	m_dRightEyePanMaxValueJoint = oRf.check("rightEyePanMaxValueJoint",  yarp::os::Value(100.0f), "Right Eye Pan maximum Joint Value (float)").asDouble();
	m_dRightEyePanNeuValueJoint = oRf.check("rightEyePanNeuValueJoint",  yarp::os::Value(50.0f), "Right Eye Pan neutral Joint Value (float)").asDouble();
	m_dRightEyePanCoeffValueJoint = oRf.check("rightEyePanCoeffValueJoint",  yarp::os::Value(1.5f), "Right Eye Pan coefficient Joint Value (float)").asDouble();
	// leftEyePan
	m_dLeftEyePanMinValueJoint = oRf.check("lefttEyePanMinValueJoint",  yarp::os::Value(0.0f), "Left Eye Pan minimum Joint Value (float)").asDouble();
	m_dLeftEyePanMaxValueJoint = oRf.check("leftEyePanMaxValueJoint",  yarp::os::Value(100.0f), "Left Eye Pan maximum Joint Value (float)").asDouble();
	m_dLeftEyePanNeuValueJoint = oRf.check("leftEyePanNeuValueJoint",  yarp::os::Value(50.0f), "Left Eye Pan neutral Joint Value (float)").asDouble();
	m_dLeftEyePanCoeffValueJoint = oRf.check("leftEyePanCoeffValueJoint",  yarp::os::Value(1.5f), "Left Eye Pan coefficient Joint Value (float)").asDouble();
	// rightEyeLid
	m_dRightEyeLidMinValueJoint = oRf.check("rightEyeLidMinValueJoint",  yarp::os::Value(0.0f), "Right Eye Lid minimum Joint Value (float)").asDouble();
	m_dRightEyeLidMaxValueJoint = oRf.check("rightEyeLidMaxValueJoint",  yarp::os::Value(100.0f), "Right Eye Lid maximum Joint Value (float)").asDouble();
	m_dRightEyeLidNeuValueJoint = oRf.check("rightEyeLidNeuValueJoint",  yarp::os::Value(90.0f), "Right Eye Lid neutral Joint Value (float)").asDouble();
	// leftEyeLid
	m_dLeftEyeLidMinValueJoint = oRf.check("lefttEyeLidMinValueJoint",  yarp::os::Value(0.0f), "Left Eye Lid minimum Joint Value (float)").asDouble();
	m_dLeftEyeLidMaxValueJoint = oRf.check("leftEyeLidMaxValueJoint",  yarp::os::Value(100.0f), "Left Eye Lid maximum Joint Value (float)").asDouble();
	m_dLeftEyeLidNeuValueJoint = oRf.check("leftEyeLidNeuValueJoint",  yarp::os::Value(90.0f), "Left Eye Lid neutral Joint Value (float)").asDouble();
	
	// miscellaneous
        m_i32Fps                    	= oRf.check("fps",              yarp::os::Value(10),  "Frame per second (int)").asInt();
        m_i32HeadTimeoutReset      	= oRf.check("headTimeoutReset", yarp::os::Value(3000), "Head timeout reset Reeti (int)").asInt();
	m_i32FaceTimeoutReset      	= oRf.check("faceTimeoutReset", yarp::os::Value(3000), "Face timeout reset Reeti (int)").asInt();
	m_i32GazeTimeoutReset      	= oRf.check("gazeTimeoutReset", yarp::os::Value(3000), "Gaze timeout reset Reeti (int)").asInt();

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
	m_pClient=new UClient(m_sRobotAddress, m_i32RobotPort);

	// switches on LED on red for the fun
	if (m_bLEDSActivated)
	{
		std::string cmd_initLED = "Global.servo.changeLedColor(\"blue\");";
		m_pClient->send("%s", cmd_initLED.c_str());
	}
     
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
		std::string cmd_neckRotat = "Global.servo.neckRotat=" +  to_string(m_dNeckRotatNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_neckRotat.c_str());
		
		std::string cmd_neckPan = "Global.servo.neckPan=" +  to_string(m_dNeckPanNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_neckPan.c_str());
		
		std::string cmd_neckTilt = "Global.servo.neckTilt=" +  to_string(m_dNeckTiltNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_neckTilt.c_str());
	}
	
	if(m_bFaceActivated)
	{
		std::string cmd_leftLC = "Global.servo.LeftLC=" +  to_string(m_dLeftLCNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_leftLC.c_str());
		
		std::string cmd_rightLC = "Global.servo.rightLC=" +  to_string(m_dRightLCNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_rightLC.c_str());
		
		std::string cmd_topLip = "Global.servo.topLip=" +  to_string(m_dTopLipNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_topLip.c_str());
		
		std::string cmd_bottomLip = "Global.servo.bottomLip=" +  to_string(m_dBottomLipNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_bottomLip.c_str());
	}
	
	if(m_bGazeActivated)
	{
		std::string cmd_rightEyeTilt = "Global.servo.rightEyeTilt=" +  to_string(m_dRightEyeTiltNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_rightEyeTilt.c_str());
		
		std::string cmd_leftEyeTilt = "Global.servo.leftEyeTilt=" +  to_string(m_dLeftEyeTiltNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_leftEyeTilt.c_str());
		
		std::string cmd_rightEyePan = "Global.servo.rightEyePan=" +  to_string(m_dRightEyePanNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_rightEyePan.c_str());
		
		std::string cmd_leftEyePan = "Global.servo.leftEyePan=" +  to_string(m_dLeftEyePanNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_leftEyePan.c_str());
		
		std::string cmd_rightEyeLid = "Global.servo.rightEyeLid=" +  to_string(m_dRightEyeLidNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_rightEyeLid.c_str());
		
		std::string cmd_leftEyeLid = "Global.servo.leftEyeLid=" +  to_string(m_dLeftEyeLidNeuValueJoint) + ";"; 
		m_pClient->send("%s", cmd_leftEyeLid.c_str());
	}
	
	// TODO: to add switch off the leds
}


bool SWTeleoperation_reeti::close()
{
	resetPosition();

	// close yarp ports
	m_oHeadTrackerPort.close();
	m_oFaceTrackerPort.close();
	m_oGazeTrackerPort.close();
  
	// close urbi m_pClient
	m_pClient->close();
	
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
		l_vGazeJoints = 0.;
	yarp::sig::Vector l_vFaceJoints;
		l_vFaceJoints.resize(6); // leftLC, rightLC, topLip, bottomLip, rightEar, leftEar
		l_vFaceJoints = 0.;
	
	// defines bottles
	Bottle *l_pHeadTarget = NULL, *l_pFaceTarget = NULL, *l_pGazeTarget = NULL;
	
	// defines joint values (by default = to neutral position)
	double l_dNeckRotatValueJoint = m_dNeckRotatNeuValueJoint;
	double l_dNeckTiltValueJoint = m_dNeckTiltNeuValueJoint;
	double l_dNeckPanValueJoint = m_dNeckPanNeuValueJoint;

	double l_dLeftLCValueJoint = m_dLeftLCNeuValueJoint;
	double l_dRightLCValueJoint = m_dRightLCNeuValueJoint;
	double l_dTopLipValueJoint = m_dTopLipNeuValueJoint;
	double l_dBottomLipValueJoint = m_dBottomLipNeuValueJoint;
	double l_dRightEarValueJoint = m_dRightEarNeuValueJoint;
	double l_dLeftEarValueJoint = m_dLeftEarNeuValueJoint;

	double l_dRightEyeTiltValueJoint = m_dRightEyeTiltNeuValueJoint;
	double l_dLeftEyeTiltValueJoint = m_dLeftEyeTiltNeuValueJoint;
	double l_dRightEyePanValueJoint = m_dRightEyePanNeuValueJoint;
	double l_dLeftEyePanValueJoint = m_dLeftEyePanNeuValueJoint;
	double l_dRightEyeLidValueJoint = m_dRightEyeLidNeuValueJoint;
	double l_dLeftEyeLidValueJoint = m_dRightEyeLidNeuValueJoint;
	
	// read head commands
	if(m_bHeadActivated)
	{
		l_pHeadTarget = m_oHeadTrackerPort.read(false);

		if(l_pHeadTarget)
		{
			//std::cout << "Head Bottle received" << std::endl;
				
			int l_deviceId = l_pHeadTarget->get(0).asInt();
			
			switch(l_deviceId)
			{
				case swTracking::DUMMY_LIB :
				{
					l_vHeadJoints[0] = l_pHeadTarget->get(1).asDouble();
					l_vHeadJoints[1] = l_pHeadTarget->get(2).asDouble();
					l_vHeadJoints[2] = l_pHeadTarget->get(3).asDouble();
				}
				break;
				case swTracking::FASTRAK_LIB :
				{
					l_vHeadJoints[0] = l_pHeadTarget->get(2).asDouble();
					l_vHeadJoints[1] = l_pHeadTarget->get(3).asDouble();
					l_vHeadJoints[2] = -(l_pHeadTarget->get(1).asDouble()-90.0);
				}
				break;
				case swTracking::FOREST_LIB :
				{
					l_vHeadJoints[0] = -l_pHeadTarget->get(1).asDouble(); //head rotation "yes" 
					l_vHeadJoints[1] = -l_pHeadTarget->get(3).asDouble(); //head rotation
					l_vHeadJoints[2] = -l_pHeadTarget->get(2).asDouble(); //head rotation "no"
				}
				break;
				case swTracking::COREDATA_LIB :
				{
					l_vHeadJoints[0] =  swUtil::rad2Deg(l_pHeadTarget->get(4).asDouble()); //head rotation "yes" 
					l_vHeadJoints[1] = -swUtil::rad2Deg(l_pHeadTarget->get(6).asDouble()); //head rotation
					l_vHeadJoints[2] =  swUtil::rad2Deg(l_pHeadTarget->get(5).asDouble()); //head rotation "no"
				}
				break;
				case swTracking::EMICP_LIB :
				{
					l_vHeadJoints[0] = -l_pHeadTarget->get(4).asDouble(); 
					l_vHeadJoints[1] = -l_pHeadTarget->get(6).asDouble(); 
					l_vHeadJoints[2] = -l_pHeadTarget->get(5).asDouble();
				}
				break;
				case swTracking::FACESHIFT_LIB :
				{
					l_vHeadJoints[0] = -swUtil::rad2Deg(l_pHeadTarget->get(4).asDouble()); 
					l_vHeadJoints[1] = -swUtil::rad2Deg(l_pHeadTarget->get(6).asDouble());
					l_vHeadJoints[2] = swUtil::rad2Deg(l_pHeadTarget->get(5).asDouble());
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

					l_vHeadJoints[0] = -l_rpyHead[1];
					l_vHeadJoints[1] = -l_rpyHead[0];
					l_vHeadJoints[2] =  l_rpyHead[2];
				}
				break;
			}
			
			// fills the joint variables (neutral value is not 0! therefore need to rereference the data)
			l_dNeckRotatValueJoint = l_vHeadJoints[2] * m_dNeckRotatCoeffValueJoint + m_dNeckRotatNeuValueJoint;
			l_dNeckTiltValueJoint = l_vHeadJoints[0] * m_dNeckTiltCoeffValueJoint + m_dNeckTiltNeuValueJoint;
			l_dNeckPanValueJoint = l_vHeadJoints[1] * m_dNeckPanCoeffValueJoint + m_dNeckPanNeuValueJoint;

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
				//std::cout << "Gaze bottle received" << std::endl;				
				case swTracking::FACESHIFT_LIB :
				{
					//tilt
					l_dRightEyeTiltValueJoint =  -l_pGazeTarget->get(3).asDouble() * m_dRightEyeTiltCoeffValueJoint + m_dRightEyeTiltNeuValueJoint; 
					l_dLeftEyeTiltValueJoint =  -l_pGazeTarget->get(1).asDouble() * m_dLeftEyeTiltCoeffValueJoint + m_dLeftEyeTiltNeuValueJoint; 
					//pan
					l_dRightEyePanValueJoint =  l_pGazeTarget->get(4).asDouble() * m_dRightEyePanCoeffValueJoint + m_dRightEyePanNeuValueJoint;
					l_dLeftEyePanValueJoint =  l_pGazeTarget->get(2).asDouble() * m_dLeftEyePanCoeffValueJoint + m_dLeftEyePanNeuValueJoint;
					//blink
					l_dRightEyeLidValueJoint=  m_dRightEyeLidNeuValueJoint - l_pGazeTarget->get(6).asDouble()*100; 
					l_dLeftEyeLidValueJoint =  m_dRightEyeLidNeuValueJoint - l_pGazeTarget->get(5).asDouble()*100; 
					
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
					l_dRightEyeTiltValueJoint = swUtil::rad2Deg(l_pGazeTarget->get(14).asDouble()) * m_dRightEyeTiltCoeffValueJoint + m_dRightEyeTiltNeuValueJoint;//gaze: rightGazeRotation x / get(14).asDouble()
					l_dLeftEyeTiltValueJoint = swUtil::rad2Deg(l_pGazeTarget->get(9).asDouble()) * m_dLeftEyeTiltCoeffValueJoint + m_dLeftEyeTiltNeuValueJoint; //gaze: leftGazeRotation x   / get(9).asDouble()

					l_dRightEyePanValueJoint = swUtil::rad2Deg(l_pGazeTarget->get(15).asDouble()) * m_dRightEyePanCoeffValueJoint + m_dRightEyePanNeuValueJoint;//gaze: rightGazeRotation y / get(15).asDouble()
					l_dLeftEyePanValueJoint = swUtil::rad2Deg(l_pGazeTarget->get(10).asDouble()) * m_dLeftEyePanCoeffValueJoint + m_dLeftEyePanNeuValueJoint;//gaze: leftGazeRotation y   / get(10).asDouble()

					l_dRightEyeLidValueJoint = m_dRightEyeLidNeuValueJoint-l_pGazeTarget->get(13).asDouble()*100;//gaze: rightEyeClosure     / get(13).asDouble()
					l_dLeftEyeLidValueJoint = m_dLeftEyeLidNeuValueJoint-l_pGazeTarget->get(8).asDouble()*100; //gaze: leftEyeClosure       / get(8).asDouble()

				}
				break;
			}

			m_i32GazeTimeLastBottle = 0.;
                
		}
		else  // manage timeout and reset position
		{
			m_i32GazeTimeLastBottle += 1000/m_i32Fps;

			if(m_i32GazeTimeLastBottle > m_i32GazeTimeoutReset)
			{
				m_i32GazeTimeLastBottle = 0;
				resetPosition();
			}
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
				//std::cout << "Face bottle received" << std::endl;				
				case swTracking::FACESHIFT_LIB :
				{
					
					l_dBottomLipValueJoint = m_dBottomLipNeuValueJoint - l_pFaceTarget->get(20).asDouble()*100; // JawOpen
					l_dTopLipValueJoint = m_dTopLipNeuValueJoint + l_pFaceTarget->get(42).asDouble()*50; // LipsFunnel
					l_dRightLCValueJoint = m_dRightLCNeuValueJoint+l_pFaceTarget->get(32).asDouble()*50; //MouthSmile_R
					l_dLeftLCValueJoint = m_dLeftLCNeuValueJoint+l_pFaceTarget->get(31).asDouble()*50; //MouthSmile_L
					l_dRightEarValueJoint = m_dRightEarNeuValueJoint - l_pFaceTarget->get(19).asDouble()*100; // Right eyebrow
					l_dLeftEarValueJoint = m_dLeftEarNeuValueJoint - l_pFaceTarget->get(18).asDouble()*100; // Left eyebrow
				}
				break;
				case swTracking::COREDATA_LIB :
				{
					// retrieve values
					// mouth
					// bottom lip
					std::vector<double> l_vInnerLip2, l_vInnerLip6;
					for(int ii = 0; ii < 3; ++ii)
					{
						l_vInnerLip2.push_back(l_pFaceTarget->get(25+ii).asDouble());
						l_vInnerLip6.push_back(l_pFaceTarget->get(37+ii).asDouble());
					}
					double l_dLipsDistance = sqrt( pow(l_vInnerLip6[0] - l_vInnerLip2[0], 2) + pow(l_vInnerLip6[1] - l_vInnerLip2[1], 2) + pow(l_vInnerLip6[2] - l_vInnerLip2[2], 2));
					
					l_dBottomLipValueJoint = m_dBottomLipMaxValueJoint-l_dLipsDistance*5000;

					// top lip (protrusion)
					std::vector<double> l_vInnerLip0, l_vInnerLip4;
					for(int ii = 0; ii < 3; ++ii)
					{
						l_vInnerLip0.push_back(l_pFaceTarget->get(19+ii).asDouble());
						l_vInnerLip4.push_back(l_pFaceTarget->get(31+ii).asDouble());
					}
					double l_dCornerLipsDistance = sqrt( pow(l_vInnerLip4[0] - l_vInnerLip0[0], 2) + pow(l_vInnerLip4[1] - l_vInnerLip0[1], 2) + pow(l_vInnerLip4[2] - l_vInnerLip0[2], 2));			
					l_dTopLipValueJoint = m_dTopLipMaxValueJoint-(l_dCornerLipsDistance-0.03)*5000;

					// lip corners
					std::vector<double> l_vNoseTip;
					for(int ii = 0; ii < 3; ++ii)
					{
						l_vNoseTip.push_back(l_pFaceTarget->get(61+ii).asDouble());
					}
					double l_dRightCornerNoseTipYDistance = sqrt( pow(l_vInnerLip4[1] - l_vNoseTip[1], 2) );
					
					//l_dRightLCValueJoint = m_dRightLCNeuValueJoint-(l_dRightCornerNoseTipYDistance-0.025)*5000;

					//std::cout << "l_l_dRightCornerNoseTipYDistance = " << l_dRightCornerNoseTipYDistance << std::endl;
					
				}
				break;
			}

			m_i32FaceTimeLastBottle = 0;
			
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
	//ear
	if (l_dLeftEarValueJoint < m_dLeftEarMinValueJoint)
		l_dLeftEarValueJoint = m_dLeftEarMinValueJoint;
	if (l_dLeftEarValueJoint > m_dLeftEarMaxValueJoint)
		l_dLeftEarValueJoint = m_dLeftEarMaxValueJoint;
	if (l_dRightEarValueJoint < m_dRightEarMinValueJoint)
		l_dRightEarValueJoint = m_dRightEarMinValueJoint;
	if (l_dRightEarValueJoint > m_dRightEarMaxValueJoint)
		l_dRightEarValueJoint = m_dRightEarMaxValueJoint;
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
	if (m_bHeadActivated)
	{
		std::string cmd_neckRotat = "Global.servo.neckRotat=" +  to_string(l_dNeckRotatValueJoint) + ";"; 
		m_pClient->send("%s", cmd_neckRotat.c_str());
		
		
		std::string cmd_neckPan = "Global.servo.neckPan="+  to_string(l_dNeckPanValueJoint) + ";"; 
		m_pClient->send("%s", cmd_neckPan.c_str());
		
		std::string cmd_neckTilt = "Global.servo.neckTilt=" +  to_string(l_dNeckTiltValueJoint) + ";"; 
		m_pClient->send("%s", cmd_neckTilt.c_str());
		
		
		//~ std::cout << cmd_neckTilt<< std::endl;
		//~ std::cout << cmd_neckPan<< std::endl;
		//std::cout << "-->Head command sent!" << std::endl;
	}
	// face
	if(m_bFaceActivated)
	{
		std::string cmd_leftLC = "Global.servo.leftLC=" +  to_string(l_dLeftLCValueJoint) + ";"; 
		m_pClient->send("%s", cmd_leftLC.c_str());
		
		std::string cmd_rightLC = "Global.servo.rightLC=" +  to_string(l_dRightLCValueJoint) + ";"; 
		m_pClient->send("%s", cmd_rightLC.c_str());
		
		std::string cmd_topLip = "Global.servo.topLip=" +  to_string(l_dTopLipValueJoint) + ";"; 
		m_pClient->send("%s", cmd_topLip.c_str());
		
		std::string cmd_bottomLip = "Global.servo.bottomLip=" +  to_string(l_dBottomLipValueJoint) + ";"; 
		m_pClient->send("%s", cmd_bottomLip.c_str());
		
		std::string cmd_leftEar = "Global.servo.leftEar=" +  to_string(l_dLeftEarValueJoint) + ";"; 
		m_pClient->send("%s", cmd_leftEar.c_str());
		
		std::string cmd_rightEar = "Global.servo.rightEar=" +  to_string(l_dRightEarValueJoint) + ";"; 
		m_pClient->send("%s", cmd_rightEar.c_str());

		//std::cout << "-->Face command sent!" << std::endl;
	}
	// gaze
	if(m_bGazeActivated)
	{
		std::string cmd_rightEyeTilt = "Global.servo.rightEyeTilt=" + to_string(l_dRightEyeTiltValueJoint) + ";"; 
		m_pClient->send("%s", cmd_rightEyeTilt.c_str());
		//std::cout << cmd_rightEyeTilt << std::endl;		

		std::string cmd_leftEyeTilt = "Global.servo.leftEyeTilt=" + to_string(l_dLeftEyeTiltValueJoint) + ";"; 
		m_pClient->send("%s", cmd_leftEyeTilt.c_str());
		//std::cout << cmd_leftEyeTilt << std::endl;

		std::string cmd_rightEyePan = "Global.servo.rightEyePan=" + to_string(l_dRightEyePanValueJoint) + ";"; 
		m_pClient->send("%s", cmd_rightEyePan.c_str());
		
		std::string cmd_leftEyePan = "Global.servo.leftEyePan=" + to_string(l_dLeftEyePanValueJoint) + ";"; 
		m_pClient->send("%s", cmd_leftEyePan.c_str());
		
		std::string cmd_rightEyeLid = "Global.servo.rightEyeLid=" + to_string(l_dRightEyeLidValueJoint) + ";"; 
		m_pClient->send("%s", cmd_rightEyeLid.c_str());
		
		std::string cmd_leftEyeLid = "Global.servo.leftEyeLid=" + to_string(l_dLeftEyeLidValueJoint) + ";"; 
		m_pClient->send("%s", cmd_leftEyeLid.c_str());

		//std::cout << "-->Gaze command sent!" << std::endl;
	}

	return true;
}



template <typename T> 
std::string SWTeleoperation_reeti::to_string(T value)
{
	std::ostringstream os ;
	os << value ;
	return os.str() ;
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

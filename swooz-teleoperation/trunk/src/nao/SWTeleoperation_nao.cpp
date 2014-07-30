/**
 * \file SWTeleoperation_nao.cpp
 * \author Emmanuel Body
 * \date 21/01/2014
 * \brief ...
 */

#include "nao/SWTeleoperation_nao.h"

#include "SWTrackingDevice.h"

#include <sstream>
#include <vector>
#include <cmath>
#include <iostream>


#include "opencv2/core/core.hpp"
#include "opencvUtility.h"
#include "geometryUtility.h"

SWTeleoperation_nao::SWTeleoperation_nao() :  m_i32HeadTimeLastBottle(0)
{    
    m_bHeadActivatedDefault     = true;
    m_bTorsoActivatedDefault    = false;
    m_bLEDSActivatedDefault     = false;
    m_bLeftArmActivatedDefault  = false;
    m_bRightArmActivatedDefault = false;

    double l_aDHeadMinJointDefault[]     = {-2.0, -0.5};
    double l_aDHeadMaxJointDefault[]     = { 2.0,  0.5};
    double l_aDLeftArmMinJointDefault[]  = {-2.0, -0.31, -2.0, -1.5, -1.8, 0.0};
    double l_aDLeftArmMaxJointDefault[]  = { 1.9,  1.3,   2.0,  0.0,  1.8, 1.0};
    double l_aDRightArmMinJointDefault[] = {-2.0, -1.3,  -2.0,  0.0, -1.8, 0.0};
    double l_aDRightArmMaxJointDefault[] = { 1.9,  0.31,  2.0,  1.5,  1.8, 1.0};

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
    m_aTorsoAngles.arraySetSize(2);
    m_aLArmAngles.arraySetSize(6);
    m_aRArmAngles.arraySetSize(6);
    m_aLLegAngles.arraySetSize(6);
    m_aRLegAngles.arraySetSize(6);   
}


bool SWTeleoperation_nao::configure(ResourceFinder &oRf)
{
    // gets the module name which will form the stem of all module port names
        m_sModuleName   = oRf.check("name", Value("teleoperation_nao"), "Teleoperation/nao Module name (string)").asString();
        setName(m_sModuleName.c_str());
        m_sRobotAddress = oRf.check("IP", Value("169.254.108.110"), "IP Adress of the Nao Robot").asString();

    // acceleration/speeds values for nao
        m_dJointVelocityValue     = oRf.check("jointVelocityValue",  yarp::os::Value(0.1),  "Joint Velocity Value (float)").asDouble();

    // robot parts to control
        m_bHeadActivated    = oRf.check("headActivated",yarp::os::Value(m_bHeadActivatedDefault), "Head activated (int)").asInt() != 0;
        m_bLEDSActivated    = oRf.check("LEDSActivated",yarp::os::Value(m_bLEDSActivatedDefault), "LEDS activated (int)"). asInt() != 0;
        m_bTorsoActivated   = oRf.check("torsoActivated",yarp::os::Value(m_bTorsoActivatedDefault), "Torso activated (int)"). asInt() != 0;
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

        // torso
        m_dTorsoMinValueJoint = oRf.check("torsoMinValueJoint",  yarp::os::Value(-1.0f), "Torso minimum Joint Value (float)").asDouble();
        m_dTorsoMaxValueJoint = oRf.check("torsoMaxValueJoint",  yarp::os::Value(-0.45f), "Torso maximum Joint Value (float)").asDouble();


    // miscellaneous
        m_i32Fps                    = oRf.check("fps",              yarp::os::Value(100),  "Frame per second (int)").asInt();
        m_i32HeadTimeoutReset       = oRf.check("headTimeoutReset", yarp::os::Value(3000), "Head gaze timeout reset iCub (int)").asInt();

    // init ports
        std::string l_sHeadTrackerPortName  = "/teleoperation/nao/head";
        std::string l_sTorsoTrackerPortName = "/teleoperation/nao/torso";
        std::string l_sFaceTrackerPortName  = "/teleoperation/nao/face";
        std::string l_sLeftArmTrackerPortName = "/teleoperation/nao/left_arm";
        std::string l_sRightArmTrackerPortName= "/teleoperation/nao/right_arm";

        if(!m_oHeadTrackerPort.open(l_sHeadTrackerPortName.c_str()) ||
            !m_oTorsoTrackerPort.open(l_sTorsoTrackerPortName.c_str()) ||
            !m_oLeftArmTrackerPort.open(l_sLeftArmTrackerPortName.c_str()) ||
            !m_oFaceTrackerPort.open(l_sFaceTrackerPortName.c_str()) ||
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
                 l_oRobotRobotPosture.goToPosture("Crouch", 1.0);
             }
             else if(m_startPosture == 1)
             {
                 l_oRobotRobotPosture.goToPosture("Stand", 1.0);
             }
             else if(m_startPosture == 2)
             {
                l_oRobotRobotPosture.goToPosture("Sit", 1.0);

             }
        }
        catch (const AL::ALError& e)
        {
            std::cerr << "-ERROR: " << e.what() << std::endl;
            return false;
        }

        // set stiffnesses
        if(m_bHeadActivated)
        {
            m_oRobotMotionProxy->setStiffnesses("Head", 1.0f);
        }
        if(m_bTorsoActivated)
        {
            m_oRobotMotionProxy->setStiffnesses("Torso", 1.0f);
        }
        if(m_bLeftArmActivated)
        {
            m_oRobotMotionProxy->setStiffnesses("LArm", 1.0f);
        }
        if(m_bRightArmActivated)
        {
            m_oRobotMotionProxy->setStiffnesses("RArm", 1.0f);
        }

    return true;
}


bool SWTeleoperation_nao::interruptModule()
{
    m_oHeadTrackerPort.interrupt();
    m_oTorsoTrackerPort.interrupt();
    m_oLeftArmTrackerPort.interrupt();
    m_oRightArmTrackerPort.interrupt();
    m_oFaceTrackerPort.interrupt();

    std::cout << "--Interrupting the nao Teleoperation module..." << std::endl;
    return true;
}

void SWTeleoperation_nao::resetHeadPosition()
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
    }
    catch (const AL::ALError& e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
}

void SWTeleoperation_nao::resetTorsoPosition()
{
    if(!m_bTorsoActivated)
    {
        return;
    }

    // TO CHECK
    try
    {
        m_oRobotMotionProxy->setStiffnesses("LLeg",1.0f);
        m_oRobotMotionProxy->setStiffnesses("RLeg",1.0f);
        m_oRobotMotionProxy->setAngles(AL::ALValue("LLeg"),AL::ALValue::array(0.f,0.f,-0.2f,0.70f,-0.35f,0.f),static_cast<float>(m_dJointVelocityValue)*0.5f);
        m_oRobotMotionProxy->setAngles(AL::ALValue("RLeg"),AL::ALValue::array(0.f,0.f,-0.2f,0.70f,-0.35f,0.f),static_cast<float>(m_dJointVelocityValue)*0.5f);
        m_oRobotMotionProxy->post.angleInterpolationWithSpeed(AL::ALValue("LLeg"),AL::ALValue::array(0.f,0.f,-0.45f,0.70f,-0.35f,0.f),static_cast<float>(m_dJointVelocityValue)*0.5f);
        m_oRobotMotionProxy->post.angleInterpolationWithSpeed(AL::ALValue("RLeg"),AL::ALValue::array(0.f,0.f,-0.45f,0.70f,-0.35f,0.f),static_cast<float>(m_dJointVelocityValue)*0.5f);
    }
    catch (const AL::ALError& e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
}

void SWTeleoperation_nao::resetLeftArmPosition()
{
    if(!m_bLeftArmActivated)
    {
        return;
    }

    // TO CHECK
    m_aLArmAngles[0] = 1.3f;
    m_aLArmAngles[1] = 0.f;
    m_aLArmAngles[2] = 0.f;
    m_aLArmAngles[3] = 0.f;
    m_aLArmAngles[4] = 0.f;
    m_aLArmAngles[5] = 0.f;
    try {
        m_oRobotMotionProxy->setStiffnesses("LArm",1.0f);
        m_oRobotMotionProxy->setAngles(AL::ALValue("LArm"), m_aLArmAngles, static_cast<float>(m_dJointVelocityValue));
    }
    catch (const AL::ALError& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

}

void SWTeleoperation_nao::resetRightArmPosition()
{
    if(!m_bRightArmActivated)
    {
        return;
    }

    //  TO CHECK
    m_aRArmAngles[0] = 1.3f;
    m_aRArmAngles[1] = 0.f;
    m_aRArmAngles[2] = 0.f;
    m_aRArmAngles[3] = 0.f;
    m_aRArmAngles[4] = 0.f;
    m_aRArmAngles[5] = 0.f;
    try {
        m_oRobotMotionProxy->setStiffnesses("RArm",1.0f);
        m_oRobotMotionProxy->angleInterpolationWithSpeed(AL::ALValue("RArm"), m_aRArmAngles, static_cast<float>(m_dJointVelocityValue));
    }
    catch (const AL::ALError& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

}

bool SWTeleoperation_nao::close()
{
    resetHeadPosition();
    resetTorsoPosition();
    resetLeftArmPosition();
    resetRightArmPosition();

    // close ports
    m_oHeadTrackerPort.close();
    m_oTorsoTrackerPort.close();
    m_oLeftArmTrackerPort.close();
    m_oRightArmTrackerPort.close();
    m_oFaceTrackerPort.close();

    // set stiffnesses
    if(m_bHeadActivated)
    {
        m_oRobotMotionProxy->setStiffnesses("Head", 0.0f);
    }
    if(m_bTorsoActivated)
    {
        m_oRobotMotionProxy->setStiffnesses("Torso", 0.0f);
    }
    if(m_bLeftArmActivated)
    {
        m_oRobotMotionProxy->setStiffnesses("LArm", 0.0f);
    }
    if(m_bRightArmActivated)
    {
        m_oRobotMotionProxy->setStiffnesses("RArm", 0.0f);
    }

    // set ending posture
    ALRobotPostureProxy l_oRobotRobotPosture=  ALRobotPostureProxy(m_sRobotAddress);
    try
    {
         if(m_endPosture == 0)
         {
             l_oRobotRobotPosture.goToPosture("Crouch", 1.0);
         }
         else if(m_endPosture == 1)
         {
             l_oRobotRobotPosture.goToPosture("Stand", 1.0);
         }
         else if(m_endPosture == 2)
         {
            l_oRobotRobotPosture.goToPosture("Sit", 1.0);

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

bool SWTeleoperation_nao::updateModule()
{

//    RShoulderPitch 	Right shoulder joint front and back (Y) 	-119.5 to 119.5 	-2.0857 to 2.0857
//    RShoulderRoll 	Right shoulder joint right and left (Z) 	-76 to 18 	-1.3265 to 0.3142
//    RElbowYaw 	Right shoulder joint twist (X) 	-119.5 to 119.5 	-2.0857 to 2.0857
//    RElbowRoll 	Right elbow joint (Z) 	2 to 88.5 	0.0349 to 1.5446
//    RWristYaw 	Right wrist joint (X) 	-104.5 to 104.5 	-1.8238 to 1.8238
//    RHand 	Right hand 	Open and Close 	Open and Close

//    AL::ALValue namesRightArm  = AL::ALValue::array("RElbowRoll");
//    AL::ALValue anglesRightArm = AL::ALValue::array(0.3f, -0.3f);


    Bottle *l_pHeadTarget = NULL, *l_pTorsoTarget = NULL, *l_pLeftArmTarget = NULL, *l_pRightArmTarget = NULL, *l_pFaceTarget = NULL;

    bool l_bHeadCapture = false, l_bTorsoCapture = false, l_bLeftArmCapture = false, l_bRightArmCapture = false, l_bFaceCapture = false;

    // read head commands
    if(m_bHeadActivated)
    {
        l_pHeadTarget = m_oHeadTrackerPort.read(false);

        if(l_pHeadTarget)
        {
            int l_deviceId = l_pHeadTarget->get(0).asInt();
            switch(l_deviceId)
            {
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

                    std::vector<double> l_vecClavicles		= swUtil::vec(l_pointLShoulder,	l_pointRShoulder);
                    std::vector<double> l_vecHead			= swUtil::vec(l_pointNeck,		l_pointHead);

                    std::vector<double> l_rpyHead = swUtil::computeRollPitchYaw(l_vecHead, l_vecClavicles);

                    m_aHeadAngles[0] = swUtil::deg2rad(l_rpyHead[2]);
                    m_aHeadAngles[1] = swUtil::deg2rad(l_rpyHead[1]);
                }
                break;
                case swTracking::FOREST_LIB :
                {
                    m_aHeadAngles[0] = swUtil::deg2rad(-l_pHeadTarget->get(2).asDouble()+5); //head rotation "yes" [-40 30]
                    m_aHeadAngles[1] = swUtil::deg2rad(l_pHeadTarget->get(1).asDouble()-5); //head rotation [-70 60]
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

    //read torso commands
    if(m_bTorsoActivated)
    {
        l_pTorsoTarget = m_oTorsoTrackerPort.read(false);

        if (l_pTorsoTarget)
        {
            int l_deviceId = l_pTorsoTarget->get(0).asInt();
            switch(l_deviceId)
            {
                case swTracking::OPENNI_LIB:
                {
                    std::vector<double> l_pointTorso(3), l_pointNeck(3), l_pointLShoulder(3), l_pointRShoulder(3);
                    l_pointTorso[0] = l_pTorsoTarget->get(1).asDouble();
                    l_pointTorso[1] = l_pTorsoTarget->get(2).asDouble();
                    l_pointTorso[2] = l_pTorsoTarget->get(3).asDouble();
                    l_pointNeck[0] = l_pTorsoTarget->get(4).asDouble();
                    l_pointNeck[1] = l_pTorsoTarget->get(5).asDouble();
                    l_pointNeck[2] = l_pTorsoTarget->get(6).asDouble();
                    l_pointLShoulder[0] = l_pTorsoTarget->get(7).asDouble();
                    l_pointLShoulder[1] = l_pTorsoTarget->get(8).asDouble();
                    l_pointLShoulder[2] = l_pTorsoTarget->get(9).asDouble();
                    l_pointRShoulder[0] = l_pTorsoTarget->get(10).asDouble();
                    l_pointRShoulder[1] = l_pTorsoTarget->get(11).asDouble();
                    l_pointRShoulder[2] = l_pTorsoTarget->get(12).asDouble();

                    std::vector<double> l_vecTorso     = swUtil::vec(l_pointTorso, l_pointNeck);
                    std::vector<double> l_vecClavicles = swUtil::vec(l_pointLShoulder, l_pointRShoulder);
                    std::vector<double> l_rpyTorso     = swUtil::computeRollPitchYaw(l_vecTorso, l_vecClavicles);

                    m_aTorsoAngles[0] = -swUtil::deg2rad(l_rpyTorso[1]+28.5);
                    m_aTorsoAngles[1] = m_aTorsoAngles[0];
                }
                break;
            }

            l_bTorsoCapture = true;
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
                case swTracking::OPENNI_LIB :
                {
                    std::vector<double> l_pointTorso(3), l_pointNeck(3), l_pointLShoulder(3), l_pointLElbow(3), l_pointLHand(3);
                    l_pointTorso[0] = l_pLeftArmTarget->get(1).asDouble();
                    l_pointTorso[1] = l_pLeftArmTarget->get(2).asDouble();
                    l_pointTorso[2] = l_pLeftArmTarget->get(3).asDouble();
                    l_pointNeck[0] = l_pLeftArmTarget->get(4).asDouble();
                    l_pointNeck[1] = l_pLeftArmTarget->get(5).asDouble();
                    l_pointNeck[2] = l_pLeftArmTarget->get(6).asDouble();
                    l_pointLShoulder[0] = l_pLeftArmTarget->get(7).asDouble();
                    l_pointLShoulder[1] = l_pLeftArmTarget->get(8).asDouble();
                    l_pointLShoulder[2] = l_pLeftArmTarget->get(9).asDouble();
                    l_pointLElbow[0] = l_pLeftArmTarget->get(10).asDouble();
                    l_pointLElbow[1] = l_pLeftArmTarget->get(11).asDouble();
                    l_pointLElbow[2] = l_pLeftArmTarget->get(12).asDouble();
                    l_pointLHand[0] = l_pLeftArmTarget->get(13).asDouble();
                    l_pointLHand[1] = l_pLeftArmTarget->get(14).asDouble();
                    l_pointLHand[2] = l_pLeftArmTarget->get(15).asDouble();

                    std::vector<double> l_vecTorso = swUtil::vec(l_pointTorso, l_pointNeck);
                    std::vector<double> l_vecLForearm = swUtil::vec(l_pointLElbow, l_pointLHand);
                    std::vector<double> l_vecLArm = swUtil::vec(l_pointLShoulder, l_pointLElbow);
                    std::vector<double> l_rpyLShoulder = swUtil::computeRollPitchYaw(l_vecLArm, l_vecTorso);
                    std::vector<double> l_rpyLElbow = swUtil::computeRollPitchYaw(l_vecLForearm, l_vecLArm);


                    m_aLArmAngles[0] = swUtil::deg2rad(swUtil::degree180(l_rpyLShoulder[1] - 90.));
                    m_aLArmAngles[1] = swUtil::deg2rad(swUtil::degree180(- l_rpyLShoulder[0] - 180.));
                    //m_aLArmAngles[2] = swUtil::degree180(l_rpyLElbow[2]+105) * DEG_TO_RAD;
                    m_aLArmAngles[3] = swUtil::deg2rad(swUtil::degree180(l_rpyLElbow[0]));

                    if (m_aLArmAngles[1].getUnionValue().asFloat > PI / 2.)
                    {
                        m_aLArmAngles[1] = PI - m_aLArmAngles[1].getUnionValue().asFloat;
                        //m_aLArmAngles[2] = 0.0;
                        m_aLArmAngles[3] = swUtil::deg2rad(swUtil::degree180(l_rpyLElbow[0] - 90.));
                    }
                }
               break;

            }
            l_bLeftArmCapture = true;
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
                    // retrieve leap data
                    std::vector<double> l_vArmDirection(3,0.), l_vHandDirection(3,0.),l_vHandDirectionE(3,0.), l_vHandPalmCoord(3,0.), l_vHandPalmNormal(3,0.), l_vHandPalmNormalE(3,0.);
                    for(int ii = 0; ii < 3; ++ii)
                    {
                        l_vArmDirection[ii]     = l_pRightArmTarget->get(1 + ii).asDouble();
                        l_vHandDirection[ii]    = l_pRightArmTarget->get(4 + ii).asDouble();
                        l_vHandDirectionE[ii]   = l_pRightArmTarget->get(7 + ii).asDouble();
                        l_vHandPalmCoord[ii]    = l_pRightArmTarget->get(10 + ii).asDouble();
                        l_vHandPalmNormal[ii]   = l_pRightArmTarget->get(13 + ii).asDouble();
                        l_vHandPalmNormalE[ii]  = l_pRightArmTarget->get(16 + ii).asDouble();
                    }

                    // convert to vec3D
                        cv::Vec3d l_vecHandPalmNormal(l_vHandPalmNormal[0], l_vHandPalmNormal[1], l_vHandPalmNormal[2]);
                        cv::Vec3d l_vecHandPalmCoord(l_vHandPalmCoord[0], l_vHandPalmCoord[1], l_vHandPalmCoord[2]);
                        cv::Vec3d l_vecHandDirection(l_vHandDirection[0], l_vHandDirection[1], l_vHandDirection[2]);
                        cv::Vec3d l_vecArmDirection(l_vArmDirection[0], l_vArmDirection[1], l_vArmDirection[2]);

                    // normalize vectors
                        cv::normalize(l_vecHandPalmNormal, l_vecHandPalmNormal);
                        cv::normalize(l_vecArmDirection, l_vecArmDirection);
                        cv::normalize(l_vecHandDirection, l_vecHandDirection);

                    // convert to mat
                        cv::Mat l_matHandDirection(l_vecHandDirection);
                        cv::Mat l_matHandPalmNormal(l_vecHandPalmNormal);
                        cv::Mat l_matArmDirection(l_vecArmDirection);

                    // compute angle for wrist yaw
                        double l_dot  = l_vecArmDirection.dot(cv::Vec3d(1.0,0.0,0.0));
                        double l_angle = swUtil::rad2Deg(acos(l_dot));
                        m_aRArmAngles[3] = l_angle - 90.0; // LElbowRoll

                    //LElbowRoll 	Left elbow joint (Z) 	-88.5 to -2 	-1.5446 to -0.0349
                    // direction arm / angle par rapport à axe x

                    // LElbowRoll 3 ?


                }
                break;
                case swTracking::OPENNI_LIB :
                {
                    std::vector<double> l_pointTorso(3), l_pointNeck(3), l_pointLShoulder(3), l_pointLElbow(3), l_pointLHand(3), l_rpyRShoulder(3), l_rpyRElbow(3);
                    l_pointTorso[0] = l_pRightArmTarget->get(1).asDouble();
                    l_pointTorso[1] = l_pRightArmTarget->get(2).asDouble();
                    l_pointTorso[2] = l_pRightArmTarget->get(3).asDouble();
                    l_pointNeck[0] = l_pRightArmTarget->get(4).asDouble();
                    l_pointNeck[1] = l_pRightArmTarget->get(5).asDouble();
                    l_pointNeck[2] = l_pRightArmTarget->get(6).asDouble();
                    l_pointLShoulder[0] = l_pRightArmTarget->get(7).asDouble();
                    l_pointLShoulder[1] = l_pRightArmTarget->get(8).asDouble();
                    l_pointLShoulder[2] = l_pRightArmTarget->get(9).asDouble();
                    l_pointLElbow[0] = l_pRightArmTarget->get(10).asDouble();
                    l_pointLElbow[1] = l_pRightArmTarget->get(11).asDouble();
                    l_pointLElbow[2] = l_pRightArmTarget->get(12).asDouble();
                    l_pointLHand[0] = l_pRightArmTarget->get(13).asDouble();
                    l_pointLHand[1] = l_pRightArmTarget->get(14).asDouble();
                    l_pointLHand[2] = l_pRightArmTarget->get(15).asDouble();
                    l_rpyRShoulder[0] = l_pRightArmTarget->get(1).asDouble();
                    l_rpyRShoulder[1] = l_pRightArmTarget->get(2).asDouble();
                    l_rpyRShoulder[2] = l_pRightArmTarget->get(3).asDouble();
                    l_rpyRElbow[0] = l_pRightArmTarget->get(4).asDouble();
                    l_rpyRElbow[1] = l_pRightArmTarget->get(5).asDouble();
                    l_rpyRElbow[2] = l_pRightArmTarget->get(6).asDouble();

                    std::vector<double> l_vecTorso      = swUtil::vec(l_pointTorso, l_pointNeck);
                    std::vector<double> l_vecLForearm   = swUtil::vec(l_pointLElbow, l_pointLHand);
                    std::vector<double> l_vecLArm       = swUtil::vec(l_pointLShoulder, l_pointLElbow);
                    std::vector<double> l_rpyLShoulder  = swUtil::computeRollPitchYaw(l_vecLArm, l_vecTorso);
                    std::vector<double> l_rpyLElbow     = swUtil::computeRollPitchYaw(l_vecLForearm, l_vecLArm);

                    m_aRArmAngles[0] = swUtil::deg2rad(swUtil::degree180(l_rpyRShoulder[1] - 90.));
                    m_aRArmAngles[1] = swUtil::deg2rad(swUtil::degree180(-l_rpyRShoulder[0]-180));
                    //m_aRArmAngles[2] = swUtil::degree180(l_rpyRElbow[2]+75) * DEG_TO_RAD;
                    m_aRArmAngles[3] = swUtil::deg2rad(swUtil::degree180(l_rpyRElbow[0]));
                    if (m_aRArmAngles[1].getUnionValue().asFloat < -(PI/2.) )
                    {
                        m_aRArmAngles[1] = - PI - m_aRArmAngles[1].getUnionValue().asFloat;
                        //m_aRArmAngles[2] = 0.0;
                        m_aRArmAngles[3] = swUtil::deg2rad(swUtil::degree180(l_rpyRElbow[0]+90.));
                    }
                }
                break;
            }

            l_bRightArmCapture = true;
        }
    }

    if(m_bLEDSActivated)
    {
        l_pFaceTarget = m_oFaceTrackerPort.read(false);

        if(l_pFaceTarget)
        {
            int l_deviceId = l_pFaceTarget->get(0).asInt();

//            switch(l_deviceId)
//            {

//            }
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

    if (m_aTorsoAngles[0].getUnionValue().asFloat > static_cast<float>(m_dTorsoMaxValueJoint))
    {
        m_aTorsoAngles[0] = m_dTorsoMaxValueJoint;
        m_aTorsoAngles[1] = m_dTorsoMaxValueJoint;
    }
    if (m_aTorsoAngles[0].getUnionValue().asFloat < static_cast<float>(m_dTorsoMinValueJoint))
    {
        m_aTorsoAngles[0] = m_dTorsoMinValueJoint;
        m_aTorsoAngles[1] = m_dTorsoMinValueJoint;
    }

    if (l_bHeadCapture)
    {
        m_oRobotMotionProxy->setAngles(AL::ALValue("Head"), m_aHeadAngles, static_cast<float>(m_dJointVelocityValue));
    }

    if (l_bTorsoCapture)
    {
        m_oRobotMotionProxy->setAngles(AL::ALValue::array("LHipPitch","RHipPitch"), m_aTorsoAngles, static_cast<float>(m_dJointVelocityValue));
    }

    if (l_bLeftArmCapture)
    {
        m_oRobotMotionProxy->setAngles(AL::ALValue("LArm"), m_aLArmAngles, static_cast<float>(m_dJointVelocityValue));
    }

    if (l_bRightArmCapture)
    {       
        m_oRobotMotionProxy->setAngles(AL::ALValue("RArm"), m_aRArmAngles, static_cast<float>(m_dJointVelocityValue));
    }

    return true;
}


double SWTeleoperation_nao::getPeriod()
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
    SWTeleoperation_nao l_oTeleoperation_nao;

    // prepare and configure the resource finder
    yarp::os::ResourceFinder l_oRf;
    l_oRf.setVerbose(true);
    l_oRf.setDefaultConfigFile("teleoperation_nao.ini");
    l_oRf.setDefaultContext("swooz-teleoperation/conf");
    l_oRf.configure("NAO_ROOT", argc, argv);

    // configure the module
    std::cout << "Configuring the nao Teleoperation module..."<< std::endl;
    if (l_oTeleoperation_nao.configure(l_oRf))
    {
        // run the module
        std::cout << "Starting the nao Teleoperation module..." << std::endl;
        l_oTeleoperation_nao.runModule();
    }
    else
    {
        std::cerr << "Failed to configure the nao Teleoperation module!"<< std::endl;
    }

    return 0;
}

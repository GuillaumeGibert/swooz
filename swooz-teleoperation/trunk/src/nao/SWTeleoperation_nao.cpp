/**
 * \file SWTeleoperation_nao.cpp
 * \author Emmanuel Body
 * \date 21/01/2014
 * \brief ...
 */

#include "nao/SWTeleoperation_nao.h"

#include "geometryUtility.h"

#include "SWTrackingDevice.h"

//#include <iCub/ctrl/math.h>

#include <sstream>
#include <vector>
#include <cmath>
#include <iostream>

#define PI 3.14159
#define DEG_TO_RAD PI / 180.f;


SWTeleoperation_nao::SWTeleoperation_nao() :  m_i32HeadTimeLastBottle(0), m_bFastrakCalibrated(false){}

SWTeleoperation_nao::~SWTeleoperation_nao()
{}

bool SWTeleoperation_nao::configure(ResourceFinder &rf)
{

    // gets the module name which will form the stem of all module port names
        m_sModuleName   = rf.check("name", Value("teleoperation_nao"), "Teleoperation/nao Module name (string)").asString();
         setName(m_sModuleName.c_str());
         m_sRobotAddress = rf.check("IP", Value("169.254.108.110"), "IP Adress of the Nao Robot").asString();

    // acceleration/speeds values for nao
        m_fJointVelocityValue     = (float)rf.check("jointVelocityValue",      Value(0.1),  "Joint Velocity Value (float)").asDouble();

    // min / max values for nao joints
        // head
        //  min
        m_fHeadMinValueJoint0 = (float)rf.check("headMinValueJoint0",  Value(-2.f), "Minimum Joint0 Value (float)").asDouble();
        m_fHeadMinValueJoint1 = (float)rf.check("headMinValueJoint1",  Value(-0.5f), "Minimum Joint1 Value (float)").asDouble();
        //  max
        m_fHeadMaxValueJoint0 = (float)rf.check("headMaxValueJoint0",  Value( 2.f), "Maximum Joint0 Value (float)").asDouble();
        m_fHeadMaxValueJoint1 = (float)rf.check("headMaxValueJoint1",  Value( 0.5f), "Maximum Joint1 Value (float)").asDouble();

        // left arm
        //  min
        m_fLeftArmMinValueJoint0 = (float)rf.check("leftArmMinValueJoint0",  Value(-2.0f), "Minimum Joint0 Value (float)").asDouble();
        m_fLeftArmMinValueJoint1 = (float)rf.check("leftArmMinValueJoint1",  Value(-0.31f),"Minimum Joint1 Value (float)").asDouble();
        m_fLeftArmMinValueJoint2 = (float)rf.check("leftArmMinValueJoint2",  Value(-2.0f), "Minimum Joint2 Value (float)").asDouble();
        m_fLeftArmMinValueJoint3 = (float)rf.check("leftArmMinValueJoint3",  Value(-1.5f), "Minimum Joint3 Value (float)").asDouble();
        m_fLeftArmMinValueJoint4 = (float)rf.check("leftArmMinValueJoint4",  Value(-1.8f), "Minimum Joint4 Value (float)").asDouble();
        m_fLeftArmMinValueJoint5 = (float)rf.check("leftArmMinValueJoint5",  Value(  0.f), "Minimum Joint5 Value (float)").asDouble();
        //  max
        m_fLeftArmMaxValueJoint0 = (float)rf.check("leftArmMaxValueJoint0",  Value( 1.9f), "Maximum Joint0 Value (float)").asDouble();
        m_fLeftArmMaxValueJoint1 = (float)rf.check("leftArmMaxValueJoint1",  Value( 1.3f), "Maximum Joint1 Value (float)").asDouble();
        m_fLeftArmMaxValueJoint2 = (float)rf.check("leftArmMaxValueJoint2",  Value( 2.0f), "Maximum Joint2 Value (float)").asDouble();
        m_fLeftArmMaxValueJoint3 = (float)rf.check("leftArmMaxValueJoint3",  Value( 0.0f), "Maximum Joint3 Value (float)").asDouble();
        m_fLeftArmMaxValueJoint4 = (float)rf.check("leftArmMaxValueJoint4",  Value( 1.8f), "Maximum Joint4 Value (float)").asDouble();
        m_fLeftArmMaxValueJoint5 = (float)rf.check("leftArmMaxValueJoint5",  Value( 1.0f), "Maximum Joint5 Value (float)").asDouble();

        // rigth arm
        //  min
        m_fRightArmMinValueJoint0 = (float)rf.check("rightArmMinValueJoint0",  Value(-2.0f), "Minimum Joint0 Value (float)").asDouble();
        m_fRightArmMinValueJoint1 = (float)rf.check("rightArmMinValueJoint1",  Value(-1.3f), "Minimum Joint1 Value (float)").asDouble();
        m_fRightArmMinValueJoint2 = (float)rf.check("rightArmMinValueJoint2",  Value(-2.0f), "Minimum Joint2 Value (float)").asDouble();
        m_fRightArmMinValueJoint3 = (float)rf.check("rightArmMinValueJoint3",  Value( 0.0f), "Minimum Joint3 Value (float)").asDouble();
        m_fRightArmMinValueJoint4 = (float)rf.check("rightArmMinValueJoint4",  Value(-1.8f), "Minimum Joint4 Value (float)").asDouble();
        m_fRightArmMinValueJoint5 = (float)rf.check("rightArmMinValueJoint5",  Value(  0.f), "Minimum Joint5 Value (float)").asDouble();
        //  max
        m_fRightArmMaxValueJoint0 = (float)rf.check("rightArmMaxValueJoint0",  Value( 1.9f), "Maximum Joint0 Value (float)").asDouble();
        m_fRightArmMaxValueJoint1 = (float)rf.check("rightArmMaxValueJoint1",  Value( 0.31f),"Maximum Joint1 Value (float)").asDouble();
        m_fRightArmMaxValueJoint2 = (float)rf.check("rightArmMaxValueJoint2",  Value( 2.0f), "Maximum Joint2 Value (float)").asDouble();
        m_fRightArmMaxValueJoint3 = (float)rf.check("rightArmMaxValueJoint3",  Value( 1.5f), "Maximum Joint3 Value (float)").asDouble();
        m_fRightArmMaxValueJoint4 = (float)rf.check("rightArmMaxValueJoint4",  Value( 1.8f), "Maximum Joint4 Value (float)").asDouble();
        m_fRightArmMaxValueJoint5 = (float)rf.check("rightArmMaxValueJoint5",  Value( 1.0f), "Maximum Joint5 Value (float)").asDouble();

        // torso
        //  min
        m_fTorsoMinValueJoint0 = (float)rf.check("torsoMinValueJoint0",  Value(-1.0f), "Minimum Joint0 Value (float)").asDouble();
        //  max
        m_fTorsoMaxValueJoint0 = (float)rf.check("torsoMaxValueJoint0",  Value(-0.45f), "Maximum Joint1 Value (float)").asDouble();


    // miscellaneous
        m_i32Fps                    = rf.check("fps",                   Value(100),  "Frame per second (int)").asInt();
        m_i32HeadTimeoutReset       = rf.check("headTimeoutReset",      Value(3000), "Head gaze timeout reset iCub (int)").asInt();


        m_aHeadAngles.arraySetSize(2);
        m_aTorsoAngles.arraySetSize(2);
        m_aLArmAngles.arraySetSize(6);
        m_aRArmAngles.arraySetSize(6);
        m_aLLegAngles.arraySetSize(6);
        m_aRLegAngles.arraySetSize(6);


    // init ports
        m_sHeadTrackerPortName          = "/teleoperation/nao/head";
        m_sTorsoTrackerPortName         = "/teleoperation/nao/torso";

        m_sLeftArmTrackerPortName       = "/teleoperation/nao/left_arm/arm";
        m_sLeftHandTrackerPortName      = "/teleoperation/nao/left_arm/hand";
        m_sLeftFingersTrackerPortName   = "/teleoperation/nao/left_arm/fingers";

        m_sRightArmTrackerPortName      = "/teleoperation/nao/right_arm/arm";
        m_sRightHandTrackerPortName     = "/teleoperation/nao/right_arm/hand";
        m_sRightFingersTrackerPortName  = "/teleoperation/nao/right_arm/fingers";

        if(!m_oHeadTrackerPort.open(m_sHeadTrackerPortName.c_str()) ||
           //!m_oGazeTrackerPort.open(m_sGazeTrackerPortName.c_str()) ||
           //!m_oFaceTrackerPort.open(m_sFaceTrackerPortName.c_str()) ||
           !m_oTorsoTrackerPort.open(m_sTorsoTrackerPortName.c_str()) ||
           //!m_oFaceHandlerPort.open(m_sEyelidOutputPortName.c_str()) ||
            !m_oLeftArmTrackerPort.open(m_sLeftArmTrackerPortName.c_str()) ||
            !m_oLeftHandTrackerPort.open(m_sLeftHandTrackerPortName.c_str()) ||
            !m_oLeftFingersTrackerPort.open(m_sLeftFingersTrackerPortName.c_str()) ||
            !m_oRightArmTrackerPort.open(m_sRightArmTrackerPortName.c_str()) ||
            !m_oRightHandTrackerPort.open(m_sRightHandTrackerPortName.c_str()) ||
            !m_oRightFingersTrackerPort.open(m_sRightFingersTrackerPortName.c_str())
            )
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

        m_oRobotMotionProxy->setStiffnesses("Body", 1.0f);

        try	{
            ALRobotPostureProxy l_oRobotRobotPosture=  ALRobotPostureProxy(m_sRobotAddress);
            l_oRobotRobotPosture.goToPosture("Stand", 1.0);
        }
        catch (const AL::ALError& e){
            std::cerr << "-ERROR: " << e.what() << std::endl;
            return false;
        }

        resetHeadPosition();
        resetTorsoPosition();
        resetLeftArmPosition();
        resetRightArmPosition();

        m_bHeadCapture = m_bTorsoCapture = m_bLeftArmCapture = m_bRightArmCapture = false;

    return true;
}


bool SWTeleoperation_nao::interruptModule()
{
    m_oHeadTrackerPort.interrupt();
    //m_oGazeTrackerPort.interrupt();
    //m_oFaceTrackerPort.interrupt();
    m_oTorsoTrackerPort.interrupt();
    //m_oFaceHandlerPort.interrupt();
    m_oLeftHandTrackerPort.interrupt();
    m_oLeftFingersTrackerPort.interrupt();
    m_oRightHandTrackerPort.interrupt();
    m_oRightFingersTrackerPort.interrupt();

    std::cout << "--Interrupting the iCub Teleoperation module..." << std::endl;
    return true;
}

void SWTeleoperation_nao::resetHeadPosition()
{
    std::cout << "resetH" << std::endl;
    m_aHeadAngles[0] = 0.f;
    m_aHeadAngles[1] = 0.f;

    try {
        m_oRobotMotionProxy->setStiffnesses("Head",1.0f);
        m_oRobotMotionProxy->setAngles(AL::ALValue("Head"), m_aHeadAngles, m_fJointVelocityValue);
    }
    catch (const AL::ALError& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

    // m_oRobotMotionProxy->angleInterpolationWithSpeed(AL::ALValue("Head"), m_aHeadAngles, m_fJointVelocityValue);
}

void SWTeleoperation_nao::resetTorsoPosition()
{
    std::cout << "resetT" << std::endl;

    try {
            m_oRobotMotionProxy->setStiffnesses("LLeg",1.0f);
            m_oRobotMotionProxy->setStiffnesses("RLeg",1.0f);
            m_oRobotMotionProxy->setAngles(AL::ALValue("LLeg"),AL::ALValue::array(0.f,0.f,-0.2f,0.70f,-0.35f,0.f),m_fJointVelocityValue*0.5f);
            m_oRobotMotionProxy->setAngles(AL::ALValue("RLeg"),AL::ALValue::array(0.f,0.f,-0.2f,0.70f,-0.35f,0.f),m_fJointVelocityValue*0.5f);
            m_oRobotMotionProxy->post.angleInterpolationWithSpeed(AL::ALValue("LLeg"),AL::ALValue::array(0.f,0.f,-0.45f,0.70f,-0.35f,0.f),m_fJointVelocityValue*0.5f);
            m_oRobotMotionProxy->post.angleInterpolationWithSpeed(AL::ALValue("RLeg"),AL::ALValue::array(0.f,0.f,-0.45f,0.70f,-0.35f,0.f),m_fJointVelocityValue*0.5f);
    }
    catch (const AL::ALError& e) {
            std::cerr << "Caught exception: " << e.what() << std::endl;
    }
}

void SWTeleoperation_nao::resetLeftArmPosition()
{
    std::cout << "resetLA" << std::endl;
    m_aLArmAngles[0] = 1.3f;
    m_aLArmAngles[1] = 0.f;
    m_aLArmAngles[2] = 0.f;
    m_aLArmAngles[3] = 0.f;
    m_aLArmAngles[4] = 0.f;
    m_aLArmAngles[5] = 0.f;
    try {
        m_oRobotMotionProxy->setStiffnesses("LArm",1.0f);
        m_oRobotMotionProxy->setAngles(AL::ALValue("LArm"), m_aLArmAngles, m_fJointVelocityValue);
    }
    catch (const AL::ALError& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }

}

void SWTeleoperation_nao::resetRightArmPosition()
{
    std::cout << "resetRA" << std::endl;
    m_aRArmAngles[0] = 1.3f;
    m_aRArmAngles[1] = 0.f;
    m_aRArmAngles[2] = 0.f;
    m_aRArmAngles[3] = 0.f;
    m_aRArmAngles[4] = 0.f;
    m_aRArmAngles[5] = 0.f;
    try {
        m_oRobotMotionProxy->setStiffnesses("RArm",1.0f);
        m_oRobotMotionProxy->angleInterpolationWithSpeed(AL::ALValue("RArm"), m_aRArmAngles, m_fJointVelocityValue);
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
    m_oLeftHandTrackerPort.close();
    m_oLeftFingersTrackerPort.close();
    m_oRightArmTrackerPort.close();
    m_oRightHandTrackerPort.close();
    m_oRightFingersTrackerPort.close();

    try	{
        ALRobotPostureProxy l_oRobotRobotPosture=  ALRobotPostureProxy(m_sRobotAddress);
        l_oRobotRobotPosture.goToPosture("Crouch", 1.0);
    }
    catch (const AL::ALError& e){
        std::cerr << "-ERROR: " << e.what() << std::endl;
        return false;
    }

    m_oRobotMotionProxy->setStiffnesses("Body", 0.0f);

    delete m_oRobotMotionProxy;

    return true;
}

bool SWTeleoperation_nao::updateModule()
{

    Bottle *l_pHeadTarget = NULL, *l_pTorsoTarget = NULL, *l_pLeftArmTarget = NULL, *l_pLeftHandTarget = NULL, *l_pLeftFingersTarget = NULL, *l_pRightArmTarget = NULL, *l_pRightHandTarget = NULL, *l_pRightFingersTarget = NULL;

    // read head commands
    {

        l_pHeadTarget = m_oHeadTrackerPort.read(false);

        if(l_pHeadTarget)
        {
            int l_deviceId = l_pHeadTarget->get(0).asInt();
            switch(l_deviceId)
            {

                case swTracking::OPENNI_LIB :
                    {
                        std::vector<double> l_pointNeck(3);
                            l_pointNeck[0] = l_pHeadTarget->get(1).asDouble();
                            l_pointNeck[1] = l_pHeadTarget->get(2).asDouble();
                            l_pointNeck[2] = l_pHeadTarget->get(3).asDouble();
                        std::vector<double> l_pointHead(3);
                            l_pointHead[0] = l_pHeadTarget->get(4).asDouble();
                            l_pointHead[1] = l_pHeadTarget->get(5).asDouble();
                            l_pointHead[2] = l_pHeadTarget->get(6).asDouble();
                        std::vector<double> l_pointLShoulder(3);
                            l_pointLShoulder[0] = l_pHeadTarget->get(7).asDouble();
                            l_pointLShoulder[1] = l_pHeadTarget->get(8).asDouble();
                            l_pointLShoulder[2] = l_pHeadTarget->get(9).asDouble();
                        std::vector<double> l_pointRShoulder(3);
                            l_pointRShoulder[0] = l_pHeadTarget->get(10).asDouble();
                            l_pointRShoulder[1] = l_pHeadTarget->get(11).asDouble();
                            l_pointRShoulder[2] = l_pHeadTarget->get(12).asDouble();

                        std::vector<double> l_vecClavicles		= swUtil::vec(l_pointLShoulder,	l_pointRShoulder);
                        std::vector<double> l_vecHead			= swUtil::vec(l_pointNeck,		l_pointHead);

                        std::vector<double> l_rpyHead = swUtil::computeRollPitchYaw(l_vecHead, l_vecClavicles);

                        m_aHeadAngles[0] = l_rpyHead[2] * DEG_TO_RAD;
                        m_aHeadAngles[1] = l_rpyHead[1] * DEG_TO_RAD;
                    }
                break;
            }

            m_i32HeadTimeLastBottle = 0;
            m_bHeadCapture = true;
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
    l_pTorsoTarget = m_oTorsoTrackerPort.read(false);
    if (l_pTorsoTarget)
    {
        int l_deviceId = l_pTorsoTarget->get(0).asInt();
        switch(l_deviceId)
        {
            case swTracking::OPENNI_LIB:
                std::vector<double> l_pointTorso(3);
                    l_pointTorso[0] = l_pTorsoTarget->get(1).asDouble();
                    l_pointTorso[1] = l_pTorsoTarget->get(2).asDouble();
                    l_pointTorso[2] = l_pTorsoTarget->get(3).asDouble();
                std::vector<double> l_pointNeck(3);
                    l_pointNeck[0] = l_pTorsoTarget->get(4).asDouble();
                    l_pointNeck[1] = l_pTorsoTarget->get(5).asDouble();
                    l_pointNeck[2] = l_pTorsoTarget->get(6).asDouble();
                std::vector<double> l_pointLShoulder(3);
                    l_pointLShoulder[0] = l_pTorsoTarget->get(7).asDouble();
                    l_pointLShoulder[1] = l_pTorsoTarget->get(8).asDouble();
                    l_pointLShoulder[2] = l_pTorsoTarget->get(9).asDouble();
                std::vector<double> l_pointRShoulder(3);
                    l_pointRShoulder[0] = l_pTorsoTarget->get(10).asDouble();
                    l_pointRShoulder[1] = l_pTorsoTarget->get(11).asDouble();
                    l_pointRShoulder[2] = l_pTorsoTarget->get(12).asDouble();

                std::vector<double> l_vecTorso = swUtil::vec(l_pointTorso, l_pointNeck);
                std::vector<double> l_vecClavicles = swUtil::vec(l_pointLShoulder, l_pointRShoulder);

                std::vector<double> l_rpyTorso = swUtil::computeRollPitchYaw(l_vecTorso, l_vecClavicles);

                m_aTorsoAngles[0] = -(l_rpyTorso[1]+28.5)*DEG_TO_RAD;
                m_aTorsoAngles[1] = m_aTorsoAngles[0];
            break;
        }
        m_bTorsoCapture = true;
    }

    //read left arm
    l_pLeftArmTarget = m_oLeftArmTrackerPort.read(false);

    if(l_pLeftArmTarget)
    {
        int l_deviceId = l_pLeftArmTarget->get(0).asInt();
     //   std::cout << "Recieved Left Arm data (device " << l_deviceId << ")" << std::endl;

        switch(l_deviceId)
        {
        case swTracking::OPENNI_LIB :
            std::vector<double> l_pointTorso(3);
                l_pointTorso[0] = l_pLeftArmTarget->get(1).asDouble();
                l_pointTorso[1] = l_pLeftArmTarget->get(2).asDouble();
                l_pointTorso[2] = l_pLeftArmTarget->get(3).asDouble();
            std::vector<double> l_pointNeck(3);
                l_pointNeck[0] = l_pLeftArmTarget->get(4).asDouble();
                l_pointNeck[1] = l_pLeftArmTarget->get(5).asDouble();
                l_pointNeck[2] = l_pLeftArmTarget->get(6).asDouble();
            std::vector<double> l_pointLShoulder(3);
                l_pointLShoulder[0] = l_pLeftArmTarget->get(7).asDouble();
                l_pointLShoulder[1] = l_pLeftArmTarget->get(8).asDouble();
                l_pointLShoulder[2] = l_pLeftArmTarget->get(9).asDouble();
            std::vector<double> l_pointLElbow(3);
                l_pointLElbow[0] = l_pLeftArmTarget->get(10).asDouble();
                l_pointLElbow[1] = l_pLeftArmTarget->get(11).asDouble();
                l_pointLElbow[2] = l_pLeftArmTarget->get(12).asDouble();
            std::vector<double> l_pointLHand(3);
                l_pointLHand[0] = l_pLeftArmTarget->get(13).asDouble();
                l_pointLHand[1] = l_pLeftArmTarget->get(14).asDouble();
                l_pointLHand[2] = l_pLeftArmTarget->get(15).asDouble();

            std::vector<double> l_vecTorso = swUtil::vec(l_pointTorso, l_pointNeck);
            std::vector<double> l_vecLForearm = swUtil::vec(l_pointLElbow, l_pointLHand);
            std::vector<double> l_vecLArm = swUtil::vec(l_pointLShoulder, l_pointLElbow);

            std::vector<double> l_rpyLShoulder = swUtil::computeRollPitchYaw(l_vecLArm, l_vecTorso);
            std::vector<double> l_rpyLElbow = swUtil::computeRollPitchYaw(l_vecLForearm, l_vecLArm);


            // std::cout << "RECIEVED : " << swUtil::degree180(l_rpyLElbow[0] - 90.) << std::endl;
            m_aLArmAngles[0] = swUtil::degree180(l_rpyLShoulder[1] - 90.) * DEG_TO_RAD;
            m_aLArmAngles[1] = swUtil::degree180(- l_rpyLShoulder[0] - 180.) * DEG_TO_RAD;
            //m_aLArmAngles[2] = swUtil::degree180(l_rpyLElbow[2]+105) * DEG_TO_RAD;
            m_aLArmAngles[3] = swUtil::degree180(l_rpyLElbow[0]) * DEG_TO_RAD;
            if (m_aLArmAngles[1].getUnionValue().asFloat > PI / 2.){
                m_aLArmAngles[1] = PI - m_aLArmAngles[1].getUnionValue().asFloat;
                //m_aLArmAngles[2] = 0.0;
                m_aLArmAngles[3] = swUtil::degree180(l_rpyLElbow[0] - 90.) * DEG_TO_RAD;}

            std::cout << m_aLArmAngles.toString() << std::endl;

           break;

        }
        m_bLeftArmCapture = true;
    }

    //read right arm
    l_pRightArmTarget = m_oRightArmTrackerPort.read(false);

    if(l_pRightArmTarget)
    {
        int l_deviceId = l_pRightArmTarget->get(0).asInt();
        std::cout << "Recieved Right Arm data (device " << l_deviceId << ")" << std::endl;

        switch(l_deviceId)
        {
            case swTracking::FORTH_LIB :
                    // ...
            break;
            case swTracking::FASTRAK_LIB :
                    // ...
            break;
        case swTracking::OPENNI_LIB :
            std::vector<double> l_pointTorso(3);
                l_pointTorso[0] = l_pRightArmTarget->get(1).asDouble();
                l_pointTorso[1] = l_pRightArmTarget->get(2).asDouble();
                l_pointTorso[2] = l_pRightArmTarget->get(3).asDouble();
            std::vector<double> l_pointNeck(3);
                l_pointNeck[0] = l_pRightArmTarget->get(4).asDouble();
                l_pointNeck[1] = l_pRightArmTarget->get(5).asDouble();
                l_pointNeck[2] = l_pRightArmTarget->get(6).asDouble();
            std::vector<double> l_pointLShoulder(3);
                l_pointLShoulder[0] = l_pRightArmTarget->get(7).asDouble();
                l_pointLShoulder[1] = l_pRightArmTarget->get(8).asDouble();
                l_pointLShoulder[2] = l_pRightArmTarget->get(9).asDouble();
            std::vector<double> l_pointLElbow(3);
                l_pointLElbow[0] = l_pRightArmTarget->get(10).asDouble();
                l_pointLElbow[1] = l_pRightArmTarget->get(11).asDouble();
                l_pointLElbow[2] = l_pRightArmTarget->get(12).asDouble();
            std::vector<double> l_pointLHand(3);
                l_pointLHand[0] = l_pRightArmTarget->get(13).asDouble();
                l_pointLHand[1] = l_pRightArmTarget->get(14).asDouble();
                l_pointLHand[2] = l_pRightArmTarget->get(15).asDouble();

            std::vector<double> l_rpyRShoulder(3);
                            l_rpyRShoulder[0] = l_pRightArmTarget->get(1).asDouble();
                            l_rpyRShoulder[1] = l_pRightArmTarget->get(2).asDouble();
                            l_rpyRShoulder[2] = l_pRightArmTarget->get(3).asDouble();

            std::vector<double> l_rpyRElbow(3);
                            l_rpyRElbow[0] = l_pRightArmTarget->get(4).asDouble();
                            l_rpyRElbow[1] = l_pRightArmTarget->get(5).asDouble();
                            l_rpyRElbow[2] = l_pRightArmTarget->get(6).asDouble();


            std::vector<double> l_vecTorso = swUtil::vec(l_pointTorso, l_pointNeck);
            std::vector<double> l_vecLForearm = swUtil::vec(l_pointLElbow, l_pointLHand);
            std::vector<double> l_vecLArm = swUtil::vec(l_pointLShoulder, l_pointLElbow);

            std::vector<double> l_rpyLShoulder = swUtil::computeRollPitchYaw(l_vecLArm, l_vecTorso);
            std::vector<double> l_rpyLElbow = swUtil::computeRollPitchYaw(l_vecLForearm, l_vecLArm);

            m_aRArmAngles[0] = swUtil::degree180(l_rpyRShoulder[1] - 90.) * DEG_TO_RAD;
            m_aRArmAngles[1] = swUtil::degree180(-l_rpyRShoulder[0]-180) * DEG_TO_RAD;
            //m_aRArmAngles[2] = swUtil::degree180(l_rpyRElbow[2]+75) * DEG_TO_RAD;
            m_aRArmAngles[3] = swUtil::degree180(l_rpyRElbow[0]) * DEG_TO_RAD;
            if (m_aRArmAngles[1].getUnionValue().asFloat < -(PI/2.) ){
                m_aRArmAngles[1] = - PI - m_aRArmAngles[1].getUnionValue().asFloat;
                //m_aRArmAngles[2] = 0.0;
                m_aRArmAngles[3] = swUtil::degree180(l_rpyRElbow[0]+90.) * DEG_TO_RAD;
            }


            break;
        }
        m_bRightArmCapture = true;
    }

    //read right hand
    l_pLeftHandTarget = m_oLeftHandTrackerPort.read(false);

    if(l_pLeftHandTarget)
    {
        int l_deviceId = l_pLeftHandTarget->get(0).asInt();

        //std::cout << "Recieved Left Hand data (device " << l_deviceId << ")" << std::endl;
        switch(l_deviceId)
        {
        case swTracking::FASTRAK_LIB:
            {

                if (!m_bFastrakCalibrated)
                {
                    std::cout << "Place your hand at the level of your belly button and press enter";
                    std::cin.ignore();
                    m_vFastrakOffsets.resize(3);
                    m_vFastrakOffsets[0] = l_pLeftHandTarget->get(2).asDouble();
                    m_vFastrakOffsets[1] = l_pLeftHandTarget->get(1).asDouble();
                    m_vFastrakOffsets[2] = l_pLeftHandTarget->get(3).asDouble();
                    m_bFastrakCalibrated = true;
                }


                //cartesian position
                Vector l_xd(3);
                //cartesian orientation
                Vector l_ori(3);
                Vector l_od(4);

                Vector l_joints;

                //std::cout << "current : "<< l_xd.toString() << std::endl;

                l_xd[0] = (l_pLeftHandTarget->get(2).asDouble() - m_vFastrakOffsets[0]) / (- 70.);
                l_xd[1] = (l_pLeftHandTarget->get(1).asDouble() - m_vFastrakOffsets[1]) / (-158.);
                l_xd[2] = (l_pLeftHandTarget->get(3).asDouble() - m_vFastrakOffsets[2]) / ( 250.);

                double l_degToRad = 3.14159 / 180;

                l_ori[0] = l_degToRad * (-l_pLeftHandTarget->get(4).asDouble() + 90);
                l_ori[1] = l_degToRad * (-l_pLeftHandTarget->get(5).asDouble());
                l_ori[2] = l_degToRad * (-l_pLeftHandTarget->get(6).asDouble());

                //std::cout << "new : "<< l_xd.toString() << std::endl;


        //        l_od = dcm2axis(euler2dcm(l_ori));



//                    m_pILeftArmCartesian->goToPose(l_xd,l_od, 4.);

                 //std::cout << l_joints.subVector(7,9).toString(2) << " (askforPose)" << std::endl;
                // std::cout << l_ori.toString(0) << " (ori)" << std::endl;

//                    m_vLeftArmJoints[0] = l_joints[3];
//                    m_vLeftArmJoints[1] = l_joints[4];
//                    m_vLeftArmJoints[2] = l_joints[5];
//                    m_vLeftArmJoints[3] = l_joints[6];
//                    m_vLeftArmJoints[4] = l_joints[7];
//                    m_vLeftArmJoints[5] = l_joints[8];
//                    m_vLeftArmJoints[6] = l_joints[9];

                std::cout << m_aLArmAngles.toString() << "\t";

                m_bLeftArmCapture = true;

            }
        break;
        }
    }

    //read right hand
//    l_pRightHandTarget = m_oRightHandTrackerPort.read(false);

//    if(l_pRightArmTarget)
//    {
//        int l_deviceId = l_pRightArmTarget->get(0).asInt();


//        switch(l_deviceId)
//        {
//        case swTracking::FASTRAK_LIB:
//            {
//                //cartesian position
//                Vector l_xd(3);
//                //cartesian orientation
//                Vector l_ori(3);
//                Vector l_od(4);

//                Vector l_joints;

//                l_xd[0] = (l_pRightArmTarget->get(2).asDouble() + 6.) / (- 70.);
//                l_xd[1] = (l_pRightArmTarget->get(1).asDouble() - 21.) / (-158.);
//                l_xd[2] = (l_pRightArmTarget->get(3).asDouble() + 51) / ( 250.);

//                l_ori[0] = -l_pRightArmTarget->get(4).asDouble() + 90.;
//                l_ori[1] = -l_pRightArmTarget->get(5).asDouble();
//                l_ori[2] = -l_pRightArmTarget->get(6).asDouble();

//                l_od = dcm2axis(euler2dcm(l_ori));

//                std::cout << "l_xd :" << l_xd.toString() << std::endl;

//                m_pIRightArmCartesian->askForPose(l_xd,l_od,l_xd,l_od,l_joints);

//                std::cout << l_joints.toString() << " (askforPose)" << std::endl;


//                //m_bRightArmCapture = true;
//            }
//            break;
//        }
//    }

    // read left fingers commands
    //{
        //l_pLeftFingersTarget = m_oLeftFingersTrackerPort.read(false);

//		if(l_pLeftFingersTarget)
//		{
//			int l_deviceId = l_pLeftFingersTarget->get(0).asInt();

//			switch(l_deviceId)
//			{
//				case swTracking::FORTH_LIB :
//					{
//						// ...
//					}
//				break;
//			}
//		}
//	}
//    // read right arm commands
//    {
//        l_pRightArmTarget = m_oRightArmTrackerPort.read();

//        if(l_pRightArmTarget)
//        {
//            int l_deviceId = l_pRightArmTarget->get(0).asInt();

//            switch(l_deviceId)
//            {
//                case swTracking::FORTH_LIB :
//                    {
//                        // ...
//                    }
//                break;
//                case swTracking::OPENNI_LIB :
//                    {
//                        // ...
//                    }
//                break;
//            }
//        }
//    }

    //check each joint value to ensure it is in the right range, if not crop to the max/min values

    if (m_aHeadAngles[0].getUnionValue().asFloat > m_fHeadMaxValueJoint0)
                 m_aHeadAngles[0] = m_fHeadMaxValueJoint0;
         if (m_aHeadAngles[0].getUnionValue().asFloat < m_fHeadMinValueJoint0)
                 m_aHeadAngles[0] = m_fHeadMinValueJoint0;

         if (m_aHeadAngles[1].getUnionValue().asFloat > m_fHeadMaxValueJoint1)
                 m_aHeadAngles[1] = m_fHeadMaxValueJoint1;
         if (m_aHeadAngles[1].getUnionValue().asFloat < m_fHeadMinValueJoint1)
                 m_aHeadAngles[1] = m_fHeadMinValueJoint1;

         if (m_aLArmAngles[0].getUnionValue().asFloat > m_fLeftArmMaxValueJoint0)
                 m_aLArmAngles[0] = m_fLeftArmMaxValueJoint0;
         if (m_aLArmAngles[0].getUnionValue().asFloat < m_fLeftArmMinValueJoint0)
                 m_aLArmAngles[0] = m_fLeftArmMinValueJoint0;

         if (m_aLArmAngles[1].getUnionValue().asFloat > m_fLeftArmMaxValueJoint1)
                 m_aLArmAngles[1] = m_fLeftArmMaxValueJoint1;
         if (m_aLArmAngles[1].getUnionValue().asFloat < m_fLeftArmMinValueJoint1)
                 m_aLArmAngles[1] = m_fLeftArmMinValueJoint1;

         if (m_aLArmAngles[2].getUnionValue().asFloat > m_fLeftArmMaxValueJoint2)
                 m_aLArmAngles[2] = m_fLeftArmMaxValueJoint2;
         if (m_aLArmAngles[2].getUnionValue().asFloat < m_fLeftArmMinValueJoint2)
                 m_aLArmAngles[2] = m_fLeftArmMinValueJoint2;

         if (m_aLArmAngles[3].getUnionValue().asFloat > m_fLeftArmMaxValueJoint3)
                 m_aLArmAngles[3] = m_fLeftArmMaxValueJoint3;
         if (m_aLArmAngles[3].getUnionValue().asFloat < m_fLeftArmMinValueJoint3)
                 m_aLArmAngles[3] = m_fLeftArmMinValueJoint3;

         if (m_aLArmAngles[4].getUnionValue().asFloat > m_fLeftArmMaxValueJoint4)
                 m_aLArmAngles[4] = m_fLeftArmMaxValueJoint4;
         if (m_aLArmAngles[4].getUnionValue().asFloat < m_fLeftArmMinValueJoint4)
                 m_aLArmAngles[4] = m_fLeftArmMinValueJoint4;

         if (m_aLArmAngles[5].getUnionValue().asFloat > m_fLeftArmMaxValueJoint5)
                 m_aLArmAngles[5] = m_fLeftArmMaxValueJoint5;
         if (m_aLArmAngles[5].getUnionValue().asFloat < m_fLeftArmMinValueJoint5)
                 m_aLArmAngles[5] = m_fLeftArmMinValueJoint5;

         if (m_aRArmAngles[0].getUnionValue().asFloat > m_fRightArmMaxValueJoint0)
                 m_aRArmAngles[0] = m_fRightArmMaxValueJoint0;
         if (m_aRArmAngles[0].getUnionValue().asFloat < m_fRightArmMinValueJoint0)
                 m_aRArmAngles[0] = m_fRightArmMinValueJoint0;

         if (m_aRArmAngles[1].getUnionValue().asFloat > m_fRightArmMaxValueJoint1)
                 m_aRArmAngles[1] = m_fRightArmMaxValueJoint1;
         if (m_aRArmAngles[1].getUnionValue().asFloat < m_fRightArmMinValueJoint1)
                 m_aRArmAngles[1] = m_fRightArmMinValueJoint1;

         if (m_aRArmAngles[2].getUnionValue().asFloat > m_fRightArmMaxValueJoint2)
                 m_aRArmAngles[2] = m_fRightArmMaxValueJoint2;
         if (m_aRArmAngles[2].getUnionValue().asFloat < m_fRightArmMinValueJoint2)
                 m_aRArmAngles[2] = m_fRightArmMinValueJoint2;

         if (m_aRArmAngles[3].getUnionValue().asFloat > m_fRightArmMaxValueJoint3)
                 m_aRArmAngles[3] = m_fRightArmMaxValueJoint3;
         if (m_aRArmAngles[3].getUnionValue().asFloat < m_fRightArmMinValueJoint3)
                 m_aRArmAngles[3] = m_fRightArmMinValueJoint3;

         if (m_aRArmAngles[4].getUnionValue().asFloat > m_fRightArmMaxValueJoint4)
                 m_aRArmAngles[4] = m_fRightArmMaxValueJoint4;
         if (m_aRArmAngles[4].getUnionValue().asFloat < m_fRightArmMinValueJoint4)
                 m_aRArmAngles[4] = m_fRightArmMinValueJoint4;

         if (m_aRArmAngles[5].getUnionValue().asFloat > m_fRightArmMaxValueJoint5)
                 m_aRArmAngles[5] = m_fRightArmMaxValueJoint5;
         if (m_aRArmAngles[5].getUnionValue().asFloat < m_fRightArmMinValueJoint5)
                 m_aRArmAngles[5] = m_fRightArmMinValueJoint5;

         if (m_aTorsoAngles[0].getUnionValue().asFloat > m_fTorsoMaxValueJoint0){
                 m_aTorsoAngles[0] = m_fTorsoMaxValueJoint0;
                 m_aTorsoAngles[1] = m_fTorsoMaxValueJoint0;
         }
         if (m_aTorsoAngles[0].getUnionValue().asFloat < m_fTorsoMinValueJoint0){
                 m_aTorsoAngles[0] = m_fTorsoMinValueJoint0;
                 m_aTorsoAngles[1] = m_fTorsoMinValueJoint0;
         }


         if (m_bHeadCapture)
         {
             if(l_pHeadTarget)
             {
                 m_oRobotMotionProxy->setAngles(AL::ALValue("Head"), m_aHeadAngles, m_fJointVelocityValue);
             }
         }

         if (m_bTorsoCapture)
         {
             if(l_pTorsoTarget)
             {
                 m_oRobotMotionProxy->setAngles(AL::ALValue::array("LHipPitch","RHipPitch"), m_aTorsoAngles, m_fJointVelocityValue);
             }
         }

         if (m_bLeftArmCapture)
         {
            if (l_pLeftArmTarget)
            {
                m_oRobotMotionProxy->setAngles(AL::ALValue("LArm"), m_aLArmAngles, m_fJointVelocityValue);
            }
         }

         if (m_bRightArmCapture)
         {
             if (l_pRightArmTarget)
             m_oRobotMotionProxy->setAngles(AL::ALValue("RArm"), m_aRArmAngles, m_fJointVelocityValue);
         }


    return true;
}


double SWTeleoperation_nao::getPeriod()
{
    return 1./m_i32Fps;
}

int main(int argc, char* argv[])
{
    /* initialize yarp network */
    Network yarp;
    if (!yarp.checkNetwork())
    {
        std::cerr << "-ERROR: Problem connecting to YARP server" << std::endl;
        return -1;
    }

    /* create your module */
    SWTeleoperation_nao l_oTeleoperation_nao;

    /* prepare and configure the resource finder */
    ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("teleoperation_nao.ini");
    rf.setDefaultContext("swooz-teleoperation/conf");
    rf.configure("NAO_ROOT", argc, argv);

    /* configure the module */
    std::cout << "Configuring the nao Teleoperation module..."<< std::endl;
    if (l_oTeleoperation_nao.configure(rf))
    {
        /* run the module */
        std::cout << "Starting the nao Teleoperation module..." << std::endl;
        l_oTeleoperation_nao.runModule();
    }
    else
    {
        std::cerr << "Failed to configure the nao Teleoperation module!"<< std::endl;
    }

    return 0;
}

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

SWTeleoperation_nao::SWTeleoperation_nao() :  m_i32HeadTimeLastBottle(0), m_bFastrakCalibrated(false)
{
    double l_aDHeadMinJointDefault[]     = {-2.0, -0.5};
    double l_aDHeadMaxJointDefault[]     = { 2.0,  0.5};
    double l_aDLeftArmMinJointDefault[]  = {-2.0, -0.31, -2.0, -1.5, -1.8, 0.0};
    double l_aDLeftArmMaxJointDefault[]  = { 1.9,  1.3,   2.0,  0.0,  1.8, 1.0};
    double l_aDRightArmMinJointDefault[] = {-2.0, -1.3,  -2.0,  0.0, -1.8, 0.0};
    double l_aDRightArmMaxJointDefault[] = { 1.9,  0.31,  2.0,  1.5,  1.8, 1.0};

    m_vHeadMinJointDefault = std::vector<double>(l_aDHeadMinJointDefault, l_aDHeadMinJointDefault + sizeof(l_aDHeadMinJointDefault) / sizeof(double));
    m_vHeadMaxJointDefault = std::vector<double>(l_aDHeadMaxJointDefault, l_aDHeadMaxJointDefault + sizeof(l_aDHeadMaxJointDefault) / sizeof(double));
    m_vLeftArmMinJointDefault = std::vector<double>(l_aDLeftArmMinJointDefault, l_aDLeftArmMinJointDefault + sizeof(l_aDLeftArmMinJointDefault) / sizeof(double));
    m_vLeftArmMaxJointDefault = std::vector<double>(l_aDLeftArmMaxJointDefault, l_aDLeftArmMaxJointDefault + sizeof(l_aDLeftArmMaxJointDefault) / sizeof(double));
    m_vRightArmMinJointDefault = std::vector<double>(l_aDRightArmMinJointDefault, l_aDRightArmMinJointDefault + sizeof(l_aDRightArmMinJointDefault) / sizeof(double));
    m_vRightArmMaxJointDefault = std::vector<double>(l_aDRightArmMaxJointDefault, l_aDRightArmMaxJointDefault + sizeof(l_aDRightArmMaxJointDefault) / sizeof(double));

    m_aHeadAngles.arraySetSize(2);
    m_aTorsoAngles.arraySetSize(2);
    m_aLArmAngles.arraySetSize(6);
    m_aRArmAngles.arraySetSize(6);
    m_aLLegAngles.arraySetSize(6);
    m_aRLegAngles.arraySetSize(6);

    m_bHeadCapture = m_bTorsoCapture = m_bLeftArmCapture = m_bRightArmCapture = false;
}


bool SWTeleoperation_nao::configure(ResourceFinder &oRf)
{
    // gets the module name which will form the stem of all module port names
        m_sModuleName   = oRf.check("name", Value("teleoperation_nao"), "Teleoperation/nao Module name (string)").asString();
        setName(m_sModuleName.c_str());
        m_sRobotAddress = oRf.check("IP", Value("169.254.108.110"), "IP Adress of the Nao Robot").asString();

    // acceleration/speeds values for nao
        m_dJointVelocityValue     = oRf.check("jointVelocityValue",  yarp::os::Value(0.1),  "Joint Velocity Value (float)").asDouble();

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
        m_sHeadTrackerPortName          = "/teleoperation/nao/head";
        m_sTorsoTrackerPortName         = "/teleoperation/nao/torso";

        m_sLeftArmTrackerPortName       = "/teleoperation/nao/left_arm/arm";
        m_sLeftHandTrackerPortName      = "/teleoperation/nao/left_arm/hand";
        m_sLeftFingersTrackerPortName   = "/teleoperation/nao/left_arm/fingers";

        m_sRightArmTrackerPortName      = "/teleoperation/nao/right_arm/arm";
        m_sRightHandTrackerPortName     = "/teleoperation/nao/right_arm/hand";
        m_sRightFingersTrackerPortName  = "/teleoperation/nao/right_arm/fingers";

        if(!m_oHeadTrackerPort.open(m_sHeadTrackerPortName.c_str()) ||
            !m_oTorsoTrackerPort.open(m_sTorsoTrackerPortName.c_str()) ||
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

//        m_oRobotMotionProxy->setStiffnesses("Body", 1.0f);
        m_oRobotMotionProxy->setStiffnesses("Head", 1.0f);

//        try	{
//            ALRobotPostureProxy l_oRobotRobotPosture=  ALRobotPostureProxy(m_sRobotAddress);
//            l_oRobotRobotPosture.goToPosture("Stand", 1.0);
//        }
//        catch (const AL::ALError& e){
//            std::cerr << "-ERROR: " << e.what() << std::endl;
//            return false;
//        }

        resetHeadPosition();
//        resetTorsoPosition(); //
//        resetLeftArmPosition(); //
//        resetRightArmPosition(); //


    return true;
}


bool SWTeleoperation_nao::interruptModule()
{
    m_oHeadTrackerPort.interrupt();
    m_oTorsoTrackerPort.interrupt();
    m_oLeftHandTrackerPort.interrupt();
    m_oLeftFingersTrackerPort.interrupt();
    m_oRightHandTrackerPort.interrupt();
    m_oRightFingersTrackerPort.interrupt();

    std::cout << "--Interrupting the nao Teleoperation module..." << std::endl;
    return true;
}

void SWTeleoperation_nao::resetHeadPosition()
{
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

    // m_oRobotMotionProxy->angleInterpolationWithSpeed(AL::ALValue("Head"), m_aHeadAngles, m_fJointVelocityValue);
}

void SWTeleoperation_nao::resetTorsoPosition()
{
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
//    resetTorsoPosition();
//    resetLeftArmPosition();
//    resetRightArmPosition();

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

    deleteAndNullify(m_oRobotMotionProxy);

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

        m_bTorsoCapture = true;
    }

    //read left arm
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
        m_bLeftArmCapture = true;
    }

    //read right arm
    l_pRightArmTarget = m_oRightArmTrackerPort.read(false);

    if(l_pRightArmTarget)
    {
        int l_deviceId = l_pRightArmTarget->get(0).asInt();

        switch(l_deviceId)
        {
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

//                if (!m_bFastrakCalibrated)
//                {
//                    std::cout << "Place your hand at the level of your belly button and press enter";
//                    std::cin.ignore();
//                    m_vFastrakOffsets.resize(3);
//                    m_vFastrakOffsets[0] = l_pLeftHandTarget->get(2).asDouble();
//                    m_vFastrakOffsets[1] = l_pLeftHandTarget->get(1).asDouble();
//                    m_vFastrakOffsets[2] = l_pLeftHandTarget->get(3).asDouble();
//                    m_bFastrakCalibrated = true;
//                }


//                //cartesian position
//                Vector l_xd(3);
//                //cartesian orientation
//                Vector l_ori(3);
//                Vector l_od(4);

//                Vector l_joints;

//                //std::cout << "current : "<< l_xd.toString() << std::endl;

//                l_xd[0] = (l_pLeftHandTarget->get(2).asDouble() - m_vFastrakOffsets[0]) / (- 70.);
//                l_xd[1] = (l_pLeftHandTarget->get(1).asDouble() - m_vFastrakOffsets[1]) / (-158.);
//                l_xd[2] = (l_pLeftHandTarget->get(3).asDouble() - m_vFastrakOffsets[2]) / ( 250.);

//                double l_degToRad = 3.14159 / 180;

//                l_ori[0] = l_degToRad * (-l_pLeftHandTarget->get(4).asDouble() + 90);
//                l_ori[1] = l_degToRad * (-l_pLeftHandTarget->get(5).asDouble());
//                l_ori[2] = l_degToRad * (-l_pLeftHandTarget->get(6).asDouble());

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

//                std::cout << m_aLArmAngles.toString() << "\t";

                m_bLeftArmCapture = true;

            }
            break;
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


    if (m_bHeadCapture)
    {
        if(l_pHeadTarget)
        {
            m_oRobotMotionProxy->setAngles(AL::ALValue("Head"), m_aHeadAngles, static_cast<float>(m_dJointVelocityValue));
        }
    }

    if (m_bTorsoCapture)
    {
        if(l_pTorsoTarget)
        {
            m_oRobotMotionProxy->setAngles(AL::ALValue::array("LHipPitch","RHipPitch"), m_aTorsoAngles, static_cast<float>(m_dJointVelocityValue));
        }
    }

    if (m_bLeftArmCapture)
    {
        if (l_pLeftArmTarget)
        {
            m_oRobotMotionProxy->setAngles(AL::ALValue("LArm"), m_aLArmAngles, static_cast<float>(m_dJointVelocityValue));
        }
    }

    if (m_bRightArmCapture)
    {
        if (l_pRightArmTarget)
        {
            m_oRobotMotionProxy->setAngles(AL::ALValue("RArm"), m_aRArmAngles, static_cast<float>(m_dJointVelocityValue));
        }
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

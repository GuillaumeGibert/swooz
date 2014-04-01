/**
 * \file SWTeleoperation_iCub.cpp
 * \author Florian Lance, Guillaume Gibert, Emmanuel Body
 * \date 05-12-2013
 * \brief ...
 */

#include "icub/SWTeleoperation_iCub.h"

#include "SWTrackingDevice.h"

#include <sstream>

#include "geometryUtility.h"
//#include <iCub/ctrl/math.h>

SWTeleoperation_iCub::SWTeleoperation_iCub() : m_pIHeadVelocity(NULL), m_pIHeadEncoders(NULL), m_pIHeadPosition(NULL), m_bIsRunning(false), m_bFirstLEDCommand(true),
            m_i32HeadTimeLastBottle(0), m_i32FaceTimeLastBottle(0), m_i32ArmsTimeLastBottle(0), m_i32GazeTimeLastBottle(0)
{}

SWTeleoperation_iCub::~SWTeleoperation_iCub()
{}


bool SWTeleoperation_iCub::configure(ResourceFinder &rf)
{
    // gets the module name which will form the stem of all module port names
        m_sModuleName   = rf.check("name", Value("teleoperation_iCub"), "Teleoperation/iCub Module name (string)").asString();
        m_sRobotName    = rf.check("robot",  Value("icubSim"),  "Robot name (string)").asString();
        setName(m_sModuleName.c_str());

    // acceleration/speeds values for iCub
        m_i32JointVelocityValue     = rf.check("jointVelocityValue",      Value(600),  "Joint Velocity Value (int)").asInt();
        m_i32JointAccelerationValue = rf.check("jointAccelerationValue",  Value(600),  "Joint Acceleration Value (int)").asInt();

    // min / max values for iCub joints
        // head
        m_i32HeadMinValueJoint0 = rf.check("minValueJoint0",  Value(-40), "Head minimum Joint0 Value (int)").asInt();
        m_i32HeadMaxValueJoint0 = rf.check("maxValueJoint0",  Value( 30), "Head maximum Joint0 Value (int)").asInt();
        m_i32HeadMinValueJoint1 = rf.check("minValueJoint1",  Value(-70), "Head minimum Joint1 Value (int)").asInt();
        m_i32HeadMaxValueJoint1 = rf.check("maxValueJoint1",  Value( 60), "Head maximum Joint1 Value (int)").asInt();
        m_i32HeadMinValueJoint2 = rf.check("minValueJoint2",  Value(-55), "Head minimum Joint2 Value (int)").asInt();
        m_i32HeadMaxValueJoint2 = rf.check("maxValueJoint2",  Value( 55), "Head maximum Joint2 Value (int)").asInt();
        // torso
        m_i32TorsoMinValueJoint0 = rf.check("xxx",  Value(-50), "Torso minimum Joint0 Value (int)").asInt();
        m_i32TorsoMaxValueJoint0 = rf.check("xxx",  Value( 50), "Torso maximum Joint0 Value (int)").asInt();
        m_i32TorsoMinValueJoint1 = rf.check("xxx",  Value(-30), "Torso minimum Joint1 Value (int)").asInt();
        m_i32TorsoMaxValueJoint1 = rf.check("xxx",  Value( 30), "Torso maximum Joint1 Value (int)").asInt();
        m_i32TorsoMinValueJoint2 = rf.check("xxx",  Value(-10), "Torso minimum Joint2 Value (int)").asInt();
        m_i32TorsoMaxValueJoint2 = rf.check("xxx",  Value( 70), "Torso maximum Joint2 Value (int)").asInt();
        // left arm
        // ...
        // right arm
        // ...


    // min / max values for iCub eyelids
        if(m_sRobotName == "icubSim")
        {
            m_i32MinValueEyelids = rf.check("minValueEyelidsSim",  Value( 24 ),  "Min value eyelidSim (int)").asInt();
            m_i32MaxValueEyelids = rf.check("maxValueEyelidsSim",  Value( 48 ),  "Max value eyelidSim (int)").asInt();
        }
        else
        {
            m_i32MinValueEyelids = rf.check("minValueEyelids",  Value( 30 ),  "Min value eyelid (int)").asInt();
            m_i32MaxValueEyelids = rf.check("maxValueEyelids",  Value( 70 ),  "Max value eyelid (int)").asInt();
        }

    // miscellaneous
        m_i32Fps                    = rf.check("fps",                   Value(100),  "Frame per second (int)").asInt();
        m_i32HeadTimeoutReset       = rf.check("headTimeoutReset",      Value(3000), "Head gaze timeout reset iCub (int)").asInt();
        m_i32FaceTimeoutReset       = rf.check("faceTimeoutReset",      Value(3000), "Face gaze timeout reset iCub (int)").asInt();
        m_i32ArmsTimeoutReset       = rf.check("armsTimeoutReset",      Value(3000), "Arms timeout reset iCub (int)").asInt();
        m_i32GazeTimeoutReset       = rf.check("gazeTimeoutReset",      Value(3000), "Gaze timeout reset iCub (int)").asInt();

    // head
        // set polydriver options
            m_oHeadOptions.put("robot",     m_sRobotName.c_str());
            m_oHeadOptions.put("device",    "remote_controlboard");
            m_oHeadOptions.put("local",    ("/local/" + m_sRobotName + "/head").c_str());
            m_oHeadOptions.put("name",     ("/" + m_sRobotName + "/head").c_str());
            m_oHeadOptions.put("remote",   ("/" + m_sRobotName + "/head").c_str());

        // init polydriver
            m_oRobotHead.open(m_oHeadOptions);
            if(!m_oRobotHead.isValid())
            {
                std::cerr << "-ERROR: robotHead is not valid" << std::endl;
                return false;
            }

        // initializing controllers
            if (!m_oRobotHead.view(m_pIHeadVelocity) || !m_oRobotHead.view(m_pIHeadPosition) || !m_oRobotHead.view(m_pIHeadEncoders))
            {
                std::cerr << "-ERROR: while getting required robot head interfaces." << std::endl;
                m_oRobotHead.close();
                return false;
            }

    // torso
        // set polydriver options
            m_oTorsoOptions.put("robot",     m_sRobotName.c_str());
            m_oTorsoOptions.put("device",    "remote_controlboard");
            m_oTorsoOptions.put("local",    ("/local/" + m_sRobotName + "/torso").c_str());
            m_oTorsoOptions.put("name",     ("/" + m_sRobotName + "/torso").c_str());
            m_oTorsoOptions.put("remote",   ("/" + m_sRobotName + "/torso").c_str());
        // init polydriver
            m_oRobotTorso.open(m_oTorsoOptions);
            if(!m_oRobotTorso.isValid())
            {
                std::cerr << "-ERROR: robotTorso is not valid" << std::endl;
                return false;
            }
        // initializing controllers
            if (!m_oRobotTorso.view(m_pITorsoVelocity) || !m_oRobotTorso.view(m_pITorsoPosition) || !m_oRobotTorso.view(m_pITorsoEncoders))
            {
                std::cerr << "-ERROR: while getting required robot torso interfaces." << std::endl;
                m_oRobotTorso.close();
                return false;
            }


    // left arm
        // set polydriver options
            m_oLeftArmOptions.put("robot",     m_sRobotName.c_str());
            m_oLeftArmOptions.put("device", "remote_controlboard");
            m_oLeftArmOptions.put("local",     ("/local/" + m_sRobotName + "/left_arm").c_str());
            m_oLeftArmOptions.put("name",     ("/" + m_sRobotName + "/left_arm").c_str());
            m_oLeftArmOptions.put("remote",   ("/" + m_sRobotName + "/left_arm").c_str());
        // init polydriver
            m_oRobotLeftArm.open(m_oLeftArmOptions);
            if (!m_oRobotLeftArm.isValid())
            {
                std::cerr << "-ERROR: left arm is not valid" << std::endl;
                return false;
            }
        // initializing controllers
            if (!m_oRobotLeftArm.view(m_pILeftArmEncoders) || !m_oRobotLeftArm.view(m_pILeftArmVelocity) || !m_oRobotLeftArm.view(m_pILeftArmPosition))
            {
                std::cerr << "-ERROR: while getting required robot left arm interfaces." << std::endl;
                return false;
            }

    // left arm (cartesian)
        // set polydriver options
            m_oLeftArmCartesianOptions.put("robot",     m_sRobotName.c_str());
            m_oLeftArmCartesianOptions.put("device",    "cartesiancontrollerclient");
            m_oLeftArmCartesianOptions.put("local",    ("/local/" + m_sRobotName + "/cartesianController/left_arm").c_str());
            m_oLeftArmCartesianOptions.put("name",     ("/" + m_sRobotName + "/cartesianController/left_arm").c_str());
            m_oLeftArmCartesianOptions.put("remote",   ("/" + m_sRobotName + "/cartesianController/left_arm").c_str());
        // init polydriver
            m_oRobotLeftArmCartesian.open(m_oLeftArmCartesianOptions);
            if (!m_oRobotLeftArmCartesian.isValid())
            {
                std::cerr << "-WARNING: left arm cartesian is not valid" << std::endl;

            }
            else
            {
                // initializing controllers
                if (!m_oRobotLeftArmCartesian.view(m_pILeftArmCartesian))
                {
                    std::cerr << "-ERROR: Couldn't open the Left Arm ICartesianControl client!" << std::endl;
                    return false;
                }
                //  limit the torso DOF for the cartesian controller
                Vector l_torsoDof(3);
                l_torsoDof = 2.; // This values tells the kinematic solver to skip the torso
                if( m_pILeftArmCartesian->setDOF(l_torsoDof, l_torsoDof))
                {
                    std::cerr << "-WARNING: Unable to set the torso DOFs";
                }
            }

    // right arm
        // set polydriver options
            m_oRightArmOptions.put("robot",     m_sRobotName.c_str());
            m_oRightArmOptions.put("device", "remote_controlboard");
            m_oRightArmOptions.put("local",     ("/local/" + m_sRobotName + "/right_arm").c_str());
            m_oRightArmOptions.put("name",     ("/" + m_sRobotName + "/right_arm").c_str());
            m_oRightArmOptions.put("remote",   ("/" + m_sRobotName + "/right_arm").c_str());
        // init polydriver
            m_oRobotRightArm.open(m_oRightArmOptions);
            if (!m_oRobotRightArm.isValid())
            {
                std::cerr << "-ERROR: right arm is not valid" << std::endl;
                return false;
            }
        // init controller
            if (!m_oRobotRightArm.view(m_pIRightArmEncoders) || !m_oRobotRightArm.view(m_pIRightArmVelocity) || !m_oRobotRightArm.view(m_pIRightArmPosition))
            {
                std::cerr << "-ERROR: while getting required robot right arm interfaces." << std::endl;
                return false;
            }

     // right arm (cartesian)
        //set polydriver options
            m_oRightArmCartesianOptions.put("robot",     m_sRobotName.c_str());
            m_oRightArmCartesianOptions.put("device",    "cartesiancontrollerclient");
            m_oRightArmCartesianOptions.put("local",    ("/local/" + m_sRobotName + "/cartesianController/right_arm").c_str());
            m_oRightArmCartesianOptions.put("name",     ("/" + m_sRobotName + "/cartesianController/right_arm").c_str());
            m_oRightArmCartesianOptions.put("remote",   ("/" + m_sRobotName + "/cartesianController/right_arm").c_str());
        //init polydriver
            m_oRobotRightArmCartesian.open(m_oRightArmCartesianOptions);
            if (!m_oRobotRightArmCartesian.isValid())
            {
                std::cerr << "-WARNING: right arm cartesian is not valid" << std::endl;
            }
            else
            {
                // init controller
                if (!m_oRobotRightArmCartesian.view(m_pIRightArmCartesian))
                {
                    std::cerr << "-ERROR: Couldn't open the right Arm ICartesianControl client!" << std::endl;
                    return false;
                }

                //limit the torso DOF for the cartesian controller
                Vector l_torsoDof(3);
                l_torsoDof = 0.;
                if( m_pIRightArmCartesian->setDOF(l_torsoDof, l_torsoDof))
                {
                    std::cerr << "-WARNING: Unable to set the torso DOFs" << std::endl;
                }
            }

    // init ports
        m_sHeadTrackerPortName          = "/teleoperation/" + m_sRobotName + "/head";
        m_sGazeTrackerPortName          = "/teleoperation/" + m_sRobotName + "/gaze";
        m_sFaceTrackerPortName          = "/teleoperation/" + m_sRobotName + "/face";
        m_sTorsoTrackerPortName         = "/teleoperation/" + m_sRobotName + "/torso";
        m_sEyelidInputPortName          = "/" + m_sRobotName + "/face/raw/in";
        m_sEyelidOutputPortName         = "/teleoperation/" + m_sRobotName + "/eyelids/out";
        m_sLeftArmTrackerPortName       = "/teleoperation/" + m_sRobotName + "/left_arm/arm";
        m_sLeftHandTrackerPortName      = "/teleoperation/" + m_sRobotName + "/left_arm/hand";
        m_sLeftFingersTrackerPortName   = "/teleoperation/" + m_sRobotName + "/left_arm/fingers";
        m_sRightArmTrackerPortName      = "/teleoperation/" + m_sRobotName + "/right_arm/arm";
        m_sRightHandTrackerPortName     = "/teleoperation/" + m_sRobotName + "/right_arm/hand";
        m_sRightFingersTrackerPortName  = "/teleoperation/" + m_sRobotName + "/right_arm/fingers";

    // open ports
        if(!m_oHeadTrackerPort.open(m_sHeadTrackerPortName.c_str()) ||
           !m_oGazeTrackerPort.open(m_sGazeTrackerPortName.c_str()) ||
           !m_oFaceTrackerPort.open(m_sFaceTrackerPortName.c_str()) ||
           !m_oTorsoTrackerPort.open(m_sTorsoTrackerPortName.c_str()) ||
           !m_oFaceHandlerPort.open(m_sEyelidOutputPortName.c_str()) ||
           !m_oLeftArmTrackerPort.open(m_sLeftArmTrackerPortName.c_str()) ||
           !m_oLeftHandTrackerPort.open(m_sLeftHandTrackerPortName.c_str()) ||
           !m_oLeftFingersTrackerPort.open(m_sLeftFingersTrackerPortName.c_str()) ||
           !m_oRightArmTrackerPort.open(m_sRightArmTrackerPortName.c_str()) ||
           !m_oRightHandTrackerPort.open(m_sRightHandTrackerPortName.c_str()) ||
           !m_oRightFingersTrackerPort.open(m_sRightFingersTrackerPortName.c_str()))
        {
            std::cerr << "-ERROR: Unable to open ports." << std::endl;
            m_oRobotHead.close();
            m_oRobotLeftArm.close();
            m_oRobotLeftArmCartesian.close();
            m_oRobotTorso.close();
            m_oRobotRightArm.close();
            m_oRobotRightArmCartesian.close();
            interruptModule();
            return false;
        }

    //  attach to port
        Network::connect(m_sEyelidOutputPortName.c_str(), m_sEyelidInputPortName.c_str());

    // move to rest position
        int l_i32JointsNb = 0;
        m_pIHeadPosition->getAxes(&l_i32JointsNb );

    // reset position
        resetHeadPosition();
//        resetTorsoPosition();
//        resetLeftArmPosition();
//        resetRightArmPosition();

        m_bTorsoCapture = m_bLeftArmCapture = m_bRightArmCapture = false;

    // set accelerations
        Vector l_aHeadTmp;
        l_aHeadTmp.resize(l_i32JointsNb );
        for (int i = 0; i < l_i32JointsNb ; i++)
        {
            l_aHeadTmp[i] = m_i32JointAccelerationValue;
        }
        m_pIHeadPosition->setRefAccelerations(l_aHeadTmp.data());

    // set speed
        for (int i = 0; i < l_i32JointsNb ; i++)
        {            
            if(i != 5)
            {
                l_aHeadTmp[i] = m_i32JointVelocityValue;
            }
            else
            {
                l_aHeadTmp[i] = 50; // vergence speed
            }
        }
        m_pIHeadPosition->setRefSpeeds(l_aHeadTmp.data());

    return (m_bIsRunning = true); // let the RFModule know everything went well
}


bool SWTeleoperation_iCub::interruptModule()
{
    m_bIsRunning = false;

    m_oHeadTrackerPort.interrupt();
    m_oGazeTrackerPort.interrupt();
    m_oFaceTrackerPort.interrupt();
    m_oFaceHandlerPort.interrupt();
    m_oLeftHandTrackerPort.interrupt();
    m_oLeftFingersTrackerPort.interrupt();
    m_oRightHandTrackerPort.interrupt();
    m_oRightFingersTrackerPort.interrupt();

    std::cout << "--Interrupting the iCub Teleoperation module..." << std::endl;
    return true;
}

void SWTeleoperation_iCub::resetHeadPosition()
{
    m_pIHeadPosition->positionMove(0,0);
    m_pIHeadPosition->positionMove(1,0);
    m_pIHeadPosition->positionMove(2,0);
}

void SWTeleoperation_iCub::resetTorsoPosition()
{
    m_pITorsoPosition->positionMove(0,0);
    m_pITorsoPosition->positionMove(1,0);
    m_pITorsoPosition->positionMove(2,0);
}

void SWTeleoperation_iCub::resetLeftArmPosition()
{
    m_vLeftArmJoints.resize(16);
    m_vLeftArmJoints[ 0] = -25;
    m_vLeftArmJoints[ 1] =  20;
    m_vLeftArmJoints[ 2] =   0;
    m_vLeftArmJoints[ 3] =  50;
    m_vLeftArmJoints[ 4] =   0;
    m_vLeftArmJoints[ 5] =   0;
    m_vLeftArmJoints[ 6] =   0;
    m_vLeftArmJoints[ 7] =  60;
    m_vLeftArmJoints[ 8] =  20;
    m_vLeftArmJoints[ 9] =  20;
    m_vLeftArmJoints[10] =  20;
    m_vLeftArmJoints[11] =  10;
    m_vLeftArmJoints[12] =  10;
    m_vLeftArmJoints[13] =  10;
    m_vLeftArmJoints[14] =  10;
    m_vLeftArmJoints[15] =  10;
    m_pILeftArmPosition->positionMove(m_vLeftArmJoints.data());
}

void SWTeleoperation_iCub::resetRightArmPosition()
{
    std::cout << "resetRightArmPosition" << std::endl;
    m_vRightArmJoints.resize(16);
    m_vRightArmJoints[ 0] = -25;
    m_vRightArmJoints[ 1] =  20;
    m_vRightArmJoints[ 2] =   0;
    m_vRightArmJoints[ 3] =  50;
    m_vRightArmJoints[ 4] =   0;
    m_vRightArmJoints[ 5] =   0;
    m_vRightArmJoints[ 6] =   0;
    m_vRightArmJoints[ 7] =  60;
    m_vRightArmJoints[ 8] =  20;
    m_vRightArmJoints[ 9] =  20;
    m_vRightArmJoints[10] =  20;
    m_vRightArmJoints[11] =  10;
    m_vRightArmJoints[12] =  10;
    m_vRightArmJoints[13] =  10;
    m_vRightArmJoints[14] =  10;
    m_vRightArmJoints[15] =  10;
    m_pIRightArmPosition->positionMove(m_vRightArmJoints.data());
}


bool SWTeleoperation_iCub::close()
{
    // reset position
        resetHeadPosition();
//        resetTorsoPosition();
//        resetLeftArmPosition();
//        resetRightArmPosition();

    // close ports
        m_oHeadTrackerPort.close();
        m_oFaceTrackerPort.close();
        m_oFaceHandlerPort.close();
        m_oGazeTrackerPort.close();
        m_oLeftArmTrackerPort.close();
        m_oLeftHandTrackerPort.close();
        m_oLeftFingersTrackerPort.close();
        m_oRightArmTrackerPort.close();
        m_oRightHandTrackerPort.close();
        m_oRightFingersTrackerPort.close();

    // close polydrivers
        m_oRobotHead.close();
        m_oRobotLeftArm.close();
        m_oRobotLeftArmCartesian.close();
        m_oRobotTorso.close();
        m_oRobotRightArm.close();
        m_oRobotRightArmCartesian.close();

    return true;
}

bool SWTeleoperation_iCub::updateModule()
{

    if(!m_bIsRunning)
    {
        return false;
    }

    // head / gaze
        int l_i32HeadJointsNb = 0;
        Vector l_vHeadEncoders, l_vHeadCommand;
        m_pIHeadPosition->getAxes(&l_i32HeadJointsNb);
        l_vHeadCommand.resize(l_i32HeadJointsNb);
        l_vHeadEncoders.resize(l_i32HeadJointsNb );
        //read encoders
            m_pIHeadEncoders->getEncoders(l_vHeadEncoders.data());
        // init command vlaues
            std::vector<double> l_vHeadJoints(6,0.);

    // torso
        int l_i32TorsoJointsNb = 0;
        Vector l_vTorsoEncoders, l_vTorsoCommand;
        m_pITorsoPosition->getAxes(&l_i32TorsoJointsNb);
        l_vTorsoCommand.resize(l_i32TorsoJointsNb);
        l_vTorsoEncoders.resize(l_i32TorsoJointsNb );
        //read encoders
            m_pITorsoEncoders->getEncoders(l_vTorsoEncoders.data());
        // init command vlaues
            m_vTorsoJoints.resize(l_i32TorsoJointsNb,0.);

    // left arm
        int l_i32LeftArmJointsNb = 0;
        Vector l_vLeftArmEncoders, l_vLeftArmCommand;
        m_pILeftArmVelocity->getAxes(&l_i32LeftArmJointsNb);
        l_vLeftArmCommand.resize(l_i32LeftArmJointsNb);
        l_vLeftArmEncoders.resize(l_i32LeftArmJointsNb );
        //read encoders
            m_pILeftArmEncoders->getEncoders(l_vLeftArmEncoders.data());
        // init command vlaues
            m_vLeftArmJoints.resize(l_i32LeftArmJointsNb,0.);


    // right arm
        int l_i32RightArmJointsNb = 0;
        Vector l_vRightArmEncoders, l_vRightArmCommand;
        m_pIRightArmVelocity->getAxes(&l_i32RightArmJointsNb);
        l_vRightArmCommand.resize(l_i32RightArmJointsNb);
        l_vRightArmEncoders.resize(l_i32RightArmJointsNb );
        //read encoders
            m_pIRightArmEncoders->getEncoders(l_vRightArmEncoders.data());
        // init command vlaues
            m_vRightArmJoints.resize(l_i32RightArmJointsNb, 0.);

    // face
        // ...

    // define bottles
        Bottle *l_pHeadTarget = NULL, *l_pTorsoTarget = NULL, *l_pFaceTarget = NULL;
        Bottle *l_pGazeTarget = NULL, *l_pLeftArmTarget = NULL, *l_pLeftHandTarget = NULL;
        Bottle *l_pLeftFingersTarget = NULL, *l_pRightArmTarget = NULL, *l_pRightHandTarget = NULL;
        Bottle *l_pRightFingersTarget = NULL;

    // read head commands
    {
        l_pHeadTarget = m_oHeadTrackerPort.read(false);

        if(l_pHeadTarget)
        {
            int l_deviceId = l_pHeadTarget->get(0).asInt();

            switch(l_deviceId)
            {
                case swTracking::FOREST_LIB :
                    {
                        l_vHeadJoints[0] = -l_pHeadTarget->get(1).asDouble(); //head rotation "yes" [-40 30]
                        l_vHeadJoints[1] = -l_pHeadTarget->get(3).asDouble(); //head rotation [-70 60]
                        l_vHeadJoints[2] = -l_pHeadTarget->get(2).asDouble(); //head rotation "no" [-55 55]
                    }
                break;
                case swTracking::COREDATA_LIB :
                    {
                        l_vHeadJoints[0] =  l_pHeadTarget->get(4).asDouble()*180./3.1416; // up/down head
                        l_vHeadJoints[1] =  l_pHeadTarget->get(6).asDouble()*180./3.1416; // left/right head
                        l_vHeadJoints[2] = -l_pHeadTarget->get(5).asDouble()*180./3.1416; // head
                    }
                break;
                case swTracking::EMICP_LIB :
                    {
                        l_vHeadJoints[0] = -l_pHeadTarget->get(4).asDouble(); // up/down head
                        l_vHeadJoints[1] = -l_pHeadTarget->get(6).asDouble();  // left/right head
                        l_vHeadJoints[2] = -l_pHeadTarget->get(5).asDouble(); // head
                    }
                break;
                case swTracking::FACESHIFT_LIB :
                    {
                        l_vHeadJoints[0] = -l_pHeadTarget->get(4).asDouble()*180./3.1416; // up/down head
                        l_vHeadJoints[1] = -l_pHeadTarget->get(6).asDouble()*180./3.1416; // left/right head
                        l_vHeadJoints[2] = -l_pHeadTarget->get(5).asDouble()*180./3.1416; // head
                    }
                break;
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

                        l_vHeadJoints[0] = -l_rpyHead[1];
                        l_vHeadJoints[1] = -l_rpyHead[0];
                        l_vHeadJoints[2] =  l_rpyHead[2];
                    }
                break;
            }

            m_i32HeadTimeLastBottle = 0;
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

    // read face commands
    {
        l_pFaceTarget = m_oFaceTrackerPort.read(false);

        if(l_pFaceTarget)
        {
            int l_deviceId = l_pFaceTarget->get(0).asInt();

            switch(l_deviceId)
            {
                case swTracking::COREDATA_LIB :
                    {
                        Bottle &l_oFaceMotionBottle = m_oFaceHandlerPort.prepare();

                        // mouth
                            l_oFaceMotionBottle.clear();
                            std::vector<double> l_vInnerLip2, l_vInnerLip6;
                            for(int ii = 0; ii < 3; ++ii)
                            {
                                l_vInnerLip2.push_back(l_pFaceTarget->get(25+ii).asDouble());
                                l_vInnerLip6.push_back(l_pFaceTarget->get(37+ii).asDouble());
                            }
                            l_oFaceMotionBottle.addString(m_oIcubFaceLEDCmd.lipCommand(l_vInnerLip2, l_vInnerLip6).c_str());
                            m_oFaceHandlerPort.write();
                            Time::delay(0.001);


                        // eyebrows
                            // retrieve values
                                std::vector<double> l_vLeftEyeBrowPoints, l_vRightEyeBrowPoints, l_vLeftEyeCenter, l_vRightEyeCenter;
                                for(int ii = 0; ii < 9; ++ii)
                                {
                                    l_vLeftEyeBrowPoints.push_back(l_pFaceTarget->get(52+ii).asDouble());
                                    l_vRightEyeBrowPoints.push_back(l_pFaceTarget->get(43+ii).asDouble());

                                    if(ii < 3)
                                    {
                                        l_vLeftEyeCenter.push_back(l_pFaceTarget->get(64+ii).asDouble());
                                        l_vRightEyeCenter.push_back(l_pFaceTarget->get(67+ii).asDouble());
                                    }
                                }

                                if(m_bFirstLEDCommand)
                                {
                                    m_oIcubFaceLEDCmd.setNeutralPoints(l_vLeftEyeBrowPoints, l_vRightEyeBrowPoints, l_vLeftEyeCenter, l_vRightEyeCenter);
                                    m_bFirstLEDCommand = false;
                                }

                                // TO BE TESTED
                            // left
                                l_oFaceMotionBottle.clear();
                                l_oFaceMotionBottle.addString(m_oIcubFaceLEDCmd.leftEyeBrowCommand(l_vLeftEyeBrowPoints, l_vLeftEyeCenter).c_str());
                                m_oFaceHandlerPort.write();
                                Time::delay(0.001);
                            // right
                                l_oFaceMotionBottle.clear();
                                l_oFaceMotionBottle.addString(m_oIcubFaceLEDCmd.rightEyeBrowCommand(l_vRightEyeBrowPoints, l_vRightEyeCenter).c_str());
                                m_oFaceHandlerPort.write();
                                Time::delay(0.001);
                    }
                break;
                case swTracking::STASM_LIB :
                    {
                        // ...
                    }
                break;
                case swTracking::FOREST_LIB :
                    {
                        // ...
                    }
                break;
                case swTracking::FACESHIFT_LIB :
                    {
                        // ...
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
                // reset face ... TODO
            }
        }
    }

    // read gaze commands
    {
        l_pGazeTarget = m_oGazeTrackerPort.read(false);

        if(l_pGazeTarget)
        {
            int l_deviceId = l_pGazeTarget->get(0).asInt();

            switch(l_deviceId)
            {
                case swTracking::COREDATA_LIB :
                    {
                        // TODO : check facelab eyes values (avatar / icub)
                        l_vHeadJoints[3] =  ((l_pGazeTarget->get(9).asDouble()  + l_pGazeTarget->get(14).asDouble())/2)*180/3.1416; // up/down eye [-35; +15]
                        l_vHeadJoints[4] = -((l_pGazeTarget->get(10).asDouble() + l_pGazeTarget->get(15).asDouble())/2)*180/3.1416; // version angle [-50; 52] = (L+R)/2
                        l_vHeadJoints[5] =  (-l_pGazeTarget->get(10).asDouble() + l_pGazeTarget->get(15).asDouble())   *180/3.1416; // vergence angle [0 90] = R-L

                        Bottle &l_oFaceMotionBottle = m_oFaceHandlerPort.prepare();
                        // eye closure
                            l_oFaceMotionBottle.clear();
                            double l_dLeftEyeClosure = l_pGazeTarget->get(8).asDouble(), l_dRightEyeClosure = l_pGazeTarget->get(13).asDouble();
                            l_oFaceMotionBottle.addString(
                            eyesOpeningCode((1.0-(l_dLeftEyeClosure + l_dRightEyeClosure)/2.0), m_i32MinValueEyelids, m_i32MaxValueEyelids).c_str());
                            m_oFaceHandlerPort.write();
                            Time::delay(0.001);

                            std::cout << "eyesOpeningCode " << std::endl;
                    }
                break;
                case swTracking::FACESHIFT_LIB :
                    {
                        // ...
                    }
                break;
                case swTracking::TOBII_LIB :
                    {
                        // ...
                    }
                break;
            }


            m_i32GazeTimeLastBottle = 0;
        }
        else
        {
            m_i32GazeTimeLastBottle += 1000/m_i32Fps;

            if(m_i32GazeTimeLastBottle > m_i32GazeTimeoutReset)
            {
                m_i32GazeTimeLastBottle = 0;
                // reset gaze ... TODO
            }
        }


    }

    // read torso commands
    {
        l_pTorsoTarget = m_oTorsoTrackerPort.read(false);

        if (l_pTorsoTarget)
        {
            int l_deviceId = l_pTorsoTarget->get(0).asInt();

            switch(l_deviceId)
            {
                case swTracking::OPENNI_LIB:
                {
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

                    m_vTorsoJoints[0] = l_rpyTorso[2];
                    m_vTorsoJoints[1] = l_rpyTorso[0];
                    m_vTorsoJoints[2] = l_rpyTorso[1];
                }
                break;
            }
            m_bTorsoCapture = true;
        }
    }

    // read left arm
    {
        l_pLeftArmTarget = m_oLeftArmTrackerPort.read(false);

        if(l_pLeftArmTarget)
        {
            int l_deviceId = l_pLeftArmTarget->get(0).asInt();
            std::cout << "Recieved Left Arm data (device " << l_deviceId << ")" << std::endl;

            switch(l_deviceId)
            {
                case swTracking::FORTH_LIB :
                {
                    // ...
                }
                break;
                case swTracking::FASTRAK_LIB :
                {
                    // ...
                }
                break;
                case swTracking::OPENNI_LIB :
                {
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

                    m_vLeftArmJoints[0] = swUtil::degree180(l_rpyLShoulder[1] - 180.);
                    m_vLeftArmJoints[1] = swUtil::degree180(- l_rpyLShoulder[0] - 180.);
                    m_vLeftArmJoints[2] = swUtil::degree180(l_rpyLElbow[1] - 90.);
                    m_vLeftArmJoints[3] = swUtil::degree180(- l_rpyLElbow[0] + 90);
                }
                break;
            }

            m_bLeftArmCapture = true;
        }
    }

    // read right arm
    {
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

                std::vector<double> l_vecTorso = swUtil::vec(l_pointTorso, l_pointNeck);
                std::vector<double> l_vecLForearm = swUtil::vec(l_pointLElbow, l_pointLHand);
                std::vector<double> l_vecLArm = swUtil::vec(l_pointLShoulder, l_pointLElbow);

                std::vector<double> l_rpyLShoulder = swUtil::computeRollPitchYaw(l_vecLArm, l_vecTorso);
                std::vector<double> l_rpyLElbow = swUtil::computeRollPitchYaw(l_vecLForearm, l_vecLArm);

                m_vRightArmJoints[0] = swUtil::degree180(l_rpyLShoulder[1] - 180.);
                m_vRightArmJoints[1] = swUtil::degree180(l_rpyLShoulder[0] - 180.);
                m_vRightArmJoints[2] = swUtil::degree180(l_rpyLElbow[1] - 90.);
                m_vRightArmJoints[3] = swUtil::degree180(l_rpyLElbow[0] + 90);


                break;
            }
            m_bRightArmCapture = true;
        }
    }

    //read right hand
//    {
//        l_pLeftHandTarget = m_oLeftHandTrackerPort.read(false);

//        if(l_pLeftHandTarget)
//        {
//            int l_deviceId = l_pLeftHandTarget->get(0).asInt();

//            //std::cout << "Recieved Left Hand data (device " << l_deviceId << ")" << std::endl;
//            switch(l_deviceId)
//            {
//                case swTracking::FASTRAK_LIB:
//                {
//                    if (m_oRobotLeftArmCartesian.isValid())
//                    {
//                        if (!m_bFastrakCalibrated)
//                        {
//                            std::cout << "Place your hand at the level of your belly button and press enter";
//                            std::cin.ignore();
//                            m_vFastrakOffsets.resize(3);
//                            m_vFastrakOffsets[0] = l_pLeftHandTarget->get(2).asDouble();
//                            m_vFastrakOffsets[1] = l_pLeftHandTarget->get(1).asDouble();
//                            m_vFastrakOffsets[2] = l_pLeftHandTarget->get(3).asDouble();
//                            m_bFastrakCalibrated = true;
//                        }


//                        //cartesian position
//                        Vector l_xd(3);
//                        //cartesian orientation
//                        Vector l_ori(3);
//                        Vector l_od(4);

//                        Vector l_joints;


//                        m_pILeftArmCartesian->getPose(l_xd, l_od);
//                        //std::cout << "current : "<< l_xd.toString() << std::endl;

//                        l_xd[0] = (l_pLeftHandTarget->get(2).asDouble() - m_vFastrakOffsets[0]) / (- 70.);
//                        l_xd[1] = (l_pLeftHandTarget->get(1).asDouble() - m_vFastrakOffsets[1]) / (-158.);
//                        l_xd[2] = (l_pLeftHandTarget->get(3).asDouble() - m_vFastrakOffsets[2]) / ( 250.);

//                        double l_degToRad = 3.14159 / 180;

//                        l_ori[0] = l_degToRad * (-l_pLeftHandTarget->get(4).asDouble() + 90);
//                        l_ori[1] = l_degToRad * (-l_pLeftHandTarget->get(5).asDouble());
//                        l_ori[2] = l_degToRad * (-l_pLeftHandTarget->get(6).asDouble());

//                        //std::cout << "new : "<< l_xd.toString() << std::endl;


//                        l_od = dcm2axis(euler2dcm(l_ori));

//                        //std::cout << "l_xd : " << l_xd.toString() << std::endl;

//                        //std::cout << "askForPose() " << m_oRobotLeftArmCartesian.isValid() << std::endl;
//                        Vector l_xdhat, l_odhat;
//                        m_pILeftArmCartesian->setInTargetTol(0.1);
//                        m_pILeftArmCartesian->askForPose(l_xd,l_od,l_xdhat,l_odhat,l_joints);

//                        std::cout << "xd    : " << l_xd.toString() << std::endl;
//                        std::cout << "xdhat : " << l_xdhat.toString() << std::endl;
//                        std::cout << "o. ori: " << l_ori.toString() << std::endl;
//                        std::cout << "ori   : " << dcm2euler(axis2dcm(l_od)).toString() << std::endl;
//                        std::cout << "orihat: " << dcm2euler(axis2dcm(l_odhat)).toString() << std::endl;

//    //                    m_pILeftArmCartesian->goToPose(l_xd,l_od, 4.);

//                        m_vLeftArmJoints[0] = l_joints[3];
//                        m_vLeftArmJoints[1] = l_joints[4];
//                        m_vLeftArmJoints[2] = l_joints[5];
//                        m_vLeftArmJoints[3] = l_joints[6];
//                        m_vLeftArmJoints[4] = l_joints[7];
//                        m_vLeftArmJoints[5] = l_joints[8];
//                        m_vLeftArmJoints[6] = l_joints[9];

//                        m_bLeftArmCapture = true;
//                    }
//                    else
//                    {
//                        std::cerr << "Left hand cartesian controller not initialized, could not process Fastrak data." << std::endl;
//                    }
//                }
//                break;
//            }
//        }
//    }

    //check each joint value to ensure it is in the right range, if not crop to the max/min values
    //  head
        if (l_vHeadJoints[0] > m_i32HeadMaxValueJoint0)
                l_vHeadJoints[0] = m_i32HeadMaxValueJoint0;
        if (l_vHeadJoints[0] < m_i32HeadMinValueJoint0)
                l_vHeadJoints[0] = m_i32HeadMinValueJoint0;

        if (l_vHeadJoints[1] > m_i32HeadMaxValueJoint1)
                l_vHeadJoints[1] = m_i32HeadMaxValueJoint1;
        if (l_vHeadJoints[1] < m_i32HeadMinValueJoint1)
                l_vHeadJoints[1] = m_i32HeadMinValueJoint1;

        if (l_vHeadJoints[2] > m_i32HeadMaxValueJoint2)
                l_vHeadJoints[2] = m_i32HeadMaxValueJoint2;
        if (l_vHeadJoints[2] < m_i32HeadMinValueJoint2)
                l_vHeadJoints[2] = m_i32HeadMinValueJoint2;

    //  torso
        if (m_vTorsoJoints[0] > m_i32TorsoMaxValueJoint0)
                m_vTorsoJoints[0] = m_i32TorsoMaxValueJoint0;
        if (m_vTorsoJoints[0] < m_i32TorsoMinValueJoint0)
                m_vTorsoJoints[0] = m_i32TorsoMinValueJoint0;

        if (m_vTorsoJoints[1] > m_i32TorsoMaxValueJoint1)
                m_vTorsoJoints[1] = m_i32TorsoMaxValueJoint1;
        if (m_vTorsoJoints[1] < m_i32TorsoMinValueJoint1)
                m_vTorsoJoints[1] = m_i32TorsoMinValueJoint1;

        if (m_vTorsoJoints[2] > m_i32TorsoMaxValueJoint2)
                m_vTorsoJoints[2] = m_i32TorsoMaxValueJoint2;
        if (m_vTorsoJoints[2] < m_i32TorsoMinValueJoint2)
                m_vTorsoJoints[2] = m_i32TorsoMinValueJoint2;

    //  left arm
        if (m_vLeftArmJoints[0] > m_i32LeftArmMaxValueJoint0)
                m_vLeftArmJoints[0] = m_i32LeftArmMaxValueJoint0;
        if (m_vLeftArmJoints[0] < m_i32LeftArmMinValueJoint0)
                m_vLeftArmJoints[0] = m_i32LeftArmMinValueJoint0;

        if (m_vLeftArmJoints[1] > m_i32LeftArmMaxValueJoint1)
                m_vLeftArmJoints[1] = m_i32LeftArmMaxValueJoint1;
        if (m_vLeftArmJoints[1] < m_i32LeftArmMinValueJoint1)
                m_vLeftArmJoints[1] = m_i32LeftArmMinValueJoint1;

        if (m_vLeftArmJoints[2] > m_i32LeftArmMaxValueJoint2)
                m_vLeftArmJoints[2] = m_i32LeftArmMaxValueJoint2;
        if (m_vLeftArmJoints[2] < m_i32LeftArmMinValueJoint2)
                m_vLeftArmJoints[2] = m_i32LeftArmMinValueJoint2;

        if (m_vLeftArmJoints[3] > m_i32LeftArmMaxValueJoint3)
                m_vLeftArmJoints[3] = m_i32LeftArmMaxValueJoint3;
        if (m_vLeftArmJoints[3] < m_i32LeftArmMinValueJoint3)
                m_vLeftArmJoints[3] = m_i32LeftArmMinValueJoint3;

        if (m_vLeftArmJoints[4] > m_i32LeftArmMaxValueJoint4)
                m_vLeftArmJoints[4] = m_i32LeftArmMaxValueJoint4;
        if (m_vLeftArmJoints[4] < m_i32LeftArmMinValueJoint4)
                m_vLeftArmJoints[4] = m_i32LeftArmMinValueJoint4;

        if (m_vLeftArmJoints[5] > m_i32LeftArmMaxValueJoint5)
                m_vLeftArmJoints[5] = m_i32LeftArmMaxValueJoint5;
        if (m_vLeftArmJoints[5] < m_i32LeftArmMinValueJoint5)
                m_vLeftArmJoints[5] = m_i32LeftArmMinValueJoint5;

        if (m_vLeftArmJoints[6] > m_i32LeftArmMaxValueJoint6)
                m_vLeftArmJoints[6] = m_i32LeftArmMaxValueJoint6;
        if (m_vLeftArmJoints[6] < m_i32LeftArmMinValueJoint6)
                m_vLeftArmJoints[6] = m_i32LeftArmMinValueJoint6;

        if (m_vLeftArmJoints[7] > m_i32LeftArmMaxValueJoint7)
                m_vLeftArmJoints[7] = m_i32LeftArmMaxValueJoint7;
        if (m_vLeftArmJoints[7] < m_i32LeftArmMinValueJoint7)
                m_vLeftArmJoints[7] = m_i32LeftArmMinValueJoint7;

        if (m_vLeftArmJoints[8] > m_i32LeftArmMaxValueJoint8)
                m_vLeftArmJoints[8] = m_i32LeftArmMaxValueJoint8;
        if (m_vLeftArmJoints[8] < m_i32LeftArmMinValueJoint8)
                m_vLeftArmJoints[8] = m_i32LeftArmMinValueJoint8;

        if (m_vLeftArmJoints[9] > m_i32LeftArmMaxValueJoint9)
                m_vLeftArmJoints[9] = m_i32LeftArmMaxValueJoint9;
        if (m_vLeftArmJoints[9] < m_i32LeftArmMinValueJoint9)
                m_vLeftArmJoints[9] = m_i32LeftArmMinValueJoint9;

        if (m_vLeftArmJoints[10] > m_i32LeftArmMaxValueJoint10)
                m_vLeftArmJoints[10] = m_i32LeftArmMaxValueJoint10;
        if (m_vLeftArmJoints[10] < m_i32LeftArmMinValueJoint10)
                m_vLeftArmJoints[10] = m_i32LeftArmMinValueJoint10;

        if (m_vLeftArmJoints[11] > m_i32LeftArmMaxValueJoint11)
                m_vLeftArmJoints[11] = m_i32LeftArmMaxValueJoint11;
        if (m_vLeftArmJoints[11] < m_i32LeftArmMinValueJoint11)
                m_vLeftArmJoints[11] = m_i32LeftArmMinValueJoint11;

        if (m_vLeftArmJoints[12] > m_i32LeftArmMaxValueJoint12)
                m_vLeftArmJoints[12] = m_i32LeftArmMaxValueJoint12;
        if (m_vLeftArmJoints[12] < m_i32LeftArmMinValueJoint12)
                m_vLeftArmJoints[12] = m_i32LeftArmMinValueJoint12;

        if (m_vLeftArmJoints[13] > m_i32LeftArmMaxValueJoint13)
                m_vLeftArmJoints[13] = m_i32LeftArmMaxValueJoint13;
        if (m_vLeftArmJoints[13] < m_i32LeftArmMinValueJoint13)
                m_vLeftArmJoints[13] = m_i32LeftArmMinValueJoint13;

        if (m_vLeftArmJoints[14] > m_i32LeftArmMaxValueJoint14)
                m_vLeftArmJoints[14] = m_i32LeftArmMaxValueJoint14;
        if (m_vLeftArmJoints[14] < m_i32LeftArmMinValueJoint14)
                m_vLeftArmJoints[14] = m_i32LeftArmMinValueJoint14;

        if (m_vLeftArmJoints[15] > m_i32LeftArmMaxValueJoint15)
                m_vLeftArmJoints[15] = m_i32LeftArmMaxValueJoint15;
        if (m_vLeftArmJoints[15] < m_i32LeftArmMinValueJoint15)
                m_vLeftArmJoints[15] = m_i32LeftArmMinValueJoint15;

    // right arm
        if (m_vRightArmJoints[0] > m_i32RightArmMaxValueJoint0)
                m_vRightArmJoints[0] = m_i32RightArmMaxValueJoint0;
        if (m_vRightArmJoints[0] < m_i32RightArmMinValueJoint0)
                m_vRightArmJoints[0] = m_i32RightArmMinValueJoint0;

        if (m_vRightArmJoints[1] > m_i32RightArmMaxValueJoint1)
                m_vRightArmJoints[1] = m_i32RightArmMaxValueJoint1;
        if (m_vRightArmJoints[1] < m_i32RightArmMinValueJoint1)
                m_vRightArmJoints[1] = m_i32RightArmMinValueJoint1;

        if (m_vRightArmJoints[2] > m_i32RightArmMaxValueJoint2)
                m_vRightArmJoints[2] = m_i32RightArmMaxValueJoint2;
        if (m_vRightArmJoints[2] < m_i32RightArmMinValueJoint2)
                m_vRightArmJoints[2] = m_i32RightArmMinValueJoint2;

        if (m_vRightArmJoints[3] > m_i32RightArmMaxValueJoint3)
                m_vRightArmJoints[3] = m_i32RightArmMaxValueJoint3;
        if (m_vRightArmJoints[3] < m_i32RightArmMinValueJoint3)
                m_vRightArmJoints[3] = m_i32RightArmMinValueJoint3;

        if (m_vRightArmJoints[4] > m_i32RightArmMaxValueJoint4)
                m_vRightArmJoints[4] = m_i32RightArmMaxValueJoint4;
        if (m_vRightArmJoints[4] < m_i32RightArmMinValueJoint4)
                m_vRightArmJoints[4] = m_i32RightArmMinValueJoint4;

        if (m_vRightArmJoints[5] > m_i32RightArmMaxValueJoint5)
                m_vRightArmJoints[5] = m_i32RightArmMaxValueJoint5;
        if (m_vRightArmJoints[5] < m_i32RightArmMinValueJoint5)
                m_vRightArmJoints[5] = m_i32RightArmMinValueJoint5;

        if (m_vRightArmJoints[6] > m_i32RightArmMaxValueJoint6)
                m_vRightArmJoints[6] = m_i32RightArmMaxValueJoint6;
        if (m_vRightArmJoints[6] < m_i32RightArmMinValueJoint6)
                m_vRightArmJoints[6] = m_i32RightArmMinValueJoint6;

        if (m_vRightArmJoints[7] > m_i32RightArmMaxValueJoint7)
                m_vRightArmJoints[7] = m_i32RightArmMaxValueJoint7;
        if (m_vRightArmJoints[7] < m_i32RightArmMinValueJoint7)
                m_vRightArmJoints[7] = m_i32RightArmMinValueJoint7;

        if (m_vRightArmJoints[8] > m_i32RightArmMaxValueJoint8)
                m_vRightArmJoints[8] = m_i32RightArmMaxValueJoint8;
        if (m_vRightArmJoints[8] < m_i32RightArmMinValueJoint8)
                m_vRightArmJoints[8] = m_i32RightArmMinValueJoint8;

        if (m_vRightArmJoints[9] > m_i32RightArmMaxValueJoint9)
                m_vRightArmJoints[9] = m_i32RightArmMaxValueJoint9;
        if (m_vRightArmJoints[9] < m_i32RightArmMinValueJoint9)
                m_vRightArmJoints[9] = m_i32RightArmMinValueJoint9;

        if (m_vRightArmJoints[10] > m_i32RightArmMaxValueJoint10)
                m_vRightArmJoints[10] = m_i32RightArmMaxValueJoint10;
        if (m_vRightArmJoints[10] < m_i32RightArmMinValueJoint10)
                m_vRightArmJoints[10] = m_i32RightArmMinValueJoint10;

        if (m_vRightArmJoints[11] > m_i32RightArmMaxValueJoint11)
                m_vRightArmJoints[11] = m_i32RightArmMaxValueJoint11;
        if (m_vRightArmJoints[11] < m_i32RightArmMinValueJoint11)
                m_vRightArmJoints[11] = m_i32RightArmMinValueJoint11;

        if (m_vRightArmJoints[12] > m_i32RightArmMaxValueJoint12)
                m_vRightArmJoints[12] = m_i32RightArmMaxValueJoint12;
        if (m_vRightArmJoints[12] < m_i32RightArmMinValueJoint12)
                m_vRightArmJoints[12] = m_i32RightArmMinValueJoint12;

        if (m_vRightArmJoints[13] > m_i32RightArmMaxValueJoint13)
                m_vRightArmJoints[13] = m_i32RightArmMaxValueJoint13;
        if (m_vRightArmJoints[13] < m_i32RightArmMinValueJoint13)
                m_vRightArmJoints[13] = m_i32RightArmMinValueJoint13;

        if (m_vRightArmJoints[14] > m_i32RightArmMaxValueJoint14)
                m_vRightArmJoints[14] = m_i32RightArmMaxValueJoint14;
        if (m_vRightArmJoints[14] < m_i32RightArmMinValueJoint14)
                m_vRightArmJoints[14] = m_i32RightArmMinValueJoint14;

        if (m_vRightArmJoints[15] > m_i32RightArmMaxValueJoint15)
                m_vRightArmJoints[15] = m_i32RightArmMaxValueJoint15;
        if (m_vRightArmJoints[15] < m_i32RightArmMinValueJoint15)
                m_vRightArmJoints[15] = m_i32RightArmMinValueJoint15;

    if(l_pHeadTarget || l_pGazeTarget) // TODO : ...
    {
        l_vHeadCommand    = 0.0;

        // head rotation
        l_vHeadCommand[0] = 0.9*(l_vHeadJoints[0] - l_vHeadEncoders[0]);
        l_vHeadCommand[1] = 0.9*(l_vHeadJoints[1] - l_vHeadEncoders[1]);
        l_vHeadCommand[2] = 0.9*(l_vHeadJoints[2] - l_vHeadEncoders[2]);

        // gaze
        l_vHeadCommand[3] = 1.0 * (l_vHeadJoints[3] - l_vHeadEncoders[3]);
        l_vHeadCommand[4] = 1.0 * (l_vHeadJoints[4] - l_vHeadEncoders[4]);
        l_vHeadCommand[5] = 1.0 * (l_vHeadJoints[5] - l_vHeadEncoders[5]);

        // move
        std::cout << "velocityMove Head" << std::endl;
        m_pIHeadVelocity->velocityMove(l_vHeadCommand.data());
    }

    if(l_pTorsoTarget)
    {
        l_vTorsoCommand    = 0.0;

        // torso rotation
        l_vTorsoCommand[0] = 0.9*(m_vTorsoJoints[0] - l_vTorsoEncoders[0]);
        l_vTorsoCommand[1] = 0.9*(m_vTorsoJoints[1] - l_vTorsoEncoders[1]);
        l_vTorsoCommand[2] = 0.9*(m_vTorsoJoints[2] - l_vTorsoEncoders[2]);

        // move
        std::cout << "velocityMove Torso " << std::endl;
        m_pITorsoVelocity->velocityMove(l_vTorsoCommand.data());
    }

    if(m_bLeftArmCapture)
    {
        l_vLeftArmCommand = 0.;

        for(int ii = 0; ii < 16; ++ii)
        {
            l_vLeftArmCommand[ii] = 0.9 *(m_vLeftArmJoints[ii] - l_vLeftArmEncoders[ii]);
        }

        // move
        std::cout << "velocityMove Left Arm" << std::endl;
        m_pILeftArmVelocity->velocityMove(l_vLeftArmCommand.data());

    }

    if(m_bRightArmCapture)
    {
        l_vRightArmCommand = 0.;

        for(int ii = 0; ii < 4; ++ii)
        {
            l_vRightArmCommand[ii] = 0.9 *(m_vRightArmJoints[ii] - l_vRightArmEncoders[ii]);
        }

        // move
        std::cout << "velocityMove Right Arm" << std::endl;
        m_pIRightArmVelocity->velocityMove(l_vRightArmCommand.data());
    }

//    if(l_pFaceTarget)
//    {

//    }

    return true;
}


double SWTeleoperation_iCub::getPeriod()
{
    return 1./m_i32Fps;
}


std::string SWTeleoperation_iCub::eyesOpeningCode(cdouble dEyeLids, cint i32MinValue, cint i32MaxValue)
{
    int l_i32ScaledValue = static_cast<int>((i32MinValue + (i32MaxValue - i32MinValue) * dEyeLids));

    std::ostringstream l_osCodeValue;
    l_osCodeValue << 'S';
    l_osCodeValue << l_i32ScaledValue;

    return l_osCodeValue.str();
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
    SWTeleoperation_iCub l_oTeleoperation_iCub;

    /* prepare and configure the resource finder */
    ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("teleoperation_iCub.ini"); //overridden by --from parameter
    rf.setDefaultContext("swooz-teleoperation/conf");   //overridden by --context parameter
    rf.configure("ICUB_ROOT", argc, argv);

    /* configure the module */
    std::cout << "Configuring the iCub Teleoperation module..."<< std::endl;
    if (l_oTeleoperation_iCub.configure(rf))
    {
        /* run the module */
        std::cout << "Starting the iCub Teleoperation module..." << std::endl;
        l_oTeleoperation_iCub.runModule();
    }
    else
    {
        std::cerr << "Failed to configure the iCub Teleoperation module!"<< std::endl;
    }

    return 0;
}




/**
 * \file SWIcubArm.cpp
 * \author Florian Lance
 * \date 29-04-2014
 * \brief Defines SWIcubArm
 */


#include <sstream>

#include "geometryUtility.h"
#include "SWTrackingDevice.h"

#include "icub/SWIcubArm.h"
#include <yarp/math/Math.h>

using namespace yarp::os;


swTeleop::SWIcubArm::SWIcubArm() : m_bInitialized(false), m_bIsRunning(false), m_dArmTimeLastBottle(-1.),
                                       m_pIArmVelocity(NULL), m_pIArmEncoders(NULL), m_pIArmPosition(NULL), m_pVelocityController(NULL)
{
    // set ini file defaults values
        // parts to be activated
            m_bArmActivatedDefault = 1;

        // timeout
            m_i32TimeoutArmResetDefault = 3000;

        // accelerations / speeds
            double l_aDMinJointDefault[]                       = {-95., 0., -37., 16., -90., -90., -20., 0., 10., 0., 0., 0., 0., 0., 0., 0.};
            double l_aDMaxJointDefault[]                       = { 10., 160., 80., 106., 90., 0., 40., 60., 90., 90., 180., 90., 180., 90., 180., 270.};
            double l_aDArmResetPosition[]                      = {-25.,20.,0.,50.,0.,0.,0.,60.,20.,20.,20.,10.,10.,10.,10.,10.};
            double l_aDArmJointVelocityDefault[]               = {50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.};
            double l_aDArmJointVelocityKDefault[]              = {0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9};
            double l_aDArmJointPositionAccelerationDefault[]   = {50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.};
            double l_aDArmJointPositionSpeedDefault[]          = {50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.};
            m_vArmMinJointDefault                  = std::vector<double>(l_aDMinJointDefault, l_aDMinJointDefault + sizeof(l_aDMinJointDefault) / sizeof(double));
            m_vArmMaxJointDefault                  = std::vector<double>(l_aDMaxJointDefault, l_aDMaxJointDefault + sizeof(l_aDMaxJointDefault) / sizeof(double));
            m_vArmResetPositionDefault             = std::vector<double>(l_aDArmResetPosition, l_aDArmResetPosition + sizeof(l_aDArmResetPosition) / sizeof(double));
            m_vArmJointVelocityKDefault            = std::vector<double>(l_aDArmJointVelocityKDefault, l_aDArmJointVelocityKDefault + sizeof(l_aDArmJointVelocityKDefault) / sizeof(double));
            m_vArmJointVelocityAccelerationDefault = std::vector<double>(l_aDArmJointVelocityDefault, l_aDArmJointVelocityDefault + sizeof(l_aDArmJointVelocityDefault) / sizeof(double));
            m_vArmJointPositionAccelerationDefault = std::vector<double>(l_aDArmJointPositionAccelerationDefault, l_aDArmJointPositionAccelerationDefault + sizeof(l_aDArmJointPositionAccelerationDefault) / sizeof(double));
            m_vArmJointPositionSpeedDefault        = std::vector<double>(l_aDArmJointPositionSpeedDefault, l_aDArmJointPositionSpeedDefault + sizeof(l_aDArmJointPositionSpeedDefault) / sizeof(double));

            m_vArmMinJoint                    = std::vector<double>(m_vArmMinJointDefault.size());
            m_vArmMaxJoint                    = std::vector<double>(m_vArmMaxJointDefault.size());
            m_vArmResetPosition               = std::vector<double>(m_vArmResetPositionDefault.size());
            m_vArmJointVelocityAcceleration   = std::vector<double>(m_vArmJointVelocityAccelerationDefault.size());
            m_vArmJointVelocityK              = std::vector<double>(m_vArmJointVelocityKDefault.size());
            m_vArmJointPositionAcceleration   = std::vector<double>(m_vArmJointPositionAccelerationDefault.size());
            m_vArmJointPositionSpeed          = std::vector<double>(m_vArmJointPositionSpeedDefault.size());
            m_i32ArmJointsNb = m_vArmMinJoint.size();

}

swTeleop::SWIcubArm::~SWIcubArm()
{
    if(m_pVelocityController)
    {
        while(m_pVelocityController->isRunning())
        {
            yarp::os::Time::delay(0.1);
        }
    }

    deleteAndNullify(m_pVelocityController);
}

bool swTeleop::SWIcubArm::init( yarp::os::ResourceFinder &oRf, bool bLeftArm)
{

    if(m_bInitialized)
    {
        std::cerr << "Icub Arm is already initialized. " << std::endl;
        return true;
    }

    if(bLeftArm)
    {
        m_sArm = "left";
    }
    else
    {
        m_sArm = "right";
    }

    // gets the module name which will form the stem of all module port names
        m_sModuleName   = oRf.check("name", Value("teleoperation_iCub"), "Teleoperation/iCub Module name (string)").asString();
        m_sRobotName    = oRf.check("robot",Value("icubSim"),  "Robot name (string)").asString();

    // robot parts to control

        m_bArmActivated = oRf.check(std::string(m_sArm + "ArmActivated").c_str(), Value(m_bArmActivatedDefault), std::string(m_sArm + " Arm activated (int)").c_str()).asInt() != 0;

        if(!m_bArmActivated)
        {
            std::cout << m_sArm + " arm not activated, icub " + m_sArm + " arm initialization aborted. " << std::endl;
            return (m_bInitialized = false);
        }

    // min / max values for iCub Torso joints
        for(uint ii = 0; ii < m_vArmJointVelocityAcceleration.size(); ++ii)
        {
            std::ostringstream l_os;
            l_os << ii;

            std::string l_sMinJoint(m_sArm + "ArmMinValueJoint" + l_os.str());
            std::string l_sMaxJoint(m_sArm + "ArmMaxValueJoint" + l_os.str());
            std::string l_sArmResetPosition(m_sArm + "ArmResetPosition" + l_os.str());
            std::string l_sArmJointVelocityAcceleration(m_sArm + "ArmJointVelocityAcceleration" + l_os.str());
            std::string l_sArmJointVelocityK(m_sArm + "ArmJointVelocityK" + l_os.str());
            std::string l_sArmJointPositionAcceleration(m_sArm + "ArmJointPositionAcceleration" + l_os.str());
            std::string l_sArmJointPositionSpeed(m_sArm + "ArmJointPositionSpeed" + l_os.str());

            std::string l_sMinJointInfo(m_sArm + " arm minimum joint" + l_os.str() + " Value (double)");
            std::string l_sMaxJointInfo(m_sArm + " arm maximum joint" + l_os.str() + " Value (double)");
            std::string l_sArmResetPositionInfo(m_sArm + " arm reset position " + l_os.str() + " Value (double)");
            std::string l_sArmJointVelocityAccelerationInfo(m_sArm + " arm joint velocity acceleration " + l_os.str() + " Value (double)");
            std::string l_sArmJointVelocityKInfo(m_sArm + " arm joint velocity K coeff"+ l_os.str() + " Value (double)");
            std::string l_sArmJointPositionAccelerationInfo(m_sArm + " arm joint position acceleration " + l_os.str() + " Value (double)");
            std::string l_sArmJointPositionSpeedInfo(m_sArm + " arm joint position speed " + l_os.str() + " Value (double)");

            m_vArmMinJoint[ii] = oRf.check(l_sMinJoint.c_str(), m_vArmMinJointDefault[ii], l_sMinJointInfo.c_str()).asDouble();
            m_vArmMaxJoint[ii] = oRf.check(l_sMaxJoint.c_str(), m_vArmMaxJointDefault[ii], l_sMaxJointInfo.c_str()).asDouble();
            m_vArmResetPosition[ii] = oRf.check(l_sArmResetPosition.c_str(), m_vArmResetPositionDefault[ii], l_sArmResetPositionInfo.c_str()).asDouble();
            m_vArmJointVelocityAcceleration[ii]= oRf.check(l_sArmJointVelocityAcceleration.c_str(), m_vArmJointVelocityAccelerationDefault[ii], l_sArmJointVelocityAccelerationInfo.c_str()).asDouble();
            m_vArmJointPositionAcceleration[ii]= oRf.check(l_sArmJointPositionAcceleration.c_str(), m_vArmJointPositionAccelerationDefault[ii], l_sArmJointPositionAccelerationInfo.c_str()).asDouble();
            m_vArmJointPositionSpeed[ii]       = oRf.check(l_sArmJointPositionSpeed.c_str(),        m_vArmJointPositionSpeedDefault[ii],        l_sArmJointPositionSpeedInfo.c_str()).asDouble();
            m_vArmJointVelocityK[ii]           = oRf.check(l_sArmJointVelocityK.c_str(),            m_vArmJointVelocityKDefault[ii],            l_sArmJointVelocityKInfo.c_str()).asDouble();
        }

    // miscellaneous
        m_i32TimeoutArmReset   = oRf.check(std::string(m_sArm + "ArmTimeoutReset").c_str(),       Value(m_i32TimeoutArmResetDefault), std::string(m_sArm + " arm timeout reset iCub (int)").c_str()).asInt();

    // set polydriver options
        m_oArmOptions.put("robot",     m_sRobotName.c_str());
        m_oArmOptions.put("device",    "remote_controlboard");
        m_oArmOptions.put("local",    ("/local/" + m_sRobotName + "/" + m_sArm + "_arm").c_str());
        m_oArmOptions.put("name",     ("/" + m_sRobotName + "/" + m_sArm + "_arm").c_str());
        m_oArmOptions.put("remote",   ("/" + m_sRobotName + "/" + m_sArm + "_arm").c_str());

    //set cartesian polydriver options
        m_oArmCartesianOptions.put("robot",     m_sRobotName.c_str());
        m_oArmCartesianOptions.put("device",    "cartesiancontrollerclient");
        m_oArmCartesianOptions.put("local",    ("/cartesian_client/" + m_sArm + "_arm"));
        m_oArmCartesianOptions.put("name",     ("/" + m_sRobotName + "/cartesianController/" + m_sArm + "_arm").c_str());
        m_oArmCartesianOptions.put("remote",   ("/" + m_sRobotName + "/cartesianController/" + m_sArm + "_arm").c_str());


    // init polydriver
        m_oRobotArm.open(m_oArmOptions);
        if(!m_oRobotArm.isValid())
        {
            std::cerr << std::endl <<"-ERROR: " << m_sArm << " robotArm is not valid, escape arm initialization. " << std::endl <<std::endl;
            return (m_bInitialized=false);
        }

    //init cartesian polydriver

        if(!m_oRobotArmCartesian.open(m_oArmCartesianOptions))
        {
            std::cout<<"Failing at OPENING polydriver (RobotArmCartesian)"<<std::endl;
        }

        if (!m_oRobotArmCartesian.isValid())
        {
            std::cerr << std::endl <<"-ERROR: " << m_sArm << " arm cartesian is not valid" << std::endl <<std::endl;
            return (m_bInitialized=false);
        }
        else
        {
                // initializing controllers
                if (!m_oRobotArmCartesian.view(m_pIArmCartesian))
                {
                    std::cerr << "-ERROR: Couldn't open the "<< m_sArm <<" Arm ICartesianControl client!" << std::endl;
                    return false;
                }
                //  limit the torso DOF for the cartesian controller
                yarp::sig::Vector l_torsoDof(3);
                l_torsoDof = 2.; // This values tells the kinematic solver to skip the torso
                if( m_pIArmCartesian->setDOF(l_torsoDof, l_torsoDof))
                {
                    std::cerr << "-WARNING: Unable to set the torso DOFs";
                }
        }


    // initializing controllers
        if (!m_oRobotArm.view(m_pIArmVelocity) || !m_oRobotArm.view(m_pIArmPosition) || !m_oRobotArm.view(m_pIArmEncoders))
        {
            std::cerr << std::endl <<  "-ERROR: " << m_sArm << " while getting required robot Arm interfaces." << std::endl <<std::endl;
            m_oRobotArm.close();
            return (m_bInitialized=false);
        }

    // init ports
        m_sArmTrackerPortName      = "/teleoperation/" + m_sRobotName + "/" + m_sArm + "_arm";
        m_sHandTrackerPortName      = "/teleoperation/" + m_sRobotName + "/" + m_sArm + "_arm/hand";
        m_sFingersTrackerPortName   = "/teleoperation/" + m_sRobotName + "/" + m_sArm + "_arm/fingers";

    // open ports
        bool l_bPortOpeningSuccess = true;
        if(m_bArmActivated)
        {
            l_bPortOpeningSuccess = m_oArmTrackerPort.open(m_sArmTrackerPortName.c_str());

            if(l_bPortOpeningSuccess)
                l_bPortOpeningSuccess = m_oFingersTrackerPort.open(m_sFingersTrackerPortName.c_str());

            if(l_bPortOpeningSuccess)
                 l_bPortOpeningSuccess = m_oHandTrackerPort.open(m_sHandTrackerPortName.c_str());
        }

        if(!l_bPortOpeningSuccess)
        {
            std::cerr << std::endl <<"-ERROR: Unable to open ports." << std::endl <<std::endl;
            m_oRobotArm.close();
            m_oRobotArmCartesian.close();
            return (m_bInitialized=false);
        }

    // retrieve Torso number of joints
        m_pIArmPosition->getAxes(&m_i32ArmJointsNb);

    // set accelerations/speeds
        for(int ii = 0; ii < m_i32ArmJointsNb; ++ii)
        {
            m_pIArmPosition->setRefAcceleration(ii, m_vArmJointPositionAcceleration[ii]);
            m_pIArmPosition->setRefSpeed(ii, m_vArmJointPositionSpeed[ii]);
            m_pIArmVelocity->setRefAcceleration(ii, m_vArmJointVelocityAcceleration[ii]);
        }

    // init controller
        m_pVelocityController = new swTeleop::SWArmVelocityController(m_pIArmEncoders, m_pIArmVelocity, m_vArmJointVelocityK, 10);
        m_pVelocityController->enableArm(m_bArmActivated);

        // display parameters
            std::cout << std::endl << std::endl;
            displayDebug(m_sArm + std::string(" arm activated"), m_bArmActivated);
            displayDebug(std::string("Gaze activated"), m_i32TimeoutArmReset);
            std::cout << std::endl;
            displayVectorDebug(m_sArm + std::string(" arm min joint                  : "), m_vArmMinJoint);
            displayVectorDebug(m_sArm + std::string(" arm max joint                  : "), m_vArmMaxJoint);
            displayVectorDebug(m_sArm + std::string(" arm reset position joint       : "), m_vArmResetPosition);
            displayVectorDebug(m_sArm + std::string(" arm joint velocity acceleration: "), m_vArmJointVelocityAcceleration);
            displayVectorDebug(m_sArm + std::string(" arm joint position acceleration: "), m_vArmJointPositionAcceleration);
            displayVectorDebug(m_sArm + std::string(" arm joint position speed       : "), m_vArmJointPositionSpeed);
            displayVectorDebug(m_sArm + std::string(" arm head joint velocity        : "), m_vArmJointVelocityK);
            std::cout << std::endl << std::endl;

    return (m_bIsRunning=m_bInitialized=true);
}

bool swTeleop::SWIcubArm::checkBottles()
{
    if(!m_bIsRunning)
    {
        return false;
    }


    if(!m_bInitialized)
    {
        std::cerr << "Icub Arm control module not initialized. " << std::endl;
        return (m_bIsRunning=false);
    }

    // init command vector
        yarp::sig::Vector l_vArmJoints;
        l_vArmJoints.resize(m_i32ArmJointsNb);
        l_vArmJoints = 0.;
    // set default values to arm joint
        for(uint ii = 0; ii < l_vArmJoints.size(); ++ii)
        {
            l_vArmJoints[ii] = m_vArmResetPositionDefault[ii];
        }

    // defines bottles
        Bottle *l_pArmTarget = NULL,*l_pFingersTarget = NULL,*l_pHandTarget = NULL;

        // read arm commands
        if(m_bArmActivated)
        {
            l_pArmTarget = m_oArmTrackerPort.read(false);

            if (l_pArmTarget)
            {
                int l_deviceId = l_pArmTarget->get(0).asInt();

                switch(l_deviceId)
                {
                    case swTracking::DUMMY_LIB :
                    {
                        for(uint ii = 0; ii < l_vArmJoints.size(); ++ii)
                        {
                            l_vArmJoints[ii] = l_pArmTarget->get(ii+1).asDouble();
                        }
                    }
                    break;
                    case swTracking::OPENNI_LIB :
                    {
                        std::vector<double> l_pointTorso(3), l_pointNeck(3), l_pointShoulder(3), l_pointElbow(3), l_pointHand(3);
                            l_pointTorso[0] = l_pArmTarget->get(1).asDouble();
                            l_pointTorso[1] = l_pArmTarget->get(2).asDouble();
                            l_pointTorso[2] = l_pArmTarget->get(3).asDouble();
                            l_pointNeck[0] = l_pArmTarget->get(4).asDouble();
                            l_pointNeck[1] = l_pArmTarget->get(5).asDouble();
                            l_pointNeck[2] = l_pArmTarget->get(6).asDouble();
                            l_pointShoulder[0] = l_pArmTarget->get(7).asDouble();
                            l_pointShoulder[1] = l_pArmTarget->get(8).asDouble();
                            l_pointShoulder[2] = l_pArmTarget->get(9).asDouble();
                            l_pointElbow[0] = l_pArmTarget->get(10).asDouble();
                            l_pointElbow[1] = l_pArmTarget->get(11).asDouble();
                            l_pointElbow[2] = l_pArmTarget->get(12).asDouble();
                            l_pointHand[0] = l_pArmTarget->get(13).asDouble();
                            l_pointHand[1] = l_pArmTarget->get(14).asDouble();
                            l_pointHand[2] = l_pArmTarget->get(15).asDouble();

                        std::vector<double> l_vecTorso = swUtil::vec(l_pointTorso, l_pointNeck);
                        std::vector<double> l_vecForearm = swUtil::vec(l_pointElbow, l_pointHand);
                        std::vector<double> l_vecArm = swUtil::vec(l_pointShoulder, l_pointElbow);

                        std::vector<double> l_rpyShoulder = swUtil::computeRollPitchYaw(l_vecArm, l_vecTorso);
                        std::vector<double> l_rpyElbow = swUtil::computeRollPitchYaw(l_vecForearm, l_vecArm);

                        l_vArmJoints[0] = swUtil::degree180(l_rpyShoulder[1] - 180.);
                        l_vArmJoints[1] = swUtil::degree180(- l_rpyShoulder[0] - 180.);
                        l_vArmJoints[2] = swUtil::degree180(l_rpyElbow[1] - 90.);
                        l_vArmJoints[3] = swUtil::degree180(- l_rpyElbow[0] + 90);
                    }
                    break;
                    case swTracking::LEAP_LIB :
                    {

                        l_pHandTarget = m_oHandTrackerPort.read(false);
                        l_pFingersTarget = m_oFingersTrackerPort.read(false);

                        if(!l_pHandTarget)
                        {

                            std::cout<<"TIPS : Failling to receive an Hand bottle - Skip "<<std::endl;
                            break;
                        }
                        if(!l_pFingersTarget)
                        {

                            std::cout<<"TIPS : Failling to receive a Fingers bottle - Skip"<<std::endl;
                            break;
                        }

//                        if (m_oRobotArmCartesian.isValid())
//                        {

                        //	m_int32cptframe++; //smooth function

                            yarp::sig::Vector x0,o0;
                            yarp::sig::Vector od(3);
                            yarp::sig::Vector l_od(4);
                        //	yarp::sig::Vector l_aLeftArmTmp;


                            //m_pIArmCartesian->getPose(x0,o0); //getting the position X/Y/Z and the orientation of the hand

                            /*
                            m_dsmoothx+=l_pHandTarget->get(3).asDouble()/(150);//Smooth
                            m_dsmoothy+=l_pHandTarget->get(1).asDouble()/(150);//Smooth
                            m_dsmoothz+=(l_pHandTarget->get(2).asDouble()-250)/(150);//Smooth
                            */

                            yarp::sig::Vector xd=x0;

                            xd[0]+=l_pHandTarget->get(3).asDouble()/(400);
                            xd[1]+=l_pHandTarget->get(1).asDouble()/(400);
                            xd[2]+=(l_pHandTarget->get(2).asDouble()-250)/(400);//On Y (leap), you can only move from 0 to XXX  so to get negatives position on Z (Icub) we need to substract 250 !

                            //std::cout<<"X : "<<l_pHandTarget->get(3).asDouble()<<" Y : "<<l_pHandTarget->get(1).asDouble()<<" Z : "<<l_pHandTarget->get(2).asDouble()<<std::endl;
                            /*
                            xd[0]+=m_dsmoothx/m_int32cptframe;//Smooth
                            xd[1]+=m_dsmoothy/m_int32cptframe;//Smooth
                            xd[2]+=m_dsmoothy/m_int32cptframe;//Smooth
                            */

                            //reverse because in Icub this is [PITCH,ROLL,YAW]

                            od[0]=l_pHandTarget->get(4).asDouble();
                            od[1]=l_pHandTarget->get(5).asDouble();
                            od[2]=l_pHandTarget->get(6).asDouble();



                            //l_od=iCub::ctrl::dcm2axis(iCub::ctrl::euler2dcm(od));

                            // m_oqdhat.resize(m_i32ArmJointsNb); ################################################ ?
                            yarp::sig::Vector xdhat,odhat;

                            yarp::sig::Vector l_vLeftArmEncoders;
                            l_vLeftArmEncoders.resize(m_i32ArmJointsNb);


                            m_pIArmEncoders->getEncoders(l_vLeftArmEncoders.data());

                            //m_pIArmCartesian->askForPosition(l_vLeftArmEncoders,xd,xdhat,odhat,m_oqdhat);
                            //or
                            //m_pIArmCartesian->goToPosition(xd,0.8);


                            //Setting Speeds and Accelerations for each joints
                /*
                            l_aLeftArmTmp.resize(m_i32ArmJointsNb);
                            for (int i = 0; i < m_i32ArmJointsNb ; i++)
                            {

                                l_aLeftArmTmp[i] = 100;
                            }
                            m_pIArmPosition->setRefAccelerations(l_aLeftArmTmp.data());
                            m_pIArmPosition->setRefSpeeds(l_aLeftArmTmp.data());
    */



                            /*
                            TIPS : When we get m_oqdhat, the joints from 0 to 2 are use to control the torso (This is possible with the cartesian Controler) that's why we start from the 4th.
                            Askforposition/pose gives you the joint configuration with the base of the DOF of the arm. */
                            /*
                            double l_SumFingerDist;
                            double l_finger1 = l_pFingersTarget->get(1).asDouble();
                            double l_finger2 = l_pFingersTarget->get(4).asDouble();
                            double l_finger3 = l_pFingersTarget->get(7).asDouble();
                            double l_finger4 = l_pFingersTarget->get(10).asDouble();
                            double l_finger5 = l_pFingersTarget->get(13).asDouble();

                            l_SumFingerDist = (l_finger5 - l_finger4) + (l_finger4 - l_finger3) + (l_finger3 - l_finger2) + (l_finger2 - l_finger1);
                            */
                            //std::cout<<"The average distance between finger is : "<<m_faveragediff/m_int32cptframe<<std::endl;



                            /*

                            //TEMPLATE
                                                                //Arm
                                                                    l_vArmJoints[ 0] = m_oqdhat[ 3];
                                                                    l_vArmJoints[ 1] = m_oqdhat[ 4];
                                                                    l_vArmJoints[ 2] = m_oqdhat[ 5];
                                                                    l_vArmJoints[ 3] = m_oqdhat[ 6];


                                                                //Roll Pitch Yaw
                                                                    l_vArmJoints[ 4] = m_oqdhat[ 7];//(l_pHandTarget->get(5).asDouble()*180 / 3.1415)/2*(-1);   // RAD TO DEG | [-180:180] / 2 | [-90:90] * (-1) | [90:-90]
                                                                    l_vArmJoints[ 5] = m_oqdhat[ 8];//(l_pHandTarget->get(4).asDouble()*180 / 3.1415)*(-1);     // RAD TO DEG | [-180:180]
                                                                    l_vArmJoints[ 6] = m_oqdhat[ 9];//(l_pHandTarget->get(6).asDouble()*180 / 3.1415)*(-1);     // RAD TO DEG | [-180:180]

                                                                //Fingers (0 hand open)
                                                                    //Space Between Fingers
                                                                    l_vArmJoints[ 7] = 15 ;// 60 - (((l_SumFingerDist/60)-1)*30);						 // [60:180] /60 | [1:3]  - 1 | [0:2] * 30  | 60  - [0:60]	| [60:0]

                                                                    l_vArmJoints[ 8] = l_pFingersTarget->get(1).asDouble();
                                                                    l_vArmJoints[ 9] = 0;															 //thumb orientation
                                                                    l_vArmJoints[10] = l_pFingersTarget->get(2).asDouble();
                                                                    l_vArmJoints[11] = 0;//l_pFingersTarget->get(3).asDouble();
                                                                    l_vArmJoints[12] = 0;//l_pFingersTarget->get(4).asDouble();
                                                                    l_vArmJoints[13] = 0;//l_pFingersTarget->get(5).asDouble();
                                                                    l_vArmJoints[14] = 0;//l_pFingersTarget->get(6).asDouble();
                                                                    l_vArmJoints[15] = 0;//l_pFingersTarget->get(7).asDouble() + l_pFingersTarget->get(8).asDouble() + l_pFingersTarget->get(9).asDouble() + l_pFingersTarget->get(10).asDouble();

            */

                                // USE THIS WITH THE iCub !!!!!
                                l_vArmJoints[0] = -25;
                                l_vArmJoints[1] = 20;
                                l_vArmJoints[2] = 0;
                                l_vArmJoints[3] = 50;
                                l_vArmJoints[4] = (l_pHandTarget->get(5).asDouble()*180 / 3.1415)*(-1) + 90;
                                l_vArmJoints[5] = (l_pHandTarget->get(4).asDouble()*180 / 3.1415)*(-1);
                                l_vArmJoints[6] = (l_pHandTarget->get(6).asDouble()*180 / 3.1415)*(-1);
                                l_vArmJoints[7] = 20;
                                l_vArmJoints[8] = 20;
                                l_vArmJoints[9] = 0;
                                l_vArmJoints[10] = 20;

                                l_vArmJoints[11] = l_pFingersTarget->get(3).asDouble();
                                l_vArmJoints[12] = l_pFingersTarget->get(4).asDouble();

                                l_vArmJoints[13] = l_pFingersTarget->get(5).asDouble();
                                l_vArmJoints[14] = l_pFingersTarget->get(6).asDouble();

                                l_vArmJoints[15] = l_pFingersTarget->get(7).asDouble() + l_pFingersTarget->get(8).asDouble() + l_pFingersTarget->get(9).asDouble() + l_pFingersTarget->get(10).asDouble();





                            /*


                            if(m_int32cptframe==4)
                            {
                                m_bLeftArmCapture = true;//Smooth
                                m_int32cptframe = 0;//Smooth
                                m_dsmoothx=0;//Smooth
                                m_dsmoothy=0;//Smooth
                                m_dsmoothz=0;//Smooth

                            }
                            else
                            {
                                m_bLeftArmCapture = false;
                            }
                            */

                            //	m_bLeftArmCapture = true


//                        }
//                        else
//                        {
//                            std::cerr << "Left hand cartesian controller not initialized, could not process leap data." << std::endl;
//                        }
                    }
                    break;
                }

                m_dArmTimeLastBottle = -1.;
                m_pVelocityController->enableArm(true);

            }
            else // manage timeout and reset position
            {
                if(m_dArmTimeLastBottle < 0.)
                {
                    m_dArmTimeLastBottle = yarp::os::Time::now();
                }
                else
                {
                    if(yarp::os::Time::now() - m_dArmTimeLastBottle > 0.001 * m_i32TimeoutArmReset)
                    {
                        m_pVelocityController->enableArm(false);
                        resetArmPosition();
                        m_dArmTimeLastBottle = -1.;
                    }
                }
            }


        }

    // check each joint value to ensure it is in the right range, if not crop to the max/min values
        for(uint ii = 0; ii < l_vArmJoints.size(); ++ii)
        {
            if(l_vArmJoints[ii] < m_vArmMinJoint[ii])
            {
                l_vArmJoints[ii] = m_vArmMinJoint[ii];
            }
            if(l_vArmJoints[ii] > m_vArmMaxJoint[ii])
            {
                l_vArmJoints[ii] = m_vArmMaxJoint[ii];
            }
        }

        if(l_pArmTarget)
        {
            m_pVelocityController->setJoints(l_vArmJoints);

            if(!m_pVelocityController->isRunning())
            {                
                m_pVelocityController->start();
            }
        }

    return true;
}

void swTeleop::SWIcubArm::resetArmPosition()
{
    if(m_bArmActivated)
    {
        for(int ii = 0; ii < m_i32ArmJointsNb; ++ii)
        {
            m_pIArmPosition->positionMove(ii,m_vArmResetPosition[ii]);
        }
    }
}

bool swTeleop::SWIcubArm::close()
{

    bool l_bArmPositionCloseState       = m_pIArmPosition->stop();
    bool l_bRobotArmCloseState          = m_oRobotArm.close();
    bool l_bRobotCartesianArmCloseState = m_oRobotArmCartesian.close();

    if(m_pVelocityController->isRunning())
    {
        m_pVelocityController->stop();
    }

    // close ports
        if(m_bArmActivated)
        {
            m_oArmTrackerPort.close();
            m_oHandTrackerPort.close();
            m_oFingersTrackerPort.close();
        }

    return (l_bArmPositionCloseState && l_bRobotArmCloseState && l_bRobotCartesianArmCloseState);
}

bool swTeleop::SWIcubArm::interruptModule()
{

    m_bIsRunning = false;

    // reset positions
        if(m_bArmActivated)
        {
            resetArmPosition();
        }

    if(m_pVelocityController->isRunning())
    {
        m_pVelocityController->stop();
    }

    // close ports
        if(m_bArmActivated)
        {
            m_oArmTrackerPort.interrupt();
            m_oHandTrackerPort.interrupt();
            m_oFingersTrackerPort.interrupt();
        }

    std::cout << "--Interrupting the iCub Arm Teleoperation module..." << std::endl;

    return true;
}


swTeleop::SWArmVelocityController::SWArmVelocityController(yarp::dev::IEncoders *pIArmEncoders, yarp::dev::IVelocityControl *pIArmVelocity,
                                                     std::vector<double> &vArmJointVelocityK, int i32Rate)
    : RateThread(i32Rate), m_bArmEnabled(false), m_vArmJointVelocityK(vArmJointVelocityK)
{

    if(pIArmEncoders)
    {
        m_pIArmEncoders = pIArmEncoders;
    }
    if(pIArmVelocity)
    {
        m_pIArmVelocity = pIArmVelocity;
    }
}

void swTeleop::SWArmVelocityController::run()
{
        m_oMutex.lock();
            yarp::sig::Vector l_vArmJoints = m_vLastArmJoint; // Check values with Joint before
        m_oMutex.unlock();

        yarp::sig::Vector l_vEncoders, l_vCommand;
        l_vEncoders.resize(l_vArmJoints.size());
        l_vCommand.resize(l_vArmJoints.size());

        m_pIArmEncoders->getEncoders(l_vEncoders.data());

//        std::cout << "arm joints : ";
        for(uint ii = 0; ii < l_vCommand.size(); ++ii)
        {
            l_vCommand[ii] =  m_vArmJointVelocityK[ii] * (l_vArmJoints[ii] - l_vEncoders[ii]);
//            std::cout << m_vArmJointVelocityK[ii] << " " << l_vArmJoints[ii] << " " << l_vEncoders[ii] << " " <<  l_vCommand[ii];

        }
//        std::cout << std::endl;

        l_vCommand[7]=0;//NEEDS TO BE REMOVE / ONLY TO BLOCK FINGER APPERTURE

        if(m_bArmEnabled)
        {
            for(uint ii = 0; ii < l_vArmJoints.size(); ++ii)
            {
                m_pIArmVelocity->velocityMove(ii, l_vCommand[ii]);
            }
        }
}

void swTeleop::SWArmVelocityController::enableArm(cbool bActivated)
{
    m_oMutex.lock();
        m_bArmEnabled = bActivated;
    m_oMutex.unlock();
}

void swTeleop::SWArmVelocityController::setJoints(const yarp::sig::Vector &vJoints)
{

    m_oMutex.lock();
        m_vLastArmJoint = vJoints;
    m_oMutex.unlock();
}



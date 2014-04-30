

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
            m_dVelocityToleranceArmDefault = 15.;
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

        if(bLeftArm)
            m_dVelocityToleranceArm= oRf.check(std::string("velocityToleranceLeftArm").c_str(), Value(m_dVelocityToleranceArmDefault), std::string(m_sArm + " left arm velocity tolerance (double)").c_str()).asDouble();
        else
            m_dVelocityToleranceArm= oRf.check(std::string("velocityToleranceRightArm").c_str(),Value(m_dVelocityToleranceArmDefault), std::string(m_sArm + " right arm velocity tolerance (double)").c_str()).asDouble();

    // set polydriver options
        m_oArmOptions.put("robot",     m_sRobotName.c_str());
        m_oArmOptions.put("device",    "remote_controlboard");
        m_oArmOptions.put("local",    ("/local/" + m_sRobotName + "/" + m_sArm + "_arm").c_str());
        m_oArmOptions.put("name",     ("/" + m_sRobotName + "/" + m_sArm + "_arm").c_str());
        m_oArmOptions.put("remote",   ("/" + m_sRobotName + "/" + m_sArm + "_arm").c_str());

    //set cartesian polydriver options
        m_oArmOptions.put("robot",     m_sRobotName.c_str());
        m_oArmCartesianOptions.put("device",    "cartesiancontrollerclient");
        m_oArmCartesianOptions.put("local",    ("/local/" + m_sRobotName + "/cartesianController/" + m_sArm + "_arm").c_str());
        m_oArmCartesianOptions.put("name",     ("/" + m_sRobotName + "/cartesianController/" + m_sArm + "_arm").c_str());
        m_oArmCartesianOptions.put("remote",   ("/" + m_sRobotName + "/cartesianController/" + m_sArm + "_arm").c_str());

    // init polydriver
        m_oRobotArm.open(m_oArmOptions);
        if(!m_oRobotArm.isValid())
        {
            std::cerr << "-ERROR: " << m_sArm << " robotArm is not valid, escape arm initialization. " << std::endl;
            return (m_bInitialized=false);
        }

    //init cartesian polydriver
        m_oRobotArmCartesian.open(m_oArmCartesianOptions);
        if (!m_oRobotArmCartesian.isValid())
        {
            std::cerr << "-ERROR: " << m_sArm << "right arm cartesian is not valid" << std::endl;
            return (m_bInitialized=false);
        }

    // initializing controllers
        if (!m_oRobotArm.view(m_pIArmVelocity) || !m_oRobotArm.view(m_pIArmPosition) || !m_oRobotArm.view(m_pIArmEncoders))
        {
            std::cerr <<  "-ERROR: " << m_sArm << " while getting required robot Arm interfaces." << std::endl;
            m_oRobotArm.close();
            return (m_bInitialized=false);
        }

    // init ports
        m_sArmTrackerPortName  = "/teleoperation/" + m_sRobotName + "/" + m_sArm + "_arm";

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
            std::cerr << "-ERROR: Unable to open ports." << std::endl;
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
        m_pVelocityController = new swTeleop::SWArmVelocityController(m_pIArmEncoders, m_pIArmVelocity, m_vArmJointVelocityK, m_dVelocityToleranceArm, 10);
        m_pVelocityController->enableArm(m_bArmActivated);

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

    // defines bottles
        Bottle *l_pArmTarget = NULL;

        // read arm commands
        if(m_bArmActivated)
        {
            l_pArmTarget = m_oArmTrackerPort.read(false);

            if (l_pArmTarget)
            {
                int l_deviceId = l_pArmTarget->get(0).asInt();

                switch(l_deviceId)
                {
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
                                                     std::vector<double> &vArmJointVelocityK, double dVelocityTolerance, int i32Rate)
    : RateThread(i32Rate), m_dVelocityToleranceArm(dVelocityTolerance), m_bArmEnabled(false), m_vArmJointVelocityK(vArmJointVelocityK)
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
            bool l_bArmEnabled = m_bArmEnabled;
            yarp::sig::Vector l_vArmJoints = m_vLastArmJoint;
        m_oMutex.unlock();

        yarp::sig::Vector l_vEncoders, l_vCommand;
        l_vEncoders.resize(l_vArmJoints.size());
        l_vCommand.resize(l_vArmJoints.size());

        m_pIArmEncoders->getEncoders(l_vEncoders.data());

        // ...

        // ...

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


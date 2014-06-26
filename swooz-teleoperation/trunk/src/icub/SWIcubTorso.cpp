

/**
 * \file SWIcubTorso.cpp
 * \author Florian Lance
 * \date 23-04-2014
 * \brief Defines SWIcubTorso
 */


#include <sstream>

#include "geometryUtility.h"
#include "SWTrackingDevice.h"

#include "icub/SWIcubTorso.h"

using namespace yarp::os;


swTeleop::SWIcubTorso::SWIcubTorso() : m_bInitialized(false), m_bIsRunning(false), m_dTorsoTimeLastBottle(-1.),
                                       m_pITorsoVelocity(NULL), m_pITorsoEncoders(NULL), m_pITorsoPosition(NULL), m_pVelocityController(NULL)
{
    // set ini file defaults values
        // parts to be activated
            m_bTorsoActivatedDefault = 1;

        // timeout
            m_i32TimeoutTorsoResetDefault = 3000;

        // accelerations / speeds
            double l_aDMinJointDefault[]                        = {-50.,-30.,-10.};
            double l_aDMaxJointDefault[]                        = { 50., 30., 70.};
            double l_aDTorsoJointVelocityDefault[]               = {50.,50.,50.};
            double l_aDTorsoJointVelocityKDefault[]              = {0.9,0.9,0.9};
            double l_aDTorsoJointPositionAccelerationDefault[]   = {50.,50.,50.};
            double l_aDTorsoJointPositionSpeedDefault[]          = {50.,50.,50.};
            double l_aDTorsoResetPositionDefault[]               = {0., 0. ,0.};
            m_vTorsoMinJointDefault                  = std::vector<double>(l_aDMinJointDefault, l_aDMinJointDefault + sizeof(l_aDMinJointDefault) / sizeof(double));
            m_vTorsoMaxJointDefault                  = std::vector<double>(l_aDMaxJointDefault, l_aDMaxJointDefault + sizeof(l_aDMaxJointDefault) / sizeof(double));
            m_vTorsoJointVelocityKDefault            = std::vector<double>(l_aDTorsoJointVelocityKDefault, l_aDTorsoJointVelocityKDefault + sizeof(l_aDTorsoJointVelocityKDefault) / sizeof(double));
            m_vTorsoJointVelocityAccelerationDefault = std::vector<double>(l_aDTorsoJointVelocityDefault, l_aDTorsoJointVelocityDefault + sizeof(l_aDTorsoJointVelocityDefault) / sizeof(double));
            m_vTorsoJointPositionAccelerationDefault = std::vector<double>(l_aDTorsoJointPositionAccelerationDefault, l_aDTorsoJointPositionAccelerationDefault + sizeof(l_aDTorsoJointPositionAccelerationDefault) / sizeof(double));
            m_vTorsoJointPositionSpeedDefault        = std::vector<double>(l_aDTorsoJointPositionSpeedDefault, l_aDTorsoJointPositionSpeedDefault + sizeof(l_aDTorsoJointPositionSpeedDefault) / sizeof(double));
            m_vTorsoResetPositionDefault             = std::vector<double>(l_aDTorsoResetPositionDefault, l_aDTorsoResetPositionDefault + sizeof(l_aDTorsoResetPositionDefault) / sizeof(double));

            m_vTorsoMinJoint                    = std::vector<double>(m_vTorsoMinJointDefault.size());
            m_vTorsoMaxJoint                    = std::vector<double>(m_vTorsoMaxJointDefault.size());
            m_vTorsoJointVelocityAcceleration   = std::vector<double>(m_vTorsoJointVelocityAccelerationDefault.size());
            m_vTorsoJointVelocityK              = std::vector<double>(m_vTorsoJointVelocityKDefault.size());
            m_vTorsoJointPositionAcceleration   = std::vector<double>(m_vTorsoJointPositionAccelerationDefault.size());
            m_vTorsoJointPositionSpeed          = std::vector<double>(m_vTorsoJointPositionSpeedDefault.size());
            m_vTorsoResetPosition               = std::vector<double>(m_vTorsoResetPositionDefault.size());
            m_i32TorsoJointsNb = m_vTorsoMinJoint.size();
}

swTeleop::SWIcubTorso::~SWIcubTorso()
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

bool swTeleop::SWIcubTorso::init( yarp::os::ResourceFinder &oRf)
{
    if(m_bInitialized)
    {
        std::cerr << "Icub Torso is already initialized. " << std::endl;
        return true;
    }

    // gets the module name which will form the stem of all module port names
        m_sModuleName   = oRf.check("name", Value("teleoperation_iCub"), "Teleoperation/iCub Module name (string)").asString();
        m_sRobotName    = oRf.check("robot",Value("icubSim"),  "Robot name (string)").asString();

    // robot parts to control
        m_bTorsoActivated = oRf.check("torsoActivated", Value(m_bTorsoActivatedDefault), "Torso activated (int)").asInt() != 0;
        if(!m_bTorsoActivated)
        {
            return (m_bInitialized=false);
        }

    // min / max values for iCub Torso joints
        for(uint ii = 0; ii < m_vTorsoJointVelocityAcceleration.size(); ++ii)
        {
            std::ostringstream l_os;
            l_os << ii;

            std::string l_sMinJoint("torsoMinValueJoint" + l_os.str());
            std::string l_sMaxJoint("torsoMaxValueJoint" + l_os.str());
            std::string l_sTorsoJointVelocityAcceleration("torsoJointVelocityAcceleration" + l_os.str());
            std::string l_sTorsoJointVelocityK("torsoJointVelocityK" + l_os.str());
            std::string l_sTorsoJointPositionAcceleration("torsoJointPositionAcceleration" + l_os.str());
            std::string l_sTorsoJointPositionSpeed("torsoJointPositionSpeed" + l_os.str());
            std::string l_sTorsoResetPosition("torsoResetPosition" + l_os.str());

            std::string l_sMinJointInfo("torso minimum joint" + l_os.str() + " Value (double)");
            std::string l_sMaxJointInfo("torso maximum joint" + l_os.str() + " Value (double)");
            std::string l_sTorsoJointVelocityAccelerationInfo("torso joint velocity acceleration " + l_os.str() + " Value (double)");
            std::string l_sTorsoJointVelocityKInfo("torso joint velocity K coeff"+ l_os.str() + " Value (double)");
            std::string l_sTorsoJointPositionAccelerationInfo("torso joint position acceleration " + l_os.str() + " Value (double)");
            std::string l_sTorsoJointPositionSpeedInfo("torso joint position speed " + l_os.str() + " Value (double)");
            std::string l_sTorsoResetPositionInfo("torso reset position " + l_os.str() + " Value (double)");

            m_vTorsoMinJoint[ii]                    = oRf.check(l_sMinJoint.c_str(), m_vTorsoMinJointDefault[ii], l_sMinJointInfo.c_str()).asDouble();
            m_vTorsoMaxJoint[ii]                    = oRf.check(l_sMaxJoint.c_str(), m_vTorsoMaxJointDefault[ii], l_sMaxJointInfo.c_str()).asDouble();
            m_vTorsoResetPosition[ii]               = oRf.check(l_sTorsoResetPosition.c_str(), m_vTorsoResetPositionDefault[ii], l_sTorsoResetPositionInfo.c_str()).asDouble();
            m_vTorsoJointVelocityAcceleration[ii]   = oRf.check(l_sTorsoJointVelocityAcceleration.c_str(), m_vTorsoJointVelocityAccelerationDefault[ii], l_sTorsoJointVelocityAccelerationInfo.c_str()).asDouble();
            m_vTorsoJointPositionAcceleration[ii]   = oRf.check(l_sTorsoJointPositionAcceleration.c_str(), m_vTorsoJointPositionAccelerationDefault[ii], l_sTorsoJointPositionAccelerationInfo.c_str()).asDouble();
            m_vTorsoJointPositionSpeed[ii]          = oRf.check(l_sTorsoJointPositionSpeed.c_str(),        m_vTorsoJointPositionSpeedDefault[ii],        l_sTorsoJointPositionSpeedInfo.c_str()).asDouble();
            m_vTorsoJointVelocityK[ii]              = oRf.check(l_sTorsoJointVelocityK.c_str(),            m_vTorsoJointVelocityKDefault[ii],            l_sTorsoJointVelocityKInfo.c_str()).asDouble();
        }

    // miscellaneous
        m_i32TimeoutTorsoReset   = oRf.check("torsoTimeoutReset",   Value(m_i32TimeoutTorsoResetDefault), "torso timeout reset iCub (int)").asInt();

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
            std::cerr << "-ERROR: robotTorso is not valid, escape torso initialization. " << std::endl;
            return (m_bInitialized=false);
        }

    // initializing controllers
        if (!m_oRobotTorso.view(m_pITorsoVelocity) || !m_oRobotTorso.view(m_pITorsoPosition) || !m_oRobotTorso.view(m_pITorsoEncoders))
        {
            std::cerr << "-ERROR: while getting required robot Torso interfaces." << std::endl;
            m_oRobotTorso.close();
            return (m_bInitialized=false);
        }

    // init ports
        m_sTorsoTrackerPortName  = "/teleoperation/" + m_sRobotName + "/torso";

    // open ports
        bool l_bPortOpeningSuccess = true;
        if(m_bTorsoActivated)
        {
            l_bPortOpeningSuccess = m_oTorsoTrackerPort.open(m_sTorsoTrackerPortName.c_str());
        }

        if(!l_bPortOpeningSuccess)
        {
            std::cerr << "-ERROR: Unable to open ports." << std::endl;
            m_oRobotTorso.close();
            return (m_bInitialized=false);
        }

    // retrieve Torso number of joints
        m_pITorsoPosition->getAxes(&m_i32TorsoJointsNb);

    // set accelerations/speeds
        for(int ii = 0; ii < m_i32TorsoJointsNb; ++ii)
        {
            m_pITorsoPosition->setRefAcceleration(ii, m_vTorsoJointPositionAcceleration[ii]);
            m_pITorsoPosition->setRefSpeed(ii, m_vTorsoJointPositionSpeed[ii]);
            m_pITorsoVelocity->setRefAcceleration(ii, m_vTorsoJointVelocityAcceleration[ii]);
        }

    // init controller                
        m_pVelocityController = new swTeleop::SWTorsoVelocityController(m_pITorsoEncoders, m_pITorsoVelocity, m_vTorsoJointVelocityK, 10);
        m_pVelocityController->enableTorso(m_bTorsoActivated);

    // display parameters
        std::cout << std::endl << std::endl;
        displayDebug(std::string("Torso activated"), m_bTorsoActivated);
        displayDebug(std::string("Timeout torso reset"), m_i32TimeoutTorsoReset);
        std::cout << std::endl;
        displayVectorDebug(std::string("Torso min joint                  : "), m_vTorsoMinJoint);
        displayVectorDebug(std::string("Torso max joint                  : "), m_vTorsoMaxJoint);
        displayVectorDebug(std::string("Torso reset position joint       : "), m_vTorsoResetPosition);
        displayVectorDebug(std::string("Torso joint velocity acceleration: "), m_vTorsoJointVelocityAcceleration);
        displayVectorDebug(std::string("Torso joint position acceleration: "), m_vTorsoJointPositionAcceleration);
        displayVectorDebug(std::string("Torso joint position speed       : "), m_vTorsoJointPositionSpeed);
        displayVectorDebug(std::string("Torso head joint velocity K      : "), m_vTorsoJointVelocityK);
        std::cout << std::endl << std::endl;


    return (m_bIsRunning=m_bInitialized=true);
}

bool swTeleop::SWIcubTorso::checkBottles()
{
    if(!m_bIsRunning)
    {
        return false;
    }

    if(!m_bInitialized)
    {
        std::cerr << "Icub Torso control module not initialized. " << std::endl;
        return (m_bIsRunning=false);
    }

    // init command vector
        yarp::sig::Vector l_vTorsoJoints;
        l_vTorsoJoints.resize(m_i32TorsoJointsNb);
        l_vTorsoJoints = 0.;

    // defines bottles
        Bottle *l_pTorsoTarget = NULL;

        // read torso commands
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

                        std::vector<double> l_vecTorso      = swUtil::vec(l_pointTorso, l_pointNeck);
                        std::vector<double> l_vecClavicles  = swUtil::vec(l_pointLShoulder, l_pointRShoulder);
                        std::vector<double> l_rpyTorso      = swUtil::computeRollPitchYaw(l_vecTorso, l_vecClavicles);

                        l_vTorsoJoints[0] = l_rpyTorso[2];
                        l_vTorsoJoints[1] = l_rpyTorso[0];
                        l_vTorsoJoints[2] = l_rpyTorso[1];

                    }
                    break;
                }

                m_dTorsoTimeLastBottle = -1.;
                m_pVelocityController->enableTorso(true);
            }
            else // manage timeout and reset position
            {
                if(m_dTorsoTimeLastBottle < 0.)
                {
                    m_dTorsoTimeLastBottle = yarp::os::Time::now();
                }
                else
                {
                    if(yarp::os::Time::now() - m_dTorsoTimeLastBottle > 0.001 * m_i32TimeoutTorsoReset)
                    {
                        m_pVelocityController->enableTorso(false);
                        resetTorsoPosition();
                        m_dTorsoTimeLastBottle = -1.;
                    }
                }
            }
        }

    // check each joint value to ensure it is in the right range, if not crop to the max/min values
        for(uint ii = 0; ii < l_vTorsoJoints.size(); ++ii)
        {
            if(l_vTorsoJoints[ii] < m_vTorsoMinJoint[ii])
            {
                l_vTorsoJoints[ii] = m_vTorsoMinJoint[ii];
            }
            if(l_vTorsoJoints[ii] > m_vTorsoMaxJoint[ii])
            {
                l_vTorsoJoints[ii] = m_vTorsoMaxJoint[ii];
            }
        }

        if(l_pTorsoTarget)
        {
            m_pVelocityController->setJoints(l_vTorsoJoints);

            if(!m_pVelocityController->isRunning())
            {
                m_pVelocityController->start();
            }
        }

    return true;
}

void swTeleop::SWIcubTorso::resetTorsoPosition()
{
    if(m_bTorsoActivated)
    {
        for(int ii = 0; ii < m_i32TorsoJointsNb; ++ii)
        {
            m_pITorsoPosition->positionMove(ii, m_vTorsoResetPosition[ii]);
        }
    }
}

bool swTeleop::SWIcubTorso::close()
{

    bool l_bTorsoPositionCloseState = m_pITorsoPosition->stop();
    bool l_bRobotTorsoCloseState    = m_oRobotTorso.close();

    if(m_pVelocityController->isRunning())
    {
        m_pVelocityController->stop();
    }

    // close ports
        if(m_bTorsoActivated)
        {
            m_oTorsoTrackerPort.close();
        }

    return (l_bTorsoPositionCloseState && l_bRobotTorsoCloseState);
}

bool swTeleop::SWIcubTorso::interruptModule()
{
    m_bIsRunning = false;

    // reset positions
        if(m_bTorsoActivated)
        {
            resetTorsoPosition();
        }

    if(m_pVelocityController->isRunning())
    {
        m_pVelocityController->stop();
    }

    // close ports
        if(m_bTorsoActivated)
        {
            m_oTorsoTrackerPort.interrupt();
        }

    std::cout << "--Interrupting the iCub Torso Teleoperation module..." << std::endl;

    return true;
}


swTeleop::SWTorsoVelocityController::SWTorsoVelocityController(yarp::dev::IEncoders *pITorsoEncoders, yarp::dev::IVelocityControl *pITorsoVelocity,
                                                     std::vector<double> &vTorsoJointVelocityK, int i32Rate)
    : RateThread(i32Rate), m_bTorsoEnabled(false), m_vTorsoJointVelocityK(vTorsoJointVelocityK)
{
    if(pITorsoEncoders)
    {
        m_pITorsoEncoders = pITorsoEncoders;
    }
    if(pITorsoVelocity)
    {
        m_pITorsoVelocity = pITorsoVelocity;
    }
}

void swTeleop::SWTorsoVelocityController::run()
{
        m_oMutex.lock();
            bool l_bTorsoEnabled = m_bTorsoEnabled;
            yarp::sig::Vector l_vTorsoJoints = m_vLastTorsoJoint;
        m_oMutex.unlock();

        yarp::sig::Vector l_vEncoders, l_vCommand;
        l_vEncoders.resize(l_vTorsoJoints.size());
        l_vCommand.resize(l_vTorsoJoints.size());

        m_pITorsoEncoders->getEncoders(l_vEncoders.data());

        // Torso
            for(uint ii = 0; ii < l_vCommand.size(); ++ii)
            {
                l_vCommand[ii] = m_vTorsoJointVelocityK[ii] * (l_vTorsoJoints[ii] - l_vEncoders[ii]);
            }

        // velocity move
        //  Torso
            if(l_bTorsoEnabled)
            {
                for(uint ii = 0; ii < l_vCommand.size(); ++ii)
                {
                    m_pITorsoVelocity->velocityMove(ii, l_vCommand[ii]);
                }
            }
}

void swTeleop::SWTorsoVelocityController::enableTorso(cbool bActivated)
{
    m_oMutex.lock();
        m_bTorsoEnabled = bActivated;
    m_oMutex.unlock();
}

void swTeleop::SWTorsoVelocityController::setJoints(const yarp::sig::Vector &vJoints)
{
    m_oMutex.lock();
        m_vLastTorsoJoint = vJoints;
    m_oMutex.unlock();
}

/**
 * \file SWTeleoperation_iCub.cpp
 * \author Florian Lance, Guillaume Gibert, Emmanuel Body
 * \date 05-12-2013
 * \brief ...
 */

#include "icub/SWTeleoperation_iCub.h"


YARP_DECLARE_DEVICES(icubmod)

SWTeleoperation_iCub::SWTeleoperation_iCub() : m_bIsRunning(false)
{}

SWTeleoperation_iCub::~SWTeleoperation_iCub()
{}

bool SWTeleoperation_iCub::configure(ResourceFinder &rf)
{
    // gets the module name which will form the stem of all module port names
        m_sModuleName   = rf.check("name", Value("teleoperation_iCub"), "Teleoperation/iCub Module name (string)").asString();
        m_sRobotName    = rf.check("robot", Value("icubSim"),  "Robot name (string)").asString();
        setName(m_sModuleName.c_str());

    // miscellaneous
        m_i32Fps        = rf.check("fps", Value(100),  "Frame per second (int)").asInt();

    // init sub control mobules
        m_bHeadInitialized      = m_oIcubHeadControl.init(rf);
        m_bTorsoInitialized     = m_oIcubTorsoControl.init(rf);
        m_bLeftArmInitialized   = m_oIcubLeftArmControl.init(rf, true);
        m_bRightArmInitialized  = m_oIcubRightArmControl.init(rf, false);

        if(m_bHeadInitialized)
        {
            std::cout << "  -- iCub head initialized. " << std::endl;
        }
        if(m_bTorsoInitialized)
        {
            std::cout << "  -- iCub torso initialized. " << std::endl;
        }
        if(m_bLeftArmInitialized)
        {
            std::cout << "  -- iCub left arm initialized. " << std::endl;
        }
        if(m_bRightArmInitialized)
        {
            std::cout << "  -- iCub right arm initialized. " << std::endl;
        }

        if(!m_bHeadInitialized && !m_bTorsoInitialized && !m_bLeftArmInitialized && !m_bRightArmInitialized)
        {
            std::cerr << "No iCub parts initialized, end of the teleoperation. " << std::endl;
            return false;
        }

    return (m_bIsRunning=true);
}

bool SWTeleoperation_iCub::interruptModule()
{
    m_bIsRunning = false;

    if(m_bHeadInitialized)
        m_oIcubHeadControl.interruptModule();

    if(m_bTorsoInitialized)
        m_oIcubTorsoControl.interruptModule(); 

    if(m_bLeftArmInitialized)
        m_oIcubLeftArmControl.interruptModule();

    if(m_bRightArmInitialized)
        m_oIcubRightArmControl.interruptModule();

    std::cout << "--Interrupting the iCub Teleoperation module..." << std::endl;

    return true;
}


bool SWTeleoperation_iCub::close()
{
    if(m_bHeadInitialized)
        m_oIcubHeadControl.close();

    if(m_bTorsoInitialized)
        m_oIcubTorsoControl.close();

    if(m_bLeftArmInitialized)
        m_oIcubLeftArmControl.close();

    if(m_bRightArmInitialized)
        m_oIcubRightArmControl.close();

    std::cout << "Close iCub teleoperation module. " << std::endl;

    return true;
}

bool SWTeleoperation_iCub::updateModule()
{
    if(!m_bIsRunning)
    {
        return false;
    }

    if(m_bHeadInitialized)
    {
        if(!m_oIcubHeadControl.checkBottles())
        {
            return false;
        }
    }

    if(m_bTorsoInitialized)
    {
        if(!m_oIcubTorsoControl.checkBottles())
        {
            return false;
        }
    }

    if(m_bLeftArmInitialized)
    {
        if(!m_oIcubLeftArmControl.checkBottles())
        {
            return false;
        }
    }

    if(m_bRightArmInitialized)
    {
        if(!m_oIcubRightArmControl.checkBottles())
        {
            return false;
        }
    }

    return true;
}

double SWTeleoperation_iCub::getPeriod()
{
    return 1./m_i32Fps;
}


int main(int argc, char* argv[])
{
    YARP_REGISTER_DEVICES(icubmod);

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

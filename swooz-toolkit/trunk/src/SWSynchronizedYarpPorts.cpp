/**
 * \file SWSynchronizedYarpPorts.cpp
 * \author Guillaume Gibert
 * \date 01/10/2015
 * \brief ...
 */

#include "SWSynchronizedYarpPorts.h"


#include <cmath>
#include <iostream>

#include "geometryUtility.h"



SWSynchronizedYarpPorts::SWSynchronizedYarpPorts() :  m_bIsRunning(false), m_bInitialized(false)
{    
	m_bHeadActivatedDefault     		= true;
	m_bLeftArmActivatedDefault     	= true;
	m_bRightArmActivatedDefault     	= true;   
	m_sTriggerPortNameDefault		="/syncdata/icubSim/trigger";
	
	
	if (m_bHeadActivated)
	{	
		m_pIHeadPosition = 0;
		m_pIHeadEncoders = 0;
		m_pIHeadVelocity = 0;
	}
	
	if (m_bLeftArmActivated)
	{
		m_pILeftArmEncoders = 0;             
		m_pILeftArmPosition = 0;    
		m_pILeftArmVelocity = 0;
	}
	
	if (m_bRightArmActivated)
	{
		m_pIRightArmEncoders = 0;             
		m_pIRightArmPosition = 0;   
		m_pIRightArmVelocity = 0;		
	}
	
}

SWSynchronizedYarpPorts::~SWSynchronizedYarpPorts()
{
	
}

bool SWSynchronizedYarpPorts::configure(ResourceFinder &oRf)
{
	// gets the module name which will form the stem of all module port names
        m_sModuleName   = oRf.check("name", Value("synchronizedYarpPorts"), "Toolkit/Synchronized Yarp Ports Module name (string)").asString();
	m_sRobotName    = oRf.check("robot",Value("icubSim"),  "Robot name (string)").asString();
	
	// robot parts to control
        m_bHeadActivated    	= oRf.check("headActivated",yarp::os::Value(m_bHeadActivatedDefault), "Head activated (int)").asInt() != 0;
	m_bLeftArmActivated   = oRf.check("leftArmActivated",yarp::os::Value(m_bLeftArmActivatedDefault), "LeftArm activated (int)").asInt() != 0;
	m_bRightArmActivated  = oRf.check("rightArmActivated",yarp::os::Value(m_bRightArmActivatedDefault), "RightArm activated (int)").asInt() != 0;
	m_sTriggerPortName 	=  oRf.check("triggerPortName",yarp::os::Value(m_sTriggerPortNameDefault), "RightArm activated (int)").asInt() != 0;
	// miscellaneous
        m_i32Fps                = oRf.check("fps",              yarp::os::Value(10),  "Frame per second (int)").asInt();
       
	
	// init trigger port
	m_sTriggerPortName  = "/sync/" + m_sRobotName + "/trigger";
	if (!m_oTriggerPort.open(m_sTriggerPortName.c_str()))
        {
            std::cerr << "-ERROR: Unable to open trigger port." << std::endl;
            interruptModule();
            return false;
        }
	
	
	// init sync data port
	m_sSynchronizedDataPortName = "/sync/" + m_sRobotName + "/syncdata";
	if (!m_oSynchronizedDataPort.open(m_sSynchronizedDataPortName.c_str()))
        {
            std::cerr << "-ERROR: Unable to open sync data port." << std::endl;
            interruptModule();
            return false;
        }
     
	// HEAD init
	if (m_bHeadActivated)
	{
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
		    std::cerr << std::endl << "-ERROR: robotHead is not valid, escape head initialization. " << std::endl<< std::endl;
		    return (m_bInitialized=false);
		}
		// initializing controllers
		if (!m_oRobotHead.view(m_pIHeadPosition) ||!m_oRobotHead.view(m_pIHeadEncoders) || !m_oRobotHead.view(m_pIHeadVelocity))
		{
		    std::cerr << std::endl << "-ERROR: while getting required robot head interfaces." << std::endl<< std::endl;
		    m_oRobotHead.close();
		    return (m_bInitialized=false);
		}
		// retrieve head number of joints
		m_pIHeadPosition->getAxes(&m_i32HeadJointsNb);
	}
	
	// LEFT ARM init
	if (m_bLeftArmActivated)
	{
		// set polydriver options
		m_oLeftArmOptions.put("robot",     m_sRobotName.c_str());
		m_oLeftArmOptions.put("device",    "remote_controlboard");
		m_oLeftArmOptions.put("local",    ("/local/" + m_sRobotName + "/" + "left_arm").c_str());
		m_oLeftArmOptions.put("name",     ("/" + m_sRobotName + "/" + "left_arm").c_str());
		m_oLeftArmOptions.put("remote",   ("/" + m_sRobotName + "/" + "left_arm").c_str());
		// init polydriver
		m_oRobotLeftArm.open(m_oLeftArmOptions);
		if(!m_oRobotLeftArm.isValid())
		{
		    std::cerr << std::endl <<"-ERROR: Left robotArm is not valid, escape arm initialization. " << std::endl <<std::endl;
		    return (m_bInitialized=false);
		}
		// initializing controllers
		if (!m_oRobotLeftArm.view(m_pILeftArmPosition) ||!m_oRobotLeftArm.view(m_pILeftArmEncoders) || !m_oRobotLeftArm.view(m_pILeftArmVelocity))
		{
		    std::cerr << std::endl <<  "-ERROR: while getting required robot Left Arm interfaces." << std::endl <<std::endl;
		    m_oRobotLeftArm.close();
		    return (m_bInitialized=false);
		}
		// retrieve Torso number of joints
		m_pILeftArmPosition->getAxes(&m_i32LeftArmJointsNb);
	}

	// RIGHT ARM init
	if (m_bRightArmActivated)
	{
		// set polydriver options
		m_oRightArmOptions.put("robot",     m_sRobotName.c_str());
		m_oRightArmOptions.put("device",    "remote_controlboard");
		m_oRightArmOptions.put("local",    ("/local/" + m_sRobotName + "/" + "right_arm").c_str());
		m_oRightArmOptions.put("name",     ("/" + m_sRobotName + "/" + "right_arm").c_str());
		m_oRightArmOptions.put("remote",   ("/" + m_sRobotName + "/" + "right_arm").c_str());
		// init polydriver
		m_oRobotRightArm.open(m_oRightArmOptions);
		if(!m_oRobotRightArm.isValid())
		{
		    std::cerr << std::endl <<"-ERROR: Right robotArm is not valid, escape arm initialization. " << std::endl <<std::endl;
		    return (m_bInitialized=false);
		}
		// initializing controllers
		if (!m_oRobotRightArm.view(m_pIRightArmPosition) ||!m_oRobotRightArm.view(m_pIRightArmEncoders) || !m_oRobotRightArm.view(m_pIRightArmVelocity))
		{
		    std::cerr << std::endl <<  "-ERROR: while getting required robot Right Arm interfaces." << std::endl <<std::endl;
		    m_oRobotRightArm.close();
		    return (m_bInitialized=false);
		}
		// retrieve Torso number of joints
		m_pIRightArmPosition->getAxes(&m_i32RightArmJointsNb);
	}
	
	return (m_bIsRunning=m_bInitialized=true);
}


bool SWSynchronizedYarpPorts::interruptModule()
{
	m_oTriggerPort.interrupt();
	m_oSynchronizedDataPort.interrupt();
	
	std::cout << "--Interrupting the synchronized yarp ports module..." << std::endl;
	return true;
}



bool SWSynchronizedYarpPorts::close()
{
	if (m_bHeadActivated)
	{
		m_oRobotHead.close();
		m_pIHeadPosition = NULL;
		m_pIHeadEncoders = NULL;
		m_pIHeadVelocity = NULL;
	}
	
	if (m_bLeftArmActivated)
	{
		m_oRobotLeftArm.close();
		m_pILeftArmEncoders = NULL;             
		m_pILeftArmPosition = NULL;  
		m_pILeftArmVelocity = NULL;	
	}
	
	if (m_bRightArmActivated)
	{
		m_oRobotRightArm.close();
		m_pIRightArmEncoders = NULL;             
		m_pIRightArmPosition = NULL;  
		m_pIRightArmVelocity = NULL;
	}
	
	// closes yarp ports
	m_oTriggerPort.close();
	m_oSynchronizedDataPort.close();
	
	std::cout << "--Closing the synchronized yarp ports module..." << std::endl;
	return true;
}


bool SWSynchronizedYarpPorts::updateModule()
{
	yarp::sig::Vector l_vHeadEncoders;
	yarp::sig::Vector l_vLeftArmEncoders;
	yarp::sig::Vector l_vRightArmEncoders;
	
	// sends sync data to yarp port
	yarp::os::Bottle & l_syncDataBottle       = m_oSynchronizedDataPort.prepare();
	l_syncDataBottle.clear();
	
	// Retrieves encoder data
	/////// HEAD
	if (m_bHeadActivated)
	{
		l_vHeadEncoders.resize(m_i32HeadJointsNb);
		m_pIHeadEncoders->getEncoders(l_vHeadEncoders.data());
		for (int l_data=0; l_data<m_i32HeadJointsNb; l_data++)
		{
			l_syncDataBottle.addDouble(l_vHeadEncoders[l_data]);
		}
	}
	
	//LEFT ARM
	if (m_bLeftArmActivated)
	{
		l_vLeftArmEncoders.resize(m_i32LeftArmJointsNb);
		m_pILeftArmEncoders->getEncoders(l_vLeftArmEncoders.data());
		for (int l_data=0; l_data<m_i32LeftArmJointsNb; l_data++)
		{
			l_syncDataBottle.addDouble(l_vLeftArmEncoders[l_data]);
		}
	}
	
	//RIGHT ARM
	if (m_bRightArmActivated)
	{
		l_vRightArmEncoders.resize(m_i32RightArmJointsNb);
		m_pIRightArmEncoders->getEncoders(l_vRightArmEncoders.data());
		for (int l_data=0; l_data<m_i32RightArmJointsNb; l_data++)
		{
			l_syncDataBottle.addDouble(l_vRightArmEncoders[l_data]);
		}
	}

	//////TRIGGER
	// defines bottles
	Bottle *l_pTrigger = NULL;
	// reads the trigger port
	l_pTrigger = m_oTriggerPort.read(false);
	//l_pTrigger->get(1).asDouble();
	
	
	// sends sync data to yarp port
	m_oSynchronizedDataPort.write();

	return true;
}





double SWSynchronizedYarpPorts::getPeriod()
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
	SWSynchronizedYarpPorts l_oSynchronizedYarpPorts;

	// prepare and configure the resource finder
	yarp::os::ResourceFinder l_oRf;
	l_oRf.setVerbose(true);
	l_oRf.setDefaultConfigFile("synchronized_yarp_ports.ini");
	l_oRf.setDefaultContext("swooz-toolkit/conf");
	l_oRf.configure("SYNCHRONIZEDYARPPORTS_ROOT", argc, argv);

	// configure the module
	if (l_oSynchronizedYarpPorts.configure(l_oRf))
	{
		// run the module
		l_oSynchronizedYarpPorts.runModule();
	}
	else
	{
		std::cerr << "Failed to configure the synchronized yarp ports module!"<< std::endl;
	}

	return 0;
}

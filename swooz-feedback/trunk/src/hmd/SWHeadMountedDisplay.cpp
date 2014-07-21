
/**
 * \file SWHeadMountedDisplay.cpp
 * \brief Defines SWHeadMountedDisplay class
 * \author Guillaume Gibert
 * \date 17/07/14
 */

#include <iostream>
#include <yarp/os/Network.h>


#include "hmd/SWSonyHMZT3W.h"
#include <yarp/os/RFModule.h>

using namespace yarp::os;

class SWHeadMountedDisplay: public RFModule
{
	SWSonyHMZT3W hmd;
	int m_i32Fps;

	public:

	bool configure(ResourceFinder &rf)
	{
		// gets the module name which will form the stem of all module port names
		std::string l_sModuleName   = rf.check("name", Value("feedback_hmd_iCub"), "Feedback/HMD-iCub Module name (string)").asString();
		setName(l_sModuleName.c_str());
		
		int displayImgWidth   	= rf.check("displayImgWidth",     	 Value(1280),  "Width of the display (int)").asInt();
		int displayImgHeight	= rf.check("displayImgHeight", 	 Value(720),  "Height of the display (int)").asInt();
		m_i32Fps            	= rf.check("fps",                  	 Value(50),  "Frame per second (int)").asInt();
		
		return hmd.open(displayImgWidth, displayImgHeight);
	}

	double getPeriod()
	{
		return 1.0f/m_i32Fps;
	}

	bool updateModule()
	{ 
		hmd.loop();
		return true; 
	}

	bool interruptModule()
	{
		fprintf(stderr, "Interrupting\n");
		hmd.interrupt();
		return true;
	}

	bool close()
	{
		fprintf(stderr, "Calling close\n");
		hmd.close();
		return true;
	}
};

int main(int argc, char *argv[]) 
{
	Network yarp;

	SWHeadMountedDisplay module;
	ResourceFinder rf;
	rf.setVerbose(true);
	rf.setDefaultConfigFile("feedback_hmd_iCub.ini"); //overridden by --from parameter
	rf.setDefaultContext("swooz-feedback/conf");   //overridden by --context parameter
	rf.configure("ICUB_ROOT", argc, argv);

	
	if (!module.configure(rf))
	{
		fprintf(stderr, "> Error configuring module SWHeadMountedDisplay returning\n");
		return -1;
	}


	module.runModule();

	printf("> Module SWHeadMountedDisplay shutting down\n");

	return 0;
}

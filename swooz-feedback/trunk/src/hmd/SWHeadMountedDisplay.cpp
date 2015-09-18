
/**
 * \file SWHeadMountedDisplay.cpp
 * \brief Defines SWHeadMountedDisplay class
 * \author Guillaume Gibert
 * \date 17/07/14
 */

#include <iostream>
#include <yarp/os/Network.h>


#include "hmd/SWSonyHMZT3W.h"
#include "hmd/SWOculusRiftDK2.h"
#include <yarp/os/RFModule.h>

using namespace yarp::os;

class SWHeadMountedDisplay: public RFModule
{
	private :
		bool m_startLoop;
		int m_i32Fps;
		int hmdIndex;
		SWSonyHMZT3W m_hmdSony;
		SWOculusRiftDK2 m_hmdOculus;

	public:
		SWHeadMountedDisplay(){m_startLoop = false;}

        bool configure(ResourceFinder &rf)
        {
		// gets the module name which will form the stem of all module port names
		std::string l_sModuleName   = rf.check("name", Value("feedback_hmd_iCub"), "Feedback/HMD-iCub Module name (string)").asString();
		setName(l_sModuleName.c_str());

		int displayImgWidth   	= rf.check("displayImgWidth",     	 Value(1280),  "Width of the display (int)").asInt();
		int displayImgHeight	= rf.check("displayImgHeight", 	 Value(720),  "Height of the display (int)").asInt();
		m_i32Fps            	= rf.check("fps",                  	 Value(50),  "Frame per second (int)").asInt();
		hmdIndex	= rf.check("hmdIndex", 	 Value(0),  "HMD Index (int)").asInt();

		if (hmdIndex==0)
		{
			m_startLoop = m_hmdSony.open(displayImgWidth, displayImgHeight);
		}
		else if (hmdIndex==1)
		{
			m_startLoop = m_hmdOculus.open(displayImgWidth, displayImgHeight);
		}
		else
		{
			fprintf(stderr, "Not a correct HMD index!\n");
			return false;
		}
		
		
		return m_startLoop;
        }

        double getPeriod()
        {
            return 1.0/m_i32Fps;
        }

        bool updateModule()
        {
		if(m_startLoop)
		{
			if (hmdIndex==0)
			{
				return m_hmdSony.loop();
			}
			else if (hmdIndex==1)
			{
				return m_hmdOculus.loop();
			}
		}
		else
		{
			return false;
		}
        }

        bool interruptModule()
        {
		fprintf(stderr, "Interrupting\n");
		if (hmdIndex==0)
		{
			m_hmdSony.interrupt();
		}
		else if (hmdIndex==1)
		{
			m_hmdOculus.interrupt();
		}
	
		return true;
        }

        bool close()
        {
		fprintf(stderr, "Calling close\n");
		
		if (hmdIndex==0)
		{
			m_hmdSony.close();
		}
		else if (hmdIndex==1)
		{
			m_hmdOculus.close();
		}
		
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


	printf("##################### \n\n");
	printf("Press 'f' to enable/disable fullscreen. \n");
	printf("Press 'e' to change eyes display mode (both eyes -> left eye -> right eye -> both eyes). \n");
	printf("Press 'q' to leave. \n");
	printf("\n#####################");


	module.runModule();

	printf("> Module SWHeadMountedDisplay shutting down\n");

	return 0;
}

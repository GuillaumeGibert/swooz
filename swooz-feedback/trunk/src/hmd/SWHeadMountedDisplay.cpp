
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
    private :

        bool m_startLoop;
        int m_i32Fps;

        SWSonyHMZT3W m_hmd;


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

            m_startLoop = m_hmd.open(displayImgWidth, displayImgHeight);
            return m_startLoop;
        }

        double getPeriod()
        {
            return 1.0f/m_i32Fps;
        }

        bool updateModule()
        {
            if(m_startLoop)
            {
                return m_hmd.loop();
            }
            else
            {
                return false;
            }
        }

        bool interruptModule()
        {
            fprintf(stderr, "Interrupting\n");
            m_hmd.interrupt();
            return true;
        }

        bool close()
        {
            fprintf(stderr, "Calling close\n");
            m_hmd.close();
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

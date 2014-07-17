/**
 * \file SWFastrackTracking.h
 * \author Emmanuel Body
 * \date 29-11-2013
 * \brief The file containing the Fastrak Tracker module template
 *
 * This file contains the template of a Tracker module, connected
 * to a Fastrack sensor and sending arm/hand data to a yarp port
 *
 */

#include <yarp/os/Time.h>
#include "fastrak/SWFastrakTracking.h"
#include "SWTrackingDevice.h"

#define PI 3.14159265


using namespace swTracking;


bool SWFastrakTracking::configure(yarp::os::ResourceFinder & rf)
{
    int l_timeOut = 0;

	while (!m_fastrakDevice.init())
	{
		std::cout << "Waiting for fastrak" << std::endl;
        yarp::os::Time::delay(1);

        if(l_timeOut++ > 10)
        {
            std::cerr << "Timeout... " << std::endl;
            return false;
        }
	}

	if (m_fastrakDevice.init())
	{
	   std::cout << "--> Fastrack initialized" << std::endl;
	}
	else
	{
	   std::cerr << "<!!> Failed to initialize Fastrak" << std::endl;
	   exit(0);
	}

    m_trackerPortName = "/tracking/polhemus/fastrak/head";
	m_trackerPort.open(m_trackerPortName.c_str());

	m_fastrakDevice.startListening();
	std::cout << "--> Fastrak capture has started" << std::endl;
	return true;
}


bool SWFastrakTracking::updateModule()
{
    std::vector<float> l_dataTranslations   = m_fastrakDevice.translations();
    std::vector<float> l_dataRotations      = m_fastrakDevice.rotations();
    yarp::os::Bottle & l_targetBottle       = m_trackerPort.prepare();

	l_targetBottle.clear();
	l_targetBottle.addInt(FASTRAK_LIB);

    for (int i=0; i<3; i++)
    {
        l_targetBottle.addDouble((double) l_dataRotations[i]);
    }

	for (int i=0; i<3; i++)
	{
		l_targetBottle.addDouble((double) l_dataTranslations[i]);
	}

	m_trackerPort.write();

	return true;
}

double SWFastrakTracking::getPeriod()
{
	return 0.05;
}

bool SWFastrakTracking::interruptModule()
{
	m_trackerPort.interrupt();
	std::cout << "--> Interrupting the Fastrak Tracking module..." << std::endl;
	return true;
}

bool SWFastrakTracking::close()
{
	m_fastrakDevice.stopListening();
	std::cout << "--> Fastrak capture ended" << std::endl;

	std::cout << "Closing Yarp..." << std::endl;
	m_trackerPort.close();

	std::cout << "Closing the Fastrak tracking module..." << std::endl;

	return true;
}

int main(int argc, char* argv[])
{
	/* initialize yarp network */
    yarp::os::Network yarp;
	if (!yarp.checkNetwork())
	{
		std::cout << "-ERROR: Problem connecting to YARP server" << std::endl;
		return -1;
	}

	/* create your module */
    SWFastrakTracking l_handTracker;

	/* prepare and configure the resource finder */
    yarp::os::ResourceFinder rf;
		rf.setVerbose(true);
		rf.setDefaultConfigFile("teleoperation.ini"); //overridden by --from parameter
		rf.setDefaultContext("handTeleoperation/conf");   //overridden by --context parameter
		rf.configure("ICUB_ROOT", argc, argv);


	/* configure the module */
    std::cout << "Configuring the Fastrak tracking module..."<< std::endl;
	if (l_handTracker.configure(rf))
	{
		/* run the module */
        std::cout << "Starting the Fastrak tracking module..." << std::endl;
		l_handTracker.runModule();
	}
	else
	{
        std::cout << "Failed to configure the Fastrak tracking module!"<< std::endl;
	}
	return 0;
}

/*******************************************************************************
**                                                                            **
**  SWoOz is a software platform written in C++ used for behavioral           **
**  experiments based on interactions between people and robots               **
**  or 3D avatars.                                                            **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
** *****************************************************************************
**          Authors: Guillaume Gibert, Florian Lance                          **
**  Website/Contact: http://swooz.free.fr/                                    **
**       Repository: https://github.com/GuillaumeGibert/swooz                 **
********************************************************************************/

/**
 * \file SWOculusTracking.h
 * \author Guillaume Gibert
 * \date 29-05-2015
 * \brief The file containing the Oculus Tracker module template
 *
 * This file contains the template of a Tracker module, connected
 * to a Oculus sensor and sending arm/hand data to a yarp port
 *
 */

#include <yarp/os/Time.h>
#include "oculus/SWOculusTracking.h"
#include "SWTrackingDevice.h"


using namespace swTracking;
using namespace yarp::os;

bool SWOculusTracking::configure(yarp::os::ResourceFinder & oRf)
{
	int l_timeOut = 0;
	m_dFpsDefault = 50;
	
	while (!m_oculusDevice.init())
	{
		std::cout << "Waiting for oculus" << std::endl;
		yarp::os::Time::delay(1);

		if(l_timeOut++ > 10)
		{
		    std::cerr << "Timeout... " << std::endl;
		    return false;
		}
	}

	if (m_oculusDevice.init())
	{
		std::cout << "--> Oculus initialized" << std::endl;
	}
	else
	{
		std::cerr << "<!!> Failed to initialize Oculus" << std::endl;
		exit(0);
	}
	
	// retrieves info from ini (config) file
	m_sModuleName   = oRf.check("name", Value("tracking_oculus"), "Tracking/Oculus Module name (string)").asString();
	m_dFps = oRf.check("fps", Value(m_dFpsDefault), "Frame Per Second (double)").asDouble();
	
	m_trackerPortName = "/tracking/oculusVR/oculus/head";
	m_trackerPort.open(m_trackerPortName.c_str());

	m_oculusDevice.startListening();
	std::cout << "--> Oculus capture has started" << std::endl;
	return true;
}


bool SWOculusTracking::updateModule()
{
	std::vector<float> l_dataTranslations   = m_oculusDevice.translations();
	std::vector<float> l_dataRotations      = m_oculusDevice.rotations();
	yarp::os::Bottle & l_targetBottle       = m_trackerPort.prepare();

	l_targetBottle.clear();
	l_targetBottle.addInt(OCULUS_LIB);

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

double SWOculusTracking::getPeriod()
{
	return 1/m_dFps;
}

bool SWOculusTracking::interruptModule()
{
	m_trackerPort.interrupt();
	std::cout << "--> Interrupting the Oculus Tracking module..." << std::endl;
	return true;
}

bool SWOculusTracking::close()
{
	m_oculusDevice.stopListening();
	std::cout << "--> Oculus capture ended" << std::endl;

	std::cout << "Closing Yarp..." << std::endl;
	m_trackerPort.close();

	std::cout << "Closing the Oculus tracking module..." << std::endl;

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
	SWOculusTracking l_headTracker;

	/* prepare and configure the resource finder */
	yarp::os::ResourceFinder rf;
		rf.setVerbose(true);
		rf.setDefaultConfigFile("oculus.ini"); //overridden by --from parameter
		rf.setDefaultContext("swooz-tracking/conf");   //overridden by --context parameter
		rf.configure("ICUB_ROOT", argc, argv);


	/* configure the module */
	std::cout << "Configuring the Oculus tracking module..."<< std::endl;
	if (l_headTracker.configure(rf))
	{
		/* run the module */
		std::cout << "Starting the Oculus tracking module..." << std::endl;
		l_headTracker.runModule();
	}
	else
	{
		std::cout << "Failed to configure the Oculus tracking module!"<< std::endl;
	}
	return 0;
}

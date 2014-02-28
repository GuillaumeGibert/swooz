/**
 * \file ForthTracking.h
 * \brief Defines ForthTracking class
 * \author Emmanuel Body
 * \date 07/01/13
 */


#ifndef HAND_TRACKER_H
#define HAND_TRACKER_H

#include <stdio.h>
#include <string>
#include <sstream>

///////////////////////////////
/*From the Yarp library */
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/Drivers.h>
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
////////////////////////////////////
/*From the FORTH library */
#include <OpenNI.h>
#include <PS1080.h>

//Opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

///////////////////////////////
#include <HandTrackerLib/HandTracker.hpp>
#include <stdexcept>
#include <iostream>
#include <math.h>


#define LOG(msg) (std::cout<<msg<<std::endl);


#define PI 3.14159265


/**
 * \file SWHandTracker.h
 * \author Guillaume Gibert
 * \date 19-07-2013
 * \brief The file containing the Hand Tracker module template
 *
 * This file contains the template of a Hand Tracker module, connected
 * to a RGBD sensor and sending arm/hand data to a yarp port
 *
 */


/**
 * \class CSWHandTracker
 * \author Guillaume Gibert
 * \date 19-07-2013
 * \brief The CSWHandTracker class sends arm/hand data from a RGBD sensor to
 * yarp ports
 *
 * This class connects to a specified RGBD sensor via the FORTH library, and sends
 * estimated values to a yarp port.
 */
class SWForthTracking : public RFModule
{

	private:
		// Config variables retrieved from the ini file
		std::string moduleName;
		std::string robotName;
		std::string effectorName;
		std::string deviceName;
		double handScale;
		double offsetTx;
		double offsetTy;
		double offsetTz;
		int initialValueJoint4;
		int initialValueJoint5;
		int initialValueJoint6;

		double m_eulerx;
		double m_eulery;
		double m_eulerz;

		// yarp ports & bottle
		std::string handTrackerPortName;
		std::string fingersTrackerPortName;
		yarp::os::BufferedPort<yarp::os::Bottle> handTrackerPort;
		yarp::os::BufferedPort<yarp::os::Bottle> fingersTrackerPort;

		// FORTH library
		FORTH::HandTracker *ht;
		openni::VideoStream depth, color;
		openni::Device device;
		bool tracking;


	public:
		/**
		 * \function CSWHandTracker
		 * \brief The CSWHandTracker constructor creates the object to
		 *  interface RGBD sensor, the FORTH library and yarp
		 *
		 * This constructor creates the module which will connect to the
		 * specified RGBD sensor for data retrieval and which will send data
		 * to yarp ports.
		 */
        SWForthTracking();

		/**
		 * \function ~CSWHandTracker
		 * \brief The ~CSWHandTracker destructor destroys the module and closes any
		 * related connections
		 *
		 * This destructor deletes all instances and closes any opened connections
		 * with the RGBD sensor yarp ports.
		 */
        ~SWForthTracking();


		/**
		 * \function configure
		 * \param rf: the resource finder  address
		 * \return true if the configure step was successfull
		 * \brief The configure function loads the config options.
		 *
		 * This function loads the config options.
		 */
		bool configure(ResourceFinder &rf);

		/**
		 * \function updateModule
		 * \return true if the update step was successfull
		 * \brief The updateModule function update the module.
		 *
		 * This function updates the module.
		 */
		bool updateModule();

		/**
		 * \function getPeriod
		 * \return a value in second which correponds to the period of calling th upDateModule() method
		 * \brief The getPeriod function to choose the period of update.
		 *
		 * This function gets the period of update.
		 */
		double getPeriod();

		/**
		 * \function interruptModule
		 * \return true if the interrupt module step was successfull
		 * \brief The interruptModule function interrupts the module.
		 *
		 * This function interrupts the module.
		 */
		bool interruptModule();

		/**
		 * \function close
		 * \return true if the closing step was ok
		 * \brief The close function terminates the connection and listening
		 *
		 * This function terminates the listening mechanism and related processes
		 * to stop the RGBD sensor connection.
		 */
		bool close();

};



#endif


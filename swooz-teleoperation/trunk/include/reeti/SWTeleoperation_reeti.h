
/**
 * \file SWTeleoperation_reeti.h
 * \author Guillaume Gibert
 * \date 16/03/2015
 * \brief ...
 */

#ifndef _TELEOPERATION_REETI_
#define _TELEOPERATION_REETI_

// STD
#include <vector>
#include <string>

// SWOOZ
#include "commonTypes.h"

// YARP
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>

#include <yarp/sig/Vector.h>

#include <yarp/math/Math.h>

#include <yarp/dev/Drivers.h>
#include <yarp/dev/CartesianControl.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include "urbi/uclient.hh"


using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

using namespace urbi;

/**
 * \class SWTeleoperation_reeti
 * \author Guillaume Gibert
 * \date 16-03-2015
 * \brief ...
 */
class SWTeleoperation_reeti : public RFModule
{
    public:

        /**
         * \brief SWTeleoperation_reeti constructor
         */
        SWTeleoperation_reeti();
    
	/**
         * \brief SWTeleoperation_reeti destructor
         */
        ~SWTeleoperation_reeti();  

        /**
         * \brief The configure function loads the config options.
         *
         * This function loads the config options.
         * \param oRf: the resource finder  address
         * \return true if the configure step was successfull
         */
        bool configure(ResourceFinder &oRf);

        /**
         * \brief Reset Reeti to neutral position
         */
        void resetPosition();

       /**
         * \brief The close function terminates the connection and listening
         *
         * This function terminates the listening mechanism and related processes
         * to stop the eye-tracker connection.
         * \return true if the closing step was ok
         */
        bool close();

        /**
         * \brief The updateModule function update the module.
         *
         * This function updates the module.
         * \return true if the update step was successfull
         */
        bool updateModule();

        /**
         * \brief The interruptModule function interrupts the module.
         *
         * This function interrupts the module.
         * \return true if the interrupt module step was successfull
         */
        bool interruptModule();

        /**
         * \brief The getPeriod function to choose the period of update.
         *
         * This function gets the period of update.
         * \return a value in second which correponds to the period of calling th upDateModule() method
         */
        double getPeriod();


    private:

        bool m_bHeadActivatedDefault;
	bool m_bFaceActivatedDefault;
	bool m_bGazeActivatedDefault;
        bool m_bLEDSActivatedDefault;
      
        bool m_bHeadActivated;
	bool m_bFaceActivated;
	bool m_bGazeActivated;
        bool m_bLEDSActivated;
      
        int m_i32Fps;                 /**< fps (define the period for calling updateModule) */
        int m_i32HeadTimeLastBottle;  /**< time elapsed without head bottle command */
        int m_i32HeadTimeoutReset;    /**< head timeout reset reeti */
        
	// Config variables retrieved from the ini file
        std::string m_sModuleName;      /**< name of the module (config) */
	std::string m_sRobotName;	/**< name of the robot (config) */
        std::string m_sRobotAddress;    /**< address of the robot (config) */
	int m_i32RobotPort; /**< port of the robot (config) */
	double m_dNeckRotatMinValueJoint;
	double m_dNeckRotatMaxValueJoint;
	double m_dNeckRotatNeuValueJoint;
	
	double m_dNeckTiltMinValueJoint;
	double m_dNeckTiltMaxValueJoint;
	double m_dNeckTiltNeuValueJoint;
	
	double m_dNeckPanMinValueJoint;
	double m_dNeckPanMaxValueJoint;
	double m_dNeckPanNeuValueJoint;
	
	double m_dLeftLCMinValueJoint;
	double m_dLeftLCMaxValueJoint;
	double m_dLeftLCNeuValueJoint;
	
	double m_dRightLCMinValueJoint;
	double m_dRightLCMaxValueJoint;
	double m_dRightLCNeuValueJoint;
	
	double m_dTopLipMinValueJoint;
	double m_dTopLipMaxValueJoint;
	double m_dTopLipNeuValueJoint;
	
	double m_dBottomLipMinValueJoint;
	double m_dBottomLipMaxValueJoint;
	double m_dBottomLipNeuValueJoint;
	
	double m_dRightEarMinValueJoint;
	double m_dRightEarMaxValueJoint;
	double m_dRightEarNeuValueJoint;
	
	double m_dLeftEarMinValueJoint;
	double m_dLeftEarMaxValueJoint;
	double m_dLeftEarNeuValueJoint;
	
	double m_dRightEyeTiltMinValueJoint;
	double m_dRightEyeTiltMaxValueJoint;
	double m_dRightEyeTiltNeuValueJoint;
	
	double m_dLeftEyeTiltMinValueJoint;
	double m_dLeftEyeTiltMaxValueJoint;
	double m_dLeftEyeTiltNeuValueJoint;
	
	double m_dRightEyePanMinValueJoint;
	double m_dRightEyePanMaxValueJoint;
	double m_dRightEyePanNeuValueJoint;
	
	double m_dLeftEyePanMinValueJoint;
	double m_dLeftEyePanMaxValueJoint;
	double m_dLeftEyePanNeuValueJoint;
	
	double m_dRightEyeLidMinValueJoint;
	double m_dRightEyeLidMaxValueJoint;
	double m_dRightEyeLidNeuValueJoint;
	
	double m_dLeftEyeLidMinValueJoint;
	double m_dLeftEyeLidMaxValueJoint;
	double m_dLeftEyeLidNeuValueJoint;
	
	yarp::os::BufferedPort<yarp::os::Bottle> m_oHeadTrackerPort; /**< head yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oFaceTrackerPort; /**< Face yarp tracker port */
	yarp::os::BufferedPort<yarp::os::Bottle> m_oGazeTrackerPort; /**< Gaze yarp tracker port */
	
       // Urbi client
       UClient* m_pClient;
};

#endif


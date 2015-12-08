
/**
 * \file SWSynchronizediCubEncoders.h
 * \author Guillaume Gibert
 * \date 01/10/2015
 * \brief ...
 */

#ifndef _SYNCHRONIZED_ICUB_ENCODERS_
#define _SYNCHRONIZED_ICUB_ENCODERS_

// STD
#include <vector>
#include <string>
#include <sstream>


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


using namespace yarp::os;
using namespace yarp::sig;


/**
 * \class SWSynchronizediCubEncoders
 * \author Guillaume Gibert
 * \date 01-10-2015
 * \brief ...
 */
class SWSynchronizediCubEncoders : public RFModule
{
    public:

        /**
         * \brief SWTeleoperation_reeti constructor
         */
        SWSynchronizediCubEncoders();
    
	/**
         * \brief SWTeleoperation_reeti destructor
         */
        ~SWSynchronizediCubEncoders();  

        /**
         * \brief The configure function loads the config options.
         *
         * This function loads the config options.
         * \param oRf: the resource finder  address
         * \return true if the configure step was successfull
         */
        bool configure(ResourceFinder &oRf);

     
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

	bool m_bIsRunning;                      /**<  Whether the thread is running */
	bool m_bInitialized;
        
	// body parts activated by default
	bool m_bHeadActivatedDefault;                /**< ... */
	bool m_bTorsoActivatedDefault;                /**< ... */
        bool m_bLeftArmActivatedDefault;               /**< ... */
        bool m_bRightArmActivatedDefault;               /**< ... */
    
	// body parts activated
	bool m_bHeadActivated;                /**< ... */
	bool m_bTorsoActivated;                /**< ... */
        bool m_bLeftArmActivated;               /**< ... */
        bool m_bRightArmActivated;               /**< ... */

        int m_i32Fps;                           /**< fps (define the period for calling updateModule) */

        // Config variables retrieved from the ini file
        std::string m_sModuleName;              /**< name of the mondule (config) */
        std::string m_sRobotName;               /**< name of the robot (config) */

	// sync yarp data port 
	std::string m_sSynchronizedDataPortName;
	yarp::os::BufferedPort<yarp::os::Bottle> m_oSynchronizedDataPort; /**< head yarp tracker port  */
       	
	// head control
	yarp::os::Property m_oHeadOptions;              /**< robot interfaces for head/gaze movements */
	yarp::dev::PolyDriver        m_oRobotHead;      /**< ... */         
	yarp::dev::IPositionControl *m_pIHeadPosition;                           /**< arm position control pointer */	
	yarp::dev::IEncoders        *m_pIHeadEncoders;  /**< ... */
	yarp::dev::IVelocityControl *m_pIHeadVelocity;
	int m_i32HeadJointsNb;  /**< ... */
		
	// torso control
	yarp::os::Property m_oTorsoOptions;              /**< robot interfaces for head/gaze movements */
	yarp::dev::PolyDriver        m_oRobotTorso;      /**< ... */         
	yarp::dev::IPositionControl *m_pITorsoPosition;                           /**< arm position control pointer */	
	yarp::dev::IEncoders        *m_pITorsoEncoders;  /**< ... */
	yarp::dev::IVelocityControl *m_pITorsoVelocity;
	int m_i32TorsoJointsNb;  /**< ... */
	
	// left arm control
	yarp::os::Property m_oLeftArmOptions;              /**< robot interfaces for head/gaze movements */
	yarp::dev::IEncoders        *m_pILeftArmEncoders;                           /**< arm encoder pointer */
	yarp::dev::IPositionControl *m_pILeftArmPosition;                           /**< arm position control pointer */
	yarp::dev::PolyDriver        m_oRobotLeftArm;                               /**< robot arm controller */
	yarp::dev::IVelocityControl *m_pILeftArmVelocity;
	int  m_i32LeftArmJointsNb;  /**< ... */
		
	// Right arm control
	yarp::os::Property m_oRightArmOptions;              /**< robot interfaces for head/gaze movements */
	yarp::dev::IEncoders        *m_pIRightArmEncoders;                           /**< arm encoder pointer */
	yarp::dev::IPositionControl *m_pIRightArmPosition;                           /**< arm position control pointer */
	yarp::dev::PolyDriver        m_oRobotRightArm;                               /**< robot arm controller */
	yarp::dev::IVelocityControl *m_pIRightArmVelocity;
	int  m_i32RightArmJointsNb;  /**< ... */
};

#endif


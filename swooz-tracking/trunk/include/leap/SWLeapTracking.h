
/**
 * \file SWLeapTracking.h
 * \brief Defines SWFaceLabTracking class
 * \author Gabriel MEYNET
 * \date 03/04/14
 */

#ifndef _SWLEAPTRACKING_
#define _SWLEAPTRACKING_

// SWOOZ
#include "SWExceptions.h"

// LEAP
#include "devices/leap/SWLeap.h"

// YARP

#include <yarp/dev/all.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>


/**
 * \class SWLeapTracking
 * \brief This module sends lead data...
 *
 * Bottles contents :
 *
 *
 */
class SWLeapTracking : public yarp::os::RFModule
{
    public :

        /**
         * \brief default constructor of SWLeapTracking
         */
        SWLeapTracking();

        /**
         * \brief Init configuration values with the config file
         * \param [in] oRf : icub resource config file
         * \return true if configuration successful
         */
        bool configure(yarp::os::ResourceFinder &oRf);


        /**
         * \brief Return the init state of the Leap.
         * \return true if the Leap has been initialized
         */
        bool isLeapInitialized() const;

    private :

        /**
         * \brief Close connections.
         * \return true (Module heritage)
         */
        bool close();

        /**
         * \brief Interrupt the module.
         * \return true (Module heritage)
         */
        bool interruptModule();

        /**
         * \brief Init Leap device
         */
        void initLeap();

        /**
         * \brief Called periodically every getPeriod() seconds
         * \return true
         */
        bool updateModule();

        /**
         * \brief Retrieve the update function call period of the module.
         * \return the period
         */
        double getPeriod();

    private :

		bool m_bIsLeapInitialized;           /**< is leap initialized ? */
        int m_i32Fps;                           /**< refresh rate of updateModule calling */


		std::string m_sFingerTrackingPortNameLeft;	/**< yarp finger tracking port name */
		std::string m_sHandTrackingPortNameLeft;	/**< yarp hand tracking port name */
		std::string m_sArmTrackingPortNameLeft;		/**< yarp Arm tracking port name */
		std::string m_sFingerTrackingPortNameRight;	/**< yarp finger tracking port name */
		std::string m_sHandTrackingPortNameRight;	/**< yarp hand tracking port name */
		std::string m_sArmTrackingPortNameRight;		/**< yarp Arm tracking port name */
	
		yarp::os::BufferedPort<yarp::os::Bottle> m_oFingerTrackingPortLeft;	 /**< yarp finger tracking port */
		yarp::os::BufferedPort<yarp::os::Bottle> m_oHandTrackingPortLeft; /**< yarp hand tracking port */
		yarp::os::BufferedPort<yarp::os::Bottle> m_oArmTrackingPortLeft;  /**< yarp Arm tracking port */
		yarp::os::BufferedPort<yarp::os::Bottle> m_oFingerTrackingPortRight;	 /**< yarp finger tracking port */
		yarp::os::BufferedPort<yarp::os::Bottle> m_oHandTrackingPortRight; /**< yarp hand tracking port */
		yarp::os::BufferedPort<yarp::os::Bottle> m_oArmTrackingPortRight;  /**< yarp Arm tracking port */

        swDevice::SWLeap m_oLeap;         /**< Leap device */ 
		
};


#endif

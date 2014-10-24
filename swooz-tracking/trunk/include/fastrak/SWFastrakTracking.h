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

#ifndef __SWFASTRAKTRACKING__
#define __SWFASTRAKTRACKING__


#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/Drivers.h>


#include "devices/fastrak/SWFastrak_thread.h"


/**
 * \class SWFastrakTracking
 * \author Emmanuel Body
 * \date 29-11-2013
 * \brief The SWFastrakTracking class sends position/orientation data from a tracker
 * to yarp ports
 *
 * This class connects to a tracker, and sends
 * estimated values to a yarp port.
 */
class SWFastrakTracking : public yarp::os::RFModule
{
    public:

        /**
         * \brief Init configuration values with config files, herited from RFModule
         * \param [in] oRf : icub resource config file
         * \return true if configuration successful
         */
        bool configure(yarp::os::ResourceFinder & rf);

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

        /**
         * \brief Interrupt the module.
         * \return true to confirm that the module is ready to be closed
         */
        bool interruptModule();

        /**
         * \brief Close connections.
         * \return true (Module heritage)
         */
        bool close();

    private:

        std::vector<float> & getData();


        swDevice::SWFastrak_thread m_fastrakDevice;

        //yarp ports and bottle
        std::string m_trackerPortName;
        yarp::os::BufferedPort<yarp::os::Bottle> m_trackerPort;

        int m_fps;   /**< refresh rate of updateModule calling */
};

#endif // SWFASTRAKTRACKING_H

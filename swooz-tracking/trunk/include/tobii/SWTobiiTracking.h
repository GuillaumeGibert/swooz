
/**
 * \file SWTobiiTracking.h
 * \brief Defines SWTobiiTracking class
 * \author Florian Lance
 * \date 29/07/14
 */

#ifndef _SWTOBIITRACKING_
#define _SWTOBIITRACKING_


// TOBII
#include "devices/tobii/Tobii.h"

// YARP

#include <yarp/dev/all.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>


/**
 * \class SWTobiiTracking
 * \brief This module sends tobii data...
 *
 * Bottles contents :
 *
 *
 */
class SWTobiiTracking : public yarp::os::RFModule
{
    public :

        /**
         * \brief default constructor of SWTobiiTracking
         */
        SWTobiiTracking();

        /**
         * \brief Init configuration values with the config file
         * \param [in] oRf : icub resource config file
         * \return true if configuration successful
         */
        bool configure(yarp::os::ResourceFinder &oRf);

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

        int m_i32Fps;               /**< refresh rate of updateModule calling */

        uint64_t m_ui64LastUpdate;
        boost::shared_ptr<TobiiTracker::CTobii> m_tobii; /**< tobii interface */

        tobii::sdk::cpp::tracking::gaze_data_item* m_pGazeData;

        yarp::os::BufferedPort<yarp::os::Bottle> m_gazeTrackingPort;    /**< ... */

};


#endif

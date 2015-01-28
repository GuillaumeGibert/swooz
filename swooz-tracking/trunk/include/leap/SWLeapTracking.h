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
 * \file SWLeapTracking.h
 * \brief Defines SWLeapTracking class
 * \author Gabriel MEYNET, Florian LANCE
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
 * \brief This module sends leap data...
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

        bool m_bIsLeapInitialized;  /**< is leap initialized ? */
        int m_i32Fps;               /**< refresh rate of updateModule calling */

        std::string m_sHandFingersTrackingPortNameLeft;  /**< ... */
        std::string m_sHandFingersTrackingPortNameRight; /**< ... */
        std::string m_sHandTrackingPortNameLeft;         /**< ... */
        std::string m_sHandTrackingPortNameRight;        /**< ... */

        yarp::os::BufferedPort<yarp::os::Bottle> m_oHandFingersTrackingPortLeft;    /**< ... */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oHandFingersTrackingPortRight;   /**< ... */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oHandTrackingPortRight;          /**< ... */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oHandTrackingPortLeft;           /**< ... */

        swDevice::SWLeap m_oLeap;         /**< Leap device */ 
		
};


#endif

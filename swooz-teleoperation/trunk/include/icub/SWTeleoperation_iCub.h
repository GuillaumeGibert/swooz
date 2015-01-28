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
 * \file SWTeleoperation_iCub.h
 * \author Florian Lance, Guillaume Gibert, Emmanuel Body
 * \date 05-12-2013
 * \brief ...
 */

#ifndef _TELEOPERATION_ICUB_
#define _TELEOPERATION_ICUB_


#include "SWIcubHead.h"
#include "SWIcubTorso.h"
#include "SWIcubArm.h"



// STD
#include <vector>
#include <string>

// SWOOZ
#include "commonTypes.h"

// YARP
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>

#include <yarp/sig/Vector.h>

#include <yarp/math/Math.h>

#include <yarp/dev/Drivers.h>
#include <yarp/dev/CartesianControl.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;


/**
 * \class SWTeleoperation_iCub
 * \author Guillaume Gibert, Florian Lance, Emmanuel Body
 * \date 05-12-2013
 * \brief ...
 */
class SWTeleoperation_iCub : public RFModule
{
    public:

        /**
         * \brief SWTeleoperation_iCub constructor
         */
        SWTeleoperation_iCub();

        /**
         * \brief SWTeleoperation_iCub destructor
         */
        ~SWTeleoperation_iCub();

        /**
         * \brief The configure function loads the config options.
         *
         * This function loads the config options.
         * \param rf: the resource finder  address
         * \return true if the configure step was successfull
         */
        bool configure(ResourceFinder &rf);

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
        bool m_bHeadInitialized;                /**< ... */
        bool m_bTorsoInitialized;               /**< ... */
        bool m_bLeftArmInitialized;               /**< ... */
        bool m_bRightArmInitialized;               /**< ... */

        int m_i32Fps;                           /**< fps (define the period for calling updateModule) */

        // Config variables retrieved from the ini file
        std::string m_sModuleName;              /**< name of the mondule (config) */
        std::string m_sRobotName;               /**< name of the robot (config) */

        swTeleop::SWIcubHead m_oIcubHeadControl; /**< ... */
        swTeleop::SWIcubTorso m_oIcubTorsoControl; /**< ... */
        swTeleop::SWIcubArm m_oIcubLeftArmControl; /**< ... */
        swTeleop::SWIcubArm m_oIcubRightArmControl; /**< ... */
};

#endif


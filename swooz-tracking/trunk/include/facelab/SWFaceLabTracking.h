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
 * \file SWFaceLabTracking.h
 * \brief Defines SWFaceLabTracking class
 * \author Florian Lance
 * \date 29/11/13
 */

#ifndef _SWFACELABTRACKING_
#define _SWFACELABTRACKING_

// SWOOZ
#include "SWExceptions.h"

// FACELAB
#include "devices/faceLab/SWFaceLab.h"

// YARP
#include <yarp/dev/all.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>


/**
 * \class SWFaceLabTracking
 * \brief This module sends faceLab data (head,face,gaze) on 3 differents yarp ports.
 *
 * Bottles contents :
 *
 * Head :
 *
 *  device id : COREDATA_LIB  / get(0).asInt()
 *
 *   translation
 *       Tx / get(1).asDouble()
 *       Ty / get(2).asDouble()
 *       Tz / get(3).asDouble()
 *   rotation
 *       Rx / get(4).asDouble()
 *       Ry / get(5).asDouble()
 *       Rz / get(6).asDouble()
 *
 * Face :
 *
 *  device id : COREDATA_LIB  / get(0).asInt()
 *
 *  outer lip
 *      outerLip0 x / get(1).asDouble()
 *      outerLip0 y / get(2).asDouble()
 *      outerLip0 z / get(3).asDouble()
 *      outerLip1 x / get(4).asDouble()
 *      outerLip1 y / get(5).asDouble()
 *      outerLip1 z / get(6).asDouble()
 *      outerLip2 x / get(7).asDouble()
 *      outerLip2 y / get(8).asDouble()
 *      outerLip2 z / get(9).asDouble()
 *      outerLip3 x / get(10).asDouble()
 *      outerLip3 y / get(11).asDouble()
 *      outerLip3 z / get(12).asDouble()
 *
 *  ... (see cpp)
 *
 *  TODO : end doc
 *
 */
class SWFaceLabTracking : public yarp::os::RFModule
{
    public :

        /**
         * \brief default constructor of SWFaceLabTracking
         */
        SWFaceLabTracking();

        /**
         * \brief Init configuration values with the config file
         * \param [in] oRf : icub resource config file
         * \return true if configuration successful
         */
        bool configure(yarp::os::ResourceFinder &oRf);


        /**
         * \brief Return the init state of the faceLab.
         * \return true if the faceLab has been initialized
         */
        bool isFaceLabInitialized() const;

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
         * \brief Init facelab device
         */
        void initFaceLab();

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

        bool m_bIsFaceLabInitialized;           /**< is facelab initialized ? */
        int m_i32Fps;                           /**< refresh rate of updateModule calling */

        std::string m_sHeadTrackingPortName;    /**< yarp head tracking port name */
        std::string m_sFaceTrackingPortName;    /**< yarp face tracking port name */
        std::string m_sGazeTrackingPortName;    /**< yarp gaze tracking port name */

        yarp::os::BufferedPort<yarp::os::Bottle> m_oHeadTrackingPort;   /**< yarp head tracking port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oFaceTrackingPort;   /**< yarp face tracking port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oGazeTrackingPort;   /**< yarp gaze tracking port */

        swDevice::SWFaceLab m_oFaceLab;         /**< facelab device */
};


#endif

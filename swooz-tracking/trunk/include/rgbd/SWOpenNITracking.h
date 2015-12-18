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
 * \file OpenNITracking.h
 * \brief Defines OpenNITracking class
 * \author Emmanuel Body
 * \date 07/01/13
 */

#ifndef _SWOPENNITRACKING_
#define _SWOPENNITRACKING_


// kinect thread (for display)
#include "devices/rgbd/SWKinect_thread.h"

// Kinect Skeleton (OpenNi)
#include "devices/rgbd/SWKinectSkeleton.h"

// YARP
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

// stdlib
#include <string>
#include <vector>
#include <iostream>

// BOOST
#include <boost/scoped_ptr.hpp>

/**
 * \class SWOpenNITracking
 * \brief This module sends OpenNI upper body skeleton data on a multitude of differents ports
 */
class SWOpenNITracking : public yarp::os::RFModule
{
    public:

        /**
         * @brief SWOpenNITracking constructor
         */
        SWOpenNITracking();

        /**
         * @brief SWOpenNITracking destructor
         */
        ~SWOpenNITracking();

        /**
         * @brief configure
         * @param [in] rf : resource finder
         * @return
         */
        bool configure(yarp::os::ResourceFinder &rf);

        /**
         * @brief isOpenNIInitialized
         * @return
         */
        bool isOpenNIInitialized() const;

        /**
         * @brief initOpenNi
         */
        void initOpenNi();

        /**
         * @brief updateModule
         * @return
         */
        bool updateModule();

        /**
         * @brief getPeriod
         * @return
         */
        double getPeriod();

        /**
         * @brief interruptModule
         * @return
         */
        bool interruptModule();

    private:

        bool m_IsOpenNiInitialized;

        std::string m_sHeadTrackingPortName;     /**< head tracking port name */
        std::string m_sTorsoTrackingPortName;    /**< torso tracking port name */
        std::string m_sLeftArmTrackingPortName;  /**< left arm trakcing port name */
        std::string m_sRightArmTrackingPortName; /**< right arm tracking port name */
    
	std::string m_siCubEncodersTrackingPortName; /**< right arm tracking port name */

        // buffered ports
        yarp::os::BufferedPort<yarp::os::Bottle> m_oHeadTrackingPort;        /**< head tracking port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oTorsoTrackingPort;       /**< torso tracking port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oLeftArmTrackingPort;     /**< left arm tracking port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oRightArmTrackingPort;    /**< right arm tracking port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oAllJointsTrackingPort;   /**< ... */
    
	yarp::os::BufferedPort<yarp::os::Bottle> m_oiCubEncodersTrackingPort;   /**< ... */

        boost::scoped_ptr<swDevice::SWKinectSkeleton> m_oKinectSkeleton;     /**< kinect skeleton */
};

#endif


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

        // buffered ports
        yarp::os::BufferedPort<yarp::os::Bottle> m_oHeadTrackingPort;        /**< head tracking port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oTorsoTrackingPort;       /**< torso tracking port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oLeftArmTrackingPort;     /**< left arm tracking port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oRightArmTrackingPort;    /**< right arm tracking port */

        boost::scoped_ptr<swDevice::SWKinectSkeleton> m_oKinectSkeleton;     /**< kinect skeleton */
};

#endif

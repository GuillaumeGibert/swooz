
/**
 * \file SWTeleoperation_nao.h
 * \author Marwin Sorce / Florian Lance / Emmanuel Body
 * \date 21/01/2014
 * \brief ...
 */

#ifndef _TELEOPERATION_NAO_
#define _TELEOPERATION_NAO_

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
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>

#include <yarp/sig/Vector.h>

#include <yarp/math/Math.h>

#include <alproxies/almotionproxy.h>
#include <alproxies/alrobotpostureproxy.h>
#include <alerror\alerror.h>
#include <qi/os.hpp>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;
using namespace AL;

/**
 * \class SWTeleoperation_iCub
 * \author Guillaume Gibert, Florian Lance, Emmanuel Body
 * \date 05-12-2013
 * \brief ...
 */
class SWTeleoperation_nao : public RFModule
{
    public:

        /**
         * \brief SWTeleoperation_nao constructor
         */
        SWTeleoperation_nao();

        /**
         * \brief The configure function loads the config options.
         *
         * This function loads the config options.
         * \param oRf: the resource finder  address
         * \return true if the configure step was successfull
         */
        bool configure(ResourceFinder &oRf);

        /**
         * \brief Reset iCub head position
         */
        void resetHeadPosition();

        /**
         * \brief Reset iCub torso position
         */
        void resetTorsoPosition();

        /**
         * \brief Reset iCub torso position
         */
        void resetLeftArmPosition();

        /**
         * \brief Reset iCub torso position
         */
        void resetRightArmPosition();

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
        bool m_bTorsoActivatedDefault;
        bool m_bLEDSActivatedDefault;
        bool m_bLeftArmActivatedDefault;
        bool m_bRightArmActivatedDefault;

        bool m_bHeadActivated;
        bool m_bTorsoActivated;
        bool m_bLEDSActivated;
        bool m_bLeftArmActivated;
        bool m_bRightArmActivated;

        int m_startPosture;
        int m_endPosture;

        std::vector<double> m_vHeadMinJointDefault;         /**< ... */
        std::vector<double> m_vHeadMaxJointDefault;         /**< ... */
        std::vector<double> m_vLeftArmMinJointDefault;      /**< ... */
        std::vector<double> m_vLeftArmMaxJointDefault;      /**< ... */
        std::vector<double> m_vRightArmMinJointDefault;     /**< ... */
        std::vector<double> m_vRightArmMaxJointDefault;     /**< ... */

        std::vector<double> m_vHeadMinJoint;                /**< ... */
        std::vector<double> m_vHeadMaxJoint;                /**< ... */
        std::vector<double> m_vLeftArmMinJoint;             /**< ... */
        std::vector<double> m_vLeftArmMaxJoint;             /**< ... */
        std::vector<double> m_vRightArmMinJoint;            /**< ... */
        std::vector<double> m_vRightArmMaxJoint;            /**< ... */

        double m_dTorsoMinValueJoint;   /**< ... */
        double m_dTorsoMaxValueJoint;   /**< ... */

        int m_i32Fps;                 /**< fps (define the period for calling updateModule) */
        int m_i32HeadTimeLastBottle;  /**< time elapsed without head bottle command */
        int m_i32HeadTimeoutReset;    /**< head timeout reset nao */
        double m_dJointVelocityValue; /**< ano velocity value */

        // Array for nao's joints
        AL::ALValue m_aHeadAngles;
        AL::ALValue m_aTorsoAngles;
        AL::ALValue m_aLArmAngles;
        AL::ALValue m_aRArmAngles;
        AL::ALValue m_aLLegAngles;
        AL::ALValue m_aRLegAngles;

        std::string m_sModuleName;      /**< name of the mondule (config) */
        std::string m_sRobotAddress;    /**< name of the robot (config) */

        yarp::os::BufferedPort<yarp::os::Bottle> m_oHeadTrackerPort; /**< head yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oTorsoTrackerPort; /**< torso yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oFaceTrackerPort; /**< Face yarp tracker port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oLeftArmTrackerPort; /**< Left Arm yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oRightArmTrackerPort; /**< Right Arm yarp tracker port  */

        ALMotionProxy *m_oRobotMotionProxy;
};

#endif


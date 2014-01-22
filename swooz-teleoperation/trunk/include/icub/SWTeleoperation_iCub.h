
/**
 * \file SWTeleoperation_iCub.h
 * \author Florian Lance, Guillaume Gibert, Emmanuel Body
 * \date 05-12-2013
 * \brief ...
 */

#ifndef _TELEOPERATION_ICUB_
#define _TELEOPERATION_ICUB_

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
         * \brief Reset iCub head position
         */
        void resetHeadPosition();

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

        /**
         * \brief The eyesOpeningCode function transforms a percentage of opening eyelids
         * into a hexa code ready to send to the iCub.
         *
         * This function transforms a command in percentage of eyelid opening form into a hexa code
         * that can be understood by the iCub.
         * \param dEyeLids    : the percentage of opening/closing eyelids
         * \param i32MinValue : the min value accepted by the iCub (30) or by the simulator (24)
         * \param i32MaxValue : the max value accepted by the iCub (70) or by the simulator (48)
         * \return a string containing the hexa code
         */
        std::string eyesOpeningCode(cdouble dEyeLids, cint i32MinValue, cint i32MaxValue);

    private:

        bool m_bIsRunning;                      /**<  Whether the thread is running */

        int m_i32Fps;                           /**< fps (define the period for calling updateModule) */
        int m_i32TimeoutReset;                  /**< timeout reset icub */


        int m_i32HeadTimeLastBottle;            /**< time elapsed without head bottle command */
        int m_i32FaceTimeLastBottle;            /**< time elapsed without face bottle command */
        int m_i32ArmsTimeLastBottle;            /**< time elapsed without arms bottle command */
        int m_i32GazeTimeLastBottle;            /**< time elapsed without gaze bottle command */

        int m_i32HeadTimeoutReset;              /**< head timeout reset icub */
        int m_i32FaceTimeoutReset;              /**< face timeout reset icub */
        int m_i32ArmsTimeoutReset;              /**< arms timeout reset icub */
        int m_i32GazeTimeoutReset;              /**< gaze timeout reset icub */

        int m_i32JointVelocityValue;            /**< iCub velocity value */
        int m_i32JointAccelerationValue;        /**< iCub accelereation value */

        int m_i32HeadMaxValueJoint0;            /**< ... */
        int m_i32HeadMinValueJoint0;            /**< ... */
        int m_i32HeadMaxValueJoint1;            /**< ... */
        int m_i32HeadMinValueJoint1;            /**< ... */
        int m_i32HeadMaxValueJoint2;            /**< ... */
        int m_i32HeadMinValueJoint2;            /**< ... */
        int m_i32MinValueEyelids;               /**< ... */
        int m_i32MaxValueEyelids;               /**< ... */

        // Config variables retrieved from the ini file
        std::string m_sModuleName;              /**< name of the mondule (config) */
        std::string m_sRobotName;               /**< name of the robot (config) */

        std::string m_sHeadTrackerPortName;     /**< name of the head tracker port */
        std::string m_sHeadLocalPortName;       /**< name of the head local port */
        std::string m_sHeadRemotePortName;      /**< name of the head remote port */
        std::string m_sHeadControlName;         /**< control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oHeadTrackerPort; /**< head yarp tracker port  */

        std::string m_sFaceTrackerPortName;     /**< name of the Face tracker port */
        std::string m_sFaceLocalPortName;       /**< name of the Face local port */
        std::string m_sFaceRemotePortName;      /**< name of the Face remote port */
        std::string m_sFaceControlName;         /**< control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oFaceTrackerPort; /**< Face yarp tracker port */

        std::string m_sEyelidInputPortName;     /**< name of the eyelid input port */
        std::string m_sEyelidOutputPortName;    /**< name of the eyelid output port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oFaceHandlerPort; /**< Face handler port */

        std::string m_sGazeTrackerPortName;     /**< name of the Gaze tracker port */
        std::string m_sGazeLocalPortName;       /**< name of the Gaze local port */
        std::string m_sGazeRemotePortName;      /**< name of the Gaze remote port */
        std::string m_sGazeControlName;         /**< Gaze control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oGazeTrackerPort; /**< gaze yarp tracker port  */

//        std::string m_sLeftArmTrackerPortName;     /**< name of the left arm tracker port */
//        std::string m_sLeftArmLocalPortName;       /**< name of the left arm local port */
//        std::string m_sLeftArmRemotePortName;      /**< name of the left arm remote port */
//        std::string m_sLeftArmControlName;         /**< left arm control name */
//        yarp::os::BufferedPort<yarp::os::Bottle> m_oLeftArmTrackerPort; /**< left arm yarp port */

//        std::string m_sRightArmTrackerPortName;     /**< name of the right arm tracker port */
//        std::string m_sRightArmLocalPortName;       /**< name of the right arm local port */
//        std::string m_sRightArmRemotePortName;      /**< name of the right arm remote port */
//        std::string m_sRightArmControlName;         /**< right arm control name */
//        yarp::os::BufferedPort<yarp::os::Bottle> m_oRightArmTrackerPort; /**< right arm yarp port */


        // head
        yarp::dev::PolyDriver        m_oRobotHead;     /**< ... */
        yarp::dev::IEncoders        *m_pIHeadEncoders; /**< ... */
        yarp::dev::IPositionControl *m_pIHeadPosition; /**< ... */
        yarp::dev::IVelocityControl *m_pIHeadVelocity; /**< ... */

        // left arm
        yarp::dev::PolyDriver       m_oRobotLeftArm; /**< ... */
        // ...

        // right arm
        yarp::dev::PolyDriver       m_oRobotRightArm;/**< ... */
        // ...

        yarp::os::Property m_oHeadOptions;      /**< robot interfaces for head/gaze movements */
//        yarp::os::Property m_oLeftArmOptions;   /**< robot interfaces for left arm/hand movements */
//        yarp::os::Property m_oRightArmOptions;  /**< robot interfaces for right arm/hand movements */
};

#endif


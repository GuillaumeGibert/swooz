
/**
 * \file SWTeleoperation_nao.h
 * \author Emmanuel Body
 * \date 21/01/2014
 * \brief ...
 */

#ifndef _TELEOPERATION_NAO_
#define _TELEOPERATION_NAO_

//#include "conio.h"

//#include <stdio.h>

//#include <sstream>

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
         * \brief SWTeleoperation_nao destructor
         */
        ~SWTeleoperation_nao();

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

        float m_fJointVelocityValue;            /**< iCub velocity value */

        float m_fHeadMaxValueJoint0;            /**< ... */
        float m_fHeadMinValueJoint0;            /**< ... */
        float m_fHeadMaxValueJoint1;            /**< ... */
        float m_fHeadMinValueJoint1;            /**< ... */
        float m_fLeftArmMinValueJoint0;
        float m_fLeftArmMinValueJoint1;
        float m_fLeftArmMinValueJoint2;
        float m_fLeftArmMinValueJoint3;
        float m_fLeftArmMinValueJoint4;
        float m_fLeftArmMinValueJoint5;
        float m_fLeftArmMaxValueJoint0;
        float m_fLeftArmMaxValueJoint1;
        float m_fLeftArmMaxValueJoint2;
        float m_fLeftArmMaxValueJoint3;
        float m_fLeftArmMaxValueJoint4;
        float m_fLeftArmMaxValueJoint5;
        float m_fRightArmMinValueJoint0;
        float m_fRightArmMinValueJoint1;
        float m_fRightArmMinValueJoint2;
        float m_fRightArmMinValueJoint3;
        float m_fRightArmMinValueJoint4;
        float m_fRightArmMinValueJoint5;
        float m_fRightArmMaxValueJoint0;
        float m_fRightArmMaxValueJoint1;
        float m_fRightArmMaxValueJoint2;
        float m_fRightArmMaxValueJoint3;
        float m_fRightArmMaxValueJoint4;
        float m_fRightArmMaxValueJoint5;
        float m_fLeftLegMinValueJoint0;
        float m_fLeftLegMinValueJoint1;
        float m_fLeftLegMinValueJoint2;
        float m_fLeftLegMinValueJoint3;
        float m_fLeftLegMinValueJoint4;
        float m_fLeftLegMinValueJoint5;
        float m_fLeftLegMaxValueJoint0;
        float m_fLeftLegMaxValueJoint1;
        float m_fLeftLegMaxValueJoint2;
        float m_fLeftLegMaxValueJoint3;
        float m_fLeftLegMaxValueJoint4;
        float m_fLeftLegMaxValueJoint5;
        float m_fRightLegMinValueJoint0;
        float m_fRightLegMinValueJoint1;
        float m_fRightLegMinValueJoint2;
        float m_fRightLegMinValueJoint3;
        float m_fRightLegMinValueJoint4;
        float m_fRightLegMinValueJoint5;
        float m_fRightLegMaxValueJoint0;
        float m_fRightLegMaxValueJoint1;
        float m_fRightLegMaxValueJoint2;
        float m_fRightLegMaxValueJoint3;
        float m_fRightLegMaxValueJoint4;
        float m_fRightLegMaxValueJoint5;
        float m_fTorsoMinValueJoint0;
        float m_fTorsoMaxValueJoint0;

        // Config variables retrieved from the ini file
        std::string m_sModuleName;              /**< name of the mondule (config) */
        std::string m_sRobotAddress;               /**< name of the robot (config) */

        std::string m_sHeadTrackerPortName;     /**< name of the head tracker port */
        std::string m_sHeadLocalPortName;       /**< name of the head local port */
        std::string m_sHeadRemotePortName;      /**< name of the head remote port */
        std::string m_sHeadControlName;         /**< control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oHeadTrackerPort; /**< head yarp tracker port  */
        bool m_bHeadCapture;

        std::string m_sTorsoTrackerPortName;     /**< name of the torso tracker port */
        std::string m_sTorsoLocalPortName;       /**< name of the torso local port */
        std::string m_sTorsoRemotePortName;      /**< name of the torso remote port */
        std::string m_sTorsoControlName;         /**< control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oTorsoTrackerPort; /**< torso yarp tracker port  */
        bool m_bTorsoCapture;

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

        std::string m_sLeftArmTrackerPortName;     /**< name of the Left Arm arm tracker port */
        std::string m_sLeftHandTrackerPortName;     /**< name of the Left Arm hand tracker port */
        std::string m_sLeftFingersTrackerPortName; /**< name of the Left Arm fingers tracker port */
        std::string m_sLeftArmControlName;         /**< Left Arm hand control name */
        std::string m_sLeftFingersControlName;     /**< Left Arm fingers control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oLeftArmTrackerPort; /**< Left Arm yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oLeftHandTrackerPort; /**< Left Hand yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oLeftFingersTrackerPort; /**< Left Fingers yarp tracker port  */
        bool m_bLeftArmCapture;



        std::string m_sRightArmTrackerPortName;     /**< name of the Right Arm arm tracker port */
        std::string m_sRightHandTrackerPortName;     /**< name of the Right Arm hand tracker port */
        std::string m_sRightFingersTrackerPortName; /**< name of the Right Arm fingers tracker port */
        std::string m_sRightArmControlName;         /**< Right Arm hand control name */
        std::string m_sRightFingersControlName;     /**< Right Arm fingers control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oRightArmTrackerPort; /**< Right Arm yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oRightHandTrackerPort; /**< Right Hand yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oRightFingersTrackerPort; /**< Right Fingers yarp tracker port  */
        bool m_bRightArmCapture;

        bool m_bFastrakCalibrated;
        yarp::sig::Vector m_vFastrakOffsets;

        // Array for nao's joints
        AL::ALValue m_aHeadAngles;
        AL::ALValue m_aTorsoAngles;
        AL::ALValue m_aLArmAngles;
        AL::ALValue m_aRArmAngles;
        AL::ALValue m_aLLegAngles;
        AL::ALValue m_aRLegAngles;


        ALMotionProxy *m_oRobotMotionProxy;


};

#endif


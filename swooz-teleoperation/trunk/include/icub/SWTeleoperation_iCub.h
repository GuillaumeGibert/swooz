
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
#include "icub/SWiCubFaceMotion.h"

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

        bool m_bIsRunning;                      /**<  Whether the thread is running */

        int m_i32Fps;                           /**< fps (define the period for calling updateModule) */
        int m_i32TimeoutReset;                  /**< timeout reset icub */


        bool m_bFastrakCalibrated;
        yarp::sig::Vector m_vFastrakOffsets;

        bool m_bFirstLEDCommand;    /**< ... */
        swIcub::SWIcubFaceLEDCommand m_oIcubFaceLEDCmd; /**< ... */

        // timeout
        int m_i32HeadTimeLastBottle;            /**< time elapsed without head bottle command */
        int m_i32FaceTimeLastBottle;            /**< time elapsed without face bottle command */
        int m_i32ArmsTimeLastBottle;            /**< time elapsed without arms bottle command */
        int m_i32GazeTimeLastBottle;            /**< time elapsed without gaze bottle command */

        // reset
        int m_i32HeadTimeoutReset;              /**< head timeout reset icub */
        int m_i32FaceTimeoutReset;              /**< face timeout reset icub */
        int m_i32ArmsTimeoutReset;              /**< arms timeout reset icub */
        int m_i32GazeTimeoutReset;              /**< gaze timeout reset icub */

        // velocity / acceleration
        int m_i32JointVelocityValue;            /**< iCub velocity value */
        int m_i32JointAccelerationValue;        /**< iCub accelereation value */

        // min/max joints
        //  head
        int m_i32HeadMaxValueJoint0;            /**< ... */
        int m_i32HeadMinValueJoint0;            /**< ... */
        int m_i32HeadMaxValueJoint1;            /**< ... */
        int m_i32HeadMinValueJoint1;            /**< ... */
        int m_i32HeadMaxValueJoint2;            /**< ... */
        int m_i32HeadMinValueJoint2;            /**< ... */
        //  eylids
        int m_i32MinValueEyelids;               /**< ... */
        int m_i32MaxValueEyelids;               /**< ... */
        //  torso
        int m_i32TorsoMinValueJoint0;
        int m_i32TorsoMaxValueJoint0;
        int m_i32TorsoMinValueJoint1;
        int m_i32TorsoMaxValueJoint1;
        int m_i32TorsoMinValueJoint2;
        int m_i32TorsoMaxValueJoint2;
        //  left arm
        int m_i32LeftArmMinValueJoint0;
        int m_i32LeftArmMinValueJoint1;
        int m_i32LeftArmMinValueJoint2;
        int m_i32LeftArmMinValueJoint3;
        int m_i32LeftArmMinValueJoint4;
        int m_i32LeftArmMinValueJoint5;
        int m_i32LeftArmMinValueJoint6;
        int m_i32LeftArmMinValueJoint7;
        int m_i32LeftArmMinValueJoint8;
        int m_i32LeftArmMinValueJoint9;
        int m_i32LeftArmMinValueJoint10;
        int m_i32LeftArmMinValueJoint11;
        int m_i32LeftArmMinValueJoint12;
        int m_i32LeftArmMinValueJoint13;
        int m_i32LeftArmMinValueJoint14;
        int m_i32LeftArmMinValueJoint15;
        int m_i32LeftArmMaxValueJoint0;
        int m_i32LeftArmMaxValueJoint1;
        int m_i32LeftArmMaxValueJoint2;
        int m_i32LeftArmMaxValueJoint3;
        int m_i32LeftArmMaxValueJoint4;
        int m_i32LeftArmMaxValueJoint5;
        int m_i32LeftArmMaxValueJoint6;
        int m_i32LeftArmMaxValueJoint7;
        int m_i32LeftArmMaxValueJoint8;
        int m_i32LeftArmMaxValueJoint9;
        int m_i32LeftArmMaxValueJoint10;
        int m_i32LeftArmMaxValueJoint11;
        int m_i32LeftArmMaxValueJoint12;
        int m_i32LeftArmMaxValueJoint13;
        int m_i32LeftArmMaxValueJoint14;
        int m_i32LeftArmMaxValueJoint15;
        //  right arm
        int m_i32RightArmMinValueJoint0;
        int m_i32RightArmMinValueJoint1;
        int m_i32RightArmMinValueJoint2;
        int m_i32RightArmMinValueJoint3;
        int m_i32RightArmMinValueJoint4;
        int m_i32RightArmMinValueJoint5;
        int m_i32RightArmMinValueJoint6;
        int m_i32RightArmMinValueJoint7;
        int m_i32RightArmMinValueJoint8;
        int m_i32RightArmMinValueJoint9;
        int m_i32RightArmMinValueJoint10;
        int m_i32RightArmMinValueJoint11;
        int m_i32RightArmMinValueJoint12;
        int m_i32RightArmMinValueJoint13;
        int m_i32RightArmMinValueJoint14;
        int m_i32RightArmMinValueJoint15;
        int m_i32RightArmMaxValueJoint0;
        int m_i32RightArmMaxValueJoint1;
        int m_i32RightArmMaxValueJoint2;
        int m_i32RightArmMaxValueJoint3;
        int m_i32RightArmMaxValueJoint4;
        int m_i32RightArmMaxValueJoint5;
        int m_i32RightArmMaxValueJoint6;
        int m_i32RightArmMaxValueJoint7;
        int m_i32RightArmMaxValueJoint8;
        int m_i32RightArmMaxValueJoint9;
        int m_i32RightArmMaxValueJoint10;
        int m_i32RightArmMaxValueJoint11;
        int m_i32RightArmMaxValueJoint12;
        int m_i32RightArmMaxValueJoint13;
        int m_i32RightArmMaxValueJoint14;
        int m_i32RightArmMaxValueJoint15;

        // Config variables retrieved from the ini file
        std::string m_sModuleName;              /**< name of the mondule (config) */
        std::string m_sRobotName;               /**< name of the robot (config) */

        //  head
        std::string m_sHeadTrackerPortName;     /**< name of the head tracker port */
        std::string m_sHeadLocalPortName;       /**< name of the head local port */
        std::string m_sHeadRemotePortName;      /**< name of the head remote port */
        std::string m_sHeadControlName;         /**< control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oHeadTrackerPort; /**< head yarp tracker port  */

        //  face
        std::string m_sFaceTrackerPortName;     /**< name of the Face tracker port */
        std::string m_sFaceLocalPortName;       /**< name of the Face local port */
        std::string m_sFaceRemotePortName;      /**< name of the Face remote port */
        std::string m_sFaceControlName;         /**< control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oFaceTrackerPort; /**< Face yarp tracker port */

        //  eyelid
        std::string m_sEyelidInputPortName;     /**< name of the eyelid input port */
        std::string m_sEyelidOutputPortName;    /**< name of the eyelid output port */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oFaceHandlerPort; /**< Face handler port */

        //  gaze
        std::string m_sGazeTrackerPortName;     /**< name of the Gaze tracker port */
        std::string m_sGazeLocalPortName;       /**< name of the Gaze local port */
        std::string m_sGazeRemotePortName;      /**< name of the Gaze remote port */
        std::string m_sGazeControlName;         /**< Gaze control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oGazeTrackerPort; /**< gaze yarp tracker port  */

        //  torso
        std::string m_sTorsoTrackerPortName;        /**< name of the torso tracker port */
        std::string m_sTorsoLocalPortName;          /**< name of the torso local port */
        std::string m_sTorsoRemotePortName;         /**< name of the torso remote port */
        std::string m_sTorsoControlName;            /**< control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oTorsoTrackerPort; /**< torso yarp tracker port*/
        bool m_bTorsoCapture;                       /**< ... */

        //  left arm
        std::string m_sLeftArmTrackerPortName;      /**< name of the Left Arm arm tracker port */
        std::string m_sLeftHandTrackerPortName;     /**< name of the Left Arm hand tracker port */
        std::string m_sLeftFingersTrackerPortName;  /**< name of the Left Arm fingers tracker port */
        std::string m_sLeftArmControlName;          /**< Left Arm hand control name */
        std::string m_sLeftFingersControlName;      /**< Left Arm fingers control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oLeftArmTrackerPort;     /**< Left Arm yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oLeftHandTrackerPort;    /**< Left Hand yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oLeftFingersTrackerPort; /**< Left Fingers yarp tracker port  */
        bool m_bLeftArmCapture;                     /**< ... */

        //  right arm
        std::string m_sRightArmTrackerPortName;     /**< name of the Right Arm arm tracker port */
        std::string m_sRightHandTrackerPortName;    /**< name of the Right Arm hand tracker port */
        std::string m_sRightFingersTrackerPortName; /**< name of the Right Arm fingers tracker port */
        std::string m_sRightArmControlName;         /**< Right Arm hand control name */
        std::string m_sRightFingersControlName;     /**< Right Arm fingers control name */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oRightArmTrackerPort;    /**< Right Arm yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oRightHandTrackerPort;   /**< Right Hand yarp tracker port  */
        yarp::os::BufferedPort<yarp::os::Bottle> m_oRightFingersTrackerPort;/**< Right Fingers yarp tracker port  */
        bool m_bRightArmCapture;                    /**< ... */

        // head
        yarp::dev::PolyDriver        m_oRobotHead;      /**< ... */
        yarp::dev::IEncoders        *m_pIHeadEncoders;  /**< ... */
        yarp::dev::IPositionControl *m_pIHeadPosition;  /**< ... */
        yarp::dev::IVelocityControl *m_pIHeadVelocity;  /**< ... */

        // torso
        yarp::dev::PolyDriver        m_oRobotTorso;     /**< ... */
        yarp::dev::IEncoders        *m_pITorsoEncoders; /**< ... */
        yarp::dev::IPositionControl *m_pITorsoPosition; /**< ... */
        yarp::dev::IVelocityControl *m_pITorsoVelocity; /**< ... */
        std::vector<double>			 m_vTorsoJoints;    /**< ... */


        // left arm
        yarp::dev::PolyDriver         m_oRobotLeftArm;          /**< Left Arm Polydriver  for Cartesian Controller*/
        yarp::dev::PolyDriver         m_oRobotLeftArmCartesian; /**< Left Arm Polydriver  */
        yarp::dev::IEncoders		 *m_pILeftArmEncoders;      /**< Left Arm Encoders  */
        yarp::dev::IVelocityControl  *m_pILeftArmVelocity;      /**< Left Arm Velocity control  */
        yarp::dev::IPositionControl	 *m_pILeftArmPosition;      /**< Left Arm Position control  */
        yarp::dev::ICartesianControl *m_pILeftArmCartesian;     /**< Left Arm Cartesian Controller  */
        std::vector<double>			  m_vLeftArmJoints;

        // Right arm
        yarp::dev::PolyDriver         m_oRobotRightArm;         /**< Right Arm Polydriver  for Cartesian Controller*/
        yarp::dev::PolyDriver         m_oRobotRightArmCartesian;/**< Right Arm Polydriver  */
        yarp::dev::IEncoders		 *m_pIRightArmEncoders;     /**< Right Arm Encoders  */
        yarp::dev::IVelocityControl  *m_pIRightArmVelocity;     /**< Right Arm Velocity control  */
        yarp::dev::IPositionControl	 *m_pIRightArmPosition;     /**< Right Arm Position control  */
        yarp::dev::ICartesianControl *m_pIRightArmCartesian;    /**< Right Arm Cartesian Controller  */
        std::vector<double>			  m_vRightArmJoints;        /**< ... */

        // options
        yarp::os::Property m_oHeadOptions;              /**< robot interfaces for head/gaze movements */
        yarp::os::Property m_oTorsoOptions;             /**< robot interfaces for torso movements */
        yarp::os::Property m_oLeftArmCartesianOptions;  /**< robot interfaces for left hand cartesian position */
        yarp::os::Property m_oLeftArmOptions;           /**< robot interfaces for left arm/hand movements */
        yarp::os::Property m_oRightArmCartesianOptions; /**< robot interfaces for Right hand cartesian position */
        yarp::os::Property m_oRightArmOptions;          /**< robot interfaces for Right arm/hand movements */
};

#endif



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

        int m_i32Fps;                           /**< fps (define the period for calling updateModule) */
        int m_i32HeadTimeLastBottle;            /**< time elapsed without head bottle command */
        int m_i32HeadTimeoutReset;              /**< head timeout reset nao */
        double m_dJointVelocityValue;            /**< ano velocity value */

        // Array for nao's joints
        AL::ALValue m_aHeadAngles;
        AL::ALValue m_aTorsoAngles;
        AL::ALValue m_aLArmAngles;
        AL::ALValue m_aRArmAngles;
        AL::ALValue m_aLLegAngles;
        AL::ALValue m_aRLegAngles;





        // ############
        std::vector<float> m_vRightLegJointMin;
        std::vector<float> m_vRightLegJointMax;

        std::vector<float> m_vLeftLegJointMin;
        std::vector<float> m_vLeftLegJointMax;

        // ############

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




        ALMotionProxy *m_oRobotMotionProxy;


};

#endif


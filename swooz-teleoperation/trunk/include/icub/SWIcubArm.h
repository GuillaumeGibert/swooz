
/**
 * \file SWIcubArm.h
 * \author Florian Lance
 * \date 29-04-2014
 * \brief Defines SWIcubArm class.
 */

#ifndef _SWICUBARM_
#define _SWICUBARM_

// STD
#include <vector>
#include <string>

// SWOOZ
#include "commonTypes.h"

// YARP
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>


#include <yarp/sig/Vector.h>


#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/Mutex.h>

#include <yarp/os/Time.h>


namespace swTeleop
{

    /**
     * @brief The SWArmVelocityController class
     */
    class SWArmVelocityController : public yarp::os::RateThread
    {
        public :

            /**
             * @brief SWArmVelocityController
             * @param pIArmEncoders
             * @param pIArmVelocity
             * @param vArmJointVelocityK
             * @param dVelocityToleranceArm
             * @param i32Rate
             */
            SWArmVelocityController(yarp::dev::IEncoders *pIArmEncoders, yarp::dev::IVelocityControl *pIArmVelocity,
                                 std::vector<double> &vArmJointVelocityK, double dVelocityToleranceArm = 15.,int i32Rate = 10);

            /**
             * @brief run
             */
            void run();

            /**
             * @brief setNewCommand
             * @param vArmCommand
             */
            void setJoints(const yarp::sig::Vector &vJoints);

            /**
             * @brief enableArm
             * @param bActivated
             */
            void enableArm(cbool bActivated);


        private :

            bool m_bArmEnabled;
            double m_dVelocityToleranceArm;                /**< ... */

            yarp::os::Mutex m_oMutex;                   /**< ... */
            yarp::dev::IEncoders *m_pIArmEncoders;         /**< ... */
            yarp::dev::IVelocityControl *m_pIArmVelocity;  /**< ... */
            yarp::sig::Vector m_vLastArmJoint;             /**< ... */

            std::vector<double> m_vArmJointVelocityK;      /**< ... */
    };

    /**
     * \class SWIcubArm
     * \author Florian Lance
     * \date 05-12-2013
     * \brief A class for initializing and controlling one iCub arm using swooz-tracking bottles.
     */
    class SWIcubArm
    {
        public:

            /**
             * \brief SWIcubArm constructor
             */
            SWIcubArm();

            /**
             * \brief SWIcubArm destructor
             */
            ~SWIcubArm();

            /**
             * @brief init
             * @param oRf
             * @param bLeftArm
             * @return
             */
            bool init( yarp::os::ResourceFinder &oRf, bool bLeftArm = true);

            /**
             * @brief checkBottles
             * @return
             */
            bool checkBottles();

            /**
             * @brief resetArmPosition
             * @return
             */
            void resetArmPosition();

            /**
             * @brief close
             * @return true if the closing step was ok
             */
            bool close();

            /**
             * @brief interruptModule
             * @return
             */
            bool interruptModule();


        private :


            bool m_bInitialized;    /**< .... */
            bool m_bIsRunning;      /**< ... */

            int m_i32ArmJointsNb;  /**< ... */

            double m_dArmTimeLastBottle;/**< ... */

            // Config variables retrieved from the ini file
            bool m_bArmActivated;  /**< .... */
            int m_bArmActivatedDefault;  /**< .... */

            int m_i32TimeoutArmReset;  /**< ... */
            int m_i32TimeoutArmResetDefault;  /**< ... */

            double m_dVelocityToleranceArm;        /**< ... */
            double m_dVelocityToleranceArmDefault; /**< ... */

            std::vector<double> m_vArmMinJoint;            /**< ... */
            std::vector<double> m_vArmMaxJoint;            /**< ... */
            std::vector<double> m_vArmMinJointDefault;     /**< ... */
            std::vector<double> m_vArmMaxJointDefault;     /**< ... */

            std::vector<double> m_vArmResetPosition;        /**< ... */
            std::vector<double> m_vArmResetPositionDefault; /**< ... */

            std::vector<double> m_vArmJointVelocityAcceleration;       /**< ... */
            std::vector<double> m_vArmJointPositionAcceleration;       /**< ... */
            std::vector<double> m_vArmJointPositionSpeed;              /**< ... */
            std::vector<double> m_vArmJointVelocityAccelerationDefault;/**< ... */
            std::vector<double> m_vArmJointPositionAccelerationDefault;/**< ... */
            std::vector<double> m_vArmJointPositionSpeedDefault;       /**< ... */

            std::vector<double> m_vArmJointVelocityK;         /**< ... */
            std::vector<double> m_vArmJointVelocityKDefault;  /**< ... */

            std::string m_sModuleName;              /**< name of the mondule (config) */
            std::string m_sRobotName;               /**< name of the robot (config) */


            // arm control
            yarp::os::Property m_oArmOptions;                   /**< robot interfaces properties */
            yarp::os::Property m_oArmCartesianOptions;          /**< robot cartesan interfaces properties */
            yarp::dev::PolyDriver        m_oRobotArm;           /**< robot arm controller */
            yarp::dev::PolyDriver        m_oRobotArmCartesian;  /**< robot arm cartesian controller */
            yarp::dev::IEncoders        *m_pIArmEncoders;       /**< ... */
            yarp::dev::IPositionControl *m_pIArmPosition;       /**< ... */
            yarp::dev::IVelocityControl *m_pIArmVelocity;       /**< ... */

            // yarp ports / bottles
            //  arm
            std::string m_sArmLocalPortName;       /**< name of the arm local port */
            std::string m_sArmRemotePortName;      /**< name of the arm remote port */
            std::string m_sArmControlName;         /**< control name */

            std::string m_sArmTrackerPortName;      /**< name of the  Arm arm tracker port */
            std::string m_sHandTrackerPortName;     /**< name of the  Arm hand tracker port */
            std::string m_sFingersTrackerPortName;  /**< name of the  Arm fingers tracker port */
            yarp::os::BufferedPort<yarp::os::Bottle> m_oArmTrackerPort;     /**< Arm yarp tracker port  */
            yarp::os::BufferedPort<yarp::os::Bottle> m_oHandTrackerPort;    /**< Hand yarp tracker port  */
            yarp::os::BufferedPort<yarp::os::Bottle> m_oFingersTrackerPort; /**< Fingers yarp tracker port  */

            SWArmVelocityController *m_pVelocityController;    /**< ... */

            std::string m_sArm;         /**< ... */
    };
}


#endif
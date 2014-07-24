
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
#include <yarp/dev/CartesianControl.h>

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
             * @param i32Rate
             */
            SWArmVelocityController(yarp::dev::IEncoders *pIArmEncoders, yarp::dev::IVelocityControl *pIArmVelocity,
                                 std::vector<double> &vArmJointVelocityK,int i32Rate = 10);

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
             * @brief enable arm parts
             * @param bArmHandActivated
             * @param bFingersActivated
             */
            void enable(cbool bArmHandActivated, cbool bFingersActivated);


        private :

            bool m_bArmHandEnabled; /**< ... */
            bool m_bFingersEnabled; /**< ... */

            yarp::os::Mutex m_oMutex;                      /**< ... */
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

            /**
             * @brief Compute the arm/hand angles values with an input hand yarp bottle
             * @param [in] handBottle    : hand yarp bottle pointer
             * @param [out] vHandAngles  : hand angles array
             */
            void computeHandAngles(yarp::os::Bottle* handBottle,std::vector<double> &vHandAngles);

            /**
             * @brief Compute the finger angles values with an input hand finger yarp bottle
             * @param [in] handFingerBottle : hand fingers  yarp bottle pointer
             * @param [out] vFingerAngles   : finger angles array
             */
            void computeFingerAngles(yarp::os::Bottle* handFingerBottle,std::vector<double> &vFingerAngles);

            bool m_bInitialized;                /**< is the module initialized */
            bool m_bIsRunning;                  /**< is the module running */

            int m_i32ArmJointsNb;               /**< arm joint number */

            // Config variables retrieved from the ini file
            bool m_bArmHandActivated;           /**< is arm/hand activated value */
            bool m_bFingersActivated;           /**< is fingers activated value */
            int m_i32TimeoutArmReset;           /**< timeout arm value  */

            bool m_bFingersActivatedDefault;    /**< is fingers activated DEFAULT value */
            bool m_bArmHandActivatedDefault;    /**< is arm/hand activated DEFAULT value */
            int m_i32TimeoutArmResetDefault;    /**< timeout arm DEFAULT value */

            int m_i32RateVelocityControl;   /**< ... */
            int m_i32RateVelocityControlDefault; /**< ... */

            std::vector<double> m_vArmMinJoint;                         /**< arm minimum joint values array */
            std::vector<double> m_vArmMaxJoint;                         /**< arm maximum joint values array  */
            std::vector<double> m_vArmResetPosition;                    /**< arm reset positions values values array */
            std::vector<double> m_vArmJointVelocityAcceleration;        /**< arm join velocity control acceleration values array */
            std::vector<double> m_vArmJointPositionAcceleration;        /**< arm joint position control acceleration values array  */
            std::vector<double> m_vArmJointPositionSpeed;               /**< arm joint position control speed values array  */
            std::vector<double> m_vArmJointVelocityK;                   /**< arm joint velocity K values array */

            std::vector<double> m_vArmMinJointDefault;                  /**< arm minimum joint DEFAULT values array */
            std::vector<double> m_vArmMaxJointDefault;                  /**< arm maximum joint DEFAULT values array  */
            std::vector<double> m_vArmResetPositionDefault;             /**< arm reset positions DEFAULT values array */
            std::vector<double> m_vArmJointVelocityAccelerationDefault; /**< arm join velocity control acceleration DEFAULT values array */
            std::vector<double> m_vArmJointPositionAccelerationDefault; /**< arm joint position control acceleration DEFAULT values array  */
            std::vector<double> m_vArmJointPositionSpeedDefault;        /**< arm joint position control speed DEFAULT values array  */
            std::vector<double> m_vArmJointVelocityKDefault;            /**< arm joint velocity K DEFAULT values array */

            std::string m_sModuleName;                                              /**< name of the mondule (config) */
            std::string m_sRobotName;                                               /**< name of the robot (config) */

            // icub arm options
            std::string m_sArmLocalPortName;                                        /**< name of the arm local port */
            std::string m_sArmRemotePortName;                                       /**< name of the arm remote port */
            std::string m_sArmControlName;                                          /**< control name */
            yarp::os::Property m_oArmOptions;                                       /**< robot interfaces properties */

            // icub control
            yarp::dev::IEncoders        *m_pIArmEncoders;                           /**< arm encoder pointer */
            yarp::dev::IPositionControl *m_pIArmPosition;                           /**< arm position control pointer */
            yarp::dev::IVelocityControl *m_pIArmVelocity;                           /**< arm velocity control pointer */
            yarp::dev::PolyDriver        m_oRobotArm;                               /**< robot arm controller */

            // yarp ports / bottles
            //  arm
            std::string m_sHandTrackerPortName;                                     /**< name of the hand tracker port */
            std::string m_sHandFingersTrackerPortName;                              /**< name of the hand fingers tracker port */
            yarp::os::BufferedPort<yarp::os::Bottle> m_oHandTrackerPort;            /**< Hand yarp tracker port  */
            yarp::os::BufferedPort<yarp::os::Bottle> m_oHandFingersTrackerPort;     /**< Hand fingers yarp tracker port  */



            SWArmVelocityController *m_pVelocityController;                         /**< velocity controller class pointer */

            std::string m_sArm;                                                     /**< indicates if left or right arm */
    };
}


#endif


/**
 * \file SWIcubTorso.h
 * \author Florian Lance
 * \date 29-04-2014
 * \brief Defines SWIcubTorso class.
 */

#ifndef _SWICUBTORSO_
#define _SWICUBTORSO_

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

//#include <yarp/math/Math.h>

//#include <yarp/dev/Drivers.h>
//#include <yarp/dev/CartesianControl.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/Mutex.h>

#include <yarp/os/Time.h>


namespace swTeleop
{
    /**
     * @brief The SWTorsoVelocityController class
     */
    class SWTorsoVelocityController : public yarp::os::RateThread
    {
        public :

            /**
             * @brief SWVelocityController
             * @param pITorsoEncoders
             * @param pITorsoVelocity
             * @param vTorsoJointVelocityK
             * @param dVelocityTolerance
             * @param i32Rate
             */
            SWTorsoVelocityController(yarp::dev::IEncoders *pITorsoEncoders, yarp::dev::IVelocityControl *pITorsoVelocity,
                                 std::vector<double> &vTorsoJointVelocityK, double dVelocityTolerance = 15., int i32Rate = 10);

            /**
             * @brief run
             */
            void run();

            /**
             * @brief setNewCommand
             * @param vHeadCommand
             */
            void setJoints(const yarp::sig::Vector &vJoints);

            /**
             * @brief enableTorso
             * @param bActivated
             */
            void enableTorso(cbool bActivated);

        private :

            bool m_bTorsoEnabled;

            double m_dVelocityTolerance;                    /**< ... */
            yarp::os::Mutex m_oMutex;                       /**< ... */
            yarp::dev::IEncoders *m_pITorsoEncoders;         /**< ... */
            yarp::dev::IVelocityControl *m_pITorsoVelocity;  /**< ... */
            yarp::sig::Vector m_vLastTorsoJoint;             /**< ... */

            std::vector<double> m_vTorsoJointVelocityK;      /**< ... */
    };

    /**
     * \class SWIcubTorso
     * \author Florian Lance
     * \date 05-12-2013
     * \brief A class for initializing and controlling the iCub torso using swooz-tracking bottles.
     */
    class SWIcubTorso
    {
        public:

            /**
             * \brief SWIcubTorso constructor
             */
            SWIcubTorso();

            /**
             * \brief SWIcubTorso destructor
             */
            ~SWIcubTorso();

            /**
             * @brief init
             * @param [in] rf : the resource finder
             * @return
             */
            bool init( yarp::os::ResourceFinder &oRf);

            /**
             * @brief checkBottles
             * @return
             */
            bool checkBottles();

            /**
             * @brief resetTorsoPosition
             * @return
             */
            void resetTorsoPosition();

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

            int m_i32TorsoJointsNb;  /**< ... */

            double m_dTorsoTimeLastBottle; /**< ... */

            // Config variables retrieved from the ini file
            bool m_bTorsoActivated;  /**< .... */
            int m_bTorsoActivatedDefault;  /**< .... */

            int m_i32TimeoutTorsoReset;  /**< ... */
            int m_i32TimeoutTorsoResetDefault;  /**< ... */

            double m_dVelocityToleranceTorso;        /**< ... */
            double m_dVelocityToleranceTorsoDefault; /**< ... */

            std::vector<double> m_vTorsoMinJoint;            /**< ... */
            std::vector<double> m_vTorsoMaxJoint;            /**< ... */
            std::vector<double> m_vTorsoMinJointDefault;     /**< ... */
            std::vector<double> m_vTorsoMaxJointDefault;     /**< ... */

            std::vector<double> m_vTorsoJointVelocityAcceleration;       /**< ... */
            std::vector<double> m_vTorsoJointPositionAcceleration;       /**< ... */
            std::vector<double> m_vTorsoJointPositionSpeed;              /**< ... */
            std::vector<double> m_vTorsoJointVelocityAccelerationDefault;/**< ... */
            std::vector<double> m_vTorsoJointPositionAccelerationDefault;/**< ... */
            std::vector<double> m_vTorsoJointPositionSpeedDefault;       /**< ... */

            std::vector<double> m_vTorsoJointVelocityK;         /**< ... */
            std::vector<double> m_vTorsoJointVelocityKDefault;  /**< ... */


            std::string m_sModuleName;              /**< name of the mondule (config) */
            std::string m_sRobotName;               /**< name of the robot (config) */

            // yarp ports / bottles
            //  torso
            std::string m_sTorsoTrackerPortName;        /**< name of the torso tracker port */
            std::string m_sTorsoLocalPortName;          /**< name of the torso local port */
            std::string m_sTorsoRemotePortName;         /**< name of the torso remote port */
            std::string m_sTorsoControlName;            /**< control name */
            yarp::os::BufferedPort<yarp::os::Bottle> m_oTorsoTrackerPort; /**< torso yarp tracker port*/

            // torso control
            yarp::os::Property m_oTorsoOptions;             /**< robot interfaces for torso movements */
            yarp::dev::PolyDriver        m_oRobotTorso;     /**< ... */
            yarp::dev::IEncoders        *m_pITorsoEncoders; /**< ... */
            yarp::dev::IPositionControl *m_pITorsoPosition; /**< ... */
            yarp::dev::IVelocityControl *m_pITorsoVelocity; /**< ... */


            SWTorsoVelocityController *m_pVelocityController;    /**< ... */
    };
}


#endif

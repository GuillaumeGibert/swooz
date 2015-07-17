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
 * \file SWIcubHead.h
 * \author Florian Lance
 * \date 23-04-2014
 * \brief Defines SWIcubHead class.
 */

#ifndef _SWICUBHEAD_
#define _SWICUBHEAD_

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
#include <yarp/dev/IControlMode2.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/Mutex.h>

#include <yarp/os/Time.h>


namespace swTeleop
{
    /**
     * @brief The SWIcubFaceLabLEDCommand class
     */
    class SWIcubFaceLabLEDCommand
    {
        public :

            /**
             * @brief SWIcubLEDCommand
             */
            SWIcubFaceLabLEDCommand();


            /**
             * @brief leftEyeBrowCommand
             * @param a3DLeftEyeBrow
             * @return
             */
            std::string leftEyeBrowCommand(const std::vector<double> &a3DLeftEyeBrow);

            /**
             * @brief rightEyeBrowCommand
             * @param a3DRightEyeBrow
             * @return
             */
            std::string rightEyeBrowCommand(const std::vector<double> &a3DRightEyeBrow);

            /**
             * @brief swTeleop::SWVelocityController::eyeBrowCommand
             * @param a3DEyeBrow
             * @param bLeftEye
             * @return
             */
            std::string eyeBrowCommand(const std::vector<double> &a3DEyeBrow, const bool bLeftEye);


            /**
             * @brief lipCommand
             * @param a3DLip1
             * @param a3DLip2
             * @return
             */
            std::string lipCommand(const std::vector<double> &a3DLip1, const std::vector<double> &a3DLip2);

        private :

            double m_dLeftThresholdDown;    /**< ... */
            double m_dLeftThresholdNeutral; /**< ... */
            double m_dLeftThresholdUp1;     /**< ... */
            double m_dLeftThresholdUp2;     /**< ... */

            double m_dRightThresholdDown;   /**< ... */
            double m_dRightThresholdNeutral;/**< ... */
            double m_dRightThresholdUp1;    /**< ... */
            double m_dRightThresholdUp2;    /**< ... */

            double m_dThresholdMouth;       /**< ... */
    };

    /**
     * @brief The SWVelocityController class
     */
    class SWHeadVelocityController : public yarp::os::RateThread
    {
        public :

            /**
             * @brief SWVelocityController
             * @param pIHeadEncoders
             * @param pIHeadVelocity
             * @param vHeadJointVelocityK
             * @param i32Rate
             */
            SWHeadVelocityController(yarp::dev::IEncoders *pIHeadEncoders, yarp::dev::IVelocityControl *pIHeadVelocity,  yarp::dev::IControlMode2 *pIHeadControlMode,
                                 std::vector<double> &vHeadJointVelocityK, int i32Rate = 10);

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
             * @brief enableHead
             * @param bActivated
             */
            void enableHead(cbool bActivated);

            /**
             * @brief enableGaze
             * @param bActivated
             */
            void enableGaze(cbool bActivated);

            /**
             * @brief setMinMaxJoints
             * @param vMinJoints
             * @param vMaxJoints
             */
            void setMinMaxJoints(const std::vector<double> &vMinJoints, const std::vector<double> &vMaxJoints);


        private :

            bool m_bGazeEnabled;
            bool m_bHeadEnabled;

            yarp::os::Mutex m_oMutex;                       /**< ... */
            yarp::dev::IEncoders *m_pIHeadEncoders;         /**< ... */
            yarp::dev::IVelocityControl *m_pIHeadVelocity;  /**< ... */
		  yarp::dev::IControlMode2    *m_pIHeadControlMode;
            yarp::sig::Vector m_vLastHeadJoint;             /**< ... */

            std::vector<double> m_vHeadJointVelocityK;      /**< ... */
            std::vector<double> m_vMinJoints;
            std::vector<double> m_vMaxJoints;
    };

    /**
     * \class SWIcubHead
     * \author Florian Lance
     * \date 05-12-2013
     * \brief A class for initializing and controlling the iCub head using swooz-tracking bottles.
     */
    class SWIcubHead
    {
        public:

            /**
             * \brief SWIcubHead constructor
             */
            SWIcubHead();

            /**
             * \brief SWIcubHead destructor
             */
            ~SWIcubHead();

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
             * @brief resetHeadPosition
             * @return
             */
            void resetHeadPosition();

            /**
             * @brief resetGazePosition
             */
            void resetGazePosition();

            /**
             * @brief resetLEDS
             */
            void resetLEDS();

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
             * \brief The eyesOpeningCode function transforms a percentage of opening eyelids
             * into a hexa code ready to send to the iCub.
             *
             * This function transforms a command in percentage of eyelid opening form into a hexa code
             * that can be understood by the iCub.
             * \param dEyeLids  : the percentage of opening/closing eyelids
             * \param dMinValue : the min value accepted by the iCub
             * \param dMaxValue : the max value accepted by the iCub
             * \return a string containing the hexa code
             */
            std::string swTeleop::SWIcubHead::eyesOpeningCode(cdouble dEyeLids, cdouble dMinValue, cdouble dMaxValue);


            bool m_bInitialized;    /**< .... */
            bool m_bIsRunning;      /**< ... */

            int m_i32HeadJointsNb;  /**< ... */

            double m_dHeadTimeLastBottle;/**< ... */
            double m_dGazeTimeLastBottle;/**< ... */
            double m_dLEDTimeLastBottle; /**< ... */

            // Config variables retrieved from the ini file
            bool m_bHeadActivated;  /**< .... */            
            bool m_bGazeActivated;  /**< .... */            
            bool m_bLEDActivated;   /**< .... */
            int m_bHeadActivatedDefault;  /**< .... */
            int m_bGazeActivatedDefault;  /**< .... */
            int m_bLEDActivatedDefault;   /**< .... */

            int m_i32RateVelocityControl;   /**< ... */
            int m_i32RateVelocityControlDefault; /**< ... */

            int m_i32TimeoutHeadReset;  /**< ... */
            int m_i32TimeoutGazeReset;  /**< ... */
            int m_i32TimeoutLEDReset;   /**< ... */            
            int m_i32TimeoutHeadResetDefault;  /**< ... */
            int m_i32TimeoutGazeResetDefault;  /**< ... */
            int m_i32TimeoutLEDResetDefault;   /**< ... */

            double m_dMinEyelids;       /**< ... */
            double m_dMaxEyelids;       /**< ... */
            double m_dMinEyelidsDefault;    /**< ... */
            double m_dMaxEyelidsDefault;    /**< ... */
            double m_dMinEyelidsSimDefault; /**< ... */
            double m_dMaxEyelidsSimDefault; /**< ... */

            std::vector<double> m_vHeadMinJoint;                        /**< ... */
            std::vector<double> m_vHeadMaxJoint;                        /**< ... */
            std::vector<double> m_vHeadResetPosition;                   /**< ... */
            std::vector<double> m_vHeadJointVelocityAcceleration;       /**< ... */
            std::vector<double> m_vHeadJointPositionAcceleration;       /**< ... */
            std::vector<double> m_vHeadJointPositionSpeed;              /**< ... */
            std::vector<double> m_vHeadJointVelocityK;                  /**< ... */

            std::vector<double> m_vHeadMinJointDefault;                 /**< ... */
            std::vector<double> m_vHeadMaxJointDefault;                 /**< ... */
            std::vector<double> m_vHeadResetPositionDefault;            /**< ... */
            std::vector<double> m_vHeadJointVelocityAccelerationDefault;/**< ... */
            std::vector<double> m_vHeadJointPositionAccelerationDefault;/**< ... */
            std::vector<double> m_vHeadJointPositionSpeedDefault;       /**< ... */
            std::vector<double> m_vHeadJointVelocityKDefault;           /**< ... */

            std::string m_sModuleName;              /**< name of the mondule (config) */
            std::string m_sRobotName;               /**< name of the robot (config) */

            // yarp ports / bottles
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

            // head control
            yarp::os::Property m_oHeadOptions;              /**< robot interfaces for head/gaze movements */
            yarp::dev::PolyDriver        m_oRobotHead;      /**< ... */                                    
            yarp::dev::IEncoders        *m_pIHeadEncoders;  /**< ... */
            yarp::dev::IPositionControl *m_pIHeadPosition;  /**< ... */
            yarp::dev::IVelocityControl *m_pIHeadVelocity;  /**< ... */
	    yarp::dev::IControlMode2    *m_pIHeadControlMode;


            SWHeadVelocityController *m_pVelocityController;    /**< ... */
            SWIcubFaceLabLEDCommand m_ICubFaceLabLED;       /**< ... */
    };
}


#endif

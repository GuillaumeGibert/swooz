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
 * \file SWIcubArm.cpp
 * \author Florian Lance
 * \date 29-04-2014
 * \brief Defines SWIcubArm
 */


#include <sstream>

#include "geometryUtility.h"
#include "SWTrackingDevice.h"

#include "icub/SWIcubArm.h"
#include <yarp/math/Math.h>

#include "opencv2/core/core.hpp"
#include "opencvUtility.h"
#include "geometryUtility.h"



swTeleop::SWIcubArm::SWIcubArm() : m_bInitialized(false), m_bIsRunning(false), m_dArmTimeLastBottle(-1.),
                                       m_pIArmVelocity(NULL), m_pIArmEncoders(NULL), m_pIArmPosition(NULL), m_pVelocityController(NULL)
{
    // set ini file defaults values
        // parts to be activated
            m_bArmHandActivatedDefault = 1;
            m_bFingersActivatedDefault = 1;

        // timeout
            m_i32TimeoutArmResetDefault = 3000;

        // accelerations / speeds
            m_i32RateVelocityControlDefault = 100;
            double l_aDMinJointDefault[]                       = {-95., 0., -37., 16., -90., -90., -20., 0., 10., 0., 0., 0., 0., 0., 0., 0.};
            double l_aDMaxJointDefault[]                       = { 10., 160., 80., 106., 90., 0., 40., 60., 90., 90., 180., 90., 180., 90., 180., 270.};
            double l_aDArmResetPosition[]                      = {-25.,20.,0.,50.,0.,0.,0.,60.,20.,20.,20.,10.,10.,10.,10.,10.};
            double l_aDArmJointVelocityDefault[]               = {50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.};
            double l_aDArmJointVelocityKDefault[]              = {0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9};
            double l_aDArmJointPositionAccelerationDefault[]   = {50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.};
            double l_aDArmJointPositionSpeedDefault[]          = {50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.,50.};
            m_vArmMinJointDefault                  = std::vector<double>(l_aDMinJointDefault, l_aDMinJointDefault + sizeof(l_aDMinJointDefault) / sizeof(double));
            m_vArmMaxJointDefault                  = std::vector<double>(l_aDMaxJointDefault, l_aDMaxJointDefault + sizeof(l_aDMaxJointDefault) / sizeof(double));
            m_vArmResetPositionDefault             = std::vector<double>(l_aDArmResetPosition, l_aDArmResetPosition + sizeof(l_aDArmResetPosition) / sizeof(double));
            m_vArmJointVelocityKDefault            = std::vector<double>(l_aDArmJointVelocityKDefault, l_aDArmJointVelocityKDefault + sizeof(l_aDArmJointVelocityKDefault) / sizeof(double));
            m_vArmJointVelocityAccelerationDefault = std::vector<double>(l_aDArmJointVelocityDefault, l_aDArmJointVelocityDefault + sizeof(l_aDArmJointVelocityDefault) / sizeof(double));
            m_vArmJointPositionAccelerationDefault = std::vector<double>(l_aDArmJointPositionAccelerationDefault, l_aDArmJointPositionAccelerationDefault + sizeof(l_aDArmJointPositionAccelerationDefault) / sizeof(double));
            m_vArmJointPositionSpeedDefault        = std::vector<double>(l_aDArmJointPositionSpeedDefault, l_aDArmJointPositionSpeedDefault + sizeof(l_aDArmJointPositionSpeedDefault) / sizeof(double));

            m_vArmMinJoint                    = std::vector<double>(m_vArmMinJointDefault.size());
            m_vArmMaxJoint                    = std::vector<double>(m_vArmMaxJointDefault.size());
            m_vArmResetPosition               = std::vector<double>(m_vArmResetPositionDefault.size());
            m_vArmJointVelocityAcceleration   = std::vector<double>(m_vArmJointVelocityAccelerationDefault.size());
            m_vArmJointVelocityK              = std::vector<double>(m_vArmJointVelocityKDefault.size());
            m_vArmJointPositionAcceleration   = std::vector<double>(m_vArmJointPositionAccelerationDefault.size());
            m_vArmJointPositionSpeed          = std::vector<double>(m_vArmJointPositionSpeedDefault.size());
            m_i32ArmJointsNb = m_vArmMinJoint.size();

}

swTeleop::SWIcubArm::~SWIcubArm()
{
}

bool swTeleop::SWIcubArm::init( yarp::os::ResourceFinder &oRf, bool bLeftArm)
{

    //~ if(m_bInitialized)
    //~ {
        //~ std::cerr << "Icub Arm is already initialized. " << std::endl;
        //~ return false;
    //~ }

    if(bLeftArm)
    {
        m_sArm = "left";
    }
    else
    {
        m_sArm = "right";
    }

    // gets the module name which will form the stem of all module port names
        m_sModuleName   = oRf.check("name", yarp::os::Value("teleoperation_iCub"), "Teleoperation/iCub Module name (string)").asString();
        m_sRobotName    = oRf.check("robot",yarp::os::Value("icubSim"),  "Robot name (string)").asString();

    // robot parts to control
        m_bArmHandActivated = oRf.check(std::string(m_sArm + "ArmHandActivated").c_str(), yarp::os::Value(m_bArmHandActivatedDefault),     std::string(m_sArm + " Arm/hand activated (int)").c_str()).asInt() != 0;
        m_bFingersActivated = oRf.check(std::string(m_sArm + "FingersActivated").c_str(), yarp::os::Value(m_bFingersActivatedDefault), std::string(m_sArm + " Fingers activated (int)").c_str()).asInt() != 0;

        m_i32RateVelocityControl = oRf.check("armsRateVelocityControl", yarp::os::Value(m_i32RateVelocityControlDefault), "Arms rate velocity control (int)").asInt();

        if(!m_bArmHandActivated && !m_bFingersActivated)
        {
            std::cout << m_sArm + " arm/hand and " + m_sArm  +" fingers not activated, icub " + m_sArm + " arm initialization aborted. " << std::endl;
            return (m_bInitialized = false);
        }

    // min / max values for iCub Torso joints
        for(uint ii = 0; ii < m_vArmJointVelocityAcceleration.size(); ++ii)
        {
            std::ostringstream l_os;
            l_os << ii;

            std::string l_sMinJoint(m_sArm + "ArmMinValueJoint" + l_os.str());
            std::string l_sMaxJoint(m_sArm + "ArmMaxValueJoint" + l_os.str());
            std::string l_sArmResetPosition(m_sArm + "ArmResetPosition" + l_os.str());
            std::string l_sArmJointVelocityAcceleration(m_sArm + "ArmJointVelocityAcceleration" + l_os.str());
            std::string l_sArmJointVelocityK(m_sArm + "ArmJointVelocityK" + l_os.str());
            std::string l_sArmJointPositionAcceleration(m_sArm + "ArmJointPositionAcceleration" + l_os.str());
            std::string l_sArmJointPositionSpeed(m_sArm + "ArmJointPositionSpeed" + l_os.str());

            std::string l_sMinJointInfo(m_sArm + " arm minimum joint" + l_os.str() + " Value (double)");
            std::string l_sMaxJointInfo(m_sArm + " arm maximum joint" + l_os.str() + " Value (double)");
            std::string l_sArmResetPositionInfo(m_sArm + " arm reset position " + l_os.str() + " Value (double)");
            std::string l_sArmJointVelocityAccelerationInfo(m_sArm + " arm joint velocity acceleration " + l_os.str() + " Value (double)");
            std::string l_sArmJointVelocityKInfo(m_sArm + " arm joint velocity K coeff"+ l_os.str() + " Value (double)");
            std::string l_sArmJointPositionAccelerationInfo(m_sArm + " arm joint position acceleration " + l_os.str() + " Value (double)");
            std::string l_sArmJointPositionSpeedInfo(m_sArm + " arm joint position speed " + l_os.str() + " Value (double)");

            m_vArmMinJoint[ii] = oRf.check(l_sMinJoint.c_str(), m_vArmMinJointDefault[ii], l_sMinJointInfo.c_str()).asDouble();
            m_vArmMaxJoint[ii] = oRf.check(l_sMaxJoint.c_str(), m_vArmMaxJointDefault[ii], l_sMaxJointInfo.c_str()).asDouble();
            m_vArmResetPosition[ii] = oRf.check(l_sArmResetPosition.c_str(), m_vArmResetPositionDefault[ii], l_sArmResetPositionInfo.c_str()).asDouble();
            m_vArmJointVelocityAcceleration[ii]= oRf.check(l_sArmJointVelocityAcceleration.c_str(), m_vArmJointVelocityAccelerationDefault[ii], l_sArmJointVelocityAccelerationInfo.c_str()).asDouble();
            m_vArmJointPositionAcceleration[ii]= oRf.check(l_sArmJointPositionAcceleration.c_str(), m_vArmJointPositionAccelerationDefault[ii], l_sArmJointPositionAccelerationInfo.c_str()).asDouble();
            m_vArmJointPositionSpeed[ii]       = oRf.check(l_sArmJointPositionSpeed.c_str(),        m_vArmJointPositionSpeedDefault[ii],        l_sArmJointPositionSpeedInfo.c_str()).asDouble();
            m_vArmJointVelocityK[ii]           = oRf.check(l_sArmJointVelocityK.c_str(),            m_vArmJointVelocityKDefault[ii],            l_sArmJointVelocityKInfo.c_str()).asDouble();
        }

    // miscellaneous
        m_i32TimeoutArmReset   = oRf.check(std::string(m_sArm + "ArmTimeoutReset").c_str(),       yarp::os::Value(m_i32TimeoutArmResetDefault), std::string(m_sArm + " arm timeout reset iCub (int)").c_str()).asInt();

    // set polydriver options
        m_oArmOptions.put("robot",     m_sRobotName.c_str());
        m_oArmOptions.put("device",    "remote_controlboard");
        m_oArmOptions.put("local",    ("/local/" + m_sRobotName + "/" + m_sArm + "_arm").c_str());
        m_oArmOptions.put("name",     ("/" + m_sRobotName + "/" + m_sArm + "_arm").c_str());
        m_oArmOptions.put("remote",   ("/" + m_sRobotName + "/" + m_sArm + "_arm").c_str());

    // init polydriver
        m_oRobotArm.open(m_oArmOptions);
        if(!m_oRobotArm.isValid())
        {
            std::cerr << std::endl <<"-ERROR: " << m_sArm << " robotArm is not valid, escape arm initialization. " << std::endl <<std::endl;
            return (m_bInitialized=false);
        }

    // initializing controllers
        if (!m_oRobotArm.view(m_pIArmVelocity) || !m_oRobotArm.view(m_pIArmPosition) || !m_oRobotArm.view(m_pIArmEncoders) ||!m_oRobotArm.view(m_pIArmControlMode) )
        {
            std::cerr << std::endl <<  "-ERROR: " << m_sArm << " while getting required robot Arm interfaces." << std::endl <<std::endl;
            m_oRobotArm.close();
            return (m_bInitialized=false);
        }


    // init ports
        m_sHandTrackerPortName         = "/teleoperation/" + m_sRobotName + "/" + m_sArm + "_arm/hand";
        m_sHandFingersTrackerPortName  = "/teleoperation/" + m_sRobotName + "/" + m_sArm + "_arm/hand_fingers";

    // open ports
        bool l_bPortOpeningSuccess = true;
        if(m_bArmHandActivated || m_bFingersActivated)
        {            
            l_bPortOpeningSuccess = m_oHandFingersTrackerPort.open(m_sHandFingersTrackerPortName.c_str());

            if(l_bPortOpeningSuccess)
                 l_bPortOpeningSuccess = m_oHandTrackerPort.open(m_sHandTrackerPortName.c_str());
        }

        if(!l_bPortOpeningSuccess)
        {
            std::cerr << std::endl <<"-ERROR: Unable to open ports." << std::endl <<std::endl;
            m_oRobotArm.close();
            return (m_bInitialized=false);
        }

    // retrieve Torso number of joints
        m_pIArmPosition->getAxes(&m_i32ArmJointsNb);

    // set accelerations/speeds
        for(int ii = 0; ii < m_i32ArmJointsNb; ++ii)
        {
            m_pIArmPosition->setRefAcceleration(ii, m_vArmJointPositionAcceleration[ii]);
            m_pIArmPosition->setRefSpeed(ii, m_vArmJointPositionSpeed[ii]);
            m_pIArmVelocity->setRefAcceleration(ii, m_vArmJointVelocityAcceleration[ii]);
        }

    // init controller
        m_pVelocityController = new swTeleop::SWArmVelocityController(m_pIArmEncoders, m_pIArmVelocity, m_pIArmControlMode, m_vArmJointVelocityK, m_i32RateVelocityControl);
        m_pVelocityController->enable(m_bArmHandActivated, m_bFingersActivated);

        // display parameters
            std::cout << std::endl << std::endl;
            displayDebug(m_sArm + std::string(" arm/hand activated"), m_bArmHandActivated);
            displayDebug(m_sArm + std::string(" fingers activated"), m_bFingersActivated);
            displayDebug(std::string("Gaze activated"), m_i32TimeoutArmReset);
            displayDebug(std::string("Rate velocity control"), m_i32RateVelocityControl);
            std::cout << std::endl;
            displayVectorDebug(m_sArm + std::string(" arm min joint                  : "), m_vArmMinJoint);
            displayVectorDebug(m_sArm + std::string(" arm max joint                  : "), m_vArmMaxJoint);
            displayVectorDebug(m_sArm + std::string(" arm reset position joint       : "), m_vArmResetPosition);
            displayVectorDebug(m_sArm + std::string(" arm joint velocity acceleration: "), m_vArmJointVelocityAcceleration);
            displayVectorDebug(m_sArm + std::string(" arm joint position acceleration: "), m_vArmJointPositionAcceleration);
            displayVectorDebug(m_sArm + std::string(" arm joint position speed       : "), m_vArmJointPositionSpeed);
            displayVectorDebug(m_sArm + std::string(" arm joint velocity             : "), m_vArmJointVelocityK);
            std::cout << std::endl << std::endl;

    return (m_bIsRunning=m_bInitialized=true);
}

void swTeleop::SWIcubArm::computeHandAngles(yarp::os::Bottle* handBottle,std::vector<double> &vHandAngles)
{
    vHandAngles = std::vector<double>(4,0.);

    // retrieve leap data
        std::vector<double> l_vArmDirection(3,0.), l_vHandDirection(3,0.),l_vHandDirectionE(3,0.), l_vHandPalmCoord(3,0.), l_vHandPalmNormal(3,0.), l_vHandPalmNormalE(3,0.);
        for(int ii = 0; ii < 3; ++ii)
        {
            l_vArmDirection[ii]     = handBottle->get(1 + ii).asDouble();
            l_vHandDirection[ii]    = handBottle->get(4 + ii).asDouble();
            l_vHandDirectionE[ii]   = handBottle->get(7 + ii).asDouble();
            l_vHandPalmCoord[ii]    = handBottle->get(10 + ii).asDouble();
            l_vHandPalmNormal[ii]   = handBottle->get(13 + ii).asDouble();
            l_vHandPalmNormalE[ii]  = handBottle->get(16 + ii).asDouble();
        }

    // convert to vec3D
        cv::Vec3d l_vecHandPalmNormal(l_vHandPalmNormal[0], l_vHandPalmNormal[1], l_vHandPalmNormal[2]);
        cv::Vec3d l_vecHandPalmCoord(l_vHandPalmCoord[0], l_vHandPalmCoord[1], l_vHandPalmCoord[2]);
        cv::Vec3d l_vecHandDirection(l_vHandDirection[0], l_vHandDirection[1], l_vHandDirection[2]);
        cv::Vec3d l_vecArmDirection(l_vArmDirection[0], l_vArmDirection[1], l_vArmDirection[2]);

    // normalize vectors
        l_vecHandPalmNormal = cv::normalize(l_vecHandPalmNormal);
        l_vecArmDirection   = cv::normalize(l_vecArmDirection);
        l_vecHandDirection  = cv::normalize(l_vecHandDirection);

    // convert to mat
        cv::Mat l_matHandDirection(l_vecHandDirection);
        cv::Mat l_matHandPalmNormal(l_vecHandPalmNormal);
        cv::Mat l_matArmDirection(l_vecArmDirection);

    // check hand palm orientation
        bool l_bHandPalmUp = false;
         if(swUtil::rad2Deg(acos(cv::Vec3d(0.,-1.,0.).dot(l_vecHandPalmNormal))) > 90.)
        {
            l_bHandPalmUp = true;
        }

    // compute transformation for aligning arm to z axis
        cv::Mat l_matTransfo;
        cv::Vec3d l_vecAxis(0.,0.,-1.);
        swUtil::rodriguesRotation(l_vecArmDirection, l_vecAxis, l_matTransfo);
    // apply transformation to the arm and the hand
        cv::Mat l_matTransfoHandDirection = l_matTransfo * l_matHandDirection;
        cv::Mat l_matTransfoHandNormal    = l_matTransfo * l_matHandPalmNormal;
        cv::Mat l_matTransfoArmDirection  = l_matTransfo * l_matArmDirection;

    // compute transformation for aligning palm normal to Y axis
        if(!l_bHandPalmUp)
        {
            l_vecAxis = cv::Vec3d(0.,-1.,0.);
        }
        else
        {
            l_vecAxis = cv::Vec3d(0.,1.,0.);
        }

        cv::Vec3d l_vecTransfoHandNormal(l_matTransfoHandNormal);
        swUtil::rodriguesRotation(l_vecTransfoHandNormal, l_vecAxis, l_matTransfo);

        cv::Mat l_matTransfoHandDirection2 = l_matTransfo * l_matTransfoHandDirection;

    // compute angle for wrist yaw
        double l_dot  = l_matTransfoHandDirection2.dot(l_matTransfoArmDirection);
        double l_angle = swUtil::rad2Deg(acos(l_dot/(cv::norm(l_matTransfoHandDirection2)* cv::norm(l_matTransfoArmDirection))));
        cv::Mat l_matCross = l_matTransfoHandDirection2.cross(l_matTransfoArmDirection);

        double l_dCrossY = l_matCross.at<double>(1);

        if(m_sArm != "left")
        {
            l_dCrossY *= -1;
        }

        if(l_dCrossY > 0.)
        {
            if(!l_bHandPalmUp)
            {
                l_angle = -l_angle;
            }
        }
        else
        {
            if(l_bHandPalmUp)
            {
                l_angle = -l_angle;
            }
        }

        // set joint value
        vHandAngles[3] = l_angle;

    // compute angle for wrist ptich
        cv::Vec3d l_vecTransfoHandDirection(l_matTransfoHandDirection);
        cv::Vec3d l_vecTransfoHandRight = l_vecTransfoHandNormal.cross(l_vecTransfoHandDirection);

        bool l_bHandPalmLeft = true;
        if(swUtil::rad2Deg(acos(cv::Vec3d(-1.,0.,0.).dot(l_vecHandPalmNormal))) > 90.)
        {
            l_bHandPalmLeft = false;
        }

        // compute transformation for aligning palm normal to X axis
        if(l_bHandPalmLeft)
        {
            l_vecAxis = cv::Vec3d(0.,-1.,0.);
        }
        else
        {
            l_vecAxis = cv::Vec3d(0.,1.,0.);
        }

        swUtil::rodriguesRotation(l_vecTransfoHandRight, l_vecAxis, l_matTransfo);
        l_matTransfoHandDirection2 = l_matTransfo * l_matTransfoHandDirection;


        l_dot  = l_matTransfoHandDirection2.dot(l_matTransfoArmDirection);
        l_angle = swUtil::rad2Deg(acos(l_dot/(cv::norm(l_matTransfoHandDirection2)* cv::norm(l_matTransfoArmDirection))));


        l_matCross = l_matTransfoHandDirection2.cross(l_matTransfoArmDirection);

        if(m_sArm != "left")
        {
            l_dCrossY *= -1;
        }


        if(l_dCrossY > 0.)
        {
            if(l_bHandPalmLeft)
            {
                 l_angle = -l_angle;
            }
            else
            {
                 l_angle = 0.0;
            }

        }
        else
        {
            if(l_bHandPalmLeft)
            {
                 l_angle = 0.0;
            }
            else
            {
                l_angle = -l_angle;
            }
        }

        // set joint value
        vHandAngles[2] = l_angle;


        if(m_sArm != "left")
        {
            vHandAngles[1] = (swUtil::rad2Deg(l_vHandPalmNormalE[1]) + 90.0);
        }
        else
        {
            vHandAngles[1] = -(swUtil::rad2Deg(l_vHandPalmNormalE[1]) - 90.0);
        }

        double l_dAngle = swUtil::rad2Deg(acos(cv::normalize(cv::Vec3d(0.0,l_vArmDirection[1],l_vArmDirection[2])).dot(cv::Vec3d(0.0,1.0,0.0))));
        l_dAngle *= -1.0;
        l_dAngle += 140.0;
        vHandAngles[0] = l_dAngle;
}

void swTeleop::SWIcubArm::computeFingerAngles(yarp::os::Bottle *handFingersBottle, std::vector<double> &vFingerAngles)
{
    // arm joint 0 hand_finger
    // arm joint 1 thumb_oppose
    // arm joint 2 thumb_proximal
    // arm joint 3 thumb_distal
    // arm joint 4 index_proximal
    // arm joint 5 index_distal
    // arm joint 6 middle_proximal
    // arm joint 7 middle_distal
    // arm joint 8 pinky

    // init res angles
        vFingerAngles = std::vector<double>(9,0.);

    // retrieve leap data
        std::vector<cv::Vec3d> l_vecThumbDirections(3,    cv::Vec3d(0.,0.,0.));
        std::vector<cv::Vec3d> l_vecIndexDirections(4,    cv::Vec3d(0.,0.,0.));
        std::vector<cv::Vec3d> l_vecMiddleDirections(4,   cv::Vec3d(0.,0.,0.));
        std::vector<cv::Vec3d> l_vecRingDirections(4,     cv::Vec3d(0.,0.,0.));
        std::vector<cv::Vec3d> l_vecPinkyDirections(4,    cv::Vec3d(0.,0.,0.));
        cv::Vec3d l_vecHandNormal    = cv::normalize(cv::Vec3d(handFingersBottle->get(13).asDouble(), handFingersBottle->get(14).asDouble(), handFingersBottle->get(15).asDouble()));
        cv::Vec3d l_vecHandDirection = cv::normalize(cv::Vec3d(handFingersBottle->get(4).asDouble(), handFingersBottle->get(5).asDouble(), handFingersBottle->get(6).asDouble()));

        for(int ii = 0; ii < 4; ++ii)
        {
            for(int jj = 0; jj < 3; ++jj)
            {
                if(ii < 3)
                {
                    l_vecThumbDirections[ii][jj] = handFingersBottle->get(19 + ii * 3 + jj).asDouble();
                }

                l_vecIndexDirections[ii][jj] = handFingersBottle->get(28 + ii * 3 + jj).asDouble();
                l_vecMiddleDirections[ii][jj] = handFingersBottle->get(40 + ii * 3 + jj).asDouble();
                l_vecRingDirections[ii][jj] = handFingersBottle->get(52 + ii * 3 + jj).asDouble();
                l_vecPinkyDirections[ii][jj] = handFingersBottle->get(64 + ii * 3 + jj).asDouble();
            }

            if(l_vecThumbDirections[ii][0] != 0 && l_vecThumbDirections[ii][1] != 0 && l_vecThumbDirections[ii][2] != 0)
            {
                l_vecThumbDirections[ii]    = cv::normalize(l_vecThumbDirections[ii]);
            }
            if(l_vecIndexDirections[ii][0] != 0 && l_vecIndexDirections[ii][1] != 0 && l_vecIndexDirections[ii][2] != 0)
            {
                l_vecIndexDirections[ii]    = cv::normalize(l_vecIndexDirections[ii]);
            }
            if(l_vecMiddleDirections[ii][0] != 0 && l_vecMiddleDirections[ii][1] != 0 && l_vecMiddleDirections[ii][2] != 0)
            {
                l_vecMiddleDirections[ii]   = cv::normalize(l_vecMiddleDirections[ii]);
            }
            if(l_vecRingDirections[ii][0] != 0 && l_vecRingDirections[ii][1] != 0 && l_vecRingDirections[ii][2] != 0)
            {
                l_vecRingDirections[ii]     = cv::normalize(l_vecRingDirections[ii]);
            }
            if(l_vecPinkyDirections[ii][0] != 0 && l_vecPinkyDirections[ii][1] != 0 && l_vecPinkyDirections[ii][2] != 0)
            {
                l_vecPinkyDirections[ii]    = cv::normalize(l_vecPinkyDirections[ii]);
            }
        }


        std::vector<cv::Mat> l_vMatThumbDirectionsTransfo(3,    cv::Mat(cv::Vec3d(0.,0.,0.)));
        std::vector<cv::Mat> l_vMatIndexDirectionsTransfo(4,    cv::Mat(cv::Vec3d(0.,0.,0.)));
        std::vector<cv::Mat> l_vMatMiddleDirectionsTransfo(4,   cv::Mat(cv::Vec3d(0.,0.,0.)));
        std::vector<cv::Mat> l_vMatRingDirectionsTransfo(4,     cv::Mat(cv::Vec3d(0.,0.,0.)));
        std::vector<cv::Mat> l_vMatPinkyDirectionsTransfo(4,    cv::Mat(cv::Vec3d(0.,0.,0.)));
        cv::Mat l_matHandDirectionTransfo(cv::Vec3d(0.,0.,0.));


    // compute transformation for aligning palm normal to Y axis
        cv::Vec3d l_vecAxis;

        bool l_bHandPalmUp = false;
        if(swUtil::rad2Deg(acos(cv::Vec3d(0.,-1.,0.).dot(l_vecHandNormal))) > 90.)
        {
            l_bHandPalmUp = true;
        }
        bool l_bHandPalmLeft = true;
        if(swUtil::rad2Deg(acos(cv::Vec3d(-1.,0.,0.).dot(l_vecHandNormal))) > 90.)
        {
            l_bHandPalmLeft = false;
        }

        if(!l_bHandPalmUp)
        {
            l_vecAxis = cv::Vec3d(0.,-1.,0.);
        }
        else
        {
            l_vecAxis = cv::Vec3d(0.,1.,0.);
        }


        cv::Mat l_matTransfo;
        swUtil::rodriguesRotation(l_vecHandNormal, l_vecAxis, l_matTransfo);

        for(int ii = 0; ii < 4; ++ii)
        {
            if(ii < 3)
            {
                l_vMatThumbDirectionsTransfo[ii] = l_matTransfo * cv::Mat(l_vecThumbDirections[ii]);
            }

            l_vMatIndexDirectionsTransfo[ii]     = l_matTransfo * cv::Mat(l_vecIndexDirections[ii]);
            l_vMatMiddleDirectionsTransfo[ii]    = l_matTransfo * cv::Mat(l_vecMiddleDirections[ii]);
            l_vMatRingDirectionsTransfo[ii]      = l_matTransfo * cv::Mat(l_vecRingDirections[ii]);
            l_vMatPinkyDirectionsTransfo[ii]     = l_matTransfo * cv::Mat(l_vecPinkyDirections[ii]);
        }

        l_matHandDirectionTransfo = l_matTransfo * cv::Mat(l_vecHandDirection);




    // compute fingers interval (hand_finger)
        // ... better not (hight risk of breaking)

    // compute thumbs angles
        // thumb metacarpal-> index metarcapal (thumb_oppose)
            cv::Vec3d l_vecTemp1(cv::normalize(cv::Vec3d(l_vMatThumbDirectionsTransfo[0]))); // TODO :...
            cv::Vec3d l_vecTemp2(cv::normalize(cv::Vec3d(l_vMatIndexDirectionsTransfo[0])));
            double l_dDot = l_vecTemp1.dot(l_vecTemp2);
            double l_dAngle = swUtil::rad2Deg(acos(l_dDot));
            vFingerAngles[2] = 90.0 - l_dAngle;

        // proximal->intermediate (thumb_proximal)
            l_vecTemp1 = cv::normalize(l_vecThumbDirections[0]);
            l_vecTemp2 = cv::normalize(l_vecThumbDirections[1]);
            l_dDot = l_vecTemp1.dot(l_vecTemp2);
            cv::Vec3d l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle = swUtil::rad2Deg(acos(l_dDot));
//            vFingerAngles[1] += l_dAngle;

        // intermediate->distal (thumb_distal)
            l_vecTemp1 = cv::normalize(l_vecThumbDirections[1]);
            l_vecTemp2 = cv::normalize(l_vecThumbDirections[2]);
            l_dDot     = l_vecTemp1.dot(l_vecTemp2);
            l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle = swUtil::rad2Deg(acos(l_dDot));
            vFingerAngles[3] += l_dAngle;

    // compute index angles
        // metacarpal->proximal (index_proximal)
            l_vecTemp1 = cv::normalize(l_vecIndexDirections[0]);
            l_vecTemp2 = cv::normalize(l_vecIndexDirections[1]);
            l_dDot     = l_vecTemp1.dot(l_vecTemp2);
            l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle = swUtil::rad2Deg(acos(l_dDot));

            if(!l_bHandPalmLeft)
            {
                if(l_vecCross[1] < 0.)
                {
                    vFingerAngles[4] += l_dAngle;
                }
            }
            else
            {
                if(l_vecCross[1] >= 0.)
                {
                    vFingerAngles[4] += l_dAngle;
                }
            }

        // proximal->intermediate + intermediate->distal (index_distal)
            l_vecTemp1 = cv::normalize(l_vecIndexDirections[1]);
            l_vecTemp2 = cv::normalize(l_vecIndexDirections[2]);
            l_dDot     = l_vecTemp1.dot(l_vecTemp2);
            l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

            if(!l_bHandPalmLeft)
            {
                if(l_vecCross[1] < 0.)
                {
                    vFingerAngles[5] += l_dAngle;
                }
            }
            else
            {
                if(l_vecCross[1] >= 0.)
                {
                    vFingerAngles[5] += l_dAngle;
                }
            }

            l_vecTemp1 = cv::normalize(l_vecIndexDirections[2]);
            l_vecTemp2 = cv::normalize(l_vecIndexDirections[3]);
            l_dDot     = l_vecTemp1.dot(l_vecTemp2);
            l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

            if(!l_bHandPalmLeft)
            {
                if(l_vecCross[1] < 0.)
                {
                    vFingerAngles[5] += l_dAngle;
                }
            }
            else
            {
                if(l_vecCross[1] >= 0.)
                {
                    vFingerAngles[5] += l_dAngle;
                }
            }

    // compute middle angles
        // metacarpal->proximal (middle_proximal)
            l_vecTemp1 = cv::normalize(l_vecMiddleDirections[0]);
            l_vecTemp2 = cv::normalize(l_vecMiddleDirections[1]);
            l_dDot     = l_vecTemp1.dot(l_vecTemp2);
            l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

            if(!l_bHandPalmLeft)
            {
                if(l_vecCross[1] < 0.)
                {
                    vFingerAngles[6] += l_dAngle;
                }
            }
            else
            {
                if(l_vecCross[1] >= 0.)
                {
                    vFingerAngles[6] += l_dAngle;
                }
            }

        // proximal->intermediate + intermediate->distal (middle_distal)
            l_vecTemp1 = cv::normalize(l_vecMiddleDirections[1]);
            l_vecTemp2 = cv::normalize(l_vecMiddleDirections[2]);
            l_dDot     = l_vecTemp1.dot(l_vecTemp2);
            l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

            if(!l_bHandPalmLeft)
            {
                if(l_vecCross[1] < 0.)
                {
                    vFingerAngles[7] += l_dAngle;
                }
            }
            else
            {
                if(l_vecCross[1] >= 0.)
                {
                    vFingerAngles[7] += l_dAngle;
                }
            }

            l_vecTemp1 = cv::normalize(l_vecMiddleDirections[2]);
            l_vecTemp2 = cv::normalize(l_vecMiddleDirections[3]);
            l_dDot     = l_vecTemp1.dot(l_vecTemp2);
            l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

            if(!l_bHandPalmLeft)
            {
                if(l_vecCross[1] < 0.)
                {
                    vFingerAngles[7] += l_dAngle;
                }
            }
            else
            {
                if(l_vecCross[1] >= 0.)
                {
                    vFingerAngles[7] += l_dAngle;
                }
            }

    // compute ring + pinky angles
        // metacarpal->proximal + proximal->intermediate + intermediate->distal (pinky)
            l_vecTemp1 = cv::normalize(l_vecPinkyDirections[0]);
            l_vecTemp2 = cv::normalize(l_vecPinkyDirections[1]);
            l_dDot     = l_vecTemp1.dot(l_vecTemp2);
            l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

            if(!l_bHandPalmLeft)
            {
                if(l_vecCross[1] < 0.)
                {
                    vFingerAngles[8] += l_dAngle;
                }
            }
            else
            {
                if(l_vecCross[1] >= 0.)
                {
                    vFingerAngles[8] += l_dAngle;
                }
            }

            l_vecTemp1 = cv::normalize(l_vecPinkyDirections[1]);
            l_vecTemp2 = cv::normalize(l_vecPinkyDirections[2]);
            l_dDot     = l_vecTemp1.dot(l_vecTemp2);
            l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

            if(!l_bHandPalmLeft)
            {
                if(l_vecCross[1] < 0.)
                {
                    vFingerAngles[8] += l_dAngle;
                }
            }
            else
            {
                if(l_vecCross[1] >= 0.)
                {
                    vFingerAngles[8] += l_dAngle;
                }
            }

            l_vecTemp1 = cv::normalize(l_vecPinkyDirections[2]);
            l_vecTemp2 = cv::normalize(l_vecPinkyDirections[3]);
            l_dDot     = l_vecTemp1.dot(l_vecTemp2);
            l_vecCross = l_vecTemp1.cross(l_vecTemp2);
            l_dAngle   = swUtil::rad2Deg(acos(l_dDot));

            if(!l_bHandPalmLeft)
            {
                if(l_vecCross[1] < 0.)
                {
                    vFingerAngles[8] += l_dAngle;
                }
            }
            else
            {
                if(l_vecCross[1] >= 0.)
                {
                    vFingerAngles[8] += l_dAngle;
                }
            }
}

bool swTeleop::SWIcubArm::checkBottles()
{
    if(!m_bIsRunning)
    {
        return false;
    }

    if(!m_bInitialized)
    {
        std::cerr << "Icub Arm control module not initialized. " << std::endl;
        return (m_bIsRunning=false);
    }

    // init command vector
        yarp::sig::Vector l_vArmJoints;
        l_vArmJoints.resize(m_i32ArmJointsNb);
        l_vArmJoints = 0.;
    // set default values to arm joint
        for(uint ii = 0; ii < l_vArmJoints.size(); ++ii)
        {
            l_vArmJoints[ii] = m_vArmResetPositionDefault[ii];
        }

    // defines bottles
        yarp::os::Bottle *l_pHandTarget = NULL;//, *l_pHandCartesianTarget = NULL; // *l_pFingersTarget = NULL, *l_pArmTarget = NULL,


        l_pHandTarget = m_oHandFingersTrackerPort.read(false);


        if(l_pHandTarget)
        {
            int l_deviceId = l_pHandTarget->get(0).asInt();

            // arm joint 0 shoulder_pitch
            // arm joint 1 shoulder_roll
            // arm joint 2 shoulder_yaw
            // arm joint 3 elbow
            // arm joint 4 wrist_prosup
            // arm joint 5 wrist_pitch
            // arm joint 6 wrist_yaw
            // arm joint 7 hand_finger
            // arm joint 8 thumb_oppose
            // arm joint 9 thumb_proximal
            // arm joint 10 thumb_distal
            // arm joint 11 index_proximal
            // arm joint 12 index_distal
            // arm joint 13 middle_proximal
            // arm joint 14 middle_distal
            // arm joint 15 pinky

            switch(l_deviceId)
            {
                case swTracking::DUMMY_LIB :

                    // ...
                break;
                case swTracking::LEAP_LIB :
		{

                    // set default joint values
                        for(int ii = 0; ii < m_i32ArmJointsNb; ++ii)
                        {
//                            l_vArmJoints[ii] = m_vArmResetPosition[ii];
                        }

                        std::vector<double> l_vHandAngles;
//                        std::cout << "computeHandAngles  ";
                        computeHandAngles(l_pHandTarget, l_vHandAngles);
//                        std::cout << "-> end  |";

                        for(uint ii = 0; ii < l_vHandAngles.size(); ++ii)
                        {
                            l_vArmJoints[3 + ii] = l_vHandAngles[ii];
                        }

                        std::vector<double> l_vFingerAngles;
//                        std::cout << "computeFingerAngles  ";
                        computeFingerAngles(l_pHandTarget, l_vFingerAngles);
//                        std::cout << "-> end  |";

                        for(uint ii = 0; ii < l_vFingerAngles.size(); ++ii)
                        {
                            l_vArmJoints[7 + ii] = l_vFingerAngles[ii];
                        }
		}
                break;
		case swTracking::OPENNI_LIB :
		{
			
			std::vector<double> l_pointTorso(3);
				l_pointTorso[0] = l_pHandTarget->get(1).asDouble();
				l_pointTorso[1] = l_pHandTarget->get(2).asDouble();
				l_pointTorso[2] = l_pHandTarget->get(3).asDouble();
			std::vector<double> l_pointNeck(3);
				l_pointNeck[0] = l_pHandTarget->get(4).asDouble();
				l_pointNeck[1] = l_pHandTarget->get(5).asDouble();
				l_pointNeck[2] = l_pHandTarget->get(6).asDouble();
			std::vector<double> l_pointShoulder(3);
				l_pointShoulder[0] = l_pHandTarget->get(7).asDouble();
				l_pointShoulder[1] = l_pHandTarget->get(8).asDouble();
				l_pointShoulder[2] = l_pHandTarget->get(9).asDouble();
			std::vector<double> l_pointElbow(3);
				l_pointElbow[0] = l_pHandTarget->get(10).asDouble();
				l_pointElbow[1] = l_pHandTarget->get(11).asDouble();
				l_pointElbow[2] = l_pHandTarget->get(12).asDouble();
			std::vector<double> l_pointHand(3);
				l_pointHand[0] = l_pHandTarget->get(13).asDouble();
				l_pointHand[1] = l_pHandTarget->get(14).asDouble();
				l_pointHand[2] = l_pHandTarget->get(15).asDouble();

			std::vector<double> l_vecTorso = swUtil::vec(l_pointNeck, l_pointTorso);
			std::vector<double> l_vecForearm = swUtil::vec(l_pointElbow, l_pointHand);
			std::vector<double> l_vecShoulder = swUtil::vec(l_pointShoulder, l_pointNeck);
			std::vector<double> l_vecArm = swUtil::vec( l_pointShoulder, l_pointElbow);

			std::vector<double> l_rpyShoulder = swUtil::computeRollPitchYaw(l_vecArm, l_vecTorso);
			std::vector<double> l_rpyElbow = swUtil::computeRollPitchYaw(l_vecForearm, l_vecArm);

			//~ l_vArmJoints[0] = swUtil::degree180(l_rpyShoulder[1] - 180.);
			//~ l_vArmJoints[1] = swUtil::degree180(- l_rpyShoulder[0] - 180.);
			//l_vArmJoints[2] = swUtil::degree180(l_rpyElbow[1] - 90.);
			//~ l_vArmJoints[1] = swUtil::vectorAngle(l_vecArm, l_vecShoulder);
			
			//~ l_vArmJoints[3] = swUtil::vectorAngle(l_vecForearm, l_vecArm);
			
			std::vector<double> l_vecArm_roll(3);
			l_vecArm_roll[0] = 0; l_vecArm_roll[1] = l_vecArm[1]; l_vecArm_roll[2] = l_vecArm[2]; 
			l_vArmJoints[0] = -swUtil::vectorAngle(l_vecTorso, l_vecArm_roll);
			
			
			std::vector<double> l_vecArm_pitch(3);
			l_vecArm_pitch[0] =  l_vecArm[0]; l_vecArm_pitch[1] = l_vecArm[1]; l_vecArm_pitch[2] = 0; 
			l_vArmJoints[1] = swUtil::vectorAngle(l_vecTorso, l_vecArm_pitch);
			
			l_vArmJoints[3] = swUtil::vectorAngle(l_vecForearm, l_vecArm);
			
			//~ std::cerr <<  l_vArmJoints[0]  << std::endl;
			
			
		}			
		break;
		
            }
	    
	    m_dArmTimeLastBottle = -1.;
                m_pVelocityController->enable(m_bArmHandActivated, m_bFingersActivated);
        }
	else // manage timeout and reset position
        {
		if(m_dArmTimeLastBottle < 0.)
		{
		    m_dArmTimeLastBottle = yarp::os::Time::now();
		}
		else
		{
		    if(yarp::os::Time::now() - m_dArmTimeLastBottle > 0.001 * m_i32TimeoutArmReset)
		    {
			m_pVelocityController->enable(false, false);
			resetArmPosition();
			m_dArmTimeLastBottle = -1.;
		    }
		}
	}


    // check each joint value to ensure it is in the right range, if not crop to the max/min values
        for(uint ii = 0; ii < l_vArmJoints.size(); ++ii)
        {
            if(l_vArmJoints[ii] < m_vArmMinJoint[ii])
            {
                l_vArmJoints[ii] = m_vArmMinJoint[ii];
            }
            if(l_vArmJoints[ii] > m_vArmMaxJoint[ii])
            {
                l_vArmJoints[ii] = m_vArmMaxJoint[ii];
            }
        }
	
	//~ std::cerr <<  l_vArmJoints[0]  << std::endl;

        if(l_pHandTarget)
        {
//            std::cout << "send joints " << std::endl;
            m_pVelocityController->setJoints(l_vArmJoints);

            if(!m_pVelocityController->isRunning())
            {                
                m_pVelocityController->start();
            }
        }

    return true;
}

void swTeleop::SWIcubArm::resetArmPosition()
{
    if(m_bArmHandActivated)
    {
        for(int ii = 0; ii < m_i32ArmJointsNb; ++ii)
        {
		m_pIArmControlMode->setControlMode(ii,VOCAB_CM_POSITION);
            m_pIArmPosition->positionMove(ii,m_vArmResetPosition[ii]);
        }
    }
}

bool swTeleop::SWIcubArm::close()
{

    bool l_bArmPositionCloseState       = m_pIArmPosition->stop();
    bool l_bRobotArmCloseState          = m_oRobotArm.close();

    if(m_pVelocityController->isRunning())
    {
	m_pVelocityController->stop();
	while(m_pVelocityController->isRunning())
	{
	yarp::os::Time::delay(0.1);
	}
    }

    // close ports
        if(m_bArmHandActivated)
        {
            m_oHandTrackerPort.close();
            m_oHandFingersTrackerPort.close();
        }
	
	m_pIArmEncoders = NULL;
	m_pIArmPosition = NULL;
	m_pIArmVelocity = NULL;
	m_pIArmControlMode = NULL;
	
	if (NULL !=m_pVelocityController)
	{
		delete m_pVelocityController;
		m_pVelocityController = NULL;
	}
	
    return (l_bArmPositionCloseState && l_bRobotArmCloseState);// && l_bRobotCartesianArmCloseState);
}

bool swTeleop::SWIcubArm::interruptModule()
{

    m_bIsRunning = false;

    // reset positions
        if(m_bArmHandActivated)
        {
            resetArmPosition();
        }

    if(m_pVelocityController->isRunning())
    {
        m_pVelocityController->stop();
    }

    // close ports
        if(m_bArmHandActivated)
        {
            m_oHandFingersTrackerPort.interrupt();
            m_oHandTrackerPort.interrupt();
        }

    std::cout << "--Interrupting the iCub Arm Teleoperation module..." << std::endl;

    return true;
}


swTeleop::SWArmVelocityController::SWArmVelocityController(yarp::dev::IEncoders *pIArmEncoders, yarp::dev::IVelocityControl *pIArmVelocity, yarp::dev::IControlMode2    *pIArmControlMode,
                                                     std::vector<double> &vArmJointVelocityK, int i32Rate)
    : RateThread(i32Rate), m_bArmHandEnabled(false), m_bFingersEnabled(false), m_vArmJointVelocityK(vArmJointVelocityK)
{

    if(pIArmEncoders)
    {
        m_pIArmEncoders = pIArmEncoders;
    }
    if(pIArmVelocity)
    {
        m_pIArmVelocity = pIArmVelocity;
    }
    	if(pIArmControlMode)
    {
	m_pIArmControlMode = pIArmControlMode;
    }   
}


swTeleop::SWArmVelocityController::~SWArmVelocityController()
{
}


void swTeleop::SWArmVelocityController::run()
{
//    std::cout << "start run ";
    m_oMutex.lock();
        yarp::sig::Vector l_vArmJoints = m_vLastArmJoint; // Check values with Joint before
    m_oMutex.unlock();

    yarp::sig::Vector l_vEncoders, l_vCommand;
    l_vEncoders.resize(l_vArmJoints.size());
    l_vCommand.resize(l_vArmJoints.size());

    m_pIArmEncoders->getEncoders(l_vEncoders.data());

    for(uint ii = 0; ii < l_vCommand.size(); ++ii)
    {
        l_vCommand[ii] =  m_vArmJointVelocityK[ii] * (l_vArmJoints[ii] - l_vEncoders[ii]);

    }

    // ...
    l_vCommand[7] = 0;

    if(m_bArmHandEnabled)
    {
        for(uint ii = 0; ii < 7; ++ii)
        {
		m_pIArmControlMode->setControlMode(ii,VOCAB_CM_VELOCITY);
            m_pIArmVelocity->velocityMove(ii, l_vCommand[ii]);
        }
    }

    if(m_bFingersEnabled)
    {
        for(uint ii = 7; ii < l_vArmJoints.size(); ++ii)
        {
		m_pIArmControlMode->setControlMode(ii,VOCAB_CM_VELOCITY);
            m_pIArmVelocity->velocityMove(ii, l_vCommand[ii]);
        }
    }
//    std::cout << " - end run |";
}

void swTeleop::SWArmVelocityController::enable(cbool bArmHandActivated, cbool bFingersActivated)
{
    m_oMutex.lock();
        m_bArmHandEnabled = bArmHandActivated;
        m_bFingersEnabled = bFingersActivated;
    m_oMutex.unlock();
}

void swTeleop::SWArmVelocityController::setJoints(const yarp::sig::Vector &vJoints)
{

    m_oMutex.lock();
        m_vLastArmJoint = vJoints;
    m_oMutex.unlock();
}







//// retrieve leap data
//    std::vector<double> l_vArmDirection(3,0.), l_vHandDirection(3,0.),l_vHandDirectionE(3,0.), l_vHandPalmCoord(3,0.), l_vHandPalmNormal(3,0.), l_vHandPalmNormalE(3,0.);
//    for(int ii = 0; ii < 3; ++ii)
//    {
//        l_vArmDirection[ii]     = l_pHandTarget->get(1 + ii).asDouble();
//        l_vHandDirection[ii]    = l_pHandTarget->get(4 + ii).asDouble();
//        l_vHandDirectionE[ii]   = l_pHandTarget->get(7 + ii).asDouble();
//        l_vHandPalmCoord[ii]    = l_pHandTarget->get(10 + ii).asDouble();
//        l_vHandPalmNormal[ii]   = l_pHandTarget->get(13 + ii).asDouble();
//        l_vHandPalmNormalE[ii]  = l_pHandTarget->get(16 + ii).asDouble();
//    }

//// convert to vec3D
//    cv::Vec3d l_vecHandPalmNormal(l_vHandPalmNormal[0], l_vHandPalmNormal[1], l_vHandPalmNormal[2]);
//    cv::Vec3d l_vecHandPalmCoord(l_vHandPalmCoord[0], l_vHandPalmCoord[1], l_vHandPalmCoord[2]);
//    cv::Vec3d l_vecHandDirection(l_vHandDirection[0], l_vHandDirection[1], l_vHandDirection[2]);
//    cv::Vec3d l_vecArmDirection(l_vArmDirection[0], l_vArmDirection[1], l_vArmDirection[2]);

//// normalize vectors
//    l_vecHandPalmNormal = cv::normalize(l_vecHandPalmNormal);
//    l_vecArmDirection   = cv::normalize(l_vecArmDirection);
//    l_vecHandDirection  = cv::normalize(l_vecHandDirection);

//// convert to mat
//    cv::Mat l_matHandDirection(l_vecHandDirection);
//    cv::Mat l_matHandPalmNormal(l_vecHandPalmNormal);
//    cv::Mat l_matArmDirection(l_vecArmDirection);

//// check hand palm orientation
//    bool l_bHandPalmUp = false;
//    if((acos(cv::Vec3d(0.,-1.,0.).dot(l_vecHandPalmNormal)) * 180./3.14) > 90.)
//    {
//        l_bHandPalmUp = true;
//    }

//// compute transformation for aligning arm to z axis
//    cv::Mat l_matTransfo;
//    cv::Vec3d l_vecAxis(0.,0.,-1.);
//    swUtil::rodriguesRotation(l_vecArmDirection, l_vecAxis, l_matTransfo);
//// apply transformation to the arm and the hand
//    cv::Mat l_matTransfoHandDirection = l_matTransfo * l_matHandDirection;
//    cv::Mat l_matTransfoHandNormal    = l_matTransfo * l_matHandPalmNormal;
//    cv::Mat l_matTransfoArmDirection  = l_matTransfo * l_matArmDirection;

//// compute transformation for aligning palm normal to Y axis
//    if(!l_bHandPalmUp)
//    {
//        l_vecAxis = cv::Vec3d(0.,-1.,0.);
//    }
//    else
//    {
//        l_vecAxis = cv::Vec3d(0.,1.,0.);
//    }

//    cv::Vec3d l_vecTransfoHandNormal(l_matTransfoHandNormal);
//    swUtil::rodriguesRotation(l_vecTransfoHandNormal, l_vecAxis, l_matTransfo);

//    cv::Mat l_matTransfoHandDirection2 = l_matTransfo * l_matTransfoHandDirection;

//// compute angle for wrist yaw
//    double l_dot  = l_matTransfoHandDirection2.dot(l_matTransfoArmDirection);
//    double l_angle = acos(l_dot/(cv::norm(l_matTransfoHandDirection2)* cv::norm(l_matTransfoArmDirection))) * 180./3.14;
//    cv::Mat l_matCross = l_matTransfoHandDirection2.cross(l_matTransfoArmDirection);

//    double l_dCrossY = l_matCross.at<double>(1);

//    if(m_sArm != "left")
//    {
//        l_dCrossY *= -1;
//    }

//    if(l_dCrossY > 0.)
//    {
//        if(!l_bHandPalmUp)
//        {
//            l_angle = -l_angle;
//        }
//    }
//    else
//    {
//        if(l_bHandPalmUp)
//        {
//            l_angle = -l_angle;
//        }
//    }

//    // set joint value
//    l_vArmJoints[6] = l_angle;

//    cv::Vec3d l_vecTransfoHandDirection(l_matTransfoHandDirection);
//    cv::Vec3d l_vecTransfoHandRight = l_vecTransfoHandNormal.cross(l_vecTransfoHandDirection);

//    bool l_bHandPalmLeft = true;
//    if((acos(cv::Vec3d(-1.,0.,0.).dot(l_vecHandPalmNormal)) * 180./3.14) > 90.)
//    {
//        l_bHandPalmLeft = false;
//    }

//    // compute transformation for aligning palm normal to X axis
//    if(l_bHandPalmLeft)
//    {
//        l_vecAxis = cv::Vec3d(0.,-1.,0.);
//    }
//    else
//    {
//        l_vecAxis = cv::Vec3d(0.,1.,0.);
//    }

//    swUtil::rodriguesRotation(l_vecTransfoHandRight, l_vecAxis, l_matTransfo);
//    l_matTransfoHandDirection2 = l_matTransfo * l_matTransfoHandDirection;

//// compute angle for wrist ptich
//    l_dot  = l_matTransfoHandDirection2.dot(l_matTransfoArmDirection);
//    l_angle = acos(l_dot/(cv::norm(l_matTransfoHandDirection2)* cv::norm(l_matTransfoArmDirection))) * 180./3.14;


//    l_matCross = l_matTransfoHandDirection2.cross(l_matTransfoArmDirection);
////                        l_dCrossY = l_matCross.at<double>(1);

//    if(m_sArm != "left")
//    {
//        l_dCrossY *= -1;
//    }


//    if(l_dCrossY > 0.)
//    {
//        if(l_bHandPalmLeft)
//        {
//             l_angle = -l_angle;
//        }
//        else
//        {
//             l_angle = 0.0;
//        }

//    }
//    else
//    {
//        if(l_bHandPalmLeft)
//        {
//             l_angle = 0.0;
//        }
//        else
//        {
//            l_angle = -l_angle;
//        }
//    }

//    // set joint value
//    l_vArmJoints[5] = l_angle;


//    if(m_sArm != "left")
//    {
//        l_vArmJoints[4] = (l_vHandPalmNormalE[1]* 180. / 3.14 + 90.0);
//    }
//    else
//    {
//        l_vArmJoints[4] = -(l_vHandPalmNormalE[1]* 180. / 3.14 - 90.0);
//    }

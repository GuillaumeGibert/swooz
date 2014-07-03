
/**
 * \file SWFaceLabTracking.cpp
 * \brief Defines SWFaceLabTracking class
 * \author Florian Lance
 * \date 29/11/13
 */

#include "leap/SWLeapTracking.h"

#include "SWTrackingDevice.h"

using namespace yarp::os;
using namespace yarp::dev;

using namespace yarp::sig;
using namespace swExcept;
using namespace Leap;




SWLeapTracking::SWLeapTracking() : m_bIsLeapInitialized(true), m_i32Fps(40)
{
    std::string l_sDeviceName  = "leap";
    std::string l_sLibraryName = "leapSDK";

    // set ports name
        m_sHandTrackingPortNameLeft           = "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/left_arm/hand";
        m_sHandCartesianTrackingPortNameLeft  = "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/left_arm/hand_cartesian";
        m_sHandTrackingPortNameRight          = "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/right_arm/hand";
        m_sHandCartesianTrackingPortNameRight = "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/right_arm/hand_cartesian";

    // open ports
        m_oHandTrackingPortLeft          .open(m_sHandTrackingPortNameLeft.c_str());
        m_oHandCartesianTrackingPortLeft .open(m_sHandCartesianTrackingPortNameLeft.c_str());
        m_oHandTrackingPortRight         .open(m_sHandTrackingPortNameRight.c_str());
        m_oHandCartesianTrackingPortRight.open(m_sHandCartesianTrackingPortNameRight.c_str());

    initLeap();

	if(!isLeapInitialized())
	{
        std::cerr << "-ERROR : leap not initialized, Leap tracking RF module aborted. " << std::endl;
		interruptModule();
		close();
	}
}

bool SWLeapTracking::isLeapInitialized() const
{
    return m_bIsLeapInitialized;
}

bool SWLeapTracking::close()
{
    m_oHandTrackingPortLeft.close();
    m_oHandCartesianTrackingPortLeft.close();
    m_oHandTrackingPortRight.close();
    m_oHandCartesianTrackingPortRight.close();

    // terminate network
    Network::fini();

    return true;
}

bool SWLeapTracking::interruptModule()
{
    return true;
}

bool SWLeapTracking::configure(ResourceFinder &oRf)
{
    // ... nothing for now
    return true;
}

void SWLeapTracking::initLeap()
{
	m_bIsLeapInitialized = m_oLeap.init();
}

double SWLeapTracking::getPeriod()
{
    // module periodicity (seconds), called implicitly by myModule
    return 1./m_i32Fps;
}


bool SWLeapTracking::updateModule()
{
    if(!m_bIsLeapInitialized)
    {
        // if an error occured during the configuration
        interruptModule();
        close();

        return false;
    }

    //  grab Leap data
    m_oLeap.grab();

    std::vector<float> l_vLambda;

    // LEFT HAND
    //      HAND CARTESIAN
    yarp::os::Bottle &l_oHandCartesianBottleLeft = m_oHandCartesianTrackingPortLeft.prepare();
    l_oHandCartesianBottleLeft.clear();
        l_oHandCartesianBottleLeft.addInt(swTracking::LEAP_LIB);                 // Left hand : LEAP_LIB id / get(0).asInt()
        m_oLeap.directionArm(true, l_vLambda);
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : arm direction x / get(1).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : arm direction y / get(2).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : arm direction z / get(3).asDouble()
        m_oLeap.directionHand(true, l_vLambda);
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : hand direction x / get(4).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : hand direction y / get(5).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : hand direction z / get(6).asDouble()
        m_oLeap.directionHandEuclidian(true, l_vLambda);
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : hand direction pitch / get(7).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : hand direction roll  / get(8).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : hand direction yaw   / get(9).asDouble()
        m_oLeap.coordPalmHand(true, l_vLambda);
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : hand palm coord x / get(10).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : hand palm coord y / get(11).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : hand palm coord z / get(12).asDouble()
        m_oLeap.normalPalmHand(true, l_vLambda);
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : hand palm normal x / get(13).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : hand palm normal y / get(14).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : hand palm normal z / get(15).asDouble()
        m_oLeap.normalPalmHandEuclidian(true, l_vLambda);
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : hand palm normal pitch / get(16).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : hand palm normal roll  / get(17).asDouble()
        l_oHandCartesianBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : hand palm normal yaw   / get(18).asDouble()
    m_oHandCartesianTrackingPortLeft.write();

    //      HAND  -> copy current cartesian hand
    yarp::os::Bottle &l_oHandBottleLeft = m_oHandTrackingPortLeft.prepare();
        l_oHandBottleLeft.copy(l_oHandCartesianBottleLeft);

        // fingers ....
        //        void fingerRotation(cbool leftHand, const Leap::Finger::Type fingerType, std::vector<float> &fingerRotation);
//        m_oLeap.fingerRotation(false, Leap::Finger::TYPE_THUMB, l_vRotation);

////			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(1,1,false)); //Finger2Rot: x / get(1).asDouble()
////			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(1,2,false)); //Finger2Rot: x / get(2).asDouble()
//        l_oFingerBottleRight.addDouble(static_cast<double>(l_vRotation[0])); //Finger2Rot: x / get(1).asDouble()
//        l_oFingerBottleRight.addDouble(static_cast<double>(l_vRotation[1])); //Finger2Rot: x / get(2).asDouble()

    m_oHandTrackingPortLeft.write();


    // RIGHT HAND
    //      HAND CARTESIAN
    yarp::os::Bottle &l_oHandCartesianBottleRight = m_oHandCartesianTrackingPortRight.prepare();
    l_oHandCartesianBottleRight.clear();
        l_oHandCartesianBottleRight.addInt(swTracking::LEAP_LIB);                 // Right hand : LEAP_LIB id / get(0).asInt()
        m_oLeap.directionArm(false, l_vLambda);
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : arm direction x / get(1).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : arm direction y / get(2).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : arm direction z / get(3).asDouble()
        m_oLeap.directionHand(false, l_vLambda);
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : hand direction x / get(4).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : hand direction y / get(5).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : hand direction z / get(6).asDouble()
        m_oLeap.directionHandEuclidian(false, l_vLambda);
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : hand direction pitch / get(7).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : hand direction roll  / get(8).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : hand direction yaw   / get(9).asDouble()
        m_oLeap.coordPalmHand(false, l_vLambda);
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : hand palm coord x / get(10).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : hand palm coord y / get(11).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : hand palm coord z / get(12).asDouble()
        m_oLeap.normalPalmHand(false, l_vLambda);
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : hand palm normal x / get(13).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : hand palm normal y / get(14).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : hand palm normal z / get(15).asDouble()
        m_oLeap.normalPalmHandEuclidian(false, l_vLambda);
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : hand palm normal pitch / get(16).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : hand palm normal roll  / get(17).asDouble()
        l_oHandCartesianBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : hand palm normal yaw   / get(18).asDouble()
    m_oHandCartesianTrackingPortRight.write();

    //      HAND  -> copy current cartesian hand
    yarp::os::Bottle &l_oHandBottleRight = m_oHandTrackingPortRight.prepare();
        l_oHandBottleRight.copy(l_oHandCartesianBottleRight);

        // ...

    m_oHandTrackingPortRight.write();

    return true;
}


int main(int argc, char* argv[])
{

    // initialize yarp network 
    Network l_oYarp;
    if (!l_oYarp.checkNetwork())
    {
        std::cerr << "-ERROR: Problem connecting to YARP server" << std::endl;
        return -1;
    }
	
    SWLeapTracking l_oLeapTracking;

    if(!l_oLeapTracking.isLeapInitialized())
    {
        std::cerr << "-ERROR: Failed to init the Leap module. " << std::endl;
        return 0;
    }
	
    std::cout << "Starting the Leap tracking module..." << std::endl;
    l_oLeapTracking.runModule();

    return 0;
}

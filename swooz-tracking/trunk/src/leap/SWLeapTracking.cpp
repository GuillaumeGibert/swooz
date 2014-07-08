
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

//          HAND  -> copy current cartesian hand
    yarp::os::Bottle &l_oHandBottleLeft = m_oHandTrackingPortLeft.prepare();
    l_oHandBottleLeft.clear();
        l_oHandBottleLeft.copy(l_oHandCartesianBottleLeft);
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_THUMB, Leap::Bone::TYPE_PROXIMAL, l_vLambda);
//        std::cout << "1 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << " ";
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : thumb metacarpal direction x / get(19).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : thumb metacarpal direction y / get(20).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : thumb metacarpal direction z / get(21).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_THUMB, Leap::Bone::TYPE_INTERMEDIATE, l_vLambda);
//        std::cout << "2 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << " ";
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : thumb proximal direction x / get(22).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : thumb proximal direction y / get(23).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : thumb proximal direction z / get(24).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_THUMB, Leap::Bone::TYPE_DISTAL, l_vLambda);
//        std::cout << "3 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << std::endl;
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : thumb distal direction x / get(25).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : thumb distal direction y / get(26).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : thumb distal direction z / get(27).asDouble()

        m_oLeap.boneDirection(true, Leap::Finger::TYPE_INDEX, Leap::Bone::TYPE_METACARPAL, l_vLambda);
//        std::cout << "4 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << std::endl;
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : index metacarpal direction x / get(28).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : index metacarpal direction y / get(29).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : index metacarpal direction z / get(30).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_INDEX, Leap::Bone::TYPE_PROXIMAL, l_vLambda);
//        std::cout << "5 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << std::endl;
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : index proximal direction x / get(31).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : index proximal direction y / get(32).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : index proximal direction z / get(33).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_INDEX, Leap::Bone::TYPE_INTERMEDIATE, l_vLambda);
//        std::cout << "6 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << std::endl;
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : index intermediate direction x / get(34).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : index intermediate direction y / get(35).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : index intermediate direction z / get(36).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_INDEX, Leap::Bone::TYPE_DISTAL, l_vLambda);
//        std::cout << "7 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << std::endl;
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : index distal direction x / get(37).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : index distal direction y / get(38).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : index distal direction z / get(39).asDouble()

        m_oLeap.boneDirection(true, Leap::Finger::TYPE_MIDDLE, Leap::Bone::TYPE_METACARPAL, l_vLambda);
//        std::cout << "8 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << std::endl;
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : index metacarpal direction x / get(40).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : index metacarpal direction y / get(41).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : index metacarpal direction z / get(42).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_MIDDLE, Leap::Bone::TYPE_PROXIMAL, l_vLambda);
//        std::cout << "9 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << std::endl;
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : middle proximal direction x / get(43).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : middle proximal direction y / get(44).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : middle proximal direction z / get(45).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_MIDDLE, Leap::Bone::TYPE_INTERMEDIATE, l_vLambda);
//        std::cout << "10 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << std::endl;
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : middle intermediate direction x / get(46).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : middle intermediate direction y / get(47).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : middle intermediate direction z / get(48).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_MIDDLE, Leap::Bone::TYPE_DISTAL, l_vLambda);
//        std::cout << "11 -> " << l_vLambda[0] << " " << l_vLambda[1] << " " << l_vLambda[2] << std::endl;
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : middle distal direction x / get(49).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : middle distal direction y / get(50).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : middle distal direction z / get(51).asDouble()

        m_oLeap.boneDirection(true, Leap::Finger::TYPE_RING, Leap::Bone::TYPE_METACARPAL, l_vLambda);
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : ring metacarpal direction x / get(52).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : ring metacarpal direction y / get(53).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : ring metacarpal direction z / get(54).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_RING, Leap::Bone::TYPE_PROXIMAL, l_vLambda);
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : ring proximal direction x / get(55).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : ring proximal direction y / get(56).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : ring proximal direction z / get(57).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_RING, Leap::Bone::TYPE_INTERMEDIATE, l_vLambda);
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : ring intermediate direction x / get(58).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : ring intermediate direction y / get(59).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : ring intermediate direction z / get(60).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_RING, Leap::Bone::TYPE_DISTAL, l_vLambda);
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : ring distal direction x / get(61).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : ring distal direction y / get(62).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : ring distal direction z / get(63).asDouble()

        m_oLeap.boneDirection(true, Leap::Finger::TYPE_PINKY, Leap::Bone::TYPE_METACARPAL, l_vLambda);
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : pinky metacarpal direction x / get(64).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : pinky metacarpal direction y / get(65).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : pinky metacarpal direction z / get(66).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_PINKY, Leap::Bone::TYPE_PROXIMAL, l_vLambda);
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : pinky proximal direction x / get(67).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : pinky proximal direction y / get(68).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : pinky proximal direction z / get(69).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_PINKY, Leap::Bone::TYPE_INTERMEDIATE, l_vLambda);
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : pinky intermediate direction x / get(70).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : pinky intermediate direction y / get(71).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : pinky intermediate direction z / get(72).asDouble()
        m_oLeap.boneDirection(true, Leap::Finger::TYPE_PINKY, Leap::Bone::TYPE_DISTAL, l_vLambda);
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[0])); // Left hand : pinky distal direction x / get(73).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[1])); // Left hand : pinky distal direction y / get(74).asDouble()
        l_oHandBottleLeft.addDouble(static_cast<double>(l_vLambda[2])); // Left hand : pinky distal direction z / get(75).asDouble()
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
    l_oHandBottleRight.clear();
        l_oHandBottleRight.copy(l_oHandCartesianBottleRight);
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_THUMB, Leap::Bone::TYPE_METACARPAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : thumb metacarpal direction x / get(19).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : thumb metacarpal direction y / get(20).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : thumb metacarpal direction z / get(21).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_THUMB, Leap::Bone::TYPE_PROXIMAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : thumb proximal direction x / get(22).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : thumb proximal direction y / get(23).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : thumb proximal direction z / get(24).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_THUMB, Leap::Bone::TYPE_DISTAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : thumb distal direction x / get(25).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : thumb distal direction y / get(26).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : thumb distal direction z / get(27).asDouble()

        m_oLeap.boneDirection(false, Leap::Finger::TYPE_INDEX, Leap::Bone::TYPE_METACARPAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : index metacarpal direction x / get(28).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : index metacarpal direction y / get(29).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : index metacarpal direction z / get(30).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_INDEX, Leap::Bone::TYPE_PROXIMAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : index proximal direction x / get(31).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : index proximal direction y / get(32).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : index proximal direction z / get(33).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_INDEX, Leap::Bone::TYPE_INTERMEDIATE, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : index intermediate direction x / get(34).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : index intermediate direction y / get(35).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : index intermediate direction z / get(36).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_INDEX, Leap::Bone::TYPE_DISTAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : index distal direction x / get(37).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : index distal direction y / get(38).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : index distal direction z / get(39).asDouble()

        m_oLeap.boneDirection(false, Leap::Finger::TYPE_MIDDLE, Leap::Bone::TYPE_METACARPAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : index metacarpal direction x / get(40).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : index metacarpal direction y / get(41).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : index metacarpal direction z / get(42).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_MIDDLE, Leap::Bone::TYPE_PROXIMAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : middle proximal direction x / get(43).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : middle proximal direction y / get(44).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : middle proximal direction z / get(45).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_MIDDLE, Leap::Bone::TYPE_INTERMEDIATE, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : middle intermediate direction x / get(46).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : middle intermediate direction y / get(47).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : middle intermediate direction z / get(48).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_MIDDLE, Leap::Bone::TYPE_DISTAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : middle distal direction x / get(49).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : middle distal direction y / get(50).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : middle distal direction z / get(51).asDouble()

        m_oLeap.boneDirection(false, Leap::Finger::TYPE_RING, Leap::Bone::TYPE_METACARPAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : ring metacarpal direction x / get(52).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : ring metacarpal direction y / get(53).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : ring metacarpal direction z / get(54).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_RING, Leap::Bone::TYPE_PROXIMAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : ring proximal direction x / get(55).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : ring proximal direction y / get(56).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : ring proximal direction z / get(57).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_RING, Leap::Bone::TYPE_INTERMEDIATE, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : ring intermediate direction x / get(58).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : ring intermediate direction y / get(59).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : ring intermediate direction z / get(60).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_RING, Leap::Bone::TYPE_DISTAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : ring distal direction x / get(61).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : ring distal direction y / get(62).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : ring distal direction z / get(63).asDouble()

        m_oLeap.boneDirection(false, Leap::Finger::TYPE_PINKY, Leap::Bone::TYPE_METACARPAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : pinky metacarpal direction x / get(64).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : pinky metacarpal direction y / get(65).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : pinky metacarpal direction z / get(66).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_PINKY, Leap::Bone::TYPE_PROXIMAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : pinky proximal direction x / get(67).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : pinky proximal direction y / get(68).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : pinky proximal direction z / get(69).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_PINKY, Leap::Bone::TYPE_INTERMEDIATE, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : pinky intermediate direction x / get(70).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : pinky intermediate direction y / get(71).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : pinky intermediate direction z / get(72).asDouble()
        m_oLeap.boneDirection(false, Leap::Finger::TYPE_PINKY, Leap::Bone::TYPE_DISTAL, l_vLambda);
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[0])); // Right hand : pinky distal direction x / get(73).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[1])); // Right hand : pinky distal direction y / get(74).asDouble()
        l_oHandBottleRight.addDouble(static_cast<double>(l_vLambda[2])); // Right hand : pinky distal direction z / get(75).asDouble()
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

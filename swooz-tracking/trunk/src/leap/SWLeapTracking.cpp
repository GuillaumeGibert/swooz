
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
    std::string l_sEffector    = "left_hand";

    // set ports name
		m_sFingerTrackingPortNameLeft =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/left_arm/fingers";
		m_sHandTrackingPortNameLeft =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/left_arm/hand";
		m_sArmTrackingPortNameLeft =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/left_arm";	


		m_sFingerTrackingPortNameRight =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/right_arm/fingers";
		m_sHandTrackingPortNameRight =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/right_arm/hand";
		m_sArmTrackingPortNameRight =  "/tracking/" + l_sDeviceName + "/"+ l_sLibraryName + "/right_arm";	


    m_oFingerTrackingPortLeft.open(m_sFingerTrackingPortNameLeft.c_str());
	m_oHandTrackingPortLeft.open(m_sHandTrackingPortNameLeft.c_str());
	m_oArmTrackingPortLeft.open(m_sArmTrackingPortNameLeft.c_str());

	m_oFingerTrackingPortRight.open(m_sFingerTrackingPortNameRight.c_str());
	m_oHandTrackingPortRight.open(m_sHandTrackingPortNameRight.c_str());
	m_oArmTrackingPortRight.open(m_sArmTrackingPortNameRight.c_str());


    initLeap();
	if(!isLeapInitialized())
	{
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
//    m_oLeap.stop();
	m_oFingerTrackingPortLeft.close();
	m_oHandTrackingPortLeft.close();
	m_oArmTrackingPortLeft.close();

	m_oFingerTrackingPortRight.close();
	m_oHandTrackingPortRight.close();
	m_oArmTrackingPortRight.close();

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
//    m_oLeap.read();
    m_oLeap.grab();


	//ARM PART ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Left Arm Part
		Bottle &l_oArmBottleLeft = m_oArmTrackingPortLeft.prepare();
		l_oArmBottleLeft.clear();
			// device lib id
			l_oArmBottleLeft.addInt(swTracking::LEAP_LIB); //Arm : LEAP_LIB id / get(0).asInt()
			m_oArmTrackingPortLeft.write();

		//Right Arm Part
		Bottle &l_oArmBottleRight = m_oArmTrackingPortRight.prepare();
		l_oArmBottleRight.clear();
			// device lib id
			l_oArmBottleRight.addInt(swTracking::LEAP_LIB); //Arm : LEAP_LIB id / get(0).asInt()
			m_oArmTrackingPortRight.write();
	//    X/Y/Z of the LEAP Space State

	//FINGER PART////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Left Finger Part
		Bottle &l_oFingerBottleLeft = m_oFingerTrackingPortLeft.prepare();
		l_oFingerBottleLeft.clear();

			// device lib id
			l_oFingerBottleLeft.addInt(swTracking::LEAP_LIB); //Finger : LEAP_LIB id / get(0).asInt()

            std::vector<float> l_vRotation(2,0.f);
            m_oLeap.fingerRotation(true, Leap::Finger::TYPE_THUMB, l_vRotation);
//			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(1,1,true)); //Finger2Rot: x / get(1).asDouble()
//			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(1,2,true)); //Finger2Rot: x / get(2).asDouble()
            l_oFingerBottleLeft.addDouble(l_vRotation[0]); //Finger2Rot: x / get(1).asDouble()
            l_oFingerBottleLeft.addDouble(l_vRotation[1]); //Finger2Rot: x / get(2).asDouble()
			
            m_oLeap.fingerRotation(true, Leap::Finger::TYPE_INDEX, l_vRotation);
//			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(2,1,true)); //Finger2Rot: x / get(3).asDouble()
//			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(2,2,true)); //Finger2Rot: x / get(4).asDouble()
            l_oFingerBottleLeft.addDouble(l_vRotation[0]); //Finger2Rot: x / get(1).asDouble()
            l_oFingerBottleLeft.addDouble(l_vRotation[1]); //Finger2Rot: x / get(2).asDouble()

            m_oLeap.fingerRotation(true, Leap::Finger::TYPE_MIDDLE, l_vRotation);
//			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(3,1,true)); //Finger2Rot: x / get(5).asDouble()
//			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(3,2,true)); //Finger2Rot: x / get(6).asDouble()
            l_oFingerBottleLeft.addDouble(l_vRotation[0]); //Finger2Rot: x / get(1).asDouble()
            l_oFingerBottleLeft.addDouble(l_vRotation[1]); //Finger2Rot: x / get(2).asDouble()

            m_oLeap.fingerRotation(true, Leap::Finger::TYPE_RING, l_vRotation);
//			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(4,1,true)); //Finger2Rot: x / get(7).asDouble()
//			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(4,2,true)); //Finger2Rot: x / get(8).asDouble()
            l_oFingerBottleLeft.addDouble(l_vRotation[0]); //Finger2Rot: x / get(1).asDouble()
            l_oFingerBottleLeft.addDouble(l_vRotation[1]); //Finger2Rot: x / get(2).asDouble()

            m_oLeap.fingerRotation(true, Leap::Finger::TYPE_PINKY, l_vRotation);
//			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(5,1,true)); //Finger2Rot: x / get(9).asDouble()
//			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(5,2,true)); //Finger2Rot: x / get(10).asDouble()
            l_oFingerBottleLeft.addDouble(l_vRotation[0]); //Finger2Rot: x / get(1).asDouble()
            l_oFingerBottleLeft.addDouble(l_vRotation[1]); //Finger2Rot: x / get(2).asDouble()
			
            l_oFingerBottleLeft.addDouble(static_cast<double>(0));  // TODO : eurgh
			// Others
			
				//Frame per Second
//                l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFPS())); //FramePerSecond :  get(12).asDouble() // TODO : eurgh
            l_oFingerBottleLeft.addDouble(m_oLeap.fps()); //FramePerSecond :  get(12).asDouble() // TODO : eurgh
       
		m_oFingerTrackingPortLeft.write();

				//Right Finger Part
		Bottle &l_oFingerBottleRight = m_oFingerTrackingPortRight.prepare();
		l_oFingerBottleRight.clear();

			// device lib id
			l_oFingerBottleRight.addInt(swTracking::LEAP_LIB); //Finger : LEAP_LIB id / get(0).asInt()


            m_oLeap.fingerRotation(false, Leap::Finger::TYPE_THUMB, l_vRotation);

//			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(1,1,false)); //Finger2Rot: x / get(1).asDouble()
//			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(1,2,false)); //Finger2Rot: x / get(2).asDouble()
            l_oFingerBottleRight.addDouble(l_vRotation[0]); //Finger2Rot: x / get(1).asDouble()
            l_oFingerBottleRight.addDouble(l_vRotation[1]); //Finger2Rot: x / get(2).asDouble()

            m_oLeap.fingerRotation(false, Leap::Finger::TYPE_INDEX, l_vRotation);
//			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(2,1,false)); //Finger2Rot: x / get(3).asDouble()
//			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(2,2,false)); //Finger2Rot: x / get(4).asDouble()
            l_oFingerBottleRight.addDouble(l_vRotation[0]); //Finger2Rot: x / get(1).asDouble()
            l_oFingerBottleRight.addDouble(l_vRotation[1]); //Finger2Rot: x / get(2).asDouble()

            m_oLeap.fingerRotation(false, Leap::Finger::TYPE_MIDDLE, l_vRotation);
//			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(3,1,false)); //Finger2Rot: x / get(5).asDouble()
//			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(3,2,false)); //Finger2Rot: x / get(6).asDouble()
            l_oFingerBottleRight.addDouble(l_vRotation[0]); //Finger2Rot: x / get(1).asDouble()
            l_oFingerBottleRight.addDouble(l_vRotation[1]); //Finger2Rot: x / get(2).asDouble()

            m_oLeap.fingerRotation(false, Leap::Finger::TYPE_RING, l_vRotation);
//			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(4,1,false)); //Finger2Rot: x / get(7).asDouble()
//			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(4,2,false)); //Finger2Rot: x / get(8).asDouble()
            l_oFingerBottleRight.addDouble(l_vRotation[0]); //Finger2Rot: x / get(1).asDouble()
            l_oFingerBottleRight.addDouble(l_vRotation[1]); //Finger2Rot: x / get(2).asDouble()

            m_oLeap.fingerRotation(false, Leap::Finger::TYPE_PINKY, l_vRotation);
//			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(5,1,false)); //Finger2Rot: x / get(9).asDouble()
//			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(5,2,false)); //Finger2Rot: x / get(10).asDouble()
            l_oFingerBottleRight.addDouble(l_vRotation[0]); //Finger2Rot: x / get(1).asDouble()
            l_oFingerBottleRight.addDouble(l_vRotation[1]); //Finger2Rot: x / get(2).asDouble()

            l_oFingerBottleRight.addDouble(static_cast<double>(0));  //11 // TODO : eurgh
			// Others

				//Frame per Second
//                l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFPS())); //FramePerSecond :  get(35).asDouble() // TODO : eurgh
                l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.fps())); //FramePerSecond :  get(35).asDouble() // TODO : eurgh
       
		m_oFingerTrackingPortRight.write();


	//HAND PART ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//left Hand Part
		Bottle &l_oHandBottleLeft =  m_oHandTrackingPortLeft.prepare();
		l_oHandBottleLeft.clear();

			// device lib id
			l_oHandBottleLeft.addInt(swTracking::LEAP_LIB); //Finger : LEAP_LIB id / get(0).asInt()

			 // Palm Position
            std::vector<float> l_vPalmCoord(3,0.f);
            m_oLeap.coordPalmHand(true, l_vPalmCoord);

//			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getPalmCoords(0,true))); //hand: x / get(1).asDouble()
//			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getPalmCoords(1,true))); //hand: y / get(2).asDouble()
//			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getPalmCoords(2,true))); //hand: z / get(3).asDouble()
            l_oHandBottleLeft.addDouble(l_vPalmCoord[0]); //hand: x / get(1).asDouble()
            l_oHandBottleLeft.addDouble(l_vPalmCoord[1]); //hand: y / get(2).asDouble()
            l_oHandBottleLeft.addDouble(l_vPalmCoord[2]); //hand: z / get(3).asDouble()

            std::vector<float> l_vHandRotation(3,0.f);
            m_oLeap.handRotation(true, l_vHandRotation);

			// Rotation - RAD
//			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getHandPitch(true))); //hand: In RAD, rotation on X axis / get(4).asDouble()
//			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getHandRoll(true)));  //hand: In RAD, rotation on Z axis / get(5).asDouble()
//			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getHandYaw(true)));   //hand: In RAD, rotation on Y axis / get(6).asDouble()
            l_oHandBottleLeft.addDouble(l_vHandRotation[0]); //hand: In RAD, rotation on X axis / get(4).asDouble()
            l_oHandBottleLeft.addDouble(l_vHandRotation[1]); //hand: In RAD, rotation on Z axis / get(5).asDouble()
            l_oHandBottleLeft.addDouble(l_vHandRotation[2]); //hand: In RAD, rotation on Y axis / get(6).asDouble()


			// Others

			//Frame per Second
//            l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getFPS())); //FramePerSecond :  get(7).asDouble() // TODO : eurgh
            l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.fps())); //FramePerSecond :  get(7).asDouble()

		m_oHandTrackingPortLeft.write();

			//Right Hand Part
		Bottle &l_oHandBottleRight =  m_oHandTrackingPortRight.prepare();
		l_oHandBottleRight.clear();

			// device lib id
			l_oHandBottleRight.addInt(swTracking::LEAP_LIB); //Finger : LEAP_LIB id / get(0).asInt()


            m_oLeap.coordPalmHand(false, l_vPalmCoord);

			 // Palm Position
//			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getPalmCoords(0,false))); //hand: x / get(1).asDouble()
//			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getPalmCoords(1,false))); //hand: y / get(2).asDouble()
//			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getPalmCoords(2,false))); //hand: z / get(3).asDouble()
            l_oHandBottleRight.addDouble(l_vPalmCoord[0]); //hand: x / get(1).asDouble()
            l_oHandBottleRight.addDouble(l_vPalmCoord[1]); //hand: y / get(2).asDouble()
            l_oHandBottleRight.addDouble(l_vPalmCoord[2]); //hand: z / get(3).asDouble()

            m_oLeap.handRotation(false, l_vHandRotation);

			//Rotation - RAD
//			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getHandPitch(false))); //hand: In RAD, rotation on X axis / get(4).asDouble()
//			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getHandRoll(false)));  //hand: In RAD, rotation on Z axis / get(5).asDouble()
//			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getHandYaw(false)));   //hand: In RAD, rotation on Y axis / get(6).asDouble()
            l_oHandBottleRight.addDouble(l_vHandRotation[0]); //hand: In RAD, rotation on X axis / get(4).asDouble()
            l_oHandBottleRight.addDouble(l_vHandRotation[1]); //hand: In RAD, rotation on Z axis / get(5).asDouble()
            l_oHandBottleRight.addDouble(l_vHandRotation[2]); //hand: In RAD, rotation on Y axis / get(6).asDouble()

			// Others

			//Frame per Second
//            l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getFPS())); //FramePerSecond :  get(7).asDouble() // TODO : eurgh
            l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.fps())); //FramePerSecond :  get(7).asDouble() // TODO : eurgh

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

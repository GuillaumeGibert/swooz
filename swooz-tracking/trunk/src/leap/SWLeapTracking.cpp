
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
    std::string l_sDeviceName  = "Leap";
    std::string l_sLibraryName = "LeapSDK";
	std::string l_sEffector = "left_hand";

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
	std::cout<<"ENTRER DANS Close"<<std::endl;
    m_oLeap.stop();
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
    m_oLeap.read();
    //  Time::delay(0.001);
		
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


			/*//FINGER POSITIONS

				// Finger 1 Position
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(0,0,true))); //Finger1Pos: x / get(1).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(0,1,true))); //Finger1Pos: y / get(2).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(0,2,true))); //Finger1Pos: z / get(3).asDouble()

				// Finger 2 Position
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(1,0,true))); //Finger2Pos: x / get(4).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(1,1,true))); //Finger2Pos: y / get(5).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(1,2,true))); //Finger2Pos: z / get(6).asDouble()

				// Finger 3 Position
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(2,0,true))); //Finger3Pos: x / get(7).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(2,1,true))); //Finger3Pos: y / get(8).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(2,2,true))); //Finger3Pos: z / get(9).asDouble()

				// Finger 4 Position
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(3,0,true))); //Finger4Pos: x / get(10).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(3,1,true))); //Finger4Pos: y / get(11).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(3,2,true))); //Finger4Pos: z / get(12).asDouble()

				// Finger 5 Position
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(4,0,true))); //Finger5Pos: x / get(13).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(4,1,true))); //Finger5Pos: y / get(14).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerCoords(4,2,true))); //Finger5Pos: z / get(15).asDouble()

			//FINGER DIRECTIONS

				// Finger 1 Direction
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(0,1,true))); //Finger1Dir: x / get(16).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(0,1,true))); //Finger1Dir: y / get(17).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(0,2,true))); //Finger1Dir: z / get(18).asDouble()

				// Finger 2 Direction
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(1,0,true))); //Finger2Dir: x / get(19).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(1,1,true))); //Finger2Dir: y / get(20).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(1,2,true))); //Finger2Dir: z / get(21).asDouble()

				// Finger 3 Direction
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(2,0,true))); //Finger3Dir: x / get(22).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(2,1,true))); //Finger3Dir: y / get(23).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(2,2,true))); //Finger3Dir: z / get(24).asDouble()

				// Finger 4 Direction
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(3,0,true))); //Finger4Dir: x / get(25).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(3,1,true))); //Finger4Dir: y / get(26).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(3,2,true))); //Finger4Dir: z / get(27).asDouble()

				// Finger 5 Direction
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(4,0,true))); //Finger5Dir: x / get(28).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(4,1,true))); //Finger5Dir: y / get(29).asDouble()
				l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFingerDirection(4,2,true))); //Finger5Dir: z / get(30).asDouble()
				
				
			//Sphere Center
			l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getHandSphereCoords(0,true))); //hand: x / center of the sphere / get(31).asDouble()
			l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getHandSphereCoords(1,true))); //hand: y / center of the sphere / get(32).asDouble()
			l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getHandSphereCoords(2,true))); //hand: z / center of the sphere / get(33).asDouble()	

			//Sphere Radius
			l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getHandSphereRadius(true))); //hand: Give the radius of the sphere taken by the hand / get(34).asDouble()	
			*/
			
			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(1,1,true)); //Finger2Rot: x / get(1).asDouble()
			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(1,2,true)); //Finger2Rot: x / get(2).asDouble()
			
			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(2,1,true)); //Finger2Rot: x / get(3).asDouble()
			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(2,2,true)); //Finger2Rot: x / get(4).asDouble()

			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(3,1,true)); //Finger2Rot: x / get(5).asDouble()
			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(3,2,true)); //Finger2Rot: x / get(6).asDouble()

			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(4,1,true)); //Finger2Rot: x / get(7).asDouble()
			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(4,2,true)); //Finger2Rot: x / get(8).asDouble()

			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(5,1,true)); //Finger2Rot: x / get(9).asDouble()
			l_oFingerBottleLeft.addDouble(m_oLeap.getFingerRotation(5,2,true)); //Finger2Rot: x / get(10).asDouble()
			
            l_oFingerBottleLeft.addDouble(static_cast<double>(0));  // TODO : eurgh
			// Others
			
				//Frame per Second
                l_oFingerBottleLeft.addDouble(static_cast<double>(m_oLeap.getFPS())); //FramePerSecond :  get(12).asDouble() // TODO : eurgh
       
		m_oFingerTrackingPortLeft.write();

				//Right Finger Part
		Bottle &l_oFingerBottleRight = m_oFingerTrackingPortRight.prepare();
		l_oFingerBottleRight.clear();

			// device lib id
			l_oFingerBottleRight.addInt(swTracking::LEAP_LIB); //Finger : LEAP_LIB id / get(0).asInt()
			/*

			//FINGER POSITIONS

				// Finger 1 Position
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(0,0,false))); //Finger1Pos: x / get(1).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(0,1,false))); //Finger1Pos: y / get(2).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(0,2,false))); //Finger1Pos: z / get(3).asDouble()
				
				// Finger 2 Position
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(1,0,false))); //Finger2Pos: x / get(4).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(1,1,false))); //Finger2Pos: y / get(5).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(1,2,false))); //Finger2Pos: z / get(6).asDouble()

				// Finger 3 Position
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(2,0,false))); //Finger3Pos: x / get(7).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(2,1,false))); //Finger3Pos: y / get(8).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(2,2,false))); //Finger3Pos: z / get(9).asDouble()

				// Finger 4 Position
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(3,0,false))); //Finger4Pos: x / get(10).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(3,1,false))); //Finger4Pos: y / get(11).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(3,2,false))); //Finger4Pos: z / get(12).asDouble()

				// Finger 5 Position
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(4,0,false))); //Finger5Pos: x / get(13).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(4,1,false))); //Finger5Pos: y / get(14).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerCoords(4,2,false))); //Finger5Pos: z / get(15).asDouble()

			//FINGER DIRECTIONS

				// Finger 1 Direction
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(0,1,false))); //Finger1Dir: x / get(16).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(0,1,false))); //Finger1Dir: y / get(17).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(0,2,false))); //Finger1Dir: z / get(18).asDouble()

				// Finger 2 Direction
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(1,0,false))); //Finger2Dir: x / get(19).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(1,1,false))); //Finger2Dir: y / get(20).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(1,2,false))); //Finger2Dir: z / get(21).asDouble()

				// Finger 3 Direction
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(2,0,false))); //Finger3Dir: x / get(22).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(2,1,false))); //Finger3Dir: y / get(23).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(2,2,false))); //Finger3Dir: z / get(24).asDouble()

				// Finger 4 Direction
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(3,0,false))); //Finger4Dir: x / get(25).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(3,1,false))); //Finger4Dir: y / get(26).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(3,2,false))); //Finger4Dir: z / get(27).asDouble()

				// Finger 5 Direction
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(4,0,false))); //Finger5Dir: x / get(28).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(4,1,false))); //Finger5Dir: y / get(29).asDouble()
				l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFingerDirection(4,2,false))); //Finger5Dir: z / get(30).asDouble()


			//Sphere Center
			l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getHandSphereCoords(0,false))); //hand: x / center of the sphere / get(31).asDouble()
			l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getHandSphereCoords(1,false))); //hand: y / center of the sphere / get(32).asDouble()
			l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getHandSphereCoords(2,false))); //hand: z / center of the sphere / get(33).asDouble()	

			//Sphere Radius
			l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getHandSphereRadius(false))); //hand: Give the radius of the sphere taken by the hand / get(34).asDouble()	



			*/
			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(1,1,false)); //Finger2Rot: x / get(1).asDouble()
			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(1,2,false)); //Finger2Rot: x / get(2).asDouble()

			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(2,1,false)); //Finger2Rot: x / get(3).asDouble()
			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(2,2,false)); //Finger2Rot: x / get(4).asDouble()

			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(3,1,false)); //Finger2Rot: x / get(5).asDouble()
			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(3,2,false)); //Finger2Rot: x / get(6).asDouble()

			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(4,1,false)); //Finger2Rot: x / get(7).asDouble()
			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(4,2,false)); //Finger2Rot: x / get(8).asDouble()

			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(5,1,false)); //Finger2Rot: x / get(9).asDouble()
			l_oFingerBottleRight.addDouble(m_oLeap.getFingerRotation(5,2,false)); //Finger2Rot: x / get(10).asDouble()

            l_oFingerBottleRight.addDouble(static_cast<double>(0));  //11 // TODO : eurgh
			// Others

				//Frame per Second
                l_oFingerBottleRight.addDouble(static_cast<double>(m_oLeap.getFPS())); //FramePerSecond :  get(35).asDouble() // TODO : eurgh
       
		m_oFingerTrackingPortRight.write();


	//HAND PART ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//left Hand Part
		Bottle &l_oHandBottleLeft =  m_oHandTrackingPortLeft.prepare();
		l_oHandBottleLeft.clear();

			// device lib id
			l_oHandBottleLeft.addInt(swTracking::LEAP_LIB); //Finger : LEAP_LIB id / get(0).asInt()

			 // Palm Position
			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getPalmCoords(0,true))); //hand: x / get(1).asDouble()
			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getPalmCoords(1,true))); //hand: y / get(2).asDouble()
			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getPalmCoords(2,true))); //hand: z / get(3).asDouble()

			// Rotation - RAD
			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getHandPitch(true))); //hand: In RAD, rotation on X axis / get(4).asDouble()
			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getHandRoll(true)));  //hand: In RAD, rotation on Z axis / get(5).asDouble()
			l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getHandYaw(true)));   //hand: In RAD, rotation on Y axis / get(6).asDouble()	

			// 


			// Others

			//Frame per Second
            l_oHandBottleLeft.addDouble(static_cast<double>(m_oLeap.getFPS())); //FramePerSecond :  get(7).asDouble() // TODO : eurgh

		m_oHandTrackingPortLeft.write();

			//Right Hand Part
		Bottle &l_oHandBottleRight =  m_oHandTrackingPortRight.prepare();
		l_oHandBottleRight.clear();

			// device lib id
			l_oHandBottleRight.addInt(swTracking::LEAP_LIB); //Finger : LEAP_LIB id / get(0).asInt()

			 // Palm Position
			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getPalmCoords(0,false))); //hand: x / get(1).asDouble()
			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getPalmCoords(1,false))); //hand: y / get(2).asDouble()
			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getPalmCoords(2,false))); //hand: z / get(3).asDouble()

			//Rotation - RAD
			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getHandPitch(false))); //hand: In RAD, rotation on X axis / get(4).asDouble()
			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getHandRoll(false)));  //hand: In RAD, rotation on Z axis / get(5).asDouble()
			l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getHandYaw(false)));   //hand: In RAD, rotation on Y axis / get(6).asDouble()	

			// Others

			//Frame per Second
            l_oHandBottleRight.addDouble(static_cast<double>(m_oLeap.getFPS())); //FramePerSecond :  get(7).asDouble() // TODO : eurgh

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
	
    // create module
    SWLeapTracking l_oLeapTracking;

    if(!l_oLeapTracking.isLeapInitialized())
    {
        std::cerr << "-ERROR: Failed to init the Leap module. " << std::endl;
        return 0;
    }
	
    // prepare and configure the resource finder
    //ResourceFinder rf;
    //    rf.setVerbose(true);
    //    rf.setDefaultConfigFile("LeapTracking.ini");
    //    rf.setDefaultContext("swtracking/conf");
    //    rf.configure("ICUB_ROOT", argc, argv);

    //// configure the module
    //std::cout << "Configuring the Leap tracking module..."<< std::endl;
    //if (l_oLeapTracking.configure(rf))
    //{
        // run the module
        std::cout << "Starting the Leap tracking module..." << std::endl;
        l_oLeapTracking.runModule();
    //}
    //else
    //{
    //    std::cerr << "Failed to configure the Leap tracking module!"<< std::endl;
    //}
	


    return 0;
}

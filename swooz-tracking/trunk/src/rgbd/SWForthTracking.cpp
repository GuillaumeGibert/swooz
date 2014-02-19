/**
 * \file ForthTracking.cpp
 * \brief Defines ForthTracking class
 * \author Emmanuel Body
 * \date 07/01/13
 */

#include "rgbd/SWForthTracking.h"
#include "SWTrackingDevice.h"

using namespace FORTH;
using namespace openni;
/**
 * Creates the module which will connect to the
 * Hand Tracking library for data retrieval and which will send data
 * to yarp ports.
 */
SWForthTracking::SWForthTracking()
{}


/**
 * Deletes all instances and closes any opened connections
 * with the Hand Tracking library and yarp ports.
 */
SWForthTracking::~SWForthTracking()
{}


/**
 * Configures the module using a ResourceFinder object
 */
bool SWForthTracking::configure(yarp::os::ResourceFinder &rf)
{
	// gets the module name which will form the stem of all module port names
	moduleName = rf.check("name", Value("handTracker"), "Module name (string)").asString();
	setName(moduleName.c_str());

	robotName  = rf.check("robot",  Value("icub"),  "Robot name (string)").asString();
	effectorName = rf.check("effector",  Value("left_arm"),  "Effector name (string)").asString();
	deviceName = rf.check("device",  Value("rgbd"),  "Device name (string)").asString();
	handScale = rf.check("handScale",  Value("1.5"),  "Hand scale (float)").asDouble();
	offsetTx = rf.check("forthOffsetTx",  Value("300"),  "Offset Tx between the tracker and iCub referentials (float)").asDouble();
	offsetTy = rf.check("forthOffsetTy",  Value("200"),  "Offset Ty between the tracker and iCub referentials (float)").asDouble();
	offsetTz = rf.check("forthOffsetTz",  Value("100"),  "Offset Tz between the tracker and iCub referentials (float)").asDouble();
	initialValueJoint4 = rf.check("initialValueJoint4",  Value("20"),  "Initial Joint4 Value (int)").asInt();
	initialValueJoint5 = rf.check("initialValueJoint5",  Value("20"),  "Initial Joint5 Value (int)").asInt();
	initialValueJoint6 = rf.check("initialValueJoint6",  Value("20"),  "Initial Joint6 Value (int)").asInt();

	// creates the yarp port that will receive the data from the FORTH library
	handTrackerPortName =  "/tracking/" + deviceName + "/forth/" + effectorName +"/hand";
	fingersTrackerPortName =  "/tracking/" + deviceName + "/forth/" + effectorName +"/fingers";
	handTrackerPort.open(handTrackerPortName.c_str());
	fingersTrackerPort.open(fingersTrackerPortName.c_str());

	// initializes the FORTH library
	try
	{
		LOG("-->Initialize OpenNI");
		if(OpenNI::initialize()!=STATUS_OK)
		{
			throw std::runtime_error(std::string("<!!>OpenNI2 Initialization Failed: ")+OpenNI::getExtendedError());
		}

		LOG("-->Open the RGBD sensor");

		if (device.open(openni::ANY_DEVICE) != STATUS_OK)
		{
			throw std::runtime_error(std::string("<!!>Couldn't open device ")+ OpenNI::getExtendedError());
		}

		device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR); // The HandTracker expects registered RGB and Depth images

		// Creates depth and color videostream
		if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
		{
			Status rc = depth.create(device, SENSOR_DEPTH);
			if (rc == STATUS_OK)
			{
				if(depth.start()!= STATUS_OK)
				{
					throw std::runtime_error(std::string("<!!>Couldn't start the color stream: ")+ OpenNI::getExtendedError());
				}
			}
			else
			{
				throw std::runtime_error(std::string("<!!>Couldn't create depth stream: ")+ OpenNI::getExtendedError());
			}
		}

		if (device.getSensorInfo(SENSOR_COLOR) != NULL)
		{
			Status rc = color.create(device, SENSOR_COLOR);
			if (rc == STATUS_OK)
			{
				if(color.start() != STATUS_OK)
				{
					throw std::runtime_error(std::string("<!!>Couldn't start the color stream")+ OpenNI::getExtendedError());
				}
			}
			else
			{
				throw std::runtime_error(std::string("<!!>Couldn't create color stream")+ OpenNI::getExtendedError());
			}
		}

		// depth properties
		int zpd;
		double zpps;
		depth.getProperty(XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE,&zpd);
		depth.getProperty(XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE,&zpps);


		LOG("-->Device Initialiazed.");
		LOG("--->Depth (ZPD, ZPPS): ("<<zpd<<", "<<zpps<<")");


		LOG("-->Initializing HandTracker");
		ht = HandTracker::getInstance();
		ht->setSensorParameters(640,480,640,480,zpd,(float)zpps);

		if (effectorName.compare("right_hand") )
		{
			ht->initialize(RIGHT_HAND,(float)handScale);
		}
		else
		{
			ht->initialize(LEFT_HAND,(float)handScale);
		}
		tracking = false;
		LOG("-->HandTracker Initialization Completed");

	}
	catch(std::exception &e)
	{
		LOG("<!!>Exception: " << e.what());
	}
	catch(...)
	{
		LOG("<!!>Unknown Error");
	}

	return true; // let the RFModule know everything went well
}


/**
 * Updates the module periodically every getPeriod() seconds
 */
bool SWForthTracking::updateModule()
{
	char key=cv::waitKey(5);
	// Acquire images and camera parameters from shared memory
	VideoFrameRef depthFrame,colorFrame;

	color.readFrame(&colorFrame);
	depth.readFrame(&depthFrame);

	if(depthFrame.isValid() && colorFrame.isValid())
	{
		// only if both color and depth are available
		const FORTH::DepthPixel* depthP = (FORTH::DepthPixel*)depthFrame.getData();
		RGBPixel* colorP = (RGBPixel*)colorFrame.getData();


		// There is an issue (flicker) with VGA RGB input on OpenNI2 alpha.
		// When fixed, remove the resize and set the device settings to produce VGA RGB input.
		// cv::Mat depth(480,640,CV_16UC1,(void*)depthP,2*640);
		// cv::Mat depthNorm;
		// cv::normalize(depth,depthNorm,0,255,CV_MINMAX,CV_8UC1);
		// cv::imshow("Depth",depthNorm);

		result_vector solution = ht->getLastHandPose();


		float score;
		if(tracking)
		{
			// solution will hold the new pose.
			score = ht->track(depthP,colorP,solution);
		}
		else
		{
            //controlValues(m_eulerx, m_eulery, m_eulerz, key, PI / 16.);
			solution[3] = (float) (cos(m_eulerx/2.) * cos(m_eulery/2.) * cos(m_eulerz/2.) + sin(m_eulerx/2.) * sin(m_eulery/2.) * sin(m_eulerz/2.));
			solution[4] = (float) (sin(m_eulerx/2.) * cos(m_eulery/2.) * cos(m_eulerz/2.) - cos(m_eulerx/2.) * sin(m_eulery/2.) * sin(m_eulerz/2.));
			solution[5] = (float) (cos(m_eulerx/2.) * sin(m_eulery/2.) * cos(m_eulerz/2.) + sin(m_eulerx/2.) * cos(m_eulery/2.) * sin(m_eulerz/2.));
			solution[6] = (float) (cos(m_eulerx/2.) * cos(m_eulery/2.) * sin(m_eulerz/2.) - sin(m_eulerx/2.) * sin(m_eulery/2.) * cos(m_eulerz/2.));
			// evaluate the observation against the current hand pose.
			score = ht->evaluateHandPose(depthP,colorP,solution);
		}

//		std::cout << "quat : ";
//		for (int i=3; i<=6; i++)
//		{
//			std::cout << solution[i] << " ";
//		}
//		std::cout << std::endl;

		// transforms retrieved data into hand position/orientation and finger joint rotations
		double handTx, handTy, handTz;
		double handRx, handRy, handRz;
		double joint4, joint5, joint6;
		double joint7, joint8, joint9, joint10, joint11, joint12, joint13, joint14, joint15;

		// changes referentiel to iCub one
		// std::cout<< solution[0]<<"," <<solution[1]<<","<<solution[2]<<std::endl;

		//controlValues(offsetTx, offsetTy, offsetTz, key, 10.);

		std::cout << solution[2] << " " << offsetTx << " " << (-solution[2]+offsetTx)/1000 << std::endl;
		handTx = (-solution[2]+offsetTx)/1000; //from millimmeter to meter
		handTy = (solution[0]+offsetTy)/1000;
		handTz = (-solution[1]+offsetTz)/1000;
		//now this should be done in the teleoperation module
		//printf("Offsets : %3.0f %3.0f %3.0f\n", offsetTx, offsetTy, offsetTz);

		// computes rotation around each axis from quaternion values
		// converts the quaternion to angles
		float Rx, Ry, Rz;
		float CRx, CRy, CRz;
		float SRx, SRz;
		float qx = solution[3];
		float qy = solution[4];
		float qz = solution[5];
		float qw = solution[6];

		Ry = asin(2*qx*qz-2*qy*qw);

		CRy = cos(Ry);

		if (abs(CRy)>0.0005)
		{
			SRx = (2*qx*qy+2*qz*qw);
			CRx = (1-2*qy*qy-2*qz*qz);
			Rx = atan2(SRx,CRx);

			SRz = (2*qx*qw+2*qy*qz);
			CRz = (1-2*qz*qz-2*qw*qw);
			Rz = atan2(SRz,CRz);
		}
		else
		{
			Rx = 0;

			SRz = (2*qx*qw+2*qy*qz);
			CRz = (1-2*qz*qz-2*qw*qw);
			Rz = atan2(SRz,CRz);
		}


		handRx = Ry + PI;
		handRy = Rz + PI / 2;
		handRz = - Rx + PI;
		fprintf(stdout, "hand T* :  %3.0f %3.0f %3.0f hand R* : %3.0f %3.0f %3.0f\n",handTx, handTy, handTz, handRx * 180. / PI, handRy * 180. / PI, handRz * 180 / PI);

		// transforms the hand rotations into angles for joint 4, 5 and 6
		// unfortunataly, this doesn't seem correct beacause the cartesian controller interfers
		// by reinterpreting the rotation values
		joint4 =  handRz*180/PI;//+initialValueJoint4;
		joint5 = -handRy*180/PI;//+initialValueJoint5;
		joint6 = (handRx+PI)*180/PI;//+initialValueJoint6;


		//fprintf (stdout, "hand rotation : %2.0f %2.0f %2.0f\n",degree90(joint4), degree90(joint5), degree90(joint6));

		// finger articulation
		// between finger gap
		joint7 = -(solution[16]+solution[8])*180/PI+60;
		//thumb control
		joint8 = -(solution[11])*180/PI+10;
		joint9 = -(solution[12]+2.8-PI/2)*180/PI;
		joint10 = -(solution[13]+solution[14])*180/PI;
		//index finger
		joint11 = -(solution[19])*180/PI;
		joint12 = -(solution[21]+solution[22])*180/PI;
		// middle fingers
		joint13 = -(solution[23])*180/PI;
		joint14 = -(solution[25]+solution[26])*180/PI;
		// ring and little fingers controlled together
		joint15 = -(solution[7]+solution[9]+solution[10])*180/PI;
		joint15 = -(solution[15]+solution[17]+solution[18])*180/PI;
		// joint15 /= 2;

		// sends the value to the yarp ports
		Bottle &targetHand=handTrackerPort.prepare();
		targetHand.clear();
        targetHand.addInt(swTracking::FORTH_LIB); //annouce
		targetHand.addDouble(handTx); //hand: Tx
		targetHand.addDouble(handTy); //hand: Ty
		targetHand.addDouble(handTz); //hand: Tz
		// if you use the cartesian controller then send the handRx,y,z
		 targetHand.addDouble(handRx); //hand: Rx
		 targetHand.addDouble(handRy); //hand: Ry
		 targetHand.addDouble(handRz); //hand: Rz
		// if not send commands to the joints 4, 5, and 6
		// but you won't get
//		target.addDouble(joint4); //hand: Rx -> joint4
//		target.addDouble(joint5); //hand: Ry -> joint5
//		target.addDouble(joint6); //hand: Rz -> joint6


		handTrackerPort.write();

		Bottle &targetFingers = fingersTrackerPort.prepare();
		targetFingers.addDouble(joint7); //in between finger gap [60 -> 0]
		targetFingers.addDouble(joint8); //thumb: first phalange rotation [10 -> 90]
		targetFingers.addDouble(joint9); //in between thumb/index gap [90 -> 0]
		targetFingers.addDouble(joint10); //thumb: second&third phalange rotation [0 -> 180]
		targetFingers.addDouble(joint11); //index finger: first phalange rotation [0 -> 90]
		targetFingers.addDouble(joint12); //index finger: second & third phalange rotation [0 -> 180]
		targetFingers.addDouble(joint13); //middle finger: first phalange rotation [0 -> 90]
		targetFingers.addDouble(joint14); //middle finger: second & third phalange rotation [0 -> 180]
		targetFingers.addDouble(joint15); //ring and little fingers rotation [0 -> 270]

		fingersTrackerPort.write();



		// The score is a negative number indicating the quality of the result. Higher is better. The best score is 0.
		// LOG("HandPose Score: "<<score);

		ht->visualizeHandPose(solution,colorP,640,480,!tracking,score);

		cv::Mat bgrMat,rgbMat(480,640,CV_8UC3,colorP,3*640);
		cv::cvtColor(rgbMat,bgrMat, CV_RGB2BGR);// opencv expects the image in BGR format

		if(tracking)
		{
			cv::putText(bgrMat,"Track Mode: Press 's' to reset, 'Ctrl-C' to quit" ,cv::Point(5,bgrMat.rows-20),CV_FONT_HERSHEY_SIMPLEX,0.7,CV_RGB(0,0,255),1,CV_AA);
		}
		else
		{
			cv::putText(bgrMat,"Init Mode: Place your hand on the model and press 's'." ,cv::Point(5,bgrMat.rows-20),CV_FONT_HERSHEY_SIMPLEX,0.7,CV_RGB(0,255,0),1,CV_AA);
		}
		cv::imshow("FORTH 3D HandTracker",bgrMat);

		if(key=='s')
		{
			tracking = !tracking;
			// reset the starting pose of the tracker to initialization position.
			ht->setHandPose(ht->getInitialHandPose());
		}
	}
	else
	{
		LOG("<!!>Frame Not Available");
	}

	return true;
}


/**
 * Gets the period of module updating
 */
double SWForthTracking::getPeriod()
{
	return 0.050; // module periodicity (seconds, i.e. 50 ms)
}


/**
 * Interrupts the module
 */
bool SWForthTracking::interruptModule()
{
	handTrackerPort.interrupt();

	std::cout << "-->Interrupting the Hand Tracker module..." << std::endl;
	return true;
}


/**
 * Closes the module
 */
bool SWForthTracking::close()
{
	//Closing any opened connections
	// OpenCV
	std::cout << "-->Closing OpenCV..." << std::endl;
	cv::destroyAllWindows();

	//OpenNI
	std::cout << "-->Closing OpenNI..." << std::endl;
	depth.stop();
	color.stop();
	depth.destroy();
	color.destroy();
	device.close();
	OpenNI::shutdown();

	//Yarp
	std::cout << "-->Closing Yarp..." << std::endl;
	handTrackerPort.close();


	std::cout << "-->Closing the Hand Tracker module..." << std::endl;
	HandTracker::destroy();

	return true;
}

int main(int argc, char* argv[])
{
    /* initialize yarp network */
    Network yarp;
    if (!yarp.checkNetwork())
    {
        std::cout << "-ERROR: Problem connecting to YARP server" << std::endl;
        return -1;
    }

    /* create your module */
    SWForthTracking l_handTracker;

    /* prepare and configure the resource finder */
    ResourceFinder rf;
        rf.setVerbose(true);
        rf.setDefaultConfigFile("teleoperation.ini"); //overridden by --from parameter
        rf.setDefaultContext("handTeleoperation/conf");   //overridden by --context parameter
        rf.configure("ICUB_ROOT", argc, argv);


    /* configure the module */
    std::cout << "Configuring the Hand Tracker module..."<< std::endl;
    if (l_handTracker.configure(rf))
    {
        /* run the module */
        std::cout << "Starting the Hand Tracker module..." << std::endl;
        l_handTracker.runModule();
    }
    else
    {
        std::cout << "Failed to configure the Hand Tracker module!"<< std::endl;
    }
    return 0;
}


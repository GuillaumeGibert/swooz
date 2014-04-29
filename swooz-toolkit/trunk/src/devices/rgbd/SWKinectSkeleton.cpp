/**
 * \file SWKinectSkeleton.cpp
 * \brief Defines SWKinectTracker
 * \author Emmanuel Body
 * \date 07/01/2014
 */

#include "devices/rgbd/SWKinectSkeleton.h"

// boost
#include <boost/assign/list_of.hpp>

// std
#include <cstdio>

using namespace swDevice;


#define CHECK_RC(nRetVal, what)					    \
	if (nRetVal != XN_STATUS_OK)				    \
{								    \
	if (m_verbose) \
	printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));    \
	return nRetVal;						    \
}


SWKinectSkeleton* SWKinectSkeleton::m_instance = NULL;

SWKinectSkeleton::SWKinectSkeleton(bool verbose) : m_verbose(verbose), m_bNeedPose(FALSE), m_i32PreviousDetected(0)
{
	m_strPose[0] = '\0';
}

SWKinectSkeleton::SkeletonProfile SWKinectSkeleton::UpperBody = boost::assign::list_of (XN_SKEL_TORSO) (XN_SKEL_NECK) (XN_SKEL_HEAD) (XN_SKEL_LEFT_SHOULDER) (XN_SKEL_RIGHT_SHOULDER) (XN_SKEL_LEFT_ELBOW) (XN_SKEL_RIGHT_ELBOW) (XN_SKEL_LEFT_HAND) (XN_SKEL_RIGHT_HAND);

const std::string SWKinectSkeleton::m_defaultConfigFile = std::string(getenv("OPEN_NI_INSTALL_PATH")) + std::string("\\Data\\SamplesConfig.xml");

SWKinectSkeleton *SWKinectSkeleton::getInstance(bool verbose)
{
    if(m_instance == NULL)
    {
        m_instance = new SWKinectSkeleton();
    }
    m_instance->m_verbose = verbose;
    return m_instance;
}

SWKinectSkeleton::~SWKinectSkeleton()
{
	if (m_verbose)
		printf("Closing OpenNi...");
    m_scriptNode.Release();
    m_UserGenerator.Release();
    m_Context.Release();
}
void SWKinectSkeleton::selectProfile(SkeletonProfile profile)
{
	m_skeletonProfile = profile;
}

int SWKinectSkeleton::grab(std::vector<Coordinates> &values)
{
	XnUserID aUsers[m_maxNumUsers];
	XnUInt16 nUsers;
	values.clear();
	m_Context.WaitOneUpdateAll(m_UserGenerator);
	nUsers=m_maxNumUsers;
    m_UserGenerator.GetUsers(aUsers, nUsers);

    if(m_i32PreviousDetected != static_cast<int>(nUsers))
    {
        std::cout << (int)nUsers << " user(s) detected" << std::endl;
        m_i32PreviousDetected = static_cast<int>(nUsers);
    }

	if (nUsers == 0)
	{
		return 1;
	}
	for (std::vector<XnSkeletonJoint>::iterator it = m_skeletonProfile.begin(); it < m_skeletonProfile.end(); it++)
	{
		XnSkeletonJointTransformation l_joint;
		m_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[0],*it,l_joint);
//		if (l_joint.orientation.fConfidence < 0.5)
//		{
//			if (m_verbose)
//				printf("Confidence on the reading is not sufficent. (joint %d, %f)\n", *it, l_joint.orientation.fConfidence);
//			return -1;
//		}
//		else
//		{
//			printf ("joint %d OK (%f)\n", *it, l_joint.orientation.fConfidence);
//		}
		Coordinates l_data (l_joint.position.position);
		values.push_back(l_data);
	}



	return 0;
}


int SWKinectSkeleton::init(const char* xmlConfigFile)
{
	XnStatus nRetVal = XN_STATUS_OK;
	xn::EnumerationErrors errors;

	if (m_verbose)
		printf("Reading config from: '%s'\n", xmlConfigFile);

	nRetVal = m_Context.InitFromXmlFile(xmlConfigFile, m_scriptNode, &errors);
	if (nRetVal == XN_STATUS_NO_NODE_PRESENT)
	{
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		if (m_verbose)
			printf("%s\n", strError);
		return (nRetVal);
	}
	else if (nRetVal != XN_STATUS_OK)
	{
		if (m_verbose)
			printf("Open failed: %s\n", xnGetStatusString(nRetVal));
		return (nRetVal);
	}

	nRetVal = m_Context.FindExistingNode(XN_NODE_TYPE_USER, m_UserGenerator);
	if (nRetVal != XN_STATUS_OK)
	{
		nRetVal = m_UserGenerator.Create(m_Context);
		CHECK_RC(nRetVal, "Find user generator");
	}

	XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected;
	if (!m_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
	{
		if (m_verbose)
			printf("Supplied user generator doesn't support skeleton\n");
		return 1;
	}

	nRetVal = m_UserGenerator.RegisterUserCallbacks(SWKinectSkeleton::User_NewUser, SWKinectSkeleton::User_LostUser, NULL, hUserCallbacks);
	CHECK_RC(nRetVal, "Register to user callbacks");
	nRetVal = m_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
	CHECK_RC(nRetVal, "Register to calibration start");
	nRetVal = m_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);
	CHECK_RC(nRetVal, "Register to calibration complete");

	if (m_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
	{
		m_bNeedPose = TRUE;
		if (!m_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
		{
			printf("Pose required, but not supported\n");
			return 1;
		}
		nRetVal = m_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
		CHECK_RC(nRetVal, "Register to Pose Detected");
		m_UserGenerator.GetSkeletonCap().GetCalibrationPose(m_strPose);
	}

	m_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

	nRetVal = m_Context.StartGeneratingAll();
	CHECK_RC(nRetVal, "StartGenerating");

	if (m_verbose)
		printf("Starting to run\n");
	if(m_bNeedPose)
	{
		if (m_verbose)
			printf("Assume calibration pose\n");
	}
	return 0;
}

void XN_CALLBACK_TYPE SWKinectSkeleton::User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
    if (m_instance->m_verbose)
        printf("New User %d\n", nId);
	// New user found
    if (m_instance->m_bNeedPose)
	{
        m_instance->m_UserGenerator.GetPoseDetectionCap().StartPoseDetection(m_instance->m_strPose, nId);
	}
	else
	{
        m_instance->m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}    
}

// Callback: An existing user was lost
void XN_CALLBACK_TYPE SWKinectSkeleton::User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
    if (m_instance->m_verbose)
        printf("Lost user %d\n", nId);
}

// Callback: Detected a pose
void XN_CALLBACK_TYPE SWKinectSkeleton::UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie){
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
    if (m_instance->m_verbose)
        printf("Pose %s detected for user %d\n", strPose, nId);
    m_instance->m_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
    m_instance->m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

// Callback: Started calibration
void XN_CALLBACK_TYPE SWKinectSkeleton::UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
    if (m_instance->m_verbose)
        printf("Calibration started for user %d\n", nId);
}

void XN_CALLBACK_TYPE SWKinectSkeleton::UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
	XnUInt32 epochTime = 0;
	xnOSGetEpochTime(&epochTime);
	if (eStatus == XN_CALIBRATION_STATUS_OK)
	{
		// Calibration succeeded
        if (m_instance->m_verbose)
            printf("Calibration complete, start tracking user %d\n", nId);
        m_instance->m_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
        if (m_instance->m_verbose)
            printf("Calibration failed for user %d\n", nId);
		if(eStatus==XN_CALIBRATION_STATUS_MANUAL_ABORT)
		{
            if (m_instance->m_verbose)
				printf("Manual abort occured, stop attempting to calibrate!");
			return;
		}
        if (m_instance->m_bNeedPose)
		{
            m_instance->m_UserGenerator.GetPoseDetectionCap().StartPoseDetection(m_instance->m_strPose, nId);
		}
		else
		{
            m_instance->m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}


XnBool SWKinectSkeleton::fileExists(const char *fn)
{
	XnBool exists;
	xnOSDoesFileExist(fn, &exists);
	return exists;
}

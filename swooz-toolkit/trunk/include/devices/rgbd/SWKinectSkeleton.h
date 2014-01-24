/**
 * \file SWKinectSkeleton.h
 * \brief Defines SWKinectTracker
 * \author Emmanuel Body
 * \date 07/01/2014
 */

#ifndef _SWKINECTTRACKER_
#define _SWKINECTTRACKER_



// OpenNI
#include <XnCppWrapper.h>

// std
#include <vector>
#include <string>


namespace swDevice
{
	/**
	 * \class SWKinectSkeleton
	 * \brief A high level interface for getting tracking data from a kinect/xtion device.
	 */
	class SWKinectSkeleton
	{
	public:
		typedef std::vector<XnSkeletonJoint> SkeletonProfile;

		/*
		 * \brief Type encapsulating X, Y, Z float coordinates.
		 */
		typedef XnVector3D Coordinates;
	private:
		xn::Context m_Context;
		xn::ScriptNode m_scriptNode;
		xn::UserGenerator m_UserGenerator;

		XnBool m_bNeedPose;
		XnChar m_strPose[20];
		static const int m_maxNumUsers = 15;

		bool m_verbose;
		SkeletonProfile m_skeletonProfile;

		static SWKinectSkeleton * m_instance;

		// New User detected
		static void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie);

		// Existing user lost
		static void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie);

		// User pose detected
		static void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie);

		// Calibration started
		static void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie);

		//Calibration complete
		static void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie);

		XnBool fileExists(const char *fn);


		/**
		 * \brief SWKinectSkeleton constructor. Private. Use getInstance();
		 * \param [in] verbose : display informations
		 */
		SWKinectSkeleton(bool verbose = true);

		static const std::string m_defaultConfigFile;
	public:

		/*
		 * \brief public method of accessing the unique instance of SWKinectSkeleton.
		 * \param [in] verbose : display informations
		 * \return A pointer to the instace of SWKinectSkeleton
		 */
		static SWKinectSkeleton * getInstance (bool verbose = true);

		/**
		 * \brief SWKinectSkeleton destructor.
		 */
		~SWKinectSkeleton();

		/**
		 * \brief Init the kinect user module.
		 * \param [in] (optional) xmlConfigPath : path to xml config file. Default is the samples original config file.
		 * \return 0 on success
		 */
		int init(const char* xmlConfigFile =  m_defaultConfigFile.c_str());

		/**
		 * \brief Selects the points to grab Must be called after init
		 * \param [in] profile : static member of SWKinectSkeleton defining a list of joints
		 */
		void selectProfile(SkeletonProfile profile);


		/*
		 * /brief Profile listing the useful joints of the upper body
		 *
		 * Defines (in this order) : torso, neck, head, left shoulder, right shoulder, left elbow, right elbow, left hand, right hand.
		 */
		static SkeletonProfile UpperBody;

		/*
		 * /brief Grabs the skeleton data. init and selectProfile must have been called
		 * /param [out] values : array of triplets (X-Y-Z coordinates) in the order defined by the selected profile.
		 */
		int grab(std::vector<SWKinectSkeleton::Coordinates> & values);

	};
}




#endif

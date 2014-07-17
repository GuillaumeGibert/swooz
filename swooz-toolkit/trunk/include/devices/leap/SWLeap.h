
/**
 * \file SWLeap.h
 * \brief Defines SWLeap
 * \author Gabriel MEYNET
 * \date 07/04/14
 */

#ifndef _SWLEAP_
#define _SWLEAP_

//#include "commonTypes.h"
//#include "leap.h"
//#include "LeapMath.h"
//#include "Windows.h"
//#include <tchar.h>
//#include <vector>


//namespace swDevice
//{
//	/**
//	 * \class SWLeap
//	 * \brief A basic interface class for getting rigid motion data from the leap device in real time.
//	 */

//    class SWLeap : public Leap::Listener {
//		public:
			
//			/**
//			 * \brief Default SWLeap constructor
//			 */
//			SWLeap();
		

//			/**
//			 * \brief Initialization of the leap device
//			 * \return true if device detected else return false
//			 */
//			virtual bool init();
		
//			/**
//			 * \brief Read leap data
//			 * \return true if data available, if the leap is disconnected return false
//			 */
//			virtual bool read();
			
//			/**
//			 * \brief Disconnect the leap
//			 */
//			virtual void stop();

//            /**
//             * @brief Return the Coordinate of a finger : X = 0, Y = 1, Z = 2 => ui32NumCoord
//             * @param ui32NumFinger
//             * @param ui32NumCoord
//             * @param isleft
//             * @return
//             */
//            float fingersCoords(cuint ui32NumFinger,cuint ui32NumCoord,bool isleft);

//            /**
//             * @brief Return the Direction of a ui32NumFinger : X = 0, Y = 1, Z = 2 => ui32NumDir
//             * @param ui32NumFinger
//             * @param ui32NumDir
//             * @param isleft
//             * @return
//             */
//			float getFingerDirection(cuint ui32NumFinger,cuint ui32NumDir,bool isleft);

//            /**
//             * @brief Return the Coordinate of the palm : X = 0, Y = 1, Z = 2 => ui32NumCoord
//             * @param ui32NumCoord
//             * @param isleft
//             * @return
//             */
//			float getPalmCoords(cuint ui32NumCoord,bool isleft);

//            /**
//             * @brief Return the Coordinate of the Sphere : X = 0, Y = 1, Z = 2 => ui32NumCoord
//             * @param ui32NumCoord
//             * @param isleft
//             * @return
//             */
//			float getHandSphereCoords(cuint ui32NumCoord,bool isleft);

//            /**
//             * @brief Return the Radius of the sphere in the hand
//             * @param isleft
//             * @return
//             */
//			float getHandSphereRadius(bool isleft);

//            /**
//             * @brief Return the pitch of the Hand
//             * @param isleft
//             * @return
//             */
//			float getHandPitch(bool isleft);

//            /**
//             * @brief Return the Roll of the Hand
//             * @param isleft
//             * @return
//             */
//			float getHandRoll(bool isleft);

//            /**
//             * @brief Return the Yaw of the Hand
//             * @param isleft
//             * @return
//             */
//			float getHandYaw(bool isleft);
			
//            /**
//             * @brief Return the frame Rate of the module
//             * @return
//             */
//			float getFPS();

//            /**
//             * @brief Extract Pitch, Roll and Yaw from a Leap::Matrix : Bone rotation
//             * @param l_LeapMatrix
//             * @return Return the angles to achive this rotation.
//             */
//			std::vector<double> computeEulerYPRAngles(Leap::Matrix l_LeapMatrix);

//            /**
//             * @brief Call ComputeEuler YPRAngles for each Bone
//             * @param l_bone
//             * @return Return the angles to achive the rotation.
//             */
//			std::vector<double> setBoneRotation(Leap::Bone l_bone);

//            /**
//             * @brief Return the rotation of a bone : ui32NumCouple of a finger : ui32NumFinger
//             * @param ui32NumFinger
//             * @param ui32NumCouple
//             * @param isleft
//             * @return
//             */
//			double getFingerRotation(cuint ui32NumFinger, cuint ui32NumCouple,bool isleft);

//            /**
//             * @brief Return a vector of Coordinate of the Palm of an hand
//             * @param hand
//             * @return
//             */
//			std::vector<float> getHandPalmCoord(Leap::Hand hand);

//            /**
//             * @brief Return the center of the sphere of an hand
//             * @param hand
//             * @return
//             */
//			std::vector<float> getHandSphereCenter(Leap::Hand hand);

//            /**
//             * @brief Update member variables of pitch, roll and yaw
//             * @param hand
//             * @param isleft
//             * @return true if no issue detected
//             */
//			bool setHandRotation(Leap::Hand hand,bool isleft);

//            /**
//             * @brief Update member variables of fingers
//             * @param frame
//             * @param l_fingers
//             * @param isleft
//             * @return true if no issue detected
//             */
//			bool setFingers(Leap::Frame frame,Leap::FingerList l_fingers,bool isleft);

//            /**
//             * @brief Update member variables of bones
//             * @param isleft
//             * @return true if no issue detected
//             */
//			bool setBones(bool isleft);

		
//		private :
//			  // Create an interface to acces data and manipulate the leap
//			Leap::Controller m_controller; /**< Leap Controler */


//		//LEFTHAND
//			std::vector<float> m_handpalmCoordsLeft; /**< Left Hand Palm Position */
			   
//			//hand angles
//			float m_pitchLeft; /**< Pitch, rotate on X axis, in RAD */
//			float m_rollLeft;  /**<Roll, rotate on Z axis, in RAD */
//			float m_yawLeft;   /**< Yaw, rotate on Y axis, in RAD */

//			//Sphere in the hand
//			std::vector<float> m_handsphereCoordsLeft; /**< Positiono of the sphere inside the left hand */
//			float m_handsphereRadiusLeft; /**< Radius of the sphere inside the left hand*/

//			//Vector of vector of coordinates
//			std::vector<std::vector<float> > m_vFingerCoordsLeft; /**<  Position of each finger, inside a vector*/
			
//			//pointing vectors of each finger
//			std::vector<std::vector<float> > m_vFingerDirLeft;  /**<  Direction of each finger, inside a vector */

//			//finger Object (left)
//			Leap::Finger m_leftThumb;  /**< Leap Object : leftThumb*/
//			Leap::Finger m_leftIndex;  /**< Leap Object : leftIndex*/
//			Leap::Finger m_leftMiddle; /**< Leap Object : leftMiddle*/
//			Leap::Finger m_leftRing;   /**< Leap Object : leftRing*/
//			Leap::Finger m_leftPinky;  /**< Leap Object : leftPinky*/


		
//			//Bones rotation
//			double m_leftThumbB01Rot;  /**< thumb left bone 0 and 1 rotation => 1 joint*/
//			double m_leftIndexB01Rot;  /**< index left bone 0 and 1 rotation => 1 joint*/
//			double m_leftMiddleB01Rot; /**< middle left bone 0 and 1 rotation => 1 joint*/
//			double m_leftRingB01Rot;   /**< ring left bone 0 and 1 rotation => 1 joint*/
//			double m_leftPinkyB01Rot;  /**< pinky left bone 0 and 1 rotation => 1 joint*/

//			double m_leftThumbB23Rot;  /**< thumb left bone 2 and 3 rotation => 1 joint*/
//			double m_leftIndexB23Rot;  /**< index left bone 2 and 3 rotation => 1 joint*/
//			double m_leftMiddleB23Rot; /**< middle left bone 2 and 3 rotation => 1 joint*/
//			double m_leftRingB23Rot;   /**< ring left bone 2 and 3 rotation => 1 joint*/
//			double m_leftPinkyB23Rot;  /**< pinky left bone 2 and 3 rotation => 1 joint*/



//		//RIGHTHAND
//			std::vector<float> m_handpalmCoordsRight; /**< Hand 2 Palm Position */

//			//hand angles
//			float m_pitchRight; /**< Pitch, rotate on X axis, in RAD */
//			float m_rollRight;  /**<Roll, rotate on Z axis, in RAD */
//			float m_yawRight;   /**< Yaw, rotate on Y axis, in RAD */

//			//Sphere in the hand
//			std::vector<float> m_handsphereCoordsRight; /**< Positiono of the sphere inside the right hand */
//			float m_handsphereRadiusRight; /**< Radius of the sphere inside the right hand*/
			
//			//Vector of vector of coordinates
//			std::vector<std::vector<float> > m_vFingerCoordsRight; /**<  Position of each finger, inside a vector*/
			
//			//pointing vectors of each finger
//			std::vector<std::vector<float> > m_vFingerDirRight;  /**<  Direction of each finger, inside a vector */
			
//			//finger Object (Right)
//			Leap::Finger m_rightThumb;  /**< Leap Object : rightThumb*/
//			Leap::Finger m_rightIndex;  /**< Leap Object : rightIndex*/
//			Leap::Finger m_rightMiddle; /**< Leap Object : rightMiddle*/
//			Leap::Finger m_rightRing;   /**< Leap Object : rightRing*/
//			Leap::Finger m_rightPinky;  /**< Leap Object : rightPinky*/
			
//			//Bones rotation
//			double m_rightThumbB01Rot;  /**< thumb right bone 0 and 1 rotation => 1 joint*/
//			double m_rightIndexB01Rot;  /**< index right bone 0 and 1 rotation => 1 joint*/
//			double m_rightMiddleB01Rot; /**< middle right bone 0 and 1 rotation => 1 joint*/
//			double m_rightRingB01Rot;   /**< ring right bone 0 and 1 rotation => 1 joint*/
//			double m_rightPinkyB01Rot;  /**< pinky right bone 0 and 1 rotation => 1 joint*/

//			double m_rightThumbB23Rot;  /**< thumb right bone 2 and 3 rotation => 1 joint*/
//			double m_rightIndexB23Rot;  /**< index right bone 2 and 3 rotation => 1 joint*/
//			double m_rightMiddleB23Rot; /**< middle right bone 2 and 3 rotation => 1 joint*/
//			double m_rightRingB23Rot;   /**< ring right bone 2 and 3 rotation => 1 joint*/
//			double m_rightPinkyB23Rot;  /**< pinky right bone 2 and 3 rotation => 1 joint*/



//		//INDEPENDANT
//			//FRAME PER SECOND
//            float m_fps; /**< Number Frame per second */  // TODO : EURRRRRRRRRGH

//			bool m_bisleft; /**< Bool to know if we are on the left hand or the right one*/
			
//			//TEST ROTATION
//			std::vector<double> m_tempRotation; /**< Varaible to keep the metacarp rotation */
//			bool isMetacarp; /**<True is we working of the metacarp */



//            // ############################################################### replacement
////            int m_fps;
////            std::vector<Leap::Finger> m_vLeftFingers;  /**< arrays of left fingers, order : thunb, index, middle, ring, pinky */
////            std::vector<Leap::Finger> m_vRightFingers; /**< arrays of right fingers, order : thunb, index, middle, ring, pinky */
//	};
//}



#include <iostream>


#include "commonTypes.h"
#include "leap.h"
#include "LeapMath.h"

#include <boost/thread.hpp>




namespace swDevice
{
    class SWLeap
    {

        public :

            /**
             * @brief SWLeap
             */
            SWLeap();

            /**
             * @brief init
             * @return
             */
            bool init();

            /**
             * @brief grab
             * @return
             */
            int grab();

            /**
             * @brief directionArm
             * @param leftArm
             * @param vDirectionArm
             */
            void directionArm(cbool leftArm, std::vector<float> &vDirectionArm) const;

            /**
             * @brief directionHandEuclidian
             * @param leftHand
             * @param vDirectionHandE
             */
            void directionHandEuclidian(cbool leftHand, std::vector<float> &vDirectionHandE) const;

            /**
             * @brief normalPalmHandEuclidian
             * @param leftHand
             * @param vNormalPalmHandE
             */
            void normalPalmHandEuclidian(cbool leftHand, std::vector<float> &vNormalPalmHandE) const;

            /**
             * @brief directionHand
             * @param leftHand
             * @param vDirectionHand
             */
            void directionHand(cbool leftHand, std::vector<float> &vDirectionHand) const;

            /**
             * @brief normalPalmHand
             * @param leftHand
             * @param vNormalPalmHand
             */
            void normalPalmHand(cbool leftHand, std::vector<float> &vNormalPalmHand) const;

            /**
             * @brief coordPalmHand
             * @param leftHand
             * @param vCoordPalmHand
             */
            void coordPalmHand(cbool leftHand, std::vector<float> &vCoordPalmHand) const;

            /**
             * @brief fingerRotation
             * @param leftHand
             * @param fingerType
             * @param fingerRotation
             */
//            void fingerRotation(cbool leftHand, const Leap::Finger::Type fingerType, std::vector<float> &fingerRotation);

            /**
             * @brief boneDirection
             * @param bLeftHand
             * @param fingerType
             * @param boneType
             * @param vBoneDirection
             */
            void boneDirection(const bool bLeftHand, const Leap::Finger::Type fingerType, const Leap::Bone::Type boneType, std::vector<float> &vBoneDirection);

            /**
             * @brief bonePosition
             * @param bLeftHand
             * @param fingerType
             * @param boneType
             * @param vBonePosition
             */
            void bonePosition(const bool bLeftHand, const Leap::Finger::Type fingerType, const Leap::Bone::Type boneType, std::vector<float> &vBonePosition);

            /**
             * @brief fps
             * @return
             */
            int fps() const;

        private :

            void toStdVector(std::vector<float> &oSTDVec, Leap::Vector &oVector);

            /**
             * @brief finger
             * @param bLeftHand
             * @param fingerType
             * @param oFinger
             */
//            void finger(const bool bLeftHand, const Leap::Finger::Type fingerType, Leap::Finger &oFinger);


            /**
             * @brief numFingerType
             * @param fingerList
             * @param fingerType
             * @return
             */
            int numFingerType(const Leap::FingerList &fingerList, const Leap::Finger::Type fingerType) const;

            /**
             * @brief computeEulerYPRAngles
             * @param bone
             * @param isMetacarp
             * @return
             */
//            std::vector<float> computeEulerYPRAngles(const Leap::Bone &bone, cbool isMetacarp) const;

            /**
             * @brief retrieveFingerRotation
             * @param finger
             * @param rotation
             */
//            void retrieveFingerRotation(const Leap::Finger &finger, std::vector<float> &rotation) const;

            Leap::Controller m_leapController; /**< ... */

            int m_fps;  /**< ... */

            // left
            //      arm
            std::vector<float> m_vLeftDirectionArm;     /**< ... */
            //      hand
            std::vector<float> m_vLeftCoordPalmHand;    /**< ... */
            std::vector<float> m_vLeftNormalPalmHand;   /**< ... */
            std::vector<float> m_vLeftDirectionHand;    /**< ... */
            std::vector<float> m_vLeftNormalPalmHandE;  /**< ... */
            std::vector<float> m_vLeftDirectionHandE;   /**< ... */
            //      hand fingers directions
            std::vector<std::vector<float> > m_vLeftThumbDirections;    /**< ... */
            std::vector<std::vector<float> > m_vLeftIndexDirections;    /**< ... */
            std::vector<std::vector<float> > m_vLeftMiddleDirections;   /**< ... */
            std::vector<std::vector<float> > m_vLeftRingDirections;     /**< ... */
            std::vector<std::vector<float> > m_vLeftPinkyDirections;    /**< ... */

            std::vector<std::vector<float> > m_vLeftThumbPositions; /**< ... */
            std::vector<std::vector<float> > m_vLeftIndexPositions; /**< ... */
            std::vector<std::vector<float> > m_vLeftMiddlePositions;/**< ... */
            std::vector<std::vector<float> > m_vLeftRingPositions;  /**< ... */
            std::vector<std::vector<float> > m_vLeftPinkyPositions; /**< ... */

            // right
            //      arm
            std::vector<float> m_vRightDirectionArm;    /**< ... */
            //      hand
            std::vector<float> m_vRightCoordPalmHand;   /**< ... */
            std::vector<float> m_vRightNormalPalmHand;  /**< ... */
            std::vector<float> m_vRightDirectionHand;   /**< ... */
            std::vector<float> m_vRightDirectionHandE;  /**< ... */
            std::vector<float> m_vRightNormalPalmHandE; /**< ... */
            //      hand fingers directions
            std::vector<std::vector<float> > m_vRightThumbDirections;   /**< ... */
            std::vector<std::vector<float> > m_vRightIndexDirections;   /**< ... */
            std::vector<std::vector<float> > m_vRightMiddleDirections;  /**< ... */
            std::vector<std::vector<float> > m_vRightRingDirections;    /**< ... */
            std::vector<std::vector<float> > m_vRightPinkyDirections;   /**< ... */

            std::vector<std::vector<float> > m_vRightThumbPositions; /**< ... */
            std::vector<std::vector<float> > m_vRightIndexPositions; /**< ... */
            std::vector<std::vector<float> > m_vRightMiddlePositions;/**< ... */
            std::vector<std::vector<float> > m_vRightRingPositions;  /**< ... */
            std::vector<std::vector<float> > m_vRightPinkyPositions; /**< ... */
    };
}

#endif

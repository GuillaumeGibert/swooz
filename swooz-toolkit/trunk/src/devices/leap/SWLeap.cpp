

/**
 * \file SWLeap.cpp
 * \brief  SWLeap implements basics Actions/Services that Leap Motion provides
 * \author Gabriel MEYNET
 * \date 07/04/14
 */

//#include "devices/leap/SWLeap.h"
//#include <iostream>
//#include <vector>
//#include <time.h>
//#include <math.h>
//#include "math.h"
//#include <cmath>


//#include <boost/thread.hpp>

//using namespace std;
//using namespace swDevice;
//using namespace Leap;

//#define PI 3.14159265


//SWLeap::SWLeap()
//{

//    m_handpalmCoordsLeft = std::vector<float>(3, 0.f);
//    m_handpalmCoordsRight = std::vector<float>(3, 0.f);

//    m_vFingerCoordsLeft = std::vector<std::vector<float> >(5,std::vector<float> (3,0.f));
//    m_vFingerCoordsRight = std::vector<std::vector<float> >(5,std::vector<float> (3,0.f));

//    m_vFingerDirLeft = std::vector<std::vector<float> >(5,std::vector<float> (3,0.f));
//    m_vFingerDirRight = std::vector<std::vector<float> >(5,std::vector<float> (3,0.f));

//    m_handsphereCoordsLeft = std::vector<float>(3,0.f);
//    m_handsphereCoordsRight = std::vector<float>(3,0.f);

//    m_pitchLeft = 0;
//    m_rollLeft = 0;
//    m_yawLeft = 0;

//    m_pitchRight = 0;
//    m_rollRight = 0;
//    m_yawRight = 0;

//    m_handsphereRadiusLeft = 0;
//    m_handsphereRadiusRight = 0;


//    m_leftThumbB01Rot = 0;
//    m_leftIndexB01Rot = 0;
//    m_leftMiddleB01Rot = 0;
//    m_leftRingB01Rot = 0;
//    m_leftPinkyB01Rot = 0;

//    m_leftThumbB23Rot = 0;
//    m_leftIndexB23Rot = 0;
//    m_leftMiddleB23Rot = 0;
//    m_leftRingB23Rot = 0;
//    m_leftPinkyB23Rot = 0;


//    m_rightThumbB01Rot = 0;
//    m_rightIndexB01Rot = 0;
//    m_rightMiddleB01Rot = 0;
//    m_rightRingB01Rot = 0;
//    m_rightPinkyB01Rot = 0;

//    m_rightThumbB23Rot = 0;
//    m_rightIndexB23Rot = 0;
//    m_rightMiddleB23Rot = 0;
//    m_rightRingB23Rot = 0;
//    m_rightPinkyB23Rot = 0;


//    m_bisleft = true;
//    m_tempRotation = std::vector<double>(3, 0.f);


////    // test
////    Frame frame = m_controller.frame(0);
////    HandList handList = frame.hands();  // The list of Hand objects detected in this frame, given in arbitrary order. The list can be empty if no hands are detected.
////    FingerList fingerList = frame.fingers();  //The list of Finger objects detected in this frame, given in arbitrary order. The list can be empty if no fingers are detected.


////    int numberHand = handList.count();
////    int numberFingers = fingerList.count();

////    if(numberHand > 0)
////    {
////        Hand handLeft = handList.leftmost();
////        Hand handRight = handList.rightmost();
////        Vector directionHand = handLeft.direction(); // The direction from the palm position toward the fingers. The direction is
////        //expressed as a unit vector pointing in the same direction as the directed line from the palm position to the fingers.

////        float pitch = handLeft.direction().pitch();
////        float yaw   = handLeft.direction().yaw();
////        float roll  = handLeft.palmNormal().roll();

////        Vector palmPosition  =handLeft.palmPosition();


////        FingerList leftFingers = handLeft.fingers();
////    }

//}


//bool SWLeap::init()
//{
//    // The background frames policy determines whether an application receives frames of tracking data while in the background
//    m_controller.setPolicyFlags(Controller::POLICY_BACKGROUND_FRAMES);

//    int l_i32TimeOut = 0;

//    while(!m_controller.isConnected())
//    {
//        if(l_i32TimeOut > 10)
//        {
//            std::cerr << "No leap device detected, initialization aborted. " << std::endl;
//            return false;
//        }

//        ++l_i32TimeOut;

//        std::cout << "Waiting for leap device..." << std::endl;
//        boost::this_thread::sleep( boost::posix_time::seconds(1) );
//    }

//    if(m_controller.isConnected())
//    {
//        return true;
//    }

//    return false;
//}

//void SWLeap::stop()
//{
//    std::cout << "Leap device disconnected. " << std::endl;
//}

//float SWLeap::getPalmCoords(cuint ui32NumCoord,bool isleft)
//{
//    if(isleft)
//    {
//        return m_handpalmCoordsLeft[ui32NumCoord];
//    }
//    else return m_handpalmCoordsRight[ui32NumCoord];

//}

//double SWLeap::getFingerRotation(cuint ui32NumFinger, cuint ui32NumCouple,bool isleft)
//{
//    if(isleft)
//    {
//        if(ui32NumFinger == 1)
//        {
//            if(ui32NumCouple == 1) return m_leftThumbB01Rot;
//            else return m_leftThumbB23Rot;
//        }
//        else if(ui32NumFinger == 2)
//        {
//            if(ui32NumCouple == 1) return m_leftIndexB01Rot;
//            else return m_leftIndexB23Rot;
//        }
//        else if(ui32NumFinger == 3)
//        {
//            if(ui32NumCouple == 1) return m_leftMiddleB01Rot;
//            else return m_leftMiddleB23Rot;
//        }
//        else if(ui32NumFinger == 4)
//        {
//            if(ui32NumCouple == 1) return m_leftRingB01Rot;
//            else return m_leftRingB23Rot;
//        }
//        else
//        {
//            if(ui32NumCouple == 1) return m_leftPinkyB01Rot;
//            else return m_leftPinkyB23Rot;
//        }
//    }
//    else
//    {
//        if(ui32NumFinger == 1)
//        {
//            if(ui32NumCouple == 1) return m_rightThumbB01Rot;
//            else return m_rightThumbB23Rot;
//        }
//        else if(ui32NumFinger == 2)
//        {
//            if(ui32NumCouple == 1) return m_rightIndexB01Rot;
//            else return m_rightIndexB23Rot;
//        }
//        else if(ui32NumFinger == 3)
//        {
//            if(ui32NumCouple == 1) return m_rightMiddleB01Rot;
//            else return m_rightMiddleB23Rot;
//        }
//        else if(ui32NumFinger == 4)
//        {
//            if(ui32NumCouple == 1) return m_rightRingB01Rot;
//            else return m_rightRingB23Rot;
//        }
//        else
//        {
//            if(ui32NumCouple == 1) return m_rightPinkyB01Rot;
//            else return m_rightPinkyB23Rot;
//        }
//    }
//}


//float SWLeap::fingersCoords(cuint ui32NumFinger,cuint ui32NumCoord,bool isleft)
//{
//    if(isleft)
//    {
//        return m_vFingerCoordsLeft[ui32NumFinger][ui32NumCoord];
//    }
//    else return m_vFingerCoordsRight[ui32NumFinger][ui32NumCoord];

//}

//float SWLeap::getFingerDirection(cuint ui32NumFinger,cuint ui32NumDir,bool isleft)
//{

//    if(isleft)
//    {
//        return m_vFingerDirLeft[ui32NumFinger][ui32NumDir];
//    }
//    else return m_vFingerDirRight[ui32NumFinger][ui32NumDir];
//}

//float SWLeap::getFPS()
//{
//    return m_fps;
//}

//float SWLeap::getHandPitch(bool isleft)
//{

//    if(isleft)
//    {
//        return m_pitchLeft;
//    }
//    else return m_pitchRight;
//}

//float SWLeap::getHandRoll(bool isleft)
//{
//    if(isleft)
//    {
//        return m_rollLeft;
//    }
//    else return m_rollRight;
//}
//float SWLeap::getHandYaw(bool isleft)
//{
//    if(isleft)
//    {
//        return m_yawLeft;
//    }
//    else return m_yawRight;
//}

//float SWLeap::getHandSphereCoords(cuint ui32NumCoord,bool isleft)
//{
//    if(isleft)
//    {
//        return m_handsphereCoordsLeft[ui32NumCoord];
//    }
//    else return m_handsphereCoordsRight[ui32NumCoord];
//}

//float SWLeap::getHandSphereRadius(bool isleft)
//{
//    if(isleft)
//    {
//        return m_handsphereRadiusLeft;
//    }
//    else return m_handsphereRadiusRight;

//}

//std::vector<float> SWLeap::getHandPalmCoord(Leap::Hand temphand)
//{
//    Leap::Vector l_handpalmCoords(temphand.palmPosition());
//    std::vector<float> temp_palmCoords(3, 0.f);

//    temp_palmCoords[0]=l_handpalmCoords[0];
//    temp_palmCoords[1]=l_handpalmCoords[1];
//    temp_palmCoords[2]=l_handpalmCoords[2];

//    return temp_palmCoords;

//}

//std::vector<float> SWLeap::getHandSphereCenter(Leap::Hand temphand)
//{
//    Leap::Vector l_handsphereCoords(temphand.sphereCenter());
//    std::vector<float> temp_spherecenter(3, 0.f);

//    temp_spherecenter[0]=l_handsphereCoords[0];
//    temp_spherecenter[1]=l_handsphereCoords[1];
//    temp_spherecenter[2]=l_handsphereCoords[2];

//    return temp_spherecenter;

//}

//bool SWLeap::setHandRotation(Leap::Hand temphand,bool isleft)
//{
//    // Get the hand's normal vector and direction
//    const Vector l_normal = temphand.palmNormal();
//    const Vector l_direction = temphand.direction();

//    //setting the angles of the hand
//    if(isleft)
//    {
//        m_pitchLeft=l_direction.pitch();
//        m_rollLeft=l_normal.roll();
//        m_yawLeft=l_direction.yaw();
//    }
//    else
//    {
//        m_pitchRight=l_direction.pitch();
//        m_rollRight=l_normal.roll();
//        m_yawRight=l_direction.yaw();
//    }
//    return true;
//}

//bool SWLeap::setFingers(Leap::Frame frame,Leap::FingerList l_fingers,bool isleft)
//{
//            std::vector<float> l_finger1Coords(3, 0.f);
//            std::vector<float> l_finger2Coords(3, 0.f);
//            std::vector<float> l_finger3Coords(3, 0.f);
//            std::vector<float> l_finger4Coords(3, 0.f);
//            std::vector<float> l_finger5Coords(3, 0.f);

//            std::vector<float> l_finger1Direction(3, 0.f);
//            std::vector<float> l_finger2Direction(3, 0.f);
//            std::vector<float> l_finger3Direction(3, 0.f);
//            std::vector<float> l_finger4Direction(3, 0.f);
//            std::vector<float> l_finger5Direction(3, 0.f);


//            if(!l_fingers.isEmpty())
//            {
//                Leap::Finger tempfinger;
//                Leap::Hand temphand;

//                for (int f = 0; f < frame.fingers().count(); f++)
//                {
//                    tempfinger = frame.fingers()[f];
//                    temphand = tempfinger.hand();

//                    if(tempfinger.type() == 0)//TYPE_THUMB
//                    {
//                        if(temphand.isLeft())
//                        {
//                            m_leftThumb=tempfinger;
//                        }
//                        else
//                        {
//                            m_rightThumb=tempfinger;
//                        }
//                    }
//                    else if(tempfinger.type() == 1)//TYPE_INDEX
//                    {
//                        if(temphand.isLeft())
//                        {
//                            m_leftIndex=tempfinger;
//                        }
//                        else
//                        {
//                            m_rightIndex=tempfinger;
//                        }
//                    }
//                    else if(tempfinger.type() == 2)//TYPE_MIDDLE
//                    {
//                        if(temphand.isLeft())
//                        {
//                            m_leftMiddle=tempfinger;
//                        }
//                        else
//                        {
//                            m_rightMiddle=tempfinger;
//                        }
//                    }
//                    else if(tempfinger.type() == 3)//TYPE_RING
//                    {
//                        if(temphand.isLeft())
//                        {
//                            m_leftRing=tempfinger;
//                        }
//                        else
//                        {
//                            m_rightRing=tempfinger;
//                        }
//                    }
//                    else //tempfinger.type() == 4 //TYPE_PINKY
//                    {
//                        if(temphand.isLeft())
//                        {
//                            m_leftPinky=tempfinger;
//                        }
//                        else
//                        {
//                            m_rightPinky=tempfinger;
//                        }
//                    }

//                }

//                Leap::Vector l_fingervalue, l_fingerdir;
//                if(isleft)
//                {
//                    //We must use a Leap::Vector to take information before doing the copy....

//                    //POS

//                        l_fingervalue = m_leftThumb.tipPosition();

//                            l_finger1Coords[0] = l_fingervalue [0];
//                            l_finger1Coords[1] = l_fingervalue [1];
//                            l_finger1Coords[2] = l_fingervalue [2];

//                        l_fingervalue = m_leftIndex.tipPosition();

//                            l_finger2Coords[0] = l_fingervalue [0];
//                            l_finger2Coords[1] = l_fingervalue [1];
//                            l_finger2Coords[2] = l_fingervalue [2];

//                        l_fingervalue = m_leftMiddle.tipPosition();

//                            l_finger3Coords[0] = l_fingervalue [0];
//                            l_finger3Coords[1] = l_fingervalue [1];
//                            l_finger3Coords[2] = l_fingervalue [2];

//                        l_fingervalue = m_leftRing.tipPosition();

//                            l_finger4Coords[0] = l_fingervalue [0];
//                            l_finger4Coords[1] = l_fingervalue [1];
//                            l_finger4Coords[2] = l_fingervalue [2];

//                        l_fingervalue = m_leftPinky.tipPosition();

//                            l_finger5Coords[0] = l_fingervalue [0];
//                            l_finger5Coords[1] = l_fingervalue [1];
//                            l_finger5Coords[2] = l_fingervalue [2];



//                        m_vFingerCoordsLeft[0] = l_finger1Coords;
//                        m_vFingerCoordsLeft[1] = l_finger2Coords;
//                        m_vFingerCoordsLeft[2] = l_finger3Coords;
//                        m_vFingerCoordsLeft[3] = l_finger4Coords;
//                        m_vFingerCoordsLeft[4] = l_finger5Coords;


//                    //DIR

//                        l_fingerdir = m_leftThumb.direction();

//                            l_finger1Direction[0] = l_fingerdir [0];
//                            l_finger1Direction[1] = l_fingerdir [1];
//                            l_finger1Direction[2] = l_fingerdir [2];

//                        l_fingerdir = m_leftIndex.direction();

//                            l_finger2Direction[0] = l_fingerdir [0];
//                            l_finger2Direction[1] = l_fingerdir [1];
//                            l_finger2Direction[2] = l_fingerdir [2];

//                        l_fingerdir = m_leftMiddle.direction();

//                            l_finger3Direction[0] = l_fingerdir [0];
//                            l_finger3Direction[1] = l_fingerdir [1];
//                            l_finger3Direction[2] = l_fingerdir [2];

//                        l_fingerdir = m_leftRing.direction();

//                            l_finger4Direction[0] = l_fingerdir [0];
//                            l_finger4Direction[1] = l_fingerdir [1];
//                            l_finger4Direction[2] = l_fingerdir [2];

//                        l_fingerdir = m_leftPinky.direction();

//                            l_finger5Direction[0] = l_fingerdir [0];
//                            l_finger5Direction[1] = l_fingerdir [1];
//                            l_finger5Direction[2] = l_fingerdir [2];


//                        m_vFingerCoordsLeft[0] = l_finger1Direction;
//                        m_vFingerCoordsLeft[1] = l_finger2Direction;
//                        m_vFingerCoordsLeft[2] = l_finger3Direction;
//                        m_vFingerCoordsLeft[3] = l_finger4Direction;
//                        m_vFingerCoordsLeft[4] = l_finger5Direction;
//                }
//                else
//                {
//                    //POS

//                        l_fingervalue = m_rightThumb.tipPosition();

//                            l_finger1Coords[0] = l_fingervalue [0];
//                            l_finger1Coords[1] = l_fingervalue [1];
//                            l_finger1Coords[2] = l_fingervalue [2];

//                        l_fingervalue = m_rightIndex.tipPosition();

//                            l_finger2Coords[0] = l_fingervalue [0];
//                            l_finger2Coords[1] = l_fingervalue [1];
//                            l_finger2Coords[2] = l_fingervalue [2];

//                        l_fingervalue = m_rightMiddle.tipPosition();

//                            l_finger3Coords[0] = l_fingervalue [0];
//                            l_finger3Coords[1] = l_fingervalue [1];
//                            l_finger3Coords[2] = l_fingervalue [2];

//                        l_fingervalue = m_rightRing.tipPosition();

//                            l_finger4Coords[0] = l_fingervalue [0];
//                            l_finger4Coords[1] = l_fingervalue [1];
//                            l_finger4Coords[2] = l_fingervalue [2];

//                        l_fingervalue = m_rightPinky.tipPosition();

//                            l_finger5Coords[0] = l_fingervalue [0];
//                            l_finger5Coords[1] = l_fingervalue [1];
//                            l_finger5Coords[2] = l_fingervalue [2];



//                        m_vFingerCoordsRight[0] = l_finger1Coords;
//                        m_vFingerCoordsRight[1] = l_finger2Coords;
//                        m_vFingerCoordsRight[2] = l_finger3Coords;
//                        m_vFingerCoordsRight[3] = l_finger4Coords;
//                        m_vFingerCoordsRight[4] = l_finger5Coords;


//                    //DIR

//                        l_fingerdir = m_rightThumb.direction();

//                            l_finger1Direction[0] = l_fingerdir [0];
//                            l_finger1Direction[1] = l_fingerdir [1];
//                            l_finger1Direction[2] = l_fingerdir [2];

//                        l_fingerdir = m_rightIndex.direction();

//                            l_finger2Direction[0] = l_fingerdir [0];
//                            l_finger2Direction[1] = l_fingerdir [1];
//                            l_finger2Direction[2] = l_fingerdir [2];

//                        l_fingerdir = m_rightMiddle.direction();

//                            l_finger3Direction[0] = l_fingerdir [0];
//                            l_finger3Direction[1] = l_fingerdir [1];
//                            l_finger3Direction[2] = l_fingerdir [2];

//                        l_fingerdir = m_rightRing.direction();

//                            l_finger4Direction[0] = l_fingerdir [0];
//                            l_finger4Direction[1] = l_fingerdir [1];
//                            l_finger4Direction[2] = l_fingerdir [2];

//                        l_fingerdir = m_rightPinky.direction();

//                            l_finger5Direction[0] = l_fingerdir [0];
//                            l_finger5Direction[1] = l_fingerdir [1];
//                            l_finger5Direction[2] = l_fingerdir [2];


//                        m_vFingerCoordsRight[0] = l_finger1Direction;
//                        m_vFingerCoordsRight[1] = l_finger2Direction;
//                        m_vFingerCoordsRight[2] = l_finger3Direction;
//                        m_vFingerCoordsRight[3] = l_finger4Direction;
//                        m_vFingerCoordsRight[4] = l_finger5Direction;
//                }
//            }


//    return setBones(isleft);
//}
//std::vector<double> SWLeap::computeEulerYPRAngles(Leap::Matrix l_tempMat)
//{


//        Vector xBasis = l_tempMat.xBasis;
//        Vector yBasis = l_tempMat.yBasis;
//        Vector zBasis = l_tempMat.zBasis;

//        double l_matrix[3][3];

//        for(int ii = 0;ii<3;ii++)
//        {
//            for(int jj = 0;jj<3;jj++)
//            {
//                if(ii==0) l_matrix[ii][jj]=xBasis[jj];
//                if(ii==1) l_matrix[ii][jj]=yBasis[jj];
//                if(ii==2) l_matrix[ii][jj]=zBasis[jj];

//            }
//        }

//        double yaw,pitch,roll;// newdata;
//        int rowFlag = (int) l_matrix[2][0];
//        switch (rowFlag) {
//        case 1:
//            roll = std::atan2(-l_matrix[0][1], l_matrix[1][1]) * 180 / PI;
//            yaw = -90.0;
//            pitch = 0.0;
//            break;
//        case -1:
//            roll = std::atan2(-l_matrix[0][1], l_matrix[1][1])* 180 / PI;
//            yaw = 90.0;
//            pitch = 0.0;
//            break;
//        default:
//            roll = std::atan2(l_matrix[1][0], l_matrix[0][0])* 180 / PI;
//            yaw = std::asin(-l_matrix[2][0])* 180 / PI;
//            pitch = std::atan2(l_matrix[2][1], l_matrix[2][2])* 180 / PI;
//            break;
//        }
//        //For the Roll, the matrice gives us value from [X<0,X>-180][X<180,X>0]
//        //that's why we do this. The issue come probably from the matrice.
//        if(roll<0)
//        {
//            roll = -180 - roll;
//        }
//        else
//        {
//            roll = 180 - roll;
//        }
//        if(isMetacarp == true)
//        {
//            m_tempRotation[0] = pitch;
//            m_tempRotation[1] = yaw;
//            m_tempRotation[2] = roll;
//            return m_tempRotation;
//        }

//        std::vector<double> l_tempRotation;
//        l_tempRotation = std::vector<double>(3, 0.f);
//        l_tempRotation[0] = pitch;
//        l_tempRotation[1] = yaw;
//        l_tempRotation[2] = roll;

//    //	std::cout<<"-------------------------------------------------------------"<<std::endl;
//    //	std::cout<<"Pitch : "<<pitch<<" Roll : "<<roll<<" Yaw : "<<yaw<<std::endl;


//        double l_matrixTempAlpha[3][3];
//        double l_matrixTempBeta[3][3];
//        double l_matrixTempGamma[3][3];

//        double l_matrixRotation[3][3];
//        double l_matrixFinal[3][3];

//        //Matrice INIT
//        for(int i=0; i<3;i++)
//        {
//            for(int ii=0; ii<3;ii++)
//            {
//                l_matrixTempAlpha[i][ii] = 0;
//                l_matrixTempBeta[i][ii] = 0;
//                l_matrixTempGamma[i][ii] = 0;
//                l_matrixRotation[i][ii] = 0;
//                l_matrixFinal[i][ii] = 0;
//            }
//        }
//        //MATRIX ROTATION ABOUT X
//        l_matrixTempAlpha[0][0] = 1;
//        l_matrixTempAlpha[1][1] = std::cos(PI * pitch / 180);
//        l_matrixTempAlpha[2][2] = std::cos(PI * pitch / 180);
//        l_matrixTempAlpha[2][1] = std::sin(PI * pitch / 180);
//        l_matrixTempAlpha[1][2] = - std::sin(PI * pitch / 180);

//        //MATRIX ROTATION ABOUT Y
//        l_matrixTempBeta[0][0] = std::cos(PI * yaw / 180);
//        l_matrixTempBeta[1][1] = 1;
//        l_matrixTempBeta[2][2] = std::cos(PI * yaw / 180);
//        l_matrixTempBeta[0][2] = std::sin(PI * yaw / 180);
//        l_matrixTempBeta[2][0] = - std::sin(PI * yaw / 180);

//        //MATRIX ROTATION ABOUT Z
//        l_matrixTempGamma[0][0] = std::cos(PI * roll / 180);
//        l_matrixTempGamma[1][1] = std::cos(PI * roll / 180);
//        l_matrixTempGamma[2][2] = 1;
//        l_matrixTempGamma[1][0] = std::sin(PI * roll / 180);
//        l_matrixTempGamma[0][1] = - std::sin(PI * roll / 180);


//        //A * B = TEMP
//        for(int i = 0; i<3; i++)
//        {
//            for(int ii = 0; ii<3;ii++)
//            {
//                l_matrixRotation[i][ii]=l_matrixTempAlpha[i][0]*l_matrixTempBeta[0][ii] + l_matrixTempAlpha[i][1]*l_matrixTempBeta[1][ii] + l_matrixTempAlpha[i][2]*l_matrixTempBeta[2][ii];
//            }
//        }
//        //TEMP * C = FINAL

//        for(int i = 0; i<3; i++)
//        {
//            for(int ii = 0; ii<3;ii++)
//            {
//                l_matrixFinal[i][ii]=l_matrixRotation[i][0]*l_matrixTempGamma[0][ii] + l_matrixRotation[i][1]*l_matrixTempGamma[1][ii] + l_matrixRotation[i][2]*l_matrixTempGamma[2][ii];

//            }
//        }

//        double oldAngles[3];
//        double newAngles[3];
//        oldAngles[0]=roll;
//        oldAngles[1]=pitch;
//        oldAngles[2]=yaw;

//        for(int i = 0;i<3;i++)
//        {
//            newAngles[i]= l_matrixFinal[i][0] * oldAngles[0] +  l_matrixFinal[i][1] * oldAngles[1] +  l_matrixFinal[i][2] * oldAngles[2];
//        }

//    //	std::cout<<"NewRoll : "<<newAngles[0]<<" NewPitch : "<<newAngles[1]<<" NewYaw : "<<newAngles[2]<<std::endl;

//        //A CHANGER CAR TEST MATRIX
//        l_tempRotation[0] = newAngles[0];
//        l_tempRotation[1] = newAngles[1];
//        l_tempRotation[2] = newAngles[2];


//        /*
//double MatrixRotTEST[3][3];

//double A = std::cos(PI * m_tempRotation[0] / 180);
//double B = std::sin(PI * m_tempRotation[0] / 180);
//double C = std::cos(PI * m_tempRotation[1] / 180);
//double D = std::sin(PI * m_tempRotation[1] / 180);
//double E = std::cos(PI * m_tempRotation[2] / 180);
//double F = std::sin(PI * m_tempRotation[2] / 180);


//double	AD =   A * D;
//double	BD =   B * D;

//        MatrixRotTEST[0][0]  =   C * E;
//        MatrixRotTEST[0][1]  =  -C * F;
//        MatrixRotTEST[0][2]  =  D;
//        MatrixRotTEST[1][0]  =  BD * E + A * F;
//        MatrixRotTEST[1][1]  =  -BD * F + A * E;
//        MatrixRotTEST[1][2]  =  -B * C;
//        MatrixRotTEST[2][0]  = -AD * E + B * F;
//        MatrixRotTEST[2][1]  =  AD * F + B * E;
//        MatrixRotTEST[2][2] =   A * C;



//        //std::cout<<"FINAL TEST MATRIX"<<std::endl;
//        for(int i = 0; i<3; i++)
//        {
//        //	std::cout<<"| "<<MatrixRotTEST[i][0]<<" | "<<MatrixRotTEST[i][1]<<" | "<<MatrixRotTEST[i][2]<<std::endl;
//        }
//        std::cout<<"---------------------------------"<<std::endl;

//        for(int i = 0;i<3;i++)
//        {
//            newcoord[i]= MatrixRotTEST[i][0] * oldcoord[0] +  MatrixRotTEST[i][1] * oldcoord[1] +  MatrixRotTEST[i][2] * oldcoord[2];
//        }
//        std::cout<<"											2dn TWO| "<<newcoord[0]<<" | "<<newcoord[1]<<" | "<<newcoord[2]<<std::endl;
//        */
//        return l_tempRotation;



//}


//bool SWLeap::setBones(bool isleft)
//{
//    Leap::Bone::Type l_Metacarpal = static_cast<Leap::Bone::Type>(0);
//    Leap::Bone::Type l_Proximal = static_cast<Leap::Bone::Type>(1);
//    Leap::Bone::Type l_Intermediate = static_cast<Leap::Bone::Type>(2);
//    Leap::Bone::Type l_Distal = static_cast<Leap::Bone::Type>(3);


//    if(isleft)
//    {

//        //thumb
//            Leap::Bone l_LeftMetacarpalT = m_leftThumb.bone(l_Metacarpal); // = 0
//            Leap::Bone l_LeftProximalT = m_leftThumb.bone(l_Proximal);
//            Leap::Bone l_LeftIntermediateT = m_leftThumb.bone(l_Intermediate);
//            Leap::Bone l_LeftDistalT = m_leftThumb.bone(l_Distal);

//        //Index

//            Leap::Bone l_LeftMetacarpalI = m_leftIndex.bone(l_Metacarpal);
//            Leap::Bone l_LeftProximalI = m_leftIndex.bone(l_Proximal);
//            Leap::Bone l_LeftIntermediateI = m_leftIndex.bone(l_Intermediate);
//            Leap::Bone l_LeftDistalI = m_leftIndex.bone(l_Distal);

//        //Middle
//            Leap::Bone l_LeftMetacarpalM = m_leftMiddle.bone(l_Metacarpal);
//            Leap::Bone l_LeftProximalM = m_leftMiddle.bone(l_Proximal);
//            Leap::Bone l_LeftIntermediateM = m_leftMiddle.bone(l_Intermediate);
//            Leap::Bone l_LeftDistalM = m_leftMiddle.bone(l_Distal);

//        //Ring
//            Leap::Bone l_LeftMetacarpalR = m_leftRing.bone(l_Metacarpal);
//            Leap::Bone l_LeftProximalR = m_leftRing.bone(l_Proximal);
//            Leap::Bone l_LeftIntermediateR = m_leftRing.bone(l_Intermediate);
//            Leap::Bone l_LeftDistalR = m_leftRing.bone(l_Distal);

//        //Pinky
//            Leap::Bone l_LeftMetacarpalP = m_leftPinky.bone(l_Metacarpal);
//            Leap::Bone l_LeftProximalP = m_leftPinky.bone(l_Proximal);
//            Leap::Bone l_LeftIntermediateP = m_leftPinky.bone(l_Intermediate);
//            Leap::Bone l_LeftDistalP = m_leftPinky.bone(l_Distal);


//    //SETTING BONES ROTATION
//        //Thumb
//        std::vector<double> l_Left2M = setBoneRotation(l_LeftMetacarpalT); //Equals 0; there is no bone here (thumb contains 3 bones instead of 4)
//            isMetacarp = true;
//            std::vector<double> l_Left2P = setBoneRotation(l_LeftProximalT);
//            isMetacarp = false;
//            std::vector<double> l_Left2I = setBoneRotation(l_LeftIntermediateT);
//            std::vector<double> l_Left2D = setBoneRotation(l_LeftDistalT);

//            m_leftThumbB01Rot = l_Left2P[1];// + l_Left2P[1] + l_Left2P[2];
//            m_leftThumbB23Rot =  (l_Left2I[1] - l_Left2P[1]) + (l_Left2D[1] - l_Left2I[1]); // + (l_Left2I[1] - l_Left2P[1]) + (l_Left2D[1] - l_Left2I[1]) + (l_Left2I[2] - l_Left2P[2]) + (l_Left2D[2] - l_Left2I[2])

//        //Index
//            isMetacarp = true;
//            l_Left2M = setBoneRotation(l_LeftMetacarpalI);
//            isMetacarp = false;
//            l_Left2P = setBoneRotation(l_LeftProximalI);
//            l_Left2I = setBoneRotation(l_LeftIntermediateI);
//            l_Left2D = setBoneRotation(l_LeftDistalI);



//            m_leftIndexB01Rot = l_Left2P[0] + l_Left2P[1] + l_Left2P[2]; // (l_Left2P[0] - l_Left2M[0]) original l_Left2P[0] + l_Left2P[2]
//            m_leftIndexB23Rot = (l_Left2I[1] - l_Left2P[1]) + (l_Left2D[1] - l_Left2I[1]) ;//+ (l_Left2I[1] - l_Left2P[1]) + (l_Left2D[1] - l_Left2I[1]) + (l_Left2I[2] - l_Left2P[2]) + (l_Left2D[2] - l_Left2I[2]);// original
//            if(m_leftIndexB23Rot<0)
//            {
//                m_leftIndexB23Rot=m_leftIndexB23Rot*-1;
//            }


//        //Middle
//            isMetacarp = true;
//            l_Left2M = setBoneRotation(l_LeftMetacarpalM);
//            isMetacarp = false;
//            l_Left2P = setBoneRotation(l_LeftProximalM);
//            l_Left2I = setBoneRotation(l_LeftIntermediateM);
//            l_Left2D = setBoneRotation(l_LeftDistalM);


//            m_leftMiddleB01Rot = l_Left2P[0] + l_Left2P[1] + l_Left2P[2];
//            m_leftMiddleB23Rot = (l_Left2I[1] - l_Left2P[1]) + (l_Left2D[1] - l_Left2I[1]);//(l_Left2I[0] - l_Left2P[0]) + (l_Left2D[0] - l_Left2I[0]) + (l_Left2I[1] - l_Left2P[1]) + (l_Left2D[1] - l_Left2I[1]) + (l_Left2I[2] - l_Left2P[2]) + (l_Left2D[2] - l_Left2I[2]);

//            if(m_leftMiddleB23Rot<0)
//            {
//                m_leftMiddleB23Rot=m_leftMiddleB23Rot*-1;
//            }

//        //Ring
//            isMetacarp = true;
//            l_Left2M = setBoneRotation(l_LeftMetacarpalR);
//            isMetacarp = false;
//            l_Left2P = setBoneRotation(l_LeftProximalR);
//            l_Left2I = setBoneRotation(l_LeftIntermediateR);
//            l_Left2D = setBoneRotation(l_LeftDistalR);

//            m_leftRingB01Rot = l_Left2P[1];
//            m_leftRingB23Rot = (l_Left2I[1] - l_Left2P[1]) + (l_Left2D[1] - l_Left2I[1]);//(l_Left2I[0] - l_Left2P[0]) + (l_Left2D[0] - l_Left2I[0]) + (l_Left2I[1] - l_Left2P[1]) + (l_Left2D[1] - l_Left2I[1]) + (l_Left2I[2] - l_Left2P[2]) + (l_Left2D[2] - l_Left2I[2]);

//            if(m_leftRingB23Rot<0)
//            {
//                m_leftRingB23Rot=m_leftRingB23Rot*-1;
//            }
//        //Pinky
//            isMetacarp = true;
//            l_Left2M = setBoneRotation(l_LeftMetacarpalP);
//            isMetacarp = false;
//            l_Left2P = setBoneRotation(l_LeftProximalP);
//            l_Left2I = setBoneRotation(l_LeftIntermediateP);
//            l_Left2D = setBoneRotation(l_LeftDistalP);

//            m_leftPinkyB01Rot =  l_Left2P[1];
//            m_leftPinkyB23Rot = (l_Left2I[1] - l_Left2P[1]) + (l_Left2D[1] - l_Left2I[1]);// (l_Left2I[0] - l_Left2P[0]) + (l_Left2D[0] - l_Left2I[0]) + (l_Left2I[1] - l_Left2P[1]) + (l_Left2D[1] - l_Left2I[1]) + (l_Left2I[2] - l_Left2P[2]) + (l_Left2D[2] - l_Left2I[2]);

//            if(m_leftPinkyB23Rot<0)
//            {
//                m_leftPinkyB23Rot=m_leftPinkyB23Rot*-1;
//            }
//    }
//    else
//    {
//        //thumb
//            Leap::Bone l_RightMetacarpalT = m_rightThumb.bone(l_Metacarpal); // = 0
//            Leap::Bone l_RightProximalT = m_rightThumb.bone(l_Proximal);
//            Leap::Bone l_RightIntermediateT = m_rightThumb.bone(l_Intermediate);
//            Leap::Bone l_RightDistalT = m_rightThumb.bone(l_Distal);

//        //Index

//            Leap::Bone l_RightMetacarpalI = m_rightIndex.bone(l_Metacarpal);
//            Leap::Bone l_RightProximalI = m_rightIndex.bone(l_Proximal);
//            Leap::Bone l_RightIntermediateI = m_rightIndex.bone(l_Intermediate);
//            Leap::Bone l_RightDistalI = m_rightIndex.bone(l_Distal);

//        //Middle
//            Leap::Bone l_RightMetacarpalM = m_rightMiddle.bone(l_Metacarpal);
//            Leap::Bone l_RightProximalM = m_rightMiddle.bone(l_Proximal);
//            Leap::Bone l_RightIntermediateM = m_rightMiddle.bone(l_Intermediate);
//            Leap::Bone l_RightDistalM = m_rightMiddle.bone(l_Distal);

//        //Ring
//            Leap::Bone l_RightMetacarpalR = m_rightRing.bone(l_Metacarpal);
//            Leap::Bone l_RightProximalR = m_rightRing.bone(l_Proximal);
//            Leap::Bone l_RightIntermediateR = m_rightRing.bone(l_Intermediate);
//            Leap::Bone l_RightDistalR = m_rightRing.bone(l_Distal);

//        //Pinky
//            Leap::Bone l_RightMetacarpalP = m_rightPinky.bone(l_Metacarpal);
//            Leap::Bone l_RightProximalP = m_rightPinky.bone(l_Proximal);
//            Leap::Bone l_RightIntermediateP = m_rightPinky.bone(l_Intermediate);
//            Leap::Bone l_RightDistalP = m_rightPinky.bone(l_Distal);


//    //SETTING BONES ROTATION
//        //Thumb
//        std::vector<double> l_Right2M = setBoneRotation(l_RightMetacarpalT); //Equals 0; there is no bone here (thumb contains 3 bones instead of 4)
//            isMetacarp = true;
//            std::vector<double> l_Right2P = setBoneRotation(l_RightProximalT);
//            isMetacarp = false;
//            std::vector<double> l_Right2I = setBoneRotation(l_RightIntermediateT);
//            std::vector<double> l_Right2D = setBoneRotation(l_RightDistalT);

//            m_rightThumbB01Rot = l_Right2P[1];// + l_Right2P[1] + l_Right2P[2];
//            m_rightThumbB23Rot =  (l_Right2I[1] - l_Right2P[1]) + (l_Right2D[1] - l_Right2I[1]); // + (l_Right2I[1] - l_Right2P[1]) + (l_Right2D[1] - l_Right2I[1]) + (l_Right2I[2] - l_Right2P[2]) + (l_Right2D[2] - l_Right2I[2])

//        //Index
//            isMetacarp = true;
//            l_Right2M = setBoneRotation(l_RightMetacarpalI);
//            isMetacarp = false;
//            l_Right2P = setBoneRotation(l_RightProximalI);
//            l_Right2I = setBoneRotation(l_RightIntermediateI);
//            l_Right2D = setBoneRotation(l_RightDistalI);



//            m_rightIndexB01Rot = l_Right2P[0] + l_Right2P[1] + l_Right2P[2]; // (l_Right2P[0] - l_Right2M[0]) original l_Right2P[0] + l_Right2P[2]
//            m_rightIndexB23Rot = (l_Right2I[1] - l_Right2P[1]) + (l_Right2D[1] - l_Right2I[1]) ;//+ (l_Right2I[1] - l_Right2P[1]) + (l_Right2D[1] - l_Right2I[1]) + (l_Right2I[2] - l_Right2P[2]) + (l_Right2D[2] - l_Right2I[2]);// original
//            if(m_rightIndexB23Rot<0)
//            {
//                m_rightIndexB23Rot=m_rightIndexB23Rot*-1;
//            }


//        //Middle
//            isMetacarp = true;
//            l_Right2M = setBoneRotation(l_RightMetacarpalM);
//            isMetacarp = false;
//            l_Right2P = setBoneRotation(l_RightProximalM);
//            l_Right2I = setBoneRotation(l_RightIntermediateM);
//            l_Right2D = setBoneRotation(l_RightDistalM);


//            m_rightMiddleB01Rot = l_Right2P[0] + l_Right2P[1] + l_Right2P[2];
//            m_rightMiddleB23Rot = (l_Right2I[1] - l_Right2P[1]) + (l_Right2D[1] - l_Right2I[1]);//(l_Right2I[0] - l_Right2P[0]) + (l_Right2D[0] - l_Right2I[0]) + (l_Right2I[1] - l_Right2P[1]) + (l_Right2D[1] - l_Right2I[1]) + (l_Right2I[2] - l_Right2P[2]) + (l_Right2D[2] - l_Right2I[2]);

//            if(m_rightMiddleB23Rot<0)
//            {
//                m_rightMiddleB23Rot=m_rightMiddleB23Rot*-1;
//            }

//        //Ring
//            isMetacarp = true;
//            l_Right2M = setBoneRotation(l_RightMetacarpalR);
//            isMetacarp = false;
//            l_Right2P = setBoneRotation(l_RightProximalR);
//            l_Right2I = setBoneRotation(l_RightIntermediateR);
//            l_Right2D = setBoneRotation(l_RightDistalR);

//            m_rightRingB01Rot = l_Right2P[1];
//            m_rightRingB23Rot = (l_Right2I[1] - l_Right2P[1]) + (l_Right2D[1] - l_Right2I[1]);//(l_Right2I[0] - l_Right2P[0]) + (l_Right2D[0] - l_Right2I[0]) + (l_Right2I[1] - l_Right2P[1]) + (l_Right2D[1] - l_Right2I[1]) + (l_Right2I[2] - l_Right2P[2]) + (l_Right2D[2] - l_Right2I[2]);

//            if(m_rightRingB23Rot<0)
//            {
//                m_rightRingB23Rot=m_rightRingB23Rot*-1;
//            }
//        //Pinky
//            isMetacarp = true;
//            l_Right2M = setBoneRotation(l_RightMetacarpalP);
//            isMetacarp = false;
//            l_Right2P = setBoneRotation(l_RightProximalP);
//            l_Right2I = setBoneRotation(l_RightIntermediateP);
//            l_Right2D = setBoneRotation(l_RightDistalP);

//            m_rightPinkyB01Rot =  l_Right2P[1];
//            m_rightPinkyB23Rot = (l_Right2I[1] - l_Right2P[1]) + (l_Right2D[1] - l_Right2I[1]);// (l_Right2I[0] - l_Right2P[0]) + (l_Right2D[0] - l_Right2I[0]) + (l_Right2I[1] - l_Right2P[1]) + (l_Right2D[1] - l_Right2I[1]) + (l_Right2I[2] - l_Right2P[2]) + (l_Right2D[2] - l_Right2I[2]);

//            if(m_rightPinkyB23Rot<0)
//            {
//                m_rightPinkyB23Rot=m_rightPinkyB23Rot*-1;
//            }

//    }
//    return true;
//}

//std::vector<double> SWLeap::setBoneRotation(Leap::Bone bone)
//{
//    Leap::Matrix l_matTemp = bone.basis();
//    return computeEulerYPRAngles(l_matTemp);
//}


//bool SWLeap::read()
//{
//    if(m_controller.isConnected())
//    {
//        Frame l_oFrame = m_controller.frame(); // retrieve the last frame

//        m_fps=l_oFrame.currentFramesPerSecond(); //getting the FPS

//        if (!l_oFrame.hands().isEmpty())
//        {
//            Hand l_leftHand, l_rightHand, l_tempHand;

//            //if there are more than 1 hand, take the leftmost
//            l_tempHand = l_oFrame.hands()[0];

//            if(l_tempHand.isLeft())
//            {
//                l_leftHand = l_tempHand;//l_oFrame.hands()[0];


//                if (l_oFrame.hands().count() > 1)
//                {

//                    l_tempHand = l_oFrame.hands()[1];

//                    if(!l_tempHand.isLeft()) //their must not be two left hand ...
//                    {
//                        l_rightHand = l_tempHand;

//                    }
//                }
//            }
//            else
//            {
//                l_rightHand = l_oFrame.hands()[0];

//                if (l_oFrame.hands().count() > 1)
//                {
//                    l_tempHand = l_oFrame.hands()[1];
//                    if(l_tempHand.isLeft()) //their must not be two Right Hands hand ...
//                    {
//                        l_leftHand = l_tempHand;
//                    }
//                }
//            }

//            //Variables
//            if(l_leftHand.isValid())
//            {
//                m_bisleft = true;
//                // Get the first hand
//                m_handpalmCoordsLeft = getHandPalmCoord(l_leftHand);

//                // Get the Sphere takes in the hand
//                m_handsphereCoordsLeft = getHandSphereCenter(l_leftHand);

//                m_handsphereRadiusLeft = l_leftHand.sphereRadius(); //Radius of the sphere

//                // Set the hand's normal vector and direction
//                if(!setHandRotation(l_leftHand,m_bisleft))
//                {
//                    std::cout<<"Error, cannot set hand rotation"<<std::endl;
//                }
//                /*
//                if( (rand() % 100) > 80 )
//                {
//                std::cout<<"Hand Palm Coordinnates : X = "<<m_handpalmCoordsLeft[0]<<" Y = "<<m_handpalmCoordsLeft[1]<<" Z = "<<m_handpalmCoordsLeft[2]<<std::endl;
//                std::cout << "Hand pitch: " << m_pitchLeft  << " rad, "
//                      << "roll: " <<m_rollLeft<< " rad, "
//                      << "yaw: " <<m_yawLeft << "rad" << std::endl; //RAD_TO_DEG
//                }*/

//                const FingerList l_fingersLeft = l_leftHand.fingers();

//                if (!l_fingersLeft.isEmpty())
//                {
//                    if(!setFingers(l_oFrame,l_fingersLeft,m_bisleft))
//                    {
//                        std::cout<<"Error Setting fingers Pos/Dir, Default Value will be send "<<std::endl;
//                    }
//                }
//            }

//            if(l_rightHand.isValid())
//            {
//                m_bisleft = false;
//                // Get the first hand
//                m_handpalmCoordsRight = getHandPalmCoord(l_rightHand);

//                // Get the Sphere takes in the hand
//                m_handsphereCoordsRight = getHandSphereCenter(l_rightHand);

//                m_handsphereRadiusRight = l_rightHand.sphereRadius(); //Radius of the sphere

//                // Set the hand's normal vector and direction
//                if(!setHandRotation(l_rightHand,m_bisleft))
//                {
//                    std::cout<<"Error, cannot set hand rotation"<<std::endl;
//                }

//                const FingerList l_fingersRight = l_rightHand.fingers();

//                if (!l_fingersRight.isEmpty())
//                {
//                    if(!setFingers(l_oFrame,l_fingersRight,m_bisleft))
//                    {
//                        std::cout<<"Error Setting fingers Pos/Dir, Default Value will be send "<<std::endl;
//                    }
//                }
//            }
//        }
//        return true;
//    }

//    std::cout<<"The device is not connected/plugged !"<<std::endl;
//    return false;
//}


#include "devices/leap/SWLeap.h"


#define PI 3.14159265f

using namespace swDevice;


//bool SWLeap::directionFinger(cbool leftHand, const Leap::Finger::Type fingerType, std::vector<float> &vDirectionFinger) const
//{
//    Leap::FingerList fingerList;

//    if(leftHand)
//    {
//        if(m_idLeftHand != -1)
//        {
//            fingerList = m_frame.hand(m_idLeftHand).fingers();
//        }
//        else
//        {
//            return false;
//        }
//    }
//    else
//    {
//        if(m_idRightHand != -1)
//        {
//            fingerList = m_frame.hand(m_idRightHand).fingers();
//        }
//        else
//        {
//            return false;
//        }
//    }

//    int numFinger = numFingerType(fingerList, fingerType);

//    Leap::Finger finger = fingerList[numFinger];

//    if(!finger.isValid())
//    {
//        return false;
//    }

//    vDirectionFinger = std::vector<float>(3,0.f);

//    Leap::Vector direction = finger.direction();

//    for(uint ii = 0; ii < vDirectionFinger.size(); ++ii)
//    {
//        vDirectionFinger[ii] = direction[ii];
//    }

//    return true;
//}



SWLeap::SWLeap()
{
    std::vector<float> l_vInitVector(3,0.f);
    m_vLeftHandRotation = m_vLeftCoordPalmHand = m_vLeftNormalPalmHand = l_vInitVector;
    m_vLeftThumbRotation =  m_vLeftIndexRotation = m_vLeftMiddleRotation = m_vLeftRingRotation =  m_vLeftPinkyRotation = l_vInitVector;

    m_vRightHandRotation = m_vRightCoordPalmHand = m_vRightNormalPalmHand = l_vInitVector;
    m_vRightThumbRotation =  m_vRightIndexRotation = m_vRightMiddleRotation = m_vRightRingRotation =  m_vRightPinkyRotation = l_vInitVector;
}

bool SWLeap::init()
{
    m_leapController.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);

    int l_i32TimeOut = 0;

    while(!m_leapController.isConnected())
    {
        if(l_i32TimeOut > 10)
        {
            std::cerr << "No leap device detected, initialization aborted. " << std::endl;
            return false;
        }

        ++l_i32TimeOut;

        std::cout << "Waiting for leap device..." << std::endl;
        boost::this_thread::sleep( boost::posix_time::seconds(1) );
    }

    return true;
}


std::vector<float> SWLeap::computeEulerYPRAngles(const Leap::Bone &bone, cbool isMetacarp) const
{
    Leap::Matrix boneBasis = bone.basis();
    Leap::Vector xBasis = boneBasis.xBasis;
    Leap::Vector yBasis = boneBasis.yBasis;
    Leap::Vector zBasis = boneBasis.zBasis;

    float l_matrix[3][3];

    for(int ii = 0;ii<3;ii++)
    {
        for(int jj = 0;jj<3;jj++)
        {
            if(ii==0) l_matrix[ii][jj]=xBasis[jj];
            if(ii==1) l_matrix[ii][jj]=yBasis[jj];
            if(ii==2) l_matrix[ii][jj]=zBasis[jj];
        }
    }

    float yaw,pitch,roll;
    int rowFlag = static_cast<int>(l_matrix[2][0]);

    switch (rowFlag)
    {
        case 1:
            roll    = std::atan2f(-l_matrix[0][1], l_matrix[1][1]) * 180.f / PI;
            yaw     = -90.f;
            pitch   = 0.f;
        break;
        case -1:
            roll    = std::atan2f(-l_matrix[0][1], l_matrix[1][1]) * 180.f / PI;
            yaw     = 90.f;
            pitch   = 0.f;
        break;
        default:
            roll    = std::atan2f(l_matrix[1][0], l_matrix[0][0]) * 180.f / PI;
            yaw     = std::asinf(-l_matrix[2][0]) * 180.f / PI;
            pitch   = std::atan2f(l_matrix[2][1], l_matrix[2][2]) * 180.f / PI;
        break;
    }

    //For the Roll, the matrice gives us value from [X<0,X>-180][X<180,X>0]
    //that's why we do this. The issue come probably from the matrice.
    if(roll < 0.f)
    {
        roll = -180.f - roll;
    }
    else
    {
        roll = 180.f - roll;
    }

    std::vector<float> l_rotation;
    l_rotation = std::vector<float>(3, 0.f);
    l_rotation[0] = pitch;
    l_rotation[1] = yaw;
    l_rotation[2] = roll;

    if(isMetacarp)
    {
        return l_rotation;
    }


    float l_matrixTempAlpha[3][3];
    float l_matrixTempBeta[3][3];
    float l_matrixTempGamma[3][3];

    float l_matrixRotation[3][3];
    float l_matrixFinal[3][3];

    //Matrice INIT
    for(int ii = 0; ii < 3; ii++)
    {
        for(int jj = 0; jj < 3; jj++)
        {
            l_matrixTempAlpha[ii][jj] = 0.f;
            l_matrixTempBeta[ii][jj]  = 0.f;
            l_matrixTempGamma[ii][jj] = 0.f;
            l_matrixRotation[ii][jj]  = 0.f;
            l_matrixFinal[ii][jj]     = 0.f;
        }
    }

    //MATRIX ROTATION ABOUT X
    l_matrixTempAlpha[0][0] = 1.f;
    l_matrixTempAlpha[1][1] = std::cos(PI * pitch / 180.f);
    l_matrixTempAlpha[2][2] = std::cos(PI * pitch / 180.f);
    l_matrixTempAlpha[2][1] = std::sin(PI * pitch / 180.f);
    l_matrixTempAlpha[1][2] = - std::sin(PI * pitch / 180.f);

    //MATRIX ROTATION ABOUT Y
    l_matrixTempBeta[0][0] = std::cos(PI * yaw / 180.f);
    l_matrixTempBeta[1][1] = 1.f;
    l_matrixTempBeta[2][2] = std::cos(PI * yaw / 180.f);
    l_matrixTempBeta[0][2] = std::sin(PI * yaw / 180.f);
    l_matrixTempBeta[2][0] = - std::sin(PI * yaw / 180.f);

    //MATRIX ROTATION ABOUT Z
    l_matrixTempGamma[0][0] = std::cos(PI * roll / 180.f);
    l_matrixTempGamma[1][1] = std::cos(PI * roll / 180.f);
    l_matrixTempGamma[2][2] = 1.f;
    l_matrixTempGamma[1][0] = std::sin(PI * roll / 180.f);
    l_matrixTempGamma[0][1] = - std::sin(PI * roll / 180.f);


    //A * B = TEMP
    for(int ii = 0; ii < 3; ii++)
    {
        for(int jj = 0; jj < 3; jj++)
        {
            l_matrixRotation[ii][jj] = l_matrixTempAlpha[ii][0]*l_matrixTempBeta[0][jj] +
                                       l_matrixTempAlpha[ii][1]*l_matrixTempBeta[1][jj] +
                                       l_matrixTempAlpha[ii][2]*l_matrixTempBeta[2][jj];
        }
    }

    //TEMP * C = FINAL
    for(int ii = 0; ii < 3; ii++)
    {
        for(int jj = 0; jj < 3; jj++)
        {
            l_matrixFinal[ii][jj] = l_matrixRotation[ii][0]*l_matrixTempGamma[0][jj] +
                                    l_matrixRotation[ii][1]*l_matrixTempGamma[1][jj] +
                                    l_matrixRotation[ii][2]*l_matrixTempGamma[2][jj];
        }
    }

    float oldAngles[3];
    float newAngles[3];
    oldAngles[0]=roll;
    oldAngles[1]=pitch;
    oldAngles[2]=yaw;

    for(int ii = 0; ii < 3; ii++)
    {
        newAngles[ii]= l_matrixFinal[ii][0] * oldAngles[0] +  l_matrixFinal[ii][1] * oldAngles[1] +  l_matrixFinal[ii][2] * oldAngles[2];
    }

    l_rotation[0] = newAngles[0];
    l_rotation[1] = newAngles[1];
    l_rotation[2] = newAngles[2];

    return l_rotation;
}

void SWLeap::retrieveFingerRotation(const Leap::Finger &finger, std::vector<float> &rotation) const
{
//        Leap::Bone l_MetacarpalT    = finger.bone(static_cast<Leap::Bone::Type>(0));
    Leap::Bone l_ProximalT      = finger.bone(static_cast<Leap::Bone::Type>(1));
    Leap::Bone l_IntermediateT  = finger.bone(static_cast<Leap::Bone::Type>(2));
    Leap::Bone l_DistalT        = finger.bone(static_cast<Leap::Bone::Type>(3));

//        std::vector<float> l_metacarpalRotation     = computeEulerYPRAngles(l_MetacarpalT,  true);
    std::vector<float> l_proximalRotation       = computeEulerYPRAngles(l_ProximalT,    false);
    std::vector<float> l_intermediateRotation   = computeEulerYPRAngles(l_IntermediateT,false);
    std::vector<float> l_distalRotation         = computeEulerYPRAngles(l_DistalT,      false);

    switch(finger.type())
    {
        case Leap::Finger::TYPE_THUMB :
            rotation[0] =  l_proximalRotation[1];
            rotation[1] = (l_intermediateRotation[1] - l_proximalRotation[1]) + (l_distalRotation[1] - l_intermediateRotation[1]);
        break;
        case Leap::Finger::TYPE_PINKY :
            rotation[0] =  l_proximalRotation[1];
            rotation[1] = (l_intermediateRotation[1] - l_proximalRotation[1]) + (l_distalRotation[1] - l_intermediateRotation[1]);

            if(rotation[1] < 0.f)
            {
                rotation[1] = -rotation[1];
            }
        break;
        case Leap::Finger::TYPE_INDEX :
            rotation[0] = l_proximalRotation[0] + l_proximalRotation[1] + l_proximalRotation[2];
            rotation[1] = (l_intermediateRotation[1] - l_proximalRotation[1]) + (l_distalRotation[1] - l_intermediateRotation[1]);
            if(rotation[1] < 0.f)
            {
                rotation[1] = -rotation[1];
            }
        break;
        case Leap::Finger::TYPE_MIDDLE :
            rotation[0] = l_proximalRotation[0] + l_proximalRotation[1] + l_proximalRotation[2];
            rotation[1] = (l_intermediateRotation[1] - l_proximalRotation[1]) + (l_distalRotation[1] - l_intermediateRotation[1]);

            if(rotation[1] < 0.f)
            {
                rotation[1] = -rotation[1];
            }
        break;
        case Leap::Finger::TYPE_RING :
            rotation[0] = l_proximalRotation[1];
            rotation[1] = (l_intermediateRotation[1] - l_proximalRotation[1]) + (l_distalRotation[1] - l_intermediateRotation[1]);

            if(rotation[1] < 0.f)
            {
                rotation[1] = -rotation[1];
            }
        break;
    }
}


int SWLeap::numFingerType(const Leap::FingerList &fingerList, const Leap::Finger::Type fingerType) const
{
    for(int ii = 0; ii < fingerList.count(); ++ii)
    {
        if(fingerList[ii].type() == fingerType)
        {
            return ii;
        }
    }

    return 0;
}

void SWLeap::normalPalmHand(cbool leftHand, std::vector<float> &vNormalPalmHand) const
{
    if(leftHand)
    {
        vNormalPalmHand = m_vLeftNormalPalmHand;
    }
    else
    {
        vNormalPalmHand = m_vRightNormalPalmHand;
    }
}

void SWLeap::coordPalmHand(cbool leftHand, std::vector<float> &vCoordPalmHand) const
{
    if(leftHand)
    {
        vCoordPalmHand = m_vLeftCoordPalmHand;
    }
    else
    {
        vCoordPalmHand = m_vRightCoordPalmHand;
    }
}


void SWLeap::handRotation(cbool leftHand, std::vector<float> &vHandRotation) const
{
    if(leftHand)
    {
        vHandRotation = m_vLeftHandRotation;
    }
    else
    {
        vHandRotation = m_vRightHandRotation;
    }
}

void SWLeap::fingerRotation(cbool leftHand, const Leap::Finger::Type fingerType, std::vector<float> &fingerRotation)
{
    fingerRotation = std::vector<float>(2, 0.f);

    if(leftHand)
    {
        switch(fingerType)
        {
            case Leap::Finger::TYPE_THUMB :
                fingerRotation = m_vLeftThumbRotation;
            break;
            case Leap::Finger::TYPE_INDEX :
                fingerRotation = m_vLeftIndexRotation;
            break;
            case Leap::Finger::TYPE_MIDDLE :
                fingerRotation = m_vLeftMiddleRotation;
            break;
            case Leap::Finger::TYPE_RING :
                fingerRotation = m_vLeftRingRotation;
            break;
            case Leap::Finger::TYPE_PINKY :
                fingerRotation = m_vLeftPinkyRotation;
            break;
        }
    }
    else
    {
        switch(fingerType)
        {
            case Leap::Finger::TYPE_THUMB :
                fingerRotation = m_vRightThumbRotation;
            break;
            case Leap::Finger::TYPE_INDEX :
                fingerRotation = m_vRightIndexRotation;
            break;
            case Leap::Finger::TYPE_MIDDLE :
                fingerRotation = m_vRightMiddleRotation;
            break;
            case Leap::Finger::TYPE_RING :
                fingerRotation = m_vRightRingRotation;
            break;
            case Leap::Finger::TYPE_PINKY :
                fingerRotation = m_vRightPinkyRotation;
            break;
        }
    }
}

int SWLeap::grab()
{
    Leap::Frame l_frame = m_leapController.frame(0);
    Leap::HandList handList = l_frame.hands();

    if(handList.count() == 0)
    {
        // no hands detected
        return -1;
    }

    int currentIdLeftHand  = -1;
    int currentIdRightHand = -1;

    if(handList[0].isLeft())
    {
        currentIdLeftHand = handList[0].id();

        if(handList.count() == 2)
        {
            currentIdRightHand = handList[1].id();
        }
        else
        {
            currentIdRightHand = -1;
        }
    }
    else
    {
        currentIdRightHand = handList[0].id();

        if(handList.count() == 2)
        {
            currentIdLeftHand = handList[1].id();
        }
        else
        {
            currentIdLeftHand = -1;
        }
    }

    // update left hand
    if(currentIdLeftHand != -1)
    {
        Leap::Hand l_leftHand = l_frame.hand(currentIdLeftHand);

        if(l_leftHand.isValid())
        {
            Leap::Vector l_palmCoord  = l_leftHand.palmPosition();
            m_vLeftCoordPalmHand = std::vector<float>(3,0.f);

            for(uint ii = 0; ii < m_vLeftCoordPalmHand.size(); ++ii)
            {
                m_vLeftCoordPalmHand[ii] = l_palmCoord[ii];
            }

            Leap::Vector l_palmNormal = l_leftHand.palmNormal();
            m_vLeftNormalPalmHand = std::vector<float>(3,0.f);

            for(uint ii = 0; ii < m_vLeftNormalPalmHand.size(); ++ii)
            {
                m_vLeftNormalPalmHand[ii] = l_palmNormal[ii];
            }

            Leap::Vector l_direction = l_leftHand.direction();
            m_vLeftHandRotation = std::vector<float>(3,0.f);
            m_vLeftHandRotation[0] = l_direction.pitch();
            m_vLeftHandRotation[1] = l_palmNormal.roll();
            m_vLeftHandRotation[2] = l_direction.yaw();

            // left fingers
            Leap::FingerList l_fingerList = l_leftHand.fingers();
            Leap::Finger l_thumb  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_THUMB)];
            Leap::Finger l_index  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_INDEX)];
            Leap::Finger l_middle = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_MIDDLE)];
            Leap::Finger l_ring   = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_RING)];
            Leap::Finger l_pinky  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_PINKY)];

            if(l_thumb.isValid())
            {
                retrieveFingerRotation(l_thumb,     m_vLeftIndexRotation);
            }
            if(l_index.isValid())
            {
                retrieveFingerRotation(l_index,     m_vLeftIndexRotation);
            }
            if(l_middle.isValid())
            {
                retrieveFingerRotation(l_middle,    m_vLeftMiddleRotation);
            }
            if(l_ring.isValid())
            {
                retrieveFingerRotation(l_ring,      m_vLeftRingRotation);
            }
            if(l_pinky.isValid())
            {
                retrieveFingerRotation(l_pinky,     m_vLeftPinkyRotation);
            }
        }
    }

    // update right hand
    if(currentIdRightHand != -1)
    {
        Leap::Hand l_rightHand = l_frame.hand(currentIdRightHand);

        if(l_rightHand.isValid())
        {
            Leap::Vector l_palmCoord  = l_rightHand.palmPosition();
            m_vRightCoordPalmHand = std::vector<float>(3,0.f);

            for(uint ii = 0; ii < m_vRightCoordPalmHand.size(); ++ii)
            {
                m_vRightCoordPalmHand[ii] = l_palmCoord[ii];
            }

            Leap::Vector l_palmNormal = l_rightHand.palmNormal();
            m_vRightNormalPalmHand = std::vector<float>(3,0.f);

            for(uint ii = 0; ii < m_vLeftNormalPalmHand.size(); ++ii)
            {
                m_vLeftNormalPalmHand[ii] = l_palmNormal[ii];
            }

            Leap::Vector l_direction = l_rightHand.direction();
            m_vRightHandRotation = std::vector<float>(3,0.f);
            m_vRightHandRotation[0] = l_direction.pitch();
            m_vRightHandRotation[1] = l_palmNormal.roll();
            m_vRightHandRotation[2] = l_direction.yaw();

            // left fingers
            Leap::FingerList l_fingerList = l_rightHand.fingers();
            Leap::Finger l_thumb  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_THUMB)];
            Leap::Finger l_index  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_INDEX)];
            Leap::Finger l_middle = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_MIDDLE)];
            Leap::Finger l_ring   = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_RING)];
            Leap::Finger l_pinky  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_PINKY)];

            if(l_thumb.isValid())
            {
                retrieveFingerRotation(l_thumb,     m_vRightThumbRotation);
            }
            if(l_index.isValid())
            {
                retrieveFingerRotation(l_index,     m_vRightIndexRotation);
            }
            if(l_middle.isValid())
            {
                retrieveFingerRotation(l_middle,    m_vRightMiddleRotation);
            }
            if(l_ring.isValid())
            {
                retrieveFingerRotation(l_ring,      m_vRightRingRotation);
            }
            if(l_pinky.isValid())
            {
                retrieveFingerRotation(l_pinky,     m_vRightPinkyRotation);
            }
        }
    }

    m_fps = static_cast<int>(l_frame.currentFramesPerSecond());

    return 0;
}

int SWLeap::fps() const
{
    return m_fps;
}

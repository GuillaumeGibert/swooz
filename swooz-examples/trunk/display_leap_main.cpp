


/**
 * \file display_leap_main.cpp
 * \author Florian Lance
 * \date 30-06-2014
 * \brief An example program for displaying kinect data using SWoOZ platform.
 */

#include <iostream>

#include "devices/leap/SWLeap.h"


//#include "commonTypes.h"
//#include "leap.h"
//#include "LeapMath.h"

//#include <boost/thread.hpp>

//#define PI 3.14159265f


//using namespace Leap;

//class SWLeap
//{

//    public :

//        SWLeap() : m_idLeftHand(-1), m_idRightHand(-1) {}

//        bool init()
//        {
//            m_leapController.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);

//            int l_i32TimeOut = 0;

//            while(!m_leapController.isConnected())
//            {
//                if(l_i32TimeOut > 10)
//                {
//                    std::cerr << "No leap device detected, initialization aborted. " << std::endl;
//                    return false;
//                }

//                ++l_i32TimeOut;

//                std::cout << "Waiting for leap device..." << std::endl;
//                boost::this_thread::sleep( boost::posix_time::seconds(1) );
//            }

//            return true;
//        }


//        bool handRotation(cbool leftHand, std::vector<float> vHandRotation) const
//        {
//            Leap::Hand hand;

//            if(leftHand)
//            {
//                if(m_idLeftHand != -1)
//                {
//                    hand = m_frame.hand(m_idLeftHand);

//                    if(!hand.isValid())
//                    {
//                        return false;
//                    }
//                }
//                else
//                {
//                    return false;
//                }
//            }
//            else
//            {
//                if(m_idRightHand != -1)
//                {
//                    hand = m_frame.hand(m_idRightHand);

//                    if(!hand.isValid())
//                    {
//                        return false;
//                    }
//                }
//                else
//                {
//                    return false;
//                }
//            }

//            Leap::Vector l_vNormal    = hand.palmNormal();
//            Leap::Vector l_vDirection = hand.direction();

//            vHandRotation = std::vector<float>(3,0.f);
//            vHandRotation[0] = l_vDirection.pitch();
//            vHandRotation[1] = l_vNormal.roll();
//            vHandRotation[2] = l_vDirection.yaw();
//        }

//        std::vector<float> computeEulerYPRAngles(const Leap::Bone &bone, cbool isMetacarp) const
//        {
//            Matrix boneBasis = bone.basis();
//            Vector xBasis = boneBasis.xBasis;
//            Vector yBasis = boneBasis.yBasis;
//            Vector zBasis = boneBasis.zBasis;

//            float l_matrix[3][3];

//            for(int ii = 0;ii<3;ii++)
//            {
//                for(int jj = 0;jj<3;jj++)
//                {
//                    if(ii==0) l_matrix[ii][jj]=xBasis[jj];
//                    if(ii==1) l_matrix[ii][jj]=yBasis[jj];
//                    if(ii==2) l_matrix[ii][jj]=zBasis[jj];
//                }
//            }

//            float yaw,pitch,roll;
//            int rowFlag = static_cast<int>(l_matrix[2][0]);

//            switch (rowFlag)
//            {
//                case 1:
//                    roll    = std::atan2f(-l_matrix[0][1], l_matrix[1][1]) * 180.f / PI;
//                    yaw     = -90.f;
//                    pitch   = 0.f;
//                break;
//                case -1:
//                    roll    = std::atan2f(-l_matrix[0][1], l_matrix[1][1]) * 180.f / PI;
//                    yaw     = 90.f;
//                    pitch   = 0.f;
//                break;
//                default:
//                    roll    = std::atan2f(l_matrix[1][0], l_matrix[0][0]) * 180.f / PI;
//                    yaw     = std::asinf(-l_matrix[2][0]) * 180.f / PI;
//                    pitch   = std::atan2f(l_matrix[2][1], l_matrix[2][2]) * 180.f / PI;
//                break;
//            }

//            //For the Roll, the matrice gives us value from [X<0,X>-180][X<180,X>0]
//            //that's why we do this. The issue come probably from the matrice.
//            if(roll < 0.f)
//            {
//                roll = -180.f - roll;
//            }
//            else
//            {
//                roll = 180.f - roll;
//            }

//            std::vector<float> l_rotation;
//            l_rotation = std::vector<float>(3, 0.f);
//            l_rotation[0] = pitch;
//            l_rotation[1] = yaw;
//            l_rotation[2] = roll;

//            if(isMetacarp)
//            {
//                return l_rotation;
//            }


//            float l_matrixTempAlpha[3][3];
//            float l_matrixTempBeta[3][3];
//            float l_matrixTempGamma[3][3];

//            float l_matrixRotation[3][3];
//            float l_matrixFinal[3][3];

//            //Matrice INIT
//            for(int ii = 0; ii < 3; ii++)
//            {
//                for(int jj = 0; jj < 3; jj++)
//                {
//                    l_matrixTempAlpha[ii][jj] = 0.f;
//                    l_matrixTempBeta[ii][jj]  = 0.f;
//                    l_matrixTempGamma[ii][jj] = 0.f;
//                    l_matrixRotation[ii][jj]  = 0.f;
//                    l_matrixFinal[ii][jj]     = 0.f;
//                }
//            }

//            //MATRIX ROTATION ABOUT X
//            l_matrixTempAlpha[0][0] = 1.f;
//            l_matrixTempAlpha[1][1] = std::cos(PI * pitch / 180.f);
//            l_matrixTempAlpha[2][2] = std::cos(PI * pitch / 180.f);
//            l_matrixTempAlpha[2][1] = std::sin(PI * pitch / 180.f);
//            l_matrixTempAlpha[1][2] = - std::sin(PI * pitch / 180.f);

//            //MATRIX ROTATION ABOUT Y
//            l_matrixTempBeta[0][0] = std::cos(PI * yaw / 180.f);
//            l_matrixTempBeta[1][1] = 1.f;
//            l_matrixTempBeta[2][2] = std::cos(PI * yaw / 180.f);
//            l_matrixTempBeta[0][2] = std::sin(PI * yaw / 180.f);
//            l_matrixTempBeta[2][0] = - std::sin(PI * yaw / 180.f);

//            //MATRIX ROTATION ABOUT Z
//            l_matrixTempGamma[0][0] = std::cos(PI * roll / 180.f);
//            l_matrixTempGamma[1][1] = std::cos(PI * roll / 180.f);
//            l_matrixTempGamma[2][2] = 1.f;
//            l_matrixTempGamma[1][0] = std::sin(PI * roll / 180.f);
//            l_matrixTempGamma[0][1] = - std::sin(PI * roll / 180.f);


//            //A * B = TEMP
//            for(int ii = 0; ii < 3; ii++)
//            {
//                for(int jj = 0; jj < 3; jj++)
//                {
//                    l_matrixRotation[ii][jj] = l_matrixTempAlpha[ii][0]*l_matrixTempBeta[0][jj] +
//                                               l_matrixTempAlpha[ii][1]*l_matrixTempBeta[1][jj] +
//                                               l_matrixTempAlpha[ii][2]*l_matrixTempBeta[2][jj];
//                }
//            }

//            //TEMP * C = FINAL
//            for(int ii = 0; ii < 3; ii++)
//            {
//                for(int jj = 0; jj < 3; jj++)
//                {
//                    l_matrixFinal[ii][jj] = l_matrixRotation[ii][0]*l_matrixTempGamma[0][jj] +
//                                            l_matrixRotation[ii][1]*l_matrixTempGamma[1][jj] +
//                                            l_matrixRotation[ii][2]*l_matrixTempGamma[2][jj];
//                }
//            }

//            float oldAngles[3];
//            float newAngles[3];
//            oldAngles[0]=roll;
//            oldAngles[1]=pitch;
//            oldAngles[2]=yaw;

//            for(int ii = 0; ii < 3; ii++)
//            {
//                newAngles[ii]= l_matrixFinal[ii][0] * oldAngles[0] +  l_matrixFinal[ii][1] * oldAngles[1] +  l_matrixFinal[ii][2] * oldAngles[2];
//            }

//            l_rotation[0] = newAngles[0];
//            l_rotation[1] = newAngles[1];
//            l_rotation[2] = newAngles[2];

//            return l_rotation;
//        }

//        bool fingerRotation(cbool leftHand, const Leap::Finger::Type fingerType, std::vector<float> &rotation) const
//        {
//            Leap::FingerList fingerList;
//            rotation = std::vector<float>(2,0.f);

//            if(leftHand)
//            {
//                if(m_idLeftHand != -1)
//                {
//                    fingerList = m_frame.hand(m_idLeftHand).fingers();
//                }
//                else
//                {
//                    return false;
//                }
//            }
//            else
//            {
//                if(m_idRightHand != -1)
//                {
//                    fingerList = m_frame.hand(m_idRightHand).fingers();
//                }
//                else
//                {
//                    return false;
//                }
//            }

//            int numFinger = numFingerType(fingerList, fingerType);

//            Leap::Finger finger = fingerList[numFinger];

//    //        Leap::Bone l_MetacarpalT    = finger.bone(static_cast<Leap::Bone::Type>(0));
//            Leap::Bone l_ProximalT      = finger.bone(static_cast<Leap::Bone::Type>(1));
//            Leap::Bone l_IntermediateT  = finger.bone(static_cast<Leap::Bone::Type>(2));
//            Leap::Bone l_DistalT        = finger.bone(static_cast<Leap::Bone::Type>(3));

//    //        std::vector<float> l_metacarpalRotation     = computeEulerYPRAngles(l_MetacarpalT,  true);
//            std::vector<float> l_proximalRotation       = computeEulerYPRAngles(l_ProximalT,    false);
//            std::vector<float> l_intermediateRotation   = computeEulerYPRAngles(l_IntermediateT,false);
//            std::vector<float> l_distalRotation         = computeEulerYPRAngles(l_DistalT,      false);

//            switch(finger.type())
//            {
//                case Finger::TYPE_THUMB :
//                    rotation[0] =  l_proximalRotation[1];
//                    rotation[1] = (l_intermediateRotation[1] - l_proximalRotation[1]) + (l_distalRotation[1] - l_intermediateRotation[1]);
//                break;
//                case Finger::TYPE_PINKY :
//                    rotation[0] =  l_proximalRotation[1];
//                    rotation[1] = (l_intermediateRotation[1] - l_proximalRotation[1]) + (l_distalRotation[1] - l_intermediateRotation[1]);

//                    if(rotation[1] < 0.f)
//                    {
//                        rotation[1] = -rotation[1];
//                    }
//                break;
//                case Finger::TYPE_INDEX :
//                    rotation[0] = l_proximalRotation[0] + l_proximalRotation[1] + l_proximalRotation[2];
//                    rotation[1] = (l_intermediateRotation[1] - l_proximalRotation[1]) + (l_distalRotation[1] - l_intermediateRotation[1]);
//                    if(rotation[1] < 0.f)
//                    {
//                        rotation[1] = -rotation[1];
//                    }
//                break;
//                case Finger::TYPE_MIDDLE :
//                    rotation[0] = l_proximalRotation[0] + l_proximalRotation[1] + l_proximalRotation[2];
//                    rotation[1] = (l_intermediateRotation[1] - l_proximalRotation[1]) + (l_distalRotation[1] - l_intermediateRotation[1]);

//                    if(rotation[1] < 0.f)
//                    {
//                        rotation[1] = -rotation[1];
//                    }
//                break;
//                case Finger::TYPE_RING :
//                    rotation[0] = l_proximalRotation[1];
//                    rotation[1] = (l_intermediateRotation[1] - l_proximalRotation[1]) + (l_distalRotation[1] - l_intermediateRotation[1]);

//                    if(rotation[1] < 0.f)
//                    {
//                        rotation[1] = -rotation[1];
//                    }
//                break;
//            }

//            return true;
//        }


//        int numFingerType(const Leap::FingerList &fingerList, const Leap::Finger::Type fingerType) const
//        {
//            for(int ii = 0; ii < fingerList.count(); ++ii)
//            {
//                if(fingerList[ii].type() == fingerType)
//                {
//                    return ii;
//                }
//            }

//            return 0;
//        }


//        bool directionFinger(cbool leftHand, const Leap::Finger::Type fingerType, std::vector<float> &vDirectionFinger)
//        {
//            Leap::FingerList fingerList;

//            if(leftHand)
//            {
//                if(m_idLeftHand != -1)
//                {
//                    fingerList = m_frame.hand(m_idLeftHand).fingers();
//                }
//                else
//                {
//                    return false;
//                }
//            }
//            else
//            {
//                if(m_idRightHand != -1)
//                {
//                    fingerList = m_frame.hand(m_idRightHand).fingers();
//                }
//                else
//                {
//                    return false;
//                }
//            }

//            int numFinger = numFingerType(fingerList, fingerType);

//            Leap::Finger finger = fingerList[numFinger];

//            if(!finger.isValid())
//            {
//                return false;
//            }

//            vDirectionFinger = std::vector<float>(3,0.f);

//            Leap::Vector direction = finger.direction();

//            for(uint ii = 0; ii < vDirectionFinger.size(); ++ii)
//            {
//                vDirectionFinger[ii] = direction[ii];
//            }

//            return true;
//        }

//        bool normalPalmHand(cbool leftHand, std::vector<float> &vNormalPalmHand) const
//        {
//            Leap::Hand hand;

//            if(leftHand)
//            {
//                if(m_idLeftHand != -1)
//                {
//                    hand = m_frame.hand(m_idLeftHand);

//                    if(!hand.isValid())
//                    {
//                        return false;
//                    }
//                }
//                else
//                {
//                    return false;
//                }
//            }
//            else
//            {
//                if(m_idRightHand != -1)
//                {
//                    hand = m_frame.hand(m_idRightHand);

//                    if(!hand.isValid())
//                    {
//                        return false;
//                    }
//                }
//                else
//                {
//                    return false;
//                }
//            }

//            Leap::Vector palmNormal = hand.palmNormal();
//            vNormalPalmHand = std::vector<float>(3,0.f);

//            for(uint ii = 0; ii < vNormalPalmHand.size(); ++ii)
//            {
//                vNormalPalmHand[ii] = palmNormal[ii];
//            }

//            return true;
//        }



//        int grab()
//        {
//            m_frame = m_leapController.frame(0);

//            Leap::HandList handList = m_frame.hands();

//            if(handList.count() == 0)
//            {
//                // no hands detected
//                return -1;
//            }

//            int currentIdLeftHand  = -1;
//            int currentIdRightHand = -1;

//            if(handList[0].isLeft())
//            {
//                currentIdLeftHand = handList[0].id();

//                if(handList.count() == 2)
//                {
//                    currentIdRightHand = handList[1].id();
//                }
//                else
//                {
//                    currentIdRightHand = -1;
//                }
//            }
//            else
//            {
//                currentIdRightHand = handList[0].id();

//                if(handList.count() == 2)
//                {
//                    currentIdLeftHand = handList[1].id();
//                }
//                else
//                {
//                    currentIdLeftHand = -1;
//                }
//            }

//            m_idLeftHand = currentIdLeftHand;
//            m_idRightHand = currentIdRightHand;


//    //        std::vector<float> rotation;

//    //        if(fingerRotation(true, Leap::Finger::TYPE_THUMB, rotation))
//    //        {
//    //            std::cout << "THUMB LEFT : " << rotation[0] << " " << rotation[1] << std::endl;
//    //        }
//    //        if(fingerRotation(true, Leap::Finger::TYPE_INDEX, rotation))
//    //        {
//    //            std::cout << "INDEX LEFT : " << rotation[0] << " " << rotation[1] << std::endl;
//    //        }
//    //        if(fingerRotation(true, Leap::Finger::TYPE_MIDDLE, rotation))
//    //        {
//    //            std::cout << "MIDDLE LEFT : " << rotation[0] << " " << rotation[1] << std::endl;
//    //        }
//    //        if(fingerRotation(true, Leap::Finger::TYPE_RING, rotation))
//    //        {
//    //            std::cout << "RING LEFT : " << rotation[0] << " " << rotation[1] << std::endl;
//    //        }
//    //        if(fingerRotation(true, Leap::Finger::TYPE_PINKY, rotation))
//    //        {
//    //            std::cout << "PINKY LEFT : " << rotation[0] << " " << rotation[1] << std::endl;
//    //        }

//    //        std::cout << std::endl;

//            return 0;
//        }

//    private :

//        Leap::Controller m_leapController;

//        int m_idLeftHand;
//        int m_idRightHand;

//        Leap::Frame m_frame;
//};


using namespace Leap;

int main()
{
    Leap::Controller leapController;
    leapController.setPolicyFlags(Controller::POLICY_BACKGROUND_FRAMES);

    int l_i32TimeOut = 0;

    while(!leapController.isConnected())
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

    if(!leapController.isConnected())
    {
        return -1;
    }


    swDevice::SWLeap test;
    test.init();

    // set the display loop
    while(true)
    {
        test.grab();

        boost::this_thread::sleep( boost::posix_time::millisec(500) );
    }


    return 0;
}

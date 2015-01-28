/*******************************************************************************
**                                                                            **
**  SWoOz is a software platform written in C++ used for behavioral           **
**  experiments based on interactions between people and robots               **
**  or 3D avatars.                                                            **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
** *****************************************************************************
**          Authors: Guillaume Gibert, Florian Lance                          **
**  Website/Contact: http://swooz.free.fr/                                    **
**       Repository: https://github.com/GuillaumeGibert/swooz                 **
********************************************************************************/

/**
 * \file SWLeap.cpp
 * \brief  SWLeap implements basics Actions/Services that Leap Motion provides
 * \author Gabriel MEYNET / Florian LANCE
 * \date 07/04/14
 */


#include "devices/leap/SWLeap.h"


using namespace swDevice;

SWLeap::SWLeap()
{
    std::vector<float> l_vInitVector(3,0.f);
    std::vector<std::vector<float> > l_vBonesInitVector3(3, l_vInitVector);
    std::vector<std::vector<float> > l_vBonesInitVector4(4, l_vInitVector);

    m_vLeftCoordPalmHand = m_vLeftNormalPalmHand = m_vLeftDirectionArm = m_vLeftDirectionHand = m_vLeftDirectionHandE = m_vLeftNormalPalmHandE = l_vInitVector;
//    m_vLeftThumbRotation =  m_vLeftIndexRotation = m_vLeftMiddleRotation = m_vLeftRingRotation =  m_vLeftPinkyRotation = l_vInitVector;

    m_vRightCoordPalmHand = m_vRightNormalPalmHand = m_vRightDirectionArm = m_vRightDirectionHand = m_vRightDirectionHandE = m_vRightNormalPalmHandE = l_vInitVector;
//    m_vRightThumbRotation =  m_vRightIndexRotation = m_vRightMiddleRotation = m_vRightRingRotation =  m_vRightPinkyRotation = l_vInitVector;

    m_vLeftThumbDirections = m_vRightThumbDirections = l_vBonesInitVector3;
    m_vLeftIndexDirections = m_vLeftMiddleDirections = m_vLeftRingDirections = m_vLeftPinkyDirections = l_vBonesInitVector4;
    m_vRightIndexDirections = m_vRightMiddleDirections = m_vRightRingDirections = m_vRightPinkyDirections = l_vBonesInitVector4;

    m_vLeftThumbPositions = m_vRightThumbPositions = l_vBonesInitVector3;
    m_vLeftIndexPositions = m_vLeftMiddlePositions = m_vLeftRingPositions = m_vLeftPinkyPositions = l_vBonesInitVector4;
    m_vRightIndexPositions = m_vRightMiddlePositions = m_vRightRingPositions = m_vRightPinkyPositions = l_vBonesInitVector4;
}

void SWLeap::toStdVector(std::vector<float> &oSTDVec, Leap::Vector &oVector)
{
    if(oVector.x != 0 && oVector.y != 0 && oVector.z != 0)
    {
        oSTDVec[0] = oVector.x;
        oSTDVec[1] = oVector.y;
        oSTDVec[2] = oVector.z;
    }
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

void SWLeap::boneDirection(const bool bLeftHand, const Leap::Finger::Type fingerType, const Leap::Bone::Type boneType, std::vector<float> &vBoneDirection)
{
    if(bLeftHand)
    {
        switch(fingerType)
        {
            case  Leap::Finger::TYPE_THUMB :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBoneDirection = m_vLeftThumbDirections[0];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBoneDirection = m_vLeftThumbDirections[1];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBoneDirection = m_vLeftThumbDirections[2];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_INDEX :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBoneDirection = m_vLeftIndexDirections[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBoneDirection = m_vLeftIndexDirections[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBoneDirection = m_vLeftIndexDirections[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBoneDirection = m_vLeftIndexDirections[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_MIDDLE :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBoneDirection = m_vLeftMiddleDirections[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBoneDirection = m_vLeftMiddleDirections[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBoneDirection = m_vLeftMiddleDirections[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBoneDirection = m_vLeftMiddleDirections[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_RING :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBoneDirection = m_vLeftRingDirections[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBoneDirection = m_vLeftRingDirections[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBoneDirection = m_vLeftRingDirections[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBoneDirection = m_vLeftRingDirections[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_PINKY :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBoneDirection = m_vLeftPinkyDirections[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBoneDirection = m_vLeftPinkyDirections[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBoneDirection = m_vLeftPinkyDirections[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBoneDirection = m_vLeftPinkyDirections[3];
                    break;
                }
            break;
        }
    }
    else
    {
        switch(fingerType)
        {
            case  Leap::Finger::TYPE_THUMB :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBoneDirection = m_vRightThumbDirections[0];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBoneDirection = m_vRightThumbDirections[1];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBoneDirection = m_vRightThumbDirections[2];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_INDEX :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBoneDirection = m_vRightIndexDirections[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBoneDirection = m_vRightIndexDirections[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBoneDirection = m_vRightIndexDirections[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBoneDirection = m_vRightIndexDirections[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_MIDDLE :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBoneDirection = m_vRightMiddleDirections[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBoneDirection = m_vRightMiddleDirections[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBoneDirection = m_vRightMiddleDirections[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBoneDirection = m_vRightMiddleDirections[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_RING :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBoneDirection = m_vRightRingDirections[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBoneDirection = m_vRightRingDirections[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBoneDirection = m_vRightRingDirections[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBoneDirection = m_vRightRingDirections[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_PINKY :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBoneDirection = m_vRightPinkyDirections[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBoneDirection = m_vRightPinkyDirections[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBoneDirection = m_vRightPinkyDirections[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBoneDirection = m_vRightPinkyDirections[3];
                    break;
                }
            break;
        }
    }
}

void SWLeap::bonePosition(const bool bLeftHand, const Leap::Finger::Type fingerType, const Leap::Bone::Type boneType, std::vector<float> &vBonePosition)
{
    if(bLeftHand)
    {
        switch(fingerType)
        {
            case  Leap::Finger::TYPE_THUMB :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBonePosition = m_vLeftThumbPositions[0];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBonePosition = m_vLeftThumbPositions[1];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBonePosition = m_vLeftThumbPositions[2];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_INDEX :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBonePosition = m_vLeftIndexPositions[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBonePosition = m_vLeftIndexPositions[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBonePosition = m_vLeftIndexPositions[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBonePosition = m_vLeftIndexPositions[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_MIDDLE :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBonePosition = m_vLeftMiddlePositions[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBonePosition = m_vLeftMiddlePositions[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBonePosition = m_vLeftMiddlePositions[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBonePosition = m_vLeftMiddlePositions[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_RING :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBonePosition = m_vLeftRingPositions[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBonePosition = m_vLeftRingPositions[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBonePosition = m_vLeftRingPositions[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBonePosition = m_vLeftRingPositions[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_PINKY :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBonePosition = m_vLeftPinkyPositions[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBonePosition = m_vLeftPinkyPositions[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBonePosition = m_vLeftPinkyPositions[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBonePosition = m_vLeftPinkyPositions[3];
                    break;
                }
            break;
        }
    }
    else
    {
        switch(fingerType)
        {
            case  Leap::Finger::TYPE_THUMB :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBonePosition = m_vRightThumbPositions[0];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBonePosition = m_vRightThumbPositions[1];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBonePosition = m_vRightThumbPositions[2];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_INDEX :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBonePosition = m_vRightIndexPositions[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBonePosition = m_vRightIndexPositions[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBonePosition = m_vRightIndexPositions[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBonePosition = m_vRightIndexPositions[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_MIDDLE :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBonePosition = m_vRightMiddlePositions[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBonePosition = m_vRightMiddlePositions[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBonePosition = m_vRightMiddlePositions[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBonePosition = m_vRightMiddlePositions[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_RING :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBonePosition = m_vRightRingPositions[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBonePosition = m_vRightRingPositions[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBonePosition = m_vRightRingPositions[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBonePosition = m_vRightRingPositions[3];
                    break;
                }
            break;
            case  Leap::Finger::TYPE_PINKY :
                switch(boneType)
                {
                    case Leap::Bone::TYPE_METACARPAL :
                        vBonePosition = m_vRightPinkyPositions[0];
                    break;
                    case Leap::Bone::TYPE_PROXIMAL :
                        vBonePosition = m_vRightPinkyPositions[1];
                    break;
                    case Leap::Bone::TYPE_INTERMEDIATE :
                        vBonePosition = m_vRightPinkyPositions[2];
                    break;
                    case Leap::Bone::TYPE_DISTAL :
                        vBonePosition = m_vRightPinkyPositions[3];
                    break;
                }
            break;
        }
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

void SWLeap::directionArm(cbool leftArm, std::vector<float> &vDirectionArm) const
{
    if(leftArm)
    {
        vDirectionArm = m_vLeftDirectionArm;
    }
    else
    {
        vDirectionArm = m_vRightDirectionArm;
    }
}

void SWLeap::directionHandEuclidian(cbool leftHand, std::vector<float> &vDirectionHandE) const
{
    if(leftHand)
    {
        vDirectionHandE = m_vLeftDirectionHandE;
    }
    else
    {
        vDirectionHandE = m_vRightDirectionHandE;
    }
}

void SWLeap::normalPalmHandEuclidian(cbool leftHand, std::vector<float> &vNormalPalmHandE) const
{
    if(leftHand)
    {
        vNormalPalmHandE = m_vLeftNormalPalmHandE;
    }
    else
    {
        vNormalPalmHandE = m_vRightNormalPalmHandE;
    }
}

void SWLeap::directionHand(cbool leftHand, std::vector<float> &vDirectionHand) const
{
    if(leftHand)
    {
        vDirectionHand = m_vLeftDirectionHand;
    }
    else
    {
        vDirectionHand = m_vRightDirectionHand;
    }
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

        if(l_leftHand.isValid() && l_leftHand.confidence() > 0.3f)
        {
            Leap::Vector l_palmCoord     = l_leftHand.palmPosition();
            Leap::Vector l_palmNormal    = l_leftHand.palmNormal();
            Leap::Vector l_handDirection = l_leftHand.direction();

            Leap::Arm l_leftArm          = l_leftHand.arm();
            Leap::Vector l_armDirection  = l_leftArm.direction();

            std::vector<float> l_vInitVector(3,0.f);
            m_vLeftCoordPalmHand  = l_vInitVector;
            m_vLeftNormalPalmHand = l_vInitVector;
            m_vLeftDirectionHand  = l_vInitVector;
            m_vLeftDirectionArm   = l_vInitVector;
            m_vLeftDirectionHandE = l_vInitVector;
            m_vLeftNormalPalmHandE= l_vInitVector;

            for(uint ii = 0; ii < l_vInitVector.size(); ++ii)
            {
                m_vLeftCoordPalmHand[ii]  = l_palmCoord[ii];
                m_vLeftNormalPalmHand[ii] = l_palmNormal[ii];
                m_vLeftDirectionHand[ii]  = l_handDirection[ii];

                if(l_leftArm.isValid())
                {
                    m_vLeftDirectionArm[ii] = l_armDirection[ii];
                }
            }

            m_vLeftDirectionHandE[0] = l_handDirection.pitch();
            m_vLeftDirectionHandE[1] = l_handDirection.roll();
            m_vLeftDirectionHandE[2] = l_handDirection.yaw();

            m_vLeftNormalPalmHandE[0] = l_palmNormal.pitch();
            m_vLeftNormalPalmHandE[1] = l_palmNormal.roll();
            m_vLeftNormalPalmHandE[2] = l_palmNormal.yaw();

            // left fingers
            Leap::FingerList l_fingerList = l_leftHand.fingers();
            Leap::Finger l_thumb  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_THUMB)];
            Leap::Finger l_index  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_INDEX)];
            Leap::Finger l_middle = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_MIDDLE)];
            Leap::Finger l_ring   = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_RING)];
            Leap::Finger l_pinky  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_PINKY)];

            Leap::Bone l_oBone;

            if(l_thumb.isValid())
            {
                l_oBone = l_thumb.bone(Leap::Bone::TYPE_PROXIMAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftThumbDirections[0], l_oBone.direction());
                    toStdVector(m_vLeftThumbPositions[0], l_oBone.center());
                }
                l_oBone = l_thumb.bone(Leap::Bone::TYPE_INTERMEDIATE);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftThumbDirections[1], l_oBone.direction());
                    toStdVector(m_vLeftThumbPositions[1], l_oBone.center());
                }
                l_oBone = l_thumb.bone(Leap::Bone::TYPE_DISTAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftThumbDirections[2], l_oBone.direction());
                    toStdVector(m_vLeftThumbPositions[2], l_oBone.center());
                }
            }
            if(l_index.isValid())
            {
                l_oBone = l_index.bone(Leap::Bone::TYPE_METACARPAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftIndexDirections[0], l_oBone.direction());
                    toStdVector(m_vLeftIndexPositions[0], l_oBone.center());
                }
                l_oBone = l_index.bone(Leap::Bone::TYPE_PROXIMAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftIndexDirections[1], l_oBone.direction());
                    toStdVector(m_vLeftIndexPositions[1], l_oBone.center());
                }                
                l_oBone = l_index.bone(Leap::Bone::TYPE_INTERMEDIATE);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftIndexDirections[2], l_oBone.direction());
                    toStdVector(m_vLeftIndexPositions[2], l_oBone.center());
                }
                l_oBone = l_index.bone(Leap::Bone::TYPE_DISTAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftIndexDirections[3], l_oBone.direction());
                    toStdVector(m_vLeftIndexPositions[3], l_oBone.center());
                }
            }
            if(l_middle.isValid())
            {
                l_oBone = l_middle.bone(Leap::Bone::TYPE_METACARPAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftMiddleDirections[0], l_oBone.direction());
                    toStdVector(m_vLeftMiddlePositions[0], l_oBone.center());
                }
                l_oBone = l_middle.bone(Leap::Bone::TYPE_PROXIMAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftMiddleDirections[1], l_oBone.direction());
                    toStdVector(m_vLeftMiddlePositions[1], l_oBone.center());
                }
                l_oBone = l_middle.bone(Leap::Bone::TYPE_INTERMEDIATE);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftMiddleDirections[2], l_oBone.direction());
                    toStdVector(m_vLeftMiddlePositions[2], l_oBone.center());
                }
                l_oBone = l_middle.bone(Leap::Bone::TYPE_DISTAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftMiddleDirections[3], l_oBone.direction());
                    toStdVector(m_vLeftMiddlePositions[3], l_oBone.center());
                }
            }
            if(l_ring.isValid())
            {                
                l_oBone = l_ring.bone(Leap::Bone::TYPE_METACARPAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftRingDirections[0], l_oBone.direction());
                    toStdVector(m_vLeftRingPositions[0], l_oBone.center());
                }
                l_oBone = l_ring.bone(Leap::Bone::TYPE_PROXIMAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftRingDirections[1], l_oBone.direction());
                    toStdVector(m_vLeftRingPositions[1], l_oBone.center());
                }
                l_oBone = l_ring.bone(Leap::Bone::TYPE_INTERMEDIATE);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftRingDirections[2], l_oBone.direction());
                    toStdVector(m_vLeftRingPositions[2], l_oBone.center());
                }
                l_oBone = l_ring.bone(Leap::Bone::TYPE_DISTAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftRingDirections[3], l_oBone.direction());
                    toStdVector(m_vLeftRingPositions[3], l_oBone.center());
                }
            }
            if(l_pinky.isValid())
            {
                l_oBone = l_pinky.bone(Leap::Bone::TYPE_METACARPAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftPinkyDirections[0], l_oBone.direction());
                    toStdVector(m_vLeftPinkyPositions[0], l_oBone.center());
                }
                l_oBone = l_pinky.bone(Leap::Bone::TYPE_PROXIMAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftPinkyDirections[1], l_oBone.direction());
                    toStdVector(m_vLeftPinkyPositions[1], l_oBone.center());
                }
                l_oBone = l_pinky.bone(Leap::Bone::TYPE_INTERMEDIATE);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftPinkyDirections[2], l_oBone.direction());
                    toStdVector(m_vLeftPinkyPositions[2], l_oBone.center());
                }
                l_oBone = l_pinky.bone(Leap::Bone::TYPE_DISTAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vLeftPinkyDirections[3], l_oBone.direction());
                    toStdVector(m_vLeftPinkyPositions[3], l_oBone.center());
                }
            }
        }
    }

    // update right hand
    if(currentIdRightHand != -1)
    {
        Leap::Hand l_rightHand = l_frame.hand(currentIdRightHand);

        if(l_rightHand.isValid() && l_rightHand.confidence() > 0.3f)
        {
            Leap::Vector l_palmCoord     = l_rightHand.palmPosition();
            Leap::Vector l_palmNormal    = l_rightHand.palmNormal();
            Leap::Vector l_handDirection = l_rightHand.direction();

            Leap::Arm l_rightArm         = l_rightHand.arm();
            Leap::Vector l_armDirection  = l_rightArm.direction();

            std::vector<float> l_vInitVector(3,0.f);
            m_vRightCoordPalmHand  = l_vInitVector;
            m_vRightNormalPalmHand = l_vInitVector;
            m_vRightDirectionHand  = l_vInitVector;
            m_vRightDirectionArm   = l_vInitVector;
            m_vRightDirectionHandE = l_vInitVector;
            m_vRightNormalPalmHandE= l_vInitVector;

            for(uint ii = 0; ii < l_vInitVector.size(); ++ii)
            {
                m_vRightCoordPalmHand[ii]  = l_palmCoord[ii];
                m_vRightNormalPalmHand[ii] = l_palmNormal[ii];
                m_vRightDirectionHand[ii]  = l_handDirection[ii];

                if(l_rightArm.isValid())
                {
                    m_vRightDirectionArm[ii] = l_armDirection[ii];
                }
            }

            m_vRightDirectionHandE[0] = l_handDirection.pitch();
            m_vRightDirectionHandE[1] = l_handDirection.roll();
            m_vRightDirectionHandE[2] = l_handDirection.yaw();

            m_vRightNormalPalmHandE[0] = l_palmNormal.pitch();
            m_vRightNormalPalmHandE[1] = l_palmNormal.roll();
            m_vRightNormalPalmHandE[2] = l_palmNormal.yaw();

            // left fingers
            Leap::FingerList l_fingerList = l_rightHand.fingers();
            Leap::Finger l_thumb  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_THUMB)];
            Leap::Finger l_index  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_INDEX)];
            Leap::Finger l_middle = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_MIDDLE)];
            Leap::Finger l_ring   = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_RING)];
            Leap::Finger l_pinky  = l_fingerList[numFingerType(l_fingerList, Leap::Finger::TYPE_PINKY)];

            Leap::Bone l_oBone;

            if(l_thumb.isValid())
            {
                l_oBone = l_thumb.bone(Leap::Bone::TYPE_PROXIMAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightThumbDirections[0], l_oBone.direction());
                    toStdVector(m_vRightThumbPositions[0], l_oBone.center());
                }
                l_oBone = l_thumb.bone(Leap::Bone::TYPE_INTERMEDIATE);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightThumbDirections[1], l_oBone.direction());
                    toStdVector(m_vRightThumbPositions[1], l_oBone.center());
                }
                l_oBone = l_thumb.bone(Leap::Bone::TYPE_DISTAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightThumbDirections[2], l_oBone.direction());
                    toStdVector(m_vRightThumbPositions[2], l_oBone.center());
                }
            }
            if(l_index.isValid())
            {
                l_oBone = l_index.bone(Leap::Bone::TYPE_METACARPAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightIndexDirections[0], l_oBone.direction());
                    toStdVector(m_vRightIndexPositions[0], l_oBone.center());
                }
                l_oBone = l_index.bone(Leap::Bone::TYPE_PROXIMAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightIndexDirections[1], l_oBone.direction());
                    toStdVector(m_vRightIndexPositions[1], l_oBone.center());
                }
                l_oBone = l_index.bone(Leap::Bone::TYPE_INTERMEDIATE);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightIndexDirections[2], l_oBone.direction());
                    toStdVector(m_vRightIndexPositions[2], l_oBone.center());
                }
                l_oBone = l_index.bone(Leap::Bone::TYPE_DISTAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightIndexDirections[3], l_oBone.direction());
                    toStdVector(m_vRightIndexPositions[3], l_oBone.center());
                }
            }
            if(l_middle.isValid())
            {
                l_oBone = l_middle.bone(Leap::Bone::TYPE_METACARPAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightMiddleDirections[0], l_oBone.direction());
                    toStdVector(m_vRightMiddlePositions[0], l_oBone.center());
                }
                l_oBone = l_middle.bone(Leap::Bone::TYPE_PROXIMAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightMiddleDirections[1], l_oBone.direction());
                    toStdVector(m_vRightMiddlePositions[1], l_oBone.center());
                }
                l_oBone = l_middle.bone(Leap::Bone::TYPE_INTERMEDIATE);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightMiddleDirections[2], l_oBone.direction());
                    toStdVector(m_vRightMiddlePositions[2], l_oBone.center());
                }
                l_oBone = l_middle.bone(Leap::Bone::TYPE_DISTAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightMiddleDirections[3], l_oBone.direction());
                    toStdVector(m_vRightMiddlePositions[3], l_oBone.center());
                }
            }
            if(l_ring.isValid())
            {
                l_oBone = l_ring.bone(Leap::Bone::TYPE_METACARPAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightRingDirections[0], l_oBone.direction());
                    toStdVector(m_vRightRingPositions[0], l_oBone.center());
                }
                l_oBone = l_ring.bone(Leap::Bone::TYPE_PROXIMAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightRingDirections[1], l_oBone.direction());
                    toStdVector(m_vRightRingPositions[1], l_oBone.center());
                }
                l_oBone = l_ring.bone(Leap::Bone::TYPE_INTERMEDIATE);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightRingDirections[2], l_oBone.direction());
                    toStdVector(m_vRightRingPositions[2], l_oBone.center());
                }
                l_oBone = l_ring.bone(Leap::Bone::TYPE_DISTAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightRingDirections[3], l_oBone.direction());
                    toStdVector(m_vRightRingPositions[3], l_oBone.center());
                }
            }
            if(l_pinky.isValid())
            {
                l_oBone = l_pinky.bone(Leap::Bone::TYPE_METACARPAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightPinkyDirections[0], l_oBone.direction());
                    toStdVector(m_vRightPinkyPositions[0], l_oBone.center());
                }
                l_oBone = l_pinky.bone(Leap::Bone::TYPE_PROXIMAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightPinkyDirections[1], l_oBone.direction());
                    toStdVector(m_vRightPinkyPositions[1], l_oBone.center());
                }
                l_oBone = l_pinky.bone(Leap::Bone::TYPE_INTERMEDIATE);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightPinkyDirections[2], l_oBone.direction());
                    toStdVector(m_vRightPinkyPositions[2], l_oBone.center());
                }
                l_oBone = l_pinky.bone(Leap::Bone::TYPE_DISTAL);
                if(l_oBone.isValid())
                {
                    toStdVector(m_vRightPinkyDirections[3], l_oBone.direction());
                    toStdVector(m_vRightPinkyPositions[3], l_oBone.center());
                }
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




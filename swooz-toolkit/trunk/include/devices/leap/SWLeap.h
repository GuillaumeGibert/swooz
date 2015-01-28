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
 * \file SWLeap.h
 * \brief Defines SWLeap
 * \author Gabriel MEYNET  / Florian LANCE
 * \date 07/04/14
 */

#ifndef _SWLEAP_
#define _SWLEAP_


#include <iostream>


#include "commonTypes.h"
#include "leap.h"
#include "LeapMath.h"

#include <boost/thread.hpp>

//! namespace for devices interfaces
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


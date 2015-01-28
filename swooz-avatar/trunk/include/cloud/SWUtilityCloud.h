
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
 * \file SWUtilityCloud.h
 * \brief Utility geometry functions.
 * \author Florian Lance
 * \date 11/12/12
 */

#ifndef _SWUTILITYCLOUD_
#define _SWUTILITYCLOUD_


#include "opencv2/imgproc/imgproc.hpp"

namespace swCloud
{

    template <typename T>
    static T squareDistancePoint2Triangle(const std::vector<T> &oV0, const std::vector<T> &oV1, const std::vector<T> &oV2, const std::vector<T> &oP)
    {
        return squareDistancePoint2Triangle(cv::Mat(oV0), cv::Mat(oV1), cv::Mat(oV2), cv::Mat(oP));
    }

    /**
     * @brief squareDistancePoint2Triangle
     * @param oV0
     * @param oV1
     * @param oV2
     * @param oP
     * @return
     */
    static float squareDistancePoint2Triangle(const cv::Vec3f &oV0, const cv::Vec3f &oV1, const cv::Vec3f &oV2, const cv::Vec3f &oP)
    {
        cv::Vec3f l_oDiff, l_oEdge0, l_oEdge1;
        l_oDiff  = oV0 - oP;
        l_oEdge0 = oV1 - oV0;
        l_oEdge1 = oV2 - oV0;

        float l_fA00 = (float)cv::norm(l_oEdge0); l_fA00 *= l_fA00;
        float l_fA01 = l_oEdge0.dot(l_oEdge1);
        float l_fA11 = (float)cv::norm(l_oEdge1); l_fA11 *= l_fA11;
        float l_fB0  = l_oDiff.dot(l_oEdge0);
        float l_fB1  = l_oDiff.dot(l_oEdge1);
        float l_fC   = (float)cv::norm(l_oDiff); l_fC *= l_fC;
        float l_fDet = fabs(l_fA00*l_fA11 - l_fA01*l_fA01);

        float l_fS   = l_fA01*l_fB1 - l_fA11*l_fB0;
        float l_fT   = l_fA01*l_fB0 - l_fA00*l_fB1;
        float l_fSqrDistance;

        if (l_fS + l_fT <= l_fDet)
        {
            if (l_fS < 0.f)
            {
                if (l_fT < 0.f)  // region 4
                {
                    if (l_fB0 < 0.f)
                    {
                        l_fT = 0.f;
                        if (-l_fB0 >= l_fA00)
                        {
                            l_fS = 1.f;
                            l_fSqrDistance = l_fA00 + (2.f)*l_fB0 + l_fC;
                        }
                        else
                        {
                            l_fS = -l_fB0/l_fA00;
                            l_fSqrDistance = l_fB0*l_fS + l_fC;
                        }
                    }
                    else
                    {
                        l_fS = 0.f;
                        if (l_fB1 >= 0.f)
                        {
                            l_fT = 0.f;
                            l_fSqrDistance = l_fC;
                        }
                        else if (-l_fB1 >= l_fA11)
                        {
                            l_fT = 1.f;
                            l_fSqrDistance = l_fA11 + (2.f)*l_fB1 + l_fC;
                        }
                        else
                        {
                            l_fT = -l_fB1/l_fA11;
                            l_fSqrDistance = l_fB1*l_fT + l_fC;
                        }
                    }
                }
                else  // region 3
                {
                    l_fS = 0.f;
                    if (l_fB1 >= 0.f)
                    {
                        l_fT = 0.f;
                        l_fSqrDistance = l_fC;
                    }
                    else if (-l_fB1 >= l_fA11)
                    {
                        l_fT = 1.f;
                        l_fSqrDistance = l_fA11 + (2.f)*l_fB1 + l_fC;
                    }
                    else
                    {
                        l_fT = -l_fB1/l_fA11;
                        l_fSqrDistance = l_fB1*l_fT + l_fC;
                    }
                }
            }
            else if (l_fT < 0.f)  // region 5
            {
                l_fT = 0.f;
                if (l_fB0 >= 0.f)
                {
                    l_fS = 0.f;
                    l_fSqrDistance = l_fC;
                }
                else if (-l_fB0 >= l_fA00)
                {
                    l_fS = 1.f;
                    l_fSqrDistance = l_fA00 + (2.f)*l_fB0 + l_fC;
                }
                else
                {
                    l_fS = -l_fB0/l_fA00;
                    l_fSqrDistance = l_fB0*l_fS + l_fC;
                }
            }
            else  // region 0
            {
                // minimum at interior point
                float  l_fInvDet = (1.f)/l_fDet;
                l_fS *= l_fInvDet;
                l_fT *= l_fInvDet;
                l_fSqrDistance = l_fS*(l_fA00*l_fS + l_fA01*l_fT + (2.f)*l_fB0) +
                   l_fT*(l_fA01*l_fS + l_fA11*l_fT + (2.f)*l_fB1) + l_fC;
            }
        }
        else
        {
            float  l_fTmp0, l_fTmp1, l_fNumer, l_fDenom;

            if (l_fS < 0.f)  // region 2
            {
                l_fTmp0 = l_fA01 + l_fB0;
                l_fTmp1 = l_fA11 + l_fB1;
                if (l_fTmp1 > l_fTmp0)
                {
                    l_fNumer = l_fTmp1 - l_fTmp0;
                    l_fDenom = l_fA00 - (2.f)*l_fA01 + l_fA11;
                    if (l_fNumer >= l_fDenom)
                    {
                        l_fS = 1.f;
                        l_fT = 0.f;
                        l_fSqrDistance = l_fA00 + (2.f)*l_fB0 + l_fC;
                    }
                    else
                    {
                        l_fS = l_fNumer/l_fDenom;
                        l_fT = 1.f - l_fS;
                        l_fSqrDistance = l_fS*(l_fA00*l_fS + l_fA01*l_fT + (2.f)*l_fB0) +
                           l_fT*(l_fA01*l_fS + l_fA11*l_fT + (2.f)*l_fB1) + l_fC;
                    }
                }
                else
                {
                   l_fS = 0.f;
                   if (l_fTmp1 <= 0.f)
                   {
                       l_fT = 1.f;
                       l_fSqrDistance = l_fA11 + (2.f)*l_fB1 + l_fC;
                   }
                   else if (l_fB1 >= 0.f)
                   {
                       l_fT = 0.f;
                       l_fSqrDistance = l_fC;
                   }
                   else
                   {
                       l_fT = -l_fB1/l_fA11;
                       l_fSqrDistance = l_fB1*l_fT + l_fC;
                   }
               }
           }
           else if (l_fT < 0.f)  // region 6
            {
               l_fTmp0 = l_fA01 + l_fB1;
                l_fTmp1 = l_fA00 + l_fB0;
                if (l_fTmp1 > l_fTmp0)
                {
                    l_fNumer = l_fTmp1 - l_fTmp0;
                    l_fDenom = l_fA00 - (2.f)*l_fA01 + l_fA11;
                    if (l_fNumer >= l_fDenom)
                    {
                        l_fT = 1.f;
                        l_fS = 0.f;
                        l_fSqrDistance = l_fA11 + (2.f)*l_fB1 + l_fC;
                    }
                    else
                    {
                        l_fT = l_fNumer/l_fDenom;
                        l_fS = 1.f - l_fT;
                        l_fSqrDistance = l_fS*(l_fA00*l_fS + l_fA01*l_fT + (2.f)*l_fB0) +
                           l_fT*(l_fA01*l_fS + l_fA11*l_fT + (2.f)*l_fB1) + l_fC;
                    }
                }
                else
                {
                    l_fT = 0.f;
                    if (l_fTmp1 <= 0.f)
                    {
                        l_fS = 1.f;
                        l_fSqrDistance = l_fA00 + (2.f)*l_fB0 + l_fC;
                    }
                    else if (l_fB0 >= 0.f)
                    {
                        l_fS = 0.f;
                        l_fSqrDistance = l_fC;
                    }
                    else
                    {
                        l_fS = -l_fB0/l_fA00;
                        l_fSqrDistance = l_fB0*l_fS + l_fC;
                    }
                }
            }
            else  // region 1
            {
                l_fNumer = l_fA11 + l_fB1 - l_fA01 - l_fB0;
                if (l_fNumer <= 0.f)
                {
                    l_fS = 0.f;
                    l_fT = 1.f;
                    l_fSqrDistance = l_fA11 + (2.f)*l_fB1 + l_fC;
                }
                else
                {
                    l_fDenom = l_fA00 - (2.f)*l_fA01 + l_fA11;
                    if (l_fNumer >= l_fDenom)
                    {
                        l_fS = 1.f;
                        l_fT = 0.f;
                        l_fSqrDistance = l_fA00 + (2.f)*l_fB0 + l_fC;
                    }
                    else
                    {
                        l_fS = l_fNumer/l_fDenom;
                        l_fT = 1.f - l_fS;
                        l_fSqrDistance = l_fS*(l_fA00*l_fS + l_fA01*l_fT + (2.f)*l_fB0) +
                           l_fT*(l_fA01*l_fS + l_fA11*l_fT + (2.f)*l_fB1) + l_fC;
                    }
                }
            }
        }

        // Account for numerical round-off error.
        if (l_fSqrDistance < 0.f)
        {
            l_fSqrDistance = 0.f;
        }


        return l_fSqrDistance;
    }
}


#endif

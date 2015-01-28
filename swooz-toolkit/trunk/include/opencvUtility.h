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
 * \file opencvUtility.h
 * \brief utility functions definitions using opencv
 * \author Florian Lance
 * \date 05/11/12
 */

#ifndef _OPENCV_UTILIY_
#define _OPENCV_UTILIY_

#include "opencv2/core/core.hpp"
#include "commonTypes.h"

static const cv::Scalar GREEN(0,255,0);
static const cv::Scalar BLUE(255,0,0);
static const cv::Scalar RED(0,0,255);
static const cv::Scalar WHITE(255,255,255);

namespace swUtil
{
    static std::string int2string(const int i)
    {
        std::stringstream out;
        out<<i;
        return out.str();
    }

    static void displayFPS(cv::Mat &oInputMat)
    {
        // determines the fps
        static double freq = cv::getTickFrequency();
        static int64 tm = 0;
        int64 fps = static_cast<int64> (freq/(cv::getTickCount() - tm)) ;
        tm = cv::getTickCount();

        putText( oInputMat, "fps: " + int2string(static_cast<int>(fps)), cv::Point( 10,50), cv::FONT_HERSHEY_SIMPLEX, 0.7, RED, 3 );
    }

    static int getFPS()
    {
        static double l_dFreq = cv::getTickFrequency();
        static int64 tm = 0;
        int64 fps = static_cast<int64> (l_dFreq/(cv::getTickCount() - tm));
        tm = cv::getTickCount();
        return static_cast<int>(fps);
    }

    static void displayTextTopRight(cv::Mat &oInputMat, const std::string &oText)
    {
        putText( oInputMat, oText, cv::Point( oInputMat.cols - 100 ,50), cv::FONT_HERSHEY_SIMPLEX, 0.7, RED, 3 );
    }

    static void displayTextDownRight(cv::Mat &oInputMat, const std::string &oText)
    {
        putText( oInputMat, oText, cv::Point( oInputMat.cols - 50 - static_cast<int>(oText.size())*10, oInputMat.rows - 50), cv::FONT_HERSHEY_SIMPLEX, 0.7, RED, 3 );
    }

    /**
     * \brief Check if the rectangle is inside the matrix.
     * \param [in] oInputRect : rectangle to check
     * \param [in] oInputMat  : matrix
     * \return true if it is inside, else return false
     */
    static bool isInside(cv::Rect &oInputRect, cv::Mat &oInputMat)
    {
        return (oInputRect.x + oInputRect.width < oInputMat.cols) ?
            ((oInputRect.y + oInputRect.height < oInputMat.rows) ? 1 : 0) : 0;
    }

    /**
     * \brief Upscale a 2D Rectangle.
     * \param [in] oRect    : rectangle to upscale
     * \param [in] fXScale  : x scale coeff
     * \param [in] fYScale  : y scale coeff
     * \return the upscaled rectangle.
     */
    static cv::Rect upScale(const cv::Rect &oRect, cfloat fXScale, cfloat fYScale)
    {
        cv::Rect l_oScaledRect = oRect;

        l_oScaledRect.x      -= (int)(l_oScaledRect.width  * fXScale);
        l_oScaledRect.y      -= (int)(l_oScaledRect.height * fYScale);
        l_oScaledRect.width  += (int)(l_oScaledRect.width  * fXScale * 2);
        l_oScaledRect.height += (int)(l_oScaledRect.height * fYScale * 2);

        return l_oScaledRect;
    }

    static void rodriguesRotation(const cv::Vec3d &oU, const cv::Vec3d &oV, cv::Mat &oRot)
    {
        cv::Vec3d u,v;
        cv::normalize(oU,u);
        cv::normalize(oV,v);
//        = cv::normalize(oU);
//        cv::Vec3d v = cv::normalize(oV);

        cv::Vec3d uXv = u.cross(v);
        double cosTheta = u.dot(v);
        double sinTheta = cv::norm(uXv);

        cv::Vec3d a(uXv * (1.0 / sinTheta));
        cv::Mat m1 = cv::Mat::eye(3,3, CV_64FC1) * cosTheta;
        cv::Mat m2(a*(a.t()) * (1.0 - cosTheta));

        cv::Mat m3 = cv::Mat(3,3, CV_64FC1, cv::Scalar(0.0));
        m3.at<double>(0,1) -= a[2]; m3.at<double>(0,2) += a[1];
        m3.at<double>(1,0) += a[2]; m3.at<double>(1,2) -= a[0];
        m3.at<double>(2,0) -= a[1]; m3.at<double>(2,1) += a[0];

        m3 *= sinTheta;

        oRot = m1 + m2 + m3;

        for(int ii = 0; ii < oRot.rows * oRot.cols; ++ii)
        {
            if(oRot.at<double>(ii) < 0.00001 && oRot.at<double>(ii) > -0.00001)
            {
                oRot.at<double>(ii) = 0.0;
            }
        }
    }
}

#endif

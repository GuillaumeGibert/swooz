
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
        putText( oInputMat, oText, cv::Point( oInputMat.cols - 50 - oText.size()*10, oInputMat.rows - 50), cv::FONT_HERSHEY_SIMPLEX, 0.7, RED, 3 );
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
}

#endif

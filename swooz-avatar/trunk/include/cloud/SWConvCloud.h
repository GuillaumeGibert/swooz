
/**
 * \file SWConvCloud.h
 * \brief Contains utility functions for converting clouds.
 * \author Florian Lance
 * \date 29/01/13
 */

#ifndef _SWCONVCLOUD_
#define _SWCONVCLOUD_

#include "cloud/SWMaskCloud.h"
#include "opencv2/imgproc/imgproc.hpp"

namespace swCloud
{
    /**
     * \brief Convert a cloud cv mat to a SWCloud
     * \param [in] oInputCloudMat  : input cv mat cloud
     * \param [in,out] oCloudPoint : result SWCloud
     * \param [in] fMinDist        : minimum depth of the points to keep
     * \param [in] fDepth          : fMinDist + fDepth will be the maximum depth of the points to keep
     * \param [in] ui8R	       : R RGB component value for coloring the cloud point
     * \param [in] ui8G	       : G RGB component value for coloring the cloud point
     * \param [in] ui8B	       : B RGB component value for coloring the cloud point
     * \return true if sucess, return false if wrong parameters
     */
    static bool convCloudMat2SWCloud(const cv::Mat &oInputCloudMat, SWCloud &oCloudPoint,
                     cfloat fMinDist = 0.f,   cfloat fDepth = 10.f,
                     cuint8 ui8R = 255, cuint8 ui8G = 255, cuint8 ui8B = 255)
    {
        if(fDepth < 0.f || oInputCloudMat.rows == 0 || oInputCloudMat.cols == 0)
        {
            std::cerr << "Error convCloudMat2SWCloud : bad parameters. " << std::endl;
            return false;
        }

        uint l_ui32NumberOfPoints = 0;

        for(int ii = 0; ii < oInputCloudMat.rows * oInputCloudMat.cols; ++ii)
        {
            if(oInputCloudMat.at<cv::Vec3f>(ii)[2] > fMinDist &&
               oInputCloudMat.at<cv::Vec3f>(ii)[2] < fDepth+fMinDist)
            {
                ++l_ui32NumberOfPoints;
            }
        }

        float *l_aFCoords   = new float[l_ui32NumberOfPoints * 3];
        uint8 *l_aUi8Colors = new uint8[l_ui32NumberOfPoints * 3];

        float *l_fX = &(*l_aFCoords);
        float *l_fY = &l_fX[l_ui32NumberOfPoints];
        float *l_fZ = &l_fY[l_ui32NumberOfPoints];

        uint8 *l_ui8R = &(*l_aUi8Colors);
        uint8 *l_ui8G = &l_ui8R[l_ui32NumberOfPoints];
        uint8 *l_ui8B = &l_ui8G[l_ui32NumberOfPoints];

        uint l_ui32NumPointToAdd = 0;

        for(int ii = 0; ii < oInputCloudMat.rows * oInputCloudMat.cols; ++ii)
        {
            if(oInputCloudMat.at<cv::Vec3f>(ii)[2] > fMinDist &&
               oInputCloudMat.at<cv::Vec3f>(ii)[2] < fDepth+fMinDist )
            {
                l_fX[l_ui32NumPointToAdd] = oInputCloudMat.at<cv::Vec3f>(ii)[0];
                l_fY[l_ui32NumPointToAdd] = oInputCloudMat.at<cv::Vec3f>(ii)[1];
                l_fZ[l_ui32NumPointToAdd] = oInputCloudMat.at<cv::Vec3f>(ii)[2];

                l_ui8R[l_ui32NumPointToAdd] = ui8R;
                l_ui8G[l_ui32NumPointToAdd] = ui8G;
                l_ui8B[l_ui32NumPointToAdd] = ui8B;

                ++l_ui32NumPointToAdd;
            }
        }

        oCloudPoint.set(l_ui32NumberOfPoints, l_aFCoords, l_aUi8Colors);

        return true;
    }


    /**
     * \brief Convert a cloud cv mat to a SWCloud
     * \param [in] oInputCloudMat  : input cv mat cloud
     * \param [in] oInputRgbMat    : input cv mat rgb, used for init the colors of the cloud points
     * \param [in,out] oCloudPoint : result SWCloud
     * \param [in] fMinDist        : minimum depth of the points to keep
     * \param [in] fDepth          : fMinDist + fDepth will be the maximum depth of the points to keep
     * \return true if sucess, return false if wrong parameters
     */
    static bool convCloudMat2SWCloud(const cv::Mat &oInputCloudMat, const cv::Mat &oInputRgbMat, SWCloud &oCloudPoint, cfloat fMinDist = 0.f, cfloat fDepth = 10.f)
    {
        if(fDepth < 0.f || oInputCloudMat.rows == 0 || oInputCloudMat.cols == 0)
        {
            std::cerr << "Error convCloudMat2SWCloud : bad parameters. " << std::endl;
            return false;
        }

        uint l_ui32NumberOfPoints = 0;

        for(int ii = 0; ii < oInputCloudMat.rows * oInputCloudMat.cols; ++ii)
        {
            if(oInputCloudMat.at<cv::Vec3f>(ii)[2] > fMinDist &&
               oInputCloudMat.at<cv::Vec3f>(ii)[2] < fDepth+fMinDist )
            {
                ++l_ui32NumberOfPoints;
            }
        }

        float *l_aFCoords   = new float[l_ui32NumberOfPoints * 3];
        uint8 *l_aUi8Colors = new uint8[l_ui32NumberOfPoints * 3];

        float *l_fX = &(*l_aFCoords);
        float *l_fY = &l_fX[l_ui32NumberOfPoints];
        float *l_fZ = &l_fY[l_ui32NumberOfPoints];

        uint8 *l_ui8R = &(*l_aUi8Colors);
        uint8 *l_ui8G = &l_ui8R[l_ui32NumberOfPoints];
        uint8 *l_ui8B = &l_ui8G[l_ui32NumberOfPoints];

        uint l_ui32NumPointToAdd = 0;

        for(int ii = 0; ii < oInputCloudMat.rows * oInputCloudMat.cols; ++ii)
        {
            if(oInputCloudMat.at<cv::Vec3f>(ii)[2] > fMinDist &&
               oInputCloudMat.at<cv::Vec3f>(ii)[2] < fDepth+fMinDist )
            {
                l_fX[l_ui32NumPointToAdd] = oInputCloudMat.at<cv::Vec3f>(ii)[0];
                l_fY[l_ui32NumPointToAdd] = oInputCloudMat.at<cv::Vec3f>(ii)[1];
                l_fZ[l_ui32NumPointToAdd] = oInputCloudMat.at<cv::Vec3f>(ii)[2];

                l_ui8R[l_ui32NumPointToAdd] = (int)oInputRgbMat.at<cv::Vec3b>(ii)[2];
                l_ui8G[l_ui32NumPointToAdd] = (int)oInputRgbMat.at<cv::Vec3b>(ii)[1];
                l_ui8B[l_ui32NumPointToAdd] = (int)oInputRgbMat.at<cv::Vec3b>(ii)[0];

                ++l_ui32NumPointToAdd;
            }
        }

        oCloudPoint.set(l_ui32NumberOfPoints, l_aFCoords, l_aUi8Colors);

        return true;
    }

    /**
     * \brief Convert a cloud cv mat to a SWMaskCloud
     * \param [in] oInputCloudMat  : input cv mat cloud
     * \param [in] oInputMaskMat   : input cv mat mask
     * \param [in,out] oCloudPoint : result SWMaskCloud
     * \param [in] fMinDist        : minimum depth of the points to keep
     * \param [in] fDepth          : fMinDist + fDepth will be the maximum depth of the points to keep
     * \param [in] ui8R	       : R RGB component value for coloring the cloud point
     * \param [in] ui8G	       : G RGB component value for coloring the cloud point
     * \param [in] ui8B	       : B RGB component value for coloring the cloud point
     * \return true if sucess, return false if wrong parameters
     */
    static bool convCloudMat2SWMaskCloud(const cv::Mat &oInputCloudMat, const cv::Mat &oInputMaskMat, SWMaskCloud &oCloudPoint,
                         cfloat fMinDist = 0.f,   cfloat fDepth = 10.f, cuint8 ui8R = 255, cuint8 ui8G = 255, cuint8 ui8B = 255)
    {
        if(fDepth < 0.f || oInputCloudMat.rows == 0 || oInputCloudMat.cols == 0)
        {
            std::cerr << "Error convCloudMat2SWMaskCloud : bad parameters. " << std::endl;
            return false;
        }

        uint l_ui32NumberOfPoints = 0;

        for(int ii = 0; ii < oInputCloudMat.rows * oInputCloudMat.cols; ++ii)
        {
            if(oInputCloudMat.at<cv::Vec3f>(ii)[2] > fMinDist &&
               oInputCloudMat.at<cv::Vec3f>(ii)[2] < fDepth+fMinDist )
            {
                ++l_ui32NumberOfPoints;
            }
        }

        float *l_aFCoords   = new float[l_ui32NumberOfPoints * 3];
        uint8 *l_aUi8Colors = new uint8[l_ui32NumberOfPoints * 3];

        std::vector<int> l_vI32Mask;

        float *l_fX = &(*l_aFCoords);
        float *l_fY = &l_fX[l_ui32NumberOfPoints];
        float *l_fZ = &l_fY[l_ui32NumberOfPoints];

        uint8 *l_ui8R = &(*l_aUi8Colors);
        uint8 *l_ui8G = &l_ui8R[l_ui32NumberOfPoints];
        uint8 *l_ui8B = &l_ui8G[l_ui32NumberOfPoints];

        uint l_ui32NumPointToAdd = 0;

        for(int ii = 0; ii < oInputCloudMat.rows * oInputCloudMat.cols; ++ii)
        {
            if(oInputCloudMat.at<cv::Vec3f>(ii)[2] > fMinDist &&
               oInputCloudMat.at<cv::Vec3f>(ii)[2] < fDepth+fMinDist )
            {
                l_fX[l_ui32NumPointToAdd] = oInputCloudMat.at<cv::Vec3f>(ii)[0];
                l_fY[l_ui32NumPointToAdd] = oInputCloudMat.at<cv::Vec3f>(ii)[1];
                l_fZ[l_ui32NumPointToAdd] = oInputCloudMat.at<cv::Vec3f>(ii)[2];

                l_ui8R[l_ui32NumPointToAdd] = ui8R;
                l_ui8G[l_ui32NumPointToAdd] = ui8G;
                l_ui8B[l_ui32NumPointToAdd] = ui8B;

                l_vI32Mask.push_back(oInputMaskMat.at<int>(ii));

                ++l_ui32NumPointToAdd;
            }
        }

        oCloudPoint.set(l_ui32NumberOfPoints, l_aFCoords, l_aUi8Colors);

        oCloudPoint.setMask(l_vI32Mask);

        return true;
    }
}

#endif

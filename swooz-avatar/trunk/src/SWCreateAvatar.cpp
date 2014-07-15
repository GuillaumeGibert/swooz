
/**
 * \file SWCreateAvatar.cpp
 * \author LANCE Florian
 * \date 05/11/12
 * \brief Defines SWCreateAvatar
 */

#include "SWCreateAvatar.h"

#include "cloud/SWImageProcessing.h"
#include "cloud/SWConvCloud.h"
#include "cloud/SWRadialProjectionCloud.h"
#include "opencvUtility.h"

// UTILITY
#include <time.h>

using namespace swDevice;


// ############################################# CONSTRUCTORS / DESTRUCTORS

SWCreateAvatar::SWCreateAvatar(cbool bVerbose) : m_bVerbose(bVerbose), m_i32NumCloud(0)
{
    // detection
        m_bDetectStasmPoints        = false;
        m_fRemoveBackGroundDistance = 1.5f;
        m_CFaceDetectPtr            = SWFaceDetectionPtr(new swDetect::SWFaceDetection(cv::Size(80,80),
                                                        false, "../data/classifier/haarcascade_frontalface_alt.xml"));
        m_CStasmDetectPtr           = SWStasmPtr(new swDetect::SWStasm("../data/stasm/mu-68-1d.conf", "../data/stasm/mu-76-2d.conf")); // TODO : manage error files

    // clouds / alignment
        m_fTemplateDownScale        = 0.2f;
        m_fTargetDownScale          = 0.2f;
        m_fDepthCloud               = 0.12f;
//        m_fDistMaxAlignment         = 0.00020f;// current
        m_fDistMaxAlignment         = 0.0001f;
//        m_fDistMaxAlignment         = 0.0000001f;


//        * @brief Set emicp alignment parameters (do no use if you don't know how emicp works, native parameters are good for a large panel of usages)
//        * @param [in] fP2      : emicp P2
//        * @param [in] fINF     : emicp INF
//        * @param [in] fFactor  : emicp Factor
//        * @param [in] fD02     : emicp D02
//        */
//        m_oAlignClouds.setEmicpParams(0.001f, 0.00001f, 0.9f, 0.01f); // best
        m_oAlignClouds.setEmicpParams(0.001f, 0.00001f, 0.6f, 0.01f); //
//        m_oAlignClouds.setEmicpParams(0.0008f, 0.000008f, 0.6f, 0.01f); // best 2



    // radial projection
        m_i32Dilate                 = 0;
        m_i32Erode                  = 0;
        m_i32WidthRadialProj        = 350;  // 350 // 200
        m_i32HeightRadialProj       = 200;  // 250 // 125
        m_fCylinderRadius           = 0.15f;
        m_i32ExpandValue            = 5;
        m_i32ExpandConnex           = 2;
        m_i32EraseValue             = 0;
        m_i32EraseConnex            = 0;

    // bilateral filters
        m_bUseBilateralFilter       = true;
        m_i32BilateralFilter        = -1;
        m_i32ColorFilterValue       = 200;   /**< color value bilateral filter*/
        m_i32SpaceFilterValue       = 0;    /**< space value bilateral filter*/

    // init rects
        m_oLastRectFace.width       = 0;
        m_oLastRectNose.width       = 0;
}

// ############################################# METHODS

void SWCreateAvatar::resetData()
{
    m_i32NumCloud = 0;
    m_oLastRectFace.width = 0;
    m_oLastRectNose.width = 0;
    m_oFaceCloudRef.erase();
    m_oNoseCloudRef.erase();
    m_oAccumulatedFaceClouds.erase();
    m_vUi32CloudNumbersOfPoints.clear();

    // stasm
    m_vStasm3DPoints.clear();
    m_vP3FStasm3DPoints.clear();
    m_vP3FStasm3DPoints.assign(68, cv::Point3f(0.f,0.f,0.f));

//    m_vP3FTotalStasm3DPoints.clear();
    m_vStasm3DPoints.clear();


//    m_vP3FTotalStasm3DPoints.assign(68, cv::Point3f(0.f,0.f,0.f));
}

void SWCreateAvatar::setHeightRadialProjection(cuint ui32HeightRadialProj)
{
    m_i32HeightRadialProj = ui32HeightRadialProj;
}

void SWCreateAvatar::setWidthRadialProjection(cuint ui32WidthRadialProj)
{
    m_i32WidthRadialProj = ui32WidthRadialProj;
}

void SWCreateAvatar::setBilateralFilter(cint i32BilateralFilterIteration)
{
    m_i32BilateralFilter = i32BilateralFilterIteration;
}

void SWCreateAvatar::setColorFilterValue(cuint ui32ColorFilterValue)
{
    m_i32ColorFilterValue = ui32ColorFilterValue;
}

void SWCreateAvatar::setSpaceFilterValue(cuint ui32SpaceFilterValue)
{
    m_i32SpaceFilterValue = ui32SpaceFilterValue;
}

void SWCreateAvatar::setCylinderRadiusValue(cfloat fRadius)
{
    m_fCylinderRadius = fRadius;
}

void SWCreateAvatar::setUseBilateralFilter(cbool bUseFilter)
{
    m_bUseBilateralFilter = bUseFilter;
}

void SWCreateAvatar::setUseStasm(cbool bUseStasm)
{
    m_bDetectStasmPoints = bUseStasm;

    m_vP3FStasm3DPoints.clear();
    m_vStasm3DPoints.clear();
    m_CStasmDetectPtr->resetParams();
}

void SWCreateAvatar::setErodeValue(cuint ui32Erode)
{
    m_i32Erode = ui32Erode;
}

void SWCreateAvatar::setDilateValue(cuint ui32Dilate)
{
    m_i32Dilate = ui32Dilate;
}

void SWCreateAvatar::setExpandValue(const int i32Val)
{
    m_i32ExpandValue = i32Val;
}

void SWCreateAvatar::setExpandConnex(const int i32Val)
{
    m_i32ExpandConnex = i32Val;
}

void SWCreateAvatar::setEraseValue(const int i32Val)
{
    m_i32EraseValue = i32Val;
}

void SWCreateAvatar::setEraseConnex(const int i32Val)
{
    m_i32EraseConnex = i32Val;
}

void SWCreateAvatar::setDepthCloud(const float fDepthCloud)
{
    m_fDepthCloud = fDepthCloud;
}

void SWCreateAvatar::addPixelToDelete(const std::vector<int> pixel)
{
    m_vPixelToDelete.push_back(pixel);
}

void SWCreateAvatar::resetDeletedPointsRadialProj()
{
    m_vPixelToDelete.clear();
}

cv::Rect SWCreateAvatar::lastRectFace() const
{
    return m_oLastRectFace;
}

cv::Rect SWCreateAvatar::lastRectNose() const
{
    return m_oLastRectNose;
}

void SWCreateAvatar::lastRadialProjection(cv::Mat &oFilteredRadialProj) const
{
    oFilteredRadialProj = m_oFilteredRadialProjection.clone();
}

bool SWCreateAvatar::addCloudToAvatar(const cv::Mat &oRgb, const cv::Mat &oDepth)
{
   // copy input data
       cv::Mat l_oRgb   = oRgb.clone();
       cv::Mat l_oDepth = oDepth.clone();

    // check size mat
       if(l_oRgb.rows != l_oDepth.rows)
       {
           cv::resize(l_oRgb, l_oRgb, cv::Size(l_oDepth.cols,l_oDepth.rows));
       }

   // remove background
       cv::Mat l_oRgbForeGround    = swImage::swUtil::removeBackground(l_oRgb, l_oDepth, m_fRemoveBackGroundDistance);

   // detect face
       if(!m_CFaceDetectPtr->detectFace(l_oRgbForeGround))
       {
           if(m_oLastRectFace.width == 0)
           {
               std::cerr << "Face not detected. The face must be detected in the first frame. " << std::endl;
               return false;
           }
           else
           {
//               std::cout << "Face not detected. Use previous face rectangle. " << std::endl;
           }
       }
       else
       {
            // retrieve face rectangle
                m_oLastRectFace = m_CFaceDetectPtr->faceRect();

            // upsize face rectangle
                m_oLastRectFace.x -= (int)(m_oLastRectFace.width *0.1);
                m_oLastRectFace.width += (int)(m_oLastRectFace.width *0.2);
                m_oLastRectFace.y -= (int)(m_oLastRectFace.height *0.05);
                m_oLastRectFace.height += (int)(m_oLastRectFace.height *0.1);
        }

    // detect nose
       cv::Rect l_oCurrentNoseRect = m_CFaceDetectPtr->detectNose(l_oRgbForeGround(m_oLastRectFace));


   // compute nose tip
       int l_i32IdNoseX, l_i32IdNoseY;
       cv::Point3f l_oNoseTip;

       cv::Rect l_oRectangleFromNoseTip;

       if(l_oCurrentNoseRect.width > 0)
       {
            l_oNoseTip = m_CFaceDetectPtr->computeNoseTip((l_oDepth(m_oLastRectFace))(l_oCurrentNoseRect), l_i32IdNoseX, l_i32IdNoseY);

            l_oRectangleFromNoseTip.x = m_oLastRectFace.x + l_oCurrentNoseRect.x + l_i32IdNoseX - 30;
            l_oRectangleFromNoseTip.y = m_oLastRectFace.y + l_oCurrentNoseRect.y + l_i32IdNoseY - 50;
       }
       else
       {
            l_oNoseTip = m_CFaceDetectPtr->computeNoseTip(l_oDepth(m_oLastRectFace), l_i32IdNoseX, l_i32IdNoseY);
            l_oRectangleFromNoseTip.x = m_oLastRectFace.x + l_i32IdNoseX - 30;
            l_oRectangleFromNoseTip.y = m_oLastRectFace.y + l_i32IdNoseY - 50;
       }

       l_oRectangleFromNoseTip.width   = 60;
       l_oRectangleFromNoseTip.height  = 70;
       m_oLastRectNose = l_oRectangleFromNoseTip;

   // detect stasm features points
        cv::Mat l_oStasmMask;
        if(m_bDetectStasmPoints && m_vStasm3DPoints.size() < 5)
        {
            m_CStasmDetectPtr->resetParams();
            m_CStasmDetectPtr->launchAsmSearch(l_oRgbForeGround, m_oLastRectFace);
            m_CStasmDetectPtr->setStasmMask(l_oDepth, l_oStasmMask);
            l_oStasmMask = l_oStasmMask(m_oLastRectFace);
            m_CStasmDetectPtr->compute3DPoints(l_oDepth, m_vP3FStasm3DPoints);           
        }

    // create cloud
        swCloud::SWCloud l_oFaceCloud, l_oNoseCloud;
        swCloud::convCloudMat2SWCloud(l_oDepth(m_oLastRectFace), l_oRgb(m_oLastRectFace), l_oFaceCloud, l_oNoseTip.z-0.5f, m_fDepthCloud+0.5f);
        swCloud::convCloudMat2SWCloud(l_oDepth(m_oLastRectNose), l_oRgb(m_oLastRectNose), l_oNoseCloud, l_oNoseTip.z-0.5f, m_fDepthCloud+0.5f );

    bool l_bIsLastCloudValid = false;

    if(m_i32NumCloud++ == 0)
    {
        // save reference face cloud
            m_oFaceCloudRef.copy(l_oFaceCloud);
        // save reference nose cloud
            m_oNoseCloudRef.copy(l_oNoseCloud);
        // retrieve face texture
            m_oTextureMat = l_oRgbForeGround(m_oLastRectFace).clone();
        // apply median blur on the texture
//            cv::medianBlur(m_oTextureMat, m_oTextureMat, 3);
        // save the texture
            string l_sPath = "../data/textures/avatars/";
            cv::imwrite(l_sPath + "texture.png" , m_oTextureMat);

        // compute cloud bbox of the texture
            swImage::swUtil::computeSizeCloudRect(m_oLastRectFace, oDepth, m_oCloudFaceBBox);
       }
   // save next clouds
       else
       {
           // align clouds
           m_oAlignClouds.setClouds(m_oNoseCloudRef, l_oNoseCloud);
           m_oAlignClouds.setCloudDownscale(m_fTargetDownScale, m_fTemplateDownScale);
           m_oAlignClouds.alignClouds();

           // transform clouds
           m_oAlignClouds.transformedCloud(l_oFaceCloud);

           float l_fScore = m_oFaceCloudRef.squareDistanceCloud(l_oFaceCloud, true, 0.1f);
           if(m_bVerbose)
           {
               std::cout << "Score : " << l_fScore << " --- " << m_fDistMaxAlignment << " : " << endl;
           }

           if(l_fScore > m_fDistMaxAlignment)
           {
               l_bIsLastCloudValid = false;
               std::cerr << "Distance too hight between current cloud and reference cloud, rejected. " << std::endl;
               return false;
           }
           else
           {
               l_bIsLastCloudValid = true;
           }
       }

   // add cloud to the sum of clouds
       if(l_bIsLastCloudValid)
       {
           m_oAccumulatedFaceClouds += l_oFaceCloud;
           m_vUi32CloudNumbersOfPoints.push_back(l_oFaceCloud.size());

           if(m_bDetectStasmPoints && m_vStasm3DPoints.size() < 5)
           {
                m_vStasm3DPoints.push_back(m_vP3FStasm3DPoints);
           }
       }

   return true;
}


void SWCreateAvatar::constructAvatar()
{
    // init
        uint l_ui32CurrPointNumber = 0;
        swCloud::SWCloud l_oCloudPart, l_oCloudParCylinderProj;
        std::vector<cv::Mat> l_vMatProj;
        swCloud::SWCloudBBox l_oBBox = m_oAccumulatedFaceClouds.bBox();

    // compute 2D images with the total cloud
        for(uint ii = 0; ii < m_vUi32CloudNumbersOfPoints.size(); ++ii)
        {
            cv::Mat l_oRadialProjMat;

            // retrieve the good parts
                m_oAccumulatedFaceClouds.retrieveCloudPart(l_oCloudPart, l_ui32CurrPointNumber, l_ui32CurrPointNumber + m_vUi32CloudNumbersOfPoints[ii]);
                l_ui32CurrPointNumber += m_vUi32CloudNumbersOfPoints[ii];

            // project the cloud on a cylinder
                swCloud::radialProjCloudOnMat(l_oCloudPart, l_oRadialProjMat, l_oCloudParCylinderProj, l_oBBox, m_i32WidthRadialProj, m_i32HeightRadialProj, m_fCylinderRadius);

            // stock current mat image
                l_vMatProj.push_back(l_oRadialProjMat);
        }

    // temporal filtering
        uint l_ui32CurrNumberOfPoints;
        float l_fCurrTotalValue;
        cv::Mat l_oFinalMat = cv::Mat(l_vMatProj.back().rows, l_vMatProj.back().cols, l_vMatProj.back().depth());

        for(int ii = 0; ii < l_oFinalMat.rows * l_oFinalMat.cols; ++ii)
        {
            l_ui32CurrNumberOfPoints = 0;
            l_fCurrTotalValue        = 0.f;

            for(uint jj = 0; jj < l_vMatProj.size(); ++jj)
            {
                if(l_vMatProj[jj].at<float>(ii) > 0.f)
                {
                    l_fCurrTotalValue += l_vMatProj[jj].at<float>(ii);
                    l_ui32CurrNumberOfPoints++;
                }
            }

            if(l_ui32CurrNumberOfPoints > 0)
            {
                l_oFinalMat.at<float>(ii) = l_fCurrTotalValue  / (float)l_ui32CurrNumberOfPoints;
            }
            else
            {
                l_oFinalMat.at<float>(ii) = 0.f;
            }
        }
        cv::imwrite("../data/images/radialProj/1_finalTemporal.png" ,l_oFinalMat);

    // keep only one connex aggregat
        swCloud::keepBiggestConnexAggregate(l_oFinalMat, 50);
            cv::imwrite("../data/images/radialProj/2_keepConnexe.png" ,l_oFinalMat);

    // expand contours
        swCloud::expandContoursRadialProj(l_oFinalMat, m_i32ExpandValue, m_i32ExpandConnex);
            cv::imwrite("../data/images/radialProj/3_expanded.png" ,l_oFinalMat);

    // dilate / erode
        if(m_i32Dilate >= 3)
        {
            cv::dilate(l_oFinalMat,  l_oFinalMat, cv::Mat(), cv::Point(-1, -1), m_i32Dilate);
        }
            cv::imwrite("../data/images/radialProj/4_dilate.png" ,l_oFinalMat);
        if(m_i32Erode >= 3)
        {
            cv::erode(l_oFinalMat,  l_oFinalMat, cv::Mat(), cv::Point(-1, -1), m_i32Erode);
        }
            cv::imwrite("../data/images/radialProj/5_erode.png" ,l_oFinalMat);

    // spatial filtering
        cv::Mat l_oFinalFilteredMat;
        cv::Mat l_oMaskFinalMal = l_oFinalMat.clone();

    // applying the spatial filtering
        if(m_bUseBilateralFilter)
        {
            cv::bilateralFilter(l_oFinalMat, l_oFinalFilteredMat, m_i32BilateralFilter, m_i32ColorFilterValue, m_i32SpaceFilterValue);
                 cv::imwrite("../data/images/radialProj/6_biolateral.png" ,l_oFinalFilteredMat);

            // cancel the effects of the filter on the borders
                for(int ii = 0; ii < l_oFinalMat.rows * l_oFinalMat.cols; ++ii)
                {
                    if(l_oMaskFinalMal.at<float>(ii) == 0.f)
                    {
                        l_oFinalFilteredMat.at<float>(ii) = 0.f;
                    }
                }

                cv::imwrite("../data/images/radialProj/7_cancelBorder.png" ,l_oFinalFilteredMat);
        }
        else
        {
            l_oFinalFilteredMat = l_oFinalMat.clone();
        }


    // erase contours
        swCloud::eraseContoursRadialProj(l_oFinalFilteredMat, m_i32EraseValue, m_i32EraseConnex);
            cv::imwrite("../data/images/radialProj/8_eraseContours.png" ,l_oFinalFilteredMat);


    // apply manual zone selection
        for(uint ii = 0; ii < m_vPixelToDelete.size(); ++ii)
        {
            l_oFinalFilteredMat.at<float>(m_vPixelToDelete[ii][1], m_vPixelToDelete[ii][0]) = 0.f;
        }
            cv::imwrite("../data/images/radialProj/9_pixelsDeleted.png" ,l_oFinalMat);

    // keep only one connex aggregat
        swCloud::keepBiggestConnexAggregate(l_oFinalFilteredMat, 50);
            cv::imwrite("../data/images/radialProj/10_keepConnexe.png" ,l_oFinalFilteredMat);

    // save radial projection for display
        m_oFilteredRadialProjection = l_oFinalFilteredMat.clone();

    // compute vertex and faces, and save the obj file
        swCloud::transformRadialProjToMesh(l_oFinalFilteredMat, m_oLastResultFaceMesh, l_oBBox, m_oCloudFaceBBox, m_i32WidthRadialProj, m_i32HeightRadialProj, 0.15f);

    if(m_bDetectStasmPoints)
    {
        computeSTASMCoords();
    }
}

void SWCreateAvatar::totalCloud(swCloud::SWCloud &oTotalCloud)
{
    oTotalCloud.copy(m_oAccumulatedFaceClouds);
}

void SWCreateAvatar::lastResultFaceMesh(swMesh::SWMesh &oResultMesh)
{
    oResultMesh = m_oLastResultFaceMesh;
}

void SWCreateAvatar::computeSTASMCoords()
{
    m_vMeshIdSTASMPoints.clear();

    std::vector<std::vector<float> >l_vMeanSTASMPoint(m_vStasm3DPoints[0].size(),std::vector<float>(3,0.f));

    for(uint ii = 0; ii < m_vStasm3DPoints.size(); ++ii) // 0 - 5
    {
        for(uint jj = 0; jj < m_vStasm3DPoints[ii].size(); ++jj) // 0 - 68
        {
            l_vMeanSTASMPoint[jj][0] += m_vStasm3DPoints[ii][jj].x;
            l_vMeanSTASMPoint[jj][1] += m_vStasm3DPoints[ii][jj].y;
            l_vMeanSTASMPoint[jj][2] += m_vStasm3DPoints[ii][jj].z;
        }
    }

    for(uint ii = 0; ii < l_vMeanSTASMPoint.size(); ++ii)
    {
        std::vector<float> l_vCurrPoint = swUtil::mul(l_vMeanSTASMPoint[ii], 1.f /  m_vStasm3DPoints.size());
        m_vMeshIdSTASMPoints.push_back(m_oLastResultFaceMesh.cloud()->idNearestPoint(l_vCurrPoint));
    }
}

std::vector<int> SWCreateAvatar::mesh_stasm_points_index() const
{
    return m_vMeshIdSTASMPoints;
}

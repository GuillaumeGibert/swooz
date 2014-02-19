
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

using namespace swDevice;



// ############################################# CONSTRUCTORS / DESTRUCTORS

SWCreateAvatar::SWCreateAvatar(const SWKinectParams &oKinectParams, bool bVerbose) : m_CKinectParams(oKinectParams),
    m_bVerbose(bVerbose), m_i32NumCloud(0)
{
    // detection
        m_bDetectStasmPoints        = false;
        m_fRemoveBackGroundDistance = 1.5f;
        m_CFaceDetectPtr            = SWFaceDetectionPtr(new swDetect::SWFaceDetection(cv::Size(80,80)));
        m_CStasmDetectPtr           = SWStasmPtr(new swDetect::SWStasm(m_CKinectParams, "../data/stasm/mu-68-1d.conf", "../data/stasm/mu-76-2d.conf")); // TODO : manage error files

    // clouds / alignment
        m_fTemplateDownScale        = 0.3f;
        m_fTargetDownScale          = 0.3f;
        m_fDepthCloud               = 0.15f;
        m_fDistMaxAlignment         = 0.00005f;
        m_oAlignClouds.setEmicpParams(0.01f, 0.00001f, 0.85f, 0.01f);

    // radial projection
        m_i32Dilate                 = 3;
        m_i32Erode                  = 5;
        m_i32WidthRadialProj        = 675;  // 300
        m_i32HeightRadialProj       = 485;  // 400
        m_fCylinderRadius           = 0.15f;

    // bilateral filters
        m_bUseBilateralFilter       = true;
        m_i32BilateralFilter        = 0;
        m_i32ColorFilterValue       = 40;   /**< color value bilateral filter*/
        m_i32SpaceFilterValue       = 8;    /**< space value bilateral filter*/

    // init rects
        m_oLastRectFace.width       = 0;
        m_oLastRectNose.width       = 0;

    // display kinect video params
        if(m_bVerbose)
        {
           m_CKinectParams.display();
        }
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
    m_vP3FStasm3DPoints.     clear();
    m_vP3FTotalStasm3DPoints.clear();
    m_vP3FStasm3DPoints.     assign(68, cv::Point3f(0.f,0.f,0.f));
    m_vP3FTotalStasm3DPoints.assign(68, cv::Point3f(0.f,0.f,0.f));
}

void SWCreateAvatar::setKinectParams(const swDevice::SWKinectParams &oKinectParams)
{
   m_CKinectParams = oKinectParams;
   m_CStasmDetectPtr->setKinectParams(m_CKinectParams);
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
    m_vP3FTotalStasm3DPoints.clear();
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

   // resize the rgb mat
       if(m_CKinectParams.m_oOriginalSize != m_CKinectParams.m_oVideoSize)
       {
           cv::resize(l_oRgb, l_oRgb, m_CKinectParams.m_oVideoSize);
       }

   // remove background
       cv::Mat l_oRgbForeGround    = swImage::swUtil::removeBackground(l_oRgb, l_oDepth, m_fRemoveBackGroundDistance);

   // detect face
       cv::Rect l_oCurrentRectFace;

       if(!m_CFaceDetectPtr->detect(l_oRgbForeGround))
       {
           if(m_oLastRectFace.width == 0)
           {
               std::cerr << "Face not detected. The face must be detected in the first frame. " << std::endl;
               return false;
           }
           else
           {
               std::cout << "Face not detected. Use previous face rectangle. " << std::endl;
               l_oCurrentRectFace = m_oLastRectFace;
           }
       }
       else
       {
           l_oCurrentRectFace = m_CFaceDetectPtr->faceRect();
           m_oLastRectFace    = l_oCurrentRectFace;
       }

   // compute nose tip
       int l_i32IdNoseX, l_i32IdNoseY;
       cv::Point3f l_oNoseTip = m_CFaceDetectPtr->computeNoseTip(l_oDepth(l_oCurrentRectFace), l_i32IdNoseX, l_i32IdNoseY);

       cv::Rect l_oCurrentRectNose;
       l_oCurrentRectNose.x = l_oCurrentRectFace.x + l_i32IdNoseX -30; // TODO : ratios
       l_oCurrentRectNose.y = l_oCurrentRectFace.y + (int)(l_oCurrentRectFace.height*0.2);
       l_oCurrentRectNose.width  = 60;
       l_oCurrentRectNose.height = (int)(l_oCurrentRectFace.height*0.6);

       m_oLastRectNose = l_oCurrentRectNose;


   // detect stasm features points
        cv::Mat l_oStasmMask;

        if(m_bDetectStasmPoints)
        {
            m_CStasmDetectPtr->resetParams();
            m_CStasmDetectPtr->launchAsmSearch(l_oRgbForeGround, l_oCurrentRectFace);
            m_CStasmDetectPtr->setStasmMask(l_oDepth, l_oStasmMask);
            l_oStasmMask = l_oStasmMask(l_oCurrentRectFace);
            m_CStasmDetectPtr->compute3DPoints(l_oDepth, m_vP3FStasm3DPoints);
        }

    // create cloud
        swCloud::SWCloud l_oFaceCloud, l_oNoseCloud;
        swCloud::convCloudMat2SWCloud(l_oDepth(l_oCurrentRectFace), l_oRgb(l_oCurrentRectFace), l_oFaceCloud, l_oNoseTip.z-0.05f, m_fDepthCloud);
        swCloud::convCloudMat2SWCloud(l_oDepth(l_oCurrentRectNose), l_oRgb(l_oCurrentRectNose), l_oNoseCloud, l_oNoseTip.z-0.05f, m_fDepthCloud);

    bool l_bIsLastCloudValid = false;

    if(m_i32NumCloud++ == 0)
    {
        // save reference face cloud
            m_oFaceCloudRef.copy(l_oFaceCloud);
        // save reference nose cloud
            m_oNoseCloudRef.copy(l_oNoseCloud);
        // save texture
            // scale the face rectangle
                cv::Rect l_oRect = swImage::swUtil::scaleRect(l_oCurrentRectFace, m_CKinectParams.m_fWidthRatio, m_CKinectParams.m_fHeightRatio );

            // apply the texture offset
                l_oRect.x += m_CKinectParams.m_i32OffsetHDTextureX;
                l_oRect.y += m_CKinectParams.m_i32OffsetHDTextureY;

            // save the texture (TODO -> interface parameter)
                string l_sPath = "../data/textures/avatars/";
                cv::imwrite(l_sPath + "t.png" , oRgb(l_oRect));
                m_oTextureMat = oRgb(l_oRect).clone();

            // compute cloud bbox of the texture
                swImage::swUtil::computeSizeCloudRect(l_oCurrentRectFace, oDepth, m_oCloudFaceBBox, m_CKinectParams.m_i32CaptureMode);

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
           std::cout << "Score : " << l_fScore << " --- " << m_fDistMaxAlignment << " : " << endl;

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

//            for(uint ii = 0; ii < m_vP3FStasm3DPoints.size(); ++ii)
//            {
//                m_vP3FTotalStasm3DPoints[ii] += m_vP3FStasm3DPoints[ii]; // BUG
//            }
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

    // dilate / erode
        swCloud::dilateErodeRadialProj(l_oFinalMat, m_i32Dilate, m_i32Erode);

    // expand contours
        swCloud::expandContoursRadialProj(l_oFinalMat, 10,3);

    // spatial filtering
        cv::Mat l_oFinalFilteredMat;
        cv::Mat l_oMaskFinalMal = l_oFinalMat.clone();

    // save the radial projection image before applying the spatial filtering
        if(m_bUseBilateralFilter)
        {
            cv::bilateralFilter(l_oFinalMat, l_oFinalFilteredMat, m_i32BilateralFilter, m_i32ColorFilterValue, m_i32SpaceFilterValue);

            // cancel the effects of the filter on the borders
                for(int ii = 0; ii < l_oFinalMat.rows * l_oFinalMat.cols; ++ii)
                {
                    if(l_oMaskFinalMal.at<float>(ii) == 0.f)
                    {
                        l_oFinalFilteredMat.at<float>(ii) = 0.f;
                    }
                }

            // erase contours
                swCloud::eraseContoursRadialProj(l_oFinalFilteredMat, 20,3);
        }
        else
        {
            l_oFinalFilteredMat = l_oFinalMat.clone();
        }


    // save radial projection for display
        m_oFilteredRadialProjection = l_oFinalFilteredMat.clone();

    // compute vertex and faces, and save the obj file
        swCloud::transformRadialProjToMesh(l_oFinalFilteredMat, m_oLastResultFaceMesh, l_oBBox, m_oCloudFaceBBox, m_i32WidthRadialProj, m_i32HeightRadialProj, 0.15f);

    // save stasm point correspondence with the cloud
//        swCloud::SWCloud l_oFinalCloud;
//        l_oFinalCloud.loadObj(l_sPathObj);
//            computeStasmCorr(l_oFinalCloud, l_sPathStasmCorr);
    // save the final radial projection image
//        cv::imwrite(l_sBasePath + "afterSpatialFiltering.png" ,l_oFinalFilteredMat);

}

void SWCreateAvatar::totalCloud(swCloud::SWCloud &oTotalCloud)
{
    oTotalCloud.copy(m_oAccumulatedFaceClouds);
}

void SWCreateAvatar::lastResultFaceMesh(swMesh::SWMesh &oResultMesh)
{
    oResultMesh = m_oLastResultFaceMesh;
}

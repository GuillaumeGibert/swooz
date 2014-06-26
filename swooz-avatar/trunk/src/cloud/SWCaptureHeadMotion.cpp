
/**
 * \file SWCaptureHeadMotion.cpp
 * \brief defines SWCaptureHeadMotion
 * \author Florian Lance
 * \date 11/02/13
 */

#include "cloud/SWCaptureHeadMotion.h"

#include "cloud/SWImageProcessing.h"
#include "cloud/SWConvCloud.h"
#include "opencvUtility.h"

using namespace swCloud;
using namespace swDevice;
using namespace cv;

// ############################################# CONSTRUCTORS / DESTRUCTORS
 
SWCaptureHeadMotion::SWCaptureHeadMotion(cfloat fAlignmentReducCoeff, cfloat fScoreReducCoeff) :
    m_bVerbose(false), m_bReferenceCloudInitialized(false), m_fAlignmentReductionCoeffTarget(fAlignmentReducCoeff),
    m_fAlignmentReductionCoeffTemplate(fAlignmentReducCoeff),m_fScoreReductionCoeff(fScoreReducCoeff), m_oLastDetectedRectFace(cv::Rect())
{
    // init parameters
        m_bApplyPreviousRigidMotion = false;
        m_fDepthCloud  = 15 * 0.01f;
        m_fDistMax     = 0.00015f;

    // init face detection
        m_CFaceDetectPtr = SWFaceDetectionPtr(new swDetect::SWFaceDetection(Size(80,80)));
}

// ############################################# METHODS


int SWCaptureHeadMotion::computeHeadMotion(SWRigidMotion &oHeadRigidMotion, const cv::Mat &oRgb, const cv::Mat &oDepth,
                                           cv::Mat &oDisplayDetectFace, cv::Point3f oNoseTip)
{
    // DEBUG
//        clock_t l_oFirstTime = clock();

    cv::Rect l_oFaceRectangle;
    cv::Mat l_oRgbForeGround    = swImage::swUtil::removeBackground(oRgb, oDepth, 1.5);//, 5, cv::Vec3b(0,255,0 ));
    oDisplayDetectFace          = l_oRgbForeGround.clone();

    // detect face
        if(!m_CFaceDetectPtr->detect(l_oRgbForeGround))
        {
            if(m_oLastDetectedRectFace.width == 0)
            {
                std::cerr << "Face not detected. Head rigid motion cannot be computed. " << std::endl;
                return -1;
            }
            else
            {
                if(m_bVerbose)
                {
                    std::cout << "Face not detected. Use previous face rectangle. " << std::endl;
                }
                l_oFaceRectangle = m_oLastDetectedRectFace;
            }
        }
        else
        {
            l_oFaceRectangle = m_CFaceDetectPtr->faceRect();
            m_oLastDetectedRectFace = l_oFaceRectangle;
        }

        // DEBUG
//            std::cout << "detect face: " << (float)(clock() - l_oFirstTime) / CLOCKS_PER_SEC << std::endl;

    // compute nose tip
        int l_i32IdNoseX, l_i32IdNoseY;        
        cv::Point3f l_oNoseTip = m_CFaceDetectPtr->computeNoseTip(oDepth(l_oFaceRectangle), l_i32IdNoseX, l_i32IdNoseY);

        cv::Rect l_oNoseRectangle;
//        l_oNoseRectangle.x      =       l_oFaceRectangle.x + l_i32IdNoseX -30;
//        l_oNoseRectangle.y      = (int)(l_oFaceRectangle.y + l_oFaceRectangle.height*0.2);// + l_i32IdNoseY + l_oFaceRectangle.height*0.5;//-40;
//        l_oNoseRectangle.width  = 60;
//        l_oNoseRectangle.height = (int)(l_oFaceRectangle.height*0.6);//95;


        l_oNoseRectangle.x = l_oFaceRectangle.x + l_i32IdNoseX -30;
        l_oNoseRectangle.width  = 60;
        l_oNoseRectangle.y = l_oFaceRectangle.y + l_i32IdNoseY - 50;
        l_oNoseRectangle.height  = 70;

    // display
        if(swUtil::isInside(l_oFaceRectangle, oDisplayDetectFace))
        {
            cv::rectangle(oDisplayDetectFace, Point(l_oFaceRectangle.x, l_oFaceRectangle.y),
                cv::Point(l_oFaceRectangle.x+l_oFaceRectangle.width, l_oFaceRectangle.y+l_oFaceRectangle.height), RED,1);
        }

        if(swUtil::isInside(l_oNoseRectangle,oDisplayDetectFace))
        {
            cv::rectangle(oDisplayDetectFace, cv::Point(l_oNoseRectangle.x, l_oNoseRectangle.y),
                    cv::Point(l_oNoseRectangle.x + l_oNoseRectangle.width, l_oNoseRectangle.y + l_oNoseRectangle.height), GREEN,1);
        }

    // update rectangles returned by the getRect function
        m_oFaceRectToDisplay = l_oFaceRectangle;
        m_oNoseRectToDisplay = l_oNoseRectangle;

    // init face depth images
        cv::Mat l_oFaceDepth      = oDepth(l_oNoseRectangle);

    // create cloud
        SWCloud l_oFaceCloud;
        swCloud::convCloudMat2SWCloud(l_oFaceDepth, l_oFaceCloud, l_oNoseTip.z-0.10f, m_fDepthCloud + 0.10f, 0, 0, 255);
        uint l_ui32SizeCurrentFaceCloud = l_oFaceCloud.size();       

    // save reference cloud
        if(!m_bReferenceCloudInitialized)
        {            
            m_oFaceCloudRef.copy(l_oFaceCloud);
            m_ui32SizeFaceCloudRef = m_oFaceCloudRef.size();

            m_oFaceCloudRef.reduce(m_fAlignmentReductionCoeffTemplate);
            m_bReferenceCloudInitialized = true;

            return 0;
        }

    // check if the cloud is valid
        if(m_ui32SizeFaceCloudRef > 3 * l_ui32SizeCurrentFaceCloud)
        {
            std::cerr << "Input face cloud not valid, the cloud does not contain enough points. " << std::endl;
            oHeadRigidMotion = SWRigidMotion();
            return -1;
        }

    // apply previous good rigid motion
//        if(m_bApplyPreviousRigidMotion)
//        {
//            l_oFaceCloud.transform(m_oLastRigidMotion.m_aFRotation, m_oLastRigidMotion.m_aFTranslation);
//        }

    // align the clouds
        m_oAlignClouds.setClouds(m_oFaceCloudRef, l_oFaceCloud);
        m_oAlignClouds.setCloudDownscale(1.f, m_fAlignmentReductionCoeffTarget);
        m_oAlignClouds.alignClouds();

        SWCloud l_oTransformedFaceCloud;
        l_oTransformedFaceCloud.copy(l_oFaceCloud);
        m_oAlignClouds.transformedCloud(l_oTransformedFaceCloud);

    // update display clouds
        m_oDisplayFaceCloud.copy(l_oFaceCloud);
        m_oDisplayTransformedFaceCloud.copy(l_oTransformedFaceCloud);

        float l_fScore = m_oFaceCloudRef.squareDistanceCloud(l_oTransformedFaceCloud, true, m_fScoreReductionCoeff);

        if(m_bVerbose)
        {
            std::cout << "Score : " << l_fScore << " / " << m_fDistMax << std::endl;
        }

        if(l_fScore > m_fDistMax)
        {
            oHeadRigidMotion = SWRigidMotion();
            std::cerr << "Alignment score is too bad, head rigid reinitialized. Score : "<< l_fScore << " , max : " << m_fDistMax << std::endl;
            return -1;
        }
        else
        {
            oHeadRigidMotion       = m_oAlignClouds.rigidMotion();
            m_oLastRigidMotion     = oHeadRigidMotion;
        }

    return 1;
}

void SWCaptureHeadMotion::reset()
{
    m_bReferenceCloudInitialized = false;
    m_oLastRigidMotion = SWRigidMotion();
    m_oLastDetectedRectFace = cv::Rect();

    m_oFaceCloudRef.erase();
    m_oDisplayFaceCloud.erase();
    m_oDisplayTransformedFaceCloud.erase();
}

void SWCaptureHeadMotion::setParameters(cdouble dTemplateCoeffReduc, cdouble dTargetCoeffReduc, cdouble dScoreComputingReduc,
                                        cint i32KSmooth, cdouble dKTransSmooth, cdouble dKRotSmooth,
                                        cdouble dP2, cdouble dINF, cdouble dFactor, cdouble dD02)
{
    m_fAlignmentReductionCoeffTemplate  = (float)dTemplateCoeffReduc;
    m_fAlignmentReductionCoeffTarget    = (float)dTargetCoeffReduc;
    m_fScoreReductionCoeff              = (float)dScoreComputingReduc;

    m_oAlignClouds.setSmoothingParams(i32KSmooth, (float)dKTransSmooth, (float)dKRotSmooth);

    if(dP2 > 0.)
    {
        m_oAlignClouds.setEmicpParams((float)dP2, (float)dINF, (float)dFactor, (float)dD02);
    }
}

void SWCaptureHeadMotion::getRect(cv::Rect &oFaceRect, cv::Rect &oNoseRect)
{
    oFaceRect = m_oFaceRectToDisplay;
    oNoseRect = m_oNoseRectToDisplay;
}

//cv::Point3f SWCaptureHeadMotion::computeNoseTip(cv::Mat &oFaceDepth, int &idX, int &idY)
//{
//    float l_fMinDist = FLT_MAX;
//    cv::Point3f l_oNoseTip;

//    // retrieve the nose part of the depth image
//    for(int ii = oFaceDepth.rows/2; ii < oFaceDepth.rows; ++ii)
//    {
//        for(int jj = 0; jj < oFaceDepth.cols; ++jj)
//        {
//            float l_fCurrDepth = oFaceDepth.at<cv::Vec3f>(ii,jj)[2]; // retrieve current depth

//            if(l_fCurrDepth == 0)
//            {
//                // if null value of to the next value
//                continue;
//            }

//            if(l_fCurrDepth < l_fMinDist)
//            {
//                l_fMinDist = l_fCurrDepth;
//                idX = jj;
//                idY = ii;
//            }
//        }
//    }

//    l_oNoseTip = oFaceDepth.at<cv::Vec3f>(idY,idX);

//    return l_oNoseTip;
//}


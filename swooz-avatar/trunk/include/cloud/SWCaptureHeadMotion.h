
/**
 * \file SWCaptureHeadMotion.h
 * \brief defines SWCaptureHeadMotion
 * \author Florian Lance
 * \date 11/02/13
 */

#ifndef _SWCAPTUREHEADMOTION_
#define _SWCAPTUREHEADMOTION_

#include "devices/rgbd/SWKinectParams.h"
#include "cloud/SWAlignClouds.h"
#include "detect/SWFaceDetection.h"


typedef boost::shared_ptr<swDetect::SWFaceDetection> SWFaceDetectionPtr;	/**< boost shared pointer for SWFaceDetection */

namespace swCloud
{	
	/**
	 * \class SWCaptureHeadMotion
     * \brief Capture Head motion from rgbd device using haarcascade for face detection and emicp to compute the head
     * rigid motion.
	 * \author Florian Lance
	 * \date 11/02/13
	 */	
	class SWCaptureHeadMotion
	{	
		// ############################################# CONSTRUCTORS / DESTRUCTORS
	
		public :
			
			/**
			 * \brief constructor of SWCaptureHeadMotion	
             * \param [in] fAlignmentReducCoeff : reduction coeff to be applied on the 2 clouds before alignement (0-1)
             * \param [in] fScoreReducCoeff     : reduction coeff to be applied on the 2 clouds before computing score (0-1)
			 */			
            SWCaptureHeadMotion(cfloat fAlignmentReducCoeff = 0.3f, cfloat fScoreReducCoeff = 0.1f);


        // ############################################# METHODS
			
            /**
             * \brief Compute the head rigid motion.
             *  The first call will init the reference cloud
             *
             * \param [out] oRigidMotion        : result compujted head rigid motion
             * \param [in] oRgb                 : input rgb image
             * \param [in] oDepth               : input depth image
             * \param [out] oDisplayDetectFace  : face detection display rgb image
             * \param [in] oNoseTip             : nose tip
             * \return -1 : error : face not detected or bad alginment score (oHeadRigidMotion not valid)
             *          0 : first frame (no results),
             *          1 : sucess (oHeadRigidMotion is valid)
             */
            int computeHeadMotion(SWRigidMotion &oHeadRigidMotion, const cv::Mat &oRgb, const cv::Mat &oDepth, cv::Mat &oDisplayDetectFace,
                                  cv::Point3f oNoseTip = cv::Point3f(0.f,0.f,0.f));

			/**
             * \brief reset reference cloud
			 */			
            void reset();

            /**
             * @brief setParameters
             * @param [in] dTemplateCoeffReduc  : template cloud coefficient reduction
             * @param [in] dTargetCoeffReduc    : target cloud coefficient reduction
             * @param [in] dScoreComputingReduc : sclore computing reduction
             * @param [in] i32KSmooth           : number of previous computed rigidMotion used for temporal filtering on the current result (if == 0, the result is not smoothed)
             * @param [in] dKTransSmooth        : emipiric value used to compute the translation smoothing for the rigid motion
             * @param [in] dKRotSmooth          : emipiric value used to compute the rotation smoothing for the rigid motion
             * @param [in] dP2                  : emicp : P2 (do no modify emicp parameters if you don't know how emicp works, native parameters are good for a large panel of usages)
             * @param [in] dINF                 : emicp : INF
             * @param [in] dFactor              : emicp : Factor
             * @param [in] dD02                 : emicp : D02
             */
            void setParameters(cdouble dTemplateCoeffReduc, cdouble dTargetCoeffReduc, cdouble dScoreComputingReduc,
                               cint i32KSmooth = 5, cdouble dKTransSmooth = 50., cdouble dKRotSmooth = 50.,
                               cdouble dP2 = -1., cdouble dINF = -1., cdouble dFactor = -1., cdouble dD02 = -1.);
						
            /**
             * @brief Return the rectangle used for the alignment computing
             * @param [out] oFaceRect : face rectangle
             * @param [out] oNoseRect : nose rectangle
             */
            void getRect(cv::Rect &oFaceRect, cv::Rect &oNoseRect);


            // DEBUG
                swCloud::SWCloud debugFaceCloudRef()const{return m_oFaceCloudRef;}
                swCloud::SWCloud debugFaceCloud()const{return m_oDisplayFaceCloud;}
                swCloud::SWCloud debugTransformedFaceCloud()const{return m_oDisplayTransformedFaceCloud;}

			
		private :


//            cv::Point3f computeNoseTip(cv::Mat &oFaceDepth, int &idX, int &idY);

            bool m_bVerbose;                            /**< ... */
            bool m_bApplyPreviousRigidMotion;           /**< ... */
            bool m_bReferenceCloudInitialized;          /**< is the reference cloud initalized */

            uint m_ui32SizeFaceCloudRef;                /**< size reference size cloud, used to check validity of next input clouds */

            float m_fDistMax;                           /**< maximum distance bewteen 2 clouds to keep the current one */
            float m_fDepthCloud;                        /**< depth value of the clouds */
            float m_fScoreReductionCoeff;               /**< reduction coeff to be applied on the 2 clouds before computing score, lower is faster (x -> we keep only x% of the clouds points) */
            float m_fAlignmentReductionCoeffTarget;     /**<  reduction coeff to be applied on the target cloud, lower is faster (x -> we keep only x% of the clouds points) */
            float m_fAlignmentReductionCoeffTemplate;   /**<  reduction coeff to be applied on the template cloud, lower is faster (x -> we keep only x% of the clouds points) */

            cv::Rect m_oFaceRectToDisplay;              /**< face rectangle returned by getRect */
            cv::Rect m_oNoseRectToDisplay;              /**< nose rectangle returned by getRect */
            cv::Rect m_oLastDetectedRectFace;           /**< last detected face rectangle */

            swCloud::SWRigidMotion m_oLastRigidMotion;

            swCloud::SWCloud m_oFaceCloudRef;           /**< reference face cloud */
            swCloud::SWCloud m_oDisplayFaceCloud;
            swCloud::SWCloud m_oDisplayTransformedFaceCloud;

            SWAlignClouds m_oAlignClouds;
            SWFaceDetectionPtr m_CFaceDetectPtr;        /**< detect face pointer */
	};
}
		

#endif	


/**
 * \file SWFaceDetection.h
 * \brief defines SWFaceDetection
 * \author Florian Lance
 * \date 04/12/12
 */

#ifndef _SWFACEDETECTIONS_
#define _SWFACEDETECTIONS_

#include "boost/shared_ptr.hpp"
#include "detect/SWHaarCascade.h"

#include "opencvUtility.h"

typedef boost::shared_ptr<swDetect::SWHaarCascade> SWHaarCascadePtr;	/**< boost shared pointer for SWHaarCascade */

namespace swDetect
{

	/**
	 * \class SWFaceDetection
     * \brief Detects the face in a cv::Mat image.
	 * \author Florian Lance
	 * \date 04/12/12
	 */
    class SWFaceDetection
	{	
		public:
		
			// ############################################# CONSTRUCTORS / DESTRUCTORS
		
			/**
			 * \brief constructor of SWFaceDetection
			 * \param [in] oMinDetectFaceSize :  min size of a detected face
             * \param [in] bVerbose           :  display verbose info
             * \param [in] sClassifierFilePath:  haar cascade classifier file path
			 */
            SWFaceDetection(const cv::Size &oMinDetectFaceSize = cv::Size(80,80), cbool bVerbose = false,
                            std::string sClassifierFilePath = std::string("../data/classifier/haarcascade_frontalface_alt2.xml"));
		
		
			// ############################################# METHODS
			
            /**
             * \brief Launch the detection
             * \param [in] oRgbImg : input rgb image
             * \return if no detection return false, else return true
             */
            bool detect(const cv::Mat &oRgbImg);

            /**
             * \brief get the face detection
             * \return the rectangle of the detection
             */
            cv::Rect faceRect() const;

            /**
             * @brief Display the face detection on the input mat image
             * @param [in,out] oRgbImg : input image
             */
            void displayFace(cv::Mat &oRgbImg, cv::Scalar oColor = RED);


            /**
             * @brief computeNoseTip
             * @param oFaceDepth
             * @param [out] idX : nose tip X position in oFaceDepth
             * @param [out] idY : nose tip Y position in oFaceDepth
             * @return noseTip
             */
            cv::Point3f computeNoseTip(cv::Mat &oFaceDepth, int &idX, int &idY);

            /**
             * @brief setRectRatios
             * @param [in] fWidthRatio  : width rectangle ratio
             * @param [in] fHeightRatio : height rectangle ratio
             */
            void setRectRatios(cfloat fWidthRatio, cfloat fHeightRatio);

        private :

            /**
             * \brief Detect one face with haar cascade
             * \param [in] oRgbImg : input rgb image
             * \return if no face detected or haar cascade not files not loaded return false, else return true
             */
            bool detectFace(const cv::Mat &oRgbImg);

				
		private:


            bool m_bVerbose;                        /**< display verbose info */
            bool m_bHaarCascadeFilesLoaded;         /**< are the haar cascade files loaded */
				
            int m_i32WidthRectFaceOffset;           /**< width offset for the face rectangle */
            int m_i32HeightRectFaceOffset;          /**< height offset for the face rectangle */
		
            float m_fWidthRatioImageToDetect;       /**< width detection ratio of the image */
            float m_fHeightRatioImageToDetect;      /**< height detection ratio of the image */

            float m_fFaceHeightRatio;               /**< face rectangle height ratio */
            float m_fFaceWidthRatio;                /**< face rectnagle width ratio */
			
            cv::Size    m_oMinDetectFaceSize;       /**< minimum size of the detected faces */

            cv::Rect m_oLastDetectFace;             /**< last detected face rectangle */
            std::vector<cv::Rect> m_oRects;         /**< vector of rectangles, will contain the haar cascade detection result */

            SWHaarCascadePtr m_CHaarCascadeFacePtr; /**< face haar cascade */
	};
}

#endif


//cv::Point3f m_oNoseTip; 	   	/**< detected nose tip */
//cv::Point3f m_oLastDetectedNoseTip; 	/**< last detected nose tip */
//			uint m_i32FaceHeightOffset;    		/**< offset to apply to the face height */
//			bool m_bNoseDetected; 	      		/**< is the nose detected ? */

//uint8 m_ui8MaskDetection;      		/**< detection mask */
/**
 * \brief Set the ratio value of the height size
 * \param [in] fRatio : ratio to apply
 */
//			static void setFaceHeightRatio(cfloat fRatio);



/**
 * \brief Compute upper face
 * \param [in] oFaceRect 	: input face rectangle
 * \param [in] oNoseTip	 	: the current nose tip
 * \return the computed upper face part rectangle
 */
//			cv::Rect computeUpperFace(const cv::Rect &oFaceRect, const cv::Point3f oNoseTip);

/**
 * \brief Detect the nose with haar cascade
 * \param [in] oRgbImg : input rgb image
 * \return if no nose detected return false, else return true
 */
//			bool detectNose(const cv::Mat &oRgbImg);

/**
 * \brief Call computeNoseTip with the appropriate depth image.
 * \param [in] oDepthImg : input depth image corresponding to the rgb image where has occured the detection
 * \return true if the nose tip is detected
 */
//			bool detectNoseTip(const cv::Mat &oDepthImg);

/**
 * \brief detect the nose tip by looking for the closest point
 * \param [in] oDepthImg : input depth image where the nose tip will be searched
 * \return the computed nose tip in the input face mat
 */
//			cv::Point3f computeNoseTip(const cv::Mat &oDepthImg);



/**
 * \brief get the middle of the face rectangle detection
 * \return the point of the middle
 */
//			cv::Point middleFacePoint();

/**
 * \brief get last detected nose tip
 * \return the point containing the coordinates of the nose tip
 */
//			cv::Point3f noseTip();

/**
 * \brief display all the detections
 * \param [in,out] oRgbImg   :	 input rgb image
 */
//			void displayAllDetections(cv::Mat &oRgbImg);



/**
 * \brief detect the nose tip z value (fast)
 * \param [in] oDepthImg : input depth image
 * \return the nose tip distance
 */
//			float retrieveNoseTipZDistance(const cv::Mat &oDepthImg);


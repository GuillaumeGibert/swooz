/**
 * \file SWFaceDetection_thread.h
 * \brief defines SWFaceDetection_thread
 * \author Florian Lance
 * \date 04/12/12
 */


#ifndef _SWFACEDETECTION_THREAD_
#define _SWFACEDETECTION_THREAD_

#include "SWFaceDetection.h"

#include "boost/thread.hpp"

namespace swDetect
{
    /**
     * \class SWFaceDetection_thread
     * \brief ...
     * \author Florian Lance
     * \date 05/05/14
     */
    class SWFaceDetection_thread
    {
        public :

        SWFaceDetection_thread(const cv::Size &oMinDetectFaceSize = cv::Size(80,80), cbool bVerbose = false,
                               std::string sClassifierFilePath = std::string("../data/classifier/haarcascade_frontalface_alt2.xml"));

        SWFaceDetection_thread(const cv::Size &oMinDetectFaceSize, const cv::Size &oMaxDetectFaceSize, cbool bVerbose,
                               std::string sClassifierFilePath);

        ~SWFaceDetection_thread();

        void startDetection();

        void stopDetection();

        void setNewRGB(const cv::Mat &oRGB);


        cv::Rect getLastRect();

        /**
         * @brief Display the face detection on the input mat image
         * @param [in,out] oRgbImg : input image
         */
        void displayFace(cv::Mat &oRgbImg, cv::Scalar oColor = RED);


        private :


        void doWork();


        SWFaceDetection *m_pFaceDetection;


        bool m_bListening;	/**< is the listening thread launched ? */

        boost::shared_ptr<boost::thread> m_pThread;	/**<  thread */
        boost::mutex m_oMutex;					/**< mutex */

        cv::Mat m_oRGB;
        bool m_bIsNewRGBAvailable;

    };

}




#endif

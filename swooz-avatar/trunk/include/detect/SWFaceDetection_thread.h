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

        /**
         * @brief SWFaceDetection_thread
         * @param oMinDetectFaceSize
         * @param bVerbose
         * @param sClassifierFilePath
         */
        SWFaceDetection_thread(const cv::Size &oMinDetectFaceSize = cv::Size(80,80), cbool bVerbose = false,
                               std::string sClassifierFilePath = std::string("../data/classifier/haarcascade_frontalface_alt2.xml"));

        /**
         * @brief SWFaceDetection_thread
         * @param oMinDetectFaceSize
         * @param oMaxDetectFaceSize
         * @param bVerbose
         * @param sClassifierFilePath
         */
        SWFaceDetection_thread(const cv::Size &oMinDetectFaceSize, const cv::Size &oMaxDetectFaceSize, cbool bVerbose,
                               std::string sClassifierFilePath);

        ~SWFaceDetection_thread();

        /**
         * @brief startDetection
         */
        void startDetection();

        /**
         * @brief stopDetection
         */
        void stopDetection();

        /**
         * @brief setNewRGB
         * @param oRGB
         */
        void setNewRGB(const cv::Mat &oRGB);

        /**
         * @brief getLastRect
         * @return
         */
        cv::Rect getLastRect();

        /**
         * @brief Display the face detection on the input mat image
         * @param [in,out] oRgbImg : input image
         */
        void displayFace(cv::Mat &oRgbImg, cv::Scalar oColor = RED);


        private :

        /**
         * @brief doWork
         */
        void doWork();


        SWFaceDetection *m_pFaceDetection;      /**< ... */


        bool m_bListening;	/**< is the listening thread launched ? */

        boost::shared_ptr<boost::thread> m_pThread;	/**<  thread */
        boost::mutex m_oMutex;					/**< mutex */

        cv::Mat m_oRGB;             /**< ... */
        bool m_bIsNewRGBAvailable;  /**< ... */

    };

}




#endif

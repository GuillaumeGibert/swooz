
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

#include "detect/SWFaceDetection_thread.h"

#include <time.h>


swDetect::SWFaceDetection_thread::SWFaceDetection_thread(const cv::Size &oMinDetectFaceSize, cbool bVerbose, std::string sClassifierFile)
    : m_bListening(false), m_bIsNewRGBAvailable(false)
{
    m_pFaceDetection = new swDetect::SWFaceDetection(oMinDetectFaceSize, bVerbose, sClassifierFile);
}

swDetect::SWFaceDetection_thread::SWFaceDetection_thread(const cv::Size &oMinDetectFaceSize, const cv::Size &oMaxDetectFaceSize, cbool bVerbose, std::string sClassifierFile) :
    m_bListening(false), m_bIsNewRGBAvailable(false)
{
    m_pFaceDetection = new swDetect::SWFaceDetection(oMinDetectFaceSize, oMaxDetectFaceSize, bVerbose, sClassifierFile);
}

swDetect::SWFaceDetection_thread::~SWFaceDetection_thread()
{
    if(m_bListening)
    {
        stopDetection();
    }

    deleteAndNullify(m_pFaceDetection);
}


void swDetect::SWFaceDetection_thread::startDetection()
{
    if(!m_bListening)
    {
        m_bListening= true;
        m_pThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&SWFaceDetection_thread::doWork, this)));
    }
}

void swDetect::SWFaceDetection_thread::stopDetection()
{
    if(m_bListening)
    {
        m_bListening = false;
        m_pThread->join();
    }
}

void swDetect::SWFaceDetection_thread::setNewRGB(const cv::Mat &oRGB)
{
    boost::lock_guard<boost::mutex> lock(m_oMutex);

    if(m_bListening)
    {
        m_oRGB = oRGB.clone();
        m_bIsNewRGBAvailable = true;
    }
}

cv::Rect swDetect::SWFaceDetection_thread::getLastRect()
{
    boost::lock_guard<boost::mutex> lock(m_oMutex);

    return m_pFaceDetection->faceRect();
}

void swDetect::SWFaceDetection_thread::displayFace(cv::Mat &oRgbImg, cv::Scalar oColor)
{
    cv::Rect faceRect = getLastRect();

    if(faceRect.width > 0)
    {
        if(swUtil::isInside(faceRect, oRgbImg))
        {
            cv::rectangle(oRgbImg, cv::Point(faceRect.x, faceRect.y),
                cv::Point(faceRect.x+faceRect.width, faceRect.y+faceRect.height), oColor,1);
        }
    }
}

void swDetect::SWFaceDetection_thread::doWork()
{
    while(m_bListening)
    {
        {
//            clock_t time = clock();


            boost::lock_guard<boost::mutex> lock(m_oMutex);
//            std::cout << "|" <<(((float)(clock() - time) / CLOCKS_PER_SEC)) << " ";

            if(m_bIsNewRGBAvailable)
            {
                m_pFaceDetection->detectFace(m_oRGB);
                m_bIsNewRGBAvailable = false;
//                std::cout << (((float)(clock() - time) / CLOCKS_PER_SEC)) << " ";
            }
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(30));
    }
}


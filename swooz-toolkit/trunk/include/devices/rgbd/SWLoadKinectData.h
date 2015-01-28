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
 * \file SWLoadKinectData.h
 * \brief Defines SWLoadKinectData
 * \author Florian Lance
 * \date 26/06/13
 */


#ifndef _SWLOADKINECTDATA_
#define _SWLOADKINECTDATA_

// UTILITY
#include <string>
#include <fstream>
//#include <ostream>
#include <boost/iostreams/device/mapped_file.hpp>

// OPENCV
#include "opencvUtility.h"
#include "opencv2/highgui/highgui.hpp"

// SWOOZ
#include "SWExceptions.h"


namespace swDevice // TODO : document data format
{
    /**
     * \class SWLoadKinectData
     * \brief This class allows to load kinect data in realtime. (see SWSaveKinectData)
     *
     */
    class SWLoadKinectData
    {
        public :

            /**
             * \brief SWLoadKinectData constructor.
             * \param [in] sLoadingPath : path containing the kinect data to be loaded.
             */
            SWLoadKinectData(const std::string &sLoadingPath);

            /**
             * \brief SWLoadKinectData destructor.
             */
            ~SWLoadKinectData();

            /**
             * \brief  Start the loading (files opened according to input booleans parameters).
             */
            void start();

            /**
             * \brief Stop the loading (mapped data files closed).
             */
            void stop();

            /**
             * \brief Save video input data.
             * \param [out] oBgr   : rgb image mat (cv::Vec3b)
             * \return false it the loading is ended, else return true
             */
            bool grabVideo(cv::Mat &oBgr);

            /**
             * \brief Save cloud input data.
             * \param [out] oCloud : cloud point mat (cv::Vec3f)
             * \return false it the loading is ended, else return true
             */
            bool grabCloud(cv::Mat &oCloud);


        private :

            bool m_bInit;                    /**< is the data initialized ? */
            bool m_bIsCloudData;             /**< is cloud data available ?*/
            bool m_bIsVideoData;             /**< is video data available ? */

            std::string m_sLoadingPath;  	 /**< loading path for mapped files */

            cv::VideoCapture m_oVideoCapture;/**< video capture */

            // mapped files parameters
            //      numbers
            int m_i32NumPoint;               /**< current number of saved cloud points */
            int m_i32NumFile;                /**< current number of data and index mapped file */
            int m_i32NumCloud;               /**< current number of saved cloud */
            int m_i32NumFrame;               /**< num of the current frame */
            //      params files
            boost::iostreams::mapped_file_params m_oCloudDataFileParams;        /**< cloud data mapped param file  */
            boost::iostreams::mapped_file_params m_oCloudIndexFileParams;       /**< cloud index mapped param file  */
            boost::iostreams::mapped_file_params m_oCloudHeaderFileParams;      /**< cloud header mapped param file */
            boost::iostreams::mapped_file_params m_oCloudSizeFileParams;        /**< cloud size mapped param file  */
            boost::iostreams::mapped_file_params m_oCloudTimeKinectFileParams;  /**< cloud time mapped param file  */
            //      sink files
            boost::iostreams::mapped_file_source m_oCloudDataFile;                /**< cloud data mapped file  */
            boost::iostreams::mapped_file_source m_oCloudIndexFile;               /**< cloud index mapped file  */
            boost::iostreams::mapped_file_source m_oCloudSizeFile;                /**< cloud header mapped file */
            boost::iostreams::mapped_file_source m_oCloudTimeKinectFile;          /**< cloud size mapped file  */
            boost::iostreams::mapped_file_source m_oCloudHeaderFile;              /**< cloud time mapped file  */
            //      data pointers
            int     *m_i32HeaderNumber;     /**< cloud head number pointer */
            int     *m_oCloudSize;          /**< cloud size number pointer */
            int     *m_oCloudIndex;     	/**< cloud index number pointer */
            float   *m_oCloudData;          /**< cloud data number pointer */
            float   *m_oCloudTimeKinect;    /**< cloud time number pointer */

    };

};

#endif

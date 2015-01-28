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
 * \file SWSaveKinectData.h
 * \brief Defines SWSaveKinectData
 * \author Florian Lance
 * \date 24/06/13
 */

#ifndef _SWSAVEDEPTHCAPTUREDEVICEDATA_
#define _SWSAVEDEPTHCAPTUREDEVICEDATA_


// UTILITY
#include <string>
#include <time.h>
#include <boost/iostreams/device/mapped_file.hpp>

// OPENCV
#include "opencvUtility.h"
#include "opencv2/highgui/highgui.hpp"

// SWOOZ
#include "SWExceptions.h"


namespace swDevice
{
	/**
	 * \class SWSaveKinectData
	 * \brief This class allows to save kinect data in realtime.
	 *  
     *  Video kinect data is saved in an avi file and cloud kinect data in mapped files.
	 */	
    class SWSaveKinectData // TODO : document data format
	{

		public :
			
			/**
			 * \brief SWSaveKinectData constructor.
			 * \param [in] sSavingPath 	: path where the data will be saved
			 * \param [in] dMaxLenght 	: maximum length of the saving
             * \param [in] dMaxSize 	: maximum size of the saving in Go (counting all mapped files)
             * \param [in] dMinDist 	: minimum distance for a point of the input kinect cloud to be saved
             * \param [in] dMaxDist 	: maximum distance for a point of the input kinect cloud to be saved
			 */
            SWSaveKinectData(const std::string &sSavingPath, const double dMaxLength = 60.0, const double dMaxSize = 20, const double dMinDist = 0.4f, const double dMaxDist = 2.f);
		
			/**
			 * \brief SWSaveKinectData destructor.
			 */		
			~SWSaveKinectData();
		
			/**
             * \brief Start the saving (files created according to input booleans parameters).
             * \param [in] bSaveVideoData 	: is the video data will be saved ?
             * \param [in] bSaveCloudData 	: is the cloud data will be saved ?
			 */			
            void start(const bool bSaveVideoData = true, const bool bSaveCloudData = true);
		
            /**
             * \brief Set the start time.
             * \param [in] oStartTime 	: program time to be redefined
             */
            void setStartTime(const clock_t oStartTime);

			/**
			 * \brief Save the input data.
             * \param [in] oData1 	: cloud point mat (cv::Vec3f) or bgr image mat (cv::Vec3b)
             * \param [in] oData2 	: cloud point mat (cv::Vec3f) or bgr image mat (cv::Vec3b)
             * \return false it the recording is ended, else return true
             *
             * In the parameters we must have a cloud mat and a rgb image mat, but the order doesn't matter.
             */
            bool save(const cv::Mat &oData1, const cv::Mat &oData2);
		
            /**
             * \brief Save the input data.
             * \param [in] oData 	: cloud point mat (cv::Vec3f) or rgb image mat (cv::Vec3b)
             * \return false it the recording is ended, else return true
             */
            bool save(const cv::Mat &oData);

			/**
			 * \brief Stop the saving (mapped data files closed).
			 */			
			void stop();


        private :

            /**
             * \brief Save video input data.
             * \param [in] oBgr : rgb image mat (cv::Vec3b)
             */
            void saveVideo(const cv::Mat &oBgr);

            /**
             * \brief Save cloud input data.
             * \param [in] oCloud 	: cloud point mat (cv::Vec3f)
             */
            void saveCloud(const cv::Mat &oCloud);

		
		private :
			
            bool m_bInit;                   /**< is the data initialized ? */
            bool m_bSaveVideoData;          /**< is the video data will be saved ? */
            bool m_bSaveCloudData;          /**< is the cloud data will be saved ? */		

            double m_dMaxSize;              /**< maximum size of the saving (counting all mapped files) */
            double m_dMaxLength;            /**< maximum length of the saving */
            double m_dMinDist;              /**< minimum distance for a point of the input kinect cloud to be saved */
            double m_dMaxDist;              /**< maximum distance for a point of the input kinect cloud to be saved */
		
            std::string m_sSavingPath;  	/**< saving path for mapped files */

            clock_t m_oProgramTime;         /**< starting time of the record */

            // opencv writer parameters
            cv::VideoWriter m_oVideoWriter;	/**< video writer for saving kinect rgb data */

            // mapped files parameters
            //      numbers
            int m_i32NumPoint;                      /**< current number of saved cloud points */
            int m_i32NumFile;                       /**< current number of data and index mapped file */
            int m_i32NumCloud;                      /**< current number of saved cloud */
            int m_i32CurrentDataSizeWritten;        /**< current size already written in the data mapped file */
            int m_i32CurrentIndexSizeWritten;       /**< current size already written in the index mapped file */
            int m_i32NumFrame;                      /**< total number of saved cloud */
            long long m_lCurrentTotalSizeWritten;   /**< total current size already written in the data mapped file */
            //      params files
            boost::iostreams::mapped_file_params m_oCloudDataFileParams;        /**< cloud data mapped param file  */
            boost::iostreams::mapped_file_params m_oCloudIndexFileParams;       /**< cloud index mapped param file  */
            boost::iostreams::mapped_file_params m_oCloudHeaderFileParams;      /**< cloud header mapped param file */
            boost::iostreams::mapped_file_params m_oCloudSizeFileParams;        /**< cloud size mapped param file  */
            boost::iostreams::mapped_file_params m_oCloudTimeKinectFileParams;  /**< cloud time mapped param file  */
            //      sink files
            boost::iostreams::mapped_file_sink m_oCloudDataFile;                /**< cloud data mapped file  */
            boost::iostreams::mapped_file_sink m_oCloudIndexFile;               /**< cloud index mapped file  */
            boost::iostreams::mapped_file_sink m_oCloudSizeFile;                /**< cloud header mapped file */
            boost::iostreams::mapped_file_sink m_oCloudTimeKinectFile;          /**< cloud size mapped file  */
            boost::iostreams::mapped_file_sink m_oCloudHeaderFile;              /**< cloud time mapped file  */
            //      data pointers
            int     *m_i32HeaderNumber;     /**< cloud head number pointer */
            int     *m_oCloudSize;          /**< cloud size number pointer */
            int     *m_oCloudIndex;     	/**< cloud index number pointer */
            float   *m_oCloudData;          /**< cloud data number pointer */
            float   *m_oCloudTimeKinect;    /**< cloud time number pointer */
	};
};

#endif

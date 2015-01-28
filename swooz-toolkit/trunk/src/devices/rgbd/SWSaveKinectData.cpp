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
 * \file SWSaveKinectData.cpp
 * \brief Defines SWSaveKinectData
 * \author Florian Lance
 * \date 25/06/13
 */

#include "devices/rgbd/SWSaveKinectData.h"

using namespace swDevice;

SWSaveKinectData::SWSaveKinectData(const std::string &sSavingPath, const double dMaxLength, const double dMaxSize, const double dMinDist, const double dMaxDist) :
    m_bInit(false), m_sSavingPath(sSavingPath), m_dMaxLength(dMaxLength), m_dMaxSize(dMaxSize), m_dMinDist(dMinDist), m_dMaxDist(dMaxDist)
{
    m_i32NumFile  = 0;
    m_i32NumPoint = 0;
    m_i32NumFile  = 0;
    m_i32NumCloud = 0;
    m_i32NumFrame = 0;
    m_i32CurrentDataSizeWritten  = 0;
    m_i32CurrentIndexSizeWritten = 0;
    m_lCurrentTotalSizeWritten   = 0;

    // ########################################### mapped file sizes

    // 351 mo for data file / 117 for index file -> theses values can be reduced if crashs arise
    m_oCloudIndexFileParams.new_file_size     = 30000000 *     sizeof(int);     // equiv. 40s, 30 frames/s, 25k points, 1 int
    m_oCloudDataFileParams.new_file_size      = 30000000 * 3 * sizeof(float);   // equiv. 40s, 30 frames/s, 25k points, 3 float
    m_oCloudSizeFileParams.new_file_size      = 500000   *     sizeof(int);     // equiv. 1 size/cloud during 4 hours at 30 fps
    m_oCloudTimeKinectFileParams.new_file_size= 500000   *     sizeof(float);   // equiv. 1 time/cloud during 4 hours at 30 fps
    m_oCloudHeaderFileParams.new_file_size    =(1 + 300) *     sizeof(int);     // equiv. 1 int for stocking the number of files (max 300), 300 for the numbers of cloud for each file

    // ########################################### mapped file paths

    m_oCloudIndexFileParams.path     =   m_sSavingPath + "index0.raw";
    m_oCloudDataFileParams.path      =   m_sSavingPath + "points0.raw";
    m_oCloudSizeFileParams.path      =   m_sSavingPath + "size.raw";
    m_oCloudTimeKinectFileParams.path=   m_sSavingPath + "timeKinect.raw";
    m_oCloudHeaderFileParams.path    =   m_sSavingPath + "header.raw";

    // ########################################### mapped file pointers

    m_i32HeaderNumber   	= NULL;
    m_oCloudData 	    	= NULL;
    m_oCloudIndex           = NULL;
    m_oCloudTimeKinect      = NULL;
    m_oCloudSize            = NULL;
}

SWSaveKinectData::~SWSaveKinectData()
{
    stop();
}

void SWSaveKinectData::start(const bool bSaveVideoData, const bool bSaveCloudData)
{
    if(bSaveVideoData == false && bSaveVideoData == bSaveCloudData)
    {
        std::cerr << "Error parameters SWSaveKinectData::start, at least one type of data must be saved. " << std::endl;
        m_bInit = false;

        return;
    }

    m_bSaveVideoData = bSaveVideoData;
    m_bSaveCloudData = bSaveCloudData;

    try
    {
        // open opencv video
        if(m_bSaveVideoData)
        {
            // init video writer
            int l_i32Codec = CV_FOURCC('M', 'J', 'P', 'G');

            m_oVideoWriter.open(m_sSavingPath + "bgr.avi", l_i32Codec, 30.0, cv::Size(640,480), true);

            if(!m_oVideoWriter.isOpened())
            {
                std::cerr << "Fail init video writer. "<< std::endl;
                throw std::exception();
            }
        }

        if(m_bSaveCloudData)
        {
            try
            {
                // open mapped files
                m_oCloudHeaderFile.open     (m_oCloudHeaderFileParams);
                m_oCloudTimeKinectFile.open (m_oCloudTimeKinectFileParams);
                m_oCloudIndexFile.open      (m_oCloudIndexFileParams);
                m_oCloudDataFile.open       (m_oCloudDataFileParams);
                m_oCloudSizeFile.open       (m_oCloudSizeFileParams);

                //  init data mapped files pointers
                m_i32HeaderNumber   	= (int *)	m_oCloudHeaderFile.data();
                m_i32HeaderNumber[0] 	= 1;
                m_oCloudData 	    	= (float*)  m_oCloudDataFile.data();
                m_oCloudIndex           = (int*)    m_oCloudIndexFile.data();
                m_oCloudTimeKinect      = (float*)  m_oCloudTimeKinectFile.data();
                m_oCloudSize            = (int*)    m_oCloudSizeFile.data();

                m_oProgramTime = clock();
            }
            catch(boost::exception&)
            {
                m_bInit = false;
                std::cerr << "Fail opening boost mapped files. " << std::endl;
                throw std::exception();
            }
        }

        m_bInit = true;
    }
    catch(std::exception&)
    {
        std::cerr << "Initialization aborted. SWSaveKinectData::start " << std::endl;
        m_bInit = false;
    }
}

void SWSaveKinectData::setStartTime(const clock_t oStartTime)
{
    m_oProgramTime = oStartTime;
}

bool SWSaveKinectData::save(const cv::Mat &oData1, const cv::Mat &oData2)
{
    if(!m_bInit)
    {
        std::cerr << "Error : start must be called before SWSaveKinectData::save. Recording stopped. " << std::endl;
        stop();
        return false;
    }

    if(!(oData1.cols >= 640 && oData1.rows >= 480 && oData2.cols >= 640 && oData2.rows >= 480))
    {
        std::cerr << "Error : parameters mat size SWSaveKinectData::save. Recording stopped. " << std::endl;
        stop();
        return false;
    }

    if(oData1.depth() == CV_8U && oData2.depth() == CV_32F)
    {
        saveVideo(oData1);
        saveCloud(oData2);
    }
    else if(oData1.depth() == CV_32F && oData2.depth() == CV_8U)
    {
        saveVideo(oData2);
        saveCloud(oData1);
    }
    else
    {
        std::cerr << "Error : parameters mat depth SWSaveKinectData::save. Recording stopped. " << std::endl;
        stop();
        return false;
    }

    if(m_dMaxLength < (float)(clock() - m_oProgramTime)/ CLOCKS_PER_SEC)
    {
        std::cout << "Time's up, end of the recording." << std::endl;
        stop();
        return false;
    }

    if(m_dMaxSize <  m_lCurrentTotalSizeWritten / 1000000000.f)
    {
        std::cout << "Allowable size reached, end of the recording. " << std::endl;
        stop();
        return false;
    }

    return true;
}


bool SWSaveKinectData::save(const cv::Mat &oData)
{
    if(!m_bInit)
    {
        std::cerr << "Error : start must be called before SWSaveKinectData::save. Recording stopped. " << std::endl;
        stop();
        return false;
    }

    if(!(oData.cols >= 640 && oData.rows >= 480))
    {
        std::cerr << "Error : parameters mat size SWSaveKinectData::save. Recording stopped. " << std::endl;
        stop();
        return false;
    }

    if(oData.depth() == CV_8U)
    {
        saveVideo(oData);
    }
    else if(oData.depth() == CV_32F)
    {
        saveCloud(oData);
    }
    else
    {
        std::cerr << "Error : parameters mat depth SWSaveKinectData::save. Recording stopped. " << std::endl;
        stop();
        return true;
    }

    if(m_dMaxLength < (float)(clock() - m_oProgramTime)/ CLOCKS_PER_SEC)
    {
        std::cout << "Time's up, end of the recording." << std::endl;
        stop();
        return false;
    }

    if(m_dMaxSize <  m_lCurrentTotalSizeWritten/1000000000.f)
    {
        std::cout << "Allowable size reached, end of the recording. " << std::endl;
        stop();
        return false;
    }

    return true;
}

void SWSaveKinectData::saveVideo(const cv::Mat &oBgr)
{
    try
    {
        m_oVideoWriter << oBgr;
    }
    catch (const cv::Exception &e)
    {
        std::cerr << "SWSaveKinectData::saveVideo : " << e.what() << std::endl;
        stop();
    }
}

void SWSaveKinectData::saveCloud(const cv::Mat &oCloud)
{
    // save kinect time
    m_oCloudTimeKinect[m_i32NumFrame] = (float)(clock() - m_oProgramTime)/ CLOCKS_PER_SEC;

    // save cloud points
    int l_i32NumPointsToAdd = 0;
    for(int ii = 0; ii < oCloud.rows * oCloud.cols; ++ii)
    {
        cv::Point3f l_p3fCurrPoint = oCloud.at<cv::Point3f>(ii);

        if(l_p3fCurrPoint.z > m_dMinDist && l_p3fCurrPoint.z < m_dMaxDist)
        {
            m_oCloudData[3*(l_i32NumPointsToAdd+m_i32NumPoint)]   = l_p3fCurrPoint.x;
            m_oCloudData[3*(l_i32NumPointsToAdd+m_i32NumPoint)+1] = l_p3fCurrPoint.y;
            m_oCloudData[3*(l_i32NumPointsToAdd+m_i32NumPoint)+2] = l_p3fCurrPoint.z;

            m_oCloudIndex[l_i32NumPointsToAdd + m_i32NumPoint]      = ii;
            ++l_i32NumPointsToAdd;
        }
    }

    // save cloud size
    m_oCloudSize[m_i32NumFrame] = l_i32NumPointsToAdd;

    // update mapped files parameters
    m_i32NumPoint += l_i32NumPointsToAdd;
    ++m_i32NumCloud;
    ++m_i32HeaderNumber[m_i32NumFile + 1];
    ++m_i32NumFrame;

    int l_i32CurrentDataSize      = 3 * sizeof(float) * l_i32NumPointsToAdd;
    int l_i32CurrentIndexSize     =     sizeof(int)   * l_i32NumPointsToAdd;
    m_i32CurrentDataSizeWritten  += l_i32CurrentDataSize;
    m_i32CurrentIndexSizeWritten += l_i32CurrentIndexSize;
    m_lCurrentTotalSizeWritten   += l_i32CurrentDataSize + l_i32CurrentIndexSize;

    // check if new mapped files must be created
    if((m_i32CurrentDataSizeWritten  + 5 * l_i32CurrentDataSize   > m_oCloudDataFileParams.new_file_size) ||
       (m_i32CurrentIndexSizeWritten + 5 * l_i32CurrentIndexSize > m_oCloudIndexFileParams.new_file_size))
    {
        std::cout << "Mapped file num " << m_i32NumFile++ << " full, create a new file. " << std::endl;

        // reset numbers of points and index and data/index sizes
        m_i32NumPoint = 0;
        m_i32NumCloud = 0;
        m_i32CurrentDataSizeWritten  = 0;
        m_i32CurrentIndexSizeWritten = 0;

        // set num file string
        std::ostringstream l_osNumFile;
        l_osNumFile.str("");
        l_osNumFile.clear();
        l_osNumFile << m_i32NumFile;

        // close mapped files
        m_oCloudDataFile.close();
        m_oCloudIndexFile.close();

        // set the new path
        m_oCloudDataFileParams.path  = m_sSavingPath + "points" + l_osNumFile.str() + ".raw";
        m_oCloudIndexFileParams.path = m_sSavingPath + "index"  + l_osNumFile.str() + ".raw";

        ++m_i32HeaderNumber[0];

        try
        {
            m_oCloudDataFile.open (m_oCloudDataFileParams);
            m_oCloudIndexFile.open(m_oCloudIndexFileParams);
            m_oCloudData  = (float*) m_oCloudDataFile.data();
            m_oCloudIndex = (int*)   m_oCloudIndexFile.data();
        }
        catch(boost::exception&)
        {
            std::cerr << "Fail opening new boost mapped files. Recording stopped.  " << std::endl;
            stop();
        }
    }
}

void SWSaveKinectData::stop()
{
    if(m_bInit)
    {
        if(m_bSaveCloudData)
        {
            m_oCloudHeaderFile.close();
            m_oCloudTimeKinectFile.close();
            m_oCloudIndexFile.close();
            m_oCloudDataFile.close();
            m_oCloudSizeFile.close();
        }

        m_bInit = false;
    }
}

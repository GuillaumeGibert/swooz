
/**
 * \file SWLoadKinectData.cpp
 * \brief Defines SWLoadKinectData
 * \author Florian Lance
 * \date 26/06/13
 */

#include "devices/rgbd/SWLoadKinectData.h"


using namespace swDevice;

SWLoadKinectData::SWLoadKinectData(const std::string &sLoadingPath) : m_bInit(false), m_sLoadingPath(sLoadingPath)
{
    m_i32NumFile  = 0;
    m_i32NumPoint = 0;
    m_i32NumFile  = 0;
    m_i32NumCloud = 0;
    m_i32NumFrame = 0;

    // ########################################### mapped file paths

    m_oCloudDataFileParams.path         = m_sLoadingPath + "points0.raw";
    m_oCloudIndexFileParams.path        = m_sLoadingPath + "index0.raw";
    m_oCloudSizeFileParams.path		 	= m_sLoadingPath + "size.raw";
    m_oCloudTimeKinectFileParams.path   = m_sLoadingPath + "timeKinect.raw";
    m_oCloudHeaderFileParams.path       = m_sLoadingPath + "header.raw";

    // ########################################### mapped file flags

    m_oCloudDataFileParams.flags        = boost::iostreams::mapped_file::readonly;
    m_oCloudIndexFileParams.flags       = boost::iostreams::mapped_file::readonly;
    m_oCloudSizeFileParams.flags        = boost::iostreams::mapped_file::readonly;
    m_oCloudTimeKinectFileParams.flags  = boost::iostreams::mapped_file::readonly;
    m_oCloudHeaderFileParams.flags      = boost::iostreams::mapped_file::readonly;

    // ########################################### mapped file pointers

    m_i32HeaderNumber   	= NULL;
    m_oCloudData 	    	= NULL;
    m_oCloudIndex           = NULL;
    m_oCloudTimeKinect      = NULL;
    m_oCloudSize            = NULL;
}


SWLoadKinectData::~SWLoadKinectData()
{
    stop();
}


void SWLoadKinectData::start()
{
    {
        std::ifstream l_oFluxIndex(m_sLoadingPath + "index0.raw",       std::ifstream::in);
        std::ifstream l_oFluxData (m_sLoadingPath + "points0.raw",      std::ifstream::in);
        std::ifstream l_oSize     (m_sLoadingPath + "size.raw",         std::ifstream::in);
        std::ifstream l_oHeader   (m_sLoadingPath + "header.raw",       std::ifstream::in);
        std::ifstream l_oTime     (m_sLoadingPath + "timeKinect.raw",   std::ifstream::in);
        std::ifstream l_oVideo    (m_sLoadingPath + "bgr.avi",          std::ifstream::in);

        m_bIsCloudData = l_oFluxIndex.good() && l_oFluxData.good() && l_oSize.good() && l_oHeader.good() && l_oTime.good();
        m_bIsVideoData = l_oVideo.good();
    }

    try
    {
        if(!m_bIsCloudData)
        {
            std::cout << "Mapped files missing, no cloud data will be loaded. " << std::endl;
        }
        if(!m_bIsVideoData)
        {
            std::cout << "Video file missing, no video data will be loaded. " << std::endl;
        }
        if(!m_bIsVideoData && !m_bIsCloudData)
        {
            std::cerr << "No kinect data detected, end of the loading. " << std::endl;
            throw std::exception();
        }

        if(m_bIsVideoData)
        {
            m_oVideoCapture = cv::VideoCapture (m_sLoadingPath + "bgr.avi");

            if(!m_oVideoCapture.isOpened())
            {
                std::cerr << "Fail video file. " << std::endl;
                throw std::exception();
            }
        }

        if(m_bIsCloudData)
        {
            try
            {
                m_oCloudTimeKinectFile.open(m_oCloudTimeKinectFileParams);
                m_oCloudDataFile.open(m_oCloudDataFileParams);
                m_oCloudSizeFile.open(m_oCloudSizeFileParams);
                m_oCloudIndexFile.open(m_oCloudIndexFileParams);
                m_oCloudHeaderFile.open(m_oCloudHeaderFileParams);

                m_i32HeaderNumber   	= (int *)	m_oCloudHeaderFile.data();
                m_oCloudData 	    	= (float*)  m_oCloudDataFile.data();
                m_oCloudIndex           = (int*)    m_oCloudIndexFile.data();
                m_oCloudTimeKinect      = (float*)  m_oCloudTimeKinectFile.data();
                m_oCloudSize            = (int*)    m_oCloudSizeFile.data();
            }
            catch(boost::exception&)
            {
                std::cerr << "Fail opening boost mapped files " << std::endl;
                throw std::exception();
            }
        }

        m_bInit = true;
    }
    catch(std::exception&)
    {
        std::cerr << "Initialization aborted. SWLoadKinectData::start " << std::endl;
        m_bInit = false;
    }
}

void SWLoadKinectData::stop()
{
    if(m_bInit)
    {
        if(m_bIsCloudData)
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

bool SWLoadKinectData::grabVideo(cv::Mat &oBgr)
{
    if(!m_bInit)
    {
        std::cerr << "SWLoadKinectData must be initialized before grabbing data, loading stopped." << std::endl;
        stop();
        return false;
    }

    if(!m_bIsVideoData)
    {
        std::cout << "No video data initialized, loading stopped." << std::endl;
        stop();
        return false;
    }

    if(m_oVideoCapture.grab())
    {
        m_oVideoCapture.retrieve(oBgr);
    }
    else
    {
        std::cout << "No more video frames to grab, end of the loading." << std::endl;
        stop();
        return false;
    }

    return true;
}


bool SWLoadKinectData::grabCloud(cv::Mat &oCloud)
{
    if(!m_bInit)
    {
        std::cerr << "SWLoadKinectData must be initialized before grabbing data, loading stopped." << std::endl;
        stop();
        return false;
    }

    if(!m_bIsCloudData)
    {
        std::cout << "No cloud data initialized,  loading stopped." << std::endl;
        stop();
        return false;
    }

    cv::Scalar l_oScalPoint(0.f,0.f,0.f,0.f);
    oCloud = cv::Mat(cv::Size(640,480), CV_32FC3, l_oScalPoint); // init the cloud map

    for(int ii = 0; ii < m_oCloudSize[m_i32NumFrame]; ++ii)
    {        
        cv::Point3f l_p3fCurrPoint(m_oCloudData[3*(ii+m_i32NumPoint)], m_oCloudData[3*(ii+m_i32NumPoint)+1], m_oCloudData[3*(ii+m_i32NumPoint)+2]);
        oCloud.at<cv::Point3f>(m_oCloudIndex[ii+m_i32NumPoint]) = l_p3fCurrPoint;
    }

    m_i32NumPoint += m_oCloudSize[m_i32NumFrame];
    ++m_i32NumCloud;
    ++m_i32NumFrame;

    if(m_i32HeaderNumber[1 + m_i32NumFile] < m_i32NumCloud + 1)
    {
        std::cout << " File num " << m_i32NumFile++ << " fully loaded, load the next file " << std::endl;

        if(m_i32NumFile < m_i32HeaderNumber[0] )
        {
            // reset numbers of points and index
            m_i32NumPoint = 0;
            m_i32NumCloud = 0;

            // set num file string
            std::ostringstream l_osNumFile;
            l_osNumFile.str("");
            l_osNumFile.clear();
            l_osNumFile << m_i32NumFile;

            // close mapped files
            m_oCloudDataFile.close();
            m_oCloudIndexFile.close();

            // set the new path
            m_oCloudDataFileParams.path  = m_sLoadingPath + "points" + l_osNumFile.str() + ".raw";
            m_oCloudIndexFileParams.path = m_sLoadingPath + "index"  + l_osNumFile.str() + ".raw";

            try
            {
                // re open mapped files
                m_oCloudDataFile.open(m_oCloudDataFileParams);
                m_oCloudIndexFile.open(m_oCloudIndexFileParams);

                // get data pointers
                m_oCloudData      = (float *)m_oCloudDataFile.data();
                m_oCloudIndex     = (int *)  m_oCloudIndexFile.data();
            }
            catch (boost::exception&)
            {
                std::cerr << "Fail opening new boost mapped files. " << std::endl;
                stop();
                return false;
            }
        }
        else
        {
            std::cout << "No more cloud data to grab, end of the loading." << std::endl;
            stop();
            return false;
        }
    }

    return true;
}

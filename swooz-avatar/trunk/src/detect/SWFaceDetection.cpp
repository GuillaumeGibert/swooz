
/**
 * \file SWFaceDetection.cpp
 * \brief definition of SWFaceDetection
 * \author Florian Lance
 * \date 04/12/12
 */

#include "detect/SWFaceDetection.h"


using namespace swDetect;
using namespace swExcept;

SWFaceDetection::SWFaceDetection(const cv::Size &oMinDetectFaceSize, cbool bVerbose, std::string sClassifierFilePath) : m_oMinDetectFaceSize(oMinDetectFaceSize), m_bVerbose(bVerbose)
{	
    // init cascade files paths
        std::string l_sCascadeFaceFile  = sClassifierFilePath;
        std::string l_sCascadeNoseFile  = "../data/classifier/haarcascade_mcs_nose.xml";

    // set sizes for the nose
        m_oMinDetectNoseSize = cv::Size(30,30);
        m_oMaxDetectNoseSize = cv::Size(100,100);
	
    if(m_bVerbose)
    {
        std::cout << "Haar cascade file initialized : " << l_sCascadeFaceFile << std::endl;
    }

	m_bHaarCascadeFilesLoaded = false;
	
	try
	{
		// init face cascade detection
        m_CHaarCascadeFacePtr = SWHaarCascadePtr(new SWHaarCascade(l_sCascadeFaceFile, 3, m_oMinDetectFaceSize));
        m_CHaarCascadeNosePtr = SWHaarCascadePtr(new SWHaarCascade(l_sCascadeNoseFile, 3, m_oMinDetectNoseSize,m_oMaxDetectNoseSize));
		m_bHaarCascadeFilesLoaded = true;
	}
	catch(const haarFileInitError &e)
	{
        std::cerr << e.what() << std::endl;
	}
	
	// init detection ratios
    m_fWidthRatioImageToDetect  = 0.9f;
    m_fHeightRatioImageToDetect = 0.9f;

    m_fFaceHeightRatio 	        = 0.0f;
    m_fFaceWidthRatio           = 0.0f;
	
	// init rectangle offset
	m_i32WidthRectFaceOffset  = 10;
	m_i32HeightRectFaceOffset = 15;
	
    // init rects
    m_oLastDetectFace.width = 0;
    m_oLastDetectNose.width = 0;
}

SWFaceDetection::SWFaceDetection(const cv::Size &oMinDetectFaceSize, const cv::Size &oMaxDetectFaceSize, cbool bVerbose, std::string sClassifierFilePath):
    m_oMinDetectFaceSize(oMinDetectFaceSize),m_oMaxDetectFaceSize(oMaxDetectFaceSize), m_bVerbose(bVerbose)
{
    // init cascade files paths
        std::string l_sCascadeFaceFile  = sClassifierFilePath;
        std::string l_sCascadeNoseFile  = "../data/classifier/haarcascade_mcs_nose.xml";

    // set sizes for the nose
        m_oMinDetectNoseSize = cv::Size(30,30);
        m_oMaxDetectNoseSize = cv::Size(100,100);

    if(m_bVerbose)
    {
        std::cout << "Haar cascade file initialized : " << l_sCascadeFaceFile << std::endl;
    }

    m_bHaarCascadeFilesLoaded = false;

    try
    {
        // init face cascade detection
        m_CHaarCascadeFacePtr = SWHaarCascadePtr(new SWHaarCascade(l_sCascadeFaceFile, 3, m_oMinDetectFaceSize,m_oMaxDetectFaceSize));
        m_CHaarCascadeNosePtr = SWHaarCascadePtr(new SWHaarCascade(l_sCascadeNoseFile, 3, m_oMinDetectNoseSize,m_oMaxDetectNoseSize));
        m_bHaarCascadeFilesLoaded = true;
    }
    catch(const haarFileInitError &e)
    {
        std::cerr << e.what() << std::endl;
    }

    // init detection ratios
        m_fWidthRatioImageToDetect  = 0.9f;
        m_fHeightRatioImageToDetect = 0.9f;
        m_fFaceHeightRatio 	        = 0.0f;
        m_fFaceWidthRatio           = 0.0f;

    // init rectangle offset
        m_i32WidthRectFaceOffset  = 10;
        m_i32HeightRectFaceOffset = 15;

    // init rects
        m_oLastDetectFace.width = 0;
        m_oLastDetectNose.width = 0;
}

void SWFaceDetection::setRectRatios(cfloat fWidthRatio, cfloat fHeightRatio)
{
    m_fFaceWidthRatio  = fWidthRatio;
    m_fFaceHeightRatio = fHeightRatio;
}

cv::Rect SWFaceDetection::faceRect() const
{
    return m_oLastDetectFace;
}

cv::Rect SWFaceDetection::noseRect() const
{
    return m_oLastDetectNose;
}

bool SWFaceDetection::detectFace(const cv::Mat &oRgbImg)
{
    if(!m_bHaarCascadeFilesLoaded)
    {
        std::cerr << "-ERROR : (SWFaceDetection::detectFace) -> Haar cascade files not loaded, the face detection can't be done" << std::endl;
        return false;
    }

    if(m_oMinDetectFaceSize.width > oRgbImg.cols || m_oMinDetectFaceSize.height > oRgbImg.rows)
    {
        std::cerr << "-ERROR : (SWFaceDetection::detectFace) -> The input rgb image is too small for the wanted minimum face size,  the face detection can't be done. " << std::endl;
        return false;
    }

    m_oRects.clear();
    if(m_CHaarCascadeFacePtr->detect(oRgbImg, m_oRects))
    {
        if(m_oRects[0].x < oRgbImg.cols * (1-m_fWidthRatioImageToDetect)  || m_oRects[0].x + m_oRects[0].width  > m_fWidthRatioImageToDetect  * oRgbImg.cols ||
           m_oRects[0].y < oRgbImg.rows * (1-m_fHeightRatioImageToDetect) || m_oRects[0].y + m_oRects[0].height > m_fHeightRatioImageToDetect * oRgbImg.rows)
        {
            // detected face is not in the part of the image defined by the ratios
            m_oRects.clear();
            return false;
        }

        // resize face
        m_oRects[0].y      -= (int)(m_fFaceHeightRatio * m_oRects[0].height);
        m_oRects[0].height += (int)(m_fFaceHeightRatio * 2 * m_oRects[0].height);
        m_oRects[0].x      -= (int)(m_fFaceWidthRatio * m_oRects[0].width);
        m_oRects[0].width  += (int)(m_fFaceWidthRatio * 2 * m_oRects[0].width);

        // apply offsets on the face rectangle
        if(m_oRects[0].x > m_i32WidthRectFaceOffset)
        {
            m_oRects[0] -= cv::Point (m_i32WidthRectFaceOffset, 0);
            m_oRects[0] += cv::Size2i(m_i32WidthRectFaceOffset, 0);
        }
        if(m_oRects[0].y > m_i32HeightRectFaceOffset)
        {
            m_oRects[0] -= cv::Point (0, m_i32HeightRectFaceOffset);
            m_oRects[0] += cv::Size2i(0, m_i32HeightRectFaceOffset);
        }
        if(m_oRects[0].x + m_oRects[0].width + m_i32WidthRectFaceOffset   < oRgbImg.cols)
        {
            m_oRects[0] += cv::Size2i(m_i32WidthRectFaceOffset,0);
        }
        if(m_oRects[0].y + m_oRects[0].height + m_i32HeightRectFaceOffset < oRgbImg.rows)
        {
            m_oRects[0] += cv::Size2i(0, m_i32HeightRectFaceOffset);
        }

        m_oLastDetectFace = m_oRects[0];
        return true;
    }


    return  false;
}


cv::Rect SWFaceDetection::detectNose(const cv::Mat &oRgbImg)
{
    cv::Rect l_oNullRect;
    l_oNullRect.width = 0;

    if(!m_bHaarCascadeFilesLoaded)
    {
        std::cerr << "-ERROR : (SWFaceDetection::detectNose) -> Haar cascade files not loaded, the nose detection can't be done" << std::endl;
        return l_oNullRect;
    }

    if(m_oMinDetectNoseSize.width > oRgbImg.cols || m_oMinDetectNoseSize.height > oRgbImg.rows)
    {
        std::cerr << "-ERROR : (SWFaceDetection::detectNose) -> The input rgb image is too small for the wanted minimum nose size, the nose detection can't be done. " << std::endl;
        return l_oNullRect;
    }

    m_oRects.clear();

    if(m_CHaarCascadeNosePtr->detect(oRgbImg, m_oRects))
    {
        m_oLastDetectNose = m_oRects[0];
    }

    return m_oLastDetectNose;
}


void SWFaceDetection::displayFace(cv::Mat &oRgbImg, cv::Scalar oColor)
{
    if(m_oLastDetectFace.width > 0)
    {
        if(swUtil::isInside(m_oLastDetectFace, oRgbImg))
        {
            cv::rectangle(oRgbImg, cv::Point(m_oLastDetectFace.x, m_oLastDetectFace.y),
                cv::Point(m_oLastDetectFace.x+m_oLastDetectFace.width, m_oLastDetectFace.y+m_oLastDetectFace.height), oColor,1);
        }
    }
}

//cv::Point3f SWFaceDetection::computeNoseTip(cv::Mat &oFaceDepth, int &idX, int &idY)
//{
//    float l_fMinDist = FLT_MAX;
//    cv::Point3f l_oNoseTip;

//    // retrieve the nose part of the depth image
//    for(int ii = oFaceDepth.rows/2; ii < oFaceDepth.rows; ++ii)  // /2 for avoiding the forehead   // TODO : to be parametered
//    {
//        for(int jj = 0; jj < oFaceDepth.cols; ++jj)
//        {
//            float l_fCurrDepth = oFaceDepth.at<cv::Vec3f>(ii,jj)[2]; // retrieve current depth

//            if(l_fCurrDepth == 0)
//            {
//                // if null value go to the next value
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


cv::Point3f SWFaceDetection::computeNoseTip(cv::Mat &oMatDepth, int &idX, int &idY)
{
    float l_fMinDist = FLT_MAX;
    cv::Point3f l_oNoseTip;

    // find the closest point

    for(int ii = 0; ii < oMatDepth.rows; ++ii)
    {
        for(int jj = 0; jj < oMatDepth.cols; ++jj)
        {
            float l_fCurrDepth = oMatDepth.at<cv::Vec3f>(ii,jj)[2]; // retrieve current depth

            if(l_fCurrDepth == 0)
            {
                // if null value of to the next value
                continue;
            }

            if(l_fCurrDepth < l_fMinDist)
            {
                l_fMinDist = l_fCurrDepth;
            }
        }
    }

    float l_fMaxDist = l_fMinDist + 0.0035f;
//    float l_fMaxDist = l_fMinDist + 0.01f;

    struct PointCoordinate
    {
        int ii;
        int jj;
    };

    std::vector<PointCoordinate> l_oMinPoints;

    for(int ii = 0; ii < oMatDepth.rows; ++ii)
    {
        for(int jj = 0; jj < oMatDepth.cols; ++jj)
        {
            float l_fCurrDepth = oMatDepth.at<cv::Vec3f>(ii,jj)[2]; // retrieve current depth

            if(l_fCurrDepth == 0)
            {
                // if null value go to the next value
                continue;
            }

            if(l_fCurrDepth < l_fMaxDist)
            {
                PointCoordinate pt;
                pt.ii = ii;
                pt.jj = jj;
                l_oMinPoints.push_back(pt);
            }
        }
    }


    int l_i32SumII = 0;
    int l_i32SumJJ = 0;

    for(uint ii = 0; ii < l_oMinPoints.size(); ++ii)
    {
        l_i32SumII += l_oMinPoints[ii].ii;
        l_i32SumJJ += l_oMinPoints[ii].jj;
    }

    l_i32SumII /= l_oMinPoints.size();
    l_i32SumJJ /= l_oMinPoints.size();


    idY = l_i32SumII;
    idX = l_i32SumJJ;

    l_oNoseTip = oMatDepth.at<cv::Vec3f>(l_i32SumII,l_i32SumJJ);

    return l_oNoseTip;

}

//cv::Point3f SWFaceDetection::computeNoseTip(cv::Mat &oFaceDepth, int &idX, int &idY)
//{
//    float l_fMinDist = FLT_MAX;
//    cv::Point3f l_oNoseTip;

//    // find the closest point

//    for(int ii = 0; ii < oFaceDepth.rows; ++ii)
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
//            }
//        }
//    }

//    std::cout << "cnt : " << l_fMinDist << std::endl;

//    // ...
//    bool  l_bNoseTipFound= false;
//    float l_fFactorClose = 1.01f;

//    for(int ii = 0; ii < oFaceDepth.rows/2; ++ii)
//    {
//        for(int jj = 0; jj < oFaceDepth.cols/2; ++jj)
//        {
//            float l_fCurrDepthUpRight   = oFaceDepth.at<cv::Vec3f>(oFaceDepth.rows/2 - ii, oFaceDepth.cols/2 + jj)[2];
//            float l_fCurrDepthUpLeft    = oFaceDepth.at<cv::Vec3f>(oFaceDepth.rows/2 - ii, oFaceDepth.cols/2 - jj)[2];

//            float l_fCurrDepthDownRight = oFaceDepth.at<cv::Vec3f>(oFaceDepth.rows/2 + ii, oFaceDepth.cols/2 + jj)[2];
//            float l_fCurrDepthDownLeft  = oFaceDepth.at<cv::Vec3f>(oFaceDepth.rows/2 + ii, oFaceDepth.cols/2 - jj)[2];

////            std::cout << "up : "   << oFaceDepth.rows/2 - ii << " "  << jj  << " " << l_fCurrDepthUp << " | ";
////            std::cout << "down : " << oFaceDepth.rows/2 + ii << " "  << jj  << " " << l_fCurrDepthDown << " | ";

//            // check up
//            if(l_fCurrDepthUpRight != 0)
//            {
//                if(l_fCurrDepthUpRight < l_fFactorClose * l_fMinDist)
//                {
//                    idX = oFaceDepth.cols/2 + jj;
//                    idY = oFaceDepth.rows/2 - ii;
//                    l_bNoseTipFound = true;
//                    std::cout << "up right found : " << idX << " " << idY << " " << l_fCurrDepthUpRight << std::endl;
//                    break;
//                }
//            }
//            if(l_fCurrDepthUpLeft != 0)
//            {
//                if(l_fCurrDepthUpLeft < l_fFactorClose * l_fMinDist)
//                {
//                    idX = oFaceDepth.cols/2 - jj;
//                    idY = oFaceDepth.rows/2 - ii;
//                    l_bNoseTipFound = true;
//                    std::cout << "up left found : " << idX << " " << idY << " " << l_fCurrDepthUpLeft << std::endl;
//                    break;
//                }
//            }


//            // check down
//            if(l_fCurrDepthDownRight != 0)
//            {
//                if(l_fCurrDepthDownRight < l_fFactorClose * l_fMinDist)
//                {
//                    idX = oFaceDepth.cols/2 + jj;
//                    idY = oFaceDepth.rows/2 + ii;
//                    l_bNoseTipFound = true;
//                    std::cout << "down right found : " << idX << " " << idY << " " << l_fCurrDepthDownRight << std::endl;
//                    break;
//                }
//            }
//            if(l_fCurrDepthDownLeft != 0)
//            {
//                if(l_fCurrDepthDownLeft < l_fFactorClose * l_fMinDist)
//                {
//                    idX = oFaceDepth.cols/2 - jj;
//                    idY = oFaceDepth.rows/2 + ii;
//                    l_bNoseTipFound = true;
//                    std::cout << "down left found : " << idX << " " << idY << " " << l_fCurrDepthDownLeft << std::endl;
//                    break;
//                }
//            }

//        }

//        if(l_bNoseTipFound)
//        {
//            break;
//        }
//    }

//    l_oNoseTip = oFaceDepth.at<cv::Vec3f>(idY,idX);

//    return l_oNoseTip;
//}



//void SWFaceDetection::setFaceHeightRatio(cfloat fRatio)
//{
//	(fRatio < 0.5f) ? (m_fFaceHeightRatio = 0.5f) : ((fRatio > 1.5f) ? (m_fFaceHeightRatio = 1.5f) : (m_fFaceHeightRatio = fRatio));
//}



//cv::Point SWFaceDetection::middleFacePoint()
//{
//	Point l_oMiddle;
	
//	l_oMiddle.x = m_oRects[0].x + m_oRects[0].width/2;
//	l_oMiddle.y = m_oRects[0].y + m_oRects[0].height/2;

//	return l_oMiddle;
//}




//Rect SWFaceDetection::computeUpperFace(const Rect &oFaceRect, const Point3f oNoseTip)
//{
//	Rect  l_oUpperFace = oFaceRect;
//	Point3f l_oNoseTip = oNoseTip;
	
//	int l_ui32IndexXMin = (int)(oFaceRect.x + l_oNoseTip.x);
//	int l_ui32IndexYMin = (int)(oFaceRect.y + l_oNoseTip.y);

//	l_oUpperFace.width = (int)(l_oUpperFace.width * 1.5f/5.f); //3.f/5.f;
//	l_oUpperFace.x     = l_ui32IndexXMin - l_oUpperFace.width/2;
	
//	l_oUpperFace.height = (int)(l_oUpperFace.height / 2.8);
//	l_oUpperFace.y      = l_ui32IndexYMin - l_oUpperFace.height;

//	return l_oUpperFace;
//}


//bool SWFaceDetection::detectNose(const Mat &oRgbImg)
//{
//	if(m_CHaarCascadeNosePtr->detect(oRgbImg, m_oNose))
//	{
//		// retrieve the nose image
//		m_oRgbNoseImg = oRgbImg(m_oNose[0]);
		
//		return true;
//	}
	
//	return false;
//}

//Point3f SWFaceDetection::noseTip()
//{
//	return m_oLastDetectedNoseTip;
//}

//float SWFaceDetection::retrieveNoseTipZDistance(const Mat &oDepthImg)
//{
//	unsigned short l_UI16MinDepthValue = SHRT_MAX;
//	float 	       l_fMinDepthValue    = FLT_MAX;
	
//	if(oDepthImg.depth() == CV_16U && oDepthImg.channels() == 1)
//	{
//		// retrieve the nose part of the depth image
//		for(int ii = 0; ii < oDepthImg.rows; ++ii)
//		{
//			for(int jj = 0; jj < oDepthImg.cols; ++jj)
//			{
//				unsigned short l_UI16CurrDepth = oDepthImg.at<unsigned short>(ii,jj); // retrieve current depth
				
//				if(l_UI16CurrDepth == 0)
//				{
//					// if null value of to the next value
//					continue;
//				}
				
//				if(l_UI16CurrDepth < l_UI16MinDepthValue)
//				{
//					l_UI16MinDepthValue = l_UI16CurrDepth;
//				}
//			}
//		}
		
//		return l_UI16MinDepthValue;
//	}
//	else
//	{
//		// retrieve the nose part of the depth image
//		for(int ii = 0; ii < oDepthImg.rows; ++ii)
//		{
//			for(int jj = 0; jj < oDepthImg.cols; ++jj)
//			{
//				float l_fCurrDepth = oDepthImg.at<cv::Vec3f>(ii,jj)[2]; // retrieve current depth
				
//				if(l_fCurrDepth == 0)
//				{
//					// if null value of to the next value
//					continue;
//				}
				
//				if(l_fCurrDepth < l_fMinDepthValue)
//				{
//					l_fMinDepthValue = l_fCurrDepth;
//				}
//			}
//		}
		
//		return l_fMinDepthValue;
//	}
	
//	return -1;
//}


//Point3f SWFaceDetection::computeNoseTip(const Mat &oDepthImg)
//{
	
//	// TODO : not working very well
	
//	unsigned short l_UI16MinDepthValue = SHRT_MAX;
//	float 	       l_fMinDepthValue = FLT_MAX;
	
//	vector<Point3f> 	l_oVP3fCloserPoints(1);
//	vector<unsigned short>  l_oVUI16CloserPointsDepth(1,SHRT_MAX);
//	vector<float>  		l_oV3FCloserPointsDepth(1,FLT_MAX);
		
	
//	if(oDepthImg.depth() == CV_16U && oDepthImg.channels() == 1)
//	{
//		// retrieve the nose part of the depth image
//		for(int ii = 0; ii < oDepthImg.rows; ++ii)
//		{
//			for(int jj = 0; jj < oDepthImg.cols; ++jj)
//			{
//				unsigned short l_UI16CurrDepth = oDepthImg.at<unsigned short>(ii,jj); // retrieve current depth

//				if(l_UI16CurrDepth == 0)
//				{
//					// if null value of to the next value
//					continue;
//				}
				
//				// update the min value array (used for computing mean coordinates for the nose tip)
//				for(uint kk = 0; kk < l_oVUI16CloserPointsDepth.size(); ++kk)
//				{
//					if(l_UI16CurrDepth < l_oVUI16CloserPointsDepth[kk])
//					{
//						l_oVUI16CloserPointsDepth[kk] = l_UI16CurrDepth;
//						l_oVP3fCloserPoints[kk]       = Point3f((float)jj,(float)ii,0);
//						break;
//					}
//				}
				
//				// update the minimum depth
//				if(l_UI16CurrDepth < l_UI16MinDepthValue)
//				{
//					l_UI16MinDepthValue = l_UI16CurrDepth;
//				}
//			}
//		}
//	}
//	else
//	{
//		// retrieve the nose part of the depth image
//		for(int ii = 0; ii < oDepthImg.rows; ++ii)
//		{
//			for(int jj = 0; jj < oDepthImg.cols; ++jj)
//			{
//				float l_fCurrDepth = oDepthImg.at<cv::Vec3f>(ii,jj)[2]; // retrieve current depth

//				if(l_fCurrDepth == 0)
//				{
//					// if null value of to the next value
//					continue;
//				}
				
//				// update the min value array (used for computing mean coordinates for the nose tip)
//				for(uint kk = 0; kk < l_oV3FCloserPointsDepth.size(); ++kk)
//				{
//					if(l_fCurrDepth < l_oV3FCloserPointsDepth[kk])
//					{
//						l_oV3FCloserPointsDepth[kk] = l_fCurrDepth;
//						l_oVP3fCloserPoints[kk]       = Point3f((float)jj,(float)ii,0);
//						break;
//					}
//				}
				
//				// update the minimum depth
//				if(l_fCurrDepth < l_fMinDepthValue)
//				{
//					l_fMinDepthValue = l_fCurrDepth;
//				}
//			}
//		}
//	}

//	// compute the mean nose tip point (only for x and y coordinates)
//	Point3f l_oTotalPoint(0,0,0);
//	for(uint ii = 0; ii < l_oVP3fCloserPoints.size(); ++ii)
//	{
//		l_oTotalPoint += l_oVP3fCloserPoints[ii];
//	}
	
//	Point3f l_oNoseTip = l_oTotalPoint* (1.f / l_oVP3fCloserPoints.size());
	
//	if(oDepthImg.depth() == CV_16U && oDepthImg.channels() == 1)
//	{
//		l_oNoseTip.z += l_UI16MinDepthValue;
//	}
//	else
//	{
//		l_oNoseTip.z += l_fMinDepthValue;
//	}
	
	

//	return l_oNoseTip;
//}

//bool SWFaceDetection::detectNoseTip(const Mat &oDepthImg)
//{
//	m_oNoseTip = Point3f(0,0,0);
	
//		Mat l_DepthImg;
		
//		if(!m_bNoseDetected)
//		{
//			l_DepthImg = oDepthImg(m_oRects[0]);
//		}
//        else if(m_oRects.size() > 0)
//		{
//			l_DepthImg = (oDepthImg(m_oRects[0]))(m_oNose[0]);
//		}
//        else
//        {
//            l_DepthImg = oDepthImg;
//        }

		
//		m_oNoseTip = computeNoseTip(l_DepthImg);
		
//		// add the face rectangle values
//        if(m_oRects.size() > 0)
//        {
//            m_oNoseTip.x += m_oRects[0].x;
//            m_oNoseTip.y += m_oRects[0].y;
//        }
		
//		// if the nose has been detected add also nose rectangles values
//		if(m_bNoseDetected)
//		{
//			m_oNoseTip.x += m_oNose[0].x;
//			m_oNoseTip.y += m_oNose[0].y;
//		}
				
//		m_oLastDetectedNoseTip = m_oNoseTip;

//	return true;
//}



//void SWFaceDetection::displayAllDetections(Mat &oRgbImg)
//{
//	// draw rectangle face
//	if(m_oRects.size() > 0)
//	{
//        if(swUtil::isInside(m_oRects[0],oRgbImg))
//		{
//			rectangle(oRgbImg, Point(m_oRects[0].x, m_oRects[0].y),
//			           Point(m_oRects[0].x + m_oRects[0].width, m_oRects[0].y + m_oRects[0].height), RED,1);
//		}

//		if(m_bNoseDetected)
//		{
//			Point l_oNoseLeftUpperPoint(m_oRects[0].x + m_oNose[0].x, m_oRects[0].y + m_oNose[0].y);
			
//			rectangle(oRgbImg, l_oNoseLeftUpperPoint,
//					   l_oNoseLeftUpperPoint + Point(m_oNose[0].width,m_oNose[0].height), WHITE,1);
//		}
		
//		if(m_oLastDetectedNoseTip !=  Point3f(0,0,0))
//		{
//			circle(oRgbImg, Point2d(m_oLastDetectedNoseTip.x,m_oLastDetectedNoseTip.y),1, RED);
			
//			Rect l_oUpperFace = computeUpperFace(m_oRects[0], m_oLastDetectedNoseTip);
//			l_oUpperFace.x -= m_oRects[0].x;
//			l_oUpperFace.y -= m_oRects[0].y;
						
//			rectangle(oRgbImg, Point(l_oUpperFace.x,l_oUpperFace.y),
//			           Point(l_oUpperFace.x + l_oUpperFace.width, l_oUpperFace.y + l_oUpperFace.height), GREEN,1);
//		}
//	}
//}



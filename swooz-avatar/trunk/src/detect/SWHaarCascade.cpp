
/**
 * \file SWHaarCascade.cpp
 * \brief definition of SWHaarCascade
 * \author Florian Lance
 * \date 04/12/12
 */

#include "detect/SWHaarCascade.h"

using namespace cv;
using namespace cv::gpu;
using namespace std;
using namespace swDetect;
using namespace swExcept;

SWHaarCascade::SWHaarCascade(const string &sCascadeFile, const uint ui32NumberOfDetections, const Size oMinDetectSize, const Size oMaxDetectSize, cbool bUseGPU)
  : m_ui32NumberOfDetections(ui32NumberOfDetections), m_oMinDetectSize(oMinDetectSize), m_oMaxDetectSize(oMaxDetectSize), m_bUseGPU(bUseGPU)
{	
	m_fPyramidScale = 1.1f;
	
	// m_bUseGPU = true;
	m_bUseGPU = false;
	
//	if(m_bUseGPU)
//	{
//		cv::gpu::printShortCudaDeviceInfo(cv::gpu::getDevice());
//		m_initFile = m_oCascadeGpu.load( sCascadeFile );
//	}
//	else
//	{
		// load haar cascade file
		try
		{		
			m_initFile = m_oCascade.load( sCascadeFile );
		}
		catch (const cv::Exception &e)
		{
            cerr << "-ERROR : (SWHaarCascade::SWHaarCascade) -> " << e.what() << endl;
			m_initFile = false;
		}
//	}
	
	if(!m_initFile)
	{
		throw haarFileInitError();
	}
}

SWHaarCascade::~SWHaarCascade()
{

}

bool SWHaarCascade::detect(const Mat& oRgbImg, vector<Rect> &oRects)
{
//	if(m_bUseGPU)
//	{
//		return detectGPU(oRgbImg, oRects);
//	}
//	else
//	{
		return detectCPU(oRgbImg, oRects);
//	}
}

bool SWHaarCascade::detectCPU(const Mat& oRgbImg, vector<Rect> &oRects)
{	
	if(!m_initFile)
	{
        cerr << "-ERROR : (SWHaarCascade::detectCPU) -> Cascade classifier not initialized. " << endl;
		return false;
	}
	
	try
	{
		// haar cascade algorithm only works with Gray images
		Mat l_oGrayImg; 
		cvtColor( oRgbImg, l_oGrayImg, CV_BGR2GRAY ); // conversion
		equalizeHist( l_oGrayImg, l_oGrayImg ); // equalize histogram
		
        // detect
        if(m_oMaxDetectSize.width == 0)
        {
            m_oCascade.detectMultiScale( l_oGrayImg, oRects, m_fPyramidScale, m_ui32NumberOfDetections,
                0 , m_oMinDetectSize );
        }
        else
        {
            m_oCascade.detectMultiScale( l_oGrayImg, oRects, m_fPyramidScale, m_ui32NumberOfDetections,
                0 , m_oMinDetectSize, m_oMaxDetectSize );
        }
	}
	catch (const cv::Exception &e)
	{
		cerr << e.what() << endl;
		throw haarDetectError();		
	}

	return (oRects.size() > 0);
}

//bool SWHaarCascade::detectGPU(const Mat& oRgbImg, vector<Rect> &oRects)
//{
//    if (getCudaEnabledDeviceCount() == 0)
//    {
//        cerr << "No GPU found or the library is compiled without GPU support" << endl;
//        return false;
//    }

//	if(!m_initFile)
//	{
//		cout << "Cascade classifier gpu not initialized. " << endl;
//		return false;
//	}
//	try
//	{
//		// haar cascade algorithm only works with Gray images
//		Mat l_oGrayImg, l_oFacesDownloaded;

//		cvtColor( oRgbImg, l_oGrayImg, CV_BGR2GRAY ); // conversion
//		equalizeHist( l_oGrayImg, l_oGrayImg ); // equalize histogram

//		GpuMat l_oGMatFrame, l_oGMatObjBuff;

//		l_oGMatFrame.upload(l_oGrayImg);
		
//		uint l_ui32DetectionsNum;
		
//		// m_oCascadeGpu.findLargestObject = true;
		
//		// detect
//		if(m_oMaxDetectSize.width == 0)
//		{
//			l_ui32DetectionsNum = m_oCascadeGpu.detectMultiScale( l_oGMatFrame, l_oGMatObjBuff, m_fPyramidScale, m_ui32NumberOfDetections, m_oMinDetectSize );
//		}
//		else
//		{
//			l_ui32DetectionsNum = m_oCascadeGpu.detectMultiScale( l_oGMatFrame, l_oGMatObjBuff, m_oMaxDetectSize, m_oMinDetectSize, m_fPyramidScale, 1);
//		}
		
//		l_oGMatObjBuff.colRange(0, l_ui32DetectionsNum).download(l_oFacesDownloaded);
	
//		Rect *l_oFaceRects = l_oFacesDownloaded.ptr<Rect>();
//		oRects.clear();
		
//		// Rect l_oRect =  l_oFaceRects[0];
//		// oRects.push_back(l_oRect);

//		Rect l_oMoyRect;
//		if(l_ui32DetectionsNum > 0)
//		{
//			int l_i32TotalHeight = 0, l_i32TotalWidth = 0;
//			int l_i32TotalX = 0, l_i32TotalY = 0;
			
//			Rect l_oMinRect =  l_oFaceRects[0];
			
//			for(uint ii = 0; ii < l_ui32DetectionsNum; ++ii)
//			{
//				Rect l_oFaceRect;
//				l_oFaceRect = l_oFaceRects[ii];
				
//				l_i32TotalHeight += l_oFaceRects[ii].height;
//				l_i32TotalWidth  += l_oFaceRects[ii].width;
//				l_i32TotalX      += l_oFaceRects[ii].x;
//				l_i32TotalY      += l_oFaceRects[ii].y;
//			}
			
//			l_oMoyRect.x = l_i32TotalX/ l_ui32DetectionsNum;
//			l_oMoyRect.y = l_i32TotalY/ l_ui32DetectionsNum;
//			l_oMoyRect.height = l_i32TotalHeight/ l_ui32DetectionsNum;
//			l_oMoyRect.width  = l_i32TotalWidth/ l_ui32DetectionsNum;
			
//			oRects.push_back(l_oMoyRect);
//		}
//	}
//	catch (const cv::Exception &e)
//	{
//		cerr << e.what() << endl;
//		throw haarDetectError();
//	}


//	return (oRects.size() > 0);
//}

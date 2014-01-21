
/**
 * \file SWStasm.cpp
 * \brief definition of SWStasm
 * \author Florian Lance
 * \date 18/02/13
 */

#include "detect/SWStasm.h"
#include "stdafx.h"

using namespace swDetect;
using namespace swDevice;
using namespace swExcept;

SWStasm::SWStasm(const SWKinectParams &oKinectParams, const std::string &sConfFile0Path, const std::string &sConfFile1Path) :
	m_CKinectParams(oKinectParams), m_sConfFile0(sConfFile0Path), m_sConfFile1(sConfFile1Path)
{
	// init result features points array
    m_oFeaturesPoints.assign(68, cv::Point2i(0,0));

	// read and inits models
	m_i32InitializedModels = nInitAsmModels(m_oModels, sConfFile0Path.c_str(), sConfFile1Path.c_str());	
	std::cout << "Number of stasm models initialized : " << m_i32InitializedModels << std::endl;
	
	if(m_i32InitializedModels != 2)
	{
		std::cerr << "The two models must be initialized (SWStasm constructor ) " << std::endl;
		m_bInitDone = false;
	}
	else
	{
		m_bInitDone = true;
	}
	
	m_bInitDone = true;
	
	m_bInitFirstShape = false;
}
	
SWStasm::~SWStasm()
{

}

void SWStasm::setKinectParams(const SWKinectParams &oKinectParams)
{
	m_CKinectParams = oKinectParams;
}

void SWStasm::resetParams()
{
	m_bInitFirstShape= false;		
	m_oInitShape	 = SHAPE();
	m_oCombinedShape = SHAPE();	
}


void SWStasm::convRectToStasmDetParams(const cv::Rect &oCurrAugmentedRectFace, const cv::Rect &oCurrRectFace, DET_PARAMS &oDetParams)
{
	cv::Rect l_oRectToUse = oCurrRectFace;
	l_oRectToUse.x =  oCurrRectFace.x - oCurrAugmentedRectFace.x;
	l_oRectToUse.y =  oCurrRectFace.y - oCurrAugmentedRectFace.y;
	
	oDetParams.x      = l_oRectToUse.x - oCurrAugmentedRectFace.width/2 + l_oRectToUse.width/2;
	oDetParams.y      = oCurrAugmentedRectFace.height/2 - l_oRectToUse.y - l_oRectToUse.height/2;
	oDetParams.width  = l_oRectToUse.width;
	oDetParams.height = l_oRectToUse.height;
}

void SWStasm::convMatToStasmImage(const cv::Mat &oCurrRgbMat, ImageT<RGB_TRIPLE> &oStasmImage)
{
	// init data
	for(int ii = 0; ii < oCurrRgbMat.rows ; ++ii)
	{
		for(int jj = 0; jj < oCurrRgbMat.cols; ++jj)
		{
			RGB_TRIPLE l_oRgb;
			l_oRgb.Red  = oCurrRgbMat.at<cv::Vec<uchar, 3> >(ii,jj)[2];
			l_oRgb.Green= oCurrRgbMat.at<cv::Vec<uchar, 3> >(ii,jj)[1];
			l_oRgb.Blue = oCurrRgbMat.at<cv::Vec<uchar, 3> >(ii,jj)[0];

			oStasmImage(jj,oCurrRgbMat.rows-1 - ii) = l_oRgb;
		}
	}
}

    
bool SWStasm::launchAsmSearch(const cv::Mat &oCurrRgbMat,  cv::Rect &oCurrFaceRect)
{
	if(!m_bInitDone)
	{
		std::cerr << "No configuration files loaded. (launchAsmSearch) " << std::endl;
		return false;
	}

	try
	{		
		DET_PARAMS l_oDetParams;
		cv::Mat l_oCurrRgbMatResized = oCurrRgbMat.clone();
		
		// resize the rgb mat
		if(m_CKinectParams.m_oOriginalSize != m_CKinectParams.m_oVideoSize)
		{
			resize(l_oCurrRgbMatResized,l_oCurrRgbMatResized, m_CKinectParams.m_oVideoSize);
		}		
		
		// init augmented face rectangle
		cv::Rect l_oAugmentedFaceRect = oCurrFaceRect;
		cv::Mat l_oFaceRgbMat;		
		l_oAugmentedFaceRect.x       -= oCurrFaceRect.width /10;
		l_oAugmentedFaceRect.y       -= oCurrFaceRect.height /10;
		l_oAugmentedFaceRect.width   += oCurrFaceRect.width  /5;
		l_oAugmentedFaceRect.height  += oCurrFaceRect.height /5;	
		l_oFaceRgbMat = l_oCurrRgbMatResized.clone()(l_oAugmentedFaceRect);
		
		// conv opencv mat to stasm image
		ImageT<RGB_TRIPLE> l_oStasmImage(l_oFaceRgbMat.cols, l_oFaceRgbMat.rows);
		ImageT<unsigned char> l_oStasmGrayImage(l_oFaceRgbMat.cols, l_oFaceRgbMat.rows);

		// convert cv::mat to stasm image
		convMatToStasmImage(l_oFaceRgbMat, l_oStasmImage);

		// convert to gray image
		ConvertRgbImageToGray(l_oStasmGrayImage, l_oStasmImage);

		// conv opencv rect to stasm det_params
		convRectToStasmDetParams(l_oAugmentedFaceRect, oCurrFaceRect, l_oDetParams);

		SHAPE l_oDetAttr = m_oModels[0].VjAv;
		SHAPE l_oStartShape, l_oShape;
			
		
		// Align l_oDetParams to the face detector parameters and return it as StartShape
		// This undoes AlignToDetFrame().  It ignores the eye positions, if any.
		AlignStartShapeToDet(l_oStartShape, l_oDetParams, l_oDetAttr, CONF_VjScale);
		
		// Jitter points at 0,0 if any.
		// In a shape, both x and y equal to 0 is taken by the stasm software to mean
		// that the point is unused.
		// If this is the case for any points in Shape, jitter x so the point is
		// not later seen as unused.
		JitterPoints(l_oStartShape); // jitter points at 0,0 if any
		l_oShape = l_oStartShape;
			
		
			
		for (int iiModel = 0; iiModel < m_i32InitializedModels; iiModel++)
		{
			ASM_MODEL *l_oModel = &m_oModels[iiModel];
				
			// if (iiModel != 0)
			if (m_bInitFirstShape)
			{
				// GetStartShapeFromPreviousSearch(l_oShape, m_oCombinedShape, l_oModel->FileMeanShape);
				GetStartShapeFromPreviousSearch(l_oShape, m_oInitShape, l_oModel->FileMeanShape);
			}

			// Scale Shape and Img, so the face width is nStandardFaceWidth,
			// using the start shape to approximate the face width.
			double l_dImageScale = l_oModel->nStandardFaceWidth / xShapeExtent(l_oShape);
			SHAPE l_oWorkingShape(l_oShape * l_dImageScale);   		// working shape
			Image l_oWorkingImage(l_oStasmGrayImage);      	  		// working Img
			
			int l_i32NewWidth  = iround(l_oWorkingImage.width  * l_dImageScale);
			int l_i32NewHeight = iround(l_oWorkingImage.height * l_dImageScale);
			ScaleImage(l_oWorkingImage, l_i32NewWidth, l_i32NewHeight, IM_BILINEAR);

			// dimKeep is needed when this model has different number
			// of landmarks from previous model
			l_oWorkingShape.dimKeep(l_oModel->nPoints, 2);
			
			int l_i32StartLev = l_oModel->nStartLev;
			l_oWorkingShape  /= pow(l_oModel->PyrRatio ,l_i32StartLev); 	// GetPyrScale(nStartLev, l_oModel->PyrRatio);
			
			for (int iiLev = l_i32StartLev; iiLev >= 0; iiLev--)   		// for each lev in image pyr
			{
				double l_dPyrScale =  pow(l_oModel->PyrRatio, iiLev );  // GetPyrScale(iiLev, pModel->PyrRatio);
				
				SEARCH_IMAGES l_oSearchImgs;   		  		// the images used during search
				l_oSearchImgs.Img = l_oWorkingImage;      	        // l_oSearchImgs.Img gets scaled to this pyr lev
				
				ReduceImage(l_oSearchImgs.Img, l_dPyrScale, l_oModel->PyrReduceMethod);
				
				InitGradsIfNeeded(l_oSearchImgs.Grads,   	        // get l_oSearchImgs.Grads
				l_oModel->AsmLevs[iiLev].ProfSpecs, l_oSearchImgs.Img, l_oWorkingShape.nrows());

				AsmLevSearch(l_oWorkingShape, l_oSearchImgs, m_oModels[iiModel], iiLev, gLandTab);

				// use best shape from this iter as starting point for next
				if (iiLev != 0) 
				{
					l_oWorkingShape *= l_oModel->PyrRatio;
				}
			}
				
			m_oCombinedShape.assign(l_oWorkingShape);   			// use assign not "=" because size may differ
			m_oCombinedShape = m_oCombinedShape / l_dImageScale; 		// descale back to original size
			
			m_oInitShape = m_oCombinedShape;
			m_bInitFirstShape = true;
		}
	
		for(uint ii = 0; ii < m_oFeaturesPoints.size(); ++ii)
		{
			// compute points coordinates for opencv
			int l_i32CurrX = (uint)m_oCombinedShape.getElem(ii,0) + l_oAugmentedFaceRect.width/2 - oCurrFaceRect.width/2;
			l_i32CurrX    += l_oAugmentedFaceRect.x + oCurrFaceRect.width/2;
			
			int l_i32CurrY =  l_oAugmentedFaceRect.height/2 - oCurrFaceRect.height/2 - (uint)m_oCombinedShape.getElem(ii,1);
			l_i32CurrY    += l_oAugmentedFaceRect.y + oCurrFaceRect.height/2;
			
			m_oFeaturesPoints[ii] =  cv::Point2i(l_i32CurrX, l_i32CurrY);
		}
	}
	catch (const stasmComputeError &e)
	{
		cerr << e.what() << endl;
		
		m_bInitFirstShape= false;		
		m_oInitShape	 = SHAPE();
		m_oCombinedShape = SHAPE();
		
		throw stasmError();
		
		return false;
	}
		
	return true;
}


void SWStasm::display(cv::Mat& oOutputDetectRgbImg, const cv::Scalar &oColor) const
{
	// display feature points
	for(uint ii = 0; ii < m_oFeaturesPoints.size(); ++ii)
	{
		cv::circle(oOutputDetectRgbImg, m_oFeaturesPoints[ii], 1, oColor);		
	}		
}

void SWStasm::compute3DPoints(const cv::Mat& oCloudMap, std::vector<cv::Point3f> &vP3FStasmPoints)
{	
	vP3FStasmPoints.clear();
	
	for(uint ii = 0; ii < m_oFeaturesPoints.size(); ++ii)
	{
		cv::Point3f l_oCurrPoint = oCloudMap.at<cv::Point3f>(m_oFeaturesPoints[ii].y, m_oFeaturesPoints[ii].x);
        std::cout << ii << " -> " << l_oCurrPoint << " | ";
        vP3FStasmPoints.push_back(l_oCurrPoint);
	}
}

void SWStasm::featuresPoints(std::vector<cv::Point2i> &oFeaturesPoints)
{
    oFeaturesPoints = m_oFeaturesPoints;
}

void SWStasm::setStasmMask(const cv::Mat &oCloudMap, cv::Mat &oStasmMask)
{
    oStasmMask = cv::Mat(oCloudMap.size(), CV_32SC1, cv::Scalar(0));

//    cout << "Stasm : " << oCloudMap.size() << " " << oStasmMask.size() << " " << m_oFeaturesPoints.size() << " # ";
	for(uint ii = 0; ii < m_oFeaturesPoints.size(); ++ii)
	{	        
		uint l_i32CoordX = m_oFeaturesPoints[ii].y;
		uint l_i32CoordY = m_oFeaturesPoints[ii].x;
//        cout << "[" << l_i32CoordX << "," << l_i32CoordY << "] ";
		oStasmMask.at<int>(l_i32CoordX, l_i32CoordY) = ii + 1;
	}
}

cv::Rect SWStasm::computeUpperFaceRect()
{
	cv::Rect l_oCurrUpperFaceRect;
	
	// nose
	// l_oCurrUpperFaceRect.x = m_oFeaturesPoints[41].x - 20;
	// l_oCurrUpperFaceRect.y = m_oFeaturesPoints[41].y - 45;
	
	// l_oCurrUpperFaceRect.height = 35;
	// l_oCurrUpperFaceRect.width = 40;
	
	
	// front
	l_oCurrUpperFaceRect.x = m_oFeaturesPoints[41].x - 30;
	l_oCurrUpperFaceRect.y = m_oFeaturesPoints[41].y - 75;
	
	l_oCurrUpperFaceRect.height = 25;
	l_oCurrUpperFaceRect.width = 60;	
	
	return l_oCurrUpperFaceRect;
}

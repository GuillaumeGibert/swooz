#include "devices/dimenco/SWDimenco3DDisplay.h"

#include "devices/rgbd/SWKinect.h"

	
SWDimenco3DDisplay::~SWDimenco3DDisplay(void)
{
	cvDestroyWindow("dimenco3D");
}


int SWDimenco3DDisplay::init(int msize) //the buffer 
{
	// retrieves display size and setups display constants
	if( msize!=23 && msize!=42  && msize!=55)
	{
		std::cout<<"-ERROR: Display size must be : 23 , 42 or 55"<<std::endl;
		std::cout<<"-ERROR: Display size was set to : " << msize << "!" <<std::endl;
		return -1;
	}
	else
	{
		displaySize = msize;
		// sets 23'' display	constants
		if(displaySize==23)
		{
			displayImgWidth  = 960;
			displayImgHeight = 540;

			Zd = 0.459813;
			vz = 6.180772;
			M  = -1586.34;
			C  = 127.5;
		}
		// sets 42'' display	constants
		if(displaySize==42)
		{
			displayImgWidth  = 960;
			displayImgHeight = 540;

			Zd = 0.467481;
			vz = 7.655192;
			M  = -1960.37;
			C  = 127.5;
		}
		// sets 55'' display	constants
		if(displaySize==55)
		{
			// Copied form the 42'' display, needs to be adjusted once the info is available
			displayImgWidth  = 960;
			displayImgHeight = 540;

			Zd = 0.467481;
			vz = 7.655192;
			M  = -1960.37;
			C  = 127.5;
		}
	}
	
	// creates a full screen cv window
	cvNamedWindow("dimenco3D", CV_WINDOW_NORMAL);
	cv::setWindowProperty("dimenco3D", CV_WND_PROP_FULLSCREEN ,CV_WINDOW_FULLSCREEN );
	cv::waitKey(10);
	
	return 0;
}





void SWDimenco3DDisplay::refresh(const cv::Mat& rgbImg, const cv::Mat& depthImg)
{
	// retrieves information on RGB original image
	//cvtColor(rgbImg, m_rgbImg, CV_BGR2RGB );
	m_rgbImg = rgbImg;
	originalRGBImgWidth = rgbImg.cols;
	originalRGBImgHeight= rgbImg.rows;
		
	// creates a padded RGB image
	cv::Mat paddedRGBImg( cv::Size(displayImgWidth, displayImgHeight), CV_8UC3, cv::Scalar::all(0) );
	padImageBorder( rgbImg, paddedRGBImg );
	//~ cv::imshow("zero",paddedRGBImg );
	// retrieves information on Depth image
	m_depthImg = depthImg;
	originalDepthImgWidth = depthImg.cols;
	originalDepthImgHeight= depthImg.rows;
		
	// converts the depth image into a disparity image
	cv::Mat disparityImg(cv::Size(originalDepthImgWidth, originalDepthImgHeight), CV_8UC3, cv::Scalar::all(0));
	depth2disparity(depthImg, disparityImg);
	
	// creates a padded Disparity Image
	cv::Mat paddedDisparityImg( cv::Size(displayImgWidth, displayImgHeight), CV_8UC3, cv::Scalar::all(0) );
	padImageBorder( disparityImg, paddedDisparityImg );

	// concats the RGB and Disparity images
	cv::Mat rgbdImg( cv::Size( paddedRGBImg.cols+paddedDisparityImg.cols, paddedRGBImg.rows ), CV_8UC3 );
	concatRGBDImages( paddedRGBImg, paddedDisparityImg, rgbdImg );

	// adds zero lines for even rows
	cv::Mat zeroAddedRGBDImg( cv::Size(rgbdImg.cols, rgbdImg.rows*2), CV_8UC3, cv::Scalar::all(0) );
	addZeroLines( rgbdImg, zeroAddedRGBDImg);

	// adds the 3D header, this way the display will interpret the image as 3D
	add3DHeader( zeroAddedRGBDImg ); //be aware of the Mat, the rgb order is actually bgr;

	// determines the fps
	static double freq = cv::getTickFrequency();
	static int64 tm = 0;
	int64 fps = static_cast<int64>(freq/(cv::getTickCount() - tm) );
	tm = cv::getTickCount();

    cv::putText( zeroAddedRGBDImg, "fps: " + swUtil::int2string(static_cast<int>(fps)), cv::Point( 15,50), cv::FONT_HERSHEY_SIMPLEX, 1, RED, 3 );
	cv::imshow("dimenco3D",zeroAddedRGBDImg );
	
}

void SWDimenco3DDisplay::depth2disparity(const cv::Mat& depthImg, cv::Mat& disparityImg)
{	
	int numRows = depthImg.rows;
	int numCols = depthImg.cols;
	
	for (int l_row=0; l_row<numRows; l_row++)
	{
		for (int l_col=0; l_col<numCols; l_col++)
		{
			double depthValue = depthImg.at<float>(l_row,l_col);
			int disparity = static_cast<int>(floor(M*(1 - vz/(depthValue - Zd + vz))+C));
			
			//~ if (l_row==numRows/2 && l_col==numCols/2)
				//~ std::cout << "disparity = " <<disparity <<std::endl;
			
			for(int k=0; k<disparityImg.channels(); k++) //all channels
			{
				disparityImg.at<cv::Vec3b>(l_row,l_col)[k] = disparity;
			}
			
		}
	}
	
//   cv::flip(disparityImg, disparityImg, 0);
	
}
	
	
	
void SWDimenco3DDisplay::padImageBorder( const cv::Mat& inputImg, cv::Mat& outputImg )
{
	cv::Rect roiPadRect( (displayImgWidth - inputImg.cols)/2, (displayImgHeight - inputImg.rows)/2, inputImg.cols, inputImg.rows ) ;
	cv::Mat roiPadImg( outputImg, roiPadRect );
	cv::addWeighted( roiPadImg, 1.0, inputImg, 1.0, 0., roiPadImg );
}


void SWDimenco3DDisplay::concatRGBDImages( const cv::Mat& pRgbImg, const cv::Mat& pDepthImg, cv::Mat& rgbdImg ) //join the padded images
{
	pRgbImg.copyTo(   rgbdImg( cv::Rect( 0,0, pRgbImg.cols, pRgbImg.rows )));
	pDepthImg.copyTo( rgbdImg( cv::Rect( pRgbImg.cols,0, pDepthImg.cols, pDepthImg.rows )));
}

void SWDimenco3DDisplay::addZeroLines(const cv::Mat& inputImg, cv::Mat& outputImg)
{
   	for(int i=0; i<inputImg.rows; i++)
	{
		inputImg.row(i).copyTo(outputImg.row(i*2));
	}
}

void SWDimenco3DDisplay::add3DHeader( cv::Mat& img )
{
    if (true)
	{
		unsigned char hd[24];

		hd[0] = 0xf0;
		hd[1] = 0x14;
		hd[2] = 0x00;
		hd[3] = 0x00;
		hd[4] = 0x01;
		hd[5] = 0x00;
		hd[6] = 0x3c;
		hd[7] = 0x02;
		hd[8] = 0x1c;
		hd[9] = 0x00;
		hd[10] = 0x00;
		hd[11] = 0x00;
		hd[12] = 0x00;
		hd[13] = 0x00;
		hd[14] = 0x00;
		hd[15] = 0x00;
		hd[16] = 0x00;
		hd[17] = 0x00;
		hd[18] = 0x00;
		hd[19] = 0x00;
		hd[20] = 0x31;
		hd[21] = 0x4f;
		hd[22] = 0x33;
		hd[23] = 0x84;
		//int len = x * 2 * (y * 2 - 1) * 3;
		embedBlueBit7B(img.ptr(), hd, 24);
	}
	else
	{	
		/* header information from Philips' white paper, firmware 10.7 and higher */
		/* encode Header information for Philips 3D solution */
		unsigned char hd[32];
		
		hd[0] = 0xf1; /* Header_ID:        11110001*/
		hd[1] = 0x01;    /* Hdr_Content_type: 00000001 -> 3D */
		hd[2] = 0x40;   /* factor: 64 by default*/
		hd[3] = 0x80;  /* offset: 128 by default*/
		hd[4] = 0x00; /*- select: 0-> optimal settings for the content type*/
		/* hd[4] 8th bit: Hdr_Factor_select: '1': Hdr_Factor is used; '0': Hdr_content_type is used */
		/* hd[4] 7th bit: Hdr_Offset_CC_select: '1': Hdr_Offset_CC is used; '0': Hdr_content_type is used */
		hd[5] = 0x00; /*reserved: 0
		/* 4 byte Error Detection Code, computed over the first 6 header bytes */
		/* you may use the crc32 table or use the standard header defined in the manual */
		/* We are lazy so we choose the 2nd option! */
		hd[6] = 0xc4;
		hd[7] = 0x2d;
		hd[8] = 0xd3;
		hd[9] = 0xaf;
		
		hd[10] = 0xf2;
		hd[11] = 0x14;
		hd[12] = 0x00;
		hd[13] = 0x00;
		hd[14] = 0x00;
		hd[15] = 0x00;
		hd[16] = 0x00;
		hd[17] = 0x00;
		hd[18] = 0x00;
		hd[19] = 0x00;
		hd[20] = 0x00;
		hd[21] = 0x00;
		hd[22] = 0x00;
		hd[23] = 0x00;
		hd[24] = 0x00;
		hd[25] = 0x00;
		hd[26] = 0x00;
		hd[27] = 0x00;
		hd[28] = 0x36;
		hd[29] = 0x95;
		hd[30] = 0x82;
		hd[31] = 0x21;
		// embed the header
		/* please note that this is just the first header part! */
		/* we do not copy the 2nd part because it is sufficient for the use of 2D+Depth format*/
		embedBlueBit7B(img.ptr(), hd, 32);
	}
}

void SWDimenco3DDisplay::embedBlueBit7B(unsigned char *data, unsigned char *hd, int len)
{
	/*static unsigned char bitval[] = {128, 64, 32, 16, 8, 4, 2, 1};*/
	static unsigned char bitval[] = {  1,   2,   4,   8,  16,  32,  64, 128};
	int lmax, i, ih, ib, pos;

	lmax = len * 8;
	ih = 0;
	ib = 7;
	for (i = 0, pos = 0; i < lmax; i ++, pos += 6)
	{
		if ((hd[ih] & bitval[ib]) == bitval[ib]) 
		{
			/*      fprintf(stderr, "hd[%d]=[0x%x], bit%d is '1': blue_data[%3d]=[0x%x]->", ih, hd[ih], ib, pos / 3, data[pos]);*/
			/*      data[po] = data[pos] | bitval[7];*/
			//~ data[pos] = 255;
			data[pos] = 255;
			/*      fprintf(stderr, "[0x%x]\n", data[pos]);*/
		}
		else 
		{
			/*      fprintf(stderr, "hd[%d]=[0x%x], bit%d is '0': blue_data[%3d]=[0x%x]->", ih, hd[ih], ib, pos / 3, data[pos]);*/
			/*      data[pos] = data[pos] & bitinv[7];*/
			data[pos] = 0;
			/*      fprintf(stderr, "[0x%x]\n", data[pos]);*/
		}
		ib --;
		if (ib == -1)
		{
			ih ++;
			ib = 7;
		}
	}
}

int main(int argc, char* argv[])
{
    // get the 3d monitor screen size
    int monitorSize = 23; //default is 23
    if(argc == 1)
    {
        std::cout<<"\nDefault size 23.\n"<<std::endl;
    }
    else //more than one argument, detect the second one as monitor size
    {
        monitorSize  = _ttoi(argv[1]);
    }

    if(monitorSize != 23 && monitorSize != 42 && monitorSize != 55)
    {
        std::cout<<"-ERROR: Display size must be : 23 , 42 or 55"<<std::endl;
        return -1;
    }

    SWDimenco3DDisplay swDimenco;
    swDevice::SWKinect swKinect;

    // initialization
    swKinect.init(0);
    swDimenco.init(monitorSize);

    for (;;)
    {
        // grabs a new set of images (rgb + depth)
        swKinect.grab();
        // swKinect.normalizeDepthImage();
        // displays them on the 3D display
        // swDimenco.refresh(swKinect.bgrImage,swKinect.normalizedDepthMap);
        swDimenco.refresh(swKinect.bgrImage,swKinect.normalizedDepthMap);
        // waits 30ms before looking for new images
        if( cv::waitKey( 16 ) >= 0 )
            break;
    }

    return 0;
}


// $imutil.hpp 3.0 milbo$ monochrome image utilities

#if !defined(imutil_hpp)
#define imutil_hpp

// namespace swStasm
// {
	static const int IM_NEAREST_PIXEL = 0;  // for fBilinear parameter
	static const int IM_BILINEAR      = 1;
	static const int IM_AVERAGE_ALL   = 2;

	static const bool IM_NO_WIDTH_DIVISIBLE_BY_4 = false; // for fWidthDivisibleBy4 param
	static const bool IM_WIDTH_DIVISIBLE_BY_4    = true;

	void ScaleImage(Image &Img,                                                     // io
		    const int nNewWidth, const int nNewHeight, const bool fBilinear);   // in

	void ReduceImage(Image &Img,                        // io
			 double Scale, int ReduceMethod);   // in

	void FillImage (Image &Img,         // io
			byte Color);        // in

	void FillRectangle(Image &Img,                                  // io
		       int ix1, int iy1, int ix2, int iy2, int Color);  // in

	void ReduceSizeBy1_2(Image &Dest,   // out
			     Image &Src);   //  in

	void ReduceImageAssign(Image &OutImg,                           // out
			       const Image &InImg, double Scale,        // in
			       int ReduceMethod, bool fVerbose=false);  // in

// };
#endif // imutil_hpp

// $rgbimutil.hpp 3.0 milbo$ RGB image utilities

#if !defined(rgbimgutil_hpp)
#define rgbimgutil_hpp

// namespace swStasm
// {


	void ScaleRgbImage(RgbImage &Img,                   // io
			   int nNewWidth, int nNewHeight,   // in
			   bool fVerbose, bool fBilinear);  // in

	void CropRgbImage(RgbImage &Img,                    // io
			  int nTopCrop, int nBottomCrop,    // in
			  int nLeftCrop, int nRightCrop,    // in
			  bool fWidthDivisibleBy4=false);   // in

	void ConvertRgbImageToGray(Image &OutImg,           // out
				   const RgbImage &Img);    // in

	void DesaturateRgbImage(RgbImage &Img);  // io: convert to gray

	#if _MSC_VER // these function are only supported under Windows

	void RgbPrintf(RgbImage &Img,                           // io
		       int ix, int iy, unsigned Color,          // in
		       int iFontSize, const char *pArgs, ...);  // in: args like printf

	void BeginRgbPrintf(RgbImage &Img);

	void DoRgbPrintf(int ix, int iy,
			 unsigned Color, int iFontSize,
			 const char *pArgs, ...);   // in: args like printf

	void EndRgbPrintf(RgbImage &Img);
// };

#endif // _MSC_VER

#endif // rgbimgutil_hpp

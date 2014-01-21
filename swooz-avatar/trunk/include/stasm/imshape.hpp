// $imshape.hpp 3.0 milbo$ routines for using shapes in images

#if !defined(imshape_hpp)
#define imshape_hpp
// namespace swStasm
// {

	double GetPyrScale(int iLev, double PyrRatio);

	void DrawPoint(RgbImage &Img,                 // io
	       int ix, int iy,                        // in
	       unsigned Red=0x7f, unsigned Green=0, unsigned Blue=0, // in: default is C_DRED
	       bool fTransparent=false);              // in

	void
	DrawPoint(RgbImage &Img,                      // io
		  double x, double y,                 // in
		  unsigned Red=0x7f, unsigned Green=0, unsigned Blue=0, // in: default is C_DRED
		  bool fTransparent=false);           // in

	void
	DrawCross(RgbImage &Img,                                   // io
		  double x, double y,                              // in
		  unsigned Red, unsigned Green, unsigned Blue);    // in

	void DrawShape(RgbImage &Img,                 // io
		const GslMat::SHAPE &Shape,                   // in
		unsigned Color=C_DRED,                // in
		bool fConnectTheDots = true,          // in
		unsigned ConnectTheDotsColor=-1,      // in: -1 means use default dark red
		bool fAnnotate = false,               // in
		int iAnnotateLandmark = -1);          // in: landmark to annotate, -1 for none

	void CropImageToShape (RgbImage &Img,         // io
		const GslMat::SHAPE &Shape,                   // in
		int ixMargin = 0,                     // in
		int iyMargin = 0);                    // in

	void ConvertToShape17(GslMat::SHAPE &Shape17, const GslMat::SHAPE &Shape);

	void GetPointCoords(double &x, double &y,                       // out
			    int iPoint, const GslMat::SHAPE &Shape);            // in

	GslMat::SHAPE End76ToMid76(const GslMat::SHAPE &Shape);
// };

#endif // imshape_hpp

// $image.hpp 3.0 milbo$ basic image class
// Note: this has been stripped down for the "stasm" build.
//       Refer to the original image.hpp if you want more functionality.

#if !defined(image_hpp)
#define image_hpp


#include "stasm.hpp" /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ADDED

// namespace swStasm
// {
	static const int MAX_IMAGE_DIM = 10000; // arbitrary, used for parameter sanity checking

	static const bool IM_NO_CLEAR = false;  // values for fClear parameter
	static const bool IM_CLEAR = true;

	typedef struct RGB_TRIPLE   // same layout as RGBTRIPLE in windows file wingdi.h
	    {
	    byte Blue, Green, Red;  // we assume packing is on byte boundaries
	    }                       // i.e. size of this is 3 bytes
	RGB_TRIPLE;

	// CIE conversion to gray using integer arithmetic, add 500 to take care of rounding
	static const inline byte RgbToGray (RGB_TRIPLE Rgb)
	    { return byte((299 * Rgb.Red + 587 * Rgb.Green + 114 * Rgb.Blue + 500) / 1000); }

	//-----------------------------------------------------------------------------
	template <typename T> class ImageT  // instantiate to ImageT<byte> or ImageT<RGB_TRIPLE>
	{
	public:
	T *buf;
	int width, height;

	ImageT() :buf(NULL), width(0), height(0) {} // null constructor

	ImageT(int width1, int height1, bool fClear=false):
		buf(NULL), width(width1), height(height1)
	    {
	    ASSERT(width1 >= 0 && width1 <= MAX_IMAGE_DIM &&
		height1 >= 0 && height1 <= MAX_IMAGE_DIM);
	    int nLen = width1 * height1;
	    if (nLen)
		{
		if (fClear)
		    buf = (T *)calloc(nLen, sizeof(T));
		else
		    buf = (T *)malloc(nLen * sizeof(T));
		}
	    }
	// construct by copying an image of another type
	template<typename S> ImageT(const ImageT<S> &other) :buf(NULL) { copy(other); }

	// construct by copying an image of same type
	ImageT(const ImageT &other) :buf(NULL) { copy(other); }

	// load image from a file
	ImageT(const char sFile[], bool fVerbose=false): buf(NULL)
	{
	    const char *s = sLoadImage(*this, sFile, fVerbose, true);
	    if (s)
		Err("%s", s);
	}

	~ImageT() { free(); }

	// get pixel at iRow,iCol
	T &operator()(int iRow, int iCol)
	    { return buf[iRow + iCol * width]; }

	const T &operator()(int iRow, int iCol) const
	    { return buf[iRow + iCol * width]; }

	T &operator()(int i) { return buf[i]; } // get pixel at i

	const T &operator()(int i) const { return buf[i]; }

	T pixel(int ix, int iy) // get pixel using SHAPE coords
	    { return buf[ix + width/2 + (iy + height/2) * width]; }

	const T pixel(int ix, int iy) const
	    { return buf[ix + width/2 + (iy + height/2) * width];}

	// Assignment copies the whole pixel buffer.  This is sometimes slower than
	// optimal, depending on what you are doing with "other" and "this" in your
	// subsequent code.  The advantage is that we don't need reference counts
	// because we don't share pixel buffers.
	// When necessary, this automatically redimensions the lhs to match the rhs
	// and does a type conversion.

	ImageT &operator=(const ImageT &other) { copy(other); return *this; }

	bool operator==(const ImageT<T> &other) const
	    {
	    if (width != other.width || height != other.height)
	      return false;
	    return 0 == memcmp(buf, other.buf, width * height * sizeof(T));
	    }
	bool operator!=(const ImageT &other) const { return !(*this == other); }

	void dim (int width1, int height1) // redim, contents random, quicker than dimClear
	    {
	    int nelems = width1 * height1;
	    if (nelems != width * height)
		{
		ASSERT(width1 >= 0 && width1 <= MAX_IMAGE_DIM &&
		    height1 >= 0 && height1 <= MAX_IMAGE_DIM);
		free();
		buf = (T *)malloc(nelems * sizeof(T));
		}
	    width = width1;
	    height = height1;
	    }
	void dimClear (int width1, int height1) const // redim and fill with zeroes
	    {
	    free();
	    ASSERT(width1 >= 0 && width1 <= MAX_IMAGE_DIM &&
		height1 >= 0 && height1 <= MAX_IMAGE_DIM);
	    buf = (T *)calloc(width1 * height1, sizeof(T));
	    width = width1;
	    height = height1;
	    }
	void free(void)
	    {
	    if (buf)
		{
		::free(buf);
		buf = NULL;
		}
	    width = 0;
	    height = 0;
	    }
	private:

	void copyBuf (T *pTo, const T *const pFrom, int width1, int height1)
	    { memcpy(pTo, pFrom, width1 * height1 * sizeof(T)); }

	// specialization for type conversion
	void copyBuf (byte *pTo, const RGB_TRIPLE *const pFrom, int width1, int height1)
	    {
	    for (int iy = 0; iy < height1; iy++)
		for (int ix = 0; ix < width1; ix++)
		    {
		    RGB_TRIPLE Pix = pFrom[ix + ((height1 - 1 - iy) * width1)];
		    pTo[ix + iy * width1] = RgbToGray(Pix);
		    }
	    }
	// specialization for type conversion
	void copyBuf (RGB_TRIPLE *pTo, const byte *const pFrom, int width1, int height1)
	    {
	    for (int iy = 0; iy < height1; iy++)
		for (int ix = 0; ix < width1; ix++)
		    {
		    RGB_TRIPLE Pix;
		    Pix.Red = Pix.Green = Pix.Blue = pFrom[ix + ((height1 - 1 - iy) * width1)];
		    pTo[ix + iy * width1] = Pix;
		    }
	    }
	template<typename S>
	void copy(const S &other)
	    {
	    if (static_cast<const void *>(this) != static_cast<const void *>(&other))
		{
		free();
		int nLen = other.width * other.height;
		buf = (T *)malloc(nLen * sizeof(T));
		width = other.width;
		height = other.height;
		copyBuf(buf, other.buf, width, height); // does type conversion when needed
		}
	    }
	};

	typedef ImageT<byte> Image;
	typedef ImageT<RGB_TRIPLE> RgbImage;
// };

#endif // image_hpp

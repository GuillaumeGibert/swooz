// $imutil.cpp 3.0 milbo$ monochrome image utilities

#include "stasm.hpp"

//-----------------------------------------------------------------------------
// Helper function for ScaleImage.  Actually a macro, for speed.
// using namespace swStasm;

static int igPos, igPos1; double gFrac;

#define INTERPOLATE_PIXEL(pIn, ix, Scale, Max)                          \
{                                                                       \
igPos = int(ix * Scale);                                                \
igPos1 = igPos + 1;                                                     \
if (igPos1 >= Max)                                                      \
    igPos1 = Max-1;                                                     \
gFrac = (ix * Scale) - igPos;                                           \
gFrac = int((1.0 - gFrac) * pIn[igPos] + gFrac * pIn[igPos1] + 0.5);    \
}

//-----------------------------------------------------------------------------
// If fBilinear is false we use the nearest pixel (usually a sharper image)
// I lifted the original version of this from Rowley img.cc:ReduceSize()

void
ScaleImage (Image &Img,                                                     // io
        const int nNewWidth, const int nNewHeight, const bool fBilinear)    // in
{
int   ix, iy;
const int width = Img.width;
const int height = Img.height;
const double scaleX = (double)width /nNewWidth;
const double scaleY = (double)height/nNewHeight;

if (width != nNewWidth || height != nNewHeight)
    {
    if (fBilinear)
        {
        // scale horizontally

        Image Out1(nNewWidth, height);
        byte *pIn = Img.buf;
        for (iy = 0; iy < height; iy++)
            {
            for (ix = 0; ix < nNewWidth; ix++)
                {
                INTERPOLATE_PIXEL(pIn, ix, scaleX, width);
                Out1(iy + ix * height) = (byte)gFrac;
                }
            pIn += width;
            }
        // scale vertically

        Img.dim(nNewWidth, nNewHeight);
        byte * const pOut = Img.buf;
        pIn = Out1.buf;

        for (iy = 0; iy < nNewWidth; iy++)
            {
            for (ix = 0; ix < nNewHeight; ix++)
                {
                INTERPOLATE_PIXEL(pIn, ix, scaleY, height);
                pOut[iy + ix * nNewWidth] = (byte)gFrac;
                }
            pIn += height;
            }
        }
    else    // nearest pixel
        {
        Image OutImg(nNewWidth, nNewHeight);
        for (iy = 0; iy < nNewHeight; iy++)
            {
            int iy1 = (iy * height) / nNewHeight;
            for (ix = 0; ix < nNewWidth; ix++)
                OutImg(ix, iy) = Img((ix * width) / nNewWidth, iy1);
            }
        Img = OutImg;
        }
    }
}

//-----------------------------------------------------------------------------
void
ReduceImage (Image &Img,                        // io
             double Scale, int ReduceMethod)    // in
{
if (!fEqual(Scale, 1, 1e-6))    // only reduce if we have to
    {
    int nNewWidth = iround(Img.width / Scale), nNewHeight = iround(Img.height / Scale);
    ASSERT(nNewWidth > 10 && nNewHeight > 10);  // 10 is rather arbitrary
    switch (ReduceMethod)
        {
        case IM_NEAREST_PIXEL:
		ScaleImage(Img, nNewWidth, nNewHeight, IM_NEAREST_PIXEL);
            break;
        case IM_BILINEAR:
            ScaleImage(Img, nNewWidth, nNewHeight, IM_BILINEAR);
            break;
        case IM_AVERAGE_ALL:
        default:
            Err("ReduceImage: bad ReduceMethod");
            break;
        }
    }
}

//-----------------------------------------------------------------------------
void
FillImage (Image &Img,      // io
           byte Color)      // in
{
for (int i = 0; i < Img.width * Img.height; i++)
    Img.buf[i] = Color;
}

//-----------------------------------------------------------------------------
void
FillRectangle (Image &Img,                                          // io
               int ix1, int iy1, int ix2, int iy2, int Color)       // in
{
for (int iy = iy1; iy < iy2; iy++)
    if (iy >= 0 && iy < Img.height)
        for (int ix = ix1; ix < ix2; ix++)
            if (ix >= 0 && ix < Img.width)
                Img(ix, iy) = (byte)Color;
}

//-----------------------------------------------------------------------------
// Scale the given byte pImg down by a factor of 1.2, putting the result
// in the given destination (which can be the same as the source).  The
// scaling uses bilinear interpolation, implemented by two steps of linear
// interpolation: first scaling in the X direction, then in the Y direction.

void
ReduceSizeBy1_2 (Image &Dest,       // out
                 Image &Src)        // in
{
#if 1
// This version is based on Henry Rowleys's original code.
// I've kept it for backwards compatibility in regression tests.

int width = Src.width;
int height = Src.height;
int nNewWidth = int(width / 1.2);
int nNewHeight = int(height / 1.2);
double scaleX = 1.2, scaleY = 1.2;

// first scale in X

Image Tmp(nNewWidth, height);
int x, y;
byte *pIn = Src.buf;
for (y = 0; y < height; y++)
    {
    for (x = 0; x < nNewWidth; x++)
        {
        int pos = int(x * scaleX);
        double frac = (x * scaleX) - pos;
        double val = (1.0 - frac) * pIn[pos] + frac * pIn[pos + 1];
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        Tmp(y + x * height) = (byte)(val + 0.5);
        }
    pIn += width;
    }
// scale in Y

Image Tmp2(nNewWidth, nNewHeight);
pIn = Tmp.buf;
for (y = 0; y < nNewWidth; y++)
    {
    for (x = 0; x < nNewHeight; x++)
        {
        int pos = int(x * scaleY);
        double frac = (x * scaleY) - pos;
        double val = (1.0 - frac) * pIn[pos] + frac * pIn[pos + 1];
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        Tmp2(y + x * nNewWidth) = (byte)(val + 0.5);
        }
    pIn += height;
    }
Dest = Tmp2;

#else
// This is a new version.  It gives slightly different results from the above version
// because of small differences in pixel conversion

Dest = Src; // could be made more efficient since we are overwriting Dest?

ScaleImage(Dest, iround(Src.width / 1.2), iround(Src.height / 1.2), IM_BILINEAR);

#endif
}

//-----------------------------------------------------------------------------
// Like ReduceImage but output image is same as input image -- this
// is faster because we don't need to copy memory.

void ReduceImageAssign (Image &OutImg,                                                      // out
                        const Image &InImg, double Scale, int ReduceMethod, bool fVerbose)  // in
{
// if (fVerbose)
    // lprintf("Reduce %g ", Scale);

if (fEqual(Scale, 1, 1e-3)) // an optimization for speed
    OutImg = InImg;
else
    {
    int iScale, ix, iy;
    int nNewWidth = iround(InImg.width / Scale), nNewHeight = iround(InImg.height / Scale);
    if (nNewWidth < 10 || nNewHeight < 10)  // 10 is rather arbitrary
        Err("ReduceImageAssign: image too small, nNewWidth %d nNewHeight %d", nNewWidth, nNewHeight);
    switch (ReduceMethod)
        {
        case IM_NEAREST_PIXEL:
            OutImg.dim(nNewWidth, nNewHeight);
            for (iy = 0; iy < nNewHeight; iy++)
                {
                int iy1 = (iy * InImg.height) / nNewHeight;
                for (ix = 0; ix < nNewWidth; ix++)
                    OutImg(ix, iy) = InImg((ix * InImg.width) / nNewWidth, iy1);
                }
            break;
        case IM_BILINEAR:
            OutImg = InImg;             //TODOSTASMS make this more efficient?
            ScaleImage(OutImg, nNewWidth, nNewHeight, IM_BILINEAR);
            break;
        case IM_AVERAGE_ALL:
            ASSERT(fEqual(floor(Scale), Scale, 1e-3));  // scale is an integer?
            iScale = (int)Scale;        // for efficiency do calculations using ints not doubles
            OutImg.dim(nNewWidth, nNewHeight);
            for (iy = 0; iy < nNewHeight; iy++)
                for (ix = 0; ix < nNewWidth; ix++)
                    {
                    int Pixel = 0;

                    for (int j = 0; j < iScale; j++)
                        for (int i = 0; i < iScale; i++)
                            Pixel += InImg((ix * iScale) + i, iy * iScale + j);

                    OutImg(ix, iy) = (byte)(Pixel / (iScale * iScale));
                    }
            break;
        default:
            Err("CONF_ReduceMethod");
            break;
        }
    }
}

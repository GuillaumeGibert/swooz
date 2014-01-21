// $rgbimutil.cpp 3.0 milbo$ image utilities
// milbo durban dec05

// #include "ezfont.h"
#include "stasm.hpp"
// using namespace swStasm;
//-----------------------------------------------------------------------------
// Helper function for ScaleRgbImage
// TODOSTASMS turn this into a macro? inlining here is possibly ignored by MS compiler

static inline RGB_TRIPLE InterpolateRgbPixel (const RGB_TRIPLE *pIn,
                                        int ix, double Scale, int Max)  // in
{
int iPos = (int)(ix * Scale);
int iPos1 = iPos + 1;
if (iPos1 >= Max)   // don't overrun buffer
    iPos1 = Max-1;
RGB_TRIPLE Rgb;
for (int iColor = 0; iColor < 3; iColor++)
    {
    int c0, c1;
    switch (iColor)
        {
        case 0:     c0 = pIn[iPos].Red;     c1 = pIn[iPos1].Red;   break;
        case 1:     c0 = pIn[iPos].Green;   c1 = pIn[iPos1].Green; break;
        default:    c0 = pIn[iPos].Blue;    c1 = pIn[iPos1].Blue;  break;
        }
    double Frac = (ix * Scale) - iPos;
    double Value = (1.0 - Frac) * c0 + Frac * c1;   // linear interpolation
    byte iValue = (byte)(Value + 0.5);
    switch (iColor)
        {
        case 0:     Rgb.Red =   iValue; break;
        case 1:     Rgb.Green = iValue; break;
        default:    Rgb.Blue =  iValue; break;
        }
    }
return Rgb;
}

//-----------------------------------------------------------------------------
// Scale using two steps of linear interpolation: first in the X direction,
// then in the Y direction
//
// If fBilinear is false we use the nearest pixel (and get a sharper image)
//
// Is the following a bug? If size is reduced by more than 2, this ignores some
// pixels in the input image, even when fBilinear is true -- because we look
// a max of 2 pixels when doing bilinear interpretation.
// Use ReduceRgbImage if this matters to you.
//
// I lifted the original version of this from Henry Rowley img.cc:ReduceSize()

void ScaleRgbImage (RgbImage &Img,                  // io
                    int nNewWidth, int nNewHeight,  // in
                    bool fVerbose, bool fBilinear)  // in
{
int ix, iy;
int width = Img.width;
int height = Img.height;
double scaleX = (double)width  / nNewWidth;
double scaleY = (double)height / nNewHeight;

if (fVerbose)
    {
    // if (scaleX > 1)
        // lprintf("ScaleDown %.2g ", 1/scaleX);
    // else if (scaleX < 1)
        // lprintf("ScaleUp %.2g ", 1/scaleX);
    // else
        // lprintf("Scale %.2g ", 1/scaleX);
    }
if (width == nNewWidth && height == nNewHeight)
    return;                                         // NOTESTASM: RETURN

if (!fBilinear)
    {
    // use nearest pixel

    RgbImage OutImg(nNewWidth, nNewHeight);
    for (iy = 0; iy < nNewHeight; iy++)
        for (ix = 0; ix < nNewWidth; ix++)
            OutImg(ix, iy) = Img((ix * width) / nNewWidth, (iy * height) / nNewHeight);

    Img = OutImg;
    }
else
    {
    // scale horizontally

    RgbImage Out1(height, nNewWidth);
    RGB_TRIPLE *pIn = Img.buf;
    for (iy = 0; iy < height; iy++)
        {
        for (ix = 0; ix < nNewWidth; ix++)
            Out1(iy + ix * height) = InterpolateRgbPixel(pIn, ix, scaleX, width);
        pIn += width;
        }
    // scale vertically

    Img.dim(nNewWidth, nNewHeight);
    RGB_TRIPLE *pOut = Img.buf;
    pIn = Out1.buf;

    for (ix = 0; ix < nNewWidth; ix++)
        {
        for (iy = 0; iy < nNewHeight; iy++)
            pOut[ix + iy * nNewWidth] = InterpolateRgbPixel(pIn, iy, scaleY, height);
        pIn += height;
        }
    }
}

//-----------------------------------------------------------------------------
void CropRgbImage (RgbImage &Img,                   // io
                   int nTopCrop, int nBottomCrop,   // in
                   int nLeftCrop, int nRightCrop,   // in
                   bool fWidthDivisibleBy4)         // in
{
int width = Img.width;
int height = Img.height;
if (fWidthDivisibleBy4)
    nLeftCrop += (Img.width - nRightCrop - nLeftCrop) % 4; // ensure width is div by 4
int nNewWidth = Img.width - nLeftCrop - nRightCrop;
int nNewHeight = Img.height - nTopCrop - nBottomCrop;
if (nTopCrop < 0 || nBottomCrop < 0 || nLeftCrop < 0 || nRightCrop < 0)
    Err("you can't specify a crop less than 0");
if (nNewWidth <= 0)
    Err("specified left or right crop would cause a width less than or equal to zero");
if (nNewWidth > width)
    Err("specified left or right crop would cause a width bigger than current width");
if (nNewHeight <= 0)
    Err("specified top or bottom crop would cause a height less than or equal to zero");
if (nNewHeight > height)
    Err("specified top or bottom crop would cause a height bigger than current height");

RgbImage OutImg(nNewWidth, nNewHeight);

for (int iy = 0; iy < nNewHeight; iy++)
    for (int ix = 0; ix < nNewWidth; ix++)
        OutImg(ix, iy) = Img(ix + nLeftCrop, iy + nBottomCrop);

Img = OutImg;
}

//-----------------------------------------------------------------------------
void ConvertRgbImageToGray (Image &OutImg,          // out
                            const RgbImage &Img)    // in
{
int width = Img.width;
int height = Img.height;

OutImg.dim(width, height);

for (int iy = 0; iy < height; iy++)
    for (int ix = 0; ix < width; ix++)
        OutImg(ix, iy) = (byte)RgbToGray(Img(ix + ((height - 1 - iy) * width)));
}

//-----------------------------------------------------------------------------
void DesaturateRgbImage (RgbImage &Img)  // io: convert to gray
{
int width = Img.width;
int height = Img.height;

for (int iy = 0; iy < height; iy++)
    for (int ix = 0; ix < width; ix++)
        Img(ix, iy).Red = Img(ix, iy).Green = Img(ix, iy).Blue =
            RgbToGray(Img(ix, iy));
}

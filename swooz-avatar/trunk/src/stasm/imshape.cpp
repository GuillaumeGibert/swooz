// $imshape.cpp 3.0 milbo$ routines for using shapes in images
//-----------------------------------------------------------------------------
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// A copy of the GNU General Public License is available at
// http://www.r-project.org/Licenses/
//-----------------------------------------------------------------------------

#include "stasm.hpp"
// using namespace swStasm;
// if true, skip lines connecting extra eye points >= 68 in DrawShape
// i.e. if true, draw the shape using the same points as an XM2VTS shape

static const bool SKIP_EXTRA_EYE_LINES_IN_DISPLAYED_SHAPE = true;

// shapes wider than this will be drawn with thicker lines, makes line more visible

int igThickenShapeWidth = 300;

//-----------------------------------------------------------------------------
double GetPyrScale (int iLev, double PyrRatio)
{
return pow(PyrRatio, iLev);
}

//-----------------------------------------------------------------------------
void
DrawPoint (RgbImage &Img,                                   // io
           int ix, int iy,                                  // in
           unsigned Red, unsigned Green, unsigned Blue,     // in
           bool fTransparent)                               // in
{
int width = Img.width, height = Img.height;
ix += width / 2;       // convert shape to image coords
iy += height / 2;
int i =  ix + iy * width;
if (i > 0 && i < width * height)
    {
    if (fTransparent)
        {
        Img(ix, iy).Red   = byte(Img(ix, iy).Red / 2   + Red / 2);
        Img(ix, iy).Green = byte(Img(ix, iy).Green / 2 + Green / 2);
        Img(ix, iy).Blue  = byte(Img(ix, iy).Blue / 2  + Blue / 2);
        }
    else
        {
        Img(ix, iy).Red = byte(Red);
        Img(ix, iy).Green = byte(Green);
        Img(ix, iy).Blue = byte(Blue);
        }
    }
}

//-----------------------------------------------------------------------------
// same as above but with double arguments

void
DrawPoint (RgbImage &Img,                                   // io
           double x, double y,                              // in
           unsigned Red, unsigned Green, unsigned Blue,     // in
           bool fTransparent)                               // in
{
DrawPoint(Img, iround(x), iround(y), Red, Green, Blue, fTransparent);
}

//-----------------------------------------------------------------------------
void
DrawCross (RgbImage &Img,                               // io
           double x, double y,                          // in
           unsigned Red, unsigned Green, unsigned Blue) // in
{
DrawPoint(Img, x-2, y-2, Red, Green, Blue);
DrawPoint(Img, x-1, y-1, Red, Green, Blue);
DrawPoint(Img, x-1, y+1, Red, Green, Blue);
DrawPoint(Img, x-2, y+2, Red, Green, Blue);
DrawPoint(Img, x+2, y-2, Red, Green, Blue);
DrawPoint(Img, x+1, y-1, Red, Green, Blue);
DrawPoint(Img, x+1, y+1, Red, Green, Blue);
DrawPoint(Img, x+2, y+2, Red, Green, Blue);
}

//-----------------------------------------------------------------------------
// This function uses a Bresenham-like algorithm to draw a line from x0,y0
// to x1,y1.  Based on "Bresenham-based supercover line algorithm" by
// Eugen Dedu www.ese-metz.fr/~dedu/projects/bresenham.

static void
DrawLine (RgbImage &Img,                            // io
    int x0, int y0, int x1, int y1,                 // in
    unsigned Red, unsigned Green, unsigned Blue,    // in
    bool fTransparent=false,                        // in
    bool fThick=false)                              // in
{
int i;               // loop counter
int yStep, xStep;    // the step on y and x axis
int Error;           // the error accumulated during the increment
int ddy, ddx;
int x = x0, y = y0;
int dx = x1 - x0;
int dy = y1 - y0;

DrawPoint(Img, x, y, Red, Green, Blue, fTransparent); // first point
if (fThick)
    DrawPoint(Img, x+1, y+1, Red, Green, Blue, fTransparent);

if (dy < 0)
    {
    yStep = -1;
    dy = -dy;
    }
else
    yStep = 1;

if (dx < 0)
    {
    xStep = -1;
    dx = -dx;
    }
else
    xStep = 1;

ddy = 2 * dy;
ddx = 2 * dx;

if (ddx >= ddy)                 // first octant (0 <= slope <= 1)
    {
    Error = dx;                 // start in the middle of the square
    for (i = 0; i < dx; i++)    // do not use the first point (already done)
        {
        x += xStep;
        Error += ddy;
        if (Error > ddx)        // increment y if AFTER the middle (>)
            {
            y += yStep;
            Error -= ddx;
            }
       DrawPoint(Img, x, y, Red, Green, Blue, fTransparent);
        if (fThick)
            DrawPoint(Img, x+yStep, y+xStep, Red, Green, Blue, fTransparent);
        }
    }
else
    {
    Error = dy;
    for (i = 0; i < dy; i++)
        {
        y += yStep;
        Error += ddx;
        if (Error > ddy)
            {
            x += xStep;
            Error -= ddy;
            }
        DrawPoint(Img, x, y, Red, Green, Blue, fTransparent);
        if (fThick)
            DrawPoint(Img, x+yStep, y+xStep, Red, Green, Blue, fTransparent);
        }
    }
}

//-----------------------------------------------------------------------------
static void GetPrevAndNext (int &iPrev, int &iNext,              // out
                            int iPoint, const SHAPE &Shape)      // in
{
const int nPoints = static_cast<int>(Shape.nrows());
iNext = iPoint;
int nCount = 0; // safety counter, needed if no points are used
do
    {
    iNext++;
    if (iNext >= nPoints)
        iNext = 0;
    }
while(nCount++ < nPoints && !fPointUsed(Shape, iNext));

iPrev = iPoint;
nCount = 0;
do
    {
    iPrev--;
    if (iPrev < 0)
        iPrev = nPoints-1;
    }
while(nCount++ < nPoints && !fPointUsed(Shape, iPrev));
}

//-----------------------------------------------------------------------------
// Get the coords of iPoint
// If iPoint is not used, synthesize the coords from the previous and next points.

void GetPointCoords (double &x, double &y,                        // out
                     int iPoint, const SHAPE &Shape)             // in
{
if (fPointUsed(Shape, iPoint))
    {
    x = Shape(iPoint, VX);
    y = Shape(iPoint, VY);
    }
else
    {
    int iPrev, iNext;
    GetPrevAndNext(iPrev, iNext, iPoint, Shape);
    int iPrev1 = iPrev, iNext1 = iNext;   // handle wraparound
    if (iPrev > iPoint)
        iPrev1 -= static_cast<int>(Shape.nrows());
    if (iNext < iPoint)
        iNext1 += static_cast<int>(Shape.nrows());
    double ScalePrev = double(iNext1 - iPoint) / (iNext1 - iPrev1);
    double ScaleNext = double(iPoint - iPrev1) / (iNext1 - iPrev1);
    x = ScalePrev * Shape(iPrev, VX) + ScaleNext * Shape(iNext, VX);
    y = ScalePrev * Shape(iPrev, VY) + ScaleNext * Shape(iNext, VY);
    }
}

//-----------------------------------------------------------------------------
// Reorder the points of a 76 point shape to display the eye landmarks nicely.
// We assume here that a 76 point shape must be a shape from the MUCT database.

SHAPE End76ToMid76 (const SHAPE &Shape)
{
ASSERT(Shape.nrows() == 76);
static SHAPE Shape1(76, 2);
for (unsigned i = 0; i < Shape1.nrows(); i++)
    {
    int j = iEnd76ToMid76[i];
    Shape1(i, VX) = Shape(j, VX); Shape1(i, VY) = Shape(j, VY);
    }
return Shape1;
}


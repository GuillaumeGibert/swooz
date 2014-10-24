// $prof.cpp 3.0 milbo$ routines for ASM profiles
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

double CONF_NormalizedProfLen = 1;      // root mean square length of profiles

//-----------------------------------------------------------------------------
double GetX (double X, int iOffset, int iOrthOffset,    // all in
                double DeltaX, double DeltaY)
{
    return X + (iOffset * DeltaX) + (iOrthOffset * DeltaY);
}

double GetY (double Y, int iOffset, int iOrthOffset,    // all in
                double DeltaX, double DeltaY)
{
    return Y + (iOffset * DeltaY) - (iOrthOffset * DeltaX);
}

static inline int iGetX (double X, int iOffset, int iOrthOffset, // all in
                            double DeltaX, double DeltaY)
{
    return iround(X + (iOffset * DeltaX) + (iOrthOffset * DeltaY));
}

static inline int iGetY (double Y, int iOffset, int iOrthOffset, // all in
                            double DeltaX, double DeltaY)
{
    return iround(Y + (iOffset * DeltaY) - (iOrthOffset * DeltaX));
}

//-----------------------------------------------------------------------------
int
nGetProfWidth (int nCols, unsigned ProfSpec)    // in: all
{
if (!IS_2D(ProfSpec))                       // one dimensional profile?
    return nCols;
const int nRoot = int(sqrt(double(nCols)));
ASSERT(SQ(nRoot) == nCols); // check that nCols is indeed a squared integer
return nRoot;
}

//-----------------------------------------------------------------------------
// return normalized vector bisector of three ordered points

static StasmVec
GetBisector (const StasmVec &Prev, const StasmVec &This, const StasmVec &Next) // all in
{
static StasmVec p(2, ROWVEC), q(2, ROWVEC);  // static to reduce number of mallocs

ASSERT(!(Prev(VX) == 0 && Prev(VY) == 0));
ASSERT(!(This(VX) == 0 && This(VY) == 0));
ASSERT(!(Next(VX) == 0 && Next(VY) == 0));

static StasmVec d0; d0.assign(This); d0 -= Prev; d0.normalizeMe();

p(VX) = d0(VY);                                 // rotate 90 degrees
p(VY) = -d0(VX);

static StasmVec d1; d1.assign(Next); d1 -=This; d1.normalizeMe();

q(VX) = d1(VY);
q(VY) = -d1(VX);

StasmVec d(p); d += q; d.normalizeMe();

if (fabs(d(VX)) < 1e-10 && fabs(d(VY)) < 1e-10) // are Prev and Next in same line?
    return (This - Prev).normalize();           // yes, avoid numerical issues

return d;
}

//-----------------------------------------------------------------------------
// Get pixel in Img at ix and iy (which are SHAPE coords), with index range forcing.
//
// SHAPE coords: 0,0 is center of image
//               image spans -width/2 to width/2, -height/2 to height/2

static int
iGetPixel (const Image &Img, int ix, int iy)    // in: all
{
const int width = Img.width;
const int height = Img.height;
ix += width/2;     // convert SHAPE to Image coords
iy += height/2;
if (ix < 0)
    ix = 0;
else if (ix > width-1)
    ix = width-1;
if (iy < 0)
    iy = 0;
else if (iy > height-1)
    iy = height-1;

return Img(ix, (height - 1 - iy)); // height-1-iy min is 0 when iy at max=height-1
}

//-----------------------------------------------------------------------------
static void GetPrevNextLandmarks (int &iPrev, int &iNext,   // out
                    const SHAPE &Shape, const int iPoint,   // in
                    const LANDMARK LandTab[],               // in
                    const bool fExplicitPrevNext)           // in
{
const int nPoints = static_cast<int>(Shape.nrows());
if (fExplicitPrevNext)
    {
    iPrev = LandTab[iPoint].iPrev;
    if (iPrev < 0)
        iPrev = (iPoint + nPoints - 1) % nPoints;
    iNext = LandTab[iPoint].iNext;
    if (iNext < 0)
        iNext = (iPoint + 1) % nPoints;
    }
else // use previous and next index as iPrev and iNext
    {
    iPrev = iPoint;
    if (--iPrev < 0)
        iPrev = nPoints-1;

    iNext = iPoint;
    if (++iNext >= nPoints)
        iNext = 0;
    }
ASSERT(fPointUsed(Shape, iPrev));
ASSERT(fPointUsed(Shape, iNext));
}

//-----------------------------------------------------------------------------
// Get normalized step size so we take single pixel steps.
// So either DeltaX or DeltaY will be +-1, and the other will be smaller than +-1.
//
// *pDeltaX is along whisker, *pDeltaY is orthogonal to whisker

void
GetProfStepSize (double &DeltaX, double &DeltaY,    // out
                 const SHAPE &Shape, int iPoint,    // in
                 const LANDMARK LandTab[],          // in
                 const bool fExplicitPrevNext)      // in
{
ASSERT(fPointUsed(Shape, iPoint));

int iPrev, iNext;
GetPrevNextLandmarks(iPrev, iNext,
                     Shape, iPoint, LandTab, fExplicitPrevNext);
static StasmVec PrevRow; PrevRow.dim(Shape.ncols(), ROWVEC); PrevRow = Shape.row(iPrev);
static StasmVec NextRow; NextRow.dim(Shape.ncols(), ROWVEC); NextRow = Shape.row(iNext);

static StasmVec Whisker;
Whisker.assign(GetBisector(PrevRow, Shape.row(iPoint), NextRow));

DeltaX = Whisker(VX);
DeltaY = Whisker(VY);
const double AbsDeltaX = fabs(DeltaX);
const double AbsDeltaY = fabs(DeltaY);
if (AbsDeltaX >= AbsDeltaY)
    {
    DeltaY /= AbsDeltaX;
    DeltaX /= AbsDeltaX;
    }
else
    {
    DeltaX /= AbsDeltaY;
    DeltaY /= AbsDeltaY;
    }
}

//-----------------------------------------------------------------------------
// variables for PrepareProf and GetProf routines

// max number of elems in a profile including nPixSearch on each end
static const int MAX_PROF_WIDTH_1D = 50;

static StasmVec          gProf(MAX_PROF_WIDTH_1D, MAX_PROF_WIDTH_1D);
static int          ngProfWidth;
static int          igProfPoint;    // for sanity checking
static const byte   *pgProfImage;   // ditto
static unsigned     gSubProfSpec;   // ditto

//-----------------------------------------------------------------------------
// If you want multiple 1D profiles along one whisker, then
//      i.  call PrepareProf1D once with nProfWidth set to the length
//          of the entire prof
//      ii. call Get1dProf multiple times, incrementing iOffset each time
//
// If you want just a single 1D profile, do the above at a single iOffset.
//
// The two-step approach is for speed since we only have to prepare the
// profile once for multiple iOffset's.  (We don't need the two-step
// approach for 2D profiles because we don't use whiskers for 2D profs and
// thus don't have to sample along a whisker).
//
// Shape is used for figuring out the direction of the whisker.  The whisker
// center point is x and y (which will not be on the shape if iOrthOffset!=0).
// iOrthOffset is the offset from the point orthogonally to the whisker.
//
// Note on direction of "previous":
// if whisker is vertical, PrevPix is below this pixel

void
PrepareProf1D (const Image &Img,            // in: all
               const StasmMat &Shape,
               unsigned SubProfSpec,
               const LANDMARK LandTab[],
               int iPoint,
               int nProfWidth,
               int iOrthOffset,
               bool fExplicitPrevNext)
{
ASSERT(!IS_2D(SubProfSpec));
ASSERT(nProfWidth < MAX_PROF_WIDTH_1D);
ASSERT((SubProfSpec & PROF_TBits) == PROF_Grad);

ngProfWidth = nProfWidth;
igProfPoint = iPoint;       // for sanity checking in Get1dProf later
pgProfImage = Img.buf;      // ditto
gSubProfSpec = SubProfSpec; // ditto

// number of profile sample points
// nProfWidth is +-nSamplePoints and middle point
int nSamplePoints = (nProfWidth - 1) / 2;

double DeltaX; // steps we move away along whisker from iPoint when sampling image
double DeltaY; // DeltaX is along whisker, DeltaY is orthogonal to whisker
GetProfStepSize(DeltaX, DeltaY,
    Shape, iPoint, LandTab, fExplicitPrevNext);

double x = GetX(Shape(iPoint, VX), 0, iOrthOffset, DeltaX, DeltaY);
double y = GetY(Shape(iPoint, VY), 0, iOrthOffset, DeltaX, DeltaY);

double PrevPix = iGetPixel(Img,
                    iGetX(x, -nSamplePoints-1, 0, DeltaX, DeltaY),
                    iGetY(y, -nSamplePoints-1, 0, DeltaX, DeltaY));

for (int iSample = -nSamplePoints; iSample <= nSamplePoints; iSample++)
    {
    int ix = iGetX(x, iSample, 0, DeltaX, DeltaY);
    int iy = iGetY(y, iSample, 0, DeltaX, DeltaY);
    double Pix = iGetPixel(Img, ix, iy);
    gProf(0, iSample + nSamplePoints) = Pix - PrevPix;
    PrevPix = Pix;
    }
}

//-----------------------------------------------------------------------------
static void
Normalize1d (StasmVec &Prof,             // io
             unsigned SubProfSpec)  // in
{
ASSERT(!IS_2D(SubProfSpec));
ASSERT((SubProfSpec & PROF_NormalizationField) == PROF_Flat);

const double Sum = Prof.absSum() / (Prof.ncols() * CONF_NormalizedProfLen);
if (!fEqual(Sum, 0))
    Prof /= Sum;
}

//-----------------------------------------------------------------------------
// Use this after preparing global vars by calling PrepareProf1D.
// iOffset is offset along whisker from point, can be +ve or -ve.

void
Get1dProf (StasmVec &Prof,                                                   // out
     unsigned SubProfSpec, const Image &Img, int iPoint, int iOffset)   // in
{
ASSERT(!IS_2D(SubProfSpec));
ASSERT((SubProfSpec & PROF_FBit) == 0);

// sanity checks -- make sure PrepareProf1D was called correctly

ASSERT(iPoint == igProfPoint);
ASSERT(Img.buf == pgProfImage);
ASSERT(SubProfSpec == gSubProfSpec);

const int nProfWidth = static_cast<int>(Prof.ncols());      // +-nSamplePoints and middle point
Prof = gProf.view(0,
            ngProfWidth/2 + iOffset - nProfWidth/2, 1, nProfWidth);

Normalize1d(Prof, SubProfSpec);
}

//-----------------------------------------------------------------------------
static void
Normalize2d (StasmVec &Prof,                                     // io
    const unsigned SubProfSpec, const double SigmoidScale)  // in
{
ASSERT(IS_2D(SubProfSpec));
ASSERT((SubProfSpec & PROF_NormalizationField) == PROF_SigmAbsSum);

const int nelems = static_cast<int>(Prof.nelems());
int iCol = 0;
double * const pData = Prof.m->data; // for efficiency, access mat buf directly

// In unusual conditions, AbsSum==0 with small profile lengths, hence
// the test against AbsSum != 0.
// Note that we scale but don't center here because
// we don't want to center an all positive profile.

const double AbsSum = Prof.absSum();
if (AbsSum != 0)
    {
    const double Scale = nelems * CONF_NormalizedProfLen / AbsSum;
    if (SigmoidScale == 0) // treat separately for speed
        {
        while (iCol < nelems)
            {
        pData[iCol] *= Scale;
            iCol++;
            }
        }
    else
        {
        const double SigmoidScale10 = SigmoidScale / 10;
        while (iCol < nelems)
            {
            pData[iCol] *= Scale;
            pData[iCol] /= fabs(pData[iCol]) + SigmoidScale10;
            iCol++;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Get the profiles from Grads.
// The Grads arrays must be initialized before calling this function.

static void
Get2dProfFromGradMats (StasmVec &Prof,                                // out
    unsigned ProfSpec, const StasmMat &Grads, const SHAPE &Shape,     // in
    int iPoint, int ixOffset, int iyOffset, int nProfWidth)      // in
{
ASSERT(IS_ODD(nProfWidth));
ASSERT(Grads.nrows() && Grads.ncols());
    // ensure gradient matrices are inited, done previously by InitGradsIfNeeded

ASSERT((ProfSpec & PROF_WindowField) == PROF_WindowEquallyWeighted);

const int nProfWidthEachSide = (nProfWidth - 1) / 2;
const int nRows = static_cast<int>(Grads.nrows());
const int nCols = static_cast<int>(Grads.ncols());

// convert shape coords to mat coords,
// offset by ixOffset and iyOffset and nProfWidthEachSide

int iy = iround(Shape(iPoint, VY)) - iyOffset;
int ix = iround(Shape(iPoint, VX)) - ixOffset;

const int iyMin = nRows/2 - 1 - iy - nProfWidthEachSide;
const int iyMax = nRows/2 - 1 - iy + nProfWidthEachSide;
const int ixMin = nCols/2     + ix - nProfWidthEachSide;
const int ixMax = nCols/2     + ix + nProfWidthEachSide;

// ensure we don't overrun Prof
ASSERT(Prof.nelems() == unsigned(nProfWidth * nProfWidth));

double *pData = Prof.m->data; // for speed, access mat buf directly
const double * const pGrad = Grads.m->data;
const int Tda = static_cast<int>(Grads.m->tda);

// For efficiency there are two loops: one to hande the case where the
// profile is entirely in the image boundaries and one for the case where
// the profile goes off the edge the edge of the image.

if (ixMin >= 0 && ixMax < nCols && iyMin >= 0 && iyMax < nRows) // in image boundary?
    {
    // prof is in the image
#if 1 // original code
    for (iy = iyMin; iy <= iyMax; iy++)
        {
        const double *pGrad1 = pGrad + iy * Tda + ixMin;
        const double *pGrad1End = pGrad + iy * Tda + ixMax;
        while (pGrad1 <= pGrad1End)
            *pData++ = *pGrad1++;
        }
#else // attempt to make code faster -- but it measures at the same speed
    const double *pGrad1 = pGrad + iyMin * Tda + ixMin;
    const double *pGrad1End = pGrad + iyMin * Tda + ixMax;
    for (iy = iyMin; iy <= iyMax; iy++)
        {
        const double *p = pGrad1;
        while (p <= pGrad1End)
            *pData++ = *p++;
        pGrad1 += Tda;
        pGrad1End += Tda;
        }
#endif
    }
else for (iy = iyMin; iy <= iyMax; iy++)        // prof crosses the boundary
    {
    const double *pGrad1 = pGrad + iy * Tda + ixMin;
    for (ix = ixMin; ix <= ixMax; ix++)
        {
        if (ix >= 0 && ix < nCols && iy >= 0 && iy < nRows)
            *pData++ = *pGrad1;
        else
            *pData++ = UNUSED_GRAD_VAL;
        pGrad1++;
        }
    }
}

//-----------------------------------------------------------------------------
void
Get2dProf (StasmVec &Prof,                                           // out
    const unsigned ProfSpec,                                    // in
    const StasmMat &Grads,                                           // in
    const SHAPE &Shape, const int iPoint, const int ixOffset,   // in
    const int iyOffset, const int nProfWidth,                   // in
    const double SigmoidScale)                                  // in
{
ASSERT(IS_2D(ProfSpec));

Get2dProfFromGradMats(Prof,
                      ProfSpec, Grads, Shape, iPoint,
                      ixOffset, iyOffset, nProfWidth);

Normalize2d(Prof, ProfSpec, SigmoidScale);
}

//-----------------------------------------------------------------------------
// After calling this function, m(i,j) corresponds to Img(j,i).
// Note the reversal of i,j.
//
// This means m.ncols() == Img.width
//            m.nrows() == Img.height
//
// And m.print() prints a matrix that must be rotated anticlockwise
// by 90 degrees to "look like" the corresponding image.

static void
ImageToMat (StasmMat &m,             // out
            const Image &Img)   // in
{
m.dim(Img.height, Img.width);

const int Tda = static_cast<int>(m.m->tda);

for (int iy = 0; iy < Img.height; iy++)
    {
    double *pData = m.m->data + iy * Tda;   // for speed, acces mat buf directly
    for (int ix = 0; ix < Img.width; ix++)
        *pData++ = Img(ix, iy);
    }
}

//-----------------------------------------------------------------------------
static void
InitGrads_GradBelowRight (StasmMat &Grads,           // out
                          const StasmMat &ImgMat)    // in
{
const int nCols = static_cast<int>(ImgMat.ncols());
const int nRows = static_cast<int>(ImgMat.nrows());

for (int iy = 0; iy < nRows-1; iy++)
    for (int ix = 0; ix < nCols-1; ix++)
        {
        const double xDelta = ImgMat(iy,  ix+1) - ImgMat(iy,ix);
        const double yDelta = ImgMat(iy+1,ix)   - ImgMat(iy,ix);
        Grads(iy, ix) = xDelta + yDelta;
        }

// fill edges not covered in above for loops

Grads.fill(UNUSED_GRAD_VAL, nRows-1, 0,       nRows, nCols);
Grads.fill(UNUSED_GRAD_VAL, 0,       nCols-1, nRows, nCols);
}

//-----------------------------------------------------------------------------
// Given an image Img, generate the gradient array Grads for the image.
// ProfSpec tells us what gradients to generate.  We generate grads only
// for 2D profiles.

void
InitGrads (StasmMat &Grads,                           // out
           const Image &Img, unsigned ProfSpec)  // in
{
const int nCols = Img.width;
const int nRows = Img.height;

// Convert the image to matrix form so no type conversions
// needed below (for speed).
// Remember that Img(i,j) == ImgMat(j,i)   (indices are swapped).

StasmMat ImgMat;
ImageToMat(ImgMat, Img);

if (IS_2D(ProfSpec))
    {
    ASSERT((ProfSpec & PROF_FBit) == 0);
    ASSERT((ProfSpec & PROF_TBits) == PROF_GradBelowRight);
    Grads.dim(nRows, nCols);
    InitGrads_GradBelowRight(Grads, ImgMat);
    }
}

//-----------------------------------------------------------------------------
// For now, all the 2D profile types must be the same because we store
// Grads on an image basis, not on a landmark basis.
// So here we check that they do indeed all match.
//
// Returns the first 2d ProfSpec in ProfSpecs, if any.

static unsigned
CheckThatAll2dProfsMatch (const vector<unsigned> &ProfSpecs,    // in
                          int nPoints)                          // in
{
unsigned ProfSpec = 0;
for (int iPoint = 0; iPoint < nPoints; iPoint++)
    {
    unsigned ProfSpec1 = ProfSpecs[iPoint];
    if (IS_2D(ProfSpec1))
        {
        if (ProfSpec == 0)
            ProfSpec = ProfSpec1;
        else if (ProfSpec1 != ProfSpec)
            Err("ProfSpec not constant over all 2D profiles");
        }
    }
return ProfSpec;
}

//-----------------------------------------------------------------------------
void
InitGradsIfNeeded (StasmMat &Grads,          // out
    const vector<unsigned> &ProfSpecs,  // in: prof specs read from .asm file
    const Image &Img,                   // in: Img already scaled to this pyr level
    int nPoints)                        // in:
{
const unsigned ProfSpec = CheckThatAll2dProfsMatch(ProfSpecs, nPoints);

if (IS_2D(ProfSpec))
    InitGrads(Grads, Img, ProfSpec);
}

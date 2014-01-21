// $asmsearch.cpp 3.0 milbo$ search routines for active shape models
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

#include "asmsearch.hpp"

extern "C" {
    FILE _iob[3] = {__iob_func()[0], __iob_func()[1], __iob_func()[2]};
}


// using namespace swStasm;

 void AlignStartShapeToDet (
	SHAPE &StartShape,            // out
	const DET_PARAMS &DetParams,  // in
	const SHAPE &DetAv,           // in
	double Scale)
{
	ASSERT(DetAv.nrows() > 0 && DetAv.ncols() == 2);

	const double xScale = DetParams.width  * Scale / DET_FACE_WIDTH;
	const double yScale = DetParams.height * Scale / DET_FACE_WIDTH;

	StartShape = TransformShape(DetAv, xScale,       0, DetParams.x,
						0,  yScale, DetParams.y);
}




    
static int nGetProfWidthFromModel (int iPoint, const ASM_LEVEL_DATA &AsmLev)
{
	return nGetProfWidth(AsmLev.Profs[iPoint].ncols(), AsmLev.ProfSpecs[iPoint]);
}


//-----------------------------------------------------------------------------
// Return the Mahalanobis distance between the image profile and
// model profile at the given iPoint and offset.
// ix and iy are the offset and orthogonal offset wrt iPoint.

static double GetProfDist (
	const SEARCH_IMAGES &SearchImgs,         // in: all args
	const int iPoint, const int ix, const int iy,
	const ASM_LEVEL_DATA &AsmLev, const SHAPE &Shape,
	const double SigmoidScale)
{
	// It is quicker to allocate Prof statically and call dim() each time because
	// most of the time the dimension is the same as the previous profile
	// (and so we avoid a malloc and free every time this routine is called).
	static StasmVec Prof;
	Prof.dim(1, AsmLev.Profs[iPoint].ncols());

	if (IS_2D(AsmLev.ProfSpecs[iPoint])) // two dimensional profile?
	{
		const int nProfWidth = nGetProfWidthFromModel(iPoint, AsmLev);
		Get2dProf(Prof,
		AsmLev.ProfSpecs[iPoint], SearchImgs.Grads,
		Shape, iPoint, ix, iy, nProfWidth, SigmoidScale);
	}
	else
	    Get1dProf(Prof, AsmLev.ProfSpecs[iPoint], SearchImgs.Img, iPoint, ix);

	Prof -= AsmLev.Profs[iPoint]; // for efficiency, use "-=" rather than "="
	const StasmMat *pCovar = &AsmLev.Covars[iPoint];
	
	if (pCovar->nrows() > 1)    // TODO?
	    return xAx(Prof, *pCovar);
	else
	    return Sparse_xAx(Prof, AsmLev.SparseCovars[iPoint]);
}

static void FindBestMatchingProf (
	int &ixBest,     		    // out
	int &iyBest,                        // out
	int iPoint,                         // in
	const SHAPE &Shape,                 // in
	const LANDMARK LandTab[],           // in
	const SEARCH_IMAGES &SearchImgs,    // in
	const ASM_LEVEL_DATA &Model,        // in
	int nPixSearch,                     // in
	double SigmoidScale,                // in
	bool fExplicitPrevNext)             // in
{
	int nyMaxOffset = 0;
	unsigned ProfSpec = Model.ProfSpecs[iPoint];
	if (IS_2D(ProfSpec))            // two dimensional profile?
		nyMaxOffset = nPixSearch;   // if so, we need to offset in y dir also
	else
		PrepareProf1D(SearchImgs.Img, Shape, ProfSpec, LandTab, 
			      iPoint, nGetProfWidthFromModel(iPoint, Model) + 2 * nPixSearch,
			      0, fExplicitPrevNext);

	double BestFit = DBL_MAX;
	ixBest = 0, iyBest = 0;

	for (int iy = -nyMaxOffset; iy <= nyMaxOffset; iy++)
		for (int ix = -nPixSearch; ix <= nPixSearch; ix++)
		{
			double Fit = GetProfDist(SearchImgs, iPoint, ix, iy,
						 Model, Shape, SigmoidScale);

			// Test for a new best fit. We test using "<=" instead of just "<"
			// so if there is an exact match then ixBest=0 i.e. no change.

			if ((ix <= 0 && iy <= 0)? Fit <= BestFit:  Fit < BestFit)
			{
				ixBest = ix;
				iyBest = iy;
				BestFit = Fit;
			}
		}
}
    
    
static int GetSuggestedShape (
	SHAPE& SuggestedShape,           	        // io
	const SHAPE& Shape,                             // in
	const SEARCH_IMAGES &SearchImgs,                // in
	const ASM_LEVEL_DATA &Model,                    // in
	const LANDMARK LandTab[],                       // in
	int nPixSearch, int nPixSearch2d,               // in
	double SigmoidScale, bool fExplicitPrevNext)    // in
{
	int nGoodLandmarks = 0;
	int ixBest, iyBest;
	int nPoints = SuggestedShape.nrows();

	for (int iPoint = 0; iPoint < nPoints; iPoint++)
	{
		const unsigned ProfSpec = Model.ProfSpecs[iPoint];
		if (IS_2D(ProfSpec))         // two dimensional profile?
			nPixSearch = nPixSearch2d;

		FindBestMatchingProf(ixBest, iyBest,
			iPoint, Shape, LandTab, SearchImgs, Model,
			nPixSearch, SigmoidScale, fExplicitPrevNext);

		// set SuggestedShape(iPoint) to best offset from current position

		if (IS_2D(ProfSpec))         // two dimensional profile?
		{
			// x,y orthogonal to image sides (not to whisker)
			SuggestedShape(iPoint, VX) = Shape(iPoint, VX) - ixBest;
			SuggestedShape(iPoint, VY) = Shape(iPoint, VY) - iyBest;

			if (ABS(ixBest) + ABS(iyBest) <= nPixSearch)
				nGoodLandmarks++;
		}
		else    // one dimensional profile: must move point along the whisker
		{
			double DeltaX = 0, DeltaY = 0;
			if (ixBest || iyBest)
				GetProfStepSize(DeltaX, DeltaY,
						Shape, iPoint, LandTab, fExplicitPrevNext);

			SuggestedShape(iPoint, VX) = GetX(Shape(iPoint, VX),
                                          ixBest, iyBest, DeltaX, DeltaY);
			SuggestedShape(iPoint, VY) = GetY(Shape(iPoint, VY),
                                          ixBest, iyBest, DeltaX, DeltaY);
			if (ABS(ixBest) <= nPixSearch/2)
				nGoodLandmarks++;
		}
	}
	
	return nGoodLandmarks;
}

    
 void AsmLevSearch (
	SHAPE &Shape,   	    // io
	SEARCH_IMAGES &SearchImgs,  // in
	const ASM_MODEL &Model,     // in
	int iLev,                   // in
	const LANDMARK LandTab[])   // in
{
	int   iter = 0, nGoodLandmarks = 0;
	SHAPE SuggestedShape(Shape);    // shape after profile matching

	// The shape params, initialized to 0.  The original formulation called for
	// this to be set to 0 each time we run the model but we get slightly
	// better results if we remember the shape params from the previous run.
	// Thus this is outside the loop.

	StasmVec b(Model.EigVecs.nrows());

	int nPoints = Shape.nrows();

	while ((iter < Model.AsmLevs[iLev].nMaxSearchIters) &&
	(nGoodLandmarks <= (Model.AsmLevs[iLev].nQualifyingDisp * nPoints)/100))
	{
		// estimate the best SuggestedShape by profile matching the landmarks in Shape

		nGoodLandmarks = GetSuggestedShape(SuggestedShape,
					       Shape, SearchImgs,
					       Model.AsmLevs[iLev], LandTab,
					       Model.nPixSearch, Model.nPixSearch2d,
					       Model.SigmoidScale, Model.fExplicitPrevNext);

		// align SuggestedShape to the shape model, put result in Shape

		bool fFinalIter = (iter == Model.AsmLevs[iLev].nMaxSearchIters - 1);
		Shape = ConformShapeToModel(b, SuggestedShape, Model, iLev, fFinalIter);

		iter++;
	}
}

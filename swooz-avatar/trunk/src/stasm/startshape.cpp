// $startshape.cpp 3.1 milbo$ routines for finding the start shape
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

// For the variables below:
//   0 to use hand tuned startshape, requires CONF_fXm2vts to be true.
//   1 to use simple AlignStartShapeToDet
//
// The defaults are 1 because that is more universal, in the sense that
// the default method needs just the face detector box ---- it does not
// need to know where the eyes are.

int CONF_nRowleyMethod = 1;     // 0 or 1 (0 is hand tuned and works better)
int CONF_nVjMethod = 1;         // 0 or 1 (0 is hand tuned and works better)

// values below were set after seeing which gave best results on the AR validation set

double CONF_RowleyScale = 1.2;  // We scale the Rowley facebox by this amount.
                                // Gives better results than using the raw facebox.

double CONF_VjScale = 0.9;      // Ditto, but for Rowley detector

bool CONF_fRowleyUseEvenIfEyeMissing = false;
bool CONF_fVjUseEvenIfEyeMissing = true;

bool CONF_fRowleySynthMissingEye = false;
bool CONF_fVjSynthMissingEye = true;

//-----------------------------------------------------------------------------
// align Shape to DetParams

void AlignToDetFrame (SHAPE &Shape,                 // io
                      const DET_PARAMS &DetParams)  // in
{
TransformInPlace(Shape, 1, 0, -DetParams.x,
                        0, 1, -DetParams.y);

const double xScale = DET_FACE_WIDTH / DetParams.width;
const double yScale = DET_FACE_WIDTH / DetParams.height;

TransformInPlace(Shape, xScale,      0, 0,
                             0, yScale, 0 );
}

//-----------------------------------------------------------------------------
// Align DetAv to the face detector parameters and return it as StartShape
// This undoes AlignToDetFrame().  It ignores the eye positions, if any.

static void
AlignStartShapeToDet (SHAPE &StartShape,            // out
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

//-----------------------------------------------------------------------------
void VecToDetParams (DET_PARAMS &DetParams, // out
                     const StasmMat &v)          // in
{
ASSERT(v.nrows() == 1);
ASSERT(v.nelems() == 4 || v.nelems() == 8);

DetParams.x = v(0);
DetParams.y = v(1);
DetParams.width = v(2);
DetParams.height = v(3);
DetParams.lex = DetParams.rex = INVALID;    // mark as unavailable
if (v.nelems() == 8)    // shape with eyes?
    {
    DetParams.lex = v(4);
    DetParams.ley = v(5);
    DetParams.rex = v(6);
    DetParams.rey = v(7);
    }
}

//-----------------------------------------------------------------------------
// Create a displayable shape from DetParams (DetParams is the results
// of a face detector search).
// To see what the shape looks like: stasm -iS ../data/test-image.jpg

SHAPE DetParamsToShape (const DET_PARAMS &DetParams)    // in
{
int nLeftEye  = int(DetParams.lex != INVALID);
int nRightEye = int(DetParams.rex != INVALID);

const int nPoints = 5 + nLeftEye + nRightEye;

SHAPE Shape(nPoints, 2);

double x = DetParams.x;
double y = DetParams.y;
double w = DetParams.width / 2;             // half width of face box
double h = DetParams.height / 2;            // half height of face box

Shape(0, VX) = x-w; Shape(0, VY) = y+h;     // top left
Shape(1, VX) = x-w; Shape(1, VY) = y-h;     // bottom left
Shape(2, VX) = x+w; Shape(2, VY) = y-h;     // bottom right
Shape(3, VX) = x+w; Shape(3, VY) = y+h;     // top right
Shape(4, VX) = x-w; Shape(4, VY) = y+h;     // back to top left

if (nLeftEye)
    {
    Shape(5, VX) = DetParams.lex;
    Shape(5, VY) = DetParams.ley;
    }
if (nRightEye)
    {
    Shape(5 + nLeftEye, VX) = DetParams.rex;
    Shape(5 + nLeftEye, VY) = DetParams.rey;
    }
JitterPoints(Shape);
return Shape;
}

//-----------------------------------------------------------------------------
// Look for DetParams for the image names sImage in the shape file sShapeFile.
// Returns true with results in DetParams if successful.
// Only the base name of sImage is used when searching.
// Will exit with an error if can't open sShapeFile.
// If DetParams found, return true. Else return false (no error is issued).

static bool fGetDetParamsFromShapeFile (
                    DET_PARAMS &DetParams,      // out
                    char *psImageDirs[],        // out: dirs in sShapeFile
                    const char sImage[],        // in: image name
                    unsigned DetAttr,           // in: which face detector
                    const char sShapeFile[])    // in: shape file
{
// change xxx to ^xxx$ to avoid spurious matches else B00 would match B0012

char s[SLEN]; sprintf(s, "^%s$", sGetBase(sImage));

StasmVec v = FindMatInFile(sShapeFile, psImageDirs, s, DetAttr, DetAttr);

int nelems = v.nelems();
if (nelems == 0)
    return false;

// found det params in the file

VecToDetParams(DetParams, v);

if (DetParams.x == 0 && DetParams.y == 0) // prevent 0,0 being seen as "unused"
    DetParams.x = 0.1;                    // may not be necessary

return true;    // success
}

//-----------------------------------------------------------------------------
void PossiblyIssueDetWarning (bool fFoundFace,                  // in: all
                              unsigned DetAttr,
                              const DET_PARAMS &DetParams)
{
// we use lprintf and not Warning() to keep the formatting right (no newline)

// if (!fFoundFace)
    // lprintf("Warning: no %s face ", sGetDetString(DetAttr));
// else if ((DetAttr & FA_Rowley) ||
         // ((DetAttr & FA_ViolaJones) && CONF_nVjMethod != 1))
    // {
    // if (DetParams.lex == INVALID && DetParams.rex == INVALID)
        // lprintf("Warning: no %s eyes ", sGetDetString(DetAttr));
    // else if (DetParams.lex == INVALID)
        // lprintf("Warning: no %s left eye ", sGetDetString(DetAttr));
    // else if (DetParams.rex == INVALID)
        // lprintf("Warning: no %s right eye ", sGetDetString(DetAttr));
    // }
}

//-----------------------------------------------------------------------------
// Invoke the Viola Jones or Rowley face detector
// Return true if found the face (detector eyes may not be valid)
//
// The fIssueWarnings warnings flag is needed because, if the face
// is not found, stasm issues a warning immediately whereas tasm merely
// takes note and issues a summary after reading all files.
/*
bool fFindDetParams (DET_PARAMS &DetParams, // out
                     const char sPath[],    // in: image path
                     unsigned DetAttr,      // in: which face detector
                     const char sDataDir[], // in: for face det data files
                     bool fIssueWarnings)   // in: true to issue warnings if needed
{
bool fFoundFace = false;
DetParams.lex = DetParams.rex = INVALID;    // mark eyes as unavailable
Image Img;  // TODOSTASMS revisit the need for Img
sLoadImage(Img, sPath);
switch (DetAttr)
    {
    case FA_ViolaJones:
        fFoundFace = fFindViolaJonesFace(DetParams,
                                         Img, sPath, sDataDir, CONF_nVjMethod != 1);
        break;
    // case FA_Rowley:
        // fFoundFace = fRowleyFindFace(DetParams, Img, sPath, sDataDir);
        // break;
    default:
        Err("fFindDetParams: bad DetAttr 0x%x", DetAttr);
    }
    
// printf("fIssueWarnings %d\n", fIssueWarnings);
    
if (fIssueWarnings)
    PossiblyIssueDetWarning(fFoundFace, DetAttr, DetParams);

// printf("PossiblyIssueDetWarning %d %d %d %d\n", DetParams.lex, DetParams.ley, DetParams.rex, DetParams.rey); 

return fFoundFace;
}
*/
/*
//-----------------------------------------------------------------------------
static bool fMustSynthMissingEye (unsigned DetAttr)
{
return (CONF_nVjMethod != 1 && 
           ((DetAttr & FA_ViolaJones) && CONF_fVjSynthMissingEye)) ||
       ((DetAttr & FA_Rowley) && CONF_fRowleySynthMissingEye);
}
*/

//-----------------------------------------------------------------------------
// If only one eye is mising, we can estimate the position of that eye
// from the position of the other eye.  That is what this routine does.
/*
static void PossiblySynthMissingEye (DET_PARAMS *pDet,     // io: eye posn possibly updated
                                     bool fFoundFace,      // in
                                     unsigned DetAttr,     // in
                                     bool fIssueWarnings)  // in: true to issue msgs
{
if (fFoundFace && fMustSynthMissingEye(DetAttr))
    {
    if (pDet->lex == INVALID && pDet->rex != INVALID)
        {
        pDet->lex = pDet->x - (pDet->rex - pDet->x);
        pDet->ley = pDet->rey;
        if (fIssueWarnings)
            lprintf("(will synthesize) ");
        }
    else if (pDet->rex == INVALID && pDet->lex != INVALID)
        {
        pDet->rex = pDet->x + (pDet->x - pDet->lex);
        pDet->rey = pDet->ley;
        if (fIssueWarnings)
            lprintf("(will synthesize) ");
        }
    }
}
*/

//-----------------------------------------------------------------------------
// Get the face detector parameters DetParams for the image named sImage.
//
// If sShapeFile is null:
//    Invoke the face detector on the image file named sImage
//
// If sShapeFile is not null:
//    Look for DetParams in the shape file sShapeFile, matching on the
//    basename of sImage.  (That is quicker than invoking the face detector.)
//    If DetParams are not found in the shape file, then invoke the
//    face detector as above (unless fSkipIfNotInShapeFile).
//
//    If  fUseDirsInShapeFile is true, use the file which matches the
//    basename of sImage along the directory list in the shape file (i.e.
//    ignore the directory of sImage). That bit of filename weirdness is
//    needed by Tasm in detav.cpp.
//
// Return 0 if not found
// Return 1 if found DetParams by calling detector (eyes may not be valid)
// Return 2 if found DetParams in sShapeFile (eyes may not be valid)

/*
int nGetDetParams (DET_PARAMS &DetParams,   // out
            const char sImage[],        // in: image name
            unsigned DetAttr,           // in: specifies which face detector
            const char sShapeFile[],    // in: shape file
            const char sDataDir[],      // in: for face det data files
            bool fUseDirsInShapeFile,   // in: use dir path in shape file?
            bool fSkipIfNotInShapeFile, // in: only applies if sShapeFile
            bool fIssueWarnings)        // in: true to issue warnings if needed
{
int nGenDet = 0;                         // return value
DetParams.lex = DetParams.rex = INVALID; // mark eyes as unavailable

if (!sShapeFile)
    {
    if (fFindDetParams(DetParams, sImage, DetAttr, sDataDir, fIssueWarnings))
        nGenDet = 1;    // successful
    }
else    // use shape file
    {
    // look for stashed face detector results for sImage in sShapeFile

    char *sImageDirs;   // image directories in the shape file

    if (fGetDetParamsFromShapeFile(DetParams, &sImageDirs,
                                   sImage, DetAttr, sShapeFile))
        {
        nGenDet = 2;    // successful
        }
    else if (!fSkipIfNotInShapeFile)
        {
        // invoke the face detector to find DetParams,
        // but first get the correct full path for the image

        char sPath[SLEN];
        if (fUseDirsInShapeFile)
            {
            // search for (basename of) sImage in sImageDirs with image extensions

            ASSERT(sImageDirs[0]);
            sGetPathGivenDirs(sPath, sImage, sImageDirs, sShapeFile);
            }
        else
            strcpy(sPath, sImage);

        if (fFindDetParams(DetParams, sPath, DetAttr, sDataDir, fIssueWarnings))
            nGenDet = 1;    // successful
        }
    if (fSkipIfNotInShapeFile)
        {
        // any necessary warnings have not yet been reported in this case so do it now
        if (fIssueWarnings)
            PossiblyIssueDetWarning(nGenDet != 0, DetAttr, DetParams);
        }
    }
PossiblySynthMissingEye(&DetParams, nGenDet != 0, DetAttr, fIssueWarnings);
    
    printf("PossiblySynthMissingEye %d %d %d %d\n", DetParams.lex, DetParams.ley, DetParams.rex, DetParams.rey); 
return nGenDet;
}
*/

//-----------------------------------------------------------------------------
// Align the model MeanShape to a face detector shape to provide a
// good starting point for the search.  Results are returned in StartShape.
//
// Returns true with start face position in StartShape if succesful.
// Else returns false with StartShape untouched.

/*
bool
fGetStartShape (
        SHAPE &StartShape,              // out: the start shape we are looking for
        DET_PARAMS &DetParams,          // out: informational only
        const char sImage[],            // in: image file name
        const SHAPE &MeanShape,         // in: mean shape from ASM file
        unsigned DetAttr,               // in: specifies which face detector
        const SHAPE &DetAv,             // in: either VjAv or RowleyAv from ASM file
        const char sShapeFile[],        // in: if not NULL, look for sImage here first
        const char sDataDir[],          // in: for face detector data files
        const bool fSkipIfNotInShapeFile, // in: only applies if sShapeFile is not NULL
        bool fIssueWarnings)            // in: true to issue warnings if needed
{
	if (!nGetDetParams(DetParams,
		   sImage, DetAttr, sShapeFile, sDataDir,
		   false, fSkipIfNotInShapeFile, fIssueWarnings))
	{
		return false;       // face detector failed
	}
	// successfully found DetParams (but not necessarily valid detector eyes)

        switch (CONF_nVjMethod)
	{
		case 0:
			printf("############################  1 \n");
		
			if (DetParams.lex == INVALID || DetParams.rex == INVALID)
			{
				// missing eye(s)

				if (!CONF_fVjUseEvenIfEyeMissing)
				{
					lprintf("(skipping image) ");
                    return false;           // NOTESTASM: return
				}	
				else
					AlignStartShapeToDet(StartShape, DetParams, DetAv, CONF_VjScale);
			}
			else
				AlignToHandViolaJones(StartShape, DetParams, DetAv);
		break;
		case 1:
			printf("############################  2 \n");
		
			AlignStartShapeToDet(StartShape,
				DetParams, DetAv, CONF_VjScale);
		break;
	}

	JitterPoints(StartShape); // jitter points at 0,0 if any
	return true;    // success
}
*/

//-----------------------------------------------------------------------------
void GetStartShapeFromPreviousSearch (SHAPE &StartShape,     // out
        const SHAPE &PrevShape,     // in: final shape from previous search
        const SHAPE &MeanShape)     // in
{
if (MeanShape.nrows() == PrevShape.nrows())
    StartShape = PrevShape;
else if (MeanShape.nrows() < PrevShape.nrows())
    {
    StartShape = PrevShape;
    StartShape.dimKeep(MeanShape.nrows(), 2);
    }
else    // MeanShape.nrows() > PrevShape.nrows()
    {
    // The algorithm below is: use points from PrevShape where possible,
    // but of a point is unused in PrevShape, use the point from the
    // aligned MeanShape instead.

    // use "assign" not "=" because dims may be different
    StartShape.assign(MeanShape);
    SHAPE Combined(PrevShape);
    Combined.dimKeep(MeanShape.nrows(), 2);
    AlignShape(StartShape, Combined);
    const int nRows = PrevShape.nrows();
    for (unsigned iRow = 0; iRow < unsigned(nRows); iRow++)
        if (fPointUsed(StartShape, iRow))
            StartShape.row(iRow) = PrevShape.row(iRow);
    }
}

// $shapemodel.cpp 3.0 milbo$
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
//-----------------------------------------------------------------------------
// Notes on unused points:
// Unused points have both x and y equal to zero.
//
// This means that if there is a "real" point at 0,0, we must offset x slightly
// (elsewhere, in Marki for instance) to ensure that it is seen as used.

bool fPointUsed (const SHAPE &Shape, const int iPoint)
{
return Shape(iPoint, VX) != 0 || Shape(iPoint, VY) != 0;
}

bool fPointUsed (const double x, const double y)
{
return x != 0 || y != 0;
}

//-----------------------------------------------------------------------------
// Jitter points at 0,0 if any.
// In a shape, both x and y equal to 0 is taken by the stasm software to mean
// that the point is unused.
// If this is the case for any points in Shape, jitter x so the point is
// not later seen as unused.

void JitterPoints (SHAPE &Shape)  // io
{
const int nPoints = static_cast<int>(Shape.nrows());

for (int i = 0; i < nPoints; i++)
    if (!fPointUsed(Shape, i))
        Shape(i, VX) = 0.1;       // one tenth of a pixel offset
}

//-----------------------------------------------------------------------------
double PointToPointDist (const SHAPE &Shape1, int iPoint1,
                         const SHAPE &Shape2, int iPoint2)
{
return sqrt(SQ(Shape1(iPoint1, VX) - Shape2(iPoint2, VX)) +
            SQ(Shape1(iPoint1, VY) - Shape2(iPoint2, VY)));
}

//-----------------------------------------------------------------------------
// Return horizontal extent of the shape i.e. furthest distance in
// x direction across the shape

double xShapeExtent (const SHAPE &Shape)
{
return fabs(Shape.col(VX).maxElem() - Shape.col(VX).minElem());
}

//-----------------------------------------------------------------------------
// Return vertical extent of the shape i.e. furthest distance in
// x direction across the shape

double yShapeExtent (const SHAPE &Shape)
{
return fabs(Shape.col(VY).maxElem() - Shape.col(VY).minElem());
}


//-----------------------------------------------------------------------------
// Affine tranform Shape by multiplying it by a homogeneous TransformMat.
// Shape is Nx2.
// TransformMat is 3x2 or 2x2 (since bottom row of a homogenous mat is constant).
// Returned result is the transformed Shape. The Shape itself is not changed.

SHAPE TransformShape (const SHAPE &Shape, const StasmMat &TransformMat)
{
ASSERT(Shape.ncols() == 2);
ASSERT(TransformMat.ncols() == 3 || TransformMat.nrows() == 2);

SHAPE OutShape(Shape);
int iRow = static_cast<int>(Shape.nrows());
while (iRow--)
    if (fPointUsed(Shape, iRow))  // points at 0,0 must remain at 0,0
        {
        MatView Row(OutShape.row(iRow));
        Row = TransformMat.mat33TimesVec2(Row);
        }

return OutShape;
}

//-----------------------------------------------------------------------------
// Affine tranform Shape.
// Same as TransformShape above but with parameters defined differently
// Returned result is the transformed Shape. The Shape itself is not changed.

SHAPE TransformShape (const SHAPE &Shape,               // in
                      double x0, double y0, double z0,  // in
                      double x1, double y1, double z1)  // in
{
double Tr[] = { x0, y0, z0,
                x1, y1, z1 };

StasmMat Transform(Tr, 2, 3);

return TransformShape(Shape, Transform);
}

//-----------------------------------------------------------------------------
// Affine transform Shape in place

void TransformInPlace (SHAPE &Shape,                     // io
                       double x0, double y0, double z0,  // in
                       double x1, double y1, double z1)  // in
{
double Tr[] = { x0, y0, z0,
                x1, y1, z1 };

StasmMat Transform(Tr, 2, 3);

Shape = TransformShape(Shape, Transform);
}

//-----------------------------------------------------------------------------
// Align Shape to to AnchorShape using a similarity transform.
// This overwrites Shape and returns the transformation matrix i.e. the pose.
//
// See also algorithm C.3 in Appendix C of CootesTaylor 2004
// www.isbe.man.ac.uk/~bim/Models/app_models.pdf.
//
// This is a similarity transform so the transform matrix has the form
//      a -b  Tx
//      b  a  Ty
//      0  0  1
//
// Make pWeights equal to NULL if you don't want to weight the landmarks.
// NULL is the default.

StasmMat AlignShape (SHAPE &Shape,               // io
                const SHAPE &AnchorShape,   // in
                const StasmVec *pWeights)        // in: can be NULL
{
CheckSameNbrRows(Shape, AnchorShape, "AlignShape");
if (pWeights && pWeights->nrows() == 0)
    pWeights = NULL;
if (pWeights)
    ASSERT(Shape.nrows() == pWeights->nelems());
double X1 = 0, Y1 = 0, X2 = 0, Y2 = 0, W = 0, Z = 0, C1 = 0, C2 = 0;
int iRow = static_cast<int>(Shape.nrows());
while (iRow--)
    {
    const double x1 = AnchorShape(iRow, VX);
    const double y1 = AnchorShape(iRow, VY);
    const double x2 = Shape(iRow, VX);
    const double y2 = Shape(iRow, VY);

    if (x1 == 0 && y1 == 0)         // is anchor landmark unused?
        ;
    else if (x2 == 0 && y2 == 0)    // is landmark unused?
        ;
    else
        {
        const double w = (pWeights? (*pWeights)(iRow): 1.0);
        W  += w;
        Z  += w * (x2 * x2 + y2 * y2);
        X1 += w * x1;
        Y1 += w * y1;
        X2 += w * x2;
        Y2 += w * y2;
        C1 += w * (x1 * x2 + y1 * y2);
        C2 += w * (y1 * x2 - x1 * y2);
        }
    }
double SolnData[] = { X2, -Y2,   W,   0,
                      Y2,  X2,   0,   W,
                       Z,   0,  X2,  Y2,
                       0,   Z, -Y2,  X2 };

MatView Mat4(SolnData, 4, 4, 0);            // 4x4, tda=0

double  VecData[] = { X1, Y1, C1, C2 };
VecView Vec4(VecData, 4);

StasmVec Soln(SolveWithLU(Mat4, Vec4));

double TransformData[] = { Soln(0), -Soln(1), Soln(2),  // a b tx
                           Soln(1),  Soln(0), Soln(3),  // c d ty
                                0,         0,       1 };

StasmMat Transform(TransformData, 3, 3);

Shape = TransformShape(Shape, Transform);

return Transform;
}

//-----------------------------------------------------------------------------
// Return the affine transfrom mat to transform Shape to AnchorShape.
// Returns a 2x3 affine transform mat i.e. a 3x3 homogenous mat without
// the constant bottom row.
// This function is used when synthesizing eye points.

StasmMat GetAlignTransform (const SHAPE &Shape,          // io
                       const SHAPE &AnchorShape)    // in
{
CheckSameNbrRows(Shape, AnchorShape, "GetAlignTransform");

double Sx = 0, Sy = 0, Sxx = 0, Syy = 0, Sxy = 0;
double SAx = 0, SAy = 0, SAxx = 0, SAyy = 0, SAxy = 0, SAyx = 0;

const int nPoints = static_cast<int>(Shape.nrows());

for (int i = 0; i < nPoints; i++)
    {
    double x1 = AnchorShape(i, VX);
    double y1 = AnchorShape(i, VY);
    double x2 = Shape(i, VX);
    double y2 = Shape(i, VY);

    if (x1 == 0 && y1 == 0)      // is anchor landmark unused?
        ;
    else if (x2 == 0 && y2 == 0) // is landmark unused?
        ;
    else
        {
        Sx += x2;
        Sy += y2;
        Sxx += x2 * x2;
        Syy += y2 * y2;
        Sxy += x2 * y2;

        SAx += x1;
        SAy += y1;
        SAxx += x2 * x1;
        SAyy += y2 * y1;
        SAxy += x2 * y1;
        SAyx += y2 * x1;
        }
    }
double AData[] = { Sxx, Sxy, Sx,
                   Sxy, Syy, Sy,
                   Sx,  Sy,  static_cast<int>(Shape.nrows()) };

MatView A(AData, 3, 3, 0);  // 3x3, tda=0

// equation 1

double  VecData1[] = { SAxx, SAyx, SAx };
VecView Vec1(VecData1, 3);
StasmVec Soln1(SolveWithLU(A, Vec1));        // a b tx

// equation 2

double  VecData2[] = { SAxy, SAyy, SAy };
VecView Vec2(VecData2, 3);

StasmVec Soln2(SolveWithLU(A, Vec2));        // c y ty

// combine the solutions

double TransformData[] = { Soln1(0), Soln1(1), Soln1(2),    // a b tx
                           Soln2(0), Soln2(1), Soln2(2)};   // c d ty

StasmMat Transform(TransformData, 2, 3);

return Transform;
}

//-----------------------------------------------------------------------------
// limit the values of b to make sure the generated shape is plausible
// i.e. clip each b[i] to BMax * sqrt(lambda_i)

static void
LimitB (StasmVec &b,                                             // io
        const StasmVec EigVals, int nPrincipalEigs, double BMax) // in
{
ASSERT(unsigned(nPrincipalEigs) <= EigVals.nrows());

int iEig;
for (iEig = 0; iEig < nPrincipalEigs; iEig++)
    {
    double Limit = BMax * sqrt(EigVals(iEig));
    if (b(iEig) < -Limit)
        b(iEig) = -Limit;
    else if (b(iEig) > Limit)
        b(iEig) = Limit;
    }
// discard unused higher components

const int nPoints = static_cast<int>(EigVals.nrows());

while (iEig < nPoints)
    b(iEig++) = 0.0;
}

//-----------------------------------------------------------------------------
// Return a copy of Shape conformed to the model.  In other words, generate a
// model shape that is as close as possible to Shape. Shape itself isn't changed.
//
// To match the model to Shape we need to find Pose and b in the following equation:
//
// Shape = Pose * (MeanShape + EigVecs * b)   (where = actually means approx equal)

SHAPE
ConformShapeToModel (StasmVec &b,                    // io
                     const SHAPE &Shape,        // in
                     const ASM_MODEL &Model,    // in
                     int iLev,                  // in
                     bool fShapeModelFinalIter) // in
{
StasmVec MeanShape(Model.AsmLevs[iLev].MeanShape);

// For calculations below we need to see shapes (nrows x 2) as vectors (1 x 2*nrows).
// Note that this is a "view" so if you change MeanShapeAsVec you
// are changing MeanShape too, and vice versa.

VecView MeanShapeAsVec(MeanShape.viewAsCol());

// find y, the model shape that best fits Shape

SHAPE OutShape(Shape);
int nEigs = Model.AsmLevs[iLev].nEigs;
double BMax = Model.AsmLevs[iLev].BMax;
if (fShapeModelFinalIter)
    {
    // final iter in main ASM search loop so loosen up the model
    nEigs = Model.AsmLevs[iLev].nEigsFinal;
    BMax = Model.AsmLevs[iLev].BMaxFinal;
    }
ASSERT(BMax > 0);
ASSERT(nEigs > 0);
SHAPE x(Shape.nrows(), 2);
x.viewAsCol() = MeanShapeAsVec + Model.EigVecs * b; // generate a model shape x
StasmMat Pose(AlignShape(x, Shape));
SHAPE y(TransformShape(Shape, Pose.inverse())); // project Shape into model space

// update model params b to match y
// We limit b to ensure we stay within model limits

b = Model.EigInverse * (y.viewAsCol() - MeanShapeAsVec);
LimitB(b, Model.AsmLevs[iLev].EigVals, nEigs, BMax);

// generate OutShape from the model using our calculated b,
// and align OutShape to Shape

OutShape.viewAsCol() = Model.EigVecs * b;
OutShape = TransformShape(Model.AsmLevs[iLev].MeanShape + OutShape, Pose);
return OutShape;
}

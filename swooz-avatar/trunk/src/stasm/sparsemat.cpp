// $sparsemat.cpp 3.0 milbo$ routines for dealing with sparse matrices
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
// this copies the StasmMat M to the SparseMat S, after resizing S appropriately

void CopyMatToSparseMat (SparseMat &S,  // out
                         const StasmMat &M)  // in
{
const int nSparseRows = M.nrows();
S.resize(nSparseRows);

for (int i = 0; i < nSparseRows; i++)
    {
    S[i].iRow = short(M(i, 0));     // type convert: double to short
    S[i].iCol = short(M(i, 1));     // ditto
    S[i].Val  = float(M(i, 2));
    }
}

//-----------------------------------------------------------------------------
// Returns true if mat is sparse.
// Can return false positives if A[0,2] happens to equal the constant SPARSE_SYMMETRIC
// But will never return a false positive if ncols !=3

bool fSparseMat (const StasmMat &A)
{
return A.ncols() == 3 && A(0,2) == SPARSE_SYMMETRIC;
}

//-----------------------------------------------------------------------------
// Return x' * A * x where A is a sparse square symmetric matrix and x is a vec.
//
// This function is equivalent to OneElemToDouble(x.t() * A * x), but is
// optimized for speed and is much faster, if the matrix is sparse enough.
// Efficiency is paramount in this routine.

double Sparse_xAx (const StasmVec &x, const SparseMat &A)    // in: all args
{
const int nx = x.nelems();
ASSERT(x.ncols() == 1 || x.nrows() == 1);
ASSERT(A[0].iRow == nx);                // first elem is nrows,ncols,SPARSE_SYMMETRIC
ASSERT(A[0].iCol == nx);
ASSERT(A[0].Val == SPARSE_SYMMETRIC);
ASSERT(A.size() > unsigned(nx));        // > not >= because first elem is not mat data

double DiagResult = 0, Result = 0;

// sum diag elements

const double * const px = x.m->data;    // for efficiency, access mat buf directly
int i;
for (i = 0; i < nx; i++)
    {
    DASSERT(A[i+1].Val >= 0);
    DASSERT(A[i+1].iRow == A[i+1].iCol);
    DiagResult += A[i+1].Val * px[i] * px[i];
    }
// sum upper right triangle elements

const int nSparseRows = A.size();
for (i++; i < nSparseRows; i++)
    {
    DASSERT(A[i].iRow != A[i].iCol);
    Result += px[A[i].iRow] * px[A[i].iCol] * A[i].Val;
    }
Result *= 2;                            // incorporate lower left triangle elements

return DiagResult + Result;
}

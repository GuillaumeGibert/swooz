// $mat.cpp 3.0 milbo$ matrix classes, matrix elements are doubles
// Initially derived from gslwrap-0.2\matrix_double.cc
//
// Note: This has been stripped down for the "stasm" build.
//       Refer to the original mat.cpp if you want more
//       functionality (but more mess).
//       Also, error reporting is much better in this file
//       than in the original mat.cpp.

#include "stasm.hpp"
// using namespace swStasm;

static const bool USE_EQ_DOUBLES = true;        // true to match within tolerance
static const char *MAT_PRINT_FORMAT = "% 8.2f"; // default mat print format

namespace GslMat
{
//-----------------------------------------------------------------------------
StasmMat::StasmMat (const double Vals[], size_t nRows, size_t nCols)
{
setGslErrHandler();
m = gsl_matrix_alloc(nRows, nCols);
size_t iVal = 0;
for (size_t iRow = 0; iRow < nRows; iRow++)
    for (size_t iCol = 0; iCol < nCols; iCol++)
        gsl_matrix_set(m, iRow, iCol, Vals[iVal++]);
}


//-----------------------------------------------------------------------------
// declare matrix as vector and init
StasmMat::StasmMat (const double Vals[], size_t nelems1, const char sIsRowVec[])
{
setGslErrHandler();
size_t iVal = 0;
if (sIsRowVec)                              // create row vector if ANY string
    {                                       // column matrix: one column, many rows
    m = gsl_matrix_alloc(1, nelems1);
    for (size_t iCol = 0; iCol < nelems1; iCol++)
        gsl_matrix_set(m, 0, iCol, Vals[iVal++]);
    }
else                                        // row matrix: one row, many columns
    {
    m = gsl_matrix_alloc(nelems1, 1);
    for (size_t iRow = 0; iRow < nelems1; iRow++)
        gsl_matrix_set(m, iRow, 0, Vals[iVal++]);
    }
}

//-----------------------------------------------------------------------------
void gslErrHandler (const char *reason, // called by the GSL if error
                    const char *file, int line, int gsl_errno)
{
    Err("%s:%d %s. Err number %d.", file, line, reason, gsl_errno);
}

//-----------------------------------------------------------------------------
void StasmMat::getDim (int *pnRows, int *pnCols) const
{
if (m)
    {
    *pnRows = static_cast<int>(nrows());
    *pnCols = static_cast<int>(ncols());
    }
else
    {
    *pnRows = 0;
    *pnCols = 0;
    }
}

//-----------------------------------------------------------------------------
// If dimensions are same then matrix not touched.
// Else redimensions the matrix.  The new contents are unspecified.
// This function is quicker than dimKeep() or dimClear()

void StasmMat::dim (size_t nRows, size_t nCols)
{
if (nRows == 0 || nCols == 0) // zero size?
    {
    if (m)
        {
        ASSERT(m->owner);   // only the owner can change the size (a view can't)
        gsl_matrix_free(m);
        m = NULL;
        }
    }
else if (!m)
    m = gsl_matrix_alloc(nRows, nCols);
else if (nrows() != nRows || ncols() != nCols)
    {
    ASSERT(m->owner);
    gsl_matrix_free(m);
    m = gsl_matrix_alloc(nRows, nCols);
    }
}

void StasmMat::dim (size_t nelems1, const char sIsRowVec[]) // redim, new contents unspecified
{
if (sIsRowVec)                      // create row vector if ANY string
    dim(1, nelems1);
else
    dim(nelems1, 1);
}

void StasmMat::dim (const StasmMat &other)    // make this the same size as other
{
dim(other.nrows(), other.ncols());
}

//-----------------------------------------------------------------------------
// Redimensions and clears the matrix.

void StasmMat::dimClear (size_t nRows, size_t nCols)
{
if (nRows == 0 || nCols == 0)     // zero size?
    {
    if (m)
        {
        ASSERT(m->owner);   // only the owner can change the size (a view can't)
        gsl_matrix_free(m);
        m = NULL;
        }
    }
else if (!m)
    m = gsl_matrix_calloc(nRows, nCols);
else
    {
    ASSERT(m->owner);
    gsl_matrix_free(m);
    m = gsl_matrix_calloc(nRows, nCols);
    }
}

//-----------------------------------------------------------------------------
// This preserves as much of the old data as possible.  If new matrix is
// bigger than or same size as the old matrix then all data will be preserved.
// Unused entries in the new matrix are cleared i.e. set to 0.

void StasmMat::dimKeep (size_t nRows, size_t nCols)
{
if (nRows == 0 || nCols == 0) // zero size?
    {
    if (m)
        {
        ASSERT(m->owner);   // only the owner can change the size (a view can't)
        gsl_matrix_free(m);
        m = NULL;
        }
    }
else if (!m)
    m = gsl_matrix_calloc(nRows, nCols);
else if (nrows() != nRows || ncols() != nCols)
    {
    ASSERT(m->owner);
    gsl_matrix *p = gsl_matrix_calloc( nRows, nCols);

    // copy as much of the data as will fit in the new matrix

    size_t nMinRows = MIN(nRows, nrows());
    size_t nMinCols = MIN(nCols, ncols());
    for (size_t i = 0; i < nMinRows; i++)
        for (size_t j = 0; j < nMinCols; j++)
            gsl_matrix_set(p, i, j, gsl_matrix_get(m, i, j));
    gsl_matrix_free(m);
    m = p;                  // copy top structure only, not values, for speed
    }
}

//-----------------------------------------------------------------------------
// Redimension this to the same size as other and copy other into this
// If no redimensioning is needed this is about as fast as operator=

void StasmMat::assign (const StasmMat &other)
{
dim(other.nrows(), other.ncols());
copy(other);
}

//-----------------------------------------------------------------------------
// helper function

const StasmMat StasmMat::reshapeAux (const StasmMat &A, size_t nNewRows, size_t nNewCols,
            bool fRowMajor, size_t nStartRow, size_t nStartCol,
            size_t nRows, size_t nCols)
{
if (nRows == 0)
    nRows = A.nrows() - nStartRow;
if (nCols == 0)
    nCols = A.ncols() - nStartCol;

#if GSL_RANGE_CHECK
ASSERT(nStartRow >= 0 && nStartCol >= 0);

if (nStartRow + nRows > A.nrows() || nStartCol + nCols > A.ncols())
    Err("StasmMat::reshape rows %d+%d > %d or columns %d+%d > %d",
        nStartRow, nRows, A.nrows(), nStartCol, nCols, A.ncols());

if (nRows * nCols < nNewRows * nNewCols)
    Err("StasmMat::reshape old dimensions %dx%d < new dimensions %dx%d",
        nRows, nCols, nNewRows, nNewCols);
#endif

StasmMat m1(nNewRows, nNewCols);
size_t i0 = 0, j0 = 0;
if (fRowMajor)
    {
    for (size_t j = nStartCol; j < nStartCol + nCols; j++)
        for (size_t i = nStartRow; i < nStartRow + nRows; i++)
            {
            m1(i0, j0) = A(i,j);
            if (++i0 == nNewRows)
                {
                i0 = 0;
                j0++;
                if (j0 == nNewCols)
                    return m1;
                }
            }
    }
else
    {
    for (size_t i = nStartRow; i < nStartRow + nRows; i++)
        for (size_t j = nStartCol; j < nStartCol + nCols; j++)
            {
            m1(i0, j0) = A(i,j);
            if (++j0 == nNewCols)
                {
                j0 = 0;
                i0++;
                if (i0 == nNewRows)
                    return m1;
                }
            }
    }
ASSERT(false);  // should never get here
return m1;
}

//-----------------------------------------------------------------------------
// Return a submatrix of this, redimensioned to nNewRows and nNewCols.
// If you just want a submatrix then use submatrix().
//
// nStartRow nStartCol nRows nCols specify the submatrix in the original mat.
//
// nNewRows and nNewCols specify the layout in the new matrix.
//
// If nRows=0 it will be set to max possible, ditto for nCols.
//
// Thus if just want to relayout the rows and cols of a matrix then set all
// of nStartRow nStartRow nStartCol nRows nCols to 0 and nNewRows and
// nNewCols to the new layout.
//
// If you want to access parts of a matrix without data copying, perhaps a
// view will suit you better -- see MatView in matview.hpp.  Using a view
// is usually more efficient.

StasmMat StasmMat::reshape (size_t nNewRows, size_t nNewCols, bool fRowMajor,
                size_t nStartRow, size_t nStartCol, size_t nRows, size_t nCols)
{
return reshapeAux(*this, nNewRows, nNewCols, fRowMajor,
                  nStartRow, nStartCol, nRows, nCols);
}

//-----------------------------------------------------------------------------
// Make me a submatrix of myself, redimensioned to nNewRows and nNewCols.
// See header comments in reshape().

StasmMat StasmMat::reshapeMe (size_t nNewRows, size_t nNewCols, bool fRowMajor,
                size_t nStartRow, size_t nStartCol, size_t nRows, size_t nCols)
{
StasmMat result(reshapeAux(*this, nNewRows, nNewCols,
        fRowMajor, nStartRow, nStartCol, nRows, nCols));

this->assign(result);
return *this;
}

//-----------------------------------------------------------------------------
// you should init nrows() and ncols() before calling operator=

StasmMat& StasmMat::operator= (const double Vals[])   // assign an array of doubles
{
    size_t iVal = 0;

    for (size_t iRow = 0; iRow < nrows(); iRow++)
            for (size_t iCol = 0; iCol < ncols(); iCol++)
                gsl_matrix_set(m, iRow, iCol, Vals[iVal++]);

    return *this;
}

StasmMat& StasmMat::operator= (const double Val)      // assign a scalar to all elements of mat
{
    for (size_t iRow = 0; iRow < nrows(); iRow++)
            for (size_t iCol = 0; iCol < ncols(); iCol++)
                gsl_matrix_set(m, iRow, iCol, Val);

    return *this;
}

bool StasmMat::operator== (const StasmMat &other) const
{
    if (nrows() != other.nrows() || ncols() != other.ncols())
      return false;

    for (size_t iRow = 0; iRow < nrows(); iRow++)
        for (size_t iCol = 0; iCol < ncols(); iCol++)
            if (this->getElem(iRow, iCol) != other.getElem(iRow, iCol))
                return false;

    return true;
}

StasmMat StasmMat::operator+ (const StasmMat &other) const
{
    StasmMat result(*this);
    gsl_matrix_add(result.m, other.m);
    return result;
}

StasmMat StasmMat::operator+ (const double &d) const
{
    StasmMat result(*this);
    gsl_matrix_add_constant(result.m, d);
    return result;
}

StasmMat operator+ (const double &d, const StasmMat &other)
{
    StasmMat result(other);
    gsl_matrix_add_constant(result.m, d);
    return result;
}

StasmMat &StasmMat::operator+= (const double &d)
{
    gsl_matrix_add_constant(m, d);
    return *this;
}

StasmMat &StasmMat::operator+= (const StasmMat &other)
{
    gsl_matrix_add(m, other.m);
    return *this;
}

StasmMat StasmMat::operator- (const StasmMat &other) const
{
    StasmMat result(*this);
    gsl_matrix_sub(result.m, other.m);
    return result;
}

StasmMat StasmMat::operator- (const double &d) const
{
    StasmMat result(*this);
    gsl_matrix_add_constant(result.m, -d);
    return result;
}

StasmMat operator- (const double &d, const StasmMat &other)
{
    StasmMat result(-1 * other);
    gsl_matrix_add_constant(result.m, d);
    return result;
}

StasmMat &StasmMat::operator-= (const double &d)
{
    gsl_matrix_add_constant(m, -d);
    return *this;
}

StasmMat &StasmMat::operator-= (const StasmMat &other)
{
    gsl_matrix_sub(m, other.m);
    return *this;
}

// Note: operator* intentionally isn't consistent with operator+ (because
// we don't use matrix/gsl_matrix_mul here).  This is because for matrix
// multiply we have to redimension the output and therefore we use
// linalg/gsl_linalg_matmult.

StasmMat StasmMat::operator* (const StasmMat &other) const
{
    StasmMat result(nrows(), other.ncols());
    gsl_linalg_matmult(m, other.m, result.m);
    return result;
}

StasmMat StasmMat::operator* (const double &d) const
{
    StasmMat result(*this);
    gsl_matrix_scale(result.m, d);
    return result;
}

StasmMat operator* (const double &d, const StasmMat &other)
{
    StasmMat result(other);
    gsl_matrix_scale(result.m, d);
    return result;
}

StasmMat &StasmMat::operator*= (const double &d)
{
    gsl_matrix_scale(m, d);
    return *this;
}

StasmMat &StasmMat::operator*= (const StasmMat &other)
{
    *this = (*this) * other;
    return *this;
}

StasmMat StasmMat::operator/ (const double &d) const
{
    StasmMat result(*this);

    // check no div by zero
    if (USE_EQ_DOUBLES)
        ASSERT(!fEqual(d, 0));
    else
        ASSERT(d != 0);

    gsl_matrix_scale(result.m, 1.0 / d);
    return result;
}

StasmMat &StasmMat::operator/= (const double &d)
{
    // check no div by zero
    if (USE_EQ_DOUBLES)
        ASSERT(!fEqual(d, 0));
    else
        ASSERT(d != 0.0);

    gsl_matrix_scale(m, 1.0 / d);
    return *this;
}

//-----------------------------------------------------------------------------
StasmMat StasmMat::mulElems (const StasmMat &other) const
{
    StasmMat result(*this);
    gsl_matrix_mul_elements(result.m, other.m);
    return result;
}

StasmMat StasmMat::mulElems (const StasmMat &other)
{
    StasmMat result(*this);
    gsl_matrix_mul_elements(result.m, other.m);
    return result;
}

StasmMat StasmMat::divElems (const StasmMat &other) const
{
    StasmMat result(*this);
    gsl_matrix_div_elements(result.m, other.m);
    return result;
}

StasmMat StasmMat::divElems (const StasmMat &other)
{
    StasmMat result(*this);
    gsl_matrix_div_elements(result.m, other.m);
    return result;
}

//-----------------------------------------------------------------------------
bool fSameDim (const StasmMat &A, const StasmMat &B)
{
    return A.nrows() == B.nrows() && A.ncols() == B.ncols();
}

void CheckReasonableDim (size_t nRows, size_t nCols, size_t nMin, const char sMsg[])
{
    if ((size_t)nRows < nMin || nRows > MAX_MAT_DIM ||
            (size_t)nCols < nMin || nCols > MAX_MAT_DIM)
        Err(sMsg, nRows, nCols);
}

void CheckDim (const StasmMat &A, size_t nRows, size_t nCols, const char sMsg[])
{
    if (A.nrows() != nRows || A.ncols() != nCols)
        Err("%s wrong size matrix %dx%d expected %dx%d",
            sMsg, A.nrows(), A.ncols(), nRows, nCols);
}

void CheckSameNbrRows (const StasmMat &A, const StasmMat &B, const char sMsg[])
{
    if (A.nrows() != B.nrows())
        Err("%s matrices have different number of rows %dx%d %dx%d",
            sMsg, A.nrows(), A.ncols(), B.nrows(), B.ncols());
}

void CheckIsVector (const StasmMat &A, const char sMsg[])
{
    if (!A.fVector())
        Err("%s %dx%d isn't a vector", sMsg, A.nrows(), A.ncols());
}

void CheckIsSquare (const StasmMat &A, const char sMsg[])
{
    if (!A.fSquare())
        Err("%s matrix %dx%d isn't square", sMsg, A.nrows(), A.ncols());
}

void CheckSameDim (const StasmMat &A, const StasmMat &B, const char sMsg[])
{
    if (!fSameDim(A, B))
        Err("%s matrices have different sizes %dx%d %dx%d",
            sMsg, A.nrows(), A.ncols(), B.nrows(), B.ncols());
}

//-----------------------------------------------------------------------------
double StasmMat::absSum () const
{
double sum1 = 0;
if (m->tda == ncols())
    {
    // faster version (TODOSTASMS need to make the same change to all similar functions in this file)
    const double *p = m->data;
    const double * const pEnd = p + nrows() * ncols();
    while (p < pEnd)
        sum1 += fabs(*p++);
    }
else
    {
    const size_t nRows = nrows();
    for (size_t iRow = 0; iRow < nRows; iRow++)
        for (size_t iCol = 0; iCol < ncols(); iCol++)
            sum1 += fabs(getElem(iRow, iCol));
    }
return sum1;
}

//-----------------------------------------------------------------------------
// Apply the given function to each element in matrix.
// See the definitions of P_MAT_FUNC at top of mat.hpp to see meaning of params.
//
// Examples:
//
// A.map(sqrt) returns a mat where each elem is the root of the
// coresponding element in A.
//
// double times2(double Elem) { return 2 * x; }
// A.map(times2) returns a mat where each elem is twice the coresponding
// element in A.

StasmMat StasmMat::map (P_MAT_FUNC pFunc) const
{
StasmMat result(*this);
const size_t nRows = nrows();
for (size_t iRow = 0; iRow < nRows; iRow++)
    for (size_t iCol = 0; iCol < ncols(); iCol++)
        gsl_matrix_set(result.m, iRow, iCol, pFunc(getElem(iRow, iCol)));

return result;
}

//-----------------------------------------------------------------------------
// utility function for squareElems

static inline double square1 (double x)
{
return x * x;
}

//-----------------------------------------------------------------------------
StasmMat  StasmMat::squareElems () const
{
return map(square1);
}

//-----------------------------------------------------------------------------
double StasmMat::trace () const
{
double sum1 = 0;
#if GSL_RANGE_CHECK
CheckIsSquare(*this, "StasmMat trace");
#endif
const size_t nRows = nrows();
for (size_t iRow = 0; iRow < nRows; iRow++)
    sum1 += getElem(iRow, iRow);
return sum1;
}

//-----------------------------------------------------------------------------
// Dot product.
// This is an efficient implementation of: mulElems(other)).sum().

double StasmMat::dot (const StasmMat &other) const
{
const size_t nRows = this->nrows();
const size_t nCols = this->ncols();
ASSERT(other.nrows() == nRows && other.ncols() == nCols);

double result = 0;
const size_t thisTda   = this->m->tda;
const size_t otherTda  = other.m->tda;
const double *thisData = this->m->data;
const double *otherData = other.m->data;

// for speed we check for special cases

if (nRows == 1)                        // column vector?
    for (size_t j = 0; j < nCols; j++)
        result += thisData[j] * otherData[j];

else if (nCols == 1)                   // row vector?
    for (size_t i = 0; i < nRows; i++)
        result += thisData[i * thisTda] * otherData[i * otherTda];

else                                    // not a vector, must be a matrix
    for (size_t i = 0; i < nRows; i++)
        {
        const size_t iThis  = i * thisTda;
        const size_t iOther = i * otherTda;
        for (size_t j = 0; j < nCols; j++)
            result += thisData[iThis + j] * otherData[iOther + j];
        }

return result;
}

//-----------------------------------------------------------------------------
StasmMat StasmMat::diagMe (double d)
{
size_t n;
if (nrows() < ncols())
    n = nrows();
else
    n = ncols();
for (size_t i = 0; i < n; i++)
    gsl_matrix_set(m, i, i, d);
return *this;
}

//-----------------------------------------------------------------------------
// set all elems except diags elems to 0

StasmMat StasmMat::clearOffDiagEntries (void)
{
const size_t nRows = nrows();
for (size_t iRow = 0; iRow < nRows; iRow++)
    for (size_t iCol = 0; iCol < ncols(); iCol++)
        if (iRow != iCol)
            gsl_matrix_set(m, iRow, iCol, 0);

return *this;
}

//-----------------------------------------------------------------------------
// true if a row is all zeroes, or all equal to Val

bool StasmMat::fARowIsZero (double Val) const
{
const size_t nRows = nrows();
for (size_t iRow = 0; iRow < nRows; iRow++)
    {
    bool fAllZero = true;
    for (size_t iCol = 0; iCol < ncols(); iCol++)
        if (getElem(iRow, iCol) != Val)
            {
            fAllZero = false;
            break;
            }
    if (fAllZero)
        return true;
    }
return false;
}

//-----------------------------------------------------------------------------
bool StasmMat::fAllZeroes (double Val) const // true if all values zero, or equal to Val
{
bool fAllZero = true;
const size_t nRows = nrows();
for (size_t iRow = 0; iRow < nRows; iRow++)
    for (size_t iCol = 0; iCol < ncols(); iCol++)
        if (getElem(iRow, iCol) != Val)
            {
            fAllZero = false;
            break;
            }

return fAllZero;
}

//-----------------------------------------------------------------------------
double StasmMat::det (int sign) const
{
StasmMat a = *this;
a = a.LU_decomp();
return gsl_linalg_LU_det(a.m, sign);
}

//-----------------------------------------------------------------------------
double StasmMat::lndet () const
{
StasmMat a = *this;
a = a.LU_decomp();
return gsl_linalg_LU_lndet(a.m);
}

//-----------------------------------------------------------------------------
StasmMat StasmMat::t () const             // transpose of matrix
{
const size_t nRows = nrows();

StasmMat result(ncols(), nRows);

for (size_t i = 0; i < nRows; i++)
    for (size_t j = 0; j < ncols(); j++)
        gsl_matrix_set(result.m, j, i, gsl_matrix_get(m, i, j));

return result;
}

//-----------------------------------------------------------------------------
StasmMat StasmMat::inverse () const
{
gsl_permutation *pPerm = gsl_permutation_alloc(nrows());
StasmMat A = *this;
A = A.LU_decomp(pPerm);
StasmMat AInverse(nrows(), ncols());
int iRet = gsl_linalg_LU_invert(A.m, pPerm, AInverse.m);
ASSERT(iRet == 0);
gsl_permutation_free(pPerm);
return AInverse;
}

//-----------------------------------------------------------------------------
// If pFile is NULL then it is opened.  If pFile is already open then we append
// to it (and sFile isn't used except for user messages).
//
// File format is:
//
// # optional comments each prefixed by #
// "optional mat tag string, can be read back or ignored by StasmMat::read"
// # more optional comments
// {
// nrows ncols          // comments NOT allowed between { and }
// elements...
// }
//
// { and } are header and footer chars (for sanity checking when reading).

void StasmMat::write (const char sFile[], FILE *pFile,
                const char sFormat[],
                const char sTag[],
                bool fLimitOutputWidth) const
{
// must give a filename for error reporting even if pFile is given
ASSERT(sFile && sFile[0]);
bool fCloseFile = false;
if (pFile == NULL)
    {
    pFile = fopen(sFile, "w");
    if (!pFile)
        Err("can't open %s", sFile);
    fCloseFile = true;
    }
size_t nRows = nrows(), nCols = ncols();

if (sTag)
    fprintf(pFile, "\"%s\"\n", sTag);

if (2 != fwrite("{ ", 1, 2, pFile))
    Err("can't write to %s", sFile);

// we don't use gsl_matrix_fprintf because debugging is easier if matrix
// is laid out on a per row basis

if (0 == fprintf(pFile, "%d %d", nRows, nCols))
    Err("can't write matrix to %s", sFile);

// a little tweak so small vectors can be seen more easily
if (nRows == 1 && nCols < 20)
    fprintf(pFile, " ");
else
    fprintf(pFile, "\n");

size_t ncolsMax = nCols;
if (fLimitOutputWidth)
    {
    if (ncolsMax > 20)
        ncolsMax = int(sqrt(double(nCols)));
    if (ncolsMax > 20)
        ncolsMax = 20;
    }
for (size_t iRow = 0; iRow < nRows  && !fgErr; iRow++)
    for (size_t iCol = 0; iCol < nCols && !fgErr; iCol++)
        {
        if (sFormat)
            {
            if (fprintf(pFile, sFormat, getElem(iRow, iCol)) < 1)
                Err("can't write matrix to %s", sFile);
            }
        else if (fprintf(pFile, "%13g", getElem(iRow, iCol)) < 13)
            Err("can't write matrix to %s", sFile);
        // print \n every 20th number and at end of row
        if (iCol % ncolsMax == (ncolsMax-1) || iCol == nCols-1)
            {
            if (fprintf(pFile, "\n") != 1)
                Err("can't write matrix to %s", sFile);
            }
        else if (fprintf(pFile, " ") != 1)
            Err("can't write matrix to %s", sFile);
        }

if (2 != fwrite("}\n", 1, 2, pFile))
    Err("can't write matrix to %s", sFile);

if (fCloseFile)
    fclose(pFile);
}

//-----------------------------------------------------------------------------
void StasmMat::printAux (const char sMsg[], const char sFormat[],    // helper for print()
                    const char sFile[], FILE *pFile, size_t nMax,
                    bool fPrintRowIndex) const
{
bool fCloseFile = false;
if (sFile && pFile == NULL)
    {
    ASSERT(sFile[0]);
    pFile = fopen(sFile, "w");
    if (!pFile)
        Err("can't open %s", sFile);
    fCloseFile = true;
    }
if (nMax == 0)
    nMax = MAX_MAT_DIM;

size_t nRows = MIN(nMax, nrows());
size_t nCols = MIN(nMax, ncols());

if (sMsg)
    {
    // if (pFile)
        // fprintf(pFile, sMsg, nrows(), ncols());
    // else
        // lprintf(sMsg, nrows(), ncols());
    }
if (nRows == 0){}
    // lprintf("Empty matrix\n");
else for (size_t iRow = 0; iRow < nRows; iRow++)
    {
    if (fPrintRowIndex)
        {
        // if (pFile)
            // fprintf(pFile, "%3d: ", iRow);
        // else
            // lprintf("%3d: ", iRow);
        }
    for (size_t iCol = 0; iCol < nCols; iCol++)
        if (pFile)
            {
            // if (sFormat)
                // fprintf(pFile, sFormat, getElem(iRow, iCol));
            // else
                // fprintf(pFile, MAT_PRINT_FORMAT, getElem(iRow, iCol));
            }
        else
            {
            // if (sFormat)

                // lprintf(sFormat, getElem(iRow, iCol));
            // else
                // lprintf(MAT_PRINT_FORMAT, getElem(iRow, iCol));
            }
    if (iRow < nRows)
        {
        // if (pFile)
            // fprintf(pFile, "\n");
        // else
            // lprintf("\n");
        }
    }
if (pFile)
    fflush(pFile);  // flush it out so it can be seen even if application crashes later
else
    fflush(stdout);

if (fCloseFile)
    fclose(pFile);
}

void StasmMat::print (const char sMsg[], const char sFormat[],
                    const char sFile[], FILE *pFile, int nMax,
                    bool fPrintRowIndex) const
{
printAux(sMsg, sFormat, sFile, pFile, nMax, fPrintRowIndex);
}

void StasmMat::print (const char sMsg[], const char sFormat[],
                    const char sFile[], FILE *pFile, int nMax,
                    bool fPrintRowIndex)
{
printAux(sMsg, sFormat, sFile, pFile, nMax, fPrintRowIndex);
}

//-----------------------------------------------------------------------------
// skip comment in matrix file

static void SkipComment (const char sFile[], FILE *pFile)
{
char c = ' ';
while (c != '\n')
    if (1 != fread(&c, 1, 1, pFile))
        Err("can't read line %d of %s (premature EOF)",
            nGetLineNbr(pFile), sFile);
}

//-----------------------------------------------------------------------------
static void ReadTag (char sOptionalTag[], const char sFile[], FILE *pFile)
{
// sTag is the mat prefix we read from file, possibly copied into sOptionalTag
char sTag[SLEN]; sTag[0] = 0;

if (!fgets(sTag, SLEN-1, pFile))
    Err("can't read line %d of %s (premature EOF)",
        nGetLineNbr(pFile), sFile);

if (sOptionalTag)   // user wants the tag?
    {
    int iLen = static_cast<int>(strlen(sTag));
    if (iLen == 0)
        Err("empty tag in line %d of %s", nGetLineNbr(pFile), sFile);
    char *pDest = strchr(sTag, '"');
    if (!pDest)
        Err("unterminated string in line %d of %s", nGetLineNbr(pFile), sFile);
    *pDest = 0; // remove closing quote and possible trailing comment
    strcpy(sOptionalTag, sTag);
    if (sTag[0] == 0)
        Err("empty tag in line %d of %s", nGetLineNbr(pFile), sFile);
    }
}

//-----------------------------------------------------------------------------
// This is our version of the GSL function gsl_matrix_fscanf(pFile, m).
// This version has better error reporting than the original GSL function.
// This assumes we have read the header "{ nrows ncols".
// It reads up to but not including the final "}".

static void ReadData (gsl_matrix *m,                     // out
                      const int nRows, const int nCols,  // in
                      FILE * pFile,                      // in
                      const char sFile[])                // in
{
double *data = m->data;
for (int i = 0; i < nRows * nCols && !fgErr; i++)
    {
    int n;
    char s[SLEN];
    while ((n = fscanf(pFile, "%s", s)) && s[0] == '#') // skip comments
        { ; }                                           // curly braces keep gcc quiet
    if (n == 0)
        {
        long nLine = nGetLineNbr(pFile);
        if (nLine > 0)
            Err("premature end of file at line %d in %s", nLine, sFile);
        else
            Err("can't read from %s", sFile);
        }
    if (s[0] != '#')    // skip comments
        {
        float temp; // msc compiler can't sscanf doubles so use float
        if (1 != sscanf(s, "%g", &temp))
            {
            if (strchr(s, '}'))
                Err("can't read %dx%d matrix from line %d of %s\n"
                    "       Reached end of matrix \"}\" before reading %dx%d values",
                    nRows, nCols, nGetLineNbr(pFile), sFile, nRows, nCols);
            else
                Err("can't read %dx%d matrix from line %d of %s",
                    nRows, nCols, nGetLineNbr(pFile), sFile);
            }
        data[i] = temp;
        }
    }
}

//-----------------------------------------------------------------------------
static char *sprintf1 (const char *pArgs, ...)       // args like printf
{
static char s[1000];
va_list pArg;
va_start(pArg, pArgs);
vsprintf(s, pArgs, pArg);
va_end(pArg);
return s;
}

//-----------------------------------------------------------------------------
// If we try to read a matrix and can't then we call Err, or,
// if if fExitOnErr is false, we return a string with the error msg.
// ReadMatVec uses fExitOnErr to allow reading of an unknown number of mats.
// The "s" prefix indicates that this function returns a string.
//
// If pFile is NULL then the file sFile is opened.
// If pFile is already open then we read from it (and sFile isn't used
// except for user messages).
//
// We copy the matrix tag string into sOptionalTag, if sOptionalTag is not null.
// The caller must ensure that there is enough space in sOptionalTag.
// A tag string is an optional string (with no ~) enclosed in quotes
// before the matrix first delimiter '{'.  It can be used for storing
// filenames and other info for the matrix.
// The leading and closing quotes are removed before copying into sOptionalTag.
// If there is no tag string, sOptionalTag[0] is set to 0.
//
// if sExpectedTag not null then expect tag before the matrix
//
// File format: see StasmMat::write

char *StasmMat::sread (const char sFile[],
                  FILE *pFile,
                  char sOptionalTag[],
                  const char sExpectedTag[],
                  bool fExitOnErr)
{
char *sErr = NULL;  // returned string
bool fLastLineWasATag = false;
bool fFound = false;

// must give a filename for error reporting even if pFile is given
ASSERT(sFile && sFile[0]);
bool fCloseFile = false;
if (pFile == NULL)
    {
    pFile = fopen(sFile, "r");
    if (!pFile)
        {
        sErr = sprintf1("Can't open %s", sFile);
        goto end;
        }
    fCloseFile = true;
    }

char sExpected[SLEN];
sExpected[0] = 0;
if (sExpectedTag && sExpectedTag[0])
    strcat(sExpected, sExpectedTag); // will set sExpected[0]=0 if we match it

char sOptional[SLEN];
if (sOptionalTag)
    sOptionalTag[0] = 0;
else    // need sOptionalTag for sExpectedTag check
    sOptionalTag = sOptional;

// search for first delimiter '{', skipping
// optional comments prefixed by # or tag staring with "

size_t nRows, nCols;
char c;
ASSERT(sizeof(char) == 1);
while (!fFound && !fgErr)
    {
    if (1 != fread(&c, 1, 1, pFile))
        {
        // note that we don't call nGetLineNbr() here because that would
        // slow down ReadMatVec considerably which always "fails" here

        sErr = sprintf1("Can't read matrix header from %s "
                        "(could not find \"{\")", sFile);
        goto end;
        }
    switch (c)
        {
        case '{':
            fFound = true;
            break;
        case '#':                   // comment?
            SkipComment(sFile, pFile);
            break;
        case '"':                   // tag?
            ReadTag(sOptionalTag, sFile, pFile);
            if (fLastLineWasATag)
                Warn("matrix is tagged twice at line %d of %s",
                    nGetLineNbr(pFile), sFile);
            fLastLineWasATag = true;
            if (sExpected[0] && strcmp(sOptionalTag, sExpected))
                {
                sErr = sprintf1("Expected \"%s\" but got "
                                "\"%s\" in line %d of %s",
                                sExpectedTag, sOptionalTag,
                                nGetLineNbr(pFile), sFile);
                goto end;
                }
            sExpected[0] = 0;
            break;
        default:                    // any other char
            fLastLineWasATag = false;
            if (c != '\n' && c != '\r' && c != '\t' && c != ' ')
                {
                sErr = sprintf1("Unexpected character \"%c\" "
                                "in line %d of %s",
                                c, nGetLineNbr(pFile), sFile);
                goto end;
                }
        break;
        }
    } // end while

if (sExpected[0])
    {
    sErr = sprintf1("Did not find expected tag %s "
                    "near line %d of %s",
                    sExpectedTag, nGetLineNbr(pFile), sFile);
    goto end;
    }
int nr, nc;
if (2 != fscanf(pFile, "%d %d", &nr, &nc) || nr < 0 || nc < 0)
    {
    sErr = sprintf1("Can't read matrix size from line %d of %s",
                    nGetLineNbr(pFile), sFile);
    goto end;
    }
nRows = (size_t)nr;
nCols = (size_t)nc;
char sErr1[SLEN];
sprintf(sErr1, "Out-of-range number of rows %d or cols %d in %s",
        nRows, nCols, sFile);
CheckReasonableDim(nRows, nCols, 0, sErr1);

free();
if (nRows * nCols)
    {
    m = gsl_matrix_alloc(nRows, nCols);
    ReadData(m, static_cast<int>(nRows), static_cast<int>(nCols), pFile, sFile);
    }

// make sure that next non-white char is matrix terminator '}'

c = ' ';
while (c == ' ' || c == '\t' || c == '\n' || c == '\r') // skip white space
    if (1 != fread(&c, 1, 1, pFile))
        {
        sErr = sprintf1("Can't read matrix from line %d of %s\n"
                        "       Reached EOF before finding \"}\"",
                        nGetLineNbr(pFile), sFile);
        goto end;
        }
if (c == '#')
    {
    sErr = sprintf1("Illegal comment in line of %d of %s ",
                    nGetLineNbr(pFile), sFile);
    goto end;
    }
if (c != '}')
    {
    sErr = sprintf1("Footer is not \"}\" in line of %d of %s "
                    "(too many entries in mat?)\n",
                    nGetLineNbr(pFile), sFile);
    goto end;
    }
if (fCloseFile)
    fclose(pFile);

end:
if (sErr && fExitOnErr)
    Err(sErr);
return sErr;
}

//-----------------------------------------------------------------------------
void StasmMat::read (const char sFile[],
                FILE *pFile,
                char sOptionalTag[],
                const char sExpectedTag[])
{
sread(sFile, pFile, sOptionalTag, sExpectedTag, EXIT_ON_ERR);
}

//-----------------------------------------------------------------------------
StasmMat StasmMat::normalizeMe ()
{
double Len = this->len();

// We want to be able to normalize zero length vectors without worrying about
// about div by zero or checks exterior to this function. So we do a check here.

if (!fEqual(Len, 0))
    gsl_matrix_scale(this->m, 1.0 / Len);

return *this;
}

StasmMat StasmMat::normalize () const
{
StasmMat result(*this);
double Len = this->len();
if (!fEqual(Len, 0))
    gsl_matrix_scale(result.m, 1.0 / Len);
return result;
}

//-----------------------------------------------------------------------------
double StasmMat::sum () const
{
double sum1 = 0;
const size_t nRows = nrows();
for (size_t iRow = 0; iRow < nRows; iRow++)
    for (size_t iCol = 0; iCol < ncols(); iCol++)
        sum1 += getElem(iRow, iCol);
return sum1;
}

//-----------------------------------------------------------------------------
StasmMat StasmMat::LU_decomp (gsl_permutation *pPerm, int *pSign) const
{
bool fRetPerm = (pPerm != NULL);
if (!pPerm)
    pPerm = gsl_permutation_alloc(nrows());
int Sign;
StasmMat result = *this;
int iRet = gsl_linalg_LU_decomp(result.m, pPerm, &Sign);

// check if matrix is singular i.e. has a zero pivot
// we use FLT_EPSILON as a convenient small nbr
for (int i = static_cast<int>(result.nrows())-1; i >= 0; i--)    // smallest last so look there 1st
    ASSERT(!fEqual(result(i,i), FLT_EPSILON));

ASSERT(iRet == 0);

if (!fRetPerm)
    gsl_permutation_free(pPerm);
if (pSign)
    *pSign = Sign;
return result;
}

//-----------------------------------------------------------------------------
// Solves Ax=b by LU decomposition.
// Returns col vec x and LU factorization in LU.
// A doesn't get changed.
// If A is singular this will fail (it doesn't do least squares or
// anything like that)

StasmVec SolveWithLU (const StasmMat &A, StasmVec &b)
{
#if GSL_RANGE_CHECK
CheckIsVector(b, "SolveWithLU b");
if (A.nrows() != b.nelems())
    Err("SolveWithLU b %dx%d doesn't conform to A %dx%d",
        b.nrows(), b.ncols(), A.nrows(), A.ncols());
#endif

int Sign, iRet;
size_t nRows = A.nrows();

gsl_permutation *pPerm = gsl_permutation_alloc(nRows);
gsl_vector      *xVec  = gsl_vector_alloc(nRows);
gsl_vector_view  bVec  = gsl_vector_view_array(b.m->data, nRows);
gsl_matrix      *pLU   = gsl_matrix_alloc(nRows, A.ncols());

gsl_matrix_memcpy(pLU, A.m);    // use copy not view because decomp destroys A

if (0 != (iRet = gsl_linalg_LU_decomp(pLU, pPerm, &Sign)))
    Err("SolveWithLU gsl_linalg_LU_decomp returned %d", iRet);

if (0 != (iRet = gsl_linalg_LU_solve(pLU, pPerm, &bVec.vector, xVec)))
    Err("SolveWithLU gsl_linalg_LU_solve returned %d", iRet);

StasmVec x(xVec->data, nRows);   // convert gsl_vector to StasmVec
gsl_permutation_free(pPerm);
gsl_vector_free(xVec);
gsl_matrix_free(pLU);

return x;
}

//-----------------------------------------------------------------------------
// Multiply a 3x3 matrix by a 1x2 vector. Used for homogenous transforms.
// Actually, this function also accepts a 2x3 matrix for the 3x3 matrix
// because the last row is ignored.
//
// TODOSTASMS Could be made more efficient by using arrays internally instead of mats.

StasmVec StasmMat::mat33TimesVec2 (const StasmVec &v2) const   // returns row StasmVec(2)
{
#if GSL_RANGE_CHECK

CheckDim(v2, 1, 2, "mat33TimesVec2 vector");
ASSERT(ncols() == 3);
ASSERT(nrows() == 2 || nrows() == 3);

#endif

const double x = v2(0);
const double y = v2(1);

StasmVec v(2, ROWVEC);
v(0) = getElem(0, 0) * x + getElem(0, 1) * y +  getElem(0, 2);
v(1) = getElem(1, 0) * x + getElem(1, 1) * y +  getElem(1, 2);

return v;
}

//-----------------------------------------------------------------------------
MatView StasmMat::view (size_t iStartRow, size_t iStartCol,
                    size_t nRows, size_t nCols, size_t Tda)
{
    MatView View(*this, static_cast<int>(iStartRow), static_cast<int>(iStartCol), static_cast<int>(nRows), static_cast<int>(nCols), static_cast<int>(Tda));
    return View;
}

const MatView StasmMat::view (size_t iStartRow, size_t iStartCol,
                        size_t nRows, size_t nCols, size_t Tda) const
{
    MatView View(*this, static_cast<int>(iStartRow), static_cast<int>(iStartCol), static_cast<int>(nRows), static_cast<int>(nCols), static_cast<int>(Tda));
    return View;
}

VecView StasmMat::row (size_t iRow)
{
    return VecView(*this, iRow, 0, 1, this->ncols());
}

const VecView StasmMat::row (size_t iRow) const
{
    return VecView(*this, iRow, 0, 1, this->ncols());
}

VecView StasmMat::rowAsCol (size_t iRow)
{
    return VecView(*this, iRow, 0, this->ncols(), 1, 1);
}

const VecView StasmMat::rowAsCol (size_t iRow) const
{
    return VecView(*this, iRow, 0, this->ncols(), 1, 1);
}

VecView StasmMat::col (size_t iCol)
{
    return VecView(*this, 0, iCol, this->nrows(), 1);
}

const VecView StasmMat::col (size_t iCol) const
{
    return VecView(*this, 0, iCol, this->nrows(), 1);
}

VecView StasmMat::viewDiagAsCol ()
{
    return VecView(*this, 0, 0, static_cast<int>(this->nrows()), 1, static_cast<int>(ncols())+1);
}

const VecView StasmMat::viewDiagAsCol () const
{
    return VecView(*this, 0, 0, static_cast<int>(this->nrows()), 1, static_cast<int>(ncols())+1);
}

VecView StasmMat::viewAsRow ()
{
    return VecView(*this, 0, 0, 1, this->nrows() * this->ncols());
}

const VecView StasmMat::viewAsRow () const
{
    return VecView(*this, 0, 0, 1, this->nrows() * this->ncols());
}

VecView StasmMat::viewAsCol ()
{
    return VecView(*this, 0, 0, this->nrows() * this->ncols(), 1, 1);
}

const VecView StasmMat::viewAsCol () const
{
    return VecView(*this, 0, 0, this->nrows() * this->ncols(), 1, 1);
}

MatView StasmMat::viewAsSquare ()
{
    size_t nCols = size_t(sqrt(double(this->nelems())));
    // check that can square properly (assertion is not
    // essential but protects the user)
    ASSERT(nCols * nCols == this->nelems());
    return MatView(*this, 0, 0, static_cast<int>(nCols), static_cast<int>(nCols), static_cast<int>(nCols));
}

const MatView StasmMat::viewAsSquare () const
{
    size_t nCols = size_t(sqrt(double(this->nelems())));
    // check that can square properly (assertion is not
    // essential but protects the user)
    ASSERT(nCols * nCols == this->nelems());
    return MatView(*this, 0, 0, static_cast<int>(nCols), static_cast<int>(nCols), static_cast<int>(nCols));
}

//-----------------------------------------------------------------------------
// returns a row vector whose elements are the sums of the columns

StasmVec StasmMat::colSums () const
{
StasmVec Cols(ncols(), ROWVEC);  // initialized to 0
int iRow = static_cast<int>(nrows());
while (iRow--)
    Cols += row(iRow);
return Cols;
}

//-----------------------------------------------------------------------------
// Set elements in the given range to Val.
// See also fill(const double &d) in mat.hpp.

void StasmMat::fill (double Val, size_t iStartRow, size_t iStartCol,
                size_t nEndRow, size_t nEndCol)
{
for (size_t iRow = iStartRow; iRow < nEndRow; iRow++)
    for (size_t iCol = iStartCol; iCol < nEndCol; iCol++)
        gsl_matrix_set(m, iRow, iCol, Val);
}

//-----------------------------------------------------------------------------
// Type convert a one element matrix to a double.
// It's probably possible to do this type conversion automatically but making
// the programmer do it explicitly helps catch errors.

double OneElemToDouble (const StasmMat &A)
{
ASSERT(A.nrows() == 1 && A.ncols() == 1);
return A(0, 0);
}

//-----------------------------------------------------------------------------
// This returns a double equal to x * A * x'.
//
// x is a row vector.
// A is assumed to be a symmetric matrix
// (but only the upper right triangle of A is actually used).
//
// This function is equivalent to OneElemToDouble(x * A * x.t()), but is
// optimized for speed and is much faster.  It's faster because we use the
// fact that A is symmetric to roughly halve the number of operations.  We
// also bypass the overhead of normal matrix routines, which can be slow.

double xAx (const StasmVec &x, const StasmMat &A)
{
// copy x into here because it's faster accessing an array than a StasmVec
double xTemp[MAX_TEMP_ARRAY_LEN];

const size_t nelems1 = x.nelems();

ASSERT(nelems1 < size_t(MAX_TEMP_ARRAY_LEN));
ASSERT(A.nrows() == nelems1);
ASSERT(A.ncols() == nelems1);
ASSERT(x.nrows() == 1);

double DiagResult = 0, Result = 0;

// init xTemp and sum diag elements

size_t i;
for (i = 0; i < nelems1; i++)
    {
    xTemp[i] = x(i);
    DiagResult += A(i, i) * xTemp[i] * xTemp[i];
    }
// Sum lower left triangle elems
// We increase the column (not the row) most rapidly for better processor cache use

for (i = 0; i < nelems1; i++)
    {
    double xi = xTemp[i];
    for (size_t j = i+1; j < nelems1; j++)
        Result += A(j, i) * xi * xTemp[j];
    }
Result *= 2;    // incorporate upper right triangle elements

return DiagResult + Result;
}

//-----------------------------------------------------------------------------
// This function is like DASSERT(A == A.t()) but tells you where the
// difference is, if any
// It's also quicker than testing A == A.t().

bool fIsMatrixSymmetric (const StasmMat &A, double Min)
{
bool fExact = (Min == 0.0);

if (A.nrows() != A.ncols())
    {
    // lprintf("\nMatrix not square\n");
    return false;
    }
if (fExact)
    {
    const size_t nRows = A.nrows();
    for (size_t iRow = 0; iRow < nRows; iRow++)
        for (size_t iCol = iRow+1; iCol < A.ncols(); iCol++)
            if (A(iRow, iCol) != A(iCol, iRow))
                {
                // lprintf("\nMatrix not exactly symmetric: A(%d,%d)=%g != A(%d,%d)=%g   A(%d,%d)-A(%d,%d)=%g\n",
                    // iRow, iCol, A(iRow, iCol), iCol, iRow, A(iCol, iRow),
                    // iRow, iCol, iRow, iCol, A(iRow, iCol)-A(iCol, iRow));
                return false;
                }
    }
else
    {
    Min = ABS(Min);
    const size_t nRows = A.nrows();
    for (size_t iRow = 0; iRow < nRows; iRow++)
        for (size_t iCol = iRow+1; iCol < A.ncols(); iCol++)
            {
            if (!fEqual(A(iRow, iCol), A(iCol, iRow), Min))
                {
                // lprintf("\nMatrix not symmetric: A(%d,%d)=%g != A(%d,%d)=%g   A(%d,%d)-A(%d,%d)=%g   Min=%g\n",
                    // iRow, iCol, A(iRow, iCol), iCol, iRow, A(iCol, iRow),
                    // iRow, iCol, iRow, iCol, A(iRow, iCol)-A(iCol, iRow), Min);
                return false;
                }
            }
    }
return true;
}

//-----------------------------------------------------------------------------
// Returns a matrix of eigenvectors for the symmetric matrix A.
//
// Each column of the returned matrix will be an eigenvector, normalized to
// unit length.  The eigenvals are returned in EigenVals.
// If fSort is set, then eigenvecs and eigenvals are sorted, largest
// eigenval first.
// A must be a symmetric matrix. If fTestForSymmetry then Err if A not symmetric.
// A is not modified.
//
// fFixSigns: the sign of an eigenvector is not uniquely determined by the matrix.
//   For consistency across software versions, if fFixSigns=true (the default)
//   then we flip signs if necessary so the largest (in magnitude) eigenvector
//   element is positive.

StasmMat GetEigsForSymMat (const StasmMat &A, StasmVec &EigVals, bool fSort,
                      bool fTestForSymmetry, bool fFixSigns)
{
int iRet;

// some preliminary checks, GSL funcs will do more

ASSERT(A.nrows() == A.ncols());
ASSERT(EigVals.nrows() == A.nrows());

//TODOSTASMS magic 1e5 chosen based for ASM model building, make it a parameter?
if (fTestForSymmetry && !fIsMatrixSymmetric(A, A(0,0) / 1e5))
    Err("GetEigsForSymMat matrix %dx%d not symmetric", A.nrows(), A.ncols());

StasmMat EigVecs(A.ncols(), A.ncols());

StasmMat Work(A); // gsl_eigen_symmv() destroys the A->m.data, so we use temp variable

gsl_eigen_symmv_workspace *W = gsl_eigen_symmv_alloc(Work.ncols());
gsl_vector_view EigValsVec = gsl_vector_view_array(EigVals.m->data,
                                                   EigVals.nrows());

if (0 != (iRet = gsl_eigen_symmv(Work.m, &EigValsVec.vector, EigVecs.m, W)))
    Err("GetEigsForSymMat matrix %dx%d gsl_eigen_symmv returned %d",
        Work.nrows(), Work.ncols(), iRet);

gsl_eigen_symmv_free(W);

// if asked to, sort the eigenvalues and corresponding eigenvectors

if (fSort && 0 != (iRet =
        gsl_eigen_symmv_sort(&EigValsVec.vector, EigVecs.m,
                            GSL_EIGEN_SORT_VAL_DESC)))
    {
    Err("GetEigsForSymMat matrix %dx%d gsl_eigen_symmv_sort returned %d",
        Work.nrows(), Work.ncols(), iRet);
    }
if (fFixSigns)
    for (unsigned iCol = 0; iCol < EigVecs.ncols(); iCol++)  // iCol is the eigenvector number
        {
        double Max = -1; unsigned iMax = 0; unsigned iRow;
        for (iRow = 0; iRow < Work.nrows(); iRow++)     // iRow is the elem in eigenvector
            if (ABS(EigVecs(iRow, iCol)) > Max)
                {
                Max = ABS(EigVecs(iRow, iCol));
                iMax = iRow;
                }
        if (EigVecs(iMax, iCol) < 0)    // largest elem in eigenvec is negative?
            for (iRow = 0; iRow < Work.nrows(); iRow++) // flip signs
                EigVecs(iRow, iCol) = -EigVecs(iRow, iCol);
        }

return EigVecs;
}

//-----------------------------------------------------------------------------
// Solves Ax=b for x and returns x.  Destroys A.
// This routine silently sets pfPosDef to false and immediately returns
// if A is not symmetric positive definite and it can't be solved by Cholesky.

StasmVec SolveWithCholesky (StasmMat &A, const StasmVec &b, bool *pfPosDef)
{
StasmVec x(A.nrows());

*pfPosDef = false;  // assume

if (0 == gsl_linalg_cholesky_decomp_return_on_EDOM(A.m))
    {
    int iRet;

    gsl_vector_view bVec = gsl_vector_view_array(b.m->data, A.nrows());
    gsl_vector_view xVec = gsl_vector_view_array(x.m->data, A.nrows());

    if (0 != (iRet = gsl_linalg_cholesky_solve(A.m, &bVec.vector, &xVec.vector)))
        Err("SolveWithCholesky gsl_linalg_cholesky_solve returned %d", iRet);

    *pfPosDef = true;
    }
return x;
}

//-----------------------------------------------------------------------------
// Return true if A is positive definite

bool fPosDef (const StasmMat &A)
{
StasmMat ACopy(A);   // SolveWithCholesky destroys A, so make a copy
StasmVec b(A.nrows());
bool fPosDef;
SolveWithCholesky(ACopy, b, &fPosDef);
return fPosDef;
}

//-----------------------------------------------------------------------------
// true if x is a "normal" IEEE number

static inline bool fIeeeNormal1 (double x)
{
#if _MSC_VER
    int c = _fpclass(x);

    // Negative normal | -0 | +0 | Positive normal

    return (c & (_FPCLASS_NN|_FPCLASS_NZ|_FPCLASS_PZ|_FPCLASS_PN)) != 0;
#else

    return isnormal(x);
#endif
}

//-----------------------------------------------------------------------------
bool StasmMat::fIeeeNormal () const  // true if fIeeeNormal true for all elems of mat
{
const size_t nRows = nrows();

for (size_t iRow = 0; iRow < nRows; iRow++)
    for (size_t iCol = 0; iCol < ncols(); iCol++)
        if (!fIeeeNormal1(getElem(iRow, iCol)))
            return false;
return true;
}

//-----------------------------------------------------------------------------
double StasmMat::maxAbsElem () const
{
double max = -DBL_MIN;
const size_t nRows = nrows();

for (size_t iRow = 0; iRow < nRows; iRow++)
    for (size_t iCol = 0; iCol < ncols(); iCol++)
        {
        double Elem = fabs(getElem(iRow, iCol));
        if (Elem  > max)
            max = Elem;
        }
return max;
}

//-----------------------------------------------------------------------------
StasmMat IdentMat (size_t n)     // returns an identity mat of the given size
{
StasmMat result(n, n);
result.diagMe(1);
return result;
}

//-----------------------------------------------------------------------------
// Clear (i.e. set to 0) all elems that are absolute smaller than MinVal
// If MinVal==0 then nothing will be changed

void StasmMat::clearSmallElems (double MinVal) const
{
ASSERT(MinVal >= 0);

if (MinVal == 0.0)      // for efficiency, not strictly needed
    return;

const size_t nRows = nrows();

for (size_t iRow = 0; iRow < nRows; iRow++)
    for (size_t iCol = 0; iCol < ncols(); iCol++)
        if (fabs(gsl_matrix_get(m, iRow, iCol)) < MinVal)
            gsl_matrix_set(m, iRow, iCol, 0);
}

//-----------------------------------------------------------------------------
StasmMat StasmMat::identityMe (double DiagVal)
{
gsl_matrix_set_zero(m);
return diagMe(DiagVal);
}

}   // end namespace GslMat

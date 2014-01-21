// $matview.hpp 3.0 milbo$ matrix view class, see mat.hpp for a description

#if !defined(matview_hpp)
#define matview_hpp

#include "stasm.hpp"

// namespace swStasm
// {

	namespace GslMat
	{
	//-----------------------------------------------------------------------------
	// Note: a destructor is unneeded because ~StasmMat suffices.  When ~StasmMat destructs
	// a MatView, it sees that m->owner == 0 and thus it knows that it should not
	// release the matrix being viewed.
	// Address-of operator also unneeded, compiler default is ok.

	class MatView : public StasmMat
	{
	public:
	MatView () {}   // default constructor

	MatView (const StasmMat     &other) { init(other); } // copy constructors

    // TODOSTASMS following causes gcc 3.4.1 -Wextra warning: base class should be explicitly initialized
	//MatView (const MatView &other) { init(other); }

	// view matrix/view/array as a matrix by initializing a view with two dimensions

	MatView (const StasmMat &other, size_t iStartRow, size_t iStartCol,
		    size_t nRows, size_t nCols, int Tda=0) :StasmMat()
	    {
	    init(other, iStartRow, iStartCol, nRows, nCols, Tda);
	    }
	MatView (double other[], size_t iStartRow, size_t iStartCol,
		    size_t nRows, size_t nCols, int Tda=0) :StasmMat()
	    {
	    init(other, iStartRow, iStartCol, nRows, nCols, Tda);
	    }
	MatView (double other[], size_t nRows, size_t nCols, int Tda) :StasmMat()
	    {
	    init(other, 0, 0, nRows, nCols, Tda);
	    }
	// view matrix/view/array as vector by initializing a view with single dimension
	// See also row, rowAsCol, etc.

	MatView (const StasmMat &other, size_t iStart, char sIsRowVec[]=NULL) :StasmMat()
	    {
	    sIsRowVec? init(other, iStart, 0, 1, other.ncols()):
		init(other, 0, iStart, other.nrows(), 1);
	    }
	MatView (double other[], size_t iStart, size_t nelems1, char sIsRowVec[]=NULL) :StasmMat()
	    {
	    sIsRowVec? init(other, iStart, 0, 1, nelems1, 1): init(other, 0, iStart, nelems1, 1);
	    }
	MatView (double other[], size_t nelems1, char sIsRowVec[]=NULL): StasmMat()
	    {
	    sIsRowVec? init(other, 0, 0, 1, nelems1, 1): init(other, 0, 0, nelems1, 1);
	    }
	// Equals operator copies all matrix elements (not just struct m)
	// If you just want to refer to another matrix without copying elements, use init(other)

	MatView& operator= (const MatView &other)
	    {
	    if (static_cast<const void *>(this) == static_cast<const void *>(&other))
		return *this;
	    ASSERT(other.m);    // this func doesn't handle zero size other (yet)
	    if (!m)
		{
		m = (gsl_matrix *)calloc(sizeof(gsl_matrix), 1);
		memcpy(m, other.m, sizeof(gsl_matrix));
		}
	    gsl_matrix_memcpy(m, other.m);
	    m->block = NULL;
	    m->owner = 0;
	    return *this;
	    }
	// redimension (this simply changes the sizes and doesn't change the underlying data)

	void dim (size_t nRows, size_t nCols)
	    {
	    m->size1 = nRows;
	    m->size2 = nCols;
	    m->tda = nCols;
	    }
	void init (const StasmMat& other)
	    {
	    ASSERT(static_cast<const void *>(this) != static_cast<const void *>(&other));
	    if (!m)
		m = (gsl_matrix *)calloc(sizeof(gsl_matrix), 1);
	    if (other.m)
		memcpy(m, other.m, sizeof(gsl_matrix));
	    m->block = NULL;
	    m->owner = 0;
	    }
	void init (const StasmMat& other, size_t iStartRow, size_t iStartCol,
				size_t nRows, size_t nCols, int Tda=0)
	    {
	    ASSERT(static_cast<const void *>(this) != static_cast<const void *>(&other));
	    ASSERT(other.m);    // doesn't make sense to view a zero sized matrix this way
	    #if GSL_RANGE_CHECK
	    CheckReasonableDim(iStartRow, iStartCol, 0, "MatView bad start row %d or column %d");
	    CheckReasonableDim(nRows, nCols, 1, "MatView bad number of rows %d or columns %d");
	    #endif

	    if (!m)
		m = (gsl_matrix *)malloc(sizeof(gsl_matrix));
	    m->size1 = nRows;
	    m->size2 = nCols;
	    m->tda = other.m->tda;
	    if (Tda > 0)
		m->tda = Tda;
	    m->data = other.m->data + (iStartRow * other.m->tda + iStartCol);
	    m->block = NULL;
	    m->owner = 0;

	    #if GSL_RANGE_CHECK
	    // check that the end of the view isn't past the end of the mat it is viewing

	    const double *pEndThis = &(*this)(nrows()-1, ncols()-1);
	    const double *pEndOther = &other(other.nrows()-1, other.ncols()-1);
	    if (pEndOther - pEndThis < 0)
		{
		Err("MatView::view %dx%d tda %d "
		    "(start %dx%d len %dx%d) doesn't fit into %dx%d tda %d (%d elements too big)",
		    m->size1, m->size2, m->tda, iStartRow, iStartCol, nRows, nCols,
		    other.m->size1, other.m->size2, other.m->tda, pEndThis - pEndOther);
		}
	    #endif
	}
	void init (double other[], size_t iStartRow, size_t iStartCol,
				size_t nRows, size_t nCols, int Tda=0)
	    {
	    ASSERT(static_cast<const void *>(this) != static_cast<const void *>(&other));

	    #if GSL_RANGE_CHECK
	    CheckReasonableDim(iStartRow, iStartCol, 0, "MatView bad start row %d or column %d");
	    CheckReasonableDim(nRows, nCols, 1, "MatView bad number of rows %d or columns %d");

	    if (iStartRow != 0 && iStartCol != 0)   //TODO figure out sensible implementation
		Err("MatView non-zero start row %d and start col %d not yet implemented",
		    iStartRow, iStartCol);
	    #endif

	    if (!m)
		m = (gsl_matrix *)malloc(sizeof(gsl_matrix));
	    m->size1 = nRows;
	    m->size2 = nCols;
	    m->tda = nCols;
	    if (Tda > 0)
		m->tda = Tda;
	    m->data = other;
	    m->block = NULL;
	    m->data += iStartRow * m->tda + iStartCol;
	    m->owner = 0;
	    }
	};  // end class MatView
	}   // end namespace GslMat
// };
#endif // matview_hpp

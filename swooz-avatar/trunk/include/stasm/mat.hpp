// $mat.hpp 3.0 milbo$ matrix classes, matrix elements are doubles
// Initially derived from gslwrap-0.2\matrix_double.h
//
// Note: this has been stripped down for the "stasm" build.
//       Refer to the original mat.hpp if you want more functionality.
//
// This file needs a few external functions: Err() Err() fEqual() lprintf(), ...
//
// Facilities Provided
// -------------------
//
// This is a C++ wrapper class for the Gnu Scientific library matrices.  It provides
// two classes: StasmMat and MatView.  It allows matrix expressions like A + B
// where A and B are matrices: the + operator is overloaded.  It also has
// facilities for the inverse and other linear algebra functions. This library
// is good for non-sparse matrices with n in dozens or hundreds.  It is not good
// for the specialized matrix functions used in graphics.
//
// The MatView class allows different views of a matrix. For example, you
// can view a matrix column as a vector.  Or create a view that is a
// sub matrix of a larger matrix.  Matrix views point to the same underlying
// data as the matrix they are viewing -- they are an alias to the same data.
// A speed advantage of views is that they allow you to manipulate a matrix
// without copying the actual elements of the matrix.
//
// A variable of the StasmMat class has a single element m which is a GSL
// matrix of doubles. Complex matrices aren't supported -- thus when
// extracting eigenvalues, matrices must be symmetric.
//
// The approach is to make programmer errors as visible as possible, although
// this can often only be done at run time by issuing an error msg (for example
// for matrix conformance mismatches).  Convenience extensions like negative
// indexing aren't allowed.  This seems appropriate in this compiled
// non-interactive environment.
//
// Assignment
// ----------
//
// You can't assign different sized matrices using "=" (but see AUTO_REDIM_ON_ASSIGN).
// Thus you will get a runtime error message if the size of mat1 size is not the
// same as the size as mat1 in the assignment
//      mat1 = mat2;
// There is an exception though: if mat1's size is 0 then it will automatically
// be resized by the above statement.  This allows code like the following:
//
//      StasmMat mat1;       // default constructor
//      ...
//      mat1 = mat2;    // this assignment could be in a different function
//
// If you want to assign different sized matrices, use assign().
//
// The () operator
// ---------------
//
// Matrices are indexed using (iRow,iCol) or (y,x). This is the mathematical/linear
// algebra convention, but is opposite to what you may expect: (x,y).  Matrices are
// stored using row-major ordering i.e. elements in each row are stored contigously.
//
// Indexed matrices like A(1,2) or A(3) always refer to a single element of A,
// except when defining a matrix.
// If you want to refer to a whole column or row conveniently, use views e.g. row()
// Vectors have the same type as matrices, so you can mix and match
// vectors and matrices in expressions,  as long as the dimensions conform.
//
// dyadic ()    X(1,2) refers to an element of X if X is a matrix -- row 1 col 2
//              X(1,2) is illegal if X is a vector (will get runtime error msg)
//              X(0,2) is okay if X is a row vector
//
// monadic ()   X(1)   refers to an element of X if X is a vector
//              X(1)   is illegal if X is an two-dimesional matrix (runtime error msg)
//
// Accessing a one-dimensional matrix using 2 subscripts (one at 0) is slightly
// faster than accessing it using one subscript.  Using just one subscript can make
// the programmer's intention clearer though.
//
//
// Declaring Vectors
// -----------------
//
// Vec v(3);        creates a column vector e.g.  1     // identical to StasmMat v(3,1)
//                                                2
//                                                3
//
// Vec v(3,"");     creates a row vector e.g. [1 2 3]   // identical to StasmMat v(1,3)
//
// In the above declaration, "" can be any string.  A string type
// was chosen so (3,"") could be disambiguated from (3,i) where i is an int.
// By convention, you should use ROWVEC for the string.
//
//
// Error handling, and speed
// -------------------------
//
// If you this use module and the GSL library with GSL_RANGE_CHECK enabled,
// then all matrix indexes are checked at run time.  To help locating bugs, this
// code sometimes precheck matrix indexes (redundantly) before calling GSL
// functions.  These range checks help a lot,  but of course all this
// checking slows things down.  So what you should do is build two binaries for
// this class and the GSL object files: one with range checking on, one
// with it off.  You also want to HAVE_INLINE defined to 1 in the GSL library.
//
// The code runs 2..4 times faster with compiler optimizations enabled and
// GSL_RANGE_CHECK disabled (tested with MS compiler on test.cpp and the
// test.cpp parameter NBIG varying from 100 to 500).
// Compiling takes noticeably longer with optimizations enabled.
//
// This class needs some exernal functions e.g. Err().  Look at the make file
// for test.cpp to see where these functions are defined.
// It is a good idea to point the GSL error handler to Err so error handling
// is uniform across the application. See mat/test.cpp
//
// Zero sized matrices
// -------------------
//
// TODOSTASMS are these comments still correct?
// Matrices with no rows or no cols have m==NULL.  This is convenient for new()
// and related funcs on arrays of matrices.
// For efficiency, not all member functions can be used on zero sized matrices.
// Accessing array elements using (x,y) is illegal on zero size arrays -- you
// will get a null pointer crash.  Changing this would introduce a
// not-worth-it inefficiency in operator().
// Some functions that are legal on zero sized matrices:
//      free dim dimClear getDim nelems nrows ncols size1 size2
//
// Notes
// -----
//
// Matrix expressions like A + B are somewhat inefficient, because we have
// to create a hidden temporary matrix for the result.  But these
// expressions are notationally convenient.  "+=" is faster than "+".
//
// Also, within a function
//      static StasmMat m; m.dimClear(nRows, nCols)
// is quicker than
//      StasmMat m(nRows, nCols)
// because no mallocs and frees are needed if nrows and ncols are
// the same as the previous time you called the function.
// But this only matters if you call the function a lot.
//
// TODOSTASMS
// ----
//
// add matView(anotherMat) which gives a view of a mat using anotherMat's dimensions
// use func pointer instead of fprintf/lprintf in printAux
// remove operators from class so dyadic doubles ops can be in either order
//
//
// History
// -------
//
// The following people wrote the original gslwrap class. This class is a
// modification of that -- specifically from gslwrap-0.2\matrix_double.h
//      Ramin Nakisa
//      Brian Gough, rodney sparapani, Marcel Bosc, Nigel Wetters
// Note that gslwrap-0.2 (which was the latest version when this project
// started) did not build under Microsoft C and that was the original
// motivation for this code.
//
// milbo jun 05, petaluma

#if !defined(mat_hpp)
#define mat_hpp

#include "stasm.hpp"
	#define StasmVec     StasmMat         // vectors are matrices with 1 row or 1 col
	#define VecView MatView     // ditto

// namespace swStasm
// {


	// mostly used for parameter sanity checking but also used sometimes for a max temp alloc
	static const size_t MAX_MAT_DIM = 10000;

	// arbitrary max len of temporary arrays used internally in some functions
	static const int MAX_TEMP_ARRAY_LEN = 1000;

	static const bool NO_CLEAR = false; // for fClear parameter when initing a mat
	static const bool CLEAR = true;

	static const unsigned VX = 0;       // X,Y,Z coords for indexing, by convention
	static const unsigned VY = 1;       // we use these rather than 0,1,3
	static const unsigned VZ = 2;

	// If you set AUTO_REDIM_ON_ASSIGN, you can have statements like A = B
	// where A is of different dimensions to B.  A will be automatically
	// redimensioned to match B.  This is a global setting for all matrices.
	// But I prefer AUTO_REDIM_ON_ASSIGN=false because it helps catch
	// programming errors.
	//
	// There is an exception: matrices of size 0 are always redimensioned as
	// needed (regardless of the AUTO_REDIM_ON_ASSIGN setting) if they are on
	// the left hand side of an assignment.  This allows, for example:
	//      StasmMat a; A = anotherMat;

	// static const bool AUTO_REDIM_ON_ASSIGN = false;
	static const bool AUTO_REDIM_ON_ASSIGN = true;

	// ROWVEC is used when defining define a row vector (as opposed to a
	// column vector): use this as the 2nd index to make your intentions
	// clear when defining the vector.

	static const char * const ROWVEC = "";

	#if GSL_RANGE_CHECK
	static const char *MAT_OPERATOR_SINGLE_ARG_ILLEGAL = \
	    "StasmMat::operator() single arg illegal on mat with ncols>1 or nrows>1";
	#endif

	// Function argument for map function, see map function header for details.
	// P_MAP_FUNC takes a single double argument which is the matrix element.

	typedef double (*P_MAT_FUNC)(double);

	//-----------------------------------------------------------------------------
	namespace GslMat
	{
	class StasmMat;
	class MatView;
	
	// externs for functions defined in mat.cpp
	extern void gslErrHandler(const char *reason, const char *file,
				  int line, int gsl_errno);
	extern double OneElemToDouble(const StasmMat &A);
	bool fSameDim(const StasmMat &A, const StasmMat &B);
	extern void CheckReasonableDim(size_t nRows, size_t nCols, size_t nMin, const char sMsg[]);
	extern void CheckDim(const StasmMat &A, size_t nRows, size_t nCols, const char sMsg[]);
	extern void CheckSameNbrRows(const StasmMat &A, const StasmMat &B, const char sMsg[]);
	extern void CheckIsVector(const StasmMat &A, const char sMsg[]);
	extern void CheckIsSquare(const StasmMat &A, const char sMsg[]);
	extern void CheckSameDim(const StasmMat &A, const StasmMat &B, const char sMsg[]);
	extern StasmVec  SolveWithLU(const StasmMat &A, StasmVec &b);
	extern double xAx(const StasmVec &x, const StasmMat &A);
	extern bool fPosDef(const StasmMat &A);  // true if A is positive definite
	extern StasmMat GetEigsForSymMat(const StasmMat &A, StasmVec &EigVals, bool fSort=true, bool fTestForSymmetry=true, bool fFixSigns=true);
	extern bool fIsMatrixSymmetric(const StasmMat &A, double Min=0.0);
	extern StasmVec SolveWithCholesky(StasmMat &A, const StasmVec &b, bool *pfPosDef);
	extern StasmMat  IdentMat(size_t n);     // returns an identity mat of the given size

	//-----------------------------------------------------------------------------
	class StasmMat
	{
	public:

	// For convenient reference, this is what a gsl_matrix is (defined in gsl_matrix_double.h)
	//  typedef struct
	//  {
	//    size_t size1;         // nrows
	//    size_t size2;         // ncols
	//    size_t tda;
	//    double *data;
	//    gsl_block *block;
	//    int owner;            // non-zero if own the block pointed to data
	//  } gsl_matrix;

	gsl_matrix *m;  // the underlying GSL matrix

	void copy(const StasmMat &other)  // used for copy constructor and assignment
	    {
	    if (static_cast<const void *>(this) == static_cast<const void *>(&other))
		return;
	    if (other.m == NULL)    // zero sized other?
		free();
	    else
		{
		if (m == NULL)      // zero sized this?
		    m = gsl_matrix_alloc(other.nrows(), other.ncols());
		else if (nrows() != other.nrows() || ncols() != other.ncols())
		    {
		    if (AUTO_REDIM_ON_ASSIGN)
			{
			gsl_matrix_free(m);
			m = gsl_matrix_alloc(other.nrows(), other.ncols());
			}
		    else
			Err("mats not conformable assigning %dx%d to %dx%d",
			    other.nrows(), other.ncols(), nrows(), ncols());
		    }
		gsl_matrix_memcpy(m, other.m);
		}
	    }
	StasmMat(): m(NULL) { setGslErrHandler(); } // default constructor

	StasmMat(size_t nRows, size_t nCols, bool fClear=true)
	    {
	    setGslErrHandler();
	    if (nRows == 0 || nCols == 0)
		m = NULL;
	    else
		m = (fClear? gsl_matrix_calloc(nRows, nCols): gsl_matrix_alloc(nRows, nCols));
	    }
	StasmMat(size_t nelems1, const char sIsRowVec[]=NULL, bool fClear=true)
	    {                       // sIsRowVec is a string instead of a bool to disambiguate type
	    setGslErrHandler();
	    if (nelems1 == 0)
		m = NULL;
	    else
		{
		if (sIsRowVec)      // create row vector if any string (use ROWVEC defined above)
		    m = (fClear? gsl_matrix_calloc(1, nelems1):
			    gsl_matrix_alloc(1, nelems1)); // row matrix: one row, many columns
		else
		    m = (fClear? gsl_matrix_calloc(nelems1, 1):
			    gsl_matrix_alloc(nelems1, 1)); // column matrix: one column, many rows
		}
	    }
	StasmMat(const double Vals[], size_t nRows, size_t nCols);  // declare and init from an array

	// declare as vec and init
	StasmMat(const double Vals[], size_t nelems1, const char sIsRowVec[]=NULL);

	StasmMat(const StasmMat &other)       // copy constructor
	    {
	    setGslErrHandler();
	    if (other.m)
		{
		m = gsl_matrix_alloc(other.nrows(), other.ncols());
		copy(other);
		}
	    else
		m = NULL;
	    }
	~StasmMat() { free(); }

	void setGslErrHandler (void)  // assign func to be called on GSL err
	    {
	    static bool fSet = false;
	    if (!fSet)
		{
		gsl_set_error_handler(gslErrHandler);
		fSet = true;
		}
	    }
	size_t nelems() const { if (m == NULL) return 0; else return m->size1 * m->size2; }

	size_t nrows() const  { if (m == NULL) return 0; else return m->size1; }
	size_t ncols() const  { if (m == NULL) return 0; else return m->size2; }

	size_t size1() const  { if (m == NULL) return 0; else return m->size1; }
	size_t size2() const  { if (m == NULL) return 0; else return m->size2; }

	void   getDim(int *pnRows, int *pnCols) const;

	// release underlying GSL mat (gsl_matrix_free checks if owner before doing the free)

	void   free() { if (m) gsl_matrix_free(m); m = NULL; }

	// (i,j) operator -- for accessing mat elements

	const double &operator()(size_t iRow, size_t iCol) const
	    { return *gsl_matrix_ptr(m, iRow, iCol); }

	double &operator()(size_t iRow, size_t iCol)
	    { return *gsl_matrix_ptr(m, iRow, iCol); }

	// (i) operator -- for accessing mat elements when matrix is treated like a vector

	const double &operator()(size_t i) const
	    {
	    if (nrows() == 1)
		return *gsl_matrix_ptr(m, 0, i);
	#if GSL_RANGE_CHECK
	    if (ncols() != 1)   // must have either 1 col or 1 row to be a vec
		Err(MAT_OPERATOR_SINGLE_ARG_ILLEGAL);
	#endif
	    return *gsl_matrix_ptr(m, i, 0);
	    }

	double &operator()(size_t i)
	    {
	    if (nrows() == 1)
		return *gsl_matrix_ptr(m, 0, i);
	#if GSL_RANGE_CHECK
	    if (ncols() != 1)   // must have either 1 col or 1 row to be a vec
		Err(MAT_OPERATOR_SINGLE_ARG_ILLEGAL);
	#endif
	    return *gsl_matrix_ptr(m, i, 0);
	    }

	double getElem(size_t iRow, size_t iCol) const
	    { return  gsl_matrix_get(m, iRow, iCol); }

	void setElem(size_t row1, size_t col1, const double &v)
	    { gsl_matrix_set(m, row1, col1, v); }

	void dim(size_t nRows, size_t nCols);  // redimension, new contents unspecified
	void dim(size_t nelems, const char sIsRowVec[]=NULL);
					// redimension as vec, new contents unspecified
	void dim(const StasmMat &other);     // redimen to same size as other, new contents unspecified
	void dimKeep(size_t nRows, size_t nCols);  // redimension, keep old data
	void dimClear(size_t nRows, size_t nCols); // redimension the mat and zero it
	void assign(const StasmMat &other);  // redim to same size as other and copy other into this

	// return a redimensioned submatrix of this matrix
	StasmMat reshape(size_t nNewRows, size_t nNewCols, bool fRowMajor=false,
	      size_t nStartRow=0, size_t nStartCol=0, size_t nRows=0, size_t nCols=0);

	StasmMat reshapeMe(size_t nNewRows, size_t nNewCols, bool fRowMajor=false,
	      size_t nStartRow=0, size_t nStartCol=0, size_t nRows=0, size_t nCols=0);

	// return submatrix of this matrix
	StasmMat submat(size_t nStartRow, size_t nStartCol, size_t nRows=0, size_t nCols=0)
	      { return reshape(nRows, nCols, false, nStartRow, nStartCol); }

	StasmMat t() const;                  // transpose of this matrix

	// assign (i.e. "=") copies all elements of other
	// (so use views instead if you want to refer to another matrix without copying)
	StasmMat&        operator=(const StasmMat &other) { copy(other); return *this; }
	StasmMat&        operator=(const double Vals[]); // assign an array of doubles
	StasmMat&        operator=(const double Val);    // assign a scalar to all elements of mat

	bool        operator==(const StasmMat &other) const;
					    // does exact compare, use fEqualMat if want tolerance
	bool        operator!=(const StasmMat &other) const { return !(*this == other); }

	StasmMat         operator+(const StasmMat &other) const;
	StasmMat         operator+(const double &f) const;
	friend StasmMat  operator+(const double &f, const StasmMat &other);
	StasmMat         &operator+=(const double &f);
	StasmMat         &operator+=(const StasmMat &other);

	StasmMat         operator-(const StasmMat &other) const;
	StasmMat         operator-(const double &f) const;
	friend StasmMat  operator-(const double &f, const StasmMat &other);
	StasmMat         &operator-=(const double &f);
	StasmMat         &operator-=(const StasmMat &other);

	StasmMat         operator*(const StasmMat &other) const;
	StasmMat         operator*(const double &f) const;
	friend StasmMat  operator*(const double &f, const StasmMat &other);
	StasmMat         &operator*=(const double &f);
	StasmMat         &operator*=(const StasmMat &other);

	StasmMat         operator/(const double &) const;
	StasmMat         &operator/=(const double &);

	StasmMat         mulElems(const StasmMat &other) const;
	StasmMat         mulElems(const StasmMat &other);
	StasmMat         divElems(const StasmMat &other) const;
	StasmMat         divElems(const StasmMat &other);
	StasmMat         map(P_MAT_FUNC pFunc) const; // applies pFunc to each elem
	StasmMat         squareElems() const;        // returns square of elems

	double sum() const;                 // scalar sum of all elements
	double absSum() const;              // scalar sum of absolute value of elements
	double trace() const;               // sum of diag elements
	double dot(const StasmMat &other) const; // dot product
	double firstElem() const            { return getElem(0,0); };
	double lastElem() const             { return getElem(nrows()-1, ncols()-1); };
	double distSquared(StasmMat &other)      { return (*this - other).squareElems().sum(); }
	double distSquared(const StasmMat &other) const { return (*this - other).squareElems().sum(); }
	double maxElem() const              { return gsl_matrix_max(m); }
	double maxAbsElem() const;              // max of absolute value of elements
	double minElem() const              { return gsl_matrix_min(m); }
	double l2Norm() const               { return sqrt(dot(*this)); }
	double len() const                  { return sqrt(dot(*this)); }    // same as l2Norm()
	double lenSquared() const           { return dot(*this); }
	StasmMat    fill(const double &d)        { gsl_matrix_set_all(m, d); return *this; }
						// set all elems to a scalar value
	void   fill(double Val, size_t iStartRow, size_t iStartCol, size_t nEndRow, size_t nEndCol);
						// set specified elems to a scalar value
	StasmMat    normalizeMe();                   // uses L2 norm
	StasmMat    normalize() const;               // ditto
	StasmMat    identityMe(double DiagVal=1);    // turn mat into an diag mat (DiagVal on diag, others 0)
	StasmMat    transposeMe()    { assign(t()); return *this; }
						// transposes the mat, redims if necessesary

	StasmMat    diagMe(double f);                // set diag elems to f, doesn't clear other elems
	StasmMat    clearOffDiagEntries(void);       // set all elems except diags elems to 0
	StasmMat    zeroMe()         { gsl_matrix_set_zero(m); return *this;}    // set all elems to 0
	bool   fSquare() const  { return nrows() == ncols(); }              // true if mat is square
	bool   fVector() const  { return nrows() == 1 || ncols() == 1;}     // true if mat is vec
	bool   fARowIsZero(double x = 0) const;     // true if a row is all equal to x
	bool   fAllZeroes(double Val = 0) const;    // true if matrix is all equal to Val
	void   clearSmallElems(double MinVal) const;// clear all elems that are absolute smaller than MinVal
	double det(int sign=1) const;               // determinant
	double lndet() const;                       // log of determinant
	StasmMat    inverse() const;                     // return inverse, bad results if mat singular
	StasmMat    invertMe() { assign(inverse()); return *this; }
						    // invert this, bad results if mat is singular

	StasmVec    mat33TimesVec2(const StasmVec &v2) const; // returns StasmVec(2)
	StasmVec    colSums() const;                     // vector of column sums

	bool   fIeeeNormal() const;                 // true if fIeeeNormal() true for all elements

	// print() prints the mat, all parameters are optional.
	//
	// If sMsg is specified it gets printed before the matrix.
	// nrows() and ncols() are passed as printf params to sMsg which you
	// can use if you want (use %d in sMsg).
	//
	// sFormat is a printf style format string for printing matrice elements e.g. "%g "
	// Default sFormat is the global define MAT_PRINT_FORMAT.
	//
	// If sFile and pFile both null      prints using lprintf
	// If pFile specified                writes to it (and uses sFile only for user msgs)
	// If sFile specified but pFile null then opens sFile and writes to it and then closes it
	//
	// nMax is max rows and/or cols to print, 0 for all.
	// If you specify nMax, will print min of nMax, nRows, nCols.
	//
	// If fPrintRowIndex is true then prefixes each row with the row index.

	void print(const char sMsg[]=NULL, const char sFormat[]=NULL,
		   const char sFile[]=NULL, FILE *pFile=NULL,
		   int nMax=0, bool fPrintRowIndex=false);

	void print(const char sMsg[]=NULL, const char sFormat[]=NULL, const char sFile[]=NULL,
		   FILE *pFile=NULL, int nMax=0, bool fPrintRowIndex=false) const;

	// For the file reading and writing routines, if pFile parameter is not null then
	// the file is assumed to be already open and pFile is used for appending (and
	// sFile is just for msgs to the user).

	void write(const char sFile[], FILE *pFile=NULL, // write to a file
			const char sFormat[] = NULL,
			const char sTag[]=NULL,
			bool fLimitOutputWidth=false) const;

	// sread() reads a mat and returns an error string if necessary.
	// If error, returns an error string, or fExitOnErr=true
	// then calls Er and never returns.
	//
	// sread() resizes the mat to match the mat in the file
	//
	// If the pFile parameter is not null then the file is assumed to be
	// already open, and sFile is just for msgs to the user.

	char *sread (const char sFile[],
			  FILE *pFile,
			  char sOptionalTag[],
			  const char sExpectedTag[]=NULL,
			  bool fExitOnErr=true);

	void read(const char sFile[],
		       FILE *pFile,
		       char sOptionalTag[]=NULL,
		       const char sExpectedTag[]=NULL);

	MatView view(size_t iStartRow, size_t iStartCol, size_t nRows, size_t nCols,
		     size_t Tda=0);

	const MatView view(size_t iStartRow, size_t iStartCol, size_t nRows, size_t nCols,
			size_t Tda=0) const;

	VecView       row       (size_t iRow);          // select a single row of mat
	const VecView row       (size_t iRow) const;
	VecView       rowAsCol  (size_t iRow);          // view a single column as a row
	const VecView rowAsCol  (size_t iRow) const;

	VecView       col       (size_t iCol);          // select a single column of mat
	const VecView col       (size_t iCol) const;

	VecView       viewAsRow ();                     // view entire mat as a row vector
	const VecView viewAsRow () const;
	VecView       viewAsCol ();                     // view entire mat as a col vector
	const VecView viewAsCol () const;

	VecView       viewDiagAsCol();                  // view diag of mat as a col vector
	const VecView viewDiagAsCol() const;

	MatView       viewAsSquare();                   // view matrix as a square matrix
	const MatView viewAsSquare() const;

	private:

	void errHandler(const char *reason, // called by the GSL if error
			const char *file, int line, int gsl_errno);

	StasmMat LU_decomp(gsl_permutation *pPerm=NULL, int *pSign=NULL) const;

	void printAux(const char sMsg[], const char sFormat[],  // helper for print()
		const char sFile[], FILE *pFile, size_t nMax, bool fPrintRowIndex) const;

	const StasmMat reshapeAux(const StasmMat &A, size_t nNewRows, size_t nNewCols,
		bool fRowMajor, size_t nStartRow, size_t nStartCol, size_t nRows, size_t nCols);

	};  // end class StasmMat
	}   // end namespace GslMat
// };

#endif // mat_hpp

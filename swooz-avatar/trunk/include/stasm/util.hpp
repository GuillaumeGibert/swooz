// $util.hpp 3.0 milbo$ utilities for the stasm package

#if !defined(util_hpp)
#define util_hpp

//-----------------------------------------------------------------------------
// DASSERT is only enabled if debugging, ASSERT is always enabled.
// Use DASSERT when the release code must be fast.

// namespace swStasm
// {


	#if _DEBUG
	#define DASSERT(x) (void)((x) || (Err("%s %d: FAIL %s", __FILE__, __LINE__, #x), 0) )
	#else
	#define DASSERT(x)
	#endif
	#define ASSERT(x) (void)((x) || (Err("%s %d: FAIL %s", __FILE__, __LINE__, #x), 0) )

	#ifndef M_PI
	#define M_PI 3.14159265358979323846264338328
	#endif

	#define SHAPE StasmMat       // By convention this specifies a Nx2 matrix holding a shape.
				// Coord 0,0 is the center of the image which makes it
				// easier to deal with various image sizes (but 0,0 also
				// means "unused landmark").

	#define vec_SHAPE vector<GslMat::StasmMat> // convention to indicate that this is a
				      // vector of Nx2 matrices holding shapes

	typedef vector<int>  vec_int;
	typedef vector<bool> vec_bool;

	typedef unsigned char byte;

	#define ABS(x)      (((x) < 0)? -(x): (x))
	#define SQ(x)       ((x) * (x))
	#ifndef MAX
	#define MAX(x,y)    (((x) > (y)) ? (x) : (y))
	#define MIN(x,y)    (((x) < (y)) ? (x) : (y))
	#endif
	#define IS_ODD(x)   (((x) & 1)==1)
	#define IS_EVEN(x)  (((x) & 1)==0)
	#define NELEMS(x)   int(sizeof(x) / sizeof((x)[0])) // nbr of elems in an array

	static const int INVALID = 9999;    // used to specify invalid eye locations, etc

	static const bool ENTER_DEBUGGER_ON_ERR = true;  // only applies if _DEBUG

	static const bool QUIET   = false;      // values for fVerbose parameter
	static const bool VERBOSE = true;

	static const bool NO_EXIT_ON_ERR = false;   // values for fExitOnErr
	static const bool EXIT_ON_ERR    = true;

	static const int SLEN = 256;            // generic string length

	static const int MAX_PRINT_LEN = 10000; // big enough for big printfs

	static const bool VERBOSE_ASM_SEARCH = true;

	#ifndef _MAX_PATH // definitions from Microsoft stdlib.h, need in non MS environment
	#define _MAX_PATH   260 /* max. length of full pathname */
	#define _MAX_DRIVE  3   /* max. length of drive component */
	#define _MAX_DIR    256 /* max. length of path component */
	#define _MAX_FNAME  256 /* max. length of file name component */
	#define _MAX_EXT    256 /* max. length of extension component */
	#endif

	//-----------------------------------------------------------------------------
	// abort() doesn't always work under MSC, hence the ENTER_DEBUGGER macro

	#define ENTER_DEBUGGER(s)   \
	{                           \
	    printf("\n%s\n", s);    \
	    fflush(stdout);         \
	    *(int *)0x99 = 0x99;    \
	}

	//-----------------------------------------------------------------------------
	// iround rounds towards zero i.e. it is symmetrical in that e.g. 1.5
	// becomes 1 and -1.5 becomes -1.  Note that these inline routines have to
	// be in the header file to be seen by microsoft linker.

	static inline int iround (double x)
	{
	if (x < 0)
	    return int(x - 0.5);
	else
	    return int(x + 0.5);
	}

	// fEqual () returns true if x == y within reasonable tolerance
	// If one or both of the numbers are NANs then the test fails (even if they
	// are equal NANs).
	//
	// I'm just doing a straight comparism because GSL gsl_fcmp() tests ratios and
	// is not useful for small numbers (because two small numbers can be close
	// in absolute value but far in ratio).
	//
	// But I added the _isnan tests because the microsoft v6 compiler says NANs
	// are equal to anything and also less than anything i.e NAN==x and NAN<x are true
	// for any double x.  This is a compiler bug I believe (because any comparison to
	// a NAN should give a NAN).

	static inline bool fEqual (double x, double y, double MaxDiff=1e-10)
	{
	#if _MSC_VER
	    return !_isnan(x) && !_isnan(y) && fabs(x-y) < MaxDiff;
	#else
	    return fabs(x-y) < MaxDiff;
	#endif
	}

	static inline double Degrees (double Radians)   // radians to degrees
	{
	    return 360 * Radians / (2 * M_PI);
	}

	// extern FILE *pgLogFile;     // lprintfs go to this file
	extern char sgBuf[];        // general purpose msg buffer
	// FILE *pOpenLogFile (const char sLogFile[], bool fVerbose=false);
	// void lprintf(const char *pArgs, ...);   // args like printf
	// void logprintf(const char *pArgs, ...); // args like printf
	FILE *Fopen(const char sFile[], const char sMode[], const char sOptionalMsg[] = NULL);
	// void Fwrite(const void *buf, size_t Size, size_t Count, FILE *pFile, char sFile[]);
	// void Fprintf(FILE *pFile, const char *pArgs, ...);
	// void Fprintf(const char sFile[], FILE *pFile, const char *pArgs, ...);
	void Fread(void *buf, size_t Size, size_t Count, FILE *pFile, const char sFile[]);
	char *Fgets(char s[], int n, FILE *stream, bool fTreatQuoteAsComment=false);
	char *sGetBase(const char sPath[]);
	char *sGetBaseExt(const char sPath[]);
	// void ShutdownStasm(void);
	char *sFormatTime(double Time);
	int nGetLineNbr(FILE *pFile);
	void GetDataDir(char sDataDir[], const char sExePath[]);
	void PrintCmdLine(FILE *pFile, int argc, const char *argv[]);
	void CheckFileIsReadable(const char sFile[]);
	char *sGetTempDir(void);
	void InitPacifyUser(int n);
	void PacifyUser(int i);

	void splitpath(const char* inpath, char * drv, char * dir, char* fname, char * ext );

	void makepath(char * path, const char * drive,
		      const char *directory, const char * filename,
		      const char * extension);

// };

#endif // util_hpp

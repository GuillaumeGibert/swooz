// $stasm-stasm.hpp 3.0 milbo$ (nearly) all include files for stasm

#if !defined(stasm_all_hpp)
#define stasm_all_hpp

static const char * const STASM_VERSION = "version 3.0";

#include <stdio.h>
#include <stdarg.h>
#include <vector>

using namespace std;


#define FA_BadImage   0x01       // image is "bad" in some way (blurred, face tilted, etc.)
#define FA_Glasses    0x02       // face is wearing specs
#define FA_Beard      0x04       // beard including possible mustache
#define FA_Mustache   0x08       // mustache but no beard occluding chin or cheeks
#define FA_Obscured   0x10       // faces is obscured e.g. by subject's hand
#define FA_EyesClosed 0x20       // eyes closed (partially open is not considered closed)
#define FA_Expression 0x40       // non-neutral expression on face
#define FA_NnFailed   0x80       // Rowley search failed (does not return 1 face with 2 eyes)
#define FA_Synthesize 0x100      // synthesize eye points from twin landmark
#define FA_VjFailed   0x200      // Viola Jones detector failed (no face found)
#define FA_ViolaJones 0x1000     // Viola Jones detector results
#define FA_Rowley     0x2000     // Rowley detector results


// #include "safe_alloc.hpp" // safe_alloc.hpp must be first to redefine malloc and alloc
#if _WIN32  // TODO why?
// #include "gsl/config.h"   // config.h must be first to build with inline defined etc.
#endif

#include "gsl_math.h"

#include "gsl_matrix_double.h"
#include "gsl_permutation.h"
#include "gsl_linalg.h"
#include "gsl_eigen.h"
#include "gsl_blas.h"

#include "err.hpp"
#include "mat.hpp"
#include "forward.hpp"
#include "initnet.hpp"
#include "util.hpp" 
#include "mchol.hpp"
#include "matview.hpp"
#include "matvec.hpp"
#include "shapefile.hpp"
#include "image.hpp"
#include "startshape.hpp" 
#include "imutil.hpp"
#include "sparsemat.hpp"


#include <exception>

/**
 * \class stasmComputeError
 * \brief Stasm computing exception.
 * \author Florian Lance
 * \date 28/02/12
 */	
class stasmComputeError : public std::exception
{
	public:
	virtual const char * what(void) const throw ()
	{
		return "Error during stasm computing. ";
	}
};


static const int MAX_NBR_LEVS     = 6;   // max levs in image pyr
static const int ASM_FILE_VERSION = 1;   // ASM file version

typedef struct SEARCH_IMAGES    // the images for one pyr level during a search
{
	Image  	 Img;     // the input image, scaled to current pyramid level
	StasmMat Grads;   // image gradients
}
SEARCH_IMAGES;

typedef struct ASM_LEVEL_DATA     // ASM data for one level: level 0 is full size
{
	int               nEigs;      // nEigs for LimitB()
	int               nEigsFinal; // nEigs for for the final search iteration
	double            BMax;       // for LimitB()
	double            BMaxFinal;
	int               nMaxSearchIters;
	int               nQualifyingDisp;
	GslMat::SHAPE     	  MeanShape;  	  // FileMeanShape, scaled for this level

	vector<GslMat::StasmMat>       Covars;     	  // vec [nPoints] of INVERSE of prof covar
						  // mat, each is mat nPoints x nPoints

	vector< vector<SPARSE_ELEM>> SparseCovars; // same as Covars, but sparse arrays

	vector<GslMat::StasmVec>       Profs;      	  // vec [nPoints] of landmark profile
						  // row vecs, each is 1 x nPoints

	vector<unsigned>  ProfSpecs;  	          // see PROF_ defs in prof.hpp
	GslMat::StasmVec               EigVals;        // n x 1 vector, EigVals scaled for this level
	
} ASM_LEVEL_DATA;

typedef struct ASM_MODEL  // an ASM model
{
	char   sFile[SLEN];
	int    iModel;            		 // used if models are stacked, 1st model is 0
	int    nPoints;           		 // number of landmarks
	int    nStartLev;         		 // start the search at this level in image pyramid
	double PyrRatio;
	int    PyrReduceMethod;   		 // one of IM_NEAREST_PIXEL IM_BILINEAR IM_AVERAGE_ALL
	int    nTrimCovar;        		 // covariance trim
	double SigmoidScale;      		 // specify how curvy profile equalization sigmoid is
	int    nStandardFaceWidth;		 // if >0 then prescale input face to this size
	int    nPixSearch;        		 // max dist from landmark when searching, in pixels
	int    nPixSearch2d;      		 // ditto but for for 2d profiles
	bool   fExplicitPrevNext;  		 // use iPrev and iNext in LandTab? see landmarks.hpp
	bool   fBilinearRescale;
	GslMat::SHAPE  FileMeanShape;      	 // mean shape read from .asm file
	GslMat::SHAPE  VjAv;
	GslMat::SHAPE  RowleyAv;
	GslMat::StasmMat    EigVecs;           	 // n x n matrix
	GslMat::StasmMat    EigInverse;        	 // EigVecs inverted
	ASM_LEVEL_DATA AsmLevs[MAX_NBR_LEVS];    // model for each pyramid level
	
} ASM_MODEL;

using namespace GslMat;

#include "shapemodel.hpp"
#include "landmarks.hpp"
#include "prof.hpp"
#include "readconf.hpp"
#include "readasm.hpp"
#include "asmsearch.hpp"
#include "initasm.hpp"
#include "rgbimutil.hpp"
#include "colors.hpp"
#include "imshape.hpp"

	
#endif // stasm_all_hpp

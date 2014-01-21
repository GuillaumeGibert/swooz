// $prof.hpp 3.0 milbo$ routines for ASM profiles
// Most of these defs are unused by "stasm".

#if !defined(prof_hpp)
#define prof_hpp

// namespace swStasm
// {

	extern double CONF_NormalizedProfLen;   // root mean square length of profiles

	#define IS_2D(ProfSpec) ((ProfSpec) & PROF_2d) // true if dimensional prof?

	//-----------------------------------------------------------------------------
    //   NOTESTASM: The stasm and tasm code support only one profile per landmark,
	//   unlike the research code.
	//
	//   Furthermore, in stasm and tasm, 1D profiles have to be
	//   PROF_Grad|PROF_Flat and 2D profiles have to be
	//   PROF_WindowEquallyWeighted|PROF_SigmAbsSum|PROF_GradBelowRight.
	//
	//   See the definitions of CONF_ProfType and CONF_ProfType2d0 in tasm.cpp.
	//   This limitation can be easily lifted by pulling in code from masm/prof.cpp
	//   into tasm/prof.cpp.  The definitions below should not need to be changed.

	//-----------------------------------------------------------------------------
	// Profile specifications.
	// We OR bits defined below to get a full specification of the profile type
	// in 32 bits.
	//
	// Layout is 2.ww.... Byte2 Byte1 Byte0
	//
	// where .      unused bits (always 0)
	//       2      is 2d/1d field
	//       ww     is window field (2d profs only)
	//
	// Byte2, Byte1, and Byte0 specify the profile type for up to 3 subprofile
	// types per landmark.
	// If there is only one subprofile type for a landmark then Byte2 and
	// Byte1 are 0.
	//
	// Bits in Byte2, Byte1, and Byte0 are
	//       nn     is normalization field e.g. PROF_Flat
	//       F      FBit filter bit:
	//                  0 means uses filter specified by TypeField tttt bits
	//                      see defs below
	//                      these are specialized filters optimized for speed
	//                      e.g. PROF_Grad
	//                  1 means use gImMasks array where ttt is an index into gImMasks
	//                      these are general purpose filters for experimentation
	//       tttt   TBits: meaning depends on F bit above e.g. PROF_MaskGray
	//
	// For classic Cootes 1-dimensional profiles use PROF_Grad|PROF_Flat
	//
	// For a given ProfSpec you can have either
	//      (i)  a single one-dimensional subprofile
	//   or (ii) 1 to 3 two-dimensional subprofiles.
	//
	// Mixing 1d and 2d profiles at the same landmark isn't allowed.
	// More than one 1d profile at a landmark isn't allowed (the code
	// doesn't support it even though the defines below allow it).

	#define PROF_1d                     0x00000000  // 1D profile along whisker
	#define PROF_2d                     0x80000000  // two dimensional profile

	#define PROF_WindowEquallyWeighted  0x00000000  // windows are for use with PROF_2d only
	#define PROF_WindowGaussian         0x10000000
	#define PROF_WindowCircle           0x20000000
	#define PROF_WindowField            0x30000000  // all of the above

	// defs for Byte2, Byte1, Byte0

	#define PROF_SigmLen                0x00    // use sigmoid equalization, variation 1
	#define PROF_Flat                   0x40    // normalize
	#define PROF_SigmAbsSum             0x80    // use sigmoid equalization
	#define PROF_NormalizationField     0xC0    // all of the above

	#define PROF_FBit                   0x10    // 2D: if 1, tttt bits are a PROF_Mask
						    //               i.e. an index into gImMask
						    //     if 0, tttt bits are defined below
	// defs if F bit is 0
	#define PROF_Grad                   0x1     // 1D: signed grad between this pixel and next
						    //          (not supported for 2D profiles)

	#define PROF_GradMagBelowRight      0x2     // Called "Mask021Mag" in my master's thesis
						    // 2D: unsigned grad between this pix and pixels
						    //      to right and below, always non-negative
						    // 1D: unsigned grad between this pixel and pixel
						    //      to right, always non-negative
	#define PROF_Edgeness               0x3     // 2D: edgeness as defined in ScottCootesTaylor
						    //      "Improving Appearance Model..."
	#define PROF_Cornerness             0x4     // 2D: cornerness, as defined in ditto (note: code is
						    //      quicker if SubProfs for edgeness and
						    //      cornerness are adjacent)

	#define PROF_GradBelowRight         0x5     // Called "Mask021" in my master's thesis
						    // Equivalent to PROF_Mask021 (but quicker)
						    // 2D: similar to PROF_GradMagBelowRight but
						    //      doesn't use root-mean-square

	#define PROF_HarrisStephens         0x6     // 2D: Harris Stephens detector
	#define PROF_TBits                  0x7     // 1D,2D: all of the above

	#define UNUSED_GRAD_VAL             0       // used for parts of gradient image
						    // where grad can't be calculated

	//-----------------------------------------------------------------------------
	// We use these defs if F bit is 1, the ttt bits are an index into gImMasks.
	// Note that PROF_Mask021 is the same as PROF_GradBelowRight.

	typedef double IM_MASK_33[3][3]; // 3 by 3 mask

	typedef enum eProfMasks
	    {
	    PROF_MaskGray,      // must match PROF_Masks below
	    PROF_Mask021,       // names based on the middle horizontal elements
	    PROF_Mask041,
	    PROF_Mask081,
	    PROF_Mask000,
	    PROF_Mask202,
	    PROF_Mask011,
	    PROF_Mask010
	    }
	eProfMasks;

	static const IM_MASK_33 gImMasks[] = // must match eImMasks above
	{
	    {{ 0,  0,  0},  // PROF_MaskGray: gray level
	     { 0,  1,  0},
	     { 0,  0,  0}},

	    {{ 0,  0,  0},  // PROF_Mask021: same as PROF_GradBelowRight
	     { 0, -2,  1},
	     { 0,  1,  0}},

	    {{ 0,  1,  0},  // PROF_Mask041: 4 connected Laplacian
	     { 1, -4,  1},
	     { 0,  1,  0}},

	    {{ 1,  1,  1},  // PROF_Mask081: 8 connected Laplacian
	     { 1, -8,  1},
	     { 1,  1,  1}},

	    {{ -1, -2, -1}, // PROF_Mask000: vertical Sobel
	      { 0,  0,  0},
	      { 1,  2,  1}},

	    {{ -1, 0, 1},   // PROF_Mask202: horizontal Sobel
	     { -2, 0, 2},
	     { -1, 0, 1}},

	    {{ 0,  0,  0},  // PROF_Mask011: horizontal part of PROF_Mask021
	     { 0, -1,  1},
	     { 0,  0,  0}},

	    {{ 0,  0,  0},  // PROF_Mask010: vertical part of PROF_Mask021
	     { 0, -1,  0},
	     { 0,  1,  0}}
	};

	//-----------------------------------------------------------------------------
	double GetX(double X, int iOffset, int iOrthOffset, double DeltaX, double DeltaY);
	double GetY(double Y, int iOffset, int iOrthOffset, double DeltaX, double DeltaY);

	void
	GetProfStepSize(double &DeltaX, double &DeltaY,     // out
			 const GslMat::SHAPE &Shape, int iPoint,    // in
			 const LANDMARK LandTab[],          // in
			 const bool fExplicitPrevNext);     // in

	int
	nGetProfWidth(int ncols, unsigned ProfSpec);   // in: all

	void
	InitGradsIfNeeded(GslMat::StasmMat &Grads,          // out
	    const vector<unsigned> &ProfSpecs, // in: prof specs read from .asm file
	    const Image &Img,                  // in: Img already scaled to this pyr level
	    int nPoints);                      // in:

	void
	PrepareProf1D(const Image &Img, const GslMat::StasmMat &Shape,               // in: all
		      unsigned SubProfSpec,
		      const LANDMARK LandTab[],
		      int iPoint, int nProfWidth,
		      int iOrthOffset, bool fExplicitPrevNext);

	void
	Get1dProf(GslMat::StasmVec &Prof,                                                    // out
	     unsigned SubProfSpec, const Image &Img, int iPoint, int iOffset);  // in

	void
	Get2dProf(GslMat::StasmVec &Prof,                                            // out
	    const unsigned ProfSpec,                                    // in
	    const GslMat::StasmMat   &Grads,                                         // in
	    const GslMat::SHAPE &Shape, const int iPoint, const int ixOffset,   // in
	    const int iyOffset, const int nProfWidth,                   // in
	    const double SigmoidScale);                                 // in

	void InitGrads(GslMat::StasmMat &Grads,                          // out
		   const Image &Img, unsigned ProfSpec);    // in
// };
#endif // prof_hpp

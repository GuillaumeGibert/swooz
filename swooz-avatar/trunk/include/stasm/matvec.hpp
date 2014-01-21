// $matvec.hpp 3.0 milbo$ routines for handling vectors of matrices

#if !defined(matvec_hpp)
#define matvec_hpp

	using namespace GslMat;
	using namespace std;

// namespace swStasm
// {

	typedef vector<GslMat::StasmMat>    vec_Mat;         // a vector of Mats
	typedef vector<string> vec_string;      // a vector of strings

	static const int MAT_NO_CHECK_SAME_DIM = 0;   // values for fCheckSameDim
	static const int MAT_CHECK_SAME_DIM    = 1;

	bool fMatchAttr(unsigned Attr, unsigned Mask0, unsigned Mask1);

	void ReadMatVec(vec_Mat &MatV,
		vec_string *pTags,
		const char sFile[], FILE *pFile,
		const char sTagRegex[] = NULL,
		unsigned Mask0 = 0, unsigned Mask1 = 0,
		int nWantedMats = 0);

	void AllocMatVec(vec_Mat &MatV,
			 size_t nMats, size_t nrows, size_t ncols, bool fClear=true);

	void WriteMatVec(const vec_Mat &MatV,
			 const char sFile[],
			 FILE *pFile,
			 const char sTag[]=NULL,
			 const char sFormat[]=NULL);

	GslMat::StasmMat FindMatInFile (const char sFile[],  // in
			   char *psImageDirs[], // out: directories in shape file if any, optional
			   const char sRegex[], // in: can be null
			   unsigned Mask0,      // in
			   unsigned Mask1);     // in
			   
// };

#endif // matvec_hpp

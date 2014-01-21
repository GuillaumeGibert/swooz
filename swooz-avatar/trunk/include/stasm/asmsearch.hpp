// $asmsearch.hpp 3.0 milbo$ search routines for active shape models

#if !defined(asmsearch_hpp)
#define asmsearch_hpp

#include "stasm.hpp"
#include "sparsemat.hpp"
#include "landmarks.hpp"

// namespace swStasm
// {
  void AlignStartShapeToDet (
	SHAPE &StartShape,            // out
	const DET_PARAMS &DetParams,  // in
	const SHAPE &DetAv,           // in
	double Scale);

  void AsmLevSearch (
	SHAPE &Shape,   	    // io
	SEARCH_IMAGES &SearchImgs,  // in
	const ASM_MODEL &Model,     // in
	int iLev,                   // in
	const LANDMARK LandTab[]   // in
	);
// };

#endif // asmsearch_hpp

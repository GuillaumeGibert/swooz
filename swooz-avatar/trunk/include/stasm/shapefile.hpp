// $shapefile.hpp 3.0 milbo$ routines for shape files

#if !defined(shapefile_hpp)
#define shapefile_hpp

// namespace swStasm
// {


	extern int CONF_nSeed_SelectShapes;

	void ReadSelectedShapes(vec_SHAPE &Shapes,    // out:
	    vec_string &Tags,                         // out:
	    char sImageDirs[],                        // out:
	    const char sShapeFile[],                  // in:
	    int nWantedShapes = 0,                    // in: nbr of wanted shapes, 0 for all
	    const char sTagRegex[] = "",              // in: only read matrices whose tag
						      //     matches this regular expression
	    unsigned Mask0 = 0, unsigned Mask1 = 0);  // in: only read matrices whose Attr matches these

	void ReadShapeFile(vec_Mat &Mats,   // out
	    vec_string &Tags,               // out: strings preceding each mat i.e. tags
	    char sImageDirs[],              // out: string following "Directories" in shape file
					    //      can be NULL
	    const char sTagRegex[],         // in: only read matrices whose tag
					    //     matches this regular expression
	    unsigned Mask0, unsigned Mask1, // in: only read matrices whose Attr matches these
	    const char sShapeFile[]);       // in:

	int iGetRefShapeIndex(const vec_string &Tags,
			      unsigned Mask0, unsigned Mask1,
			      const char sFile[]);  // in: for error reporting

	const char *sGetBasenameFromTag(const char *sTag);
	const char *sGetBasenameFromTag(const string &sTag);

	void DecomposeTag(const char sTag[],                        // in
			   unsigned *pAttr, char sImage[]=NULL);    // out, can be null

	void DecomposeTag(const string &sTag,                       // in
			  unsigned *pAttr, char sImage[]=NULL);     // out, can be null

	void
	DiscardFaceDetectorShapes(vec_SHAPE &Shapes,     // io: updated
				  vec_string &Tags,      // io: updated
				  bool fDiscardUnderscores=true);

	void
	DiscardShapesWithDifferentNbrOfPoints(vec_SHAPE &Shapes,    // io: updated
					      vec_string &Tags,     // io: updated
					      int nWantedPoints);
					      
// };


#endif // shapefile_hpp

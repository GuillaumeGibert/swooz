// $matvec.cpp 3.0 milbo$ routines for handling vectors of matrices
// This originally used C arrays and was changed to use STL vectors.

#include "stasm.hpp"

// using namespace swStasm;
//-----------------------------------------------------------------------------
// If fClear is false the contents of the matrices will be unspecified

void AllocMatVec (vec_Mat &MatV,
                  size_t nMats, size_t nrows, size_t ncols, bool fClear)
{
MatV.resize(0);         // get rid of old matrices if necessary
MatV.resize(nMats, StasmMat(nrows, ncols, fClear));      // resize to what we want
}

//-----------------------------------------------------------------------------
// Return true if CompiledRegExp matches the string

// static bool fRegexMatch (regex_t &CompiledRegex, const char s[])
// {
// regmatch_t pmatch[1];
// return 0 == regexec(&CompiledRegex, s, 0, pmatch, 0);
// }

//-----------------------------------------------------------------------------
// Returns true iff Attr matches Mask0 and Mask1

bool fMatchAttr (unsigned Attr, unsigned Mask0, unsigned Mask1)
{
if (Mask0 == 0 && Mask1 == 0)
    return true;

if (Mask0 != 0 && Mask1 == 0xffffffff)  // special case: match any bit in Mask0
    return (Attr & Mask0) != 0;

return (Attr & Mask0) == Mask1;
}

//-----------------------------------------------------------------------------
// Read matrices from a file, put them into pMatV.
//
// If pFile is NULL then it is opened.  If pFile is already open then we read
// from it (and sFile isn't used except for user messages).
//
// Matrix vector file format:
//
// # Optional comments prefixed by #
// Matrices one after each other, in standard format (see StasmMat::write function header)
//
// Actually comments can appear anywhere except within the { and } of a matrix.
// The starting character # must always be in the FIRST COLUMN.


void ReadMatVec (vec_Mat &MatV,      // out
    vec_string *pTags,               // out: tags before each mat, can be NULL
    const char sFile[], FILE *pFile, // in
    const char sTagRegex[],          // in: only read matrices whose tag strings match
                                     // this regular expression (default is null)
    unsigned Mask0, unsigned Mask1,  // in: only read matrices whose Attr matches Mask0 and Mask1
                                     // (Attr is hex part of tag string)
    int  nWantedMats)                // in: 0 means read all, n means read up to n matrices
{
// regcomp flags: egrep style expression (supports |),
// ignore case, use simple failure reporting in regexec

// regex_t CompiledRegex;
// if (sTagRegex && 0 != regcomp(&CompiledRegex, sTagRegex,
                              // REG_EXTENDED|REG_ICASE|REG_NOSUB))
    // {
    // Err("invalid regular expression %s", sTagRegex);
    // }
bool fCloseFile = false;
if (pFile == NULL)
    {
    ASSERT(sFile);
    pFile = Fopen(sFile, "r") ;
    fCloseFile = true;
    }
if (!sFile) // make sure we have something to print in user messages
    sFile = "file";

MatV.clear();       // erase what's currently in MatV
if (pTags)
    pTags->clear(); // erase what's currently in StringV

// read the actual matrices

int iMat = 0;   // index of matrices accepted so far
while ((nWantedMats == 0 || iMat < nWantedMats) && !fgErr)
    {
    char sTag[SLEN];
    StasmMat TempMat;
    bool fGetTag = (pTags || sTagRegex || Mask0 || Mask1);
    char *s = TempMat.sread(sFile, pFile, (fGetTag? sTag: NULL), NULL, NO_EXIT_ON_ERR);
    if (s)
        {
        // sread returned an error string
        // hack to ignore a legit error but report the rest
        // TODOSTASMS revisit, this is really ugly!
        if (0 == strncmp(s, "Can't read matrix header", strlen("Can't read matrix header")))
            {
            if (nWantedMats == 0)
                break;  // succesful completion
            else
                Err("could read only %d of %d matrices", iMat, nWantedMats);
            }
        else
            Err(s);
        }
    bool fKeep = true;
    if (sTagRegex)
        {
        // match matrix tag string against regular expression
        // to decide if we want to keep the matrix

        // if (!fRegexMatch(CompiledRegex, sTag))
            // fKeep = false;          // unsuccessful match
        }
    if (fKeep && (Mask0 || Mask1))
        {
        // match attribute in matrix tag string against Mask0 and
        // Mask1 to decide if we want to keep the matrix
        unsigned Attr;
        if (sTag[4] != ' ' && sTag[8] != ' ')
            Err("malformed tag %s "
                "in line %d of %s", sTag,  nGetLineNbr(pFile), sFile);
        if (1 != sscanf(sTag, "%x", &Attr))
            Err("can't convert first part of tag %s to a hex number "
                "in line %d of %s", sTag, nGetLineNbr(pFile), sFile);
        fKeep = fMatchAttr(Attr, Mask0, Mask1);
        }
    if (fKeep)
        {
        MatV.push_back(TempMat);
        if (pTags)
            pTags->push_back(sTag);
        iMat++;
        }
    }
if (fCloseFile)
    fclose(pFile);

// if (sTagRegex && sTagRegex[0])
    // regfree(&CompiledRegex);
}

//-----------------------------------------------------------------------------
// WriteMatVec writes a vector of matrices
//
// sTag is an optional tag printed before each matrix.
// It get printed followed by matrix nbr.
//
// If pFile is NULL then the file opened.  If pFile is already open then we
// append to it (and sFile isn't used except for user messages).

void WriteMatVec (const vec_Mat &MatV,  // in:
                  const char sFile[],   // in: see above comments
                  FILE *pFile,          // in: see above commentt
                  const char sTag[],    // in: optional
                  const char sFormat[]) // in: optional
{
bool fCloseFile = false;

ASSERT(!(pFile && !sFile)); // must give a filename for error reporting if pFile is given

if (pFile == NULL)
    {
    pFile = Fopen(sFile, "w") ;
    fCloseFile = true;
    }

for (vec_Mat::const_iterator iter = MatV.begin(); iter != MatV.end(); iter++)
    {
    if (sTag)
        {
        char s[SLEN];
        sprintf(s, "%s %d", sTag, iter-MatV.begin());
        // Fprintf(pFile, "\"%s\"\n", s);
        }
    iter->write(sFile, pFile, sFormat);
    }
if (fCloseFile)
    fclose(pFile);
}

//-----------------------------------------------------------------------------
// Return the matrix in file sFile that matches sRegex, Mask0, Mask1.
// If not found, returned StasmMat.nrows() will be 0.
//
// sTagRegex matches the _filename_ in the matrix tag (i.e. ignores hex prefix)
//
// The attributes of the matrix Attr must match Mask0 and Mask1 (see fMatchAttr).
// To ignore face detector shapes:
//      Mask0 = FA_ViolaJones|FA_Rowley
//      Mask1 = 0
//
// If sRegex matches more than one tag, then one of the matching
// shapes will be returned on each invocation of this routine, in
// the order the shapes are in the file.
//
// psImageDirs can be NULL if you don't need it

StasmMat FindMatInFile (const char sFile[],  // in
                   char *psImageDirs[], // out: directories in shape file if any, optional
                   const char sRegex[], // in: can be null
                   unsigned Mask0,      // in
                   unsigned Mask1)      // in
{
static char       sFile1[SLEN]; // the mat file name
static char       sImageDirs[SLEN];
static vec_Mat    MatV;         // the matrices read in from the file
static vec_string Tags;         // the tags (i.e. string before each mat in the file)
static vec_int    TagInts;      // hex number at start of each of above tags

// iMat is static so we start where we finished last time, which means that
// searches are fast if this function is invoked for matrices in order
// It also means with multiple matches to sRegex, succesive matching shapes
// are returned each time this function is called.

static unsigned iMat;

if (strcmp(sFile, sFile1))  // first time (for this file)?
    {
    // initialize the static variables

    strcpy(sFile1, sFile);
    iMat = 0;
    // read all shapes, we will filter the one we want later
    ReadShapeFile(MatV, Tags, sImageDirs, NULL, 0, 0, sFile);
    TagInts.resize(MatV.size());
    for (size_t i = 0; i < MatV.size(); i++)
        {
        char const *s = Tags[i].c_str();
        unsigned n;
        if (1 != sscanf(s, "%x", &n))
            Err("can't convert tag %s in %s to a hex number", s, sFile);
        TagInts[i] = n;
        }
    }
// regcomp flags: egrep style expression (supports |),
// ignore case, use simple failure reporting in regexec

// regex_t CompiledRegex;
// if (0 != regcomp(&CompiledRegex, sRegex, REG_EXTENDED|REG_ICASE|REG_NOSUB))
    // Err("invalid regular expression %s", sRegex);

size_t i;
for (i = 0; i < MatV.size(); i++)
    {
    if (fMatchAttr(TagInts[iMat], Mask0, Mask1))
	    // && // filter first on attributes
        // fRegexMatch(CompiledRegex, sGetBasenameFromTag(Tags[iMat])))
        {
        break;                  // found
        }
    iMat++;
    if (iMat >= MatV.size())
        iMat = 0;               // wrap
    }
// regfree(&CompiledRegex);

if (psImageDirs)
    *psImageDirs = sImageDirs;

StasmMat m;                          // returned matrix
if (i < MatV.size())            // found?
    m = MatV[iMat];

return m;
}

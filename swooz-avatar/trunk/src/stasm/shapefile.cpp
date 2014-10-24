// $shapefile.cpp 3.0 milbo$ routines for shape files
//-----------------------------------------------------------------------------
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// A copy of the GNU General Public License is available at
// http://www.r-project.org/Licenses/
//-----------------------------------------------------------------------------

#include "stasm.hpp"
// using namespace swStasm;

// The following is used only if nMaxShapes != 0.
//
// If CONF_nSeed_SelectShapes == 0 then we select the first nMaxShapes
//
// If CONF_nSeed_SelectShapes != 0 then we select a random selection of
// shapes as follows: randomly shuffle the shapes with the given random
// seed, and then select the first nMaxShapes from the shuffled shapes.

int CONF_nSeed_SelectShapes = 99;

//-----------------------------------------------------------------------------
// Note: the "magic number" of any file is the first few chars in the file
// that identify the file type

static void CheckShapeFileMagicNumber (FILE *pFile, const char sFile[])
{
char s[SLEN];
if (NULL == fgets(s, SLEN-1, pFile))
    Err("can't read %s", sFile);
if (s[0] != 's' || s[1] != 's')
    Err("%s is not a shape file (first two chars aren't \"ss\")", sFile);
}

//-----------------------------------------------------------------------------
// On return sImageDirs will be a string holding the image
// directories in sShapeFile, something like "/dir1;/dir2".
//
// On entry, we assume pShapeFile points to the string (or to comments
// preceding it which will be ignored). We also assume sImageDirs is big enough.

static void
GetImageDirsFromShapeFile (char sImageDirs[],       // out
                           const char sShapeFile[], // in
                           FILE *pShapeFile)        // in
{
char s[SLEN];
Fgets(s, SLEN-1, pShapeFile);   // this skips blank lines and comments, if any

const char *sWhiteSpace = " \t\n\r";
char *sToken = strtok(s, sWhiteSpace);
if (!sToken || 0 != strcmp(sToken, "Directories"))
    Err("expected \"Directories\" in line %d of %s",
        nGetLineNbr(pShapeFile), sShapeFile);

sToken = strtok(NULL, sWhiteSpace);
if (!sToken)
    Err("can't read image directories in line %d of %s",
        nGetLineNbr(pShapeFile), sShapeFile);

strcpy(sImageDirs, sToken);
}

//-----------------------------------------------------------------------------
// static void PrintMatchParams (const char sTagRegex[],           // in
                              // unsigned Mask0, unsigned Mask1)   // in
// {
// if ((sTagRegex && sTagRegex[0]) || Mask0 || Mask1)
    // lprintf("Matching ");
// if (sTagRegex && sTagRegex[0])
    // lprintf("\"%s\" ", sTagRegex);
// if (Mask0 || Mask1)
    // {
    // lprintf("Mask0 %x [%s]",  Mask0, sGetAtFaceString(Mask0));
    // lprintf(" Mask1 %x [%s]",  Mask1, sGetAtFaceString(Mask1, true));
    // }
// if ((sTagRegex && sTagRegex[0]) || Mask0 || Mask1)
    // lprintf("\n");
// }

//-----------------------------------------------------------------------------
void ReadShapeFile (
    vec_Mat &Mats,           // out
    vec_string &Tags,        // out: strings preceding each mat i.e. tags
    char sImageDirs[],       // out: string following "Directories" in shape file
                             //      can be NULL
    const char sTagRegex[],  // in: only read matrices whose tag
                             //     matches this regular expression
    unsigned Mask0,          // in:
    unsigned Mask1,          // in: only read matrices where
                             //     (Attr & Mask0) == Mask1
                             //     (Attr is hex part of tag string)
    const char sShapeFile[]) // in:
{
// clock_t StartTime = clock();
// PrintMatchParams(sTagRegex, Mask0, Mask1);  // print informational msg
// if (VERBOSE_ASM_SEARCH)
    // lprintf("Reading %s ", sShapeFile);
FILE *pShapeFile = Fopen(sShapeFile, "r");  // will issue Err if can't open
CheckShapeFileMagicNumber(pShapeFile, sShapeFile);
char sImageDirsDummy[SLEN];
if (!sImageDirs)
    sImageDirs = sImageDirsDummy;
GetImageDirsFromShapeFile(sImageDirs, sShapeFile, pShapeFile);
ReadMatVec(Mats, &Tags, sShapeFile, pShapeFile, sTagRegex, Mask0, Mask1);
fclose(pShapeFile);
// lprintf("%d mat%s [%.1f secs]\n",
        // Mats.size(), (Mats.size()==1? "": "s"), double(clock() - StartTime) / CLOCKS_PER_SEC);
}

//-----------------------------------------------------------------------------
const char *sGetBasenameFromTag (const char *sTag)
{
    if (sTag[4] == ' ')
        return sTag + 5;
    if (sTag[8] == ' ') // extended tags
        return sTag + 9;
    Err("malformed tag \"%s\" in sGetBasenameFromTag", sTag);
    return sTag;
}

const char *sGetBasenameFromTag (const string &sTag)
{
    return sGetBasenameFromTag(sTag.c_str());
}

//-----------------------------------------------------------------------------
void DecomposeTag (const char sTag[],               // in
                   unsigned *pAttr, char sImage[])  // out, can be null
{
// Tag string should be of the form: "1234 filename" or "12345678 filename".
// (Actually, the 8 character version of the tag is not used in the released
// version of stasm).

if (sTag[4] != ' ' && sTag[8] != ' ')
    Err("malformed tag \"%s\"", sTag);
unsigned Attr1 = unsigned(-1);
char sImage1[SLEN]; sImage1[0] = 0;
if (2 != sscanf(sTag, "%x %s", &Attr1, sImage1) || Attr1 == unsigned(-1) || !sImage1[0])
    Err("Cannot parse tag \"%s\"", sTag);
if (pAttr)
   *pAttr = Attr1;
if (sImage)
    strcpy(sImage, sImage1);
}

void DecomposeTag (const string &sTag,              // in
                   unsigned *pAttr, char sImage[])  // out, can be null
{
DecomposeTag(sTag.c_str(), pAttr, sImage);
}

//-----------------------------------------------------------------------------
// Remove shapes in pShapes whose attribute tag begins with "1" or "2".
// Also update Tags to match the new *pShapes.
// This will also discard shapes whose names begins with an
// underscore if fDiscardUnderscores true.

/*
void
DiscardFaceDetectorShapes (vec_SHAPE &Shapes,       // io: updated
                           vec_string &Tags,        // io: updated
                           bool fDiscardUnderscores)
{
unsigned iShape1 = 0;
unsigned nShapes = Tags.size();

ASSERT(Shapes.size() == nShapes);

for (unsigned iShape = 0; iShape < nShapes; iShape++)
    {
    const char *sTag = Tags[iShape].c_str();
    // tag string should be of the form: "1234 filename" or "12345678 filename"
    if (sTag[4] != ' ' && sTag[8] != ' ')
        Err("malformed tag \"%s\"", sTag);
    unsigned Attr;
    if (1 == sscanf(sTag, "%x", &Attr) &&
        !((Attr & FA_ViolaJones) || (Attr & FA_Rowley)) && // not a face detector shape
        (!fDiscardUnderscores || sGetBasenameFromTag(sTag)[0] != '_'))
        {
        Shapes[iShape1].assign(Shapes[iShape]);
        Tags[iShape1] = Tags[iShape];
        iShape1++;
        }
    }
if (nShapes - iShape1)
    lprintf("Ignored %d face detector%s shape%s\n",
            nShapes - iShape1,
            (fDiscardUnderscores? " and underscore": ""),
            (nShapes - iShape1 == 1? "": "s"));

Shapes.resize(iShape1);
Tags.resize(iShape1);
}
*/


//-----------------------------------------------------------------------------
// chuck out entries in Shapes that don't have nWantedPoints

void
DiscardShapesWithDifferentNbrOfPoints (vec_SHAPE &Shapes,    // io: updated
                                       vec_string &Tags,     // io: updated
                                       int nWantedPoints)
{
unsigned iShape1 = 0;
unsigned nShapes = static_cast<int>(Tags.size());

ASSERT(Shapes.size() == nShapes);

for (unsigned iShape = 0; iShape < nShapes; iShape++)
    if (Shapes[iShape].nrows() == (unsigned)nWantedPoints)
        {
        Shapes[iShape1].assign(Shapes[iShape]);
        Tags[iShape1] = Tags[iShape];
        iShape1++;
        }

// if (nShapes - iShape1)
    // lprintf("Ignored %d shape%s "
            // "not matching the reference number of points %d\n",
            // nShapes - iShape1,
            // (nShapes - iShape1 == 1? "": "s"),
            // nWantedPoints);

Shapes.resize(iShape1);
Tags.resize(iShape1);
}

//-----------------------------------------------------------------------------
// Return a sample of Shapes (without replacement) with sample size nWantedShapes
//
// If nSeed==0 then simply return the first nWantedShapes in Shapes
//
// If nSeed!=0 is true then return a random sample of Shapes, with nSeed as
// the seed for the rand number generator. Do it like this: randomly shuffle
// the shapes then use the first n shapes (where n is nWantedShapes).

/*
static void SelectNShapes (
            vec_SHAPE &Shapes,  // io
            vec_string &Tags,   // io: also shuffled, in step with Shapes
            int nWantedShapes,  // in: 0 means return all shapes
            int nSeed=0)        // in: 0 means no random selection; if any other
                                //     val select randomly with rand seed=nSeed
{
unsigned nShapes = Tags.size();
if (nWantedShapes == 0)
    nWantedShapes = nShapes;
nWantedShapes = MIN((unsigned)nWantedShapes, nShapes);
if (nSeed)
    {
    // generate a shuffled set of indices in iShuffledShapes

    vec_int iShuffledShapes(nShapes);
    unsigned iShape;
    for (iShape = 0; iShape < nShapes; iShape++)
        iShuffledShapes[iShape] = iShape;

    SeedRand(nSeed);

    // We use our own random shuffle here because different compilers
    // give different results which messes up regression testing.
    // (I think only Visual C 6.0 is incompatible with everyone else?)
    //
    // Following code is equivalent to
    //    random_shuffle(iShuffledShapes.begin(), iShuffledShapes.end(),
    //       pointer_to_unary_function<int,int>(Rand));

    vec_int::iterator pNext = iShuffledShapes.begin();
    for (int i = 2; ++pNext != iShuffledShapes.end(); ++i)
        iter_swap(pNext, iShuffledShapes.begin() + Rand(i));

    iShuffledShapes.resize(nWantedShapes);

    // sort the selected indices so we can do an in-place replacement in Shapes

    sort(iShuffledShapes.begin(), iShuffledShapes.end());

    // keep the first nWantedShapes in iShuffledShapes

    for (iShape = 0; iShape < unsigned(nWantedShapes); iShape++)
        {
        unsigned iOldShape = iShuffledShapes[iShape];
        if (iShape > 0 && Shapes[0].nrows() != Shapes[iOldShape].nrows())
            {
            static bool fIssuedWarning;
            if (!fIssuedWarning)
                {
                fIssuedWarning = true;
                WarnWithNewLine("different sized shapes (%s has %d rows, %s has %d rows)\n",
                     sGetBasenameFromTag(Tags[0].c_str()), Shapes[0].nrows(),
                     sGetBasenameFromTag(Tags[iOldShape].c_str()), Shapes[iOldShape].nrows());
                }
            }
        Shapes[iShape].assign(Shapes[iOldShape]);
        Tags[iShape] = Tags[iOldShape];
        }
    }
Shapes.resize(nWantedShapes);
Tags.resize(nWantedShapes);
}

*/

//-----------------------------------------------------------------------------
static void ShowFirstFewShapes (const vec_string &Tags) // in
{
if (Tags.size() == 0){}
    // lprintf("No shapes");
else
    {
    // lprintf("First few shapes are ");
    // unsigned iShape;
    // for (iShape = 0; iShape < MIN(5, Tags.size()); iShape++)
        // lprintf("%s ", sGetBase(sGetBasenameFromTag(Tags[iShape])));
    // if (iShape < Tags.size())
        // lprintf("...");
    // lprintf("\n");
    }
}

/*
//-----------------------------------------------------------------------------
void ReadSelectedShapes (
        vec_SHAPE &Shapes,              // out:
        vec_string &Tags,               // out:
        char sImageDirs[],              // out:
        const char sShapeFile[],        // in:
        int nMaxShapes,                 // in: nbr of wanted shapes, 0 for all
        const char sTagRegex[],         // in: only read matrices whose tag
                                        //     matches this regular expression
        unsigned Mask0, unsigned Mask1) // in: only read matrices where
                                        //     Attr&Mask0 == Mask1
                                        //     (Attr is hex part of tag string)
{
ReadShapeFile(Shapes, Tags, sImageDirs, sTagRegex, Mask0, Mask1, sShapeFile);

DiscardFaceDetectorShapes(Shapes, Tags);

unsigned nShapes = Tags.size();
if (nShapes == 0)
    Err("no shapes");
if (unsigned(nMaxShapes) > nShapes)
    {
    lprintf("\n");
    ShowFirstFewShapes(Tags);
    Err("want %d shapes but there are only %d shapes", nMaxShapes, nShapes);
    }
if (nMaxShapes != 0)
    {
    SelectNShapes(Shapes, Tags, nMaxShapes, CONF_nSeed_SelectShapes);
    if (CONF_nSeed_SelectShapes)
        lprintf("Selected a random sample of %d shape%s from %d shape%s (seed=%d)\n",
            nMaxShapes, ((nMaxShapes==1)? "":"s"),
            nShapes, ((nShapes==1)? "":"s"), CONF_nSeed_SelectShapes);
    else
        lprintf("Selected the first %d shape%s of %d shape%s\n",
            nMaxShapes, ((nMaxShapes==1)? "":"s"),
            nShapes, ((nShapes==1)? "":"s"));
    }
else
    lprintf("Read %d shape%s\n", nShapes, ((nShapes==1)? "":"s"));

ShowFirstFewShapes(Tags);
}
*/

//-----------------------------------------------------------------------------
static int iGetRefShapeIndex1 (const vec_string &Tags,
                               unsigned Mask0, unsigned Mask1,
                               const char sFile[])  // in: for error reporting
{
for (unsigned iShape = 0; iShape < Tags.size(); iShape++)
    {
    const char *sTag = Tags[iShape].c_str();
    if (!sTag || sTag[0] == 0)
        Err("GetRefShapeIndex: shape index %d in %s does not have a tag",
            iShape, sFile);
    unsigned Tag;
    if (sTag[4] != ' ' && sTag[8] != ' ')
        Err("GetRefShapeIndex: tag %s in %s is malformed", sTag, sFile);
    if (1 != sscanf(sTag, "%x", &Tag))
        Err("GetRefShapeIndex: tag %s in %s is not a hex number", sTag, sFile);
    if (fMatchAttr(Tag, Mask0, Mask1))
        return iShape;      // found it
    }
return -1;  // not found
}

//-----------------------------------------------------------------------------
// return the index of the first shape in Tags that has tag
// attributes which match Mask0 and Mask1

int iGetRefShapeIndex (const vec_string &Tags,          // in:
                       unsigned Mask0, unsigned Mask1,  // in:
                       const char sFile[])              // in: for error reporting
{
int iRefShape = iGetRefShapeIndex1(Tags, Mask0, Mask1, sFile);

if (iRefShape < 0)
    {
    // char s[SLEN]; strcpy(s, sGetAtFaceString(Mask0));

    // Err("no shapes in %s match Mask0 %x [%s] Mask1 %x [%s] ",
        // sFile, Mask0, s, Mask1, sGetAtFaceString(Mask1));
    }
return iRefShape;
}

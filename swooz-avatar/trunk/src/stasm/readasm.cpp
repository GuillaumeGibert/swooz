// $readasm.cpp 3.0 milbo$ routines to read .asm files
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

//-----------------------------------------------------------------------------
// Check first line of .asm file. It should be: ASM2 [filename]

static void
CheckAsmFileHeader (const char sAsmFile[], FILE *pAsmFile) // in
{
char s[SLEN], sFile[SLEN];
Fgets(s, SLEN-1, pAsmFile);

if (strncmp(s, "ASM", 3) != 0)          // check .asm file "magic number"
    Err("%s is not an ASM file (first three chars are not \"ASM\")", sAsmFile);

int nVersion;
if (sscanf(s+3, "%d %s", &nVersion, sFile) != 2)
    Err("%s has a bad header (could not read version number)", sFile);

if (nVersion != ASM_FILE_VERSION)
    Err("%s has a bad version number %d (expected %d)",
        sFile, nVersion, ASM_FILE_VERSION);
}

//-----------------------------------------------------------------------------
// read the configuration parameters embedded in the .asm file

static void ReadAsmProlog (ASM_MODEL  &Asm,         // out
                           int        &nLevs,       // out
                           const char sAsmFile[],   // in
                           FILE       *pAsmFile)    // in
{
// set to parameters to illegal values, so if any params are missing in
// the .asm file CheckConfTab will issue an error

Asm.nPoints = INVALID;
Asm.PyrRatio = INVALID;
Asm.PyrReduceMethod = INVALID;
Asm.fExplicitPrevNext = (bool)INVALID;
CONF_NormalizedProfLen = INVALID;
Asm.nStandardFaceWidth = INVALID;
Asm.fBilinearRescale = (bool)INVALID;
Asm.nTrimCovar = INVALID;
Asm.SigmoidScale = INVALID;

// The numbers like 1000 below are arbitrary and intended to catch gross errs
//
// Note: when changing this table, be very careful that the Type char
// matches the actual type of p.  Else fReadEntry will write into
// the wrong place in the stack, which is a hard problem to debug.

static CONF_ENTRY AsmFileConfTab[] =
{
// p   sName                   Min Max             Type
{NULL, "nLevs",                1,  MAX_NBR_LEVS,   'i'},
{NULL, "fXm2vts",              0,  1,              'f'},
{NULL, "nPoints",              3,  1000,           'i'},
{NULL, "PyramidRatio",         1,  1000,           'd'},
{NULL, "nPyramidReduceMethod", IM_NEAREST_PIXEL, IM_AVERAGE_ALL, 'i'},
{NULL, "fExplicitPrevNext",    0,  1,              'f'},
{NULL, "NormalizedProfLen",    1,  1000,           'd'},
{NULL, "nStandardFaceWidth",   0,  1000,           'i'},
{NULL, "fBilinearRescale",     0,  1,              'f'},
{NULL, "nTrimCovar",           0,  1000,           'i'},
{NULL, "SigmoidScale",         0,  1e5,            'd'},
};

// init non static part of table

AsmFileConfTab[0].p  = &nLevs;
AsmFileConfTab[1].p  = &CONF_fXm2vts;
AsmFileConfTab[2].p  = &Asm.nPoints;
AsmFileConfTab[3].p  = &Asm.PyrRatio;
AsmFileConfTab[4].p  = &Asm.PyrReduceMethod;
AsmFileConfTab[5].p  = &Asm.fExplicitPrevNext;
AsmFileConfTab[6].p  = &CONF_NormalizedProfLen;
AsmFileConfTab[7].p  = &Asm.nStandardFaceWidth;
AsmFileConfTab[8].p  = &Asm.fBilinearRescale;
AsmFileConfTab[9].p  = &Asm.nTrimCovar;
AsmFileConfTab[10].p = &Asm.SigmoidScale;

ReadConfTab(AsmFileConfTab, NELEMS(AsmFileConfTab), pAsmFile, sAsmFile);

if (Asm.fExplicitPrevNext && Asm.nPoints > ngElemsLandTab)
    Err("fExplicitPrevNext %d but nPoints %d is greater than ngElemsLandTab %d\n"
        "       Check %s",
        Asm.fExplicitPrevNext, Asm.nPoints, ngElemsLandTab, sAsmFile);
}

//-----------------------------------------------------------------------------
// Read and check header of a level within an .asm file
// No results are kept, this just checks that the header is good

static void
CheckLevelHeader (const char sAsmFile[], FILE *pAsmFile, int iLev) // in: all
{
// expect line to look like this: Lev 0

char s[SLEN], sLev[SLEN];
int nLev;

if (!Fgets(s, SLEN-1, pAsmFile))
    Err("can't read level %d header from line %d of %s",
        iLev, nGetLineNbr(pAsmFile), sAsmFile);

if (sscanf(s, "%s %d", sLev, &nLev) != 2)
    Err("bad format level header in line %d of %s",
        nGetLineNbr(pAsmFile), sAsmFile);

if (strcmp(sLev, "Lev") != 0)
    Err("bad format level header in line %d of %s",
        nGetLineNbr(pAsmFile), sAsmFile);

if (nLev != iLev)
    Err("illegal level %d in level header in line %d of %s, expected %d",
        nLev, nGetLineNbr(pAsmFile), sAsmFile, iLev);
}

//-----------------------------------------------------------------------------
// This duplicates the functionality of Mat.read but for vector<unsigned>.
// TODOSTASMS replace this function and ReadIntVec, duplicated code?

static void
ReadProfSpecs (vector<unsigned> &ProfSpecs,                 // out
      int nPoints, const char sAsmFile[], FILE *pAsmFile)   // in
{
char s[SLEN];

// header "{ 68 1"

if (!Fgets(s, SLEN-1, pAsmFile, true))
    Err("can't read ProfSpecs header from line %d of %s",
        nGetLineNbr(pAsmFile), sAsmFile);
char sBrace[SLEN];
int nRows = -1, nCols = -1;

if (sscanf(s, "%s %d %d", sBrace, &nRows, &nCols) != 3 ||
        sBrace[0] != '{' || sBrace[1] != 0)
    Err("bad format ProfSpecs header in line %d of %s",
        nGetLineNbr(pAsmFile), sAsmFile);

if (nRows != nPoints || nCols != 1)
    Err("bad number of rows or cols in ProfSpecs header in line %d of %s\n"
        "       Got %d %d expected %d 1",
        nGetLineNbr(pAsmFile), sAsmFile, nRows, nCols, nPoints);

// body, lines like "1234"

ProfSpecs.resize(nPoints);
for (int iPoint = 0; iPoint < nPoints; iPoint++)
    {
    if (!Fgets(s, SLEN-1, pAsmFile))
        Err("can't read ProfSpecs from line %d of %s",
            nGetLineNbr(pAsmFile), sAsmFile);
    unsigned ProfSpec = unsigned(-1);
    if (sscanf(s, "%x", &ProfSpec) != 1 || ProfSpec == unsigned(-1))
        Err("can't read ProfSpecs from from line %d of %s (expected a hex number)",
            nGetLineNbr(pAsmFile), sAsmFile);
    if (ProfSpec & 0xffff00)
        Err("profile %x in line %d of %s has more than 1 subprofile",
            ProfSpec, nGetLineNbr(pAsmFile), sAsmFile);
    ProfSpecs[iPoint] = ProfSpec;
    }

// footer "}"

if (!Fgets(s, SLEN-1, pAsmFile))
    Err("can't read ProfSpecs footer from from line %d of %s",
        nGetLineNbr(pAsmFile), sAsmFile);
if (s[0] != '}')
    Err("bad format ProfSpecs footer, expected \"}\", got %s"
        "       Check line %d of %s",
        s, nGetLineNbr(pAsmFile), sAsmFile);
}

//-----------------------------------------------------------------------------
static void
ReadAsmLev (ASM_LEVEL_DATA &AsmLev,                        // out
            int iLev,                               // in
            int nPoints,                            // in
            const char sAsmFile[], FILE *pAsmFile)  // in
{
CheckLevelHeader(sAsmFile, pAsmFile, iLev);

ReadProfSpecs(AsmLev.ProfSpecs, nPoints, sAsmFile, pAsmFile);

AsmLev.Covars.resize(nPoints);
AsmLev.SparseCovars.resize(nPoints);

int iPoint;
for (iPoint = 0; iPoint < nPoints && !fgErr; iPoint++)
    AsmLev.Covars[iPoint].read(sAsmFile, pAsmFile);

AsmLev.Profs.resize(nPoints);

for (iPoint = 0; iPoint < nPoints && !fgErr; iPoint++)
    AsmLev.Profs[iPoint].read(sAsmFile, pAsmFile);

// convert sparse covar matrices from Mat to SparseMat, where necessary

for (iPoint = 0; iPoint < nPoints && !fgErr; iPoint++)
    if (fSparseMat(AsmLev.Covars[iPoint]))
        {
        if (AsmLev.SparseCovars[iPoint].size() < 1)
            AsmLev.SparseCovars[iPoint].resize(1);

        CopyMatToSparseMat(AsmLev.SparseCovars[iPoint], AsmLev.Covars[iPoint]);

        // TODOSTASMS We set the dimensions here to 1,1 not 0,0 so calls to nrows()
        // later don't fail -- should be revisited for a cleaner interface?

        AsmLev.Covars[iPoint].dim(1, 1);  // mark Covars as unneeded
        }
}

//-----------------------------------------------------------------------------
// init an active shape model in Asm by reading an .asm file

void
ReadAsmFile (ASM_MODEL  &Asm,           // out
             int        &nLevs,         // out: initialized by ReadAsmProlog
             const char sAsmFile[])     // in
{
// if (VERBOSE_ASM_SEARCH)
    // lprintf("Reading %s\n", sAsmFile);
FILE *pAsmFile = Fopen(sAsmFile, "r");

// header

CheckAsmFileHeader(sAsmFile, pAsmFile);

// prolog

int nLevsTemp;
ReadAsmProlog(Asm, nLevsTemp, sAsmFile, pAsmFile); // init global CONF_ params
nLevs = nLevsTemp;
Asm.nStartLev = nLevs - 1;  // default start level is bottom of image pyramid

// eigen values and vectors

ASM_LEVEL_DATA *pLev = &Asm.AsmLevs[0];
pLev->EigVals.read(sAsmFile, pAsmFile, NULL, "EigVals");

if (pLev->EigVals(0) < 0.1)     // number is somewhat arbitrary
    Warn("eigen data invalid "
         "(not enough variation, max eigenvalue is %g)", pLev->EigVals(0));

Asm.EigVecs.read(sAsmFile, pAsmFile, NULL, "EigVecs");

// EigInverse is inverse of EigVecs (EigVecs is orthog, so transpose=inverse)
Asm.EigInverse = Asm.EigVecs.t();

// File mean shape (has tag AvShape).  To allow future expansion, ignore
// any matrices before AvShape, with a warning.

char sTag[SLEN];
while (1)
    {
    pLev->MeanShape.read(sAsmFile, pAsmFile, sTag);
    if (0 == strcmp(sTag, "AvShape"))
        break;      // found it
    Warn("ignored unexpected matrix %s in %s", sTag, sAsmFile);
    }
if (unsigned(Asm.nPoints) != pLev->MeanShape.nrows())
    Err("nPoints %d in %s does not match the number %d "
        "of rows in the mean shape\n",
        Asm.nPoints, sAsmFile, pLev->MeanShape.nrows());

ASSERT(2 * unsigned(Asm.nPoints) == Asm.EigVecs.nrows());

// start shapes

Asm.VjAv.read(sAsmFile, pAsmFile, NULL, "VjAv");
Asm.RowleyAv.read(sAsmFile, pAsmFile, NULL, "RowleyAv");

// level specific data

int iLev;
for (iLev = nLevs-1; iLev >= 0 && !fgErr; iLev--) // for each level in image pyramid
    ReadAsmLev(Asm.AsmLevs[iLev], iLev, Asm.nPoints, sAsmFile, pAsmFile);

fclose(pAsmFile);
}

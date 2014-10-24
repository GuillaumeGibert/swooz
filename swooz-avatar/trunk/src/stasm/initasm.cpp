// $initasm.cpp 3.0 milbo$ routines to read .asm files and associated .conf files
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
// The CONF_ values below are the defaults and can be changed by
// the stasm .conf file.  See InitAsm.

// Note on the CONF_sAsmFile entry in the .conf file:
// This is assumed to be a relative path name (relative to the
// dir of the .conf file) --- unless the entry is prefixed with
// a "/" or "./" indicating that the path name should be used as is.

static char CONF_sAsmFile[SLEN] = "";

bool CONF_fXm2vts = false;      // true to use (extended) XM2VTS table in landmarks.hpp
bool CONF_fMe17 = false;        // true to use me17 measure for fitness
static int    CONF_nEigs        = -1;  // nbr of shape model eigs to use
                                       //     0 for all, -1 for first third
static int    CONF_nEigsLev0    = 0;   // 1D profs: nEigs to use at level 0, 0 for CONF_nEigs
static int    CONF_nEigsLev0_2d = 0;   // ditto but for 2D profiles
static int    CONF_nEigsFinal   = 0;   // nEigs for final search iter at each
                                       //     level, 0 for CONF_nEigs
static double CONF_BMax         = 1.8; // limit b in shape model to this
static double CONF_BMaxLev0     = 0;   // 1D profs: BMax to use at level 0, 0 for CONF_BMax
static double CONF_BMaxLev0_2d  = 2.0; // ditto but for 2D profiles
static double CONF_BMaxFinal    = 3.0; // BMax to use for final iteration, 0 for CONF_BMax
static int    CONF_nPixSearch   = 3;   // how far along the whisker do we search, each side
static int    CONF_nPixSearch2d = 2;   // ditto, but for 2D profs
static int    CONF_nMaxSearchIters   = 4;
static int    CONF_nMaxSearchIters2d = 4;
static int    CONF_nQualifyingDisp    = 75;
static int    CONF_nQualifyingDisp2d  = 90;

static int CONF_n2ndModelStartLev = 1;  // level at which 2nd stacked model starts search

bool CONF_fStasmSkipIfNotInShapeFile = true; // only applies if -t flag is used

// The Min and Max limits in the table below are more-or-less arbitrary
//
// Note: when changing this table, be very careful that the Type char
// matches the actual type of p.  Else fReadEntry will write into
// the wrong place in the stack, which is a hard problem to debug.
int CONF_nSleep = 10; 

static CONF_ENTRY StasmConfTab[] =
{
// p                             sName                       Min Max Type
{&CONF_sAsmFile,                 "sAsmFile",                 0, 0,   's'},
{&CONF_fMe17,                    "fMe17",                    0,  1,  'f'},
{&CONF_nEigs,                    "nEigs",                    1, 100, 'i'},
{&CONF_nEigsLev0,                "nEigsLev0",                0, 100, 'i'},
{&CONF_nEigsLev0_2d,             "nEigsLev0_2d",             0, 100, 'i'},
{&CONF_nEigsFinal,               "nEigsFinal",               0, 100, 'i'},
{&CONF_BMax,                     "BMax",                    .5,   5, 'd'},
{&CONF_BMaxLev0,                 "BMaxLev0",                 0,   5, 'd'},
{&CONF_BMaxLev0_2d,              "BMaxLev0_2d",              0,   5, 'd'},
{&CONF_BMaxFinal,                "BMaxFinal",                0,   5, 'd'},
{&CONF_nPixSearch,               "nPixSearch",               1,  10, 'i'},
{&CONF_nPixSearch2d,             "nPixSearch2d",             1,  10, 'i'},
{&CONF_nMaxSearchIters,          "nMaxSearchIters",          1, 100, 'i'},
{&CONF_nMaxSearchIters2d,        "nMaxSearchIters2d",        1, 100, 'i'},
{&CONF_nQualifyingDisp,          "nQualifyingDisp",          1, 100, 'i'},
{&CONF_nQualifyingDisp2d,        "nQualifyingDisp2d",        1, 100, 'i'},
{&CONF_n2ndModelStartLev,        "n2ndModelStartLev",        0,  10, 'i'},
{&CONF_nSleep,                   "nSleep",                   0, 500, 'i'},
{&CONF_fStasmSkipIfNotInShapeFile,"fStasmSkipIfNotInShapeFile",0, 1, 'f'},
// params for start shape
{&CONF_nRowleyMethod,            "nRowleyMethod",            0,   1, 'i'},
{&CONF_nVjMethod,                "nVjMethod",                0,   1, 'i'},
{&CONF_fVjUseEvenIfEyeMissing,    "fVjUseEvenIfEyeMissing",    0, 1, 'f'},
{&CONF_fRowleyUseEvenIfEyeMissing,"fRowleyUseEvenIfEyeMissing",0, 1, 'f'},
{&CONF_VjScale,                  "VjScale",                0.1,  10, 'd'},
{&CONF_RowleyScale,              "RowleyScale",            0.1,  10, 'd'},
{&CONF_fVjSynthMissingEye,       "fVjSynthMissingEye",       0,   1, 'f'},
{&CONF_fRowleySynthMissingEye,   "fRowleySynthMissingEye",   0,   1, 'f'},
};

//-----------------------------------------------------------------------------
static void ExpandAsmFileName (char sExpandedFile[],    // out
                   const char sConfFile[],  // in
                   const char sAsmFile[])   // in: sAsmFile entry from .conf file
{
ASSERT(sConfFile[0]);
ASSERT(sAsmFile[0]);
if (sAsmFile[0] == '/' || sAsmFile[0] == '\\' ||
        (sAsmFile[0] == '.' && sAsmFile[1] == '/') ||
        (sAsmFile[0] == '.' && sAsmFile[1] == '\\'))
    {
    strcpy(sExpandedFile, sAsmFile);    // absolute path name, use as given
    }
else
    {
    // generate ASM filename relative to .conf directory

    char sConfDir[SLEN];
    char sDrive[_MAX_DRIVE], sDir[_MAX_DIR], sBase[_MAX_FNAME];
    splitpath(sConfFile, sDrive, sDir, sBase, NULL);
    makepath(sConfDir, sDrive, sDir, "", "");
    sprintf(sExpandedFile, "%s%s", sConfDir, sAsmFile);
    }
FILE *pFile = fopen(sExpandedFile, "r");    // check that we can open the file
if (!pFile)
    Err("can't open %s\n"
        "       Check sAsmFile entry in %s, and remember that unless the filename\n"
        "       is prefixed with / or ./ it is treated as relative to %s",
        sExpandedFile, sConfFile, sConfFile);
fclose(pFile);
}

//-----------------------------------------------------------------------------
// return true if there are any 2D profs in the given model level

static bool
fAny2dProfs (const ASM_LEVEL_DATA &Model, int nPoints)      // in: all
{
for (int iPoint = 0; iPoint < nPoints; iPoint++)
    if (IS_2D(Model.ProfSpecs[iPoint]))
        return true;

return false;
}

//-----------------------------------------------------------------------------
static void Assign_nEigs (int &nEigs,           // io: will set to nEigsAlt if 0
                        const int nEigsNew,     // in: value to assign to nEigs
                        const int nEigsAlt,     // in: use if nEigsNew is 0
                        const int nEigsTotal,   // in: for error checking only
                        const char sMsg[],      // in: for error reporting only
                        const int iLev,         // in: for error reporting only
                        const char sConfFile[]) // in: for error reporting only
{
nEigs = nEigsNew;
if (nEigs == 0)
    nEigs = nEigsAlt;
if (nEigs < 1)
   Err("level %d %s is %d.\n"
        "       Check definitions of nEigs and friends in %s",
        iLev, sMsg, nEigs, sConfFile);
if (nEigs > nEigsTotal)
   Err("level %d %s %d is more than the number %d of eigs in the model.\n"
        "       Check definitions of nEigs and friends in %s",
        iLev, sMsg, nEigs, nEigsTotal, sConfFile);
}

//-----------------------------------------------------------------------------
static void
Init_nEigs_bMax (ASM_MODEL &Asm,            // io
                 int iLev, int f2dProfs,    // in
                 const char sConfFile[])    // in: for error reporting only
{
const int nEigsTotal = static_cast<int>(Asm.EigVecs.nrows());
if (iLev)
    {
    Assign_nEigs(Asm.AsmLevs[iLev].nEigs,
        CONF_nEigs, nEigsTotal, nEigsTotal, "nEigs", iLev, sConfFile);

    Asm.AsmLevs[iLev].BMax = CONF_BMax;
    }
else    // level 0 in the image pyramid
    {
    Assign_nEigs(Asm.AsmLevs[iLev].nEigs,
        (f2dProfs? CONF_nEigsLev0_2d: CONF_nEigsLev0),
        CONF_nEigs, nEigsTotal, "nEigsLev0", iLev, sConfFile);

    if (f2dProfs)
        Asm.AsmLevs[0].BMax = (CONF_nEigsLev0_2d? CONF_BMaxLev0_2d: CONF_BMax);
    else
        Asm.AsmLevs[0].BMax = (CONF_nEigsLev0? CONF_BMaxLev0: CONF_BMax);
    if (Asm.AsmLevs[0].BMax == 0)
        Asm.AsmLevs[0].BMax = CONF_BMax;
    }
Assign_nEigs(Asm.AsmLevs[iLev].nEigsFinal,
    CONF_nEigsFinal, Asm.AsmLevs[iLev].nEigs, nEigsTotal,
    "nEigsFinal", iLev, sConfFile);

Asm.AsmLevs[iLev].BMaxFinal = (CONF_BMaxFinal? CONF_BMaxFinal: CONF_BMax);

// logprintf("iLev %d nEigs %d nEigsFinal %d BMax %-3g BMaxFinal %g\n",
    // iLev, Asm.AsmLevs[iLev].nEigs, Asm.AsmLevs[iLev].nEigsFinal,
    // Asm.AsmLevs[iLev].BMax, Asm.AsmLevs[iLev].BMaxFinal);
}

//-----------------------------------------------------------------------------
static void InitAsm (ASM_MODEL &Asm,                // out
                     int &nLevs,                    // out
                     char sAsmFile[],               // out
                     const CONF_ENTRY ConfTab[],    // in
                     const int nConfTab,            // in
                     const char sConfFile[])        // in
{
ReadConfFile(ConfTab, nConfTab, sConfFile, "STASM");
if (!CONF_sAsmFile[0])
    Err("no sAsmFile entry in %s", sConfFile);
ExpandAsmFileName(sAsmFile, sConfFile, CONF_sAsmFile);  // init sAsmFile
ReadAsmFile(Asm, nLevs, sAsmFile);
if (CONF_fXm2vts)
    InitLandTab(Asm.nPoints);

if (CONF_nEigs == -1)                       // uses default nEigs?
    CONF_nEigs = static_cast<int>(Asm.EigVecs.ncols()) / 3;   // just a crude approx to optimimum val
if (CONF_nEigs < 1)
    CONF_nEigs = 1;

Asm.FileMeanShape = Asm.AsmLevs[0].MeanShape;

if (CONF_fMe17 && !CONF_fXm2vts)
    Err("fMe17 is true yet fXm2vts is false, check %s and %s", sAsmFile, sConfFile);

bool fForcedRowley = false;
if (CONF_nRowleyMethod == 1 && (Asm.RowleyAv.nelems() == 0 || !fPointUsed(Asm.RowleyAv, 0)))
    {
    Warn("Forcing nRowleyMethod to 0 because RowleyAv (in %s) is empty\n"
         "         To get rid of this warning, set nRowleyMethod to 0 in %s,\n"
         "         or init RowleyAv in %s with valid data",
         sConfFile, sConfFile, sAsmFile);
    CONF_nRowleyMethod = 0;
    fForcedRowley = true;
    }
if (CONF_nRowleyMethod == 0)
    {
    if (!CONF_fXm2vts)
        Err("nRowleyMethod is 0 yet fXm2vts is false, check %s", sConfFile);
    if (Asm.FileMeanShape.nelems() == 0 || !fPointUsed(Asm.FileMeanShape, 0))
        Err("AvShape in %s is empty", sAsmFile);
    }
if (Asm.VjAv.nelems() == 0 || !fPointUsed(Asm.VjAv, 0))
    Err("VjAv in %s is empty", sAsmFile);

// initializations from global CONF_'s

Asm.nPixSearch = CONF_nPixSearch;
Asm.nPixSearch2d = CONF_nPixSearch2d;

// logprintf("Adjusted params:\n", sAsmFile);
for (int iLev = 0; iLev < nLevs; iLev++)    // for each level in image pyramid
    {
    ASM_LEVEL_DATA *pAsmLev = &Asm.AsmLevs[iLev];
    const bool fTwoD = fAny2dProfs(*pAsmLev, Asm.nPoints);
    Init_nEigs_bMax(Asm, iLev, fTwoD, sConfFile);
    if (fTwoD)
        {
        pAsmLev->nMaxSearchIters = CONF_nMaxSearchIters2d;
        pAsmLev->nQualifyingDisp = CONF_nQualifyingDisp2d;
        }
    else
        {
        pAsmLev->nMaxSearchIters = CONF_nMaxSearchIters;
        pAsmLev->nQualifyingDisp = CONF_nQualifyingDisp;
        }
    pAsmLev->EigVals = Asm.AsmLevs[0].EigVals /
                            pow(Asm.PyrRatio * Asm.PyrRatio, iLev);

    // In theory we shouldn't have to scale MeanShape here because
    // ConformShapeToModel takes care of scaling. In practice we get better
    // results when we do scale here.  TODOSTASMS Look into this.

    pAsmLev->MeanShape = Asm.AsmLevs[0].MeanShape / (pow(Asm.PyrRatio, iLev));  // GetPyrScale(iLev, Asm.PyrRatio); 
                            

    // ReleaseProcessor();     // give others a chance
    }
}

//-----------------------------------------------------------------------------
static void CheckInt (const char sName[],
                        const int x0, const int x1,
                        const char sAsm0[], const char sAsm1[])
{
if (x0 != x1)
    Err("%s %d in %s does not match %s %d in %s",
        sName, x0, sAsm0, sName, x1, sAsm1);
}

//-----------------------------------------------------------------------------
static void CheckDouble (const char sName[],
                            const double x0, const double x1,
                            const char sAsm0[], const char sAsm1[])
{
if (x0 != x1)
    Err("%s %g in %s does not match %s %g in %s",
        sName, x0, sAsm0, sName, x1, sAsm1);
}

//-----------------------------------------------------------------------------
// Checks that params for Model1 are consistent with Model0.
// Doesn't check everything.

static void CheckModelConsistency (
         const ASM_MODEL  &Mod0,
         const ASM_MODEL  &Mod1,
         const int        nLevs0,
         const int        nLevs1,
         const char       sAsm0[],
         const char       sAsm1[])
{
CheckInt("nLevs", nLevs0, nLevs1, sAsm0, sAsm1);

CheckDouble("PyrRatio", Mod0.PyrRatio, Mod1.PyrRatio, sAsm0, sAsm1);

CheckInt("PyrReduceMethod", Mod0.PyrReduceMethod, Mod1.PyrReduceMethod,
         sAsm0, sAsm1);

// TODOSTASMS is this check necessary?
CheckInt("nStandardFaceWidth", Mod0.nStandardFaceWidth, Mod1.nStandardFaceWidth,
         sAsm0, sAsm1);

if (Mod1.nStartLev >= nLevs1)
    Err("nStartLevel %d in %s is greater than the number of levels %d",
        Mod1.nStartLev, sAsm1);
}

//-----------------------------------------------------------------------------
// Init Models from the .asm and .conf files and global defines.
// Returns the number of models, 1 or 2.

int                                      // returns the number of models
nInitAsmModels (ASM_MODEL  Models[],     // out: two ASM models
                const char sConfFile0[], // in: 1st config filename
                const char sConfFile1[]) // in: 2nd config filename, "" if none
{
static int nModels = 0;
if (nModels == 0)                       // Models not yet initialized?
    {
    ASSERT(sConfFile0 != NULL);
    ASSERT(sConfFile0[0]);
    int nLevs0;
    char sAsmFile0[SLEN];
    // if (VERBOSE_ASM_SEARCH)
        // lprintf("\n");
    InitAsm(Models[0], nLevs0, sAsmFile0,
            StasmConfTab, NELEMS(StasmConfTab), sConfFile0);
    nModels++;
    if (sConfFile1 && sConfFile1[0])    // stacked models?
        {
        int nLevs1;
        char sAsmFile1[SLEN];
        InitAsm(Models[1], nLevs1, sAsmFile1,
                StasmConfTab, NELEMS(StasmConfTab), sConfFile1);
        CheckModelConsistency(Models[0], Models[1],
                              nLevs0, nLevs1, sAsmFile0, sAsmFile1);
        Models[1].nStartLev = CONF_n2ndModelStartLev;
        nModels++;
        }
    }
return nModels;
}

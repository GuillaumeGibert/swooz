// $readconf.cpp 3.0 milbo$ routines to read .conf files
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

static const int CONF_FILE_VERSION = 1;

//-----------------------------------------------------------------------------
// check first line of .conf file

static void
CheckConfFileHeader (const char sMagic[], const char sFile[], FILE *pFile) // in
{
char s[SLEN];
Fgets(s, SLEN-1, pFile);

int n = strlen(sMagic);
if (strncmp(s, sMagic, n) != 0) // check CONF file magic number
    {
    // truncate s at the first white space or after 20 chars
    int i;
    for (i = 0; i < n+1 && i < 20; i++)
        if (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r')
            break;
    s[i] = 0;
    Err("%s is not a %s config file\n"
        "       First part of %s is \"%s\", expected \"%s%d\"",
        sFile, sMagic, sFile, s, sMagic, CONF_FILE_VERSION);
    }
int nVersion;
if (sscanf(s+n, "%d", &nVersion) != 1)
    Err("%s has a bad header (could not read version number)", sFile);

if (nVersion != CONF_FILE_VERSION)
    Err("%s has a bad version number %d (expected %d)",
        sFile, nVersion, CONF_FILE_VERSION);
}

//-----------------------------------------------------------------------------
// Read a configuration entry, expected format is: STRING VALUE
// Return true if got the entry (the result will be in the global
// CONF_ variable referenced in ConfTab).
// Return false if reached the end of the table in the file.

static bool
fReadEntry (const CONF_ENTRY ConfTab[], const int nConfTab,
            const char sFile[], FILE *pFile)
{
char s[SLEN];

if (!Fgets(s, SLEN-1, pFile))
    Err("premature end of %s (expected a \"}\" before the end)", sFile);
if (strchr(s, '}'))
    return false;       // reached end of table

// logprintf("%s", s);     // echo to log file

// read sName and sTemp from file

char sName[SLEN], sTemp[SLEN];
if (2 != sscanf(s, "%s %s", sName, sTemp))
    Err("bad config format in line %d of %s: %s",
        nGetLineNbr(pFile), sFile, s);

// look for keyword (stored in sName) in ConfTab

int i;
for (i = 0; i < nConfTab; i++)
    if (0 == strcmp(ConfTab[i].sName, sName))
        break;  // found entry in ConfTab
if (i == nConfTab)
    {
    // TODOSTASMS we use lprintf here instead of Warn to get the line spacing right
    // lprintf("Warning: unrecognized entry in %s: %s", sFile, s);
    }
else
    {
    float Temp = FLT_MAX;
    const CONF_ENTRY *pEntry = ConfTab + i;
    ASSERT(pEntry->p);
    if (pEntry->Type == 's')        // string in quotes
        {
        char *p = s;
        while (*p && *p != ' ' && *p != '\t')
            p++;                    // skip over sName
        while (*p && (*p == ' ' || *p == '\t'))
            p++;                    // skip to start of string
        if (*p != '"')
            Err("bad config format in line %d of %s: %s"
                "       String must be enclosed in \"quotes\"",
                nGetLineNbr(pFile), sFile, s);
        char *p1 = ++p;
        while (*p1 && *p1 != '"')
            p1++;                   // skip to end of string
        if (*p1 != '"')
            Err("bad config format in line %d of %s: %s"
                "       String must be enclosed in \"quotes\"",
                nGetLineNbr(pFile), sFile, s);
        *p1 = 0;                    // zap trailing quote
        strcpy((char *)(pEntry->p), p);
        }
    else if (pEntry->Type == 'x')   // hex
        {
        const char *sErr = "Bad config entry in line %d of %s: %s"
                           "       Expected 0x1234 "
                           "where 1234 is one or more hex digits (must have 0x prefix)";
        unsigned Hex;
        if (sTemp[0] != '0' && sTemp[1] != 'x') // check "0x" prefix
            Err(sErr, nGetLineNbr(pFile), sFile, s);
        if (1 != sscanf(sTemp+2, "%x", &Hex))   // +2 to skip 0x prefix
            Err(sErr, nGetLineNbr(pFile), sFile, s);
        *(int *)(pEntry->p) = int(Hex);
        }
    else    // manipulate non hex types as doubles
        {
        if (1 != sscanf(sTemp, "%g", &Temp))
            Err("bad config entry in line %d of %s: %s",
                nGetLineNbr(pFile), sFile, s);
        if (Temp == FLT_MAX)
            Err("can't read config value in line %d of %s: %s",
                nGetLineNbr(pFile), sFile, s);
        if (Temp < pEntry->Min || Temp > pEntry->Max)
            Err("out of range value config %g for %s in line %d of %s\n"
                "       Allowed range for %s is %g to %g",
                Temp, sName, nGetLineNbr(pFile), sFile,
                sName, pEntry->Min, pEntry->Max);
        }
    switch (pEntry->Type)
        {
        case 'd':   // double
            *(double *)(pEntry->p) = Temp;
            break;
        case 'i':   // int
            if (floor(Temp) != Temp)
                Err("non integer config value %g for %s in line %d of %s",
                    Temp, sName, nGetLineNbr(pFile), sFile);
            *(int *)(pEntry->p) = int(Temp);
            break;
        case 'f':   // bool (f for flag)
            if (Temp != 0 && Temp != 1)
                Err("non boolean config value %g for %s line %d of %s",
                    Temp, sName, nGetLineNbr(pFile), sFile);
            *(bool *)(pEntry->p) = (Temp == 1);
            break;
        case 'x':   // hex
        case 's':   // string
            break;
        default:
            ASSERT(0);
        }
    }
return true;
}

//-----------------------------------------------------------------------------
static void CheckEntry (const double Temp,  // in
                const CONF_ENTRY *pEntry,   // in
                const char sFile[])         // in
{
if (Temp < pEntry->Min || Temp > pEntry->Max)
    Err("bad value %g for %s\n"
        "       The entry for %s in %s is possibly missing",
        Temp, pEntry->sName, pEntry->sName, sFile);
}

//-----------------------------------------------------------------------------
// Make sure all values in ConfTab are valid i.e. between Min and Max

static void CheckConfTab (const CONF_ENTRY ConfTab[], const int nConfTab,
                          const char sFile[])
{
for (int i = 0; i < nConfTab; i++)  // for each entry in ConfTab
    {
    const CONF_ENTRY *pEntry = ConfTab + i;
    switch (pEntry->Type)
        {
        case 'd':   // double
            CheckEntry(*(double *)(pEntry->p), pEntry, sFile);
            break;
        case 'i':   // int
            CheckEntry(*(int *)(pEntry->p), pEntry, sFile);
            break;
        case 'f':   // bool (f for flag)
            CheckEntry(*(bool *)(pEntry->p) == 1, pEntry, sFile);
            break;
        case 'x':   // hex
        case 's':   // string
            // no check on these
            break;
        default:
            ASSERT(0);
        }
    }
}

//-----------------------------------------------------------------------------
// Read all entries between "{" and "}"
// The results of this routine go into the global CONF_ variables referenced
// in the config table.

void
ReadConfTab (const CONF_ENTRY ConfTab[], // in: all
             const int nConfTab,
             FILE *pFile,
             const char sFile[])         // in: filename, for err reporting only
{
// read opening brace

char s[SLEN];
Fgets(s, SLEN-1, pFile);    // Fgets skips comments and blank lines
if (s[0] != '{')
    Err("expected a \"{\" but did not find one in line %d of %s",
        nGetLineNbr(pFile), sFile);

// read all entries and the closing brace

// logprintf("{\n");
while (fReadEntry(ConfTab, nConfTab, sFile, pFile))
    ;
// logprintf("}\n");

CheckConfTab(ConfTab, nConfTab, sFile);
}

//-----------------------------------------------------------------------------
// The results of this routine go into the global CONF_ variables referenced
// in the config file.

void
ReadConfFile (const CONF_ENTRY ConfTab[],   // in
              const int nConfTab,           // in
              const char sFile[],           // in
              const char sMagic[])          // in
{
// if (VERBOSE_ASM_SEARCH)
    // lprintf("Reading %s\n", sFile);
FILE *pFile = Fopen(sFile, "r");
CheckConfFileHeader(sMagic, sFile, pFile);
ReadConfTab(ConfTab, nConfTab, pFile, sFile);
fclose(pFile);
}

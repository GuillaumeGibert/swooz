// $util.cpp 3.1 milbo$ utilities for stasm package
//
// If you are linking the stasm code into an existing application you
// will probably have to tweak some routines in this file.
//
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
#include "sys/stat.h"
// using namespace swStasm;

FILE *pgLogFile = NULL;                 // log file: lprintfs go here

char sgBuf[MAX_PRINT_LEN];              // general purpose msg buffer

static char sgPrint[MAX_PRINT_LEN];     // buffer for routines in this file
                                        // so print routines don't use sgBuf

//-----------------------------------------------------------------------------
// ShutdownStasm is called when stasm finishes (either correctly or with an error)
/*
void ShutdownStasm(void)
{
if (pgLogFile)
    fclose(pgLogFile);
FreeNetworks();
// CloseViolaJones();
}

//-----------------------------------------------------------------------------
// Opens the log file. After this when you call lprintf, you print on the
// screen and the log file.

FILE *pOpenLogFile (const char sLogFile[], bool fVerbose)
{
if (fVerbose)
    printf("Writing %s\n", sLogFile);
pgLogFile = fopen(sLogFile, "w");
if (!pgLogFile)
    {
    printf("\nCan't open \"%s\"\n", sLogFile);
    exit(1);    // we don't call Err(), to avoid possible recursive Err
    }
return pgLogFile;
}

//-----------------------------------------------------------------------------
// like printf but prints to the log file as well, it is is open

void lprintf (const char *pArgs, ...)   // args like printf
{
va_list pArg;
va_start(pArg, pArgs);
vsprintf(sgPrint, pArgs, pArg);
va_end(pArg);
printf("%s", sgPrint);
fflush(stdout);     // flush so if there is a crash we can see what happened
if (pgLogFile)
    {
    fputs(sgPrint, pgLogFile);
    fflush(pgLogFile);
    }
}

//-----------------------------------------------------------------------------
// Like printf but prints to the log file only (and not to screen)
// Used for detailed stuff that we may want to know but we don't want to
// usually bother the user with

void logprintf (const char *pArgs, ...) // args like printf
{
if (pgLogFile)
    {
    va_list pArg;
    va_start(pArg, pArgs);
    vsprintf(sgPrint, pArgs, pArg);
    va_end(pArg);
    fputs(sgPrint, pgLogFile);
    fflush(pgLogFile);
    }
}

*/
//-----------------------------------------------------------------------------
// like fopen but prints a message and exits if the file open fails

FILE *Fopen (const char sFile[], const char sMode[], const char sOptionalMsg[])
{
FILE *pFile = fopen(sFile, sMode);
if (!pFile)
    {
    const char *s;
    switch (sMode[0])
        {
        case 'w':
            s = " for writing";
            break;
        case 'a':
            s = " for appending";
            break;
        case 'r':
            s = "";
            break;
        default:
            s = " (unknown mode)";
            break;
        }
    int nErr = errno; // save before print below clears it
    // lprintf("\nfopen: %s\n", strerror(nErr));

    if (sOptionalMsg && sOptionalMsg[0])
        Err("can't open %s%s (%s)", sFile, s, sOptionalMsg);
    else
        Err("can't open %s%s", sFile, s);
    }
return pFile;
}

//-----------------------------------------------------------------------------
// like fwrite but prints a message and exits if write fails
/*
void Fwrite (const void *buf, size_t Size, size_t Count, FILE *pFile, char sFile[])
{
if (fwrite(buf, Size, Count, pFile) != Count)
    Err("can't write %s", sFile);
}

//-----------------------------------------------------------------------------
// like fprintf but prints a message and exits if fprintf fails

void Fprintf (FILE *pFile, const char *pArgs, ...)
{
va_list     pArg;
va_start(pArg, pArgs);
vsprintf(sgPrint, pArgs, pArg);
va_end(pArg);
int Len = strlen(sgPrint);
ASSERT(Len < MAX_PRINT_LEN-1);
if (Len != fprintf(pFile, "%s", sgPrint))
    Err("can't write to file: %s", sgPrint);
}

//-----------------------------------------------------------------------------
// like Fprintf above but has a filename parameter for better error reporting

void Fprintf (const char sFile[], FILE *pFile, const char *pArgs, ...)
{
va_list     pArg;
va_start(pArg, pArgs);
vsprintf(sgPrint, pArgs, pArg);
va_end(pArg);
int Len = strlen(sgPrint);
ASSERT(Len < MAX_PRINT_LEN-1);
if (Len != fprintf(pFile, "%s", sgPrint))
    Err("can't write to %s: %s", sFile, sgPrint);
}

//-----------------------------------------------------------------------------
// like fread but prints a message and exits if the read fails

void Fread (void *buf, size_t Size, size_t Count, FILE *pFile, const char sFile[])
{
if (fread(buf, Size, Count, pFile) != Count)
    Err("can't read %s", sFile);
}

*/
//-----------------------------------------------------------------------------
// like gets but skips white space lines and comments (lines beginning with #)
// Also removes \r if necessary.

char *Fgets (char s[], int n, FILE *stream, bool fTreatQuoteAsComment)
{
char *p;
do
    {
    p = fgets(s, n, stream);
    if (p)
        {
        // discard \r
        int len = strlen(s);
        if (len >= 2 && s[len-2] == '\r')
            {
            s[len-2] = '\n';
            s[len-1] = 0;
            }
        }
    }
while ((p != NULL) &&
            (s[0] == '#' ||
            (fTreatQuoteAsComment && s[0] == '"') ||
            s[strspn(s, " \t\n")] == 0));   // all white space line

return p;
}
/*
//-----------------------------------------------------------------------------
// given "C:/bin/cat.exe" returns "cat.exe"

char *sGetBaseExt (const char sPath[])
{
static char s[SLEN];
char sBase[SLEN], sExt[SLEN];
splitpath(sPath, NULL, NULL, sBase, sExt);
sprintf(s, "%s%s", sBase, sExt);
return s;
}

//-----------------------------------------------------------------------------
// given "C:/bin/cat.exe" returns "cat"

char *sGetBase (const char sPath[])
{
static char s[SLEN];
splitpath(sPath, NULL, NULL, s, NULL);
return s;
}

/*
//-----------------------------------------------------------------------------
char *sFormatTime (double Time)
{
int temp1, temp;
static char s[SLEN];

if (Time >= (60.0 * 60.0 * CLOCKS_PER_SEC))
    {
    temp = (int)(Time / (60.0 * CLOCKS_PER_SEC));
    temp1 = temp / 60;
    sprintf(s, "%d hour%s %d min", temp1, ((temp1==1)? "":"s"),
        temp - 60 * temp1);
    }
else if (Time >= 60.0 * CLOCKS_PER_SEC)
    {
    temp1 = (int)(Time / (60.0 * CLOCKS_PER_SEC));
    temp = (int)(Time/CLOCKS_PER_SEC - 60 * temp1);
    sprintf(s, "%d min %d sec%s", temp1, temp, ((temp==1)? "":"s"));
    }
else
    sprintf(s, "%.2f secs", Time / CLOCKS_PER_SEC);

return s;
}
*/

//-----------------------------------------------------------------------------
// Return the current line number in pFile.
// This is very slow but is only used for reporting errors.

int nGetLineNbr (FILE *pFile)
{
if (!pFile)
    return 0;
int iLine = 0;
long n = ftell(pFile);      // original position in file
fseek(pFile, 0, SEEK_SET);  // rewind to start of file
while (ftell(pFile) < n)    // read lines until back at original position
    {
    if (!fgets(sgPrint, MAX_PRINT_LEN-1, pFile))
        return iLine;       // should never get here
    iLine++;
    }
fseek(pFile, n, SEEK_SET);  // restore original position
return iLine;
}

//-----------------------------------------------------------------------------
#ifdef WIN32
static void ConvertBackslashesToForward (char s[])
{
for (int i = 0; s[i]; i++)
    if (s[i] == '\\')
        s[i] = '/';
}
#endif

//-----------------------------------------------------------------------------
// Get the location of the data directory (which holds the data files).
// Do this by using the directory containing the program that is currently
// running (e.g. stasm.exe), and append ../data.
// This means that you can invoke stasm.exe from any directory and
// we still find the correct data directory.

/*
void GetDataDir (char sDataDir[],        // out
                 const char sExePath[])  // in
{
#ifdef WIN32

    char s[SLEN];

    if (sExePath[0] == '"')
        {
        // Special handling for paths like "C:/stasm/stasm.exe"
        // Used when this is run as a window application from "\Program Files\stasm"

        // Copy sExePath but with quotes and trailing spaces stripped
        int j = 0;
        for (int i = 1; sExePath[i] && sExePath[i] != '"'; i++)
                s[j++] = sExePath[i];
        s[j] = 0;
        }
    else
        strcpy(s, sExePath);

    // drop filename.exe

    char sDrive[_MAX_DRIVE], sDir[_MAX_DIR];
    _splitpath(s, sDrive, sDir, NULL, NULL);
    _makepath(s, sDrive, sDir, NULL, NULL);

    // strip final '\' if any

    const int len = strlen(s);
    if (s[len-1] == '\\')
        s[len-1] = 0;

    ConvertBackslashesToForward(s);
    char s1[SLEN];
    if (s[0])
        sprintf(s1, "%s/../data", s);
    else
        sprintf(s1, "../data");

    // hack so stasm.exe can be in Release/ or Debug/ subdirectory (VisualC IDE environment)

    struct _stat stat;
    if (_stat(s1, &stat) != 0)
        {
        char s2[SLEN];
        if (s[0])
            sprintf(s2, "%s/../../data", s);
        else
            sprintf(s2, "../../data");
        if (_stat(s2, &stat) != 0)
            Err("Can't locate the data directory\n(tried %s and %s)", s1, s2);
        strcpy(s1, s2);
        }
    _fullpath(sDataDir, s1, SLEN-1);

    // force drive prefix C: to be upper case (for mingw compat with VisualC)

    if (strlen(sDataDir) && sDataDir[1] == ':')
        sDataDir[0] = toupper(sDataDir[0]);
#else

    // TODOSTASMS this only works if the current directory is the stasm directory
    sprintf(sDataDir, "../data");

#endif
}
*/
/*
//-----------------------------------------------------------------------------
void PrintCmdLine (FILE *pFile, int argc, const char *argv[])
{
ASSERT(pFile);

fprintf(pFile, "Command: ");

// Different version of the run time libraries have a different argv[0] so print
// only the base.  This actually only matters in tests when we diff log files.

char sBase[SLEN];
splitpath(argv[0], NULL, NULL, sBase, NULL);
fprintf(pFile, "%s ", sBase);

int n = MIN(10, argc);
for (int i = 1 ; i < n; i++)
    fprintf(pFile, "%s ", argv[i]);
if (n != argc)
    fprintf(pFile, "...");
fprintf(pFile, "\n");
}

//-----------------------------------------------------------------------------
void CheckFileIsReadable (const char sFile[])
{
  FILE *pFile = fopen(sFile, "rb");
  if (!pFile)
    Err("can't open %s", sFile);   // will call exit(1)
  fclose(pFile);
}

//-----------------------------------------------------------------------------
static bool fWriteableDirExists (const char sDir[])
{
#if _MSC_VER // microsoft
    struct _stat st;
    if (_stat(sDir, &st) == 0)
#else
    struct stat st;
    if (stat(sDir, &st) == 0)
#endif
        return (st.st_mode & S_IFDIR) != 0 && (st.st_mode & S_IWRITE) != 0;
    return false;
}

//-----------------------------------------------------------------------------
char *sGetTempDir (void)
{
static char sDir[SLEN];
if (sDir[0] == 0)
    {
    const char *sTmpDir = getenv("TMPDIR");
    if (!sTmpDir || !fWriteableDirExists(sTmpDir))
        sTmpDir = getenv("TEMP");
    if (!sTmpDir || !fWriteableDirExists(sTmpDir))
        sTmpDir = getenv("TMP");
    if (!sTmpDir || !fWriteableDirExists(sTmpDir))
    #if _WIN32
        Err("can't find temporary directory (no or incorrect evironment variables TMPDIR TEMP TMP)");
    #else
        sTmpDir = "/tmp";       // TODOSTASMS: is this the usual way in linux environments?
    #endif
    strcpy(sDir, sTmpDir);
    }
return sDir;
}

//-----------------------------------------------------------------------------
// PacifyUser prints a progress digit every 10%
//
// To init: call InitPacifyUser with n = total loop count
// To use:  call this in loop repeatedly with i = current loop count
//
// This will print 0123456789
// I usually terminate with a final 0, printed manually outside this routine.
//
// TODOSTASMS this doesn't work well if n too small (<10?)

static const int CONF_nPacifyPercent = 100/10;      // pacify every 10%

static int ngPacify;
static int ngPacify1;   // smallest nbr divisible by 10 but bigger than n
static int ngPacifyPercent;

void InitPacifyUser (int n)
{
ngPacify = 0;
ngPacifyPercent = CONF_nPacifyPercent;
ngPacify1 = ((n + CONF_nPacifyPercent) / CONF_nPacifyPercent) * CONF_nPacifyPercent;
}

void PacifyUser (int i)
{
if (i >= ngPacify)
    {
    // we use printf and not lprintf here because we want to print to
    // the screen but not to the log file

    printf("%d", (ngPacify * 10) / ngPacify1);
    fflush(stdout);
    ngPacify += ngPacify1 / ngPacifyPercent;
    }
}

/*-----------------------------------------------------------------------------
 * Our own version of _splitpath so we don't need the WIN32 code.
 * This function was copied from string.h in the the Wine sources
 * http://source.winehq.org/WineAPI/_splitpath.html
 * which had the following header:
 *
 * Copyright 2000 Alexandre Julliard
 * Copyright 2000 Jon Griffiths
 * Copyright 2003 Thomas Mertes
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

void splitpath(const char* inpath, char * drv, char * dir,
               char* fname, char * ext )
{
    const char *p, *end;

    if (inpath[0] && inpath[1] == ':')
    {
        if (drv)
        {
            drv[0] = inpath[0];
            drv[1] = inpath[1];
            drv[2] = 0;
        }
        inpath += 2;
    }
    else if (drv) drv[0] = 0;

    /* look for end of directory part */
    end = NULL;
    for (p = inpath; *p; p++) if (*p == '/' || *p == '\\') end = p + 1;

    if (end)  /* got a directory */
    {
        if (dir)
        {
            memcpy( dir, inpath, end - inpath );
            dir[end - inpath] = 0;
        }
        inpath = end;
    }
    else if (dir) dir[0] = 0;

    /* look for extension: what's after the last dot */
    end = NULL;
    for (p = inpath; *p; p++) if (*p == '.') end = p;

    if (!end) end = p; /* there's no extension */

    if (fname)
    {
        memcpy( fname, inpath, end - inpath );
        fname[end - inpath] = 0;
    }
    if (ext) strcpy( ext, end );
}

/*-----------------------------------------------------------------------------
 * Our own version of _makepath so we don't need the WIN32 code.
 * This function was copied from dir.h in the the Wine sources
 * http://source.winehq.org/WineAPI/_makepath.html
 * which had the following header.
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997,2000 Uwe Bonnes
 * Copyright 2000 Jon Griffiths
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

void makepath(char * path, const char * drive,
              const char *directory, const char * filename,
              const char * extension)
{
    char *p = path;

    if ( !path )
        return;

    if (drive && drive[0])
    {
        *p++ = drive[0];
        *p++ = ':';
    }
    if (directory && directory[0])
    {
        unsigned int len = strlen(directory);
        memmove(p, directory, len);
        p += len;
        if (p[-1] != '/' && p[-1] != '\\')
            *p++ = '/'; // milbo: was '\\'
    }
    if (filename && filename[0])
    {
        unsigned int len = strlen(filename);
        memmove(p, filename, len);
        p += len;
    }
    if (extension && extension[0])
    {
        if (extension[0] != '.')
            *p++ = '.';
        strcpy(p, extension);
    }
    else
        *p = '\0';
}

#if 0 // works, but unused
//----------------------------------------------------------------------------
// Return the name of a currently non-existing writeable file that has the
// name given in sPattern, but with NNN appended to the basename (where NNN
// is a number from 000 to 999).
// So for example, "/dir/myfile.bmp" becomes "/dir/myfile000.bmp".

char *sGetNewWriteableFilename (const char sPattern[])
{
static char sPath[SLEN]; // returned path name
static int iLastFile;    // start where we left last time for efficiency

char sDrive[_MAX_DRIVE], sDir[_MAX_DIR], sBase[_MAX_FNAME], sExt[_MAX_EXT];
splitpath(sPattern, sDrive, sDir, sBase, sExt);

for (int iFile = iLastFile; iFile < 1000; iFile++)
    {
    char sBaseN[SLEN]; sprintf(sBaseN, "%s%3.3d", sBase, iFile);
    _makepath(sPath, sDrive, sDir, sBaseN, sExt);
    if (_access(sPath, 0) == -1)
        {
        // sPath does not exist, check that we can create it

        FILE *pFile = fopen(sPath, "w");
        if (pFile)
            {
            fclose(pFile);
            unlink(sPath);
            return sPath;
            }
        }
    }
return NULL; // unsuccesful
}
#endif

// $err.cpp 3.0 milbo$ error reporting routines
//
// Many routines in this file use the general purpose global
// buffer sgBuf for storing error messages.
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
#include "SWExceptions.h"
using namespace swExcept;

bool fgErr;                         // set if there is an error used by Windows
static char sgErr[MAX_PRINT_LEN];   // general purpose error msg buffer

//-----------------------------------------------------------------------------
// Print an error message and exit.
// This puts a \n on the msg so you shouldn't.
#include <iostream>
void Err (const char *pArgs, ...)       // args like printf
{
	fgErr = false;
	throw stasmError();
}

//-----------------------------------------------------------------------------
// Print an warning message with standard "Warning" prefix
// Using a standard format allows easy post processing of
// log file: grep for "Warning:"
// This puts a \n on the msg so you shouldn't.

void Warn (const char *pArgs, ...)              // args like printf
{
	va_list pArg;
	va_start(pArg, pArgs);
	vsprintf(sgErr, pArgs, pArg);
	va_end(pArg);
	// lprintf("Warning: %s\n", sgErr);
}

//-----------------------------------------------------------------------------
// same as Warn() but prefix a new line

void WarnWithNewLine (const char *pArgs, ...)   // args like printf
{
	// lprintf("\n");
	va_list pArg;
	va_start(pArg, pArgs);
	vsprintf(sgErr, pArgs, pArg);
	va_end(pArg);
	// lprintf("Warning: %s\n", sgErr);
}

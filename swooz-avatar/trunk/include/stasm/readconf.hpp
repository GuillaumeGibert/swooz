// $readconf.hpp 3.0 milbo$ routines to read .conf files
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

#if !defined(readconf_hpp)
#define readconf_hpp

// namespace swStasm
// {


	typedef struct CONF_ENTRY
	    {
	    void *p;            // pointer to parameter value (double, int, or bool)
	    const char *sName;  // parameter name eg nEigs
	    double Min;         // min allowed parameter value
	    double Max;         // max allowed parameter value
	    char Type;          // parameter type: 'd'=double, 'i'=int, 'f'=bool 'x'=hex
	    }
	CONF_ENTRY;

	void
	ReadConfTab(const CONF_ENTRY ConfTab[], // in: all
		     const int nConfTab,
		     FILE *pFile,
		     const char sFile[]);       // in: filename, for err reporting only
	void
	ReadConfFile(const CONF_ENTRY ConfTab[], // in: all
		     const int nConfTab,
		     const char sFile[],
		     const char sMagic[] = "CONF");
    // };

#endif // readconf_hpp

@rem The following is a basic check that you have VC 10.0 (2008) on your path
@echo off

if not exist ./bin/nul (
    mkdir bin
)

if not exist ./lib/nul (
    mkdir lib
)

if not exist ./moc/nul (
    mkdir moc
)

if not exist ./doc/nul (
    mkdir doc
)

if not exist ./genUI/nul (
	mkdir genUI
)

@nmake -nologo CFG=%1 -f makefile-cuda
if exist "%VS100COMNTOOLS%/vsvars32.bat" (
 	call "%VS100COMNTOOLS%/vsvars32.bat"
	goto doit
)

@echo Environment is not VC 10.0
@goto end

:doit 

@nmake -nologo CFG=%1 -f makefile 
 
:end

@rem The following is a basic check that you have VC 10.0 (2008) on your path
@echo off

if not exist ./bin/nul (
    mkdir bin
)

if not exist ./lib/nul (
    mkdir lib
)

if not exist ./lib/amd64/nul (
    cd lib
    mkdir amd64
    cd amd64
    mkdir Release
    mkdir Debug
    cd ..
    mkdir x86
    cd x86
    mkdir Release
    mkdir Debug
    cd ..
    cd ..
)

if not exist ./doc/nul (
    mkdir doc
)


if %2 == x86 (
    if exist "%VS100COMNTOOLS%/vsvars32.bat" (
            call "%VS100COMNTOOLS%/vsvars32.bat"
            goto doit
    )
)

if %2 == amd64 (
    if exist "%VS100COMNTOOLS%/../../VC/vcvarsall.bat" (
            call "%VS100COMNTOOLS%/../../VC/vcvarsall.bat" amd64
            goto doit
    )
)


@echo Environment is not VC 10.0
@goto end

:doit

@nmake -nologo CFG=%1 ARCH=%2 -f makefile

:end

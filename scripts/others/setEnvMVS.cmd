
@echo off

if %3 == cl (
    if %2 == x86 (
        if exist "%VS100COMNTOOLS%/vsvars32.bat" (
                call "%VS100COMNTOOLS%/vsvars32.bat"
        )
    )
    if %2 == amd64 (
        if exist "%VS100COMNTOOLS%/../../VC/vcvarsall.bat" (
                call "%VS100COMNTOOLS%/../../VC/vcvarsall.bat" amd64
        )
    )

    if %CUDA_FOUND% == yes (
        if exist makefile-cuda (
            @nmake -nologo CFG=%1 ARCH=%2 -f makefile-cuda
        )
    )

    if exist makefile (
        @nmake -nologo CFG=%1 ARCH=%2 COMP=%3 -f makefile
    )
)

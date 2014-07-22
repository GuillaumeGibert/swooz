# makefile of swooz-toolkit project using Visual C++
# Guillaume Gibert / Florian Lance
# lyon sept 2012

!IF "$(CFG)" != "Release" &&  "$(CFG)" != "Debug"
!ERROR Invalid CFG "($CFG)
!ENDIF

!IF "$(ARCH)" != "x86" &&  "$(ARCH)" != "amd64"
!ERROR Invalid ARCH "($ARCH)
!ENDIF


CC=cl
LINK=link

!INCLUDE <./makefile-include>

############################################################################## OBJ PROGRAMS

    
HMD_OBJ=\
         $(LIBDIR)/SWHeadMountedDisplay.obj\
         $(LIBDIR)/SWSonyHMZT3W.obj\

############################################################################## Makefile commands
	
!if "$(ARCH)" == "x86"
all: $(BINDIR)/SWHeadMountedDisplay.exe
!endif

!if "$(ARCH)" == "amd64"
all:
!endif

############################################################################## exe files

WIN_CONFIG = $(SETARGV) $(BINMODE) $(WINLIBS)

$(BINDIR)/SWHeadMountedDisplay.exe: $(HMD_OBJ)  $(LIBS_HMD)
        $(LINK) /OUT:$(BINDIR)/SWHeadMountedDisplay.exe $(LFLAGS_FEEDBACK) $(HMD_OBJ) $(LIBS_HMD) $(WIN_CONFIG)

##################################################### devices

$(LIBDIR)/SWHeadMountedDisplay.obj: ./src/hmd/SWHeadMountedDisplay.cpp
        $(CC) -c ./src/hmd/SWHeadMountedDisplay.cpp $(CFLAGS_DYN) $(SW_HMD) -Fo"$(LIBDIR)/SWHeadMountedDisplay.obj"
	
$(LIBDIR)/SWSonyHMZT3W.obj: ./src/hmd/SWSonyHMZT3W.cpp
        $(CC) -c ./src/hmd/SWSonyHMZT3W.cpp $(CFLAGS_DYN) $(SW_HMD) -Fo"$(LIBDIR)/SWSonyHMZT3W.obj"


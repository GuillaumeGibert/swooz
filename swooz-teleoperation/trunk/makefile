# $vc10\makefile 0.1 ggibert$ for FaceLab module of SWoOZ project using Visual C++
#
# Release build: mkdir Release; nmake -nologo CFG=Release -f makefile
# Debug build: mkdir Debug; nmake -nologo CFG=Debug -f makefile
#
# ggibert lyon sept 2012

!IF "$(CFG)" != "Release" &&  "$(CFG)" != "Debug" &&  "$(CFG)" != "Profile"
!ERROR Invalid CFG "($CFG)
!ENDIF


CC=cl
LINK=link

# following include allows this makefile to work for all supported versions of VisualC
!INCLUDE <./makefile-include>

############################################################################## OBJ PROGRAMS

OBJ_TELEOPERATION_ICUB=\
        $(LIBDIR)/SWTeleoperation_iCub.obj
	
############################################################################## Makefile commands
	

all: $(BINDIR)/SWTeleoperation_iCub.exe
 
clean:

clean all:

############################################################################## exe files

$(BINDIR)/SWTeleoperation_iCub.exe: $(OBJ_TELEOPERATION_ICUB)  $(LIBS_TELEOP_ICUB)
        $(LINK) /OUT:$(BINDIR)/SWTeleoperation_iCub.exe $(LFLAGS) $(OBJ_TELEOPERATION_ICUB)  $(SETARGV) $(BINMODE) $(LIBS_TELEOP_ICUB) $(WINLIBS)

##################################################### devices

$(LIBDIR)/SWTeleoperation_iCub.obj: ./src/icub/SWTeleoperation_iCub.cpp
        $(CC) -c ./src/icub/SWTeleoperation_iCub.cpp $(CFLAGS_DYN) $(SW_TELE_ICUB) -Fo"$(LIBDIR)/"

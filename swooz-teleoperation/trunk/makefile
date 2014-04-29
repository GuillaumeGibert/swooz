# makefile of swooz-teleoperation project using Visual C++
# Guillaume Gibert / Florian Lance
# lyon sept 2013

!IF "$(CFG)" != "Release" &&  "$(CFG)" != "Debug"
!ERROR Invalid CFG "($CFG)
!ENDIF

!IF "$(ARCH)" != "x86" &&  "$(ARCH)" != "amd64"
!ERROR Invalid ARCH "($ARCH)
!ENDIF

CC=cl
LINK=link

# following include allows this makefile to work for all supported versions of VisualC
!INCLUDE <./makefile-include>

############################################################################## OBJ PROGRAMS

OBJ_TELEOPERATION_ICUB=\
        $(LIBDIR)/SWIcubHead.obj\
        $(LIBDIR)/SWIcubTorso.obj\
        $(LIBDIR)/SWTeleoperation_iCub.obj\

OBJ_TELEOPERATION_NAO=\
        $(LIBDIR)/SWTeleoperation_nao.obj\

	
############################################################################## Makefile commands

!if "$(ARCH)" == "x86"
all: $(BINDIR)/SWTeleoperation_iCub.exe $(BINDIR)/SWTeleoperation_nao.exe
!endif

!if "$(ARCH)" == "amd64"
all:
!endif

############################################################################## exe files

$(BINDIR)/SWTeleoperation_iCub.exe: $(OBJ_TELEOPERATION_ICUB)  $(LIBS_TELEOP_ICUB)
        $(LINK) /OUT:$(BINDIR)/SWTeleoperation_iCub.exe $(LFLAGS) $(OBJ_TELEOPERATION_ICUB)  $(SETARGV) $(BINMODE) $(LIBS_TELEOP_ICUB) $(WINLIBS)

$(BINDIR)/SWTeleoperation_nao.exe: $(OBJ_TELEOPERATION_NAO) $(LIBS_TELEOP_NAO)
        $(LINK) /OUT:$(BINDIR)/SWTeleoperation_nao.exe $(LFLAGS) $(OBJ_TELEOPERATION_NAO)  $(SETARGV) $(BINMODE) $(LIBS_TELEOP_NAO) $(WINLIBS)

##################################################### devices

$(LIBDIR)/SWIcubHead.obj: ./src/icub/SWIcubHead.cpp
        $(CC) -c ./src/icub/SWIcubHead.cpp $(CFLAGS_DYN) $(SW_TELE_ICUB) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWIcubTorso.obj: ./src/icub/SWIcubTorso.cpp
        $(CC) -c ./src/icub/SWIcubTorso.cpp $(CFLAGS_DYN) $(SW_TELE_ICUB) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWTeleoperation_iCub.obj: ./src/icub/SWTeleoperation_iCub.cpp
        $(CC) -c ./src/icub/SWTeleoperation_iCub.cpp $(CFLAGS_DYN) $(SW_TELE_ICUB) -Fo"$(LIBDIR)/"


$(LIBDIR)/SWTeleoperation_nao.obj: ./src/nao/SWTeleoperation_nao.cpp
        $(CC) -c ./src/nao/SWTeleoperation_nao.cpp $(CFLAGS_DYN) $(SW_TELE_NAO) -Fo"$(LIBDIR)/"



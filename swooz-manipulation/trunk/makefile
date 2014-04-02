# makefile of swooz-manipulation project using Visual C++
# Guillaume Gibert / Florian Lance
# lyon feb 2014

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

OBJ_MANIPULATION=\
        $(LIBDIR)/SWManipulationInterface.obj\

############################################################################## Makefile commands

!if  "$(ARCH)" == "x86"
all: manipulation
!endif

!if "$(ARCH)" == "amd64"
all:
!endif

manipulation       : $(QTGENW)/SWUI_Manipulation.h $(MOCDIR)/moc_SWManipulationInterface.cpp $(BINDIR)/SWManipulation.exe

clean:

clean all:

############################################################################## exe files

$(BINDIR)/SWManipulation.exe: $(OBJ_MANIPULATION) $(LIBS_MANIPULATION)
        $(LINK) /OUT:$(BINDIR)/SWManipulation.exe $(LFLAGS_MANIPULATION) $(OBJ_MANIPULATION) $(SETARGV) $(BINMODE) $(LIBS_MANIPULATION) $(WINLIBS)

############################################################################## MANIPULATION OBJ

$(LIBDIR)/SWManipulationInterface.obj: ./src/SWManipulationInterface.cpp
        $(CC) -c ./src/SWManipulationInterface.cpp $(CFLAGS_STA) $(SW_MANIPULATION) -Fo"$(LIBDIR)/"

############################################################################## Qt ui files

$(QTGENW)/SWUI_Manipulation.h: $(FORMDIR)/SWUI_Manipulation.ui
        uic $(FORMDIR)/SWUI_Manipulation.ui -o $@

############################################################################## Qt moc files

$(MOCDIR)/moc_SWManipulationInterface.cpp: ./include/SWManipulationInterface.h
         moc  ./include/SWManipulationInterface.h -o $@

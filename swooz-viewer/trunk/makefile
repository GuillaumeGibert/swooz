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

!INCLUDE <./makefile-include>


############################################################################## OBJ LISTS

VIEWER_LINK_D_OBJ=\
    $(DIST_LIBDIR)/SWCloud_d.obj $(DIST_LIBDIR)/SWMesh_d.obj $(DIST_LIBDIR)/SWGLWidget_d.obj $(DIST_LIBDIR)/SWQtCamera_d.obj $(DIST_LIBDIR)/SWGLMultiObjectWidget_d.obj $(LIBDIR)/SWViewerInterface_d.obj\

############################################################################## Makefile commands

!if "$(ARCH)" == "x86"
all: $(QTGENW)/SWUI_Viewer.h $(MOCDIR)/moc_SWViewerInterface.cpp $(BINDIR)/SWViewer.exe
!endif

!if "$(ARCH)" == "amd64"
all: $(QTGENW)/SWUI_Viewer.h $(MOCDIR)/moc_SWViewerInterface.cpp $(BINDIR)/SWViewer-x64.exe
!endif

############################################################################## exe files

WIN_CONFIG = $(SETARGV) $(BINMODE) $(WINLIBS)

# exec files to be generated
$(BINDIR)/SWViewer.exe: $(VIEWER_LINK_D_OBJ) $(LIBS_VIEWER)
        $(LINK) /OUT:$(BINDIR)/SWViewer.exe $(LFLAGS_VIEWER) $(VIEWER_LINK_D_OBJ) $(LIBS_VIEWER) $(WIN_CONFIG)


$(BINDIR)/SWViewer-x64.exe: $(VIEWER_LINK_D_OBJ) $(LIBS_VIEWER)
        $(LINK) /OUT:$(BINDIR)/SWViewer-x64.exe $(LFLAGS_VIEWER) $(VIEWER_LINK_D_OBJ) $(LIBS_VIEWER) $(WIN_CONFIG)

##################################################### devices

$(LIBDIR)/SWViewerInterface_d.obj: ./src/SWViewerInterface.cpp
        $(CC) -c ./src/SWViewerInterface.cpp $(CFLAGS_DYN) $(SW_VIEWER_INTERFACE) -Fo"$(LIBDIR)/SWViewerInterface_d.obj"

############################################################################## Qt ui files

$(QTGENW)/SWUI_Viewer.h: $(FORMDIR)/SWUI_Viewer.ui
        uic $(FORMDIR)/SWUI_Viewer.ui -o $@

############################################################################## Qt moc files

$(MOCDIR)/moc_SWViewerInterface.cpp: ./include/SWViewerInterface.h
         moc  ./include/SWViewerInterface.h -o $@



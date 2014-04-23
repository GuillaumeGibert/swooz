# makefile of swooz-tracking project using Visual C++
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


############################################################################## OBJ PROGRAMS

OBJ_TRACKING_HEAD_FOREST=\
        $(LIBDIR)/gl_camera.obj \
        $(LIBDIR)/CRForestEstimator.obj \
        $(LIBDIR)/CRTree.obj\
        $(LIBDIR)/SWForestHeadTracking.obj\

OBJ_TRACKING_OPENNI=\
        $(DIST_LIBDIR)/SWKinectSkeleton_d.obj\
        $(LIBDIR)/SWOpenNITracking.obj\


OBJ_TRACKING_FACELAB=\
        $(LIBDIR)/SWFaceLabTracking_d.obj\
        $(DIST_LIBDIR)/SWFaceLab_d.obj\
        $(DIST_LIBDIR)/FaceLab_d.obj\

OBJ_TRACKING_FACESHIFT=\
        $(LIBDIR)/fsbinarystream.obj\
        $(LIBDIR)/SWFaceShiftTracking.obj\

OBJ_TRACKING_EMCIP=\
        $(DIST_LIBDIR)/SWKinect_thread.obj $(DIST_LIBDIR)/SWKinect.obj\
        $(DIST_LIBDIR)/SWCaptureHeadMotion.obj $(DIST_LIBDIR)/SWAlignClouds.obj \
        $(DIST_LIBDIR)/SWCloud.obj $(DIST_LIBDIR)/emicp.obj $(DIST_LIBDIR)/findRTfromS.obj\
        $(DIST_LIBDIR)/SWFaceDetection.obj $(DIST_LIBDIR)/SWHaarCascade.obj\
        $(DIST_LIBDIR)/SWDisplayImageWidget.obj $(DIST_LIBDIR)/SWDisplayCurvesWidget.obj\
        $(DIST_LIBDIR)/SWGLCloudWidget.obj $(DIST_LIBDIR)/SWGLWidget.obj $(DIST_LIBDIR)/SWQtCamera.obj \
        $(LIBDIR)/SWEmicpHeadTracking.obj\

OBJ_TRACKING_FORTH=\
        $(LIBDIR)/SWForthTracking.obj

############################################################################## Makefile commands

!if  "$(ARCH)" == "x86"
all: trackingHeadForest trackingHeadEmicp trackingHeadForest trackingFaceLab trackingFaceShift trackingOpenNI
!endif

!if "$(ARCH)" == "amd64"
all:
!endif
#trackingForth

trackingHeadForest : $(BINDIR)/SWTrackingHeadForest.exe
trackingFaceLab    : $(BINDIR)/SWFaceLabTracking.exe
trackingFaceShift  : $(BINDIR)/SWFaceShiftTracking.exe
trackingHeadEmicp  : $(QTGENW_RGBD)/SWUI_WEmicpHeadTracking.h $(MOCDIR)/moc_SWEmicpHeadTracking.cpp $(BINDIR)/SWEmicpHeadTracking.exe
trackingOpenNI     : $(BINDIR)/SWOpenNITracking.exe
trackingForth      : $(BINDIR)/SWForthTracking.exe

clean:

clean all:

############################################################################## exe files

$(BINDIR)/SWTrackingHeadForest.exe: $(OBJ_TRACKING_HEAD_FOREST)  $(LIBS_HEAD_FOREST)
        $(LINK) /OUT:$(BINDIR)/SWTrackingHeadForest.exe $(LFLAGS_FOREST_HEAD_TRACKING) $(OBJ_TRACKING_HEAD_FOREST) $(SETARGV) $(BINMODE) $(LIBS_HEAD_FOREST) $(WINLIBS)

$(BINDIR)/SWFaceLabTracking.exe: $(OBJ_TRACKING_FACELAB) $(LIBS_FACELAB_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWFaceLabTracking.exe $(LFLAGS) $(OBJ_TRACKING_FACELAB) $(SETARGV) $(BINMODE) $(LIBS_FACELAB_TRACK) $(WINLIBS)

$(BINDIR)/SWFaceShiftTracking.exe: $(OBJ_TRACKING_FACESHIFT) $(LIBS_FACESHIFT_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWFaceShiftTracking.exe $(LFLAGS_FACESHIFT_TRACKING) $(OBJ_TRACKING_FACESHIFT) $(SETARGV) $(BINMODE) $(LIBS_FACESHIFT_TRACK) $(WINLIBS)

$(BINDIR)/SWEmicpHeadTracking.exe: $(OBJ_TRACKING_EMCIP) $(LIBS_EMICP_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWEmicpHeadTracking.exe $(LFLAGS_EMICP_HEAD_TRACKING) $(OBJ_TRACKING_EMCIP) $(SETARGV) $(BINMODE) $(LIBS_EMICP_TRACK) $(WINLIBS)

$(BINDIR)/SWOpenNITracking.exe: $(OBJ_TRACKING_OPENNI) $(LIBS_OPENNI_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWOpenNITracking.exe $(LFLAGS) $(OBJ_TRACKING_OPENNI) $(SETARGV) $(BINMODE) $(LIBS_OPENNI_TRACK) $(WINLIBS)

$(BINDIR)/SWForthTracking.exe: $(OBJ_TRACKING_FORTH) $(LIBS_FORTH_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWForthTracking.exe $(LFLAGS) $(OBJ_TRACKING_FORTH) $(SETARGV) $(BINMODE) $(LIBS_FORTH_TRACK) $(WINLIBS)

############################################################################## OPENNI TRACKING OBJ

$(LIBDIR)/SWOpenNITracking.obj: ./src/rgbd/SWOpenNITracking.cpp
        $(CC) -c ./src/rgbd/SWOpenNITracking.cpp $(CFLAGS_DYN) $(SW_OPENNITRACKING) -Fo"$(LIBDIR)/"

############################################################################## FORTH TRACKING OBJ

$(LIBDIR)/SWForthTracking.obj: ./src/rgbd/SWForthTracking.cpp
        $(CC) -c ./src/rgbd/SWForthTracking.cpp $(CFLAGS_DYN) $(SW_FORTHTRACKING) -Fo"$(LIBDIR)/"

############################################################################## FOREST TRACKING OBJ

$(LIBDIR)/gl_camera.obj: ./src/rgbd/forest/gl_camera.cpp
        $(CC) -c ./src/rgbd/forest/gl_camera.cpp $(CFLAGS_STA) $(SW_FOREST) -Fo"$(LIBDIR)/"

$(LIBDIR)/CRForestEstimator.obj: ./src/rgbd/forest/CRForestEstimator.cpp
        $(CC) -c ./src/rgbd/forest/CRForestEstimator.cpp $(CFLAGS_STA) $(SW_FOREST) -Fo"$(LIBDIR)/"

$(LIBDIR)/CRTree.obj: ./src/rgbd/forest/CRTree.cpp
        $(CC) -c ./src/rgbd/forest/CRTree.cpp $(CFLAGS_STA) $(SW_FOREST) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWForestHeadTracking.obj: ./src/rgbd/SWForestHeadTracking.cpp
        $(CC) -c ./src/rgbd/SWForestHeadTracking.cpp $(CFLAGS_STA) $(SW_FOREST) -Fo"$(LIBDIR)/"

############################################################################## FACELAB TRACKING OBJ

$(LIBDIR)/SWFaceLabTracking_d.obj: ./src/facelab/SWFaceLabTracking.cpp
        $(CC) -c ./src/facelab/SWFaceLabTracking.cpp $(CFLAGS_DYN) $(SW_FACELABTRACKING) -Fo"$(LIBDIR)/SWFaceLabTracking_d.obj"

############################################################################## FACESHIFT TRACKING OBJ

$(LIBDIR)/fsbinarystream.obj: ./src/rgbd/faceshift/fsbinarystream.cpp
        $(CC) -c ./src/rgbd/faceshift/fsbinarystream.cpp $(CFLAGS_STA) $(FSBINARYSTREAM) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWFaceShiftTracking.obj: ./src/rgbd/SWFaceShiftTracking.cpp
        $(CC) -c ./src/rgbd/SWFaceShiftTracking.cpp $(CFLAGS_STA) $(SW_FACESHIFTTRACKING) -Fo"$(LIBDIR)/"

############################################################################## EMCIP HEAD TRACKING OBJ

$(LIBDIR)/SWEmicpHeadTracking.obj: ./src/rgbd/SWEmicpHeadTracking.cpp
        $(CC) -c ./src/rgbd/SWEmicpHeadTracking.cpp $(CFLAGS_STA) $(SW_EMICPHEADTRACKING) -Fo"$(LIBDIR)/"

############################################################################## Qt ui files

$(QTGENW_RGBD)/SWUI_WEmicpHeadTracking.h: $(FORMDIR)/SWUI_WEmicpHeadTracking.ui
        uic $(FORMDIR)/SWUI_WEmicpHeadTracking.ui -o $@

############################################################################## Qt moc files

$(MOCDIR)/moc_SWEmicpHeadTracking.cpp: ./include/rgbd/SWEmicpHeadTracking.h
         moc  ./include/rgbd/SWEmicpHeadTracking.h -o $@

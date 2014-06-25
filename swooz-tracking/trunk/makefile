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
        $(LIBDIR)/gl_camera_d.obj \
        $(LIBDIR)/CRForestEstimator_d.obj \
        $(LIBDIR)/CRTree_d.obj\
        $(LIBDIR)/SWForestHeadTracking_d.obj\

OBJ_TRACKING_OPENNI=\
        $(LIBDIR)/SWOpenNITracking.obj\

OBJ_TRACKING_FACELAB=\
        $(LIBDIR)/SWFaceLabTracking_d.obj\

OBJ_TRACKING_FACESHIFT=\
        $(LIBDIR)/fsbinarystream_d.obj\
        $(LIBDIR)/SWFaceShiftTracking_d.obj\

OBJ_TRACKING_EMCIP=\
        $(DIST_LIBDIR)/SWCaptureHeadMotion_d.obj $(DIST_LIBDIR)/SWDisplayImageWidget_d.obj $(DIST_LIBDIR)/SWDisplayCurvesWidget_d.obj\
        $(DIST_LIBDIR)/SWGLCloudWidget_d.obj $(DIST_LIBDIR)/SWGLWidget_d.obj $(DIST_LIBDIR)/SWQtCamera_d.obj\
        $(LIBDIR)/SWEmicpHeadTracking_d.obj\

OBJ_TRACKING_FORTH=\
        $(LIBDIR)/SWForthTracking.obj

OBJ_TRACKING_FAKE=\
        $(LIBDIR)/SWFakeTracking_d.obj\

OBJ_TRACKING_LEAP=\
        $(DIST_LIBDIR)/SWLeap_d.obj\
        $(LIBDIR)/SWLeapTracking_d.obj\

############################################################################## Makefile commands

!if  "$(ARCH)" == "x86"
all: trackingHeadForest trackingHeadEmicp trackingHeadForest trackingFaceLab trackingFaceShift trackingOpenNI trackingFake trackingLeap
!endif

!if "$(ARCH)" == "amd64"
all:
!endif

trackingHeadForest : $(BINDIR)/SWTrackingHeadForest.exe
trackingFaceLab    : $(BINDIR)/SWFaceLabTracking.exe
trackingFaceShift  : $(BINDIR)/SWFaceShiftTracking.exe
trackingHeadEmicp  : $(QTGENW_RGBD)/SWUI_WEmicpHeadTracking.h $(MOCDIR)/moc_SWEmicpHeadTracking.cpp $(BINDIR)/SWEmicpHeadTracking.exe
trackingOpenNI     : $(BINDIR)/SWOpenNITracking.exe
trackingForth      : $(BINDIR)/SWForthTracking.exe
trackingFake       : $(BINDIR)/SWFakeTracking.exe
trackingLeap	   : $(BINDIR)/SWLeapTracking.exe

clean:

clean all:

############################################################################## exe files

WIN_CONFIG = $(SETARGV) $(BINMODE) $(WINLIBS)

$(BINDIR)/SWTrackingHeadForest.exe: $(OBJ_TRACKING_HEAD_FOREST)  $(LIBS_HEAD_FOREST)
        $(LINK) /OUT:$(BINDIR)/SWTrackingHeadForest.exe $(LFLAGS) $(OBJ_TRACKING_HEAD_FOREST) $(LIBS_HEAD_FOREST) $(WIN_CONFIG)

$(BINDIR)/SWFaceLabTracking.exe: $(OBJ_TRACKING_FACELAB) $(LIBS_FACELAB_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWFaceLabTracking.exe $(LFLAGS) $(OBJ_TRACKING_FACELAB) $(LIBS_FACELAB_TRACK) $(WIN_CONFIG)

$(BINDIR)/SWFaceShiftTracking.exe: $(OBJ_TRACKING_FACESHIFT) $(LIBS_FACESHIFT_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWFaceShiftTracking.exe $(LFLAGS) $(OBJ_TRACKING_FACESHIFT) $(LIBS_FACESHIFT_TRACK) $(WIN_CONFIG)

$(BINDIR)/SWEmicpHeadTracking.exe: $(OBJ_TRACKING_EMCIP) $(LIBS_EMICP_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWEmicpHeadTracking.exe $(LFLAGS) $(OBJ_TRACKING_EMCIP) $(LIBS_EMICP_TRACK) $(WIN_CONFIG)

$(BINDIR)/SWOpenNITracking.exe: $(OBJ_TRACKING_OPENNI) $(LIBS_OPENNI_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWOpenNITracking.exe $(LFLAGS) $(OBJ_TRACKING_OPENNI) $(LIBS_OPENNI_TRACK) $(WIN_CONFIG)

$(BINDIR)/SWForthTracking.exe: $(OBJ_TRACKING_FORTH) $(LIBS_FORTH_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWForthTracking.exe $(LFLAGS) $(OBJ_TRACKING_FORTH) $(LIBS_FORTH_TRACK) $(WIN_CONFIG)

$(BINDIR)/SWFakeTracking.exe: $(OBJ_TRACKING_FAKE) $(LIBS_FAKE_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWFakeTracking.exe $(LFLAGS) $(OBJ_TRACKING_FAKE) $(LIBS_FAKE_TRACK) $(WIN_CONFIG)

$(BINDIR)/SWLeapTracking.exe: $(OBJ_TRACKING_LEAP) $(LIBS_LEAP_TRACK)
        $(LINK) /OUT:$(BINDIR)/SWLeapTracking.exe $(LFLAGS) $(OBJ_TRACKING_LEAP) $(LIBS_LEAP_TRACK) $(WIN_CONFIG)

############################################################################## OPENNI TRACKING OBJ

$(LIBDIR)/SWOpenNITracking.obj: ./src/rgbd/SWOpenNITracking.cpp
        $(CC) -c ./src/rgbd/SWOpenNITracking.cpp $(CFLAGS_DYN) $(SW_OPENNITRACKING) -Fo"$(LIBDIR)/"

############################################################################## FORTH TRACKING OBJ

$(LIBDIR)/SWForthTracking.obj: ./src/rgbd/SWForthTracking.cpp
        $(CC) -c ./src/rgbd/SWForthTracking.cpp $(CFLAGS_DYN) $(SW_FORTHTRACKING) -Fo"$(LIBDIR)/"

############################################################################## FOREST TRACKING OBJ

$(LIBDIR)/gl_camera_d.obj: ./src/rgbd/forest/gl_camera.cpp
        $(CC) -c ./src/rgbd/forest/gl_camera.cpp $(CFLAGS_DYN) $(SW_FOREST) -Fo"$(LIBDIR)/gl_camera_d.obj"

$(LIBDIR)/CRForestEstimator_d.obj: ./src/rgbd/forest/CRForestEstimator.cpp
        $(CC) -c ./src/rgbd/forest/CRForestEstimator.cpp $(CFLAGS_DYN) $(SW_FOREST) -Fo"$(LIBDIR)/CRForestEstimator_d.obj"

$(LIBDIR)/CRTree_d.obj: ./src/rgbd/forest/CRTree.cpp
        $(CC) -c ./src/rgbd/forest/CRTree.cpp $(CFLAGS_DYN) $(SW_FOREST) -Fo"$(LIBDIR)/CRTree_d.obj"

$(LIBDIR)/SWForestHeadTracking_d.obj: ./src/rgbd/SWForestHeadTracking.cpp
        $(CC) -c ./src/rgbd/SWForestHeadTracking.cpp $(CFLAGS_DYN) $(SW_FOREST) -Fo"$(LIBDIR)/SWForestHeadTracking_d.obj"

############################################################################## FACELAB TRACKING OBJ

$(LIBDIR)/SWFaceLabTracking_d.obj: ./src/facelab/SWFaceLabTracking.cpp
        $(CC) -c ./src/facelab/SWFaceLabTracking.cpp $(CFLAGS_DYN) $(SW_FACELABTRACKING) -Fo"$(LIBDIR)/SWFaceLabTracking_d.obj"

############################################################################## FACESHIFT TRACKING OBJ

$(LIBDIR)/fsbinarystream_d.obj: ./src/rgbd/faceshift/fsbinarystream.cpp
        $(CC) -c ./src/rgbd/faceshift/fsbinarystream.cpp $(CFLAGS_DYN) $(FSBINARYSTREAM) -Fo"$(LIBDIR)/fsbinarystream_d.obj"

$(LIBDIR)/SWFaceShiftTracking_d.obj: ./src/rgbd/SWFaceShiftTracking.cpp
        $(CC) -c ./src/rgbd/SWFaceShiftTracking.cpp $(CFLAGS_DYN) $(SW_FACESHIFTTRACKING) -Fo"$(LIBDIR)/SWFaceShiftTracking_d.obj"

############################################################################## EMCIP HEAD TRACKING OBJ

$(LIBDIR)/SWEmicpHeadTracking_d.obj: ./src/rgbd/SWEmicpHeadTracking.cpp
        $(CC) -c ./src/rgbd/SWEmicpHeadTracking.cpp $(CFLAGS_DYN) $(SW_EMICPHEADTRACKING) -Fo"$(LIBDIR)/SWEmicpHeadTracking_d.obj"

############################################################################## FAKE TRACKING OBJ

$(LIBDIR)/SWFakeTracking_d.obj: ./src/SWFakeTracking.cpp
        $(CC) -c ./src/SWFakeTracking.cpp $(CFLAGS_DYN) $(SW_FAKETRACKING) -Fo"$(LIBDIR)/SWFakeTracking_d.obj"

############################################################################## LEAP TRACKING OBJ

$(LIBDIR)/SWLeapTracking_d.obj: ./src/leap/SWLeapTracking.cpp
        $(CC) -c ./src/leap/SWLeapTracking.cpp $(CFLAGS_DYN) $(SW_LEAPTRACKING) -Fo"$(LIBDIR)/SWLeapTracking_d.obj"

############################################################################## Qt ui files

$(QTGENW_RGBD)/SWUI_WEmicpHeadTracking.h: $(FORMDIR)/SWUI_WEmicpHeadTracking.ui
        uic $(FORMDIR)/SWUI_WEmicpHeadTracking.ui -o $@

############################################################################## Qt moc files

$(MOCDIR)/moc_SWEmicpHeadTracking.cpp: ./include/rgbd/SWEmicpHeadTracking.h
         moc  ./include/rgbd/SWEmicpHeadTracking.h -o $@

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

# following include allows this makefile to work for all supported versions of VisualC
!INCLUDE <./makefile-include>


############################################################################## OBJ PROGRAMS

TOOLKIT_OBJ=\
    $(LIBDIR)/SWKinect.obj $(LIBDIR)/SWKinect_thread.obj $(LIBDIR)/SWSaveKinectData.obj $(LIBDIR)/SWLoadKinectData.obj $(LIBDIR)/SWKinectSkeleton.obj\
    $(LIBDIR)/SWFastrak.obj $(LIBDIR)/SWFastrak_thread.obj $(LIBDIR)/SWOculus.obj $(LIBDIR)/SWOculus_thread.obj \
    $(LIBDIR)/Tobii.obj\

TOOLKIT_DYN_OBJ=\
    $(LIBDIR)/SWKinect_d.obj $(LIBDIR)/SWKinect_thread_d.obj $(LIBDIR)/SWSaveKinectData_d.obj \
    $(LIBDIR)/SWLoadKinectData_d.obj $(LIBDIR)/SWKinectSkeleton_d.obj $(LIBDIR)/FaceLab_d.obj \
    $(LIBDIR)/SWFaceLab_d.obj $(LIBDIR)/SWFastrak_d.obj $(LIBDIR)/SWFastrak_thread_d.obj\
    $(LIBDIR)/SWOculus_d.obj $(LIBDIR)/SWOculus_thread_d.obj\
    $(LIBDIR)/Tobii_d.obj $(LIBDIR)/SWLeap_d.obj\

KINECT_DIMENCO_OBJ=\
    $(LIBDIR)/SWKinect.obj\
    $(LIBDIR)/SWDimenco3DDisplay.obj\

KINECT_OBJ=\
     $(LIBDIR)/SWKinectRFModule_d.obj $(LIBDIR)/SWKinect_d.obj $(LIBDIR)/SWKinect_thread_d.obj\

SYNC_ICUB_OBJ=\
	$(LIBDIR)/SWSynchronizediCubEncoders.obj
############################################################################## Makefile commands
	
# $(KINECT_OBJ) $(FACELAB_OBJ) $(TOBII_OBJ) $(FASTRAK_OBJ)

!if  "$(ARCH)" == "x86"
all: $(BINDIR)/SWDisplayKinectOnDimenco.exe $(BINDIR)/SWKinectRFModule.exe $(LIBDIR)/SWToolkit.lib $(LIBDIR)/SWToolkit_d.lib $(BINDIR)/SWSynchronizediCubEncoders.exe
#$(FILTER_OBJ)
!endif

!if "$(ARCH)" == "amd64"
all:
!endif

############################################################################## lib files

$(LIBDIR)/SWToolkit.lib: $(TOOLKIT_OBJ)
        LIB.EXE /OUT:$(LIBDIR)/SWToolkit.lib $(TOOLKIT_OBJ)

$(LIBDIR)/SWToolkit_d.lib: $(TOOLKIT_DYN_OBJ)
        LIB.EXE /OUT:$(LIBDIR)/SWToolkit_d.lib $(TOOLKIT_DYN_OBJ)

############################################################################## exe files

$(BINDIR)/SWDisplayKinectOnDimenco.exe: $(KINECT_DIMENCO_OBJ)  $(LIBS_DIMENCO)
        $(LINK) /OUT:$(BINDIR)/SWDisplayKinectOnDimenco.exe $(LFLAGS) $(KINECT_DIMENCO_OBJ)  $(SETARGV) $(BINMODE) $(LIBS_DIMENCO) $(WINLIBS)

$(BINDIR)/SWKinectRFModule.exe: $(KINECT_OBJ)  $(LIBS_KINECT)
        $(LINK) /OUT:$(BINDIR)/SWKinectRFModule.exe $(LFLAGS2) $(KINECT_OBJ)  $(SETARGV) $(BINMODE) $(LIBS_KINECT) $(WINLIBS)
	
$(BINDIR)/SWSynchronizediCubEncoders.exe: $(SYNC_ICUB_OBJ)  $(LIBS_YARP)
        $(LINK) /OUT:$(BINDIR)/SWSynchronizediCubEncoders.exe $(LFLAGS2) $(SYNC_ICUB_OBJ)  $(SETARGV) $(BINMODE) $(LIBS_YARP) $(WINLIBS)

##################################################### devices

####### static

$(LIBDIR)/SWKinect.obj: ./src/devices/rgbd/SWKinect.cpp
        $(CC) -c ./src/devices/rgbd/SWKinect.cpp $(CFLAGS_STA) $(SW_KINECT) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWKinect_thread.obj: ./src/devices/rgbd/SWKinect_thread.cpp
        $(CC) -c ./src/devices/rgbd/SWKinect_thread.cpp $(CFLAGS_STA) $(SW_KINECT_THREAD) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWFastrak.obj: ./src/devices/fastrak/SWFastrak.cpp
        $(CC) -c ./src/devices/fastrak/SWFastrak.cpp $(CFLAGS_STA) $(SW_FASTRAK) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWFastrak_thread.obj: ./src/devices/fastrak/SWFastrak_thread.cpp
        $(CC) -c ./src/devices/fastrak/SWFastrak_thread.cpp $(CFLAGS_STA) $(SW_FASTRAK_THREAD) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWOculus.obj: ./src/devices/oculus/SWOculus.cpp
        $(CC) -c ./src/devices/oculus/SWOculus.cpp $(CFLAGS_STA) $(SW_OCULUS) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWOculus_thread.obj: ./src/devices/oculus/SWOculus_thread.cpp
        $(CC) -c ./src/devices/oculus/SWOculus_thread.cpp $(CFLAGS_STA) $(SW_OCULUS_THREAD) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWSaveKinectData.obj: ./src/devices/rgbd/SWSaveKinectData.cpp
        $(CC) -c ./src/devices/rgbd/SWSaveKinectData.cpp $(CFLAGS_STA) $(SW_SAVE_KINECT_DATA) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWLoadKinectData.obj: ./src/devices/rgbd/SWLoadKinectData.cpp
        $(CC) -c ./src/devices/rgbd/SWLoadKinectData.cpp $(CFLAGS_STA) $(SW_LOAD_KINECT_DATA) -Fo"$(LIBDIR)/"

$(LIBDIR)/Tobii.obj: ./src/devices/tobii/Tobii.cpp
        $(CC) -c ./src/devices/tobii/Tobii.cpp $(CFLAGS_STA) $(TOBII) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWKinectSkeleton.obj: ./src/devices/rgbd/SWKinectSkeleton.cpp
        $(CC) -c ./src/devices/rgbd/SWKinectSkeleton.cpp $(CFLAGS_STA) $(SW_KINECT_SKELETON) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWDimenco3DDisplay.obj: ./src/devices/dimenco/SWDimenco3DDisplay.cpp
        $(CC) -c ./src/devices/dimenco/SWDimenco3DDisplay.cpp $(CFLAGS_STA) $(SW_SDIMENCO3DPLAY) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWSynchronizediCubEncoders.obj: ./src/SWSynchronizediCubEncoders.cpp
        $(CC) -c ./src/SWSynchronizediCubEncoders.cpp $(CFLAGS_DYN) $(SW_SYNC_ICUB) -Fo"$(LIBDIR)/"

####### dynamic

$(LIBDIR)/SWKinect_d.obj: ./src/devices/rgbd/SWKinect.cpp
        $(CC) -c ./src/devices/rgbd/SWKinect.cpp $(CFLAGS_DYN) $(SW_KINECT) -Fo"$(LIBDIR)/SWKinect_d.obj"

$(LIBDIR)/SWKinect_thread_d.obj: ./src/devices/rgbd/SWKinect_thread.cpp
        $(CC) -c ./src/devices/rgbd/SWKinect_thread.cpp $(CFLAGS_DYN) $(SW_KINECT_THREAD) -Fo"$(LIBDIR)/SWKinect_thread_d.obj"

$(LIBDIR)/SWFastrak_d.obj: ./src/devices/fastrak/SWFastrak.cpp
        $(CC) -c ./src/devices/fastrak/SWFastrak.cpp $(CFLAGS_DYN) $(SW_FASTRAK) -Fo"$(LIBDIR)/SWFastrak_d.obj"

$(LIBDIR)/SWFastrak_thread_d.obj: ./src/devices/fastrak/SWFastrak_thread.cpp
        $(CC) -c ./src/devices/fastrak/SWFastrak_thread.cpp $(CFLAGS_DYN) $(SW_FASTRAK_THREAD) -Fo"$(LIBDIR)/SWFastrak_thread_d.obj"
	
$(LIBDIR)/SWOculus_d.obj: ./src/devices/oculus/SWOculus.cpp
        $(CC) -c ./src/devices/oculus/SWOculus.cpp $(CFLAGS_DYN) $(SW_OCULUS) -Fo"$(LIBDIR)/SWOculus_d.obj"

$(LIBDIR)/SWOculus_thread_d.obj: ./src/devices/oculus/SWOculus_thread.cpp
        $(CC) -c ./src/devices/oculus/SWOculus_thread.cpp $(CFLAGS_DYN) $(SW_OCULUS_THREAD) -Fo"$(LIBDIR)/SWOculus_thread_d.obj"

$(LIBDIR)/SWSaveKinectData_d.obj: ./src/devices/rgbd/SWSaveKinectData.cpp
        $(CC) -c ./src/devices/rgbd/SWSaveKinectData.cpp $(CFLAGS_DYN) $(SW_SAVE_KINECT_DATA) -Fo"$(LIBDIR)/SWSaveKinectData_d.obj"

$(LIBDIR)/SWLoadKinectData_d.obj: ./src/devices/rgbd/SWLoadKinectData.cpp
        $(CC) -c ./src/devices/rgbd/SWLoadKinectData.cpp $(CFLAGS_DYN) $(SW_LOAD_KINECT_DATA) -Fo"$(LIBDIR)/SWLoadKinectData_d.obj"

$(LIBDIR)/Tobii_d.obj: ./src/devices/tobii/Tobii.cpp
        $(CC) -c ./src/devices/tobii/Tobii.cpp $(CFLAGS_DYN) $(TOBII)  -Fo"$(LIBDIR)/Tobii_d.obj"

$(LIBDIR)/FaceLab_d.obj: ./src/devices/faceLab/FaceLab.cpp
        $(CC) -c ./src/devices/faceLab/FaceLab.cpp $(CFLAGS_DYN) $(FACELAB) -Fo"$(LIBDIR)/FaceLab_d.obj"

$(LIBDIR)/SWFaceLab_d.obj: ./src/devices/faceLab/SWFaceLab.cpp
        $(CC) -c ./src/devices/faceLab/SWFaceLab.cpp $(CFLAGS_DYN) $(SW_FACELAB) -Fo"$(LIBDIR)/SWFaceLab_d.obj"

$(LIBDIR)/SWKinectSkeleton_d.obj: ./src/devices/rgbd/SWKinectSkeleton.cpp
        $(CC) -c ./src/devices/rgbd/SWKinectSkeleton.cpp $(CFLAGS_DYN) $(SW_KINECT_SKELETON) -Fo"$(LIBDIR)/SWKinectSkeleton_d.obj"

$(LIBDIR)/SWLeap_d.obj: ./src/devices/leap/SWLeap.cpp
        $(CC) -c ./src/devices/leap/SWLeap.cpp $(CFLAGS_DYN) $(SW_LEAP) -Fo"$(LIBDIR)/SWLeap_d.obj"

$(LIBDIR)/SWKinectRFModule_d.obj: ./src/devices/rgbd/SWKinectRFModule.cpp
        $(CC) -c ./src/devices/rgbd/SWKinectRFModule.cpp $(CFLAGS_DYN) $(SW_KINECT) -Fo"$(LIBDIR)/SWKinectRFModule_d.obj"
	


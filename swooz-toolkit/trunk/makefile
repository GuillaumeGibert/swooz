# makefile of swooz-toolkit project using Visual C++
# Guillaume Gibert / Florian Lance
# lyon sept 2012

!IF "$(CFG)" != "Release" &&  "$(CFG)" != "Debug" &&  "$(CFG)" != "Profile"
!ERROR Invalid CFG "($CFG)
!ENDIF


CC=cl
LINK=link

# following include allows this makefile to work for all supported versions of VisualC
!INCLUDE <./makefile-include>

############################################################################## OBJ PROGRAMS

KINECT_DIMENCO_OBJ=\
    $(LIBDIR)/SWKinect.obj\
    $(LIBDIR)/SWDimenco3DDisplay.obj\

KINECT_OBJ=\
    $(LIBDIR)/SWKinect.obj $(LIBDIR)/SWKinect_thread.obj $(LIBDIR)/SWKinect_dynamic.obj $(LIBDIR)/SWKinect_thread_dynamic.obj\
    $(LIBDIR)/SWSaveKinectData.obj $(LIBDIR)/SWSaveKinectData_dynamic.obj $(LIBDIR)/SWLoadKinectData.obj $(LIBDIR)/SWLoadKinectData_dynamic.obj\

FACELAB_OBJ=\
    $(LIBDIR)/FaceLab_dynamic.obj $(LIBDIR)/SWFaceLab_dynamic.obj\

FASTRAK_OBJ=\
    $(LIBDIR)/SWFastrak.obj $(LIBDIR)/SWFastrak_dynamic.obj $(LIBDIR)/SWFastrak_thread.obj $(LIBDIR)/SWFastrak_thread_dynamic.obj\

TOBII_OBJ=\
    $(LIBDIR)/Tobii.obj\

FILTER_OBJ=\
    $(LIBDIR)/SWApplyTemporalFilter.obj $(LIBDIR)/SWComputeTemporalFilterCoefficients.obj\

############################################################################## Makefile commands
	
all: $(KINECT_OBJ) $(FACELAB_OBJ) $(TOBII_OBJ) $(FASTRAK_OBJ) $(FILTER_OBJ)

############################################################################## exe files

$(BINDIR)/SWDisplayKinectOnDimenco.exe: $(KINECT_DIMENCO_OBJ)  $(LIBS_DIMENCO)
        $(LINK) /OUT:$(BINDIR)/SWDisplayKinectOnDimenco.exe $(LFLAGS) $(KINECT_DIMENCO_OBJ)  $(SETARGV) $(BINMODE) $(LIBS_DIMENCO) $(WINLIBS)

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
	
$(LIBDIR)/SWSaveKinectData.obj: ./src/devices/rgbd/SWSaveKinectData.cpp
        $(CC) -c ./src/devices/rgbd/SWSaveKinectData.cpp $(CFLAGS_STA) $(SW_SAVE_KINECT_DATA) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWLoadKinectData.obj: ./src/devices/rgbd/SWLoadKinectData.cpp
        $(CC) -c ./src/devices/rgbd/SWLoadKinectData.cpp $(CFLAGS_STA) $(SW_LOAD_KINECT_DATA) -Fo"$(LIBDIR)/"

$(LIBDIR)/Tobii.obj: ./src/devices/tobii/Tobii.cpp
        $(CC) -c ./src/devices/tobii/Tobii.cpp $(CFLAGS_STA) $(TOBII) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWDimenco3DDisplay.obj: ./src/devices/dimenco/SWDimenco3DDisplay.cpp
        $(CC) -c ./src/devices/dimenco/SWDimenco3DDisplay.cpp $(CFLAGS_STA) $(SW_SDIMENCO3DPLAY) -Fo"$(LIBDIR)/"

####### dynamic

$(LIBDIR)/FaceLab_dynamic.obj: ./src/devices/faceLab/FaceLab.cpp
        $(CC) -c ./src/devices/faceLab/FaceLab.cpp $(CFLAGS_DYN) $(FACELAB) -Fo"$(LIBDIR)/FaceLab_dynamic.obj"

$(LIBDIR)/SWFaceLab_dynamic.obj: ./src/devices/faceLab/SWFaceLab.cpp
        $(CC) -c ./src/devices/faceLab/SWFaceLab.cpp $(CFLAGS_DYN) $(SW_FACELAB) -Fo"$(LIBDIR)/SWFaceLab_dynamic.obj"

$(LIBDIR)/SWKinect_dynamic.obj: ./src/devices/rgbd/SWKinect.cpp
        $(CC) -c ./src/devices/rgbd/SWKinect.cpp $(CFLAGS_DYN) $(SW_KINECT) -Fo"$(LIBDIR)/SWKinect_dynamic.obj"

$(LIBDIR)/SWKinect_thread_dynamic.obj: ./src/devices/rgbd/SWKinect_thread.cpp
        $(CC) -c ./src/devices/rgbd/SWKinect_thread.cpp $(CFLAGS_DYN) $(SW_KINECT_THREAD) -Fo"$(LIBDIR)/SWKinect_thread_dynamic.obj"

$(LIBDIR)/SWFastrak_dynamic.obj: ./src/devices/fastrak/SWFastrak.cpp
        $(CC) -c ./src/devices/fastrak/SWFastrak.cpp $(CFLAGS_DYN) $(SW_FASTRAK) -Fo"$(LIBDIR)/SWFastrak_dynamic.obj"

$(LIBDIR)/SWFastrak_thread_dynamic.obj: ./src/devices/fastrak/SWFastrak_thread.cpp
        $(CC) -c ./src/devices/fastrak/SWFastrak_thread.cpp $(CFLAGS_DYN) $(SW_FASTRAK_THREAD) -Fo"$(LIBDIR)/SWFastrak_thread_dynamic.obj"

$(LIBDIR)/SWSaveKinectData_dynamic.obj: ./src/devices/rgbd/SWSaveKinectData.cpp
        $(CC) -c ./src/devices/rgbd/SWSaveKinectData.cpp $(CFLAGS_DYN) $(SW_SAVE_KINECT_DATA) -Fo"$(LIBDIR)/SWSaveKinectData_dynamic.obj"

$(LIBDIR)/SWLoadKinectData_dynamic.obj: ./src/devices/rgbd/SWLoadKinectData.cpp
        $(CC) -c ./src/devices/rgbd/SWLoadKinectData.cpp $(CFLAGS_DYN) $(SW_LOAD_KINECT_DATA) -Fo"$(LIBDIR)/SWLoadKinectData_dynamic.obj"


##################################################### filters
	
$(LIBDIR)/SWComputeTemporalFilterCoefficients.obj: ./src/filters/SWComputeTemporalFilterCoefficients.cpp
	$(CC) -c ./src/filters/SWComputeTemporalFilterCoefficients.cpp $(CFLAGS_STA) $(SW_CTEMPFILTERCOEFF) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWApplyTemporalFilter.obj: ./src/filters/SWApplyTemporalFilter.cpp
	$(CC) -c ./src/filters/SWApplyTemporalFilter.cpp $(CFLAGS_STA) $(SW_APPLYTEMPFILRER) -Fo"$(LIBDIR)/"


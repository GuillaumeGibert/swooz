# makefile of swooz-avatar project using Visual C++
# Florian Lance
# lyon sept 2013

!IF "$(CFG)" != "Release" &&  "$(CFG)" != "Debug"
!ERROR Invalid CFG "($CFG)
!ENDIF

!IF "$(ARCH)" != "x86" &&  "$(ARCH)" != "amd64"
!ERROR Invalid ARCH "($ARCH)
!ENDIF

CC=cl
CUDA=nvcc 
LINK=link

!INCLUDE <./makefile-include>


############################################################################## OBJ LIST

STASM_LIST_OBJ=\
        $(LIBDIR)/mat.obj $(LIBDIR)/matvec.obj $(LIBDIR)/err.obj $(LIBDIR)/initnet.obj $(LIBDIR)/forward.obj $(LIBDIR)/util.obj $(LIBDIR)/mchol.obj\
        $(LIBDIR)/shapemodel.obj $(LIBDIR)/startshape.obj $(LIBDIR)/imutil.obj $(LIBDIR)/shapefile.obj $(LIBDIR)/landmarks.obj $(LIBDIR)/hypot.obj\
        $(LIBDIR)/prof.obj $(LIBDIR)/initasm.obj $(LIBDIR)/imshape.obj $(LIBDIR)/readconf.obj $(LIBDIR)/readasm.obj $(LIBDIR)/sparsemat.obj\
        $(LIBDIR)/rgbimutil.obj $(LIBDIR)/asmsearch.obj\

CLOUD_LIST_OBJ=\
        $(LIBDIR)/SWCloud.obj $(LIBDIR)/SWMaskCloud.obj $(LIBDIR)/SWAlignClouds.obj \

MESH_LIST_OBJ=\
        $(LIBDIR)/SWMesh.obj $(LIBDIR)/SWOptimalStepNonRigidICP.obj\
	
DETECT_LIST_OBJ=\
        $(LIBDIR)/SWHaarCascade.obj $(LIBDIR)/SWFaceDetection.obj $(LIBDIR)/SWStasm.obj\

EMICP_LIST_OBJ=\
        $(LIBDIR)/emicp.obj $(LIBDIR)/findRTfromS.obj\

GPUMAT_LIST_OBJ=\
        $(LIBDIR)/gpuMat.obj\

TRACK_LIST_OBJ=\
        $(LIBDIR)/SWTrackFlow.obj $(LIBDIR)/SWTrack.obj\
	
QT_INTER_LIST_OBJ=\
    $(LIBDIR)/SWDisplayImageWidget.obj $(LIBDIR)/SWDisplayCurvesWidget.obj\
		
QT_GL_LIST_OBJ=\
        $(LIBDIR)/SWQtCamera.obj $(LIBDIR)/SWGLWidget.obj $(LIBDIR)/SWGLCloudWidget.obj $(LIBDIR)/SWGLMeshWidget.obj\
        $(LIBDIR)/SWGLOptimalStepNonRigidICP.obj\

FACELAB_LIST_OBJ=\
        $(DIST_LIB)/FaceLab_dynamic.obj \

DYNAMIC_LIST_OBJ=\
        $(LIBDIR)/SWCloud_dynamic.obj $(LIBDIR)/SWMaskCloud_dynamic.obj\

	
############################################################################## MOC LIST

MOC_LIST=\
        $(MOCDIR)/moc_SWCreateAvatarInterface.cpp $(MOCDIR)/moc_SWMorphingInterface.cpp $(MOCDIR)/moc_SWDisplayImageWidget.cpp $(MOCDIR)/moc_SWDisplayCurvesWidget.cpp \
        $(MOCDIR)/moc_SWGLWidget.cpp $(MOCDIR)/moc_SWGLCloudWidget.cpp $(MOCDIR)/moc_SWGLMeshWidget.cpp $(MOCDIR)/moc_SWCreateAvatarWorker.cpp\
        $(MOCDIR)/moc_SWMorphingWorker.cpp $(MOCDIR)/moc_SWGLOptimalStepNonRigidICP.cpp\

############################################################################## UI LIST

UI_LIST=\
        $(INCDIR_QTGENW)/SWMainWindow.h $(INCDIR_QTGENW)/SWUI_WMeshCorr.h $(INCDIR_QTGENW)/SWUI_WParamsDisplay.h $(INCDIR_QTGENW)/SWUI_WTargetTransfo.h\
        $(INCDIR_QTGENW)/SWUI_WCreateAvatar.h\

############################################################################## OBJ PROGRAMS

CREATE_AVATAR_OBJ=\
        $(EMICP_LIST_OBJ) $(STASM_LIST_OBJ) $(CLOUD_LIST_OBJ) $(MESH_LIST_OBJ) $(GPUMAT_LIST_OBJ) $(DETECT_LIST_OBJ) $(TRACK_LIST_OBJ) $(CV_INTER_LIST_OBJ) $(QT_INTER_LIST_OBJ) $(QT_GL_LIST_OBJ)\
        $(LIBDIR)/SWCaptureHeadMotion.obj $(LIBDIR)/SWCreateAvatarWorker.obj $(LIBDIR)/SWCreateAvatarInterface.obj $(LIBDIR)/SWCreateAvatar.obj\

MORPHING_OBJ=\
       $(EMICP_LIST_OBJ) $(GPUMAT_LIST_OBJ) $(CLOUD_LIST_OBJ) $(MESH_LIST_OBJ) $(QT_GL_LIST_OBJ)\
       $(LIBDIR)/SWMorphingWorker.obj $(LIBDIR)/SWMorphingInterface.obj \

############################################################################## Makefile commands  $(QT_INTER_LIST_OBJ)

!if  "$(ARCH)" == "x86"
all: createAvatar morphing
!endif

!if "$(ARCH)" == "amd64"
all: morphing64
!endif

createAvatar   : $(UI_LIST) $(MOC_LIST) $(BINDIR)/SWCreateAvatar.exe
		
morphing   : $(UI_LIST) $(MOC_LIST) $(BINDIR)/SWMorphing.exe

morphing64 : $(UI_LIST) $(MOC_LIST) $(BINDIR)/SWMorphing-x64.exe


############################################################################## exe files

$(BINDIR)/SWCreateAvatar.exe: $(CREATE_AVATAR_OBJ) $(LIBS_AVATAR)
        $(LINK) /OUT:$(BINDIR)/SWCreateAvatar.exe $(LFLAGS) $(CREATE_AVATAR_OBJ) $(SETARGV) $(BINMODE) $(LIBS_AVATAR) $(WINLIBS)

$(BINDIR)/SWMorphing.exe: $(MORPHING_OBJ) $(LIBS_MORPHING)
        $(LINK) /OUT:$(BINDIR)/SWMorphing.exe $(LFLAGS_MORPHING) $(MORPHING_OBJ) $(SETARGV) $(BINMODE) $(LIBS_MORPHING) $(WINLIBS)

$(BINDIR)/SWMorphing-x64.exe: $(MORPHING_OBJ) $(LIBS_MORPHING)
        $(LINK) /OUT:$(BINDIR)/SWMorphing-x64.exe $(LFLAGS_MORPHING) $(MORPHING_OBJ) $(SETARGV) $(BINMODE) $(LIBS_MORPHING) $(WINLIBS)

############################################################################## SW Files

$(LIBDIR)/SWCreateAvatar.obj: ./src/SWCreateAvatar.cpp
        $(CC) -c ./src/SWCreateAvatar.cpp $(CFLAGS_STA) $(SW_CREATE_AVATAR) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWLaunchExp.obj: ./src/SWLaunchExp.cpp
        $(CC) -c ./src/SWLaunchExp.cpp $(CFLAGS_DYN) $(SW_LAUNCH_EXP) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWProcessExpData.obj: ./src/SWProcessExpData.cpp
        $(CC) -c ./src/SWProcessExpData.cpp $(CFLAGS_STA) $(SW_PROCESS_EXP_DATA) -Fo"$(LIBDIR)/"

############################################################################## experiences

$(LIBDIR)/SWSaveMultiRigidMotionData.obj: ./src/exp/SWSaveMultiRigidMotionData.cpp
        $(CC) -c ./src/exp/SWSaveMultiRigidMotionData.cpp $(CFLAGS_DYN) $(SW_SAVE_MULTI_RM_DATA) -Fo"$(LIBDIR)/"

############################################################################## interface files
#	Qt
$(LIBDIR)/SWCreateAvatarInterface.obj: ./src/interface/SWCreateAvatarInterface.cpp
        $(CC) -c ./src/interface/SWCreateAvatarInterface.cpp $(CFLAGS_STA) $(SW_CREATEAVATAR_INTERFACE) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWMorphingInterface.obj: ./src/interface/SWMorphingInterface.cpp
        $(CC) -c ./src/interface/SWMorphingInterface.cpp $(CFLAGS_STA) $(SW_MORPH_INTERFACE) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWQtCamera.obj: ./src/interface/SWQtCamera.cpp
        $(CC) -c ./src/interface/SWQtCamera.cpp $(CFLAGS_STA) $(SW_QT_CAMERA) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWDisplayImageWidget.obj: $(SRCDIR_QTW)/SWDisplayImageWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWDisplayImageWidget.cpp $(CFLAGS_STA) $(SW_DISPLAY_IMAGEW) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWDisplayCurvesWidget.obj: $(SRCDIR_QTW)/SWDisplayCurvesWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWDisplayCurvesWidget.cpp $(CFLAGS_STA) $(SW_DISPLAY_CURVES) -Fo"$(LIBDIR)/"

#           Opengl
$(LIBDIR)/SWGLWidget.obj: $(SRCDIR_QTW)/SWGLWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLWidget.cpp $(CFLAGS_STA) $(SW_GL_WIDGET) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWGLCloudWidget.obj: $(SRCDIR_QTW)/SWGLCloudWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLCloudWidget.cpp $(CFLAGS_STA) $(SW_GL_CLOUD_WIDGET) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWGLMeshWidget.obj: $(SRCDIR_QTW)/SWGLMeshWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLMeshWidget.cpp $(CFLAGS_STA) $(SW_GL_MESH_WIDGET) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWGLOptimalStepNonRigidICP.obj: $(SRCDIR_QTW)/SWGLOptimalStepNonRigidICP.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLOptimalStepNonRigidICP.cpp $(CFLAGS_STA) $(SW_GL_OSNRI_WIDGET) -Fo"$(LIBDIR)/"

#           Workers
$(LIBDIR)/SWCreateAvatarWorker.obj: $(SRCDIR_QTWORKERS)/SWCreateAvatarWorker.cpp
        $(CC) -c $(SRCDIR_QTWORKERS)/SWCreateAvatarWorker.cpp $(CFLAGS_STA) $(SW_CREATEAVATAR_WORKER) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWMorphingWorker.obj: $(SRCDIR_QTWORKERS)/SWMorphingWorker.cpp
        $(CC) -c $(SRCDIR_QTWORKERS)/SWMorphingWorker.cpp $(CFLAGS_STA) $(SW_MORPHING_WORKER) -Fo"$(LIBDIR)/"

############################################################################## cloud files

####### static

$(LIBDIR)/SWCloud.obj: ./src/cloud/SWCloud.cpp
        $(CC) -c ./src/cloud/SWCloud.cpp $(CFLAGS_STA) $(SW_CLOUD) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWMaskCloud.obj: ./src/cloud/SWMaskCloud.cpp
        $(CC) -c ./src/cloud/SWMaskCloud.cpp $(CFLAGS_STA) $(SW_CLOUD) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWAlignClouds.obj: ./src/cloud/SWAlignClouds.cpp
        $(CC) -c ./src/cloud/SWAlignClouds.cpp $(CFLAGS_STA) $(SW_ALIGN_CLOUDS) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWCaptureHeadMotion.obj: ./src/cloud/SWCaptureHeadMotion.cpp
        $(CC) -c ./src/cloud/SWCaptureHeadMotion.cpp $(CFLAGS_STA) $(SW_CAPTURE_HEAD_M)  -Fo"$(LIBDIR)/"
	
####### dynamic

$(LIBDIR)/SWCloud_dynamic.obj: ./src/cloud/SWCloud.cpp
        $(CC) -c ./src/cloud/SWCloud.cpp $(CFLAGS_DYN) $(SW_CLOUD) -Fo"$(LIBDIR)/SWCloud_dynamic.obj"
	
$(LIBDIR)/SWMaskCloud_dynamic.obj: ./src/cloud/SWMaskCloud.cpp
        $(CC) -c ./src/cloud/SWMaskCloud.cpp $(CFLAGS_DYN) $(SW_CLOUD) -Fo"$(LIBDIR)/SWMaskCloud_dynamic.obj"

############################################################################## mesh files

$(LIBDIR)/SWMesh.obj: ./src/mesh/SWMesh.cpp
        $(CC) -c ./src/mesh/SWMesh.cpp $(CFLAGS_STA) $(SW_MESH) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWOptimalStepNonRigidICP.obj: ./src/mesh/SWOptimalStepNonRigidICP.cpp
        $(CC) -c ./src/mesh/SWOptimalStepNonRigidICP.cpp $(CFLAGS_STA) $(SW_OSNRICP) -Fo"$(LIBDIR)/"

############################################################################## track files

$(LIBDIR)/SWTrack.obj: ./src/track/SWTrack.cpp
        $(CC) -c ./src/track/SWTrack.cpp $(CFLAGS_STA) $(SW_TRACK) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWTrackFlow.obj: ./src/track/SWTrackFlow.cpp
        $(CC) -c ./src/track/SWTrackFlow.cpp $(CFLAGS_STA) $(SW_TRACK_FLOW) -Fo"$(LIBDIR)/"

############################################################################## detect files

$(LIBDIR)/SWHaarCascade.obj: ./src/detect/SWHaarCascade.cpp
        $(CC) -c ./src/detect/SWHaarCascade.cpp $(CFLAGS_STA) $(SW_HAAR_CASCADE) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWFaceDetection.obj: ./src/detect/SWFaceDetection.cpp
        $(CC) -c ./src/detect/SWFaceDetection.cpp $(CFLAGS_STA) $(SW_FACE_DETECTION) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWStasm.obj: ./src/detect/SWStasm.cpp
        $(CC) -c ./src/detect/SWStasm.cpp $(CFLAGS_STA) $(SW_STASM_DETECTION) -Fo"$(LIBDIR)/"

############################################################################## stasm files

$(LIBDIR)/mat.obj: ./src/stasm/mat.cpp
        $(CC) -c ./src/stasm/mat.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/err.obj: ./src/stasm/err.cpp
        $(CC) -c ./src/stasm/err.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/util.obj: ./src/stasm/util.cpp
        $(CC) -c ./src/stasm/util.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/mchol.obj: ./src/stasm/mchol.cpp
        $(CC) -c ./src/stasm/mchol.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/startshape.obj: ./src/stasm/startshape.cpp
        $(CC) -c ./src/stasm/startshape.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/initnet.obj: ./src/stasm/initnet.cpp
        $(CC) -c ./src/stasm/initnet.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/forward.obj: ./src/stasm/forward.cpp
        $(CC) -c ./src/stasm/forward.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/shapemodel.obj: ./src/stasm/shapemodel.cpp
        $(CC) -c ./src/stasm/shapemodel.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/matvec.obj: ./src/stasm/matvec.cpp
        $(CC) -c ./src/stasm/matvec.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/imutil.obj: ./src/stasm/imutil.cpp
        $(CC) -c ./src/stasm/imutil.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/shapefile.obj: ./src/stasm/shapefile.cpp
        $(CC) -c ./src/stasm/shapefile.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/landmarks.obj: ./src/stasm/landmarks.cpp
        $(CC) -c ./src/stasm/landmarks.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/hypot.obj: ./src/stasm/hypot.c
        $(CC) -c ./src/stasm/hypot.c $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/asmsearch.obj: ./src/stasm/asmsearch.cpp
        $(CC) -c ./src/stasm/asmsearch.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/prof.obj: ./src/stasm/prof.cpp
        $(CC) -c ./src/stasm/prof.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/initasm.obj: ./src/stasm/initasm.cpp
        $(CC) -c ./src/stasm/initasm.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/rgbimutil.obj: ./src/stasm/rgbimutil.cpp
        $(CC) -c ./src/stasm/rgbimutil.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/imshape.obj: ./src/stasm/imshape.cpp
        $(CC) -c ./src/stasm/imshape.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/readconf.obj: ./src/stasm/readconf.cpp
        $(CC) -c ./src/stasm/readconf.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/readasm.obj: ./src/stasm/readasm.cpp
        $(CC) -c ./src/stasm/readasm.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

$(LIBDIR)/sparsemat.obj: ./src/stasm/sparsemat.cpp
        $(CC) -c ./src/stasm/sparsemat.cpp $(CFLAGS_STA) $(STASM) -Fo"$(LIBDIR)/"

############################################################################## Qt ui files

$(INCDIR_QTGENW)/SWMainWindow.h: $(FORMDIR)/SWMainWindow.ui  
        uic $(FORMDIR)/SWMainWindow.ui -o $@

$(INCDIR_QTGENW)/SWUI_WMeshCorr.h: $(FORMDIR)/SWUI_WMeshCorr.ui
        uic $(FORMDIR)/SWUI_WMeshCorr.ui -o $@

$(INCDIR_QTGENW)/SWUI_WParamsDisplay.h: $(FORMDIR)/SWUI_WParamsDisplay.ui
        uic $(FORMDIR)/SWUI_WParamsDisplay.ui -o $@

$(INCDIR_QTGENW)/SWUI_WTargetTransfo.h: $(FORMDIR)/SWUI_WTargetTransfo.ui
        uic $(FORMDIR)/SWUI_WTargetTransfo.ui -o $@

$(INCDIR_QTGENW)/SWUI_WCreateAvatar.h: $(FORMDIR)/SWUI_CreateAvatar.ui
        uic $(FORMDIR)/SWUI_CreateAvatar.ui -o $@

############################################################################## Qt moc files

$(MOCDIR)/moc_SWTestCode.cpp: ./src/SWTestCode.cpp
         moc  ./src/SWTestCode.cpp -o $@

$(MOCDIR)/moc_SWMorphingInterface.cpp: $(INCDIR_INTERFACE)/SWMorphingInterface.h
         moc  $(INCDIR_INTERFACE)/SWMorphingInterface.h -o $@

$(MOCDIR)/moc_SWCreateAvatarInterface.cpp: $(INCDIR_INTERFACE)/SWCreateAvatarInterface.h
         moc  $(INCDIR_INTERFACE)/SWCreateAvatarInterface.h -o $@

$(MOCDIR)/moc_SWDisplayImageWidget.cpp: $(INCDIR_QTW)/SWDisplayImageWidget.h
         moc  $(INCDIR_QTW)/SWDisplayImageWidget.h -o $@

$(MOCDIR)/moc_SWDisplayCurvesWidget.cpp: $(INCDIR_QTW)/SWDisplayCurvesWidget.h
         moc  $(INCDIR_QTW)/SWDisplayCurvesWidget.h -o $@

$(MOCDIR)/moc_SWGLWidget.cpp: $(INCDIR_QTW)/SWGLWidget.h
         moc  $(INCDIR_QTW)/SWGLWidget.h -o $@

$(MOCDIR)/moc_SWGLCloudWidget.cpp: $(INCDIR_QTW)/SWGLCloudWidget.h
         moc  $(INCDIR_QTW)/SWGLCloudWidget.h -o $@

$(MOCDIR)/moc_SWGLMeshWidget.cpp: $(INCDIR_QTW)/SWGLMeshWidget.h
         moc  $(INCDIR_QTW)/SWGLMeshWidget.h -o $@

$(MOCDIR)/moc_SWGLOptimalStepNonRigidICP.cpp: $(INCDIR_QTW)/SWGLOptimalStepNonRigidICP.h
         moc  $(INCDIR_QTW)/SWGLOptimalStepNonRigidICP.h -o $@

#   Workers

$(MOCDIR)/moc_SWCreateAvatarWorker.cpp: $(INCDIR_QTWORKERS)/SWCreateAvatarWorker.h
         moc  $(INCDIR_QTWORKERS)/SWCreateAvatarWorker.h -o $@

$(MOCDIR)/moc_SWMorphingWorker.cpp: $(INCDIR_QTWORKERS)/SWMorphingWorker.h
         moc  $(INCDIR_QTWORKERS)/SWMorphingWorker.h -o $@

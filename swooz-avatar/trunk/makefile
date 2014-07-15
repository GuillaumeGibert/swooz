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
        $(LIBDIR)/rgbimutil.obj $(LIBDIR)/asmsearch.obj $(LIBDIR)/SWStasm.obj\

SWOOZ_LIST_OBJ=\
        $(LIBDIR)/SWCloud.obj $(LIBDIR)/SWMaskCloud.obj $(LIBDIR)/SWAlignClouds.obj\
        $(LIBDIR)/SWMesh.obj $(LIBDIR)/SWOptimalStepNonRigidICP.obj\
        $(LIBDIR)/SWHaarCascade.obj $(LIBDIR)/SWFaceDetection.obj $(LIBDIR)/SWFaceDetection_thread.obj\
        $(LIBDIR)/emicp.obj $(LIBDIR)/findRTfromS.obj $(LIBDIR)/gpuMat.obj $(LIBDIR)/SWTrackFlow.obj $(LIBDIR)/SWTrack.obj\
        $(LIBDIR)/SWDisplayImageWidget.obj $(LIBDIR)/SWDisplayCurvesWidget.obj\
        $(LIBDIR)/SWQtCamera.obj $(LIBDIR)/SWGLWidget.obj $(LIBDIR)/SWGLCloudWidget.obj $(LIBDIR)/SWGLMeshWidget.obj $(LIBDIR)/SWGLMultiObjectWidget.obj\
        $(LIBDIR)/SWGLOptimalStepNonRigidICP.obj $(LIBDIR)/SWCaptureHeadMotion.obj\
        $(LIBDIR)/SWCreateAvatarWorker.obj $(LIBDIR)/SWCreateAvatar.obj $(LIBDIR)/SWCreateAvatarInterface.obj\
        $(LIBDIR)/SWMorphingWorker.obj $(LIBDIR)/SWMorphingInterface.obj\

STASM_DYN_LIST_OBJ=\
        $(LIBDIR)/mat_d.obj $(LIBDIR)/matvec_d.obj $(LIBDIR)/err_d.obj $(LIBDIR)/initnet_d.obj $(LIBDIR)/forward_d.obj $(LIBDIR)/util_d.obj $(LIBDIR)/mchol_d.obj\
        $(LIBDIR)/shapemodel_d.obj $(LIBDIR)/startshape_d.obj $(LIBDIR)/imutil_d.obj $(LIBDIR)/shapefile_d.obj $(LIBDIR)/landmarks_d.obj $(LIBDIR)/hypot_d.obj\
        $(LIBDIR)/prof_d.obj $(LIBDIR)/initasm_d.obj $(LIBDIR)/imshape_d.obj $(LIBDIR)/readconf_d.obj $(LIBDIR)/readasm_d.obj $(LIBDIR)/sparsemat_d.obj\
        $(LIBDIR)/rgbimutil_d.obj $(LIBDIR)/asmsearch_d.obj $(LIBDIR)/SWStasm_d.obj\

SWOOZ_DYN_LIST_OBJ=\
        $(LIBDIR)/SWCloud_d.obj $(LIBDIR)/SWMaskCloud_d.obj $(LIBDIR)/SWAlignClouds_d.obj\
        $(LIBDIR)/SWMesh_d.obj $(LIBDIR)/SWOptimalStepNonRigidICP_d.obj\
        $(LIBDIR)/SWHaarCascade_d.obj $(LIBDIR)/SWFaceDetection_d.obj $(LIBDIR)/SWFaceDetection_thread_d.obj\
        $(LIBDIR)/emicp.obj $(LIBDIR)/findRTfromS.obj $(LIBDIR)/gpuMat.obj $(LIBDIR)/SWTrackFlow_d.obj $(LIBDIR)/SWTrack_d.obj\
        $(LIBDIR)/SWDisplayImageWidget_d.obj $(LIBDIR)/SWDisplayCurvesWidget_d.obj\
        $(LIBDIR)/SWQtCamera_d.obj $(LIBDIR)/SWGLWidget_d.obj $(LIBDIR)/SWGLCloudWidget_d.obj $(LIBDIR)/SWGLMeshWidget_d.obj $(LIBDIR)/SWGLMultiObjectWidget_d.obj\
        $(LIBDIR)/SWGLOptimalStepNonRigidICP_d.obj $(LIBDIR)/SWCaptureHeadMotion_d.obj\
        $(LIBDIR)/SWCreateAvatarWorker_d.obj $(LIBDIR)/SWCreateAvatar_d.obj $(LIBDIR)/SWCreateAvatarInterface_d.obj\
        $(LIBDIR)/SWMorphingWorker_d.obj $(LIBDIR)/SWMorphingInterface_d.obj\

# For compiling files before the linking
COMPIL_LIST=\
        $(STASM_LIST_OBJ) $(SWOOZ_LIST_OBJ) $(STASM_DYN_LIST_OBJ) $(SWOOZ_DYN_LIST_OBJ)\

COMPIL_64_LIST=\
        $(SWOOZ_LIST_OBJ) $(SWOOZ_DYN_LIST_OBJ)\

# For linking the avatar creation application
AVATAR_LINK_OBJ=\
        $(STASM_LIST_OBJ) $(LIBDIR)/SWCloud.obj $(LIBDIR)/SWMaskCloud.obj $(LIBDIR)/SWAlignClouds.obj $(LIBDIR)/SWMesh.obj\
        $(LIBDIR)/SWHaarCascade.obj $(LIBDIR)/SWFaceDetection.obj $(LIBDIR)/SWFaceDetection_thread.obj $(LIBDIR)/emicp.obj $(LIBDIR)/findRTfromS.obj\
        $(LIBDIR)/SWDisplayImageWidget.obj $(LIBDIR)/SWDisplayCurvesWidget.obj\
        $(LIBDIR)/SWQtCamera.obj $(LIBDIR)/SWGLWidget.obj $(LIBDIR)/SWGLCloudWidget.obj $(LIBDIR)/SWGLMeshWidget.obj\
        $(LIBDIR)/SWCaptureHeadMotion.obj $(LIBDIR)/SWCreateAvatarWorker.obj $(LIBDIR)/SWCreateAvatar.obj $(LIBDIR)/SWCreateAvatarInterface.obj\

AVATAR_LINK_D_OBJ=\
        $(STASM_DYN_LIST_OBJ) $(LIBDIR)/SWCloud_d.obj $(LIBDIR)/SWMaskCloud_d.obj $(LIBDIR)/SWAlignClouds_d.obj $(LIBDIR)/SWMesh_d.obj\
        $(LIBDIR)/SWHaarCascade_d.obj $(LIBDIR)/SWFaceDetection_d.obj $(LIBDIR)/SWFaceDetection_thread_d.obj $(LIBDIR)/emicp.obj $(LIBDIR)/findRTfromS.obj\
        $(LIBDIR)/SWDisplayImageWidget_d.obj $(LIBDIR)/SWDisplayCurvesWidget_d.obj\
        $(LIBDIR)/SWQtCamera_d.obj $(LIBDIR)/SWGLWidget_d.obj $(LIBDIR)/SWGLCloudWidget_d.obj $(LIBDIR)/SWGLMeshWidget_d.obj\
        $(LIBDIR)/SWCaptureHeadMotion_d.obj $(LIBDIR)/SWCreateAvatarWorker_d.obj $(LIBDIR)/SWCreateAvatar_d.obj $(LIBDIR)/SWCreateAvatarInterface_d.obj\

# For linking the morphing application
MORPHING_LINK_OBJ=\
        $(LIBDIR)/SWCloud.obj $(LIBDIR)/SWAlignClouds.obj $(LIBDIR)/SWMesh.obj $(LIBDIR)/SWOptimalStepNonRigidICP.obj\
        $(LIBDIR)/emicp.obj $(LIBDIR)/findRTfromS.obj $(LIBDIR)/gpuMat.obj $(LIBDIR)/SWDisplayImageWidget.obj\
        $(LIBDIR)/SWQtCamera.obj $(LIBDIR)/SWGLWidget.obj $(LIBDIR)/SWGLCloudWidget.obj $(LIBDIR)/SWGLMeshWidget.obj $(LIBDIR)/SWGLMultiObjectWidget.obj\
        $(LIBDIR)/SWGLOptimalStepNonRigidICP.obj\
        $(LIBDIR)/SWMorphingWorker.obj $(LIBDIR)/SWMorphingInterface.obj\

MORPHING_LINK_D_OBJ=\
        $(LIBDIR)/SWCloud_d.obj $(LIBDIR)/SWAlignClouds_d.obj $(LIBDIR)/SWMesh_d.obj $(LIBDIR)/SWOptimalStepNonRigidICP_d.obj\
        $(LIBDIR)/emicp.obj $(LIBDIR)/findRTfromS.obj $(LIBDIR)/gpuMat.obj $(LIBDIR)/SWDisplayImageWidget_d.obj\
        $(LIBDIR)/SWQtCamera_d.obj $(LIBDIR)/SWGLWidget_d.obj $(LIBDIR)/SWGLCloudWidget_d.obj $(LIBDIR)/SWGLMeshWidget_d.obj $(LIBDIR)/SWGLMultiObjectWidget_d.obj\
        $(LIBDIR)/SWGLOptimalStepNonRigidICP_d.obj\
        $(LIBDIR)/SWMorphingWorker_d.obj $(LIBDIR)/SWMorphingInterface_d.obj\

# For generating SWAvatar.lib
AVATAR_GEN_LIB_OBJ=\
        $(LIBDIR)/SWCloud.obj $(LIBDIR)/SWMaskCloud.obj $(LIBDIR)/SWAlignClouds.obj $(LIBDIR)/SWMesh.obj\
        $(LIBDIR)/SWHaarCascade.obj $(LIBDIR)/SWFaceDetection.obj $(LIBDIR)/SWFaceDetection_thread.obj $(LIBDIR)/emicp.obj $(LIBDIR)/findRTfromS.obj\
        $(LIBDIR)/gpuMat.obj $(LIBDIR)/SWTrackFlow.obj $(LIBDIR)/SWTrack.obj\

# For generating SWAvatar_d.lib
AVATAR_GEN_DYN_LIB_OBJ=\
        $(LIBDIR)/SWCloud_d.obj $(LIBDIR)/SWMaskCloud_d.obj $(LIBDIR)/SWAlignClouds_d.obj $(LIBDIR)/SWMesh_d.obj\
        $(LIBDIR)/SWHaarCascade_d.obj $(LIBDIR)/SWFaceDetection_d.obj $(LIBDIR)/SWFaceDetection_thread_d.obj $(LIBDIR)/emicp.obj $(LIBDIR)/findRTfromS.obj\
        $(LIBDIR)/gpuMat.obj $(LIBDIR)/SWTrackFlow_d.obj $(LIBDIR)/SWTrack_d.obj\

############################################################################## MOC LIST

# Qt Moc files to be generated
MOC_LIST=\
        $(MOCDIR)/moc_SWCreateAvatarInterface.cpp $(MOCDIR)/moc_SWMorphingInterface.cpp $(MOCDIR)/moc_SWDisplayImageWidget.cpp $(MOCDIR)/moc_SWDisplayCurvesWidget.cpp \
        $(MOCDIR)/moc_SWGLWidget.cpp $(MOCDIR)/moc_SWGLCloudWidget.cpp $(MOCDIR)/moc_SWGLMeshWidget.cpp $(MOCDIR)/moc_SWGLMultiObjectWidget.cpp $(MOCDIR)/moc_SWCreateAvatarWorker.cpp\
        $(MOCDIR)/moc_SWMorphingWorker.cpp $(MOCDIR)/moc_SWGLOptimalStepNonRigidICP.cpp\

############################################################################## UI LIST

# Qt UI header files to be generated
UI_LIST=\
        $(INCDIR_QTGENW)/SWUI_Morphing.h $(INCDIR_QTGENW)/SWUI_WCreateAvatar.h\


############################################################################## Makefile commands

# Files to be generated by the x86 compilation mode
!if  "$(ARCH)" == "x86"
all: $(UI_LIST) $(MOC_LIST) $(COMPIL_LIST) $(BINDIR)/SWCreateAvatar.exe $(BINDIR)/SWMorphing.exe $(LIBDIR)/SWAvatar.lib $(LIBDIR)/SWAvatar_d.lib
!endif

# Files to be generated by the amd64 compilation mode
!if "$(ARCH)" == "amd64"
all: $(UI_LIST) $(MOC_LIST) $(COMPIL_64_LIST) $(BINDIR)/SWMorphing-x64.exe $(LIBDIR)/SWAvatar_d.lib
!endif

############################################################################## lib files

# Lib files to be generated
$(LIBDIR)/SWAvatar.lib: $(AVATAR_GEN_LIB_OBJ)
        LIB.EXE /OUT:$(LIBDIR)/SWAvatar.lib $(AVATAR_GEN_LIB_OBJ)

$(LIBDIR)/SWAvatar_d.lib: $(AVATAR_GEN_DYN_LIB_OBJ)
        LIB.EXE /OUT:$(LIBDIR)/SWAvatar_d.lib $(AVATAR_GEN_DYN_LIB_OBJ)

############################################################################## exe files

WIN_CONFIG = $(SETARGV) $(BINMODE) $(WINLIBS)

# exec files to be generated
$(BINDIR)/SWCreateAvatar.exe: $(AVATAR_LINK_D_OBJ) $(LIBS_AVATAR)
        $(LINK) /OUT:$(BINDIR)/SWCreateAvatar.exe $(LFLAGS_AVATAR) $(AVATAR_LINK_D_OBJ) $(LIBS_AVATAR) $(WIN_CONFIG)

$(BINDIR)/SWMorphing.exe: $(MORPHING_LINK_OBJ) $(LIBS_MORPHING)
        $(LINK) /OUT:$(BINDIR)/SWMorphing.exe $(LFLAGS_MORPHING) $(MORPHING_LINK_OBJ) $(LIBS_MORPHING) $(WIN_CONFIG)

$(BINDIR)/SWMorphing-x64.exe: $(MORPHING_LINK_OBJ) $(LIBS_MORPHING)
        $(LINK) /OUT:$(BINDIR)/SWMorphing-x64.exe $(LFLAGS_MORPHING) $(MORPHING_LINK_OBJ) $(LIBS_MORPHING) $(WIN_CONFIG)

############################################################################## SW Files

################################## static

#           Cloud
$(LIBDIR)/SWCloud.obj: ./src/cloud/SWCloud.cpp
        $(CC) -c ./src/cloud/SWCloud.cpp $(CFLAGS_STA) $(SW_CLOUD) -Fo"$(LIBDIR)/"
	
$(LIBDIR)/SWMaskCloud.obj: ./src/cloud/SWMaskCloud.cpp
        $(CC) -c ./src/cloud/SWMaskCloud.cpp $(CFLAGS_STA) $(SW_CLOUD) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWAlignClouds.obj: ./src/cloud/SWAlignClouds.cpp
        $(CC) -c ./src/cloud/SWAlignClouds.cpp $(CFLAGS_STA) $(SW_ALIGN_CLOUDS) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWCaptureHeadMotion.obj: ./src/cloud/SWCaptureHeadMotion.cpp
        $(CC) -c ./src/cloud/SWCaptureHeadMotion.cpp $(CFLAGS_STA) $(SW_CAPTURE_HEAD_M)  -Fo"$(LIBDIR)/"

#           Mesh
$(LIBDIR)/SWMesh.obj: ./src/mesh/SWMesh.cpp
        $(CC) -c ./src/mesh/SWMesh.cpp $(CFLAGS_STA) $(SW_MESH) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWOptimalStepNonRigidICP.obj: ./src/mesh/SWOptimalStepNonRigidICP.cpp
        $(CC) -c ./src/mesh/SWOptimalStepNonRigidICP.cpp $(CFLAGS_STA) $(SW_OSNRICP) -Fo"$(LIBDIR)/"

#           Workers
$(LIBDIR)/SWCreateAvatarWorker.obj: $(SRCDIR_QTWORKERS)/SWCreateAvatarWorker.cpp
        $(CC) -c $(SRCDIR_QTWORKERS)/SWCreateAvatarWorker.cpp $(CFLAGS_STA) $(SW_CREATEAVATAR_WORKER) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWMorphingWorker.obj: $(SRCDIR_QTWORKERS)/SWMorphingWorker.cpp
        $(CC) -c $(SRCDIR_QTWORKERS)/SWMorphingWorker.cpp $(CFLAGS_STA) $(SW_MORPHING_WORKER) -Fo"$(LIBDIR)/"

#           Detect
$(LIBDIR)/SWHaarCascade.obj: ./src/detect/SWHaarCascade.cpp
        $(CC) -c ./src/detect/SWHaarCascade.cpp $(CFLAGS_STA) $(SW_HAAR_CASCADE) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWFaceDetection.obj: ./src/detect/SWFaceDetection.cpp
        $(CC) -c ./src/detect/SWFaceDetection.cpp $(CFLAGS_STA) $(SW_FACE_DETECTION) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWFaceDetection_thread.obj: ./src/detect/SWFaceDetection_thread.cpp
        $(CC) -c ./src/detect/SWFaceDetection_thread.cpp $(CFLAGS_STA) $(SW_FACE_DETECTION_THREAD) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWStasm.obj: ./src/detect/SWStasm.cpp
        $(CC) -c ./src/detect/SWStasm.cpp $(CFLAGS_STA) $(SW_STASM_DETECTION) -Fo"$(LIBDIR)/"

#           Tracking
$(LIBDIR)/SWTrack.obj: ./src/track/SWTrack.cpp
        $(CC) -c ./src/track/SWTrack.cpp $(CFLAGS_STA) $(SW_TRACK) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWTrackFlow.obj: ./src/track/SWTrackFlow.cpp
        $(CC) -c ./src/track/SWTrackFlow.cpp $(CFLAGS_STA) $(SW_TRACK_FLOW) -Fo"$(LIBDIR)/"

#           OpenGL
$(LIBDIR)/SWGLWidget.obj: $(SRCDIR_QTW)/SWGLWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLWidget.cpp $(CFLAGS_STA) $(SW_GL_WIDGET) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWGLCloudWidget.obj: $(SRCDIR_QTW)/SWGLCloudWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLCloudWidget.cpp $(CFLAGS_STA) $(SW_GL_CLOUD_WIDGET) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWGLMeshWidget.obj: $(SRCDIR_QTW)/SWGLMeshWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLMeshWidget.cpp $(CFLAGS_STA) $(SW_GL_MESH_WIDGET) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWGLMultiObjectWidget.obj: $(SRCDIR_QTW)/SWGLMultiObjectWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLMultiObjectWidget.cpp $(CFLAGS_STA) $(SW_GL_MULTI_WIDGET) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWGLOptimalStepNonRigidICP.obj: $(SRCDIR_QTW)/SWGLOptimalStepNonRigidICP.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLOptimalStepNonRigidICP.cpp $(CFLAGS_STA) $(SW_GL_OSNRI_WIDGET) -Fo"$(LIBDIR)/"

#           interface
$(LIBDIR)/SWCreateAvatarInterface.obj: ./src/interface/SWCreateAvatarInterface.cpp
        $(CC) -c ./src/interface/SWCreateAvatarInterface.cpp $(CFLAGS_STA) $(SW_AVATAR_INTERFACE) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWMorphingInterface.obj: ./src/interface/SWMorphingInterface.cpp
        $(CC) -c ./src/interface/SWMorphingInterface.cpp $(CFLAGS_STA) $(SW_MORPH_INTERFACE) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWQtCamera.obj: ./src/interface/SWQtCamera.cpp
        $(CC) -c ./src/interface/SWQtCamera.cpp $(CFLAGS_STA) $(SW_QT_CAMERA) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWDisplayImageWidget.obj: $(SRCDIR_QTW)/SWDisplayImageWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWDisplayImageWidget.cpp $(CFLAGS_STA) $(SW_DISPLAY_IMAGEW) -Fo"$(LIBDIR)/"

$(LIBDIR)/SWDisplayCurvesWidget.obj: $(SRCDIR_QTW)/SWDisplayCurvesWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWDisplayCurvesWidget.cpp $(CFLAGS_STA) $(SW_DISPLAY_CURVES) -Fo"$(LIBDIR)/"

#           avatar
$(LIBDIR)/SWCreateAvatar.obj: ./src/SWCreateAvatar.cpp
        $(CC) -c ./src/SWCreateAvatar.cpp $(CFLAGS_STA) $(SW_CREATE_AVATAR) -Fo"$(LIBDIR)/"

################################## dynamic

#           Cloud
$(LIBDIR)/SWCloud_d.obj: ./src/cloud/SWCloud.cpp
        $(CC) -c ./src/cloud/SWCloud.cpp $(CFLAGS_DYN) $(SW_CLOUD) -Fo"$(LIBDIR)/SWCloud_d.obj"
	
$(LIBDIR)/SWMaskCloud_d.obj: ./src/cloud/SWMaskCloud.cpp
        $(CC) -c ./src/cloud/SWMaskCloud.cpp $(CFLAGS_DYN) $(SW_CLOUD) -Fo"$(LIBDIR)/SWMaskCloud_d.obj"

$(LIBDIR)/SWAlignClouds_d.obj: ./src/cloud/SWAlignClouds.cpp
        $(CC) -c ./src/cloud/SWAlignClouds.cpp $(CFLAGS_DYN) $(SW_ALIGN_CLOUDS) -Fo"$(LIBDIR)/SWAlignClouds_d.obj"

$(LIBDIR)/SWCaptureHeadMotion_d.obj: ./src/cloud/SWCaptureHeadMotion.cpp
        $(CC) -c ./src/cloud/SWCaptureHeadMotion.cpp $(CFLAGS_DYN) $(SW_CAPTURE_HEAD_M) -Fo"$(LIBDIR)/SWCaptureHeadMotion_d.obj"

#           Mesh
$(LIBDIR)/SWMesh_d.obj: ./src/mesh/SWMesh.cpp
        $(CC) -c ./src/mesh/SWMesh.cpp $(CFLAGS_DYN) $(SW_MESH) -Fo"$(LIBDIR)/SWMesh_d.obj"

$(LIBDIR)/SWOptimalStepNonRigidICP_d.obj: ./src/mesh/SWOptimalStepNonRigidICP.cpp
        $(CC) -c ./src/mesh/SWOptimalStepNonRigidICP.cpp $(CFLAGS_DYN) $(SW_OSNRICP) -Fo"$(LIBDIR)/SWOptimalStepNonRigidICP_d.obj"

#           Workers
$(LIBDIR)/SWCreateAvatarWorker_d.obj: $(SRCDIR_QTWORKERS)/SWCreateAvatarWorker.cpp
        $(CC) -c $(SRCDIR_QTWORKERS)/SWCreateAvatarWorker.cpp $(CFLAGS_DYN) $(SW_CREATEAVATAR_WORKER) -Fo"$(LIBDIR)/SWCreateAvatarWorker_d.obj"

$(LIBDIR)/SWMorphingWorker_d.obj: $(SRCDIR_QTWORKERS)/SWMorphingWorker.cpp
        $(CC) -c $(SRCDIR_QTWORKERS)/SWMorphingWorker.cpp $(CFLAGS_DYN) $(SW_MORPHING_WORKER) -Fo"$(LIBDIR)/SWMorphingWorker_d.obj"

#           Detect
$(LIBDIR)/SWHaarCascade_d.obj: ./src/detect/SWHaarCascade.cpp
        $(CC) -c ./src/detect/SWHaarCascade.cpp $(CFLAGS_DYN) $(SW_HAAR_CASCADE) -Fo"$(LIBDIR)/SWHaarCascade_d.obj"

$(LIBDIR)/SWFaceDetection_d.obj: ./src/detect/SWFaceDetection.cpp
        $(CC) -c ./src/detect/SWFaceDetection.cpp $(CFLAGS_DYN) $(SW_FACE_DETECTION) -Fo"$(LIBDIR)/SWFaceDetection_d.obj"

$(LIBDIR)/SWFaceDetection_thread_d.obj: ./src/detect/SWFaceDetection_thread.cpp
        $(CC) -c ./src/detect/SWFaceDetection_thread.cpp $(CFLAGS_DYN) $(SW_FACE_DETECTION_THREAD) -Fo"$(LIBDIR)/SWFaceDetection_thread_d.obj"

$(LIBDIR)/SWStasm_d.obj: ./src/detect/SWStasm.cpp
        $(CC) -c ./src/detect/SWStasm.cpp $(CFLAGS_DYN) $(SW_STASM_DETECTION) -Fo"$(LIBDIR)/SWStasm_d.obj"

#           Tracking
$(LIBDIR)/SWTrack_d.obj: ./src/track/SWTrack.cpp
        $(CC) -c ./src/track/SWTrack.cpp $(CFLAGS_DYN) $(SW_TRACK) -Fo"$(LIBDIR)/SWTrack_d.obj"

$(LIBDIR)/SWTrackFlow_d.obj: ./src/track/SWTrackFlow.cpp
        $(CC) -c ./src/track/SWTrackFlow.cpp $(CFLAGS_DYN) $(SW_TRACK_FLOW) -Fo"$(LIBDIR)/SWTrackFlow_d.obj"

#           OpenGL
$(LIBDIR)/SWGLWidget_d.obj: $(SRCDIR_QTW)/SWGLWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLWidget.cpp $(CFLAGS_DYN) $(SW_GL_WIDGET) -Fo"$(LIBDIR)/SWGLWidget_d.obj"

$(LIBDIR)/SWGLCloudWidget_d.obj: $(SRCDIR_QTW)/SWGLCloudWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLCloudWidget.cpp $(CFLAGS_DYN) $(SW_GL_CLOUD_WIDGET) -Fo"$(LIBDIR)/SWGLCloudWidget_d.obj"

$(LIBDIR)/SWGLMeshWidget_d.obj: $(SRCDIR_QTW)/SWGLMeshWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLMeshWidget.cpp $(CFLAGS_DYN) $(SW_GL_MESH_WIDGET) -Fo"$(LIBDIR)/SWGLMeshWidget_d.obj"

$(LIBDIR)/SWGLMultiObjectWidget_d.obj: $(SRCDIR_QTW)/SWGLMultiObjectWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLMultiObjectWidget.cpp $(CFLAGS_DYN) $(SW_GL_MULTI_WIDGET) -Fo"$(LIBDIR)/SWGLMultiObjectWidget_d.obj"

$(LIBDIR)/SWGLOptimalStepNonRigidICP_d.obj: $(SRCDIR_QTW)/SWGLOptimalStepNonRigidICP.cpp
        $(CC) -c $(SRCDIR_QTW)/SWGLOptimalStepNonRigidICP.cpp $(CFLAGS_DYN) $(SW_GL_OSNRI_WIDGET) -Fo"$(LIBDIR)/SWGLOptimalStepNonRigidICP_d.obj"

#           interface
$(LIBDIR)/SWCreateAvatarInterface_d.obj: ./src/interface/SWCreateAvatarInterface.cpp
        $(CC) -c ./src/interface/SWCreateAvatarInterface.cpp $(CFLAGS_DYN) $(SW_AVATAR_INTERFACE) -Fo"$(LIBDIR)/SWCreateAvatarInterface_d.obj"

$(LIBDIR)/SWMorphingInterface_d.obj: ./src/interface/SWMorphingInterface.cpp
        $(CC) -c ./src/interface/SWMorphingInterface.cpp $(CFLAGS_DYN) $(SW_MORPH_INTERFACE)  -Fo"$(LIBDIR)/SWMorphingInterface_d.obj"

$(LIBDIR)/SWQtCamera_d.obj: ./src/interface/SWQtCamera.cpp
        $(CC) -c ./src/interface/SWQtCamera.cpp $(CFLAGS_DYN) $(SW_QT_CAMERA)  -Fo"$(LIBDIR)/SWQtCamera_d.obj"

$(LIBDIR)/SWDisplayImageWidget_d.obj: $(SRCDIR_QTW)/SWDisplayImageWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWDisplayImageWidget.cpp $(CFLAGS_DYN) $(SW_DISPLAY_IMAGEW)  -Fo"$(LIBDIR)/SWDisplayImageWidget_d.obj"

$(LIBDIR)/SWDisplayCurvesWidget_d.obj: $(SRCDIR_QTW)/SWDisplayCurvesWidget.cpp
        $(CC) -c $(SRCDIR_QTW)/SWDisplayCurvesWidget.cpp $(CFLAGS_DYN) $(SW_DISPLAY_CURVES)  -Fo"$(LIBDIR)/SWDisplayCurvesWidget_d.obj"

#           avatar
$(LIBDIR)/SWCreateAvatar_d.obj: ./src/SWCreateAvatar.cpp
        $(CC) -c ./src/SWCreateAvatar.cpp $(CFLAGS_DYN) $(SW_CREATE_AVATAR) -Fo"$(LIBDIR)/SWCreateAvatar_d.obj"



############################################################################## STASM Files

################################## static

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

################################## dynamic

$(LIBDIR)/mat_d.obj: ./src/stasm/mat.cpp
        $(CC) -c ./src/stasm/mat.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/mat_d.obj"

$(LIBDIR)/err_d.obj: ./src/stasm/err.cpp
        $(CC) -c ./src/stasm/err.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/err_d.obj"

$(LIBDIR)/util_d.obj: ./src/stasm/util.cpp
        $(CC) -c ./src/stasm/util.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/util_d.obj"

$(LIBDIR)/mchol_d.obj: ./src/stasm/mchol.cpp
        $(CC) -c ./src/stasm/mchol.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/mchol_d.obj"

$(LIBDIR)/startshape_d.obj: ./src/stasm/startshape.cpp
        $(CC) -c ./src/stasm/startshape.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/startshape_d.obj"

$(LIBDIR)/initnet_d.obj: ./src/stasm/initnet.cpp
        $(CC) -c ./src/stasm/initnet.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/initnet_d.obj"

$(LIBDIR)/forward_d.obj: ./src/stasm/forward.cpp
        $(CC) -c ./src/stasm/forward.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/forward_d.obj"

$(LIBDIR)/shapemodel_d.obj: ./src/stasm/shapemodel.cpp
        $(CC) -c ./src/stasm/shapemodel.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/shapemodel_d.obj"

$(LIBDIR)/matvec_d.obj: ./src/stasm/matvec.cpp
        $(CC) -c ./src/stasm/matvec.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/matvec_d.obj"

$(LIBDIR)/imutil_d.obj: ./src/stasm/imutil.cpp
        $(CC) -c ./src/stasm/imutil.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/imutil_d.obj"

$(LIBDIR)/shapefile_d.obj: ./src/stasm/shapefile.cpp
        $(CC) -c ./src/stasm/shapefile.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/shapefile_d.obj"

$(LIBDIR)/landmarks_d.obj: ./src/stasm/landmarks.cpp
        $(CC) -c ./src/stasm/landmarks.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/landmarks_d.obj"

$(LIBDIR)/hypot_d.obj: ./src/stasm/hypot.c
        $(CC) -c ./src/stasm/hypot.c $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/hypot_d.obj"

$(LIBDIR)/asmsearch_d.obj: ./src/stasm/asmsearch.cpp
        $(CC) -c ./src/stasm/asmsearch.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/asmsearch_d.obj"

$(LIBDIR)/prof_d.obj: ./src/stasm/prof.cpp
        $(CC) -c ./src/stasm/prof.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/prof_d.obj"

$(LIBDIR)/initasm_d.obj: ./src/stasm/initasm.cpp
        $(CC) -c ./src/stasm/initasm.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/initasm_d.obj"

$(LIBDIR)/rgbimutil_d.obj: ./src/stasm/rgbimutil.cpp
        $(CC) -c ./src/stasm/rgbimutil.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/rgbimutil_d.obj"

$(LIBDIR)/imshape_d.obj: ./src/stasm/imshape.cpp
        $(CC) -c ./src/stasm/imshape.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/imshape_d.obj"

$(LIBDIR)/readconf_d.obj: ./src/stasm/readconf.cpp
        $(CC) -c ./src/stasm/readconf.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/readconf_d.obj"

$(LIBDIR)/readasm_d.obj: ./src/stasm/readasm.cpp
        $(CC) -c ./src/stasm/readasm.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/readasm_d.obj"

$(LIBDIR)/sparsemat_d.obj: ./src/stasm/sparsemat.cpp
        $(CC) -c ./src/stasm/sparsemat.cpp $(CFLAGS_DYN) $(STASM) -Fo"$(LIBDIR)/sparsemat_d.obj"


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

$(INCDIR_QTGENW)/SWUI_Morphing.h: $(FORMDIR)/SWUI_Morphing.ui
        uic $(FORMDIR)/SWUI_Morphing.ui -o $@

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

$(MOCDIR)/moc_SWGLMultiObjectWidget.cpp: $(INCDIR_QTW)/SWGLMultiObjectWidget.h
         moc  $(INCDIR_QTW)/SWGLMultiObjectWidget.h -o $@

$(MOCDIR)/moc_SWGLOptimalStepNonRigidICP.cpp: $(INCDIR_QTW)/SWGLOptimalStepNonRigidICP.h
         moc  $(INCDIR_QTW)/SWGLOptimalStepNonRigidICP.h -o $@

#   Workers

$(MOCDIR)/moc_SWCreateAvatarWorker.cpp: $(INCDIR_QTWORKERS)/SWCreateAvatarWorker.h
         moc  $(INCDIR_QTWORKERS)/SWCreateAvatarWorker.h -o $@

$(MOCDIR)/moc_SWMorphingWorker.cpp: $(INCDIR_QTWORKERS)/SWMorphingWorker.h
         moc  $(INCDIR_QTWORKERS)/SWMorphingWorker.h -o $@

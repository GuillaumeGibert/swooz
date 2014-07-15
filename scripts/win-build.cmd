@echo off

setlocal EnableDelayedExpansion
setlocal enableextensions 


REM ######################################################################## CHECK ARGUMENTS

set /a argNumber = 0

for %%a in (%*) do @set /a argNumber+=1

set CFG=Release
set ARCH=x86

if %argNumber% == 1 (
    set CFG= %1
)

if %argNumber% == 2 (
    set ARCH= %2
)

echo %CFG% %ARCH%

REM #######################################################################################
REM ## Uncomment if you want to clean before each compilation
REM call "win-clean.cmd"

REM #######################################################################################

call "win-init_env_command.cmd" noPath %ARCH%

REM #######################################################################################

set saved_directory=%CD%
set target_dist=..\dist

REM #######################################################################################
REM ## create the dist folder where the binaries will be stored
echo _______________________________________________________________________________
echo.
echo Creating 'dist' folder structure...

if not exist  %target_dist%\nul (
    mkdir %target_dist%
)

if not exist %target_dist%\examples\nul (
    mkdir %target_dist%\examples
)

if not exist  %target_dist%\bin\nul (
    mkdir %target_dist%\bin
)

if not exist  %target_dist%\include\nul (
    mkdir %target_dist%\include
)

if not exist  %target_dist%\lib\nul (
    mkdir %target_dist%\lib
)

if not exist  %target_dist%\data\nul (
    mkdir %target_dist%\data
)

if not exist  %target_dist%\doc\nul (
    mkdir %target_dist%\doc
)

if not exist  %target_dist%\log\nul (
    mkdir %target_dist%\log
)


REM #######################################################################################
echo.
echo _______________________________________________________________________________
echo.
echo Configuring, Building and Installing...
echo.


for /F %%s in (%SW_build_order%) do (
    set SW_project_name_full=%%s
    set SW_project_name_rel=!SW_project_name_full:%SW_base%\=!

    rem ## build the files
    echo ----
    echo [!SW_project_name_rel!]
    echo.

    pushd %%s

    call "win-build_branch.cmd" %CFG% %ARCH%

    rem ## copy the solutions to the dist folder
    echo Installing files to dist folder
    echo.
    %SystemRoot%\system32\xcopy /q /e /y /c bin %saved_directory%\%target_dist%\bin             >> NULL
    %SystemRoot%\system32\xcopy /q /e /y %%s\include %saved_directory%\%target_dist%\include    >> NULL
    %SystemRoot%\system32\xcopy /q /e /y %%s\lib %saved_directory%\%target_dist%\lib            >> NULL
    %SystemRoot%\system32\xcopy /q /e /y %%s\data %saved_directory%\%target_dist%\data          >> NULL
    %SystemRoot%\system32\xcopy /q /e /y %%s\doc %saved_directory%\%target_dist%\doc            >> NULL

    popd
)

echo _______________________________________________________________________________
echo.
echo Building process terminated successfully !
echo.
REM #######################################################################################
REM ### Kinect on Dimenco 3D display
echo @echo off                                               				>  %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo pushd ..\scripts                                        				>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo call win-init_env_command.cmd %ARCH_EXE_toolkit_kinectOnDimenco%                 >> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo popd                                                    				>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo cd bin										>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo SWDisplayKinectOnDimenco.exe %PARAMS_EXEC%							>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo cd ../../scripts									>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo call win-clean-env_command.cmd							>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo cd ../dist										>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
REM echo pause 										>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
REM #######################################################################################
REM ### Create avatar
echo @echo off                                               				>  %target_dist%\swooz-createAvatar.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-createAvatar.cmd
echo pushd ..\scripts                                        				>> %target_dist%\swooz-createAvatar.cmd
echo call win-init_env_command.cmd %ARCH_EXE_createAvatar%                        	>> %target_dist%\swooz-createAvatar.cmd
echo popd                                                    				>> %target_dist%\swooz-createAvatar.cmd
echo cd bin										>> %target_dist%\swooz-createAvatar.cmd
echo SWCreateAvatar.exe %PARAMS_EXEC%							>> %target_dist%\swooz-createAvatar.cmd
echo cd ../../scripts									>> %target_dist%\swooz-createAvatar.cmd
echo call win-clean-env_command.cmd							>> %target_dist%\swooz-createAvatar.cmd
echo cd ../dist										>> %target_dist%\swooz-createAvatar.cmd
REM echo pause 										>> %target_dist%\swooz-createAvatar.cmd
REM #######################################################################################
REM ### Morphing
echo @echo off                                               				>  %target_dist%\swooz-morphing.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-morphing.cmd
echo pushd ..\scripts                                        				>> %target_dist%\swooz-morphing.cmd
echo call win-init_env_command.cmd x86                                                  >> %target_dist%\swooz-morphing.cmd
echo popd                                                    				>> %target_dist%\swooz-morphing.cmd
echo cd bin										>> %target_dist%\swooz-morphing.cmd
echo SWMorphing.exe %PARAMS_EXEC%					>> %target_dist%\swooz-morphing.cmd
echo cd ../../scripts									>> %target_dist%\swooz-morphing.cmd
echo call win-clean-env_command.cmd							>> %target_dist%\swooz-morphing.cmd
echo cd ../dist										>> %target_dist%\swooz-morphing.cmd
REM echo pause 										>> %target_dist%\swooz-morphing.cmd
REM #######################################################################################
REM ### Morphing x64
echo @echo off                                               				>  %target_dist%\swooz-morphing-x64.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-morphing-x64.cmd
echo pushd ..\scripts                                        				>> %target_dist%\swooz-morphing-x64.cmd
echo call win-init_env_command.cmd amd64                                >> %target_dist%\swooz-morphing-x64.cmd
echo popd                                                    				>> %target_dist%\swooz-morphing-x64.cmd
echo cd bin										>> %target_dist%\swooz-morphing-x64.cmd
echo SWMorphing-x64.exe %PARAMS_EXEC%						>> %target_dist%\swooz-morphing-x64.cmd
echo cd ../../scripts									>> %target_dist%\swooz-morphing-x64.cmd
echo call win-clean-env_command.cmd							>> %target_dist%\swooz-morphing-x64.cmd
echo cd ../dist										>> %target_dist%\swooz-morphing-x64.cmd
REM echo pause 										>> %target_dist%\swooz-morphing-x64.cmd
REM #######################################################################################
REM ### Head Tracking Forest
echo @echo off                                                                          >  %target_dist%\swooz-trackingHeadForest.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingHeadForest.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingHeadForest.cmd
echo call win-init_env_command.cmd %ARCH_EXE_trackingHeadForest%                      >> %target_dist%\swooz-trackingHeadForest.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingHeadForest.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingHeadForest.cmd
echo SWTrackingHeadForest.exe ../data/ini/headForestConfig.ini                          >> %target_dist%\swooz-trackingHeadForest.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingHeadForest.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingHeadForest.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingHeadForest.cmd
REM echo pause                                                                          >> %target_dist%\swooz-trackingHeadForest.cmd
REM #######################################################################################
REM ### FaceLab tracking
echo @echo off                                                                          >  %target_dist%\swooz-trackingFaceLab.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingFaceLab.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingFaceLab.cmd
echo call win-init_env_command.cmd %ARCH_EXE_trackingFaceLab%                         >> %target_dist%\swooz-trackingFaceLab.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingFaceLab.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingFaceLab.cmd
echo SWFaceLabTracking.exe config.txt                                                   >> %target_dist%\swooz-trackingFaceLab.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingFaceLab.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingFaceLab.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingFaceLab.cmd
REM echo pause                                                                          >> %target_dist%\swooz-trackingFaceLab.cmd
REM ###############################################é########################################
REM ### FaceShift tracking
echo @echo off                                                                          >  %target_dist%\swooz-trackingFaceShift.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingFaceShift.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingFaceShift.cmd
echo call win-init_env_command.cmd %ARCH_EXE_trackingFaceShift%                       >> %target_dist%\swooz-trackingFaceShift.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingFaceShift.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingFaceShift.cmd
echo SWFaceShiftTracking.exe                                                            >> %target_dist%\swooz-trackingFaceShift.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingFaceShift.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingFaceShift.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingFaceShift.cmd
REM echo pause                                                                          >> %target_dist%\swooz-trackingFaceShift.cmd
REM #######################################################################################
REM ### Emicp head tracking
echo @echo off                                                                          >  %target_dist%\swooz-trackingHeadEmicp.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo call win-init_env_command.cmd %ARCH_EXE_trackingHeadEmicp%                       >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo SWEmicpHeadTracking.exe                                                            >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingHeadEmicp.cmd
REM echo pause                                                                          >> %target_dist%\swooz-trackingHeadEmicp.cmd
REM #######################################################################################
REM ### iCub Teleoperation
echo @echo off                                                                          >  %target_dist%\swooz-teleoperation_iCub.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-teleoperation_iCub.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-teleoperation_iCub.cmd
echo call win-init_env_command.cmd %ARCH_EXE_teleoperation_iCub%                      >> %target_dist%\swooz-teleoperation_iCub.cmd
echo popd                                                                               >> %target_dist%\swooz-teleoperation_iCub.cmd
echo cd bin                                                                             >> %target_dist%\swooz-teleoperation_iCub.cmd
echo SWTeleoperation_iCub.exe                                                           >> %target_dist%\swooz-teleoperation_iCub.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-teleoperation_iCub.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-teleoperation_iCub.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-teleoperation_iCub.cmd
REM echo pause                                                                          >> %target_dist%\swooz-teleoperation_iCub.cmd
REM #######################################################################################
REM ### nao Teleoperation
echo @echo off                                                                          >  %target_dist%\swooz-teleoperation_nao.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-teleoperation_nao.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-teleoperation_nao.cmd
echo call win-init_env_command.cmd %ARCH_EXE_teleoperation_nao%                      >> %target_dist%\swooz-teleoperation_nao.cmd
echo popd                                                                               >> %target_dist%\swooz-teleoperation_nao.cmd
echo cd bin                                                                             >> %target_dist%\swooz-teleoperation_nao.cmd
echo SWTeleoperation_nao.exe                                                           >> %target_dist%\swooz-teleoperation_nao.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-teleoperation_nao.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-teleoperation_nao.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-teleoperation_nao.cmd
REM echo pause                                                                          >> %target_dist%\swooz-teleoperation_nao.cmd
REM #######################################################################################
REM ### OpenNI tracking
echo @echo off                                                                          >  %target_dist%\swooz-trackingOpenNI.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingOpenNI.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingOpenNI.cmd
echo call win-init_env_command.cmd %ARCH_EXE_trackingOpenNI%                          >> %target_dist%\swooz-trackingOpenNI.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingOpenNI.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingOpenNI.cmd
echo SWOpenNITracking.exe                                                               >> %target_dist%\swooz-trackingOpenNI.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingOpenNI.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingOpenNI.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingOpenNI.cmd
REM #######################################################################################
REM ### Manipulation
echo @echo off                                                                          >  %target_dist%\swooz-manipulation.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-manipulation.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-manipulation.cmd
echo call win-init_env_command.cmd %ARCH_EXE_manipulation%                           >> %target_dist%\swooz-manipulation.cmd
echo popd                                                                               >> %target_dist%\swooz-manipulation.cmd
echo cd bin                                                                             >> %target_dist%\swooz-manipulation.cmd
echo SWManipulation.exe                                                                >> %target_dist%\swooz-manipulation.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-manipulation.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-manipulation.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-manipulation.cmd
REM #######################################################################################
REM ### Forth tracking
REM echo @echo off                                                                          >  %target_dist%\swooz-trackingForth.cmd
REM echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingForth.cmd
REM echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingForth.cmd
REM echo call win-init_env_command.cmd %ARCH_EXE_trackingForth%                           >> %target_dist%\swooz-trackingForth.cmd
REM echo popd                                                                               >> %target_dist%\swooz-trackingForth.cmd
REM echo cd bin                                                                             >> %target_dist%\swooz-trackingForth.cmd
REM echo SWForthTracking.exe                                                                >> %target_dist%\swooz-trackingForth.cmd
REM echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingForth.cmd
REM echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingForth.cmd
REM echo cd ../dist                                                                         >> %target_dist%\swooz-trackingForth.cmd
REM #######################################################################################
REM ### Fake tracking
echo @echo off                                                                          >  %target_dist%\swooz-trackingFake.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingFake.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingFake.cmd
echo call win-init_env_command.cmd %ARCH_EXE_trackingFake%                           >> %target_dist%\swooz-trackingFake.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingFake.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingFake.cmd
echo SWFakeTracking.exe %PARAMS_EXEC%                                                        >> %target_dist%\swooz-trackingFake.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingFake.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingFake.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingFake.cmd
REM #######################################################################################
REM ### Leap tracking
echo @echo off                                                                          >  %target_dist%\swooz-trackingLeap.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingLeap.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingLeap.cmd
echo call win-init_env_command.cmd %ARCH_EXE_trackingLeap%                           >> %target_dist%\swooz-trackingLeap.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingLeap.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingLeap.cmd
echo SWLeapTracking.exe %PARAMS_EXEC%                                                        >> %target_dist%\swooz-trackingLeap.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingLeap.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingLeap.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingLeap.cmd
REM #######################################################################################
REM ### Viewer
echo @echo off                                                                          >  %target_dist%\swooz-viewer.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-viewer.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-viewer.cmd
echo call win-init_env_command.cmd x86                                                  >> %target_dist%\swooz-viewer.cmd
echo popd                                                                               >> %target_dist%\swooz-viewer.cmd
echo cd bin                                                                             >> %target_dist%\swooz-viewer.cmd
echo SWViewer.exe                                                                       >> %target_dist%\swooz-viewer.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-viewer.cmd
echo call win-clean-env_command.cmd                                                     >> %target_dist%\swooz-viewer.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-viewer.cmd
REM #######################################################################################
REM ### Viewer 64
echo @echo off                                                                          >  %target_dist%\swooz-viewer-x64.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-viewer-x64.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-viewer-x64.cmd
echo call win-init_env_command.cmd amd64                                                >> %target_dist%\swooz-viewer-x64.cmd
echo popd                                                                               >> %target_dist%\swooz-viewer-x64.cmd
echo cd bin                                                                             >> %target_dist%\swooz-viewer-x64.cmd
echo SWViewer-x64.exe                                                                   >> %target_dist%\swooz-viewer-x64.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-viewer-x64.cmd
echo call win-clean-env_command.cmd                                                     >> %target_dist%\swooz-viewer-x64.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-viewer-x64.cmd

REM #####################   EXAMPLES

REM #######################################################################################
REM ### display kinect
echo @echo off                                                                          >  %target_dist%\examples\display_kinect.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\examples\display_kinect.cmd
echo pushd ..\..\scripts                                                                   >> %target_dist%\examples\display_kinect.cmd
echo call win-init_env_command.cmd %ARCH_EXE_examples%                           >> %target_dist%\examples\display_kinect.cmd
echo cd ..\dist\bin                                                                             >> %target_dist%\examples\display_kinect.cmd
echo kinect_display.exe                                                                >> %target_dist%\examples\display_kinect.cmd
echo cd ../../scripts                                                                   >> %target_dist%\examples\display_kinect.cmd
echo call win-clean-env_command.cmd                                                     >> %target_dist%\examples\display_kinect.cmd
echo cd ../dist/examples                                                                >> %target_dist%\examples\display_kinect.cmd
REM #######################################################################################
REM ### display thread kinect
echo @echo off                                                                          >  %target_dist%\examples\display_kinect_thread.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\examples\display_kinect_thread.cmd
echo pushd ..\..\scripts                                                                   >> %target_dist%\examples\display_kinect_thread.cmd
echo call win-init_env_command.cmd %ARCH_EXE_examples%                           >> %target_dist%\examples\display_kinect_thread.cmd
echo cd ..\dist\bin                                                                             >> %target_dist%\examples\display_kinect_thread.cmd
echo kinect_thread_display.exe                                                                >> %target_dist%\examples\display_kinect_thread.cmd
echo cd ../../scripts                                                                   >> %target_dist%\examples\display_kinect_thread.cmd
echo call win-clean-env_command.cmd                                                     >> %target_dist%\examples\display_kinect_thread.cmd
echo cd ../dist/examples                                                                >> %target_dist%\examples\display_kinect_thread.cmd
REM #######################################################################################
REM ### save data kinect
echo @echo off                                                                          >  %target_dist%\examples\data_saver_kinect.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\examples\data_saver_kinect.cmd
echo pushd ..\..\scripts                                                                   >> %target_dist%\examples\data_saver_kinect.cmd
echo call win-init_env_command.cmd %ARCH_EXE_examples%                           >> %target_dist%\examples\data_saver_kinect.cmd
echo cd ..\dist\bin                                                                             >> %target_dist%\examples\data_saver_kinect.cmd
echo kinect_data_saver.exe                                                                >> %target_dist%\examples\data_saver_kinect.cmd
echo cd ../../scripts                                                                   >> %target_dist%\examples\data_saver_kinect.cmd
echo call win-clean-env_command.cmd                                                     >> %target_dist%\examples\data_saver_kinect.cmd
echo cd ../dist/examples                                                                >> %target_dist%\examples\data_saver_kinect.cmd
REM #######################################################################################
REM ### load data kinect
echo @echo off                                                                          >  %target_dist%\examples\data_loader_kinect.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\examples\data_loader_kinect.cmd
echo pushd ..\..\scripts                                                                   >> %target_dist%\examples\data_loader_kinect.cmd
echo call win-init_env_command.cmd %ARCH_EXE_examples%                           >> %target_dist%\examples\data_loader_kinect.cmd
echo cd ..\dist\bin                                                                             >> %target_dist%\examples\data_loader_kinect.cmd
echo kinect_data_loader.exe                                                                >> %target_dist%\examples\data_loader_kinect.cmd
echo cd ../../scripts                                                                   >> %target_dist%\examples\data_loader_kinect.cmd
echo call win-clean-env_command.cmd                                                     >> %target_dist%\examples\data_loader_kinect.cmd
echo cd ../dist/examples                                                                >> %target_dist%\examples\data_loader_kinect.cmd
REM #######################################################################################
REM ### detect face stasm
echo @echo off                                                                          >  %target_dist%\examples\detect_face_stasm.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\examples\detect_face_stasm.cmd
echo pushd ..\..\scripts                                                                   >> %target_dist%\examples\detect_face_stasm.cmd
echo call win-init_env_command.cmd %ARCH_EXE_examples%                           >> %target_dist%\examples\detect_face_stasm.cmd
echo cd ..\dist\bin                                                                             >> %target_dist%\examples\detect_face_stasm.cmd
echo detect_face_stasm.exe                                                                >> %target_dist%\examples\detect_face_stasm.cmd
echo cd ../../scripts                                                                   >> %target_dist%\examples\detect_face_stasm.cmd
echo call win-clean-env_command.cmd                                                     >> %target_dist%\examples\detect_face_stasm.cmd
echo cd ../dist/examples                                                                >> %target_dist%\examples\detect_face_stasm.cmd
REM #######################################################################################
REM ### display leap
echo @echo off                                                                          >  %target_dist%\examples\display_leap.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\examples\display_leap.cmd
echo pushd ..\..\scripts                                                                >> %target_dist%\examples\display_leap.cmd
echo call win-init_env_command.cmd %ARCH_EXE_examples%                                  >> %target_dist%\examples\display_leap.cmd
echo cd ..\dist\bin                                                                     >> %target_dist%\examples\display_leap.cmd
echo display_leap.exe                                                                   >> %target_dist%\examples\display_leap.cmd
echo cd ../../scripts                                                                   >> %target_dist%\examples\display_leap.cmd
echo call win-clean-env_command.cmd                                                     >> %target_dist%\examples\display_leap.cmd
echo cd ../dist/examples                                                                >> %target_dist%\examples\display_leap.cmd
REM #######################################################################################

echo.
echo Install completed !
echo.

REM ####################################################################################### COPY SWOOZ-CONFIG TO DIST

%SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data

echo.
REM ####################################################################################### CLEAN PATH
call "win-clean-env_command.cmd"
echo.

goto terminate_success
REM #######################################################################################

:terminate_error

echo.
echo An error occured during building process !
echo.
pause

goto terminate

REM #######################################################################################

:terminate_success

pause
goto terminate

REM #######################################################################################

:terminate

del NULL

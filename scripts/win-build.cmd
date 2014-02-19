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

call "win-init_env_command.cmd" noPath

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
REM ####################################################################################### Creates start files
REM ### 3D Player
echo @echo off                                               				>  %target_dist%\swooz-3dplayer.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-3dplayer.cmd
echo pushd ..\scripts                                        				>> %target_dist%\swooz-3dplayer.cmd
echo call win-init_env_command.cmd %ARCH_EXE_3dplayer%                                >> %target_dist%\swooz-3dplayer.cmd
echo popd                                                    				>> %target_dist%\swooz-3dplayer.cmd
echo cd bin										>> %target_dist%\swooz-3dplayer.cmd
echo java -Xmx512m SW3DPlayer -config configPlayer.txt -v 				>> %target_dist%\swooz-3dplayer.cmd
echo cd ../../scripts									>> %target_dist%\swooz-3dplayer.cmd
echo call win-clean-env_command.cmd							>> %target_dist%\swooz-3dplayer.cmd
echo cd ../dist										>> %target_dist%\swooz-3dplayer.cmd
REM echo pause 										>> %target_dist%\swooz-3dplayer.cmd
REM #######################################################################################
REM ### Dummy 3D Client
echo @echo off                                               				>  %target_dist%\swooz-3dclient_dummy.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-3dclient_dummy.cmd
echo pushd ..\scripts                                        				>> %target_dist%\swooz-3dclient_dummy.cmd
echo call win-init_env_command.cmd %ARCH_EXE_3dclient_dummy%                          >> %target_dist%\swooz-3dclient_dummy.cmd
echo popd                                                    				>> %target_dist%\swooz-3dclient_dummy.cmd
echo cd bin										>> %target_dist%\swooz-3dclient_dummy.cmd
echo java  SW3DClientDummy -config configClient_Stelarc_head.txt -v  -seq Block_1.seq -gaze Block_1.gaze -fps 60>> %target_dist%\swooz-3dclient_dummy.cmd
echo cd ../../scripts									>> %target_dist%\swooz-3dclient_dummy.cmd
echo call win-clean-env_command.cmd							>> %target_dist%\swooz-3dclient_dummy.cmd
echo cd ../dist										>> %target_dist%\swooz-3dclient_dummy.cmd
REM echo pause 										>> %target_dist%\swooz-3dclient_dummy.cmd
REM #######################################################################################
REM ### Kinect on Dimenco 3D display
echo @echo off                                               				>  %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo pushd ..\scripts                                        				>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo call win-init_env_command.cmd %ARCH_EXE_toolkit_kinectOnDimenco%                 >> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo popd                                                    				>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo cd bin										>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo SWDisplayKinectOnDimenco.exe 							>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
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
echo SWCreateAvatar.exe %1 %2 %3							>> %target_dist%\swooz-createAvatar.cmd
echo cd ../../scripts									>> %target_dist%\swooz-createAvatar.cmd
echo call win-clean-env_command.cmd							>> %target_dist%\swooz-createAvatar.cmd
echo cd ../dist										>> %target_dist%\swooz-createAvatar.cmd
REM echo pause 										>> %target_dist%\swooz-createAvatar.cmd
REM #######################################################################################
REM ### Test code
echo @echo off                                               				>  %target_dist%\swooz-testCode.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-testCode.cmd
echo pushd ..\scripts                                        				>> %target_dist%\swooz-testCode.cmd
echo call win-init_env_command.cmd %ARCH_EXE_testCode%                                >> %target_dist%\swooz-testCode.cmd
echo popd                                                    				>> %target_dist%\swooz-testCode.cmd
echo cd bin										>> %target_dist%\swooz-testCode.cmd
echo SWTestCode.exe %1 %2 %3 %PARAMS_EXEC%						>> %target_dist%\swooz-testCode.cmd 
echo cd ../../scripts									>> %target_dist%\swooz-testCode.cmd
echo call win-clean-env_command.cmd							>> %target_dist%\swooz-testCode.cmd
echo cd ../dist										>> %target_dist%\swooz-testCode.cmd
REM echo pause 										>> %target_dist%\swooz-testCode.cmd
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
REM ### Forth tracking
echo @echo off                                                                          >  %target_dist%\swooz-trackingForth.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingForth.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingForth.cmd
echo call win-init_env_command.cmd %ARCH_EXE_trackingForth%                           >> %target_dist%\swooz-trackingForth.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingForth.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingForth.cmd
echo SWForthTracking.exe                                                                >> %target_dist%\swooz-trackingForth.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingForth.cmd
echo call win-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingForth.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingForth.cmd
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

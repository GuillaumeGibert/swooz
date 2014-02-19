@echo off

setlocal EnableDelayedExpansion
setlocal enableextensions 

REM #######################################################################################
REM ## Uncomment if you want to clean before each compilation
REM ~ call "win32-clean.cmd"

REM #######################################################################################

call "win32-init_env_command.cmd"

REM
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
REM
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

    call "win32-build_branch.cmd" %1

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
echo call win32-init_env_command.cmd               					>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo popd                                                    				>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo cd bin										>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo SWDisplayKinectOnDimenco.exe 							>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo cd ../../scripts									>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo call win32-clean-env_command.cmd							>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
echo cd ../dist										>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
REM echo pause 										>> %target_dist%\swooz-toolkit_kinectOnDimenco.cmd
REM #######################################################################################
REM ### Head Tracking Forest
echo @echo off                                                                          >  %target_dist%\swooz-trackingHeadForest.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingHeadForest.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingHeadForest.cmd
echo call win32-init_env_command.cmd                                                    >> %target_dist%\swooz-trackingHeadForest.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingHeadForest.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingHeadForest.cmd
echo SWTrackingHeadForest.exe ../data/ini/headForestConfig.ini                          >> %target_dist%\swooz-trackingHeadForest.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingHeadForest.cmd
echo call win32-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingHeadForest.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingHeadForest.cmd
REM echo pause                                                                              >> %target_dist%\swooz-trackingHeadForest.cmd
REM #######################################################################################
REM ### Emicp head tracking
echo @echo off                                                                          >  %target_dist%\swooz-trackingHeadEmicp.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo call win32-init_env_command.cmd                                                    >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo SWEmicpHeadTracking.exe                                                            >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo call win32-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingHeadEmicp.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingHeadEmicp.cmd
REM echo pause                                                                              >> %target_dist%\swooz-trackingHeadEmicp.cmd
REM #######################################################################################
REM ### FaceLab tracking
echo @echo off                                                                          >  %target_dist%\swooz-trackingFaceLab.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingFaceLab.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingFaceLab.cmd
echo call win32-init_env_command.cmd                                                    >> %target_dist%\swooz-trackingFaceLab.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingFaceLab.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingFaceLab.cmd
echo SWFaceLabTracking.exe                                                              >> %target_dist%\swooz-trackingFaceLab.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingFaceLab.cmd
echo call win32-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingFaceLab.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingFaceLab.cmd
REM echo pause                                                                              >> %target_dist%\swooz-trackingFaceLab.cmd
REM #######################################################################################
REM ### FaceShift tracking
echo @echo off                                                                          >  %target_dist%\swooz-trackingFaceShift.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-trackingFaceShift.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-trackingFaceShift.cmd
echo call win32-init_env_command.cmd                                                    >> %target_dist%\swooz-trackingFaceShift.cmd
echo popd                                                                               >> %target_dist%\swooz-trackingFaceShift.cmd
echo cd bin                                                                             >> %target_dist%\swooz-trackingFaceShift.cmd
echo SWFaceShiftTracking.exe                                                            >> %target_dist%\swooz-trackingFaceShift.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-trackingFaceShift.cmd
echo call win32-clean-env_command.cmd                                                   >> %target_dist%\swooz-trackingFaceShift.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-trackingFaceShift.cmd
REM echo pause                                                                              >> %target_dist%\swooz-trackingFaceShift.cmd
REM #######################################################################################
REM ### iCub Teleoperation
echo @echo off                                                                          >  %target_dist%\swooz-teleoperation_iCub.cmd
echo %SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data >> %target_dist%\swooz-teleoperation_iCub.cmd
echo pushd ..\scripts                                                                   >> %target_dist%\swooz-teleoperation_iCub.cmd
echo call win32-init_env_command.cmd                                                    >> %target_dist%\swooz-teleoperation_iCub.cmd
echo popd                                                                               >> %target_dist%\swooz-teleoperation_iCub.cmd
echo cd bin                                                                             >> %target_dist%\swooz-teleoperation_iCub.cmd
echo SWTeleoperation_iCub.exe                                                           >> %target_dist%\swooz-teleoperation_iCub.cmd
echo cd ../../scripts                                                                   >> %target_dist%\swooz-teleoperation_iCub.cmd
echo call win32-clean-env_command.cmd                                                   >> %target_dist%\swooz-teleoperation_iCub.cmd
echo cd ../dist                                                                         >> %target_dist%\swooz-teleoperation_iCub.cmd
echo pause                                                                              >> %target_dist%\swooz-teleoperation_iCub.cmd
REM #######################################################################################

echo.
echo Install completed !
echo.

REM ####################################################################################### COPY SWOOZ-CONFIG TO DIST
%SystemRoot%\system32\xcopy /q /e /y %saved_directory%\..\swooz-config %saved_directory%\%target_dist%\data

echo.
REM ####################################################################################### CLEAN PATH
call "win32-clean-env_command.cmd"
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

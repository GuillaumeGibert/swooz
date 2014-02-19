@echo off

if "%1" == "" goto recall_me

goto base

REM #######################################################################################

:recall_me

cmd /e /v /c %0 dummy

goto terminate

REM #######################################################################################

:base

REM #######################################################################################
REM ## start the initialisation of variables: path, classpath, branches...

call "win32-init_env_command.cmd"

REM #######################################################################################

for /F %%s in (%SW_build_order%) do (
	set SW_project_name_full=%%s
	set SW_project_name_rel=!SW_project_name_full:%SW_base%\=!
	echo 
	
	rem ## remove the files
	echo ----	
	echo [Deleting in !SW_project_name_rel!]
	echo.
	
	pushd %%s

        if exist ./bin/nul (
            rmdir bin /s /q
            mkdir bin
        )

        if exist ./lib/nul (
            rmdir lib /s /q
            mkdir lib
        )

        if exist ./moc/nul (
            rmdir moc /s /q
            mkdir moc
        )
		
        if exist ./genUI/nul (
            rmdir genUI /s /q
            mkdir genUI
        )		

	popd
)

REM #######################################################################################
echo ----	
	echo [Deleting in dist]
	echo.


set target_dist=..\dist

cd %target_dist%
rmdir /s /q %target_dist%\bin       > NULL 2<&1
rmdir /s /q %target_dist%\include   > NULL 2<&1
rmdir /s /q %target_dist%\lib       > NULL 2<&1
rmdir /s /q %target_dist%\doc       > NULL 2<&1
rmdir /s /q %target_dist%\log       > NULL 2<&1
rem rmdir /s /q %target_dist%\data  > NULL 2<&1
del  %target_dist%\*.cmd            > NULL 2<&1




echo.
echo Clean process terminated successfully !
echo.
echo ----	


:terminate

del NULL

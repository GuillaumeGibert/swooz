@echo off

REM ############################################################################ SET PATH
echo.
call "win-init_env_command.cmd" noPATH
REM set saved_directory=%CD%
REM set target_dist=..\dist
echo.
REM ############################################################################ GENERATE DOC
echo.
echo Generation of the documentation for all projects :
echo.

for /F %%s in (%SW_build_order%) do (
	set SW_project_name_full=%%s
	set SW_project_name_rel=!SW_project_name_full:%SW_base%\=!
	
	rem ## build the files
	echo ----	
	echo [!SW_project_name_rel!]
	echo.
	
	pushd %%s
	
        call "win-generate_doc.cmd"
		
	popd	
)

REM ############################################################################ CLEAN PATH
echo.
call "win-clean-env_command.cmd"
echo.


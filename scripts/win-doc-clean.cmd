@echo off


REM ############################################################################ SET PATH
echo.
call "win-init_env_command.cmd" noPATH
REM set saved_directory=%CD%
REM set target_dist=..\dist
echo.
REM ############################################################################ CLEAN DOC

for /F %%s in (%SW_build_order%) do (
	set SW_project_name_full=%%s
	set SW_project_name_rel=!SW_project_name_full:%SW_base%\=!
	
	rem ## remove the files
	echo ----	
	echo [Deleting in !SW_project_name_rel!]
	echo.
	
	pushd %%s
	
	rmdir /s/q doc
	mkdir doc
	cd ..
	cd ..
	
	popd
)

REM ############################################################################ CLEAN PATH
echo.
call "win-clean-env_command.cmd"
echo Clean doc terminated successfully !
echo ----

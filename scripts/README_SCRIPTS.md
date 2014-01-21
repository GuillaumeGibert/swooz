SWOOZ - SCRIPTS
===============

1. BUILD SWOOZ
--------------

	* win32-build.cmd builds the platform :
		* win32-build Debug    -> for the debug mode
		* win32-build Release  -> for the release mode
	
This script creates the ../dist file tree :  

	* ../dist
		* ../dist/bin
		* ../dist/include
		* ../dist/lib
		* ../dist/data
		* ../dist/doc

It calls all the **win32-build_branch.cmd** scripts of all the swooz projects defined in **SW_build_order.txt**,
this file is created by the **win32-init-env_command.cmd** script, you can remove or add a project to be build
by commend or uncomment the lines, example :

**win32-init-env_command.cmd** (at the end) :

	set SW_build_order=SW_build_order.txt
	del %SW_build_order% > NULL 2<&1
	echo %SW_toolkit%\%SW_toolkit_branch% >> %SW_build_order%
	echo %SW_tracking%\%SW_tracking_branch% >> %SW_build_order%
	echo %SW_teleoperation%\%SW_teleoperation_branch% >> %SW_build_order%
	
The 3 swooz projects, SW_toolkit, SW_tracking, SW_teleoperation will be builded.  
Add 'REM' before the line of the project you don't want to build.
	
	set SW_build_order=SW_build_order.txt
	del %SW_build_order% > NULL 2<&1
	echo %SW_toolkit%\%SW_toolkit_branch% >> %SW_build_order%
	REM echo %SW_tracking%\%SW_tracking_branch% >> %SW_build_order%
	REM echo %SW_teleoperation%\%SW_teleoperation_branch% >> %SW_build_order%	
	
By commenting the two last lines, now only SW_toolkit will be builded.
	
__IMPORTANT : do not change the build order of the projects, and do not comment SW_toolkit, its contains numerous 
functions used in the others projects, if you do so the build may be not working.__
	
...	
	
	
2. CLEAN SWOOZ
--------------
	
win32-clean.cmd will delete all the content of the dist repertory (except dist/data) and all the compiled files
in the swooz projects defined in the auto-generated file "SW_build_order.txt" (see win32-build.cmd)

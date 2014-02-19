SWOOZ - SCRIPTS
===============

0. DEPENDENCIES
---------------

Install openni...

...

Install cuda...

...

Checkout of the git repository...

...

Install the swooz-3rdparty installer...

...


Modify the **win32-init_env_command.cmd.skeleton** script.
If you have used the **swooz-3rdparty** installer, only rename **"win32-init_env_command.cmd.skeleton"** into **"win32-init_env_command.cmd"**, else you will have to open it and define manually all the librairies links before renaming it.


1. BUILD SWOOZ
--------------

Go to ./scripts and open a console.

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
bin, include, lib, data, doc directories of all theses project will be copied in ./dist.



2. CHOOSE project to be built
-----------------------------


**SW_build_order.txt** is created by the **win32-init-env_command.cmd** script, you can remove or add a project to be buildt by comment or uncomment the lines, example :

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
	
	
3. CLEAN SWOOZ
--------------
	
win32-clean.cmd will delete all the content of the dist repertory (except dist/data) and all the compiled files
in the swooz projects defined in the auto-generated file **"SW_build_order.txt"** (see win32-build.cmd)


4. GENERATE DOC
---------------
 
win32-doc-generate.cmd will call all the win32-generate_doc.cmd of each project, these scripts generate the Doxygen documentation using their respective Doxyfile.

5. CLEAN DOC
------------

win32-doc-clean.cmd will delete the doc directories of all the projects.



SWOOZ - SCRIPTS
===============

0. DEPENDENCIES
---------------


**0.1** If your kinect/xtion device is not installed yet :

- install openni compliant driver :
	* http://www.openni.org/wp-content/uploads/2013/11/Sensor-Win32-5.1.6.6-Redist.zip


**0.2** For compiling projects using cuda (swooz-trackingHeadEmicp, swooz-morphing, swooz-createAvatar)

- install cuda sdk :	
	* ...

**0.3** Checkout of the git repository.


**0.4** Install the **swooz-3rdparty** installer in the swooz-master directory.


**0.5** Modify the **win-init_env_command.cmd.skeleton** script.

If you have used the **swooz-3rdparty** installer, only rename **"win-init_env_command.cmd.skeleton"** into **"win-init_env_command.cmd"**, else you will have to open it and define manually all the librairies links before renaming it.


1. BUILD SWOOZ
--------------

Go to ./scripts and open a console.

	* win-build.cmd builds the platform :	
		* win-build					-> for release x86 mode (recommanded)
		* win-build Debug    		-> for the debug x86 mode
		* win-build Release  		-> for the release x86 mode
		* win-build Debug amd64 	-> for the debug amd64 mode
		* win-build Release amd64 	-> for the release amd64 mode

	
This script creates the ../dist file tree :  

	* ../dist
		* ../dist/bin
		* ../dist/include
		* ../dist/lib
		* ../dist/data
		* ../dist/doc

It calls all the **win-build_branch.cmd** scripts of all the swooz projects defined in **SW_build_order.txt**,
bin, include, lib, data, doc directories of all theses project will be copied in ./dist.



2. CHOOSE project to be built
-----------------------------


**SW_build_order.txt** is created by the **win-init-env_command.cmd** script, you can remove or add a project to be buildt by comment or uncomment the lines, example :

**win-init-env_command.cmd** (at the end) :

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
functions used in the others projects, if you do so the build may be not working.
	
	
3. CLEAN SWOOZ
--------------
	
win-clean.cmd will delete all the content of the dist repertory (except dist/data) and all the compiled files
in the swooz projects defined in the auto-generated file **"SW_build_order.txt"** (see win-build.cmd)


4. GENERATE DOC
---------------
 
win-doc-generate.cmd will call all the win-generate_doc.cmd of each project, these scripts generate the Doxygen documentation using their respective Doxyfile.

5. CLEAN DOC
------------

win-doc-clean.cmd will delete the doc directories of all the projects.




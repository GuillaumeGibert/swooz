SWOOZ - SCRIPTS
===============

0. DEPENDENCIES
---------------

 * Install openni :

	OpenNI-Win32-1.5.4.0-Dev1.msi -> http://www.openni.org/wp-content/uploads/2012/12/OpenNI-Win32-1.5.4.0-Dev1.zip
	
	SensorKinect093-Bin-Win32-v5.1.2.1.msi -> https://github.com/avin2/SensorKinect (in the bin folder)
	
	Sensor-Win32-5.1.2.1-Redist.msi -> http://www.openni.org/wp-content/uploads/2012/12/Sensor-Win32-5.1.2.1-Redist.zip
	
	NITE-Win32-1.5.2.21-Dev.msi -> http://www.openni.org/wp-content/uploads/2012/12/NITE-Win32-1.5.2.21-Dev.zip  
	 
	
 * Install cuda : https://developer.nvidia.com/cuda-downloads

 * Checkout of the git repository...

 * Install the swooz-3rdparty installer in the swooz directory : temporary link -> http://www.sendspace.com/file/l35mg7

In the scripts directory, rename **"win-init_env_command.cmd.skeleton"** into **"win-init_env_command.cmd"**.
You can modify it in order to change the dependencies of the project.

1. BUILD SWOOZ
--------------

Go to ./scripts and open a console.

	* win-build.cmd builds the platform :
		* win-build 		   -> for the release x86 mode
		* win-build Debug          -> for the debug x86 mode 
		* win-build Release  	   -> for the release x86 mode
		* win-build Debug adm64    -> for the debug amd64 mode 
		* win-build Release adm64  -> for the release amd64 mode
	
This script creates the ../dist file tree :  

	* ../dist
		* ../dist/bin
		* ../dist/include
		* ../dist/lib
			* ../dist/lib/x86
				* ../dist/lib/x86/Debug
				* ../dist/lib/x86/Release
			* ../dist/lib/amd64
				* ../dist/lib/amd64/Debug
				* ../dist/lib/amd64/Release
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
functions used in the others projects, if you do so the build may be not working.__
	
	
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




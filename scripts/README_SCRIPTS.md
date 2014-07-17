SWOOZ - SCRIPTS
===============

0. DEPENDENCIES
---------------

 * Devices kinect : some SWoOZ modules use Kinect/xtion depth captors, for using them you have to install openni and some drivers :

	- OpenNI-Win32-1.5.4.0-Dev1.msi -> http://dl.free.fr/qmaobqSzQ

	- NITE-Win32-1.5.2.21-Dev.msi -> http://dl.free.fr/verV6lgqc
	
	- Sensor-Win32-5.1.2.1-Redist.msi -> http://dl.free.fr/vQVq8w54d

	- This one is only necessary for the microsoft Kinect  SensorKinect093-Bin-Win32-v5.1.2.1.msi -> https://github.com/avin2/SensorKinect (in the bin folder)
	
 * Leap : some SWoOZ modules use the Leap motion captor,  for using it you have to install the Leap Motion Software -> https://developer.leapmotion.com/downloads

	
 * Install cuda : https://developer.nvidia.com/cuda-downloads

 * Checkout of the git repository...

 * Install the swooz-3rdparty installer in the swooz directory : http://dl.free.fr/getfile.pl?file=/u9gyouLD


1. BUILD SWOOZ
--------------

Go to ./scripts and open a console.

	* win-build.pl builds the platform :
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

It calls all the **win-build_branch.pl** scripts of all the swooz projects defined in **win-init_env_command.pl**,
bin, include, lib, data, doc directories of all theses project will be copied in ./dist.

In the scripts directory, you can modfiy win-init_env_command.pl in order to change the libs paths of the project


2. CHOOSE project to be built
-----------------------------

You can set the SWoOZ modules to be built by modifiying the **PBuildOrder** variable in **win-init_env_command.pl** :

	my @PbuildOrder  = ($Toolkit, $Avatar, $Tracking, $Teleop, $Manip, $Viewer, $Examples, $Feedback);

Here The first project to be built is Toolkit and after that Avatar.

	my @PbuildOrder  = ($Toolkit, $Avatar, $Tracking, $Teleop, $Manip, $Viewer, $Examples);

Now Feedback is excluded from the SWoOZ project built.
The order is important, some projects depends from each others, Toolkit is necessary for all projects.


	
3. CLEAN SWOOZ
--------------
	
win-clean.pl will delete all the content of the dist repertory (except dist/data) and all the compiled files
in the swooz projects defined in @PbuildOrder.


4. GENERATE DOC
---------------
 
win-doc-generate.pl will call all the win-generate_doc.pl of each project, these scripts generate the Doxygen documentation using their respective Doxyfile.

5. CLEAN DOC
------------

win-doc-clean.pl will delete the doc directories of all the projects.




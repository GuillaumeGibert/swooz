SWOOZ - SCRIPTS
===============

This README is only for Windows 7, others Windows have not been tested.


0. DEPENDENCIES
---------------

 * Devices kinect : some SWoOz modules use Kinect/xtion depth captors, for using them you have to install openni and some drivers :

	- [OpenNI-Win32-1.5.4.0-Dev1.msi](https://mega.co.nz/#!vZNWBLgC!SHhJy4ZCSKf__OQDq-fPIUuYvHvsceWFXNdUzaANtLs)

	- [NITE-Win32-1.5.2.21-Dev.msi](https://mega.co.nz/#!LQ8lRDob!sjsfcA2Dze2tQx4gov23RLPSPxp8QRliX1DayLbz_2k)
	
	- [Sensor-Win32-5.1.2.1-Redist.msi](https://mega.co.nz/#!DdlXRBzZ!rTq6ODc1Vy8IVyUxNIHzVv_FF_iqMphy5UA8lYiOeAs)

	- This one is only necessary for the microsoft Kinect  [SensorKinect093-Bin-Win32-v5.1.2.1.msi](https://mega.co.nz/#!SMEyhawC!pF3DAzSxhCd6IqNDvhCoqQgOe6F5UFuxAnpv1BdETP4)
	
 * Leap : some SWoOz modules use the Leap motion captor,  for using it you have to install the [Leap Motion Software](https://mega.co.nz/#!jM0ClIAL!CQDI7-5zoPD5soe-ieEooHyTxMU0Q8OYGJ5zweTbGF0) (this is the 2.0 tracking skeleton beta version, pay attention if you donwload it from the official website)
	
 * CUDA is necessary for the avatar creation and morphing and for the EMCIP tracking, in these cases install cuda : https://developer.nvidia.com/cuda-downloads

 * Checkout of the git repository...

 * Install the [swooz-3rdparty installer](https://mega.co.nz/#!SE0EAZQL!4avWNJyCWS-glk6MBFZEqZjMTNHYNLGxL8-yR7dHBaI) in the swooz directory

 * Perl is needed for launching the scripts, you can install this perl distribution : http://strawberryperl.com/

 * The scripts use the VS 2010 compiler to build the solution, if you don't have any version of Visual Studio you can install the [Microsoft SDK](http://www.microsoft.com/en-us/download/details.aspx?id=8279)

1. BUILD SWOOZ
--------------

Go to ./scripts and open a console.

	* win-build.pl builds the platform :
		* win-build 		   -> for the release x86 mode
		* win-build Debug          -> for the debug x86 mode 
		* win-build Release  	   -> for the release x86 mode
		* win-build Debug adm64    -> for the debug amd64 mode 
		* win-build Release adm64  -> for the release amd64 mode
	* IMPORTANT : swooz-3rdparty installer contains only libraries for release x86 and release amd64 modes
	
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

It calls all the **win-build_branch.pl** scripts of all the SWoOz projects defined in **win-init_env_command.pl**,
bin, include, lib, data, doc directories of all theses project will be copied in ./dist.

In the scripts directory, you can modify win-init_env_command.pl in order to change the libs paths of the project


2. CHOOSE project to be built
-----------------------------

You can set the SWoOz modules to be built by modifiying the **PBuildOrder** variable in **win-init_env_command.pl** :

	my @PbuildOrder  = ($Toolkit, $Avatar, $Tracking, $Teleop, $Manip, $Viewer, $Examples, $Feedback);

Here The first project to be built is Toolkit and after that Avatar.

	my @PbuildOrder  = ($Toolkit, $Avatar, $Tracking, $Teleop, $Manip, $Viewer, $Examples);

Now Feedback is excluded from the SWoOz project build.
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




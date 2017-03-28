================================================================================                  

                  Elo Touchscreen Linux Driver - Serial v 3.3.2
      
                    Intel i686 (x86) - 32 bit Driver Package
       
              Installation/Calibration/Uninstallation Instructions 

--------------------------------------------------------------------------------                      
                      
                                  Version 3.3.2 
                                  June 07, 2013
                               Elo Touch Solutions

================================================================================                  

Elo Linux Serial Driver package contains native Linux drivers designed for Linux kernel 2.4.18 and later, video alignment utility, rotation utility and control panel utilities for Elo touchmonitors. 


This readme file is organized as follows:

  1. Supported Touchmonitors and Elo Touchscreen Controllers
  2. System Requirements
  3. Installing the Elo Touchscreen Serial Driver 
  4. Calibrating the Touchscreen 
  5. Retrieving Calibration Values from NVRAM (Non Volatile Random Access Memory) / Pre-Calibration (Optional)   
  6. Accessing the Control Panel 
  7. Adapting Touchscreen Calibration to Video Rotation (Optional) 
  8. Uninstalling the Elo Touchscreen Serial Driver
  9. Troubleshooting
  10. Contacting Elo Touch Solutions



==========================================================
1. Supported Touchmonitors and Elo Touchscreen Controllers
==========================================================

 - All Elo Entuitive brand touchmonitors with an internal serial         
   controller

 - Elo Serial Controllers (IntelliTouch(R) 2500S, 2310B, 2310, 2300, 2701S  
                           CarrollTouch(R) 4000S, 4500S
                           AccuTouch(R)    2210, 2216) 



======================
2. System Requirements
======================

 - 32 bit Intel i686 (x86) platforms only   [ 64 bit AMD/Intel x86_64 platform - Visit the Linux downloads section at www.elotouch.com ]          

 - Kernels supported: 
    Kernel version 2.4.18 and later
    Kernel version 2.6.xx
    Kernel version 3.x.xx

 - XFree86/Xorg supported: 
    Xfree86 version 4.3.0 
    Xorg version 6.8.2 - 7.2
    Xorg (Xserver) version 1.3 - 1.9
    Xorg (Xserver) version 1.10 and later

 - Motif versions supported:
    Motif version 3.0 (libXm.so.3)
    Motif version 4.0 (libXm.so.4)


Note:
=====

The same binaries folder can be used for installation on other machines using 
the same kernel.



===============================================
3. Installing the Elo Touchscreen Serial Driver 
===============================================

Important:
==========
a.) Must have administrator access rights on the Linux machine to         
    install the Elo Touchscreen Serial Driver. 
 
b.) Ensure all earlier Elo drivers are uninstalled from the system. 
    Follow the uninstallation steps from the old driver's readme.txt 
    file to remove the old driver completely.   

c.) Do not extract the downloaded binary package on a Windows system.

d.) The environment should be setup for building kernel modules. 
    Check if necessary packages like gcc, make, kernel header files
    or kernel sources are installed. Refer to other documentation on 
    setting up the system for building loadable kernel modules.

e.) Motif 3.0(libXm.so.3) or 4.0(libXm.so.4) library is required to 
    perform standard touchscreen calibration (elova) and use the 
    graphical control panel (cpl). Openmotif or lesstif or libmotif 
    installation packages provide the required libXm.so.3 or 
    libXm.so.4 library.




Step I:
-------

Copy the elo driver files from the binary folder to the default elo folder. Compile the kernel module and copy it to the default elo folder. Change the permissions for all the elo driver files. Copy and place the X display Elo component file in the proper location.

  a.) Copy the driver files to /etc/opt/elo-ser folder location.

       # cp -r ./bin-serial/  /etc/opt/elo-ser
       # cd /etc/opt/elo-ser


  b.) Compile and build the kernel module "elok_s.ko" or "elok_s.o"
      using the compile instructions in readme_compile.txt. Check 
      if the kernel module compiles without any errors and then copy it  
      to /etc/opt/elo-ser/setup/ folder. 

       # cd /etc/opt/elo-ser/elok_s-source

       <Use the readme_compile.txt instructions to generate the kernel 
        module and copy it to /etc/opt/elo-ser/setup/ folder.>


      Proceed with the rest of the installation instructions.


  c.) Use the chmod command to set full permissions for all the             
      users.(read/write/execute) 

       # cd /etc/opt/elo-ser
       # chmod 777 *


  d.) Copy and place the X display Elo component file in the proper
      location. Use "# X -version" command to check the X display
      version.


       For Xorg (Xserver) version 1.10 or later: (example: Ubuntu 11.04)
           
         Skip to the next step. You do not need any elo_drv.so modules. A new X display 
         component present in the /etc/opt/elo-ser/setup folder, eloxevent will be launched 
         using the /etc/rc.local file.  

       (or)

       For Xorg (Xserver) version 1.9 or later: (example: Ubuntu 10.10)
           
         # cp  /etc/opt/elo-ser/setup/elo_drv.so_1.9  /usr/lib/xorg/modules/input/elo_drv.so

       (or)

       For Xorg (Xserver) version 1.8 or later: (example: Fedora 13)
           
         # cp  /etc/opt/elo-ser/setup/elo_drv.so_1.8  /usr/lib/xorg/modules/input/elo_drv.so

       (or)

       For Xorg (Xserver) version 1.7 or later: (example: Ubuntu 10.04)
           
         # cp  /etc/opt/elo-ser/setup/elo_drv.so_1.7  /usr/lib/xorg/modules/input/elo_drv.so

       (or)

       For Xorg (Xserver) version 1.6 or later: (example: Ubuntu 9.04)
           
         # cp  /etc/opt/elo-ser/setup/elo_drv.so_1.6  /usr/lib/xorg/modules/input/elo_drv.so

       (or)

       For Xorg (Xserver) version 1.5 or later: (example: Ubuntu 8.10)
           
         # cp  /etc/opt/elo-ser/setup/elo_drv.so_1.5  /usr/lib/xorg/modules/input/elo_drv.so

       (or)

       For Xorg (Xserver) version 1.4 or later: (example: Ubuntu 8.04)
           
         # cp  /etc/opt/elo-ser/setup/elo_drv.so_1.4  /usr/lib/xorg/modules/input/elo_drv.so

       (or)

       For Xorg (Xserver) version 1.3: (example: Ubuntu 7.10)
           
         # cp  /etc/opt/elo-ser/setup/elo_drv.so_1.3  /usr/lib/xorg/modules/input/elo_drv.so

       (or)

       For Xorg version 7.2 to 7.3 using Xserver version 1.2: (example: Ubuntu 7.04)
           
         # cp  /etc/opt/elo-ser/setup/elo_drv.so_7.2  /usr/lib/xorg/modules/input/elo_drv.so

       (or)

       For Xorg version 7.0 to 7.1.1: (example: Fedora Core 5)
           
         # cp  /etc/opt/elo-ser/setup/elo_drv.o_4.3  /usr/lib/xorg/modules/input/elo_drv.o

       (or)

       For other XFree86 or Xorg versions: (example: XFree86 4.3 or Xorg 6.8.2) 

         # cp  /etc/opt/elo-ser/setup/elo_drv.o_4.3  /usr/X11R6/lib/modules/input/elo_drv.o


Note: 
=====
 
If the Elo X input file(elo_drv.o_x.x) specified above does not work for the Xorg Xserver version, try to use the next closest version and see if that works. For example, some Linux distributions that use Xorg v7.1.1 may work with the elo_drv.so_7.2 file instead of the elo_drv.o_4.3 file. See the "/var/log/Xorg.0.log" file for details of the Elo Input Device section and corresponding Elo component's X Input protocol version. The Elo X input component's Input Protocol version should match the X Input Protocol supported by the Xorg Xserver for the driver to work properly. 




Step II:
--------

For newer Linux distributions using Xorg (Xserver) version 1.10 or later (example: Ubuntu 11.04), skip to the next step. You do not need to make any modifications to the X Windows configuration file. A new X display component present in the /etc/opt/elo-ser/setup folder, eloxevent will be launched using the /etc/rc.local file.  

For older Xorg (Xserver) versions, modify the X windows configuration file (xorg.conf or XF86Config or XF86Config-4). This file is located in the /etc/X11 directory. Check the X windows log file ("/var/log/XFree86.0.log" or "/var/log/Xorg.0.log") to verify the X windows configuration file in use. 

  a.) Add the following lines to create a new Elo device                    
      configuration at the end of the file.
   
        Section "InputDevice"
           Identifier "elo"
           Driver "elo"
           Option "Device" "/dev/input/elo_ser"
           Option "SendCoreEvents" "true"          [If the keyword "true" does not work for a Xorg version, try to use the keyword "on" instead of "true".]
        EndSection


  b.) Add the following line to the ServerLayout section to include         
      the elo input device.
         
        InputDevice "elo"


Note: 
=====
 
The modifications to the xorg.conf file are the same for configuring single or multiple serial touchscreens. Only one "InputDevice" section is necessary for handling all serial Elo touchscreens while using this Elo Serial Linux driver. Do NOT create separate "InputDevice" sections for each serial touchscreen like the Xorg elographics touchscreen driver (Xorg public driver).

Starting in Xorg 1.7 certain Linux distributions (e.g. Ubuntu) uses auto-probing to detect the current hardware without requiring a copy of xorg.conf. There are 2 possible options in this case. 

Option 1:
---------
Copy a file "70-elo_serial.conf" containing the necessary X Windows Input device configuration for Elo Serial Touchscreen to /usr/share/X11/xorg.conf.d/ folder. In this case there is no need to generate a new xorg.conf file and make manual modifications as specified in Step 2. 

  # cp /etc/opt/elo-ser/setup/70-elo_serial.conf /usr/share/X11/xorg.conf.d/

Option 2:
---------
Generate a new xorg.conf file and make manual modifications as specified in Step 2. To generate a copy of xorg.conf based on the current hardware configuration, boot into single user mode and run

  # X -configure

A copy of xorg.conf based on the current hardware configuration will be generated at /root/xorg.conf.new or /xorg.conf.new.




Step III:
---------

Confirm whether the "/dev/input" directory exists. If it does not exist, create a new directory.
           
  # mkdir -p /dev/input




Step IV:
--------
 
Install the elocontrol(elok_S) module by running the following command.

  # cd /etc/opt/elo-ser/setup/
  # ./install.sh




Step V:
-------

Install a script to invoke Elo drivers at system startup. See Note 2 below for configuring touch driver on multi-video setup [ATI Big Desktop, Nvidia Twinview, Xorg Xinerama and Xorg Separate X screens(non-Xinerama) are supported].


Redhat, Fedora, Mandrake, Slackware and Ubuntu (6.10 or later) systems:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

On the above distributions, there are some restrictions for running the elorc script from /etc/rc.d/rc*.d directory. Hence, add the following two lines at the end of daemon configuration script in "/etc/rc.local" file. 

[ rc.local file might also be at location /etc/rc.d/rc.local. Use the   
  "# find /etc -name rc.local" command to locate the rc.local file.]

  /etc/opt/elo-ser/setup/loadelo  
  /etc/opt/elo-ser/eloser <PORTNAME1> <PORTNAME2> ...  [See Note 1 below for <PORTNAME>]

For newer Linux distributions using Xorg (Xserver) version 1.10 or later (example: Ubuntu 11.04), add the following line too, to launch the new X display component /etc/opt/elo-ser/setup/eloxevent after the driver eloser is launched in the /etc/rc.local file.  

  /etc/opt/elo-ser/setup/eloxevent --xwarppointer 




SUSE Systems:
- - - - - - -

Add the following two lines at the end of the configuration script in "/etc/init.d/boot.local" file.

  /etc/opt/elo-ser/setup/loadelo
  /etc/opt/elo-ser/eloser <PORTNAME1> <PORTNAME2> ...  [See the note below for <PORTNAME>]

For newer Linux distributions using Xorg (Xserver) version 1.10 or later (example: OpenSuse 12.1), add the following line too, to launch the new X display component /etc/opt/elo-ser/setup/eloxevent after the driver eloser is launched in the "/etc/init.d/boot.local" file.  

  /etc/opt/elo-ser/setup/eloxevent --xwarppointer 


Note 1:
=======
 
Replace <PORTNAME> in the command /etc/opt/elo-ser/eloser <PORTNAME> with one of the following names based on where the touch input is connected.

  ttyS0 : for /dev/ttyS0
  ttyS1 : for /dev/ttyS1
  ttyS2 : for /dev/ttyS2 ,etc.


Example: The modified lines for ttyS1 and ttyS2 (2 touchscreens) should be, 
  ...
  /etc/opt/elo-ser/setup/loadelo
  /etc/opt/elo-ser/eloser ttyS1 ttyS2
  ...   


Note 2: Information for Multi-Video setup [ATI Big Desktop, Nvidia Twinview, Xorg Xinerama and Xorg Separate X screens(non-Xinerama) are supported]. 
=========================================
 
Use the following example to configure multi-video setup based on ATI Big Desktop, Nvidia Twinview, Xorg Xinerama mode.

  ...
  /etc/opt/elo-ser/setup/loadelo
  /etc/opt/elo-ser/eloser --twinview ttyS1 ttyS2                [Driver option --twinview has to be enabled]
  /etc/opt/elo-ser/setup/eloxevent --xwarppointer                
  ...  

 
Use the following example to configure multi-video setup based on Xorg Separate X screens (non-Xinerama) mode.

  ...
  /etc/opt/elo-ser/setup/loadelo
  /etc/opt/elo-ser/eloser ttyS1 ttyS2
  /etc/opt/elo-ser/setup/eloxevent --xwarppointer               [eloxevent option --xwarppointer has to be enabled]
  ...   



Debian, Ubuntu (prior to 6.10) systems:
- - - - - - - - - - - - - - - - - - - -
 
Copy the elorc script file present in the /etc/opt/elo-ser/setup/ directory to the /etc/init.d directory.
 
  # cp /etc/opt/elo-ser/setup/elorc /etc/init.d 

A symbolic link for the elorc script has to be created in the desired runlevel directory (example: rc2.d,rc3.d,....rc5.d). This will allow the elorc script to run at system startup. Ubuntu and Debian systems use runlevel 2 (rc2.d directory) as default.

This directory has startup files (symbolic links) of the form SDDxxxx where DD is the sequence number. Pick a sequence number XX which is at least one smaller than the sequence number of the display manager script (xdm, gdm, etc.) found in this directory. 

Use the maintainer script update-rc.d to create the elorc symbolic link with selected sequence number XX.

  # cd /etc/rc2.d 
  # update-rc.d elorc start XX 2 .


Important: 
==========

 - Only use the update-rc.d maintainer script to modify these        
   symbolic links. The elorc script will not be run at startup
   if these symbolic links are  manually created. 

 - Notice that the update-rc.d command syntax has a space and            
   period after the run-level parameter. 

 - The above example is for runlevel 2. Pick the appropriate folder      
   for the desired runlevel. The default runlevel can be found in        
   the /etc/inittab file.


Note:
===== 

The path of the runlevel directories might vary from distribution to distribution. The path for runlevel 5 in Redhat is "/etc/rc.d/rc5.d" while the path for Debian and Ubuntu is "/etc/rc2.d" for runlevel 2.

Locate the corresponding runlevel directory in the system and create the symbolic link for elorc script file in that directory using the update-rc.d maintainer script.
   



Step VI:
--------
 
Debian, Ubuntu (prior to 6.10) systems: (Other Linux systems skip to step VII)
- - - - - - - - - - - - - - - - - - - -

Edit the "/etc/init.d/elorc" daemon configuration script file that was created in Step V. Check and modify the <PORTNAME> in the command /etc/opt/elo-ser/eloser ttyS0 present in the "start" section of the script. The default <PORTNAME> in the elorc file is ttyS0 corresponding to the serial device /dev/ttyS0. 

Replace <PORTNAME> in the command /etc/opt/elo-ser/eloser ttyS0 with one of the following names based on where the touch input is connected.

  ttyS0 : for /dev/ttyS0
  ttyS1 : for /dev/ttyS1
  ttyS2 : for /dev/ttyS2 ,etc.


Example: The modified lines for ttyS1 and ttyS2 (2 touchscreens) should be, 
  ...
  /etc/opt/elo-ser/setup/loadelo
  /etc/opt/elo-ser/eloser ttyS1 ttyS2
  ...   


For newer Linux distributions using Xorg (Xserver) version 1.10 or later (example: Debian Wheezy), add the following line too, to launch the new X display component /etc/opt/elo-ser/setup/eloxevent after the driver eloser is launched in the "/etc/init.d/elorc" daemon configuration script file.  

  /etc/opt/elo-ser/setup/eloxevent --xwarppointer 




Step VII:
---------
 
Reboot the system to complete the driver installation process.
 
  # shutdown -r now 




==============================
4. Calibrating the Touchscreen
==============================

Important:
==========

a.) Users must have read and write access to "/dev/elo-ser" folder to perform the touchscreen calibration.

b.) If the standard calibration program(elova) is not working as expected, try the lighter version (elovaLite) instead. This utility has limited functionality but should work properly on most window managers. The option to store calibration data to NVRAM and calibration verification procedure(see Step II) are not available in elovaLite. Use the command "# /etc/opt/elo-ser/elovaLite --help" to see the available options and usage information. 


Step I:
-------

Run the calibration program from a command window in X Windows from the /etc/opt/elo-ser directory for a single monitor or multiple monitor setup.  

  # cd /etc/opt/elo-ser
  # ./elova --nvram       

The '--nvram' or '--precal' option writes the calibration data to the NVRAM on the monitor and the configuration file on the hard disk. To perform the calibration and update only the configuration file on the hard disk, use the command shown below.    

  # cd /etc/opt/elo-ser
  # ./elova

To view all the available options and specific usage for elova calibration program, use the command shown below. 

  # cd /etc/opt/elo-ser
  # ./elova --help        
   

        
Step II:
--------

Touch the targets from a position of normal use. The calibration targets have a default timeout of 30 seconds, which can be disabled or modified using the "--caltargettimeout" commandline option. If a timeout occurs the calibration program will move on to the next available video screen.

Once the calibration is over, the new calibration values will be applied temporarily so that the calibration can be verified. A dialog box will be displayed to either accept the new calibration or to redo the calibration. The dialog box has a fixed timeout period of 30 seconds. If a timeout occurs, previous calibration values will be restored and the calibration program will move on to the next available video screen.

If the new calibration values are not acceptable, select the redo button to restart the calibration process again. The calibration values computed during the previous calibration will be discarded.

Select the accept button, if the calibration is good. The calibration data is written to the configuration file and the NVRAM based on the command line parameters. The calibration program will move on to the next available video screen. The new calibration values are available for use immediately after the calibration.




=============================================================
5. Retrieving Calibration Values from NVRAM / Pre-Calibration  (Optional) 
=============================================================

Important:
==========

A valid calibration must exist in the monitor NVRAM (Non Volatile Random Access Memory) to use this function. Users must first perform the standard touchscreen calibration using elova (not elovaLite) and write the calibration values to the monitor NVRAM. The existing values in the NVRAM will be lost as only one set of calibration values can be stored in the NVRAM. Hence ensure that the current NVRAM calibration values can be overwritten before performing a new calibration and writing to the NVRAM.  


Option I: [Manual Option]
---------

To retrieve the calibration values from the NVRAM immediately, run the program 'eloautocalib' from a command window in X Windows from the /etc/opt/elo-ser directory. The command line option '--renew' enables the reading of the calibration values from monitor NVRAM and overwriting the current values in the configuration file on the hard disk.    

  # cd /etc/opt/elo-ser
  # ./eloautocalib --renew    

To view all the available options and specific usage for eloautocalib program, use the command shown below. 

  # cd /etc/opt/elo-ser
  # ./eloautocalib --help    



Option II: [Automatic Option]
----------

Copy the xEloInit.sh script file present in the /etc/opt/elo-ser/setup directory to the /etc/X11/xinit/xinitrc.d/ directory. If the destination "xinitrc.d" does not exist (example: Ubuntu), edit the /etc/X11/xinit/xinitrc script file and add a line to invoke the /etc/opt/elo-ser/setup/xEloInit.sh script file.
 
  # cp /etc/opt/elo-ser/setup/xEloInit.sh /etc/X11/xinit/xinitrc.d/ 


To retrieve the calibration values from the NVRAM automatically on system
startup, enable the 'eloautocalib' entry in the 'xEloInit.sh' script file located
in the '/etc/X11/xinit/xinitrc.d/' directory. The eloautocalib entry is
commented out by default and does not load the calibration values from monitor
NVRAM. Uncomment the entry '/etc/opt/elo-ser/eloautocalib --renew' to enable
reading the calibration values from monitor NVRAM and overwriting the current
values in the configuration file on the hard disk during system startup. 

Default:  '# /etc/opt/elo-ser/eloautocalib --renew'  - Does not load calibration values from NVRAM

Modified: '/etc/opt/elo-ser/eloautocalib --renew'    - Loads calibration values from NVRAM  




==============================
6. Accessing the Control Panel 
==============================

The control panel application allows the user to easily set the available driver configuration options. After the driver package is installed, change to the /etc/opt/elo-ser directory and run control panel application. 


Important:
==========

Users must have read and write access to "/dev/elo-ser" folder to run the control panel applications.


Step I:
-------

Run the control panel program from a command window in X Windows from the /etc/opt/elo-ser directory. Motif version 3.0(libXm.so.3) or version 4.0(libXm.so.4) is required to use GUI Control Panel (/etc/opt/elo-ser/cpl). 

  # cd /etc/opt/elo-ser
  # ./cpl 


Step II:
--------

Navigate through the various tabs by clicking on them. Here is an overview of information related to each tab.

  General	- Perform standard touchscreen calibration using elova (not elovaLite)
  Mode		- Change the touchscreen mode
  Sound		- Change Beep on Touch Parameters (Enable/Disable Beep, Beep Tone, Beep Duration)	
  Touchscreen-1	- Display data related to the Serial touchscreen 1.
  Touchscreen-2	- Display data related to the Serial touchscreen 2. (multiple touchscreen setup)
  About		- Information about the package. Click on the Readme button to open this readme file. 
	

Step III:
---------

If Motif is not installed, use the command line version of the application to access the control panel. Run the command line application from a command window in X Windows from the /etc/opt/elo-ser directory.

  # cd /etc/opt/elo-ser
  # ./cplcmd



=====================================================
7. Adapting Touchscreen Calibration to Video Rotation  (Optional) 
=====================================================

Important:
==========

Users must setup the touchscreen rotation support using the /etc/opt/elo-ser/rotate/setup_rotation.sh script. This script must be run in normal video mode without any rotation.  


Step I:
-------

Run the setup_rotation.sh script from a command window in X Windows from the /etc/opt/elo-ser/rotate/ directory. The script will invoke elova calibration utility to perform an accurate calibration in normal video mode. It then saves this normal video mode touchscreen configuration file as /etc/opt/elo-ser/rotate/SerialConfigData_rotate_normal.Once this file is created, the touchscreen rotation feature can be used.   

  # cd /etc/opt/elo-ser/rotate
  # ./setup_rotation.sh 

As an option, if you like the current calibration and do not wish to run calibration utility again, you can manually copy the /etc/opt/elo-ser/SerialConfigData file and rename it as /etc/opt/elo-ser/rotate/SerialConfigData_rotate_normal. Proceed to the next step.


Step II:
--------

Use the elorotate utility to adapt the touchscreen calibration to the current video rotation mode.

 # cd /etc/opt/elo-ser/rotate
 # ./elorotate --help      [ Lists the utility description, usage and options available ]
 # ./elorotate --left      [ Adapts the touchscreen calibration for LEFT video rotation ]    
 # ./elorotate --right     [ Adapts the touchscreen calibration for RIGHT video rotation ]
 # ./elorotate --inverted  [ Adapts the touchscreen calibration for INVERTED video rotation ]
 # ./elorotate --normal    [ Restores the touchscreen calibration to NORMAL video rotation ]

The utility can be used to adjust the touchscreen calibration after or before the video rotation is performed using the command line options. Another option would be to use a script file (example: /etc/opt/elo-ser/rotate/elo_video_rotate.sh) to perform both the video and touchscreen rotation at the same time. Please edit the script file to suit the target system.

 # cd /etc/opt/elo-ser/rotate
 # ./elo_video_rotate left   [ Rotate video and touchscreen to the LEFT ]
 # ./elo_video_rotate normal [ Restore video and touchscreen to normal mode ]



=================================================
8. Uninstalling the Elo Touchscreen Serial Driver
=================================================


Important:
==========
Must have administrator access rights on the Linux machine to uninstall the Elo Touchscreen Serial Driver. 


Step I:
-------

Delete the script or commands that invoke Elo service at startup.  


SUSE systems: 
- - - - - - -
Remove the following entries created in Step V of Installation section from the configuration script in"/etc/init.d/boot.local" file.

  /etc/opt/elo-ser/setup/loadelo
  /etc/opt/elo-ser/eloser ttyS0
  /etc/opt/elo-ser/setup/eloxevent --xwarppointer 


Redhat, Fedora, Mandrake, Slackware and Ubuntu (6.10 or later) systems:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Remove the following entries created in Step V of Installation section from the configuration script in "/etc/rc.local" file. (or "/etc/rc.d/rc.local" file)
        
  /etc/opt/elo-ser/setup/loadelo
  /etc/opt/elo-ser/eloser ttyS0
  /etc/opt/elo-ser/setup/eloxevent --xwarppointer 


Debian, Ubuntu (older than 6.10) systems:
- - - - - - - - - - - - - - - - - - - - -

a.) Remove the symbolic link file created in Step V of Installation     
    section using the update-rc.d maintainer script.
 
      # update-rc.d -f elorc remove


b.) Remove the elo script file "elorc" placed in the "/etc/init.d"        
    directory.

      # rm /etc/init.d/elorc



Step II:
--------

Delete the following lines from X windows configuration file (xorg.conf or XF86Config or XF86Config-4). This file is located in the /etc/X11 directory. Check the X windows log file ("/var/log/XFree86.0.log" or "/var/log/Xorg.0.log") to verify the X windows configuration file in use. 

  a.) Delete Elo device configuration section
       
      Section "InputDevice"
        Identifier "elo"
        Driver "elo"
        Option "Device" "/dev/input/elo_ser"
        Option "SendCoreEvents" "true"
      EndSection

   b.) Delete the elo device from the ServerLayout section
      
       InputDevice "elo"


Delete the "70-elo_serial.conf" configuration file if it was used, instead of the manual modifications shown above to xorg.conf file.  

      # rm /usr/share/X11/xorg.conf.d/70-elo_serial.conf  



Step III:
---------

Delete all the elo driver files from the system.

  a.) Delete the elo driver folder.

        # cd /etc/opt
        # rm -rf /etc/opt/elo-ser


  b.) Delete X display elo component. Use "# X -version" command to  
      check the X display version.

        For Xorg version 7.2, Xorg (Xserver) versions 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9 or later: (example: Fedora 7)
        
          # rm /usr/lib/xorg/modules/input/elo_drv.so

        (or)

        For Xorg version 7.0 to 7.1.1: (example: Fedora Core 5)
        
          # rm /usr/lib/xorg/modules/input/elo_drv.o

        (or)

        For other XFree86 or Xorg versions: 
  
          # rm /usr/X11R6/lib/modules/input/elo_drv.o


  c.) Delete all the elo related folders.

        # rm -rf /dev/elo-ser
        # rm /dev/input/elo_ser



Step IV:
--------

Remove the installed "elok_s" driver module from the directory "/lib/modules/`uname -r`/kernel/drivers".

  For 2.4 Kernels:
    # cd /lib/modules/`uname -r`/kernel/drivers
    # rm -r elok_s.o 
    # depmod -a
 
  (or)  

  For 2.6 or 3.x Kernels:
    # cd /lib/modules/`uname -r`/kernel/drivers
    # rm -r elok_s.ko
    # depmod -a



Step V:
-------

Delete the script file, if present, that invokes the Elo Driver at startup. Remove the elo script file "xEloInit.sh" placed in the "/etc/X11/xinit/xinitrc.d/" directory. If the destination "xinitrc.d" does not exist (example: Ubuntu), edit the /etc/X11/xinit/xinitrc script file and remove the line (if present) that invokes /etc/opt/elo-ser/setup/xEloInit.sh script file.

    # rm /etc/X11/xinit/xinitrc.d/xEloInit.sh



Step VI:
--------

Reboot the system to complete the driver uninstallation process.
 
  # shutdown -r now 




==================
9. Troubleshooting
==================

A. Every time X Windows is reconfigured using XWindows configurator, 
   the X windows configuration file (xorg.conf or XF86Config) has to     
   be modified to include the elo device configurations mentioned in     
   Step II of the Installation section.

B. Hardware Check 
   --------------

   To verify that the touchmonitor is connected and functioning 
   correctly, type the following command in an command window 
   for serial port 1 (COM1).

     # od -h -w10 </dev/ttyS0 

   Similarly for serial port 2 (COM2) use,

     # od -h -w10 </dev/ttyS1 

   The screen should start displaying a lot of numbers when the
   screen is touched. The standard Elo protocol data packet is 
   identified by the distinctive appearance of "5455" as the 
   first byte pair of a 10 byte data packet. 

C. The display image should be centered on the screen and fill 
   as much of the visible area of the touchmonitor screen as 
   possible without image distortion. 

D. Incase of Mandrake Linux, use the xdm display manager instead of      
   "Mandrake Display Manager". This can be changed using the      
   "settings" from "display manager". 

E. Fedora 13 comes with a software called modem-manager that
   performs auto-probing of serial ports and it causes conflicts with eloser.
   If one or some of the touchscreens are not responding to touches,
   disable modem-manager by running the following command in a command window:

     # chmod a-x /usr/sbin/modem-manager

   Restart the computer and the affected touchscreen(s) should be responding.

F. If touch is not working as expected on a multi-video setup, check Note 2 in
   Step V of Installation section for information on configuring the touch driver
   for multi-video systems.




==================================
10. Contacting Elo Touch Solutions
==================================

Website: http://www.elotouch.com


E-mail: customerservice@elotouch.com


Mailing Address: 
----------------

  Elo Touch Solutions Inc
  1033 McCarthy Blvd.
  Milpitas, CA 95035
  USA

  Phone:   (800) 557-1458
           (650) 361-4800

  Fax:     (650) 361-4722

--------------------------------  

  Elo TouchSystems GmbH & Co. KG
  Haidgraben 6
  D-85521 Ottobrunn
  Germany
         
  Phone: +49 (0) 89/60822-0
  Fax:   +49 (0) 89/60822-150

--------------------------------  

  Elo TouchSystems, NV
  Diestsesteenweg 692
  B-3010 Kessel-Lo
  Belgium

  Phone: +32 (16) 35-2100
  Fax:   +32 (16) 35-2101




===============================================================================                  

                      Copyright (c) 2013 Elo Touch Solutions
 
                             All rights reserved.

===============================================================================

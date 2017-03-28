================================================================================                  

                  Elo Touchscreen Linux Driver - Serial v 3.3.2
       
                  Kernel module (elok_s) Compile Instructions 

--------------------------------------------------------------------------------
                      
                                 Version 3.3.2 
                                 June 07, 2013
                               Elo Touch Solutions

================================================================================ 


==================
System Requirement
==================

 Kernels supported: 
  Kernel version 2.4.18 and later
  Kernel version 2.6.xx
  Kernel version 3.x.xx


Note: 
=====

I.   You must have a kernel mode driver build environment setup on your 
     system. The environment includes kernel development tools(kernel 
     headers or kernel source, etc.) and standard development tools (gcc, 
     make, etc.) 

II.  For 2.4 kernel base the kernel sources is assumed to be present in
     /usr/src/linux. Modify the INCLUDE_DIR, in makefile_2.4 if the header 
     files are in any other location. 

III. The Elo touchscreen driver files should be located in the /etc/opt/elo-ser 
     folder and the kernel module (elok_s) source code should be located in 
     the /etc/opt/elo-ser/elok_s-source folder. 




=============================
Build Kernel mode Elo Driver 
=============================


A. For 2.4.xx kernels:
   ===================

   1.) Build the Elo kernel module (elok_s.o) using the command:

         # cd /etc/opt/elo-ser/elok_s-source 
         # make -f makefile_2.4
 
       The kernel module, elok_s.o should be now built and ready for use. 
       Please refer to the troubleshooting section if you get errors during
       compiling the kernel module.

     
   3.) Copy elok_s.o to the /etc/opt/elo-ser/setup/ folder on your machine, 
       and follow the main "readme.txt" file for installation instructions.  

         # cp /etc/opt/elo-ser/elok_s-source/elok_s.o /etc/opt/elo-ser/setup/



B. For 2.6.xx or 3.x.xx kernels:
   =============================

 
   1.) Build the Elo kernel module (elok_s.ko) using the command:
  
         # cd /etc/opt/elo-ser/elok_s-source
         # cp makefile_2.6 Makefile
         # make
 
       The kernel module, elok_s.ko should be now built and ready for use. 
       Please refer to the troubleshooting section if you get errors during
       compiling the kernel module.

     
   2.) Copy elok_s.ko to the /etc/opt/elo-ser/setup/ folder on your machine, 
       and follow the main "readme.txt" file for installation instructions.  

         # cp /etc/opt/elo-ser/elok_s-source/elok_s.ko /etc/opt/elo-ser/setup/




===============
Troubleshooting 
===============


1.) Make sure that the "_KERNEL_2_6_" definition in elocontrol.h is undefined 
    while compiling for 2.4 kernels.


2.) New Linux kernels have outdated the "config.h" header file. Replace the 
    outdated "config.h" header file with "autoconf.h" header file in 
    "elocontrol.h" and recompile the kernel module. 


3.) "MODULE_PARM" macro in 2.4 kernels is replaced with "module_param" macro in
    2.6 kernels. However, all the 2.6 kernels with version number less than 
    "2.6.17"have backward compatibility with "MODULE_PARM" macro. The 
    "MODULE_PARM" macro is completely deprecated from "2.6.17". The code uses 
    "module_param" for 2.6 kernels and "MODULE_PARM" for 2.4 kernels. However, 
    some of the initial version of 2.6 kernels(specific to some distributions) 
    may not support "module_param" macro. In that case use "MODULE_PARM" macro 
    instead of "module_param" macro. Make the corresponding changes to the 
    source code if necessary.




================================================================================
                 
                 Copyright (c) 2013 Elo Touch Solutions

                          All rights reserved.

================================================================================

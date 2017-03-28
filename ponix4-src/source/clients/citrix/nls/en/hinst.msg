#!/bin/sh
###############################################################################
#
#	Name:		Version.msg
#
#	$Id: //icaclient/unix13.0/client/unix/installer/scripts/en/Version.msg#1 $
#
#	Purpose:	Sets shell variables for the version of Client
#			to be installed.
#
#	Copyright 1996-2000, 2006, 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

# note: the INSTALLER_... strings should be in lowercase. The installer will then
# accept lowercase or uppercase versions of them as input.
INSTALLER_YES=yes
INSTALLER_NO=no
INSTALLER_QUIT=quit
INSTALLER_Y=y
INSTALLER_N=n
INSTALLER_Q=q
PACKAGE_LANGUAGE=en
DISP_PRODUCT_NAME="Citrix Receiver for Linux"

###############################################################################
#
#	Name:		util_tr.msg
#
#
#	Created:	October 1999
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/en/util_tr.msg#1 $
#
#	Purpose:	Defines language dependent lowercase and uppercase
#			alphabets for use in case conversion with 'tr'.
#
#	Copyright 1999 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

lowercase_letters=abcdefghijklmnopqrstuvwxyz
uppercase_letters=ABCDEFGHIJKLMNOPQRSTUVWXYZ

###############################################################################
#
#	Name:		signon.msg
#
#	Purpose:	Displays a sign-on message.  This module should come
#			quite early on in the script, so that the message is
#			displayed before any others, and is displayed promptly
#			rather than having to wait while the shell reads in
#			all those other functions.  Must appear after the
#			version number module.
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/en/signon.msg#1 $
#
#	Copyright 1996-2013 Citrix Systems, Inc.  All Rights Reserved.
#
###############################################################################


signon1="$DISP_PRODUCT_NAME $DISP_VERSION setup."

signon2=\
"Copyright 1996-2013 Citrix Systems, Inc. All rights reserved.\n\
Copyright (c) 1986-1997 RSA Security, Inc. All rights reserved.\n\
This software uses libraries from the FFmpeg project under the LGPLv2.1\n\
\n\
Citrix, Independent Computing Architecture (ICA), Program Neighborhood,\n\
MetaFrame, and MetaFrame XP are registered trademarks and Citrix Receiver,\n\
Citrix XenApp, XenDesktop, Citrix Presentation Server, Citrix Access Suite,\n\
and SpeedScreen are trademarks of Citrix Systems, Inc. in the United States\n\
and other countries.\n\
\n\
Microsoft, MS, MS-DOS, Outlook, Windows, Windows NT, and BackOffice are\n\
either registered trademarks or trademarks of Microsoft Corporation in\n\
the United States and other countries.\n\
\n\
All other Trade Names referred to are the Servicemark, Trademark,\n\
or Registered Trademark of the respective manufacturers."

###############################################################################
#
#	Name:		getyesno.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/en/getyesno.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Reads input to get a simple yes/no response from the
#			user.
#
#	Copyright 1996-1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


getyesno1="You must answer $INSTALLER_YES or $INSTALLER_NO "

getyesno2="default"

###############################################################################
#
#	Name:		calcspace.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/en/calcspace.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Functions to calculate the space required for an installation.
#
#	Copyright 1996-1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


calcspace1="\nChecking available disk space ..."

# both lines followed by size in K.
calcspace2a="\n\tDisk space available"
calcspace2b="\n\tDisk space required"

calcspace3="\n\
\n\
Insufficient disk space available.  Please make space available, or try\n\
installing on another file system.\n\
\n\
Press any key to continue: "

calcspace4="\n\
\n\
Continuing ..."

###############################################################################
#
#	Name:		inst_ica.msg
#
#	$Id: //icaclient/unix13.0/client/unix/installer/scripts/en/inst_ica.msg#1 $
#
#	Purpose:	To install the ICA client.
#
#	Copyright 1996-2004, 2007-2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instica1="Installation abandoned."

# installer destination directory goes after instica2 
instica2a="\nYou have chosen to install $DISP_PRODUCT_NAME $DISP_VERSION in"
instica2b="."

instica3="\nThis is not the default location. You must set environment variable\n\
ICAROOT to this location after installation has completed, for example:\n\n\
export ICAROOT="

instica4="\nProceed with installation? [default $INSTALLER_N]: "

instica5="$DISP_PRODUCT_NAME $DISP_VERSION installation abandoned."

instica6="Removal abandoned."

# installer source directory name goes between instica8a and instica8b
instica8a="\nThe directory"
instica8b="does not contain a valid installation of\n\
$DISP_PRODUCT_NAME $DISP_VERSION\n\
\n\
Removal Abandoned."

# existing installation name and version go between instica9a and instica9b
# existing installation directory goes between instica9b and instica9a
instica9a="\nYou have chosen to remove the"
instica9b="installation in"
instica9c=".\nThis will remove all software and configuration files in this directory.\n\
\n\
Do you want to continue with removal? [default $INSTALLER_N]: "

# ica installation directory name goes after instica10
instica10="Removing"
instica10a='$instica10 $ICAInstDir'

# installer destination directory name goes between instica11a and instica11b
instica11a="The directory"
instica11b="could not be removed. You must try removing this directory manually."

# installer destination directory name goes between instica12a and instica12b
instica12a="Directory"
instica12b="removed."

instica13="\nClient license successfully installed."
instica14="\nFailed to install client license."

instica15="\nThis installation can only be removed by its owner."
###############################################################################
#
#	Name:		dir_util.msg
#
#
#	Created:	28 May 1996
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/en/dir_util.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Functions for obtaining, checking and creating
#			the directories required for installation.
#
#	Copyright 1996-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


# the installer source directory and ']' goes between dirutil1a and dirutil1b.
dirutil1a="\n\
Enter the location of your $DISP_PRODUCT_NAME $DISP_VERSION CDROM\n\
or installable image [default"
dirutil1b="\nor type \"$INSTALLER_QUIT\" to abandon the installation: "

dirutil2="\nCannot access"

# the installer source directory goes between dirutil3a and dirutil3b.
dirutil3a="\nThe directory" 
dirutil3b="\ndoes not contain a $DISP_PRODUCT_NAME $DISP_VERSION package."

# the installer destination directory and ']' goes between dirutil4a and dirutil4b.
dirutil4a="\nPlease enter the directory in which $DISP_PRODUCT_NAME is to be installed.\n\
[default"
dirutil4b="\nor type \"$INSTALLER_QUIT\" to abandon the installation: "

dirutil5="\nThis directory already exists.\n\
You must choose a directory name that does not exist."

# the installer destination directory goes between dirutil6a and dirutil6b.
dirutil6a="\nShould I use the name"
dirutil6b="instead? [default $INSTALLER_Y]: "

# the existing installation's name and version go between dirutil7a and dirutil7b.
dirutil7a="\nAn installation of"
dirutil7b="\n\
already exists in this directory. Re-installing will\n\
overwrite that installation and all its configuration with \n\
$DISP_PRODUCT_NAME $DISP_VERSION.\n\
\n\
Do you want to proceed? [default $INSTALLER_N]: "

# the installer destination directory goes between dirutil8a and dirutil8b.
dirutil8a="\nCannot create directory\n\t"
dirutil8b="A file with this name already exists."

# the installer destination directory goes between dirutil9a and dirutil9b.
dirutil9a="\nThe parent directory"
dirutil9b="does not exist.\nDo you want to create it? [default $INSTALLER_Y]: "

# the installer destination directory goes between dirutil10a and dirutil10b.
dirutil10a="\nThe directory"
dirutil10b="\ndoes not contain a valid $DISP_PRODUCT_NAME\n\
installation."

# the (modified or removed) installer destination directory goes between 
# dirutil11a and dirutil11b.
dirutil11a="\nThe directory"
dirutil11b="\ndoes not exist."

# the (modified or removed) installer destination directory goes between 
# dirutil12a and dirutil12b.
dirutil12a="\nPlease enter the directory containing the\n\
$DISP_PRODUCT_NAME installation\n\
[default"
dirutil12b="\nor type \"$INSTALLER_QUIT\" to return to the menu: "

dirutil13="User $INSTALLER_QUIT operation"

###############################################################################
#
#	Name:		instpkgs.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/en/instpkgs.msg#1 $
#
#	Purpose:	Installs the required parts of the ICA Client.
#
#	Copyright 1996-2011 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instpkgs1="Installation proceeding..."

# instpkgs is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs2="Creating directory"
instpkgs2c='$instpkgs2 "$INST_DIR"'

# instpkgs3a is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs3a="\nCannot create directory"
instpkgs3b="\nInstallation aborted."

instpkgs4="Core package..."

instpkgs5="Setting file permissions..."

###############################################################################
#
# $Id: $
#
# Copyright 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

# Warn the user that only root gets USB
instusb1="USB support cannot be installed by non-root users. Run the installer as root to access this install option."

# Ask to install USB
instusb2="Do you want to install USB support? [default $INSTALLER_N]: "

# Inform USB was not chosen
instusb3="USB support not installed."
###############################################################################
#
#       Name:           setup-netscape.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/en/setup-netscape.msg#1 $
#
#       Purpose:        Installer text for integrating with Netscape 
#
#       Copyright 1999-2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_netscape1="Integrating with browsers..."

integrate_netscape2="\nCould not find a browser installation on your system.\n\
Is a browser installed? [default $INSTALLER_N]: "

integrate_netscape3="Browsers found."

integrate_netscape4="\nFound entries in browser configuration(s) from an earlier installation.\n\
Do you want these entries to point to the new installation? [default $INSTALLER_Y]: "

integrate_netscape5="\nIntegration complete."

integrate_netscape6="\nIntegration cancelled."

disintegrate_netscape1="\nBrowser configurations have entries referring to \
Citrix Receiver.\n\
Remove references to Citrix Receiver? [default $INSTALLER_Y]: "

disintegrate_netscape2="Browsers' references to Citrix Receiver removed.\n"
###############################################################################
#
#       Name:           setup-CDE.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/en/setup-CDE.msg#1 $
#
#       Purpose:        Installer text for integrating with CDE 
#
#       Copyright 2002, 2006, 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_CDE1="Do you want to integrate Citrix Receiver with CDE? [default $INSTALLER_Y]: "

integrate_CDE2="\nFound entries in CDE from an earlier installation.\n\
Do you want these entries to point to the new installation? [default $INSTALLER_Y]: "

integrate_CDE3="\nIntegration complete. Please logout and log back in to complete the integration."

integrate_CDE4="\nIntegration cancelled."

disintegrate_CDE1="\nCDE has entries referring to Citrix Receiver.\n\
Remove references to Citrix Receiver? [default $INSTALLER_Y]: "

disintegrate_CDE2="CDE's references to Citrix Receiver removed. Please logout and log back in to \
complete the removal.\n"

disintegrate_CDE3="\nRemoval cancelled."
###############################################################################
#
#       Name:           setup-dt.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/en/setup-dt.msg#1 $
#
#       Purpose:        Installer text for integrating with GNOME and KDE 
#
#       Copyright 2003, 2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_DT1="No GNOME or KDE directories were found, skipping integration."
integrate_DT2="Do you want to integrate Citrix Receiver with KDE and GNOME? [default $INSTALLER_Y]: "
integrate_DT3="\nFound KDE or GNOME desktop entries from an earlier installation.\n\
Do you want these entries to point to the new installation? [default $INSTALLER_Y]: "
###############################################################################
#
#       Name:           setup-gst.msg 
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/en/setup-gst.msg#1 $
#
#       Purpose:        Installer text for integrating with GStreamer
#
#       Copyright 2003, 2006, 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_GST1="No GStreamer directories were found, skipping GStreamer integration."
integrate_GST2="Do you want GStreamer to use the plugin from this client? [default $INSTALLER_Y]: "
integrate_GST3="\nFound GStreamer entry from an earlier installation.\n\
Do you want this entry to point to the new installation? [default $INSTALLER_Y]: "
integrate_GST4a="Could not create directory: "
integrate_GST4b=""
integrate_GST5a="Could not create link in: "
integrate_GST5b=""
###############################################################################
#
#	Name:		menus.msg
#
#
#	Created:	May 1996 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/en/menus.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Displays menus for ICA Client installer.
#
#	Copyright 1996-2013 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

menus1=\
"\n\
Select a setup option:\n\
\n\
  1.   Install $DISP_PRODUCT_NAME $DISP_VERSION\n\
  2.   Remove $DISP_PRODUCT_NAME $DISP_VERSION\n\
  3.   Quit $DISP_PRODUCT_NAME $DISP_VERSION setup\n"

menus2="Enter option number"

menus3="Invalid Entry"

menus4=\
"  4.   Install $DISP_PRODUCT_NAME $DISP_VERSION license\n"

menus5=\
"\nYou need to configure the client license before installing this client.\n\
Log on as super-user (root) and choose option 4.\n"

###############################################################################
#
#	Name:		install.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/en/install.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	The main (top-level) part of the ICA Client installer.
#			For correct operation this should be at the very
#			bottom of the script.
#
#	Copyright 1996-1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


install1="\nTo install $DISP_PRODUCT_NAME run the setupwfc program\n\
in the root directory of the CD-ROM.\n"

install2="\nFor further information see your $DISP_PRODUCT_NAME $DISP_VERSION manual.\n"

install3="Quitting $DISP_PRODUCT_NAME $DISP_VERSION setup."

install4="User install mode."
###############################################################################
#
#	Name:		usb_inst_ica.msg
#
#	$Id:$
#
#	Copyright 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

usbinstica1="Installation complete."
usbinstica2="Removal complete."
usbinstica3="You must log on as root to perform this installation."

# Instructions on how to install the ctxusb daemon into the init run levels
usbinstica4="It has not been possible to integrate the ctxusbd daemon into the init run\n\
levels.\n\n\
This is because none of the following commands were available:\n\
\t\t1: install_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
It is possible to use the following command to launch the ctxusbd daemon\n\
manually:\n\
\t\t/etc/init.d/ctxusbd start\n\n\
However, this will not be persistent after shutdown. To rectify this, either\n\
install one of the above commands and reinstall USB Support, or install the\n\
ctxusbd daemon into the init run levels manually.\n"

# Instructions on how to remove the ctxusb daemon from the init run levels
usbinstica5="It has not been possible to remove the ctxusbd daemon from the init run levels.\n\n\
This is because none of the following commands were available:\n\
\t\t1: remove_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
To rectify this, either install one of the above commands and run the removal\n\
script again, or remove the ctxusbd daemon from the init run levels manually.\n"

usbinstica6="\nYou can install $DISP_PRODUCT_NAME only if the $HOST_PRODUCT_NAME\n\
was originally installed as root."


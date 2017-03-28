#!/bin/sh
###############################################################################
#
#	Name:		Version.msg
#
#	$Id: //icaclient/unix13.0/client/unix/installer/scripts/de/Version.msg#1 $
#
#	Purpose:	Sets shell variables for the version of Client
#			to be installed.
#
#	Copyright 1996-2000, 2006, 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

# note: the INSTALLER_... strings should be in lowercase. The installer will then
# accept lowercase or uppercase versions of them as input.
INSTALLER_YES=ja
INSTALLER_NO=nein
INSTALLER_QUIT=beenden
INSTALLER_Y=j
INSTALLER_N=n
INSTALLER_Q=b
PACKAGE_LANGUAGE=de
DISP_PRODUCT_NAME="Citrix Receiver für Linux"

###############################################################################
#
#	Name:		util_tr.msg
#
#
#	Created:	October 1999
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/de/util_tr.msg#1 $
#
#	Purpose:	Defines language dependent lowercase and uppercase
#			alphabets for use in case conversion with 'tr'.
#
#	Copyright 1999 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

lowercase_letters=abcdefghijklmnopqrstuvwxyz\\344\\366\\374
uppercase_letters=ABCDEFGHIJKLMNOPQRSTUVWXYZ\\304\\326\\334

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
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/de/signon.msg#1 $
#
#	Copyright 2000-2013 Citrix Systems, Inc.  All Rights Reserved.
#
###############################################################################


signon1="$DISP_PRODUCT_NAME $DISP_VERSION-Setup."

signon2=\
"Copyright 1998-2013 Citrix Systems, Inc. Alle Rechte vorbehalten.\n\
Copyright (c) 1986-1997 RSA Security, Inc. Alle Rechte vorbehalten.\n\
Diese Software verwendet Bibliotheken des FFmpeg-Projekts unter LGPLv2.1\n\
\n\
Citrix, Independent Computing Architecture (ICA), Program Neighborhood,\n\
MetaFrame und MetaFrame XP sind eingetragene Marken und Citrix Receiver,\n\
Citrix XenApp, XenDesktop, Citrix Presentation Server, Citrix Access Suite\n\
und SpeedScreen sind Marken von Citrix Systems, Inc., in den USA und\n\
anderen Ländern.\n\
\n\
Microsoft, MS, MS-DOS, Outlook, Windows, Windows NT und BackOffice\n\
sind eingetragene Marken oder Marken der Microsoft Corporation in\n\
den USA und anderen Ländern.\n\
\n\
Alle anderen erwähnten Warenbezeichnungen sind Gebrauchsmuster, Marken\n\
oder eingetragene Marken der jeweiligen Hersteller."

###############################################################################
#
#	Name:		getyesno.msg
#
#
#	Created:	June 1998
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/de/getyesno.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Reads input to get a simple yes/no response from the
#			user.
#
#	Copyright 1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


getyesno1="Sie müssen mit $INSTALLER_YES oder $INSTALLER_NO antworten"

getyesno2="Standardwert"

###############################################################################
#
#	Name:		calcspace.msg
#
#
#	Created:	June 1998
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/de/calcspace.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Functions to calculate the space required for an installation.
#
#	Copyright 1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


calcspace1="\nVerfügbarer Plattenplatz wird überprüft ..."

# both lines followed by size in K.
calcspace2a="\n\tVerfügbarer Plattenplatz:"
calcspace2b="\n\tBenötigter Plattenplatz:"

calcspace3="\n\
\n\
Nicht genügend Plattenplatz verfügbar. Bitte geben Sie Plattenplatz frei\n\
oder versuchen Sie die Installation in einem anderen Dateisystem.\n\
\n\
Zur Fortsetzung drücken Sie eine beliebige Taste: "

calcspace4="\n\
\n\
Ausführung wird fortgesetzt ..."

###############################################################################
#
#	Name:		inst_ica.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/de/inst_ica.msg#1 $
#
#	Purpose:	To install the ICA client.
#
#	Copyright 1996-2004, 2007-2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instica1="Installation abgebrochen."

# installer destination directory goes after instica2 
instica2a="\nSie haben sich dafür entschieden, $DISP_PRODUCT_NAME $DISP_VERSION\n\
im Verzeichnis"
instica2b=" zu installieren."
instica3="\nDies ist nicht die Standardeinstellung.\n\
Sie müssen nach vollendeter Installation die Umgebungsvariable\n\
ICAROOT auf dieses Verzeichnis setzen. Beispiel:\n\n\
export ICAROOT="

instica4="\nMöchten Sie mit der Installation fortfahren? [Standardwert $INSTALLER_N]: "

instica5="$DISP_PRODUCT_NAME $DISP_VERSION-Installation abgebrochen."

instica6="Deinstallation abgebrochen."

# installer source directory name goes between instica8a and instica8b
instica8a="\nDas Verzeichnis"
instica8b="enthält keine gültige Installation von\n\
$DISP_PRODUCT_NAME $DISP_VERSION\n\
\n\
Deinstallation abgebrochen."

# existing installation name and version go between instica9a and instica9b
# existing installation directory goes between instica9b and instica9a
instica9a="\nSie haben sich dafür entschieden, die Installation von"
instica9b="im Verzeichnis"
instica9c="zu entfernen.\nDadurch werden alle Programm- und\n\
Konfigurationsdateien in diesem Verzeichnis entfernt.\n\
\n\
Möchten Sie mit der Deinstallation fortfahren? [Standardwert $INSTALLER_N]: "

# ica installation directory name goes before instica10
instica10="wird entfernt"
instica10a='$ICAInstDir $instica10'

# installer destination directory name goes between instica11a and instica11b
instica11a="Das Verzeichnis"
instica11b="konnte nicht entfernt werden. Sie müssen versuchen,\n\
dieses Verzeichnis manuell zu entfernen."

# installer destination directory name goes between instica12a and instica12b
instica12a="Verzeichnis"
instica12b="entfernt."

instica13="\nClientlizenz erfolgreich installiert."
instica14="\nClientlizenz konnte nicht installiert werden."

instica15="\nDiese Installation kann nur vom Besitzer entfernt werden."
###############################################################################
#
#	Name:		dir_util.msg
#
#
#	Created:	June 1998
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/de/dir_util.msg#2 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Functions for obtaining, checking and creating
#			the directories required for installation.
#
#	Copyright 1998-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


# the installer source directory and ']' goes between dirutil1a and dirutil1b.
dirutil1a="\n\
Geben Sie den Speicherort Ihrer $DISP_PRODUCT_NAME $DISP_VERSION CDROM\n\
oder des installierbaren Images [Standardwert"
dirutil1b="\nein oder $INSTALLER_QUIT ein, um die Installation abzubrechen: "

dirutil2="\nZugriff nicht möglich:"

# the installer source directory goes between dirutil3a and dirutil3b.
dirutil3a="\nDas Verzeichnis" 
dirutil3b="\nenthält kein $DISP_PRODUCT_NAME $DISP_VERSION-Paket."

# the installer destination directory and ']' goes between dirutil4a and dirutil4b.
dirutil4a="\nBitte geben Sie das Verzeichnis ein, in das der\n\ $DISP_PRODUCT_NAME \n\
installiert werden soll [Standardwert"
dirutil4b="\noder geben Sie $INSTALLER_QUIT ein, um die Installation abzubrechen: "

dirutil5="\nDieses Verzeichnis existiert bereits.\n\
Wählen Sie einen nicht existierenden Verzeichnisnamen aus."

# the installer destination directory goes between dirutil6a and dirutil6b.
dirutil6a="\nSoll stattdessen der Name"
dirutil6b="verwendet werden? [Standardwert $INSTALLER_Y]: "

# the existing installation's name and version go between dirutil7a and dirutil7b.
dirutil7a="\nIn diesem Verzeichnis existiert bereits eine Installation von"
dirutil7b=".\n\
Bei einer erneuten Installation wird die\n\
vorhandene Installation samt ihrer Konfiguration mit \n\
$DISP_PRODUCT_NAME $DISP_VERSION überschrieben.\n\
\n\
Möchten Sie fortfahren? [Standardwert $INSTALLER_N]: "

# the installer destination directory goes between dirutil8a and dirutil8b.
dirutil8a="\nDas Verzeichnis\n\t"
dirutil8b="kann nicht erstellt werden. Es existiert bereits eine Datei\n\
dieses Namens."

# the installer destination directory goes between dirutil9a and dirutil9b.
dirutil9a="\nDas übergeordnete Verzeichnis"
dirutil9b="existiert nicht.\nSoll es erstellt werden? [Standardwert $INSTALLER_Y]: "

# the installer destination directory goes between dirutil10a and dirutil10b.
dirutil10a="\nDas Verzeichnis"
dirutil10b="\nenthält keine gültige $DISP_PRODUCT_NAME-\n\
Installation."

# the (modified or removed) installer destination directory goes between 
# dirutil11a and dirutil11b.
dirutil11a="\nDas Verzeichnis"
dirutil11b="\nexistiert nicht."

# the (modified or removed) installer destination directory goes between 
# dirutil12a and dirutil12b.
dirutil12a="\nGeben Sie das Verzeichnis ein, das die\n\
$DISP_PRODUCT_NAME-Installation enthält\n\
[Standardwert"
dirutil12b="\noder geben Sie $INSTALLER_QUIT ein, um zum Menü zurückzukehren: "

dirutil13="Operation wurde vom Benutzer abgebrochen"

###############################################################################
#
#	Name:		instpkgs.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/de/instpkgs.msg#1 $
#
#	Purpose:	Installs the required parts of the ICA Client.
#
#	Copyright 1998-2011 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instpkgs1="Installation beginnt..."

# instpkgs is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
# and then followed by instpkgs.
instpkgs2a="Verzeichnis"
instpkgs2b="wird erstellt"
instpkgs2c='$instpkgs2a "$INST_DIR" $instpkgs2b'

# instpkgs3a is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs3a="\nVerzeichnis"
instpkgs3b="\nkann nicht erstellt werden. Installation abgebrochen."

instpkgs4="Core-Paket..."

instpkgs5="Setzen der Dateiberechtigungen..."

###############################################################################
#
# $Id: //icaclient/unix13.0/client/unix/installer/scripts/de/inst_usb.msg#1 $
#
# Copyright 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

# Warn the user that only root gets USB
instusb1="USB-Unterstützung kann nur von root-Benutzern installiert werden. Führen Sie den Installer als root aus, um diese Option installieren zu können."

# Ask to install USB
instusb2="Möchten Sie USB-Unterstützung installieren? [Standard $INSTALLER_N]: "

# Inform USB was not chosen
instusb3="USB-Unterstützung ist nicht installiert."
###############################################################################
#
#       Name:           setup-netscape.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/de/setup-netscape.msg#1 $
#
#       Purpose:        Installer text for integrating with Netscape 
#
#       Copyright 1999-2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_netscape1="Integration mit Browsern..."

integrate_netscape2="\nEs wurde kein installierter Browser auf dem System gefunden.\n\
Ist ein Browser installiert? [Standardwert $INSTALLER_N]: "

integrate_netscape3="Browser wurden gefunden."

integrate_netscape4="\nIn den Browserkonfiguration wurden Einträge von einer früheren Installation gefunden.\n\
Sollen diese Einträge auf die neue Installation verweisen? [Standardwert $INSTALLER_Y]: "

integrate_netscape5="\nIntegration abgeschlossen."

integrate_netscape6="\nIntegration abgebrochen."

disintegrate_netscape1="\nDie Browserkonfigurationen enthalten Einträge, die auf\n\
den Citrix Receiver verweisen.\n\
Sollen die Verweise auf den Citrix Receiver entfernt werden? [Standardwert $INSTALLER_Y]: "

disintegrate_netscape2="Die Verweise der Browser auf den Citrix Receiver wurden entfernt.\n"

###############################################################################
#
#       Name:           setup-CDE.msg 
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/de/setup-CDE.msg#1 $
#
#       Purpose:        Installer text for integrating with CDE 
#
#       Copyright 2002, 2006, 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_CDE1="Möchten Sie den Citrix Receiver in CDE integrieren? [Standardwert $INSTALLER_Y]: "

integrate_CDE2="\nEs wurden Einträge in CDE von einer früheren Installation gefunden.\n\
Möchten Sie diese Einträge auf die neue Installation verweisen? [default $INSTALLER_Y]: "

integrate_CDE3="\nIntegration beendet. Bitte melden Sie sich ab und anschließend wieder an, um die Integration abzuschließen."

integrate_CDE4="\nIntegration abgebrochen."

disintegrate_CDE1="\nCDE hat Einträge, die auf den Citrix Receiver verweisen.\n\
Verweise auf den Citrix Receiver entfernen? [Standardwert $INSTALLER_Y]: "

disintegrate_CDE2="CDE-Verweise auf den Citrix Receiver wurden entfernt. Bitte melden Sie sich ab und anschließend wieder an, \
um den Vorgang abzuschließen.\n"

disintegrate_CDE3="\nEntfernen abgebrochen."
###############################################################################
#
#       Name:           setup-dt.msg 
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/de/setup-dt.msg#1 $
#
#       Purpose:        Installer text for integrating with GNOME and KDE 
#
#       Copyright 2003, 2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_DT1="Es wurden keine GNOME- oder KDE-Verzeichnisse gefunden. Integration wird übersprungen."
integrate_DT2="Möchten Sie den Citrix Receiver in KDE und GNOME integrieren? [Standardwert $INSTALLER_Y]: "
integrate_DT3="\nEs wurden KDE- oder GNOME-Desktop-Einträge aus einer früheren Installation gefunden.\n\
Möchten Sie diese Einträge auf die neue Installation verweisen? [Standardwert $INSTALLER_Y]: "
###############################################################################
#
#       Name:           setup-gst.msg 
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/de/setup-gst.msg#1 $
#
#       Purpose:        Installer text for integrating with GStreamer
#
#       Copyright 2003, 2006, 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_GST1="Keine GStreamer-Verzeichnisse gefunden. GStreamer-Integration wird übersprungen."
integrate_GST2="Möchten Sie, dass GStreamer das Plugin von diesem Client verwendet? [Standardwert $INSTALLER_Y]: "
integrate_GST3="\nGStreamer-Eintrag einer früheren Installation gefunden.\n\
Möchten Sie, dass dieser Eintrag auf die neue Installation verweist? [Standardwert $INSTALLER_Y]: "
integrate_GST4a="Verzeichnis konnte nicht erstellt werden: "
integrate_GST4b=" "
integrate_GST5a="Hier konnte keine Verknüpfung erstellt werden: "
integrate_GST5b=" "
###############################################################################
#
#	Name:		menus.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/de/menus.msg#1 $
#
#	Purpose:	Displays menus for ICA Client installer.
#
#	Copyright 1998-2013 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

menus1=\
"\n\
Wählen Sie eine Setup-Option aus:\n\
\n\
  1.   $DISP_PRODUCT_NAME $DISP_VERSION installieren\n\
  2.   $DISP_PRODUCT_NAME $DISP_VERSION deinstallieren \n\
  3.   Setup von $DISP_PRODUCT_NAME $DISP_VERSION verlassen\n"

menus2="Geben Sie die Nummer der Option ein:"

menus3="Ungültige Eingabe"

menus4=\
"  4.   Lizenz für $DISP_PRODUCT_NAME $DISP_VERSION installieren\n"

menus5=\
"\nVor der Installation des Clients müssen Sie die Clientlizenz konfigurieren.\n\
Melden Sie sich als Super-User (root) an und wählen Sie Option 4.\n"

###############################################################################
#
#	Name:		install.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/de/install.msg#1 $
#
#	Purpose:	The main (top-level) part of the ICA Client installer.
#			For correct operation this should be at the very
#			bottom of the script.
#
#	Copyright 1998-2003 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


install1="\nRufen Sie zur Installation von $DISP_PRODUCT_NAME das\n\
Programm setupwfc im Hauptverzeichnis der CD-ROM auf.\n"

install2="\nWeitere Informationen entnehmen Sie Ihrem $DISP_PRODUCT_NAME $DISP_VERSION-Handbuch.\n"

install3="$DISP_PRODUCT_NAME $DISP_VERSION-Setup wird verlassen."

install4="Benutzerinstallationsmodus."
###############################################################################
#
#	Name:		usb_inst_ica.msg
#
#	$Id:$
#
#	Copyright 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

usbinstica1="Installation abgeschlossen."
usbinstica2="Deinstallation abgeschlossen."
usbinstica3="Sie müssen sich als root anmelden, um diese Installation ausführen zu können."

# Instructions on how to install the ctxusb daemon into the init run levels
usbinstica4="Der ctxusbd-Daemon konnte nicht in die init-\n\
Runlevels integriert werden.\n\n\
Der Grund hierfür ist, dass keiner der folgenden Befehle verfügbar war:\n\
\t\t1: install_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
Sie können den ctxusbd-Daemon mit dem folgenden Befehl\n\
manuell starten:\n\
\t\t/etc/init.d/ctxusbd start\n\n\
Diese Änderung bleibt nach dem Herunterfahren aber nicht bestehen. Um dies zu beheben,\n\
installieren Sie entweder einen der obigen Befehle und installieren Sie USB-Unterstützung\n\
neu oder installieren Sie den ctxusbd-Daemon manuell in die init-Runlevel.\n"

# Instructions on how to remove the ctxusb daemon from the init run levels
usbinstica5="Der ctxusbd-Daemon konnte nicht aus den init-Runlevels entfernt werden.\n\n\
Der Grund hierfür ist, dass keiner der folgenden Befehle verfügbar war:\n\
\t\t1: remove_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
Um dies zu beheben, installieren Sie entweder einen der obigen Befehle\n\
und führen Sie das Skript zum Entfernen erneut aus oder entfernen Sie den\n\
ctxusbd-Daemon manuell aus den init-Runlevels.\n"

usbinstica6="\nSie können $DISP_PRODUCT_NAME nur installieren, wenn der\n\
$HOST_PRODUCT_NAME vorher als Host installiert war."

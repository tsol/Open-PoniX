#!/bin/sh
###############################################################################
#
#	Name:		Version.msg
#
#	$Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/Version.msg#2 $
#
#	Purpose:	Sets shell variables for the version of Client
#			to be installed.
#
#	Copyright 1996-2013 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

# note: the INSTALLER_... strings should be in lowercase. The installer will then
# accept lowercase or uppercase versions of them as input.
INSTALLER_YES=oui
INSTALLER_NO=non
INSTALLER_QUIT=quitter
INSTALLER_Y=o
INSTALLER_N=n
INSTALLER_Q=q
PACKAGE_LANGUAGE=fr
DISP_PRODUCT_NAME="Citrix Receiver pour Linux"

###############################################################################
#
#	Name:		util_tr.msg
#
#
#	Created:	October 1999
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/util_tr.msg#1 $
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
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/signon.msg#1 $
#
#	Copyright 1996-2013 Citrix Systems, Inc.  All Rights Reserved.
#
###############################################################################


signon1="Installation de $DISP_PRODUCT_NAME $DISP_VERSION."

signon2=\
"Copyright 1996-2013 Citrix Systems, Inc. Tous droits réservés.\n\
Copyright (c) 1986-1997 RSA Security, Inc. Tous droits réservés.\n\
Ce logiciel utilise des bibliothèques du projet FFmpeg sous LGPLv2.1\n\
\n\
Citrix, Independent Computing Architecture (ICA), Program Neighborhood,\n\
MetaFrame et MetaFrame XP sont des marques déposées et Citrix Receiver,\n\
Citrix XenApp, XenDesktop, Citrix Presentation Server, Citrix Access Suite,\n\
et SpeedScreen sont des marques de commerce de Citrix Systems, Inc. aux États-Unis\n\
et dans d'autres pays.\n\
\n\
Microsoft, MS, MS-DOS, Outlook, Windows, Windows NT et BackOffice sont\n\
des marques déposées ou des marques de commerce de Microsoft Corporation aux\n\
États-Unis et dans d'autres pays.\n\
\n\
Tous les autres noms de marque référencés ici sont des marques de service, marques de commerce,\n\
ou marques déposées de leurs fabricants respectifs."

###############################################################################
#
#	Name:		getyesno.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/getyesno.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Reads input to get a simple yes/no response from the
#			user.
#
#	Copyright 1996-1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


getyesno1="Vous devez répondre par $INSTALLER_YES ou par $INSTALLER_NO "

getyesno2="valeur par défaut"

###############################################################################
#
#	Name:		calcspace.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/calcspace.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Functions to calculate the space required for an installation.
#
#	Copyright 1996-1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


calcspace1="\nVérification de l'espace disque disponible..."

# both lines followed by size in K.
calcspace2a="\n\tEspace disque disponible"
calcspace2b="\n\tEspace disque requis"

calcspace3="\n\
\n\
Espace disque disponible insuffisant. Libérez de l'espace ou essayez\n\
d'installer sur un autre système de fichiers.\n\
\n\
Appuyez sur une touche pour continuer : "

calcspace4="\n\
\n\
Poursuite de l'opération..."

###############################################################################
#
#	Name:		inst_ica.msg
#
#	$Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/inst_ica.msg#1 $
#
#	Purpose:	To install the ICA client.
#
#	Copyright 1996-2004, 2007-2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instica1="Installation abandonnée."

# installer destination directory goes after instica2 
instica2a="\nVous avez choisi d'installer $DISP_PRODUCT_NAME $DISP_VERSION dans"
instica2b="."

instica3="\nIl ne s'agit pas de l'emplacement par défaut. Vous devez définir une variable d'environnement\n\
ICAROOT sur cet emplacement une fois l'installation terminée, par exemple :\n\n\
export ICAROOT="

instica4="\nPoursuivre l'installation ? [default $INSTALLER_N] : "

instica5="Installation de $DISP_PRODUCT_NAME $DISP_VERSION abandonnée."

instica6="Suppression abandonnée."

# installer source directory name goes between instica8a and instica8b
instica8a="\nLe répertoire"
instica8b="ne contient pas une installation valide de\n\
$DISP_PRODUCT_NAME $DISP_VERSION\n\
\n\
Suppression abandonnée."

# existing installation name and version go between instica9a and instica9b
# existing installation directory goes between instica9b and instica9a
instica9a="\nVous avez choisi de supprimer"
instica9b="l'installation dans"
instica9c=".\nCela supprimera tous les logiciels et fichiers de configuration dans ce répertoire.\n\
\n\
Voulez-vous poursuivre la suppression ? [default $INSTALLER_N] : "

# ica installation directory name goes after instica10
instica10="Suppression"
instica10a='$instica10 $ICAInstDir'

# installer destination directory name goes between instica11a and instica11b
instica11a="Le répertoire"
instica11b="n'a pas pu être supprimé. Vous devez essayer de supprimer ce répertoire manuellement."

# installer destination directory name goes between instica12a and instica12b
instica12a="Répertoire"
instica12b="supprimé."

instica13="\nLicence client installée avec succès."
instica14="\nImpossible d'installer la licence client."

instica15="\nCette installation peut uniquement être supprimée par son propriétaire."
###############################################################################
#
#	Name:		dir_util.msg
#
#
#	Created:	28 May 1996
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/dir_util.msg#2 $
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
Entrez l'emplacement de votre $DISP_PRODUCT_NAME $DISP_VERSION CDROM\n\
ou d'une image d'installation [default"
dirutil1b="\nou tapez « $INSTALLER_QUIT » pour abandonner l'installation : "

dirutil2="\nImpossible d'accéder"

# the installer source directory goes between dirutil3a and dirutil3b.
dirutil3a="\nLe répertoire" 
dirutil3b="\nne contient pas de package $DISP_PRODUCT_NAME $DISP_VERSION."

# the installer destination directory and ']' goes between dirutil4a and dirutil4b.
dirutil4a="\nEntrez le répertoire dans lequel $DISP_PRODUCT_NAME doit être installé.\n\
[default"
dirutil4b="\nou tapez « $INSTALLER_QUIT » pour abandonner l'installation : "

dirutil5="\nCe répertoire existe déjà.\n\
Vous devez choisir un nom de répertoire qui n'existe pas."

# the installer destination directory goes between dirutil6a and dirutil6b.
dirutil6a="\nDois-je plutôt utiliser le"
dirutil6b="nom ? [default $INSTALLER_Y] : "

# the existing installation's name and version go between dirutil7a and dirutil7b.
dirutil7a="\nUne installation de"
dirutil7b="\n\
existe déjà dans ce répertoire. La réinstallation\n\
effacera cette installation et sa configuration avec \n\
$DISP_PRODUCT_NAME $DISP_VERSION.\n\
\n\
Voulez-vous continuer ? [default $INSTALLER_N]: "

# the installer destination directory goes between dirutil8a and dirutil8b.
dirutil8a="\nImpossible de créer le répertoire\n\t"
dirutil8b="Un fichier du même nom existe déjà."

# the installer destination directory goes between dirutil9a and dirutil9b.
dirutil9a="\nLe répertoire parent"
dirutil9b="n'existe pas.\nVoulez-vous le créer ? [default $INSTALLER_Y] : "

# the installer destination directory goes between dirutil10a and dirutil10b.
dirutil10a="\nLe répertoire"
dirutil10b="\nne contient pas une installation $DISP_PRODUCT_NAME\n\
valide."

# the (modified or removed) installer destination directory goes between 
# dirutil11a and dirutil11b.
dirutil11a="\nLe répertoire"
dirutil11b="\nn'existe pas."

# the (modified or removed) installer destination directory goes between 
# dirutil12a and dirutil12b.
dirutil12a="\nEntrez le répertoire contenant l'installation de\n\
$DISP_PRODUCT_NAME\n\
[default"
dirutil12b="\nou tapez « $INSTALLER_QUIT » pour revenir au menu : "

dirutil13="Opération $INSTALLER_QUIT par l'utilisateur"

###############################################################################
#
#	Name:		instpkgs.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/instpkgs.msg#1 $
#
#	Purpose:	Installs the required parts of the ICA Client.
#
#	Copyright 1996-2011 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instpkgs1="Installation en cours..."

# instpkgs is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs2="Création du répertoire"
instpkgs2c='$instpkgs2 "$INST_DIR"'

# instpkgs3a is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs3a="\nImpossible de créer le répertoire"
instpkgs3b="\nInstallation abandonnée."

instpkgs4="Pack de base..."

instpkgs5="Définition des autorisations de fichier..."

###############################################################################
#
# $Id: $
#
# Copyright 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

# Warn the user that only root gets USB
instusb1="La prise en charge USB ne peut pas être installée par des utilisateurs non racine. Exécutez le programme en tant que racine pour accéder à cette option d'installation."

# Ask to install USB
instusb2="Voulez-vous installer la prise en charge USB ? [default $INSTALLER_N] : "

# Inform USB was not chosen
instusb3="Prise en charge USB non installée."
###############################################################################
#
#       Name:           setup-netscape.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/setup-netscape.msg#1 $
#
#       Purpose:        Installer text for integrating with Netscape 
#
#       Copyright 1999-2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_netscape1="Intégration aux navigateurs..."

integrate_netscape2="\nImpossible de trouver une installation de navigateur sur votre système.\n\
Un  navigateur est-il installé ? [default $INSTALLER_N] : "

integrate_netscape3="Navigateurs trouvés."

integrate_netscape4="\nDes entrées provenant d'une version antérieure ont été trouvées dans les configurations de navigateur.\n\
Voulez-vous que ces entrées pointent vers la nouvelle installation ? [default $INSTALLER_Y] : "

integrate_netscape5="\nIntégration terminée."

integrate_netscape6="\nIntégration annulée."

disintegrate_netscape1="\nLes configurations de navigateur ont des entrées qui font référence à \
Citrix Receiver.\n\
Supprimer les références à Citrix Receiver ? [default $INSTALLER_Y] : "

disintegrate_netscape2="Les références à Citrix Receiver ont été supprimées des navigateurs.\n"
###############################################################################
#
#       Name:           setup-CDE.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/setup-CDE.msg#1 $
#
#       Purpose:        Installer text for integrating with CDE 
#
#       Copyright 2002, 2006, 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_CDE1="Voulez-vous intégrer Citrix Receiver à CDE ? [default $INSTALLER_Y] : "

integrate_CDE2="\nDes entrées provenant d'une version antérieure ont été trouvées dans CDE.\n\
Voulez-vous que ces entrées pointent vers la nouvelle installation ? [default $INSTALLER_Y] : "

integrate_CDE3="\nIntégration terminée. Fermez la session et rouvrez-la pour terminer l'intégration."

integrate_CDE4="\nIntégration annulée."

disintegrate_CDE1="\nDes entrées dans CDE font référence à Citrix Receiver.\n\
Supprimer les références à Citrix Receiver? [default $INSTALLER_Y] : "

disintegrate_CDE2="Les références à Citrix Receiver dans CDE ont été supprimées. Fermez la session et rouvrez-la pour \
terminer la suppression.\n"

disintegrate_CDE3="\nSuppression annulée."
###############################################################################
#
#       Name:           setup-dt.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/setup-dt.msg#1 $
#
#       Purpose:        Installer text for integrating with GNOME and KDE 
#
#       Copyright 2003, 2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_DT1="Aucun répertoire GNOME ou KDE n'a été trouvé, intégration ignorée."
integrate_DT2="Voulez-vous intégrer Citrix Receiver à KDE et GNOME ? [default $INSTALLER_Y] : "
integrate_DT3="\nDes entrées de bureau provenant d'une version antérieure ont été trouvées dans KDE ou GNOME.\n\
Voulez-vous que ces entrées pointent vers la nouvelle installation ? [default $INSTALLER_Y] : "
###############################################################################
#
#       Name:           setup-gst.msg 
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/setup-gst.msg#1 $
#
#       Purpose:        Installer text for integrating with GStreamer
#
#       Copyright 2003, 2006, 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_GST1="Aucun répertoire GStreamer n'a été trouvé, intégration de GStreamer ignorée."
integrate_GST2="Voulez-vous que GStreamer utilise le plug-in de ce client ? [default $INSTALLER_Y] : "
integrate_GST3="\nUne entrée provenant d'une version antérieure a été trouvée dans GStreamer.\n\
Voulez-vous que cette entrée pointe vers la nouvelle installation ? [default $INSTALLER_Y] : "
integrate_GST4a="Impossible de créer le répertoire : "
integrate_GST4b=""
integrate_GST5a="Impossible de créer le lien dans : "
integrate_GST5b=""
###############################################################################
#
#	Name:		menus.msg
#
#
#	Created:	May 1996 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/menus.msg#1 $
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
Sélectionnez une option d'installation :\n\
\n\
  1.   Installer $DISP_PRODUCT_NAME $DISP_VERSION\n\
  2.   Supprimer $DISP_PRODUCT_NAME $DISP_VERSION\n\
  3.   Quitter l'installation de $DISP_PRODUCT_NAME $DISP_VERSION\n"

menus2="Entrer un numéro d'option"

menus3="Entrée non valide"

menus4=\
"  4.   Installer la licence $DISP_PRODUCT_NAME $DISP_VERSION\n"

menus5=\
"\nVous devez configurer la licence client avant d'installer ce client.\n\
Ouvrez une session en tant que super utilisateur (root) et choisissez l'option 4.\n"


###############################################################################
#
#	Name:		install.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/install.msg#1 $
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


install1="\nPour installer $DISP_PRODUCT_NAME, exécutez le programme setupwfc\n\
dans le répertoire racine du CD-ROM.\n"

install2="\nPour de plus amples informations, consultez le manuel de votre $DISP_PRODUCT_NAME $DISP_VERSION.\n"

install3="Abandon de l'installation de $DISP_PRODUCT_NAME $DISP_VERSION."

install4="Mode d'installation utilisateur."
###############################################################################
#
#	Name:		usb_inst_ica.msg
#
#	$Id:$
#
#	Copyright 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

usbinstica1="Installation terminée."
usbinstica2="Suppression terminée."
usbinstica3="Vous devez ouvrir une session en tant que racine pour effectuer cette installation."

# Instructions on how to install the ctxusb daemon into the init run levels
usbinstica4="Impossible d'intégrer le démon ctxusbd aux niveaux d'exécution\n\
init.\n\n\
Ceci est dû au fait qu'aucune des commandes suivantes n'est disponible :\n\
\t\t1: install_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
Il est possible d'utiliser la commande suivante pour lancer le démon ctxusbd\n\
manuellement :\n\
\t\t/etc/init.d/ctxusbd start\n\n\
Toutefois, cette commande est supprimée après chaque arrêt. Pour remédier à ce problème,\n\
installez l'une des commandes ci-dessus et réinstallez la prise en charge USB ou installez le\n\
démon ctxusbd dans les niveaux d'exécution init manuellement.\n"

# Instructions on how to remove the ctxusb daemon from the init run levels
usbinstica5="Impossible de supprimer le démon ctxusbd des niveaux d'exécution init.\n\n\
Ceci est dû au fait qu'aucune des commandes suivantes n'est disponible :\n\
\t\t1: remove_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
Pour remédier à ce problème, installez l'une des commandes ci-dessus et exécutez de nouveau le\n\
script de désinstallation ou supprimez le démon ctxusbd des niveaux d'exécution init manuellement.\n"

usbinstica6="\nVous ne pouvez installer $DISP_PRODUCT_NAME que si $HOST_PRODUCT_NAME\n\
a été installé en tant que racine."


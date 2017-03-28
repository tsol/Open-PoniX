#!/bin/sh
###############################################################################
#
#	Name:		Version.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/es/Version.msg#2 $
#
#	Purpose:	Sets shell variables for the version of Client
#			to be installed.
#
#	Copyright 1996-2013 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

# note: the INSTALLER_... strings should be in lowercase. The installer will then
# accept lowercase or uppercase versions of them as input.
INSTALLER_YES=sí
INSTALLER_NO=no
INSTALLER_QUIT=salir
INSTALLER_Y=s
INSTALLER_N=n
INSTALLER_Q=q
PACKAGE_LANGUAGE=es
DISP_PRODUCT_NAME="Citrix Receiver para Linux"

###############################################################################
#
#	Name:		util_tr.msg
#
#
#	Created:	October 1999
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/es/util_tr.msg#1 $
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
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/es/signon.msg#1 $
#
#	Copyright 1996-2013 Citrix Systems, Inc.  All Rights Reserved.
#
###############################################################################


signon1="Instalación de $DISP_PRODUCT_NAME $DISP_VERSION."

signon2=\
"Copyright 1996-2013 Citrix Systems, Inc. Reservados todos los derechos.\n\
Copyright (c) 1986-1997 RSA Security, Inc. Reservados todos los derechos.\n\
Este software usa bibliotecas del proyecto FFmpeg bajo la licencia LGPLv2.1\n\
\n\
Citrix, Independent Computing Architecture (ICA), Program Neighborhood,\n\
MetaFrame y MetaFrame XP son marcas registradas, y Citrix Receiver,\n\
Citrix XenApp, XenDesktop, Citrix Presentation Server, Citrix Access Suite\n\
y SpeedScreen son marcas comerciales de Citrix Systems, Inc. en los Estados Unidos\n\
de América y en otros países.\n\
\n\
Microsoft, MS, MS-DOS, Outlook, Windows, Windows NT y BackOffice son\n\
marcas registradas o marcas comerciales de Microsoft Corporation en\n\
los Estados Unidos de América y en otros países.\n\
\n\
Todos los demás nombres comerciales a los que se haga referencia son marcas de servicio,\n\
marcas comerciales o marcas registradas de sus respectivos fabricantes."

###############################################################################
#
#	Name:		getyesno.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/es/getyesno.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Reads input to get a simple yes/no response from the
#			user.
#
#	Copyright 1996-1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


getyesno1="Debe responder $INSTALLER_YES o $INSTALLER_NO "

getyesno2="predeterminado"

###############################################################################
#
#	Name:		calcspace.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/es/calcspace.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Functions to calculate the space required for an installation.
#
#	Copyright 1996-1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


calcspace1="\nComprobando el espacio disponible en el disco..."

# both lines followed by size in K.
calcspace2a="\n\tEspacio en disco disponible"
calcspace2b="\n\tEspacio en disco requerido"

calcspace3="\n\
\n\
No hay suficiente espacio disponible en el disco. Libere espacio,\n\
o intente instalar en otro sistema de archivos.\n\
\n\
Presione cualquier tecla para continuar: "

calcspace4="\n\
\n\
Continuando ..."

###############################################################################
#
#	Name:		inst_ica.msg
#
#	$Id: //icaclient/unix13.0/client/unix/installer/scripts/es/inst_ica.msg#1 $
#
#	Purpose:	To install the ICA client.
#
#	Copyright 1996-2004, 2007-2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instica1="Instalación abandonada."

# installer destination directory goes after instica2 
instica2a="\nHa elegido instalar $DISP_PRODUCT_NAME $DISP_VERSION en"
instica2b="."

instica3="\nEsta no es la ubicación predeterminada. Debe configurar la variable de entorno\n\
ICAROOT con esta ubicación, una vez completada la instalación; por ejemplo:\n\n\
export ICAROOT="

instica4="\n¿Quiere continuar con la instalación? [predeterminado $INSTALLER_N]: "

instica5="Instalación de $DISP_PRODUCT_NAME $DISP_VERSION abandonada."

instica6="Eliminación abandonada."

# installer source directory name goes between instica8a and instica8b
instica8a="\nEl directorio"
instica8b="no contiene una instalación válida de\n\
$DISP_PRODUCT_NAME $DISP_VERSION\n\
\n\
Eliminación abandonada."

# existing installation name and version go between instica9a and instica9b
# existing installation directory goes between instica9b and instica9a
instica9a="\nHa elegido eliminar la"
instica9b="instalación en"
instica9c=".\nEsto quitará todo el software y los archivos de configuración de este directorio.\n\
\n\
¿Quiere continuar con la eliminación? [predeterminado $INSTALLER_N]: "

# ica installation directory name goes after instica10
instica10="Quitando"
instica10a='$instica10 $ICAInstDir'

# installer destination directory name goes between instica11a and instica11b
instica11a="El directorio"
instica11b="no pudo eliminarse. Intente quitarlo manualmente."

# installer destination directory name goes between instica12a and instica12b
instica12a="Directorio"
instica12b="eliminado."

instica13="\nLicencia de cliente instalada correctamente."
instica14="\nNo se pudo instalar la licencia de cliente."

instica15="\nEsta instalación solo puede ser eliminada por su propietario."
###############################################################################
#
#	Name:		dir_util.msg
#
#
#	Created:	28 May 1996
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/es/dir_util.msg#1 $
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
Introduzca la ubicación del $DISP_PRODUCT_NAME $DISP_VERSION CDROM\n\
o de una imagen de instalación [predeterminado"
dirutil1b="\no escriba \"$INSTALLER_QUIT\" para cancelar la instalación: "

dirutil2="\nNo se puede acceder a"

# the installer source directory goes between dirutil3a and dirutil3b.
dirutil3a="\nEl directorio" 
dirutil3b="\nno contiene un paquete de $DISP_PRODUCT_NAME $DISP_VERSION."

# the installer destination directory and ']' goes between dirutil4a and dirutil4b.
dirutil4a="\nIntroduzca el directorio donde quiere instalar $DISP_PRODUCT_NAME.\n\
[predeterminado"
dirutil4b="\no escriba \"$INSTALLER_QUIT\" para cancelar la instalación: "

dirutil5="\nEste directorio ya existe.\n\
Debe elegir un nombre de directorio que no exista."

# the installer destination directory goes between dirutil6a and dirutil6b.
dirutil6a="\n¿Hay que usar el nombre"
dirutil6b="en su lugar? [predeterminado $INSTALLER_Y]: "

# the existing installation's name and version go between dirutil7a and dirutil7b.
dirutil7a="\nYa hay una instalación de"
dirutil7b="\n\
en este directorio. La reinstalación\n\
sobrescribirá la instalación existente y toda su configuración \n\
con $DISP_PRODUCT_NAME $DISP_VERSION.\n\
\n\
¿Quiere continuar? [predeterminado $INSTALLER_N]: "

# the installer destination directory goes between dirutil8a and dirutil8b.
dirutil8a="\nNo se puede crear el directorio\n\t"
dirutil8b="Ya existe un archivo con este nombre."

# the installer destination directory goes between dirutil9a and dirutil9b.
dirutil9a="\nEl directorio principal"
dirutil9b="no existe.\n¿Quiere crearlo? [predeterminado $INSTALLER_Y]: "

# the installer destination directory goes between dirutil10a and dirutil10b.
dirutil10a="\nEl directorio"
dirutil10b="\nno contiene una instalación válida\n\
de $DISP_PRODUCT_NAME."

# the (modified or removed) installer destination directory goes between 
# dirutil11a and dirutil11b.
dirutil11a="\nEl directorio"
dirutil11b="\nno existe."

# the (modified or removed) installer destination directory goes between 
# dirutil12a and dirutil12b.
dirutil12a="\nIntroduzca el directorio que contiene la\n\
instalación de $DISP_PRODUCT_NAME\n\
[predeterminado"
dirutil12b="\no escriba \"$INSTALLER_QUIT\" para volver al menú: "

dirutil13="Operación $INSTALLER_QUIT del usuario"

###############################################################################
#
#	Name:		instpkgs.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/es/instpkgs.msg#1 $
#
#	Purpose:	Installs the required parts of the ICA Client.
#
#	Copyright 1996-2011 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instpkgs1="Instalación en curso..."

# instpkgs is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs2="Creando el directorio"
instpkgs2c='$instpkgs2 "$INST_DIR"'

# instpkgs3a is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs3a="\nNo se puede crear el directorio"
instpkgs3b="\nInstalación cancelada."

instpkgs4="Paquete principal..."

instpkgs5="Definiendo permisos de archivos..."

###############################################################################
#
# $Id: $
#
# Copyright 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

# Warn the user that only root gets USB
instusb1="El respaldo de USB solo puede ser instalado por usuarios root. Ejecute el instalador como root para acceder a esta opción de instalación."

# Ask to install USB
instusb2="¿Quiere instalar el respaldo de USB? [predeterminado $INSTALLER_N]: "

# Inform USB was not chosen
instusb3="Respaldo de USB no instalado."
###############################################################################
#
#       Name:           setup-netscape.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/es/setup-netscape.msg#1 $
#
#       Purpose:        Installer text for integrating with Netscape 
#
#       Copyright 1999-2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_netscape1="Integrándose con exploradores..."

integrate_netscape2="\nNo se encontró ninguna instalación de explorador en su sistema.\n\
¿Hay algún explorador instalado? [predeterminado $INSTALLER_N]: "

integrate_netscape3="Exploradores encontrados."

integrate_netscape4="\nSe han encontrado entradas de configuración de explorador pertenecientes a una instalación anterior.\n\
¿Quiere que esas entradas hagan referencia ahora a la nueva instalación? [predeterminado $INSTALLER_Y]: "

integrate_netscape5="\nIntegración completada."

integrate_netscape6="\nIntegración cancelada."

disintegrate_netscape1="\nLas configuraciones de explorador tienen entradas que hacen \
referencia a Citrix Receiver.\n\
¿Quiere quitar las referencias a Citrix Receiver? [predeterminado $INSTALLER_Y]: "

disintegrate_netscape2="Las referencias a Citrix Receiver se han eliminado de los exploradores.\n"
###############################################################################
#
#       Name:           setup-CDE.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/es/setup-CDE.msg#1 $
#
#       Purpose:        Installer text for integrating with CDE 
#
#       Copyright 2002, 2006, 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_CDE1="¿Quiere integrar Citrix Receiver con CDE? [predeterminado $INSTALLER_Y]: "

integrate_CDE2="\nSe han encontrado entradas en CDE pertenecientes a una instalación anterior.\n\
¿Quiere que esas entradas hagan referencia ahora a la nueva instalación? [predeterminado $INSTALLER_Y]: "

integrate_CDE3="\nIntegración completa. Cierre la sesión y vuelva a iniciarla para completar la integración."

integrate_CDE4="\nIntegración cancelada."

disintegrate_CDE1="\nCDE tiene entradas que hacen referencia a Citrix Receiver.\n\
¿Quiere quitar las referencias a Citrix Receiver? [predeterminado $INSTALLER_Y]: "

disintegrate_CDE2="Las referencias a Citrix Receiver se han eliminado de CDE. Cierre la sesión y vuelva a iniciarla \
para completar la eliminación.\n"

disintegrate_CDE3="\nEliminación cancelada."
###############################################################################
#
#       Name:           setup-dt.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/es/setup-dt.msg#1 $
#
#       Purpose:        Installer text for integrating with GNOME and KDE 
#
#       Copyright 2003, 2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_DT1="No se encontraron directorios de GNOME o KDE. Se omitirá la integración."
integrate_DT2="¿Quiere integrar Citrix Receiver con KDE y GNOME? [predeterminado $INSTALLER_Y]: "
integrate_DT3="\nSe han encontrado entradas de escritorio de KDE o GNOME pertenecientes a una instalación anterior.\n\
¿Quiere que esas entradas hagan referencia ahora a la nueva instalación? [predeterminado $INSTALLER_Y]: "
###############################################################################
#
#       Name:           setup-gst.msg 
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/es/setup-gst.msg#1 $
#
#       Purpose:        Installer text for integrating with GStreamer
#
#       Copyright 2003, 2006, 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_GST1="No se encontraron directorios de GStreamer. Se omitirá la integración con GStreamer."
integrate_GST2="¿Quiere que GStreamer use el plugin de este cliente? [predeterminado $INSTALLER_Y]: "
integrate_GST3="\nSe ha encontrado una entrada de GStreamer perteneciente a una instalación anterior.\n\
¿Quiere que esa entrada haga referencia ahora a la nueva instalación? [predeterminado $INSTALLER_Y]: "
integrate_GST4a="No se pudo crear el directorio: "
integrate_GST4b=""
integrate_GST5a="No se pudo crear vínculo en: "
integrate_GST5b=""
###############################################################################
#
#	Name:		menus.msg
#
#
#	Created:	May 1996 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/es/menus.msg#1 $
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
Seleccione una opción:\n\
\n\
  1.   Instalar $DISP_PRODUCT_NAME $DISP_VERSION\n\
  2.   Quitar $DISP_PRODUCT_NAME $DISP_VERSION\n\
  3.   Salir de la instalación de $DISP_PRODUCT_NAME $DISP_VERSION\n"


menus2="Introduzca el número de opción"

menus3="Entrada no válida"

menus4=\
"  4.   Instalar licencia de $DISP_PRODUCT_NAME $DISP_VERSION\n"

menus5=\
"\nNecesita configurar la licencia de cliente antes de instalar este cliente.\n\
Inicie una sesión como superusuario (root) y elija la opción 4.\n"


###############################################################################
#
#	Name:		install.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/es/install.msg#1 $
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


install1="\nPara instalar $DISP_PRODUCT_NAME ejecute el programa setupwfc\n\
en el directorio raíz del CD-ROM.\n"

install2="\nPara obtener más información, consulte el manual de $DISP_PRODUCT_NAME $DISP_VERSION.\n"

install3="Saliendo de la instalación de $DISP_PRODUCT_NAME $DISP_VERSION."

install4="Modo de instalación de usuario."
###############################################################################
#
#	Name:		usb_inst_ica.msg
#
#	$Id:$
#
#	Copyright 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

usbinstica1="Instalación completada."
usbinstica2="Eliminación completada."
usbinstica3="Debe iniciar una sesión como root para llevar a cabo esta instalación."

# Instructions on how to install the ctxusb daemon into the init run levels
usbinstica4="No ha sido posible integrar el demonio ctxusbd en los niveles de\n\
ejecución de init.\n\n\
Esto se debe a que ninguno de los comandos siguientes estaba disponible:\n\
\t\t1: install_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
Se puede usar el comando siguiente para iniciar el demonio ctxusbd\n\
manualmente:\n\
\t\t/etc/init.d/ctxusbd start\n\n\
Sin embargo, esto no es permanente y no se conservará después de apagar el equipo. Para rectificarlo:\n\
instale alguno de los comandos anteriores y vuelva a instalar el respaldo para USB, o instale el\n\
demonio ctxusbd en los niveles de ejecución de init manualmente.\n"

# Instructions on how to remove the ctxusb daemon from the init run levels
usbinstica5="No ha sido posible quitar el demonio ctxusbd de los niveles de ejecución de init.\n\n\
Esto se debe a que ninguno de los comandos siguientes estaba disponible:\n\
\t\t1: remove_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
Para rectificarlo, instale alguno de los comandos anteriores y vuelva a ejecutar el script\n\
de eliminación, o quite el demonio ctxusbd de los niveles de ejecución de init manualmente.\n"

usbinstica6="\nSolo puede instalar $DISP_PRODUCT_NAME si $HOST_PRODUCT_NAME\n\
se instaló originalmente como root."


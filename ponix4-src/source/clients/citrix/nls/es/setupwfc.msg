#!/bin/sh
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

#
# =============================================================================
#
#       Name:           setup.msg
#
#       Purpose:        To invoke a host-specific hinst script for installing
#                       or removing Citrix Receiver for Unix
#
#                       Note: This part of the file must be preceded by the
#                       functions tr_file and get_tr_key.
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/es/setup.msg#1 $
#
#       Copyright 1996-2002, 2006, 2009 Citrix Systems, Inc.
#       All rights reserved.
#
# =============================================================================


setup1="Este paquete no contiene una versión de Citrix Receiver para esta estación de trabajo."

# The name of the installer script goes between setup2a and setup2b.
setup2a="No se encuentran archivos fundamentales. Ejecute "
setup2b="usando una ruta completa."

# The platform type (HP, Dec, etc.) goes between setup3a and setup3b.
setup3a="Esta instalación no es del tipo correcto para una"
setup3b="estación de trabajo."

# The OS name (Free/Net/Open BSD) goes between setup4a and setup4b.
setup4a="Citrix no ha sometido a pruebas ni respalda este cliente en el"
setup4b="sistema operativo."


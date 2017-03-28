#!/bin/sh
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
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/fr/setup.msg#1 $
#
#       Copyright 1996-2002, 2006, 2009 Citrix Systems, Inc.
#       All rights reserved.
#
# =============================================================================


setup1="Ce pack ne contient aucune version de Citrix Receiver adaptée à cette station de travail."

# The name of the installer script goes between setup2a and setup2b.
setup2a="Impossible de trouver les fichiers essentiels. Procédez à l'exécution "
setup2b="à l'aide d'un chemin d'accès complet."

# The platform type (HP, Dec, etc.) goes between setup3a and setup3b.
setup3a="Cette installation n'est pas du type correct pour une"
setup3b="station de travail."

# The OS name (Free/Net/Open BSD) goes between setup4a and setup4b.
setup4a="Citrix ne teste pas et ne prend pas en charge ce client sur le"
setup4b="système d'exploitation."


#!/bin/sh
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
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/de/setup.msg#1 $
#
#       Copyright 1996-2002, 2006, 2009 Citrix Systems, Inc.
#       All rights reserved.
#
# =============================================================================


setup1="Dieses Paket enthält keine Citrix Receiver-Version für diese Workstation."

# the name of the installer script goes between setup2a and setup2b.
setup2a="Wichtige Dateien können nicht gefunden werden. Bitte rufen Sie"
setup2b="mit vollständiger Pfadangabe auf."

# the platform type (HP, Dec, etc.) goes between setup3a and setup3b.
setup3a="Diese Installation ist nicht vom richtigen Typ für eine"
setup3b="-Workstation."

# The OS name (Free/Net/Open BSD) goes between setup4a and setup4b.
setup4a="Dieser Client wird von Citrix nicht auf dem Betriebssystem"
setup4b="getestet und unterstützt."


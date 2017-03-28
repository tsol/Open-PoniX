#!/bin/sh
###############################################################################
#
#	Name:		util_tr.msg
#
#
#	Created:	October 1999
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/util_tr.msg#1 $
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
#	Name:		setup.msg
#
#	Purpose:	To invoke a host-specific hinst script for installing
#			or removing Citrix Receiver for Unix 
#		
#			Note: This part of the file must be preceded by the
#			functions tr_file and get_tr_key.
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/setup.msg#1 $
#
#	Copyright 1996-2002, 2006, 2009 Citrix Systems, Inc. All rights reserved.
#
# =============================================================================


setup1="このパッケージには、このワークステーション向けの Citrix 
Receiver のバージョンは含まれていません。"

# the name of the installer script goes between setup2a and setup2b.
setup2a="基本ファイルが見つかりません。フル パスを使って "
setup2b="実行してください。"

# the platform type (HP, Dec, etc.) goes between setup3a and setup3b.
setup3a="この種類のインストールは、ワークステーションに"
setup3b="適していません。"

# The OS name (Free/Net/Open BSD) goes between setup4a and setup4b.
setup4a="Citrix 社では、"
setup4b=" オペレーティング システム上でのこの Citrix Receiver
のテストまたはサポートを行っていません。"



#!/bin/sh
###############################################################################
#
#	Name:		util_tr.msg
#
#
#	Created:	October 1999
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/util_tr.msg#1 $
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
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/setup.msg#1 $
#
#       Copyright 1996-2002, 2006, 2009 Citrix Systems, Inc.
#       All rights reserved.
#
# =============================================================================


setup1="此软件包中未包含适用于此工作站的 Citrix Receiver 版本。"

# The name of the installer script goes between setup2a and setup2b.
setup2a="找不到必需的文件。请使用完整路径运行 "
setup2b="。"

# The platform type (HP, Dec, etc.) goes between setup3a and setup3b.
setup3a="此安装不是适用于"
setup3b="工作站的正确类型。"

# The OS name (Free/Net/Open BSD) goes between setup4a and setup4b.
setup4a="Citrix 未在以下操作系统中测试此客户端，或不支持在其中安装此客户端:"
setup4b="。"


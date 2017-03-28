#!/bin/sh
###############################################################################
#
#	Name:		Version.msg
#
#	$Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/Version.msg#1 $
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
PACKAGE_LANGUAGE=zh_HANS
DISP_PRODUCT_NAME="Citrix Receiver for Linux"

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
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/signon.msg#1 $
#
#	Copyright 1996-2013 Citrix Systems, Inc.  All Rights Reserved.
#
###############################################################################


signon1="$DISP_PRODUCT_NAME $DISP_VERSION 安装程序。"

signon2=\
"版权所有 1996-2013 Citrix Systems, Inc. 保留所有权利。\n\
版权所有 (c) 1986-1997 RSA Security, Inc. 保留所有权利。\n\
此软件使用 LGPLv2.1 下 FFmpeg 项目中的库\n\
\n\
Citrix、Independent Computing Architecture (ICA)、Program Neighborhood、\n\
MetaFrame 和 MetaFrame XP 是 Citrix Systems, Inc. 在美国和其他国家/地区\n\
的注册商标；Citrix Receiver、Citrix XenApp、XenDesktop、Citrix Presentation Server、\n\
Citrix Access Suite 和 SpeedScreen 是 Citrix Systems, Inc. 在美国和\n\
其他国家/地区的商标。\n\
\n\
Microsoft、MS、MS-DOS、Outlook、Windows、Windows NT \n\
和 BackOffice 是 Microsoft Corporation 在美国和\n\
其他国家/地区的注册商标或商标。\n\
\n\
涉及的所有其他商标名称是其各自制造商的服务标记、\n\
商标或注册商标。"

###############################################################################
#
#	Name:		getyesno.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/getyesno.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Reads input to get a simple yes/no response from the
#			user.
#
#	Copyright 1996-1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


getyesno1="必须响应 $INSTALLER_YES 或 $INSTALLER_NO "

getyesno2="默认值"

###############################################################################
#
#	Name:		calcspace.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/calcspace.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Functions to calculate the space required for an installation.
#
#	Copyright 1996-1998 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


calcspace1="\n正在检查可用磁盘空间..."

# both lines followed by size in K.
calcspace2a="\n\t可用磁盘空间:"
calcspace2b="\n\t所需磁盘空间:"

calcspace3="\n\
\n\
可用磁盘空间不足。请释放空间，或者尝试\n\
安装到其他文件系统中。\n\
\n\
按任意键继续: "

calcspace4="\n\
\n\
正在继续..."

###############################################################################
#
#	Name:		inst_ica.msg
#
#	$Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/inst_ica.msg#1 $
#
#	Purpose:	To install the ICA client.
#
#	Copyright 1996-2004, 2007-2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instica1="已放弃安装。"

# installer destination directory goes after instica2 
instica2a="\n您已选择将 $DISP_PRODUCT_NAME $DISP_VERSION 安装到"
instica2b="。"

instica3="\n此位置不是默认位置。必须在安装完成后\n\
将环境变量 ICAROOT 设置为此位置，例如:\n\n\
export ICAROOT="

instica4="\n是否继续安装? [默认值为 $INSTALLER_N]: "

instica5="已放弃安装 $DISP_PRODUCT_NAME $DISP_VERSION。"

instica6="已放弃删除。"

# installer source directory name goes between instica8a and instica8b
instica8a="\n目录"
instica8b="中未包含有效的 $DISP_PRODUCT_NAME $DISP_VERSION\n\
安装\n\
\n\
已放弃删除。"

# existing installation name and version go between instica9a and instica9b
# existing installation directory goes between instica9b and instica9a
instica9a="\n您已选择删除以下位置处的"
instica9b="安装:"
instica9c="。\n此操作将删除此目录中的所有软件和配置文件。\n\
\n\
是否继续删除? [默认值为 $INSTALLER_N]: "

# ica installation directory name goes after instica10
instica10="正在删除"
instica10a='$instica10 $ICAInstDir'

# installer destination directory name goes between instica11a and instica11b
instica11a="无法删除目录"
instica11b="。必须尝试手动删除此目录。"

# installer destination directory name goes between instica12a and instica12b
instica12a="已删除目录"
instica12b="。"

instica13="\n已成功安装客户端许可证。"
instica14="\n无法安装客户端许可证。"

instica15="\n此安装只能由其所有者删除。"
###############################################################################
#
#	Name:		dir_util.msg
#
#
#	Created:	28 May 1996
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/dir_util.msg#1 $
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
请输入 $DISP_PRODUCT_NAME $DISP_VERSION CDROM\n\
或可安装映像的位置 [默认值为"
dirutil1b="\n或键入 $INSTALLER_QUIT 放弃安装: "

dirutil2="\n无法访问"

# the installer source directory goes between dirutil3a and dirutil3b.
dirutil3a="\n目录"
dirutil3b="\n中未包含 $DISP_PRODUCT_NAME $DISP_VERSION 软件包。"

# the installer destination directory and ']' goes between dirutil4a and dirutil4b.
dirutil4a="\n请输入要在其中安装 $DISP_PRODUCT_NAME 的目录。\n\
[默认值为"
dirutil4b="\n或键入 $INSTALLER_QUIT 放弃安装: "

dirutil5="\n此目录已存在。\n\
必须选择一个不存在的目录名称。"

# the installer destination directory goes between dirutil6a and dirutil6b.
dirutil6a="\n我是否应改为使用名称"
dirutil6b="? [默认值为 $INSTALLER_Y]: "

# the existing installation's name and version go between dirutil7a and dirutil7b.
dirutil7a="\n此目录中已存在"
dirutil7b="\n\
的安装。重新安装将覆盖该安装\n\
以及对 $DISP_PRODUCT_NAME $DISP_VERSION \n\
所做的所有配置。\n\
\n\
是否要继续? [默认值为 $INSTALLER_N]: "

# the installer destination directory goes between dirutil8a and dirutil8b.
dirutil8a="\n无法创建目录\n\t"
dirutil8b="具有此名称的文件已存在。"

# the installer destination directory goes between dirutil9a and dirutil9b.
dirutil9a="\n父目录"
dirutil9b="不存在。\n是否要创建该目录? [默认值为 $INSTALLER_Y]: "

# the installer destination directory goes between dirutil10a and dirutil10b.
dirutil10a="\n目录"
dirutil10b="\n中未包含有效的 $DISP_PRODUCT_NAME\n\
安装。"

# the (modified or removed) installer destination directory goes between 
# dirutil11a and dirutil11b.
dirutil11a="\n目录"
dirutil11b="\n不存在。"

# the (modified or removed) installer destination directory goes between 
# dirutil12a and dirutil12b.
dirutil12a="\n请输入包含\n\
$DISP_PRODUCT_NAME 安装的目录。\n\
[默认值为"
dirutil12b="\n或键入 $INSTALLER_QUIT 返回到菜单: "

dirutil13="用户 $INSTALLER_QUIT 操作"

###############################################################################
#
#	Name:		instpkgs.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/instpkgs.msg#1 $
#
#	Purpose:	Installs the required parts of the ICA Client.
#
#	Copyright 1996-2011 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instpkgs1="正在安装..."

# instpkgs is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs2="正在创建目录"
instpkgs2c='$instpkgs2 "$INST_DIR"'

# instpkgs3a is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs3a="\n无法创建目录"
instpkgs3b="\n已终止安装。"

instpkgs4="核心包..."

instpkgs5="正在设置文件权限..."

###############################################################################
#
# $Id: $
#
# Copyright 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

# Warn the user that only root gets USB
instusb1="USB 支持无法由非 root 用户安装。请以 root 用户身份运行安装程序以访问此安装选项。"

# Ask to install USB
instusb2="是否要安装 USB 支持? [默认值为 $INSTALLER_N]: "

# Inform USB was not chosen
instusb3="未安装 USB 支持。"
###############################################################################
#
#       Name:           setup-netscape.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/setup-netscape.msg#1 $
#
#       Purpose:        Installer text for integrating with Netscape 
#
#       Copyright 1999-2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_netscape1="正在与浏览器集成..."

integrate_netscape2="\n在您的系统中找不到浏览器安装。\n\
是否已安装浏览器? [默认值为 $INSTALLER_N]: "

integrate_netscape3="已找到浏览器。"

integrate_netscape4="\n在早期安装的浏览器配置中找到条目。\n\
是否希望这些条目指向新安装? [默认值为 $INSTALLER_Y]: "

integrate_netscape5="\n集成完成。"

integrate_netscape6="\n已取消集成。"

disintegrate_netscape1="\n浏览器配置具有引用 \
Citrix Receiver 的条目。\n\
是否要删除对 Citrix Receiver 的引用? [默认值为 $INSTALLER_Y]: "

disintegrate_netscape2="已删除浏览器对 Citrix Receiver 的引用。\n"
###############################################################################
#
#       Name:           setup-CDE.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/setup-CDE.msg#1 $
#
#       Purpose:        Installer text for integrating with CDE 
#
#       Copyright 2002, 2006, 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_CDE1="是否要将 Citrix Receiver 与 CDE 集成? [默认值为 $INSTALLER_Y]: "

integrate_CDE2="\n在早期安装的 CDE 中找到条目。\n\
是否希望这些条目指向新安装? [默认值为 $INSTALLER_Y]: "

integrate_CDE3="\n集成完成。请注销并重新登录以完成集成。"

integrate_CDE4="\n已取消集成。"

disintegrate_CDE1="\nCDE 具有引用 Citrix Receiver 的条目。\n\
是否要删除对 Citrix Receiver 的引用? [默认值为 $INSTALLER_Y]: "

disintegrate_CDE2="已删除 CDE 对 Citrix Receiver 的引用。请注销并重新登录以\
完成删除。\n"

disintegrate_CDE3="\n已取消删除。"
###############################################################################
#
#       Name:           setup-dt.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/setup-dt.msg#1 $
#
#       Purpose:        Installer text for integrating with GNOME and KDE 
#
#       Copyright 2003, 2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_DT1="未找到 GNOME 或 KDE 目录，即将跳过集成。"
integrate_DT2="是否要将 Citrix Receiver 与 KDE 和 GNOME 集成? [默认值为 $INSTALLER_Y]: "
integrate_DT3="\n在早期安装中找到 KDE 或 GNOME 桌面条目。\n\
是否希望这些条目指向新安装? [默认值为 $INSTALLER_Y]: "
###############################################################################
#
#       Name:           setup-gst.msg 
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/setup-gst.msg#1 $
#
#       Purpose:        Installer text for integrating with GStreamer
#
#       Copyright 2003, 2006, 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_GST1="未找到 GStreamer 目录，即将跳过 GStreamer 集成。"
integrate_GST2="是否希望 GStreamer 使用此客户端中的插件? [默认值为 $INSTALLER_Y]: "
integrate_GST3="\n在早期安装中找到 GStreamer 条目。\n\
是否希望此条目指向新安装? [默认值为 $INSTALLER_Y]: "
integrate_GST4a="无法创建目录: "
integrate_GST4b=""
integrate_GST5a="无法在以下位置创建链接: "
integrate_GST5b=""
###############################################################################
#
#	Name:		menus.msg
#
#
#	Created:	May 1996 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/menus.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Displays menus for ICA Client installer.
#
#	Copyright 1996-2013 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

menus1="\
选择一个安装选项:\n\
\n\
  1.   安装 $DISP_PRODUCT_NAME $DISP_VERSION\n\
  2.   删除 $DISP_PRODUCT_NAME $DISP_VERSION\n\
  3.   退出 $DISP_PRODUCT_NAME $DISP_VERSION 安装程序\n"


menus2="请输入选项编号"

menus3="无效输入"

menus4=\
"  4.   安装 $DISP_PRODUCT_NAME $DISP_VERSION 许可证\n"

menus5=\
"\n需要先配置客户端许可证，才能安装此客户端。\n\
请以超级用户(root)身份登录并选择选项 4。\n"


###############################################################################
#
#	Name:		install.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/zh_HANS/install.msg#1 $
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


install1="\n要安装 $DISP_PRODUCT_NAME，请运行 CD-ROM 的\n\
根目录中的程序 setupwfc。\n"

install2="\n有关详细信息，请参阅 $DISP_PRODUCT_NAME $DISP_VERSION 手册。\n"

install3="正在退出 $DISP_PRODUCT_NAME $DISP_VERSION 安装程序。"

install4="用户安装模式。"
###############################################################################
#
#	Name:		usb_inst_ica.msg
#
#	$Id:$
#
#	Copyright 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

usbinstica1="安装完成。"
usbinstica2="删除完成。"
usbinstica3="必须以 root 用户身份登录才能执行此安装。"

# Instructions on how to install the ctxusb daemon into the init run levels
usbinstica4="无法将 ctxusbd 守护程序集成到初始运行\n\
级别中。\n\n\
发生此问题的原因是以下命令均不可用:\n\
\t\t1: install_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
无法使用以下命令手动启动 ctxusbd \n\
守护程序:\n\
\t\t/etc/init.d/ctxusbd start\n\n\
However, this will not be persistent after shutdown. To rectify this, either\n\
install one of the above commands and reinstall USB Support, or install the\n\
ctxusbd daemon into the init run levels manually.\n"

# Instructions on how to remove the ctxusb daemon from the init run levels
usbinstica5="无法从初始运行级中删除 ctxusbd 守护程序。\n\n\
出现此问题的原因是以下命令均不可用:\n\
\t\t1: remove_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
要修复此问题，请安装上述命令之一并重新运行删除脚本，\n\
或者手动从初始运行级中删除 ctxusbd 守护程序。\n"

usbinstica6="\n仅当 $HOST_PRODUCT_NAME 最初是以 root 用户身份安装的，\n\
才能安装 $DISP_PRODUCT_NAME。"


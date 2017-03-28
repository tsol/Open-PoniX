#!/bin/sh
###############################################################################
#
#	Name:		Version.msg
#
#	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/Version.msg#1 $
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
PACKAGE_LANGUAGE=ja
DISP_PRODUCT_NAME="Citrix Receiver for Linux"
PAGER_CMD=cat
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
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/signon.msg#1 $
#
#	Copyright 1996-2013 Citrix Systems, Inc.  All Rights Reserved.
#
###############################################################################


signon1="$DISP_PRODUCT_NAME $DISP_VERSION セットアップを行います。"

signon2=\
"Copyright 1996-2013 Citrix Systems, Inc. All rights reserved.\n\
Copyright (c) 1986-1997 RSA Security, Inc. All rights reserved.\n\
このソフトウェアは、LGPLv2.1 の FFmpeg プロジェクトからのライブラリを使用します。\n\
\n\
Citrix、Independent Computing Architecture (ICA)、Program \n\
Neighborhood、MetaFrame、および MetaFrame XP は米国およびその他の国\n\
における Citrix Systems, Inc. の登録商標です。Citrix Receiver、\n\
Citrix XenApp、XenDesktop、Presentation Server、Citrix Access Suite、\n\
および SpeedScreen は、米国およびその他の国における Citrix Systems, \n\
Inc. の商標です。\n\
Microsoft、MS、MS-DOS、Outlook、Windows、Windows NT および BackOffice\n\
は、米国およびその他の国における Microsoft Corporation の登録商標または\n\
商標です。\n\
\n\
その他のすべての商標および登録商標は、該当する各社の財産です。"

###############################################################################
#
#	Name:		getyesno.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/getyesno.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Reads input to get a simple yes/no response from the
#			user.
#
#	Copyright 1996-2006 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


getyesno1="$INSTALLER_YES または $INSTALLER_NO で答えてください。"

getyesno2="デフォルト"

###############################################################################
#
#	Name:		calcspace.msg
#
#
#	Created:	May 1995
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/calcspace.msg#1 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Functions to calculate the space required for an installation.
#
#	Copyright 1996-2006 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


calcspace1="\n使用可能なディスク容量をチェックしています ..."

# both lines followed by size in K.
calcspace2a="\n\t使用可能なディスク容量"
calcspace2b="\n\t必要なディスク容量"

calcspace3="\n\
\n\
ディスク容量が足りません。使用可能なディスク容量を確保するか、\n\
別のファイル システムへのインストールを試みてください。\n\
\n\
続行する場合は、どれかキーを押してください。: "

calcspace4="\n\
\n\
続行中 ..."

###############################################################################
#
#	Name:		inst_ica.msg
#
#	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/inst_ica.msg#1 $
#
#	Purpose:	To install the ICA client.
#
#	Copyright 1996-2004, 2007-2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instica1="インストールは中断されました。"

# installer destination directory goes after instica2 
instica2a="\n$DISP_PRODUCT_NAME $DISP_VERSION のインストールを選択しました。"
instica2b="."

instica3="\nこれはデフォルトの場所ではありません。インストール完了後に環境変\n\
数 ICAROOT をこの場所に設定する必要があります。例: export ICAROOT="

instica4="\nインストールを続行しますか？ [デフォルト $INSTALLER_N]: "

instica5="$DISP_PRODUCT_NAME $DISP_VERSION のインストールが中断されました。"

instica6="削除は中断されました。"

# installer source directory name goes between instica8a and instica8b
instica8a="\nこのディレクトリに "
instica8b=" は\n有効な $DISP_PRODUCT_NAME $DISP_VERSION が含まれていません。\n\
\n\
\n\
削除は中断されました。"

# existing installation name and version go between instica9a and instica9b
# existing installation directory goes between instica9b and instica9a
instica9a="\n以下のディレクトリにインストールされている"
instica9b="の削除を選択しました。"
instica9c="\nこれにより、すべてのソフトウェアとこのディレクトリ内の設定ファイルが削除されます。\n\
\n\
削除を続行しますか? [デフォルト $INSTALLER_N]: "

# ica installation directory name goes after instica10
instica10="削除"
instica10a='$instica10 $ICAInstDir'

# installer destination directory name goes between instica11a and instica11b
instica11a="ディレクトリを"
instica11b="削除できませんでした。このディレクトリの手動による削除を試みてください。"

# installer destination directory name goes between instica12a and instica12b
instica12a="ディレクトリ "
instica12b=" は削除されました。"

instica13="\nクライアント ライセンスがインストールされました。"
instica14="\nクライアント ライセンスのインストールに失敗しました。"

instica15="\nこのインストールは、インストールの所有者のみが削除できます。"

###############################################################################
#
#	Name:		dir_util.msg
#
#
#	Created:	28 May 1996
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/dir_util.msg#2 $
#
#	Coding Stds:	2.4
#
#	Purpose:	Functions for obtaining, checking and creating
#			the directories required for installation.
#
#	Copyright 1996-2010 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


# the installer source directory and ']' goes between dirutil1a and dirutil1b.
dirutil1a="\n\
$DISP_PRODUCT_NAME $DISP_VERSION の CD または\n\
インストールイメージ [デフォルト"
dirutil1b="\nのパスを入力してください。インストールを中止する場合は \"$INSTALLER_QUIT\" と入力してください。: "

dirutil2="\nアクセスできません"

# the installer source directory goes between dirutil3a and dirutil3b.
dirutil3a="\nディレクトリに "
dirutil3b=" は\n $DISP_PRODUCT_NAME $DISP_VERSION パッケージは含まれていません。"

# the installer destination directory and ']' goes between dirutil4a and dirutil4b.
dirutil4a="\n$DISP_PRODUCT_NAME をインストールするディレクトリを入力してください。\n\
[デフォルト"
dirutil4b="\nインストールを中止する場合は \"$INSTALLER_QUIT\" を入力してください: "

dirutil5="\nこのディレクトリはすでに存在しています。\n\
まだ存在しないディレクトリ名を選択してください。"

# the installer destination directory goes between dirutil6a and dirutil6b.
dirutil6a="\n代わりに"
dirutil6b="を使用しますか? [デフォルト $INSTALLER_Y]:"

# the existing installation's name and version go between dirutil7a and dirutil7b.
dirutil7a="\nインストールされている"
dirutil7b="\n\
が既にこのディレクトリにあります。再インストールすると\n\
既存のインストールおよび$DISP_PRODUCT_NAME $DISP_VERSION\n\
のすべての設定が上書きされます。\n\
\n\
処理を続行しますか? [デフォルト $INSTALLER_N]: "

# the installer destination directory goes between dirutil8a and dirutil8b.
dirutil8a="\nディレクトリを作成できません\n\t"
dirutil8b="この名前のファイルがすでに存在します。"

# the installer destination directory goes between dirutil9a and dirutil9b.
dirutil9a="\n親ディレクトリ"
dirutil9b="が存在しません。\n親ディレクトリを作成しますか？ [デフォルト $INSTALLER_Y]: "

# the installer destination directory goes between dirutil10a and dirutil10b.
dirutil10a="\nディレクトリ "
dirutil10b="\n有効な $DISP_PRODUCT_NAME\n\
がありません。"

# the (modified or removed) installer destination directory goes between 
# dirutil11a and dirutil11b.
dirutil11a="\nディレクトリ "
dirutil11b=" は\n存在しません。"


# the (modified or removed) installer destination directory goes between 
# dirutil12a and dirutil12b.
dirutil12a="\n$DISP_PRODUCT_NAME を含むディレクトリを入力\n\
入力してください\n\
[default"
dirutil12b="\nメニューに戻る場合は、\"$INSTALLER_QUIT\" を入力してください。: "

dirutil13="ユーザーの $INSTALLER_QUIT 操作"

###############################################################################
#
#	Name:		instpkgs.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/instpkgs.msg#1 $
#
#	Purpose:	Installs the required parts of the ICA Client.
#
#	Copyright 1996-2011 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


instpkgs1="インストール中 ..."

# instpkgs is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs2="ディレクトリの作成"
instpkgs2c='$instpkgs2 "$INST_DIR"'

# instpkgs3a is followed by the name of a directory, e.g. /opt/Citrix/ICAClient
instpkgs3a="\nディレクトリを作成できません。"
instpkgs3b="\nインストールは中断されました。"

instpkgs4="コア パッケージ ..."

instpkgs5="ファイル許可を設定中 ..."

###############################################################################
#
# $Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/inst_usb.msg#1 $
#
# Copyright 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

# Warn the user that only root gets USB
instusb1="スーパーユーザー（root）以外のユーザーは USB サポートをインストールできません。スーパーユーザーとしてインストーラーを実行し、このインストール オプションにアクセスしてください。"

# Ask to install USB
instusb2="USB サポートをインストールしますか? [デフォルト $INSTALLER_N]: "

# Inform USB was not chosen
instusb3="USB サポートはインストールされていません。"
###############################################################################
#
#       Name:           setup-netscape.msg 
#
#       $Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/setup-netscape.msg#1 $
#
#       Purpose:        Installer text for integrating with Netscape 
#
#       Copyright 1999-2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_netscape1="Web ブラウザーと統合中..."

integrate_netscape2="\nシステム上に Web ブラウザーが見つかりません。\n\
Web ブラウザーがインストールされていますか? [デフォルト $INSTALLER_N]: "

integrate_netscape3="Web ブラウザーが見つかりました。"

integrate_netscape4="\n前回インストールした Web ブラウザーを使うように設定されている項目があります。\n\
新しい Web ブラウザーを使うように変更しますか? [デフォルト $INSTALLER_Y]: "

integrate_netscape5="\n統合が完了しました。"

integrate_netscape6="\n統合がキャンセルされました。"

disintegrate_netscape1="\nWeb ブラウザーが Citrix Receiver \を参照するように設定されています。\n\
Citrix Receiver への参照を削除しますか? [デフォルト $INSTALLER_Y]: "

disintegrate_netscape2="Web ブラウザーの Citrix Receiver への参照が削除されました。\n"
###############################################################################
#
#       Name:           setup-CDE.msg 
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/setup-CDE.msg#1 $
#
#       Purpose:        Installer text for integrating with CDE 
#
#       Copyright 2002, 2006, 2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_CDE1="Citrix Receiver を CDE と統合しますか? [デフォルト $INSTALLER_Y]: "

integrate_CDE2="\n以前のインストールから CDE のエントリが見つかりました。\n\
これらのエントリを新しいインストールと置き換えますか? [デフォルト $INSTALLER_Y]: "

integrate_CDE3="\n統合が行われました。ログアウト後にもう一度ログオンして、統合を完了してください。"

integrate_CDE4="\n統合がキャンセルされました。"

disintegrate_CDE1="\nCitrix Receiver を参照する\CDE エントリがあります。\n\
Citrix Receiver への参照を削除しますか? [デフォルト $INSTALLER_Y]: "

disintegrate_CDE2="Citrix Receiver への CDE の参照を削除しました。ログアウト\
後にもう一度ログオンして、削除を完了してください。\n"

disintegrate_CDE3="\n削除がキャンセルされました。"
###############################################################################
#
#       Name:           setup-dt.msg 
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/setup-dt.msg#1 $
#
#       Purpose:        Installer text for integrating with GNOME and KDE 
#
#       Copyright 2003, 2006, 2008-2009 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_DT1="GNOME または KDE ディレクトリが見つかりません。統合がスキップされます。"
integrate_DT2="Citrix Receiver を KDE および GNOME に統合しますか? [デフォルト $INSTALLER_Y]: "
integrate_DT3="\n以前のインストールから KDE または GNOME デスクトップ エントリが見つかりました。\n\
これらのエントリを新しいインストールと置き換えますか? [デフォルト $INSTALLER_Y]: "
###############################################################################
#
#       Name:           setup-gst.msg 
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/setup-gst.msg#1 $
#
#       Purpose:        Installer text for integrating with GStreamer
#
#       Copyright 2003, 2006, 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

integrate_GST1="GStreamer ディレクトリが見つかりませんでした。GStreamer 統合はスキップされます。"
integrate_GST2="GStreamer でこのクライアントからのプラグインを使用しますか? [デフォルト $INSTALLER_Y]: "
integrate_GST3="\n以前のインストールから GStreamer エントリが見つかりました。\n\
このエントリが新しいインストールを指定するようにしますか? [デフォルト $INSTALLER_Y]: "
integrate_GST4a="次のディレクトリを作成できませんでした : "
integrate_GST4b=" "
integrate_GST5a="次の中にリンクを作成できませんでした : "
integrate_GST5b=" "
###############################################################################
#
#	Name:		menus.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/menus.msg#1 $
#
#	Purpose:	Displays menus for ICA Client installer.
#
#	Copyright 1996-2013 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

menus1=\
"\n\
セットアップ オプションを選択してください:\n\
\n\
  1.   $DISP_PRODUCT_NAME $DISP_VERSION のインストール\n\
  2.   $DISP_PRODUCT_NAME $DISP_VERSION の削除\n\
  3.   $DISP_PRODUCT_NAME $DISP_VERSION セットアップの終了\n"

menus2="オプション番号を入力してください"

menus3="無効なエントリです。"

menus4=\
"  4.   $DISP_PRODUCT_NAME $DISP_VERSION ライセンスのインストール\n"

menus5=\
"\nこのクライアントをインストールする前に、クライアント ライセンスを設定する必要があります。\n\
スーパーユーザー (root) としてログオンし、オプション 4 を選択してください。\n"

###############################################################################
#
#	Name:		install.msg
#
#	Version:	$Id: //icaclient/unix13.0/client/unix/installer/scripts/ja/install.msg#1 $
#
#	Purpose:	The main (top-level) part of the ICA Client installer.
#			For correct operation this should be at the very
#			bottom of the script.
#
#	Copyright 1996-2006 Citrix Systems, Inc. All rights reserved.
#
###############################################################################


install1="\n$DISP_PRODUCT_NAME をインストールするには、CD-ROM のルート ディレクトリにある\n\
setupwfc プログラムを実行します。\n"

install2="\n詳細は、$DISP_PRODUCT_NAME $DISP_VERSION マニュアルを参照してください。\n"

install3="$DISP_PRODUCT_NAME $DISP_VERSION のセットアップを終了します。"

install4="ユーザー インストール モード"
###############################################################################
#
#	Name:		usb_inst_ica.msg
#
#	$Id:$
#
#	Copyright 2008 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

usbinstica1="インストールが完了しました。"
usbinstica2="削除が完了しました。"
usbinstica3="このインストールを実行するには、root としてログオンする必要があります。"

# Instructions on how to install the ctxusb daemon into the init run levels
usbinstica4="ctxusbd デーモンを init 実行レベルに統合することは\n\
できませんでした。\n\n\
これは、次のコマンドを使用できなかったことによります :\n\
\t\t1: install_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
次のコマンドを使って、ctxusbd デーモンを手作業で起動することは\n\
できます :\n\
\t\t/etc/init.d/ctxusbd start\n\n\
ただし、この設定はシャットダウン後には無効となります。この問題を解決\n\
するには、前述のコマンドのいずれかをインストールして USB サポートを\n\
再インストールするか、または ctxusbd デーモンを init 実行レベルに手作業でインストールします。\n"

# Instructions on how to remove the ctxusb daemon from the init run levels
usbinstica5="実行レベルから ctxusbd デーモンを削除することはできませんでした。\n\n\
これは、次のコマンドを使用できなかったことによります :\n\
\t\t1: remove_initd\n\
\t\t2: update-rc.d\n\
\t\t3: chkconfig\n\n\
この問題を解決するには、これらのコマンドのいずれかをインストールして削除スクリプト\n\
を再度実行するか、init 実行レベルから ctxusbd デーモンを手作業で削除します。\n"

usbinstica6="\nルートとして $HOST_PRODUCT_NAME が初めからインストールされている場合\n\
にのみ $DISP_PRODUCT_NAME をインストールできます。"

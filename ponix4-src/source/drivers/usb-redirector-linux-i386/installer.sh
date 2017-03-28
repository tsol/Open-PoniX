#!/bin/sh
#
# IncentivesPro USB Redirector for Linux installation script
#
# Copyright (c) 2008-2013, SimplyCore LLC
#

# Target directories (can be changed to customize the installation)
INSTALLDIR=/usr/local/usb-redirector  # root dir where the software will be installed
FILESDIR=files                     # directory name where installer files are located
MODULESBINDIR=modules/bin          # directory name where compiled stub driver modules are located
MODULESSRCDIR=modules/src          # directory name where stub driver sources are located
SHELLLINKDIR=/usr/local/bin        # directory to place symbolic link for shell executable

# Default kernel sources path
KERNELDIR=/lib/modules/`uname -r`/build

# How to install kernel sources (or kernel headers).
#
# Debian,Ubuntu: apt-get install make gcc linux-headers-`uname -r`
# RedHat,Fedora,Scientific: yum install make gcc kernel-devel-`uname -r`
# Proxmox VE: apt-get install build-essential pve-headers-`uname -r`
# SuSE: sudo zypper in make gcc kernel-devel

# File names
PIDFILE=/var/run/usbsrvd.pid
STUBNAME=tusbd                     # name of the stub driver
DAEMONNAME=usbsrvd                 # name of the daemon executable
SHELLNAME=usbsrv                   # name of the shell executable
CLIENTSHELLNAME=usbclnt            # name of the client shell executable
INITSCRIPTNAME=rc.usbsrvd          # init script name
UNINSTALLERNAME=uninstall.sh       # uninstaller script name
TMPINITSCRIPTNAME=init.tmp         # temporary init script name
TMPUNINSTALLERNAME=uninst.tmp      # temporary uninstaller script name

# System requirements
KERNELMIN="2.6.15"
KERNELMAX="3.15.0"

# Manufacturer
SUPPORTEMAIL="support@incentivespro.com"

###############################################################################
#
# Helper subroutines

exit_on_file_exist()
{
  if ( [ -h $1 ] || [ -f $1 ] || [ -x $1 ] ); then
    echo "!!!  Previous installation detected. Please uninstall it first ($1)"
    exit 1
  fi
}

exit_on_daemon_running()
{
  if [ -n "`ps -A|grep $1`" ]; then
    echo "!!!  Previous installation detected. Please uninstall it first ($1 daemon running)"
    exit 1
  fi
}

exit_on_file_not_exist()
{
  if ( [ ! -f $1 ] && [ ! -x $1 ] && [ ! -h $1 ] ); then
    echo "!!!  Required file $1 is missing. Installer cannot continue."
    exit 2
  fi
}

exit_on_error() {
  if [ $? -ne 0 ]; then
    echo "***  Cleaning up installation..."
    usbsrv_cleanup
    echo "***"
    echo "***  Installation failed!"
    echo "??? $1"
    exit 4
  fi
}

exit_with_error() 
{
    echo "***  Cleaning up installation..."
    usbsrv_cleanup
    echo "***"
    echo "***  INSTALLATION FAILED!"
    echo "??? $1"
    exit 4
}

set_tag() 
{
  local file_path=$1
  local tag=$2
  local value=$3
  # Convert '\' to '\/'
  value=`echo "$value" | sed 's/\//\\\\\//g'`
  # Replace tag "$tag" with "$value" in file $file
  sed s/$tag/$value/ $file_path > settag.tmp
  mv -f settag.tmp ${file_path}
}

###############################################################################
#
# System-depedent subroutines

check_distrib_syscalls()
{
  # RedHat/Fedora/Mandriva
  if [ "$SYS_DISTRIB_NAME" = "redhat" ] || [ "$SYS_DISTRIB_NAME" = "fedora" ] || [ "$SYS_DISTRIB_NAME" = "mandriva" ]; then
    [ ! -x /sbin/chkconfig ] && ERROR_MESSAGE="/sbin/chkconfig not found" && return 1
  # Debian/Ubuntu
  elif [ "$SYS_DISTRIB_NAME" = "debian" ] || [ "$SYS_DISTRIB_NAME" = "ubuntu" ]; then
    [ ! -x `which update-rc.d` ] && ERROR_MESSAGE="update-rc.d not found" && return 1
  # SuSE
  elif [ "$SYS_DISTRIB_NAME" = "suse" ]; then
    [ ! -x /sbin/insserv ] && ERROR_MESSAGE="/sbin/insserv not found" && return 1
  # Gentoo
  elif [ "$SYS_DISTRIB_NAME" = "gentoo" ]; then
    [ ! -x `which rc-update` ] && ERROR_MESSAGE="rc-update not found" && return 1
  fi
  return 0
}

detect_kernel()
{
  SYS_KERNEL_RELEASE=`uname -r`
  SYS_KERNEL_VERSION=`echo $SYS_KERNEL_RELEASE| sed 's/^\([0-9]\+\.[0-9]\+\.[0-9]\+\).*/\1/'`

#  [ "$SYS_KERNEL_VERSION" = "$SYS_KERNEL_RELEASE" ] && SYS_KERNEL_VERSION=unknown

  return 0;
}

# digitize the kernel version so that we are able to compare it
parse_kernel_version()
{
  local kernel_min_major=`echo $KERNELMIN| sed 's/^\([0-9]\+\)\..*/\1/'`
  local kernel_min_minor=`echo $KERNELMIN| sed 's/^\([0-9]\+\)\.\([0-9]\+\)\..*/\2/'`
  local kernel_min_patch=`echo $KERNELMIN| sed 's/^\([0-9]\+\)\.\([0-9]\+\)\.\([0-9]\+\).*/\3/'`
  local kernel_min_build=`echo $KERNELMIN| sed 's/^\([0-9]\+\)\.\([0-9]\+\)\.\([0-9]\+\)\.\([0-9]\+\).*/\4/'`
  
  [ "$kernel_min_major" = "$KERNELMIN" ] && return 1
  [ "$kernel_min_minor" = "$KERNELMIN" ] && return 1
  [ "$kernel_min_patch" = "$KERNELMIN" ] && return 1
  [ "$kernel_min_build" = "$KERNELMIN" ] && kernel_min_build=0
  
  local kernel_max_major=`echo $KERNELMAX| sed 's/^\([0-9]\+\)\..*/\1/'`
  local kernel_max_minor=`echo $KERNELMAX| sed 's/^\([0-9]\+\)\.\([0-9]\+\)\..*/\2/'`
  local kernel_max_patch=`echo $KERNELMAX| sed 's/^\([0-9]\+\)\.\([0-9]\+\)\.\([0-9]\+\).*/\3/'`
  local kernel_max_build=`echo $KERNELMAX| sed 's/^\([0-9]\+\)\.\([0-9]\+\)\.\([0-9]\+\)\.\([0-9]\+\).*/\4/'`
  
  [ "$kernel_max_major" = "$KERNELMAX" ] && return 1
  [ "$kernel_max_minor" = "$KERNELMAX" ] && return 1
  [ "$kernel_max_patch" = "$KERNELMAX" ] && return 1
  [ "$kernel_max_build" = "$KERNELMAX" ] && kernel_max_build=0
  
  local kernel_cur_major=`echo $SYS_KERNEL_VERSION| sed 's/^\([0-9]\+\)\..*/\1/'`
  local kernel_cur_minor=`echo $SYS_KERNEL_VERSION| sed 's/^\([0-9]\+\)\.\([0-9]\+\)[\.-].*/\2/'`
  local kernel_cur_patch=`echo $SYS_KERNEL_VERSION| sed 's/^\([0-9]\+\)\.\([0-9]\+\)\.\([0-9]\+\).*/\3/'`
  local kernel_cur_build=`echo $SYS_KERNEL_VERSION| sed 's/^\([0-9]\+\)\.\([0-9]\+\)\.\([0-9]\+\)\.\([0-9]\+\).*/\4/'`

  [ "$kernel_cur_major" = "$SYS_KERNEL_VERSION" ] && return 1
  [ "$kernel_cur_minor" = "$SYS_KERNEL_VERSION" ] && return 1
  [ "$kernel_cur_patch" = "$SYS_KERNEL_VERSION" ] && kernel_cur_patch=0
  [ "$kernel_cur_build" = "$SYS_KERNEL_VERSION" ] && kernel_cur_build=0

  SYS_KERNEL_MIN_D=$(( $kernel_min_major * 1000000000 + $kernel_min_minor * 1000000 + $kernel_min_patch * 1000 + $kernel_min_build ))
  SYS_KERNEL_MAX_D=$(( $kernel_max_major * 1000000000 + $kernel_max_minor * 1000000 + $kernel_max_patch * 1000 + $kernel_max_build ))
  SYS_KERNEL_CUR_D=$(( $kernel_cur_major * 1000000000 + $kernel_cur_minor * 1000000 + $kernel_cur_patch * 1000 + $kernel_cur_build ))

  return 0
}

detect_distrib()
{
  SYS_DISTRIB_NAME=unknown
  SYS_DISTRIB_MODVER=no
  SYS_DISTRIB_INIT_TYPE=""
  SYS_DISTRIB_INIT_DIR=""
  SYS_DISTRIB_RUNLEVEL_DIR=""
  SYS_DISTRIB_INIT_RCLOCAL=""

  # Fedora (must be before RedHat)
  if [ -f /etc/fedora-release ]; then
    SYS_DISTRIB_NAME=fedora
    SYS_DISTRIB_MODVER=no
    SYS_DISTRIB_INIT_TYPE=sysv
    SYS_DISTRIB_INIT_DIR=/etc/rc.d/init.d
    SYS_DISTRIB_RUNLEVEL_DIR=/etc/rc.d
  # Mandriva (must be before RedHat)
  elif [ -f /etc/mandriva-release ] || [ -f /etc/mandrake-release ] || [ -f /etc/mandakelinux-release ]; then
    SYS_DISTRIB_NAME=mandriva
    SYS_DISTRIB_MODVER=yes
    SYS_DISTRIB_INIT_TYPE=sysv
    SYS_DISTRIB_INIT_DIR=/etc/rc.d/init.d
    SYS_DISTRIB_RUNLEVEL_DIR=/etc/rc.d
  # RedHat/CentOS/Scientific
  elif [ -f /etc/redhat-release ]; then
    SYS_DISTRIB_NAME=redhat
    SYS_DISTRIB_MODVER=yes
    SYS_DISTRIB_INIT_TYPE=sysv
    SYS_DISTRIB_INIT_DIR=/etc/rc.d/init.d
    SYS_DISTRIB_RUNLEVEL_DIR=/etc/rc.d
  # Ubuntu (must be before Debian)
  elif [ -f /etc/debian_version ] && [ -f /etc/lsb-release ] && [ ! -z "`cat /etc/lsb-release|grep Ubuntu`" ]; then
    SYS_DISTRIB_NAME=ubuntu
    SYS_DISTRIB_MODVER=yes
    SYS_DISTRIB_INIT_TYPE=sysv
    SYS_DISTRIB_INIT_DIR=/etc/init.d
    SYS_DISTRIB_RUNLEVEL_DIR=/etc
  # Debian
  elif [ -f /etc/debian_version ]; then
    SYS_DISTRIB_NAME=debian
    SYS_DISTRIB_MODVER=yes
    SYS_DISTRIB_INIT_TYPE=sysv
    SYS_DISTRIB_INIT_DIR=/etc/init.d
    SYS_DISTRIB_RUNLEVEL_DIR=/etc
  # SuSE
  elif [ -f /etc/SuSE-release ]; then
    SYS_DISTRIB_NAME=suse
    SYS_DISTRIB_MODVER=yes
    SYS_DISTRIB_INIT_TYPE=sysv
    SYS_DISTRIB_INIT_DIR=/etc/init.d
    SYS_DISTRIB_RUNLEVEL_DIR=/etc/init.d
  # Gentoo
  elif [ -f /etc/gentoo-release ]; then
    SYS_DISTRIB_NAME=gentoo
    SYS_DISTRIB_MODVER=no
    SYS_DISTRIB_INIT_TYPE=sysv
    SYS_DISTRIB_INIT_DIR=
    SYS_DISTRIB_RUNLEVEL_DIR=
  # Slackware
  elif [ -f /etc/slackware-version ]; then
    SYS_DISTRIB_NAME=slackware
    SYS_DISTRIB_MODVER=no
    SYS_DISTRIB_INIT_TYPE=sysv
    SYS_DISTRIB_INIT_DIR=/etc/rc.d
    SYS_DISTRIB_RUNLEVEL_DIR=/etc/rc.d
  else
    # Other generic Linuxes
    SYS_DISTRIB_NAME=other
    SYS_DISTRIB_MODVER=no
    if [ -d /etc/init.d ]; then
      SYS_DISTRIB_INIT_TYPE=sysv
      SYS_DISTRIB_INIT_DIR=/etc/init.d
      SYS_DISTRIB_RUNLEVEL_DIR=/etc
    elif [ -d /etc/rc.d/init.d ]; then
      SYS_DISTRIB_INIT_TYPE=sysv
      SYS_DISTRIB_INIT_DIR=/etc/rc.d/init.d
      SYS_DISTRIB_RUNLEVEL_DIR=/etc/rc.d
    elif [ -f /etc/rc.local ]; then
      SYS_DISTRIB_INIT_TYPE=bsd
      SYS_DISTRIB_INIT_RCLOCAL=/etc/rc.local
    elif [ -f /etc/rc.d/rc.local ]; then
      SYS_DISTRIB_INIT_TYPE=bsd
      SYS_DISTRIB_INIT_RCLOCAL=/etc/rc.d/rc.local
    fi
  fi

  [ -z "$SYS_DISTRIB_INIT_TYPE" ] && return 1

  if [ "$SYS_DISTRIB_INIT_TYPE" = "sysv" ]; then
    [ -z "$SYS_DISTRIB_INIT_DIR" ] && return 1
    [ -z "$SYS_DISTRIB_RUNLEVEL_DIR" ] && return 1
    [ ! -d "$SYS_DISTRIB_INIT_DIR" ] && return 1
    [ ! -d "$SYS_DISTRIB_RUNLEVEL_DIR" ] && return 1
  else
    [ -z "$SYS_DISTRIB_INIT_RCLOCAL" ] && return 1
    [ ! -f "$SYS_DISTRIB_INIT_RCLOCAL" ] && return 1
  fi

  return 0
}

# $1 source init script path name
# $2 destination init script name
# $3 start number (2 symbols)
# $4 stop number (2 symbols)
install_init_script() 
{
  local src_script_path=$1
  local dst_script_name=$2

  local dst_script_path

  uninstall_init_script $2

  if [ "$SYS_DISTRIB_INIT_TYPE" = "sysv" ]; then
    dst_script_path="$SYS_DISTRIB_INIT_DIR/$dst_script_name"
  elif [ "$SYS_DISTRIB_INIT_TYPE" = "bsd" ]; then
#    dst_script_name="rc.$dst_script_name"
    dst_script_path="/etc/rc.d/$dst_script_name"
  else
    ERROR_MESSAGE="unknown init type"
    return 1
  fi

  cp "$src_script_path" "$dst_script_path"

  if [ ! $? -eq 0 ]; then
    ERROR_MESSAGE="can not copy init script to $dst_script_path"
    return 1
  fi

  chmod 755 "$dst_script_path" 2>/dev/null

  if [ ! $? -eq 0 ]; then
    ERROR_MESSAGE="can not chmod init script $dst_script_path"
    return 1
  fi

  # RedHat/Fedora/Mandriva
  if [ "$SYS_DISTRIB_NAME" = "redhat" ] || [ "$SYS_DISTRIB_NAME" = "fedora" ] || [ "$SYS_DISTRIB_NAME" = "mandriva" ]; then

    /sbin/chkconfig --level 35 $dst_script_name on >/dev/null 2>&1 || return 1

  # Debian/Ubuntu
  elif [ "$SYS_DISTRIB_NAME" = "debian" ] || [ "$SYS_DISTRIB_NAME" = "ubuntu" ]; then

    update-rc.d $dst_script_name defaults $3 $4 >/dev/null 2>&1

  # SuSE
  elif [ "$SYS_DISTRIB_NAME" = "suse" ]; then

    /sbin/insserv $dst_script_name>/dev/null 2>&1

  # Gentoo
  elif [ "$SYS_DISTRIB_NAME" = "gentoo" ]; then

    rc-update add $dst_script_name default >/dev/null 2>&1

  # BSD
  elif [ "$SYS_DISTRIB_INIT_TYPE" = "bsd" ]; then
    echo "Installer can not correctly set up init script. The script has been copied"
    echo "to $dst_script_path. If you would like the software to start automatically"
    echo "at system boot, please add this script to $SYS_DISTRIB_INIT_RCLOCAL or"
    echo "start it manually"
  # Slackware and others
  elif [ "$SYS_DISTRIB_INIT_TYPE" = "sysv" ]; then
    # Should match with runlevels in uninstall_init_script()
    ln -fs "$dst_script_path" "$SYS_DISTRIB_RUNLEVEL_DIR/rc0.d/K$4$dst_script_name" >/dev/null 2>&1
    ln -fs "$dst_script_path" "$SYS_DISTRIB_RUNLEVEL_DIR/rc1.d/K$4$dst_script_name" >/dev/null 2>&1
    ln -fs "$dst_script_path" "$SYS_DISTRIB_RUNLEVEL_DIR/rc2.d/K$4$dst_script_name" >/dev/null 2>&1
    ln -fs "$dst_script_path" "$SYS_DISTRIB_RUNLEVEL_DIR/rc3.d/S$3$dst_script_name" >/dev/null 2>&1
    ln -fs "$dst_script_path" "$SYS_DISTRIB_RUNLEVEL_DIR/rc4.d/S$3$dst_script_name" >/dev/null 2>&1
    ln -fs "$dst_script_path" "$SYS_DISTRIB_RUNLEVEL_DIR/rc5.d/S$3$dst_script_name" >/dev/null 2>&1
    ln -fs "$dst_script_path" "$SYS_DISTRIB_RUNLEVEL_DIR/rc6.d/K$4$dst_script_name" >/dev/null 2>&1
  fi
  return 0
}

# $1 init script name
# should match with uninstaller's uninstall_init_script()
uninstall_init_script()
{
  local dst_script_name=$1
  local dst_script_path

  if [ "$SYS_DISTRIB_INIT_TYPE" = "sysv" ]; then
    dst_script_path="$SYS_DISTRIB_INIT_DIR/$dst_script_name"
  elif [ "$SYS_DISTRIB_INIT_TYPE" = "bsd" ]; then
#    dst_script_name="rc.$dst_script_name"
    dst_script_path="/etc/rc.d/$dst_script_name"
  else
    ERROR_MESSAGE="unknown init type"
    return 1
  fi

  rm -f "$dst_script_path" >/dev/null 2>&1

  # RedHat/Fedora/Mandriva
  if [ "$SYS_DISTRIB_NAME" = "redhat" ] || [ "$SYS_DISTRIB_NAME" = "fedora" ] || [ "$SYS_DISTRIB_NAME" = "mandriva" ]; then
 
    /sbin/chkconfig --del $dst_script_name >/dev/null 2>&1

  # Debian/Ubuntu
  elif [ "$SYS_DISTRIB_NAME" = "debian" ] || [ "$SYS_DISTRIB_NAME" = "ubuntu" ]; then

    update-rc.d -f $dst_script_name remove >/dev/null 2>&1

  # SuSE
  elif [ "$SYS_DISTRIB_NAME" = "suse" ]; then

    /sbin/insserv -r $dst_script_name >/dev/null 2>&1

  # Gentoo
  elif [ "$SYS_DISTRIB_NAME" = "gentoo" ]; then

    rc-update del $dst_script_name >/dev/null 2>&1

  # bsd
  elif [ "$SYS_DISTRIB_INIT_TYPE" = "bsd" ]; then
    # nothing to do
    echo "" >/dev/null
  # Slackware and others
  elif [ "$SYS_DISTRIB_INIT_TYPE" = "sysv" ]; then
    # Should match with runlevels in add_runlevels()
    rm "$SYS_DISTRIB_RUNLEVEL_DIR/rc0.d/K??$dst_script_name" >/dev/null 2>&1
    rm "$SYS_DISTRIB_RUNLEVEL_DIR/rc1.d/K??$dst_script_name" >/dev/null 2>&1
    rm "$SYS_DISTRIB_RUNLEVEL_DIR/rc2.d/K??$dst_script_name" >/dev/null 2>&1
    rm "$SYS_DISTRIB_RUNLEVEL_DIR/rc3.d/S??$dst_script_name" >/dev/null 2>&1
    rm "$SYS_DISTRIB_RUNLEVEL_DIR/rc4.d/S??$dst_script_name" >/dev/null 2>&1
    rm "$SYS_DISTRIB_RUNLEVEL_DIR/rc5.d/S??$dst_script_name" >/dev/null 2>&1
    rm "$SYS_DISTRIB_RUNLEVEL_DIR/rc6.d/K??$dst_script_name" >/dev/null 2>&1
  fi

  return 0
}

# $1 init script name
start_init_script()
{
  local name=$1

  if [ "$SYS_DISTRIB_INIT_TYPE" = "sysv" ]; then
    "$SYS_DISTRIB_INIT_DIR/$name" start >/dev/null 2>&1
  elif [ "$SYS_DISTRIB_INIT_TYPE" = "bsd" ]; then
#    if [ -x "/etc/rc.d/rc.$name" ]; then
#      "/etc/rc.d/rc.$name" start >/dev/null 2>&1
#    fi
    if [ -x "/etc/rc.d/$name" ]; then
      "/etc/rc.d/$name" start >/dev/null 2>&1
    fi
  else
    ERROR_MESSAGE="unknown init type"
    return 1
  fi
  return 0
}

# $1 init script name
# should match with uninstaller's stop_init_script()
stop_init_script() 
{
  local name=$1

  if [ "$SYS_DISTRIB_INIT_TYPE" = "sysv" ]; then
    if [ -x "$SYS_DISTRIB_INIT_DIR/$name" ]; then
      "$SYS_DISTRIB_INIT_DIR/$name" stop >/dev/null 2>&1
    fi
  elif [ "$SYS_DISTRIB_INIT_TYPE" = "bsd" ]; then
#    if [ -x "/etc/rc.d/rc.$name" ]; then
#      "/etc/rc.d/rc.$name" stop >/dev/null 2>&1
#    fi
    if [ -x "/etc/rc.d/$name" ]; then
      "/etc/rc.d/$name" stop >/dev/null 2>&1
    fi
  else
    ERROR_MESSAGE="unknown init type"
    return 1
  fi
  return 0
}


###############################################################################
#
# Installer subroutines

usbsrv_check_syscalls()
{
  [ ! -x `which sed` ]   && ERROR_MESSAGE="sed not found"    && return 1
  [ ! -x `which grep` ]  && ERROR_MESSAGE="grep not found"   && return 1
  [ ! -x `which chmod` ] && ERROR_MESSAGE="chmod not found"  && return 1
  [ ! -x `which ln` ]    && ERROR_MESSAGE="ln not found"     && return 1
  [ ! -x /sbin/insmod ]  && ERROR_MESSAGE="insmod not found" && return 1
  [ ! -x /sbin/rmmod ]   && ERROR_MESSAGE="rmmod not found"  && return 1
  [ ! -x `which killall` ] && ERROR_MESSAGE="killall not found" && return 1
  return 0
}

usbsrv_prepare_uninstaller()
{
  local tmpfile="$INSTALLDIR/$TMPUNINSTALLERNAME"

  cp ./$FILESDIR/$UNINSTALLERNAME $tmpfile

  set_tag "$tmpfile" "%INSTALLDIR_TAG%"     "$INSTALLDIR"		  || return 1
  set_tag "$tmpfile" "%SHELLLINKDIR_TAG%"   "$SHELLLINKDIR"		  || return 1
  set_tag "$tmpfile" "%STUBNAME_TAG%"          "$STUBNAME"        || return 1
  set_tag "$tmpfile" "%DAEMONNAME_TAG%"        "$DAEMONNAME"      || return 1
  set_tag "$tmpfile" "%SHELLNAME_TAG%"         "$SHELLNAME"       || return 1
  set_tag "$tmpfile" "%CLIENTSHELLNAME_TAG%"   "$CLIENTSHELLNAME" || return 1
  set_tag "$tmpfile" "%UNINSTALLERNAME_TAG%"   "$UNINSTALLERNAME" || return 1
  set_tag "$tmpfile" "%INITSCRIPTNAME_TAG%"    "$INITSCRIPTNAME"  || return 1
  set_tag "$tmpfile" "%SYS_DISTRIB_NAME_TAG%"          "$SYS_DISTRIB_NAME"         || return 1
  set_tag "$tmpfile" "%SYS_DISTRIB_MODVER_TAG%"        "$SYS_DISTRIB_MODVER"       || return 1
  set_tag "$tmpfile" "%SYS_DISTRIB_INIT_TYPE%"         "$SYS_DISTRIB_INIT_TYPE"    || return 1
  set_tag "$tmpfile" "%SYS_DISTRIB_INIT_DIR%"          "$SYS_DISTRIB_INIT_DIR"     || return 1
  set_tag "$tmpfile" "%SYS_DISTRIB_RUNLEVEL_DIR_TAG%"  "$SYS_DISTRIB_RUNLEVEL_DIR" || return 1
  set_tag "$tmpfile" "%SYS_DISTRIB_INIT_RCLOCAL_TAG%"  "$SYS_DISTRIB_INIT_RCLOCAL" || return 1
  set_tag "$tmpfile" "%SYS_KERNEL_RELEASE_TAG%"        "$SYS_KERNEL_RELEASE" || return 1
  set_tag "$tmpfile" "%SYS_KERNEL_VERSION_TAG%"        "$SYS_KERNEL_VERSION" || return 1
  return 0
}

usbsrv_prepare_init_script() 
{
  local tmpfile="$INSTALLDIR/$TMPINITSCRIPTNAME"

  cp ./$FILESDIR/$INITSCRIPTNAME $tmpfile

  set_tag "$tmpfile" "%INSTALLDIR_TAG%" "$INSTALLDIR" || return 1
  set_tag "$tmpfile" "%PIDFILE_TAG%" "$PIDFILE" || return 1
  set_tag "$tmpfile" "%STUBNAME_TAG%"   "$STUBNAME"   || return 1
  set_tag "$tmpfile" "%DAEMONNAME_TAG%" "$DAEMONNAME" || return 1
  return 0
}

usbsrv_check_previous_old_installation() 
{
 #exit_on_daemon_running "usbsrvd"
  exit_on_file_exist /usr/local/usb-server/usbsrvd
  exit_on_file_exist /usr/local/usb-server/bin/tusbd.ko
  exit_on_file_exist /usr/local/usb-server/bin/usbsrv
  exit_on_file_exist /usr/local/usb-server/bin/usbclnt
}

usbsrv_check_previous_installation() 
{
  #exit_on_daemon_running $DAEMONNAME
  exit_on_file_exist $INSTALLDIR/bin/$DAEMONNAME
  exit_on_file_exist $INSTALLDIR/bin/$STUBNAME.ko
  exit_on_file_exist $INSTALLDIR/bin/$SHELLNAME
  exit_on_file_exist $INSTALLDIR/bin/$CLIENTSHELLNAME
  exit_on_file_exist $INSTALLDIR/$UNINSTALLERNAME
  exit_on_file_exist $SHELLLINKDIR/$SHELLNAME
}

usbsrv_check_source_files() 
{
  exit_on_file_not_exist ./$FILESDIR/$UNINSTALLERNAME
  exit_on_file_not_exist ./$FILESDIR/$INITSCRIPTNAME

  if [ $1 = 'both' ]; then
    exit_on_file_not_exist ./$FILESDIR/$SHELLNAME
    exit_on_file_not_exist ./$FILESDIR/$CLIENTSHELLNAME
    exit_on_file_not_exist ./$FILESDIR/$DAEMONNAME
  elif [ $1 = 'server' ]; then
    exit_on_file_not_exist ./$FILESDIR/$SHELLNAME
    exit_on_file_not_exist ./$FILESDIR/$DAEMONNAME-srv
  else
    exit_on_file_not_exist ./$FILESDIR/$CLIENTSHELLNAME
    exit_on_file_not_exist ./$FILESDIR/$DAEMONNAME-cl
  fi
}

usbsrv_copy_files()
{
  if [ $1 = 'both' ]; then
    cp ./$FILESDIR/$DAEMONNAME $INSTALLDIR/bin
    exit_on_error "Cannot copy file $DAEMONNAME"
    cp ./$FILESDIR/$SHELLNAME $INSTALLDIR/bin
    exit_on_error "Cannot copy file $SHELLNAME"
    cp ./$FILESDIR/$CLIENTSHELLNAME $INSTALLDIR/bin
    exit_on_error "Cannot copy file $CLIENTSHELLNAME"
  elif [ $1 = 'server' ]; then
    cp ./$FILESDIR/$DAEMONNAME-srv $INSTALLDIR/bin/$DAEMONNAME
    exit_on_error "Cannot copy file $DAEMONNAME-srv"
    cp ./$FILESDIR/$SHELLNAME $INSTALLDIR/bin
    exit_on_error "Cannot copy file $SHELLNAME"
  else
    cp ./$FILESDIR/$DAEMONNAME-cl $INSTALLDIR/bin/$DAEMONNAME
    exit_on_error "Cannot copy file $DAEMONNAME-cl"
    cp ./$FILESDIR/$CLIENTSHELLNAME $INSTALLDIR/bin
    exit_on_error "Cannot copy file $CLIENTSHELLNAME"
  fi
  cp "$SYS_KERNEL_MODULE_DIR/$SYS_KERNEL_MODULE_NAME" $INSTALLDIR/bin/$STUBNAME.ko
  exit_on_error "Cannot copy file $SYS_KERNEL_MODULE_DIR/$SYS_KERNEL_MODULE_NAME"
  cp $INSTALLDIR/$TMPUNINSTALLERNAME "$INSTALLDIR/$UNINSTALLERNAME"
  exit_on_error "Cannot copy file $UNINSTALLERNAME"
}

usbsrv_make_kernel_module() 
{
  local script_dir=$PWD
  local make_flags=""
  local driver_config=""
  local module_dir="$script_dir/$FILESDIR/$MODULESSRCDIR/$STUBNAME"

  if ( [ $1 = 'both' ] || [ $1 = 'server' ] ); then
    # kernels 2.6.25-2.6.27.20 and 2.6.28-2.6.28.8 have a memory leak in ISO transfers on EHCI controllers
    # try to walkaround

    if ( [ $SYS_KERNEL_CUR_D -ge 2006025000 ] && [ $SYS_KERNEL_CUR_D -le 2006027020 ] ) || \
       ( [ $SYS_KERNEL_CUR_D -ge 2006028000 ] && [ $SYS_KERNEL_CUR_D -le 2006028008 ] ); then
      echo "!!!  Warning! Your kernel version has a bug with EHCI USB controllers which"
      echo "     leads to large memory leaks with isochronous devices like Webcams or"
      echo "     Sound cards."
      echo "     If you have patched your kernel against this bug - please manually"
      echo "     recompile the driver using 'make USE_EHCI_FIX=n' command. Otherwise"
      echo "     the installer will try to automatically walk around this bug (without"
      echo "     guaranties)."
      echo "     Affected kernels are 2.6.25-2.6.27.20 and 2.6.28-2.6.28.8."
      echo "     See http://patchwork.kernel.org/patch/13428/ for more information."
      echo "     P.S. You should not worry about this warning if you're not going to"
      echo "          use USB 2.0 isochronous devices on your system."
      make_flags="USE_EHCI_FIX=y"
    else
      make_flags="USE_EHCI_FIX=n"
    fi
  fi
   
  if [ $1 = 'both' ]; then
    driver_config="VHCI=y STUB=y"
  elif [ $1 = 'server' ]; then
    driver_config="VHCI=n STUB=y"
  else
    driver_config="VHCI=y STUB=n"
  fi

  echo "***  Compiling kernel module..."
 
  cd $module_dir
 
  make KERNELDIR=$KERNELDIR clean >/dev/null 2>&1
  make $make_flags $driver_config KERNELDIR=$KERNELDIR >$script_dir/buildlog.txt 2>&1
 
  if ( [ $? -eq 0 ] ); then
    echo "***  Kernel module succesfully compiled"
    cd $script_dir
    # Store module location
    SYS_KERNEL_MODULE_RELEASE="$SYS_KERNEL_RELEASE"
    SYS_KERNEL_MODULE_DIR="$module_dir"
    SYS_KERNEL_MODULE_NAME="$STUBNAME.ko"
  else
    # Cleanup
    make KERNELDIR=$KERNELDIR clean >/dev/null 2>&1
    cd $script_dir
    return 5
  fi
  return 0
}

usbsrv_set_permissions()
{
  # Set permissions on copied files
  chmod 744 $INSTALLDIR/bin/$DAEMONNAME
  exit_on_error "Cannot chmod the daemon"
  chmod 644 $INSTALLDIR/bin/$STUBNAME.ko
  exit_on_error "Cannot chmod the driver"
  chmod 744 $INSTALLDIR/$UNINSTALLERNAME
  exit_on_error "Cannot chmod the uninstaller script"

  if ( [ $1 = 'both' ] || [ $1 = 'server' ] ); then
    chmod 755 $INSTALLDIR/bin/$SHELLNAME
    exit_on_error "Cannot chmod the server shell"
  fi

  if ( [ $1 = 'both' ] || [ $1 = 'client' ] ); then
    chmod 755 $INSTALLDIR/bin/$CLIENTSHELLNAME
    exit_on_error "Cannot chmod the client shell"
  fi

  if ( [ $1 = 'both' ] || [ $1 = 'server' ] ); then
    ln -s $INSTALLDIR/bin/$SHELLNAME $SHELLLINKDIR/$SHELLNAME
    exit_on_error "Cannot create link $SHELLLINKDIR/$SHELLNAME"
  fi

  if ( [ $1 = 'both' ] || [ $1 = 'client' ] ); then
    ln -s $INSTALLDIR/bin/$CLIENTSHELLNAME $SHELLLINKDIR/$CLIENTSHELLNAME
    exit_on_error "Cannot create link $SHELLLINKDIR/$CLIENTSHELLNAME"
  fi
}

###############################################################################
#
# Actual installer subroutine

usbsrv_install()
{
  echo " "
  echo "*** Installing USB Redirector for Linux v3.1"
  echo "***  Destination dir: $INSTALLDIR"

  echo "***  Checking installation..."

  # Make sure we have all required syscalls
  usbsrv_check_syscalls
  exit_on_error "Syscalls check failed: $ERROR_MESSAGE"

  # Make sure no components from old version are installed
  usbsrv_check_previous_old_installation

  # Make sure no components from previous installation are installed
  usbsrv_check_previous_installation
 
  # Make sure we have all required files in the current dir
  usbsrv_check_source_files $1

  echo "***  Detecting system..."

  # Try to detect current distribution name
  detect_distrib
  exit_on_error "Can not detect Linux distribution, possibly it is not supported."

  echo "***     distribution: $SYS_DISTRIB_NAME"

  check_distrib_syscalls
  exit_on_error "Syscalls check failed: $ERROR_MESSAGE"

  # Try to detect kernel version
  detect_kernel
  exit_on_error "Can not detect kernel version"

  echo "***     kernel: $SYS_KERNEL_RELEASE"

  parse_kernel_version
  exit_on_error "Can not parse kernel version."

  if ( [ $SYS_KERNEL_CUR_D -lt $SYS_KERNEL_MIN_D ] || [ $SYS_KERNEL_CUR_D -gt $SYS_KERNEL_MAX_D ] ); then
    answer=""
    while [ "$answer" = "" ];
    do
      echo -n "Your kernel version is not officially suppported! Would like to try installing anyway [y/n] ? "
      read answer
      case "$answer" in
        "y" | "Y") answer="y";;
        "n" | "N") answer="n";;
         *) answer="";;
      esac
    done
    if ( [ "$answer" = "n" ] ); then
      exit_with_error "Sorry, your kernel version is not supported yet. Please contact us at $SUPPORTEMAIL for update."
    else
      echo "You're doing it at your own risk! %)"
    fi
  fi

  if [ ! -d $KERNELDIR ]; then
    exit_with_error "Kernel sources or kernel headers directory not found. Please install the corresponding package first."
  fi

  usbsrv_make_kernel_module $1
  exit_on_error "Cannot compile kernel module. Installation terminated. See buildlog.txt file for more information."

  echo "***  Creating directories..."
  
  # Create program directory
  mkdir -p $INSTALLDIR/bin
  exit_on_error "Cannot create directory $INSTALLDIR/bin"

  echo "***  Preparing scripts..."

  # Prepare uninstaller
  usbsrv_prepare_uninstaller
  exit_on_error "Can not prepare uninstaller script"

  # Prepare init script
  usbsrv_prepare_init_script
  exit_on_error "Can not prepare init script"

  echo "***  Copying files..."
  
  usbsrv_copy_files $1
  usbsrv_set_permissions $1

  echo "***  Setting up init script..."

  install_init_script "$INSTALLDIR/$TMPINITSCRIPTNAME" $INITSCRIPTNAME 99 01
  exit_on_error "Init script installation error: $ERROR_MESSAGE"

  echo "***  Starting daemon..."

  start_init_script $INITSCRIPTNAME
  exit_on_error "Cannot start daemon: $ERROR_MESSAGE"

  # Remove temporary scripts
  rm -f $INSTALLDIR/$TMPINITSCRIPTNAME
  rm -f $INSTALLDIR/$TMPUNINSTALLERNAME

  if ( [ $1 = 'both' ] || [ $1 = 'server' ] ); then
    echo "***  Please allow incoming connections on 32032 port for USB Sever to be able to accept connections from remote clients."
  fi

  echo "***  INSTALLATION SUCCESSFUL! To uninstall, run $INSTALLDIR/$UNINSTALLERNAME"
}

usbsrv_cleanup()
{
  # Remove init script
  stop_init_script $INITSCRIPTNAME
  uninstall_init_script $INITSCRIPTNAME

  # Remove program files and symbolic links
  rm -f $INSTALLDIR/bin/$DAEMONNAME
  rm -f $INSTALLDIR/bin/$STUBNAME.ko
  rm -f $INSTALLDIR/bin/$SHELLNAME
  rm -f $INSTALLDIR/bin/$CLIENTSHELLNAME
  rm -f $INSTALLDIR/$UNINSTALLERNAME

  # Check if this is a symbolic link.
  if ( [ -h $SHELLLINKDIR/$SHELLNAME ] ); then 
    rm -f $SHELLLINKDIR/$SHELLNAME 
  fi

  # Check if this is a symbolic link.
  if ( [ -h $SHELLLINKDIR/$CLIENTSHELLNAME ] ); then 
    rm -f $SHELLLINKDIR/$CLIENTSHELLNAME 
  fi

  # Remove temporary scripts
  rm -f $INSTALLDIR/$TMPINITSCRIPTNAME
  rm -f $INSTALLDIR/$TMPUNINSTALLERNAME

  # Remove program directories  
  rmdir $INSTALLDIR/bin 2> /dev/null
  rmdir $INSTALLDIR 2> /dev/null
}


###############################################################################
#
# Script entry point

if ( [ `whoami` != "root" ] ); then
  echo "IncentivesPro USB Redirector for Linux v3.1 installation script"
  echo "This script is intended to be run under root account!"
  exit 255
fi

case "$1" in
  'install')
    usbsrv_install both
    ;;
  'install-client')
    usbsrv_install client
    ;;
  'install-server')
    usbsrv_install server
    ;;
  *)
    echo "IncentivesPro USB Redirector for Linux v3.1 installation script"
    echo ""
    echo "Usage: installer.sh install           - install both client and server"
    echo "       installer.sh install-server    - install server only"
    echo "       installer.sh install-client    - install client only"
esac




#!/bin/sh
#
# IncentivesPro USB Redirector for Linux uninstallation script
#

# Directories
INSTALLDIR=%INSTALLDIR_TAG%
SHELLLINKDIR=%SHELLLINKDIR_TAG%

# File names
STUBNAME=%STUBNAME_TAG%
DAEMONNAME=%DAEMONNAME_TAG%
SHELLNAME=%SHELLNAME_TAG%
CLIENTSHELLNAME=%CLIENTSHELLNAME_TAG%
UNINSTALLERNAME=%UNINSTALLERNAME_TAG%
INITSCRIPTNAME=%INITSCRIPTNAME_TAG%

# System properties
SYS_DISTRIB_NAME=%SYS_DISTRIB_NAME_TAG%
SYS_DISTRIB_MODVER=%SYS_DISTRIB_MODVER_TAG%
SYS_DISTRIB_INIT_TYPE=%SYS_DISTRIB_INIT_TYPE%
SYS_DISTRIB_INIT_DIR=%SYS_DISTRIB_INIT_DIR%
SYS_DISTRIB_RUNLEVEL_DIR=%SYS_DISTRIB_RUNLEVEL_DIR_TAG%
SYS_DISTRIB_INIT_RCLOCAL=%SYS_DISTRIB_INIT_RCLOCAL_TAG%
SYS_KERNEL_RELEASE=%SYS_KERNEL_RELEASE_TAG%
SYS_KERNEL_VERSION=%SYS_KERNEL_VERSION_TAG%

###############################################################################
#
# System-depended subroutines

# $1 init script name
#
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

# $1 init script name
#
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


###############################################################################
#
# Uninstaller subroutine

usbsrv_uninstall()
{
  echo " "
  echo "*** Uninstalling USB Redirector..."

  echo "***  Stopping daemon..."

  stop_init_script $INITSCRIPTNAME

  killall -q -1 $DAEMONNAME

  # Wait while daemon terminates
  while ( [ -n "`ps -A|grep $DAEMONNAME`" ] ); do
    sleep 1
  done

  echo "***  Stopping kernel module..."
  
  /sbin/rmmod $STUBNAME.ko 2> /dev/null

  echo "***  Removing files..."

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

  # Remove program directories  
  rmdir $INSTALLDIR/bin 2>/dev/null
  rmdir $INSTALLDIR 2>/dev/null

  echo "***  UNINSTALLATION SUCCESSFUL!"
}

###############################################################################
#
# Script entry point

if ( [ `whoami` != "root" ] ); then
  echo "IncentivesPro USB Redirector for Linux uninstallation script"
  echo "This script is intended to be run under root account!"
  exit 255
fi

case "$1" in
  'uninstall')
    usbsrv_uninstall
    ;;
  *)
    echo "IncentivesPro USB Redirector for Linux v3.1 uninstallation script"
    echo ""
    echo "Usage: uninstall.sh uninstall         - uninstall USB Redirector for Linux"
    echo "       uninstall.sh help              - show this help"
esac



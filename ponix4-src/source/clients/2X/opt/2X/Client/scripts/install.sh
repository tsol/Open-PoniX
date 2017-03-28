#!/bin/bash

# Install script for 2X Application Server Client for Linux
# Copyright 2006-2013 2X Software Ltd. All Rights Reserved

# Global variables
LINE_TO_ADD=""
LIBEXPAT="libexpat.so.0.5.0"
LIBEXPAT_PATH=""
LIBEXPAT_DETAILS=""
USERNAMES=$(cat /etc/passwd | grep /home | awk -F: '{if($3 >= 999) print $1}') #this method relies on user group being >= 999

function get_installation_dir()
{
  # Local variables
  local C_DIR="$(pwd)"
  local PRODUCT="$1"
  local INSTALL_SCRIPT="$2"
  local RETURN_STRING=""
  local FULL_PATH=""
  declare -a FOUND_LIST

  # Try to guess the installation directory
  RETURN_STRING="${C_DIR%%2X/${PRODUCT}*}2X/${PRODUCT}"
  # Try to test the directory and if successfull return string
  [ -d "${RETURN_STRING}" ] && echo "${RETURN_STRING}" && return 0
  # If not try to find the directory in another way
  # Go to /proc/<process number>/fd and try to find the full path from there
  cd "/proc/$$/fd" 2>/dev/null
  FULL_PATH="$(ls -l | grep "${INSTALL_SCRIPT}" | awk -F"-> " '{print $2}')"
  RETURN_STRING="${FULL_PATH%%2X/${PRODUCT}*}2X/${PRODUCT}"
  # Test again whether the directory exists or not
  [ -d "${RETURN_STRING}" ] && echo "${RETURN_STRING}" && return 0
  # Try to use the find utility 
  # Initialize counter
  I=0
  # Use for loop to build array with found results
  for FOUND in $(find / \( -path /proc -prune \) , \( -path /media -prune \) , \( -path /dev -prune \) , -regex .*2X/${PRODUCT}.*/${INSTALL_SCRIPT})
  do
    FOUND_LIST[${I}]="${FOUND}"
    let "I=I+1"
  done
  # More then one entry denotes two installations.  User needs to remove old installation
  [ ${#FOUND_LIST[@]} -gt 1 -o ${#FOUND_LIST[@]} -eq 0 ] && RETURN_STRING="" && echo "${RETURN_STRING}" && return 0
  [ ${#FOUND_LIST[@]} -eq 1 ] && RETURN_STRING="${FOUND_LIST[0]}"
  [ -d "${RETURN_STRING}" ] && echo "${RETURN_STRING}" && return 0
  RETURN_STRING="" && echo "${RETURN_STRING}" && return 1
}

function install_binary_links()
{
  # Local variables
  local WHERE_TO_LINK="$1"
  local FROM_TO_LINK="$2"
  declare -a BINS="$3"
  local C_DIR=$(pwd)

  # Go to the link destination directory
  cd "${WHERE_TO_LINK}"
  # Create links
  for (( I=0; I<${#BINS[@]}; I++ ))
  do
    [ -L "${BINS[$I]}" ] && rm -f "${BINS[$I]}"
    ln -s "${FROM_TO_LINK}/${BINS[$I]}" "${BINS[$I]}" 2>/dev/null
    [ ! -L "${BINS[$I]}" ] && return 1
  done
  # Go back to the original directory
  cd "${C_DIR}"
  return 0
}

function install_links()
{
  # Local variables
  local INST_DIR="$1"
  declare -a LINKS="$2"
  declare -a LIBS="$3"
  local C_DIR=$(pwd)

  # Go to the libs directory
  cd "${INST_DIR}/lib"
  # Create links
  for (( I=0; I<${#LIBS[@]}; I++ ))
  do
    [ -L "${LINKS[$I]}" ] && rm -f "${LINKS[$I]}"
    ln -s "${LIBS[$I]}" "${LINKS[$I]}" 2>/dev/null
    [ ! -L "${LINKS[$I]}" ] && return 1
  done
  # Go back to the original directory
  cd "${C_DIR}"
  return 0
}

function return_tool_path()
{
  # Local variables
  local TOOL="$1"
  declare -a TOOL_PATHS="$2"

  # Loop through the paths array
  for (( I=0; I<${#TOOL_PATHS[@]}; I++ ))
  do
    # Do an ls and filter for the tool
    ls "${TOOL_PATHS[$I]}" | grep -w "${TOOL}" | grep -v "${TOOL}." >/dev/null 2>&1
    # If found return the full path of the tool
    [ $? -eq 0 ] && echo "${TOOL_PATHS[$I]}/${TOOL}" && return 0
  done
  # If not found exit with an empty string
  echo "" && return 1
}

function which_libexpat_is_installed()
{
  # Local variables
  declare -a LIBEXPAT_LIBRARIES="$1"
  local LIBEXPAT_PATH=""

  for (( X=0; X<${#LIBEXPAT_LIBRARIES[@]}; X++ ))
  do
    LIBEXPAT_PATH="$(return_tool_path "${LIBEXPAT_LIBRARIES[$X]}" "("/lib" "/usr/lib")")"
    [ -n "${LIBEXPAT_PATH}" ] && break
  done
  echo "${LIBEXPAT_PATH}"
}

function install_app_menu()
{
#Cinnamon update for applications menu 
if [ -x "`which cinnamon 2>/dev/null`" ]; then
  for USER in ${USERNAMES}; do
    local CONFIG_PATH=$(getent passwd $USER | cut -d: -f6)/.config
    local MENUS_PATH="$CONFIG_PATH/menus"
    local CINN_APPS_MENU_PATH="$MENUS_PATH/cinnamon-applications.menu"
    local APP_MERGED_MENU_PATH="$MENUS_PATH/applications-merged"
    local TUXCLIENT_MENU_PATH="$APP_MERGED_MENU_PATH/2XClient.menu"
    if [ -e $CONFIG_PATH ]; then
      if [ ! -e $MENUS_PATH ]; then
        mkdir -p $MENUS_PATH
      fi
      if [ ! -e $APP_MERGED_MENU_PATH ]; then
        mkdir -p $APP_MERGED_MENU_PATH
        chmod 777 $APP_MERGED_MENU_PATH
      fi
      if [ -e $CINN_APPS_MENU_PATH ]; then
        #sed command below appends mergefile line under <Name>Applications<\Name>
        sed -i '\:<Name.*Applications.*/Name>:a   <MergeFile>'"$TUXCLIENT_MENU_PATH"'</MergeFile>' $CINN_APPS_MENU_PATH
      else
( cat << EOF_CINN_APPS_MENU
<!DOCTYPE Menu PUBLIC '-//freedesktop//DTD Menu 1.0//EN' 'http://standards.freedesktop.org/menu-spec/menu-1.0.dtd'>
<Menu>
  <Name>Applications</Name>
  <MergeFile type="parent">/etc/xdg/menus/cinnamon-applications.menu</MergeFile>
  <MergeFile>$TUXCLIENT_MENU_PATH</MergeFile>
</Menu>
EOF_CINN_APPS_MENU
) >> $CINN_APPS_MENU_PATH
      fi
    fi
  done
fi

if [ -x "`which gsettings 2>/dev/null`" ] && [ -e "/usr/bin/unity" ]; then
  for USER in ${USERNAMES}; do
    local UNITY_LAUNCHER_CHECK=$(gsettings list-schemas | grep -i com.canonical.Unity.Launcher)
    if [ -n $UNITY_LAUNCHER_CHECK ]; then
      local currentlist=`su -l $USER -c "gsettings get com.canonical.Unity.Launcher favorites"`
      local OLD_DUMMY_FILE=$(echo $currentlist | grep -io "/tmp/software.*2xclient.*desktop")
      if [ ! -z $OLD_DUMMY_FILE ]; then
        cp /opt/2X/Client/share/2xclient.desktop $OLD_DUMMY_FILE
      fi
    fi
  done
fi
}

function install_protocol_handling()
{
local TUXCHECKSCRIPT="/opt/2X/Client/scripts/tuxclient-schema-check.sh"
if [ -e $TUXCHECKSCRIPT ]; then
  chmod 755 $TUXCHECKSCRIPT
fi

if [ -x "`which gnome-session 2>/dev/null`" ] && [ -x "`which gconftool-2 2>/dev/null`" ] && [ -d "/etc/gconf" ]; then
    echo "Registering tuxclient url-protocol handler in gconf tree"
    gconftool-2 --config-source=xml::/etc/gconf --recursive-unset /desktop/gnome/url-handlers/tuxclient
    gconftool-2 --config-source=xml::/etc/gconf -t string -s /desktop/gnome/url-handlers/tuxclient/command "${LINE_TO_ADD}/bin/appserverclient"
    gconftool-2 --config-source=xml::/etc/gconf -s /desktop/gnome/url-handlers/tuxclient/needs_terminal false -t bool
    gconftool-2 --config-source=xml::/etc/gconf -t bool -s /desktop/gnome/url-handlers/tuxclient/enabled true
fi

if [ -x "`which ksmserver 2>/dev/null`" ]; then   #else if ksmserver is present (for KDE environments this is a standard)
  for USER in ${USERNAMES}; do
    local USER_HOME=$(getent passwd $USER | cut -d: -f6)
    local KDE4_PATH=$USER_HOME/.kde4/share/kde4
    local KDE_PATH=$USER_HOME/.kde/share/
    local KDE_HANDLER_FILE=""
    if [ -e $KDE4_PATH ]; then
      KDE_HANDLER_FILE=$KDE4_PATH/services/tuxclient.protocol
    elif [ -e $KDE_PATH ]; then
      KDE_HANDLER_FILE=$KDE_PATH/services/tuxclient.protocol
    fi

    if [ ! -z $KDE_HANDLER_FILE ] && [ ! -e $KDE_HANDLER_FILE ] ; then
      echo "Registering tuxclient url-protocol handler in kde services folder"
( cat << EOF
[Protocol]
exec=${LINE_TO_ADD}/bin/appserverclient "%u"
protocol=tuxclient
input=none
output=none
helper=true
listing=
reading=false
writing=false
makedir=false
deleting=false
EOF
) >> $KDE_HANDLER_FILE
    fi
  done
fi

#Here below is the code for modifying browser specific protocols 
#Opera is in ~/.opera/handler.ini (this needs to be done for each user)
if [ -x "`which opera 2>/dev/null`" ]; then
  for USER in ${USERNAMES}; do
    local OPERA_HOME=$(getent passwd $USER | cut -d: -f6)/.opera
    if [ -e $OPERA_HOME ]; then
      echo "Registering tuxclient url-protocol handler in opera handlers.ini/filehandler.ini files"
      local PREFS_PATH=$OPERA_HOME/operaprefs.ini
      local HANDLER_PATH=$OPERA_HOME/handlers.ini
      local FILE_HANDLER_PATH=$OPERA_HOME/filehandler.ini
      if [ -e $HANDLER_PATH ]; then
        sed -i '/tuxclient/,/Flags/ d' $HANDLER_PATH
      else
( cat << EOF_OPERA_PREFS
Opera Preferences version 2.1
; Do not edit this file while Opera is running
; This file is stored in UTF-8 encoding

EOF_OPERA_PREFS
) >> $HANDLER_PATH
      fi

( cat << EOF_OPERA
[tuxclient]
Type=Protocol
Handler=${LINE_TO_ADD}/bin/appserverclient
Webhandler
Description
Flags=4 file
EOF_OPERA
) >> $HANDLER_PATH

      if [ ! -e $PREFS_PATH ] || [ -z $(cat $PREFS_PATH | sed -n '/Trusted Protocols Section Info/=') ]; then
( cat << EOF_OPERA_PREFS
[Trusted Protocols Section Info]
Version=1
EOF_OPERA_PREFS
) >> $PREFS_PATH
      fi

      if [ -x "`which gnome-session 2>/dev/null`" ]; then
( cat << EOF_FILE_HANDLER
Opera Preferences version 2.0
; Do not edit this file while Opera is running
; This file is stored in UTF-8 encoding
[Settings]
Default File Handler=gnome-open ,1
Default Directory Handler=gnome-open ,1
EOF_FILE_HANDLER
) > $FILE_HANDLER_PATH
      elif [ -x "`which ksmserver 2>/dev/null`" ]; then
( cat << EOF_FILE_HANDLER
Opera Preferences version 2.0
; Do not edit this file while Opera is running
; This file is stored in UTF-8 encoding
[Settings]
Default File Handler=kfmclient exec,1
Default Directory Handler=kfmclient exec,1
EOF_FILE_HANDLER
) > $FILE_HANDLER_PATH
      elif [ -x "`which xdg-open 2>/dev/null`" ]; then
( cat << EOF_FILE_HANDLER
Opera Preferences version 2.0
; Do not edit this file while Opera is running
; This file is stored in UTF-8 encoding
[Settings]
Default File Handler=xdg-open,1
Default Directory Handler=xdg-open,1
EOF_FILE_HANDLER
) > $FILE_HANDLER_PATH
      fi
    fi
  done
fi

#Google chrome is in ~/.config/google-chrome
if [ -x "`which google-chrome 2>/dev/null`" ]; then
  for USER in ${USERNAMES}; do
    local GOOGLE_CHROME_HOME=$(getent passwd $USER | cut -d: -f6)/.config/google-chrome
    if [ ! -e $GOOGLE_CHROME_HOME ]; then
      mkdir $GOOGLE_CHROME_HOME
    fi

    local HANDLER_PATH="$GOOGLE_CHROME_HOME/Local State" #inside quotes to preserve space in filename
    if [ -e $HANDLER_PATH ]; then 
      local GOOGLE_CHROME_PROTOCOL_PRESENT=$(cat $HANDLER_PATH | sed -n '/protocol_handler/N;/excluded_schemes/=')
      echo "Registering tuxclient url-protocol handler in google chrome Local State file"
      if [ -z $GOOGLE_CHROME_PROTOCOL_PRESENT ]; then
        sed -i '0,/{/ a\  "protocol_handler": {\n    "excluded_schemes": {\n      "tuxclient": false\n    },' $HANDLER_PATH
      else
        sed -i '/tuxclient/ d' $HANDLER_PATH #delete tuxclient line if it exists (to avoid duplication / ambiguity)
        sed -i '/protocol_handler/N;/excluded_schemes/ a\"tuxclient": false,' $HANDLER_PATH #adds '"tuxclient": false,' after match
      fi    
    else #Local State file not found
( cat << EOF_LOCAL_STATE_FILE
{
  "protocol_handler": {
    "excluded_schemes": {
      "tuxclient": false
    }
  }
}

function install_mimeinfo()
{
  local LINE_TO_ADD=$1
  # Shared mime info by freedesktop.org
  if [ -d /usr/share/mime/packages ] && [ -x "`which update-mime-database 2>/dev/null`" ]; then
    install_binary_links "/usr/share/mime/packages" "${LINE_TO_ADD}/share/sharedmimeinfo" "("2XClient.xml")"
    update-mime-database /usr/share/mime >/dev/null
    update-desktop-database
  fi
  
  # KDE 3.5 mime info
  if [ -d /usr/share/mimelnk/application ]; then
  install_binary_links "/usr/share/mimelnk/application" "${LINE_TO_ADD}/share/mime" "("x-2xa.desktop")"
  fi
  
  # /etc/mailcap
  if [ -d /usr/lib/mime/packages ] &&  [ -x "`which update-mime 2>/dev/null`" ]; then
    install_binary_links "/usr/lib/mime/packages" "${LINE_TO_ADD}/share/mime" "("2XClient")"
    update-mime
  fi

  # old mime info
  if [ -d /usr/share/application-registry ]; then 
    install_binary_links "/usr/share/application-registry" "${LINE_TO_ADD}/share/mime" "("2XSession.applications")"
  fi
  if [ -d /usr/share/mime-info ]; then
    install_binary_links "/usr/share/mime-info" "${LINE_TO_ADD}/share/mime" "("2XClient.keys" "2XClient.mime")"
  fi
}

function install_sso()
{
  local INST_DIR="$1"
  local INSTALL_SSO_CONF=$INST_DIR/bin/install_sso_conf
  local PAMMODULEPATH=$INST_DIR/share/pam_2xclient.so

  if [ -f /etc/pam.d/common-auth.2xclient ]
  then
    echo "File /etc/pam.d/common-auth.2xclient already exists. This package cannot be installed on altered system."
    return 1
  fi

  if [ ! -x "$INSTALL_SSO_CONF" ]
  then
    echo "Installed version of 2X Client does not support SingleSignOn component. Please upgrade first."
    return 1
  fi

  if [ ! -f "$PAMMODULEPATH" ]
  then
    echo "Installed version of 2X Client does not support SingleSignOn component. Please upgrade first."
    return 1
  fi

  cp $PAMMODULEPATH /lib/security/ || return 1

  pam_services="common-auth common-password"
  for service in $pam_services
  do
    if [ -f /etc/pam.d/$service ]
    then
       cp  /etc/pam.d/$service  /etc/pam.d/$service.2xclient || return 1
       $INSTALL_SSO_CONF /etc/pam.d/$service.2xclient > /etc/pam.d/$service || return 1
    fi
  done
}

##### MAIN #####
echo -n "Finalising settings .... "
LINE_TO_ADD="$( get_installation_dir "Client" "install.sh" )"
[ -z "${LINE_TO_ADD}" ] && echo "FAILED: There are more than one installation installed.  Please remove old installation." && exit 1
LIBEXPAT_DETAILS="$(which_libexpat_is_installed "("libexpat.so.0.5.0" "libexpat.so.1.5.0" "libexpat.so.1.5.2")" )"
LIBEXPAT_PATH="${LIBEXPAT_DETAILS%/*}/"
LIBEXPAT="${LIBEXPAT_DETAILS##*/}"
[ -z "${LIBEXPAT_DETAILS}" -o -z "${LIBEXPAT_PATH}" -o -z "${LIBEXPAT}" ] && echo "FAILED: Cannot determine libexpat library path." && exit 1
if [ ! -L "${LIBEXPAT_PATH}libexpat.so.1" ]
then
  install_links "${LIBEXPAT_PATH%lib/*}" "("libexpat.so.1")" "("${LIBEXPAT}")"
  [ $? -eq 1 ] && echo "FAILED: Cannot install necessary libexpat links." && exit 1
fi
install_binary_links "/usr/local/bin" "${LINE_TO_ADD}/bin" "("appserverclient" "2XClient")"
install_binary_links "/usr/share/pixmaps" "${LINE_TO_ADD}/share" "("2X.png")"
install_binary_links "/usr/share/applications" "${LINE_TO_ADD}/share" "("2xclient.desktop" "2XSession.desktop" "tuxclient.desktop")"
install_protocol_handling
install_app_menu
install_mimeinfo ${LINE_TO_ADD}
echo "Done"

echo
echo "Setup is going to install/update 2X SingleSignOn component."
echo "When this component is installed your credentials will be used automatically."
echo
echo -n "Do you want to install this component (yes|no)? [yes]: "
read option
if [ x"$option" = xyes -o x"$option" = x ]; then
   install_sso $LINE_TO_ADD
fi

exit 0

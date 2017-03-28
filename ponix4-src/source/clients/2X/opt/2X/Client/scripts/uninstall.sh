#!/bin/bash

# Post uninstall script for 2X Application Server Client for Linux
# Copyright 2006-2013 2X Software Ltd. All Rights Reserved

# Global variables
INSTALL_DIR=""
USERNAMES=$(cat /etc/passwd | grep /home | awk -F: '{if($3 >= 999) print $1}') #this method relies on user group being >= 999

function find_rm()
{
  find $1 -type f -name "$2" -exec rm -f {} \;
}

function remove_autostart()
{
  if [ -e $1 ] && [ -d $1 ]; then
    find_rm $1 "2x-app-*.sh.desktop"
    find_rm $1 "2x-app-*.sh"
  fi
}

function find_xdg_uninstall()
{
  if [ -x "`which xdg-desktop-menu 2>/dev/null`" ]; then
    local vartest=$(find $1 -type f -name "$3")
    xdg-desktop-menu uninstall --mode user $2 $vartest
  fi
}

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

function uninstall_binary_links()
{
  # Local variables
  local WHERE_TO_LINK="$1"
  declare -a BINS="$2"
  local C_DIR=$(pwd)

  # Go to the link destination directory
  cd "${WHERE_TO_LINK}"
  # Remove links
  for (( I=0; I<${#BINS[@]}; I++ ))
  do
    [ -L "${BINS[$I]}" ] && rm -f "${BINS[$I]}"
  done
  # Go back to the original directory
  cd "${C_DIR}"
  return 0
}

function remove_dirs()
{
  # Local variable
  declare -a DIR_LIST="$1"

  for (( I=0; I<${#DIR_LIST[@]}; I++ ))
  do
    [ -d "${DIR_LIST[$I]}" ] && rm -rf "${DIR_LIST[$I]}"
  done
  return 0
}

function uninstall_protocol_handling()
{
if [ -x "`which gnome-session 2>/dev/null`" ]; then
  if [ -x "`which gconftool-2 2>/dev/null`" ] && [ -d "/etc/gconf" ]; then
    echo "Unregistering tuxclient url-protocol handler in gconf tree"
    gconftool-2 --config-source=xml::/etc/gconf --recursive-unset /desktop/gnome/url-handlers/tuxclient
  fi
elif [ -x "`which ksmserver 2>/dev/null`" ]; then   #else if ksmserver is present (for KDE environments this is a standard)
  for USER in ${USERNAMES}; do
    local KDE4_PATH=$(getent passwd $USER | cut -d: -f6)/.kde4/share/kde4
    local KDE_PATH=$(getent passwd $USER | cut -d: -f6)/.kde/share/
    local KDE_HANDLER_FILE=""
    if [ -e $KDE4_PATH ]; then
      KDE_HANDLER_FILE=$KDE4_PATH/services/tuxclient.protocol
    elif [ -e $KDE_PATH ]; then
      KDE_HANDLER_FILE=$KDE_PATH/services/tuxclient.protocol
    fi 

    if [ ! -z $KDE_HANDLER_FILE ] && [ -e $KDE_HANDLER_FILE ] ; then
      echo "Unregistering tuxclient url-protocol handler in kde services folder"
      rm $KDE_HANDLER_FILE
    fi
  done
fi

#Here below is the code for modifying browser specific protocols 
#Opera is in ~/.opera/handler.ini (this needs to be done for each user)
if [ -x "`which opera 2>/dev/null`" ]; then
  for USER in ${USERNAMES}; do
    local OPERA_HOME=$(getent passwd $USER | cut -d: -f6)/.opera
    if [ -e $OPERA_HOME ] && [ -e $OPERA_HOME/handlers.ini ]; then
      echo "Unregistering tuxclient url-protocol handler in opera handlers.ini file"
      local HANDLER_PATH=$OPERA_HOME/handlers.ini
      sed -i '/tuxclient/,/Flags/ d' $HANDLER_PATH #remove from tuxclient to flags
    fi
  done
fi

#Google chrome is in ~/.config/google-chrome
if [ -x "`which google-chrome 2>/dev/null`" ]; then
  for USER in ${USERNAMES}; do
    local GOOGLE_CHROME_HOME=$(getent passwd $USER | cut -d: -f6)/.config/google-chrome
    if [ -e $GOOGLE_CHROME_HOME ] && [ -e "$GOOGLE_CHROME_HOME/Local State" ]; then #inside quotes to preserve space in filename
      echo "Unregistering tuxclient url-protocol handler in google chrome Local State file"
      local HANDLER_PATH="$GOOGLE_CHROME_HOME/Local State"
      sed -i '/tuxclient/ d' "$HANDLER_PATH" #delete tuxclient line if it exists
    fi
  done
fi

#Chromium is in ~/.config/chromium
if [ -x "`which chromium 2>/dev/null`" ]; then
  for USER in ${USERNAMES}; do
    local CHROMIUM_HOME=$(getent passwd $USER | cut -d: -f6)/.config/chromium
    if [ -e $CHROMIUM_HOME ] && [ -e "$CHROMIUM_HOME/Local State" ]; then #inside quotes to preserve space in filename
      echo "Unregistering tuxclient url-protocol handler in chromium Local State file"
      local HANDLER_PATH="$CHROMIUM_HOME/Local State"
      sed -i '/tuxclient/ d' "$HANDLER_PATH" #delete tuxclient line if it exists   
    fi
  done
fi
}

function remove_shortcuts()
{
  for USER in ${USERNAMES}; do
    local USER_HOME=$(getent passwd $USER | cut -d: -f6)

    #First go to desktop and iconpath folders and remove any found 2x files
    local DESKTOPDIRPATH=$USER_HOME/Desktop
    local DESKTOPPIXMAPSPATH=""
    if [ ! -z $XDG_DATA_HOME ]; then
      DESKTOPPIXMAPSPATH=$XDG_DATA_HOME
    else
      DESKTOPPIXMAPSPATH=$USER_HOME/.local/share
    fi

    if [ -e $DESKTOPDIRPATH ] && [ -d $DESKTOPDIRPATH ]; then
      find_rm $DESKTOPDIRPATH "2x-ts-*.desktop"
      find_rm $DESKTOPDIRPATH "2x-app-*.desktop"
    fi

    if [ -e $DESKTOPPIXMAPSPATH ] && [ -d $DESKTOPPIXMAPSPATH ]; then
      find_rm $DESKTOPPIXMAPSPATH "2x-ts-*icon.png"
      find_rm $DESKTOPPIXMAPSPATH "2x-app-*icon.png"
      find_rm $DESKTOPPIXMAPSPATH "2x-ts-*icon.xpm"
      find_rm $DESKTOPPIXMAPSPATH "2x-app-*icon.xpm"
    fi

    #Now go to autostart folder and remove any found 2X files
    if [ -x "`which ksmserver 2>/dev/null`" ]; then
      remove_autostart $USER_HOME/.kde4/Autostart
    fi

    if [ ! -z $XDG_CONFIG_HOME ]; then
      remove_autostart $XDG_CONFIG_HOME/autostart
    else
      remove_autostart $USER_HOME/.config/autostart
    fi

    if [ ! -z $DESKTOP_SESSION ]; then
      local SESSION_LOWER_STRING=$(echo $DESKTOP_SESSION | awk '{print tolower($DESKTOP_SESSION)}')
        if [ "$SESSION_LOWER_STRING" = "enlightenment" ]; then
          local ENLIGHT_STARTUP_ORDER="$USER_HOME/.e/e/applications/startup/.order"
          if [ -e $ENLIGHT_STARTUP_ORDER ]; then
            sed -i '/2x.*desktop/ d' "$ENLIGHT_STARTUP_ORDER" #remove links to startup files
          fi
        fi
    fi

    #Finally remove 2X Menu items
    local APPMENUSPATH=""
    if [ ! -z $XDG_DATA_HOME ]; then
      APPMENUSPATH=$XDG_DATA_HOME/2X/Client/menus
    else
      APPMENUSPATH=$USER_HOME/.local/share/2X/Client/menus
    fi

    if [ -e $APPMENUSPATH ]; then
      local APPDIRLIST=$(find $APPMENUSPATH -type d -name "*.applications" | awk -F/ '{print $NF}') #use / as delimiter and take last value with awk
      for APPDIR in ${APPDIRLIST}; do
  CURRENTFARM=$(echo $APPDIR | sed "s/[^0-9]//g")
  find_xdg_uninstall $APPMENUSPATH/$APPDIR "$APPMENUSPATH/farm-$CURRENTFARM.directory" "*desktop"
  find_rm $APPMENUSPATH/$APPDIR "*"
  rm -r $APPMENUSPATH/$APPDIR
      done
      find_rm $APPMENUSPATH "farm-*.menu"
      find_rm $APPMENUSPATH "farm-*.directory"
    fi

  #Remove cinnamon application link if present
  if [ -x "`which cinnamon 2>/dev/null`" ]; then
  local CONFIG_PATH="$USER_HOME/.config"
  local MENUS_PATH="$CONFIG_PATH/menus"
  local CINN_APPS_MENU_PATH="$MENUS_PATH/cinnamon-applications.menu"
  local TUXCLIENT_MENU_PATH="$MENUS_PATH/applications-merged/2XClient.menu"
  if [ -e $MENUS_PATH ] && [ -e $CINN_APPS_MENU_PATH ]; then
    sed -i "\:$TUXCLIENT_MENU_PATH: d" "$CINN_APPS_MENU_PATH" #remove 2XClient link from cinnamon-applications.menu
  fi
  fi
  done

  #Remove dummy unity links if they exist
  if [ -x "`which gsettings 2>/dev/null`" ] && [ -e "/usr/bin/unity" ]; then
    find_rm "/tmp" "software*2xclient*desktop"
  fi
}

function uninstall_mimeinfo()
{
  # Shared mime info by freedesktop.org
  if [ -d /usr/share/mime/packages ]  && [ -x "`which update-mime-database 2>/dev/null`" ]; then
    uninstall_binary_links "/usr/share/mime/packages" "("2XClient.xml")"
    update-mime-database /usr/share/mime >/dev/null
    update-desktop-database
  fi

  # KDE 3.5 mime info
  if [ -d /usr/share/mimelnk/application ]; then
    uninstall_binary_links "/usr/share/mimelnk/application" "${LINE_TO_ADD}/share/mime" "("x-2xa.desktop")"
  fi

  # /etc/mailcap
  if [ -d /usr/lib/mime/packages ] &&  [ -x "`which update-mime 2>/dev/null`" ]; then
    uninstall_binary_links "/usr/lib/mime/packages" "("2XClient")"
    update-mime
  fi

  # old mime info
  if [ -d /usr/share/application-registry ]; then
    uninstall_binary_links "/usr/share/application-registry" "("2XSession.applications")"
  fi

  if [ -d /usr/share/mime-info ]; then
    uninstall_binary_links "/usr/share/mime-info" "("2XClient.keys" "2XClient.mime")"
  fi
}

function uninstall_sso()
{
  list=`find /etc/pam.d/ -name '*.2xclient'`
  for f in $list; do
    orig=${f%*.2xclient}
    if [ -f "$orig" ]; then
      rm -f "$orig" || continue
      mv "$f" "$orig"
    fi
  done
}

INSTALL_DIR="$( get_installation_dir "Client" "install.sh" )"

if [ -n "$INSTALL_DIR" ]; then
  uninstall_binary_links "/usr/local/bin" "("appserverclient" "2XClient")"
  uninstall_binary_links "/usr/share/pixmaps" "("2X.png")"
  uninstall_binary_links "/usr/share/applications" "("2xclient.desktop" "2XSession.desktop" "tuxclient.desktop")"
  uninstall_protocol_handling
  remove_shortcuts
  uninstall_mimeinfo
  uninstall_sso
  remove_dirs "("${INSTALL_DIR}")"
fi
exit 0

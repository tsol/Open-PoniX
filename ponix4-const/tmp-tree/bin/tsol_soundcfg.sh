#!/bin/sh

. /etc/ponix.global

l_confpath=/var/
r_confpath=asound/
configname=`/bin/tsol_get_mixfname.sh`

# this part makes initial menu:

if [ -z "$RM_sound" ]; then

   if [ "$configname" = "" ]; then
	Xdialog --stdout --msgbox "No Sound Card detected!" 0 0
	exit
   fi

   rxvt -g 128x32 -fn fixed -title "Sound Card mixer configuration" -e "/bin/alsamixer"
   alsactl store
   
   exit
fi

# this part applies menu selection:

#if [ "$RM_sound" = "sndcfg_mac" ]; then
#	r_conffile=${r_confpath}${CLIENT_MAC}.conf
#elif [ "$RM_sound" = "sndcfg_all" ]; then
#	r_conffile=${r_confpath}$configname
#fi
#
#tftp -p -b 512 -r ${r_conffile} -l /var/lib/alsa/asound.state $SERVER_IP

# todo: download it back and compare ?
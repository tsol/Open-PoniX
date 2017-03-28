#!/bin/sh

. $TS_GLOBAL

ctlbin=/bin/alsactl
l_confpath=/var/
r_confpath=asound/

configname=`/bin/tsol_get_mixfname.sh`

if [ -z "$configname" ]; then
	echo "No soundcard detected."
	exit
fi

l_conffile=${l_confpath}$configname
r_conffile=${r_confpath}$configname


if [ "$NET_FILE_ENABLED" != "OFF" ]; then

	# loading card-based settings

	echo "Getting file '$r_conffile' from tftp server '$SERVER_IP'..."
	tftp -g -r ${r_conffile} -l ${l_conffile} $SERVER_IP
	if test -f ${l_conffile}; then
		echo "Base sound settings loaded ok!"
	else
		echo "Base sound settings not loaded."
	fi


	# trying MAC based settings

	r_conffile=${r_confpath}${CLIENT_MAC}.conf

	echo "Getting file '$r_conffile' from tftp server '$SERVER_IP'..."
	tftp -g -r ${r_conffile} -l /var/tmp_asound.conf $SERVER_IP
	if test -f /var/tmp_asound.conf; then
		echo "MAC-based sound settings loaded ok!"
		mv /var/tmp_asound.conf ${l_conffile}
	else
		echo "MAC-based sound settings not found."
	fi
else
	echo "Not trying to load sound config from TFTP (NET_FILE_ENABLED=OFF)"
fi

# load from local storage

if [ -e "$HOME/$configname" ]; then
	echo "Sound settings found on local storage!"
	cp $HOME/$configname ${l_conffile}
fi

${ctlbin} -f ${l_conffile} restore

#!/bin/sh
#
# Link this file: ln -s /etc/opt/elo-ser/setup/xEloInit.sh /etc/X11/xinit/xinitrc.d/xEloInit.sh
#

eloser_PID=`ps -C eloser -o pid=`

eloautocalib_PID=`ps -C eloautocalib -o pid=`
 
if [ -n "$eloautocalib_PID" ]; then
	echo 'Killing existing eloautocalib processes in memory'
	killall eloautocalib
fi

# Uncomment the '/etc/opt/elo-ser/eloautocalib' line entry below to load the calibration values from 
# the monitor NVRAM on system startup. Type "> /etc/opt/elo-ser/eloautocalib /?" at command prompt for 
# help and options on eloautocalib utility.

# eloautocalib - not active [Default] - Does not read and apply calibration values from NVRAM.   
# eloautocalib - active               - Read calibration values from NVRAM and apply automatically. 

# Only start'eloautocalib' if 'eloser' is running.
if [ -n "$eloser_PID" ]; then
 # echo 'Loading Elo touch screen calibration data'
 # /etc/opt/elo-ser/eloautocalib --renew
fi

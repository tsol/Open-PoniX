#!/bin/sh
#
# Default script for e16 session start/restart/stop management
#
# Assuming misc.session.enable_script is set and misc.session.script
# points to this script it will be called with parameter
#
# - "init"  first time an X-session (with e16) starts
# - "start" every time e16 (re)starts
# - "stop"  when e16 exits without restarting
#
# On init, start, and stop the script will run any executable found in
# ~/.e16/Init/, ~/.e16/Start/, and ~/.e16/Stop/, respectively.
# These executables do not have to exit as they are called with '&' from here.
#
# NOTE:
# In multi-display/screen setups the DISPLAY environment variable can be used
# to differentiate.
#

#echo $DISPLAY

RunApps() {
	local d;

	d="$ECONFDIR/$1"
	test -d "$d" || return

	for f in "$d"/*
	do
		if [ -x "$f" ]; then
#			echo $f
			case "$f" in
			*~)	# Assume this is crap - skip
				;;
			*.sh)	# Scripts are executed in foreground
				"$f"
				;;
			*)	# Anything else is executed in background
				"$f"  &
				;;
			esac
		fi
	done
}


case "$1" in
init)
	RunApps Init
	;;
start)
	RunApps Start
	;;
stop)
	RunApps Stop
	;;
esac

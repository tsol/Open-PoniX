#!/bin/sh
# A sample skeleton for handling ACPI events.
# See acpi_handler-conf for a configuration file that can be used to
# run this script.

if [ $# != 1 ]; then
	exit 1
fi
set $*

case "$1" in
	button)
		case "$2" in
			power)	/sbin/init 0
				;;
			*)	logger "ACPI action $2 is not defined"
				;;
		esac
		;;

	*)
		logger "ACPI group $1 is not defined"
		;;
esac

#!/bin/bash

#
# Print and set $WLAN to the name of the
# network interface for the 'wil6210' driver
#

DRV="wil6210"

for f in /sys/class/net/*; do {
	drv=`readlink $f/device/driver`;
	drv=${drv##.*/}
	if [[ $drv == $DRV ]]; then {
		ifc=${f#/sys/class/net/}
		echo $ifc
		export WLAN=$ifc
		break
	} ; fi
} ; done


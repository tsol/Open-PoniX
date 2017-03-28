#!/bin/sh

. /etc/ponix.global

cardname=`cat /proc/asound/cards | \
		tr [:upper:] [:lower:] | \
		sed -nr 's/(\s*0\s+\[.+?\]:\s*)(.+)/\2/p' | \
		sed -r 's/[^a-z0-9]+/_/g'`

if [ -z "$cardname" ]; then
	exit
fi

echo ${cardname}.conf

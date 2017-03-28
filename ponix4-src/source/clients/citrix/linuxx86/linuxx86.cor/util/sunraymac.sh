#!/bin/sh
#
# Name:		SUNRaymac.sh
#
# Version:	$Id: //icaclient/unix13.0/client/unix/sunray/sunraymac.sh#1 $
#
# Purpose: 	SCRIpt to get a SunRay's MAC address. This works even
# 		with a mobile session.
#
#		See http://www.lewiz.org/archive/2007/06/15/getting-a-sun-ray-dtu-mac-address.
#
exec /opt/SUNWut/bin/utwho -c 2>/dev/null | nawk '$3 == ENVIRON["LOGNAME"] {print substr($5,4)}'

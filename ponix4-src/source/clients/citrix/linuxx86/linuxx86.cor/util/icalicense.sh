#!/bin/sh
###############################################################################
#
#       Name:           icalicense.sh
#
#       Created:        26 May 2000
#
#       Version:        $Id: //icaclient/unix13.0/client/unix/installer/scripts/icalicense.sh#1 $
#
#       Purpose:        Create /etc/icalicense/clientlicense for RPM install.
#
#       Copyright 2000-2011 Citrix Systems, Inc. All rights reserved.
#
###############################################################################

if test ! -d /etc/icalicense
then
    mkdir /etc/icalicense
fi
if test ! -f /etc/icalicense/clientlicense
then
    "$ICAROOT/util/echo_cmd" -l >/etc/icalicense/clientlicense
fi
chmod 444 /etc/icalicense/clientlicense
chmod 555 /etc/icalicense

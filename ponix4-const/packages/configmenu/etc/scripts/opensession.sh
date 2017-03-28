#! /bin/sh

if [ -n "`ps | grep -v grep | grep Xorg`" ]; then
       chvt 3
else
       replimenu -c 4 -f /etc/replimenu/nosession.menu
fi
                
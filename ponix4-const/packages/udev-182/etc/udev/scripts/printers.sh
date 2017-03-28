#!/bin/sh

dev=`basename $DEVNAME`

case $1 in
hplj1000) cat /lib/firmware/sihp1000.dl > /dev/usb/$dev ;;
hplj1005) cat /lib/firmware/sihp1005.dl > /dev/usb/$dev ;;
hplj1018) cat /lib/firmware/sihp1018.dl > /dev/usb/$dev ;;
hplj1020) cat /lib/firmware/sihp1020.dl > /dev/usb/$dev ;;
hpljP1005) cat /lib/firmware/sihpP1005.dl > /dev/usb/$dev ;;
hpljP1006) cat /lib/firmware/sihpP1006.dl > /dev/usb/$dev ;;
hpljP1505) cat /lib/firmware/sihpP1505.dl > /dev/usb/$dev ;;
esac

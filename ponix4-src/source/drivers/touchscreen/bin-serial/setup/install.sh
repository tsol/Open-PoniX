#!/bin/sh

if [ -f /etc/opt/elo-ser/setup/elok_s.o ]; then
 echo "Kernel base 2.4, install module elok_s.o"
 install -m 644 /etc/opt/elo-ser/setup/elok_s.o /lib/modules/`uname -r`/kernel/drivers/elok_s.o
else
 if [ -f /etc/opt/elo-ser/setup/elok_s.ko ]; then
  echo "Kernel base 2.6, install module elok_s.ko"
  install -m 644 /etc/opt/elo-ser/setup/elok_s.ko /lib/modules/`uname -r`/kernel/drivers/elok_s.ko
 else
  echo "Module missing elok_s, see readme.txt for building and copying module."
 fi
fi
/sbin/depmod -q

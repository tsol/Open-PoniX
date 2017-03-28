#!/bin/sh

make clean

./configure --prefix=/ --x-includes=/usr/xorg/include --x-libraries=/usr/xorg/lib --with-sound=alsa \
--with-ipv6 --disable-smartcard

make ; make install
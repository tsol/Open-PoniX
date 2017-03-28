#!/bin/sh

make clean

./configure --prefix=/ --x-includes=/usr/xorg/include --x-libraries=/usr/xorg/lib --enable-smartcard

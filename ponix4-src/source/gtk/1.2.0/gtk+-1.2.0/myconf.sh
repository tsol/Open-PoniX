#!/bin/sh

./configure --prefix=/usr --with-x --x-includes=/usr/xorg/include/ \
--x-libraries=/usr/xorg/lib --disable-glibtest --enable-debug=no \
--enable-static=yes --enable-shared=yes

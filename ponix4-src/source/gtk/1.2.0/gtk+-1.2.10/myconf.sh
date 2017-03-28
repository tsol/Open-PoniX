#!/bin/sh

./configure --prefix=/6 --with-x --x-includes=/usr/xorg/include/ \
--x-libraries=/usr/xorg/lib --disable-glibtest --enable-debug=yes \
--enable-static=yes --enable-shared=yes

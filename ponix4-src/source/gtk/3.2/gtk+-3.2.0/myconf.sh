#!/bin/sh

./configure --prefix=/usr --with-x --x-includes=/usr/xorg/include/ \
--x-libraries=/usr/xorg/lib --disable-glibtest --enable-debug=no \
--enable-static=no --enable-shared=yes --disable-shadowfb --enable-man=no \
--with-gdktarget=x11 --disable-gtk-doc  --disable-cups  --disable-papi \
--disable-test-print-backend --disable-gtk-doc-html --disable-gtk-doc-pdf
#--disable-modules 

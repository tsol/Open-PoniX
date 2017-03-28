#!/bin/sh

./configure --prefix=/usr --with-x --x-includes=/usr/xorg/include/ \
--x-libraries=/usr/xorg/lib --disable-glibtest --enable-debug=no \
--enable-static=yes --enable-shared=no --disable-shadowfb --enable-man=no \
--enable-gtk-doc=no --with-included-loaders=xpm,png \
--without-libjpeg --without-libtiff --disable-modules \

--disable-largefile --enable-shm --enable-fbmanager --with-xinput=no


#--with-included-modules --with-gdktarget=x11

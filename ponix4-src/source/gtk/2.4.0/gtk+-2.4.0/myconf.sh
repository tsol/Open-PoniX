#!/bin/sh

./configure --prefix=/source/Xdialog-2.3.1 --with-x --x-includes=/usr/xorg/include/ \
--x-libraries=/usr/xorg/lib --disable-glibtest --enable-debug=no \
--enable-static=yes --enable-shared=no --disable-shadowfb --enable-man=no \
--enable-gtk-doc=no --with-included-loaders=xpm,png \
--without-libjpeg --without-libtiff --disable-modules --enable-fbmanager=no \
CFLAGS="${CFLAGS} -I/source/Xdialog-2.3.1/include" \
LDFLAGS="${LDFLAGS} -L/source/Xdialog-2.3.1/lib" \
PKG_CONFIG_PATH="/source/Xdialog-2.3.1/lib/pkgconfig:${PKG_CONFIG_PATH}"

export PATH="/source/Xdialog-2.3.1/bin:${PATH}"



#--with-included-modules --with-gdktarget=x11

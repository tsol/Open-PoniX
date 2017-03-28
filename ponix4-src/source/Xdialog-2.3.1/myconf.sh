#!/bin/sh

./configure --prefix=/ --disable-gtktest --with-print-option="" --with-gtk2 \
CFLAGS=" -Os -I/source/Xdialog-2.3.1/include/gtk-2.0 -I/source/Xdialog-2.3.1/include/glib-2.0 -I/source/Xdialog-2.3.1/include/pango-1.0 -I/source/Xdialog-2.3.1/include/atk-1.0 -I/source/Xdialog-2.3.1/lib/glib-2.0/include -I/source/Xdialog-2.3.1/lib/gtk-2.0/include" \
PKG_CONFIG_PATH="/source/Xdialog-2.3.1/lib/pkgconfig" \
LDFLAGS="-L/source/Xdialog-2.3.1/lib"
#LDFLAGS="-lexpat"
#--with-gtk2=no CFLAGS="-static" 
#--disable-nls --disable-rpath --disable-gtktest

cp ./Makefile.my src/Makefile
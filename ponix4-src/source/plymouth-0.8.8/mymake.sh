#!/bin/sh

export ACLOCAL="aclocal -I/usr/xorg/share/aclocal" \
export PKG_CONFIG_PATH=/usr/xorg/lib/pkgconfig:/usr/xorg/share/pkgconfig:${PKG_CONFIG_PATH} \
export LD_LIBRARY_PATH=/usr/xorg/lib:${LD_LIBRARY_PATH} \
export LDFLAGS=-static-libgcc \
export CFLAGS=-Os \
export CXXFLAGS=-Os


make clean
./configure --prefix=/ --disable-tests --disable-pango --disable-tracing \
--disable-gtk --with-boot-tty=tty62 \
--enable-tracing --enable-libdrm_intel --enable-libdrm_radeon --enable-libkms

#--disable-pango --disable-tracing --disable-tests --with-logo=/bin/sst 

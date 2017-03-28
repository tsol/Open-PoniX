./configure --prefix=/usr --x-includes=/usr/xorg/include --x-libraries=/usr/xorg/lib --enable-xft \
--disable-png --disable-jpeg --disable-confirm --disable-shape --disable-debug --disable-xinerama \
CFLAGS="${CFLAGS} -I/source/wm/jwm-835/include" \
LDFLAGS="${LDFLAGS} -L/source/wm/jwm-835/lib" \
PKG_CONFIG_PATH=/source/wm/jwm-835/lib/pkgconfig:${PKG_CONFIG_PATH}
cp config.h.my config.h
cp src/Makefile.my src/Makefile

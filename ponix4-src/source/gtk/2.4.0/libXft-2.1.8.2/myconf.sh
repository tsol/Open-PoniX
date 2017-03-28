./configure --prefix=/source/Xdialog-2.3.1  --x-includes=/usr/xorg/include \
--x-libraries=/usr/xorg/lib --enable-static --enable-shared=no \
CFLAGS="${CFLAGS} -I/source/Xdialog-2.3.1/include" \
LDFLAGS="${LDFLAGS} -L/source/Xdialog-2.3.1/lib" \
PKG_CONFIG_PATH="/source/Xdialog-2.3.1/lib/pkgconfig:${PKG_CONFIG_PATH}"

export PATH="/source/Xdialog-2.3.1/bin:${PATH}"

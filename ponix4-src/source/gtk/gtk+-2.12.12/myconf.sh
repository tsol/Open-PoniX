#!/bin/sh

./configure --prefix=/usr --x-includes=/usr/xorg/include --x-libraries=/usr/xorg/lib --disable-glibtest \
--disable-modules --disable-cups --disable-test-print-backend --disable-gtk-doc --disable-man \
--with-gdktarget=x11 --with-x

#X features:
#  --x-includes=DIR    X include files are in DIR
#  --x-libraries=DIR   X library files are in DIR
#  --enable-shm            support shared memory if available [default=yes]
#  --enable-xkb            support XKB [default=maybe]
#  --enable-xinerama       support xinerama extension if available
#                          [default=yes]
#  --disable-rebuilds      disable all source autogeneration rules
#  --disable-visibility    don't use ELF visibility attributes
#  --enable-explicit-deps=[yes/no/auto]
#                          use explicit dependencies in .pc files
#                          [default=auto]
#  --disable-glibtest      do not try to compile and run a test GLIB program
#  --disable-modules       disable dynamic module loading
#  --disable-cups
#                              disable cups print backend
#
#  --enable-test-print-backend
#                          build test print backend
#  --enable-gtk-doc        use gtk-doc to build documentation [default=no]
#  --enable-man            regenerate man pages from Docbook [default=no]
#
#Optional Packages:
#  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
#  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
#  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]
#  --with-pic              try to use only PIC/non-PIC objects [default=use
#                          both]
#  --with-tags[=TAGS]      include additional configurations [automatic]
#  --with-xinput=[no/yes]  support XInput
#  --with-gdktarget=[x11/win32/quartz/directfb] select non-default GDK target
#  --without-libpng        disable PNG loader for gdk-pixbuf
#  --without-libjpeg       disable JPEG loader for gdk-pixbuf
#  --without-libtiff       disable TIFF loader for gdk-pixbuf
#  --with-included-loaders=LOADER1,LOADER2,...
#                          build the specified loaders into gdk-pixbuf
#  --with-x                use the X Window System
#  --with-html-dir=PATH    path to installed docs
#  --with-xml-catalog=CATALOG
#                          path to xml catalog to use

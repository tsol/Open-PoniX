#!/bin/sh

./configure --prefix=/usr --x-includes=/usr/xorg/include --x-libraries=/usr/xorg/lib --with-x \
--enable-gtk-doc=no --enable-man=no 


#X features:
#  --x-includes=DIR    X include files are in DIR
#  --x-libraries=DIR   X library files are in DIR
#  --disable-rebuilds      disable all source autogeneration rules
#  --enable-gtk-doc        use gtk-doc to build documentation [default=no]
#  --enable-man            regenerate man pages from Docbook [default=no]
#  --disable-doc-cross-references
#                          cross reference glib and cairo symbols [default=yes]
#  --enable-explicit-deps=[no/auto/yes]
#                          use explicit dependencies in .pc files
#                          [default=auto]
#
#Optional Packages:
#  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
#  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
#  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]
#  --with-pic              try to use only PIC/non-PIC objects [default=use
#                          both]
#  --with-tags[=TAGS]      include additional configurations [automatic]
#  --with-x                use the X Window System
#  --with-included-modules=no/yes/MODULE1 MODULE2 ...
#                          build the given modules included [default=no]
#  --with-dynamic-modules=no/yes/MODULE1 MODULE2 ...
#                          build the given dynamic modules [default=yes]
#  --with-html-dir=PATH    path to installed docs
#  --with-xml-catalog=CATALOG
#                          path to xml catalog to use
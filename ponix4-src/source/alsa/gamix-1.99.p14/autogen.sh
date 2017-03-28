#! /bin/sh

if [ ! -f gettext.h ]; then
    cp /usr/share/gettext/gettext.h .
fi
gettextize -f --intl -c
libtoolize -f -c
aclocal-1.6 -I m4
autoheader2.50
autoconf2.50
automake-1.6  -a --foreign -c --gnu

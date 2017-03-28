#! /bin/sh

if [  "$PWD" != "/home/work/gamix" ]; then
  ADDF="-c"
else
  ADDF=""
fi

gettextize -f $ADDF
libtoolize -f $ADDF
aclocal
autoheader2.50
autoconf2.50
automake  -a --foreign

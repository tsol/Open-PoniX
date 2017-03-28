#!/bin/sh

# Build zipfiles for Pango on Win32: separate runtime and developer packages

ZIP=/tmp/pango-1.28.0.zip
DEVZIP=/tmp/pango-dev-1.28.0.zip
cd /usr

mkdir -p share/doc/pango-1.28.0
cp -p /source/gtk/2.24/pango-1.28.0/COPYING share/doc/pango-1.28.0

mkdir -p share/doc/pango-dev-1.28.0
cp -p /source/gtk/2.24/pango-1.28.0/COPYING share/doc/pango-dev-1.28.0

rm $ZIP
zip $ZIP -@ <<EOF
bin/pango-querymodules.exe
etc/pango/pango.modules
bin/libpango-1.0-0.dll
bin/libpangoft2-1.0-0.dll
bin/libpangowin32-1.0-0.dll
bin/libpangocairo-1.0-0.dll
EOF

zip $ZIP lib/pango/1.6.0/modules/*.dll

zip -r $ZIP share/doc/pango-1.28.0

rm $DEVZIP
zip -r $DEVZIP -@ <<EOF
include/pango-1.0
lib/libpango-1.0.dll.a
lib/pango-1.0.lib
lib/pango-1.0.def
lib/libpangoft2-1.0.dll.a
lib/pangoft2-1.0.lib
lib/pangoft2-1.0.def
lib/libpangowin32-1.0.dll.a
lib/pangowin32-1.0.lib
lib/pangowin32-1.0.def
lib/libpangocairo-1.0.dll.a
lib/pangocairo-1.0.lib
lib/pangocairo-1.0.def
lib/pkgconfig/pango.pc
lib/pkgconfig/pangoft2.pc
lib/pkgconfig/pangowin32.pc
lib/pkgconfig/pangocairo.pc
share/gtk-doc/html/pango
EOF

zip -r $DEVZIP share/doc/pango-dev-1.28.0

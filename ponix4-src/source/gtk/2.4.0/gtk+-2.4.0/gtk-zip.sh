#!/bin/sh

# Build zipfiles for GTK on Win32

ZIP=/tmp/gtk+-2.4.0-`date +%Y%m%d`.zip
DEVZIP=/tmp/gtk+-dev-2.4.0-`date +%Y%m%d`.zip
cd /source/Xdialog-2.3.1
rm $ZIP

DLLDIR=lib
[ -f bin/libgdk_pixbuf-2.0-0.dll ] && DLLDIR=bin

zip $ZIP -@ <<EOF
COPYING.LIB-2
etc/gtk-2.0/gdk-pixbuf.loaders
etc/gtk-2.0/gtkrc
etc/gtk-2.0/gtk.immodues
$DLLDIR/libgdk_pixbuf-2.0-0.dll
$DLLDIR/libgdk-win32-2.0-0.dll
$DLLDIR/libgtk-win32-2.0-0.dll
EOF

zip $ZIP lib/gtk-2.0/2.4.0/loaders/*.dll lib/gtk-2.0/2.4.0/immodules/*.dll

zip $ZIP share/themes/{Default,Emacs}/gtk-2.0{,-key}/gtkrc

zip -r $ZIP lib/locale/*/LC_MESSAGES/gtk20.mo

rm $DEVZIP
zip -r $DEVZIP -@ <<EOF
include/gtk-2.0
bin/gdk-pixbuf-csource.exe
bin/gdk-pixbuf-query-loaders.exe
bin/gtk-query-immodules-2.0.exe
bin/gtk-demo.exe
man/man1/gdk-pixbuf-csource.1
lib/libgdk_pixbuf-2.0.dll.a
lib/gdk_pixbuf-2.0.lib
lib/libgdk-win32-2.0.dll.a
lib/gdk-win32-2.0.lib
lib/libgtk-win32-2.0.dll.a
lib/gtk-win32-2.0.lib
lib/gtk-2.0/include
lib/pkgconfig/gdk-pixbuf-2.0.pc
lib/pkgconfig/gdk-2.0.pc
lib/pkgconfig/gdk-win32-2.0.pc
lib/pkgconfig/gtk+-2.0.pc
lib/pkgconfig/gtk+-win32-2.0.pc
share/aclocal/gtk-2.0.m4
share/gtk-2.0
share/gtk-doc/html/gdk-pixbuf
share/gtk-doc/html/gdk
share/gtk-doc/html/gtk
EOF


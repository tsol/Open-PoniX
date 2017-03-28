#!/bin/sh

# Build zipfiles for ATK on Win32

ZIP=/tmp/atk-1.0.1-`date +%Y%m%d`.zip
DEVZIP=/tmp/atk-dev-1.0.1-`date +%Y%m%d`.zip
cd /source/Xdialog-2.3.1

rm $ZIP
zip -r $ZIP -@ <<EOF
lib/libatk-1.0-0.dll
EOF

rm $DEVZIP
zip -r $DEVZIP -@ <<EOF
include/atk-1.0
lib/libatk-1.0.dll.a
lib/atk-1.0.lib
lib/pkgconfig/atk.pc
EOF

#!/bin/sh

rm -rf /xdmenu_dist
cd /
mkdir xdmenu_dist
mkdir xdmenu_dist/etc
mkdir xdmenu_dist/etc/init.d
mkdir xdmenu_dist/etc/cmd
mkdir xdmenu_dist/etc/xdmenu
mkdir xdmenu_dist/bin
mkdir xdmenu_dist/etc/replimenu
mkdir xdmenu_dist/share
mkdir xdmenu_dist/share/lang

#mkdir xdmenu_dist/lib
#mkdir xdmenu_dist/lib/freerdp

cp -R /etc/xdmenu/* /xdmenu_dist/etc/xdmenu/
cp /bin/xdmenu* /xdmenu_dist/bin/
cp /bin/store_variable /xdmenu_dist/bin/
#cp /etc/cmd/freerdp.* /xdmenu_dist/etc/cmd/
cp /etc/cmd/deskrun /xdmenu_dist/etc/cmd/
cp /etc/init.d/autostart /xdmenu_dist/etc/init.d/
cp /etc/init.d/wireless_setup /xdmenu_dist/etc/init.d/
cp /bin/start-session /xdmenu_dist/bin/
cp /etc/init.d/network* /xdmenu_dist/etc/init.d/
cp /etc/ponix.functions /xdmenu_dist/etc/
#cp /bin/loger /xdmenu_dist/bin/
cp /bin/replimenu /xdmenu_dist/bin/
cp /etc/replimenu/* /xdmenu_dist/etc/replimenu/
cp /bin/repli_* /xdmenu_dist/bin/
#cp /lib/freerdp/rdpsnd.so.* /xdmenu_dist/lib/freerdp/
cp /bin/safe_jwm /xdmenu_dist/bin/
cp /bin/pkg /xdmenu_dist/bin/
cp /bin/replacetpl /xdmenu_dist/bin/
cp /etc/system.jwmrc.tpl /xdmenu_dist/etc/
#cp /share/lang/base.lang /xdmenu_dist/share/lang/
#cp /share/lang/config.lang /xdmenu_dist/share/lang/
cp /share/lang/* /xdmenu_dist/share/lang/
cp /bin/upadd /xdmenu_dist/bin/
cp /etc/init.d/x /xdmenu_dist/etc/init.d/
cp /bin/tsol_initsound.sh /xdmenu_dist/bin/
#cp /etc/init.d/loger /xdmenu_dist/etc/init.d/
cp /etc/gtkrc.fixedfont /xdmenu_dist/etc/

tar -cf /xdmenu_dist.tar xdmenu_dist/
tftp -p -l /xdmenu_dist.tar -r xdmenu_dist.tar 192.168.4.23



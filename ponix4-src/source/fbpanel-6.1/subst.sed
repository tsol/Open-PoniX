s/%%build%%//g
s/%%host%%//g
s/%%target%%//g
s/%%prefix%%/\/usr/g
s/%%eprefix%%/\/usr/g
s/%%bindir%%/\/usr\/bin/g
s/%%sbindir%%/\/usr\/sbin/g
s/%%libexecdir%%/\/usr\/libexec/g
s/%%libdir%%/\/usr\/lib/g
s/%%sysconfdir%%/\/usr\/etc/g
s/%%datadir%%/\/usr\/share/g
s/%%includedir%%/\/usr\/include/g
s/%%mandir%%/\/usr\/man/g
s/%%infodir%%/\/usr\/info/g
s/%%localstatedir%%/\/usr\/var/g
s/%%topdir%%/\/build3\/source\/fbpanel-6.1/g
s/%%version%%/6.1/g
s/%%glib_cflags%%/-I\/usr\/include\/glib-2.0 -I\/usr\/lib\/glib-2.0\/include/g
s/%%gtk_cflags%%/-pthread -D_REENTRANT -I\/usr\/include\/gtk-2.0 -I\/usr\/lib\/gtk-2.0\/include -I\/usr\/include\/atk-1.0 -I\/usr\/include\/cairo -I\/usr\/include\/pango-1.0 -I\/usr\/include\/gio-unix-2.0\/ -I\/usr\/include\/glib-2.0 -I\/usr\/lib\/glib-2.0\/include -I\/usr\/include\/pixman-1 -I\/usr\/include\/freetype2 -I\/usr\/include\/directfb -I\/usr\/include\/libpng12/g
s/%%glib_libs%%/-lglib-2.0/g
s/%%gtk_libs%%/-pthread -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0/g
s/%%cflagsx%%/-I\/build3\/source\/fbpanel-6.1\/panel -I\/usr\/include\/glib-2.0 -I\/usr\/lib\/glib-2.0\/include -pthread -D_REENTRANT -I\/usr\/include\/gtk-2.0 -I\/usr\/lib\/gtk-2.0\/include -I\/usr\/include\/atk-1.0 -I\/usr\/include\/cairo -I\/usr\/include\/pango-1.0 -I\/usr\/include\/gio-unix-2.0\/ -I\/usr\/include\/glib-2.0 -I\/usr\/lib\/glib-2.0\/include -I\/usr\/include\/pixman-1 -I\/usr\/include\/freetype2 -I\/usr\/include\/directfb -I\/usr\/include\/libpng12 -fPIC/g
s/%%ldflagsx%%/-lglib-2.0 -pthread -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0/g
s/%%gmodule_libs%%/-Wl,--export-dynamic -pthread -lgmodule-2.0 -lrt -lglib-2.0/g
s/%%debug%%/disabled/g
s/%%dependency%%/disabled/g
s/%%static_build%%/disabled/g


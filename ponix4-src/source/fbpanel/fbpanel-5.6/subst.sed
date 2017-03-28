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
s/%%topdir%%/\/source\/fbpanel\/fbpanel-5.6/g
s/%%version%%/5.6/g
s/%%endianess%%/LITTLE/g
s/%%os%%/LINUX/g
s/%%glib_cflags%%/-I\/usr\/include\/glib-2.0 -I\/usr\/lib\/glib-2.0\/include/g
s/%%gtk_cflags%%/-I\/usr\/include\/gtk-2.0 -I\/usr\/lib\/gtk-2.0\/include -I\/usr\/include\/atk-1.0 -I\/usr\/include\/pango-1.0 -I\/usr\/xorg\/include -I\/usr\/include\/freetype2 -I\/usr\/include\/glib-2.0 -I\/usr\/lib\/glib-2.0\/include/g
s/%%glib_libs%%/-lglib-2.0/g
s/%%gtk_libs%%/-Wl,--export-dynamic -L\/usr\/xorg\/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -lXinerama -lXext -lXcursor -latk-1.0 -lgdk_pixbuf-2.0 -lpangoxft-1.0 -lXft -lXrender -lfontconfig -lfreetype -lz -lpangox-1.0 -lX11 -lpango-1.0 -lm -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0/g
s/%%cflagsx%%/-I\/source\/fbpanel\/fbpanel-5.6\/panel -I\/usr\/include\/glib-2.0 -I\/usr\/lib\/glib-2.0\/include -I\/usr\/include\/gtk-2.0 -I\/usr\/lib\/gtk-2.0\/include -I\/usr\/include\/atk-1.0 -I\/usr\/include\/pango-1.0 -I\/usr\/xorg\/include -I\/usr\/include\/freetype2 -I\/usr\/include\/glib-2.0 -I\/usr\/lib\/glib-2.0\/include -fPIC/g
s/%%ldflagsx%%/-lglib-2.0 -Wl,--export-dynamic -L\/usr\/xorg\/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -lXinerama -lXext -lXcursor -latk-1.0 -lgdk_pixbuf-2.0 -lpangoxft-1.0 -lXft -lXrender -lfontconfig -lfreetype -lz -lpangox-1.0 -lX11 -lpango-1.0 -lm -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0/g
s/%%gmodule_libs%%/-Wl,--export-dynamic -lgmodule-2.0 -ldl -lglib-2.0/g
s/%%debug%%/disabled/g
s/%%dependency%%/enabled/g
s/%%static_build%%/disabled/g


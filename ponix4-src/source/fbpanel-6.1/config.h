/* Created by configure script ran as
** ./configure --prefix=/usr --disable-debug
*/

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Features */
#undef DEBUG
#undef DEPENDENCY
#undef STATIC_BUILD

/* Variables */
#define  BUILD  ""
#define  HOST  ""
#define  TARGET  ""
#define  PREFIX  "/usr"
#define  EPREFIX  "/usr"
#define  BINDIR  "/usr/bin"
#define  SBINDIR  "/usr/sbin"
#define  LIBEXECDIR  "/usr/libexec"
#define  LIBDIR  "/usr/lib"
#define  SYSCONFDIR  "/usr/etc"
#define  DATADIR  "/usr/share"
#define  INCLUDEDIR  "/usr/include"
#define  MANDIR  "/usr/man"
#define  INFODIR  "/usr/info"
#define  LOCALSTATEDIR  "/usr/var"
#define  TOPDIR  "/build3/source/fbpanel-6.1"
#define  VERSION  "6.1"
#define  GLIB_CFLAGS  "-I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  "
#define  GTK_CFLAGS  "-pthread -D_REENTRANT -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/directfb -I/usr/include/libpng12  "
#define  GLIB_LIBS  "-lglib-2.0  "
#define  GTK_LIBS  "-pthread -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0  "
#define  CFLAGSX  "-I/build3/source/fbpanel-6.1/panel -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include   -pthread -D_REENTRANT -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/directfb -I/usr/include/libpng12   -fPIC"
#define  LDFLAGSX  "-lglib-2.0   -pthread -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0  "
#define  GMODULE_LIBS  "-Wl,--export-dynamic -pthread -lgmodule-2.0 -lrt -lglib-2.0  "

#endif

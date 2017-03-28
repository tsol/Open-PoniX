/* Created by configure script ran as
** ./configure 
*/

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Features */
#undef DEBUG
#define DEPENDENCY
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
#define  TOPDIR  "/source/fbpanel/fbpanel-5.6"
#define  VERSION  "5.6"
#define  ENDIANESS  "LITTLE"
#define  OS  "LINUX"
#define  GLIB_CFLAGS  "-I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  "
#define  GTK_CFLAGS  "-I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/pango-1.0 -I/usr/xorg/include -I/usr/include/freetype2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  "
#define  GLIB_LIBS  "-lglib-2.0  "
#define  GTK_LIBS  "-Wl,--export-dynamic -L/usr/xorg/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -lXinerama -lXext -lXcursor -latk-1.0 -lgdk_pixbuf-2.0 -lpangoxft-1.0 -lXft -lXrender -lfontconfig -lfreetype -lz -lpangox-1.0 -lX11 -lpango-1.0 -lm -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0  "
#define  CFLAGSX  "-I/source/fbpanel/fbpanel-5.6/panel -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include   -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/pango-1.0 -I/usr/xorg/include -I/usr/include/freetype2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include   -fPIC"
#define  LDFLAGSX  "-lglib-2.0   -Wl,--export-dynamic -L/usr/xorg/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -lXinerama -lXext -lXcursor -latk-1.0 -lgdk_pixbuf-2.0 -lpangoxft-1.0 -lXft -lXrender -lfontconfig -lfreetype -lz -lpangox-1.0 -lX11 -lpango-1.0 -lm -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0  "
#define  GMODULE_LIBS  "-Wl,--export-dynamic -lgmodule-2.0 -ldl -lglib-2.0  "

#endif

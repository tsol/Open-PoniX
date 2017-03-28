#
# Configuration file for using the gdk-pixbuf library in GNOME applications
#
GDK_PIXBUF_LIBDIR="-L/usr/lib "
GDK_PIXBUF_LIBS="-L/usr/lib -rdynamic -lgmodule -lglib -ldl -lgdk_pixbuf -L/usr/lib -L/usr/xorg/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm "
GDK_PIXBUF_INCLUDEDIR="-I/usr/include -I/usr/xorg/include/ -I/usr/lib/glib/include "
MODULE_VERSION="gdk-pixbuf-0.10.0"

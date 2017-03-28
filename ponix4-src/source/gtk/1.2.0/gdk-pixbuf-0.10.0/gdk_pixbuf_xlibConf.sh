#
# Configuration file for using the gdk-pixbuf library in Xlib applications
#
GDK_PIXBUF_XLIB_LIBDIR="-L/usr/lib"
GDK_PIXBUF_XLIB_LIBS="-L/usr/lib -rdynamic -lgmodule -lglib -ldl -lgdk_pixbuf_xlib "
GDK_PIXBUF_XLIB_INCLUDEDIR="-I/usr/include -I/usr/xorg/include/ -I/usr/lib/glib/include"
MODULE_VERSION="gdk-pixbuf-0.10.0"

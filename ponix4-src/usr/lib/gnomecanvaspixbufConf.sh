#
# Configuration file for using the gnomecanvaspixbuf library in GNOME applications
#
GNOMECANVASPIXBUF_LIBDIR="-L/usr/lib"
GNOMECANVASPIXBUF_LIBS="-L/usr/lib -rdynamic -lgmodule -lglib -ldl -lgdk_pixbuf -L/usr/lib -L/usr/xorg/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm -lgnomecanvaspixbuf"
GNOMECANVASPIXBUF_INCLUDEDIR="-I/usr/include -I/usr/xorg/include/ -I/usr/lib/glib/include"
MODULE_VERSION="gnomecanvaspixbuf-0.10.0"

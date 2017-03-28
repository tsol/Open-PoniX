#for GTK 2.4.0

#glib 2.4.0
rm -r /usr/include/glib*
rm -r /usr/lib/libglib*
rm -r /usr/lib/libgmodule*
rm -r /usr/lib/libgobject*
rm -r /usr/lib/libgthread*
rm -r /usr/lib/glib*
rm -r /usr/lib/pkgconfig/glib*.pc
rm -r /usr/lib/pkgconfig/gmodule*.pc
rm -r /usr/lib/pkgconfig/gobject*.pc
rm -r /usr/lib/pkgconfig/gthread*.pc
rm -r /usr/bin/glib*
rm -r /usr/bin/gobject-query
rm -r /usr/share/gtk-doc
rm -r /usr/share/glib*
rm -r /usr/include/gdk*

#atk-1.0.1
rm -r /usr/include/atk-1.0
rm -r /usr/lib/libatk*
rm -r /usr/lib/pkgconfig/atk*

#pango-1.4.0 (зависит от libXft(из Xorg)
#libfontconfig и freetype)
rm -r /usr/bin/pango-querymodules
rm -r /usr/etc/pango
rm -r /usr/include/pango-1.0
rm -r /usr/lib/libpango*
rm -r /usr/lib/pango
rm -r /usr/lib/pkgconfig/pango*

#cairo
rm -r /usr/lib/libcairo*
rm -r /usr/lib/pkgconfig/cairo*

#libgailutil
rm -r /usr/lib/libgailutil*
rm -r /usr/lib/pkgconfig/gail*
rm -r /usr/include/gail-*

#libgio
rm -r /usr/lib/libgio*
rm -r /usr/lib/pkgconfig/gio*
rm -r /usr/lib/gio
rm -r /usr/include/gio-unix*

#libfontconfig
rm -r /usr/lib/libfontconfig*
rm -r /usr/include/fontconfig
rm -r /usr/lib/pkgconfig/fontconfig.pc
rm -r /usr/etc/conf.avail
rm -r /usr/etc/conf.d
rm -r /usr/etc/fonts.conf

#freetype-2.1.10
rm -r /usr/bin/freetype-config
rm -r /usr/include/freetype2  
rm -r /usr/include/ft2*
rm -r /usr/lib/libfreetype*
rm -r /usr/lib/pkgconfig/freetype*
rm -r /usr/share/aclocal/freetype2*

#gtk-2.4.0
rm -r /usr/bin/gtk-*
rm -r /usr/bin/gdk-*
rm -r /usr/etc/gtk*
rm -r /usr/include/gtk*
rm -r /usr/lib/libgtk*
rm -r /usr/lib/libgdk*
rm -r /usr/lib/gtk*
rm -r /usr/lib/pkgconfig/gdk*
rm -r /usr/lib/pkgconfig/gtk*
rm -r /usr/share/gtk*
rm -r /usr/share/themes
rm -r /usr/lib/gdk-pixbuf*
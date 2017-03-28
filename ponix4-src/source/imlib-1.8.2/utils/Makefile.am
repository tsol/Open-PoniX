INCLUDES = -I. -I$(srcdir) \
	-I$(top_builddir)/gdk_imlib -I$(top_srcdir)/gdk_imlib \
	$(GTK_CFLAGS)

bin_PROGRAMS = imlib_config

imlib_config_SOURCES = imlib_config.c icons.c testimg.c

LDADD = ../gdk_imlib/libgdk_imlib.la $(GTK_LIBS)


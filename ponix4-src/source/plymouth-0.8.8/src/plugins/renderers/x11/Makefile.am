INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)/../../../libply                                        \
           -I$(srcdir)/../../../libply-splash-core                            \
           -I$(srcdir)/../../..                                               \
           -I$(srcdir)/../..                                                  \
           -I$(srcdir)/..                                                     \
           -I$(srcdir)

if ENABLE_GTK
plugindir = $(libdir)/plymouth/renderers
plugin_LTLIBRARIES = x11.la

x11_la_CFLAGS = $(GTK_CFLAGS) $(PLYMOUTH_CFLAGS)
x11_la_LDFLAGS = -module -avoid-version -export-dynamic
x11_la_LIBADD = $(PLYMOUTH_LIBS)                             \
                $(GTK_LIBS)                                  \
                ../../../libply/libply.la                    \
                ../../../libply-splash-core/libply-splash-core.la
x11_la_SOURCES = $(srcdir)/plugin.c
endif

MAINTAINERCLEANFILES = Makefile.in

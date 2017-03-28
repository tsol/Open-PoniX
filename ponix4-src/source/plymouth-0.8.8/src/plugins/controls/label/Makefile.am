INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)/../../../libply                                        \
           -I$(srcdir)/../../../libply-splash-core                            \
           -I$(srcdir)/../../../libply-splash-graphics                        \
           -I$(srcdir)/../../..                                               \
           -I$(srcdir)/../..                                                  \
           -I$(srcdir)/..                                                     \
           -I$(srcdir)

plugindir = $(libdir)/plymouth
plugin_LTLIBRARIES = label.la

label_la_CFLAGS = $(PLYMOUTH_CFLAGS) $(PANGO_CFLAGS)

label_la_LDFLAGS = -module -avoid-version -export-dynamic
label_la_LIBADD = $(PLYMOUTH_LIBS) $(PANGO_LIBS)                           \
                  ../../../libply/libply.la                                \
                  ../../../libply-splash-core/libply-splash-core.la        \
                  ../../../libply-splash-graphics/libply-splash-graphics.la
label_la_SOURCES = $(srcdir)/plugin.c

MAINTAINERCLEANFILES = Makefile.in

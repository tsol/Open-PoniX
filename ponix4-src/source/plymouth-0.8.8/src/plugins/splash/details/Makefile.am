INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)/../../../libply                                        \
           -I$(srcdir)/../../../libply-splash-core                            \
           -I$(srcdir)/../../..                                               \
           -I$(srcdir)/../..                                                  \
           -I$(srcdir)/..                                                     \
           -I$(srcdir)

plugindir = $(libdir)/plymouth
plugin_LTLIBRARIES = details.la

details_la_CFLAGS = $(PLYMOUTH_CFLAGS)
details_la_LDFLAGS = -module -avoid-version -export-dynamic
details_la_LIBADD = $(PLYMOUTH_LIBS)                                         \
                    ../../../libply/libply.la                                \
                    ../../../libply-splash-core/libply-splash-core.la
details_la_SOURCES = $(srcdir)/plugin.c

MAINTAINERCLEANFILES = Makefile.in

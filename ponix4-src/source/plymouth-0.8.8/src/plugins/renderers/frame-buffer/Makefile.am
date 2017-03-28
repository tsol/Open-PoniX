INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)/../../../libply                                        \
           -I$(srcdir)/../../../libply-splash-core                            \
           -I$(srcdir)/../../..                                               \
           -I$(srcdir)/../..                                                  \
           -I$(srcdir)/..                                                     \
           -I$(srcdir)

plugindir = $(libdir)/plymouth/renderers
plugin_LTLIBRARIES = frame-buffer.la

frame_buffer_la_CFLAGS = $(PLYMOUTH_CFLAGS)

frame_buffer_la_LDFLAGS = -module -avoid-version -export-dynamic
frame_buffer_la_LIBADD = $(PLYMOUTH_LIBS)                                     \
                         ../../../libply/libply.la                            \
                         ../../../libply-splash-core/libply-splash-core.la
frame_buffer_la_SOURCES = $(srcdir)/plugin.c

MAINTAINERCLEANFILES = Makefile.in

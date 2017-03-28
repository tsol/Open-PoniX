INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)/../../../libply                                        \
           -I$(srcdir)/../../../libply-splash-core                            \
           -I$(srcdir)/../../..                                               \
           -I$(srcdir)/../..                                                  \
           -I$(srcdir)/..                                                     \
           -I$(srcdir)

plugindir = $(libdir)/plymouth
plugin_LTLIBRARIES = text.la

text_la_CFLAGS = $(PLYMOUTH_CFLAGS)                                           \
                 -DPLYMOUTH_BACKGROUND_COLOR=$(background_color)              \
                 -DPLYMOUTH_BACKGROUND_END_COLOR=$(background_end_color)      \
                 -DPLYMOUTH_BACKGROUND_START_COLOR=$(background_start_color)

text_la_LDFLAGS = -module -avoid-version -export-dynamic
text_la_LIBADD = $(PLYMOUTH_LIBS)                                            \
                 ../../../libply/libply.la                                   \
                 ../../../libply-splash-core/libply-splash-core.la
text_la_SOURCES = $(srcdir)/plugin.c

MAINTAINERCLEANFILES = Makefile.in

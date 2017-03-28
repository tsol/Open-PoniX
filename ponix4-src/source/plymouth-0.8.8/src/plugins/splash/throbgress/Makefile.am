INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)/../../../libply                                        \
           -I$(srcdir)/../../../libply-splash-core                            \
           -I$(srcdir)/../../../libply-splash-graphics                        \
           -I$(srcdir)/../../..                                               \
           -I$(srcdir)/../..                                                  \
           -I$(srcdir)/..                                                     \
           -I$(srcdir)

plugindir = $(libdir)/plymouth
plugin_LTLIBRARIES = throbgress.la

throbgress_la_CFLAGS = $(PLYMOUTH_CFLAGS)                                    \
                       -DPLYMOUTH_IMAGE_DIR=\"$(datadir)/plymouth/\"         \
                       -DPLYMOUTH_TIME_DIR=\"$(localstatedir)/lib/plymouth/\"\
                       -DPLYMOUTH_LOGO_FILE=\"$(logofile)\"                  \
                       -DPLYMOUTH_BACKGROUND_COLOR=$(background_color)       \
                       -DPLYMOUTH_BACKGROUND_END_COLOR=$(background_end_color)   \
                       -DPLYMOUTH_BACKGROUND_START_COLOR=$(background_start_color)

throbgress_la_LDFLAGS = -module -avoid-version -export-dynamic
throbgress_la_LIBADD = $(PLYMOUTH_LIBS)                                      \
                       ../../../libply/libply.la                             \
                       ../../../libply-splash-core/libply-splash-core.la     \
                       ../../../libply-splash-graphics/libply-splash-graphics.la
throbgress_la_SOURCES = $(srcdir)/plugin.c

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)                                                        \
           -I$(srcdir)/../libply                                              \
           -I$(srcdir)/../libply-splash-core                                  \
           -I$(srcdir)/../plugins/controls

lib_LTLIBRARIES = libply-splash-graphics.la

libply_splash_graphicsdir = $(includedir)/plymouth-1/ply-splash-graphics
libply_splash_graphics_HEADERS = \
                                 ply-animation.h                              \
                                 ply-entry.h                                  \
                                 ply-image.h                                  \
                                 ply-label.h                                  \
                                 ply-label-plugin.h                           \
                                 ply-progress-animation.h                     \
                                 ply-progress-bar.h                           \
                                 ply-throbber.h

libply_splash_graphics_la_CFLAGS = $(PLYMOUTH_CFLAGS)                               \
                                   $(IMAGE_CFLAGS)                                  \
                                   -DPLYMOUTH_BACKGROUND_COLOR=$(background_color)  \
                                   -DPLYMOUTH_BACKGROUND_END_COLOR=$(background_end_color) \
                                   -DPLYMOUTH_BACKGROUND_START_COLOR=$(background_start_color) \
                                   -DPLYMOUTH_PLUGIN_PATH=\"$(PLYMOUTH_PLUGIN_PATH)\"
libply_splash_graphics_la_LIBADD = $(PLYMOUTH_LIBS) $(IMAGE_LIBS) ../libply/libply.la ../libply-splash-core/libply-splash-core.la
libply_splash_graphics_la_LDFLAGS = -export-symbols-regex '^[^_].*' \
                                    -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
                                    -no-undefined
libply_splash_graphics_la_SOURCES = \
                                    $(libply_splash_graphics_HEADERS)         \
                                    ply-animation.c                           \
                                    ply-entry.c                               \
                                    ply-image.c                               \
                                    ply-label.c                               \
                                    ply-progress-animation.c                  \
                                    ply-progress-bar.c                        \
                                    ply-throbber.c

MAINTAINERCLEANFILES = Makefile.in

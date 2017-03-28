INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)                                                        \
           -I$(srcdir)/../libply                                              \
           -I$(srcdir)/../plugins/controls

if WITH_SYSTEM_ROOT_INSTALL
librarydir = $(libdir:/usr%=%)
else
librarydir = $(libdir)
endif

library_LTLIBRARIES = libply-splash-core.la

libply_splash_coredir = $(includedir)/plymouth-1/ply-splash-core
libply_splash_core_HEADERS = \
		    ply-boot-splash.h                                         \
		    ply-boot-splash-plugin.h                                  \
		    ply-keyboard.h                                            \
		    ply-pixel-buffer.h                                        \
		    ply-pixel-display.h                                       \
		    ply-renderer.h                                            \
		    ply-renderer-plugin.h                                     \
		    ply-terminal.h                                            \
		    ply-text-display.h                                        \
		    ply-text-progress-bar.h

libply_splash_core_la_CFLAGS = $(PLYMOUTH_CFLAGS)                               \
                             -DPLYMOUTH_BACKGROUND_COLOR=$(background_color)   \
                       -DPLYMOUTH_BACKGROUND_END_COLOR=$(background_end_color) \
                       -DPLYMOUTH_BACKGROUND_START_COLOR=$(background_start_color) \
                       -DPLYMOUTH_PLUGIN_PATH=\"$(PLYMOUTH_PLUGIN_PATH)\"
libply_splash_core_la_LIBADD = $(PLYMOUTH_LIBS) ../libply/libply.la
libply_splash_core_la_LDFLAGS = -export-symbols-regex '^[^_].*' \
		    -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
		    -no-undefined
libply_splash_core_la_SOURCES = \
		    $(libply_splash_core_HEADERS)                              \
		    ply-keyboard.c                                           \
		    ply-pixel-display.c                                      \
		    ply-text-display.c                                       \
		    ply-text-progress-bar.c                                  \
		    ply-terminal.c                                           \
		    ply-pixel-buffer.c                                       \
		    ply-renderer.c                                           \
		    ply-boot-splash.c

MAINTAINERCLEANFILES = Makefile.in

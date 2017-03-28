INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)/../../../libply                                        \
           -I$(srcdir)/../../../libply-splash-core                            \
           -I$(srcdir)/../../../libply-splash-graphics                        \
           -I$(srcdir)/../../..                                               \
           -I$(srcdir)/../..                                                  \
           -I$(srcdir)/..                                                     \
           -I$(srcdir)

plugindir = $(libdir)/plymouth
plugin_LTLIBRARIES = script.la

script_la_CFLAGS =  $(PLYMOUTH_CFLAGS)                                        \
                    -DPLYMOUTH_IMAGE_DIR=\"$(datadir)/plymouth/\"             \
                    -DPLYMOUTH_LOGO_FILE=\"$(logofile)\"                      \
                    -DPLYMOUTH_BACKGROUND_COLOR=$(background_color)           \
                    -DPLYMOUTH_BACKGROUND_END_COLOR=$(background_end_color)   \
                    -DPLYMOUTH_BACKGROUND_START_COLOR=$(background_start_color)
script_la_LDFLAGS = -module -avoid-version -export-dynamic
script_la_LIBADD =  $(PLYMOUTH_LIBS)                                          \
                    ../../../libply/libply.la                                 \
                    ../../../libply-splash-core/libply-splash-core.la         \
                    ../../../libply-splash-graphics/libply-splash-graphics.la
script_la_SOURCES = $(srcdir)/plugin.c                                        \
                    $(srcdir)/plugin.h                                        \
                    $(srcdir)/script.c                                        \
                    $(srcdir)/script.h                                        \
                    $(srcdir)/script-scan.c                                   \
                    $(srcdir)/script-scan.h                                   \
                    $(srcdir)/script-parse.c                                  \
                    $(srcdir)/script-parse.h                                  \
                    $(srcdir)/script-execute.c                                \
                    $(srcdir)/script-execute.h                                \
                    $(srcdir)/script-object.c                                 \
                    $(srcdir)/script-object.h                                 \
                    $(srcdir)/script-debug.c                                  \
                    $(srcdir)/script-debug.h                                  \
                    $(srcdir)/script-lib-image.c                              \
                    $(srcdir)/script-lib-image.h                              \
                    $(srcdir)/script-lib-image.script                         \
                    $(srcdir)/script-lib-sprite.c                             \
                    $(srcdir)/script-lib-sprite.h                             \
                    $(srcdir)/script-lib-sprite.script                        \
                    $(srcdir)/script-lib-plymouth.c                           \
                    $(srcdir)/script-lib-plymouth.h                           \
                    $(srcdir)/script-lib-plymouth.script                      \
                    $(srcdir)/script-lib-math.c                               \
                    $(srcdir)/script-lib-math.h                               \
                    $(srcdir)/script-lib-math.script                          \
                    $(srcdir)/script-lib-string.c                             \
                    $(srcdir)/script-lib-string.h                             \
                    $(srcdir)/script-lib-string.script

MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = *.script.h

BUILT_SOURCES = script-lib-image.script.h                                     \
                script-lib-sprite.script.h                                    \
                script-lib-plymouth.script.h                                  \
                script-lib-math.script.h                                      \
                script-lib-string.script.h

%.script.h: %.script
	sed -e 's/\\/\\\\/g'                                                  \
	    -e 's/"/\\"/g'                                                    \
	    -e 's/\(.*\)/  "\1\\n"/g'                                         \
	    -e '1istatic const char* $(subst -,_,$(@:.script.h=_string)) ='   \
	    -e '$$a;'                                                         \
	    $< > $@

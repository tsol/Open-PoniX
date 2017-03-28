SUBDIRS = tests
INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)                                                        \
           -DPLYMOUTH_TIME_DIRECTORY=\"$(localstatedir)/lib/plymouth/\"       

if WITH_SYSTEM_ROOT_INSTALL
librarydir = $(subst /usr/lib,/lib,$(libdir))
else
librarydir = $(libdir)
endif
library_LTLIBRARIES = libply.la

libplydir = $(includedir)/plymouth-1/ply
libply_HEADERS = \
		    ply-event-loop.h                                          \
		    ply-command-parser.h                                      \
		    ply-buffer.h                                              \
		    ply-array.h                                               \
		    ply-bitarray.h                                            \
		    ply-list.h                                                \
		    ply-hashtable.h                                           \
		    ply-logger.h                                              \
		    ply-key-file.h                                            \
		    ply-progress.h                                            \
		    ply-rectangle.h                                           \
		    ply-region.h                                              \
		    ply-terminal-session.h                                    \
		    ply-trigger.h                                             \
		    ply-utils.h

libply_la_CFLAGS = $(PLYMOUTH_CFLAGS)
libply_la_LIBADD = $(PLYMOUTH_LIBS)
libply_la_LDFLAGS = -export-symbols-regex '^[^_].*' \
		    -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
		    -no-undefined
libply_la_SOURCES = ply-event-loop.c                                          \
		    $(libply_HEADERS)                                         \
		    ply-command-parser.c                                      \
		    ply-buffer.c                                              \
		    ply-array.c                                               \
		    ply-bitarray.c                                            \
		    ply-list.c                                                \
		    ply-hashtable.c                                           \
		    ply-logger.c                                              \
		    ply-key-file.c                                            \
		    ply-progress.c                                            \
		    ply-rectangle.c                                           \
		    ply-region.c                                              \
		    ply-terminal-session.c                                    \
		    ply-trigger.c                                             \
		    ply-utils.c

MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = . tests

INCLUDES = -I$(top_srcdir)                                                    \
           -I$(top_srcdir)/src                                                \
           -I$(top_srcdir)/src/libply                                         \
           -I$(srcdir)

plymouthdir = $(plymouthclientdir)

plymouth_PROGRAMS = plymouth

plymouth_CFLAGS = $(PLYMOUTH_CFLAGS) -DPLYMOUTH_PLUGIN_PATH=\"$(PLYMOUTH_PLUGIN_PATH)\"
plymouth_LDADD = $(PLYMOUTH_LIBS) ../libply/libply.la
plymouth_SOURCES = \
                      $(srcdir)/../ply-boot-protocol.h                        \
                      $(srcdir)/ply-boot-client.h                             \
                      $(srcdir)/ply-boot-client.c                             \
                      $(srcdir)/plymouth.c

lib_LTLIBRARIES = libply-boot-client.la

libply_boot_clientdir = $(includedir)/plymouth-1/ply-boot-client
libply_boot_client_HEADERS = \
                      $(srcdir)/../ply-boot-protocol.h                        \
		      ply-boot-client.h

libply_boot_client_la_CFLAGS = $(PLYMOUTH_CFLAGS)
libply_boot_client_la_LIBADD = $(PLYMOUTH_LIBS) ../libply/libply.la
libply_boot_client_la_LDFLAGS = -export-symbols-regex '^[^_].*' \
		-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
		-no-undefined
libply_boot_client_la_SOURCES = \
                      $(srcdir)/../ply-boot-protocol.h                        \
                      $(srcdir)/ply-boot-client.h                             \
                      $(srcdir)/ply-boot-client.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ply-boot-client.pc

if WITH_RHGB_COMPAT_LINK
install-data-hook:
	(mkdir -p $(DESTDIR)$(bindir) && cd $(DESTDIR)$(bindir) && ln -sf ../../bin/plymouth rhgb-client)

uninstall-hook:
	rm -f $(DESTDIR)$(bindir)/rhgb-client
	-rmdir -p $(DESTDIR)$(bindir)/rhgb-client >& /dev/null
endif

EXTRA_DIST = ply-boot-client.pc.in
MAINTAINERCLEANFILES = Makefile.in

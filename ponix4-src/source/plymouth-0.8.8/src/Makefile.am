SUBDIRS = libply libply-splash-core libply-splash-graphics . plugins client viewer tests
if ENABLE_UPSTART_MONITORING
SUBDIRS += upstart-bridge
endif
INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)/libply                                                 \
           -I$(srcdir)/libply-splash-core                                     \
           -I$(srcdir)                                                        \
           -DPLYMOUTH_LOG_DIRECTORY=\"$(localstatedir)/log\"                  \
           -DPLYMOUTH_SPOOL_DIRECTORY=\"$(localstatedir)/spool/plymouth\"     \
           -DPLYMOUTH_TIME_DIRECTORY=\"$(localstatedir)/lib/plymouth/\"       \
           -DPLYMOUTH_LOGO_FILE=\"$(logofile)\"

plymouthdbindir = $(plymouthdaemondir)
plymouthdbin_PROGRAMS = plymouthd

plymouthd_CFLAGS = $(PLYMOUTH_CFLAGS)                                         \
		   -DPLYMOUTH_PLUGIN_PATH=\"$(PLYMOUTH_PLUGIN_PATH)\"         \
		   -DPLYMOUTH_THEME_PATH=\"$(PLYMOUTH_THEME_PATH)/\"          \
		   -DPLYMOUTH_POLICY_DIR=\"$(PLYMOUTH_POLICY_DIR)/\"          \
		   -DPLYMOUTH_RUNTIME_DIR=\"$(PLYMOUTH_RUNTIME_DIR)\"         \
		   -DPLYMOUTH_CONF_DIR=\"$(PLYMOUTH_CONF_DIR)/\"
plymouthd_LDADD = $(PLYMOUTH_LIBS) libply/libply.la libply-splash-core/libply-splash-core.la
plymouthd_SOURCES =                                                            \
                   ply-boot-protocol.h                                        \
                   ply-boot-server.h                                          \
                   ply-boot-server.c                                          \
                   plugins/splash/details/plugin.c                  \
                   main.c

plymouthdrundir = $(localstatedir)/run/plymouth
plymouthdspooldir = $(localstatedir)/spool/plymouth
plymouthdtimedir = $(localstatedir)/lib/plymouth

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ply-splash-core.pc ply-splash-graphics.pc

plymouthd_defaultsdir = $(PLYMOUTH_POLICY_DIR)
dist_plymouthd_defaults_DATA = plymouthd.defaults

plymouthd_confdir = $(PLYMOUTH_CONF_DIR)
dist_plymouthd_conf_DATA = plymouthd.conf

install-data-hook:
	-mkdir -p $(DESTDIR)$(plymouthdrundir)
	-mkdir -p $(DESTDIR)$(plymouthdspooldir)
	-mkdir -p $(DESTDIR)$(plymouthdtimedir)

EXTRA_DIST = ply-splash-core.pc.in ply-splash-graphics.pc.in
MAINTAINERCLEANFILES = Makefile.in

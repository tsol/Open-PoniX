EXTRA_DIST = \
  mbtheme-standalone.c mbtheme-standalone.h mbtheme.c mbtheme.h xml.c xml.h

if WANT_STANDALONE
standalone_src  = mbtheme-standalone.c mbtheme-standalone.h
else
standalone_src  = mbtheme.c mbtheme.h xml.c xml.h
endif

PREFIXDIR  = $(prefix)
PKGDATADIR = $(datadir)/matchbox
DATADIR    = $(datadir)
CONFDIR    = $(sysconfdir)/matchbox

bin_PROGRAMS = matchbox-window-manager matchbox-remote

INCLUDES = -DDATADIR=\"$(DATADIR)\" -DCONFDIR=\"$(CONFDIR)\" -DPKGDATADIR=\"$(PKGDATADIR)\" -DPREFIX=\"$(PREFIXDIR)\" $(LIBMB_CFLAGS) $(COMPO_CFLAGS) $(EXPAT_CFLAGS) $(SN_CFLAGS) $(GCONF_CFLAGS) $(XFIXES_CFLAGS) $(XCURSOR_CFLAGS)

matchbox_remote_LDADD = $(LIBMB_LIBS)

matchbox_remote_SOURCES = matchbox-remote.c 

matchbox_window_manager_LDADD = $(LIBMB_LIBS) $(COMPO_LIBS) $(EXPAT_LIBS) $(SN_LIBS) $(GCONF_LIBS) $(XFIXES_LIBS) $(XCURSOR_LIBS)

matchbox_window_manager_SOURCES =                        \
		   main.c structs.h wm.c wm.h            \
                   base_client.c base_client.h           \
                   main_client.c main_client.h           \
		   toolbar_client.c toolbar_client.h     \
		   toolbar_client_alt.c                  \
	           dockbar_client.c dockbar_client.h     \
                   dialog_client.c dialog_client.h       \
		   select_client.c select_client.h       \
		   desktop_client.c desktop_client.h     \
                   ewmh.c ewmh.h                         \
                   misc.c misc.h                         \
		   client_common.c client_common.h       \
		   keys.c keys.h                         \
                   list.c list.h                         \
	           stack.c stack.h                       \
		   composite-engine.c composite-engine.h \
                   session.c session.h                   \
                   $(standalone_src)


clean-local:
	/bin/rm *.bb *.bbg *.da *.gcov || true
        


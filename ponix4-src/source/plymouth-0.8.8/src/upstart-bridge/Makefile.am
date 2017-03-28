INCLUDES = -I$(top_srcdir)                                                    \
           -I$(top_srcdir)/src                                                \
           -I$(top_srcdir)/src/libply                                         \
           -I$(top_srcdir)/src/client                                         \
           -I$(srcdir)
plymouthdir = $(plymouthclientdir)

plymouth_PROGRAMS = plymouth-upstart-bridge

plymouth_upstart_bridge_CFLAGS = $(PLYMOUTH_CFLAGS) $(DBUS_CFLAGS)
plymouth_upstart_bridge_LDADD = \
                      $(PLYMOUTH_LIBS) \
                      $(DBUS_LIBS) \
                      $(CURSES_LIBS) \
                      ../libply/libply.la \
                      ../client/libply-boot-client.la
plymouth_upstart_bridge_SOURCES = \
                      $(srcdir)/../ply-boot-protocol.h                        \
                      $(srcdir)/ply-upstart-monitor.h                         \
                      $(srcdir)/ply-upstart-monitor.c                         \
                      $(srcdir)/plymouth-upstart-bridge.c

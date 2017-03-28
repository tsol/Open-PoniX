SUBDIRS = .
EXTRA_DIST=

INCLUDES = -I$(top_srcdir)                                                    \
           -I$(srcdir)
if WITH_LOG_VIEWER
plymouth_log_viewerdir = $(bindir)
plymouth_log_viewer_PROGRAMS = plymouth-log-viewer

plymouth_log_viewer_CFLAGS = $(GTK_CFLAGS) -DPLYMOUTH_LOG_DIRECTORY=\"$(localstatedir)/log\"
plymouth_log_viewer_LDADD = $(GTK_LIBS)
plymouth_log_viewer_SOURCES = $(srcdir)/plymouth-log-viewer.c

if START_FROM_GDM
gdmautostartdir = $(datadir)/gdm/autostart/LoginWindow
gdmautostart_DATA = plymouth-log-viewer.desktop
endif
endif

EXTRA_DIST += plymouth-log-viewer.desktop

MAINTAINERCLEANFILES = Makefile.in

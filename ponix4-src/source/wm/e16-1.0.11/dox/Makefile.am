SUBDIRS = E-docs

EXTRA_DIST = README

bin_PROGRAMS = edox

edox_SOURCES = dox.c dox.h file.c format.c text.c ttfont.c

edox_CPPFLAGS = -I$(top_builddir) $(IMLIB2_CFLAGS) $(X_CFLAGS) \
	$(CWARNFLAGS) -D ENLIGHTENMENT_ROOT=\"$(pkgdatadir)\"

LDADD = $(IMLIB2_LIBS) $(X_LIBS) $(XINERAMA_LIBS) -lX11 -lm


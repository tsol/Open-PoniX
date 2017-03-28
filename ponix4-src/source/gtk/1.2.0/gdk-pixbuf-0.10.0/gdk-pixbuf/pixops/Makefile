# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ../..
prefix = /usr
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/gdk-pixbuf
pkglibdir = $(libdir)/gdk-pixbuf
pkgincludedir = $(includedir)/gdk-pixbuf

top_builddir = ../..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = i686-pc-linux-gnu
host_triplet = i686-pc-linux-gnu
AS = @AS@
CC = i486-TSOL-linux-gnu-gcc
CPP = i486-TSOL-linux-gnu-gcc -E
DLLTOOL = @DLLTOOL@
GDK_PIXBUF_DIR = /source/gtk/1.2.0/gdk-pixbuf-0.10.0
GDK_PIXBUF_INCLUDEDIR = -I${prefix}/include -I/usr/xorg/include/ -I/usr/lib/glib/include
GDK_PIXBUF_LIBDIR = -L${libdir}
GDK_PIXBUF_LIBS = -L/usr/lib -rdynamic -lgmodule -lglib -ldl -lgdk_pixbuf -L/usr/lib -L/usr/xorg/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm
GDK_PIXBUF_MAJOR = 0
GDK_PIXBUF_MICRO = 0
GDK_PIXBUF_MINOR = 10
GDK_PIXBUF_VERSION = 0.10.0
GDK_PIXBUF_XLIB_INCLUDEDIR = -I${prefix}/include -I/usr/xorg/include/ -I/usr/lib/glib/include
GDK_PIXBUF_XLIB_LIBDIR = -L${libdir}
GDK_PIXBUF_XLIB_LIBS = -L/usr/lib -rdynamic -lgmodule -lglib -ldl -lgdk_pixbuf_xlib 
GLIB_CFLAGS = -I/usr/lib/glib/include
GLIB_CONFIG = /usr/bin/glib-config
GLIB_LIBS = -L/usr/lib -rdynamic -lgmodule -lglib -ldl
GMODULE_FLAGS = -I/usr/lib/glib/include
GMODULE_LIBS = -L/usr/lib -rdynamic -lgmodule -lglib -ldl
GNOMECANVASPIXBUF_INCLUDEDIR = -I${prefix}/include -I/usr/xorg/include/ -I/usr/lib/glib/include
GNOMECANVASPIXBUF_LIBDIR = -L${libdir}
GNOMECANVASPIXBUF_LIBS = -L/usr/lib -rdynamic -lgmodule -lglib -ldl -lgdk_pixbuf -L/usr/lib -L/usr/xorg/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm -lgnomecanvaspixbuf
GNOME_CFLAGS = 
GNOME_CONFIG = no
GNOME_LIBS = 
GTKDOC = false
GTK_CFLAGS = -I/usr/xorg/include/ -I/usr/lib/glib/include
GTK_CONFIG = /usr/bin/gtk-config
GTK_LIBS = -L/usr/lib -L/usr/xorg/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm
HAVE_GTK_DOC = 
LIBJPEG = -ljpeg
LIBPNG = -lpng -lz
LIBTIFF = -ltiff
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
MAINT = #
MAKEINFO = /source/gtk/1.2.0/gdk-pixbuf-0.10.0/missing makeinfo
OBJDUMP = @OBJDUMP@
PACKAGE = gdk-pixbuf
RANLIB = ranlib
STATIC_LIB_DEPS = -ltiff -ljpeg -lpng -lz
VERSION = 0.10.0

noinst_LTLIBRARIES = libpixops.la

INCLUDES = $(GLIB_CFLAGS) $(GTK_CFLAGS) -I$(top_srcdir)/gdk-pixbuf

noinst_PROGRAMS = timescale

timescale_SOURCES = timescale.c
timescale_LDADD = libpixops.la $(GLIB_LIBS) -lm

mmx_sources = 	have_mmx.S				scale_line_22_33_mmx.S 			composite_line_22_4a4_mmx.S 		composite_line_color_22_4a4_mmx.S

libpixops_la_SOURCES =  	pixops.c				pixops.h				pixops-internal.h			$(mmx_sources)


EXTRA_DIST =  	DETAILS

mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../../config.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(noinst_LTLIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I../..
CPPFLAGS = 
LDFLAGS = -static-libgcc
LIBS = 
X_CFLAGS =  -I/usr/xorg/include
X_LIBS =  -L/usr/xorg/lib
X_EXTRA_LIBS = 
X_PRE_LIBS =  -lSM -lICE
libpixops_la_LDFLAGS = 
libpixops_la_LIBADD = 
#libpixops_la_OBJECTS =  pixops.lo
libpixops_la_OBJECTS =  pixops.lo have_mmx.lo \
scale_line_22_33_mmx.lo composite_line_22_4a4_mmx.lo \
composite_line_color_22_4a4_mmx.lo
PROGRAMS =  $(noinst_PROGRAMS)

timescale_OBJECTS =  timescale.o
timescale_DEPENDENCIES =  libpixops.la
timescale_LDFLAGS = 
CFLAGS = -O2
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
DIST_COMMON =  README Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best
SOURCES = $(libpixops_la_SOURCES) $(timescale_SOURCES)
OBJECTS = $(libpixops_la_OBJECTS) $(timescale_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .lo .o .s
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps gdk-pixbuf/pixops/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-noinstLTLIBRARIES:

clean-noinstLTLIBRARIES:
	-test -z "$(noinst_LTLIBRARIES)" || rm -f $(noinst_LTLIBRARIES)

distclean-noinstLTLIBRARIES:

maintainer-clean-noinstLTLIBRARIES:

.c.o:
	$(COMPILE) -c $<

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

.c.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.s.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.S.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:

maintainer-clean-libtool:

libpixops.la: $(libpixops_la_OBJECTS) $(libpixops_la_DEPENDENCIES)
	$(LINK)  $(libpixops_la_LDFLAGS) $(libpixops_la_OBJECTS) $(libpixops_la_LIBADD) $(LIBS)

mostlyclean-noinstPROGRAMS:

clean-noinstPROGRAMS:
	-test -z "$(noinst_PROGRAMS)" || rm -f $(noinst_PROGRAMS)

distclean-noinstPROGRAMS:

maintainer-clean-noinstPROGRAMS:

timescale: $(timescale_OBJECTS) $(timescale_DEPENDENCIES)
	@rm -f timescale
	$(LINK) $(timescale_LDFLAGS) $(timescale_OBJECTS) $(timescale_LDADD) $(LIBS)

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags  $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = gdk-pixbuf/pixops

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
pixops.lo pixops.o : pixops.c ../../config.h pixops.h \
	../../gdk-pixbuf/gdk-pixbuf.h \
	../../gdk-pixbuf/gdk-pixbuf-features.h pixops-internal.h
timescale.o: timescale.c pixops.h ../../gdk-pixbuf/gdk-pixbuf.h \
	../../gdk-pixbuf/gdk-pixbuf-features.h

info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am:
install-exec: install-exec-am

install-data-am:
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am:
uninstall: uninstall-am
all-am: Makefile $(LTLIBRARIES) $(PROGRAMS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-noinstLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-noinstPROGRAMS \
		mostlyclean-tags mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-noinstLTLIBRARIES clean-compile clean-libtool \
		clean-noinstPROGRAMS clean-tags clean-generic \
		mostlyclean-am

clean: clean-am

distclean-am:  distclean-noinstLTLIBRARIES distclean-compile \
		distclean-libtool distclean-noinstPROGRAMS \
		distclean-tags distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-noinstLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-noinstPROGRAMS maintainer-clean-tags \
		maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-noinstLTLIBRARIES distclean-noinstLTLIBRARIES \
clean-noinstLTLIBRARIES maintainer-clean-noinstLTLIBRARIES \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile mostlyclean-libtool distclean-libtool \
clean-libtool maintainer-clean-libtool mostlyclean-noinstPROGRAMS \
distclean-noinstPROGRAMS clean-noinstPROGRAMS \
maintainer-clean-noinstPROGRAMS tags mostlyclean-tags distclean-tags \
clean-tags maintainer-clean-tags distdir info-am info dvi-am dvi check \
check-am installcheck-am installcheck install-exec-am install-exec \
install-data-am install-data install-am install uninstall-am uninstall \
all-redirect all-am all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:

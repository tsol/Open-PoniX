# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.3 from Makefile.am

# Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

# GPL (C) 1997 The Rasterman
#   raster@rasterman.com

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# the resultant executable


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
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

DISTDIR =

pkgdatadir = $(datadir)/imlib
pkglibdir = $(libdir)/imlib
pkgincludedir = $(includedir)/imlib

top_builddir = ..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /bin/install -c
INSTALL_PROGRAM = ${INSTALL}
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
CC = i486-TSOL-linux-gnu-gcc
CJPEG_PROG = /usr/bin/cjpeg
CONVERT_PATH = 
CONVERT_PROG = 
CPPFLAGS =   -I${prefix}/include -I/usr/include
DJPEG_PROG = /usr/bin/djpeg
GDK_IMLIB = gdk_imlib utils
GIFTOPNM_PROG = 
GTK_CFLAGS = 
GTK_CONFIG = /usr/bin/gtk-config
GTK_LIBS = 
LD = i486-TSOL-linux-gnu-ld
LDFLAGS = -static-libgcc
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
MAINT = #M#
MAKEINFO = /source/imlib-1.8.2/missing makeinfo
NETPBM_PATH = 
NM = /toolchain/i486-TSOL-linux-gnu/bin//nm -B
PACKAGE = imlib
RANLIB = ranlib
VERSION = 1.8.2
X_CFLAGS =  -I/usr/xorg/include
X_EXTRA_LIBS = 
X_LIBS = -lXext  -L/usr/xorg/lib
X_PRE_LIBS =  -lSM -lICE

lib_LTLIBRARIES = libgdk_imlib.la

include_HEADERS = gdk_imlib.h gdk_imlib_private.h gdk_imlib_types.h

libgdk_imlib_la_SOURCES = \
cache.c      \
colors.c     \
globals.c    \
load.c       \
misc.c       \
rend.c       \
utils.c      \
save.c

libgdk_imlib_la_LDFLAGS      = -version-info 9:1:8
libgdk_imlib_la_LDDADD = $(LIBS)

DEFS = -DDJPEG_PROG=\"/usr/bin/djpeg\"  -DCJPEG_PROG=\"/usr/bin/cjpeg\" \
	-DCONVERT_PATH=\"\" \
	-DNETPBM_PATH=\"\" -DSYSTEM_IMRC=\"${prefix}/etc/imrc\"

INCLUDES = -I. -I$(srcdir) -I.. -I$(srcdir)/.. $(GTK_CFLAGS) $(X_CFLAGS)
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(lib_LTLIBRARIES)

LIBS =  -ljpeg -ltiff -lpng -lz  -lm
libgdk_imlib_la_LIBADD = 
libgdk_imlib_la_OBJECTS =  cache.lo colors.lo globals.lo load.lo misc.lo \
rend.lo utils.lo save.lo
CFLAGS = -O2
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
LINK = $(LIBTOOL) --mode=link $(CC) $(CFLAGS) $(LDFLAGS) -o $@
HEADERS =  $(include_HEADERS)

DIST_COMMON =  ChangeLog Makefile.am Makefile.in install-sh \
mkinstalldirs


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
SOURCES = $(libgdk_imlib_la_SOURCES)
OBJECTS = $(libgdk_imlib_la_OBJECTS)

all: Makefile $(LTLIBRARIES) $(HEADERS)

.SUFFIXES:
.SUFFIXES: .S .c .lo .o .s
$(srcdir)/Makefile.in: #M# Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps gdk_imlib/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


mostlyclean-libLTLIBRARIES:

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)

distclean-libLTLIBRARIES:

maintainer-clean-libLTLIBRARIES:

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo "$(LIBTOOL)  --mode=install $(INSTALL_DATA) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL_DATA) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

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

libgdk_imlib.la: $(libgdk_imlib_la_OBJECTS) $(libgdk_imlib_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libgdk_imlib_la_LDFLAGS) $(libgdk_imlib_la_OBJECTS) $(libgdk_imlib_la_LIBADD) $(LIBS)

install-includeHEADERS: $(include_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(includedir)
	@list='$(include_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(includedir)/$$p"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(includedir)/$$p; \
	done

uninstall-includeHEADERS:
	@$(NORMAL_UNINSTALL)
	list='$(include_HEADERS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(includedir)/$$p; \
	done

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $(SOURCES) $(HEADERS) $(LISP)

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

subdir = gdk_imlib

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  test -f $(distdir)/$$file \
	  || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	  || cp -p $$d/$$file $(distdir)/$$file; \
	done

info:
dvi:
check: all
	$(MAKE)
installcheck:
install-exec: install-libLTLIBRARIES
	@$(NORMAL_INSTALL)

install-data: install-includeHEADERS
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-libLTLIBRARIES uninstall-includeHEADERS

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(DATADIR)$(libdir) $(DATADIR)$(includedir)


mostlyclean-generic:
	-test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(DISTCLEANFILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	-test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
	-test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
mostlyclean:  mostlyclean-libLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags \
		mostlyclean-generic

clean:  clean-libLTLIBRARIES clean-compile clean-libtool clean-tags \
		clean-generic mostlyclean

distclean:  distclean-libLTLIBRARIES distclean-compile distclean-libtool \
		distclean-tags distclean-generic clean
	-rm -f config.status
	-rm -f libtool

maintainer-clean:  maintainer-clean-libLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-generic \
		distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: mostlyclean-libLTLIBRARIES distclean-libLTLIBRARIES \
clean-libLTLIBRARIES maintainer-clean-libLTLIBRARIES \
uninstall-libLTLIBRARIES install-libLTLIBRARIES mostlyclean-compile \
distclean-compile clean-compile maintainer-clean-compile \
mostlyclean-libtool distclean-libtool clean-libtool \
maintainer-clean-libtool uninstall-includeHEADERS \
install-includeHEADERS tags mostlyclean-tags distclean-tags clean-tags \
maintainer-clean-tags distdir info dvi installcheck install-exec \
install-data install uninstall all installdirs mostlyclean-generic \
distclean-generic clean-generic maintainer-clean-generic clean \
mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:

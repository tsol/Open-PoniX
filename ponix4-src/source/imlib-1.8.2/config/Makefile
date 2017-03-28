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

EXTRA_DIST = imrc.in im_palette.pal
sysconf_DATA = imrc im_palette.pal
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../config.h
CONFIG_CLEAN_FILES = 
DATA =  $(sysconf_DATA)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP = --best
all: Makefile $(DATA)

.SUFFIXES:
$(srcdir)/Makefile.in: #M# Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4)
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps config/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


install-sysconfDATA: $(sysconf_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)
	@list='$(sysconf_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(sysconfdir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(sysconfdir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(sysconfdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(sysconfdir)/$$p; \
	  fi; fi; \
	done

uninstall-sysconfDATA:
	@$(NORMAL_UNINSTALL)
	list='$(sysconf_DATA)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(sysconfdir)/$$p; \
	done
tags: TAGS
TAGS:


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = config

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
install-exec: install-sysconfDATA
	@$(NORMAL_INSTALL)

install-data: 
	@$(NORMAL_INSTALL)

install: install-exec install-data all
	@:

uninstall: uninstall-sysconfDATA

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' INSTALL_SCRIPT='$(INSTALL_PROGRAM)' install
installdirs:
	$(mkinstalldirs)  $(DATADIR)$(sysconfdir)


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
mostlyclean:  mostlyclean-generic

clean:  clean-generic mostlyclean

distclean:  distclean-generic clean
	-rm -f config.status
	-rm -f libtool

maintainer-clean:  maintainer-clean-generic distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

.PHONY: uninstall-sysconfDATA install-sysconfDATA tags distdir info dvi \
installcheck install-exec install-data install uninstall all \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


imrc: imrc.in
	sed -e 's,\@sysconfdir\@,$(sysconfdir),g' \
		< $(srcdir)/imrc.in > imrc.tmp \
	&& mv imrc.tmp imrc

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:

# ./Makefile.in			-*- Makefile -*-
# $Id: Makefile.in,v 1.16 2003/03/07 02:24:18 gcw Exp $

# autoconf/Make.common.in	 -*- Makefile -*-
# release date (man), LSM date, version number/name, current maintainer
DATE=26 MARCH 2003
LSMDATE=26MAR03
VERSION=2.7.10
VERNAME=rxvt-$(VERSION)#
MAINT=Geoff Wing#
MAINTEMAIL=<gcw@rxvt.org>#
WEBMAINT=Oezguer Kesim#
WEBMAINTEMAIL=<oec@rxvt.org>#
WEBPAGE=<http://www.rxvt.org/>#
FTPSITENAME=ftp.rxvt.org#
FTPSITEDIR=/pub/rxvt#
#-------------------------------------------------------------------------
RXVTNAME=rxvt

SHELL = /bin/sh

# This variable makes it possible to move the installation root to another
# directory. This is useful when you're creating a binary distribution
# If empty, normal root will be used.
# You can run eg. 'make install DESTDIR=/packages/rxvt-xx' to accomplish
# that.
# DESTDIR = /usr/local/X11/$(VERNAME)

# Installation target directories & other installation stuff
prefix = /
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include
mandir = ${prefix}/man/man1
manext = 1

# Tools & program stuff
CC = i486-TSOL-linux-gnu-gcc
CPP = i486-TSOL-linux-gnu-gcc -E
MV = /bin/mv
RM = /bin/rm
RMF = /bin/rm -f
CP = /bin/cp
LN = /bin/ln
SED = /usr/bin/sed
AWK = gawk
ECHO = /bin/echo
CMP = /usr/bin/cmp
TBL = 
INSTALL = /bin/install -c
INSTALL_PROGRAM = /bin/install -c -m 755
INSTALL_DATA = /bin/install -c -m 644

# Flags & libs
# add -DBINDIR=\""$(bindir)/"\" to CPPFLAGS, if we need to spawn a program

CFLAGS = -O2 
CPPFLAGS =  
LDFLAGS = -static-libgcc
DEFS = -DHAVE_CONFIG_H
LIBS = 
DINCLUDE = 
DLIB = 

# X Include directory
XINC =  -I/usr/xorg/include 

# extra libraries needed by X on some systems, X library location
XLIB = -L/usr/xorg/lib -Wl,-rpath -Wl,/usr/xorg/lib  -lX11 

LIBTOOL = $(SHELL) $(top_builddir)/libtool
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(DEBUG) $(DINCLUDE) $(XINC) -I$(basedir) -I$(srcdir) -I.
LINK = $(CC) $(CFLAGS) $(LDFLAGS)

# End of common section of the Makefile
#-------------------------------------------------------------------------

srcdir =	.

.PATH:		.

first_rule: all
dummy:

subdirs = src doc rclock src/graphics src/test
allsubdirs = W11 $(subdirs)

DIST =	INSTALL README.configure configure Makefile Makefile.in ChangeLog

DIST_CFG = autoconf/aclocal.m4 autoconf/xpm.m4 autoconf/libtool.m4 \
	autoconf/configure.in  autoconf/config.h.in \
	autoconf/Make.common.in autoconf/install-sh autoconf/mkinstalldirs \
	autoconf/config.guess autoconf/config.sub \
	autoconf/ltmain.sh \

MKDIR = $(srcdir)/autoconf/mkinstalldirs

#-------------------------------------------------------------------------

all allbin alldoc tags:
	@if test xlinux-gnu = xcygwin; then (cd W11; ${MAKE} $@ || exit 1); fi
	@for I in ${subdirs}; do (cd $$I; ${MAKE} $@ || exit 1); done

realclean: clean
	$(RMF) config.h config.status config.log libtool

clean:
	$(RMF) *~ config.cache
	$(RMF) -r autom4te.cache
	@if test xlinux-gnu = xcygwin; then (cd W11; ${MAKE} $@ || exit 1); fi
	@for I in ${subdirs}; do (cd $$I; ${MAKE} $@ || exit 1); done

#
# entry points for other programs
#
rxvt:
	(cd src; ${MAKE})

clock:
	(cd rclock; ${MAKE})

graphics qplot:
	(cd src/graphics; ${MAKE} qplot)

tests:
	(cd src/test; ${MAKE} tests)

#-------------------------------------------------------------------------
configure: autoconf/configure.in autoconf/aclocal.m4 autoconf/config.h.in
	cd $(srcdir);
	./.prebuild

config.status:
	if test -x config.status; then config.status --recheck; \
	else $(SHELL) configure; fi

autoconf/config.h.in: autoconf/configure.in
	cd $(srcdir);
	./.prebuild

installdirs:
	$(MKDIR) $(DESTDIR)$(bindir)
	$(MKDIR) $(DESTDIR)$(mandir)

install: installdirs
	@if test xlinux-gnu = xcygwin; then (cd W11; ${MAKE} $@ || exit 1); fi
	@for I in $(subdirs); do (cd $$I; $(MAKE) DESTDIR=$(DESTDIR) $@ || exit 1); done

Makefiles:
	$(SHELL) config.status

cleandir: realclean

# distclean goal is for making a clean source tree, but if you have run
# configure from a different directory, then doesn't destroy all your
# hardly compiled and linked stuff. That's why there is always $(srcdir)/
# In that case most of those commands do nothing, except cleaning *~
# and cleaning source links.
distclean:
	(cd $(srcdir); $(RMF) *~ config.cache config.h config.log config.status libtool)
	@for I in $(allsubdirs); do (cd $$I; $(MAKE) $@ || exit 1); done
	(cd $(srcdir); $(RMF) Makefile autoconf/Make.common)

distdirs:
	mkdir ../$(VERNAME);
	mkdir ../$(VERNAME)/autoconf;
	@for I in $(allsubdirs); do (cd $$I; $(MAKE) $@ || (echo "Failed to make distclean in $$I"; exit 0) ); done

distcopy:
	$(CP) -p $(DIST) ../$(VERNAME);
	$(CP) -p $(DIST_CFG) ../$(VERNAME)/autoconf;
	@for I in $(allsubdirs); do (cd $$I; $(MAKE) $@ || exit 1); done

distrib: configure autoconf/config.h.in distdirs distcopy

tar.gz:  ../$(VERNAME).tar.gz
../$(VERNAME).tar.gz:
	(cd ..; tar cvf - $(VERNAME) | gzip -f9 > $(VERNAME).tar.gz)

tar.Z: ../$(VERNAME).tar.Z
../$(VERNAME).tar.Z:
	(cd ..; tar cvf - $(VERNAME) | compress > $(VERNAME).tar.Z)

tar.bz2: ../$(VERNAME).tar.bz2
../$(VERNAME).tar.bz2:
	(cd ..; tar cvf - $(VERNAME) | bzip2 -f9 > $(VERNAME).tar.bz2)

uuencode: tar.gz
	uuencode ../$(VERNAME).tar.gz $(VERNAME).tar.gz > ../$(VERNAME).tgz.uu

# ------------------------------------------------------------------------

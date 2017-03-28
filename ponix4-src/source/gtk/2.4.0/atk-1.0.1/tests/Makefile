# Makefile.in generated automatically by automake 1.4-p5 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
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

prefix = /source/Xdialog-2.3.1
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

pkgdatadir = $(datadir)/atk
pkglibdir = $(libdir)/atk
pkgincludedir = $(includedir)/atk

top_builddir = ..

ACLOCAL = aclocal 
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = 
host_triplet = i686-pc-linux-gnu
AS = @AS@
ATK_API_VERSION = 1.0
ATK_BINARY_AGE = 1
ATK_INTERFACE_AGE = 1
ATK_MAJOR_VERSION = 1
ATK_MICRO_VERSION = 1
ATK_MINOR_VERSION = 0
ATK_VERSION = 1.0.1
AWK = gawk
CC = i486-TSOL-linux-gnu-gcc
DLLTOOL = @DLLTOOL@
ECHO = echo
EXEEXT = 
GLIB_CFLAGS = -I/source/Xdialog-2.3.1/include/glib-2.0 -I/source/Xdialog-2.3.1/lib/glib-2.0/include  
GLIB_GENMARSHAL = glib-genmarshal
GLIB_LIBS = -Wl,--export-dynamic -L/source/Xdialog-2.3.1/lib -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0  
GLIB_MKENUMS = glib-mkenums
GLIB_PACKAGES = gobject-2.0 gmodule-2.0
GLIB_REQUIRED_VERSION = 2.0.0
GOBJECT_QUERY = gobject-query
GTKDOC = false
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
LT_CURRENT_MINUS_AGE = 0
LT_VERSION_INFO = 0:1:0
MAKEINFO = /source/gtk/2.4.0/atk-1.0.1/missing makeinfo
OBJDUMP = @OBJDUMP@
OBJEXT = o
PACKAGE = atk
PERL = perl
PKG_CONFIG = /usr/bin/pkg-config
RANLIB = ranlib
REBUILD = 
STRIP = strip
VERSION = 1.0.1
ms_librarian = 

noinst_LTLIBRARIES = \
	libteststateset.la	\
	libtestrelation.la


libatk = $(top_builddir)/atk/libatk-$(ATK_API_VERSION).la

INCLUDES = -I$(top_srcdir)	\
	-DG_DISABLE_DEPRECATED	\
	-D_REENTRANT -I/source/Xdialog-2.3.1/include/glib-2.0 -I/source/Xdialog-2.3.1/lib/glib-2.0/include  

DEPS = \
	$(libatk)


libteststateset_la_SOURCES = teststateset.c
libteststateset_la_LDFLAGS = -avoid-version	\
	-module


libtestrelation_la_SOURCES = testrelation.c
libtestrelation_la_LDFLAGS = -avoid-version	\
	-module

mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(noinst_LTLIBRARIES)


DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE=\"atk\" -DVERSION=\"1.0.1\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1  -I. -I$(srcdir) 
CPPFLAGS = 
LDFLAGS = -static-libgcc -L/source/Xdialog-2.3.1/lib
LIBS = 
libteststateset_la_LIBADD = 
libteststateset_la_OBJECTS =  teststateset.lo
libtestrelation_la_LIBADD = 
libtestrelation_la_OBJECTS =  testrelation.lo
CFLAGS = -Os -I/source/Xdialog-2.3.1/include -Wall
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
DIST_COMMON =  README Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best
SOURCES = $(libteststateset_la_SOURCES) $(libtestrelation_la_SOURCES)
OBJECTS = $(libteststateset_la_OBJECTS) $(libtestrelation_la_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .lo .o .obj .s
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps tests/Makefile

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

# FIXME: We should only use cygpath when building on Windows,
# and only if it is available.
.c.obj:
	$(COMPILE) -c `cygpath -w $<`

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core
	-rm -f *.$(OBJEXT)

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

libteststateset.la: $(libteststateset_la_OBJECTS) $(libteststateset_la_DEPENDENCIES)
	$(LINK)  $(libteststateset_la_LDFLAGS) $(libteststateset_la_OBJECTS) $(libteststateset_la_LIBADD) $(LIBS)

libtestrelation.la: $(libtestrelation_la_OBJECTS) $(libtestrelation_la_DEPENDENCIES)
	$(LINK)  $(libtestrelation_la_LDFLAGS) $(libtestrelation_la_OBJECTS) $(libtestrelation_la_LIBADD) $(LIBS)

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

subdir = tests

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
testrelation.lo testrelation.o : testrelation.c ../atk/atk.h \
	../atk/atkobject.h ../atk/atkstate.h ../atk/atkaction.h \
	../atk/atkcomponent.h ../atk/atkutil.h ../atk/atkdocument.h \
	../atk/atkeditabletext.h ../atk/atktext.h \
	../atk/atkgobjectaccessible.h ../atk/atkhyperlink.h \
	../atk/atkhypertext.h ../atk/atkimage.h \
	../atk/atkobjectfactory.h ../atk/atkregistry.h \
	../atk/atkrelation.h ../atk/atkrelationset.h \
	../atk/atkselection.h ../atk/atkstateset.h \
	../atk/atkstreamablecontent.h ../atk/atktable.h \
	../atk/atkvalue.h
teststateset.lo teststateset.o : teststateset.c ../atk/atk.h \
	../atk/atkobject.h ../atk/atkstate.h ../atk/atkaction.h \
	../atk/atkcomponent.h ../atk/atkutil.h ../atk/atkdocument.h \
	../atk/atkeditabletext.h ../atk/atktext.h \
	../atk/atkgobjectaccessible.h ../atk/atkhyperlink.h \
	../atk/atkhypertext.h ../atk/atkimage.h \
	../atk/atkobjectfactory.h ../atk/atkregistry.h \
	../atk/atkrelation.h ../atk/atkrelationset.h \
	../atk/atkselection.h ../atk/atkstateset.h \
	../atk/atkstreamablecontent.h ../atk/atktable.h \
	../atk/atkvalue.h

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
all-am: Makefile $(LTLIBRARIES)
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
		mostlyclean-libtool mostlyclean-tags \
		mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-noinstLTLIBRARIES clean-compile clean-libtool \
		clean-tags clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-noinstLTLIBRARIES distclean-compile \
		distclean-libtool distclean-tags distclean-generic \
		clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-noinstLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-generic \
		distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-noinstLTLIBRARIES distclean-noinstLTLIBRARIES \
clean-noinstLTLIBRARIES maintainer-clean-noinstLTLIBRARIES \
mostlyclean-compile distclean-compile clean-compile \
maintainer-clean-compile mostlyclean-libtool distclean-libtool \
clean-libtool maintainer-clean-libtool tags mostlyclean-tags \
distclean-tags clean-tags maintainer-clean-tags distdir info-am info \
dvi-am dvi check check-am installcheck-am installcheck install-exec-am \
install-exec install-data-am install-data install-am install \
uninstall-am uninstall all-redirect all-am all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:

# Makefile.in generated automatically by automake 1.4-p5 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

# We set GPATH here; this gives us semantics for GNU make
# which are more like other make's VPATH.


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

GPATH = $(srcdir)

EXTRA_DIST = atk.def atkmarshal.list
#export_symbols = #-export-symbols atk.def
#no_undefined = #-no-undefined

lib_LTLIBRARIES = libatk-1.0.la

#noinst_DATA = #atk-$(ATK_API_VERSION).lib

atk_built_headers = atk-enum-types.h
atk_built_cfiles = atk-enum-types.c

libatk_1_0_la_SOURCES = \
	atkaction.c		\
	atkcomponent.c		\
	atkdocument.c		\
	atkeditabletext.c	\
	atkgobjectaccessible.c	\
	atkhyperlink.c		\
	atkhypertext.c		\
	atkimage.c		\
	atknoopobject.c		\
	atknoopobjectfactory.c	\
	atkobject.c		\
	atkobjectfactory.c	\
	atkregistry.c		\
	atkrelation.c		\
	atkrelationset.c	\
	atkselection.c		\
	atkstate.c		\
	atkstateset.c		\
	atkstreamablecontent.c	\
	atktable.c		\
	atktext.c		\
	atkutil.c		\
	atkvalue.c		\
	$(atk_built_cfiles)

MAINTAINERCLEANFILES =  \
	atkmarshal.c		\
	atkmarshal.h		\
	stamp-atkmarshal.c	\
	stamp-atkmarshal.h


INCLUDES = \
	-I$(top_srcdir)		\
	-DG_DISABLE_DEPRECATED	\
	-D_REENTRANT -I/source/Xdialog-2.3.1/include/glib-2.0 -I/source/Xdialog-2.3.1/lib/glib-2.0/include  


LDFLAGS = \
	-version-info $(LT_VERSION_INFO)	\
	$(export_symbols)			\
	$(no_undefined)				\
	-Wl,--export-dynamic -L/source/Xdialog-2.3.1/lib -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0  


libatkincludedir = $(includedir)/atk-1.0/atk

atk_headers = \
        atk.h			\
        atkaction.h		\
        atkcomponent.h		\
        atkdocument.h		\
        atkeditabletext.h	\
        atkgobjectaccessible.h	\
        atkhyperlink.h		\
        atkhypertext.h		\
        atknoopobject.h		\
        atknoopobjectfactory.h	\
        atkobject.h		\
	atkobjectfactory.h	\
        atkimage.h		\
        atkregistry.h		\
        atkrelation.h		\
        atkrelationset.h	\
        atkselection.h		\
        atkstate.h		\
        atkstateset.h		\
        atkstreamablecontent.h	\
        atktable.h		\
        atktext.h		\
        atkutil.h		\
        atkvalue.h


libatkinclude_HEADERS = \
	$(atk_headers)		\
	$(atk_built_headers)

mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_CLEAN_FILES = 
LTLIBRARIES =  $(lib_LTLIBRARIES)


DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE=\"atk\" -DVERSION=\"1.0.1\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1  -I. -I$(srcdir) 
CPPFLAGS = 
LIBS = 
libatk_1_0_la_LDFLAGS = 
libatk_1_0_la_LIBADD = 
libatk_1_0_la_OBJECTS =  atkaction.lo atkcomponent.lo atkdocument.lo \
atkeditabletext.lo atkgobjectaccessible.lo atkhyperlink.lo \
atkhypertext.lo atkimage.lo atknoopobject.lo atknoopobjectfactory.lo \
atkobject.lo atkobjectfactory.lo atkregistry.lo atkrelation.lo \
atkrelationset.lo atkselection.lo atkstate.lo atkstateset.lo \
atkstreamablecontent.lo atktable.lo atktext.lo atkutil.lo atkvalue.lo \
atk-enum-types.lo
CFLAGS = -Os -I/source/Xdialog-2.3.1/include -Wall
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
DATA =  $(noinst_DATA)

HEADERS =  $(libatkinclude_HEADERS)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best
SOURCES = $(libatk_1_0_la_SOURCES)
OBJECTS = $(libatk_1_0_la_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .lo .o .obj .s
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu --include-deps atk/Makefile

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
	    echo "$(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(LIBTOOL)  --mode=install $(INSTALL) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	list='$(lib_LTLIBRARIES)'; for p in $$list; do \
	  $(LIBTOOL)  --mode=uninstall rm -f $(DESTDIR)$(libdir)/$$p; \
	done

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

libatk-1.0.la: $(libatk_1_0_la_OBJECTS) $(libatk_1_0_la_DEPENDENCIES)
	$(LINK) -rpath $(libdir) $(libatk_1_0_la_LDFLAGS) $(libatk_1_0_la_OBJECTS) $(libatk_1_0_la_LIBADD) $(LIBS)

install-libatkincludeHEADERS: $(libatkinclude_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libatkincludedir)
	@list='$(libatkinclude_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; \
	  echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libatkincludedir)/$$p"; \
	  $(INSTALL_DATA) $$d$$p $(DESTDIR)$(libatkincludedir)/$$p; \
	done

uninstall-libatkincludeHEADERS:
	@$(NORMAL_UNINSTALL)
	list='$(libatkinclude_HEADERS)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(libatkincludedir)/$$p; \
	done

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

subdir = atk

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
atk-enum-types.lo atk-enum-types.o : atk-enum-types.c atk.h atkobject.h \
	atkstate.h atkaction.h atkcomponent.h atkutil.h atkdocument.h \
	atkeditabletext.h atktext.h atkgobjectaccessible.h atk.h \
	atkhyperlink.h atkhypertext.h atkimage.h atkobjectfactory.h \
	atkregistry.h atkrelation.h atkrelationset.h atkselection.h \
	atkstateset.h atkstreamablecontent.h atktable.h atkvalue.h
atkaction.lo atkaction.o : atkaction.c atkaction.h atkobject.h \
	atkstate.h
atkcomponent.lo atkcomponent.o : atkcomponent.c atkcomponent.h \
	atkobject.h atkstate.h atkutil.h
atkdocument.lo atkdocument.o : atkdocument.c atkdocument.h atkobject.h \
	atkstate.h atkutil.h
atkeditabletext.lo atkeditabletext.o : atkeditabletext.c \
	atkeditabletext.h atkobject.h atkstate.h atktext.h atkutil.h
atkgobjectaccessible.lo atkgobjectaccessible.o : atkgobjectaccessible.c \
	atkgobjectaccessible.h atk.h atkobject.h atkstate.h atkaction.h \
	atkcomponent.h atkutil.h atkdocument.h atkeditabletext.h \
	atktext.h atkhyperlink.h atkhypertext.h atkimage.h \
	atkobjectfactory.h atkregistry.h atkrelation.h atkrelationset.h \
	atkselection.h atkstateset.h atkstreamablecontent.h atktable.h \
	atkvalue.h
atkhyperlink.lo atkhyperlink.o : atkhyperlink.c atkhyperlink.h \
	atkaction.h atkobject.h atkstate.h
atkhypertext.lo atkhypertext.o : atkhypertext.c atkhypertext.h \
	atkobject.h atkstate.h atkhyperlink.h atkaction.h
atkimage.lo atkimage.o : atkimage.c atkimage.h atkobject.h atkstate.h \
	atkutil.h
atknoopobject.lo atknoopobject.o : atknoopobject.c atk.h atkobject.h \
	atkstate.h atkaction.h atkcomponent.h atkutil.h atkdocument.h \
	atkeditabletext.h atktext.h atkgobjectaccessible.h atk.h \
	atkhyperlink.h atkhypertext.h atkimage.h atkobjectfactory.h \
	atkregistry.h atkrelation.h atkrelationset.h atkselection.h \
	atkstateset.h atkstreamablecontent.h atktable.h atkvalue.h \
	atknoopobject.h
atknoopobjectfactory.lo atknoopobjectfactory.o : atknoopobjectfactory.c \
	atkobject.h atkstate.h atknoopobject.h atknoopobjectfactory.h \
	atkobjectfactory.h atkobject.h
atkobject.lo atkobject.o : atkobject.c atk.h atkobject.h atkstate.h \
	atkaction.h atkcomponent.h atkutil.h atkdocument.h \
	atkeditabletext.h atktext.h atkgobjectaccessible.h atk.h \
	atkhyperlink.h atkhypertext.h atkimage.h atkobjectfactory.h \
	atkregistry.h atkrelation.h atkrelationset.h atkselection.h \
	atkstateset.h atkstreamablecontent.h atktable.h atkvalue.h \
	atkmarshal.h atk-enum-types.h
atkobjectfactory.lo atkobjectfactory.o : atkobjectfactory.c \
	atkobjectfactory.h atkobject.h atkstate.h \
	atknoopobjectfactory.h atkobjectfactory.h
atkregistry.lo atkregistry.o : atkregistry.c atkregistry.h \
	atkobjectfactory.h atkobject.h atkstate.h \
	atknoopobjectfactory.h atkobjectfactory.h
atkrelation.lo atkrelation.o : atkrelation.c atkobject.h atkstate.h \
	atkrelation.h atk-enum-types.h
atkrelationset.lo atkrelationset.o : atkrelationset.c atk.h atkobject.h \
	atkstate.h atkaction.h atkcomponent.h atkutil.h atkdocument.h \
	atkeditabletext.h atktext.h atkgobjectaccessible.h atk.h \
	atkhyperlink.h atkhypertext.h atkimage.h atkobjectfactory.h \
	atkregistry.h atkrelation.h atkrelationset.h atkselection.h \
	atkstateset.h atkstreamablecontent.h atktable.h atkvalue.h
atkselection.lo atkselection.o : atkselection.c atkselection.h \
	atkobject.h atkstate.h
atkstate.lo atkstate.o : atkstate.c atkstate.h atk-enum-types.h
atkstateset.lo atkstateset.o : atkstateset.c atkobject.h atkstate.h \
	atkstateset.h atkobject.h
atkstreamablecontent.lo atkstreamablecontent.o : atkstreamablecontent.c \
	atkstreamablecontent.h atkobject.h atkstate.h
atktable.lo atktable.o : atktable.c atktable.h atkobject.h atkstate.h \
	atkmarshal.h
atktext.lo atktext.o : atktext.c atktext.h atkobject.h atkstate.h \
	atkutil.h atkmarshal.h atk-enum-types.h
atkutil.lo atkutil.o : atkutil.c atkutil.h atkobject.h atkstate.h \
	atkmarshal.c
atkvalue.lo atkvalue.o : atkvalue.c atkvalue.h atkobject.h atkstate.h

info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am: install-libLTLIBRARIES
install-exec: install-exec-am

install-data-am: install-libatkincludeHEADERS install-data-local
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-libLTLIBRARIES uninstall-libatkincludeHEADERS \
		uninstall-local
uninstall: uninstall-am
all-am: Makefile $(LTLIBRARIES) $(DATA) $(HEADERS)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(libdir) $(DESTDIR)$(libatkincludedir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
	-test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
mostlyclean-am:  mostlyclean-libLTLIBRARIES mostlyclean-compile \
		mostlyclean-libtool mostlyclean-tags \
		mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-libLTLIBRARIES clean-compile clean-libtool clean-tags \
		clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-libLTLIBRARIES distclean-compile \
		distclean-libtool distclean-tags distclean-generic \
		clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-libLTLIBRARIES \
		maintainer-clean-compile maintainer-clean-libtool \
		maintainer-clean-tags maintainer-clean-generic \
		distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: mostlyclean-libLTLIBRARIES distclean-libLTLIBRARIES \
clean-libLTLIBRARIES maintainer-clean-libLTLIBRARIES \
uninstall-libLTLIBRARIES install-libLTLIBRARIES mostlyclean-compile \
distclean-compile clean-compile maintainer-clean-compile \
mostlyclean-libtool distclean-libtool clean-libtool \
maintainer-clean-libtool uninstall-libatkincludeHEADERS \
install-libatkincludeHEADERS tags mostlyclean-tags distclean-tags \
clean-tags maintainer-clean-tags distdir info-am info dvi-am dvi check \
check-am installcheck-am installcheck install-exec-am install-exec \
install-data-local install-data-am install-data install-am install \
uninstall-local uninstall-am uninstall all-redirect all-am all \
installdirs mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


#install-libtool-import-lib:
#	$(INSTALL) .libs/libatk-$(ATK_API_VERSION).dll.a $(DESTDIR)$(libdir)
#uninstall-libtool-import-lib:
#	-rm $(DESTDIR)$(libdir)/libatk-$(ATK_API_VERSION).dll.a
install-libtool-import-lib:
uninstall-libtool-import-lib:

#install-ms-lib:
#	$(INSTALL) atk-$(ATK_API_VERSION).lib $(DESTDIR)$(libdir)

#uninstall-ms-lib:
#	-rm $(DESTDIR)$(libdir)/atk-$(ATK_API_VERSION).lib
install-ms-lib:
uninstall-ms-lib:

$(OBJECTS): $(atk_built_headers)

$(libatk_1_0_la_OBJECTS): atkmarshal.c atkmarshal.h $(atk_built_headers)

atkmarshal.h:  stamp-atkmarshal.h
	@true
stamp-atkmarshal.h: atkmarshal.list
	case glib-genmarshal in									\
	   .*) glib_genmarshal=`pwd`/glib-genmarshal ;;				\
	   *) glib_genmarshal=glib-genmarshal ;;							\
	esac;												\
	$$glib_genmarshal --prefix=atk_marshal $(srcdir)/atkmarshal.list --header >> xgen-gmh		\
	&& (cmp -s xgen-gmh $(srcdir)/atkmarshal.h || cp xgen-gmh $(srcdir)/atkmarshal.h)		\
	&& rm -f xgen-gmh xgen-gmh~									\
	&& echo timestamp > $(@F)

atkmarshal.c:  stamp-atkmarshal.c
	@true
stamp-atkmarshal.c: atkmarshal.list
	case glib-genmarshal in									\
	   .*) glib_genmarshal=`pwd`/glib-genmarshal ;;				\
	   *) glib_genmarshal=glib-genmarshal ;;							\
	esac;												\
	$$glib_genmarshal --prefix=atk_marshal $(srcdir)/atkmarshal.list --body >> xgen-gmc		\
	&& (cmp -s xgen-gmc $(srcdir)/atkmarshal.c || cp xgen-gmc $(srcdir)/atkmarshal.c)		\
	&& rm -f xgen-gmc xgen-gmc~									\
	&& echo timestamp > $(@F)

atk-enum-types.h: s-enum-types-h
	@true

s-enum-types-h:  $(atk_headers) Makefile
	( cd $(srcdir) && glib-mkenums \
			--fhead "#ifndef __ATK_ENUM_TYPES_H__\n#define __ATK_ENUM_TYPES_H__\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type (void);\n#define ATK_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n"         \
			--ftail "G_END_DECLS\n\n#endif /* __ATK_ENUM_TYPES_H__ */" \
		$(atk_headers) ) > tmp-atk-enum-types.h \
	&& (cmp -s tmp-atk-enum-types.h $(srcdir)/atk-enum-types.h || cp tmp-atk-enum-types.h $(srcdir)/atk-enum-types.h ) \
	&& rm -f tmp-atk-enum-types.h \
	&& echo timestamp > $(@F)

atk-enum-types.c: s-enum-types-c
	@true

s-enum-types-c:  $(atk_headers) Makefile
	( cd $(srcdir) && glib-mkenums \
			--fhead "#include <atk.h>" \
			--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {"     \
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(atk_headers) ) > tmp-atk-enum-types.c \
	&& (cmp -s tmp-atk-enum-types.c $(srcdir)/atk-enum-types.c || cp tmp-atk-enum-types.c $(srcdir)/atk-enum-types.c ) \
	&& rm -f tmp-atk-enum-types.c \
	&& echo timestamp > $(@F)

atk-$(ATK_API_VERSION).lib: libatk-$(ATK_API_VERSION).la atk.def
	lib -name:libatk-$(ATK_API_VERSION)-0.dll -def:atk.def -out:$@

install-data-local: install-ms-lib install-libtool-import-lib

uninstall-local: uninstall-ms-lib uninstall-libtool-import-lib

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:

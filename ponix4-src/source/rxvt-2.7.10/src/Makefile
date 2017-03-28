# $Id: Makefile.in,v 1.58 2001/11/30 02:50:52 gcw Exp $

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

LINT = lint -DNARROWPROTO=1 $(XINC) -chapbxz

srcdir =	.

.PATH:		.

top_builddir = ..
basedir = ..
thisdir = src
MKDIR = ../autoconf/mkinstalldirs

# for developers: the following debug options may be used
#	-DDEBUG_CMD -DDEBUG_MAIN -DDEBUG_MENU -DDEBUG_MENUARROWS
#	-DDEBUG_MENUBAR_STACKING -DDEBUG_MENU_LAYOUT -DDEBUG_RESOURCES
#	-DDEBUG_SCREEN -DDEBUG_SEARCH_PATH -DDEBUG_SELECT -DDEBUG_TTY
#	-DDEBUG_TTYMODE -DDEBUG_X
DEBUG=-DDEBUG_STRICT 

first_rule: all
dummy:

LIBSRCS = command.c defaultfont.c graphics.c grkelot.c init.c logging.c \
	main.c menubar.c misc.c netdisp.c ptytty.c screen.c scrollbar.c \
	scrollbar-rxvt.c scrollbar-next.c scrollbar-xterm.c strings.c \
	xdefaults.c xpm.c

SRCS =  rxvt.c $(LIBSRCS)

HDRS =	command.h defaultfont.h feature.h grkelot.h init.h logging.h \
	menubar.h netdisp.h protos.h rxvt.h rxvtgrx.h version.h

EXTRAHDRS = rxvtlib.h

OBJS =	command.o defaultfont.o init.o    main.o  misc.o netdisp.o ptytty.o screen.o scrollbar.o   xdefaults.o  rxvt.o
LIBOBJS = command.lo defaultfont.lo init.lo main.lo misc.lo netdisp.lo ptytty.lo screen.lo scrollbar.lo xdefaults.lo
LIBVERSION = 1:0:0
INSTALL_LIBRXVT = 

RXVT_BASENAME=`$(ECHO) $(RXVTNAME)|$(SED) 's/$(EXEEXT)$$//'|$(SED) '$(transform)'`
RXVT_BINNAME=$(DESTDIR)$(bindir)/$(RXVT_BASENAME)$(EXEEXT)
RXVT_VERNAME=$(DESTDIR)$(bindir)/$(RXVT_BASENAME)-$(VERSION)$(EXEEXT)
RXVT_OLDNAME=$(DESTDIR)$(bindir)/$(RXVT_BASENAME)-old$(EXEEXT)

EXTPROS = command.extpro defaultfont.extpro graphics.extpro grkelot.extpro \
	init.extpro logging.extpro main.extpro menubar.extpro misc.extpro \
	netdisp.extpro ptytty.extpro screen.extpro scrollbar.extpro \
	scrollbar-rxvt.extpro scrollbar-next.extpro scrollbar-xterm.extpro \
	strings.extpro xdefaults.extpro xpm.extpro

INTPROS = command.intpro defaultfont.intpro graphics.intpro grkelot.intpro \
	init.intpro logging.intpro main.intpro menubar.intpro misc.intpro \
	netdisp.intpro ptytty.intpro screen.intpro scrollbar.intpro \
	scrollbar-rxvt.intpro scrollbar-next.intpro scrollbar-xterm.intpro \
	strings.intpro xdefaults.intpro xpm.intpro

DEPS =  rxvt.h rxvtlib.h ${basedir}/config.h feature.h .protos

#
# Distribution variables
#

DIST = $(HDRS) $(SRCS) Makefile.in gcc-Wall rxvtlib.h.in .indent.pro \
	makeintprotos-awk makeextprotos-awk $(INTPROS) $(EXTPROS) .protos

.SUFFIXES:	.c .o .extpro .intpro .lo

#-------------------------------------------------------------------------
# inference rules
.c.o:
	$(COMPILE) -c $<

.c.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.s.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.S.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.c.intpro:
	@$(RMF) $@.tmp
	@$(AWK) -f $(srcdir)/makeintprotos-awk $< > $@.tmp
	@if $(CMP) -s $@ $@.tmp ; then : ; else $(ECHO) "$(AWK) -f $(srcdir)/makeintprotos-awk $< > $@"; $(CP) $@.tmp $@; fi
	@$(RMF) $@.tmp

.c.extpro:
	@$(RMF) $@.tmp
	@$(AWK) -f $(srcdir)/makeextprotos-awk $< > $@.tmp
	@if $(CMP) -s $@ $@.tmp ; then : ; else $(ECHO) "$(AWK) -f $(srcdir)/makeextprotos-awk $< > $@"; $(CP) $@.tmp $@; fi
	@$(RMF) $@.tmp
#-------------------------------------------------------------------------
all: allbin

rxvt: version.h rxvt.o librxvt.la
	$(LIBTOOL) --mode=link $(LINK) rxvt.o librxvt.la $(LIBS) $(XLIB) $(DLIB) -o $@

.protos: $(EXTPROS)
	@$(RMF) .protos
	date >.protos

librxvt.la: $(LIBOBJS)
	$(LIBTOOL) --mode=link $(LINK) -rpath $(libdir) -version-info $(LIBVERSION) $(LIBOBJS) $(LIBS) -o $@
#-------------------------------------------------------------------------
tags: $(SRCS) $(HDRS) $(EXTRAHDRS)
	ctags $(SRCS) $(HDRS) $(EXTRAHDRS)

allbin: .protos rxvt

alldoc:

clean:
	$(RMF) rxvt core a.out *.o *.lo *.bak *~ *.intpro *.extpro .libs/* librxvt.la tmpproto .protos *.tmp

realclean: clean
	$(RMF) tags librxvt.h

cleandir: realclean

distclean: realclean
	if test $(srcdir) = .; then $(MAKE) realclean; fi
	(cd $(srcdir); $(RMF) Makefile)

install: allbin alldoc
	$(MKDIR) $(DESTDIR)$(includedir) $(DESTDIR)$(libdir) $(DESTDIR)$(bindir)
	@if test x$(INSTALL_LIBRXVT) = xyes; then \
	    $(ECHO) "$(LIBTOOL) --mode=install $(INSTALL_DATA) rxvtlib.h $(DESTDIR)$(includedir)/rxvtlib.h"; \
	    $(LIBTOOL) --mode=install $(INSTALL_DATA) rxvtlib.h $(DESTDIR)$(includedir)/rxvtlib.h; \
	    $(ECHO) "$(LIBTOOL) --mode=install $(INSTALL_PROGRAM) librxvt.la $(DESTDIR)$(libdir)/librxvt.la"; \
	    $(LIBTOOL) --mode=install $(INSTALL_PROGRAM) librxvt.la $(DESTDIR)$(libdir)/librxvt.la; \
	fi
	@if test -f $(RXVT_BINNAME); then \
	    $(ECHO) "$(RMF) $(RXVT_OLDNAME)"; \
	    $(RMF) $(RXVT_OLDNAME); \
	    $(ECHO) "$(MV) $(RXVT_BINNAME) $(RXVT_OLDNAME)"; \
	    $(MV) $(RXVT_BINNAME) $(RXVT_OLDNAME); \
	fi
	@$(ECHO) "$(LIBTOOL) --mode=install $(INSTALL_PROGRAM) rxvt $(RXVT_VERNAME)"
	@$(LIBTOOL) --mode=install $(INSTALL_PROGRAM) rxvt $(RXVT_VERNAME)
	@$(ECHO) "$(LN) $(RXVT_VERNAME) $(RXVT_BINNAME)"
	@$(LN) $(RXVT_VERNAME) $(RXVT_BINNAME)

uninstall:
	@$(ECHO) $(RMF) $(RXVT_VERNAME)
	@$(RMF) $(RXVT_VERNAME)
	@$(ECHO) "$(LIBTOOL) --mode=uninstall $(RMF) $(RXVT_BINNAME)"
	@$(LIBTOOL) --mode=uninstall $(RMF) $(RXVT_BINNAME)
	@if test x$(INSTALL_LIBRXVT) = xyes; then \
	    $(ECHO) "$(LIBTOOL) --mode=uninstall $(RMF) $(DESTDIR)$(libdir)/librxvt.la"; \
	    $(LIBTOOL) --mode=uninstall $(RMF) $(DESTDIR)$(libdir)/librxvt.la; \
	    $(ECHO) "$(LIBTOOL) --mode=uninstall $(RMF) $(DESTDIR)$(includedir)/rxvtlib.h"; \
	    $(LIBTOOL) --mode=uninstall $(RMF) $(DESTDIR)$(includedir)/rxvtlib.h; \
	fi

distdirs:
	mkdir $(basedir)/../$(VERNAME)/$(thisdir)

distcopy: .protos $(INTPROS)
	$(CP) -p $(DIST) $(basedir)/../$(VERNAME)/$(thisdir)

# -----------------------------------------------------------------------
# DO NOT DELETE: nice dependency list follows

command.o:          command.c   $(DEPS) command.intpro	command.h version.h
defaultfont.o:      defaultfont.c $(DEPS) defaultfont.intpro defaultfont.h
graphics.o:         graphics.c  $(DEPS) graphics.intpro
grkelot.o:          grkelot.c   $(DEPS) grkelot.intpro	grkelot.h
init.o:             init.c      $(DEPS) init.intpro	init.h	
logging.o:          logging.c   $(DEPS) logging.intpro	logging.h
main.o:             main.c      $(DEPS) main.intpro
menubar.o:          menubar.c   $(DEPS) menubar.intpro	menubar.h version.h
misc.o:             misc.c      $(DEPS) misc.intpro
netdisp.o:          netdisp.c   $(DEPS) netdisp.intpro	netdisp.h
ptytty.o:           ptytty.c    $(DEPS) ptytty.intpro
rxvt.o:             rxvt.c      $(DEPS) 
screen.o:           screen.c    $(DEPS) screen.intpro
scrollbar.o:        scrollbar.c $(DEPS) scrollbar.intpro
scrollbar-rxvt.o:   scrollbar-rxvt.c  $(DEPS) scrollbar-rxvt.intpro
scrollbar-next.o:   scrollbar-next.c  $(DEPS) scrollbar-next.intpro
scrollbar-xterm.o:  scrollbar-xterm.c $(DEPS) scrollbar-xterm.intpro
strings.o:          strings.c   $(DEPS) strings.intpro
xdefaults.o:        xdefaults.c $(DEPS) xdefaults.intpro	  version.h
xpm.o:              xpm.c       $(DEPS) xpm.intpro

command.lo:         command.c   $(DEPS) command.intpro	command.h version.h
defaultfont.lo:     defaultfont.c $(DEPS) defaultfont.intpro defaultfont.h
graphics.lo:        graphics.c  $(DEPS) graphics.intpro	
grkelot.lo:         grkelot.c   $(DEPS) grkelot.intpro	grkelot.h
init.lo:            init.c      $(DEPS) init.intpro	init.h	
logging.lo:         logging.c   $(DEPS) logging.intpro	logging.h
main.lo:            main.c      $(DEPS) main.intpro
menubar.lo:         menubar.c   $(DEPS) menubar.intpro	menubar.h version.h
misc.lo:            misc.c      $(DEPS) misc.intpro
netdisp.lo:         netdisp.c   $(DEPS) netdisp.intpro	netdisp.h
ptytty.lo:          ptytty.c    $(DEPS) ptytty.intpro
rxvt.lo:            rxvt.c      $(DEPS) 
screen.lo:          screen.c    $(DEPS) screen.intpro
scrollbar.lo:       scrollbar.c $(DEPS) scrollbar.intpro
scrollbar-rxvt.lo:  scrollbar-rxvt.c  $(DEPS) scrollbar-rxvt.intpro
scrollbar-next.lo:  scrollbar-next.c  $(DEPS) scrollbar-next.intpro
scrollbar-xterm.lo: scrollbar-xterm.c $(DEPS) scrollbar-xterm.intpro
strings.lo:         strings.c   $(DEPS) strings.intpro
xdefaults.lo:       xdefaults.c $(DEPS) xdefaults.intpro	  version.h
xpm.lo:             xpm.c       $(DEPS) xpm.intpro

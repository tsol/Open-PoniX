# aewm - a minimalist X11 window mananager. vim:ft=make:noet
# Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>
# This program is free software; see LICENSE for details.

# Uncomment to enable building with GTK+ 1.2 instead of 2.x.
#USE_OLD_GTK = 1

CC       = gcc
CFLAGS   = -g -O2 -Wall

AELIBDIR = ../lib
MYLIBDIR = lib
XROOT    = /usr/X11R6
BINDIR   = $(DESTDIR)$(XROOT)/bin
MANDIR   = $(DESTDIR)$(XROOT)/man/man1
CFGDIR   = $(DESTDIR)/etc/X11/aewm

INCLUDES = -I$(AELIBDIR) -I$(MYLIBDIR)
X11PATH  = -I$(XROOT)/include
X11LIBS  = -L$(XROOT)/lib -lX11
XAWLIBS  = -L$(XROOT)/lib -lX11 -lXt -lXaw
ifdef USE_OLD_GTK
GTKPATH  = `gtk-config --cflags` -DUSE_OLD_GTK=$(USE_OLD_GTK)
GTKLIBS  = `gtk-config --libs`
else
GTKPATH  = `pkg-config --cflags gtk+-2.0`
GTKLIBS  = `pkg-config --libs gtk+-2.0`
endif

MANPAGE  = aeclients.1x

# --------------------------------------------------------------------

XAWPROGS = aemenu-xaw aepanel-xaw
GTKPROGS = aemenu-gtk aepanel-gtk
X11PROGS = set-gnome-pda
ETCPROGS = aesession
ALLPROGS = $(GTKPROGS) $(XAWPROGS) $(X11PROGS) $(ETCPROGS)

all: $(ALLPROGS)

COMMONOBJS = $(AELIBDIR)/parser.o $(MYLIBDIR)/launch.o $(MYLIBDIR)/switch.o

aemenu-gtk: $(COMMONOBJS)
aemenu-xaw: $(COMMONOBJS) $(MYLIBDIR)/xaw-util.o
aepanel-gtk: $(COMMONOBJS) $(MYLIBDIR)/panel.o $(AELIBDIR)/gnome-pda.o
aepanel-xaw: $(COMMONOBJS) $(MYLIBDIR)/panel.o $(AELIBDIR)/gnome-pda.o $(MYLIBDIR)/xaw-util.o
set-gnome-pda: $(MYLIBDIR)/panel.o $(AELIBDIR)/gnome-pda.o

$(GTKPROGS): %: %.o
	$(CC) $^ $(GTKLIBS) -o $@

$(XAWPROGS): %: %.o
	$(CC) $^ $(XAWLIBS) -o $@

$(X11PROGS): %: %.o
	$(CC) $^ $(X11LIBS) -o $@

$(ETCPROGS): %: %.o
	$(CC) $^ -o $@

# --------------------------------------------------------------------

GTKOBJS = aemenu-gtk.o aepanel-gtk.o
XAWOBJS = aemenu-xaw.o aepanel-xaw.o $(MYLIBDIR)/xaw-util.o
X11OBJS = set-gnome-pda.o $(MYLIBDIR)/panel.o $(MYLIBDIR)/switch.o $(AELIBDIR)/gnome-pda.o
ETCOBJS = $(MYLIBDIR)/launch.o aesession.o
ALLOBJS = $(GTKOBJS) $(XAWOBJS) $(X11OBJS) $(ETCOBJS)

$(X11OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(X11PATH) -c $< -o $@

$(XAWOBJS): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(X11PATH) -c $< -o $@

$(GTKOBJS): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(GTKPATH) -c $< -o $@

$(ETCOBJS): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# --------------------------------------------------------------------

install: all
	install -s $(ALLPROGS) $(BINDIR)
	install -m 644 $(MANPAGE) $(MANDIR)
	gzip -9vf $(MANDIR)/$(MANPAGE)
	mkdir -p $(CFGDIR) && cp clientsrc.sample $(CFGDIR)/clientsrc
	cd $(BINDIR) && ln -sf aemenu-gtk aemenu
	cd $(BINDIR) && ln -sf aepanel-gtk aepanel
	cd $(MANDIR) && \
	    for i in $(ALLPROGS); do \
	        ln -sf $(MANPAGE).gz $$i.1x.gz; \
	    done \

clean:
	rm -f $(ALLPROGS) $(ALLOBJS)

.PHONY: all install clean

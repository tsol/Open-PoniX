# aewm - a minimalist X11 window mananager. vim:ft=make:noet
# Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>
# This program is free software; see LICENSE for details.

# This should be set to the location of the X installation against
# which you would like to compile.
XROOT = /usr/X11R6

# Comment out to remove shape support (for X11R5 or a smaller binary)
DEFINES += -DSHAPE
EXTRA_LIBS += -lXext

# Comment out to remove support for Gnome's GNOME_PANEL_DESKTOP_AREA
# hint, which allows aewm to avoid mapping windows over an aepanel.
# This does not link against or require Gnome to compile.
DEFINES += -DGNOME_PDA

# Uncomment to add MWM hints support. This requires Lesstif headers at
# build time, but does not link against Lesstif.
#DEFINES += -DMWM_HINTS

# Uncomment to add freetype support (requires XFree86 4.0.2 or later).
# This depends on -lXext above, even if you have disabled shape support.
#DEFINES += -DXFT
#EXTRA_INC += `pkg-config --cflags xft`
#EXTRA_LIBS += `pkg-config --libs xft`

# Uncomment for debugging info (abandon all hope, ye who enter here).
#DEFINES += -DDEBUG

# --------------------------------------------------------------------

CC       = gcc
CFLAGS   = -g -O2 -Wall

MYLIBDIR = ../lib
BINDIR   = $(DESTDIR)$(XROOT)/bin
MANDIR   = $(DESTDIR)$(XROOT)/man/man1
CFGDIR   = $(DESTDIR)/etc/X11/aewm
INCLUDES = -I$(MYLIBDIR) -I$(XROOT)/include $(EXTRA_INC)
LDPATH   = -L$(XROOT)/lib
LIBS     = -lX11 $(EXTRA_LIBS)

PROG     = aewm
MANPAGE  = aewm.1x
OBJS     = init.o events.o client.o new.o manage.o misc.o $(MYLIBDIR)/parser.o $(MYLIBDIR)/gnome-pda.o
HEADERS  = aewm.h

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(OBJS) $(LDPATH) $(LIBS) -o $@

$(OBJS): %.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

install: all
	install -s $(PROG) $(BINDIR)
	install -m 644 $(MANPAGE) $(MANDIR)
	gzip -9vf $(MANDIR)/$(MANPAGE)
	mkdir -p $(CFGDIR) && cp aewmrc.sample $(CFGDIR)/aewmrc

clean:
	rm -f $(PROG) $(OBJS)

.PHONY: all install clean

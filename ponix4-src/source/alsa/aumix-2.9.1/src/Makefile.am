# $Aumix: aumix/src/Makefile.am,v 1.5 2010/04/26 10:07:27 trevor Exp $
bin_PROGRAMS	= aumix
if CURSES
bin_SCRIPTS	= mute xaumix
else
bin_SCRIPTS	= mute
endif
aumix_SOURCES	= common.c curses.c dummy.c gpm-xterm.c gtk.c interactive.c \
		mouse.c common.h curses.h gpm-xterm.h gtk.h interactive.h  \
		mouse.h play.xpm record.xpm
localedir	= $(datadir)/locale
INCLUDES	= -I../intl -DLOCALEDIR=\"$(localedir)\" -I@includedir@
AM_CFLAGS	= @CFLAGS@ @GLIB_CFLAGS@ @GTK_CFLAGS@
LDADD		= -L@libdir@
LIBS		= @LIBS@ @GTK_LIBS@ @LIBINTL@
DEFS		= @DEFS@

curses.o: curses.c
	$(COMPILE) -DDATADIR=\"$(datadir)/aumix\" -c $<

bin_PROGRAMS	= aumix
bin_SCRIPTS	= xaumix
aumix_SOURCES	= common.c curses.c dummy.c gpm-xterm.c gtk.c interactive.c \
		mouse.c common.h curses.h gpm-xterm.h gtk.h interactive.h  \
		mouse.h play.xpm record.xpm
localedir	= $(datadir)/locale
INCLUDES	= -I../intl -DLOCALEDIR=\"$(localedir)\" -I@includedir@
CFLAGS		= @CFLAGS@ @GLIB_CFLAGS@ @GTK_CFLAGS@
LDADD		= -L@libdir@
LIBS		= @LIBS@ @GTK_LIBS@ @INTLLIBS@
DEFS		= @DEFS@

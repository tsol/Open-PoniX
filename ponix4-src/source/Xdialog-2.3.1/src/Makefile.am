## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign no-dependencies

localedir = $(datadir)/locale
INCLUDES = \
	-DLOCALEDIR=\"$(localedir)\" -I. -I../intl -I../lib \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@GTK_CFLAGS@

bin_PROGRAMS = Xdialog

Xdialog_SOURCES = \
	main.c \
	interface.c interface.h yes.xpm no.xpm help.xpm print.xpm next.xpm previous.xpm \
	callbacks.c callbacks.h \
	support.c support.h

LDADD = @LIBINTL@
Xdialog_LDADD = $(LDADD) @GTK_LIBS@ -L../lib -lgetopt

SUBDIRS = man
bin_PROGRAMS = xcursorgen

xcursorgen_SOURCES = xcursorgen.c

AM_CFLAGS = $(CWARNFLAGS) $(XCURSORGEN_CFLAGS)

xcursorgen_LDADD = $(XCURSORGEN_LIBS)

MAINTAINERCLEANFILES = ChangeLog INSTALL

.PHONY: ChangeLog INSTALL

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL

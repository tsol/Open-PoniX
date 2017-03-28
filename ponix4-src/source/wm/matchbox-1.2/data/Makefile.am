if ENABLE_SCHEMAS
  SUBDIRS = themes schemas
else
  SUBDIRS = themes
endif

confdir = $(sysconfdir)/matchbox

conf_DATA = kbdconfig


defaultsdir = $(datadir)/matchbox

if WANT_STANDALONE
  defaults_DATA =
else
  defaults_DATA = mbnoapp.xpm
endif

EXTRA_DIST = mbnoapp.xpm kbdconfig


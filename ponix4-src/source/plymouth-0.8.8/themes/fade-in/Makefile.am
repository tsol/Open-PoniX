themedir = $(datadir)/plymouth/themes/fade-in
nodist_theme_DATA = fade-in.plymouth
dist_theme_DATA = star.png bullet.png entry.png lock.png

MAINTAINERCLEANFILES = Makefile.in fade-in.plymouth
CLEANFILES = fade-in.plymouth

fade-in.plymouth: $(srcdir)/fade-in.plymouth.in
	sed -e 's,[@]PLYMOUTH_THEME_PATH[@],$(PLYMOUTH_THEME_PATH),g' \
	       $(srcdir)/fade-in.plymouth.in > fade-in.plymouth

EXTRA_DIST = fade-in.plymouth.in

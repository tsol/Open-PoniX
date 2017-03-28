themedir = $(datadir)/plymouth/themes/solar
nodist_theme_DATA = solar.plymouth
dist_theme_DATA =                                                     \
                    box.png                                           \
                    bullet.png                                        \
                    entry.png                                         \
                    lock.png                                          \
                    progress_bar.png                                  \
                    star.png



MAINTAINERCLEANFILES = Makefile.in solar.plymouth
CLEANFILES = solar.plymouth

solar.plymouth: $(srcdir)/solar.plymouth.in
	sed -e 's,[@]PLYMOUTH_THEME_PATH[@],$(PLYMOUTH_THEME_PATH),g' \
	       $(srcdir)/solar.plymouth.in > solar.plymouth

EXTRA_DIST = solar.plymouth.in

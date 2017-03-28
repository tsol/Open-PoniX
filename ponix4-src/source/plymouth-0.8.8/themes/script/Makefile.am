themedir = $(datadir)/plymouth/themes/script
nodist_theme_DATA = \
                    script.plymouth                                   

dist_theme_DATA =                                                     \
                    script.script                                     \
                    box.png                                           \
                    bullet.png                                        \
                    entry.png                                         \
                    lock.png                                          \
                    progress_box.png                                  \
                    progress_bar.png


MAINTAINERCLEANFILES = Makefile.in script.plymouth
CLEANFILES = script.plymouth

script.plymouth: $(srcdir)/script.plymouth.in
	sed -e 's,[@]PLYMOUTH_THEME_PATH[@],$(PLYMOUTH_THEME_PATH),g' \
	       $(srcdir)/script.plymouth.in > script.plymouth

EXTRA_DIST = script.plymouth.in

themedir = $(datadir)/plymouth/themes/glow
nodist_theme_DATA = glow.plymouth

dist_theme_DATA =                                                     \
                    box.png                                           \
                    bullet.png                                        \
                    entry.png                                         \
                    lock.png                                          \
                    progress-00.png                                   \
                    progress-01.png                                   \
                    progress-02.png                                   \
                    progress-03.png                                   \
                    progress-04.png                                   \
                    progress-05.png                                   \
                    progress-06.png                                   \
                    progress-07.png                                   \
                    progress-08.png                                   \
                    progress-09.png                                   \
                    progress-10.png                                   \
                    progress-11.png                                   \
                    progress-12.png                                   \
                    progress-13.png                                   \
                    progress-14.png                                   \
                    progress-15.png                                   \
                    progress-16.png                                   \
                    progress-17.png                                   \
                    progress-18.png                                   \
                    progress-19.png                                   \
                    progress-20.png                                   \
                    progress-21.png                                   \
                    progress-22.png                                   \
                    progress-23.png                                   \
                    progress-24.png                                   \
                    progress-25.png                                   \
                    progress-26.png                                   \
                    progress-27.png                                   \
                    progress-28.png                                   \
                    progress-29.png                                   \
                    progress-30.png                                   \
                    progress-31.png                                   \
                    progress-32.png                                   \
                    throbber-00.png                                   \
                    throbber-01.png                                   \
                    throbber-02.png                                   \
                    throbber-03.png                                   \
                    throbber-04.png                                   \
                    throbber-05.png                                   \
                    throbber-06.png                                   \
                    throbber-07.png                                   \
                    throbber-08.png                                   \
                    throbber-09.png                                   \
                    throbber-10.png                                   \
                    throbber-11.png                                   \
                    throbber-12.png                                   \
                    throbber-13.png                                   \
                    throbber-14.png                                   \
                    throbber-15.png                                   \
                    throbber-16.png                                   \
                    throbber-17.png                                   \
                    throbber-18.png                                   \
                    throbber-19.png

MAINTAINERCLEANFILES = Makefile.in glow.plymouth
CLEANFILES = glow.plymouth

glow.plymouth: $(srcdir)/glow.plymouth.in
	sed -e 's,[@]PLYMOUTH_THEME_PATH[@],$(PLYMOUTH_THEME_PATH),g' \
	       $(srcdir)/glow.plymouth.in > glow.plymouth

EXTRA_DIST = glow.plymouth.in

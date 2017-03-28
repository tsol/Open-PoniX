themedir = $(datadir)/plymouth/themes/spinfinity
nodist_theme_DATA = spinfinity.plymouth
dist_theme_DATA =                                                     \
                  box.png                                             \
                  bullet.png                                          \
                  entry.png                                           \
                  lock.png                                            \
                  throbber-00.png                                     \
                  throbber-01.png                                     \
                  throbber-02.png                                     \
                  throbber-03.png                                     \
                  throbber-04.png                                     \
                  throbber-05.png                                     \
                  throbber-06.png                                     \
                  throbber-07.png                                     \
                  throbber-08.png                                     \
                  throbber-09.png                                     \
                  throbber-10.png                                     \
                  throbber-11.png                                     \
                  throbber-12.png                                     \
                  throbber-13.png                                     \
                  throbber-14.png                                     \
                  throbber-15.png                                     \
                  throbber-16.png                                     \
                  throbber-17.png                                     \
                  throbber-18.png                                     \
                  throbber-19.png                                     \
                  throbber-20.png                                     \
                  throbber-21.png                                     \
                  throbber-22.png                                     \
                  throbber-23.png                                     \
                  throbber-24.png                                     \
                  throbber-25.png                                     \
                  throbber-26.png                                     \
                  throbber-27.png                                     \
                  throbber-28.png                                     \
                  throbber-29.png                                     \
                  throbber-30.png                                     \
                  throbber-31.png                                     \
                  throbber-32.png                                     \
                  throbber-33.png

MAINTAINERCLEANFILES = Makefile.in spinfinity.plymouth
CLEANFILES = spinfinity.plymouth

spinfinity.plymouth: $(srcdir)/spinfinity.plymouth.in
	sed -e 's,[@]PLYMOUTH_THEME_PATH[@],$(PLYMOUTH_THEME_PATH),g' \
	       $(srcdir)/spinfinity.plymouth.in > spinfinity.plymouth

EXTRA_DIST = spinfinity.plymouth.in


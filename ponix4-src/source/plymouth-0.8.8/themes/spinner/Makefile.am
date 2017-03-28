themedir = $(datadir)/plymouth/themes/spinner
nodist_theme_DATA = spinner.plymouth

dist_theme_DATA =                                                     \
                    box.png                                           \
                    bullet.png                                        \
                    entry.png                                         \
                    lock.png                                          \
                    animation-0001.png                                \
                    animation-0002.png                                \
                    animation-0003.png                                \
                    animation-0004.png                                \
                    animation-0005.png                                \
                    animation-0006.png                                \
                    animation-0007.png                                \
                    animation-0008.png                                \
                    animation-0009.png                                \
                    animation-0010.png                                \
                    animation-0011.png                                \
                    animation-0012.png                                \
                    animation-0013.png                                \
                    animation-0014.png                                \
                    animation-0015.png                                \
                    animation-0016.png                                \
                    animation-0017.png                                \
                    animation-0018.png                                \
                    animation-0019.png                                \
                    animation-0020.png                                \
                    animation-0021.png                                \
                    animation-0022.png                                \
                    animation-0023.png                                \
                    animation-0024.png                                \
                    animation-0025.png                                \
                    animation-0026.png                                \
                    animation-0027.png                                \
                    animation-0028.png                                \
                    animation-0029.png                                \
                    animation-0030.png                                \
                    animation-0031.png                                \
                    animation-0032.png                                \
                    animation-0033.png                                \
                    animation-0034.png                                \
                    animation-0035.png                                \
                    animation-0036.png                                \
                    throbber-0001.png                                 \
                    throbber-0002.png                                 \
                    throbber-0003.png                                 \
                    throbber-0004.png                                 \
                    throbber-0005.png                                 \
                    throbber-0006.png                                 \
                    throbber-0007.png                                 \
                    throbber-0008.png                                 \
                    throbber-0009.png                                 \
                    throbber-0010.png                                 \
                    throbber-0011.png                                 \
                    throbber-0012.png

MAINTAINERCLEANFILES = Makefile.in spinner.plymouth
CLEANFILES = spinner.plymouth

spinner.plymouth: $(srcdir)/spinner.plymouth.in
	sed -e 's,[@]PLYMOUTH_THEME_PATH[@],$(PLYMOUTH_THEME_PATH),g' \
	       $(srcdir)/spinner.plymouth.in > spinner.plymouth

EXTRA_DIST = spinner.plymouth.in

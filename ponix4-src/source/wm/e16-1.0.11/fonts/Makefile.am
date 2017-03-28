fontdir = $(pkgdatadir)/fonts

font_DATA = Vera.ttf VeraBd.ttf COPYRIGHT.Vera

EXTRA_DIST = $(font_DATA)

install-data-hook:
	cd $(DESTDIR)$(fontdir); rm -f normal.ttf; $(LN_S) Vera.ttf normal.ttf
	cd $(DESTDIR)$(fontdir); rm -f bold.ttf; $(LN_S) VeraBd.ttf bold.ttf

uninstall-local:
	cd $(DESTDIR)$(fontdir); rm -f *.ttf

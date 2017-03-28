## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign no-dependencies

docdir = $(datadir)/doc/$(PACKAGE)-$(VERSION)

CLEANFILES = index.html compatibility.html changelog.html license.html Xdialog.1

doc_DATA = index.html compatibility.html changelog.html license.html $(srcdir)/*.html $(srcdir)/*.png

man_MANS = Xdialog.1

index.html: index.html.in
	sed -e 's/%%version%%/$(VERSION)/' $(srcdir)/index.html.in >index.html

compatibility.html: compatibility.html.in
	sed -e 's/%%version%%/$(VERSION)/' \
	    -e 's:%%datadir%%:$(datadir):' \
	    $(srcdir)/compatibility.html.in >compatibility.html

changelog.html: changelog.head $(top_srcdir)/ChangeLog changelog.tail
	cat $(srcdir)/changelog.head >changelog.html
	sed -e 's/</\&lt;/' -e 's/</\&lt;/' -e 's/</\&lt;/' \
	    $(top_srcdir)/ChangeLog >>changelog.html
	cat $(srcdir)/changelog.tail >>changelog.html

license.html: license.head $(top_srcdir)/COPYING license.tail
	cat $(srcdir)/license.head $(top_srcdir)/COPYING $(srcdir)/license.tail >license.html

Xdialog.1: Xdialog.man
	sed -e 's/%%version%%/$(VERSION)/' \
	    -e 's:%%bindir%%:$(bindir):' \
	    -e 's:%%mandir%%:$(mandir):' \
	    -e 's:%%datadir%%:$(datadir):' $(srcdir)/Xdialog.man >Xdialog.1

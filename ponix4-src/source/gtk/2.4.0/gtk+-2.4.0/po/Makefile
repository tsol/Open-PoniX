# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
#
# This file file be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.
#
# - Modified by Owen Taylor <otaylor@redhat.com> to use GETTEXT_PACKAGE
#   instead of PACKAGE and to look for po2tbl in ./ not in intl/
#
# - Modified by jacob berkman <jacob@ximian.com> to install
#   Makefile.in.in and po2tbl.sed.in for use with glib-gettextize

GETTEXT_PACKAGE = gtk20
PACKAGE = gtk+
VERSION = 2.4.0

SHELL = /bin/sh


srcdir = .
top_srcdir = ..


prefix = /source/Xdialog-2.3.1
exec_prefix = ${prefix}
datadir = ${prefix}/share
libdir = ${exec_prefix}/lib
localedir = $(libdir)/locale
gnulocaledir = $(datadir)/locale
gettextsrcdir = $(datadir)/glib-2.0/gettext/po
subdir = po

INSTALL = /bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = $(top_srcdir)/./mkinstalldirs

CC = i486-TSOL-linux-gnu-gcc
GENCAT = @GENCAT@
GMSGFMT = /usr/bin/msgfmt
MSGFMT = /usr/bin/msgfmt
XGETTEXT = /usr/bin/xgettext
MSGMERGE = msgmerge

DEFS = -DHAVE_CONFIG_H
CFLAGS = -Os -I/source/Xdialog-2.3.1/include -Wall
CPPFLAGS = 

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

SOURCES = 
POFILES =  af.po am.po ar.po az.po be.po bg.po bn.po br.po ca.po cs.po cy.po da.po de.po el.po en_CA.po en_GB.po es.po et.po eu.po fa.po fi.po fr.po ga.po gl.po he.po hi.po hr.po hu.po ia.po id.po it.po ja.po ko.po li.po lt.po lv.po mi.po mk.po ml.po mn.po ms.po ne.po nl.po nn.po no.po pl.po pt.po pt_BR.po ro.po ru.po sk.po sl.po sq.po sr.po sr@ije.po sr@Latn.po sv.po ta.po th.po tr.po uk.po uz.po uz@Latn.po vi.po wa.po yi.po zh_CN.po zh_TW.po
GMOFILES =  af.gmo am.gmo ar.gmo az.gmo be.gmo bg.gmo bn.gmo br.gmo ca.gmo cs.gmo cy.gmo da.gmo de.gmo el.gmo en_CA.gmo en_GB.gmo es.gmo et.gmo eu.gmo fa.gmo fi.gmo fr.gmo ga.gmo gl.gmo he.gmo hi.gmo hr.gmo hu.gmo ia.gmo id.gmo it.gmo ja.gmo ko.gmo li.gmo lt.gmo lv.gmo mi.gmo mk.gmo ml.gmo mn.gmo ms.gmo ne.gmo nl.gmo nn.gmo no.gmo pl.gmo pt.gmo pt_BR.gmo ro.gmo ru.gmo sk.gmo sl.gmo sq.gmo sr.gmo sr@ije.gmo sr@Latn.gmo sv.gmo ta.gmo th.gmo tr.gmo uk.gmo uz.gmo uz@Latn.gmo vi.gmo wa.gmo yi.gmo zh_CN.gmo zh_TW.gmo
DISTFILES = ChangeLog Makefile.in.in POTFILES.in $(GETTEXT_PACKAGE).pot \
$(POFILES) $(GMOFILES) $(SOURCES)

POTFILES = \
	../gdk-pixbuf/gdk-pixbuf-animation.c \
	../gdk-pixbuf/gdk-pixbuf-data.c \
	../gdk-pixbuf/gdk-pixbuf-io.c \
	../gdk-pixbuf/gdk-pixbuf-loader.c \
	../gdk-pixbuf/gdk-pixbuf.c \
	../gdk-pixbuf/gdk-pixdata.c \
	../gdk-pixbuf/io-ani.c \
	../gdk-pixbuf/io-bmp.c \
	../gdk-pixbuf/io-gif.c \
	../gdk-pixbuf/io-ico.c \
	../gdk-pixbuf/io-jpeg.c \
	../gdk-pixbuf/io-pcx.c \
	../gdk-pixbuf/io-png.c \
	../gdk-pixbuf/io-pnm.c \
	../gdk-pixbuf/io-ras.c \
	../gdk-pixbuf/io-tga.c \
	../gdk-pixbuf/io-tiff.c \
	../gdk-pixbuf/io-wbmp.c \
	../gdk-pixbuf/io-xbm.c \
	../gdk-pixbuf/io-xpm.c \
	../gdk/gdkdisplaymanager.c \
	../gtk/gtkaccellabel.c \
	../gtk/gtkaction.c \
	../gtk/gtkactiongroup.c \
	../gtk/gtkadjustment.c \
	../gtk/gtkalignment.c \
	../gtk/gtkarrow.c \
	../gtk/gtkaspectframe.c \
	../gtk/gtkbbox.c \
	../gtk/gtkbox.c \
	../gtk/gtkbutton.c \
	../gtk/gtkcalendar.c \
	../gtk/gtkcelllayout.c \
	../gtk/gtkcellrenderer.c \
	../gtk/gtkcellrendererpixbuf.c \
	../gtk/gtkcellrenderertext.c \
	../gtk/gtkcellrenderertoggle.c \
	../gtk/gtkcellview.c \
	../gtk/gtkcheckbutton.c \
	../gtk/gtkcheckmenuitem.c \
	../gtk/gtkcolorbutton.c \
	../gtk/gtkcolorsel.c \
	../gtk/gtkcolorseldialog.c \
	../gtk/gtkcombo.c \
	../gtk/gtkcombobox.c \
	../gtk/gtkcomboboxentry.c \
	../gtk/gtkcontainer.c \
	../gtk/gtkcurve.c \
	../gtk/gtkdialog.c \
	../gtk/gtkentry.c \
	../gtk/gtkentrycompletion.c \
	../gtk/gtkeventbox.c \
	../gtk/gtkexpander.c \
	../gtk/gtkfilechooser.c \
	../gtk/gtkfilechooserdefault.c \
	../gtk/gtkfilesel.c \
	../gtk/gtkfilesystemmodel.c \
	../gtk/gtkfilesystemunix.c \
	../gtk/gtkfilesystemwin32.c \
	../gtk/gtkfixed.c \
	../gtk/gtkfontbutton.c \
	../gtk/gtkfontsel.c \
	../gtk/gtkframe.c \
	../gtk/gtkgamma.c \
	../gtk/gtkhandlebox.c \
	../gtk/gtkiconfactory.c \
	../gtk/gtkicontheme.c \
	../gtk/gtkimage.c \
	../gtk/gtkimagemenuitem.c \
	../gtk/gtkimmodule.c \
	../gtk/gtkinputdialog.c \
	../gtk/gtkinvisible.c \
	../gtk/gtklabel.c \
	../gtk/gtklayout.c \
	../gtk/gtkmain.c \
	../gtk/gtkmenu.c \
	../gtk/gtkmenubar.c \
	../gtk/gtkmessagedialog.c \
	../gtk/gtkmisc.c \
	../gtk/gtknotebook.c \
	../gtk/gtkoptionmenu.c \
	../gtk/gtkpaned.c \
	../gtk/gtkpathbar.c \
	../gtk/gtkpreview.c \
	../gtk/gtkprogress.c \
	../gtk/gtkprogressbar.c \
	../gtk/gtkradioaction.c \
	../gtk/gtkradiobutton.c \
	../gtk/gtkradiotoolbutton.c \
	../gtk/gtkrange.c \
	../gtk/gtkrc.c \
	../gtk/gtkruler.c \
	../gtk/gtkscale.c \
	../gtk/gtkscrollbar.c \
	../gtk/gtkscrolledwindow.c \
	../gtk/gtkseparatortoolitem.c \
	../gtk/gtksettings.c \
	../gtk/gtksizegroup.c \
	../gtk/gtkspinbutton.c \
	../gtk/gtkstatusbar.c \
	../gtk/gtkstock.c \
	../gtk/gtktable.c \
	../gtk/gtktext.c \
	../gtk/gtktextbuffer.c \
	../gtk/gtktexttag.c \
	../gtk/gtktextutil.c \
	../gtk/gtktextview.c \
	../gtk/gtkthemes.c \
	../gtk/gtktipsquery.c \
	../gtk/gtktoggleaction.c \
	../gtk/gtktogglebutton.c \
	../gtk/gtktoolbar.c \
	../gtk/gtktoolbutton.c \
	../gtk/gtktoolitem.c \
	../gtk/gtktreemodelfilter.c \
	../gtk/gtktreemodelsort.c \
	../gtk/gtktreeview.c \
	../gtk/gtktreeviewcolumn.c \
	../gtk/gtkuimanager.c \
	../gtk/gtkviewport.c \
	../gtk/gtkwidget.c \
	../gtk/gtkwindow.c \
	../modules/input/gtkimcontextxim.c \
	../modules/input/imam-et.c \
	../modules/input/imcedilla.c \
	../modules/input/imcyrillic-translit.c \
	../modules/input/iminuktitut.c \
	../modules/input/imipa.c \
	../modules/input/imthai-broken.c \
	../modules/input/imti-er.c \
	../modules/input/imti-et.c \
	../modules/input/imviqr.c \
	../modules/input/imxim.c \
	../tests/testfilechooser.c

CATALOGS =  af.gmo am.gmo ar.gmo az.gmo be.gmo bg.gmo bn.gmo br.gmo ca.gmo cs.gmo cy.gmo da.gmo de.gmo el.gmo en_CA.gmo en_GB.gmo es.gmo et.gmo eu.gmo fa.gmo fi.gmo fr.gmo ga.gmo gl.gmo he.gmo hi.gmo hr.gmo hu.gmo ia.gmo id.gmo it.gmo ja.gmo ko.gmo li.gmo lt.gmo lv.gmo mi.gmo mk.gmo ml.gmo mn.gmo ms.gmo ne.gmo nl.gmo nn.gmo no.gmo pl.gmo pt.gmo pt_BR.gmo ro.gmo ru.gmo sk.gmo sl.gmo sq.gmo sr.gmo sr@ije.gmo sr@Latn.gmo sv.gmo ta.gmo th.gmo tr.gmo uk.gmo uz.gmo uz@Latn.gmo vi.gmo wa.gmo yi.gmo zh_CN.gmo zh_TW.gmo
CATOBJEXT = .gmo
INSTOBJEXT = .mo

.SUFFIXES:
.SUFFIXES: .c .o .po .pox .gmo .mo .msg .cat

.c.o:
	$(COMPILE) $<

.po.pox:
	$(MAKE) $(GETTEXT_PACKAGE).pot
	$(MSGMERGE) $< $(srcdir)/$(GETTEXT_PACKAGE).pot -o $*.pox

.po.mo:
	$(MSGFMT) -o $@ $<

.po.gmo:
	file=$(srcdir)/`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) -o $$file $<

.po.cat:
	sed -f ../intl/po2msg.sed < $< > $*.msg \
	  && rm -f $@ && $(GENCAT) $@ $*.msg


all: all-yes

all-yes: $(CATALOGS)
all-no:

$(srcdir)/$(GETTEXT_PACKAGE).pot: $(POTFILES)
	$(XGETTEXT) --default-domain=$(GETTEXT_PACKAGE) --directory=$(top_srcdir) \
	  --add-comments --keyword=_ --keyword=N_ \
	  --files-from=$(srcdir)/POTFILES.in \
	&& test ! -f $(GETTEXT_PACKAGE).po \
	   || ( rm -f $(srcdir)/$(GETTEXT_PACKAGE).pot \
		&& mv $(GETTEXT_PACKAGE).po $(srcdir)/$(GETTEXT_PACKAGE).pot )

install: install-exec install-data
install-exec:
install-data: install-data-yes
install-data-no: all
install-data-yes: all
	if test -r "$(MKINSTALLDIRS)"; then \
	  $(MKINSTALLDIRS) $(DESTDIR)$(datadir); \
	else \
	  $(SHELL) $(top_srcdir)/mkinstalldirs $(DESTDIR)$(datadir); \
	fi
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  case "$$cat" in \
	    *.gmo) destdir=$(gnulocaledir);; \
	    *)     destdir=$(localedir);; \
	  esac; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  dir=$(DESTDIR)$$destdir/$$lang/LC_MESSAGES; \
	  if test -r "$(MKINSTALLDIRS)"; then \
	    $(MKINSTALLDIRS) $$dir; \
	  else \
	    $(SHELL) $(top_srcdir)/mkinstalldirs $$dir; \
	  fi; \
	  if test -r $$cat; then \
	    $(INSTALL_DATA) $$cat $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	    echo "installing $$cat as $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT)"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$cat $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	    echo "installing $(srcdir)/$$cat as" \
		 "$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT)"; \
	  fi; \
	  if test -r $$cat.m; then \
	    $(INSTALL_DATA) $$cat.m $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	    echo "installing $$cat.m as $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m"; \
	  else \
	    if test -r $(srcdir)/$$cat.m ; then \
	      $(INSTALL_DATA) $(srcdir)/$$cat.m \
		$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	      echo "installing $(srcdir)/$$cat as" \
		   "$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m"; \
	    else \
	      true; \
	    fi; \
	  fi; \
	done
	if test "$(PACKAGE)" = "glib"; then \
	  if test -r "$(MKINSTALLDIRS)"; then \
	    $(MKINSTALLDIRS) $(DESTDIR)$(gettextsrcdir); \
	  else \
	    $(SHELL) $(top_srcdir)/mkinstalldirs $(DESTDIR)$(gettextsrcdir); \
	  fi; \
	  $(INSTALL_DATA) $(srcdir)/Makefile.in.in \
			  $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi

# Define this as empty until I found a useful application.
installcheck:

uninstall:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	done
	if test "$(PACKAGE)" = "glib"; then \
	  rm -f $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	fi

check: all

dvi info tags TAGS ID:

mostlyclean:
	rm -f core core.* *.pox $(GETTEXT_PACKAGE).po *.old.po cat-id-tbl.tmp
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in POTFILES *.mo *.msg *.cat *.cat.m

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES)

distdir = ../$(GETTEXT_PACKAGE)-$(VERSION)/$(subdir)
dist distdir: update-po $(DISTFILES)
	dists="$(DISTFILES)"; \
	for file in $$dists; do \
	  ln $(srcdir)/$$file $(distdir) 2> /dev/null \
	    || cp -p $(srcdir)/$$file $(distdir); \
	done

update-po: Makefile
	$(MAKE) $(GETTEXT_PACKAGE).pot
	tmpdir=`pwd`; \
	cd $(srcdir); \
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  echo "$$lang:"; \
	  if $(MSGMERGE) $$lang.po $(GETTEXT_PACKAGE).pot -o $$tmpdir/$$lang.new.po; then \
	    if cmp $$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	      rm -f $$tmpdir/$$lang.new.po; \
            else \
	      if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	        :; \
	      else \
	        echo "msgmerge for $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	        rm -f $$tmpdir/$$lang.new.po; \
	        exit 1; \
	      fi; \
	    fi; \
	  else \
	    echo "msgmerge for $$cat failed!"; \
	    rm -f $$tmpdir/$$lang.new.po; \
	  fi; \
	done

# POTFILES is created from POTFILES.in by stripping comments, empty lines
# and Intltool tags (enclosed in square brackets), and appending a full
# relative path to them
POTFILES: POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' 						\
		    -e "s/^\[.*\] +//" 					\
		    -e '/^[ 	]*$$/d' 				\
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in	\
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

Makefile: Makefile.in.in ../config.status POTFILES
	cd .. \
	  && CONFIG_FILES=$(subdir)/$@.in CONFIG_HEADERS= \
	       $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:

DESKTOPS_APP = e16.desktop
DESKTOPS_IN = e16.desktop.in \
		e16-session.desktop.in e16-gnome2-session.desktop.in e16-kde-session.desktop.in
XSESSION_MISC = e16-session.desktop e16-kde-session.desktop
if WITH_GNOME2
XSESSION_GNOME2 = e16-gnome2-session.desktop
endif
if WITH_GNOME3
XSESSION_GNOME3 = e16-gnome3-session.desktop
endif
XCLIENTS = Xclients.e16.sh Xclients.e16-gnome.sh Xclients.e16-kde.sh
STARTUPS = starte16-gnome starte16-kde

XSESSION_DESKTOPS = $(XSESSION_MISC) $(XSESSION_GNOME2) $(XSESSION_GNOME3)

DESKTOPS = $(DESKTOPS_APP) $(XSESSION_DESKTOPS)

miscdir = $(pkgdatadir)/misc
misc_SCRIPTS = $(XCLIENTS) starte16 e16-dbus-cmd

applicationdir = $(datadir)/applications
application_DATA = $(DESKTOPS_APP)

xsessiondir = $(datadir)/xsessions
xsession_DATA = $(XSESSION_DESKTOPS)

if WITH_GNOME3
gsessiondir = $(datadir)/gnome-session/sessions
gsession_DATA = e16-gnome.session
endif

# Is this Fedora only stuff?
#xclientsdir = $(sysconfdir)/X11/xinit/Xclients.d
#xclients_SCRIPTS = $(XCLIENTS)

EXTRA_DIST = $(DESKTOPS_IN) $(XCLIENTS) $(SCRIPTS) starte16.in \
		$(xsession_DATA) $(gsession_DATA)

CLEANFILES = $(DESKTOPS_APP) $(XSESSION_MISC) $(XSESSION_GNOME2) starte16

starte16: $(srcdir)/starte16.in Makefile

starte16:
	sed -e "s=%bindir%=${bindir}=" $(srcdir)/$@.in > $@

e16.desktop: $(srcdir)/e16.desktop.in Makefile
e16-gnome.desktop: $(srcdir)/e16-gnome.desktop.in Makefile
e16-kde.desktop: $(srcdir)/e16-kde.desktop.in Makefile

$(DESKTOPS):
	sed -e "s=@pkgdatadir@=${pkgdatadir}=" $(srcdir)/$@.in > $@

install-data-hook:
	cd $(DESTDIR)$(bindir); rm -f starte16; $(LN_S) $(miscdir)/starte16 .
	cd $(DESTDIR)$(miscdir); rm -f $(STARTUPS); \
	  for f in $(STARTUPS); do $(LN_S) starte16 $$f; done

uninstall-local:
	cd $(DESTDIR)$(bindir); rm -f starte16
	cd $(DESTDIR)$(miscdir); rm -f $(STARTUPS)

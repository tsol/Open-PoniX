SUBDIRS= src
EXTRA_DIST = doc/gssapi_mech.conf

#	XXX: also exclude debian/files and debian/files.new ? do a clean??
dist-hook:
	mkdir $(distdir)/debian/
	find $(srcdir)/debian -maxdepth 1 -not -type d |xargs -i cp {} $(distdir)/debian/
	rm -f $(distdir)/src/gssglue/gssapi/gssapi.h

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = libgssglue.pc


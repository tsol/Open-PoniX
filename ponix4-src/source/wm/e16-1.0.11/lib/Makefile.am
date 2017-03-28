libe16dir = $(pkglibdir)

libe16_LTLIBRARIES = $(LIB_HACK) $(LIB_TRIP)

if BUILD_LIBHACK

LIB_HACK = libhack.la

libhack_la_SOURCES  = e16_hack.c
libhack_la_CPPFLAGS = -I$(top_srcdir)/src $(X_CFLAGS) $(CWARNFLAGS)

libhack_la_LIBADD   = $(DLOPEN_LIBS)
libhack_la_LDFLAGS  = -module -avoid-version

endif

if BUILD_LIBTRIP

LIB_TRIP = libtrip.la

libtrip_la_SOURCES  = e16_trip.c
libtrip_la_CPPFLAGS = -I$(top_srcdir)/src $(X_CFLAGS) $(CWARNFLAGS)

libtrip_la_LIBADD   = $(DLOPEN_LIBS)
libtrip_la_LDFLAGS  = -module -avoid-version

endif

install-data-hook:
	rm -f $(DESTDIR)$(libe16dir)/*.la

uninstall-local:
	rm -f $(DESTDIR)$(libe16dir)/*.so

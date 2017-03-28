dnl Process this file with autoconf to create configure.

AC_PREREQ([2.60])
AC_INIT([xcursorgen],
	[1.0.5],
	[https://bugs.freedesktop.org/enter_bug.cgi?product=xorg],
	[xcursorgen])
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_MAINTAINER_MODE

# Require xorg-macros 1.8 or later: XORG_MANPAGE_SECTIONS
m4_ifndef([XORG_MACROS_VERSION],
	  [m4_fatal([must install xorg-macros 1.8 or later before running autoconf/autogen])])
XORG_MACROS_VERSION(1.8)

AC_CONFIG_HEADERS([config.h])

XORG_DEFAULT_OPTIONS

# Checks for pkg-config packages
PKG_CHECK_MODULES(XCURSORGEN, x11 xcursor libpng >= 1.2.0)

AC_CONFIG_FILES([
	Makefile
	man/Makefile])
AC_OUTPUT

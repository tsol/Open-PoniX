#  Copyright © 2005 Adam Jackson.
#  Copyright © 2007-2011 Guillem Jover.
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  on the rights to use, copy, modify, merge, publish, distribute, sub
#  license, and/or sell copies of the Software, and to permit persons to whom
#  the Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice (including the next
#  paragraph) shall be included in all copies or substantial portions of the
#  Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
#  ADAM JACKSON BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
#  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Process this file with autoconf to produce a configure script

AC_PREREQ([2.60])
AC_INIT([xf86-video-glide],
        [1.2.2],
        [https://bugs.freedesktop.org/enter_bug.cgi?product=xorg],
        [xf86-video-glide])

AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR(.)

AM_INIT_AUTOMAKE([foreign dist-bzip2])

AM_MAINTAINER_MODE

# Require X.Org macros 1.8 or later for MAN_SUBSTS set by XORG_MANPAGE_SECTION
m4_ifndef([XORG_MACROS_VERSION],
          [AC_FATAL([must install xorg-macros 1.8 or later before running autoconf/autogen])])
XORG_MACROS_VERSION(1.8)
XORG_DEFAULT_OPTIONS

# Checks for programs.
LT_INIT([disable-static])

AH_TOP([#include "xorg-server.h"])

AC_ARG_WITH(xorg-module-dir,
            AS_HELP_STRING([--with-xorg-module-dir=DIR],
                           [Default xorg module directory [[default=$libdir/xorg/modules]]]),
            [moduledir="$withval"],
            [moduledir="$libdir/xorg/modules"])

AC_ARG_WITH(glide-include-dir,
            AS_HELP_STRING([--with-glide-include-dir=DIR],
                           [Default Glide include directory [[default=/usr/include/glide]]]),
            [glideincludedir="$withval"],
            [glideincludedir="/usr/include/glide"])

# Checks for extensions
XORG_DRIVER_CHECK_EXT(DPMSExtension, xextproto)

# Checks for pkg-config packages
PKG_CHECK_MODULES(XORG, [xorg-server >= 1.0.99.901 xproto $REQUIRED_MODULES])
PKG_CHECK_MODULES(XEXT, [xextproto >= 7.0.99.1],
                  HAVE_XEXTPROTO_71="yes"; AC_DEFINE(HAVE_XEXTPROTO_71, 1, [xextproto 7.1 available]),
                  HAVE_XEXTPROTO_71="no")
AM_CONDITIONAL(HAVE_XEXTPROTO_71, [ test "$HAVE_XEXTPROTO_71" = "yes" ])

# Checks for libraries.
AC_CHECK_LIB([glide3x], [grGet], [GLIDE_LDFLAGS="-lglide3x"], [
  AC_CHECK_LIB([glide2x], [grSstQueryBoards], [GLIDE_LDFLAGS="-lglide2x"], [
    AC_MSG_ERROR([missing glide shared library])], [])], [])

AC_SUBST([GLIDE_LDFLAGS])

# Checks for header files.
CPPFLAGS="$CPPFLAGS -I$glideincludedir"
AC_HEADER_STDC
AC_CHECK_HEADER([glide.h], [],
                [AC_MSG_ERROR([<glide.h> is required])])

AC_CHECK_DECL([grGet],
              [AC_DEFINE(GLIDE3, 1, [glide3x available])
               AC_DEFINE(GLIDE3_ALPHA, 1, [glide3x alpha available])],
              [],
              [#include <glide.h>])

GLIDE_CFLAGS="-I$glideincludedir"
AC_SUBST([GLIDE_CFLAGS])
AC_SUBST([XORG_CFLAGS])
AC_SUBST([moduledir])

DRIVER_NAME=glide
AC_SUBST([DRIVER_NAME])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	man/Makefile
])
AC_OUTPUT

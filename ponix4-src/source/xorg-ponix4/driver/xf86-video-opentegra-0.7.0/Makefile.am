#  Copyright 2005 Adam Jackson.
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

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src man

if HAVE_OUTPUTCLASS
sysconfig_DATA = \
	opentegra.conf
endif

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL

EXTRA_DIST = \
	opentegra.conf

#
# This is needed to work around some weird behaviour in make distcheck. What
# distcheck will try to do is run something like the following:
#
#   $ ./configure --prefix=$(distdir)/_inst
#   $ make install
#
# However that will fail to install the opentegra.org file because the path
# where it is to be installed is retrieved from the xorg-server.pc file's
# sysconfigdir variable. Unfortunately that already has the ${prefix} value
# expanded, so make distcheck will effectively try to install this file into
# /usr/share/X11/xorg.conf.d, which will fail if run as normal user.
#
# We outsmart automake by overriding sysconfigdir on the configure command-
# line, using ${datarootdir} as prefix so that the file ends up installed
# somewhere below the prefix set by distcheck. Note that it's possible that
# the path will not be what the X.Org X server was configured with, but the
# workaround here is only used for sanity checking the distribution tarball
# anyway, therefore the file will never be used, so the exact location does
# not matter.
#
AM_DISTCHECK_CONFIGURE_FLAGS = sysconfigdir=\$${datarootdir}/X11/xorg.conf.d
MAINTAINERCLEANFILES = ChangeLog INSTALL

.PHONY: ChangeLog INSTALL

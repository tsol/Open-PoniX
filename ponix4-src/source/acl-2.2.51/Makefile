#
# Copyright (c) 2000-2006 Silicon Graphics, Inc.  All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

TOPDIR = .
HAVE_BUILDDEFS = $(shell test -f $(TOPDIR)/include/builddefs && echo yes || echo no)

ifeq ($(HAVE_BUILDDEFS), yes)
include $(TOPDIR)/include/builddefs
endif

CONFIGURE = \
	aclocal.m4 \
	configure config.guess config.sub \
	ltmain.sh m4/libtool.m4 m4/ltoptions.m4 m4/ltsugar.m4 \
	m4/ltversion.m4 m4/lt~obsolete.m4
LSRCFILES = \
	configure.in Makepkgs install-sh exports README VERSION \
	$(CONFIGURE)

LDIRT = config.log .dep config.status config.cache confdefs.h conftest* \
	Logs/* built .census install.* install-dev.* install-lib.* *.gz

LIB_SUBDIRS = include libmisc libacl
TOOL_SUBDIRS = getfacl setfacl chacl m4 man doc po test examples build debian

SUBDIRS = $(LIB_SUBDIRS) $(TOOL_SUBDIRS)

default: include/builddefs include/config.h
ifeq ($(HAVE_BUILDDEFS), no)
	$(MAKE) -C . $@
else
	$(MAKE) $(SUBDIRS)
endif

# tool/lib dependencies
libacl: libmisc
getfacl setfacl chacl: libacl

ifeq ($(HAVE_BUILDDEFS), yes)
include $(BUILDRULES)
else
clean:	# if configure hasn't run, nothing to clean
endif

# Recent versions of libtool require the -i option for copying auxiliary
# files (config.sub, config.guess, install-sh, ltmain.sh), while older
# versions will copy those files anyway, and don't understand -i.
LIBTOOLIZE_INSTALL = `libtoolize -n -i >/dev/null 2>/dev/null && echo -i`

configure include/builddefs:
	libtoolize -c $(LIBTOOLIZE_INSTALL) -f
	cp include/install-sh .
	aclocal -I m4
	autoconf
	./configure \
		--prefix=/ \
		--exec-prefix=/ \
		--sbindir=/bin \
		--bindir=/usr/bin \
		--libdir=/lib \
		--libexecdir=/usr/lib \
		--enable-lib64=yes \
		--includedir=/usr/include \
		--mandir=/usr/share/man \
		--datadir=/usr/share \
		$$LOCAL_CONFIGURE_OPTIONS
	touch .census

include/config.h: include/builddefs
## Recover from the removal of $@
	@if test -f $@; then :; else \
		rm -f include/builddefs; \
		$(MAKE) $(AM_MAKEFLAGS) include/builddefs; \
	fi

install: default $(addsuffix -install,$(SUBDIRS))
	$(INSTALL) -m 755 -d $(PKG_DOC_DIR)
	$(INSTALL) -m 644 README $(PKG_DOC_DIR)

install-dev: default $(addsuffix -install-dev,$(SUBDIRS))

install-lib: default $(addsuffix -install-lib,$(SUBDIRS))

%-install:
	$(MAKE) -C $* install

%-install-lib:
	$(MAKE) -C $* install-lib

%-install-dev:
	$(MAKE) -C $* install-dev

realclean distclean: clean
	rm -f $(LDIRT) $(CONFIGURE)
	rm -f include/builddefs include/config.h install-sh libtool
	rm -rf autom4te.cache Logs

.PHONY: tests root-tests nfs-tests
tests root-tests nfs-tests: default
	$(MAKE) -C test/ $@

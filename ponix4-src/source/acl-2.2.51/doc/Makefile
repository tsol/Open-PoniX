#
# Copyright (c) 2000-2002 Silicon Graphics, Inc.  All Rights Reserved.
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

TOPDIR = ..
include $(TOPDIR)/include/builddefs

LSRCFILES = extensions.txt libacl.txt TODO PORTING CHANGES COPYING COPYING.LGPL INSTALL \
	    old-acl.5
LDIRT = *.gz

default: CHANGES.gz

include $(BUILDRULES)

CHANGES.gz:
	$(ZIP) --best -c < CHANGES > $@

install: default
	$(INSTALL) -m 755 -d $(PKG_DOC_DIR)
	$(INSTALL) -m 644 PORTING CHANGES.gz $(PKG_DOC_DIR)
ifeq ($(PKG_DISTRIBUTION), debian)
	$(INSTALL) -S CHANGES.gz $(PKG_DOC_DIR)/changelog.gz
else
	$(INSTALL) -m 644 COPYING COPYING.LGPL $(PKG_DOC_DIR)
endif

install-dev install-lib:

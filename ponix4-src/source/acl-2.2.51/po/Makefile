#
# Copyright (c) 2001-2002 Silicon Graphics, Inc.  All Rights Reserved.
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

POTHEAD = $(PKG_NAME).pot
LINGUAS ?= de es fr gl pl sv
LSRCFILES = $(LINGUAS:%=%.po) $(POTHEAD)
LDIRT = $(POTHEAD)

XGETTEXTFILES =	$(TOPDIR)/chacl/chacl.c \
		$(TOPDIR)/setfacl/do_set.c \
		$(TOPDIR)/setfacl/setfacl.c \
		$(TOPDIR)/getfacl/getfacl.c \
		$(TOPDIR)/libacl/acl_error.c \
		$(TOPDIR)/libacl/perm_copy_fd.c \
		$(TOPDIR)/libacl/perm_copy_file.c

default: $(POTHEAD) $(LINGUAS:%=%.mo)

include $(BUILDRULES)

install: default
	$(INSTALL_LINGUAS)

install-dev install-lib:

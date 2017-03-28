#
# Copyright (c) 2003-2006 Silicon Graphics, Inc.  All Rights Reserved.
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

LSRCFILES = \
	manual_format.m4 \
	package_attrdev.m4 \
	package_globals.m4 \
	package_utilies.m4 \
	visibility_hidden.m4 \
	multilib.m4

default: 

include $(BUILDRULES)

install install-dev install-lib: default

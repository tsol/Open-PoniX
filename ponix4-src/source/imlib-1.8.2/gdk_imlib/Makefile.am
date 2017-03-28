# GPL (C) 1997 The Rasterman
#   raster@rasterman.com

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# the resultant executable

lib_LTLIBRARIES = libgdk_imlib.la

include_HEADERS = gdk_imlib.h gdk_imlib_private.h gdk_imlib_types.h

libgdk_imlib_la_SOURCES = \
cache.c      \
colors.c     \
globals.c    \
load.c       \
misc.c       \
rend.c       \
utils.c      \
save.c

libgdk_imlib_la_LDFLAGS      = -version-info 9:1:8
libgdk_imlib_la_LDDADD = $(LIBS)

DEFS = -DDJPEG_PROG=\"@DJPEG_PROG@\"  -DCJPEG_PROG=\"@CJPEG_PROG@\" \
	-DCONVERT_PATH=\"@CONVERT_PATH@\" \
	-DNETPBM_PATH=\"@NETPBM_PATH@\" -DSYSTEM_IMRC=\"@sysconfdir@/imrc\"

INCLUDES = -I. -I$(srcdir) -I.. -I$(srcdir)/.. $(GTK_CFLAGS) $(X_CFLAGS)

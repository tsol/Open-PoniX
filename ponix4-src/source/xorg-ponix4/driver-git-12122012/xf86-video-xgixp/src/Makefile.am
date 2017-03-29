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

# this is obnoxious:
# -module lets us name the module exactly how we want
# -avoid-version prevents gratuitous .0.0.0 version numbers on the end
# _ladir passes a dummy rpath to libtool so the thing will actually link
# TODO: -nostdlib/-Bstatic/-lgcc platform magic, not installing the .a, etc.
AM_CFLAGS = @XORG_CFLAGS@ @DRI_CFLAGS@
xgixp_drv_la_LTLIBRARIES = xgixp_drv.la
xgixp_drv_la_LDFLAGS = -module -avoid-version
xgixp_drv_ladir = @moduledir@/drivers

xgixp_drv_la_SOURCES = \
        compat-api.h \
	xg47_accel.c \
	xg47_accel.h \
	xg47_cmdlist.c \
	xg47_cmdlist.h \
	xg47_cursor.c \
	xg47_cursor.h \
	xg47_display.c \
	xg47_i2c.c \
	xg47_i2c.h \
	xg47_misc.c \
	xg47_misc.h \
	xg47_mode.c \
	xg47_mode.h \
	xg47_native_mode.c \
	xg47_outputs.c \
	xg47_regs.h \
	xg47_tv.c \
	xg47_tv.h \
	xg47_video.c \
	xg47_video.h \
	xg47_videohw.c \
	xg47_videohw.h \
	xgi.h \
	xgi_bios.h \
	xgi_cursor.c \
	xgi_cursor.h \
	xgi_dga.c \
	xgi_dga.h \
	xgi_dri.c \
	xgi_dri.h \
	xgi_driver.c \
	xgi_driver.h \
	xgi_hwmc.c \
	xgi_hwmc.h \
	xgi_misc.c \
	xgi_misc.h \
	xgi_option.c \
	xgi_option.h \
	xgi_shadow.c \
	xgi_shadow.h \
	xgi_video.c \
	xgi_video.h \
	xgi_debug.c \
	xgi_debug.h \
	xgi_regs.h \
	xgi_version.h \
	xgi_drm.h

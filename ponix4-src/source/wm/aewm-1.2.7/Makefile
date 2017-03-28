# aewm - a minimalist X11 window mananager. vim:ft=make:noet
# Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>
# This program is free software; see LICENSE for details.

SUBDIRS = src clients

all install clean:
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

.PHONY: all install clean

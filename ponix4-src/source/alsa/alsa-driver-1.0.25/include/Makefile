#
# Makefile for ALSA driver
# Copyright (c) 1994-98 by Jaroslav Kysela <perex@perex.cz>
#

include ../Makefile.conf

all: $(TARGETS)

prepare:
	$(MAKE) -C sound prepare

fastdep: prepare

modules_install:

clean:
	rm -f .depend core *.o sndversions.h modules/*.ver modules/*.stamp
	$(MAKE) -C sound clean

mrproper: clean
	rm -f *.orig *.rej *~ .#*
	rm -rf linux/regulator
	rm -rf linux/usb
	rm -f linux/* asm/* media/*

ifndef SND_TOPDIR
SND_TOPDIR=..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

clean-files := cmp.c iso-resources.c

include $(SND_TOPDIR)/alsa-kernel/firewire/Makefile

EXTRA_CFLAGS += -I$(SND_TOPDIR)/alsa-kernel/firewire

include $(SND_TOPDIR)/Rules.make

cmp.c: cmp.patch $(SND_TOPDIR)/alsa-kernel/firewire/cmp.c
iso-resources.c: iso-resources.patch $(SND_TOPDIR)/alsa-kernel/firewire/iso-resources.c

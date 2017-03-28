ifndef SND_TOPDIR
SND_TOPDIR=../..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

EXTRA_CFLAGS = -I$(SND_TOPDIR)/alsa-kernel/pci/ctxfi

clean-files := ctatc.c

include $(SND_TOPDIR)/alsa-kernel/pci/ctxfi/Makefile

include $(SND_TOPDIR)/Rules.make

ctatc.c: ctatc.patch $(SND_TOPDIR)/alsa-kernel/pci/ctxfi/ctatc.c

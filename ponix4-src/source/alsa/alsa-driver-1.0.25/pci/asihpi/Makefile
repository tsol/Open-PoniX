ifndef SND_TOPDIR
SND_TOPDIR=../..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf


EXTRA_CFLAGS = -I$(SND_TOPDIR)/alsa-kernel/pci/asihpi

include $(SND_TOPDIR)/alsa-kernel/pci/asihpi/Makefile

include $(SND_TOPDIR)/Rules.make


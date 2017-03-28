ifndef SND_TOPDIR
SND_TOPDIR=../..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

include $(SND_TOPDIR)/alsa-kernel/pci/oxygen/Makefile

export-objs := oxygen_io.o oxygen_lib.o oxygen_pcm.o

include $(SND_TOPDIR)/Rules.make

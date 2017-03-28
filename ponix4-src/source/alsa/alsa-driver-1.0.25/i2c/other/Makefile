ifndef SND_TOPDIR
SND_TOPDIR=../..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

clean-files := tea575x-tuner.c

export-objs += ak4xxx-adda.o ak4113.o ak4114.o ak4117.o \
	       pt2258.o tea575x-tuner.o

include $(SND_TOPDIR)/alsa-kernel/i2c/other/Makefile

include $(SND_TOPDIR)/Rules.make

tea575x-tuner.c: tea575x-tuner.patch $(SND_TOPDIR)/alsa-kernel/i2c/other/tea575x-tuner.c

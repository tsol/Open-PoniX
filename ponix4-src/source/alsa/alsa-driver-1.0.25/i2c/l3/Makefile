ifndef SND_TOPDIR
SND_TOPDIR=../..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

export-objs += uda1341.o

# include $(SND_TOPDIR)/alsa-kernel/i2c/l3/Makefile

snd-uda1341-objs := uda1341.o
obj-$(CONFIG_SND_SA11XX_UDA1341) += snd-uda1341.o

include $(SND_TOPDIR)/Rules.make

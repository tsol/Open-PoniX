ifndef SND_TOPDIR
SND_TOPDIR=../..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

clean-files := hda_intel.c hda_beep.c

export-objs  := hda_codec.o

EXTRA_CFLAGS += -I$(SND_TOPDIR)/alsa-kernel/pci/hda

include $(SND_TOPDIR)/alsa-kernel/pci/hda/Makefile

ifdef CONFIG_SND_HDA_CODEC_HDMI
snd-hda-codec-atihdmi-objs :=	dummy.o
snd-hda-codec-intelhdmi-objs :=	dummy.o
snd-hda-codec-nvhdmi-objs :=	dummy.o
obj-$(CONFIG_SND_HDA_INTEL) += snd-hda-codec-atihdmi.o
obj-$(CONFIG_SND_HDA_INTEL) += snd-hda-codec-intelhdmi.o
obj-$(CONFIG_SND_HDA_INTEL) += snd-hda-codec-nvhdmi.o
endif

include $(SND_TOPDIR)/Rules.make

hda_intel.c: hda_intel.patch $(SND_TOPDIR)/alsa-kernel/pci/hda/hda_intel.c
hda_beep.c: hda_beep.patch $(SND_TOPDIR)/alsa-kernel/pci/hda/hda_beep.c

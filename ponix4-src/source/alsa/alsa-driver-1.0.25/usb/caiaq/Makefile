ifndef SND_TOPDIR
SND_TOPDIR=../..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

clean-files := audio.c device.c input.c

include $(SND_TOPDIR)/alsa-kernel/usb/caiaq/Makefile

EXTRA_CFLAGS += -I$(SND_TOPDIR)/alsa-kernel/usb/caiaq

include $(SND_TOPDIR)/Rules.make

audio.c: audio.patch $(SND_TOPDIR)/alsa-kernel/usb/caiaq/audio.c
device.c: device.patch $(SND_TOPDIR)/alsa-kernel/usb/caiaq/device.c
input.c: input.patch $(SND_TOPDIR)/alsa-kernel/usb/caiaq/input.c

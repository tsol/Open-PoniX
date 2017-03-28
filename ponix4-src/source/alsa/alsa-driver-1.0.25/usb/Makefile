ifndef SND_TOPDIR
SND_TOPDIR=..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

clean-files := card.c endpoint.c helper.c quirks.c midi.c \
	mixer.c mixer_quirks.c stream.c

# for compatibility
extra-snd-usb-lib-objs := usbcompat.o
export-objs := midi.o usbcompat.o

include $(SND_TOPDIR)/alsa-kernel/usb/Makefile

EXTRA_CFLAGS += -I$(SND_TOPDIR)/alsa-kernel/usb

include $(SND_TOPDIR)/Rules.make

card.c: card.patch card.inc card.inc1 usbaudio.inc $(SND_TOPDIR)/alsa-kernel/usb/card.c
endpoint.c: endpoint.patch usbaudio.inc $(SND_TOPDIR)/alsa-kernel/usb/endpoint.c
helper.c: helper.patch usbaudio.inc $(SND_TOPDIR)/alsa-kernel/usb/helper.c
quirks.c: quirks.patch usbaudio.inc $(SND_TOPDIR)/alsa-kernel/usb/quirks.c
midi.c: midi.patch midi.inc midi.inc1 $(SND_TOPDIR)/alsa-kernel/usb/midi.c
mixer.c: mixer.patch $(SND_TOPDIR)/alsa-kernel/usb/mixer.c
mixer_quirks.c: mixer_quirks.patch $(SND_TOPDIR)/alsa-kernel/usb/mixer_quirks.c
stream.c: stream.patch usbaudio.inc $(SND_TOPDIR)/alsa-kernel/usb/stream.c

ifndef SND_TOPDIR
SND_TOPDIR=../..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

clean-files := pcsp.c pcsp_input.c pcsp_lib.c

include $(SND_TOPDIR)/alsa-kernel/drivers/pcsp/Makefile

include $(SND_TOPDIR)/Rules.make

pcsp.c: pcsp.patch $(SND_TOPDIR)/alsa-kernel/drivers/pcsp/pcsp.c
pcsp_input.c: pcsp_input.patch $(SND_TOPDIR)/alsa-kernel/drivers/pcsp/pcsp_input.c
pcsp_lib.c: pcsp_lib.patch $(SND_TOPDIR)/alsa-kernel/drivers/pcsp/pcsp_lib.c

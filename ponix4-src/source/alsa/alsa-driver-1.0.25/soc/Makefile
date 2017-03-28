ifndef SND_TOPDIR
SND_TOPDIR=..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

clean-files := soc-core.c

include $(SND_TOPDIR)/alsa-kernel/soc/Makefile

include $(SND_TOPDIR)/Rules.make

soc-core.c: soc-core.patch $(SND_TOPDIR)/alsa-kernel/soc/soc-core.c

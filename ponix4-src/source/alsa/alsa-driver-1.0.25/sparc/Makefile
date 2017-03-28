ifndef SND_TOPDIR
SND_TOPDIR=..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

clean-files := amd7930.c cs4231.c dbri.c

include $(SND_TOPDIR)/alsa-kernel/sparc/Makefile

include $(SND_TOPDIR)/Rules.make

am7930.c: amd7930.patch $(SND_TOPDIR)/alsa-kernel/sparc/amd7930.c
cs4231.c: cs4231.patch $(SND_TOPDIR)/alsa-kernel/sparc/cs4231.c
dbri.c: dbri.patch $(SND_TOPDIR)/alsa-kernel/sparc/dbri.c


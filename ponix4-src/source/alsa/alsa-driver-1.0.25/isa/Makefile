ifndef SND_TOPDIR
SND_TOPDIR=..
endif

include $(SND_TOPDIR)/toplevel.config
include $(SND_TOPDIR)/Makefile.conf

#
# ALSA extra code
#

isapnp-files := als100.isapnp azt2320.isapnp opl3sa2.isapnp \
		cmi8330.isapnp es18xx.isapnp sscape.isapnp

#
#
#

include $(SND_TOPDIR)/alsa-kernel/isa/Makefile

include $(SND_TOPDIR)/Rules.make

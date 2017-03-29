cmd_arch/x86/realmode/rm/realmode.lds := gcc -E -Wp,-MD,arch/x86/realmode/rm/.realmode.lds.d  -nostdinc -isystem /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include -I./arch/x86/include -Iarch/x86/include/generated  -Iinclude -I./arch/x86/include/uapi -Iarch/x86/include/generated/uapi -I./include/uapi -Iinclude/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__    -P -C -Iarch/x86/realmode/rm -P -C -Ux86 -D__ASSEMBLY__ -DLINKER_SCRIPT -o arch/x86/realmode/rm/realmode.lds arch/x86/realmode/rm/realmode.lds.S

source_arch/x86/realmode/rm/realmode.lds := arch/x86/realmode/rm/realmode.lds.S

deps_arch/x86/realmode/rm/realmode.lds := \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/physical/start.h) \
    $(wildcard include/config/physical/align.h) \
    $(wildcard include/config/x86/64.h) \
  include/uapi/linux/const.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/uapi/linux/types.h \
  arch/x86/include/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/highmem4g.h) \
    $(wildcard include/config/highmem64g.h) \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/x86/pae.h) \
  arch/x86/realmode/rm/pasyms.h \

arch/x86/realmode/rm/realmode.lds: $(deps_arch/x86/realmode/rm/realmode.lds)

$(deps_arch/x86/realmode/rm/realmode.lds):

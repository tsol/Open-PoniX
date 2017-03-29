cmd_arch/x86/math-emu/reg_round.o := gcc -Wp,-MD,arch/x86/math-emu/.reg_round.o.d  -nostdinc -isystem /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include -I./arch/x86/include -Iarch/x86/include/generated  -Iinclude -I./arch/x86/include/uapi -Iarch/x86/include/generated/uapi -I./include/uapi -Iinclude/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__  -D__ASSEMBLY__ -m32 -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1  -DCONFIG_AS_CRC32=1 -DCONFIG_AS_AVX=1       -DPARANOID    -c -o arch/x86/math-emu/reg_round.o arch/x86/math-emu/reg_round.S

source_arch/x86/math-emu/reg_round.o := arch/x86/math-emu/reg_round.S

deps_arch/x86/math-emu/reg_round.o := \
  arch/x86/math-emu/fpu_emu.h \
  arch/x86/math-emu/fpu_asm.h \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/32.h) \
    $(wildcard include/config/x86/64.h) \
    $(wildcard include/config/x86/alignment/16.h) \
  arch/x86/math-emu/exception.h \
  arch/x86/math-emu/control_w.h \

arch/x86/math-emu/reg_round.o: $(deps_arch/x86/math-emu/reg_round.o)

$(deps_arch/x86/math-emu/reg_round.o):

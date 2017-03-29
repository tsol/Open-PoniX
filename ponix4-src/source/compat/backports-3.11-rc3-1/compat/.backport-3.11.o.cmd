cmd_/source/compat/backports-3.11-rc3-1/compat/backport-3.11.o := gcc -Wp,-MD,/source/compat/backports-3.11-rc3-1/compat/.backport-3.11.o.d -I/source/compat/backports-3.11-rc3-1/backport-include/ -I/source/compat/backports-3.11-rc3-1/backport-include/uapi -I/source/compat/backports-3.11-rc3-1/include/ -I/source/compat/backports-3.11-rc3-1/include/uapi -I/source/compat/backports-3.11-rc3-1/include/drm -include /source/compat/backports-3.11-rc3-1/backport-include/backport/backport.h -Wno-unused-but-set-variable -DBACKPORTS_VERSION=\""v3.11-rc3-1-0-g4e81a94"\" -DBACKPORTED_KERNEL_VERSION=\""v3.11-rc3-0-g5ae90d8"\" -DBACKPORTED_KERNEL_NAME=\""Linux"\"  -Os -I/source/kernel-3.2.12/linux-3.2.12/arch/x86/include -Iarch/x86/include/generated -Iinclude  -include /source/kernel-3.2.12/linux-3.2.12/include/linux/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -m32 -msoft-float -mregparm=3 -freg-struct-return -mpreferred-stack-boundary=2 -march=i486 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -I/source/compat/backports-3.11-rc3-1/compat  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(backport_3.11)"  -D"KBUILD_MODNAME=KBUILD_STR(compat)" -c -o /source/compat/backports-3.11-rc3-1/compat/backport-3.11.o /source/compat/backports-3.11-rc3-1/compat/backport-3.11.c

source_/source/compat/backports-3.11-rc3-1/compat/backport-3.11.o := /source/compat/backports-3.11-rc3-1/compat/backport-3.11.c

deps_/source/compat/backports-3.11-rc3-1/compat/backport-3.11.o := \
    $(wildcard include/config/mtrr.h) \
    $(wildcard include/config/x86/pat.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/backport/backport.h \
  /source/compat/backports-3.11-rc3-1/backport-include/backport/autoconf.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/version.h \
  include/linux/version.h \
  /source/compat/backports-3.11-rc3-1/backport-include/backport/checks.h \
    $(wildcard include/config/dynamic/ftrace.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/export.h \
  include/linux/export.h \
    $(wildcard include/config/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/asm/mtrr.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/mtrr.h \
    $(wildcard include/config/compat.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/types.h \
    $(wildcard include/config/x86.h) \
    $(wildcard include/config/x86/64.h) \
    $(wildcard include/config/64bit.h) \
    $(wildcard include/config/x86/pae.h) \
    $(wildcard include/config/phys/64bit.h) \
    $(wildcard include/config/ppc.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/ppc64.h) \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/avr32.h) \
    $(wildcard include/config/blackfin.h) \
    $(wildcard include/config/cris.h) \
    $(wildcard include/config/h8300.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/m68k.h) \
    $(wildcard include/config/mips.h) \
    $(wildcard include/config/parisc.h) \
    $(wildcard include/config/s390.h) \
    $(wildcard include/config/ppc32.h) \
    $(wildcard include/config/superh.h) \
    $(wildcard include/config/sparc.h) \
    $(wildcard include/config/frv.h) \
    $(wildcard include/config/m32r.h) \
    $(wildcard include/config/mn10300.h) \
    $(wildcard include/config/xtensa.h) \
    $(wildcard include/config/arm.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/types.h \
  include/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/compiler.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/x86/32.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/posix_types_32.h \
  include/linux/ioctl.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/errno.h \
  /source/compat/backports-3.11-rc3-1/backport-include/asm/errno.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/x86/invlpg.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/required-features.h \
    $(wildcard include/config/x86/minimum/cpu/family.h) \
    $(wildcard include/config/math/emulation.h) \
    $(wildcard include/config/x86/cmpxchg64.h) \
    $(wildcard include/config/x86/cmov.h) \
    $(wildcard include/config/x86/use/3dnow.h) \
    $(wildcard include/config/x86/p6/nop.h) \
    $(wildcard include/config/paravirt.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/asm.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/bitops.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/alternative.h \
    $(wildcard include/config/smp.h) \
  include/linux/stringify.h \
  include/asm-generic/bitops/find.h \
  include/asm-generic/bitops/sched.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/le.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/swab.h \
    $(wildcard include/config/x86/bswap.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/byteorder/generic.h \
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/io.h \
  include/linux/io.h \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/has/ioport.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/io.h \
    $(wildcard include/config/x86/oostore.h) \
    $(wildcard include/config/x86/ppro/fence.h) \
    $(wildcard include/config/xen.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/string.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stdarg.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/string.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/string_32.h \
    $(wildcard include/config/kmemcheck.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/page.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/page_types.h \
  include/linux/const.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/highmem4g.h) \
    $(wildcard include/config/highmem64g.h) \
    $(wildcard include/config/page/offset.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/page_32.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/debug/virtual.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/x86/3dnow.h) \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  include/asm-generic/getorder.h \
  include/asm-generic/iomap.h \
    $(wildcard include/config/pci.h) \
  include/linux/linkage.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/vmalloc.h \
  include/linux/vmalloc.h \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/typecheck.h \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/debug/stack/usage.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/processor.h \
    $(wildcard include/config/x86/vsmp.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/m386.h) \
    $(wildcard include/config/m486.h) \
    $(wildcard include/config/x86/debugctlmsr.h) \
    $(wildcard include/config/cpu/sup/amd.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/processor-flags.h \
    $(wildcard include/config/vm86.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/vm86.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ptrace.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ptrace-abi.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/segment.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/init.h \
  include/linux/init.h \
    $(wildcard include/config/hotplug.h) \
  include/asm-generic/ptrace.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/math_emu.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/sigcontext.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/current.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/percpu.h \
    $(wildcard include/config/x86/64/smp.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/kernel.h \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/printk.h \
    $(wildcard include/config/dynamic/debug.h) \
  include/linux/printk.h \
    $(wildcard include/config/printk.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/dynamic_debug.h \
  include/linux/dynamic_debug.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/asm-generic/bug.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  include/asm-generic/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/system.h \
    $(wildcard include/config/ia32/emulation.h) \
    $(wildcard include/config/x86/32/lazy/gs.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cmpxchg.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cmpxchg_32.h \
    $(wildcard include/config/x86/cmpxchg.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/nops.h \
    $(wildcard include/config/mk7.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/irqflags.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/compat/vdso.h) \
    $(wildcard include/config/proc/fs.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/pgtable_32_types.h \
    $(wildcard include/config/highmem.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/pgtable-2level_types.h \
  include/asm-generic/pgtable-nopud.h \
  include/asm-generic/pgtable-nopmd.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/msr.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/msr-index.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cpumask.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  include/linux/bitmap.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/desc_defs.h \
  include/linux/personality.h \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cache.h \
    $(wildcard include/config/x86/l1/cache/shift.h) \
    $(wildcard include/config/x86/internode/cache/shift.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/math64.h \
  include/linux/math64.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/div64.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/err.h \
  include/linux/err.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ftrace.h \
    $(wildcard include/config/function/tracer.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/atomic.h \
  include/linux/atomic.h \
    $(wildcard include/config/arch/has/atomic/or.h) \
    $(wildcard include/config/generic/atomic64.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/asm/atomic.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/atomic.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/atomic64_32.h \
  include/asm-generic/atomic-long.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/list.h \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/backport/magic.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  include/linux/spinlock_types_up.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/lockdep.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/prove/rcu.h) \
  include/linux/rwlock_types.h \
  include/linux/spinlock_up.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_up.h \

/source/compat/backports-3.11-rc3-1/compat/backport-3.11.o: $(deps_/source/compat/backports-3.11-rc3-1/compat/backport-3.11.o)

$(deps_/source/compat/backports-3.11-rc3-1/compat/backport-3.11.o):

cmd_/source/compat/backports-3.11-rc3-1/compat/drivers-base-dma-buf.o := gcc -Wp,-MD,/source/compat/backports-3.11-rc3-1/compat/.drivers-base-dma-buf.o.d -I/source/compat/backports-3.11-rc3-1/backport-include/ -I/source/compat/backports-3.11-rc3-1/backport-include/uapi -I/source/compat/backports-3.11-rc3-1/include/ -I/source/compat/backports-3.11-rc3-1/include/uapi -I/source/compat/backports-3.11-rc3-1/include/drm -include /source/compat/backports-3.11-rc3-1/backport-include/backport/backport.h -Wno-unused-but-set-variable -DBACKPORTS_VERSION=\""v3.11-rc3-1-0-g4e81a94"\" -DBACKPORTED_KERNEL_VERSION=\""v3.11-rc3-0-g5ae90d8"\" -DBACKPORTED_KERNEL_NAME=\""Linux"\"  -Os -I/source/kernel-3.2.12/linux-3.2.12/arch/x86/include -Iarch/x86/include/generated -Iinclude  -include /source/kernel-3.2.12/linux-3.2.12/include/linux/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -m32 -msoft-float -mregparm=3 -freg-struct-return -mpreferred-stack-boundary=2 -march=i486 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -I/source/compat/backports-3.11-rc3-1/compat  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(drivers_base_dma_buf)"  -D"KBUILD_MODNAME=KBUILD_STR(compat)" -c -o /source/compat/backports-3.11-rc3-1/compat/drivers-base-dma-buf.o /source/compat/backports-3.11-rc3-1/compat/drivers-base-dma-buf.c

source_/source/compat/backports-3.11-rc3-1/compat/drivers-base-dma-buf.o := /source/compat/backports-3.11-rc3-1/compat/drivers-base-dma-buf.c

deps_/source/compat/backports-3.11-rc3-1/compat/drivers-base-dma-buf.o := \
    $(wildcard include/config/debug/fs.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/backport/backport.h \
  /source/compat/backports-3.11-rc3-1/backport-include/backport/autoconf.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/version.h \
  include/linux/version.h \
  /source/compat/backports-3.11-rc3-1/backport-include/backport/checks.h \
    $(wildcard include/config/dynamic/ftrace.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/fs.h \
  include/linux/fs.h \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  include/linux/limits.h \
  include/linux/ioctl.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/blk_types.h \
    $(wildcard include/config/blk/dev/integrity.h) \
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
  include/linux/linkage.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  include/linux/stringify.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/wait.h \
  include/linux/wait.h \
    $(wildcard include/config/lockdep.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/list.h \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/const.h \
  /source/compat/backports-3.11-rc3-1/backport-include/backport/magic.h \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
  include/linux/typecheck.h \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/bitops.h \
    $(wildcard include/config/x86/cmov.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/alternative.h \
    $(wildcard include/config/paravirt.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/asm.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/x86/invlpg.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/required-features.h \
    $(wildcard include/config/x86/minimum/cpu/family.h) \
    $(wildcard include/config/math/emulation.h) \
    $(wildcard include/config/x86/cmpxchg64.h) \
    $(wildcard include/config/x86/use/3dnow.h) \
    $(wildcard include/config/x86/p6/nop.h) \
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
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/debug/stack/usage.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/page.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/page_types.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/page_32_types.h \
    $(wildcard include/config/highmem4g.h) \
    $(wildcard include/config/highmem64g.h) \
    $(wildcard include/config/page/offset.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/page_32.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/debug/virtual.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/x86/3dnow.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/string.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stdarg.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/string.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/string_32.h \
    $(wildcard include/config/kmemcheck.h) \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  include/asm-generic/getorder.h \
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
    $(wildcard include/config/modules.h) \
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
    $(wildcard include/config/x86/ppro/fence.h) \
    $(wildcard include/config/x86/oostore.h) \
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
  /source/compat/backports-3.11-rc3-1/backport-include/asm/errno.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cpumask.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/cpumask.h \
  include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  include/linux/bitmap.h \
  include/linux/errno.h \
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
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  include/linux/spinlock_types_up.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/lockdep.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/prove/rcu.h) \
  include/linux/rwlock_types.h \
  include/linux/spinlock_up.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_up.h \
  include/linux/kdev_t.h \
  include/linux/dcache.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/rculist.h \
  include/linux/rculist.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/rcupdate.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/rcu/torture/test.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/no/hz.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/tiny/preempt/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/preempt/rt.h) \
  include/linux/seqlock.h \
  include/linux/completion.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/rcutiny.h \
    $(wildcard include/config/rcu/boost.h) \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/path.h \
  include/linux/stat.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/stat.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/time.h \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/radix-tree.h \
  include/linux/radix-tree.h \
  include/linux/prio_tree.h \
  include/linux/pid.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/have/arch/mutex/cpu/relax.h) \
  include/linux/capability.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/semaphore.h \
  include/linux/semaphore.h \
  include/linux/fiemap.h \
  include/linux/shrinker.h \
  include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/rwsem.h \
  include/linux/percpu_counter.h \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
  include/linux/pfn.h \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/inet.h \
  include/linux/fcntl.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/fcntl.h \
  include/asm-generic/fcntl.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/uidgid.h \
    $(wildcard include/config/uidgid/strict/type/checks.h) \
    $(wildcard include/config/user/ns.h) \
  include/linux/highuid.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/slab.h \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/failslab.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/debug/slab.h) \
    $(wildcard include/config/slab.h) \
  include/linux/gfp.h \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/nodemask.h \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/generated/bounds.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
  include/linux/notifier.h \
  include/linux/srcu.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/sched/book.h) \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/topology.h \
    $(wildcard include/config/x86/ht.h) \
  include/asm-generic/topology.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
  include/linux/slub_def.h \
    $(wildcard include/config/slub/stats.h) \
    $(wildcard include/config/slub/debug.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/workqueue.h \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/timer.h \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/jiffies.h \
  include/linux/jiffies.h \
  include/linux/timex.h \
  include/linux/param.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/timex.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/tsc.h \
    $(wildcard include/config/x86/tsc.h) \
  include/linux/kobject.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/sysfs.h \
  include/linux/sysfs.h \
  include/linux/kobject_ns.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/kref.h \
  include/linux/kref.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/kmemleak.h \
  include/linux/kmemleak.h \
    $(wildcard include/config/debug/kmemleak.h) \
  /source/compat/backports-3.11-rc3-1/include/linux/dma-buf.h \
  /source/compat/backports-3.11-rc3-1/include/linux/backport-dma-buf.h \
  include/linux/file.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/scatterlist.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/debug/sg.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/scatterlist.h \
  include/asm-generic/scatterlist.h \
    $(wildcard include/config/need/sg/dma/length.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/mm.h \
  include/linux/mm.h \
    $(wildcard include/config/sysctl.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/transparent/hugepage.h) \
    $(wildcard include/config/ksm.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/hugetlbfs.h) \
  include/linux/rbtree.h \
  include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/cmpxchg/local.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mm/owner.h) \
    $(wildcard include/config/mmu/notifier.h) \
  include/linux/auxvec.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/auxvec.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/mmu.h \
  include/linux/range.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/pgtable.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/e820.h \
    $(wildcard include/config/efi.h) \
    $(wildcard include/config/intel/txt.h) \
    $(wildcard include/config/memtest.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/ioport.h \
  include/linux/ioport.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/x86_init.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/bootparam.h \
  include/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/linux/edd.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ist.h \
  include/video/edid.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/pgtable_32.h \
    $(wildcard include/config/highpte.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/pgtable_32_types.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/provide/ohci1394/dma/init.h) \
    $(wildcard include/config/x86/local/apic.h) \
    $(wildcard include/config/x86/io/apic.h) \
    $(wildcard include/config/x86/visws/apic.h) \
    $(wildcard include/config/x86/f00f/bug.h) \
    $(wildcard include/config/x86/cyclone/timer.h) \
    $(wildcard include/config/pci/mmconfig.h) \
    $(wildcard include/config/x86/mrst.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/acpi.h \
    $(wildcard include/config/acpi.h) \
    $(wildcard include/config/acpi/numa.h) \
  include/acpi/pdc_intel.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/numa.h \
    $(wildcard include/config/numa/emu.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/apicdef.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/numa_32.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/x86/numaq.h) \
    $(wildcard include/config/mca.h) \
    $(wildcard include/config/eisa.h) \
    $(wildcard include/config/x86/mpparse.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/mpspec_def.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/trampoline.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/io.h \
    $(wildcard include/config/xen.h) \
  include/asm-generic/iomap.h \
    $(wildcard include/config/has/ioport.h) \
    $(wildcard include/config/pci.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/vmalloc.h \
  include/linux/vmalloc.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/kmap_types.h \
    $(wildcard include/config/debug/highmem.h) \
  include/asm-generic/kmap_types.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/paravirt/spinlocks.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/pgtable-2level.h \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/swap.h) \
  include/linux/huge_mm.h \
  include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  include/linux/vm_event_item.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/dma-mapping.h \
    $(wildcard include/config/need/dma/map/state.h) \
  include/linux/dma-mapping.h \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/have/dma/attrs.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/device.h \
    $(wildcard include/config/preempt/desktop.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/export.h \
  include/linux/export.h \
    $(wildcard include/config/symbol/prefix.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
  include/linux/klist.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/pm.h \
  include/linux/pm.h \
    $(wildcard include/config/pm.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/pm/runtime.h) \
    $(wildcard include/config/pm/clk.h) \
    $(wildcard include/config/pm/generic/domains.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/device.h \
    $(wildcard include/config/intel/iommu.h) \
    $(wildcard include/config/amd/iommu.h) \
  include/linux/pm_wakeup.h \
  include/linux/ratelimit.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/dma-attrs.h \
  include/linux/dma-attrs.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/bug.h \
  include/linux/bug.h \
  include/linux/dma-direction.h \
  /source/compat/backports-3.11-rc3-1/backport-include/asm/dma-mapping.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/dma-mapping.h \
    $(wildcard include/config/isa.h) \
  include/linux/kmemcheck.h \
  include/linux/dma-debug.h \
    $(wildcard include/config/dma/api/debug.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/swiotlb.h \
    $(wildcard include/config/swiotlb.h) \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/swiotlb.h \
  include/linux/swiotlb.h \
  include/asm-generic/dma-coherent.h \
    $(wildcard include/config/have/generic/dma/coherent.h) \
  include/asm-generic/dma-mapping-common.h \
  include/linux/anon_inodes.h \
  include/linux/fdtable.h \
  include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/lockup/detector.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/virt/cpu/accounting.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/fanotify.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/irq/time/accounting.h) \
    $(wildcard include/config/cfs/bandwidth.h) \
    $(wildcard include/config/cgroup/sched.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/linux/sem.h \
  include/linux/ipc.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ipcbuf.h \
  include/asm-generic/ipcbuf.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/sembuf.h \
  include/linux/signal.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/signal.h \
  include/asm-generic/signal-defs.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/proportions.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/seccomp.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/seccomp_32.h \
  include/linux/unistd.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/unistd.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/unistd_32.h \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/resource.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/resource.h \
  include/asm-generic/resource.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/hrtimer.h \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/timerfd.h) \
  include/linux/timerqueue.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/latencytop.h \
  include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
  include/linux/key.h \
  include/linux/sysctl.h \
  include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  include/linux/aio.h \
  include/linux/aio_abi.h \
  include/linux/uio.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/debugfs.h \
  include/linux/debugfs.h \
  /source/compat/backports-3.11-rc3-1/backport-include/linux/seq_file.h \
  include/linux/seq_file.h \
  include/linux/user_namespace.h \
  include/linux/nsproxy.h \

/source/compat/backports-3.11-rc3-1/compat/drivers-base-dma-buf.o: $(deps_/source/compat/backports-3.11-rc3-1/compat/drivers-base-dma-buf.o)

$(deps_/source/compat/backports-3.11-rc3-1/compat/drivers-base-dma-buf.o):

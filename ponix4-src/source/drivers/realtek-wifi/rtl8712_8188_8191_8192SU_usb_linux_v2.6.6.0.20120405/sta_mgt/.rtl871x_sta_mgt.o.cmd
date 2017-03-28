cmd_/source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/sta_mgt/rtl871x_sta_mgt.o := gcc -Wp,-MD,/source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/sta_mgt/.rtl871x_sta_mgt.o.d  -nostdinc -isystem /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include -I/source/kernel-3.2.12/linux-3.2.12/arch/x86/include -Iarch/x86/include/generated -Iinclude  -include /source/kernel-3.2.12/linux-3.2.12/include/linux/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -m32 -msoft-float -mregparm=3 -freg-struct-return -mpreferred-stack-boundary=2 -march=i486 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -O1 -Wno-unused-variable -Wno-unused-value -Wno-unused-label -Wno-unused-parameter -Wno-uninitialized -I/source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include -Wno-unused -Wno-unused-function -DCONFIG_LITTLE_ENDIAN  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(rtl871x_sta_mgt)"  -D"KBUILD_MODNAME=KBUILD_STR(8712u)" -c -o /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/sta_mgt/rtl871x_sta_mgt.o /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/sta_mgt/rtl871x_sta_mgt.c

source_/source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/sta_mgt/rtl871x_sta_mgt.o := /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/sta_mgt/rtl871x_sta_mgt.c

deps_/source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/sta_mgt/rtl871x_sta_mgt.o := \
    $(wildcard include/config/ap/mode.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/drv_conf.h \
    $(wildcard include/config/android.h) \
    $(wildcard include/config/platform/android.h) \
    $(wildcard include/config/validate/ssid.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/autoconf.h \
    $(wildcard include/config/debug/rtl871x.h) \
    $(wildcard include/config/usb/hci.h) \
    $(wildcard include/config/sdio/hci.h) \
    $(wildcard include/config/rtl8711.h) \
    $(wildcard include/config/rtl8712.h) \
    $(wildcard include/config/rtl8716.h) \
    $(wildcard include/config/ioctl/cfg80211.h) \
    $(wildcard include/config/cfg80211/force/compatible/2/6/37/under.h) \
    $(wildcard include/config/little/endian.h) \
    $(wildcard include/config/big/endian.h) \
    $(wildcard include/config/pwrctrl.h) \
    $(wildcard include/config/h2clbk.h) \
    $(wildcard include/config/mp/included.h) \
    $(wildcard include/config/embedded/fwimg.h) \
    $(wildcard include/config/r871x/test.h) \
    $(wildcard include/config/drvext.h) \
    $(wildcard include/config/debug/rtl8712.h) \
    $(wildcard include/config/xmit/enqueue.h) \
    $(wildcard include/config/xmit/direct.h) \
    $(wildcard include/config/80211n/ht.h) \
    $(wildcard include/config/recv/reordering/ctrl.h) \
    $(wildcard include/config/xmit/bh.h) \
    $(wildcard include/config/skb/copy.h) \
    $(wildcard include/config/recv/tasklet.h) \
    $(wildcard include/config/recv/bh.h) \
    $(wildcard include/config/prealloc/recv/skb.h) \
    $(wildcard include/config/r8712/test.h) \
    $(wildcard include/config/mlme/ext.h) \
    $(wildcard include/config/hostapd/mode.h) \
    $(wildcard include/config/event/thread/mode.h) \
    $(wildcard include/config/r8712/test/asta.h) \
    $(wildcard include/config/r8712/test/bsta.h) \
    $(wildcard include/config/rtl8712/tcp/csum/offload/rx.h) \
    $(wildcard include/config/reduce/usb/tx/int.h) \
    $(wildcard include/config/minimal/memory/usage.h) \
    $(wildcard include/config/usb/tx/aggregation.h) \
    $(wildcard include/config/usb/rx/aggregation.h) \
    $(wildcard include/config/crypto/internal.h) \
    $(wildcard include/config/internal/aes.h) \
    $(wildcard include/config/internal/sha1.h) \
    $(wildcard include/config/internal/md5.h) \
    $(wildcard include/config/internal/sha256.h) \
    $(wildcard include/config/internal/libtommath.h) \
    $(wildcard include/config/no/stdout/debug.h) \
    $(wildcard include/config/eth/intf.h) \
    $(wildcard include/config/wsc/crypto/test.h) \
    $(wildcard include/config/wsc/crypto/dh/test.h) \
    $(wildcard include/config/wsc/crypto/sha256/test.h) \
    $(wildcard include/config/wsc/crypto/aes128/test.h) \
    $(wildcard include/config/drvext/module/wsc.h) \
    $(wildcard include/config/wsccrypto/prealloc.h) \
    $(wildcard include/config/wsc/debug.h) \
    $(wildcard include/config/debug/wsc.h) \
    $(wildcard include/config/dump/wsc/key.h) \
    $(wildcard include/config/debug/wpa.h) \
    $(wildcard include/config/dump/wpa/key.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/osdep_service.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/basic_types.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/types.h \
  include/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
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
  include/linux/version.h \
  include/linux/spinlock.h \
    $(wildcard include/config/smp.h) \
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
    $(wildcard include/config/compat.h) \
  include/linux/bitops.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/bitops.h \
    $(wildcard include/config/x86/cmov.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/alternative.h \
    $(wildcard include/config/paravirt.h) \
  include/linux/stringify.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/asm.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/x86/invlpg.h) \
    $(wildcard include/config/x86/64.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/required-features.h \
    $(wildcard include/config/x86/minimum/cpu/family.h) \
    $(wildcard include/config/math/emulation.h) \
    $(wildcard include/config/x86/pae.h) \
    $(wildcard include/config/x86/cmpxchg64.h) \
    $(wildcard include/config/x86/use/3dnow.h) \
    $(wildcard include/config/x86/p6/nop.h) \
  include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
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
  include/linux/byteorder/generic.h \
  include/asm-generic/bitops/ext2-atomic-setbit.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/debug/stack/usage.h) \
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
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/asm-generic/ptrace.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/math_emu.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/sigcontext.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/current.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/percpu.h \
    $(wildcard include/config/x86/64/smp.h) \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  include/linux/linkage.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/printk.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  include/linux/dynamic_debug.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
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
  include/linux/ioctl.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cpumask.h \
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
  include/linux/math64.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/div64.h \
  include/linux/err.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ftrace.h \
    $(wildcard include/config/function/tracer.h) \
    $(wildcard include/config/dynamic/ftrace.h) \
  include/linux/atomic.h \
    $(wildcard include/config/arch/has/atomic/or.h) \
    $(wildcard include/config/generic/atomic64.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/atomic.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/atomic64_32.h \
  include/asm-generic/atomic-long.h \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  include/linux/spinlock_types_up.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/prove/rcu.h) \
  include/linux/rwlock_types.h \
  include/linux/spinlock_up.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_up.h \
  include/linux/semaphore.h \
  include/linux/sched.h \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/no/hz.h) \
    $(wildcard include/config/lockup/detector.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/virt/cpu/accounting.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/fanotify.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/generic/hardirqs.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/irq/time/accounting.h) \
    $(wildcard include/config/cfs/bandwidth.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/cgroup/sched.h) \
    $(wildcard include/config/mm/owner.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  include/linux/capability.h \
  include/linux/timex.h \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/seqlock.h \
  include/linux/param.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/timex.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/tsc.h \
    $(wildcard include/config/x86/tsc.h) \
  include/linux/jiffies.h \
  include/linux/rbtree.h \
  include/linux/nodemask.h \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/cmpxchg/local.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mmu/notifier.h) \
    $(wildcard include/config/transparent/hugepage.h) \
  include/linux/auxvec.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/auxvec.h \
  include/linux/prio_tree.h \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/rwsem.h \
  include/linux/completion.h \
  include/linux/wait.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/mmu.h \
  include/linux/mutex.h \
    $(wildcard include/config/have/arch/mutex/cpu/relax.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/cputime.h \
  include/asm-generic/cputime.h \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  include/linux/sem.h \
  include/linux/ipc.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ipcbuf.h \
  include/asm-generic/ipcbuf.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/sembuf.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/rcu/torture/test.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/tiny/preempt/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/preempt/rt.h) \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/rcutiny.h \
  include/linux/signal.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/signal.h \
  include/asm-generic/signal-defs.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/linux/pid.h \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
  include/linux/pfn.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/sched/book.h) \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/generated/bounds.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
  include/linux/notifier.h \
  include/linux/srcu.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/topology.h \
    $(wildcard include/config/x86/ht.h) \
  include/asm-generic/topology.h \
  include/linux/proportions.h \
  include/linux/percpu_counter.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/seccomp.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/seccomp_32.h \
  include/linux/unistd.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/unistd.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/unistd_32.h \
  include/linux/rculist.h \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/resource.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/resource.h \
  include/asm-generic/resource.h \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/timerfd.h) \
  include/linux/timerqueue.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/latencytop.h \
  include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/user/ns.h) \
  include/linux/key.h \
    $(wildcard include/config/sysctl.h) \
  include/linux/sysctl.h \
  include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  include/linux/aio.h \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
  include/linux/aio_abi.h \
  include/linux/uio.h \
  include/linux/netdevice.h \
    $(wildcard include/config/dcb.h) \
    $(wildcard include/config/wlan.h) \
    $(wildcard include/config/ax25.h) \
    $(wildcard include/config/mac80211/mesh.h) \
    $(wildcard include/config/tr.h) \
    $(wildcard include/config/net/ipip.h) \
    $(wildcard include/config/net/ipgre.h) \
    $(wildcard include/config/ipv6/sit.h) \
    $(wildcard include/config/ipv6/tunnel.h) \
    $(wildcard include/config/netpoll.h) \
    $(wildcard include/config/rps.h) \
    $(wildcard include/config/xps.h) \
    $(wildcard include/config/rfs/accel.h) \
    $(wildcard include/config/net/poll/controller.h) \
    $(wildcard include/config/fcoe.h) \
    $(wildcard include/config/libfcoe.h) \
    $(wildcard include/config/wireless/ext.h) \
    $(wildcard include/config/vlan/8021q.h) \
    $(wildcard include/config/net/dsa.h) \
    $(wildcard include/config/net/ns.h) \
    $(wildcard include/config/net/dsa/tag/dsa.h) \
    $(wildcard include/config/net/dsa/tag/trailer.h) \
    $(wildcard include/config/netpoll/trap.h) \
  include/linux/if.h \
  include/linux/socket.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/socket.h \
  include/asm-generic/socket.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/sockios.h \
  include/asm-generic/sockios.h \
  include/linux/sockios.h \
  include/linux/hdlc/ioctl.h \
  include/linux/if_ether.h \
  include/linux/skbuff.h \
    $(wildcard include/config/nf/conntrack.h) \
    $(wildcard include/config/bridge/netfilter.h) \
    $(wildcard include/config/nf/defrag/ipv4.h) \
    $(wildcard include/config/nf/defrag/ipv6.h) \
    $(wildcard include/config/xfrm.h) \
    $(wildcard include/config/net/sched.h) \
    $(wildcard include/config/net/cls/act.h) \
    $(wildcard include/config/ipv6/ndisc/nodetype.h) \
    $(wildcard include/config/net/dma.h) \
    $(wildcard include/config/network/secmark.h) \
    $(wildcard include/config/network/phy/timestamping.h) \
  include/linux/kmemcheck.h \
  include/linux/net.h \
  include/linux/random.h \
    $(wildcard include/config/arch/random.h) \
  include/linux/irqnr.h \
  include/linux/fcntl.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/fcntl.h \
  include/asm-generic/fcntl.h \
  include/linux/textsearch.h \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/failslab.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/debug/slab.h) \
    $(wildcard include/config/slab.h) \
  include/linux/gfp.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
  include/linux/slub_def.h \
    $(wildcard include/config/slub/stats.h) \
    $(wildcard include/config/slub/debug.h) \
    $(wildcard include/config/sysfs.h) \
  include/linux/kobject.h \
  include/linux/sysfs.h \
  include/linux/kobject_ns.h \
  include/linux/kref.h \
  include/linux/kmemleak.h \
    $(wildcard include/config/debug/kmemleak.h) \
  include/net/checksum.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/x86/wp/works/ok.h) \
    $(wildcard include/config/x86/intel/usercopy.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/uaccess_32.h \
    $(wildcard include/config/debug/strict/user/copy/checks.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/checksum.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/checksum_32.h \
  include/linux/in6.h \
  include/linux/dmaengine.h \
    $(wildcard include/config/async/tx/enable/channel/switch.h) \
    $(wildcard include/config/dma/engine.h) \
    $(wildcard include/config/async/tx/dma.h) \
  include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pm.h \
    $(wildcard include/config/pm.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/pm/runtime.h) \
    $(wildcard include/config/pm/clk.h) \
    $(wildcard include/config/pm/generic/domains.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/device.h \
    $(wildcard include/config/acpi.h) \
    $(wildcard include/config/intel/iommu.h) \
    $(wildcard include/config/amd/iommu.h) \
  include/linux/pm_wakeup.h \
  include/linux/dma-direction.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/debug/sg.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/scatterlist.h \
  include/asm-generic/scatterlist.h \
    $(wildcard include/config/need/sg/dma/length.h) \
  include/linux/mm.h \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ksm.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/hugetlbfs.h) \
  include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  include/linux/range.h \
  include/linux/bit_spinlock.h \
  include/linux/shrinker.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/pgtable.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/e820.h \
    $(wildcard include/config/efi.h) \
    $(wildcard include/config/intel/txt.h) \
    $(wildcard include/config/memtest.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/x86_init.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/bootparam.h \
  include/linux/screen_info.h \
  include/linux/apm_bios.h \
  include/linux/edd.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/ist.h \
  include/video/edid.h \
    $(wildcard include/config/x86.h) \
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
    $(wildcard include/config/s390.h) \
  include/linux/huge_mm.h \
  include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  include/linux/vm_event_item.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/have/dma/attrs.h) \
    $(wildcard include/config/need/dma/map/state.h) \
  include/linux/dma-attrs.h \
  include/linux/bug.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/dma-mapping.h \
    $(wildcard include/config/isa.h) \
  include/linux/dma-debug.h \
    $(wildcard include/config/dma/api/debug.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/swiotlb.h \
    $(wildcard include/config/swiotlb.h) \
  include/linux/swiotlb.h \
  include/asm-generic/dma-coherent.h \
    $(wildcard include/config/have/generic/dma/coherent.h) \
  include/asm-generic/dma-mapping-common.h \
  include/linux/if_packet.h \
  include/linux/if_link.h \
  include/linux/netlink.h \
  include/linux/pm_qos.h \
  include/linux/miscdevice.h \
  include/linux/major.h \
  include/linux/delay.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/ethtool.h \
  include/linux/compat.h \
  include/net/net_namespace.h \
    $(wildcard include/config/ipv6.h) \
    $(wildcard include/config/ip/dccp.h) \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/wext/core.h) \
    $(wildcard include/config/net.h) \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/xfrm/statistics.h) \
  include/net/snmp.h \
  include/linux/snmp.h \
  include/linux/u64_stats_sync.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/ip/multiple/tables.h) \
    $(wildcard include/config/ip/mroute.h) \
    $(wildcard include/config/ip/mroute/multiple/tables.h) \
  include/net/inet_frag.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/ipv6/multiple/tables.h) \
    $(wildcard include/config/ipv6/mroute.h) \
    $(wildcard include/config/ipv6/mroute/multiple/tables.h) \
  include/net/dst_ops.h \
  include/net/netns/dccp.h \
  include/net/netns/x_tables.h \
    $(wildcard include/config/bridge/nf/ebtables.h) \
  include/linux/netfilter.h \
    $(wildcard include/config/netfilter/debug.h) \
    $(wildcard include/config/nf/nat/needed.h) \
  include/linux/in.h \
  include/net/flow.h \
  include/linux/proc_fs.h \
    $(wildcard include/config/proc/devicetree.h) \
    $(wildcard include/config/proc/kcore.h) \
  include/linux/fs.h \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  include/linux/limits.h \
  include/linux/blk_types.h \
    $(wildcard include/config/blk/dev/integrity.h) \
  include/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/path.h \
  include/linux/stat.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/stat.h \
  include/linux/radix-tree.h \
  include/linux/fiemap.h \
  include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/inet.h \
  include/linux/magic.h \
  include/net/netns/xfrm.h \
  include/linux/xfrm.h \
  include/linux/seq_file_net.h \
  include/linux/seq_file.h \
  include/net/dsa.h \
  include/linux/etherdevice.h \
    $(wildcard include/config/have/efficient/unaligned/access.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/unaligned.h \
  include/linux/unaligned/access_ok.h \
  include/linux/unaligned/generic.h \
  include/net/iw_handler.h \
    $(wildcard include/config/wext/priv.h) \
  include/linux/wireless.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_byteorder.h \
    $(wildcard include/config/platform/mstar389.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/byteorder/little_endian.h \
  include/net/ieee80211_radiotap.h \
  include/net/cfg80211.h \
    $(wildcard include/config/nl80211/testmode.h) \
    $(wildcard include/config/cfg80211/wext.h) \
  include/linux/debugfs.h \
    $(wildcard include/config/debug/fs.h) \
  include/linux/nl80211.h \
  include/linux/ieee80211.h \
  include/net/regulatory.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/drv_types.h \
    $(wildcard include/config/1t1r.h) \
    $(wildcard include/config/2t2r.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/wlan_bssdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_syscfg_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_cmdctrl_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_macsetting_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_timectrl_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_fifoctrl_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_ratectrl_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_edcasetting_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_wmac_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_security_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_powersave_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_gp_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_interrupt_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_debugctrl_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_offload_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_cmdctrl_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_syscfg_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_macsetting_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_timectrl_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_fifoctrl_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_ratectrl_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_edcasetting_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_wmac_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_security_bitdef.h \
    $(wildcard include/config/msk.h) \
    $(wildcard include/config/sht.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_powersave_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_gp_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_interrupt_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_debugctrl_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/ioreg_def/rtl8712_offload_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/sdio_reg/rtl8712_sdio_bitdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/sdio_reg/rtl8712_sdio_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_spec/sdio_reg/rtl8712_sdio_regdef.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/basic_types.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/drv_types_linux.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_ht.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/wifi.h \
    $(wildcard include/config/rtl8712fw.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_cmd.h \
    $(wildcard include/config/rtl8711fw.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_rf.h \
    $(wildcard include/config/.h) \
    $(wildcard include/config/1t.h) \
    $(wildcard include/config/2t.h) \
    $(wildcard include/config/1r.h) \
    $(wildcard include/config/2r.h) \
    $(wildcard include/config/1t2r.h) \
    $(wildcard include/config/turbo.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_rf.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_mp_phy_regdef.h \
  include/linux/interrupt.h \
    $(wildcard include/config/irq/forced/threading.h) \
    $(wildcard include/config/generic/irq/probe.h) \
  include/linux/irqreturn.h \
  include/linux/hardirq.h \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/x86/thermal/vector.h) \
    $(wildcard include/config/x86/mce/threshold.h) \
  include/linux/irq.h \
    $(wildcard include/config/irq/release/method.h) \
    $(wildcard include/config/generic/pending/irq.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/irq.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/irq_vectors.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/irq_regs.h \
  include/linux/irqdesc.h \
    $(wildcard include/config/irq/preflow/fasteoi.h) \
    $(wildcard include/config/sparse/irq.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/hw_irq.h \
    $(wildcard include/config/irq/remap.h) \
  include/linux/profile.h \
    $(wildcard include/config/profiling.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/sections.h \
    $(wildcard include/config/debug/rodata.h) \
  include/asm-generic/sections.h \
  include/trace/events/irq.h \
  include/linux/tracepoint.h \
    $(wildcard include/config/tracepoints.h) \
  include/linux/jump_label.h \
    $(wildcard include/config/jump/label.h) \
  include/trace/define_trace.h \
    $(wildcard include/config/event/tracing.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/ieee80211.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_cmd.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_xmit.h \
    $(wildcard include/config/rtl8712/tcp/csum/offload/tx.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/xmit_osdep.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_xmit.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_recv.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_recv.h \
    $(wildcard include/config/platform/mt53xx.h) \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_qos.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_security.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_pwrctrl.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_io.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/osdep_intf.h \
  include/linux/usb.h \
    $(wildcard include/config/usb/devicefs.h) \
    $(wildcard include/config/usb/mon.h) \
    $(wildcard include/config/usb/device/class.h) \
    $(wildcard include/config/usb/suspend.h) \
  include/linux/mod_devicetable.h \
  include/linux/usb/ch9.h \
    $(wildcard include/config/size.h) \
    $(wildcard include/config/att/one.h) \
    $(wildcard include/config/att/selfpower.h) \
    $(wildcard include/config/att/wakeup.h) \
    $(wildcard include/config/att/battery.h) \
  include/linux/pm_runtime.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_eeprom.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/sta_info.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/wifi.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_mlme.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_mp.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_debug.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_event.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_event.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/hal_init.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl8712_hal.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/rtl871x_led.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/ioctl_cfg80211.h \
  /source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/include/recv_osdep.h \

/source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/sta_mgt/rtl871x_sta_mgt.o: $(deps_/source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/sta_mgt/rtl871x_sta_mgt.o)

$(deps_/source/drivers/realtek-wifi/rtl8712_8188_8191_8192SU_usb_linux_v2.6.6.0.20120405/sta_mgt/rtl871x_sta_mgt.o):

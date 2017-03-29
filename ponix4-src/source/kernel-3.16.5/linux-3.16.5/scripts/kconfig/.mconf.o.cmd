cmd_scripts/kconfig/mconf.o := gcc -Wp,-MD,scripts/kconfig/.mconf.o.d -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer   -DCURSES_LOC="<ncurses.h>" -DLOCALE   -c -o scripts/kconfig/mconf.o scripts/kconfig/mconf.c

source_scripts/kconfig/mconf.o := scripts/kconfig/mconf.c

deps_scripts/kconfig/mconf.o := \
    $(wildcard include/config/mode.h) \
    $(wildcard include/config/color.h) \
    $(wildcard include/config/.h) \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/ctype.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/features.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/predefs.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/cdefs.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/wordsize.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/gnu/stubs.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/gnu/stubs-32.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/typesizes.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/endian.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/endian.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/byteswap.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/xlocale.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/errno.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/errno.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/linux/errno.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm/errno.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm-generic/errno.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm-generic/errno-base.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/fcntl.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/fcntl.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/time.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stddef.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/select.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/select.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/sigset.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/time.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/sysmacros.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/pthreadtypes.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/stat.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include-fixed/limits.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include-fixed/syslimits.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/limits.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/posix1_lim.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/local_lim.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/linux/limits.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/posix2_lim.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stdarg.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/stdlib.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/waitflags.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/waitstatus.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/alloca.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/string.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/string.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/string2.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/signal.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/signum.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/siginfo.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/sigaction.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/sigcontext.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm/sigcontext.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/linux/types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm/types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm-generic/types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm-generic/int-ll64.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm/bitsperlong.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm-generic/bitsperlong.h \
    $(wildcard include/config/64bit.h) \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/linux/posix_types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/linux/stddef.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm/posix_types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm/posix_types_32.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/sigstack.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/ucontext.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/sigthread.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/unistd.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/posix_opt.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/environments.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/confname.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/getopt.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/locale.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/locale.h \
  scripts/kconfig/lkc.h \
    $(wildcard include/config/prefix.h) \
    $(wildcard include/config/list.h) \
    $(wildcard include/config/y.h) \
  scripts/kconfig/expr.h \
    $(wildcard include/config/config.h) \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/assert.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/stdio.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/libio.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/_G_config.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/wchar.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/stdio_lim.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/sys_errlist.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/stdio.h \
  scripts/kconfig/list.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stdbool.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/libintl.h \
  scripts/kconfig/lkc_proto.h \
  scripts/kconfig/lxdialog/dialog.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/ncurses.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/ncurses_dll.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/unctrl.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/curses.h \

scripts/kconfig/mconf.o: $(deps_scripts/kconfig/mconf.o)

$(deps_scripts/kconfig/mconf.o):

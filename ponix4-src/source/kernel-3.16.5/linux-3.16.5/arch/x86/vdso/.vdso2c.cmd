cmd_arch/x86/vdso/vdso2c := gcc -Wp,-MD,arch/x86/vdso/.vdso2c.d -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer   -I./tools/include  -o arch/x86/vdso/vdso2c arch/x86/vdso/vdso2c.c  

source_arch/x86/vdso/vdso2c := arch/x86/vdso/vdso2c.c

deps_arch/x86/vdso/vdso2c := \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/inttypes.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/features.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/predefs.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/cdefs.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/wordsize.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/gnu/stubs.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/gnu/stubs-32.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stdint.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/stdint.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/wchar.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/unistd.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/posix_opt.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/environments.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/typesizes.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stddef.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/confname.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/getopt.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stdarg.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/stdlib.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/waitflags.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/waitstatus.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/endian.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/endian.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/byteswap.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/time.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/select.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/select.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/sigset.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/time.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/sysmacros.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/pthreadtypes.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/alloca.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/stdio.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/libio.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/_G_config.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/wchar.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/stdio_lim.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/sys_errlist.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/stdio.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/string.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/xlocale.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/string.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/string2.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/fcntl.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/fcntl.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/stat.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/err.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/mman.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/mman.h \
  tools/include/tools/le_byteshift.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/linux/elf.h \
  tools/include/linux/types.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stdbool.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm/types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm-generic/types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm-generic/int-ll64.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm/bitsperlong.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm-generic/bitsperlong.h \
    $(wildcard include/config/64bit.h) \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/linux/elf-em.h \
  arch/x86/vdso/vdso2c.h \

arch/x86/vdso/vdso2c: $(deps_arch/x86/vdso/vdso2c)

$(deps_arch/x86/vdso/vdso2c):

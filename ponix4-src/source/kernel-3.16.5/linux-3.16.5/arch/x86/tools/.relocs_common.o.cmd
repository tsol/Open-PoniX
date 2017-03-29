cmd_arch/x86/tools/relocs_common.o := gcc -Wp,-MD,arch/x86/tools/.relocs_common.o.d -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer   -I./tools/include  -c -o arch/x86/tools/relocs_common.o arch/x86/tools/relocs_common.c

source_arch/x86/tools/relocs_common.o := arch/x86/tools/relocs_common.c

deps_arch/x86/tools/relocs_common.o := \
  arch/x86/tools/relocs.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/stdio.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/features.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/predefs.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/sys/cdefs.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/wordsize.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/gnu/stubs.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/gnu/stubs-32.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stddef.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/types.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/typesizes.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/libio.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/_G_config.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/wchar.h \
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stdarg.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/stdio_lim.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/sys_errlist.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/stdio.h \
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
  /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include/stdint.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/stdint.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/wchar.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/inttypes.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/string.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/xlocale.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/string.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/string2.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/errno.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/errno.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/linux/errno.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm/errno.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm-generic/errno.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/asm-generic/errno-base.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/unistd.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/posix_opt.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/environments.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/bits/confname.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/getopt.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/elf.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/byteswap.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/regex.h \
  /toolchain/i486-TSOL-linux-gnu/sysroot/usr/include/gnu/option-groups.h \
  tools/include/tools/le_byteshift.h \

arch/x86/tools/relocs_common.o: $(deps_arch/x86/tools/relocs_common.o)

$(deps_arch/x86/tools/relocs_common.o):

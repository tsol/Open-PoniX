/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* TTY to use in boot mode */
#define BOOT_TTY "tty62"

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <ncursesw/term.h> header file. */
/* #undef HAVE_NCURSESW_TERM_H */

/* Define to 1 if you have the <ncurses/term.h> header file. */
/* #undef HAVE_NCURSES_TERM_H */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <term.h> header file. */
/* #undef HAVE_TERM_H */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "plymouth"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://bugs.freedesktop.org/enter_bug.cgi?product=plymouth"

/* Define to the full name of this package. */
#define PACKAGE_NAME "plymouth"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "plymouth 0.8.8"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "plymouth"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.8.8"

/* Enable deprecated smooth transition to GDM */
/* #undef PLY_ENABLE_DEPRECATED_GDM_TRANSITION */

/* Enable support for libdrm_intel driver */
#define PLY_ENABLE_LIBDRM_INTEL 1

/* Enable support for libdrm_nouveau driver */
/* #undef PLY_ENABLE_LIBDRM_NOUVEAU */

/* Enable support for libdrm_radeon driver */
#define PLY_ENABLE_LIBDRM_RADEON 1

/* Enable support for libkms abstraction over drm drivers */
#define PLY_ENABLE_LIBKMS 1

/* Coordinate boot up with systemd */
/* #undef PLY_ENABLE_SYSTEMD_INTEGRATION */

/* Build in verbose debug tracing spew */
#define PLY_ENABLE_TRACING 1

/* Release file path */
#define RELEASE_FILE "/etc/system-release"

/* TTY to use in shutdown mode */
#define SHUTDOWN_TTY "/dev/tty63"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Version number of package */
#define VERSION "0.8.8"

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

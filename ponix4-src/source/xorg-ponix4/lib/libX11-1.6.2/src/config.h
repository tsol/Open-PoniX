/* src/config.h.  Generated from config.h.in by configure.  */
/* src/config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if `struct sockaddr_in' has a `sin_len' member */
/* #undef BSD44SOCKETS */

/* Include compose table cache support */
#define COMPOSECACHE 1

/* Has getresuid() & getresgid() functions */
#define HASGETRESUID 1

/* Has issetugid() function */
/* #undef HASSETUGID */

/* Has shm*() functions */
#define HAS_SHM 1

/* Define to 1 if you have the `authdes_create' function. */
/* #undef HAVE_AUTHDES_CREATE */

/* Define to 1 if you have the `authdes_seccreate' function. */
/* #undef HAVE_AUTHDES_SECCREATE */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Use dlopen to load shared libraries */
#define HAVE_DLOPEN 1

/* Define to 1 if you have the <dl.h> header file. */
/* #undef HAVE_DL_H */

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* launchd support available */
/* #undef HAVE_LAUNCHD */

/* Define to 1 if you have the `ws2_32' library (-lws2_32). */
/* #undef HAVE_LIBWS2_32 */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define to 1 if you have the `seteuid' function. */
#define HAVE_SETEUID 1

/* Use shl_load to load shared libraries */
/* #undef HAVE_SHL_LOAD */

/* Define to 1 if the system has the type `socklen_t'. */
#define HAVE_SOCKLEN_T 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtol' function. */
#define HAVE_STRTOL 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Support IPv6 for TCP connections */
#define IPv6 1

/* Support os-specific local connections */
/* #undef LOCALCONN */

/* preference sorted list of transport types to try for local connections */
#define LOCAL_TRANSPORT_LIST LOCAL_TRANS,UNIX_TRANS,TCP_TRANS

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Disable XLOCALEDIR environment variable */
/* #undef NO_XLOCALEDIR */

/* Name of package */
#define PACKAGE "libX11"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://bugs.freedesktop.org/enter_bug.cgi?product=xorg"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libX11"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libX11 1.6.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libX11"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.6.2"

/* Major version of this package */
#define PACKAGE_VERSION_MAJOR 1

/* Minor version of this package */
#define PACKAGE_VERSION_MINOR 6

/* Patch version of this package */
#define PACKAGE_VERSION_PATCHLEVEL 2

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Support Secure RPC ("SUN-DES-1") authentication for X11 clients */
/* #undef SECURE_RPC */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Support TCP socket connections */
#define TCPCONN 1

/* launchd support available */
/* #undef TRANS_REOPEN */

/* Support UNIX socket connections */
#define UNIXCONN 1

/* Split some i18n functions into loadable modules */
/* #undef USE_DYNAMIC_LC */

/* Use the X cursor library to load cursors */
#define USE_DYNAMIC_XCURSOR 1

/* poll() function is available */
#define USE_POLL 1

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
#define VERSION "1.6.2"

/* Location of libX11 data */
#define X11_DATADIR "/usr/xorg/share/X11"

/* Location of libX11 library data */
#define X11_LIBDIR "/usr/xorg/lib/X11"

/* Include support for XCMS */
#define XCMS 1

/* Location of error message database */
#define XERRORDB "/usr/xorg/share/X11/XErrorDB"

/* Enable XF86BIGFONT extension */
#define XF86BIGFONT 1

/* Use XKB */
#define XKB 1

/* Location of keysym database */
#define XKEYSYMDB "/usr/xorg/share/X11/XKeysymDB"

/* support for X Locales */
#define XLOCALE 1

/* Location of libX11 locale data */
#define XLOCALEDATADIR "/usr/xorg/share/X11/locale"

/* Location of libX11 locale data */
#define XLOCALEDIR "/usr/xorg/share/X11/locale"

/* Location of libX11 locale libraries */
#define XLOCALELIBDIR "/usr/xorg/lib/X11/locale"

/* Whether libX11 is compiled with thread support */
#define XTHREADS 1

/* Whether libX11 needs to use MT safe API's */
#define XUSE_MTSAFE_API 1

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

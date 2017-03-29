/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* poll doesn't work on devices */
/* #undef BROKEN_POLL */

/* Directory for installing the binaries */
#define DBUS_BINDIR "/usr/bin"

/* Define to build test code into the library and binaries */
/* #undef DBUS_BUILD_TESTS */

/* Define to build X11 functionality */
#define DBUS_BUILD_X11 1

/* whether -export-dynamic was passed to libtool */
/* #undef DBUS_BUILT_R_DYNAMIC */

/* Use dnotify on Linux */
/* #undef DBUS_BUS_ENABLE_DNOTIFY_ON_LINUX */

/* Use inotify */
#define DBUS_BUS_ENABLE_INOTIFY 1

/* Use kqueue */
/* #undef DBUS_BUS_ENABLE_KQUEUE */

/* Directory to check for console ownerhip */
#define DBUS_CONSOLE_AUTH_DIR "/var/run/console/"

/* File to check for console ownerhip */
#define DBUS_CONSOLE_OWNER_FILE ""

/* Defined if we run on a cygwin API based system */
/* #undef DBUS_CYGWIN */

/* Directory for installing the DBUS daemon */
#define DBUS_DAEMONDIR "/usr/bin"

/* Name of executable */
#define DBUS_DAEMON_NAME "dbus-daemon"

/* Directory for installing DBUS data files */
#define DBUS_DATADIR "/usr/share"

/* Disable assertion checking */
#define DBUS_DISABLE_ASSERT 1

/* Disable public API sanity checking */
/* #undef DBUS_DISABLE_CHECKS */

/* Define to build test code into the library and binaries */
/* #undef DBUS_ENABLE_EMBEDDED_TESTS */

/* Use launchd autolaunch */
/* #undef DBUS_ENABLE_LAUNCHD */

/* Define to build independent test binaries */
/* #undef DBUS_ENABLE_MODULAR_TESTS */

/* Define to enable bus daemon usage statistics */
/* #undef DBUS_ENABLE_STATS */

/* Build with caching of user data */
#define DBUS_ENABLE_USERDB_CACHE 1

/* Support a verbose mode */
/* #undef DBUS_ENABLE_VERBOSE_MODE */

/* Define to enable X11 auto-launch */
#define DBUS_ENABLE_X11_AUTOLAUNCH 1

/* Extension for executables, typically empty or .exe */
#define DBUS_EXEEXT ""

/* Defined if gcov is enabled to force a rebuild due to config.h changing */
/* #undef DBUS_GCOV_ENABLED */

/* Define to use epoll(4) on Linux */
#define DBUS_HAVE_LINUX_EPOLL 1

/* Define to printf modifier for 64 bit integer type */
#define DBUS_INT64_PRINTF_MODIFIER "ll"

/* Directory for installing the libexec binaries */
#define DBUS_LIBEXECDIR "/usr/libexec"

/* Prefix for installing DBUS */
#define DBUS_PREFIX "/usr"

/* Where per-session bus puts its sockets */
#define DBUS_SESSION_SOCKET_DIR "/var/log/dbus"

/* The default D-Bus address of the system bus */
#define DBUS_SYSTEM_BUS_DEFAULT_ADDRESS "unix:path=/usr/var/run/dbus/system_bus_socket"

/* The name of the socket the system bus listens on by default */
#define DBUS_SYSTEM_SOCKET "/usr/var/run/dbus/system_bus_socket"

/* Full path to the daemon in the builddir */
#define DBUS_TEST_EXEC "/source/browser/dbus-dbus-1.6/test"

/* Full path to the launch helper test program in the builddir */
#define DBUS_TEST_LAUNCH_HELPER_BINARY "/source/browser/dbus-dbus-1.6/bus/dbus-daemon-launch-helper-test"

/* Where to put test sockets */
#define DBUS_TEST_SOCKET_DIR "/tmp"

/* Defined if we run on a Unix-based system */
#define DBUS_UNIX 1

/* User for running the system BUS daemon */
#define DBUS_USER "messagebus"

/* Use the gcc __sync extension */
#define DBUS_USE_SYNC 1

/* A 'va_copy' style function */
#define DBUS_VA_COPY va_copy

/* 'va_lists' cannot be copies as values */
#define DBUS_VA_COPY_AS_ARRAY 1

/* Defined if we run on a W32 API based system */
/* #undef DBUS_WIN */

/* Defined if we run on a W32 CE API based system */
/* #undef DBUS_WINCE */

/* Define if GLib, GObject, GIO are available */
#define DBUS_WITH_GLIB 1

/* The name of the gettext domain */
#define GETTEXT_PACKAGE "dbus-1"

/* Prevent post-2.32 APIs */
#define GLIB_VERSION_MAX_ALLOWED GLIB_VERSION_2_32

/* Ignore post-2.26 deprecations */
#define GLIB_VERSION_MIN_REQUIRED GLIB_VERSION_2_26

/* Disable GLib public API sanity checking */
/* #undef G_DISABLE_CHECKS */

/* Have abstract socket namespace */
#define HAVE_ABSTRACT_SOCKETS 1

/* Define to 1 if you have the `accept4' function. */
#define HAVE_ACCEPT4 1

/* Adt audit API */
/* #undef HAVE_ADT */

/* Define to 1 if you have the <alloca.h> header file. */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the `backtrace' function. */
#define HAVE_BACKTRACE 1

/* Define to 1 if you have the <byteswap.h> header file. */
#define HAVE_BYTESWAP_H 1

/* Define to 1 if you have the `clearenv' function. */
#define HAVE_CLEARENV 1

/* Have cmsgcred structure */
/* #undef HAVE_CMSGCRED */

/* Have console owner file */
/* #undef HAVE_CONSOLE_OWNER_FILE */

/* Define to 1 if you have the <crt_externs.h> header file. */
/* #undef HAVE_CRT_EXTERNS_H */

/* Have the ddfd member of DIR */
/* #undef HAVE_DDFD */

/* Define to 1 if you have the declaration of `LOG_PERROR', and to 0 if you
   don't. */
#define HAVE_DECL_LOG_PERROR 1

/* Define to 1 if you have the declaration of `MSG_NOSIGNAL', and to 0 if you
   don't. */
#define HAVE_DECL_MSG_NOSIGNAL 1

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Have dirfd function */
#define HAVE_DIRFD 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <execinfo.h> header file. */
#define HAVE_EXECINFO_H 1

/* Define to 1 if you have the <expat.h> header file. */
#define HAVE_EXPAT_H 1

/* Define to 1 if you have the `fpathconf' function. */
#define HAVE_FPATHCONF 1

/* Define to 1 if you have the `getgrouplist' function. */
#define HAVE_GETGROUPLIST 1

/* Define to 1 if you have the `getpeereid' function. */
/* #undef HAVE_GETPEEREID */

/* Define to 1 if you have the `getpeerucred' function. */
/* #undef HAVE_GETPEERUCRED */

/* Define to 1 if you have the `getresuid' function. */
#define HAVE_GETRESUID 1

/* Have GNU-style varargs macros */
#define HAVE_GNUC_VARARGS 1

/* Define to 1 if you have the `inotify_init1' function. */
#define HAVE_INOTIFY_INIT1 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Have ISO C99 varargs macros */
#define HAVE_ISO_VARARGS 1

/* Define to 1 if you have the `issetugid' function. */
/* #undef HAVE_ISSETUGID */

/* audit daemon SELinux support */
/* #undef HAVE_LIBAUDIT */

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if you have the `localeconv' function. */
#define HAVE_LOCALECONV 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if we have CLOCK_MONOTONIC */
#define HAVE_MONOTONIC_CLOCK 1

/* Define to 1 if you have the `nanosleep' function. */
#define HAVE_NANOSLEEP 1

/* Have non-POSIX function getpwnam_r */
/* #undef HAVE_NONPOSIX_GETPWNAM_R */

/* Define if your system needs _NSGetEnviron to set up the environment */
/* #undef HAVE_NSGETENVIRON */

/* Define to 1 if you have the `pipe2' function. */
#define HAVE_PIPE2 1

/* Define to 1 if you have the `poll' function. */
#define HAVE_POLL 1

/* Have POSIX function getpwnam_r */
#define HAVE_POSIX_GETPWNAM_R 1

/* SELinux support */
/* #undef HAVE_SELINUX */

/* Define to 1 if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define to 1 if you have the `setrlimit' function. */
#define HAVE_SETRLIMIT 1

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define to 1 if you have the `socketpair' function. */
#define HAVE_SOCKETPAIR 1

/* Have socklen_t type */
#define HAVE_SOCKLEN_T 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* Define to 1 if you have the `strtoull' function. */
#define HAVE_STRTOULL 1

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Have systemd */
/* #undef HAVE_SYSTEMD */

/* Define to 1 if you have the <sys/inotify.h> header file. */
#define HAVE_SYS_INOTIFY_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/syslimits.h> header file. */
/* #undef HAVE_SYS_SYSLIMITS_H */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/uio.h> header file. */
#define HAVE_SYS_UIO_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Supports sending UNIX file descriptors */
#define HAVE_UNIX_FD_PASSING 1

/* Define to 1 if you have the `unsetenv' function. */
#define HAVE_UNSETENV 1

/* Define to 1 if you have the `usleep' function. */
#define HAVE_USLEEP 1

/* Define to 1 if you have the `vasprintf' function. */
#define HAVE_VASPRINTF 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the `writev' function. */
#define HAVE_WRITEV 1

/* Define to 1 if you have the <ws2tcpip.h> header file. */
/* #undef HAVE_WS2TCPIP_H */

/* Define to 1 if you have the <wspiapi.h> header file. */
/* #undef HAVE_WSPIAPI_H */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "dbus"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://bugs.freedesktop.org/enter_bug.cgi?product=dbus"

/* Define to the full name of this package. */
#define PACKAGE_NAME "dbus"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "dbus 1.6.9"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "dbus"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.6.9"

/* The size of `char', as computed by sizeof. */
#define SIZEOF_CHAR 1

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 4

/* The size of `__int64', as computed by sizeof. */
#define SIZEOF___INT64 0

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Full path to the daemon in the builddir */
#define TEST_BUS_BINARY "/source/browser/dbus-dbus-1.6/bus/dbus-daemon"

/* Listening address for regression tests */
#define TEST_LISTEN "unix:tmpdir=/tmp"

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
#define VERSION "1.6.9"

/* Define to add Valgrind instrumentation */
/* #undef WITH_VALGRIND */

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif


			/* Use the compiler-provided endianness defines to allow universal compiling. */
			#if defined(__BIG_ENDIAN__)
			#define WORDS_BIGENDIAN 1
			#endif
		

/* Define to 1 if the X Window System is missing or not being used. */
/* #undef X_DISPLAY_MISSING */


#if defined(HAVE_NSGETENVIRON) && defined(HAVE_CRT_EXTERNS_H)
# include <sys/time.h>
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#endif


/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Defined to get newer W32 CE APIs */
/* #undef _WIN32_WCE */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

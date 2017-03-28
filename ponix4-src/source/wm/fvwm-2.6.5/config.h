/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Old AIX systems (3.2.5) don't define some common keysyms. */
/* #undef COMPAT_OLD_KEYSYMDEF */

/* Old libstroke <= 0.4 does not use STROKE_ prefix for constants. */
/* #undef COMPAT_OLD_LIBSTROKE */

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define to 1 if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Support fribidi-0.10.4 and older with "CHARSET" spelling. */
/* #undef FRIBIDI_CHARSET_SPELLING */

/* Suffix for old (to be deprecated) config filenames */
#define FVWM2RC ".fvwm2rc"

/* Produces a log of all executed commands and their times on stderr. */
/* #undef FVWM_COMMAND_LOG */

/* Name of config filenames in FVWM_USERDIR and FVWM_DATADIR */
#define FVWM_CONFIG "config"

/* if you would like to see lots of debug messages from fvwm, for debugging
   purposes, uncomment the next line */
/* #undef FVWM_DEBUG_MSGS */

/* Where to search for images. */
#define FVWM_IMAGEPATH "/usr/include/X11/bitmaps:/usr/include/X11/pixmaps"

/* Define if gdk-imlib is used */
#define GDK_IMLIB 1

/* Define to 1 if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the `atexit' function. */
#define HAVE_ATEXIT 1

/* Define if fribidi library is used. */
/* #undef HAVE_BIDI */

/* Define to 1 if you have the `bindtextdomain' function. */
/* #undef HAVE_BINDTEXTDOMAIN */

/* Have nl_langinfo (CODESET) */
#define HAVE_CODESET 1

/* Define to 1 if you have the `dgettext' function. */
/* #undef HAVE_DGETTEXT */

/* Define to 1 if you have the `div' function. */
#define HAVE_DIV 1

/* Define to 1 if you have the <dmalloc.h> header file. */
/* #undef HAVE_DMALLOC_H */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `gethostname' function. */
#define HAVE_GETHOSTNAME 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getpwuid' function. */
#define HAVE_GETPWUID 1

/* Define to 1 if you have the `gettext' function. */
/* #undef HAVE_GETTEXT */

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define if readline has full GNU interface */
#define HAVE_GNU_READLINE 1

/* Define if iconv (in the libc) or libiconv is available */
/* #undef HAVE_ICONV */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `dmalloc' library (-ldmalloc). */
/* #undef HAVE_LIBDMALLOC */

/* Define to 1 if you have the `efence' library (-lefence). */
/* #undef HAVE_LIBEFENCE */

/* Define to 1 if you have the `intl' library (-lintl). */
/* #undef HAVE_LIBINTL */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkfifo' function. */
#define HAVE_MKFIFO 1

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
/* #undef HAVE_NLS */

/* Define to 1 if you have the `on_exit' function. */
/* #undef HAVE_ON_EXIT */

/* Define if ppm library is used. */
#define HAVE_PNG 1

/* Define to 1 if you have the `putenv' function. */
#define HAVE_PUTENV 1

/* Define if readline is available. */
#define HAVE_READLINE 1

/* Define if rplay library is used. */
/* #undef HAVE_RPLAY */

/* Define if librsvg library is used. */
/* #undef HAVE_RSVG */

/* Enable the use of mkstemp */
#define HAVE_SAFETY_MKSTEMP 1

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the `setpgid' function. */
#define HAVE_SETPGID 1

/* Define to 1 if you have the `setpgrp' function. */
#define HAVE_SETPGRP 1

/* Define to 1 if you have the `setvbuf' function. */
#define HAVE_SETVBUF 1

/* Define to 1 if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define to 1 if you have the `siginterrupt' function. */
#define HAVE_SIGINTERRUPT 1

/* Define to 1 if you have the `siglongjmp' function. */
#define HAVE_SIGLONGJMP 1

/* Define to 1 if you have the `sigsetjmp' function. */
/* #undef HAVE_SIGSETJMP */

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define if Solaris' Xinerama calls are being used. (Solaris 7 11/99 and
   later) */
/* #undef HAVE_SOLARIS_XINERAMA */

/* Define if Solaris' X11/extensions/xinerama.h header is provided. (Solaris 9
   and later) */
/* #undef HAVE_SOLARIS_XINERAMA_H */

/* Define to 1 if you have the <stdarg.h> header file. */
#define HAVE_STDARG_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the `strftime' function. */
#define HAVE_STRFTIME 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define if stroke library is used. */
/* #undef HAVE_STROKE */

/* Define to 1 if you have the `strstr' function. */
#define HAVE_STRSTR 1

/* Define to 1 if you have the `strtol' function. */
#define HAVE_STRTOL 1

/* Define to 1 if you have the `sysconf' function. */
#define HAVE_SYSCONF 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/systeminfo.h> header file. */
/* #undef HAVE_SYS_SYSTEMINFO_H */

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the `textdomain' function. */
/* #undef HAVE_TEXTDOMAIN */

/* Define to 1 if you have the `uname' function. */
#define HAVE_UNAME 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `unsetenv' function. */
#define HAVE_UNSETENV 1

/* Define to 1 if you have the `usleep' function. */
#define HAVE_USLEEP 1

/* Define to 1 if you have the `vfprintf' function. */
#define HAVE_VFPRINTF 1

/* Define to 1 if you have the `wait3' function. */
/* #undef HAVE_WAIT3 */

/* Define to 1 if you have the `wait4' function. */
/* #undef HAVE_WAIT4 */

/* Define to 1 if you have the `waitpid' function. */
#define HAVE_WAITPID 1

/* Define if Xcursor library is used. */
#define HAVE_XCURSOR 1

/* Define if Xft library is used. */
/* #undef HAVE_XFT */

/* Define if Xft 2 library is used. */
/* #undef HAVE_XFT2 */

/* Define if Xft library can handle utf8 encoding */
/* #undef HAVE_XFT_UTF8 */

/* Define if Xinerama library is used. */
/* #undef HAVE_XINERAMA */

/* Enable X output method */
#define HAVE_XOUTPUT_METHOD 1

/* Define if Xrender library is used. */
#define HAVE_XRENDER 1

/* Define if MIT Shared Memory extension is used. */
#define HAVE_XSHM 1

/* define if second arg of iconv use const */
/* #undef ICONV_ARG_CONST */

/* Name of package */
#define PACKAGE "fvwm"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define a suitable cast for arguments 2-4 of `select'. On most systems, this
   will be the empty string, as select usually takes pointers to fd_set. */
#define SELECT_FD_SET_CAST 

/* Enables session management functionality. */
/* #undef SESSION */

/* Define to 1 if the `setpgrp' function takes no argument. */
#define SETPGRP_VOID 1

/* Define if you want the Shaped window extensions. Shaped window extensions
   seem to increase the window managers RSS by about 60 Kbytes. They provide
   for leaving a title-bar on the window without a border. If you don't use
   shaped window extension, you can either make your shaped windows
   undecorated, or live with a border and backdrop around all your shaped
   windows (oclock, xeyes) If you normally use a shaped window (xeyes or
   oclock), you might as well compile this extension in, since the memory cost
   is minimal in this case (The shaped window shared libs will be loaded
   anyway). If you don't normally use a shaped window, you have to decide for
   yourself. Note: if it is compiled in, run time detection is used to make
   sure that the currently running X server supports it. */
#define SHAPE 1

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of `Pixel', as computed by sizeof. */
#define SIZEOF_PIXEL 4

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 4

/* The size of `Window', as computed by sizeof. */
#define SIZEOF_WINDOW 4

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at runtime.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* define if we use libiconv (not needed in general: for example iconv is
   native with recent glibc) */
/* #undef USE_LIBICONV */

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


/* Define if Xinerama should be emulated on a single screen. */
/* #undef USE_XINERAMA_EMULATION */

/* Version number of package */
#define VERSION "2.6.5"

/* Additional version information, like date */
#define VERSIONINFO ""

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

/* Define if Xpm library is used. */
#define XPM 1

/* Define to 1 if the X Window System is missing or not being used. */
/* #undef X_DISPLAY_MISSING */

/* Enables to use setlocale() provided by X */
/* #undef X_LOCALE */

#ifdef COMPAT_OLD_KEYSYMDEF
#  define XK_Page_Up   XK_Prior
#  define XK_Page_Down XK_Next
#endif

#ifdef COMPAT_OLD_LIBSTROKE
/* currently we only use one constant */
#  define STROKE_MAX_SEQUENCE MAX_SEQUENCE
#endif

#ifdef FRIBIDI_CHARSET_SPELLING
#  define FRIBIDI_CHAR_SET_NOT_FOUND FRIBIDI_CHARSET_NOT_FOUND
#endif

#ifdef FVWM_COMMAND_LOG
#   define FVWM_DEBUG_TIME 1
#endif

#ifdef FVWM_DEBUG_MSGS
#   define DBUG(x,y) fvwm_msg(DBG,x,y)
#else
#   define DBUG(x,y) /* no messages */
#endif

#ifdef USE_LIBICONV
/* define to use locale_charset in the place of nl_langinfog if libiconv
 * is used */
/* #undef HAVE_LIBCHARSET */
#endif

#ifdef HAVE_STROKE
#    define STROKE_ARG(x) x,
#    define STROKE_CODE(x) x
#else
#    define STROKE_ARG(x)
#    define STROKE_CODE(x)
#endif

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */


/**
 * The next few defines are options that are only changed from their values
 * shown here on systems that _don't_ use the configure script.
 **/

/* Enable tests for missing too many XEvents.  Usually you want this. */
#define WORRY_ABOUT_MISSED_XEVENTS 1

/* Define if the X11 ConnectionNumber is actually a file descriptor. */
#define HAVE_X11_FD 1

/* Define if fork() has unix semantics.  On VMS, no child process is created
   until after a successful exec(). */
#define FORK_CREATES_CHILD 1

/* Suffix for executable filenames; NULL if no extension needed. */
#define EXECUTABLE_EXTENSION NULL

/* Define to remove the extension from executable pathnames before calling
   exec(). */
/* #undef REMOVE_EXECUTABLE_EXTENSION */

/* Enables the "MiniIcon" Style option to specify a small pixmap which
 * can be used as one of the title-bar buttons, shown in window list,
 * utilized by modules, etc.  Requires PIXMAP_BUTTONS to be defined
 * (see below). */
/* #undef MINI_ICONS */
/* NOTE: hard coded to 1 */
#if 1
#define FMiniIconsSupported 1
#else
#define FMiniIconsSupported 0
#endif

/* Enables tagged general decoration styles which can be assigned to
 * windows using the UseDecor Style option, or dynamically updated
 * with ChangeDecor.  To create and destroy "decor" definitions, see
 * the man page entries for AddToDecor and DestroyDecor.  There is a
 * slight memory penalty for each additionally defined decor. */
/* #undef USEDECOR */
/* NOTE: hard coded to 1 */
#define USEDECOR 1

#if RETSIGTYPE != void
#define SIGNAL_RETURN return 0
#else
#define SIGNAL_RETURN return
#endif

/* Allow GCC extensions to work, if you have GCC. */
#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later. */
#  if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5) || __STRICT_ANSI__
#    define __attribute__(x)
#  endif
/* The __-protected variants of `format' and `printf' attributes
 * are accepted by gcc versions 2.6.4 (effectively 2.7) and later. */
#  if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7)
#    define __format__ format
#    define __printf__ printf
#  endif
#endif

#if HAVE_ALLOCA_H
#  include <alloca.h>
#else
#  ifdef _AIX
       #pragma alloca
#  else
#    ifndef alloca /* predefined by HP cc +Olibcalls */
	 char *alloca ();
#    endif
#  endif
#endif

#ifdef HAVE_STRING_H
#  include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#  include <strings.h>
#endif
#ifdef HAVE_MEMORY_H
#  include <memory.h>
#endif
#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#if defined (HAVE_MALLOC_H) && !defined (__FreeBSD__) && !defined (__OpenBSD__) && !defined(__NetBSD__)
#  include <malloc.h>
#endif
#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#endif
#ifndef HAVE_STRCHR
#  define strchr(_s,_c)   index((_s),(_c))
#  define strrchr(_s,_c)  rindex((_s),(_c))
#endif

#ifndef HAVE_MEMCPY
#  define memcpy(_d,_s,_l)  bcopy((_s),(_d),(_l))
#endif
#ifndef HAVE_MEMMOVE
#  define memmove(_d,_s,_l) bcopy((_s),(_d),(_l))
#endif

#if HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif

#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifndef min
#  define min(a,b) (((a)<(b)) ? (a) : (b))
#endif
#ifndef max
#  define max(a,b) (((a)>(b)) ? (a) : (b))
#endif
#ifndef abs
#  define abs(a) (((a)>=0)?(a):-(a))
#endif

#include "libs/defaults.h"

#ifndef FEVENT_C
#include <X11/Xlib.h>
#include "libs/FEvent.h"
#endif

#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif

#ifdef HAVE_LSTAT
#define DO_USE_LSTAT 1
#define fvwm_lstat(x,y) lstat(x,y)
#else
#define DO_USE_LSTAT 0
#define fvwm_lstat(x,y) -1
#endif



/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to the type used in arguments 2-4 of `select', if not set by system
   headers. */
/* #undef fd_set */

/* Define to the type used in argument 1 `select'. Usually this is an `int'.
   */
#define fd_set_size_t int

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Specify a type for sig_atomic_t if it's not available. */
/* #undef sig_atomic_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

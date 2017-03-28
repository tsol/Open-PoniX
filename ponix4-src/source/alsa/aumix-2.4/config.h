/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if you have a working `mmap' system call.  */
#define HAVE_MMAP 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.
   To do:  check whether OSS-compatible sound drivers are available for
   UNICOS.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if you have a working `mmap' system call.  */
#define HAVE_MMAP 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Don't change these:  ALSA-specific features are disabled for now. */
/* #undef HAVE_ALSA */
#define HAVE_ALSA_DISABLED

/* Define if you want built-in emulation of a sound driver, for debugging. */
/* #undef DUMMY_MIXER */

/* Define this if your ncurses library has the getmouse routine.  This is
recommended. */ 
#define HAVE_GETMOUSE 1

/* Define this if you have the ncurses library and want to use it. */
#define HAVE_LIBNCURSES 1

/* Define this if you have the curses library and want to use it. */
/* #undef HAVE_LIBCURSES */

/* Define this if you have (n)curses and want to use it. */
#define HAVE_CURSES 1

#define HAVE_NLS 1
/* #undef HAVE_SYSMOUSE */

/*
Define this if your ncurses library contains the use_default_colors
routine and your terminal can use transparent colors.  This is
recommended.
*/
#define HAVE_USEDEFAULT 1

#define HAVE_USEKEYPAD 1
#define USE_INCLUDED_LIBINTL no
#define USE_NLS yes

/*
Define this if you want to compile with GTK+ support.
*/
#define HAVE_GTK 1

/* 
Define this to use built-in label text.  The built-in labels may
not match up to the actual sound driver, so turning this off is
the most conservative thing.
*/
/* #undef USE_OWN_LABELS */

/* Define if you have the <curses.h> header file.  */
#define HAVE_CURSES_H 1

/* Define if you have the ncurses library (-lncurses).  */
#define HAVE_LIBNCURSES 1

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the gettext function.  */
#define HAVE_GETTEXT 1

/* Define if you have the getwd function.  */
#define HAVE_GETWD 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the select function.  */
#define HAVE_SELECT 1

/* Define if you have the <curses.h> header file.  */
#define HAVE_CURSES_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <libintl.h> header file.  */
#define HAVE_LIBINTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file.  */
#define HAVE_LOCALE_H 1

/* Define if you have the <machine/console.h> header file.  */
/* #undef HAVE_MACHINE_CONSOLE_H */

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <ncurses.h> header file.  */
#define HAVE_NCURSES_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <sys/audioio.h> header file.  */
/* #undef HAVE_SYS_AUDIOIO_H */

/* Define if you have the <sys/ioctl.h> header file.  */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the gpm library (-lgpm).  */
/* #undef HAVE_LIBGPM */

/* Name of package */
#define PACKAGE "aumix"

/* Version number of package */
#define VERSION "2.4"


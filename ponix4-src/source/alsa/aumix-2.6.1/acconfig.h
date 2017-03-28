/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if using alloca.c.  */
#undef C_ALLOCA

/* Define to empty if the keyword does not work.  */
#undef const

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.
   To do:  check whether OSS-compatible sound drivers are available for
   UNICOS.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#undef HAVE_ALLOCA

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#undef HAVE_ALLOCA_H

/* Define if you have a working `mmap' system call.  */
#undef HAVE_MMAP

/* Define as __inline if that's what the C compiler calls it.  */
#undef inline

/* Define to `long' if <sys/types.h> doesn't define.  */
#undef off_t

/* Define as the return type of signal handlers (int or void).  */
#undef RETSIGTYPE

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
#undef size_t

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
#undef STACK_DIRECTION

/* Define if you have the ANSI C header files.  */
#undef STDC_HEADERS

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#undef TIME_WITH_SYS_TIME

/* Don't change these:  ALSA-specific features are disabled for now. */
#undef HAVE_ALSA
#define HAVE_ALSA_DISABLED

/* Define if you want built-in emulation of a sound driver, for debugging. */
#undef DUMMY_MIXER

/* Define this if your ncurses library has the getmouse routine.  This is
recommended. */ 
#undef HAVE_GETMOUSE

/* Define this if you have the ncurses library and want to use it. */
#undef HAVE_LIBNCURSES

/* Define this if you have the curses library and want to use it. */
#undef HAVE_LIBCURSES

/* Define this if you have (n)curses and want to use it. */
#undef HAVE_CURSES

#undef HAVE_NLS
#undef HAVE_SYSMOUSE

/*
Define this if your ncurses library contains the use_default_colors
routine and your terminal can use transparent colors.  This is
recommended.
*/
#undef HAVE_USEDEFAULT

#undef HAVE_USEKEYPAD
#undef USE_INCLUDED_LIBINTL
#undef USE_NLS

/*
Define this if you want to compile with GTK+ support.
*/
#undef HAVE_GTK

/* 
Define this to use built-in label text.  The built-in labels may
not match up to the actual sound driver, so turning this off is
the most conservative thing.
*/
#undef USE_OWN_LABELS

/* Define if you have the getcwd function.  */
#undef HAVE_GETCWD

/* Define if you have the getpagesize function.  */
#undef HAVE_GETPAGESIZE

/* Define if you have the gettext function.  */
#undef HAVE_GETTEXT

/* Define if you have the getwd function.  */
#undef HAVE_GETWD

/* Define if you have the putenv function.  */
#undef HAVE_PUTENV

/* Define if you have the select function.  */
#undef HAVE_SELECT

/* Define if you have the <curses.h> header file.  */
#undef HAVE_CURSES_H

/* Define if you have the <fcntl.h> header file.  */
#undef HAVE_FCNTL_H

/* Define if you have the <libintl.h> header file.  */
/* Define HAVE_LIBINTL_H if the system's <libintl.h> should be used. */
#undef HAVE_LIBINTL_H

/* Define if you have the <limits.h> header file.  */
#undef HAVE_LIMITS_H

/* Define if you have the <locale.h> header file.  */
#undef HAVE_LOCALE_H

/* Define if you have the <malloc.h> header file.  */
#undef HAVE_MALLOC_H

/* Define if you have the <ncurses.h> header file.  */
#undef HAVE_NCURSES_H

/* Define if you have the <strings.h> header file.  */
#undef HAVE_STRINGS_H

/* Define if you have the <sys/audioio.h> header file.  */
#undef HAVE_SYS_AUDIOIO_H

/* Define if you have the <sys/ioctl.h> header file.  */
#undef HAVE_SYS_IOCTL_H

/* Define if you have the <sys/time.h> header file.  */
#undef HAVE_SYS_TIME_H

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H

/* Define if you want the Linux-specific gpm mouse support. */
#undef HAVE_LIBGPM

/* Define if you have the ncurses library (-lncurses).  */
#undef HAVE_LIBNCURSES

/* Name of package */
#define PACKAGE "aumix"

/* Version number of package */
#undef VERSION

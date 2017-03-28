# $Aumix: aumix/configure.ac,v 1.21 2010/05/04 23:49:22 trevor Exp $
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([aumix],[2.9.1],[aumix-general@gna.org])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE
AC_PREREQ(2.61)
AC_CONFIG_SRCDIR([src/common.c])
AC_CONFIG_HEADERS([config.h])
AM_GNU_GETTEXT_VERSION([0.17])
AM_GNU_GETTEXT([external])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

# Checks for libraries.
dnl for NetBSD and OpenBSD
AC_CHECK_LIB(ossaudio, _oss_ioctl)

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([ \
	argz.h \
	fcntl.h \
	libintl.h \
	limits.h \
	locale.h \
	malloc.h \
	nl_types.h \
	stddef.h \
	stdlib.h \
	string.h \
	strings.h \
	sys/ioctl.h \
	sys/param.h \
	sys/time.h \
	unistd.h \
	sys/soundcard.h \
	soundcard.h \
	machine/soundcard.h \
	ncurses.h \
	curses.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
#AC_HEADER_TIME

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify alarm getcwd mempcpy memset munmap putenv select setenv setlocale stpcpy strcasecmp strchr strdup])

dnl Additional configure options
AC_ARG_WITH(ncurses,
   [  --without-ncurses       compile with no ncurses or mouse support],
  AC_MSG_RESULT([Compiling without ncurses support]),[
  dnl Checks for ncurses library.
  AC_CHECK_LIB(ncurses, initscr, initscr=on, initscr=off)
  if test $initscr = on; then
    CURSLIB="ncurses"
    AC_DEFINE(HAVE_LIBNCURSES, 1,
      [Define this if you have the ncurses library and want to use it.])
  else
    AC_CHECK_LIB(curses, initscr, initscr=on, initscr=off)
    if test $initscr = on; then
      CURSLIB="curses"
      AC_DEFINE(HAVE_LIBCURSES, 1,
        [Define this if you have the curses library and want to use it.])
    fi
  fi
  if test x$CURSLIB != x; then
    LIBS="-l$CURSLIB $LIBS"
    AC_DEFINE(HAVE_CURSES, 1,
      [Define this if you have (n)curses and want to use it.])
    AC_CHECK_LIB($CURSLIB, getmouse,
      [AC_DEFINE(HAVE_GETMOUSE, 1,
	[Define this if your ncurses library has the getmouse routine.])])
    AC_CHECK_LIB($CURSLIB, use_default_colors,
      [AC_DEFINE(HAVE_USEDEFAULT, 1,
	[Define this if your ncurses library contains the use_default_colors
	routine and your terminal can use transparent colors.])])
    AC_CACHE_CHECK([for private member _use_keypad in WINDOW],
      aumix_cv_struct_window_usekeypad,
      [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#ifdef HAVE_NCURSES_H
#include <ncurses.h>
#else
#include <curses.h>
#endif]], [[WINDOW w; w._use_keypad;]])],[aumix_cv_struct_window_usekeypad=yes],[aumix_cv_struct_window_usekeypad=no])])
    if test $aumix_cv_struct_window_usekeypad = yes; then
      AC_DEFINE(HAVE_USEKEYPAD, 1,
	[Define this if your ncurses library has the member _use_keypad
	in structure WINDOW.])
    fi
  fi
  AC_ARG_WITH(gpm,
    [  --without-gpm           Compile only without GPM support],
    AC_MSG_RESULT([Compiling without GPM support]),
    dnl Checks for GPM library.
    AC_CHECK_LIB(gpm, Gpm_Repeat)
  )
  AC_ARG_WITH(sysmouse,
    [  --without-sysmouse      Compile only without sysmouse support],
    AC_MSG_RESULT([Compiling without sysmouse support]),
    dnl Checks for sysmouse availability.
    AC_CACHE_CHECK([for CONS_MOUSECTL],
      aumix_cv_cons_mousectl,
      [AC_EGREP_CPP([A mouse is a mouse is a mouse],
      [#ifdef HAVE_MACHINE_CONSOLE_H
#include <machine/console.h>
#endif
#ifdef HAVE_SYS_CONSIO_H
#include <sys/consio.h>
#endif
#ifdef CONS_MOUSECTL
A mouse is a mouse is a mouse
#endif], aumix_cv_cons_mousectl=yes, aumix_cv_cons_mousectl=no)])
    if test $aumix_cv_cons_mousectl = yes; then
      AC_DEFINE(HAVE_SYSMOUSE, 1,
	[Define this if you have sysmouse console mouse support and
	want to use it.])
    fi
  )
])
AM_CONDITIONAL(CURSES, test "x$CURSLIB" != "x")
AC_SUBST(CURSLIB)

dnl The built-in labels may not match up to the actual sound driver, so
dnl turning this off is the most conservative thing.
AC_MSG_CHECKING([whether built-in label text is requested])
AC_ARG_ENABLE(own-labels,
  [  --enable-own-labels     use built-in label text (may not match system) ],
  own_labels=$enableval, own_labels=no)
AC_MSG_RESULT($own_labels)
if test $own_labels = yes; then
  AC_DEFINE(USE_OWN_LABELS, 1,
    [Define this to use built-in label text.])
fi

AC_ARG_WITH(gtk,
  [  --without-gtk           compile without GTK+ 2.0 interface],
  gtk_must=off, gtk_must=on)
if test $gtk_must = on; then
  dnl from gftp 
  AC_CHECK_PROG(PKG_CONFIG, pkg-config, yes, no)
  if test "$PKG_CONFIG" = "no"; then
    echo "pkg-config not found--compiling without GTK+ 2.0." ; 
  else
    echo "pkg-config found--compiling with GTK+ 2.0." ; 
    AC_DEFINE(HAVE_GTK, 1,
      [Define this if you want to compile with support for GTK+ 2.0.])
    dnl from GIMP 1.1.6
    dnl Check for GTK+ 2.0
    AM_PATH_GTK_2_0(2.0.0,,AC_MSG_ERROR(Test for GTK failed. See the file 'INSTALL' for help.))
    dnl Evil stuff to extract GLIB stuff from pkg-config output
    dnl (we want to make sure it matches with the gtk we're using)
    GLIB_CFLAGS=`echo $GTK_CFLAGS | sed 's/^.*\(-I[^ ]*glib[^ ]*  *-I[^ ]*\).*$/\1/'`
    GLIB_LDFLAGS=`echo $GTK_LIBS | sed -e 's/^.*-lgdk[^ ]* *\(-L[^ ]*\).*$/\1/' -e 's/^.* -lgdk[^ ]* .*$//'`
    if test -z "$GLIB_LDFLAGS" ; then
      GLIB_LDFLAGS=`echo $GTK_LIBS | sed 's/^ *\(-L[^ ]*\) .*$/\1/'`
    fi
    GLIB_LIBS="$GLIB_LDFLAGS `echo $GTK_LIBS | sed 's/^.*\(-lglib[^ ]*\).*$/\1/'`"
    AC_SUBST(GLIB_CFLAGS)
    AC_SUBST(GLIB_LIBS)
    LDFLAGS="$LDFLAGS `echo $GTK_LIBS | sed 's/\(.*\)\(-lgtk.*\)/\1/'`"
    LIBS="$LIBS $GTK_LIBS"
  fi
else
  AC_MSG_RESULT([Compiling without GTK+ 2.0.])
fi
AM_CONDITIONAL(GTK, test "$gtk_must" = on && test "$PKG_CONFIG" != "no")

AC_MSG_CHECKING([whether dummy mixer is requested])
AC_ARG_ENABLE(dummy-mixer,
  [  --enable-dummy-mixer	  emulate mixer device internally, for debugging ],
  dummy_mixer=$enableval, dummy_mixer=no)
AC_MSG_RESULT($dummy_mixer)
if test $dummy_mixer = yes; then
  AC_DEFINE(DUMMY_MIXER, 1,
    [Define if you want built-in emulation of a sound driver, for debugging.])
fi

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

dnl Checks for library functions.
AC_FUNC_ALLOCA 
AC_FUNC_MMAP  
dnl	recommended by autoscan, but gives a warning
dnl	AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_CHECK_FUNCS(getcwd getwd putenv select)

AC_CONFIG_FILES([Makefile
                 data/Makefile
                 doc/Makefile
                 po/Makefile.in
                 src/Makefile])
AC_OUTPUT

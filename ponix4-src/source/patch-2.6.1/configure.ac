# Configure `patch'.

# Copyright (C) 1993, 1997, 1998, 1999, 2002, 2003, 2006, 2009
# Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.

AC_PREREQ(2.57)
define(AC_PACKAGE_VERSION, m4_normalize(esyscmd(
[
    sh ./update-version.sh VERSION
    cat VERSION
])))
AC_INIT(patch, AC_PACKAGE_VERSION, bug-patch@gnu.org)
AC_CONFIG_SRCDIR(src/patch.c)
AC_CONFIG_HEADER(config.h:config.hin)
AC_ARG_PROGRAM

AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
# Use ed_PROGRAM, not ED_PROGRAM,
# because <errno.h> reserves symbols starting with `E'.
AC_PATH_PROG(ed_PROGRAM, ed, ed)

AC_PATH_PROG(TEST_SHELL, [bash sh])

gl_USE_SYSTEM_EXTENSIONS
AC_PROG_CC_STDC
AC_SYS_LARGEFILE
AC_EXEEXT

AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h utime.h)

AC_TYPE_MODE_T
AC_TYPE_OFF_T
AM_STDBOOL_H
gl_CHECK_TYPE_STRUCT_UTIMBUF

gl_ARGMATCH
gl_BACKUPFILE
gl_DIRNAME
gl_ERROR
gl_EXITFAIL
gl_FUNC_MEMCHR
gl_FUNC_STRNDUP
gl_FUNC_STRNLEN
gl_GETOPT
gl_HASH
gl_PREREQ_XMALLOC
gl_QUOTE
gl_QUOTEARG
gl_SAFE_WRITE
gl_STRCASE
gl_XALLOC
gl_XSTRNDUP

dnl lib/basename.c requires xstrndup.
AC_LIBOBJ([xstrndup])

dnl m4/error.m4 does not do this:
AC_LIBOBJ([error])

AC_CHECK_DECLS([mktemp])
AC_CHECK_FUNCS(geteuid getuid mktemp raise sigaction sigprocmask sigsetmask)
AC_FUNC_FSEEKO
gl_FUNC_GLIBC_UNLOCKED_IO
gl_FUNC_MALLOC_POSIX
gl_FUNC_REALLOC_POSIX
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_SETMODE_DOS
AC_FUNC_VPRINTF
PATCH_FUNC_MKDIR_TAKES_ONE_ARG

gl_AC_DOS
AC_SYS_LONG_FILE_NAMES

AC_ARG_ENABLE([merge],
  [AS_HELP_STRING([--disable-merge],
    [disable support for merging])])
if test "$enableval" != no ; then
  AC_SUBST(ENABLE_MERGE, 1)
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

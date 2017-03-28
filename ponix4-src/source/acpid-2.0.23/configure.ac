#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.60])
AC_INIT([acpid], [2.0.23])
AC_CONFIG_SRCDIR([connection_list.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign tar-pax no-dist-gzip dist-xz subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
AM_MAINTAINER_MODE
# Checks for programs.
AC_PROG_AWK
AC_PROG_CC_STDC
AM_PROG_CC_C_O
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRNLEN
AC_CHECK_FUNC(fchown, AC_DEFINE(HAVE_FCHOWN, 1, "Define if fchown() exists"), AC_MSG_ERROR(["fchown() not found"]))
AC_CHECK_FUNC(chmod, AC_DEFINE(HAVE_CHMOD, 1, "Define if chmod() exists"), AC_MSG_ERROR(["chmod() not found"]))
AC_CHECK_FUNC(fstat, AC_DEFINE(HAVE_FSTAT, 1, "Define if fstat() exists"), AC_MSG_ERROR(["fstat() not found"]))
AC_CHECK_FUNC(fstatat, AC_DEFINE(HAVE_FSTATAT, 1, "Define if fstatat() exists"), AC_MSG_ERROR(["fstatat() not found"]))
AC_CHECK_FUNC(openat, AC_DEFINE(HAVE_OPENAT, 1, "Define if openat() exists"), AC_MSG_ERROR(["openat() not found"]))
AC_CHECK_FUNC(asprintf, AC_DEFINE(HAVE_ASPRINTF, 1, "Define if asprintf() exists"), AC_MSG_ERROR(["asprintf() not found"]))
# These are actually required.  Should switch to above form.
AC_CHECK_FUNCS([alarm dup2 memset regcomp select socket strcasecmp strdup strerror strrchr strtol])

# removed for now as kacpimon was not included in dist target
#AC_ARG_ENABLE([kacpimon],AS_HELP_STRING([--enable-kacpimon], [Build kacpimon]))
#AM_CONDITIONAL(KACPIMON, test "x$enable_kacpimon" = "xyes")
#AM_COND_IF([KACPIMON],[AC_CONFIG_FILES([kacpimon/Makefile])])

AC_CONFIG_FILES([Makefile kacpimon/Makefile])
AC_OUTPUT

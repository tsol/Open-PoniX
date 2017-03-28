dnl Copyright (C) 2008 Kim Woelders
dnl This code is public domain and can be freely used or copied.

dnl Macro to set compiler warning flags

dnl Provides configure argument --enable-werror to stop compilation on warnings

dnl Usage: AC_C_WARNFLAGS([LANG])
dnl Set LANG to 'cpp' when compiling for C++

AC_DEFUN([AC_C_WARNFLAGS], [
  define(ac_c_compile_cpp, ifelse([$1], [cpp], [yes], [no]))

  AC_ARG_ENABLE(werror,
    [  --enable-werror         treat compiler warnings as errors @<:@default=no@:>@],,
    enable_werror=no)

  if test "x$GCC" = "xyes"; then
    CWARNFLAGS="-W -Wall -Waggregate-return -Wcast-align -Wpointer-arith -Wshadow -Wwrite-strings"
    ifelse(ac_c_compile_cpp, no, [
      CWARNFLAGS="$CWARNFLAGS -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes"
    ],)

    if test "x$enable_werror" = "xyes"; then
      CWARNFLAGS="$CWARNFLAGS -Werror"
    fi
  fi
  AC_SUBST(CWARNFLAGS)
])

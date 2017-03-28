dnl Copyright (C) 2010 Kim Woelders
dnl This code is public domain and can be freely used or copied.

dnl Macro for defining __func__ if not already defined

dnl Usage: AC_C___FUNC__

dnl If __func__ is not defined and __FUNCTION__ is, __func__ is defined
dnl to __FUNCTION__. If __FUNCTION__ isn't defined either, __func__ is
dnl defined to "FUNC".

AC_DEFUN([AC_C___FUNC__],
[
  AC_CACHE_CHECK([for __func__], ac_cv___func__, [
    AC_COMPILE_IFELSE([
      AC_LANG_PROGRAM([], [[const char *s = __func__;]])
    ], [
      ac_cv___func__=yes
    ], [
      AC_COMPILE_IFELSE([
        AC_LANG_PROGRAM([], [[const char *s = __FUNCTION__;]])
      ], [
        ac_cv___func__=function
      ], [
        ac_cv___func__=no
      ])
    ])
  ])

  if test $ac_cv___func__ = function; then
    AC_DEFINE(__func__, __FUNCTION__, [Define __func__ appropriately if missing])
  elif test $ac_cv___func__ = no; then
    AC_DEFINE(__func__, "FUNC", [Define __func__ appropriately if missing])
  fi
])

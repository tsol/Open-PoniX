/* $Aumix: aumix/acconfig.h,v 1.3 2002/09/07 13:38:34 trevor Exp $ */

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

#undef HAVE_NLS

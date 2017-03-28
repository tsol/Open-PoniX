AC_PREREQ(2.53)
AC_INIT([matchbox-window-manager], 1.2, [mallum@handhelds.org])
AC_CONFIG_SRCDIR([src/wm.c])

AM_INIT_AUTOMAKE()
AM_MAINTAINER_MODE
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for header files.
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

SUPPORTS_PNG=0
SUPPORTS_JPEG=0
SUPPORTS_XFT=0
SUPPORTS_PANGO=0
SUPPORTS_XSETTINGS=0

dnl ----- Args -------------------------------------------------------------

AC_ARG_ENABLE(standalone-xft,
  [  --enable-standalone-xft       enable standalone build ( with Xft font), [default=no]],
  enable_standalone_xft=$enableval, enable_standalone_xft=no)

AC_ARG_ENABLE(standalone,
  [  --enable-standalone           enable standalone build.],
  enable_standalone=$enableval, enable_standalone=no)

AC_ARG_ENABLE(composite,
  [  --enable-composite            enable composite support (EXPERIMENTAL).],
  enable_composite=$enableval, enable_composite=no)

AC_ARG_ENABLE(startup_notification,
  [  --enable-startup-notification enable startup notification support],
     enable_startup_notification=$enableval, enable_startup_notification=no )

AC_ARG_ENABLE(expat,
  [  --enable-expat                enable expat XML parser support],
     enable_expat=$enableval, enable_expat=no )

AC_ARG_WITH(expat-includes,    
  [  --with-expat-includes=DIR     Use Expat includes in DIR], 
	   expat_includes=$withval, expat_includes=yes)

AC_ARG_WITH(expat-lib,         
  [  --with-expat-lib=DIR          Use Expat library in DIR], 
	   expat_lib=$withval, expat_lib=yes)

# DISABLE PERMANTLY FOR NOW
#AC_ARG_ENABLE(xsync,
#  [  --enable-xsync                enable XSYNC X extension support (EXPERIMENTAL )],
#     enable_xsync=$enableval, enable_xsync=no )

enable_xsync=no

AC_ARG_ENABLE(gconf,
  [  --enable-gconf                enable gconf support],
     enable_gconf=$enableval, enable_gconf=no )

AC_ARG_ENABLE(session,
  [  --enable-session               enable session manager support],
     enable_session=$enableval, enable_session=no )

AC_ARG_ENABLE(keyboard,
  [  --disable-keyboard            disable keyboard support [default=no]],
   		enable_keyboard=$enableval, 
		enable_keyboard=yes)

AC_ARG_ENABLE(ping-protocol,
  [  --disable-ping-protocol       disable ping protocol support [default=no]],
   		enable_ping_protocol=$enableval, 
		enable_ping_protocol=yes)

AC_ARG_ENABLE(xrm,
  [  --disable-xrm                 disable Xrm support [default=no]],
   		enable_xrm=$enableval, 
		enable_xrm=yes)

AC_ARG_ENABLE(alt_input_wins,
  [  --enable-alt-input-wins enable alternate managing input windows ],
  enable_alt_input_wins=$enableval, enable_alt_input_wins=no)


AC_ARG_ENABLE(debug,
  [  --enable-debug                enable debug ( verbose ) build],
     enable_debug=$enableval, enable_debug=no )

AC_ARG_ENABLE(gcov,
  [  --enable-gcov                 enable gcov coverage ( testing only ) build],
     enable_gcov=$enableval, enable_gcov=no )

AC_ARG_ENABLE(gprof,
  [  --enable-gprof                enable gprof profiler ( testing only ) build],
     enable_gprof=$enableval, enable_gprof=no )




dnl ---- Check we can find libmb 

if test x$enable_standalone != xyes && test x$enable_standalone_xft != xyes; then

   PKG_CHECK_MODULES(LIBMB, libmb >= 1.5, [have_libmb="yes"], [have_libmb="no"])

   if test x$have_libmb = xno; then
echo "
******************************************************************* 
*** 
*** Cannot find libmatchbox ( >= 1.5 ) library, please check it is 
*** correctly installed. 
***
*** If you want to build without libmatchbox, you can build a 'standalone'
*** matchbox. To do so rerun configure with;
*** 
***   --enable-standalone, or --enable-standalone-xft 
*** 
*** NOTE: a 'standalone' build does _not_ support themeing. It will be ugly.
*** 
*** ( See manual at http://projects.o-hand.com/matchbox for more info )
*** 
*******************************************************************
"
      AC_MSG_ERROR([Unable to find libMatchbox and standalone build not requested.])
   fi

else
   # we need to get PKG_CONFIG set anyway
   AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

fi

XFT_LIBS=
XFT_CFLAGS=

if test x$enable_standalone_xft = xyes; then
  AC_MSG_CHECKING([for xft])
  if test x$PKG_CONFIG != xno && $PKG_CONFIG --exists xft; then 
    XFT_CFLAGS="`pkg-config --cflags xft`"
    XFT_LIBS="`pkg-config --libs xft`"
    AC_DEFINE(USE_XFT, [1], [Use Xft])
    SUPPORTS_XFT=1
    AC_MSG_RESULT(yes)
  else

    AC_PATH_PROG(XFT_CONFIG, xft-config, no)
    
    if test x$XFT_CONFIG = xno; then
      AC_MSG_RESULT(no)
      AC_MSG_WARN([*** Cannot find Xft librarys, disabling support])
      enable_standalone=yes
      enable_standalone_xft=no
    else
      XFT_CFLAGS="`xft-config --cflags`"
      XFT_LIBS="`xft-config --libs`"
      AC_DEFINE(USE_XFT, [1], [Use Xft])
      SUPPORTS_XFT=1    
      AC_MSG_RESULT(yes)
    fi
  fi
fi

if test x$enable_standalone = xyes || test x$enable_standalone_xft = xyes; then

  AC_PATH_XTRA
  if test x"$no_x" = x"yes"; then
     AC_MSG_ERROR([*** Required X11 Headers and libraries not found.***])
  fi

  LIBMB_CFLAGS="$XLIBS_CLAGS $XFT_CFLAGS"
  LIBMB_LIBS="$X_LIBS $XFT_LIBS -lX11 -lXext"

  AC_DEFINE(STANDALONE, [1], [Use Standalone])	

  if test x$enable_standalone_xft = xyes; then
     AC_DEFINE(USE_XFT, [1], [Use Xft fonts])	
  fi

fi

AM_CONDITIONAL(WANT_STANDALONE, 
	       test x$enable_standalone = xyes || test x$enable_standalone_xft = xyes)


dnl ------ XSYNC support ------------------------------------------------

if test x$enable_xsync = xyes; then

  have_xsync="yes" 

  AC_CHECK_LIB(Xext, XSyncQueryExtension,
               have_xsync="yes" , 
	       have_sync="no" ,$LIBMB_LIBS )

  if test "x$have_xsync" = "xyes"; then 	       
     AC_CHECK_HEADER(X11/extensions/sync.h,,have_xsync=no,
                     [#include <X11/Xlib.h>])	
  fi		     

  if test "x$have_xsync" = "xno"; then
     AC_MSG_WARN([Unable to meet XSync dependencies. Not enabling])
     enable_xsync="no"
   else
      AC_MSG_RESULT([Enabling XSync Support.])	
      AC_DEFINE(USE_XSYNC, 1, Have the SYNC extension library)
   fi
fi

dnl ------ keyboard support ------------------------------------------------

if test x$enable_keyboard = xno; then
      LIBMB_CFLAGS="$LIBMB_CFLAGS -DNO_KBD"
      AC_MSG_RESULT([Disabling Keyboard Support.])	
fi

dnl ------ ping proto support ----------------------------------------------

if test x$enable_ping_protocol = xno; then
      LIBMB_CFLAGS="$LIBMB_CFLAGS -DNO_PING"
      AC_MSG_RESULT([Disabling Ping Protocol Support.])	
fi

dnl ------ Input Wins----------------------------------------------------

if test x$enable_alt_input_wins = xyes; then
      AC_DEFINE(USE_ALT_INPUT_WIN, [1], [use alternative input Windows])
fi

dnl ------ Xrm support ------------------------------------------------

if test x$enable_xrm = xno; then
      AC_DEFINE(NO_XRM, [1], [Dont use Xrm])
      AC_MSG_RESULT([Disabling Xrm Support.])	
fi

dnl ------ Debug Build -----------------------------------------------------

if test x$enable_debug = xyes; then
      LIBMB_CFLAGS="$LIBMB_CFLAGS -DDEBUG"
fi

dnl ------ Composite -------------------------------------------------------

if test x$enable_composite != xno; then
  PKG_CHECK_MODULES(COMPO, xcomposite xfixes xdamage xrender, ,
			   AC_MSG_ERROR([*** Required Composite Librays not installed ***]))

  AC_DEFINE(USE_COMPOSITE, [1], [use bleeding enge composite code])

fi

dnl ------ XFixes Curosr removal check ------------------------------------

PKG_CHECK_MODULES(XFIXES, xfixes >= 4.0, have_xfixes=yes, have_xfixes=no)

if test x$have_xfixes = xyes; then
  AC_DEFINE(HAVE_XFIXES, [1], [Use XFixes ext to really hide cursor])
fi

PKG_CHECK_MODULES(XCURSOR, xcursor, have_xcursor=yes, have_xcursor=no)

if test x$have_xcursor = xyes; then
  AC_DEFINE(HAVE_XCURSOR, [1], [Use XCursor to sync pointer themes])
fi


dnl ------ Expat ------------------------------------------------------------

if test x$enable_expat != xno; then

  case "$expat_includes" in
    yes|no)
	EXPAT_CFLAGS=""
	;;
    *)
	EXPAT_CFLAGS="-I$expat_includes"
	;;
  esac
	
  case "$expat_lib" in
    yes)

	case "$expat" in
	  yes)
		EXPAT_LIBS="-lexpat"
		;;
	    *)
		EXPAT_LIBS="-L$expat/lib -lexpat"
		;;
	esac

	;;
   no)
	;;
   *)
	EXPAT_LIBS="-L$expat_lib -lexpat"
	;;
   esac

   expatsaved_CPPFLAGS="$CPPFLAGS"
   CPPFLAGS="$CPPFLAGS $EXPAT_CFLAGS"
   expatsaved_LIBS="$LIBS"
   LIBS="$LIBS $EXPAT_LIBS"

   AC_CHECK_HEADER(expat.h)
   case "$ac_cv_header_expat_h" in
     no)
	 AC_CHECK_HEADER(xmlparse.h)

	 case "$ac_cv_header_xmlparse_h" in
	   no)
		have_expat_header=no;
		;;
	   yes)
		HAVE_XMLPARSE_H=1
		AC_SUBST(HAVE_XMLPARSE_H)
		AC_DEFINE_UNQUOTED(HAVE_XMLPARSE_H,$HAVE_XMLPARSE_H,
				       [Use xmlparse.h instead of expat.h])
		have_expat_header=yes
	        ;;
	 esac
	 ;;
     yes)
	 have_expat_header=yes
	;;
   esac
	
   case "$have_expat_header" in
     no)
	expat=no
	;;
     yes)
	AC_CHECK_FUNCS(XML_ParserCreate)

	case "$ac_cv_func_XML_ParserCreate" in
          no)
	  	expat=no
		;;
	  yes)
		HAVE_EXPAT=1
		AC_SUBST(HAVE_EXPAT)
		AC_DEFINE_UNQUOTED(HAVE_EXPAT,$HAVE_EXPAT,
		[Found a useable expat library])
		;;
	esac
	;;
   esac
	
   CPPFLAGS="$saved_CPPFLAGS"
   LIBS="$saved_LIBS"

   if test x$expat = xno; then
      AC_MSG_ERROR([cannot find expat library])
   fi

   AC_DEFINE(USE_EXPAT, [1], [Use Expat XML Parser])
fi


dnl ------ Startup Notification ---------------------------------------------

if test x$enable_startup_notification != xno; then
  PKG_CHECK_MODULES(SN, libstartup-notification-1.0, ,
			   AC_MSG_ERROR([*** Required Startup Notification Librays not installed ***]))

  AC_DEFINE(USE_LIBSN, [1], [Has StartupNotification Support])
fi


dnl ----- Gconf -------------------------------------------------------------

if test x$enable_gconf != xno; then

  PKG_CHECK_MODULES(GCONF, gconf-2.0, ,
			   AC_MSG_ERROR([*** Required Gconf librarys not installed ***]))

  AC_PATH_PROG(GCONFTOOL, gconftool-2, no)

  if test x"$GCONFTOOL" = xno; then
     AC_MSG_ERROR([gconftool-2 executable not found in your path - should be installed with GConf])
  fi 

  AC_DEFINE(USE_GCONF, [1], [Has GConf Support])	

fi

AM_GCONF_SOURCE_2

AM_CONDITIONAL(ENABLE_SCHEMAS, test x$enable_gconf != xno)

dnl ----- Session Manager ---------------------------------------------------

if test x$enable_session != xno; then

  AC_CHECK_LIB(SM, SmcSaveYourselfDone,
               [AC_CHECK_HEADERS(X11/SM/SMlib.h,
                                LIBMB_LIBS="-lSM -lICE $LIBMB_LIBS" found_sm=nn)],
               , $LIBMB_LIBS)		 

   if test "$found_sm" = "no"; then
      AC_MSG_ERROR([--enable-session requested and -lSM not found])
      exit 1
   fi

   AC_DEFINE(USE_SM, [1], [Has support for session manager connection])
fi

dnl ----- Xsettings ---------------------------------------------------------
dnl FIXME FIXME: Avoid craziness with pkg-config and check for xsetting proper

if test x$enable_standalone != xyes && test x$enable_standalone_xft != xyes; then
  AC_MSG_CHECKING([for libmb xsettings support])

  if $PKG_CONFIG --libs libmb | grep -i xsettings ; then
     mb_have_xsettings="yes"
     AC_DEFINE(USE_XSETTINGS, [1], [Use XSettings])
     AC_MSG_RESULT([yes])
  else
     mb_have_xsettings="no"	
     AC_MSG_RESULT([no])
  fi

fi

dnl ------ GCC flags --------------------------------------------------------

if test "x$GCC" = "xyes"; then

   if test x$enable_gprof = xyes; then
         GCC_WARNINGS="$GCC_WARNINGS -pg"
	 AC_MSG_RESULT([Enabling gprof Support.])	
   fi	 

   if test x$enable_gcov = xyes; then
         GCC_WARNINGS="$GCC_WARNINGS -fprofile-arcs -ftest-coverage"
	 AC_MSG_RESULT([Enabling gcov Support.])	
   fi	 

   GCC_WARNINGS="-g -Wall -fno-strict-aliasing $GCC_WARNINGS"
   LIBMB_CFLAGS="$GCC_WARNINGS $LIBMB_CFLAGS"

else
   if test x$enable_gprof = xyes; then
      AC_MSG_WARN([*** Need gcc compiler for gprof support])
   fi

   if test x$enable_gcov = xyes; then
      AC_MSG_WARN([*** Need gcc compiler for gcov support])
   fi
fi


dnl ------ Substitute in found libs, clags to Makefiles etc -----------------

AC_SUBST(LIBMB_CFLAGS)
AC_SUBST(LIBMB_LIBS)

AC_SUBST(COMPO_CFLAGS)
AC_SUBST(COMPO_LIBS)

AC_SUBST(EXPAT_LIBS)
AC_SUBST(EXPAT_CFLAGS)

AC_SUBST(SN_LIBS)
AC_SUBST(SN_CFLAGS)

AC_SUBST(GCONF_LIBS)
AC_SUBST(GCONF_CFLAGS)
AC_SUBST(GCONF_SCHEMA_SUBDIRS)

AC_SUBST(XFIXES_CFLAGS)
AC_SUBST(XFIXES_LIBS)

dnl ------ Standard Stuff -

# probably needs updating but now we know its safe for x-compiles
AC_C_CONST
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strchr strdup strsep])

# FIXME: install mbsession ?

AC_OUTPUT([
Makefile
src/Makefile  
data/Makefile
data/themes/Makefile
data/themes/Default/Makefile
data/themes/blondie/Makefile
data/themes/MBOpus/Makefile
data/schemas/Makefile
])

#
#util/Makefile 
#themes/Makefile


dnl ==========================================================================
echo "
                    Matchbox-wm $VERSION
                  ======================

        prefix:                 ${prefix}
        source code location:   ${srcdir}
        compiler:               ${CC} ${X_LIBS}

        Building with Debug:                ${enable_debug}
        Building with gcov:                 ${enable_gcov}
        Building with gprof:                ${enable_gprof}

        Building Standalone:                ${enable_standalone}
        Building Standalone with Xft:       ${enable_standalone_xft}

        Building with Keyboard:             ${enable_keyboard}
	Building with XRM support           ${enable_xrm}
        Building with wm ping protocol:     ${enable_ping_protocol}
        Building with Alt input Windows:    ${enable_alt_input_wins}
        Building with Expat:                ${enable_expat}
        Building with XSync:                ${enable_xsync}
        Building with XSettings:            ${mb_have_xsettings}
        Building with Startup-Notification: ${enable_startup_notification}
        Building with GConf:                ${enable_gconf}
        Building with Session support:      ${enable_session}
        Building with Composite manager:    ${enable_composite}



"
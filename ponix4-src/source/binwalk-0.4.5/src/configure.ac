dnl Many thanks to Anthony Basile for the initial configure script and Makefile

AC_PREREQ([2.65])
AC_INIT([binwalk],[0.4.5],[http://code.google.com/p/binwalk/issues/list])
AC_CONFIG_SRCDIR([md5.c])

AC_PROG_CC
AC_LANG(C)

CFLAGS="-Wall $CFLAGS"
LIBS="$LIBS -lz -lpthread -lmagic"
LDFLAGS="$LDFLAGS $LIBS"

AC_ARG_ENABLE([static],    [  --enable-static         perform static build ], [STATIC=y], [STATIC=n])
AC_ARG_ENABLE([updates],   [  --disable-updates       disable update features that rely on libcurl ], [CURL=n], [CURL=y])

AC_TYPE_SIZE_T

AC_FUNC_MALLOC
AC_HEADER_STDC

AC_CHECK_HEADERS([arpa/inet.h fcntl.h stdlib.h string.h unistd.h pthread.h],[],[echo "error: missing necessary header file" && exit 1])

AC_CHECK_FUNCS([memset strdup strstr strtol pthread_create])

AC_CHECK_LIB([magic],[magic_buffer],[],[echo "error: missing libmagic library, get it from ftp://ftp.astron.com/pub/file/" && exit 1],[])
AC_CHECK_HEADERS([magic.h],[],[echo "error: missing libmagic header files, get them from ftp://ftp.astron.com/pub/file/" && exit 1])

if test $CURL = y
then
	AC_CHECK_LIB([curl],[curl_easy_init],[],[echo "error: missing libcurl library" && exit 1],[])
	AC_CHECK_HEADERS([curl/curl.h],[],[echo "error: missing curl header files" && exit 1])

	LDFLAGS="-lcurl $LDFLAGS"
else
	CFLAGS="-DNOCURL $CFLAGS"
fi

dnl curl static libs only required if we're doing a static build
if test $STATIC = y
then
	CLIBS=$(curl-config --static-libs)
	LDFLAGS="-static $CLIBS $LDFLAGS"
fi

cp confdefs.h config.h

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

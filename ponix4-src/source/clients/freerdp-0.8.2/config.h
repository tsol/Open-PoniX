/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Big endian */
/* #undef B_ENDIAN */

/* the name of the file descriptor member of DIR */
/* #undef DIR_FD_MEMBER_NAME */

#ifdef DIR_FD_MEMBER_NAME
# define DIR_TO_FD(Dir_p) ((Dir_p)->DIR_FD_MEMBER_NAME)
#else
# define DIR_TO_FD(Dir_p) -1
#endif

    

/* Disable TLS encryption */
/* #undef DISABLE_TLS */

/* EGD */
/* #undef EGD_SOCKET */

/* Define to 1 if you have the declaration of `dirfd', and to 0 if you don't.
   */
#define HAVE_DECL_DIRFD 1

/* Define to 1 if you have the declaration of `tzname', and to 0 if you don't.
   */
/* #undef HAVE_DECL_TZNAME */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the `dirfd' function. */
#define HAVE_DIRFD 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you have the iconv() function. */
#define HAVE_ICONV 1

/* Define to 1 if you have the <iconv.h> header file. */
#define HAVE_ICONV_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <langinfo.h> header file. */
#define HAVE_LANGINFO_H 1

/* Define to 1 if you have the `crypto' library (-lcrypto). */
/* #undef HAVE_LIBCRYPTO */

/* Define if you have libsamplerate */
/* #undef HAVE_LIBSAMPLERATE */

/* Define to 1 if you have the `ssl' library (-lssl). */
/* #undef HAVE_LIBSSL */

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <mntent.h> header file. */
#define HAVE_MNTENT_H 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the `setmntent' function. */
#define HAVE_SETMNTENT 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if `f_namelen' is a member of `struct statfs'. */
#define HAVE_STRUCT_STATFS_F_NAMELEN 1

/* Define to 1 if `f_namemax' is a member of `struct statfs'. */
/* #undef HAVE_STRUCT_STATFS_F_NAMEMAX */

/* Define to 1 if `f_namelen' is a member of `struct statvfs'. */
/* #undef HAVE_STRUCT_STATVFS_F_NAMELEN */

/* Define to 1 if `f_namemax' is a member of `struct statvfs'. */
#define HAVE_STRUCT_STATVFS_F_NAMEMAX 1

/* Define to 1 if `tm_zone' is a member of `struct tm'. */
#define HAVE_STRUCT_TM_TM_ZONE 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/filio.h> header file. */
/* #undef HAVE_SYS_FILIO_H */

/* Define to 1 if you have the <sys/modem.h> header file. */
/* #undef HAVE_SYS_MODEM_H */

/* Define to 1 if you have the <sys/mount.h> header file. */
#define HAVE_SYS_MOUNT_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/statfs.h> header file. */
#define HAVE_SYS_STATFS_H 1

/* Define to 1 if you have the <sys/statvfs.h> header file. */
#define HAVE_SYS_STATVFS_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/strtio.h> header file. */
/* #undef HAVE_SYS_STRTIO_H */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/vfs.h> header file. */
#define HAVE_SYS_VFS_H 1

/* Define to 1 if your `struct tm' has `tm_zone'. Deprecated, use
   `HAVE_STRUCT_TM_TM_ZONE' instead. */
#define HAVE_TM_ZONE 1

/* Define to 1 if you don't have `tm_zone' but do have the external array
   `tzname'. */
/* #undef HAVE_TZNAME */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define as const if the declaration of iconv() needs const. */
#define ICONV_CONST 

/* IPv6 */
#define IPv6 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Little endian */
#define L_ENDIAN 1

/* Alignment */
/* #undef NEED_ALIGN */

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "freerdp"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "freerdp-devel@lists.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "freerdp"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "freerdp 0.8.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "freerdp"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.8.2"

/* Whether statfs requires two arguments and struct statfs has bsize property
   */
/* #undef STAT_STATFS2_BSIZE */

/* Whether statfs requires 2 arguments and struct statfs has fsize */
/* #undef STAT_STATFS2_FSIZE */

/* Whether statfs requires 2 arguments and struct fs_data is available */
/* #undef STAT_STATFS2_FS_DATA */

/* Whether statfs requires 3 arguments */
/* #undef STAT_STATFS3_OSF1 */

/* Whether statfs requires 4 arguments */
/* #undef STAT_STATFS4 */

/* Whether statvfs() is available */
#define STAT_STATVFS 1

/* Whether statvfs64() is available */
/* #undef STAT_STATVFS64 */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Version number of package */
#define VERSION "0.8.2"

/* Turn on debugging messages */
/* #undef WITH_DEBUG */

/* Turn on debugging messages */
/* #undef WITH_DEBUG_CHANNEL */

/* Turn on debugging messages */
/* #undef WITH_DEBUG_KBD */

/* Turn on debugging messages */
/* #undef WITH_DEBUG_RDP5 */

/* Turn on debugging messages */
/* #undef WITH_DEBUG_SCARD */

/* Turn on debugging messages */
/* #undef WITH_DEBUG_SERIAL */

/* Turn on debugging messages */
/* #undef WITH_DEBUG_SOUND */

/* old version of PCSC */
/* #undef WITH_PCSC120 */

/* Define if smartcard is enabled */
/* #undef WITH_SCARD */

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to 1 if the X Window System is missing or not being used. */
/* #undef X_DISPLAY_MISSING */

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* type to use in place of socklen_t if not defined */
/* #undef socklen_t */

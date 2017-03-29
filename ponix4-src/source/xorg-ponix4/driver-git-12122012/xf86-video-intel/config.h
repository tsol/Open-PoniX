/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

#include "xorg-server.h"

/* Builder description */
/* #undef BUILDER_DESCRIPTION */

/* Enable memory debugging */
/* #undef DEBUG_MEMORY */

/* Enable pixmap debugging */
/* #undef DEBUG_PIXMAP */

/* Default acceleration method */
#define DEFAULT_ACCEL_METHOD UXA

/* Enable XvMC support */
/* #undef ENABLE_XVMC */

/* Enable all debugging */
/* #undef HAS_DEBUG_FULL */

/* Enable pixman glyph cache */
/* #undef HAS_PIXMAN_GLYPHS */

/* Define to 1 if you have the <dgaproc.h> header file. */
#define HAVE_DGAPROC_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Are we in a git checkout? */
#define HAVE_DOT_GIT 1

/* Enable DRI driver support */
#define HAVE_DRI1 1

/* Define to 1 if you have the <dri2.h> header file. */
#define HAVE_DRI2_H 1

/* Define to 1 if you have the <dristruct.h> header file. */
#define HAVE_DRISTRUCT_H 1

/* Define to 1 if you have the <dri.h> header file. */
#define HAVE_DRI_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <sarea.h> header file. */
#define HAVE_SAREA_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/sysinfo.h> header file. */
#define HAVE_SYS_SYSINFO_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Enable udev-based monitor hotplug detection */
/* #undef HAVE_UDEV */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Use valgrind intrinsics to suppress false warnings */
/* #undef HAVE_VALGRIND */

/* Define to 1 if you have the <xaa.h> header file. */
/* #undef HAVE_XAA_H */

/* Assume KMS support */
/* #undef KMS_ONLY */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Disable internal debugging */
#define NDEBUG 1

/* Name of package */
#define PACKAGE "xf86-video-intel"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://bugs.freedesktop.org/enter_bug.cgi?product=xorg"

/* Define to the full name of this package. */
#define PACKAGE_NAME "xf86-video-intel"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "xf86-video-intel 2.20.15"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "xf86-video-intel"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.20.15"

/* Major version of this package */
#define PACKAGE_VERSION_MAJOR 2

/* Minor version of this package */
#define PACKAGE_VERSION_MINOR 20

/* Patch version of this package */
#define PACKAGE_VERSION_PATCHLEVEL 15

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Assume only UMS (no KMS) support */
/* #undef UMS_ONLY */

/* Assume asynchronous swap support */
/* #undef USE_ASYNC_SWAP */

/* Assume "fast reloc" support */
/* #undef USE_FASTRELOC */

/* Use automagic builder description */
#define USE_GIT_DESCRIBE 1

/* Enable glamor acceleration */
/* #undef USE_GLAMOR */

/* Assume "handle LUT" support */
/* #undef USE_HANDLE_LUT */

/* Enable SNA support */
#define USE_SNA 1

/* Assume USERPTR support */
/* #undef USE_USERPTR */

/* Enable UXA support */
#define USE_UXA 1

/* Version number of package */
#define VERSION "2.20.15"

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Builder description */
/* #undef BUILDER_DESCRIPTION */

/* Enable memory debugging */
/* #undef DEBUG_MEMORY */

/* Enable pixmap debugging */
/* #undef DEBUG_PIXMAP */

/* Enable synchronous rendering for debugging */
/* #undef DEBUG_SYNC */

/* Default acceleration method */
#define DEFAULT_ACCEL_METHOD SNA

/* Default have_dri2 driver path */
#define DRI_DRIVER_PATH "/usr/xorg/lib/dri"

/* Enable XvMC support */
/* #undef ENABLE_XVMC */

/* Enable all debugging */
/* #undef HAS_DEBUG_FULL */

/* Enable pixman glyph cache */
#define HAS_PIXMAN_GLYPHS 1

/* Enable pixman triangle rasterisation */
#define HAS_PIXMAN_TRIANGLES 1

/* Enable if your compiler supports the Intel __sync_* atomic primitives */
#define HAVE_ATOMIC_PRIMITIVES 1

/* Found a useable cpuid.h */
#define HAVE_CPUID_H 1

/* Define to 1 if you have the <dgaproc.h> header file. */
/* #undef HAVE_DGAPROC_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Are we in a git checkout? */
/* #undef HAVE_DOT_GIT */

/* Enable DRI1 driver support */
/* #undef HAVE_DRI1 */

/* Enable DRI2 driver support */
#define HAVE_DRI2 1

/* Define to 1 if you have the <dri2.h> header file. */
#define HAVE_DRI2_H 1

/* Enable DRI3 driver support */
#define HAVE_DRI3 1

/* Define to 1 if you have the <dristruct.h> header file. */
/* #undef HAVE_DRISTRUCT_H */

/* Define to 1 if you have the <dri.h> header file. */
/* #undef HAVE_DRI_H */

/* Define to 1 if you have the `getline' function. */
#define HAVE_GETLINE 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Enable if you have libatomic-ops-dev installed */
/* #undef HAVE_LIB_ATOMIC_OPS */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <misyncshm.h> header file. */
#define HAVE_MISYNCSHM_H 1

/* Define to 1 if you have the <misyncstr.h> header file. */
#define HAVE_MISYNCSTR_H 1

/* Define to 1 if MIT-SHM is available */
/* #undef HAVE_MIT_SHM */

/* Enable PRESENT driver support */
#define HAVE_PRESENT 1

/* Define to 1 if you have the <present.h> header file. */
#define HAVE_PRESENT_H 1

/* Define to 1 if you have the <sarea.h> header file. */
/* #undef HAVE_SAREA_H */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strlcat' function. */
/* #undef HAVE_STRLCAT */

/* Define to 1 if you have the `strlcpy' function. */
/* #undef HAVE_STRLCPY */

/* Define to 1 if you have the `strndup' function. */
#define HAVE_STRNDUP 1

/* Define to 1 if `totalram' is a member of `struct sysinfo'. */
#define HAVE_STRUCT_SYSINFO_TOTALRAM 1

/* Define to 1 if you have the <sys/ipc.h> header file. */
#define HAVE_SYS_IPC_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/sysinfo.h> header file. */
#define HAVE_SYS_SYSINFO_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Enable udev-based monitor hotplug detection */
#define HAVE_UDEV 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Use valgrind intrinsics to suppress false warnings */
/* #undef HAVE_VALGRIND */

/* Define to 1 if you have the <vgaHW.h> header file. */
#define HAVE_VGAHW_H 1

/* Define to 1 if you have the <X11/extensions/dpmsconst.h> header file. */
/* #undef HAVE_X11_EXTENSIONS_DPMSCONST_H */

/* Define to 1 if you have the <X11/extensions/shmproto.h> header file. */
/* #undef HAVE_X11_EXTENSIONS_SHMPROTO_H */

/* Define to 1 if you have the <X11/extensions/shmstr.h> header file. */
/* #undef HAVE_X11_EXTENSIONS_SHMSTR_H */

/* Define to 1 if you have the <X11/extensions/Xinerama.h> header file. */
/* #undef HAVE_X11_EXTENSIONS_XINERAMA_H */

/* Define to 1 if you have the <X11/extensions/XShm.h> header file. */
/* #undef HAVE_X11_EXTENSIONS_XSHM_H */

/* Define to 1 if you have the <xaa.h> header file. */
/* #undef HAVE_XAA_H */

/* Define to 1 if shared memory segments are released deferred. */
/* #undef IPC_RMID_DEFERRED_RELEASE */

/* Assume KMS support */
#define KMS 1

/* libexec directory */
#define LIBEXEC_PATH "/usr/xorg/libexec"

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
#define PACKAGE_STRING "xf86-video-intel 2.99.916"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "xf86-video-intel"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.99.916"

/* Major version of this package */
#define PACKAGE_VERSION_MAJOR 2

/* Minor version of this package */
#define PACKAGE_VERSION_MINOR 99

/* Patch version of this package */
#define PACKAGE_VERSION_PATCHLEVEL 916

/* installation prefix */
#define PREFIX_PATH "/usr/xorg"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Enable "TearFree" by default */
/* #undef TEARFREE */

/* Assume UMS support */
/* #undef UMS */

/* Assume asynchronous swap support */
/* #undef USE_ASYNC_SWAP */

/* Enable control of the backlight */
#define USE_BACKLIGHT 1

/* Enable use of the backlight helper interfaces */
#define USE_BACKLIGHT_HELPER 1

/* Assume "create2" support */
/* #undef USE_CREATE2 */

/* Use automagic builder description */
/* #undef USE_GIT_DESCRIBE */

/* Enable glamor acceleration */
/* #undef USE_GLAMOR */

/* Assume "rendernode" support */
/* #undef USE_RENDERNODE */

/* Enable SNA support */
#define USE_SNA 1

/* Enable UXA support */
#define USE_UXA 1

/* Version number of package */
#define VERSION "2.99.916"

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

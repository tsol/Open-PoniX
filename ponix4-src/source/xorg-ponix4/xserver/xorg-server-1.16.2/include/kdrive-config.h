/* include/kdrive-config.h.  Generated from kdrive-config.h.in by configure.  */
/* kdrive-config.h.in: not at all generated.                      -*- c -*-
 */

#ifndef _KDRIVE_CONFIG_H_
#define _KDRIVE_CONFIG_H_

#include <dix-config.h>
#include <xkb-config.h>

/* Building kdrive server. */
/* #undef KDRIVESERVER */

/* Include framebuffer support in X servers */
/* #undef KDRIVEFBDEV */

/* Enable touchscreen support */
/* #undef TOUCHSCREEN */

/* Support tslib touchscreen abstraction library */
/* #undef TSLIB */

/* Support KDrive kbd driver */
/* #undef KDRIVE_KBD */

/* Support KDrive mouse driver */
/* #undef KDRIVE_MOUSE */

/* Support KDrive evdev driver */
/* #undef KDRIVE_EVDEV */

/* Verbose debugging output hilarity */
/* #undef DEBUG */

/* Have the backtrace() function. */
#define HAVE_BACKTRACE 1

/* Have execinfo.h for backtrace(). */
#define HAVE_EXECINFO_H 1

#endif /* _KDRIVE_CONFIG_H_ */

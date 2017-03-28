#ifndef AUMIX_H
#define AUMIX_H
#include "../config.h"
/* These next two macros are only for people compiling manually. */
#ifndef DATADIRNAME
#define DATADIRNAME "/usr/local/share/aumix"
#endif				/* DATADIRNAME */
#define AUMIXRC_PATH "/etc"
#define AUMIXRC "aumixrc"
#if defined (HAVE_CURSES) || defined (HAVE_GTK)
#define IN_CURSES 1
#define IN_GTK 2
#define IN_ANY 3
#endif				/* HAVE_CURSES || HAVE_GTK */
#ifndef TRUE			/* defined in ncurses.h */
#define TRUE 1
#endif				/* TRUE */
#ifndef FALSE
#define FALSE 0
#endif				/* FALSE */
#include <stdio.h>
#include <stdlib.h>		/* getenv() */
#ifdef __linux__
#include <getopt.h>		/* getopt() */
#endif				/* __linux__ */
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
/* "The nice thing about standards..." */
#if defined (__FreeBSD__)
#include <machine/soundcard.h>
#else
#if defined (__NetBSD__) || defined (__OpenBSD__)
#include <soundcard.h>		/* OSS emulation */
#undef ioctl
#else
/* BSDI, Linux, Solaris */
#include <sys/soundcard.h>
#endif				/* __NetBSD__ or __OpenBSD__ */
#endif				/* __FreeBSD__ */
#ifdef HAVE_ALSA
#include <sys/asoundlib.h>
#endif				/* HAVE_ALSA */
#define MAXLEVEL 100		/* highest level permitted by OSS drivers */
enum {
	ENOERROR,
	EOPENMIX,		/* trouble opening mixer device */
	EFINDDEVICE,		/* no device found */
	EREADDEV,		/* SOUND_MIXER_READ_DEVMASK */
	EREADRECMASK,		/* SOUND_MIXER_READ_RECMASK */
	EREADRECSRC,		/* SOUND_MIXER_READ_RECSRC */
	EREADSTEREO,		/* SOUND_MIXER_READ_STEREODEVS */
	EWRITERECSRC,		/* SOUND_MIXER_WRITE_RECSRC */
	EREADMIX,		/* MIXER_READ */
	EWRITEMIX,		/* MIXER_WRITE */
	ENOTOPEN,		/* mixer not open */
	EFILE			/* unable to open settings file */
};
#include <sys/param.h>		/* MAXPATHLEN */
#include <sys/time.h>

/*
 * Our `LOCAL_TEXT(STRING)' macro stands in for gettext (STRING)' when using
 * NLS, and simply returns STRING otherwise.
 */
#ifdef HAVE_NLS
#define LOCAL_TEXT(string) gettext (string)
#ifdef gettext_noop
#define LOCAL_TEXT_NOOP(string) gettext_noop(string)
#else
#define LOCAL_TEXT_NOOP(string) (string)
#endif				/* gettext_noop */
#ifdef HAVE_LIBINTL_H
#include <libintl.h>
#include <locale.h>
#endif				/* HAVE_LIBINTL_H */
#else				/* HAVE_NLS not defined */
#define LOCAL_TEXT(string) string
#define LOCAL_TEXT_NOOP(string) string
#endif				/* HAVE_NLS */
/* Debugging */
#ifdef DUMMY_MIXER
#define SOUND_IOCTL(a,b,c)	dummy_ioctl(a,b,c)
#else
#if defined (__NetBSD__) || defined (__OpenBSD__)
#define SOUND_IOCTL(a,b,c)	_oss_ioctl(a,b,c)
#else
#define SOUND_IOCTL(a,b,c)	ioctl(a,b,c)
#endif				/* defined (__NetBSD__) || defined (__OpenBSD__) */
#endif				/* DUMMY_MIXER */

extern int      current_dev, devmask, mixer_fd, recsrc, recmask, stereodevs, mutelevel[SOUND_MIXER_NRDEVICES], mutestate, interactive;
extern int      in_keysbox;
extern char    *dev_label[SOUND_MIXER_NRDEVICES];
extern char    *save_filename;
extern void     RefreshAllSettings(void);
int             CountChannels(int limit);
void            ShowWarning(int error);
void            ExitIfError(int error);
extern int      WriteLevel(int device, int leftright);
extern int      ReadLevel(int device, int *leftright);
extern int      ReadRecSrc(void);
extern int      WriteRecSrc(void);
int             ReadWriteMixer(int device, char *rw, int *left, int *right, char *rp);
int             ourlevel[SOUND_MIXER_NRDEVICES];
int             our_recplay[SOUND_MIXER_NRDEVICES];	/* Store record/play settings for updates. */
int             LoadSettings(void);
int             SaveSettings(void);
int             dummy_ioctl(int fd, unsigned long req, int *arg);
#endif				/* AUMIX_H */

/* $Aumix: aumix/src/common.h,v 1.11 2010/05/06 03:36:47 trevor Exp $
 * copyright (c) 1993, 1996-2000 the authors--see AUTHORS file
 *
 * This file is part of aumix.
 *
 * Aumix is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * Aumix is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * aumix; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef AUMIX_H
#define AUMIX_H
#include "../config.h"
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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#ifdef HAVE_SOUNDCARD_H	
#include <soundcard.h>
#else
#ifdef HAVE_MACHINE_SOUNDCARD_H
#include <machine/soundcard.h>
#endif				/* HAVE_MACHINE_SOUNDCARD_H */
#endif				/* HAVE_SOUNDCARD_H */
#endif				/* HAVE_SYS_SOUNDCARD_H */

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
#include <sys/param.h>	/* PATH_MAX */
#include <sys/time.h>

/*
 * Our `LOCAL_TEXT(STRING)' macro stands in for gettext (STRING)' when using
 * NLS, and simply returns STRING otherwise.
 */
#ifdef ENABLE_NLS
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
#else				/* ENABLE_NLS not defined */
#define LOCAL_TEXT(string) string
#define LOCAL_TEXT_NOOP(string) string
#endif				/* ENABLE_NLS */
/* Debugging */
#ifdef DUMMY_MIXER
#define SOUND_IOCTL(a,b,c)	dummy_ioctl(a,b,c)
#else
#define SOUND_IOCTL(a,b,c)	ioctl(a,b,c)
#endif				/* DUMMY_MIXER */

extern int      current_dev, devmask, mixer_fd, recsrc, recmask, stereodevs, mutelevel[SOUND_MIXER_NRDEVICES], mutestate, interactive;
extern int      in_keysbox;
extern char    *dev_label[SOUND_MIXER_NRDEVICES];
extern char    *save_filename;
int             InitializeMixer(char *device_file);
int             MixerStatus(void);
#if 0
int             ReadWriteMixer(int device, char *rw, int *left, int *right, char *rp);
#endif				/* 0 */
int             SetShowNoninter(int dev);
int             LoadSettings(void);
int             SaveSettings(void);
void            I18nInitialize(void);
#ifdef HAVE_CURSES
void            ReadInteractiveKeys(void);
#endif				/* HAVE_CURSES */
#ifdef HAVE_ALSA
void            AlsaUnmute(void);
#endif				/* HAVE_ALSA */
extern void     RefreshAllSettings(void);
int             CountChannels(int limit);
void            ErrorExitWarn(int error, int mode);
void            Usage(int);
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

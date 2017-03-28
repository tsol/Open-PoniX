/* $Aumix: aumix/src/common.c,v 1.23 2010/05/06 03:36:47 trevor Exp $
 *
 * aumix:  adjust audio mixer
 * copyright (c) 1993, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2008
 * the authors--see AUTHORS file
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

#include "common.h"
#ifdef HAVE_CURSES
#include "curses.h"
#include "gpm-xterm.h"
#endif				/* HAVE_CURSES */
#if defined (HAVE_GTK)
#include "gtk.h"
#endif				/* HAVE_GTK */
#include "interactive.h"

FILE           *OpenDefaultFile(char *mode);
FILE           *setfile;
char           *save_filename = NULL;	/* name of file for saved settings */
char           *device_filename = "/dev/mixer";	/* name of mixer device file */
unsigned short  setfile_opened = FALSE, setfile_write_perm = FALSE, setfile_read_perm = FALSE;
int             current_dev = 0, mixer_fd = -1, mutelevel[SOUND_MIXER_NRDEVICES], devmask = 0, recmask = 0, recsrc = 0, stereodevs = 0, mutestate = 0, interactive = FALSE;
#ifdef USE_OWN_LABELS
/* This is to ease translation--don't use if this copy of aumix is very old.
 * These labels correspond to the soundcard.h from Linux 2.3.12 or FreeBSD
 * 3.2-19990713-STABLE.  The point of having our own copy of these
 * macros is to make it possible to include translations of them.
 * The danger is that they won't always correspond to the sound
 * driver in the kernel.
 */

/* There's probably not enough left to copyright, but...
 * Copyright by Hannu Savolainen 1993
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

char           *dev_label[SOUND_MIXER_NRDEVICES] =
{
	LOCAL_TEXT_NOOP("Volume  "),
	LOCAL_TEXT_NOOP("Bass    "),
	LOCAL_TEXT_NOOP("Treble  "),
	LOCAL_TEXT_NOOP("Synth   "),
	LOCAL_TEXT_NOOP("PCM     "),
	LOCAL_TEXT_NOOP("Speaker "),
	LOCAL_TEXT_NOOP("Line    "),
	LOCAL_TEXT_NOOP("Mic     "),
	LOCAL_TEXT_NOOP("CD      "),
	LOCAL_TEXT_NOOP("Mix     "),
	LOCAL_TEXT_NOOP("PCM 2   "),
	LOCAL_TEXT_NOOP("Record  "),
	LOCAL_TEXT_NOOP("Input   "),
	LOCAL_TEXT_NOOP("Output  "),
	LOCAL_TEXT_NOOP("Line 1  "),
	LOCAL_TEXT_NOOP("Line 2  "),
	LOCAL_TEXT_NOOP("Line 3  "),
	LOCAL_TEXT_NOOP("Digital1"),
	LOCAL_TEXT_NOOP("Digital2"),
	LOCAL_TEXT_NOOP("Digital3"),
	LOCAL_TEXT_NOOP("Phone In"),
	LOCAL_TEXT_NOOP("PhoneOut"),
	LOCAL_TEXT_NOOP("Video   "),
	LOCAL_TEXT_NOOP("Radio   "),
	LOCAL_TEXT_NOOP("Monitor ")
};
#else
char           *dev_label[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_LABELS;
#endif				/* USE_OWN_LABELS */
char           *dev_name[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_NAMES;
/*
   An index into this array gives the device number for the
   corresponding command-line option.
 */
char           *moptindx = "vbtswplmcxWrio123";
char           *mopt;
char           *scheme_name;
extern char    *optarg;
int             usage_ok = 1;

int             main(int argc, char *argv[])
{
	int             optn, ii;
#ifdef HAVE_CURSES
	int             setcolors = FALSE;
#endif				/* HAVE_CURSES */
	/* Internationalization */
#ifdef ENABLE_NLS
	I18nInitialize();
#endif				/* ENABLE_NLS */

	interactive = FALSE;

#ifdef __linux__
	if (access(device_filename, F_OK) != 0)
		device_filename = "/dev/sound/mixer";
#endif

/* Get options from the command line.  Using numbers as options is deprecated,
   but we do it anyway because it makes sense for line1, line2 and line3.
 */
	while (TRUE) {
		optn = getopt(argc, argv, "hILqSd:f:C:v:b:t:s:w:P:p:l:m:c:x:W:R:r:i:o:1:2:3:");
		if (optn == -1)
			break;
/* Funny, strchr() returns a pointer to char, according to the man page,
   but without this cast egcs warns "assignment makes pointer from integer without
   a cast". */
		if ((mopt = (char *) strchr(moptindx, optn))) {
			usage_ok = 0;
			if (mixer_fd == -1)
				ErrorExitWarn(InitializeMixer(device_filename), 'e');
			ErrorExitWarn(SetShowNoninter(mopt - moptindx), 'e');
		} else {
			usage_ok = 0;
			switch (optn) {
			case 'q':	/* query */
				optarg = "q";
				if (mixer_fd == -1)
					ErrorExitWarn(InitializeMixer(device_filename), 'e');
				for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
					if ((1 << ii) & (devmask | recmask))
						ErrorExitWarn(SetShowNoninter(ii), 'e');
				}
				break;
			case 'd':	/* User specified the device file. */
				device_filename = strdup(optarg);
				ErrorExitWarn(InitializeMixer(device_filename), 'e');
				break;
			case 'f':	/* User specified the save file. */
				save_filename = strdup(optarg);
				break;
#ifdef HAVE_CURSES		/* no color schemes for GTK+ */
			case 'C':	/* User specified the color scheme. */
				scheme_name = strdup(optarg);
				setcolors = TRUE;
				interactive = IN_CURSES;
				break;
#endif				/* HAVE_CURSES */
			case 'S':	/* Save to file. */
				if (mixer_fd == -1)
					ErrorExitWarn(InitializeMixer(device_filename), 'e');
				ErrorExitWarn(SaveSettings(), 'e');
				break;
			case 'L':	/* Load from file. */
				if (mixer_fd == -1)
					ErrorExitWarn(InitializeMixer(device_filename), 'e');
				ErrorExitWarn(LoadSettings(), 'e');
				break;
#if defined (HAVE_CURSES) || defined (HAVE_GTK)
			case 'I':	/* User asked for interactive mode. */
				if (!interactive)
					interactive = IN_ANY;
				break;
#endif				/* HAVE_CURSES || HAVE_GTK */
			case 'h':	/* Show help. */
				Usage(EXIT_SUCCESS);
				break;
			default:	/* Show help. */
				Usage(EXIT_FAILURE);
			}
		}
	}
#if defined (HAVE_CURSES) || defined (HAVE_GTK)
/* Be interactive if no options were given. */
	if (!interactive && argc <= 1)
		interactive = IN_ANY;
/* By now we've decided whether to go interactive.
   The -C (color scheme) option implies curses, even if GTK is available.
*/
	if (interactive == IN_ANY) {
#if defined (HAVE_GTK)
		char *env_display;
/* ignore DISPLAY if it is set, but empty */
		if ((env_display = getenv("DISPLAY")) && (*env_display != '\0'))
			interactive = IN_GTK;
#endif				/* HAVE_GTK */
#ifdef HAVE_CURSES
		if (interactive == IN_ANY)
			interactive = IN_CURSES;
#endif				/* HAVE_CURSES */
	}
#endif				/* HAVE_CURSES || HAVE_GTK */
/* By now, interactive is not IN_ANY.  It is IN_GTK only if DISPLAY is set,
   and otherwise defaults to IN_CURSES.  Since interactive contains the right
   value, it should not be changed any more.
*/
#if defined (HAVE_GTK)
/* GTK+ stuff */
	if (interactive == IN_GTK) {
#ifdef HAVE_GTK /* GTK+ 2.X only */
/* Use UTF8 format even if the locale is non-UTF8. */
		bind_textdomain_codeset (PACKAGE, "UTF-8");
#endif
#ifndef DUMMY_MIXER
		if ((mixer_fd = open(device_filename, O_RDWR)) < 0)
			ErrorExitWarn(EOPENMIX, 'e');
#endif				/* DUMMY_MIXER */
		ErrorExitWarn(MixerStatus(), 'e');
		if (!devmask)
			return EFINDDEVICE;
		gtk_init(&argc, &argv);
		InitScreenGTK();
		gtk_main();
		return 0;
	}
#endif				/* HAVE_GTK */
	if (!interactive) {
		if (usage_ok && optn == -1)
			Usage(EXIT_SUCCESS);
	}
	if (mixer_fd == -1)
		ErrorExitWarn(InitializeMixer(device_filename), 'e');
#if HAVE_CURSES
	if (interactive == IN_CURSES) {
		InitCurses();
		if (setcolors == TRUE)
			ErrorExitWarn(InitColors(scheme_name), 'e');
		else
			SetDefaultColors();
		InitScreen();
		StartMouse();
		Inter();
		Gpm_Close();
#endif				/* HAVE_CURSES */
		close(mixer_fd);
#if HAVE_CURSES
		CloseScreen();
	}
#endif				/* HAVE_CURSES */
	exit(EXIT_SUCCESS);
}

void            ErrorExitWarn(int error, int mode)
{
/* Print error messages.  If mode is "e", bail out. */
	char            string[80];
	const char     *errorlist[] =
	{LOCAL_TEXT_NOOP("aumix:  error opening mixer"), LOCAL_TEXT_NOOP("aumix:  no device found"), " aumix:  SOUND_MIXER_READ_DEVMASK", " aumix:  SOUND_MIXER_READ_RECMASK", "aumix:  SOUND_MIXER_READ_RECSRC", "aumix:  SOUND_MIXER_READ_STEREODEVS", "aumix:  SOUND_MIXER_WRITE_RECSRC", "aumix:  MIXER_READ", "aumix:  MIXER_WRITE", LOCAL_TEXT_NOOP("aumix:  mixer not open"), LOCAL_TEXT_NOOP("aumix:  unable to open settings file")};
	if (!error)
		return;
#if HAVE_CURSES
/* Only close screen for curses; GTK+ has not yet been initialized.
   XXX Why the difference in program flow?
*/
	if ((mode == 'e') && (interactive == IN_CURSES))
		CloseScreen();
#endif				/* HAVE_CURSES */
	if (error > 12) {
		sprintf(string, LOCAL_TEXT("aumix:  unknown error %i"), error);
		perror(string);
	} else if (error > 0) {
		perror(LOCAL_TEXT(errorlist[error - 1]));
	}
	fprintf(stderr, "\n");
	if (mode == 'e')	/* exit */
		exit(EXIT_FAILURE);
	else
		return;		/* warn */
}

int             LoadSettings(void)
{
/* Read settings from file.
   We depend on the 'mixer' file descriptor having been set
   (and not clobbered) before entering this routine.
 */

	char            textline[80], tmpstring[80], recplay;
	int             tmp, ii, left, right;
	setfile = OpenDefaultFile("r");
	if (setfile == NULL)
		return EFILE;
	while (fgets(textline, 80, setfile) != NULL) {
		(void) sscanf(textline, "%[^:;, ]%*[:;, ]%10u%*[:;, ]%3u%*[:;, ]%1c\n", tmpstring, &left, &right, &recplay);
		if (!strcmp(tmpstring, "wait")) {
			if (!interactive)
				printf(LOCAL_TEXT("%i ms wait\n"), left);
			usleep(left * 1000);
		} else {
			for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
/* Cycle through names of channels, looking for matches. */
				if (!strcmp(tmpstring, dev_name[ii])) {
					left = (left > MAXLEVEL) ? MAXLEVEL : left;
					left = (left < 0) ? 0 : left;
					right = (right > MAXLEVEL) ? MAXLEVEL : right;
					right = (right < 0) ? 0 : right;
					tmp = left + (right << 8);
					if ((SOUND_IOCTL(mixer_fd, MIXER_WRITE(ii), &tmp) == -1) && !interactive) {
						printf("%s %s\n", dev_name[ii], LOCAL_TEXT("not set"));
					} else {
						if (!interactive)
							printf("%s %s %i, %i", dev_name[ii], LOCAL_TEXT("set to"), left, right);
						if ((1 << ii) & recmask) {
							ErrorExitWarn(ReadRecSrc(), 'e');
							recsrc = (recplay == 'R') ? recsrc | (1 << ii) : recsrc & ~(1 << ii);
							ErrorExitWarn(WriteRecSrc(), 'e');
							if (!interactive)
								printf(", %c", recplay);
						}
						if (!interactive)
							printf("\n");
					}
				}
			}
		}
	}
	fclose(setfile);
	return 0;
}

int             SaveSettings(void)
{
/* Write settings to file.
   We depend on the 'mixer' file descriptor having been set
   (and not clobbered) before entering this routine.
 */
	int             tmp, ii;
	setfile = OpenDefaultFile("w");
	if (setfile == NULL)
		return EFILE;
	ErrorExitWarn(ReadRecSrc(), 'e');
	for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
		if ((1 << ii) & (devmask | recmask)) {
			ErrorExitWarn(ReadLevel(ii, &tmp), 'e');
			fprintf(setfile, "%s:%i:%i:%c\n", dev_name[ii], (tmp & 0xFF), ((tmp >> 8) & 0xFF), ((1 << ii) & recsrc ? 'R' : 'P'));
		}
	}
	if (fclose(setfile))
		return EFILE;
	return 0;
}

int             InitializeMixer(char *device_filename)
{
/* Initialize the mixer.

   Global:

   mixer_fd   = mixer file descriptor reference number

   Return values:

   Success:   0
   Failure:
   EOPENMIX              trouble opening mixer device
   EFINDDEVICE           no device found
   EREADDEV              SOUND_MIXER_READ_DEVMASK
   EREADRECMASK          SOUND_MIXER_READ_RECMASK
   EREADRECSRC           SOUND_MIXER_READ_RECSRC
   EREADSTEREO           SOUND_MIXER_READ_STEREODEVS
 */
#ifdef HAVE_ALSA
	AlsaUnmute();
#endif				/* HAVE_ALSA */
#ifndef DUMMY_MIXER
	if ((mixer_fd = open(device_filename, O_RDWR)) < 0)
		return EOPENMIX;
#endif				/* DUMMY_MIXER */
	ErrorExitWarn(MixerStatus(), 'e');
	if (!devmask)
		return EFINDDEVICE;
	return 0;
}

int             MixerStatus(void)
{
/* Get status of the mixer.

   Global:

   mixer_fd   = mixer file descriptor reference number

   Bit masks indicating:

   devmask    = devices with adjustable levels
   recmask    = valid input devices
   recsrc     = devices currently selected for input
   stereodevs = stereo devices
 */

/*
"If an error has occurred, a value of -1 is returned and errno is set to indicate the error."
--ioctl(2), FreeBSD System Calls Manual, Joseph Koshy

"On success, zero is returned.  On error, -1 is returned, and errno is set appropriately."
--ioctl(2), Linux Programmer's Manual, Rik Faith and Eric S. Raymond

Robert Siemer says that on his Linux system the SOUND_MIXER_READ_DEVMASK ioctl returns a large positive value.
*/

	if (SOUND_IOCTL(mixer_fd, SOUND_MIXER_READ_DEVMASK, &devmask) == -1)
		return EREADDEV;
	if (SOUND_IOCTL(mixer_fd, SOUND_MIXER_READ_RECMASK, &recmask) == -1)
		return EREADRECMASK;
	ErrorExitWarn(ReadRecSrc(), 'e');
	if (SOUND_IOCTL(mixer_fd, SOUND_MIXER_READ_STEREODEVS, &stereodevs) == -1)
		return EREADSTEREO;
	return 0;
}

int             WriteLevel(int device, int leftright)
{
	if (SOUND_IOCTL(mixer_fd, MIXER_WRITE(device), &leftright) == -1)
		return EWRITEMIX;
	return 0;
}

int             ReadLevel(int device, int *leftright)
{
	if (SOUND_IOCTL(mixer_fd, MIXER_READ(device), leftright) == -1)
		return EREADMIX;
	return 0;
}

int             WriteRecSrc(void)
{
	if (SOUND_IOCTL(mixer_fd, SOUND_MIXER_WRITE_RECSRC, &recsrc) == -1)
		return EWRITERECSRC;
	return 0;
}

int             ReadRecSrc(void)
{
	if (SOUND_IOCTL(mixer_fd, SOUND_MIXER_READ_RECSRC, &recsrc) == -1)
		return EREADRECSRC;
	return 0;
}

#if 0
int             ReadWriteMixer(int device, char *rw, int *left, int *right, char *rp)
{
/* Read or write settings.

 * Global:
 *
 *   mixer_fd   = mixer file descriptor reference number
 *
 *   Bit masks indicating:
 *
 *   devmask    = devices with adjustable levels
 *   recmask    = valid input devices
 *   recsrc     = devices currently selected for input
 *   stereodevs = stereo devices
 *
 * Input:
 *
 *   device     = index into the array of mixer devices
 *   rw         = r - read : w - write
 *   left       = left channel setting
 *   right      = right channel setting
 *   rp         = r - record : p - play
 *
 *
 * Output:
 *
 *   left       = left channel setting
 *   right      = right channel setting
 *   rp         = r - record : p - play
 *
 * Return:
 *
 *   success:   = 0
 *   failure:   = 7     SOUND_MIXER_WRITE_RECSRC
 *   failure:   = 8     MIXER_READ
 *   failure:   = 9     MIXER_WRITE
 *   failure:   = 10    mixer not open
 */
	int             rightleft, result;
	if (mixer_fd == -1)	/* We haven't opened the mixer. */
		return ENOTOPEN;
	result = MixerStatus();
	if (result)
		return result;
/* Is the given device index valid? */
	if (!((1 << device) & (devmask | recmask)))
		return 0;
/* Read settings. */
	if (((1 << device) & devmask) & (*rw == 'r') {
		ErrorExitWarn(ReadLevel(device, &rightleft), 'e');
/* Unpack left and right settings. */
		if ((1 << device) & stereodevs) {
			*right = ((rightleft >> 8) & 0xFF);
			*left = rightleft & 0xFF;
		} else {
			*right = ((rightleft >> 8) & 0xFF);
			*left = rightleft & 0xFF;
		}
/* Can the current device be a recording source? */
		if ((1 << device) & recmask) {
			*rp = (1 << device) & recsrc ? 'R' : 'P';
		} else {
			*rp = '\0';
		}
	} else {
/* "But it's got 11, so it *must* be louder." */
		*left = (*left > MAXLEVEL) ? MAXLEVEL : *left;
		*left = (*left < 0) ? 0 : *left;
		*right = (*right > MAXLEVEL) ? MAXLEVEL : *right;
		*right = (*right < 0) ? 0 : *right;
/* Pack left and right settings for writing. */
		rightleft = *left + (*right << 8);
/* Write settings. */
		ErrorExitWarn(WriteLevel(device, rightleft), 'e');
	}
/* Is the current device capable of being a recording source? */
	if ((1 << device) & recmask) {
		*rp = *rp & 0xDF;
		if (*rp == 'R') {
			recsrc = recsrc | (1 << device);
		} else {
			recsrc = recsrc & ~(1 << device);
			*rp = 'P';
		}
/* Set recording or playing mode. */
		ErrorExitWarn(WriteRecSrc(), 'e');
	} else {
		*rp = '\0';
	}
	return 0;
}
#endif				/* 0 */

int             CountChannels(int limit)
{
/* How many mixing channels are less than limit? */
	int             ii, y = 0;
	for (ii = 0; ii < limit; ii++)
		if ((devmask | recmask) & (1 << ii))
			y++;
	return y;
}

FILE           *CheckAndOpen(const char *filename, const char *mode)
{
/* Check whether *filename is a directory, and if not, call fopen on it.
   CheckAndOpen() is meant to be a drop-in replacement for fopen().
 */
	struct stat s;
	int result;

	/* fopen will fail if mode=='w' and filename is a directory, so we
	 * only need to check the mode=='r' case */
	if (*mode == 'r') {
	    result = stat(filename, &s);
		if (result != 0) return NULL; /* stat() failed */
	    if (S_ISDIR(s.st_mode)) return NULL; /* is a directory, so bail */
	}
	return fopen(filename, mode);
}

FILE           *OpenDefaultFile(char *mode)
{
/* Open the settings file for reading or writing.

   Try first ${HOME}/.AUMIXRC, then AUMIXRC_PATH/AUMIXRC;
   become an error generator if neither can be opened.

   Input:

   mode should be either 'r' (read) or 'w' (write)

   Return:

   Success:   pointer to the default settings file structure
   Failure:   NULL
 */
	FILE           *setfile;
	char           *home;
	char            filename[PATH_MAX];
	if (save_filename == NULL) {
		home = getenv("HOME");
		if (home && ((strlen(home) + strlen(AUMIXRC) + 2) < PATH_MAX)) {
			sprintf(filename, "%s/.%s", home, AUMIXRC);
			setfile = CheckAndOpen(filename, mode);
		}
		if (setfile == NULL) {
			if ((strlen(AUMIXRC_PATH) + strlen(AUMIXRC) + 1) < PATH_MAX) {
				sprintf(filename, "%s/%s", AUMIXRC_PATH, AUMIXRC);
				setfile = CheckAndOpen(filename, mode);
			}
		}
		if (setfile == NULL) {
			return NULL;
		}
	} else
		setfile = CheckAndOpen(save_filename, mode);
	return setfile;
}

int             SetShowNoninter(int dev)
{
/* Change or display settings from the command line. */
	char           *devstr;
	char            dest[4];
	int             change = 0, tmp, left, right;
	/* Increase or decrease levels, optionally by a number. */
	if (!strncmp(optarg, "+", 1) || !strncmp(optarg, "-", 1)) {
		ErrorExitWarn(ReadLevel(dev, &tmp), 'e');
		right = tmp >> 8;	/* I'll take the high byte, */
		left = tmp & 0xFF;	/* and you take the low byte. */
		memset(dest, 0, sizeof(dest));
		strncpy(dest, (optarg + 1), 3);
		change = 1;	/* For compatibility with versions 1.15 to 1.17, assume one if no number was given. */
		if (atoi(dest))
			change = atoi(dest);
		if (*optarg == '+') {	/* increase */
			right += change;
			left += change;
			right = (right > MAXLEVEL) ? MAXLEVEL : right;
			left = (left > MAXLEVEL) ? MAXLEVEL : left;
		} else {	/* decrease */
			left -= change;
			right -= change;
			left = (left < 0) ? 0 : left;
			right = (right < 0) ? 0 : right;
		}
		tmp = left + (right << 8);
		ErrorExitWarn(WriteLevel(dev, tmp), 'e');	/* Try to write new settings to the mixer. */
		return 0;
	}
	if ((*optarg == 'R') || (*optarg == 'P')) {
		if ((1 << dev) & recmask) {
			recsrc = (*optarg == 'R') ? recsrc | (1 << dev) : recsrc & ~(1 << dev);
			ErrorExitWarn(WriteRecSrc(), 'e');
		}
		return 0;
	}
	if ((*optarg == 'q') || (*optarg == ' ')) {
		devstr = dev_name[dev];
		ErrorExitWarn(ReadLevel(dev, &tmp), 'e');
		printf("%s %i, %i", dev_name[dev], (tmp & 0xFF), ((tmp >> 8) & 0xFF));
		if ((1 << (dev)) & recmask) {
			ErrorExitWarn(ReadRecSrc(), 'e');
			printf(", %c", ((1 << dev) & recsrc ? 'R' : 'P'));
		}
		printf("\n");
	} else {
		tmp = atoi(optarg);
		tmp = (tmp > MAXLEVEL) ? MAXLEVEL : tmp;
		tmp = (tmp < 0) ? 0 : 257 * tmp;
		ErrorExitWarn(WriteLevel(dev, tmp), 'e');
	}
	return 0;
}

void            Usage(int status)
{
	FILE           *fp;
/* These declarations are for embedding the copyright and version information in the binary for display by utilities such as what(1).  They needn't be translated. */
	static char     version[] = "@(#) aumix " VERSION "\n";
	static char     copyright[] =
	"@(#) copyright (c) 1993, 1996-2010 the authors--see AUTHORS file\n";
	fp = (status == EXIT_SUCCESS) ? stdout : stderr;
	fprintf(fp, LOCAL_TEXT("aumix %s usage: aumix [-<channel option>[[+|-][<amount>]]|<level>|\nR[ecord]|P[lay]|q[uery]]"), VERSION);
#if defined(HAVE_CURSES) || defined(HAVE_GTK)
	fprintf(fp, LOCAL_TEXT(" [-dhILqS]"));
#else
	fprintf(fp, LOCAL_TEXT(" [-dhLqS]"));
#endif
#if defined(HAVE_CURSES)
	fprintf(fp, LOCAL_TEXT(" [-C <color scheme file>]"));
#endif
	fprintf(fp, LOCAL_TEXT(" [-f <rc file>]"));
	fprintf(fp, "\n\n");
	fprintf(fp, LOCAL_TEXT("\
channel options:\n\
  v:  main volume           x:  mix monitor\n\
  b:  bass                  W:  PCM 2\n\
  t:  treble                r:  record\n\
  s:  synthesizer           i:  input gain\n\
  w:  PCM                   o:  output gain\n\
  p:  PC speaker            1:  line 1\n\
  l:  line                  2:  line 2\n\
  m:  microphone            3:  line 3\n\
  c:  CD\n\n\
other options:\n"));
#ifdef HAVE_CURSES
	fprintf(fp, LOCAL_TEXT("\
  C:  specify color scheme\n"));
#endif				/* HAVE_CURSES */
	fprintf(fp, LOCAL_TEXT("\
  d:  adjust a device besides /dev/mixer\n\
  f:  specify file for saving and loading settings (defaults to\n\
      ~/.aumixrc or /etc/aumixrc)\n\
  h:  this helpful message\n"));
#if defined(HAVE_CURSES) || defined(HAVE_GTK)
	fprintf(fp, LOCAL_TEXT("\
  I:  start in interactive mode after doing non-interactive functions\n"));
#endif				/* HAVE_CURSES || HAVE_GTK */
	fprintf(fp, LOCAL_TEXT("\
  L:  load settings\n\
  q:  query all channels and print their settings\n\
  S:  save settings\n"));
	exit(status);
}
/* Initialize I18N.  The initialization amounts to invoking
   setlocale(), bindtextdomain() and textdomain().
   Does nothing if NLS is disabled or missing.  */
void            I18nInitialize(void)
{
	/*
	 * If ENABLE_NLS is defined, assume the existence of the three functions invoked here.
	 */
#ifdef ENABLE_NLS
	/* Set the current locale. */
	setlocale(LC_ALL, "");
	/* Set the text message domain. */
	bindtextdomain((char *) PACKAGE, LOCALEDIR);
	textdomain((char *) PACKAGE);
#endif				/* ENABLE_NLS */
}
#ifdef HAVE_ALSA
/* Unmuting "hardware mute" in ALSA driver.  Currently unmuting is done only
  for the first sound system.  Although it could be done without the library,
  this code requires alsa-lib.
 */
void            AlsaUnmute(void)
{
	int             channel;
	snd_mixer_channel_t cdata =
	{0};
	static snd_mixer_info_t mixer_info =
	{0};
	static int      card_id = 0;
	static int      mixer_id = 0;
	static void    *mixer_handle;
	static struct snd_ctl_hw_info hw_info;
	void           *ctl_handle;
	if (!snd_mixer_open(&mixer_handle, card_id, mixer_id)) {
		snd_mixer_info(mixer_handle, &mixer_info);
		channel = snd_mixer_channels(mixer_handle);
		while (channel >= 0) {
			snd_mixer_channel_read(mixer_handle, channel, &cdata);
/* Turn off hardware mute for all channels. */
			cdata.flags &= ~SND_MIXER_FLG_MUTE_LEFT;
			cdata.flags &= ~SND_MIXER_FLG_MUTE_RIGHT;
			snd_mixer_channel_write(mixer_handle, channel, &cdata);
			--channel;
		}
		snd_mixer_close(mixer_handle);
	}
}
#endif				/* HAVE_ALSA */

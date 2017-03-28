/*
 *  input_layer - Kernel ACPI Event Input Layer Interface
 *
 *  Handles the details of getting kernel ACPI events from the input
 *  layer (/dev/input/event*).
 *
 *  Inspired by (and in some cases blatantly lifted from) Vojtech Pavlik's
 *  evtest.c.
 *
 *  Copyright (C) 2008-2009, Ted Felix (www.tedfelix.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  (tabs at 4)
 */

/* system */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <glob.h>

/* local */
#include "acpid.h"
#include "log.h"
#include "connection_list.h"
#include "event.h"

#include "input_layer.h"

#define DIM(a)  (sizeof(a) / sizeof(a[0]))

struct evtab_entry {
	struct input_event event;
	const char *str;
};

/* Event Table: Events we are interested in and their strings.  Use 
   evtest.c, acpi_genl, or kacpimon to find new events to add to this
   table. */

/*
 * The two numbers (e.g. "00000080 00000000") in each string is a format
 * that Xorg and maybe others expect.
 *
 * See hw/xfree86/os-support/linux/lnx_acpi.c in xserver and specifically
 * lnxACPIGetEventFromOs().
 */

static struct evtab_entry evtab[] = {

	/*** COMMON EVENTS ***/

	{{{0,0}, EV_KEY, KEY_POWER, 1}, "button/power PBTN 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_SUSPEND, 1}, 
 		"button/suspend SUSP 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_SLEEP, 1}, "button/sleep SBTN 00000080 00000000"},
	{{{0,0}, EV_SW, SW_LID, 1}, "button/lid LID close"},
	{{{0,0}, EV_SW, SW_LID, 0}, "button/lid LID open"},
	{{{0,0}, EV_SW, SW_TABLET_MODE, 0}, "video/tabletmode TBLT 0000008A 00000000"},
	{{{0,0}, EV_SW, SW_TABLET_MODE, 1}, "video/tabletmode TBLT 0000008A 00000001"},


	/*** VIDEO ***/

	{{{0,0}, EV_KEY, KEY_ZOOM, 1}, "button/zoom ZOOM 00000080 00000000"},
	/* typical events file has "video.* 00000087" */
	{{{0,0}, EV_KEY, KEY_BRIGHTNESSDOWN, 1},
 		"video/brightnessdown BRTDN 00000087 00000000"},
 	/* typical events file has "video.* 00000086" */
	{{{0,0}, EV_KEY, KEY_BRIGHTNESSUP, 1},
 		"video/brightnessup BRTUP 00000086 00000000"},
 	/* additional events divined from the kernel's video.c */
	{{{0,0}, EV_KEY, KEY_VIDEO_NEXT, 1},
 		"video/next NEXT 00000083 00000000"},
	{{{0,0}, EV_KEY, KEY_VIDEO_PREV, 1},
 		"video/prev PREV 00000084 00000000"},
	{{{0,0}, EV_KEY, KEY_BRIGHTNESS_CYCLE, 1},
 		"video/brightnesscycle BCYC 00000085 00000000"},
	{{{0,0}, EV_KEY, KEY_BRIGHTNESS_ZERO, 1},
 		"video/brightnesszero BZRO 00000088 00000000"},
	{{{0,0}, EV_KEY, KEY_DISPLAY_OFF, 1},
			"video/displayoff DOFF 00000089 00000000"},
	/* procfs on Thinkpad 600X reports "video VID0 00000080 00000000" */
	/* typical events file has "video.* 00000080" */
	{{{0,0}, EV_KEY, KEY_SWITCHVIDEOMODE, 1},
		"video/switchmode VMOD 00000080 00000000"},


 	/*** AUDIO ***/

 	{{{0,0}, EV_KEY, KEY_VOLUMEDOWN, 1},
 		"button/volumedown VOLDN 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_VOLUMEUP, 1},
 		"button/volumeup VOLUP 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_MUTE, 1},
 		"button/mute MUTE 00000080 00000000"},
/* Kernel 3.1 or later required for KEY_MICMUTE */
#ifdef KEY_MICMUTE
	{{{0,0}, EV_KEY, KEY_MICMUTE, 1},
		"button/micmute MICMUTE 00000080 00000000"},
#endif
 	/* cd play/pause buttons */
 	{{{0,0}, EV_KEY, KEY_NEXTSONG, 1},
 		"cd/next CDNEXT 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_PREVIOUSSONG, 1},
 		"cd/prev CDPREV 00000080 00000000"},
 	{{{0,0}, EV_KEY, KEY_PLAYPAUSE, 1},
 		"cd/play CDPLAY 00000080 00000000"},
 	{{{0,0}, EV_KEY, KEY_STOPCD, 1},
 		"cd/stop CDSTOP 00000080 00000000"},


	/*** JACKS ***/

/* This test probably belongs in configure.ac. */
#ifdef SW_HEADPHONE_INSERT
 #ifndef SW_LINEIN_INSERT
  #define SW_LINEIN_INSERT 0x0d
 #endif
	{{{0,0}, EV_SW, SW_HEADPHONE_INSERT, 0},
		"jack/headphone HEADPHONE unplug"},
	{{{0,0}, EV_SW, SW_HEADPHONE_INSERT, 1},
		"jack/headphone HEADPHONE plug"},
	{{{0,0}, EV_SW, SW_MICROPHONE_INSERT, 0},
		"jack/microphone MICROPHONE unplug"},
	{{{0,0}, EV_SW, SW_MICROPHONE_INSERT, 1},
		"jack/microphone MICROPHONE plug"},
	{{{0,0}, EV_SW, SW_LINEOUT_INSERT, 0},
		"jack/lineout LINEOUT unplug"},
	{{{0,0}, EV_SW, SW_LINEOUT_INSERT, 1},
		"jack/lineout LINEOUT plug"},
	{{{0,0}, EV_SW, SW_VIDEOOUT_INSERT, 0},
		"jack/videoout VIDEOOUT unplug"},
	{{{0,0}, EV_SW, SW_VIDEOOUT_INSERT, 1},
		"jack/videoout VIDEOOUT plug"},
	{{{0,0}, EV_SW, SW_LINEIN_INSERT, 0},
		"jack/linein LINEIN unplug"},
	{{{0,0}, EV_SW, SW_LINEIN_INSERT, 1},
		"jack/linein LINEIN plug"},
#else
 #warning SW_HEADPHONE_INSERT not found in input_layer.h. Support for plug/unplug events will be disabled. Please upgrade your kernel headers to Linux-3.2 or newer.
#endif


 	/*** MISCELLANEOUS ***/

	/* blue access IBM button on Thinkpad T42p*/
	{{{0,0}, EV_KEY, KEY_PROG1, 1}, "button/prog1 PROG1 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_VENDOR, 1}, "button/vendor VNDR 00000080 00000000"},
	/* Fn-F2 produces KEY_BATTERY on Thinkpad T42p */
	{{{0,0}, EV_KEY, KEY_BATTERY, 1},
 		"button/battery BAT 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_SCREENLOCK, 1},
 		"button/screenlock SCRNLCK 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_COFFEE, 1}, "button/coffee CFEE 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_WLAN, 1}, "button/wlan WLAN 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_FN_F1, 1}, "button/fnf1 FNF1 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_FN_F2, 1}, "button/fnf2 FNF2 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_FN_F6, 1}, "button/fnf6 FNF6 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_FN_F9, 1}, "button/fnf9 FNF9 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_FN_F10, 1}, "button/fnf10 FF10 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_FN_F11, 1}, "button/fnf11 FF11 00000080 00000000"},
	/* F20 is sometimes used for micmute */
	{{{0,0}, EV_KEY, KEY_F20, 1}, "button/f20 F20 00000080 00000000"},
	/* Fn-F9 produces KEY_F24 on Thinkpad T42p */
	{{{0,0}, EV_KEY, KEY_F24, 1}, "button/f24 F24 00000080 00000000"},
	{{{0,0}, EV_KEY, KEY_KBDILLUMTOGGLE, 1},
 		"button/kbdillumtoggle KBILLUM 00000080 00000000"},

#if 0
	/* These "EV_MSC, 4, x" events cause trouble.  They are triggered */
	/* by unexpected keys on the keyboard.  */
	/* The 4 is MSC_SCAN, so these are actually scan code events.  */
 	/* Apparently there is no KEY_FN_BS, etc..., defined in input.h. */

	/* EV_MSC, MSC_SCAN, KEY_MINUS  This is triggered by the minus key. */
	{{{0,0}, EV_MSC, 4, 12}, "button/fnbs FNBS 00000080 00000000"},

	/* EV_MSC, MSC_SCAN, KEY_EQUAL  Triggered by the equals key. */
	{{{0,0}, EV_MSC, 4, 13}, "button/fnins FNINS 00000080 00000000"},

	/* EV_MSC, MSC_SCAN, KEY_BACKSPACE   Triggered by the backspace key. */
	{{{0,0}, EV_MSC, 4, 14}, "button/fndel FNDEL 00000080 00000000"},

	/* EV_MSC, MSC_SCAN, KEY_E   Triggered by the 'E' key. */
	{{{0,0}, EV_MSC, 4, 18}, "button/fnpgdown FNPGDOWN 00000080 00000000"},
#endif

};

/* special support for the MUTE key, as the key toggles we want to
 * consider repeated keys but don't report them all the time. We just
 * ensure that the number of key presses (MOD 2) is correct.
 */
static const char *
mute_string(struct input_event event)
{
	if (event.type == EV_KEY && event.code == KEY_MUTE) {
		static size_t repeat_count;
		if (event.value == 1) {
			repeat_count = 1;
			return "button/mute MUTE (key pressed)";
		} else if (event.value == 2) {
			repeat_count++;
			return NULL;
		} else if (event.value == 0) {
			if (repeat_count % 2 == 0) {
				return "button/mute MUTE (key released)";
			}
		}
	}
	return NULL;
}

/*----------------------------------------------------------------------*/
/* Given an input event, returns the string corresponding to that event.
   If there is no corresponding string, NULL is returned.  */
static const char *
event_string(struct input_event event)
{
	unsigned i;
	
	/* for each entry in the event table */
	/* ??? Is there a faster way?  This is triggered every time the user
	 *     presses a key.  Maybe a simple hash algorithm?  Or a simple check
	 *     for very common keys (alphanumeric) and bail before this?  */
	for (i = 0; i < DIM(evtab); ++i)
	{
		/* if this is a matching event, return its string */
		if (event.type == evtab[i].event.type  &&
			event.code == evtab[i].event.code  &&
			event.value == evtab[i].event.value) {
			return evtab[i].str;
		}
	}
	
	return NULL;
}

/*-----------------------------------------------------------------*/
/* returns non-zero if the event type/code is one we need */
static int 
need_event(int type, int code)
{
	unsigned i;

	/* for each entry in the event table */
	for (i = 0; i < DIM(evtab); ++i) {
		/* if we found a matching event */
		if (type == evtab[i].event.type  &&
			code == evtab[i].event.code) {
			return 1;
		}
	}

	return 0;
}

/*-----------------------------------------------------------------*/
/* called when an input layer event is received */
static void process_input(int fd)
{
	struct input_event event;
	ssize_t nbytes;
	const char *str;
	static int nerrs;
	struct connection *c;
	char str2[100];

	nbytes = TEMP_FAILURE_RETRY ( read(fd, &event, sizeof(event)) );

	if (nbytes == 0) {
		acpid_log(LOG_WARNING, "input layer connection closed");
		exit(EXIT_FAILURE);
	}
	
	if (nbytes < 0) {
		if (errno == ENODEV) {
			acpid_log(LOG_WARNING, "input device has been disconnected, fd %d",
			          fd);
			delete_connection(fd);
			return;
		}
		acpid_log(LOG_ERR, "input layer read error: %s (%d)",
			strerror(errno), errno);
		if (++nerrs >= ACPID_MAX_ERRS) {
			acpid_log(LOG_ERR,
				"too many errors reading "
				"input layer - aborting");
			exit(EXIT_FAILURE);
		}
		return;
	}

	/* ??? Is it possible for a partial message to come across? */
	/*   If so, we've got more code to write... */
	
	if (nbytes != sizeof(event)) {
		acpid_log(LOG_WARNING, "input layer unexpected length: "
			"%zd   expected: %zd", nbytes, sizeof(event));
		return;
	}

	c = find_connection(fd);
	
	/* if we're getting scancodes, we probably have a keyboard */
	if (event.type == EV_MSC  &&  event.code == MSC_SCAN) {
		if (c)
			c->kybd = 1;  /* appears to be a keyboard device */
	}
	
	/* convert the event into a string */
	if (tpmutefix) {
		str = mute_string(event);
		if (str ==  NULL)
			str = event_string(event);
	} else
		str = event_string(event);
	/* if this is not an event we care about, bail */
	if (str == NULL)
		return;

	/* If we suspect this is a keyboard, and we have enough space, tack a 
	 * "K" on to the end of the event string. */
	if (c  &&  c->kybd  &&  strnlen(str, sizeof(str2)) <= sizeof(str2) - 3) {
		strcpy(str2, str);
		strcat(str2, " K");
		str = str2;
	}
	
	/* if we're locked, don't process the event */
	if (locked()) {
		if (logevents) {
			acpid_log(LOG_INFO,
				"lockfile present, not processing "
				"input layer event \"%s\"", str);
		}
		return;
	}

	if (logevents)
		acpid_log(LOG_INFO,
			"received input layer event \"%s\"", str);
	
	/* send the event off to the handler */
	acpid_handle_event(str);

	if (logevents)
		acpid_log(LOG_INFO,
			"completed input layer event \"%s\"", str);
}

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

/*--------------------------------------------------------------------*/
/* returns non-zero if the file descriptor supports one of the events */
/* supported by event_string().  */
static int 
has_event(int fd)
{
	int type, code;
	unsigned long bit[EV_MAX][NBITS(KEY_MAX)];

	memset(bit, 0, sizeof(bit));
	/* get the event type bitmap */
	ioctl(fd, EVIOCGBIT(0, sizeof(bit[0])), bit[0]);

	/* for each event type */
	for (type = 0; type < EV_MAX; type++) {
		/* if this event type is supported */
		if (test_bit(type, bit[0])) {
			/* skip sync */
			if (type == EV_SYN) continue;
			/* get the event code mask */
			ioctl(fd, EVIOCGBIT(type, sizeof(bit[type])), bit[type]);
			/* for each event code */
			for (code = 0; code < KEY_MAX; code++) {
				/* if this event code is supported */
				if (test_bit(code, bit[type])) {
					/* if we need this event */
					if (need_event(type, code) != 0)
						return 1;
				}
			}
		}
	}
	return 0;
}

/*-----------------------------------------------------------------*
 * open a single input layer file for input  */
int open_inputfile(const char *filename)
{
	int fd;
	struct connection c;
	
	/* O_CLOEXEC: Make sure scripts we exec() (in event.c) don't get our file 
       descriptors. */
	fd = open(filename, O_RDONLY | O_NONBLOCK | O_CLOEXEC);

	if (fd >= 0) {
		char evname[256];
		
		/* if this file doesn't have events we need, indicate failure */
		if (!has_event(fd)) {
			close(fd);
			return -1;
		}

		/* get this event file's name for debugging */
		strcpy(evname, "Unknown");
		ioctl(fd, EVIOCGNAME(sizeof(evname)), evname);

		acpid_log(LOG_DEBUG, "input layer %s (%s) "
			"opened successfully, fd %d", filename, evname, fd);

		/* add a connection to the list */
		c.fd = fd;
		c.process = process_input;
		/* delete_connection() will free */
		c.pathname = malloc(strlen(filename) + 1);
		if (c.pathname)
			strcpy(c.pathname, filename);
		/* assume not a keyboard until we see a scancode */
		c.kybd = 0;

		if (add_connection(&c) < 0) {
			close(fd);
			acpid_log(LOG_ERR,
				"can't add connection for input layer %s (%s)",
				filename, evname);
			return -1;
		}

		return 0;  /* success */
	}
	
	/* open unsuccessful */
	return -1;
}

/*-----------------------------------------------------------------*
 * open each of the appropriate /dev/input/event* files for input  */
void open_input(void)
{
	char *filename = NULL;
	glob_t globbuf;
	unsigned i;
	int success = 0;

	/* get all the matching event filenames */
	glob(ACPID_INPUTLAYERFILES, 0, NULL, &globbuf);

	/* for each event file */
	for (i = 0; i < globbuf.gl_pathc; ++i) {
		filename = globbuf.gl_pathv[i];

		/* open this input layer device file */
		if (open_inputfile(filename) == 0)
			success = 1;
	}

	if (!success)
		acpid_log(LOG_ERR, "cannot open input layer");

	globfree(&globbuf);
}


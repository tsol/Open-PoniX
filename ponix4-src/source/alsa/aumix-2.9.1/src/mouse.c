/* $Aumix: aumix/src/mouse.c,v 1.5 2002/05/07 08:41:38 trevor Exp $
 * copyright (c) 1993, 1996-2000, 2002 the authors--see AUTHORS file
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
#if HAVE_CURSES
#if HAVE_SYSMOUSE
#if HAVE_SYS_CONSIO_H
#include <sys/consio.h>
#include <sys/fbio.h>
#else
#include <machine/console.h>
#endif	/* HAVE_SYS_CONSIO_H */
#include <errno.h>
#endif				/* HAVE_SYSMOUSE */
#include "interactive.h"
#include "curses.h"
#include "mouse.h"
#if HAVE_SYSMOUSE
extern void     SysmouseHandler(int sig);
extern int      Sysm_Wgetch(WINDOW * win);

static int      xpos, ypos, buttons;
static int      cwidth = 8, cheight = 16;
#endif				/* HAVE_SYSMOUSE */

void            StartMouse(void)
{
	Gpm_Connect     conn;
#if HAVE_SYSMOUSE
	mouse_info_t    mi;
	video_info_t    vi;
	int             fd = STDIN_FILENO;
#ifndef FBIO_GETMODE		/* FreeBSD 3.x */
#define FBIO_GETMODE	CONS_GET
#define FBIO_MODEINFO	CONS_MODEINFO
#endif				/* HAVE_SYSMOUSE */
	if (ioctl(fd, FBIO_GETMODE, &vi.vi_mode) != -1 && ioctl(fd, FBIO_MODEINFO, &vi) != -1) {
		cwidth = vi.vi_cwidth;
		cheight = vi.vi_cheight;
	}
	/* Have sysmouse send us SIGUSR2 for mouse state changes. */
	signal(SIGUSR2, SIG_IGN);
	mi.operation = MOUSE_MODE;
	mi.u.mode.mode = 0;
	mi.u.mode.signal = SIGUSR2;
	/*
	 * If successful, register signal handler and our wgetch() replacement.
	 */
	if (ioctl(fd, CONS_MOUSECTL, &mi) != -1) {
		signal(SIGUSR2, SysmouseHandler);
		mi.operation = MOUSE_SHOW;
		ioctl(fd, CONS_MOUSECTL, &mi);
		Wgetch = Sysm_Wgetch;
		return;
	}
#endif				/* HAVE_SYSMOUSE */
	conn.eventMask = (unsigned short) ~0;
	conn.defaultMask = GPM_MOVE | GPM_HARD;
	conn.maxMod = 0;
	conn.minMod = 0;
	/* don't check for < 0, gpm-xterm returns -2 */
	if (Gpm_Open(&conn, 0) != -1) {
		gpm_handler = MouseHandler;
		Wgetch = Gpm_Wgetch;
	} else
		Wgetch = wgetch;
#if HAVE_GETMOUSE
	/* enable ncurses mouse reporting */
#define MMASK	(BUTTON1_PRESSED | BUTTON1_CLICKED | \
		 BUTTON2_PRESSED | BUTTON2_CLICKED)
	(void) mousemask(MMASK, (mmask_t *) NULL);
#endif				/* HAVE_GETMOUSE */
}

int             MouseHandler(Gpm_Event * event, void *data)
{
	if ((event->type & GPM_DOWN) || (event->type & GPM_DRAG)) {
		if (in_keysbox)
			return ' ';	/* Simulate keyboard event. */
		else
			DoMouse(event->x - 1, event->y - 1, GPM_TO_DOMOUSE(event->buttons));
	}
	return 0;
}
#if HAVE_SYSMOUSE
/*
 * Signal handler for SIGUSR2: Retrieves mouse coordinates; converts pixels
 * to rows and columns.
 */
void            SysmouseHandler(int sig)
{
	int             fd = STDIN_FILENO;
	struct mouse_info mi;
	mi.operation = MOUSE_GETINFO;
	if (ioctl(fd, CONS_MOUSECTL, &mi) == -1)
		return;
	xpos = mi.u.data.x;
	ypos = mi.u.data.y;
	/* for cosmetic bug in syscons.c on FreeBSD 3.3/3.4 */
	mi.operation = MOUSE_HIDE;
	ioctl(fd, CONS_MOUSECTL, &mi);
	mi.operation = MOUSE_SHOW;
	ioctl(fd, CONS_MOUSECTL, &mi);
	buttons = mi.u.data.buttons & 3;
}
/*
 * Wait in select() loop.  If interrupted, check for mouse button press and
 * construct a minimal gpm pseudo-event and call MouseHandler(). Otherwise
 * hand over to wgetch().
 */
int             Sysm_Wgetch(WINDOW * win)
{
	fd_set          rfds;
	Gpm_Event       event;
	int             key;
	FD_ZERO(&rfds);
	FD_SET(STDIN_FILENO, &rfds);
	while (select(STDIN_FILENO + 1, &rfds, (fd_set *) NULL, (fd_set *) NULL, (struct timeval *) NULL) <= 0) {
		if (errno == EINTR && buttons) {
			event.buttons = SYSMOUSE_TO_GPM(buttons);
			event.x = xpos / cwidth + 1;
			event.y = ypos / cheight + 1;
			event.type = GPM_DOWN;
			if ((key = MouseHandler(&event, (void *) NULL)) != 0)
				return key;
		}
	}
	return wgetch(win);
}
#endif				/* HAVE_SYSMOUSE */

/* assumes upper left corner is (0, 0) */
void            DoMouse(int x, int y, int b)
{
	int             dev_orig, mouse_dev, ii, jj;
	if ((x < XOFFSET + menu_width) && (b & BUTTON1)) {	/* menu */
		switch (y) {
		case 2:	/* quit */
			Gpm_Close();
			close(mixer_fd);
			CloseScreen();
			exit(EXIT_SUCCESS);
		case 3:	/* load */
			LoadSettings();
			return;
		case 4:	/* save */
			SaveSettings();
			return;
		case 5:	/* keys */
			KeysBox();
			return;
		case 6:	/* mute */
			Muting(MUTE_NO_DEVICE, MUTE_GLOBAL);
			return;
		case 7:	/* only */
			Muting(current_dev, MUTE_ONLY);
			return;
		case 8:	/* undo */
			Muting(MUTE_NO_DEVICE, MUTE_OFF);
		default:
			return;
		}
	}
	mouse_dev = y - YOFFSET;
	jj = 0;
	dev_orig = mouse_dev;
	for (ii = 0; jj <= dev_orig; ii++) {
		if (!((devmask | recmask) & (1 << ii))) {
			mouse_dev++;
		} else {
			jj++;
		}
	}
	if (mouse_dev >= SOUND_MIXER_NRDEVICES)
		return;
	x = x - (XOFFSET + menu_width + R_P_WIDTH);
	if ((x < 0) && (b & BUTTON1)) {
		SwitchRecordPlay(mouse_dev);
		return;
	}
	if ((x < level_width) && (b & BUTTON1)) {
		x = x * level_increment;
		AdjustLevel(mouse_dev, 0, x);
		return;
	}
	x = x - (level_width + label_width + ARROW_WIDTH * 2);
	if (x >= 0 && x < balance_width) {	/* balance */
		x = x * balance_increment;
		if ((((1 << mouse_dev) & stereodevs) && ((1 << mouse_dev) & devmask))) {
			if (b & BUTTON1) {
				AdjustBalance(mouse_dev, 0, x);
			} else {
				if (b & BUTTON2)
					AdjustBalance(mouse_dev, -1, (MAXLEVEL / 2));
			}
		}
		return;
	}
	return;
}
#endif				/* HAVE_CURSES */

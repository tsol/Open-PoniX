/* $Aumix: aumix/src/mouse.h,v 1.2 2002/03/28 09:37:27 trevor Exp $
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

#ifndef AUMIX_MOUSE_H
#define AUMIX_MOUSE_H
#if HAVE_LIBGPM
#include <gpm.h>
#else
#include "gpm-xterm.h"
#endif				/* HAVE_LIBGPM */

/* buttons for DoMouse() */
#define BUTTON1 0x1
#define BUTTON2 0x2

/* convert mouse button representations */
#define SYSMOUSE_TO_GPM(b) \
	((((b) & 1) ? GPM_B_LEFT : 0) | \
	 (((b) & 2) ? GPM_B_MIDDLE : 0))
#define GPM_TO_DOMOUSE(b) \
	((((b) & GPM_B_LEFT) ? BUTTON1 : 0) | \
         (((b) & GPM_B_MIDDLE) ? BUTTON2 : 0))
#define NCURSES_TO_DOMOUSE(b) \
	((((b) & (BUTTON1_PRESSED | BUTTON1_CLICKED)) ? BUTTON1 : 0) | \
	 (((b) & (BUTTON2_PRESSED | BUTTON2_CLICKED)) ? BUTTON2 : 0))

extern void     DoMouse(int x, int y, int b);
extern int      MouseHandler(Gpm_Event * event, void *data);
extern void     StartMouse(void);
#endif				/* AUMIX_MOUSE_H */

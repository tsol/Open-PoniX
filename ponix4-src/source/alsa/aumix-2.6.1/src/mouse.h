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

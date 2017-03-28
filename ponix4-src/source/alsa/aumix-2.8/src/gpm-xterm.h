/* $Aumix: aumix/src/gpm-xterm.h,v 1.3 2002/10/29 21:27:52 trevor Exp $
 * gpm-xterm.h - pseudo client for non-Linux xterm only mouse support.
 *               This code has been extracted from libgpm-0.18 and then
 *               took its own way.
 *
 * Copyright 1994, 1995  rubini@ipvvis.unipv.it (Alessandro Rubini)
 * Copyright 1994        miguel@roxanne.nuclecu.unam.mx (Miguel de Icaza)
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

#ifndef _GPM_XTERM_H_
#define _GPM_XTERM_H_

#include "curses.h"

/* Xtermish stuff */
#define GPM_XTERM_ON \
  printf("\033[?1001s"), fflush(stdout), /* save old hilit tracking */ \
  printf("\033[?1000h"), fflush(stdout)	/* enable mouse tracking */
#define GPM_XTERM_OFF \
printf("\033[?1000l"), fflush(stdout), /* disable mouse tracking */ \
printf("\033[?1001r"), fflush(stdout)	/* restore old hilittracking */
#define GET_TIME(tv) (gettimeofday(&tv, (struct timezone *)NULL))
#define DIF_TIME(t1,t2) ((t2.tv_sec -t1.tv_sec) *1000+(t2.tv_usec-t1.tv_usec)/1000)
#define MAXNBPREVCHAR 4		/* I don't think more is useful, JD */
#define DELAY_MS 500		/* from mc */
/* buttons */
#define	GPM_B_RIGHT 1
#define	GPM_B_MIDDLE 2
#define	GPM_B_LEFT 4
/* event types */
#define GPM_BARE_EVENTS(type) ((type)&(0xF|GPM_ENTER|GPM_LEAVE))
enum Gpm_Etype {
	GPM_MOVE = 1,
	GPM_DRAG = 2,		/* Exactly one in four is active at a time. */
	GPM_DOWN = 4,
	GPM_UP = 8,
	GPM_SINGLE = 16,	/* At most one in three is set. */
	GPM_DOUBLE = 32,
	GPM_TRIPLE = 64,	/* Warning:  I depend on the values. */
	GPM_MFLAG = 128,	/* motion during click? */
	GPM_HARD = 256,		/* If set in the defaultMask, force an already used event to pass over to another handler. */
	GPM_ENTER = 512,	/* enter event, used in Roi's */
	GPM_LEAVE = 1024	/* leave event, used in Roi's */
};
/* event data structure */
enum Gpm_Margin {
	GPM_TOP = 1,
	GPM_BOT = 2,
	GPM_LFT = 4,
	GPM_RGT = 8
};
typedef struct Gpm_Event {
	unsigned char   buttons, modifiers;	/* Try to be a multiple of 4. */
	unsigned short  vc;
	short           dx, dy, x, y;
	enum Gpm_Etype  type;
	int             clicks;
	enum Gpm_Margin margin;
}               Gpm_Event;
/* connection data structure */
#define GPM_MAGIC 0x47706D4C	/* "GpmL" */
typedef struct Gpm_Connect {
	unsigned short  eventMask, defaultMask;
	unsigned short  minMod, maxMod;
	int             pid;
	int             vc;
}               Gpm_Connect;
/* global variables for the client */
extern int      gpm_flag, gpm_ctlfd, gpm_fd, gpm_hflag, gpm_morekeys;
typedef int     Gpm_Handler(Gpm_Event * event, void *clientdata);
extern Gpm_Handler *gpm_handler;
extern void    *gpm_data;
extern int      Gpm_Open(Gpm_Connect *, int);
extern int      Gpm_Close(void);
extern int      Gpm_Getc(FILE *);
#define    Gpm_Getchar() Gpm_Getc(stdin)
extern int      Gpm_Repeat(int millisec);
extern int      Gpm_Wgetch(WINDOW *);
#define Gpm_Getch() (Gpm_Wgetch(stdscr))
/* Disable the functions available in libgpm but not here. */
#define    Gpm_FitValuesM(x, y, margin)
#define    Gpm_FitValues(x,y)
#define    Gpm_FitEvent(ePtr)
#define Gpm_DrawPointer(x,y,fd)
#define GPM_DRAWPOINTER(ePtr)
/* from gpmCfg.h */
#define SELECT_TIME 86400	/* seconds in one day */
#endif				/* _GPM_XTERM_H_ */

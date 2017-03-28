/* $Aumix: aumix/src/curses.h,v 1.8 2003/10/09 23:24:14 trevor Exp $
 * copyright (c) 1993, 1996-2002 the authors--see AUTHORS file
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

#ifndef AUMIX_CURSES_H
#define AUMIX_CURSES_H
#if HAVE_CURSES
#include <signal.h>		/* updates only needed in interactive modes */
#define XOFFSET 0		/* blank columns to leave at the left */
#define YOFFSET 0		/* blank lines to leave at the top */
#if HAVE_NCURSES_H
#include <ncurses.h>
#else
#include <curses.h>
#endif				/* HAVE_NCURSES_H */
#ifndef CTRL
#define CTRL(x) ((x) & 0x1F)
#endif				/* CTRL */
/*
 * Says curs_color(3x):  "the 0 color pair is wired to white on black and
 * cannot be changed".
 */
enum {
	NULL_PAIR,
	ACTIVE_COLOR,
	AXIS_COLOR,
	HANDLE_COLOR,
	HOTKEY_COLOR,
	MENU_COLOR,
	PLAY_COLOR,
	RECORD_COLOR,
	TRACK_COLOR
};
static char    *LANGUAGES[] = {"ca", "de", "el", "en", "es", "fr", "gl", "it",
	"nl", "pl", "pt_BR", "ru", "sv", "uk"};
/*
 * Set to the length of the longest printable text in the
 * ncurses menu, plus one for a blank space at the right.
 */
static int      MENUSIZES[] = {
	8,			/* ca */
	11,			/* de */
	6,			/* el */
	6,			/* en */
	7,			/* es */
	10,			/* fr */
	7,			/* gl */
	7,			/* it */
	9,			/* nl */
	7,			/* pl */
	9,			/* pt_BR */
	6,			/* ru */
	10,			/* sv */
	6,			/* uk */
};

/* These numbers are equal to the length of the longest channel name. */
static int      LABELSIZES[] = {
	13,			/* ca */
	12,			/* de */
	14,			/* el */
	8,			/* en */
	8,			/* es */
	9,			/* fr */
	8,			/* gl */
	8,			/* it */
	8,			/* nl */
	8,			/* pl */
	12,			/* pt_BR */
	13,			/* ru */
	9,			/* sv */
	8,			/* uk */
};
#define ARROW_WIDTH 1		/* width of selection arrow */
#define R_P_WIDTH 1		/* width of record/play indicator */
int             level_increment, balance_increment, menu_width, level_width, label_width, balance_width, levelbalmode;
int             (*Wgetch) (WINDOW * win);
#define Getch() (*Wgetch)(stdscr)
void            WakeUpCurses(void);
void            InitScreenCurses(void);
void            PlaceCursor(void);
void            HighlightLabelCurses(void);
void            KeysBoxCurses(void);
void            ShowMutingCurses(void);
void            SetDefaultColors(void);
void            InitCurses(void);
int             InitColors(char *scheme);
void            Inter(void);
void            DrawLevelBalModeCurses(int dev, int mode);
void            DrawLevelCurses(int dev);
void            EraseLevelCurses(int dev);
void            RedrawBalanceCurses(int dev);
void            DrawRecordPlayCurses(int dev);
void            CloseScreenCurses(void);
void            StartMouse(void);
#endif				/* HAVE_CURSES */
#endif				/* AUMIX_CURSES_H */

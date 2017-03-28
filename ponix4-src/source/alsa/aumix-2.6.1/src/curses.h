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
static char    *LANGUAGES[] = {"de", "en", "es", "fr", "gl", "pl", "pt_BR", "ru", " uk"
};
static int      MENUSIZES[] = {
	11,			/* de */
	6,			/* en */
	7,			/* es */
	10,			/* fr */
	7,			/* gl */
	7,			/* pl */
	6,			/* pt_BR */
	6,			/* ru */
	6,			/* uk */
};

static int      LABELSIZES[] = {
	12,			/* de */
	8,			/* en */
	8,			/* es */
	9,			/* fr */
	8,			/* gl */
	8,			/* pl */
	8,			/* pt_BR */
	13,			/* ru */
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

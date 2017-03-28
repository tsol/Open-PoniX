#ifndef AUMIX_CURSES_H
#define AUMIX_CURSES_H
#if HAVE_CURSES
#include <signal.h>		/* updates only needed in interactive modes */
#define XOFFSET 6
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
	TRACK_COLOR,
};
#define ARROW_WIDTH 1		/* width of selection arrow */
#define LABEL_WIDTH 8		/* columns for labels */
#define R_P_WIDTH 1		/* width of record/play indicator */

int             level_increment, balance_increment, level_width, balance_width, levelbalmode;
int             (*Wgetch) (WINDOW * win);
#define Getch() (*Wgetch)(stdscr)

void            InitScreenCurses(void);
void            PlaceCursor(void);
void            HighlightLabelCurses(void);
void            KeysBoxCurses(void);
void            ShowMutingCurses(void);
void            SetDefaultColors(void);
void            AumixSignalHandler(int signal_number);
void            AumixSigwinchHandler(int signal_number);
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

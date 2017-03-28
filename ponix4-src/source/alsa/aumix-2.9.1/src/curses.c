/* $Aumix: aumix/src/curses.c,v 1.19 2010/05/04 03:16:24 trevor Exp $ */
/* curses.c:  functions specific to ncurses mode
 * copyright (c) 1993, 1996, 1997, 1998, 1999, 2000, 2002, 2004, 2005, 2010
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
#include <ctype.h>		/* tolower() */
#include "curses.h"
#include "interactive.h"
#include "mouse.h"

unsigned char  *key_keys, *key_load, *key_mute, *key_only, *key_quit, *key_save, *key_undo, *lang;
int             current_dev, level_increment, balance_increment, menu_width, level_width, label_width, balance_width, levelbalmode, cols_saved, lines_saved;
int             in_keysbox = FALSE;
static int	cursor_x, cursor_y;	/* keep track of cursor position */

void            InitScreenCurses(void)
{
	int             ii, trackwidth, y = 0;
	static char    *menuitems[] = {
		LOCAL_TEXT_NOOP("Quit"),
		LOCAL_TEXT_NOOP("Load"),
		LOCAL_TEXT_NOOP("Save"),
		LOCAL_TEXT_NOOP("Keys"),
		LOCAL_TEXT_NOOP("Mute"),
		LOCAL_TEXT_NOOP("Only"),
		LOCAL_TEXT_NOOP("Undo")
	};
/* from ALL_LINGUAS in top-level configure.in */
/* This might be nicer with something like:

	typedef struct menulanguage {
		char    *language;
		int      width;
	}               menulanguage;
*/
	char            hotkey[2] = "";
	menu_width = MENUSIZES[1];	/* "en" default */
	for (ii = 0; ii < sizeof(LANGUAGES) / sizeof(*LANGUAGES); ii++) {
		if ((lang = (char *) getenv("LANG")) && !strncmp(lang, LANGUAGES[ii], 5)) {
			menu_width = MENUSIZES[ii];
			break;
		}
	}
	label_width = LABELSIZES[1];	/* "en" default */
	for (ii = 0; ii < sizeof(LANGUAGES) / sizeof(*LANGUAGES); ii++) {
		if ((lang = (char *) getenv("LANG")) && !strncmp(lang, LANGUAGES[ii], 5)) {
			label_width = LABELSIZES[ii];
			break;
		}
	}
	cols_saved = COLS;
	lines_saved = LINES;
	/*
	 * Adjust width of tracks according to number of columns available.
	 */
	/* fail-safe defaults */
	level_increment = 1;
	balance_increment = 1;
	level_width = 1;
	balance_width = 1;
	trackwidth = COLS - XOFFSET - menu_width - R_P_WIDTH - ARROW_WIDTH * 2 - label_width;
	trackwidth = trackwidth - 2;	/* Don't count zero positions in calculations. */
	if (trackwidth > 1) {
		balance_width = trackwidth / 2;
		if (balance_width > MAXLEVEL)
			balance_width = MAXLEVEL;
		balance_increment = MAXLEVEL / balance_width;
		if (MAXLEVEL > (balance_increment * balance_width))
			balance_increment++;
		balance_width = 1 + MAXLEVEL / balance_increment;
		level_width = trackwidth - balance_width;
		if (level_width > 0)
			level_increment = MAXLEVEL / level_width;
		if ((level_increment * level_width) < MAXLEVEL)
			level_increment++;
		level_width = 1 + MAXLEVEL / level_increment;
	}
	bkgdset(COLOR_PAIR(TRACK_COLOR));
	clear();
	for (ii = 0; ii < sizeof(menuitems) / sizeof(*menuitems); ii++) {
		attrset(COLOR_PAIR(MENU_COLOR));
		/*
		 * Leave two lines above menu, one for "aumix" and one for "muted" or "only".
		 */
		mvaddstr(ii + 2, 0, (char *) LOCAL_TEXT(menuitems[ii]));
		attrset(COLOR_PAIR(HOTKEY_COLOR) | A_BOLD);
		hotkey[1] = '\0';	/* Add the null since strncpy won't. */
		strncpy(hotkey, (char *) LOCAL_TEXT(menuitems[ii]), 1);
		mvaddstr(ii + 2, 0, hotkey);
	}
	attrset(COLOR_PAIR(MENU_COLOR) | A_UNDERLINE);
	mvaddstr(0, 0, "aumix");
	attrset(COLOR_PAIR(AXIS_COLOR));
	y = CountChannels(SOUND_MIXER_NRDEVICES);
	if (YOFFSET + y <= LINES) {
		mvaddstr(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH, (char *) LOCAL_TEXT("0"));
		mvaddstr(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + (level_width - strlen(LOCAL_TEXT("Level"))) / 2, (char *) LOCAL_TEXT("Level"));
		mvaddstr(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width - 3, (char *) LOCAL_TEXT("100"));
		mvaddstr(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width + label_width + ARROW_WIDTH * 2, (char *) LOCAL_TEXT("L"));
		mvaddstr(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width + label_width + ARROW_WIDTH * 2 + (balance_width - strlen(LOCAL_TEXT("Balance"))) / 2, (char *) LOCAL_TEXT("Balance"));
		mvaddstr(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width + label_width + ARROW_WIDTH * 2 + balance_width - 1, (char *) LOCAL_TEXT("R"));
	}
	y = 0;			/* Now recycle it for a different use. */
	for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
		if ((1 << ii) & (devmask | recmask)) {
			attrset(COLOR_PAIR(AXIS_COLOR));
/* fill the label area with spaces */
			mvaddnstr(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width, "                 ", label_width + 2);
			/* draw control labels */
			mvaddstr(YOFFSET + y++, XOFFSET + menu_width + R_P_WIDTH + level_width + ARROW_WIDTH, LOCAL_TEXT(dev_label[ii]));
		}
	}
	PlaceCursor();
	RefreshAllSettings();
	HighlightLabelCurses();
}

void            PlaceCursor(void)
{
	/*
	 * The different attributes are to avoid ncurses optimizations. Let's hope ncurses doesn't optimize this away on terminals that can't underline.
	 */
	attrset(COLOR_PAIR(MENU_COLOR) | A_NORMAL);
	mvaddch(0, 4, 'x');
	refresh();
	attrset(COLOR_PAIR(MENU_COLOR) | A_UNDERLINE);
	mvaddch(0, 4, 'x');
	/* Braille-friendly cursor positioning */
	if (cursor_y >= 0) {
		move(cursor_y, cursor_x);
		leaveok(stdscr, FALSE);
	}
	refresh();
	if (cursor_y >= 0) leaveok(stdscr, TRUE);
}

void            HighlightLabelCurses(void)
{
	attrset(COLOR_PAIR(ACTIVE_COLOR) | ((has_colors()) ? A_BOLD : A_REVERSE));
/* fill the label area with spaces */
	mvaddnstr(YOFFSET + CountChannels(current_dev), XOFFSET + menu_width + R_P_WIDTH + level_width + ARROW_WIDTH, "               ", label_width);
	mvaddstr(YOFFSET + CountChannels(current_dev), XOFFSET + menu_width + R_P_WIDTH + level_width + ARROW_WIDTH, LOCAL_TEXT(dev_label[current_dev]));
	PlaceCursor();
}

void            KeysBoxCurses(void)
{
	/*
	 * "This interface is made obsolete by setitimer(2)."--alarm(3), FreeBSD
	 */
	alarm(0);		/* Disable updates. */
	timeout(-1);		/* Wait indefinitely for input. */
	in_keysbox = TRUE;
	clear();
	attrset(COLOR_PAIR(AXIS_COLOR));
	move(3, 0);
	addstr((char *) LOCAL_TEXT("page arrows\n"));
	addstr((char *) LOCAL_TEXT("tab enter < > , .\n"));
	addstr((char *) LOCAL_TEXT("+ - [ ] arrows digits\n"));
	addstr((char *) LOCAL_TEXT("space\n"));
	addstr((char *) LOCAL_TEXT("|\n"));
	printw((char *) LOCAL_TEXT("\nPress a key or mouse button to resume."));
	move(1, 0);
	printw((char *) LOCAL_TEXT("Key                       Function\n"));
	printw((char *) LOCAL_TEXT("------------------------- --------------------"));
	mvaddstr(3, 26, (char *) LOCAL_TEXT("change channel\n"));
	mvaddstr(4, 26, (char *) LOCAL_TEXT("toggle level/balance\n"));
	mvaddstr(5, 26, (char *) LOCAL_TEXT("adjust slider\n"));
	mvaddstr(6, 26, (char *) LOCAL_TEXT("toggle record/play\n"));
	mvaddstr(7, 26, (char *) LOCAL_TEXT("center balance\n"));
	refresh();

	Getch();
	alarm(REFRESH_PERIOD);	/* Enable updates again. */
	in_keysbox = FALSE;
	timeout(1000);
	clear();
	InitScreen();
	DrawLevelBalModeCurses(current_dev, levelbalmode);
	HighlightLabelCurses();
}

void            ShowMutingCurses(void)
{
	attrset(COLOR_PAIR(AXIS_COLOR));
	move(1, 0);		/* Move down one line, for the title "aumix". */
	switch (mutestate) {
	case MUTE_OFF:
		printw("%s", LOCAL_TEXT("     "));
		break;
	case MUTE_GLOBAL:
		printw("%s", LOCAL_TEXT("muted"));
		break;
	case MUTE_ONLY:
		printw("%s", LOCAL_TEXT("only "));
	}
	PlaceCursor();
}

void            SetDefaultColors(void)
{
	int             background = COLOR_BLACK;
	int             foreground = COLOR_WHITE;
#if defined HAVE_USEDEFAULT
	if (use_default_colors() == OK) {
		background = -1;
		foreground = -1;
	}
#endif				/* HAVE_USEDEFAULT */
	init_pair(MENU_COLOR, COLOR_CYAN, COLOR_BLUE);
	init_pair(HOTKEY_COLOR, COLOR_RED, COLOR_BLUE);
	init_pair(HANDLE_COLOR, foreground, background);
	init_pair(TRACK_COLOR, COLOR_BLUE, background);
	init_pair(RECORD_COLOR, COLOR_RED, background);
	init_pair(PLAY_COLOR, COLOR_GREEN, background);
	init_pair(ACTIVE_COLOR, COLOR_YELLOW, COLOR_RED);
	init_pair(AXIS_COLOR, foreground, background);
}

void            WakeUpCurses(void)
{
	if ((COLS != cols_saved) || (LINES != lines_saved)) {	/* Window was resized. */
		InitScreen();
		DrawLevelBalMode(current_dev, levelbalmode);
	}
	RefreshNewSettings();
}

void            InitCurses(void)
{
	char           *s;
	ReadInteractiveKeys();
	/*
	 * When running in an xterm, make sure DISPLAY is set, because ncurses won't process mouse activity properly if it isn't.
	 */
	if ((s = getenv("TERM")) && !strncmp(s, "xterm", 5))
		if ((s = getenv("DISPLAY")) == NULL)
			putenv("DISPLAY=");
	initscr();
	noecho();
	leaveok(stdscr, TRUE);
	keypad(stdscr, TRUE);
	meta(stdscr, TRUE);	/* not default on FreeBSD 4.0 before 1999-11-30 */
	cbreak();
	/*
	 * says curs_kernel(3):
	 * 
	 * The SVr4 man pages warn that the return value of curs_set "is currently incorrect".  This implementation gets it right, but it may be unwise to count on the correctness of the return value anywhere else.
	 */
	(void) curs_set(0);
	timeout(1000);
	start_color();
}

int             InitColors(char *scheme)
{
	FILE           *schemefile;
	char            filename[PATH_MAX], *item, *fore, *back;
	char            buf[BUFSIZ], *p;
	static char    *colors[] =
	{"black", "red", "green", "yellow", "blue", "magenta", "cyan", "white"};
	static char    *items[] =
	{"active", "axis", "handle", "hotkey", "menu", "play", "record", "track"};
	int             bg, fg, ii;
	if ((strlen(scheme) + strlen(DATADIR) + 1) >= PATH_MAX)
		ErrorExitWarn(EFILE, 'e');
	sprintf(filename, "%s", scheme);
	schemefile = fopen(filename, "r");
	if (schemefile == NULL) {
		sprintf(filename, "%s/%s", DATADIR, scheme);
		schemefile = fopen(filename, "r");
		if (schemefile == NULL)
			return EFILE;
	}
	while (fgets(buf, BUFSIZ, schemefile)) {
		p = buf;
		while (*p && isspace(*p))	/* skip whitespace */
			p++;
		if (*p == 0 || *p == '#')	/* skip empty lines and comments */
			continue;
		/*
		 * Says the Linux strtok(3) man page:
		 * 
		 * Never use this function.  This function modifies its first argument.  The identity of the delimiting character is lost.  This function cannot be used on constant strings.
		 * 
		 * and on FreeBSD:
		 * 
		 * This interface is obsoleted by strsep(3).
		 * 
		 * We bad.
		 */
		item = strtok(p, " \t\n");	/* Get first item. */
		fore = strtok(NULL, " \t\n");
		if (!fore)	/* only one item */
			continue;
		back = strtok(NULL, " \t\n");
		if (!back)	/* only two items */
			continue;
		bg = COLOR_BLACK;
		fg = COLOR_WHITE;
		/* Check whether fore or back match names of colors. */
		for (ii = COLOR_BLACK; ii < sizeof(colors) / sizeof(*colors); ii++) {
			if (strcasecmp(fore, colors[ii]) == 0)
				fg = ii;
			if (strcasecmp(back, colors[ii]) == 0)
				bg = ii;
		}
		/* Check whether item string matches names of items. */
		for (ii = 0; ii < sizeof(items) / sizeof(*items); ii++)
			if (strcasecmp(item, items[ii]) == 0)
				init_pair(ii + 1, fg, bg);
	}
	refresh();
	fclose(schemefile);
	return 0;
}

void            Inter(void)
{
	int             incr, dir, dev, key, y = 0;
#if HAVE_GETMOUSE
	MEVENT          event;
#endif				/* HAVE_GETMOUSE */
	levelbalmode = 0;
	/* Find first existing channel. */
	for (dev = 0; !((devmask | recmask) & (1 << dev)); dev++);
	y = CountChannels(dev);
	current_dev = dev;
	/* Highlight the label. */
	HighlightLabelCurses();
	if (devmask & (1 << dev)) {
		EraseLevel(current_dev);
		DrawLevel(current_dev);
		DrawLevelBalMode(current_dev, 0);
	}
	PlaceCursor();
	signal(SIGALRM, AumixSignalHandler);
	alarm(REFRESH_PERIOD);
	for (;;) {
		key = Getch();
		incr = 0;
		dir = 0;
		switch (key) {
		case '.':
		case ',':
		case '<':
		case '>':
		case '\t':
		case '\n':
			levelbalmode = !levelbalmode;
			attrset(COLOR_PAIR(AXIS_COLOR));
			mvaddstr(YOFFSET + CountChannels(current_dev), XOFFSET + menu_width + R_P_WIDTH + level_width, "          ");
			HighlightLabelCurses();
			DrawLevelBalMode(current_dev, levelbalmode);
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if ((levelbalmode) && ((1 << current_dev) & stereodevs))
				AdjustBalance(current_dev, 0, (MAXLEVEL / 10) * (key - '0'));
			else
				AdjustLevel(current_dev, 0, (MAXLEVEL / 10) * (key - '0'));
			break;
		case KEY_UP:
		case KEY_PPAGE:
		case KEY_DOWN:
		case KEY_NPAGE:
			if (key == KEY_UP || key == KEY_PPAGE)
				dir = -1;
			if (!dir)	/* if not PgUp or < pressed */
				dir = 1;
			/* un-highlight current label */
			EraseLevel(current_dev);
			DrawLevel(current_dev);
			attrset(COLOR_PAIR(AXIS_COLOR));
			y = CountChannels(current_dev);
/* fill the label area with spaces */
			mvaddnstr(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width, "                 ", label_width + 2);
			attrset(COLOR_PAIR(AXIS_COLOR));
			mvaddstr(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width + ARROW_WIDTH, LOCAL_TEXT(dev_label[current_dev]));
			/* switch to next existing device */
			do {
				if (dir == 1) {
					current_dev++;
					/* Wrap around. */
					if (current_dev > SOUND_MIXER_NRDEVICES - 1)
						current_dev = 0;
				} else {
					current_dev--;
					if (current_dev < 0)
						current_dev = SOUND_MIXER_NRDEVICES - 1;
				}
			}
			while (!((1 << current_dev) & (devmask | recmask)));
			HighlightLabelCurses();
			EraseLevel(current_dev);
			DrawLevel(current_dev);
			DrawLevelBalMode(current_dev, levelbalmode);
			PlaceCursor();
			break;
		case ' ':
			SwitchRecordPlay(current_dev);
			break;
		case '[':
			if ((levelbalmode) && ((1 << current_dev) & stereodevs))
				AdjustBalance(current_dev, 0, 0);
			else
				AdjustLevel(current_dev, 0, 0);
			break;
		case '0':
		case ']':
			if ((levelbalmode) && ((1 << current_dev) & stereodevs))
				AdjustBalance(current_dev, 0, MAXLEVEL);
			else
				AdjustLevel(current_dev, 0, MAXLEVEL);
			break;
		case '+':
		case '-':
		case KEY_LEFT:
		case KEY_RIGHT:
			if ((levelbalmode) && ((1 << current_dev) & stereodevs))
				if (key == '+' || key == KEY_RIGHT)
					AdjustBalance(current_dev, balance_increment, -1);
				else
					AdjustBalance(current_dev, -balance_increment, -1);
			else if (key == '+' || key == KEY_RIGHT)
				AdjustLevel(current_dev, level_increment, -1);
			else
				AdjustLevel(current_dev, -level_increment, -1);
			break;
		case '|':
			AdjustBalance(current_dev, -1, (MAXLEVEL / 2));
			break;
#if HAVE_GETMOUSE
		case KEY_MOUSE:
			if (getmouse(&event) == OK)
				DoMouse(event.x, event.y,
					NCURSES_TO_DOMOUSE(event.bstate));
			break;
#endif				/* HAVE_GETMOUSE */
			/* Be friendly to pre-ANSI terminals and check after KEY_. */
		case CTRL('D'):
			return;
			break;
		case CTRL('L'):
		case CTRL('R'):
			InitScreen();
			DrawLevelBalMode(current_dev, levelbalmode);
			HighlightLabelCurses();
			break;
		}
		key = tolower(key);
		if (key == *key_keys) {
			KeysBox();
		} else if (key == *key_load)
			LoadSettings();
		else if (key == *key_mute)
			ToggleMuting();
		else if (key == *key_only)
			Muting(current_dev, MUTE_ONLY);
		else if (key == *key_quit)
			return;
		else if (key == *key_save)
			SaveSettings();
		else if (key == *key_undo)
			Muting(MUTE_NO_DEVICE, MUTE_OFF);
		refresh();
	}
}

void            DrawLevelBalModeCurses(int dev, int mode)
/* arrow to show whether keyboard commands will adjust level or balance */
{
	int             y;
	y = CountChannels(dev);
	attrset(COLOR_PAIR(ACTIVE_COLOR) | ((has_colors()) ? A_BOLD : A_REVERSE));
	if ((1 << dev) & devmask) {
		if ((mode) && ((1 << dev) & stereodevs)) {
			mvaddch(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width + ARROW_WIDTH + label_width, '>');
/* cursor placement for Braille consoles */
			cursor_y = YOFFSET + y;
			cursor_x = XOFFSET + menu_width + R_P_WIDTH + level_width + ARROW_WIDTH + label_width;
		} else {
			mvaddch(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width, '<');
			attrset(COLOR_PAIR(AXIS_COLOR));
  			mvaddch(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width + ARROW_WIDTH + label_width, ' '); /* erases ">" */
			cursor_y = YOFFSET + y; /* for Braille */
			cursor_x = XOFFSET + menu_width; /* for Braille */
		}
	}
	PlaceCursor();
}

void            DrawLevelCurses(int dev)
{
	int             left, right, temp, y = 0;
	if (!((1 << dev) & devmask) || (dev > SOUND_MIXER_NRDEVICES - 1) || (dev < 0))
		return;
	y = CountChannels(dev);
	ErrorExitWarn(ReadLevel(dev, &temp), 'e');
	left = temp & 0x7F;
	right = (temp >> 8) & 0x7F;
	/*
	 * According to curs_refresh(3x),
	 * 
	 * The refresh and wrefresh routines (or wnoutrefresh and doupdate) must be called to get actual output to the ter- minal, as other routines merely manipulate data struc- tures.  The routine wrefresh copies the named window to the physical terminal screen, taking into account what is already there in order to do optimizations.
	 * 
	 * However, when the balance is centred, as it often is, we probably save a few CPU cycles by checking for this rather than letting ncurses take care of it.
	 */
	if (left != right) {
		attrset(COLOR_PAIR(AXIS_COLOR));
		mvaddch(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + left / level_increment, 'L');
		mvaddch(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + right / level_increment, 'R');
	}
	attrset(COLOR_PAIR(HANDLE_COLOR) | ((has_colors()) ? A_BOLD : A_REVERSE));
	mvaddch(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + (left + right) / level_increment / 2, 'O');
	PlaceCursor();
}

void            EraseLevelCurses(int dev)
{
	/* Redraw level track. */
	int             ii, y = 0;
	if (!((1 << dev) & devmask) || (dev > SOUND_MIXER_NRDEVICES - 1) || (dev < 0))
		return;
	y = CountChannels(dev);
	attrset(COLOR_PAIR(TRACK_COLOR));
	for (ii = 0; ii < level_width; ii++)
		mvaddch(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + ii, '+');
	PlaceCursor();
}

void            RedrawBalanceCurses(int dev)
/* Redraw balance track. */
{
	int             left, right, max, temp, balset, y;
	y = CountChannels(dev);
	if ((1 << dev) & stereodevs) {
		attrset(COLOR_PAIR(TRACK_COLOR));
		for (temp = 0; temp < balance_width; temp++)
			mvaddch(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width + label_width + ARROW_WIDTH * 2 + temp, '+');
		ErrorExitWarn(ReadLevel(dev, &temp), 'e');
		left = temp & 0x7F;
		right = (temp >> 8) & 0x7F;
		max = (left > right) ? left : right;
		if (temp) {
			balset = (left > right) ? (MAXLEVEL / 2) * right / max : MAXLEVEL - ((MAXLEVEL / 2) * left / max);
		} else {
			balset = (MAXLEVEL / 2);
		}
		attrset(COLOR_PAIR(HANDLE_COLOR) | ((has_colors()) ? A_BOLD : A_REVERSE));
		mvaddch(YOFFSET + y, XOFFSET + menu_width + R_P_WIDTH + level_width + ARROW_WIDTH * 2 + label_width + balset / balance_increment, 'O');
		PlaceCursor();
	}
}

void            DrawRecordPlayCurses(int dev)
{
	attrset((1 << dev) & recsrc ? COLOR_PAIR(RECORD_COLOR) | ((has_colors()) ? A_NORMAL : A_REVERSE) : COLOR_PAIR(PLAY_COLOR));
	mvaddch(YOFFSET + CountChannels(dev), XOFFSET + menu_width, ((1 << dev) & recsrc ? 'R' : 'P'));
	PlaceCursor();
}

void            CloseScreenCurses(void)
{
	if (interactive) {
		clear();
		refresh();
		endwin();
		printf("\n"); /* resets color of cursor */
	}
}
#endif				/* HAVE_CURSES */

/* $Aumix: aumix/src/interactive.c,v 1.10 2010/04/27 20:11:55 trevor Exp $
 * interactive.c:  functions used in all interactive modes
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

#include "common.h"
#if defined (HAVE_CURSES) || defined (HAVE_GTK)
#include "interactive.h"
#ifdef HAVE_CURSES
#include "curses.h"
#endif				/* HAVE_CURSES */
#if defined (HAVE_GTK)
#include "gtk.h"
#endif				/* HAVE_GTK */

void            AumixSignalHandler(int signal_number)
{
/* Handle SIGALRM. */
	signal(SIGALRM, AumixSignalHandler);	/* Reset the signal handler. */
#ifdef HAVE_CURSES	
	if (interactive == IN_CURSES)
		WakeUpCurses();
#endif
#if defined (HAVE_GTK)
	if (interactive == IN_GTK)
		WakeUpGTK();
#endif	
	alarm(REFRESH_PERIOD);
}

void            InitScreen(void)
{
#ifdef HAVE_CURSES
	if (interactive == IN_CURSES)
		InitScreenCurses();
#endif				/* HAVE_CURSES */
#if defined (HAVE_GTK)
	if (interactive == IN_GTK)
		InitScreenGTK();
#endif				/* HAVE_GTK */
}

void            KeysBox(void)
{
#ifdef HAVE_CURSES
	if (interactive == IN_CURSES)
		KeysBoxCurses();
#endif				/* HAVE_CURSES */
#if defined (HAVE_GTK)
/*      if (interactive == IN_GTK) KeysBoxGTK(); */
#endif				/* HAVE_GTK */
}

void            RefreshAllSettings(void)
{
	int             dev;
	for (dev = 0; dev < SOUND_MIXER_NRDEVICES; dev++) {
		if ((1 << dev) & devmask) {
			EraseLevel(dev);
			RedrawBalance(dev);
			DrawLevel(dev);
		}
		/* print record/play indicators */
		if ((1 << dev) & recmask)
			DrawRecordPlay(dev);
	}
}

void            RefreshNewSettings(void)
/* Periodically redraws the screen, in case another process has changed the
   mixer settings. */
{
	int             dev, ii = 0;
	for (dev = 0; dev < SOUND_MIXER_NRDEVICES; dev++) {
		/* record/play indicators */
		if ((1 << dev) & recmask) {
			ErrorExitWarn(ReadRecSrc(), 'e');
			/* Redraw if the setting changed. */
			if (our_recplay[dev] - recsrc)
				DrawRecordPlay(dev);
			our_recplay[dev] = recsrc;
		}
		if ((1 << dev) & devmask) {
			ErrorExitWarn(ReadLevel(dev, &ii), 'e');
			if (ii - ourlevel[dev]) {	/* Has the setting changed? */
				EraseLevel(dev);
				RedrawBalance(dev);
				DrawLevel(dev);
			}
		}
		ourlevel[dev] = ii;
	}
}

void            Muting(int device, int newstate)
/* Change muted, normal, and solo states. */
{
/* device:  current device
   * mutestate:  0 for no mute, -1 for global mute, or set to soloing device
   * newstate:  new value for mutestate */
	switch (newstate) {
		case MUTE_OFF:
		switch (mutestate) {
			case MUTE_OFF:
			return;
			break;
		case MUTE_GLOBAL:
			UnmuteAll();
			break;
		case MUTE_ONLY:
			UnmuteAll();
			break;
		}
		break;
	case MUTE_GLOBAL:
		switch (mutestate) {
		case MUTE_OFF:
			StoreAll();
			MuteAll();
			break;
		case MUTE_GLOBAL:
			return;
			break;
		case MUTE_ONLY:
			MuteAll();
			break;
		}
		break;
	case MUTE_ONLY:
		switch (mutestate) {
		case MUTE_OFF:
			StoreAll();
			MuteAllButOne(device);
			break;
		case MUTE_GLOBAL:
			UnmuteOne(device);
			break;
		case MUTE_ONLY:
			MuteAllButOne(device);
			UnmuteOne(device);
		}
	}
	RefreshAllSettings();
	mutestate = newstate;
#ifdef HAVE_CURSES
	if (interactive == IN_CURSES)
		ShowMutingCurses();
#endif				/* HAVE_CURSES */
	mutestate = newstate;
}

void            ToggleMuting(void)
/* Switch between no muting and global muting; do nothing if soloing (MUTE_ONLY). */
{
	switch (mutestate) {
		case MUTE_OFF:
		Muting(MUTE_NO_DEVICE, MUTE_GLOBAL);
		break;
	case MUTE_GLOBAL:
		Muting(MUTE_NO_DEVICE, MUTE_OFF);
		break;
	}
}

void            MuteAll(void)
/* Set all channels to zero. */
{
	int             dev;
	for (dev = 0; dev < SOUND_MIXER_NRDEVICES; dev++)
		MuteOne(dev);
}

void            UnmuteAll(void)
{
/* Restore all from mutelevel array. */
	int             dev;
	for (dev = 0; dev < SOUND_MIXER_NRDEVICES; dev++)
		UnmuteOne(dev);
}

void            StoreAll(void)
/* Read all channels into mutelevel array. */
{
	int             dev;
	for (dev = 0; dev < SOUND_MIXER_NRDEVICES; dev++)
		StoreOne(dev);
}

void            StoreOne(int device)
/* Read one channel into mutelevel array. */
{
	if ((1 << device) & devmask)
		ErrorExitWarn(ReadLevel(device, &mutelevel[device]), 'e');
}

void            MuteAllButOne(int device)
/* Set all channels except one to zero. */
{
	int             ii;
	for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++)
		if (device != ii)
			MuteOne(ii);
}

void            MuteOne(int device)
/* Set one channel to zero. */
{
	const int       null = 0;
	if ((1 << device) & devmask)
		ErrorExitWarn(WriteLevel(device, null), 'e');
}

void            UnmuteAllButOne(int device)
{
/* Restore all but current from mutelevel array. */
	int             ii;
	for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++)
		if (device != ii)
			UnmuteOne(ii);
}

void            UnmuteOne(int dev)
{
/* Restore one channel from mutelevel array. */
	if ((1 << dev) & devmask)
		ErrorExitWarn(WriteLevel(dev, mutelevel[dev]), 'e');
}

void            DrawLevelBalMode(int dev, int mode)
/* arrow to show whether keyboard commands will adjust level or balance */
{
#ifdef HAVE_CURSES
	if (interactive == IN_CURSES)
		DrawLevelBalModeCurses(dev, mode);
#endif				/* HAVE_CURSES */
}

void            AdjustLevel(int dev, int incr, int setlevel)
/*
 *  dev: device to adjust
 *  incr: signed percentage to increase (decrease) level, ignored unless
 *      setlevel = -1
 *  setlevel: level to set directly, or -1 to increment
 */
{
	int             balset, max, left, right, temp;
	if (!((1 << dev) & devmask) || (dev > SOUND_MIXER_NRDEVICES - 1) || (dev < 0))
		return;
	ErrorExitWarn(ReadLevel(dev, &temp), 'e');
	left = temp & 0x7F;
	right = (temp >> 8) & 0x7F;
	max = (left > right) ? left : right;
	if (max) {
		balset = (left > right) ? (MAXLEVEL / 2) * right / max : MAXLEVEL - ((MAXLEVEL / 2) * left / max);
	} else {
		balset = (MAXLEVEL / 2);
	}
	max += incr;
	if (balset > (MAXLEVEL / 2 - 1)) {
		left = max * (MAXLEVEL - balset) / (MAXLEVEL / 2);
		right = max;
	} else {
		right = max * balset / (MAXLEVEL / 2);
		left = max;
	}
	left = (setlevel > -1) ? setlevel : left;
	right = (setlevel > -1) ? setlevel : right;
	left = (left > MAXLEVEL) ? MAXLEVEL : left;
	right = (right > MAXLEVEL) ? MAXLEVEL : right;
	left = (left < 0) ? 0 : left;
	right = (right < 0) ? 0 : right;
	temp = (right << 8) | left;
	ErrorExitWarn(WriteLevel(dev, temp), 'e');
	EraseLevel(dev);
	/* Draw handle at new position. */
	RedrawBalance(dev);
	DrawLevel(dev);
}

void            DrawLevel(int dev)
{
#ifdef HAVE_CURSES
	if (interactive == IN_CURSES)
		DrawLevelCurses(dev);
#endif				/* HAVE_CURSES */
}

void            EraseLevel(int dev)
/* Redraw level track. */
{
#ifdef HAVE_CURSES
	if (interactive == IN_CURSES)
		EraseLevelCurses(dev);
#endif				/* HAVE_CURSES */
}

void            AdjustBalance(int dev, int incr, int setabs)
/*
 *  dev: device to adjust
 *  incr: signed percentage to change balance
 *  setabs: absolute balance setting, or -1 to increment only
 *  Balance settings go from 0 to MAXLEVEL; at setting of 0, right amplitude is 0.
 */
{
	int             balset, max, left, right, temp, left_orig, right_orig;
	if (!((1 << dev) & devmask) || (dev > SOUND_MIXER_NRDEVICES - 1))
		return;
	ErrorExitWarn(ReadLevel(dev, &temp), 'e');
	left = temp & 0x7F;
	right = (temp >> 8) & 0x7F;
	left_orig = left;
	right_orig = right;
	max = (left > right) ? left : right;
	if (max) {
		balset = (left > right) ? (MAXLEVEL / 2) * right / max : MAXLEVEL - ((MAXLEVEL / 2) * left / max);
	} else {
		balset = (MAXLEVEL / 2);
	}
	balset = (setabs == -1) ? balset + incr : setabs;
	balset = (balset > MAXLEVEL) ? MAXLEVEL : balset;
	balset = (balset < 0) ? 0 : balset;
	if (balset > (MAXLEVEL / 2 - 1)) {
		left = max * (MAXLEVEL - balset) / (MAXLEVEL / 2);
		right = max;
	} else {
		right = max * balset / (MAXLEVEL / 2);
		left = max;
	}
	temp = (right << 8) | left;
	ErrorExitWarn(WriteLevel(dev, temp), 'e');
	/* Draw handle at new position. */
	RedrawBalance(dev);
}

void            RedrawBalance(int dev)
/* Redraw balance track. */
{
#ifdef HAVE_CURSES
	if (interactive == IN_CURSES)
		RedrawBalanceCurses(dev);
#endif				/* HAVE_CURSES */
}

void            SwitchRecordPlay(int dev)
{
	/* Toggle record/play. */
	if ((1 << dev) & recmask) {
		ErrorExitWarn(ReadRecSrc(), 'e');
		if (recsrc & (1 << dev))
			recsrc &= ~(1 << dev);	/* Turn off recording. */
		else
			recsrc |= (1 << dev);	/* Turn on recording. */
		ErrorExitWarn(WriteRecSrc(), 'e');
		ErrorExitWarn(ReadRecSrc(), 'e');
		our_recplay[dev] = recsrc;	/* Store setting for updates. */
		DrawRecordPlay(dev);	/* Print indicators. */
	}
}

void            DrawRecordPlay(int dev)
{
#ifdef HAVE_CURSES
	if (interactive == IN_CURSES)
		DrawRecordPlayCurses(dev);
#endif				/* HAVE_CURSES */
}

void            CloseScreen(void)
{
#ifdef HAVE_CURSES
	if (interactive == IN_CURSES)
		CloseScreenCurses();
#endif				/* HAVE_CURSES */
#if defined (HAVE_GTK)
	if (interactive == IN_GTK)
		CloseScreenGTK();
#endif				/* HAVE_GTK */
}

void            ReadInteractiveKeys(void)
{
	key_keys = strdup(LOCAL_TEXT("k"));
	key_load = strdup(LOCAL_TEXT("l"));
	key_mute = strdup(LOCAL_TEXT("m"));
	key_only = strdup(LOCAL_TEXT("o"));
	key_quit = strdup(LOCAL_TEXT("q"));
	key_save = strdup(LOCAL_TEXT("s"));
	key_undo = strdup(LOCAL_TEXT("u"));
}
#endif				/* HAVE_CURSES || HAVE_GTK */

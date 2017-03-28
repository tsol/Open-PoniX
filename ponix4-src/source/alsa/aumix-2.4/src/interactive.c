/* interactive.c:  functions used in all interactive modes */
#include "common.h"
#if defined (HAVE_CURSES) || defined (HAVE_GTK)
#include "interactive.h"
#include "curses.h"
#ifdef HAVE_GTK
#include "gtk.h"
#endif				/* HAVE_GTK */

char           *chark, *charl, *charm, *charo, *charq, *chars, *charu;

void            InitScreen()
{
#ifdef HAVE_CURSES
	if (interactive == IN_CURSES)
		InitScreenCurses();
#endif				/* HAVE_CURSES */
#ifdef HAVE_GTK
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
#ifdef HAVE_GTK
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
			/* print record/play indicators */
			if ((1 << dev) & recmask)
				DrawRecordPlay(dev);
		}
	}
}

void            RefreshNewSettings(void)
/* Periodically redraws the screen, in case another process has changed the
   mixer settings. */
{
	int             dev, i = 0;
	for (dev = 0; dev < SOUND_MIXER_NRDEVICES; dev++) {
		if ((1 << dev) & devmask) {
/* record/play indicators */
			if ((1 << dev) & recmask) {
				ExitIfError(ReadRecSrc());
/* Redraw if the setting changed. */
				if (our_recplay[dev] - recsrc)
					DrawRecordPlay(dev);
				our_recplay[dev] = recsrc;
			}
			ExitIfError(ReadLevel(dev, &i));
			if (i - ourlevel[dev]) {	/* Has the setting changed? */
				EraseLevel(dev);
				RedrawBalance(dev);
				DrawLevel(dev);
				if ((1 << dev) & recmask)
					DrawRecordPlay(dev);
				ourlevel[dev] = i;
			}
		}
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
/* Switch between no muting and global muting; do nothing if soloing. */
{
	switch (mutestate) {
		case MUTE_OFF:
		Muting(MUTE_NO_DEVICE, MUTE_GLOBAL);
		break;
	case MUTE_GLOBAL:
		Muting(MUTE_NO_DEVICE, MUTE_OFF);
		break;
	case MUTE_ONLY:
		/* Do nothing. */
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
		ExitIfError(ReadLevel(device, &mutelevel[device]));
}

void            MuteAllButOne(int device)
/* Set all channels except one to zero. */
{
	int             i;
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++)
		if (device != i)
			MuteOne(i);
}

void            MuteOne(int device)
/* Set one channel to zero. */
{
	const int       null = 0;
	if ((1 << device) & devmask)
		ExitIfError(WriteLevel(device, null));
}

void            UnmuteAllButOne(int device)
{
/* Restore all but current from mutelevel array. */
	int             i;
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++)
		if (device != i)
			UnmuteOne(i);
}

void            UnmuteOne(int dev)
{
/* Restore one channel from mutelevel array. */
	if ((1 << dev) & devmask)
		ExitIfError(WriteLevel(dev, mutelevel[dev]));
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
	ExitIfError(ReadLevel(dev, &temp));
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
	ExitIfError(WriteLevel(dev, temp));
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
	ExitIfError(ReadLevel(dev, &temp));
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
	ExitIfError(WriteLevel(dev, temp));
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
		ExitIfError(ReadRecSrc());
		if (recsrc & (1 << dev))
			recsrc &= ~(1 << dev);	/* Turn off recording. */
		else
			recsrc |= (1 << dev);	/* Turn on recording. */
		ExitIfError(WriteRecSrc());
		ExitIfError(ReadRecSrc());
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
#ifdef HAVE_GTK
	if (interactive == IN_GTK)
		CloseScreenGTK();
#endif				/* HAVE_GTK */
}

void            ReadInteractiveKeys(void)
{
	chark = malloc(strlen(LOCAL_TEXT("k")));
	charl = malloc(strlen(LOCAL_TEXT("l")));
	charm = malloc(strlen(LOCAL_TEXT("m")));
	charo = malloc(strlen(LOCAL_TEXT("o")));
	charq = malloc(strlen(LOCAL_TEXT("q")));
	chars = malloc(strlen(LOCAL_TEXT("s")));
	charu = malloc(strlen(LOCAL_TEXT("u")));
	sprintf(chark, LOCAL_TEXT("k"));
	sprintf(charl, LOCAL_TEXT("l"));
	sprintf(charm, LOCAL_TEXT("m"));
	sprintf(charo, LOCAL_TEXT("o"));
	sprintf(charq, LOCAL_TEXT("q"));
	sprintf(chars, LOCAL_TEXT("s"));
	sprintf(charu, LOCAL_TEXT("u"));
}
#endif				/* defined (HAVE_CURSES) || defined (HAVE_GTK) */

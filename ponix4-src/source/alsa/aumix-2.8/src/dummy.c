/* $Aumix: aumix/src/dummy.c,v 1.3 2002/04/29 19:41:26 trevor Exp $
 * dummy.c: implement a dummy mixer for debugging on machines without
 *          sound card
 * copyright (c) 1999 Christian Weisgerber
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
#ifdef DUMMY_MIXER
#define	DUMMY_DEVMASK	 0x01FBFFFF
#define	DUMMY_RECMASK	 0x00040040
#define	DUMMY_RECSRC	 0x000001C8
#define	DUMMY_STEREODEVS 0x0000315F

int             dummy_ioctl(int fd, unsigned long req, int *arg)
{
	static int      recsrc = DUMMY_RECSRC;
	static int      dev[SOUND_MIXER_NRDEVICES];
	switch (req) {
	case SOUND_MIXER_READ_DEVMASK:
		*arg = DUMMY_DEVMASK;
		break;
	case SOUND_MIXER_READ_RECMASK:
		*arg = DUMMY_RECMASK;
		break;
	case SOUND_MIXER_READ_RECSRC:
		*arg = recsrc;
		break;
	case SOUND_MIXER_READ_STEREODEVS:
		*arg = DUMMY_STEREODEVS;
		break;
	case SOUND_MIXER_WRITE_RECSRC:
		recsrc = *arg;
		break;
	default:
		if (req >= MIXER_READ(0) && req < MIXER_READ(SOUND_MIXER_NRDEVICES)) {
			*arg = dev[req - MIXER_READ(0)];
			break;
		} else if (req >= MIXER_WRITE(0) && req < MIXER_WRITE(SOUND_MIXER_NRDEVICES)) {
			dev[req - MIXER_WRITE(0)] = *arg;
			break;
		}
		return -1;
	}
	return 0;
}
#endif				/* DUMMY_MIXER */

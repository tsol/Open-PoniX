/* dummy.c: implement a dummy mixer for debugging on machines without
 *          sound card
 */
#include "common.h"
#ifdef DUMMY_MIXER
#define	DUMMY_DEVMASK	 0x000033FF
#define	DUMMY_RECMASK	 0x00000040
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

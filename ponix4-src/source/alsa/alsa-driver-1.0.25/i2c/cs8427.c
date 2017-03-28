#include "adriver.h"

/* hack: define bitrev8() even without CONFIG_BITREVERSE */
#ifndef CONFIG_BITREVERSE
#include <linux/bitrev.h>
static unsigned char bitrev8(unsigned char val)
{
	int bit;
	unsigned char res = 0;
	for (bit = 0; bit < 8; bit++) {
		res <<= 1;
		res |= val & 1;
		val >>= 1;
	}
	return res;
}
#endif

#include "../alsa-kernel/i2c/cs8427.c"

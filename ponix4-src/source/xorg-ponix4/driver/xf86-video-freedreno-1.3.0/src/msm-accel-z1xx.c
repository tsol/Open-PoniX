/*
 * Copyright © 2012, 2014 Rob Clark <robclark@freedesktop.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "msm.h"
#include "msm-accel.h"
#include "msm-accel-z1xx.h"

/* matching buffer info:
                len:            00001000
                gpuaddr:        66142000

                len:            00009000
                gpuaddr:        66276000

                len:            00081000
                gpuaddr:        66280000
 */
static const uint32_t initial_state[] = {
		 0x7c000275, 0x00000000, 0x00050005, 0x7c000129,
		 0x00000000, 0x7c00012a, 0x00000000, 0x7c00012b,
		 0x00000000, 0x7c00010f, 0x00000000, 0x7c000108,
		 0x00000000, 0x7c000109, 0x00000000, 0x7c000100,
		 0x00000000, 0x7c000101, 0x00000000, 0x7c000110,
		 0x00000000, 0x7c0001d0, 0x00000000, 0x7c0001d4,
		 0x00000000, 0x7c00010c, 0x00000000, 0x7c00010e,
		 0x00000000, 0x7c00010d, 0x00000000, 0x7c00010b,
		 0x00000000, 0x7c00010a, 0x00000000, 0x7c000111,
		 0x00000000, 0x7c000114, 0x00000000, 0x7c000115,
		 0x00000000, 0x7c000116, 0x00000000, 0x7c000117,
		 0x00000000, 0x7c000118, 0x00000000, 0x7c000119,
		 0x00000000, 0x7c00011a, 0x00000000, 0x7c00011b,
		 0x00000000, 0x7c00011c, 0x00000000, 0x7c00011d,
		 0x00000000, 0x7c00011e, 0x00000000, 0x7c00011f,
		 0x00000000, 0x7c000124, 0x00000000, 0x7c000125,
		 0x00000000, 0x7c000127, 0x00000000, 0x7c000128,
		 0x00000000, 0x7b00015e, 0x00000000, 0x7b000161,
		 0x00000000, 0x7b000165, 0x00000000, 0x7b000166,
		 0x00000000, 0x7b00016e, 0x00000000, 0x7c00016f,
		 0x00000000, 0x7b000165, 0x00000000, 0x7b000154,
		 0x00000000, 0x7b000155, 0x00000000, 0x7b000153,
		 0x00000000, 0x7b000168, 0x00000000, 0x7b000160,
		 0x00000000, 0x7b000150, 0x00000000, 0x7b000156,
		 0x00000000, 0x7b000157, 0x00000000, 0x7b000158,
		 0x00000000, 0x7b000159, 0x00000000, 0x7b000152,
		 0x00000000, 0x7b000151, 0x00000000, 0x7b000156,
		 0x00000000, 0x7c00017f, 0x00000000, 0x7c00017f,
		 0x00000000, 0x7c00017f, 0x00000000, 0x7c00017f,
		 0x00000000, 0x7f000000, 0x7f000000, 0x7c000129,
/**/	 0x66142000, 0x7c00012a, 0x66276000, 0x7c00012b,
/**/	 0x66280000, 0x7c0001e2, 0x00000000, 0x7c0001e3,
		 0x00000000, 0x7c0001e4, 0x00000000, 0x7c0001e5,
		 0x00000000, 0x7c0001e6, 0x00000000, 0x7c0001e7,
		 0x00000000, 0x7c0001c0, 0x00000000, 0x7c0001c1,
		 0x00000000, 0x7c0001c2, 0x00000000, 0x7c0001c3,
		 0x00000000, 0x7c0001c4, 0x00000000, 0x7c0001c5,
		 0x00000000, 0x7c0001c6, 0x00000000, 0x7c0001c7,
		 0x00000000, 0x7c0001c8, 0x00000000, 0x7c0001c9,
		 0x00000000, 0x7c0001ca, 0x00000000, 0x7c0001d1,
		 0x00000000, 0x7c0001d2, 0x00000000, 0x7c0001d4,
		 0x00000000, 0x7c0001d3, 0x00000000, 0x7c0001d5,
		 0x00000000, 0x7c0001d0, 0x00000000, 0x7c0001e0,
		 0x00000000, 0x7c0001e1, 0x00000000, 0x7c0001e2,
		 0x00000000, 0x7c0001e3, 0x00000000, 0x7c0001e4,
		 0x00000000, 0x7c0001e5, 0x00000000, 0x7c0001e6,
		 0x00000000, 0x7c0001e7, 0x00000000, 0x7c0001c0,
		 0x00000000, 0x7c0001c1, 0x00000000, 0x7c0001c2,
		 0x00000000, 0x7c0001c3, 0x00000000, 0x7c0001c4,
		 0x00000000, 0x7c0001c5, 0x00000000, 0x7c0001c6,
		 0x00000000, 0x7c0001c7, 0x00000000, 0x7c0001c8,
		 0x00000000, 0x7c0001c9, 0x00000000, 0x7c0001ca,
		 0x00000000, 0x7c0001d1, 0x00000000, 0x7c0001d2,
		 0x00000000, 0x7c0001d4, 0x00000000, 0x7c0001d3,
		 0x00000000, 0x7c0001d5, 0x00000000, 0x7c0001d0,
		 0x00000000, 0x7c0001e0, 0x00000000, 0x7c0001e1,
		 0x00000000, 0x7c0001e2, 0x00000000, 0x7c0001e3,
		 0x00000000, 0x7c0001e4, 0x00000000, 0x7c0001e5,
		 0x00000000, 0x7c0001e6, 0x00000000, 0x7c0001e7,
		 0x00000000, 0x7c0001c0, 0x00000000, 0x7c0001c1,
		 0x00000000, 0x7c0001c2, 0x00000000, 0x7c0001c3,
		 0x00000000, 0x7c0001c4, 0x00000000, 0x7c0001c5,
		 0x00000000, 0x7c0001c6, 0x00000000, 0x7c0001c7,
		 0x00000000, 0x7c0001c8, 0x00000000, 0x7c0001c9,
		 0x00000000, 0x7c0001ca, 0x00000000, 0x7c0001d1,
		 0x00000000, 0x7c0001d2, 0x00000000, 0x7c0001d4,
		 0x00000000, 0x7c0001d3, 0x00000000, 0x7c0001d5,
		 0x00000000, 0x7c0001d0, 0x00000000, 0x7c0001e0,
		 0x00000000, 0x7c0001e1, 0x00000000, 0x7c0001e2,
		 0x00000000, 0x7c0001e3, 0x00000000, 0x7c0001e4,
		 0x00000000, 0x7c0001e5, 0x00000000, 0x7c0001e6,
		 0x00000000, 0x7c0001e7, 0x00000000, 0x7c0001c0,
		 0x00000000, 0x7c0001c1, 0x00000000, 0x7c0001c2,
		 0x00000000, 0x7c0001c3, 0x00000000, 0x7c0001c4,
		 0x00000000, 0x7c0001c5, 0x00000000, 0x7c0001c6,
		 0x00000000, 0x7c0001c7, 0x00000000, 0x7c0001c8,
		 0x00000000, 0x7c0001c9, 0x00000000, 0x7c0001ca,
		 0x00000000, 0x7c0001d1, 0x00000000, 0x7c0001d2,
		 0x00000000, 0x7c0001d4, 0x00000000, 0x7c0001d3,
		 0x00000000, 0x7c0001d5, 0x00000000, 0x7f000000,
};

void
ring_pre(struct fd_ringbuffer *ring)
{
	/* each packet seems to carry the address/size of next (w/ 0x00000000
	 * meaning no branch, next packet follows).  Each cmd packet is preceded
	 * by a dummy packet to give the size of the next..
	 */
	OUT_RING (ring, REGM(VGV3_NEXTADDR, 2));
	OUT_RING (ring, 0x00000000);	/* VGV3_NEXTADDR */
	OUT_RING (ring, 0x00000000);	/* VGV3_NEXTCMD, fixed up on flush */
	OUT_RING (ring, 0x7c000134);
	OUT_RING (ring, 0x00000000);

	OUT_RING (ring, REGM(VGV3_NEXTADDR, 2));
	OUT_RING (ring, 0x00000000);	/* fixed up by kernel */
	OUT_RING (ring, 0x00000000);	/* fixed up by kernel */
}

void
ring_post(struct fd_ringbuffer *ring)
{
	/* This appears to be common end of packet: */
	OUT_RING(ring, REG(G2D_IDLE) | G2D_IDLE_IRQ | G2D_IDLE_BCFLUSH);
	OUT_RING(ring, REG(VGV3_LAST) | 0x0);
	OUT_RING(ring, REG(VGV3_LAST) | 0x0);
}

void
next_ring(MSMPtr pMsm)
{
	struct fd_ringbuffer *ring;
	int idx = pMsm->ring.idx++ % ARRAY_SIZE(pMsm->ring.rings);

	if (pMsm->ring.rings[idx]) {
		ring = pMsm->ring.ring = pMsm->ring.rings[idx];
		fd_ringbuffer_reset(ring);
		return;
	}

	ring = pMsm->ring.ring = pMsm->ring.rings[idx] =
			fd_ringbuffer_new(pMsm->pipe,
					0x4000 + STATE_SIZE * sizeof(uint32_t));

	/* for now, until state packet is understood, just use a pre-canned
	 * state captured from libC2D2 test, and fix up the gpu addresses
	 */
	memcpy(ring->start, initial_state, STATE_SIZE * sizeof(uint32_t));
	ring->cur = &ring->start[120];
	OUT_RELOC(ring, pMsm->ring.context_bos[0], TRUE);
	ring->cur = &ring->start[122];
	OUT_RELOC(ring, pMsm->ring.context_bos[1], TRUE);
	ring->cur = &ring->start[124];
	OUT_RELOC(ring, pMsm->ring.context_bos[2], TRUE);

	fd_ringbuffer_reset(ring);
}

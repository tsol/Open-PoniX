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

#ifndef MSM_ACCEL_Z1XX_H_
#define MSM_ACCEL_Z1XX_H_

#include "msm.h"
#include "freedreno_ringbuffer.h"
#include "freedreno_z1xx.h"

#define LOG_DWORDS 0

#define STATE_SIZE  0x140

void ring_pre(struct fd_ringbuffer *ring);
void ring_post(struct fd_ringbuffer *ring);
void next_ring(MSMPtr pMsm);

static inline void
OUT_RING(struct fd_ringbuffer *ring, unsigned data)
{
	if (LOG_DWORDS) {
		ErrorF("ring[%p]: OUT_RING   %04x:  %08x\n", ring,
				(uint32_t)(ring->cur - ring->last_start), data);
	}
	fd_ringbuffer_emit(ring, data);
}

static inline void
OUT_RELOC(struct fd_ringbuffer *ring, struct fd_bo *bo, Bool write)
{
	if (LOG_DWORDS) {
		ErrorF("ring[%p]: OUT_RELOC  %04x:  %p\n", ring,
				(uint32_t)(ring->cur - ring->last_start), bo);
	}
	fd_ringbuffer_reloc(ring, &(struct fd_reloc){
		.bo = bo,
		.flags = FD_RELOC_READ | (write ? FD_RELOC_WRITE : 0),
	});
}

static inline void
FIRE_RING(MSMPtr pMsm)
{
	struct fd_ringbuffer *ring = pMsm->ring.ring;
	if (pMsm->ring.fire) {
		ring_post(ring);
		fd_ringbuffer_flush(ring);

		/* grab the timestamp off the current ringbuffer: */
		pMsm->ring.timestamp = fd_ringbuffer_timestamp(pMsm->ring.ring);

		/* cycle to next ringbuffer: */
		next_ring(pMsm);

		/* if blits haven't finished on the previous usage of the next
		 * ringbuffer, we need to wait to avoid overwriting cmds that
		 * the gpu is still processing..  ideally we don't hit this too
		 * much, otherwise we should create more ringbuffers..
		 *
		 * TODO maybe we want to time this to make sure we aren't actually
		 * blocking..
		 */
		fd_pipe_wait(pMsm->pipe, fd_ringbuffer_timestamp(pMsm->ring.ring));

		ring_pre(pMsm->ring.ring);

		pMsm->ring.fire = FALSE;
	}
}

static inline void
BEGIN_RING(MSMPtr pMsm, int size)
{
	struct fd_ringbuffer *ring = pMsm->ring.ring;

	if (LOG_DWORDS) {
		ErrorF("ring[%p]: BEGIN_RING %d\n", ring, size);
	}

	/* current kernel side just expects one cmd packet per ISSUEIBCMDS: */
	size += 11;       /* common header/footer */

	if ((ring->cur + size) > ring->end)
		FIRE_RING(pMsm);
}

static inline void
END_RING(MSMPtr pMsm)
{
	struct fd_ringbuffer *ring = pMsm->ring.ring;
	if (LOG_DWORDS) {
		ErrorF("ring[%p]: END_RING\n", ring);
	}
	pMsm->ring.fire = TRUE;
}

#endif /* MSM_ACCEL_Z1XX_H_ */

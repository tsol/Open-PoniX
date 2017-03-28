/*
 * Copyright © 2012 Rob Clark <robclark@freedesktop.org>
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

#ifdef HAVE_XA
#  include <xa_tracker.h>
#endif

Bool
MSMSetupAccel(ScreenPtr pScreen)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	Bool ret, softexa = FALSE;

	pMsm->pipe = fd_pipe_new(pMsm->dev, FD_PIPE_2D);
#ifdef HAVE_XA
	if (!pMsm->pipe && !pMsm->NoAccel) {
		struct fd_pipe *p;

		INFO_MSG("no 2D, trying 3D/XA");

		p = fd_pipe_new(pMsm->dev, FD_PIPE_3D);
		if (!p) {
			ERROR_MSG("no 3D pipe");
			goto no_xa;
		}

		pMsm->xa = xa_tracker_create(pMsm->drmFD);
		if (!pMsm->xa) {
			ERROR_MSG("could not setup XA");
			goto no_xa;
		}

		pMsm->pipe = p;

		INFO_MSG("using 3D/XA");

		goto out;
	}
no_xa:
#endif
	if (!pMsm->pipe) {
		INFO_MSG("no 2D pipe, falling back to software!");
		if (pMsm->NoKMS) {
			/* fbdev mode is lame.. we need a pipe, any pipe, to get a
			 * bo for the scanout/fbdev buffer.  So just do this instead
			 * of special casing the PrepareAccess stuff for scanout bo:
			 */
			pMsm->pipe = fd_pipe_new(pMsm->dev, FD_PIPE_3D);
		}
		softexa = TRUE;
		goto out;
	}

	if (pMsm->NoAccel) {
		INFO_MSG("Acceleration disabled in config file");
		softexa = TRUE;
	}

out:
#ifdef HAVE_XA
	if (pMsm->xa)
		ret = MSMSetupExaXA(pScreen);
	else
#endif
	ret = MSMSetupExa(pScreen, softexa);
	if (ret) {
		pMsm->dri = MSMDRI2ScreenInit(pScreen);
	}
	return ret;
}

void
MSMFlushAccel(ScreenPtr pScreen)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	if (pMsm->xa) {
#ifdef HAVE_XA
		MSMFlushXA(pMsm);
#endif
	} else {
		FIRE_RING(pMsm);
	}
}

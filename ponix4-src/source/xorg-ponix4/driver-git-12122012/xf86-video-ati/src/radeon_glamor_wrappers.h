/*
 * Copyright © 2000,2008 Keith Packard
 *             2004 Eric Anholt
 *             2005 Zack Rusin, Trolltech
 *             2012 Advanced Micro Devices, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of The copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#ifndef RADEON_GLAMOR_WRAPPERS_H
#define RADEON_GLAMOR_WRAPPERS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif
#include <xorg-server.h>
#include "xf86.h"

#include <X11/X.h>
#include <X11/Xproto.h>
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "windowstr.h"
#include "servermd.h"
#include "colormapst.h"
#include "gcstruct.h"
#include "input.h"
#include "mipointer.h"
#include "mi.h"
#include "dix.h"
#include "fb.h"
#include "fboverlay.h"
#ifdef RENDER
//#include "fbpict.h"
#include "glyphstr.h"
#include "picturestr.h"
#endif
#include "damage.h"

#include "../src/compat-api.h"

/* Provide substitutes for gcc's __FUNCTION__ on other compilers */
#if !defined(__GNUC__) && !defined(__FUNCTION__)
# if defined(__STDC__) && (__STDC_VERSION__>=199901L)	/* C99 */
#  define __FUNCTION__ __func__
# else
#  define __FUNCTION__ ""
# endif
#endif

/* 1.6 and earlier server compat */
#ifndef miGetCompositeClip
#define miCopyRegion fbCopyRegion
#define miDoCopy fbDoCopy
#endif

typedef enum {
	GLAMOR_CPU_ACCESS_RO,
	GLAMOR_CPU_ACCESS_RW,
	GLAMOR_GPU_ACCESS_RO,
	GLAMOR_GPU_ACCESS_RW
} glamor_access_t;

#include "radeon.h"
#include "glamor.h"


Bool glamor_screen_init(ScreenPtr screen);

void glamor_set_fallback_debug(ScreenPtr screen, Bool enable);

#define DEBUG_MIGRATE		0
#define DEBUG_PIXMAP		0
#define DEBUG_OFFSCREEN		0
#define DEBUG_GLYPH_CACHE	0

#define GLAMOR_FALLBACK(x)     					\
if (glamor_get_screen(screen)->fallback_debug) {			\
	ErrorF("GLAMOR fallback at %s: ", __FUNCTION__);		\
	ErrorF x;						\
}

#if DEBUG_PIXMAP
#define DBG_PIXMAP(a) ErrorF a
#else
#define DBG_PIXMAP(a)
#endif

typedef void (*EnableDisableFBAccessProcPtr) (int, Bool);
typedef struct {
	CreateGCProcPtr SavedCreateGC;
	CloseScreenProcPtr SavedCloseScreen;
	GetImageProcPtr SavedGetImage;
	GetSpansProcPtr SavedGetSpans;
	CreatePixmapProcPtr SavedCreatePixmap;
	DestroyPixmapProcPtr SavedDestroyPixmap;
	CopyWindowProcPtr SavedCopyWindow;
	ChangeWindowAttributesProcPtr SavedChangeWindowAttributes;
	BitmapToRegionProcPtr SavedBitmapToRegion;
#ifdef RENDER
	CompositeProcPtr SavedComposite;
	TrianglesProcPtr SavedTriangles;
	GlyphsProcPtr SavedGlyphs;
	TrapezoidsProcPtr SavedTrapezoids;
	AddTrapsProcPtr SavedAddTraps;
	UnrealizeGlyphProcPtr SavedUnrealizeGlyph;
#endif

	Bool fallback_debug;
} glamor_screen_t;

/*
 * This is the only completely portable way to
 * compute this info.
 */
#ifndef BitsPerPixel
#define BitsPerPixel(d) (\
    PixmapWidthPaddingInfo[d].notPower2 ? \
    (PixmapWidthPaddingInfo[d].bytesPerPixel * 8) : \
    ((1 << PixmapWidthPaddingInfo[d].padBytesLog2) * 8 / \
    (PixmapWidthPaddingInfo[d].padRoundUp+1)))
#endif

#if HAS_DEVPRIVATEKEYREC
extern DevPrivateKeyRec glamor_screen_index;
#else
extern int glamor_screen_index;
#endif

static inline glamor_screen_t *glamor_get_screen(ScreenPtr screen)
{
#if HAS_DEVPRIVATEKEYREC
	return dixGetPrivate(&screen->devPrivates, &glamor_screen_index);
#else
	return dixLookupPrivate(&screen->devPrivates, &glamor_screen_index);
#endif
}

#ifdef RENDER

/* XXX these are in fbpict.h, which is not installed */
void
fbComposite(CARD8 op,
	    PicturePtr pSrc,
	    PicturePtr pMask,
	    PicturePtr pDst,
	    INT16 xSrc,
	    INT16 ySrc,
	    INT16 xMask,
	    INT16 yMask, INT16 xDst, INT16 yDst, CARD16 width, CARD16 height);

void
fbAddTraps(PicturePtr pPicture,
	   INT16 xOff, INT16 yOff, int ntrap, xTrap * traps);

#endif

#endif /* RADEON_GLAMOR_WRAPPERS_H */

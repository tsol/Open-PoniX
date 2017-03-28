/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2009 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _ICLASS_H
#define _ICLASS_H

#include "eimage.h"
#include "etypes.h"
#include "xwin.h"

#define BEVEL_NONE              0
#define BEVEL_AMIGA             1
#define BEVEL_MOTIF             2
#define BEVEL_NEXT              3
#define BEVEL_DOUBLE            4
#define BEVEL_WIDEDOUBLE        5
#define BEVEL_THINPOINT         6
#define BEVEL_THICKPOINT        7

#define FILL_STRETCH            0
#define FILL_TILE_H             1
#define FILL_TILE_V             2
#define FILL_INT_TILE_H         4
#define FILL_INT_TILE_V         8

/* Selective Transparency item types */
#define ST_SOLID 	0
#define ST_BORDER	1
#define ST_WIDGET	2
#define ST_ICONBOX	3
#define ST_MENU		4
#define ST_MENU_ITEM	5
#define ST_TOOLTIP	6
#define ST_DIALOG	7
#define ST_HILIGHT	8
#define ST_PAGER	9
#define ST_WARPLIST	10
#define ST_BUTTON	11

#define ICLASS_ATTR_OPAQUE      0x00	/* No transparency */
#define ICLASS_ATTR_BG          0x01	/* Background transparency */
#define ICLASS_ATTR_GLASS       0x02	/* Glass transparency */
#define ICLASS_ATTR_NO_CLIP     0x04	/* Don't apply clip mask */
#define ICLASS_ATTR_USE_CM      0x08	/* Use colormodifier */

/* ImageclassApplyCopy flags */
#define IC_FLAG_NONE            0x00	/* No flags */
#define IC_FLAG_WRITABLE        0x01	/* Provide writable pixmaps */
#define IC_FLAG_MAKE_MASK       0x02	/* Make mask */
#define IC_FLAG_FULL_SIZE       0x04	/* Make full size pixmaps */

/* ITApply flags */
#define ITA_JUSTV               0x01	/* Justify text vertically */
#define ITA_BGPMAP              0x02	/* Create window background pixmap */

/* iclass.c */
int                 ImageclassConfigLoad(FILE * fs);

#ifdef ENABLE_THEME_TRANSPARENCY
void                TransparencySet(int transparency);
int                 TransparencyEnabled(void);
int                 TransparencyUpdateNeeded(void);
int                 ImageclassIsTransparent(ImageClass * ic);
#else
#define TransparencyEnabled() 0
#define TransparencyUpdateNeeded() 0
#define ImageclassIsTransparent(ic) 0
#endif

ImageClass         *ImageclassFind(const char *name, int fallback);
ImageClass         *ImageclassAlloc(const char *name, int fallback);
void                ImageclassFree(ImageClass * ic);
const char         *ImageclassGetName(ImageClass * ic);
EImageBorder       *ImageclassGetPadding(ImageClass * ic);
ImageState         *ImageclassGetImageState(ImageClass * ic, int state,
					    int active, int sticky);
ImageClass         *ImageclassCreateSimple(const char *name, const char *image);
ImageClass         *ImageclassGetBlack(void);
EImage             *ImageclassGetImage(ImageClass * ic, int active, int sticky,
				       int state);
void                ImageclassApplySimple(ImageClass * ic, Win win,
					  Drawable draw, int state, int x,
					  int y, int w, int h);
void                ImageclassApply(ImageClass * ic, Win win,
				    int active, int sticky, int state,
				    int image_type);
void                ImageclassApplyCopy(ImageClass * ic, Win win, int w,
					int h, int active, int sticky,
					int state, PmapMask * pmm,
					int pmapflags, int image_type);
EImage             *ImageclassGetImageBlended(ImageClass * ic, Win win,
					      int w, int h, int active,
					      int sticky, int state,
					      int image_type);
void                ITApply(Win win, ImageClass * ic, ImageState * is,
			    int state, int active, int sticky, int image_type,
			    TextClass * tc, TextState * ts, const char *text,
			    int flags);

#endif /* _ICLASS_H */

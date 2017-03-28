/*
 * Copyright (C) 2004-2012 Kim Woelders
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
/*
 * This code was originally derived from xcompmgr.c,  see original copyright
 * notice at end.
 * It has been mostly rewritten since, only the shadow code is more or less
 * intact.
 */

#include "E.h"
#if USE_COMPOSITE
#include "desktops.h"
#include "ecompmgr.h"
#include "emodule.h"
#include "eobj.h"
#include "events.h"
#include "hints.h"
#include "timers.h"
#include "windowmatch.h"
#include "xwin.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrender.h>

#define ENABLE_SHADOWS      1

#define USE_DESK_EXPOSE     0
#define USE_DESK_VISIBILITY 1

#define USE_CLIP_RELATIVE_TO_DESK 1

/* Composite Overlay Window (client) availability */
#if (1000 * COMPOSITE_MAJOR + COMPOSITE_MINOR) >= 3
#define USE_COMPOSITE_OVERLAY_WINDOW 1
#else
#define USE_COMPOSITE_OVERLAY_WINDOW 0
#endif

#define ENABLE_DEBUG   1
#if ENABLE_DEBUG
#define Dprintf(fmt...)  if(EDebug(EDBUG_TYPE_COMPMGR))Eprintf(fmt)
#define D1printf(fmt...) if(EDebug(EDBUG_TYPE_COMPMGR2))Eprintf(fmt)
#define D2printf(fmt...) if(EDebug(EDBUG_TYPE_COMPMGR3))Eprintf(fmt)
#define D3printf(fmt...) if(EDebug(EDBUG_TYPE_COMPMGR3)>1)Eprintf(fmt)
#else
#define Dprintf(fmt...)
#define D1printf(fmt...)
#define D2printf(fmt...)
#define D3printf(fmt...)
#endif /* ENABLE_DEBUG */

#define DEBUG_OPACITY 0

#define INV_POS     0x01
#define INV_SIZE    0x02
#define INV_SHAPE   0x04
#define INV_CLIP    0x08
#define INV_OPACITY 0x10
#define INV_SHADOW  0x20
#define INV_PIXMAP  0x40
#define INV_PICTURE 0x80
#define INV_GEOM    (INV_POS | INV_SIZE | INV_SHAPE)
#define INV_ALL     (INV_POS | INV_SIZE | INV_CLIP | INV_OPACITY | INV_SHADOW | INV_PIXMAP)

typedef struct _cmhook ECmWinInfo;

struct _cmhook {
   EObj               *next;	/* Paint order */
   EObj               *prev;	/* Paint order */
   Pixmap              pixmap;
   int                 rcx, rcy, rcw, rch;
   int                 mode;
   unsigned            damaged:1;
   unsigned            fadeout:1;
   unsigned            has_shadow:1;
   unsigned            have_shape:1;	/* Region validity - shape */
   unsigned            have_extents:1;	/* Region validity - extents */
   unsigned            have_clip:1;	/* Region validity - clip */
   Damage              damage;
   Picture             picture;
   Picture             pict_alpha;	/* Solid, current opacity */
   XserverRegion       shape;
   XserverRegion       extents;
   XserverRegion       clip;
   int                 shape_x, shape_y;
#if ENABLE_SHADOWS
   Picture             shadow_alpha;	/* Solid, sharp * current opacity */
   Picture             shadow_pict;	/* Blurred shaped shadow */
   int                 shadow_dx;
   int                 shadow_dy;
   int                 shadow_width;
   int                 shadow_height;
#endif
   unsigned int        opacity;

   unsigned int        damage_sequence;	/* sequence when damage was created */

   Animator           *anim_fade;
   unsigned int        opacity_to;
};

/*
 * Configuration
 */
#if ENABLE_SHADOWS
#define ECM_SHADOWS_OFF      0
#define ECM_SHADOWS_SHARP    1	/* use window alpha for shadow; sharp, but precise */
#define ECM_SHADOWS_ECHO     3	/* use window for shadow; sharp, but precise */
#define ECM_SHADOWS_BLURRED  2	/* use window extents for shadow, blurred */
#endif

#define ECM_OR_UNREDIRECTED  0
#define ECM_OR_ON_MAP        1
#define ECM_OR_ON_MAPUNMAP   2
#define ECM_OR_ON_CREATE     3

static struct {
   char                enable;
   char                use_name_pixmap;
#if USE_COMPOSITE_OVERLAY_WINDOW
   char                use_cow;
#endif
   int                 mode;
   struct {
      int                 mode;
      int                 offset_x, offset_y;
      struct {
	 int                 opacity;
	 int                 radius;
      } blur;
      struct {
	 int                 opacity;
      } sharp;
      unsigned int        color;
   } shadows;
   struct {
      char                enable;
      unsigned int        time;	/* Fading time, ms */
   } fading;
   struct {
      int                 mode;
      int                 opacity;
   } override_redirect;
} Conf_compmgr;

/*
 * State
 */
#define ECM_MODE_OFF    0
#define ECM_MODE_ROOT   1
#define ECM_MODE_WINDOW 2
#define ECM_MODE_AUTO   3

static struct {
   int                 mode;
   Window              root;
#if USE_COMPOSITE_OVERLAY_WINDOW
   Window              cow;
#endif
   Pixmap              pmap;	/* Compositing buffer */
   char                active;
   char                use_pixmap;
   char                reorder;
   char                ghosts;
   EObj               *eo_first;
   EObj               *eo_last;
   XserverRegion       damage;
   char                got_damage;
   XserverRegion       rgn_screen;
   XserverRegion       rgn_clip;
   int                 shadow_mode;
   float               opac_blur;	/* 0. -> 1. */
   float               opac_sharp;	/* 0. -> 1. */
} Mode_compmgr;

/* FIXME - Optimize according to what actually changed */
#define _ECM_SET_CLIP_CHANGED()   Mode_compmgr.reorder = 1
#define _ECM_SET_STACK_CHANGED()  Mode_compmgr.reorder = 1
#define _ECM_SET_SHADOW_CHANGED() Mode_compmgr.reorder = 1

static Picture      rootPicture;
static Picture      rootBuffer;

static XserverRegion rgn_tmp;	/* Region for temporary use */
static XserverRegion rgn_tmp2;	/* Region for temporary use */

static ESelection  *wm_cm_sel = NULL;

#define OPAQUE          0xffffffff
#define OP32To8(op) (((unsigned int)(op)) >> 24)

#define WINDOW_UNREDIR  0
#define WINDOW_SOLID    1
#define WINDOW_TRANS    2
#define WINDOW_ARGB     3

static void         ECompMgrDamageAll(void);
static void         ECompMgrHandleRootEvent(Win win, XEvent * ev, void *prm);
static void         ECompMgrHandleWindowEvent(Win win, XEvent * ev, void *prm);
static int          doECompMgrWinFade(void *data);
static void         ECompMgrWinInvalidate(EObj * eo, int what);
static void         ECompMgrWinSetPicts(EObj * eo);
static void         ECompMgrWinFadeEnd(EObj * eo, int done);
static int          ECompMgrDetermineOrder(EObj * const *lst, int num,
					   EObj ** first, EObj ** last,
					   Desk * dsk, XserverRegion clip);

#define PIXMAP_DESTROY(pmap) \
   if (pmap != None) { XFreePixmap(disp, pmap); pmap = None; }
#define PICTURE_DESTROY(pict) \
   if (pict != None) { XRenderFreePicture(disp, pict); pict = None; }
#define REGION_DESTROY(rgn) \
   if (rgn != None) { ERegionDestroy(rgn); rgn = None; }

void
ECompMgrWinClipToGC(EObj * eo, GC gc)
{
   XserverRegion       rgn = rgn_tmp2;

   if (!eo || !eo->cmhook)
      return;

   ERegionCopy(rgn, Mode_compmgr.rgn_screen);
   ERegionSubtract(rgn, eo->cmhook->clip);
   XFixesSetGCClipRegion(disp, gc, 0, 0, rgn);
}

#if !USE_BG_WIN_ON_ALL_DESKS
/*
 * Desk background
 */

void
ECompMgrDeskConfigure(Desk * dsk)
{
   EObj               *eo;
   ECmWinInfo         *cw;
   Picture             pict;
   XRenderPictFormat  *pictfmt;
   XRenderPictureAttributes pa;
   Pixmap              pmap;

   eo = dsk->bg.o;
   cw = eo->cmhook;

   if (dsk->bg.pmap == None)
     {
	GC                  gc;

	pmap = XCreatePixmap(disp, Mode_compmgr.root, 1, 1, WinGetDepth(VROOT));
	gc = EXCreateGC(pmap, 0, NULL);
	XSetClipMask(disp, gc, 0);
	XSetFillStyle(disp, gc, FillSolid);
	XSetForeground(disp, gc, dsk->bg.pixel);
	XFillRectangle(disp, pmap, gc, 0, 0, 1, 1);
	EXFreeGC(gc);
     }
   else
     {
	pmap = dsk->bg.pmap;
     }

   ECompMgrWinInvalidate(eo, INV_PICTURE);

   pa.repeat = True;
   pictfmt = XRenderFindVisualFormat(disp, WinGetVisual(VROOT));
   pict = XRenderCreatePicture(disp, pmap, pictfmt, CPRepeat, &pa);

   if (pmap != dsk->bg.pmap)
      XFreePixmap(disp, pmap);

   cw->picture = pict;

   D1printf
      ("ECompMgrDeskConfigure: Desk %d: using pixmap %#lx picture=%#lx\n",
       dsk->num, pmap, cw->picture);

   /* New background, all must be repainted */
   ECompMgrDamageAll();
}
#endif

#if USE_DESK_VISIBILITY
static void
ECompMgrDeskVisibility(EObj * eo, XEvent * ev)
{
   Desk               *dsk;
   int                 visible;

   switch (eo->type)
     {
     default:
	return;
     case EOBJ_TYPE_DESK:
	dsk = (Desk *) eo;
	break;
     case EOBJ_TYPE_ROOT_BG:
	dsk = DeskGet(0);
	break;
     }

   visible = dsk->viewable && ev->xvisibility.state != VisibilityFullyObscured;
   if (dsk->visible == visible)
      return;
   dsk->visible = visible;
   if (!visible)
      return;

   /*
    * A viewable desk is no longer fully obscured. Assume this happened due
    * to a VT switch to our display and repaint all. This may happen in other
    * situations as well, but most likely when we must repaint everything
    * anyway.
    */
   ECompMgrDamageAll();
}
#endif

/*
 * Root (?)
 */

static void
ECompMgrDamageMerge(XserverRegion damage)
{
   if (Mode_compmgr.got_damage)
     {
	if (EDebug(EDBUG_TYPE_COMPMGR3))
	   ERegionShow("ECompMgrDamageMerge add:", damage, NULL);

	ERegionUnion(Mode_compmgr.damage, damage);
     }
   else if (Mode_compmgr.damage != None)
     {
	ERegionCopy(Mode_compmgr.damage, damage);
     }
   else
     {
	Mode_compmgr.damage = ERegionClone(damage);
     }
   Mode_compmgr.got_damage = 1;

   if (EDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("ECompMgrDamageMerge all:", Mode_compmgr.damage, NULL);
}

static void
ECompMgrDamageMergeObject(EObj * eo, XserverRegion damage)
{
   ECmWinInfo         *cw = eo->cmhook;
   Desk               *dsk = eo->desk;

   if (!Mode_compmgr.active || damage == None)
      return;

   if (dsk->num > 0 && !dsk->viewable && eo->ilayer < 512)
      return;

   if (Mode_compmgr.reorder)
      ECompMgrDetermineOrder(NULL, 0, &Mode_compmgr.eo_first,
			     &Mode_compmgr.eo_last, DeskGet(0), None);

   damage = ERegionCopy(rgn_tmp, damage);

#if USE_CLIP_RELATIVE_TO_DESK
   if (cw->have_clip && eo->type != EOBJ_TYPE_DESK)
      ERegionSubtract(damage, cw->clip);
#endif

   if (EoGetX(dsk) != 0 || EoGetY(dsk) != 0)
      ERegionTranslate(damage, EoGetX(dsk), EoGetY(dsk));

#if !USE_CLIP_RELATIVE_TO_DESK
   if (cw->have_clip && eo->type != EOBJ_TYPE_DESK)
      ERegionSubtract(damage, cw->clip);
#endif
   if (!eo->ghost)
      Mode.events.damage_count++;

   ECompMgrDamageMerge(damage);
}

static void
ECompMgrDamageAll(void)
{
   ECompMgrDamageMerge(Mode_compmgr.rgn_screen);
}

#if ENABLE_SHADOWS

static Picture      transBlackPicture;

typedef struct {
   int                 size;
   double             *data;
} conv;

static conv        *gaussianMap = NULL;

static double
gaussian(double r, double x, double y)
{
   return ((1 / (sqrt(2 * M_PI * r))) * exp((-(x * x + y * y)) / (2 * r * r)));
}

static conv        *
make_gaussian_map(double r)
{
   conv               *c;
   int                 size = ((int)ceil((r * 3)) + 1) & ~1;
   int                 center = size / 2;
   int                 x, y;
   double              t;
   double              g;

   c = (conv *) EMALLOC(char, sizeof(conv) + size * size * sizeof(double));

   c->size = size;
   c->data = (double *)(c + 1);
   t = 0.0;
   for (y = 0; y < size; y++)
      for (x = 0; x < size; x++)
	{
	   g = gaussian(r, (double)(x - center), (double)(y - center));
	   t += g;
	   c->data[y * size + x] = g;
	}
/*    printf ("gaussian total %f\n", t); */
   for (y = 0; y < size; y++)
      for (x = 0; x < size; x++)
	{
	   c->data[y * size + x] /= t;
	}
   return c;
}

/*
 * A picture will help
 *
 *      -center   0                width  width+center
 *  -center +-----+-------------------+-----+
 *          |     |                   |     |
 *          |     |                   |     |
 *        0 +-----+-------------------+-----+
 *          |     |                   |     |
 *          |     |                   |     |
 *          |     |                   |     |
 *   height +-----+-------------------+-----+
 *          |     |                   |     |
 * height+  |     |                   |     |
 *  center  +-----+-------------------+-----+
 */

static unsigned char
sum_gaussian(conv * map, double opacity, int x, int y, int width, int height)
{
   int                 fx, fy;
   double             *g_data;
   double             *g_line = map->data;
   int                 g_size = map->size;
   int                 center = g_size / 2;
   int                 fx_start, fx_end;
   int                 fy_start, fy_end;
   double              v;

   /*
    * Compute set of filter values which are "in range",
    * that's the set with:
    *  0 <= x + (fx-center) && x + (fx-center) < width &&
    *  0 <= y + (fy-center) && y + (fy-center) < height
    *
    *  0 <= x + (fx - center)  x + fx - center < width
    *  center - x <= fx        fx < width + center - x
    */

   fx_start = center - x;
   if (fx_start < 0)
      fx_start = 0;
   fx_end = width + center - x;
   if (fx_end > g_size)
      fx_end = g_size;

   fy_start = center - y;
   if (fy_start < 0)
      fy_start = 0;
   fy_end = height + center - y;
   if (fy_end > g_size)
      fy_end = g_size;

   g_line = g_line + fy_start * g_size + fx_start;

   v = 0;
   for (fy = fy_start; fy < fy_end; fy++)
     {
	g_data = g_line;
	g_line += g_size;

	for (fx = fx_start; fx < fx_end; fx++)
	   v += *g_data++;
     }
   if (v > 1)
      v = 1;

   return ((unsigned char)(v * opacity * 255.0));
}

static XImage      *
make_shadow(double opacity, int width, int height)
{
   XImage             *ximage;
   unsigned char      *data;
   int                 gsize = gaussianMap->size;
   int                 ylimit, xlimit;
   int                 swidth = width + gsize;
   int                 sheight = height + gsize;
   int                 center = gsize / 2;
   int                 x, y;
   unsigned char       d;
   int                 x_diff;

   data = ECALLOC(unsigned char, swidth * sheight);

   if (!data)
      return NULL;

   ximage = XCreateImage(disp, DefaultVisual(disp, DefaultScreen(disp)),
			 8, ZPixmap, 0,
			 (char *)data,
			 swidth, sheight, 8, swidth * sizeof(unsigned char));
   if (!ximage)
     {
	Efree(data);
	return NULL;
     }

   /*
    * Build the gaussian in sections
    */

#if 1
   /* FIXME - Handle properly - shaped/non-shaped/offset */
   /*
    * center (fill the complete data array)
    */
   d = sum_gaussian(gaussianMap, opacity, center, center, width, height);
   memset(data, d, sheight * swidth);
#endif

   /*
    * corners
    */
   ylimit = gsize;
   if (ylimit > sheight / 2)
      ylimit = (sheight + 1) / 2;
   xlimit = gsize;
   if (xlimit > swidth / 2)
      xlimit = (swidth + 1) / 2;

   for (y = 0; y < ylimit; y++)
      for (x = 0; x < xlimit; x++)
	{
	   d = sum_gaussian(gaussianMap, opacity, x - center, y - center, width,
			    height);
	   data[y * swidth + x] = d;
	   data[(sheight - y - 1) * swidth + x] = d;
	   data[(sheight - y - 1) * swidth + (swidth - x - 1)] = d;
	   data[y * swidth + (swidth - x - 1)] = d;
	}

   /*
    * top/bottom
    */
   x_diff = swidth - (gsize * 2);
   if (x_diff > 0 && ylimit > 0)
     {
	for (y = 0; y < ylimit; y++)
	  {
	     d = sum_gaussian(gaussianMap, opacity, center, y - center, width,
			      height);
	     memset(&data[y * swidth + gsize], d, x_diff);
	     memset(&data[(sheight - y - 1) * swidth + gsize], d, x_diff);
	  }
     }

   /*
    * sides
    */
   for (x = 0; x < xlimit; x++)
     {
	d = sum_gaussian(gaussianMap, opacity, x - center, center, width,
			 height);
	for (y = gsize; y < sheight - gsize; y++)
	  {
	     data[y * swidth + x] = d;
	     data[y * swidth + (swidth - x - 1)] = d;
	  }
     }

   return ximage;
}

static              Picture
shadow_picture(double opacity, int width, int height, int *wp, int *hp)
{
   XImage             *shadowImage;
   Pixmap              shadowPixmap;
   Picture             shadowPicture;
   GC                  gc;

   shadowImage = make_shadow(opacity, width, height);
   if (!shadowImage)
      return None;

   shadowPixmap = XCreatePixmap(disp, Mode_compmgr.root,
				shadowImage->width, shadowImage->height, 8);
   shadowPicture = XRenderCreatePicture(disp, shadowPixmap,
					XRenderFindStandardFormat(disp,
								  PictStandardA8),
					0, 0);
   gc = XCreateGC(disp, shadowPixmap, 0, 0);

   XPutImage(disp, shadowPixmap, gc, shadowImage, 0, 0, 0, 0,
	     shadowImage->width, shadowImage->height);
   *wp = shadowImage->width;
   *hp = shadowImage->height;
   XFreeGC(disp, gc);
   XDestroyImage(shadowImage);
   XFreePixmap(disp, shadowPixmap);

   return shadowPicture;
}

#endif /* ENABLE_SHADOWS */

static void         ECompMgrWinSetShape(EObj * eo);

/* Region of window in screen coordinates, including shadows */
static void
ECompMgrWinSetExtents(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;
   XRectangle          r, sr;
   unsigned int        bw;

   /* FIXME - Get this right */
   bw = EobjGetBW(eo);
   if (Mode_compmgr.use_pixmap)
     {
	cw->rcx = EobjGetX(eo);
	cw->rcy = EobjGetY(eo);
	cw->rcw = EobjGetW(eo) + 2 * bw;
	cw->rch = EobjGetH(eo) + 2 * bw;
     }
   else
     {
	cw->rcx = EobjGetX(eo) + bw;
	cw->rcy = EobjGetY(eo) + bw;
	cw->rcw = EobjGetW(eo);
	cw->rch = EobjGetH(eo);
     }

   if (eo->noredir && bw)
     {
	r.x = EobjGetX(eo);
	r.y = EobjGetY(eo);
	r.width = EobjGetW(eo) + 2 * bw;
	r.height = EobjGetH(eo) + 2 * bw;
     }
   else
     {
	r.x = cw->rcx;
	r.y = cw->rcy;
	r.width = cw->rcw;
	r.height = cw->rch;
     }

   if (cw->extents == None)
      cw->extents = ERegionCreate();

#if ENABLE_SHADOWS
   cw->has_shadow = (Mode_compmgr.shadow_mode != ECM_SHADOWS_OFF) &&
      eo->shadow && (EShapeCheck(EobjGetWin(eo)) >= 0);
   if (!cw->has_shadow)
      goto skip_shadow;

   switch (Mode_compmgr.shadow_mode)
     {
     default:
	goto skip_shadow;

     case ECM_SHADOWS_SHARP:
     case ECM_SHADOWS_ECHO:
	cw->shadow_dx = Conf_compmgr.shadows.offset_x;
	cw->shadow_dy = Conf_compmgr.shadows.offset_y;
	cw->shadow_width = cw->rcw;
	cw->shadow_height = cw->rch;
	break;

     case ECM_SHADOWS_BLURRED:
	if (EobjIsShaped(eo) /* || cw->mode == WINDOW_ARGB */ )
	   goto skip_shadow;

	if (!gaussianMap)
	  {
	     gaussianMap =
		make_gaussian_map((double)Conf_compmgr.shadows.blur.radius);
	     if (!gaussianMap)
		goto skip_shadow;
	  }

	cw->shadow_dx = Conf_compmgr.shadows.offset_x - gaussianMap->size / 2;
	cw->shadow_dy = Conf_compmgr.shadows.offset_y - gaussianMap->size / 2;
	if (!cw->shadow_pict)
	   cw->shadow_pict = shadow_picture(Mode_compmgr.opac_blur,
					    cw->rcw, cw->rch,
					    &cw->shadow_width,
					    &cw->shadow_height);
	break;
     }
   sr.x = cw->rcx + cw->shadow_dx;
   sr.y = cw->rcy + cw->shadow_dy;
   sr.width = cw->shadow_width;
   sr.height = cw->shadow_height;
   if (sr.x < r.x)
     {
	r.width = (r.x + r.width) - sr.x;
	r.x = sr.x;
     }
   if (sr.y < r.y)
     {
	r.height = (r.y + r.height) - sr.y;
	r.y = sr.y;
     }
   if (sr.x + sr.width > r.x + r.width)
      r.width = sr.x + sr.width - r.x;
   if (sr.y + sr.height > r.y + r.height)
      r.height = sr.y + sr.height - r.y;

   ERegionSetRect(cw->extents, r.x, r.y, r.width, r.height);
   goto done;

 skip_shadow:
#endif

   /* No shadow - extents = shape */
   if (!cw->have_shape)
      ECompMgrWinSetShape(eo);
   ERegionCopy(cw->extents, cw->shape);

 done:
   cw->have_extents = 1;

   D1printf("extents %#lx %d %d %d %d\n", EobjGetXwin(eo), r.x, r.y, r.width,
	    r.height);

   if (EDebug(EDBUG_TYPE_COMPMGR2))
      ERegionShow("extents", cw->extents, NULL);
}

/* Region of shaped window in screen coordinates */
static void
ECompMgrWinSetShape(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;
   int                 x, y;

   if (cw->shape == None)
     {
#if 0				/* FIXME - TBD */
	/* If the window shape is changed on OR windows, cw->shape set below
	 * may not reflect the shape change. Adding a sync fixes things in
	 * some situations. */
	if (eo->type == EOBJ_TYPE_EXT)
	   ESync(0);
#endif
	cw->shape = ERegionCreateFromWindow(EobjGetWin(eo));

	if (WinIsShaped(EobjGetWin(eo)))
	  {
	     /* Intersect with window size to get effective bounding region */
	     ERegionSetRect(rgn_tmp, 0, 0, EobjGetW(eo), EobjGetH(eo));
	     ERegionIntersect(cw->shape, rgn_tmp);
	  }
	x = EobjGetX(eo) + EobjGetBW(eo);
	y = EobjGetY(eo) + EobjGetBW(eo);
     }
   else
     {
	x = EobjGetX(eo) + EobjGetBW(eo) - cw->shape_x;
	y = EobjGetY(eo) + EobjGetBW(eo) - cw->shape_y;
     }

   ERegionTranslate(cw->shape, x, y);

   cw->shape_x = EobjGetX(eo) + EobjGetBW(eo);
   cw->shape_y = EobjGetY(eo) + EobjGetBW(eo);
   cw->have_shape = 1;

   D1printf("shape %#lx: %d %d\n", EobjGetXwin(eo), cw->shape_x, cw->shape_y);
   if (EDebug(EDBUG_TYPE_COMPMGR2))
      ERegionShow("shape", cw->shape, NULL);
}

Pixmap
ECompMgrWinGetPixmap(const EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (!cw)
      return None;

   if (cw->pixmap != None)
      return cw->pixmap;

   if (eo->noredir)
      return None;

   cw->pixmap = EWindowGetPixmap(EobjGetWin(eo));

   return cw->pixmap;
}

Picture
ECompMgrWinGetAlphaPict(const EObj * eo)
{
   return (eo->cmhook) ? eo->cmhook->pict_alpha : None;
}

static void
ECompMgrWinInvalidate(EObj * eo, int what)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (!cw)
      return;

   D1printf("ECompMgrWinInvalidate %#lx: %#x\n", EobjGetXwin(eo), what);

   if ((what & (INV_SIZE | INV_PIXMAP)) && cw->pixmap != None)
     {
	XFreePixmap(disp, cw->pixmap);
	cw->pixmap = None;
	if (Mode_compmgr.use_pixmap)
	   what |= INV_PICTURE;
#if USE_GLX
	EobjTextureInvalidate(eo);
#endif
     }

   if (what & (INV_SIZE | INV_SHAPE))
      REGION_DESTROY(cw->shape);
   if (what & INV_GEOM)
      cw->have_shape = 0;

   if (what & INV_PICTURE)
      PICTURE_DESTROY(cw->picture);

   if (what & INV_OPACITY)
      PICTURE_DESTROY(cw->pict_alpha);

   if (what & (INV_CLIP | INV_GEOM))
      cw->have_clip = 0;

#if ENABLE_SHADOWS
   if (what & (INV_SIZE | INV_SHADOW))
      PICTURE_DESTROY(cw->shadow_pict);
   if (what & (INV_OPACITY | INV_SHADOW))
      PICTURE_DESTROY(cw->shadow_alpha);
#endif

   if (what & (INV_GEOM | INV_SHADOW))
      cw->have_extents = 0;
}

void
ECompMgrWinSetOpacity(EObj * eo, unsigned int opacity)
{
   ECmWinInfo         *cw = eo->cmhook;
   int                 mode;

   if (!cw || cw->opacity == opacity)
      return;

   cw->opacity = opacity;

   D1printf("ECompMgrWinSetOpacity: %#lx opacity=%#x\n", EobjGetXwin(eo),
	    cw->opacity);

   if (eo->shown || cw->fadeout)
      ECompMgrDamageMergeObject(eo, cw->extents);

   /* Invalidate stuff changed by opacity */
   ECompMgrWinInvalidate(eo, INV_OPACITY);

   if (eo->noredir)
      mode = WINDOW_UNREDIR;
   else if (EobjGetWin(eo)->argb)
      mode = WINDOW_ARGB;
   else if (cw->opacity != OPAQUE)
      mode = WINDOW_TRANS;
   else
      mode = WINDOW_SOLID;

   if (mode != cw->mode)
      _ECM_SET_CLIP_CHANGED();

   cw->mode = mode;
}

static int
doECompMgrWinFade(void *data)
{
   EObj               *eo;
   ECmWinInfo         *cw;
   unsigned int        op, step;

   eo = (EObj *) data;

   /* May be gone */
   if (EobjListStackCheck(eo) < 0)
      return 0;

   cw = eo->cmhook;
   op = cw->opacity_to;

#if DEBUG_OPACITY
   Eprintf("%s %#lx: %u/%u, %#x->%#x\n", __func__, EobjGetXwin(eo),
	   eo->fading, cw->fadeout, cw->opacity, op);
#endif
   if (!eo->fading)
      goto done;

   eo->fading = cw->fadeout;

   step = Conf_compmgr.fading.time / Conf.animation.step;
   if (step == 0)
      step = 1;
   step = 0xffffffff / step;
   if (op == cw->opacity)
     {
	op = eo->opacity;
	ECompMgrWinFadeEnd(eo, 0);
     }
   else if (op > cw->opacity)
     {
	if (op - cw->opacity > step)
	  {
	     op = cw->opacity + step;
	     eo->fading = 1;
	  }
     }
   else
     {
	if (cw->opacity - op > step)
	  {
	     op = cw->opacity - step;
	     eo->fading = 1;
	  }
     }

#if DEBUG_OPACITY
   Eprintf("%s %#lx: %#x\n", __func__, EobjGetXwin(eo), op);
#endif
   ECompMgrWinSetOpacity(eo, op);

   if (eo->fading)
      return 1;

   if (eo->type == EOBJ_TYPE_EWIN)
      ModulesSignal(eo->shown ? ESIGNAL_EWIN_CHANGE : ESIGNAL_EWIN_UNMAP, eo);

 done:
   cw->anim_fade = NULL;
   return 0;
}

static void
ECompMgrWinFade(EObj * eo, unsigned int op_from, unsigned int op_to)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (op_from == op_to && op_to == eo->opacity)
     {
	if (eo->fading)
	   ECompMgrWinFadeEnd(eo, 0);
	return;
     }

   if (!cw->anim_fade)
      cw->anim_fade = AnimatorAdd(doECompMgrWinFade, eo);
   cw->opacity_to = op_to;

   eo->fading = 1;
   ECompMgrWinSetOpacity(eo, op_from);
}

static void
ECompMgrWinFadeIn(EObj * eo)
{
#if DEBUG_OPACITY
   ECmWinInfo         *cw = eo->cmhook;

   Eprintf("%s %#lx: %u/%u, %#x %#x->%#x\n", __func__, EobjGetXwin(eo),
	   eo->fading, cw->fadeout, eo->opacity, 0x10000000, cw->opacity);
#endif
   ECompMgrWinFade(eo, 0x10000000, eo->opacity);
}

static void
ECompMgrWinFadeOut(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

#if DEBUG_OPACITY
   Eprintf("%s %#lx: %u/%u, %#x %#x->%#x\n", __func__, EobjGetXwin(eo),
	   eo->fading, cw->fadeout, eo->opacity, cw->opacity, 0x10000000);
#endif
   cw->fadeout = 1;
   ECompMgrWinFade(eo, cw->opacity, 0x10000000);
}

static void
ECompMgrWinFadeEnd(EObj * eo, int done)
{
   ECmWinInfo         *cw = eo->cmhook;

#if DEBUG_OPACITY
   Eprintf("%s %#lx: done=%d\n", __func__, EobjGetXwin(eo), done);
#endif
   if (cw->fadeout)
     {
	cw->fadeout = 0;
	ECompMgrWinInvalidate(eo, INV_PIXMAP | INV_PICTURE);
	ECompMgrDamageMergeObject(eo, cw->extents);
	_ECM_SET_CLIP_CHANGED();
     }
   eo->fading = 0;
   if (done)
     {
	AnimatorDel(cw->anim_fade);
	cw->anim_fade = NULL;
     }
}

void
ECompMgrWinChangeOpacity(EObj * eo, unsigned int opacity)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (!cw)
      return;

   if (eo->shown && Conf_compmgr.fading.enable && eo->fade)
      ECompMgrWinFade(eo, cw->opacity, opacity);
   else
      ECompMgrWinSetOpacity(eo, opacity);
}

void
ECompMgrWinMap(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (!cw)
     {
	ECompMgrWinNew(eo);
	cw = eo->cmhook;
	if (!cw)
	   return;
     }

   D1printf("ECompMgrWinMap %#lx\n", EobjGetXwin(eo));

   if (!cw->have_extents)
      ECompMgrWinSetExtents(eo);

   _ECM_SET_STACK_CHANGED();
   ECompMgrDamageMergeObject(eo, cw->extents);

   if (Conf_compmgr.fading.enable && eo->fade)
      ECompMgrWinFadeIn(eo);
}

void
ECompMgrWinUnmap(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   D1printf("ECompMgrWinUnmap %#lx shown=%d\n", EobjGetXwin(eo), eo->shown);
   if (!eo->shown)		/* Sometimes we get a synthetic one too */
      return;

   ECompMgrDamageMergeObject(eo, cw->extents);
   _ECM_SET_STACK_CHANGED();
   if (Conf_compmgr.fading.enable && eo->fade && !eo->gone)
     {
	ECompMgrWinInvalidate(eo, INV_PICTURE);
	ECompMgrWinFadeOut(eo);
     }
   else
      ECompMgrWinInvalidate(eo, INV_PIXMAP);
}

static void
ECompMgrWinSetPicts(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (cw->pixmap == None && eo->shown && !eo->noredir &&
       (Mode_compmgr.use_pixmap || (eo->fade && Conf_compmgr.fading.enable)))
     {
	cw->pixmap = EWindowGetPixmap(EobjGetWin(eo));
	D1printf("ECompMgrWinSetPicts %#lx: Pmap=%#lx\n", EobjGetXwin(eo),
		 cw->pixmap);
     }

   if (cw->picture == None)
     {
	XRenderPictFormat  *pictfmt;
	XRenderPictureAttributes pa;
	Drawable            draw = EobjGetXwin(eo);

	if ((cw->pixmap && Mode_compmgr.use_pixmap) || (cw->fadeout))
	   draw = cw->pixmap;
	if (draw == None)
	   return;

	pictfmt = XRenderFindVisualFormat(disp, WinGetVisual(EobjGetWin(eo)));
	pa.subwindow_mode = IncludeInferiors;
	cw->picture = XRenderCreatePicture(disp, draw,
					   pictfmt, CPSubwindowMode, &pa);
	D1printf("ECompMgrWinSetPicts %#lx: Pict=%#lx (drawable=%#lx)\n",
		 EobjGetXwin(eo), cw->picture, draw);

#if 0				/* Pixmap must be clipped by window shape */
	if (draw == cw->pixmap && WinIsShaped(EobjGetWin(eo)))
	  {
	     XserverRegion       clip;

	     clip = ERegionCreateFromWindow(EobjGetWin(eo));
	     EPictureSetClip(cw->picture, clip);
	     ERegionDestroy(clip);
	  }
#endif
     }
}

void
ECompMgrWinNew(EObj * eo)
{
   ECmWinInfo         *cw;

   if (!Mode_compmgr.active)	/* FIXME - Here? */
      return;

   if (eo->inputonly || EobjGetWin(eo) == VROOT)
     {
	eo->noredir = 1;
	return;
     }

   cw = ECALLOC(ECmWinInfo, 1);
   if (!cw)
      return;

   D1printf("ECompMgrWinNew %#lx\n", EobjGetXwin(eo));

   eo->cmhook = cw;

   if (eo->type == EOBJ_TYPE_EXT &&
       Conf_compmgr.override_redirect.mode == ECM_OR_UNREDIRECTED)
     {
	eo->noredir = 1;
	eo->fade = 0;
	eo->shadow = 0;
     }
   if (eo->type == EOBJ_TYPE_EXT)
     {
	XShapeSelectInput(disp, EobjGetXwin(eo), ShapeNotifyMask);
	EShapeUpdate(EobjGetWin(eo));
     }

   if (eo->noredir)
     {
	/* If already auto-redirected undo that */
	if (Mode_compmgr.mode == ECM_MODE_ROOT)
	  {
	     XCompositeUnredirectWindow(disp, EobjGetXwin(eo),
					CompositeRedirectManual);
	     if (eo->type == EOBJ_TYPE_DESK)
		XCompositeRedirectSubwindows(disp, EobjGetXwin(eo),
					     CompositeRedirectManual);
	  }
     }
   else
     {
	if (Mode_compmgr.mode == ECM_MODE_WINDOW)
	   XCompositeRedirectWindow(disp, EobjGetXwin(eo),
				    CompositeRedirectManual);
	cw->damage_sequence = NextRequest(disp);
	cw->damage =
	   XDamageCreate(disp, EobjGetXwin(eo), XDamageReportNonEmpty);
     }

   if (eo->type == EOBJ_TYPE_EXT)
      eo->opacity = OpacityFromPercent(Conf_compmgr.override_redirect.opacity);
   if (eo->opacity == 0)
      eo->opacity = 0xFFFFFFFF;

   if (eo->type == EOBJ_TYPE_DESK || eo->type == EOBJ_TYPE_ROOT_BG)
     {
	ESelectInputChange(EobjGetWin(eo), VisibilityChangeMask, 0);
     }

   if (eo->type != EOBJ_TYPE_EWIN)
      WindowMatchEobjOps(eo);

   cw->opacity = 0xdeadbeef;
   ECompMgrWinSetOpacity(eo, eo->opacity);

   EventCallbackRegister(EobjGetWin(eo), ECompMgrHandleWindowEvent, eo);
}

void
ECompMgrWinMoveResize(EObj * eo, int change_xy, int change_wh, int change_bw)
{
   ECmWinInfo         *cw = eo->cmhook;
   int                 invalidate;

   D1printf("ECompMgrWinMoveResize %#lx xy=%d wh=%d bw=%d\n",
	    EobjGetXwin(eo), change_xy, change_wh, change_bw);

   invalidate = 0;
   if (change_xy || change_bw)
      invalidate |= INV_POS;
   if (change_wh || change_bw)
      invalidate |= INV_SIZE;

   if (!invalidate)
      return;

   if (cw->fadeout)
      ECompMgrWinFadeEnd(eo, 1);

   if (!eo->shown)
     {
	ECompMgrWinInvalidate(eo, invalidate);
	return;
     }

   if (EDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("old-extents:", cw->extents, NULL);

#if 0				/* FIXME - We shouldn't have to update clip if transparent */
   if (cw->mode == WINDOW_UNREDIR || cw->mode == WINDOW_SOLID)
#endif
      _ECM_SET_CLIP_CHANGED();

   if (cw->have_extents)
     {
	/* Invalidate old window region */
	ERegionCopy(rgn_tmp2, cw->extents);
	ECompMgrWinInvalidate(eo, invalidate);
	/* Invalidate new window region */
	ECompMgrWinSetExtents(eo);
	ERegionUnion(rgn_tmp2, cw->extents);
	ECompMgrDamageMergeObject(eo, rgn_tmp2);
     }
   else
     {
	ECompMgrWinInvalidate(eo, invalidate);
	/* Invalidate new window region */
	ECompMgrWinSetExtents(eo);
	ECompMgrDamageMergeObject(eo, cw->extents);
     }
}

void
ECompMgrWinDamageArea(EObj * eo, int x __UNUSED__, int y __UNUSED__,
		      int w __UNUSED__, int h __UNUSED__)
{
   ECmWinInfo         *cw = eo->cmhook;

   ECompMgrDamageMergeObject(eo, cw->shape);
}

void
ECompMgrWinChangeShadow(EObj * eo, int shadow)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (!cw || !eo->shown)
      goto done;

   if (!shadow && eo->shadow)
     {
	/* Disable shadow */
	ECompMgrDamageMergeObject(eo, cw->extents);
	ECompMgrWinInvalidate(eo, INV_SHADOW);
     }
   else if (shadow && !eo->shadow)
     {
	/* Enable shadow */
	ECompMgrWinInvalidate(eo, INV_SHADOW);
	eo->shadow = shadow;
	ECompMgrWinSetExtents(eo);
	ECompMgrDamageMergeObject(eo, cw->extents);
     }
 done:
   eo->shadow = shadow;
}

static void
ECompMgrWinConfigure(EObj * eo, XEvent * ev)
{
   int                 x, y, w, h, bw;
   int                 change_xy, change_wh, change_bw;

   x = ev->xconfigure.x;
   y = ev->xconfigure.y;
   w = ev->xconfigure.width;
   h = ev->xconfigure.height;
   bw = ev->xconfigure.border_width;

   change_xy = EobjGetX(eo) != x || EobjGetY(eo) != y;
   change_wh = EobjGetW(eo) != w || EobjGetH(eo) != h;
   change_bw = EobjGetBW(eo) != bw;

   EWindowSetGeometry(EobjGetWin(eo), x, y, w, h, bw);

   ECompMgrWinMoveResize(eo, change_xy, change_wh, change_bw);
}

void
ECompMgrWinReparent(EObj * eo, Desk * dsk, int change_xy)
{
   ECmWinInfo         *cw = eo->cmhook;

   D1printf("ECompMgrWinReparent %#lx %#lx d=%d->%d x,y=%d,%d %d\n",
	    EobjGetXwin(eo), cw->extents,
	    (eo->desk) ? (int)eo->desk->num : -1, dsk->num,
	    EobjGetX(eo), EobjGetY(eo), change_xy);

   if (!eo->shown)
     {
	if (change_xy)
	   ECompMgrWinInvalidate(eo, INV_POS);
	return;
     }

   /* Invalidate old window region */
   if (EDebug(EDBUG_TYPE_COMPMGR3))
      ERegionShow("old-extents:", cw->extents, NULL);
   ECompMgrDamageMergeObject(eo, cw->extents);
   if (change_xy)
     {
	ECompMgrWinInvalidate(eo, INV_POS);

	/* Find new window region */
	ECompMgrWinSetExtents(eo);
     }
   eo->desk = dsk;
   _ECM_SET_STACK_CHANGED();
   ECompMgrDamageMergeObject(eo, cw->extents);
   ECompMgrWinInvalidate(eo, INV_PIXMAP);
}

static void
ECompMgrWinCirculate(EObj * eo, XEvent * ev)
{
   D1printf("ECompMgrWinCirculate %#lx %#lx\n", ev->xany.window,
	    EobjGetXwin(eo));

   _ECM_SET_STACK_CHANGED();
}

void
ECompMgrWinChangeShape(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   D1printf("ECompMgrWinChangeShape %#lx\n", EobjGetXwin(eo));

   EShapeUpdate(EobjGetWin(eo));

   if (cw->extents == None)
      return;

   ECompMgrDamageMergeObject(eo, cw->extents);
   ECompMgrWinInvalidate(eo, INV_SHAPE);	/* Invalidate extents and shape */
   ECompMgrWinSetExtents(eo);
   ECompMgrDamageMergeObject(eo, cw->extents);
   _ECM_SET_CLIP_CHANGED();
}

void
ECompMgrWinRaiseLower(EObj * eo, int delta)
{
   ECmWinInfo         *cw = eo->cmhook;

   D1printf("ECompMgrWinRaiseLower %#lx delta=%d\n", EobjGetXwin(eo), delta);

   if (delta < 0)		/* Raise */
      _ECM_SET_STACK_CHANGED();
   ECompMgrDamageMergeObject(eo, cw->extents);
   if (delta > 0)		/* Lower */
      _ECM_SET_STACK_CHANGED();
}

void
ECompMgrWinDel(EObj * eo)
{
   ECmWinInfo         *cw = eo->cmhook;

   if (!cw)
      return;

   D1printf("ECompMgrWinDel %#lx\n", EobjGetXwin(eo));

   if (eo->fading)
      ECompMgrWinFadeEnd(eo, 1);

   EventCallbackUnregister(EobjGetWin(eo), ECompMgrHandleWindowEvent, eo);

   if (!eo->gone)
     {
	ECompMgrWinInvalidate(eo, INV_PICTURE);

	if (eo->noredir)
	  {
	     if (Mode_compmgr.mode == ECM_MODE_ROOT &&
		 eo->type == EOBJ_TYPE_DESK)
		XCompositeUnredirectSubwindows(disp, EobjGetXwin(eo),
					       CompositeRedirectManual);
	  }
	else
	  {
	     if (cw->damage != None)
		XDamageDestroy(disp, cw->damage);

	     if (Mode_compmgr.mode == ECM_MODE_WINDOW)
		XCompositeUnredirectWindow(disp, EobjGetXwin(eo),
					   CompositeRedirectManual);
	  }
     }

   ECompMgrWinInvalidate(eo, INV_ALL);
   REGION_DESTROY(cw->extents);
   REGION_DESTROY(cw->clip);

   Efree(eo->cmhook);
   eo->cmhook = NULL;

   _ECM_SET_STACK_CHANGED();
}

static void
ECompMgrWinDamage(EObj * eo, XEvent * ev)
{
   ECmWinInfo         *cw = eo->cmhook;
   XDamageNotifyEvent *de = (XDamageNotifyEvent *) ev;
   XserverRegion       parts;

   D2printf("ECompMgrWinDamage %#lx %#lx damaged=%d %d,%d %dx%d\n",
	    ev->xany.window, EobjGetXwin(eo), cw->damaged,
	    de->area.x, de->area.y, de->area.width, de->area.height);

   if (!cw->damaged)
     {
	parts = cw->extents;
	XDamageSubtract(disp, cw->damage, None, None);
	cw->damaged = 1;
     }
   else
     {
	parts = rgn_tmp;
	XDamageSubtract(disp, cw->damage, None, parts);
	ERegionTranslate(parts, EobjGetX(eo) + EobjGetBW(eo),
			 EobjGetY(eo) + EobjGetBW(eo));
     }
   eo->serial = ev->xany.serial;
   ECompMgrDamageMergeObject(eo, parts);

   if (eo->type == EOBJ_TYPE_EWIN)
      ModulesSignal(ESIGNAL_EWIN_DAMAGE, eo);
}

static void
ECompMgrWinDumpInfo(const char *txt, EObj * eo, XserverRegion rgn, int ipc)
{
   void                (*prf) (const char *fmt, ...);
   ECmWinInfo         *cw = eo->cmhook;

   prf = (ipc) ? IpcPrintf : Eprintf;

   prf("%s %#lx: %d,%d %dx%d: %s\n", txt, EobjGetXwin(eo),
       EobjGetX(eo), EobjGetY(eo), EobjGetW(eo), EobjGetH(eo), EobjGetName(eo));
   if (!cw)
     {
	prf("Not managed\n");
	return;
     }

   if (ipc || EDebug(EDBUG_TYPE_COMPMGR3))
     {
	prf(" - pict=%#lx pmap=%#lx\n", cw->picture, cw->pixmap);

	ERegionShow("win extents", cw->extents, prf);
	ERegionShow("win shape  ", cw->shape, prf);
	ERegionShow("win clip   ", cw->clip, prf);
	if (rgn != None)
	   ERegionShow("region", rgn, prf);
     }
}

static void
ECompMgrDestroyClip(void)
{
   EObj               *eo, *const *lst;
   ECmWinInfo         *cw;
   int                 i, num;

   lst = EobjListStackGet(&num);
   for (i = 0; i < num; i++)
     {
	eo = lst[i];
	cw = eo->cmhook;
	if (!cw)
	   continue;
	cw->have_clip = 0;
     }
}

static int
ECompMgrDetermineOrder(EObj * const *lst, int num, EObj ** first,
		       EObj ** last, Desk * dsk, XserverRegion clip)
{
   EObj               *eo, *eo_prev, *eo_first;
   int                 i, stop;
   ECmWinInfo         *cw;

   D1printf("ECompMgrDetermineOrder %d\n", dsk->num);
   if (!lst)
      lst = EobjListStackGet(&num);
   if (clip == None)
     {
	ECompMgrDestroyClip();
	clip = Mode_compmgr.rgn_clip;
	ERegionEmpty(clip);
     }

   /* Determine overall paint order, top to bottom */
   stop = 0;
   eo_first = eo_prev = NULL;
   Mode_compmgr.ghosts = 0;

   for (i = 0; i < num; i++)
     {
	eo = lst[i];

	cw = eo->cmhook;

	if (!cw)
	   continue;

	if ((!eo->shown && !eo->fading) || eo->desk != dsk)
	   continue;

	if (EobjHasEmptyShape(eo))
	   continue;

	/* Region of shaped window in screen coordinates */
	if (!cw->have_shape)
	   ECompMgrWinSetShape(eo);

	/* Region of window in screen coordinates, including shadows */
	if (!cw->have_extents)
	   ECompMgrWinSetExtents(eo);

	D3printf(" - %#lx desk=%d shown=%d fading=%d fadeout=%d\n",
		 EobjGetXwin(eo), eo->desk->num, eo->shown, eo->fading,
		 cw->fadeout);

	if (eo->type == EOBJ_TYPE_DESK)
	  {
	     EObj               *eo1, *eo2;
	     Desk               *d = (Desk *) eo;

	     if (!d->viewable)
		continue;

#if USE_CLIP_RELATIVE_TO_DESK
	     ERegionTranslate(clip, -EoGetX(d), -EoGetY(d));
#endif
	     stop = ECompMgrDetermineOrder(lst, num, &eo1, &eo2, d, clip);
#if USE_CLIP_RELATIVE_TO_DESK
	     ERegionTranslate(clip, EoGetX(d), EoGetY(d));
#endif
	     if (eo1)
	       {
		  if (!eo_first)
		     eo_first = eo1;
		  if (eo_prev)
		     eo_prev->cmhook->next = eo1;
		  eo1->cmhook->prev = eo_prev;
		  eo_prev = eo2;
	       }

#if USE_BG_WIN_ON_ALL_DESKS	/* Only if using per desk bg overlay */
	     /* FIXME - We should break when the clip region becomes empty */
	     if (EobjGetX(eo) == 0 && EobjGetY(eo) == 0)
		stop = 1;
	     if (stop)
		break;
#endif
	  }

	if (cw->clip == None)
	   cw->clip = ERegionCreate();
	ERegionCopy(cw->clip, clip);
	cw->have_clip = 1;

	ECompMgrWinSetPicts(eo);

	D3printf(" - %#lx desk=%d shown=%d dam=%d pict=%#lx\n",
		 EobjGetXwin(eo), eo->desk->num, eo->shown, cw->damaged,
		 cw->picture);

#if 0				/* FIXME - Need this? */
	if (!cw->damaged)
	   continue;
#endif
	if (cw->picture == None && !eo->noredir)
	   continue;

	if (eo->ghost)
	  {
	     Mode_compmgr.ghosts = 1;
	     continue;
	  }

	D3printf
	   ("ECompMgrDetermineOrder hook in %d - %#lx desk=%d shown=%d\n",
	    dsk->num, EobjGetXwin(eo), eo->desk->num, eo->shown);

	if (!eo_first)
	   eo_first = eo;
	cw->prev = eo_prev;
	if (eo_prev)
	   eo_prev->cmhook->next = eo;
	eo_prev = eo;

	switch (cw->mode)
	  {
	  case WINDOW_UNREDIR:
	  case WINDOW_SOLID:
	     D3printf("-   clip %#lx %#lx %d,%d %dx%d: %s\n", EobjGetXwin(eo),
		      cw->clip, EobjGetX(eo), EobjGetY(eo), EobjGetW(eo),
		      EobjGetH(eo), EobjGetName(eo));
#if USE_CLIP_RELATIVE_TO_DESK
	     ERegionUnion(clip, cw->shape);
#else
	     ERegionUnionOffset(clip, EoGetX(dsk), EoGetY(dsk), cw->shape,
				rgn_tmp);
#endif
	     break;

	  default:
	     D3printf("- noclip %#lx %#lx %d,%d %dx%d: %s\n", EobjGetXwin(eo),
		      cw->clip, EobjGetX(eo), EobjGetY(eo), EobjGetW(eo),
		      EobjGetH(eo), EobjGetName(eo));
	     break;
	  }

#if !USE_BG_WIN_ON_ALL_DESKS	/* Not if using per desk bg overlay */
	/* FIXME - We should break when the clip region becomes empty */
	if (eo->type == EOBJ_TYPE_DESK &&
	    EobjGetX(eo) == 0 && EobjGetY(eo) == 0)
	   stop = 1;
	if (stop)
	   break;
#endif
     }
   if (eo_prev)
      eo_prev->cmhook->next = NULL;

   *first = eo_first;
   *last = eo_prev;

   Mode_compmgr.reorder = 0;
   return stop;
}

static              XserverRegion
ECompMgrRepaintObjSetClip(XserverRegion rgn, XserverRegion damage,
			  XserverRegion clip, int x, int y)
{
   ERegionCopy(rgn, damage);
#if USE_CLIP_RELATIVE_TO_DESK
   ERegionSubtractOffset(rgn, x, y, clip, rgn_tmp);
#else
   ERegionSubtract(rgn, clip);
   x = y = 0;
#endif
   return rgn;
}

static              XserverRegion
ECompMgrRepaintObjSetClip2(EObj * eo, XserverRegion clip, int x, int y)
{
#if 1
   /* This is only needed when source clipping in XRenderComposite() is broken.
    * otherwise it should be possible to set the source clip mask in
    * ECompMgrWinSetPicts() (when needed, i.e. source pict is pixmap). */
   if (WinIsShaped(EobjGetWin(eo)))
     {
	clip = ERegionCopy(rgn_tmp, clip);
	ERegionIntersectOffset(clip, x, y, eo->cmhook->shape, rgn_tmp2);
     }
#else
   eo = NULL;
   x = y = 0;
#endif
   return clip;
}

static void
ECompMgrRepaintObj(Picture pbuf, XserverRegion region, EObj * eo, int mode)
{
   static XserverRegion rgn_clip = None;
   ECmWinInfo         *cw;
   Desk               *dsk = eo->desk;
   int                 x, y;
   XserverRegion       clip, clip2;
   Picture             alpha;

   cw = eo->cmhook;

   if (rgn_clip == None)
      rgn_clip = ERegionCreate();

   x = EoGetX(dsk);
   y = EoGetY(dsk);

   if (mode == 0)
     {
	/* Painting opaque windows top down. */
#if 0
	if (ERegionIsEmpty(clip))
	  {
	     D2printf(" - Quit - repaint region is empty\n");
	     return;
	  }
#endif

	switch (cw->mode)
	  {
	  case WINDOW_UNREDIR:
	  case WINDOW_SOLID:
	     clip = ECompMgrRepaintObjSetClip(rgn_clip, region, cw->clip, x, y);
	     clip2 = ECompMgrRepaintObjSetClip2(eo, clip, x, y);
	     if (EDebug(EDBUG_TYPE_COMPMGR2))
		ECompMgrWinDumpInfo("ECompMgrRepaintObj solid", eo, clip, 0);
	     EPictureSetClip(pbuf, clip2);
	     XRenderComposite(disp, PictOpSrc, cw->picture, None, pbuf,
			      0, 0, 0, 0, x + cw->rcx, y + cw->rcy, cw->rcw,
			      cw->rch);
	     break;
	  }
     }
   else
     {
	/* Painting trans stuff bottom up. */

	switch (cw->mode)
	  {
	  default:
	     clip = None;
	     break;

	  case WINDOW_TRANS:
	  case WINDOW_ARGB:
	     clip = ECompMgrRepaintObjSetClip(rgn_clip, region, cw->clip, x, y);
	     clip2 = ECompMgrRepaintObjSetClip2(eo, clip, x, y);
	     if (EDebug(EDBUG_TYPE_COMPMGR2))
		ECompMgrWinDumpInfo("ECompMgrRepaintObj trans", eo, clip, 0);
	     EPictureSetClip(pbuf, clip2);
	     if (cw->opacity != OPAQUE && !cw->pict_alpha)
		cw->pict_alpha =
		   EPictureCreateSolid(Mode_compmgr.root, True,
				       OP32To8(cw->opacity),
				       Conf_compmgr.shadows.color);
	     XRenderComposite(disp, PictOpOver, cw->picture, cw->pict_alpha,
			      pbuf, 0, 0, 0, 0, x + cw->rcx, y + cw->rcy,
			      cw->rcw, cw->rch);
	     break;
	  }

#if ENABLE_SHADOWS
	if (!cw->has_shadow)
	   return;

	if (clip == None)
	   clip = ECompMgrRepaintObjSetClip(rgn_clip, region, cw->clip, x, y);
	ERegionSubtractOffset(clip, x, y, cw->shape, rgn_tmp);
	EPictureSetClip(pbuf, clip);

	switch (Mode_compmgr.shadow_mode)
	  {
	  case ECM_SHADOWS_SHARP:
	  case ECM_SHADOWS_ECHO:
	     if (cw->opacity != OPAQUE && !cw->shadow_alpha)
		cw->shadow_alpha =
		   EPictureCreateSolid(Mode_compmgr.root, True,
				       OP32To8(cw->opacity *
					       Mode_compmgr.opac_sharp),
				       Conf_compmgr.shadows.color);
	     alpha = cw->shadow_alpha ? cw->shadow_alpha : transBlackPicture;
	     if (Mode_compmgr.shadow_mode == ECM_SHADOWS_SHARP)
		XRenderComposite(disp, PictOpOver, alpha, cw->picture, pbuf,
				 0, 0, 0, 0,
				 x + cw->rcx + cw->shadow_dx,
				 y + cw->rcy + cw->shadow_dy,
				 cw->shadow_width, cw->shadow_height);
	     else
		XRenderComposite(disp, PictOpOver, cw->picture, alpha, pbuf,
				 0, 0, 0, 0,
				 x + cw->rcx + cw->shadow_dx,
				 y + cw->rcy + cw->shadow_dy,
				 cw->shadow_width, cw->shadow_height);
	     break;

	  case ECM_SHADOWS_BLURRED:
	     if (cw->shadow_pict == None)
		break;

	     if (cw->opacity != OPAQUE && !cw->pict_alpha)
		cw->pict_alpha =
		   EPictureCreateSolid(Mode_compmgr.root, True,
				       OP32To8(cw->opacity),
				       Conf_compmgr.shadows.color);
	     alpha = (cw->pict_alpha) ? cw->pict_alpha : transBlackPicture;
	     XRenderComposite(disp, PictOpOver, alpha, cw->shadow_pict, pbuf,
			      0, 0, 0, 0,
			      x + cw->rcx + cw->shadow_dx,
			      y + cw->rcy + cw->shadow_dy,
			      cw->shadow_width, cw->shadow_height);
	     break;
	  }
#endif
     }
}

static void
ECompMgrPaintGhosts(Picture pict, XserverRegion damage)
{
   EObj               *eo, *const *lst;
   int                 i, num;

   lst = EobjListStackGet(&num);
   for (i = 0; i < num; i++)
     {
	eo = lst[i];
	if (!eo->shown || !eo->ghost)
	   continue;

	switch (eo->cmhook->mode)
	  {
	  case WINDOW_UNREDIR:
	  case WINDOW_SOLID:
	     ECompMgrRepaintObj(pict, Mode_compmgr.rgn_screen, eo, 0);
	     break;
	  case WINDOW_TRANS:
	  case WINDOW_ARGB:
	     ECompMgrRepaintObj(pict, Mode_compmgr.rgn_screen, eo, 1);
	     break;
	  }

	/* Subtract window region from damage region */
	ERegionSubtract(damage, eo->cmhook->shape);
     }
   EPictureSetClip(pict, None);
}

void
ECompMgrRepaint(void)
{
   EObj               *eo;
   Picture             pbuf;
   Desk               *dsk = DeskGet(0);

   if (!Mode_compmgr.active || !Mode_compmgr.got_damage)
      return;

   ERegionIntersect(Mode_compmgr.damage, Mode_compmgr.rgn_screen);

   Dprintf("ECompMgrRepaint rootBuffer=%#lx rootPicture=%#lx\n",
	   rootBuffer, rootPicture);
   if (EDebug(EDBUG_TYPE_COMPMGR2))
      ERegionShow("damage", Mode_compmgr.damage, NULL);

   pbuf = rootBuffer;

   if (!dsk)
      return;

   /* Do paint order list linking */
   if (Mode_compmgr.reorder)
      ECompMgrDetermineOrder(NULL, 0, &Mode_compmgr.eo_first,
			     &Mode_compmgr.eo_last, dsk, None);

   /* Paint opaque windows top down */
   for (eo = Mode_compmgr.eo_first; eo; eo = eo->cmhook->next)
      ECompMgrRepaintObj(pbuf, Mode_compmgr.damage, eo, 0);

#if 0				/* FIXME - NoBg? */
   Picture             pict;

   if (EDebug(EDBUG_TYPE_COMPMGR2))
      ERegionShow("after opaque", region, NULL);

   /* Repaint background, clipped by damage region and opaque windows */
   pict = dsk->o.cmhook->picture;
   D1printf("ECompMgrRepaint desk picture=%#lx\n", pict);
   EPictureSetClip(pbuf, region);
   XRenderComposite(disp, PictOpSrc, pict, None, pbuf,
		    0, 0, 0, 0, 0, 0, WinGetW(VROOT), WinGetH(VROOT));
#endif

   /* Paint trans windows and shadows bottom up */
   for (eo = Mode_compmgr.eo_last; eo; eo = eo->cmhook->prev)
      ECompMgrRepaintObj(pbuf, Mode_compmgr.damage, eo, 1);

   /* Paint any ghost windows (adjusting damage region) */
   if (Mode_compmgr.ghosts)
      ECompMgrPaintGhosts(rootPicture, Mode_compmgr.damage);

   if (pbuf != rootPicture)
     {
	EPictureSetClip(pbuf, Mode_compmgr.damage);
	XRenderComposite(disp, PictOpSrc, pbuf, None, rootPicture,
			 0, 0, 0, 0, 0, 0, WinGetW(VROOT), WinGetH(VROOT));
     }

   Mode_compmgr.got_damage = 0;
}

static void
_ECompMgrIdler(void *data __UNUSED__)
{
   /* Do we get here on auto? */
   if (!Mode_compmgr.got_damage /* || Mode_compmgr.mode == ECM_MODE_AUTO */ )
      return;
   ECompMgrRepaint();
}

static void
ECompMgrRootBufferCreate(unsigned int w, unsigned int h)
{
   /* Root buffer picture and pixmap */
   rootBuffer = EPictureCreateBuffer(VROOT, w, h, &Mode_compmgr.pmap);

   /* Screen region */
   Mode_compmgr.rgn_screen = ERegionCreateRect(0, 0, w, h);

   /* Overall clip region used while recalculating window clip regions */
   Mode_compmgr.rgn_clip = ERegionCreate();
}

static void
ECompMgrRootBufferDestroy(void)
{
   PICTURE_DESTROY(rootBuffer);
   PIXMAP_DESTROY(Mode_compmgr.pmap);

   REGION_DESTROY(Mode_compmgr.rgn_screen);
   REGION_DESTROY(Mode_compmgr.rgn_clip);
}

Pixmap
ECompMgrGetRootBuffer(void)
{
   return (Mode_compmgr.pmap != None) ? Mode_compmgr.pmap : WinGetXwin(VROOT);
}

static void
ECompMgrRootConfigure(void *prm __UNUSED__, XEvent * ev)
{
   Dprintf("ECompMgrRootConfigure root\n");

   ECompMgrRootBufferDestroy();
   ECompMgrRootBufferCreate(ev->xconfigure.width, ev->xconfigure.height);
}

#if USE_DESK_EXPOSE		/* FIXME - Remove? */
static void
ECompMgrRootExpose(void *prm __UNUSED__, XEvent * ev)
{
   static XRectangle  *expose_rects = 0;
   static int          size_expose = 0;
   static int          n_expose = 0;
   int                 more = ev->xexpose.count + 1;

   if (ev->xexpose.window != WinGetXwin(VROOT))
      return;

   D1printf("ECompMgrRootExpose %d %d %d\n", n_expose, size_expose,
	    ev->xexpose.count);

   if (n_expose == size_expose)
     {
	expose_rects = EREALLOC(XRectangle, expose_rects, size_expose + more);
	size_expose += more;
     }
   expose_rects[n_expose].x = ev->xexpose.x;
   expose_rects[n_expose].y = ev->xexpose.y;
   expose_rects[n_expose].width = ev->xexpose.width;
   expose_rects[n_expose].height = ev->xexpose.height;
   n_expose++;
   if (ev->xexpose.count == 0)
     {
	XserverRegion       region;

	region = ERegionCreateFromRects(expose_rects, n_expose);

	ECompMgrDamageMerge(region);
	ERegionDestroy(region);
	n_expose = 0;
     }
}
#endif

#if ENABLE_SHADOWS
static void
ECompMgrShadowsInit(int mode, int cleanup)
{
   Mode_compmgr.shadow_mode = mode;

   Conf_compmgr.shadows.blur.opacity =
      OpacityFix(Conf_compmgr.shadows.blur.opacity, 100);
   Mode_compmgr.opac_blur = .01 * Conf_compmgr.shadows.blur.opacity;
   Conf_compmgr.shadows.sharp.opacity =
      OpacityFix(Conf_compmgr.shadows.sharp.opacity, 100);
   Mode_compmgr.opac_sharp = .01 * Conf_compmgr.shadows.sharp.opacity;

   Efree(gaussianMap);
   gaussianMap = NULL;

   if (mode != ECM_SHADOWS_OFF)
     {
	if (mode == ECM_SHADOWS_BLURRED)
	   transBlackPicture =
	      EPictureCreateSolid(Mode_compmgr.root, True, 255,
				  Conf_compmgr.shadows.color);
	else
	   transBlackPicture =
	      EPictureCreateSolid(Mode_compmgr.root, True,
				  OP32To8(Mode_compmgr.opac_sharp * OPAQUE),
				  Conf_compmgr.shadows.color);
     }
   else
     {
	PICTURE_DESTROY(transBlackPicture);
     }

   if (cleanup)
     {
	EObj               *const *lst;
	int                 i, num;

	lst = EobjListStackGet(&num);
	for (i = 0; i < num; i++)
	   ECompMgrWinInvalidate(lst[i], INV_SHADOW);
	_ECM_SET_SHADOW_CHANGED();	/* Force extents/shadow update */
     }
}
#else
#define ECompMgrShadowsInit(mode, cleanup)
#endif

int
ECompMgrIsActive(void)
{
   return Mode_compmgr.active;
}

static void
ECompMgrStart(void)
{
   EObj               *const *lst;
   int                 i, num;
   XRenderPictFormat  *pictfmt;
   XRenderPictureAttributes pa;

   if (Mode_compmgr.active || Conf_compmgr.mode == ECM_MODE_OFF)
      return;
   Conf_compmgr.enable = Mode_compmgr.active = 1;
   Mode_compmgr.mode = Conf_compmgr.mode;

   Conf_compmgr.override_redirect.opacity =
      OpacityFix(Conf_compmgr.override_redirect.opacity, 100);

   ECompMgrRootBufferCreate(WinGetW(VROOT), WinGetH(VROOT));

   Mode_compmgr.root = WinGetXwin(VROOT);
#if USE_COMPOSITE_OVERLAY_WINDOW
   if (Conf_compmgr.use_cow && !Mode.wm.window)
     {
	Mode_compmgr.cow = XCompositeGetOverlayWindow(disp, WinGetXwin(VROOT));
	if (Mode_compmgr.cow != None)
	  {
	     /* Ok, got the cow! */
	     Mode_compmgr.root = Mode_compmgr.cow;
	     /* It is possible to get it stacked below others?!? */
	     XRaiseWindow(disp, Mode_compmgr.cow);
	     /* Pass all input events through */
	     XShapeCombineRectangles(disp, Mode_compmgr.cow, ShapeInput, 0, 0,
				     NULL, 0, ShapeSet, Unsorted);
	     Dprintf("COW/CMroot=%#lx/%#lx\n",
		     Mode_compmgr.cow, Mode_compmgr.root);
	  }
     }
   else
     {
	Mode_compmgr.cow = None;
     }
#endif

   pa.subwindow_mode = IncludeInferiors;
   pictfmt = XRenderFindVisualFormat(disp, WinGetVisual(VROOT));
   rootPicture =
      XRenderCreatePicture(disp, Mode_compmgr.root, pictfmt, CPSubwindowMode,
			   &pa);

   ECompMgrShadowsInit(Conf_compmgr.shadows.mode, 0);

   EGrabServer();

   switch (Mode_compmgr.mode)
     {
     case ECM_MODE_ROOT:
	XCompositeRedirectSubwindows(disp, WinGetXwin(VROOT),
				     CompositeRedirectManual);
#if USE_DESK_EXPOSE		/* FIXME - Remove? */
	ESelectInputChange(VROOT, ExposureMask, 0);
#endif
	break;
     case ECM_MODE_WINDOW:
#if USE_DESK_EXPOSE		/* FIXME - Remove? */
	ESelectInputChange(VROOT, ExposureMask, 0);
#endif
	break;
     case ECM_MODE_AUTO:
	XCompositeRedirectSubwindows(disp, WinGetXwin(VROOT),
				     CompositeRedirectAutomatic);
	break;
     }

   Mode_compmgr.got_damage = 0;

   rgn_tmp = ERegionCreate();
   rgn_tmp2 = ERegionCreate();

   EventCallbackRegister(VROOT, ECompMgrHandleRootEvent, NULL);

   wm_cm_sel = SelectionAcquire("_NET_WM_CM_S", NULL, NULL);

   lst = EobjListStackGet(&num);
   for (i = 0; i < num; i++)
     {
	ECompMgrWinNew(lst[i]);
	if (lst[i]->shown)
	   ECompMgrWinMap(lst[i]);
     }

#if !USE_BG_WIN_ON_ALL_DESKS
   DesksBackgroundRefresh(NULL, DESK_BG_RECONFIGURE_ALL);
#endif
   _ECM_SET_CLIP_CHANGED();
   EUngrabServer();
   ESync(0);
}

static void
ECompMgrStop(void)
{
   EObj               *const *lst1, **lst;
   int                 i, num;

   if (!Mode_compmgr.active)
      return;
   Conf_compmgr.enable = Mode_compmgr.active = 0;

   EGrabServer();

   SelectionRelease(wm_cm_sel);
   wm_cm_sel = NULL;

   PICTURE_DESTROY(rootPicture);

   ECompMgrRootBufferDestroy();

   ECompMgrShadowsInit(ECM_SHADOWS_OFF, 0);

   lst1 = EobjListStackGet(&num);
   if (num > 0)
     {
	lst = EMALLOC(EObj *, num);
	if (lst)
	  {
	     memcpy(lst, lst1, num * sizeof(EObj *));
	     for (i = 0; i < num; i++)
	       {
		  if (lst[i]->type == EOBJ_TYPE_EXT)
		     EobjUnregister(lst[i]);	/* Modifies the object stack! */
		  else
		     ECompMgrWinDel(lst[i]);
	       }
	     Efree(lst);
	  }
     }

   Mode_compmgr.got_damage = 0;
   REGION_DESTROY(Mode_compmgr.damage);
   REGION_DESTROY(rgn_tmp);
   REGION_DESTROY(rgn_tmp2);

   if (Mode_compmgr.mode == ECM_MODE_ROOT)
      XCompositeUnredirectSubwindows(disp, WinGetXwin(VROOT),
				     CompositeRedirectManual);

   EventCallbackUnregister(VROOT, ECompMgrHandleRootEvent, NULL);

#if USE_COMPOSITE_OVERLAY_WINDOW
   if (Mode_compmgr.cow != None)
     {
	XCompositeReleaseOverlayWindow(disp, WinGetXwin(VROOT));
	Mode_compmgr.cow = None;
     }
#endif

#if !USE_BG_WIN_ON_ALL_DESKS
   DesksBackgroundRefresh(NULL, DESK_BG_RECONFIGURE_ALL);
#endif
   Mode.events.damage_count = 0;
   EUngrabServer();
   ESync(0);
}

void
ECompMgrConfigGet(cfg_composite * cfg)
{
   cfg->enable = Conf_compmgr.enable;
   cfg->shadow = Conf_compmgr.shadows.mode;
   cfg->fading = Conf_compmgr.fading.enable;
   cfg->opacity_focused = Conf.opacity.focused;
   cfg->opacity_unfocused = Conf.opacity.unfocused;
   cfg->opacity_override = Conf_compmgr.override_redirect.opacity;
   cfg->fade_speed = 100 - (Conf_compmgr.fading.time / 10);
}

void
ECompMgrConfigSet(const cfg_composite * cfg)
{
   if (Conf_compmgr.mode == ECM_MODE_OFF)
     {
	if (cfg->enable)
	   DialogOK("Enable Composite Error",
		    _("Cannot enable Composite Manager.\n"
		      "Use xdpyinfo to check that\n"
		      "Composite, Damage, Fixes, and Render\n"
		      "extensions are loaded."));
	return;
     }

   if (cfg->enable != Conf_compmgr.enable)
     {
	Conf_compmgr.enable = cfg->enable;
	Conf_compmgr.shadows.mode = cfg->shadow;
	if (cfg->enable)
	   ECompMgrStart();
	else
	   ECompMgrStop();
     }
   else
     {
	if (cfg->shadow != Conf_compmgr.shadows.mode)
	  {
	     Conf_compmgr.shadows.mode = cfg->shadow;
	     if (Conf_compmgr.enable)
	       {
		  ECompMgrShadowsInit(Conf_compmgr.shadows.mode, 1);
		  ECompMgrDamageAll();
	       }
	  }
     }

   Conf_compmgr.fading.enable = cfg->fading;
   Conf_compmgr.fading.time = (100 - cfg->fade_speed) * 10;

   Conf.opacity.focused = cfg->opacity_focused;
   Conf.opacity.unfocused = cfg->opacity_unfocused;
   Conf_compmgr.override_redirect.opacity = cfg->opacity_override;

   EobjsOpacityUpdate(Conf_compmgr.override_redirect.opacity);

   autosave();
}

/*
 * Event handlers
 */
#define USE_WINDOW_EVENTS 0

static void
ECompMgrHandleWindowEvent(Win win __UNUSED__, XEvent * ev, void *prm)
{
   EObj               *eo = (EObj *) prm;

   D2printf("ECompMgrHandleWindowEvent: type=%d\n", ev->type);

   switch (ev->type)
     {
#if USE_WINDOW_EVENTS
     case ConfigureNotify:
	ECompMgrWinConfigure(eo, ev);
	break;

     case MapNotify:
	ECompMgrWinMap(eo);
	break;
     case UnmapNotify:
	if (eo->type == EOBJ_TYPE_EXT && eo->cmhook)
	  {
	     ECompMgrWinUnmap(eo);
	     eo->shown = 0;
	  }
	break;

     case CirculateNotify:
	ECompMgrWinCirculate(eo, ev);
	break;
#endif

#if USE_DESK_VISIBILITY
     case VisibilityNotify:
	ECompMgrDeskVisibility(eo, ev);
	break;
#endif

     case EX_EVENT_SHAPE_NOTIFY:
	/* Only EOBJ_TYPE_EXT window shape changes will go here */
	ECompMgrWinChangeShape(eo);
	break;

     case EX_EVENT_DAMAGE_NOTIFY:
	ECompMgrWinDamage(eo, ev);
	break;
     }
}

static void
ECompMgrHandleRootEvent(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Window              xwin;
   EObj               *eo;

   D2printf("ECompMgrHandleRootEvent: type=%d\n", ev->type);

   switch (ev->type)
     {
     case CreateNotify:
	xwin = ev->xcreatewindow.window;
      case_CreateNotify:
	if (Conf_compmgr.override_redirect.mode != ECM_OR_ON_CREATE)
	   break;
	eo = EobjListStackFind(xwin);
	if (!eo)
	   EobjRegister(xwin, EOBJ_TYPE_EXT);
	break;

     case DestroyNotify:
	xwin = ev->xdestroywindow.window;
      case_DestroyNotify:
	eo = EobjListStackFind(xwin);
	if (eo && eo->type == EOBJ_TYPE_EXT)
	  {
	     if (ev->type == DestroyNotify)
		eo->gone = 1;
	     EobjUnregister(eo);
	  }
	break;

     case ReparentNotify:
     case EX_EVENT_REPARENT_GONE:
	xwin = ev->xreparent.window;
	if (ev->xreparent.parent == WinGetXwin(VROOT))
	   goto case_CreateNotify;
	else
	   goto case_DestroyNotify;

     case ConfigureNotify:
	if (ev->xconfigure.window == WinGetXwin(VROOT))
	  {
	     ECompMgrRootConfigure(prm, ev);
	  }
	else
	  {
	     eo = EobjListStackFind(ev->xconfigure.window);
	     if (eo && eo->type == EOBJ_TYPE_EXT && eo->cmhook)
	       {
		  ECompMgrWinConfigure(eo, ev);
	       }
	  }
	break;

     case MapNotify:
#if USE_COMPOSITE_OVERLAY_WINDOW
	if (ev->xmap.window == Mode_compmgr.cow)
	   break;
#endif
	eo = EobjListStackFind(ev->xmap.window);
	if (!eo)
	   eo = EobjRegister(ev->xmap.window, EOBJ_TYPE_EXT);
	if (eo && eo->type == EOBJ_TYPE_EXT && eo->cmhook)
	  {
	     eo->shown = 1;
	     EobjListStackRaise(eo, 0);
	     ECompMgrWinMap(eo);
	  }
	break;

     case UnmapNotify:
     case EX_EVENT_UNMAP_GONE:
	eo = EobjListStackFind(ev->xunmap.window);
	if (eo && eo->type == EOBJ_TYPE_EXT && eo->cmhook)
	  {
	     if (ev->type == EX_EVENT_UNMAP_GONE)
		eo->gone = 1;
#if 0
	     /* No. Unredirection seems to cause map/unmap => loop */
	     if (Conf_compmgr.override_redirect.mode == ECM_OR_ON_MAPUNMAP)
	       {
		  EobjUnregister(eo);
	       }
	     else
#endif
	       {
		  ECompMgrWinUnmap(eo);
		  eo->shown = 0;
	       }
	  }
	break;

     case CirculateNotify:
	eo = EobjListStackFind(ev->xcirculate.window);
	if (eo && eo->cmhook)
	   ECompMgrWinCirculate(eo, ev);
	break;

#if USE_DESK_EXPOSE		/* FIXME - Remove? */
     case Expose:
	if (Mode_compmgr.shadow_mode != ECM_SHADOWS_OFF)
	   ECompMgrRootExpose(prm, ev);
	break;
#endif
     }
}

/*
 * Module functions
 */

static void
ECompMgrInit(void)
{
   if (!XEXT_AVAILABLE(XEXT_CM_ALL))
     {
	Conf_compmgr.mode = ECM_MODE_OFF;
	goto done;
     }

   Mode_compmgr.use_pixmap = Conf_compmgr.use_name_pixmap;

#if 0				/* TBD - Force ECM_MODE_ROOT at startup */
   if (Conf_compmgr.mode == ECM_MODE_OFF)
#endif
      Conf_compmgr.mode = ECM_MODE_ROOT;

 done:
   if (Conf_compmgr.mode == ECM_MODE_OFF)
      Conf_compmgr.enable = 0;
   Dprintf("ECompMgrInit: enable=%d mode=%d\n", Conf_compmgr.enable,
	   Conf_compmgr.mode);
}

static void
ECompMgrSighan(int sig, void *prm __UNUSED__)
{
   if (sig != ESIGNAL_INIT && Mode_compmgr.mode == ECM_MODE_OFF)
      return;

   switch (sig)
     {
     case ESIGNAL_INIT:
	ECompMgrInit();
	if (Conf_compmgr.enable)
	   ECompMgrStart();
	IdlerAdd(_ECompMgrIdler, NULL);
	break;
     }
}

static void
CompMgrIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	IpcPrintf("CompMgr - on=%d\n", Mode_compmgr.active);
     }
   else if (!strcmp(cmd, "start"))
     {
	ECompMgrStart();
	autosave();
     }
   else if (!strcmp(cmd, "stop"))
     {
	ECompMgrStop();
	autosave();
     }
   else if (!strncmp(cmd, "list", 2))
     {
	/* TBD */
     }
   else if (!strncmp(cmd, "oi", 2))
     {
	Window              win;
	EObj               *eo;

	win = None;
	sscanf(prm, "%lx", &win);
	eo = EobjListStackFind(win);
	if (eo)
	   ECompMgrWinDumpInfo("EObj", eo, None, 1);
     }
}

static const IpcItem CompMgrIpcArray[] = {
   {
    CompMgrIpc,
    "compmgr", "cm",
    "Composite manager functions",
    "  cm ?                     Show info\n"
    "  cm start                 Start composite manager\n"
    "  cm stop                  Stop composite manager\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(CompMgrIpcArray)/sizeof(IpcItem))

static const CfgItem CompMgrCfgItems[] = {
   CFG_ITEM_BOOL(Conf_compmgr, enable, 0),
   CFG_ITEM_INT(Conf_compmgr, mode, ECM_MODE_ROOT),
   CFG_ITEM_INT(Conf_compmgr, shadows.mode, 0),
   CFG_ITEM_INT(Conf_compmgr, shadows.offset_x, 3),
   CFG_ITEM_INT(Conf_compmgr, shadows.offset_y, 5),
   CFG_ITEM_INT(Conf_compmgr, shadows.blur.radius, 5),
   CFG_ITEM_INT(Conf_compmgr, shadows.blur.opacity, 75),
   CFG_ITEM_INT(Conf_compmgr, shadows.sharp.opacity, 30),
   CFG_ITEM_HEX(Conf_compmgr, shadows.color, 0),
   CFG_ITEM_BOOL(Conf_compmgr, use_name_pixmap, 0),
#if USE_COMPOSITE_OVERLAY_WINDOW
   CFG_ITEM_BOOL(Conf_compmgr, use_cow, 1),
#endif
   CFG_ITEM_BOOL(Conf_compmgr, fading.enable, 1),
   CFG_ITEM_INT(Conf_compmgr, fading.time, 200),
   CFG_ITEM_INT(Conf_compmgr, override_redirect.mode, 1),
   CFG_ITEM_INT(Conf_compmgr, override_redirect.opacity, 90),
};
#define N_CFG_ITEMS (sizeof(CompMgrCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModCompMgr;

const EModule       ModCompMgr = {
   "compmgr", "cm",
   ECompMgrSighan,
   {N_IPC_FUNCS, CompMgrIpcArray},
   {N_CFG_ITEMS, CompMgrCfgItems}
};

#endif /* USE_COMPOSITE */

/*
 * $Id: xcompmgr.c,v 1.26 2004/08/14 21:39:51 keithp Exp $
 *
 * Copyright © 2003 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/*
 * Modified by Matthew Hawn. I don't know what to say here so follow what it 
 * says above. Not that I can really do anything about it
 */

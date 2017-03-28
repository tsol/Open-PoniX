/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
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
#include <time.h>
#include "E.h"
#include "backgrounds.h"
#include "desktops.h"
#include "dialog.h"
#include "e16-ecore_list.h"
#include "eimage.h"
#include "emodule.h"
#include "file.h"
#include "iclass.h"
#include "settings.h"
#include "tclass.h"
#include "timers.h"
#include "xwin.h"

typedef struct {
   char               *file;
   EImage             *im;
   char                keep_aspect;
   int                 xjust, yjust;
   int                 xperc, yperc;
} BgPart;

struct _background {
   char               *name;
   Pixmap              pmap;
   time_t              last_viewed;
   unsigned int        bg_solid;
   char                bg_tile;
   BgPart              bg;
   BgPart              top;
   char                external;
   char                keepim;
   char                referenced;
   unsigned int        ref_count;
   unsigned int        seq_no;
};

static Ecore_List  *bg_list = NULL;
static Timer       *bg_timer = NULL;
static unsigned int bg_seq_no = 0;

#define N_BG_ASSIGNED 32
static Background  *bg_assigned[N_BG_ASSIGNED];

static char        *
_BackgroundGetFile(char **ptr)
{
   char               *path = *ptr;

   if (isabspath(path))
      goto done;

   path = ThemeFileFind(path, FILE_TYPE_BACKGROUND);
   if (!path)
      goto done;
   Efree(*ptr);
   *ptr = path;
 done:
   return path;
}

static char        *
_BackgroundGetBgFile(Background * bg)
{
   if (!bg || !bg->bg.file)
      return NULL;
   return _BackgroundGetFile(&bg->bg.file);
}

static char        *
_BackgroundGetFgFile(Background * bg)
{
   if (!bg || !bg->top.file)
      return NULL;
   return _BackgroundGetFile(&bg->top.file);
}

char               *
BackgroundGetUniqueString(Background * bg)
{
   static const char  *chmap =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_";
   char                s[256], *f;
   int                 r, g, b;
   int                 n1, n2, n3, n4, n5, f1, f2, f3, f4, f5, f6;

   COLOR32_TO_RGB(bg->bg_solid, r, g, b);
   n1 = (r << 24) | (g << 16) | (b << 8) | (bg->bg_tile << 7)
      | (bg->bg.keep_aspect << 6) | (bg->top.keep_aspect << 5);
   n2 = (bg->bg.xjust << 16) | (bg->bg.yjust);
   n3 = (bg->bg.xperc << 16) | (bg->bg.yperc);
   n4 = (bg->top.xjust << 16) | (bg->top.yjust);
   n5 = (bg->top.xperc << 16) | (bg->top.yperc);
   f1 = 0;
   f2 = 0;
   f3 = 0;
   f4 = 0;
   f5 = 0;
   f6 = 0;

   f = _BackgroundGetBgFile(bg);
   if (f)
     {
	f1 = fileinode(f);
	f2 = filedev(f);
	f3 = (int)moddate(f);
     }
   f = _BackgroundGetFgFile(bg);
   if (f)
     {
	f4 = fileinode(f);
	f5 = filedev(f);
	f6 = (int)moddate(f);
     }

   Esnprintf(s, sizeof(s),
	     "%c%c%c%c%c%c" "%c%c%c%c%c%c" "%c%c%c%c%c%c" "%c%c%c%c%c%c"
	     "%c%c%c%c%c%c" "%c%c%c%c%c%c" "%c%c%c%c%c%c" "%c%c%c%c%c%c"
	     "%c%c%c%c%c%c" "%c%c%c%c%c%c" "%c%c%c%c%c%c",
	     chmap[(n1 >> 0) & 0x3f], chmap[(n1 >> 6) & 0x3f],
	     chmap[(n1 >> 12) & 0x3f], chmap[(n1 >> 18) & 0x3f],
	     chmap[(n1 >> 24) & 0x3f], chmap[(n1 >> 28) & 0x3f],
	     chmap[(n2 >> 0) & 0x3f], chmap[(n2 >> 6) & 0x3f],
	     chmap[(n2 >> 12) & 0x3f], chmap[(n2 >> 18) & 0x3f],
	     chmap[(n2 >> 24) & 0x3f], chmap[(n2 >> 28) & 0x3f],
	     chmap[(n3 >> 0) & 0x3f], chmap[(n3 >> 6) & 0x3f],
	     chmap[(n3 >> 12) & 0x3f], chmap[(n3 >> 18) & 0x3f],
	     chmap[(n3 >> 24) & 0x3f], chmap[(n3 >> 28) & 0x3f],
	     chmap[(n4 >> 0) & 0x3f], chmap[(n4 >> 6) & 0x3f],
	     chmap[(n4 >> 12) & 0x3f], chmap[(n4 >> 18) & 0x3f],
	     chmap[(n4 >> 24) & 0x3f], chmap[(n4 >> 28) & 0x3f],
	     chmap[(n5 >> 0) & 0x3f], chmap[(n5 >> 6) & 0x3f],
	     chmap[(n5 >> 12) & 0x3f], chmap[(n5 >> 18) & 0x3f],
	     chmap[(n5 >> 24) & 0x3f], chmap[(n5 >> 28) & 0x3f],
	     chmap[(f1 >> 0) & 0x3f], chmap[(f1 >> 6) & 0x3f],
	     chmap[(f1 >> 12) & 0x3f], chmap[(f1 >> 18) & 0x3f],
	     chmap[(f1 >> 24) & 0x3f], chmap[(f1 >> 28) & 0x3f],
	     chmap[(f2 >> 0) & 0x3f], chmap[(f2 >> 6) & 0x3f],
	     chmap[(f2 >> 12) & 0x3f], chmap[(f2 >> 18) & 0x3f],
	     chmap[(f2 >> 24) & 0x3f], chmap[(f2 >> 28) & 0x3f],
	     chmap[(f3 >> 0) & 0x3f], chmap[(f3 >> 6) & 0x3f],
	     chmap[(f3 >> 12) & 0x3f], chmap[(f3 >> 18) & 0x3f],
	     chmap[(f3 >> 24) & 0x3f], chmap[(f3 >> 28) & 0x3f],
	     chmap[(f4 >> 0) & 0x3f], chmap[(f4 >> 6) & 0x3f],
	     chmap[(f4 >> 12) & 0x3f], chmap[(f4 >> 18) & 0x3f],
	     chmap[(f4 >> 24) & 0x3f], chmap[(f4 >> 28) & 0x3f],
	     chmap[(f5 >> 0) & 0x3f], chmap[(f5 >> 6) & 0x3f],
	     chmap[(f5 >> 12) & 0x3f], chmap[(f5 >> 18) & 0x3f],
	     chmap[(f5 >> 24) & 0x3f], chmap[(f5 >> 28) & 0x3f],
	     chmap[(f6 >> 0) & 0x3f], chmap[(f6 >> 6) & 0x3f],
	     chmap[(f6 >> 12) & 0x3f], chmap[(f6 >> 18) & 0x3f],
	     chmap[(f6 >> 24) & 0x3f], chmap[(f6 >> 28) & 0x3f]);
   return Estrdup(s);
}

void
BackgroundPixmapSet(Background * bg, Pixmap pmap)
{
   if (bg->pmap != None && bg->pmap != pmap)
      Eprintf("*** BackgroundPixmapSet %s: pmap was set %#lx/%#lx\n",
	      bg->name, bg->pmap, pmap);
   bg->pmap = pmap;
}

static void
BackgroundPixmapFree(Background * bg)
{
   if (bg->pmap)
     {
	EImagePixmapsFree(bg->pmap, None);
	bg->pmap = None;
     }
}

static void
BackgroundImagesFree(Background * bg)
{
   if (bg->bg.im)
     {
	EImageFree(bg->bg.im);
	bg->bg.im = NULL;
     }
   if (bg->top.im)
     {
	EImageFree(bg->top.im);
	bg->top.im = NULL;
     }
}

#if ENABLE_DIALOGS
static void
BackgroundImagesKeep(Background * bg, int onoff)
{
   if (onoff)
     {
	bg->keepim = 1;
     }
   else
     {
	bg->keepim = 0;
	BackgroundImagesFree(bg);
     }
}
#endif /* ENABLE_DIALOGS */

static void
BackgroundFilesRemove(Background * bg)
{
   Efree(bg->bg.file);
   bg->bg.file = NULL;

   Efree(bg->top.file);
   bg->top.file = NULL;

   BackgroundImagesFree(bg);

   bg->keepim = 0;
}

static int
BackgroundDestroy(Background * bg)
{
   if (!bg)
      return -1;
#if 0
   Eprintf("%s: %s\n", __func__, bg->name);
#endif
   if (bg->ref_count > 0)
     {
	DialogOK("Background Error!", _("%u references remain"), bg->ref_count);
	return -1;
     }

   ecore_list_node_remove(bg_list, bg);

   BackgroundFilesRemove(bg);
   BackgroundPixmapFree(bg);

   Efree(bg->name);

   Efree(bg);

   return 0;
}

#if ENABLE_DIALOGS
static void
BackgroundDelete(Background * bg)
{
   char               *f;

   if (!bg)
      return;
#if 0
   Eprintf("%s: %s\n", __func__, bg->name);
#endif
   if (bg->ref_count > 0)
      return;

   /* And delete the actual image files */
   f = _BackgroundGetBgFile(bg);
   if (f)
      E_rm(f);
   f = _BackgroundGetFgFile(bg);
   if (f)
      E_rm(f);

   BackgroundDestroy(bg);
}
#endif /* ENABLE_DIALOGS */

static Background  *
BackgroundCreate(const char *name, unsigned int solid, const char *bgn,
		 char tile, char keep_aspect, int xjust, int yjust, int xperc,
		 int yperc, const char *top, char tkeep_aspect, int txjust,
		 int tyjust, int txperc, int typerc)
{
   Background         *bg;

   bg = ECALLOC(Background, 1);
   if (!bg)
      return NULL;

   if (!name)
     {
	name = "NONE";
	bg->external = 1;
     }
   bg->name = Estrdup(name);

   COLOR32_FROM_RGB(bg->bg_solid, 160, 160, 160);
   bg->bg_solid = solid;
   if (bgn)
      bg->bg.file = Estrdup(bgn);
   bg->bg_tile = tile;
   bg->bg.keep_aspect = keep_aspect;
   bg->bg.xjust = xjust;
   bg->bg.yjust = yjust;
   bg->bg.xperc = xperc;
   bg->bg.yperc = yperc;

   if (top)
      bg->top.file = Estrdup(top);
   bg->top.keep_aspect = tkeep_aspect;
   bg->top.xjust = txjust;
   bg->top.yjust = tyjust;
   bg->top.xperc = txperc;
   bg->top.yperc = typerc;

   bg->seq_no = ++bg_seq_no;

   if (!bg_list)
      bg_list = ecore_list_new();
   ecore_list_prepend(bg_list, bg);

   return bg;
}

static int
BackgroundCmp(const Background * bg, const Background * bgx)
{
   if (*bgx->name != '.')	/* Discard only generated backgrounds */
      return 1;

   if (bg->bg.file && bgx->bg.file)
     {
	if ((strcmp(bg->bg.file, bgx->bg.file)) ||
	    (bg->bg.keep_aspect != bgx->bg.keep_aspect) ||
	    (bg->bg.xjust != bgx->bg.xjust) ||
	    (bg->bg.yjust != bgx->bg.yjust) ||
	    (bg->bg.xperc != bgx->bg.xperc) || (bg->bg.yperc != bgx->bg.yperc))
	   return 1;
     }
   else if (bg->bg.file || bgx->bg.file)
      return 1;

   if (bg->top.file && bgx->top.file)
     {
	if ((strcmp(bg->top.file, bgx->top.file)) ||
	    (bg->top.keep_aspect != bgx->top.keep_aspect) ||
	    (bg->top.xjust != bgx->top.xjust) ||
	    (bg->top.yjust != bgx->top.yjust) ||
	    (bg->top.xperc != bgx->top.xperc) ||
	    (bg->top.yperc != bgx->top.yperc))
	   return 1;
     }
   else if (bg->top.file || bgx->top.file)
      return 1;

   if (bg->bg_solid != bgx->bg_solid)
      return 1;
   if (bg->bg_tile != bgx->bg_tile)
      return 1;

   return 0;
}

static int
_BackgroundMatchName(const void *data, const void *match)
{
   return strcmp(((const Background *)data)->name, (const char *)match);
}

Background         *
BackgroundFind(const char *name)
{
   return (Background *) ecore_list_find(bg_list, _BackgroundMatchName, name);
}

static Background  *
BackgroundCheck(Background * bg)
{
   return (Background *) ecore_list_goto(bg_list, bg);
}

void
BackgroundDestroyByName(const char *name)
{
   BackgroundDestroy(BackgroundFind(name));
}

static void
BackgroundInvalidate(Background * bg, int refresh)
{
   BackgroundPixmapFree(bg);
   bg->seq_no = ++bg_seq_no;
   if (bg->ref_count && refresh)
      DesksBackgroundRefresh(bg, DESK_BG_REFRESH);
}

static int
BackgroundModify(Background * bg, unsigned int solid, const char *bgn,
		 char tile, char keep_aspect, int xjust, int yjust, int xperc,
		 int yperc, const char *top, char tkeep_aspect, int txjust,
		 int tyjust, int txperc, int typerc)
{
   int                 updated = 0;

   if (solid != bg->bg_solid)
      updated = 1;
   bg->bg_solid = solid;

   if ((bg->bg.file) && (bgn))
     {
	if (strcmp(bg->bg.file, bgn))
	   updated = 1;
     }
   else
      updated = 1;
   Efree(bg->bg.file);
   bg->bg.file = (bgn && bgn[0]) ? Estrdup(bgn) : NULL;
   if ((int)tile != bg->bg_tile)
      updated = 1;
   if ((int)keep_aspect != bg->bg.keep_aspect)
      updated = 1;
   if (xjust != bg->bg.xjust)
      updated = 1;
   if (yjust != bg->bg.yjust)
      updated = 1;
   if (xperc != bg->bg.xperc)
      updated = 1;
   if (yperc != bg->bg.yperc)
      updated = 1;
   bg->bg_tile = (char)tile;
   bg->bg.keep_aspect = (char)keep_aspect;
   bg->bg.xjust = xjust;
   bg->bg.yjust = yjust;
   bg->bg.xperc = xperc;
   bg->bg.yperc = yperc;

   if ((bg->top.file) && (top))
     {
	if (strcmp(bg->top.file, top))
	   updated = 1;
     }
   else
      updated = 1;
   Efree(bg->top.file);
   bg->top.file = (top && top[0]) ? Estrdup(top) : NULL;
   if ((int)tkeep_aspect != bg->top.keep_aspect)
      updated = 1;
   if (txjust != bg->top.xjust)
      updated = 1;
   if (tyjust != bg->top.yjust)
      updated = 1;
   if (txperc != bg->top.xperc)
      updated = 1;
   if (typerc != bg->top.yperc)
      updated = 1;
   bg->top.keep_aspect = (char)tkeep_aspect;
   bg->top.xjust = txjust;
   bg->top.yjust = tyjust;
   bg->top.xperc = txperc;
   bg->top.yperc = typerc;

   if (updated)
      BackgroundInvalidate(bg, 1);

   return updated;
}

static void
BgFindImageSize(BgPart * bgp, unsigned int rw, unsigned int rh,
		unsigned int *pw, unsigned int *ph)
{
   int                 w, h, iw, ih;

   EImageGetSize(bgp->im, &iw, &ih);

#if 0				/* FIXME - Remove? */
   if (bgp->keep_aspect)
      bgp->xperc = bgp->yperc;
#endif

   if (bgp->xperc > 0)
      w = (rw * bgp->xperc) >> 10;
   else
      w = (iw * rw) / WinGetW(VROOT);

   if (bgp->yperc > 0)
      h = (rh * bgp->yperc) >> 10;
   else
      h = (ih * rh) / WinGetH(VROOT);

   if (w <= 0)
      w = 1;
   if (h <= 0)
      h = 1;

   if (bgp->keep_aspect)
     {
	if (bgp->yperc <= 0)
	  {
	     if (((w << 10) / h) != ((iw << 10) / ih))
		h = ((w * ih) / iw);
	  }
	else
	  {
	     if (((h << 10) / w) != ((ih << 10) / iw))
		w = ((h * iw) / ih);
	  }
     }

   *pw = (unsigned int)w;
   *ph = (unsigned int)h;
}

static              Pixmap
BackgroundCreatePixmap(Win win, unsigned int w, unsigned int h)
{
   Pixmap              pmap;

   /*
    * Stupid hack to avoid that a new root pixmap has the same ID as the now
    * invalid one from a previous session.
    */
   pmap = ECreatePixmap(win, w, h, 0);
   if (win == RROOT && pmap == Mode.root.ext_pmap)
     {
	EFreePixmap(pmap);
	pmap = ECreatePixmap(win, w, h, 0);
	Mode.root.ext_pmap = None;
	Mode.root.ext_pmap_valid = 0;
     }
   return pmap;
}

void
BackgroundRealize(Background * bg, Win win, Drawable draw, unsigned int rw,
		  unsigned int rh, int is_win, Pixmap * ppmap,
		  unsigned int *ppixel)
{
   Pixmap              pmap;
   int                 x, y, ww, hh;
   unsigned int        w, h;
   char               *file, hasbg, hasfg;
   EImage             *im;

   if (!bg->bg.im)
     {
	file = _BackgroundGetBgFile(bg);
	if (file)
	   bg->bg.im = EImageLoad(file);
     }

   if (!bg->top.im)
     {
	file = _BackgroundGetFgFile(bg);
	if (file)
	   bg->top.im = EImageLoad(bg->top.file);
     }

   if (!draw)
      draw = WinGetXwin(win);

   hasbg = ! !bg->bg.im;
   hasfg = ! !bg->top.im;

   if (!hasbg && !hasfg)
     {
	unsigned int        pixel;

	/* Solid color only */
	pixel = EAllocColor(WinGetCmap(VROOT), bg->bg_solid);

	if (!is_win)
	  {
	     GC                  gc;

	     gc = EXCreateGC(draw, 0, NULL);
	     XSetClipMask(disp, gc, 0);
	     XSetFillStyle(disp, gc, FillSolid);
	     XSetForeground(disp, gc, pixel);
	     XFillRectangle(disp, draw, gc, 0, 0, rw, rh);
	     EXFreeGC(gc);
	  }
	if (ppmap)
	   *ppmap = None;
	if (ppixel)
	   *ppixel = pixel;
	return;
     }

   /* Has either bg or fg image */

   w = h = x = y = 0;

   if (hasbg)
     {
	BgFindImageSize(&(bg->bg), rw, rh, &w, &h);
	x = ((int)(rw - w) * bg->bg.xjust) >> 10;
	y = ((int)(rh - h) * bg->bg.yjust) >> 10;
     }

   if (is_win && hasbg && !hasfg && x == 0 && y == 0 &&
       ((w == rw && h == rh) || (bg->bg_tile && !TransparencyEnabled())))
     {
	/* Window, no fg, no offset, and scale to 100%, or tiled, no trans */
	pmap = BackgroundCreatePixmap(win, w, h);
	EImageRenderOnDrawable(bg->bg.im, win, pmap, EIMAGE_ANTI_ALIAS,
			       0, 0, w, h);

#if 0				/* FIXME - Remove? */
	if (x == 0 && y == 0)	/* Hmmm. Always true. */
	  {
	     ESetWindowBackgroundPixmap(draw, pmap);
	  }
	else
	  {
	     gc = EXCreateGC(draw, 0, NULL);
	     XSetTile(disp, gc, pmap);
	     XSetTSOrigin(disp, gc, x, y);
	     XSetFillStyle(disp, gc, FillTiled);
	     XFillRectangle(disp, draw, gc, 0, 0, rw, rh);
	     EXFreeGC(gc);
	  }
#endif
	goto done;
     }

   /* The rest that require some more work */
   if (is_win)
      pmap = BackgroundCreatePixmap(win, rw, rh);
   else
      pmap = draw;

   if (hasbg && !hasfg && x == 0 && y == 0 && w == rw && h == rh)
     {
	im = bg->bg.im;
     }
   else
     {
	/* Create full size image */
	im = EImageCreate(rw, rh);
	EImageSetHasAlpha(im, 0);
	if (!hasbg || !bg->bg_tile)
	  {
	     /* Fill solid */
	     EImageFill(im, 0, 0, rw, rh, bg->bg_solid);
	  }
	if (hasbg)
	  {
	     if (bg->bg_tile)
	       {
		  EImageTile(im, bg->bg.im, 0, w, h, 0, 0, rw, rh, x, y);
	       }
	     else
	       {
		  EImageGetSize(bg->bg.im, &ww, &hh);
		  EImageBlend(im, bg->bg.im, EIMAGE_ANTI_ALIAS, 0, 0, ww, hh,
			      x, y, w, h, 1);
	       }
	  }
     }

   if (hasfg)
     {
	EImageGetSize(bg->top.im, &ww, &hh);

	BgFindImageSize(&(bg->top), rw, rh, &w, &h);
	x = ((rw - w) * bg->top.xjust) >> 10;
	y = ((rh - h) * bg->top.yjust) >> 10;

	EImageBlend(im, bg->top.im, EIMAGE_BLEND | EIMAGE_ANTI_ALIAS,
		    0, 0, ww, hh, x, y, w, h, 0);
     }

   EImageRenderOnDrawable(im, win, pmap, EIMAGE_ANTI_ALIAS, 0, 0, rw, rh);
   if (im != bg->bg.im)
      EImageFree(im);

 done:
   if (!bg->keepim)
      BackgroundImagesFree(bg);

   if (ppmap)
      *ppmap = pmap;
   if (ppixel)
      *ppixel = 0;
}

void
BackgroundApplyPmap(Background * bg, Win win, Drawable draw,
		    unsigned int w, unsigned int h)
{
   BackgroundRealize(bg, win, draw, w, h, 0, NULL, NULL);
}

static void
BackgroundApplyWin(Background * bg, Win win)
{
   int                 w, h;
   Pixmap              pmap;
   unsigned int        pixel;

   if (!EGetGeometry(win, NULL, NULL, NULL, &w, &h, NULL, NULL))
      return;

   BackgroundRealize(bg, win, None, w, h, 1, &pmap, &pixel);
   if (pmap != None)
     {
	ESetWindowBackgroundPixmap(win, pmap);
	EImagePixmapsFree(pmap, None);
     }
   else
     {
	ESetWindowBackground(win, pixel);
     }
   EClearWindow(win);
}

/*
 * Apply a background to window.
 * The BG pixmap is stored in bg->pmap.
 */
void
BackgroundSet(Background * bg, Win win, unsigned int w, unsigned int h)
{
   Pixmap              pmap = None;
   unsigned int        pixel = 0;

   if (bg->pmap)
      pmap = bg->pmap;
   else
      BackgroundRealize(bg, win, None, w, h, 1, &pmap, &pixel);

   bg->pmap = pmap;
   if (pmap != None)
      ESetWindowBackgroundPixmap(win, pmap);
   else
      ESetWindowBackground(win, pixel);
   EClearWindow(win);
}

Background         *
BrackgroundCreateFromImage(const char *bgid, const char *file,
			   char *thumb, int thlen)
{
   Background         *bg;
   EImage             *im, *im2;
   unsigned int        color;
   char                tile = 1, keep_asp = 0;
   int                 width, height;
   int                 scalex = 0, scaley = 0;
   int                 scr_asp, im_asp;
   int                 w2, h2;
   int                 maxw = Mode.backgrounds.mini_w;
   int                 maxh = Mode.backgrounds.mini_h;
   int                 justx = 512, justy = 512;

   bg = BackgroundFind(bgid);

   if (thumb)
     {
	Esnprintf(thumb, thlen, "%s/cached/img/%s.png", EDirUserCache(), bgid);
	if (bg && exists(thumb) && moddate(thumb) > moddate(file))
	   return bg;
	/* The thumbnail is gone or outdated - regererate */
     }
   else
     {
	if (bg)
	   return bg;
     }

   im = EImageLoad(file);
   if (!im)
      return NULL;

   EImageGetSize(im, &width, &height);

   if (thumb)
     {
	h2 = maxh;
	w2 = (width * h2) / height;
	if (w2 > maxw)
	  {
	     w2 = maxw;
	     h2 = (height * w2) / width;
	  }
	im2 = EImageCreateScaled(im, 0, 0, width, height, w2, h2);
	EImageSave(im2, thumb);
	EImageDecache(im2);
     }

   EImageDecache(im);

   /* Quit if the background itself already exists */
   if (bg)
      return bg;

   scr_asp = (WinGetW(VROOT) << 16) / WinGetH(VROOT);
   im_asp = (width << 16) / height;
   if (width == height)
     {
	justx = 0;
	justy = 0;
	scalex = 0;
	scaley = 0;
	tile = 1;
	keep_asp = 0;
     }
   else if ((!(IN_RANGE(scr_asp, im_asp, 16000)))
	    && ((width < 480) && (height < 360)))
     {
	justx = 0;
	justy = 0;
	scalex = 0;
	scaley = 0;
	tile = 1;
	keep_asp = 0;
     }
   else if (IN_RANGE(scr_asp, im_asp, 16000))
     {
	justx = 0;
	justy = 0;
	scalex = 1024;
	scaley = 1024;
	tile = 0;
	keep_asp = 0;
     }
   else if (im_asp > scr_asp)
     {
	justx = 512;
	justy = 512;
	scalex = 1024;
	scaley = 0;
	tile = 0;
	keep_asp = 1;
     }
   else
     {
	justx = 512;
	justy = 512;
	scalex = 0;
	scaley = 1024;
	tile = 0;
	keep_asp = 1;
     }

   COLOR32_FROM_RGB(color, 0, 0, 0);

   bg = BackgroundCreate(bgid, color, file, tile,
			 keep_asp, justx, justy,
			 scalex, scaley, NULL, 0, 0, 0, 0, 0);

   return bg;
}

void
BackgroundIncRefcount(Background * bg)
{
   if (!bg)
      return;
   bg->ref_count++;
}

void
BackgroundDecRefcount(Background * bg)
{
   if (!bg)
      return;
   bg->ref_count--;
   if (bg->ref_count <= 0)
      bg->last_viewed = 0;	/* Clean out asap */
}

void
BackgroundTouch(Background * bg)
{
   if (!bg)
      return;
   bg->last_viewed = time(NULL);
}

const char         *
BackgroundGetName(const Background * bg)
{
   return bg->name;
}

#if ENABLE_DIALOGS
static const char  *
BackgroundGetBgFile(const Background * bg)
{
   return bg->bg.file;
}

static const char  *
BackgroundGetFgFile(const Background * bg)
{
   return bg->top.file;
}
#endif /* ENABLE_DIALOGS */

Pixmap
BackgroundGetPixmap(const Background * bg)
{
   return (bg) ? bg->pmap : None;
}

unsigned int
BackgroundGetSeqNo(const Background * bg)
{
   return bg->seq_no;
}

int
BackgroundIsNone(const Background * bg)
{
   return (bg) ? bg->external : 1;
}

#if ENABLE_DIALOGS
static EImage      *
BackgroundCacheMini(Background * bg, int keep, int nuke)
{
   char                s[4096];
   EImage             *im;
   Pixmap              pmap;
   int                 mini_w = Mode.backgrounds.mini_w;
   int                 mini_h = Mode.backgrounds.mini_h;

   Esnprintf(s, sizeof(s), "%s/cached/bgsel/%s.png", EDirUserCache(),
	     BackgroundGetName(bg));

   im = EImageLoad(s);
   if (im)
     {
	if (nuke)
	   EImageDecache(im);
	else
	   goto done;
     }

   /* Create new cached bg mini image */
   pmap = ECreatePixmap(VROOT, mini_w, mini_h, 0);
   BackgroundApplyPmap(bg, VROOT, pmap, mini_w, mini_h);
   im = EImageGrabDrawable(pmap, None, 0, 0, mini_w, mini_h, 0);
   EImageSave(im, s);
   EFreePixmap(pmap);

 done:
   if (keep)
      return im;
   EImageFree(im);
   return NULL;
}
#endif /* ENABLE_DIALOGS */

#define S(str) ((str) ? str : "(null)")
static void
BackgroundGetInfoString1(const Background * bg, char *buf, int len)
{
   int                 r, g, b;

   COLOR32_TO_RGB(bg->bg_solid, r, g, b);
   Esnprintf(buf, len,
	     "%s ref_count %u keepim %u\n"
	     " bg.solid\t %i %i %i\n"
	     " bg.file\t %s\n"
	     " top.file\t %s\n"
	     " bg.tile\t %i\n"
	     " bg.keep_aspect\t %i \ttop.keep_aspect\t %i\n"
	     " bg.xjust\t %i \ttop.xjust\t %i\n"
	     " bg.yjust\t %i \ttop.yjust\t %i\n"
	     " bg.xperc\t %i \ttop.xperc\t %i\n"
	     " bg.yperc\t %i \ttop.yperc\t %i\n", bg->name,
	     bg->ref_count, bg->keepim, r, g, b,
	     bg->bg.file, bg->top.file, bg->bg_tile,
	     bg->bg.keep_aspect, bg->top.keep_aspect,
	     bg->bg.xjust, bg->top.xjust, bg->bg.yjust,
	     bg->top.yjust, bg->bg.xperc, bg->top.xperc,
	     bg->bg.yperc, bg->top.yperc);
}

static void
BackgroundGetInfoString2(const Background * bg, char *buf, int len)
{
   int                 r, g, b;

   COLOR32_TO_RGB(bg->bg_solid, r, g, b);
   Esnprintf(buf, len,
	     "%s %i %i %i %s %i %i %i %i %i %i %s %i %i %i %i %i",
	     bg->name, r, g, b, S(bg->bg.file), bg->bg_tile,
	     bg->bg.keep_aspect, bg->bg.xjust, bg->bg.yjust,
	     bg->bg.xperc, bg->bg.yperc, S(bg->top.file),
	     bg->top.keep_aspect, bg->top.xjust, bg->top.yjust,
	     bg->top.xperc, bg->top.yperc);
}

void
BackgroundsInvalidate(int refresh)
{
   Background         *bg;

   ECORE_LIST_FOR_EACH(bg_list, bg) BackgroundInvalidate(bg, refresh);
}

static Background  *
BackgroundGetRandom(void)
{
   Background         *bg;
   int                 num;
   unsigned int        rnd;

   num = ecore_list_count(bg_list);
   for (;;)
     {
	rnd = rand();
	bg = (Background *) ecore_list_index_goto(bg_list, rnd % num);
	if (num <= 1 || !BackgroundIsNone(bg))
	   break;
     }

   return bg;
}

void
BackgroundSetForDesk(Background * bg, unsigned int desk)
{
   if (desk >= N_BG_ASSIGNED)
      return;

   bg_assigned[desk] = bg;
}

Background         *
BackgroundGetForDesk(unsigned int desk)
{
   Background         *bg;

   if (desk >= N_BG_ASSIGNED)
      return NULL;

   bg = bg_assigned[desk];
   if (bg)
      bg = BackgroundCheck(bg);
   if (!bg)
      bg = BackgroundGetRandom();

   return bg;
}

/*
 * Config load/save
 */
#include "conf.h"

int
BackgroundsConfigLoad(FILE * fs)
{
   int                 err = 0;
   Background         *bg = 0;
   unsigned int        color;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char               *p2, *p3;
   int                 ii1;
   int                 r, g, b;
   int                 i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0;
   int                 j1 = 0, j2 = 0, j3 = 0, j4 = 0, j5 = 0;
   char               *bg1 = 0;
   char               *bg2 = 0;
   int                 desk;

   COLOR32_FROM_RGB(color, 0, 0, 0);

   while (GetLine(s, sizeof(s), fs))
     {
	ii1 = ConfigParseline1(s, s2, &p2, &p3);
	switch (ii1)
	  {
	  case CONFIG_CLOSE:
	     if (!bg)
		goto done;
	     bg->bg_solid = color;
	     bg->bg.file = bg1;
	     bg->top.file = bg2;
	     bg1 = bg2 = NULL;
	     bg->bg_tile = i1;
	     bg->bg.keep_aspect = i2;
	     bg->bg.xjust = i3;
	     bg->bg.yjust = i4;
	     bg->bg.xperc = i5;
	     bg->bg.yperc = i6;
	     bg->top.keep_aspect = j1;
	     bg->top.xjust = j2;
	     bg->top.yjust = j3;
	     bg->top.xperc = j4;
	     bg->top.yperc = j5;
	     goto done;

	  case CONFIG_COLORMOD:
	  case ICLASS_COLORMOD:
	     break;

	  case CONFIG_CLASSNAME:
	  case BG_NAME:
	     bg = BackgroundFind(s2);
	     if (!bg)
	       {
		  bg = BackgroundCreate(s2, color,
					bg1, i1, i2, i3, i4, i5, i6,
					bg2, j1, j2, j3, j4, j5);
	       }
	     else
	       {
		  color = bg->bg_solid;
		  Efree(bg1);
		  Efree(bg2);
		  bg1 = bg->bg.file;
		  bg2 = bg->top.file;
		  bg->bg.file = NULL;
		  bg->top.file = NULL;
		  i1 = bg->bg_tile;
		  i2 = bg->bg.keep_aspect;
		  i3 = bg->bg.xjust;
		  i4 = bg->bg.yjust;
		  i5 = bg->bg.xperc;
		  i6 = bg->bg.yperc;
		  j1 = bg->top.keep_aspect;
		  j2 = bg->top.xjust;
		  j3 = bg->top.yjust;
		  j4 = bg->top.xperc;
		  j5 = bg->top.yperc;
	       }
	     break;

	  case BG_DESKNUM:
	     if (!bg)
		break;
	     desk = atoi(s2);
	     if (desk >= N_BG_ASSIGNED)
		break;
	     if (desk >= 0)
	       {
		  if (!bg_assigned[desk] || Conf.backgrounds.user)
		    {
		       bg_assigned[desk] = bg;
		       bg->referenced = 1;
		    }
	       }
	     else
	       {
		  bg->referenced = 1;
		  for (ii1 = 0; ii1 < N_BG_ASSIGNED; ii1++)
		    {
		       if (!bg_assigned[ii1])
			  bg_assigned[ii1] = bg;
		    }
	       }
	     break;

	  case BG_RGB:
	     r = g = b = 0;
	     sscanf(p2, "%d %d %d", &r, &g, &b);
	     COLOR32_FROM_RGB(color, r, g, b);
	     break;

	  case BG_BG_FILE:
	     Efree(bg1);
	     bg1 = Estrdup(p2);
	     break;

	  case BG_BG_PARAM:
	     sscanf(p2, "%d %d %d %d %d %d", &i1, &i2, &i3, &i4, &i5, &i6);
	     break;

#if 1				/* Obsolete - backward compatibility */
	  case BG_BG1:
	     sscanf(p3, "%d %d %d %d %d %d", &i1, &i2, &i3, &i4, &i5, &i6);
	     Efree(bg1);
	     bg1 = Estrdup(s2);
	     break;
#endif

	  case BG_TOP_FILE:
	     Efree(bg2);
	     bg2 = Estrdup(p2);
	     break;

	  case BG_TOP_PARAM:
	     sscanf(p2, "%d %d %d %d %d", &j1, &j2, &j3, &j4, &j5);
	     break;

#if 1				/* Obsolete - backward compatibility */
	  case BG_BG2:
	     sscanf(p3, "%d %d %d %d %d", &j1, &j2, &j3, &j4, &j5);
	     Efree(bg2);
	     bg2 = Estrdup(s2);
	     break;
#endif

	  default:
	     break;
	  }
     }
   err = -1;

 done:
   Efree(bg1);
   Efree(bg2);

   return err;
}

static void
BackgroundsConfigLoadUser(void)
{
   char                s[4096];

   Esnprintf(s, sizeof(s), "%s.bg", EGetSavePrefix());
   if (!exists(s))
     {
	Mode.backgrounds.force_scan = 1;
	Esnprintf(s, sizeof(s), "%s.backgrounds", EGetSavePrefix());
	if (!exists(s))
	   return;
     }
   ConfigFileLoad(s, NULL, ConfigFileRead, 0);
}

static void
BackgroundsConfigSave(void)
{
   char                s[FILEPATH_LEN_MAX], st[FILEPATH_LEN_MAX];
   FILE               *fs;
   Background         *bg;
   unsigned int        j;
   int                 i, num, r, g, b;

   num = ecore_list_count(bg_list);
   if (num <= 0)
      return;

   Etmp(st);
   fs = fopen(st, "w");
   if (!fs)
      return;

   for (i = num - 1; i >= 0; i--)
     {
	bg = (Background *) ecore_list_index_goto(bg_list, i);
	if (!bg)
	   continue;

	/* Get full path to files */
	_BackgroundGetBgFile(bg);
	_BackgroundGetFgFile(bg);
	/* Discard if bg file is given but cannot be found (ignore bad fg) */
	if (bg->bg.file && !isabspath(bg->bg.file))
	  {
	     Eprintf("Discard broken background %s (%s)\n",
		     bg->name, bg->bg.file);
	     continue;
	  }

	fprintf(fs, "5 999\n");

	fprintf(fs, "100 %s\n", bg->name);
	COLOR32_TO_RGB(bg->bg_solid, r, g, b);
	if (r != 0 || g != 0 || b != 0)
	   fprintf(fs, "%d %d %d %d\n", BG_RGB, r, g, b);

	if (bg->bg.file)
	  {
	     fprintf(fs, "%d %s\n", BG_BG_FILE, bg->bg.file);
	     fprintf(fs, "%d %d %d %d %d %d %d\n", BG_BG_PARAM,
		     bg->bg_tile, bg->bg.keep_aspect,
		     bg->bg.xjust, bg->bg.yjust, bg->bg.xperc, bg->bg.yperc);
	  }

	if (bg->top.file)
	  {
	     fprintf(fs, "%d %s\n", BG_TOP_FILE, bg->top.file);
	     fprintf(fs, "%d %d %d %d %d %d\n", BG_TOP_PARAM,
		     bg->top.keep_aspect,
		     bg->top.xjust, bg->top.yjust,
		     bg->top.xperc, bg->top.yperc);
	  }

	for (j = 0; j < N_BG_ASSIGNED; j++)
	  {
	     if (bg == bg_assigned[j])
		fprintf(fs, "%d %d\n", BG_DESKNUM, j);
	  }

	fprintf(fs, "1000\n");
     }

   fclose(fs);

   Esnprintf(s, sizeof(s), "%s.bg", EGetSavePrefix());
   E_mv(st, s);
}

/*
 * Backgrounds module
 */

static void
BackgroundsCheckDups(void)
{
   int                 ix;
   Background         *bg, *bgx;

   for (ix = 0;; ix++)
     {
	ecore_list_index_goto(bg_list, ix);
	bg = (Background *) ecore_list_next(bg_list);
	if (!bg)
	   break;
	for (; (bgx = (Background *) ecore_list_next(bg_list));)
	  {
	     if (bgx->ref_count > 0 || bgx->referenced)
		continue;

	     if (BackgroundCmp(bg, bgx))
		continue;
#if 0
	     Eprintf("Remove duplicate background %s (==%s)\n", bgx->name,
		     bg->name);
#endif
	     BackgroundDestroy(bgx);
	  }
     }
}

static void
BackgroundsAccounting(void)
{
   Background         *bg;
   time_t              now;

   DesksBackgroundRefresh(NULL, DESK_BG_TIMEOUT);

   now = time(NULL);
   ECORE_LIST_FOR_EACH(bg_list, bg)
   {
      /* Skip if no pixmap or not timed out */
      if (bg->pmap == None ||
	  ((now - bg->last_viewed) <= Conf.backgrounds.timeout))
	 continue;

      DesksBackgroundRefresh(bg, DESK_BG_FREE);
      BackgroundPixmapFree(bg);
   }
}

static int
BackgroundsTimeout(void *data __UNUSED__)
{
   if (Conf.backgrounds.timeout <= 0)
      Conf.backgrounds.timeout = 1;

   BackgroundsAccounting();

   TimerSetInterval(bg_timer, 1000 * Conf.backgrounds.timeout);

   return 1;
}

static void
BackgroundsSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	/* create a fallback background in case no background is found */
	BackgroundCreate(NULL, 0, NULL, 0, 0, 0, 0, 0, 0, NULL, 0, 0, 0, 0, 0);
	break;

     case ESIGNAL_CONFIGURE:
	BackgroundsConfigLoadUser();
	BackgroundsCheckDups();
	StartupBackgroundsDestroy();
	break;

     case ESIGNAL_START:
	TIMER_ADD(bg_timer, 30000, BackgroundsTimeout, NULL);
	break;

     case ESIGNAL_EXIT:
	if (Mode.wm.save_ok)
	   BackgroundsConfigSave();
	break;
     }
}

#if ENABLE_DIALOGS
/*
 * Configuration dialog
 */
static DItem       *bg_sel;
static DItem       *bg_sel_slider;
static DItem       *bg_mini_disp;
static DItem       *bg_filename;
static DItem       *tmp_w[10];

static Background  *tmp_bg;	/* The background being configured */
static int          tmp_bg_sel_sliderval;
static int          tmp_bg_sel_sliderval_old;
static int          tmp_bg_r;
static int          tmp_bg_g;
static int          tmp_bg_b;
static char         tmp_bg_image;
static char         tmp_bg_tile;
static char         tmp_bg_keep_aspect;
static int          tmp_bg_xjust;
static int          tmp_bg_yjust;
static int          tmp_bg_xperc;
static int          tmp_bg_yperc;
static char         tmp_hiq;
static char         tmp_userbg;
static char         tmp_root_hint;
static int          tmp_bg_timeout;

static void         BG_RedrawView(void);
static void         BGSettingsGoTo(Background * bg);

static void
CB_ConfigureBG(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val == 2)
     {
	bg_sel = bg_sel_slider = bg_mini_disp = bg_filename = NULL;
	memset(tmp_w, 0, sizeof(tmp_w));
	BackgroundImagesKeep(tmp_bg, 0);
	tmp_bg = NULL;
	return;
     }

   if (val < 2)
     {
	Conf.backgrounds.timeout = tmp_bg_timeout;
	Conf.backgrounds.hiquality = tmp_hiq;
	Conf.backgrounds.user = tmp_userbg;
	Conf.hints.set_xroot_info_on_root_window = tmp_root_hint;

	COLOR32_FROM_RGB(tmp_bg->bg_solid, tmp_bg_r, tmp_bg_g, tmp_bg_b);
	tmp_bg->bg_tile = tmp_bg_tile;
	tmp_bg->bg.keep_aspect = tmp_bg_keep_aspect;
	tmp_bg->bg.xjust = tmp_bg_xjust;
	tmp_bg->bg.yjust = tmp_bg_yjust;
	tmp_bg->bg.xperc = tmp_bg_xperc;
	tmp_bg->bg.yperc = tmp_bg_yperc;
	if (!tmp_bg_image)
	   BackgroundFilesRemove(tmp_bg);

	BackgroundInvalidate(tmp_bg, 1);

	BackgroundCacheMini(tmp_bg, 0, 1);
	BG_RedrawView();
     }

   autosave();
}

/* Draw the background preview image */
static void
CB_DesktopMiniDisplayRedraw(Dialog * d __UNUSED__, int val __UNUSED__,
			    void *data)
{
   Background         *bg;
   Pixmap              pmap;
   int                 w, h;
   DItem              *di;
   Win                 win;
   unsigned int        color;
   const char         *fbg, *ffg;

   if (!tmp_bg)
      return;

   di = (DItem *) data;
   win = DialogItemAreaGetWindow(di);
   DialogItemAreaGetSize(di, &w, &h);

   pmap = EGetWindowBackgroundPixmap(win);
   fbg = (tmp_bg_image) ? BackgroundGetBgFile(tmp_bg) : NULL;
   ffg = (tmp_bg_image) ? BackgroundGetFgFile(tmp_bg) : NULL;
   COLOR32_FROM_RGB(color, tmp_bg_r, tmp_bg_g, tmp_bg_b);
   bg = BackgroundCreate("TEMP", color,
			 fbg, tmp_bg_tile, tmp_bg_keep_aspect,
			 tmp_bg_xjust, tmp_bg_yjust,
			 tmp_bg_xperc, tmp_bg_yperc,
			 ffg, tmp_bg->top.keep_aspect,
			 tmp_bg->top.xjust, tmp_bg->top.yjust,
			 tmp_bg->top.xperc, tmp_bg->top.yperc);

   BackgroundApplyPmap(bg, win, pmap, w, h);
   BackgroundDestroy(bg);
   EClearWindow(win);
}

/* Update tmp vars according to the current tmp_bg */
static void
BG_GetValues(void)
{
   tmp_bg_image = (tmp_bg->bg.file) ? 1 : 0;

   COLOR32_TO_RGB(tmp_bg->bg_solid, tmp_bg_r, tmp_bg_g, tmp_bg_b);
   tmp_bg_tile = tmp_bg->bg_tile;
   tmp_bg_keep_aspect = tmp_bg->bg.keep_aspect;
   tmp_bg_xjust = tmp_bg->bg.xjust;
   tmp_bg_yjust = tmp_bg->bg.yjust;
   tmp_bg_xperc = tmp_bg->bg.xperc;
   tmp_bg_yperc = tmp_bg->bg.yperc;
}

static void
BG_DialogSetFileName(DItem * di)
{
   const char         *stmp;
   char                s[1024];

   stmp = fullfileof(BackgroundGetBgFile(tmp_bg));
   Esnprintf(s, sizeof(s),
	     _("Background definition information:\nName: %s\nFile: %s"),
	     BackgroundGetName(tmp_bg), (stmp) ? stmp : _("-NONE-"));
   DialogItemSetText(di, s);
}

static void
BgDialogSetNewCurrent(Background * bg)
{
   if (tmp_bg && tmp_bg != bg)
      BackgroundImagesKeep(tmp_bg, 0);
   tmp_bg = bg;
   BackgroundImagesKeep(tmp_bg, 1);

   /* Fetch new BG values */
   BG_GetValues();

   /* Update dialog items */
   BG_DialogSetFileName(bg_filename);

   DialogItemCheckButtonSetState(tmp_w[0], tmp_bg_image);
   DialogItemCheckButtonSetState(tmp_w[1], tmp_bg_keep_aspect);
   DialogItemCheckButtonSetState(tmp_w[2], tmp_bg_tile);
   DialogItemSliderSetVal(tmp_w[3], tmp_bg_r);
   DialogItemSliderSetVal(tmp_w[4], tmp_bg_g);
   DialogItemSliderSetVal(tmp_w[5], tmp_bg_b);
   DialogItemSliderSetVal(tmp_w[6], tmp_bg_xjust);
   DialogItemSliderSetVal(tmp_w[7], tmp_bg_yjust);
   DialogItemSliderSetVal(tmp_w[8], tmp_bg_yperc);
   DialogItemSliderSetVal(tmp_w[9], tmp_bg_xperc);

   /* Redraw mini BG display */
   CB_DesktopMiniDisplayRedraw(NULL, 0, bg_mini_disp);

   /* Redraw scrolling BG list */
   BG_RedrawView();
}

/* Duplicate current (tmp_bg) to new */
static void
CB_ConfigureNewBG(Dialog * d __UNUSED__, int val __UNUSED__,
		  void *data __UNUSED__)
{
   char                s[1024];
   unsigned int        color;
   int                 lower, upper;

   Esnprintf(s, sizeof(s), "__NEWBG_%i", (unsigned)time(NULL));

   COLOR32_FROM_RGB(color, tmp_bg_r, tmp_bg_g, tmp_bg_b);

   tmp_bg = BackgroundCreate(s, color,
			     tmp_bg->bg.file, tmp_bg_tile, tmp_bg_keep_aspect,
			     tmp_bg_xjust, tmp_bg_yjust,
			     tmp_bg_xperc, tmp_bg_yperc,
			     tmp_bg->top.file, tmp_bg->top.keep_aspect,
			     tmp_bg->top.xjust, tmp_bg->top.yjust,
			     tmp_bg->top.xperc, tmp_bg->top.yperc);

   DialogItemSliderGetBounds(bg_sel_slider, &lower, &upper);
   upper += 4;
   DialogItemSliderSetBounds(bg_sel_slider, lower, upper);

   DialogItemSliderSetVal(bg_sel_slider, 0);

   DeskBackgroundSet(DesksGetCurrent(), tmp_bg);

   BG_RedrawView();

   autosave();
}

static void
CB_ConfigureDelBG(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   Background         *bg;
   int                 i, num;
   int                 lower, upper;

   num = ecore_list_count(bg_list);
   if (num <= 1)
      return;

   bg = (Background *) ecore_list_goto(bg_list, tmp_bg);
   if (!bg)
      return;

   i = ecore_list_index(bg_list);
   bg =
      (Background *) ecore_list_index_goto(bg_list,
					   (i < num - 1) ? i + 1 : i - 1);

   DeskBackgroundSet(DesksGetCurrent(), bg);

   if (val == 0)
      BackgroundDestroy(tmp_bg);
   else
      BackgroundDelete(tmp_bg);
   tmp_bg = NULL;

   DialogItemSliderGetBounds(bg_sel_slider, &lower, &upper);
   upper -= 4;
   DialogItemSliderSetBounds(bg_sel_slider, lower, upper);
   if (tmp_bg_sel_sliderval > upper)
      DialogItemSliderSetVal(bg_sel_slider, upper);

   BgDialogSetNewCurrent(bg);

   autosave();
}

/* Move current background to first position in list */
static void
CB_ConfigureFrontBG(Dialog * d __UNUSED__, int val __UNUSED__,
		    void *data __UNUSED__)
{
   ecore_list_prepend(bg_list, ecore_list_node_remove(bg_list, tmp_bg));
   BGSettingsGoTo(tmp_bg);
   BG_RedrawView();
   autosave();
}

/* Draw the scrolling background image window */
static void
BG_RedrawView(void)
{
   Background         *bg;
   int                 x, w, h, num;
   Win                 win;
   Pixmap              pmap;
   ImageClass         *ic;
   int                 mini_w = Mode.backgrounds.mini_w;
   int                 mini_h = Mode.backgrounds.mini_h;

   num = ecore_list_count(bg_list);
   if (num <= 0)
      return;

   win = DialogItemAreaGetWindow(bg_sel);
   DialogItemAreaGetSize(bg_sel, &w, &h);

   pmap = EGetWindowBackgroundPixmap(win);

   ic = ImageclassFind("DIALOG_BUTTON", 0);
   if (!ic)
      ic = ImageclassFind("DIALOG_WIDGET_BUTTON", 1);

   ImageclassApplySimple(ic, win, pmap, STATE_NORMAL, 0, 0, w, h);

   x = -(num * (mini_w + 8) - w) * tmp_bg_sel_sliderval / (4 * num);

   ECORE_LIST_FOR_EACH(bg_list, bg)
   {
      if (((x + mini_w + 8) >= 0) && (x < w))
	{
	   EImage             *im;

	   ImageclassApplySimple(ic, win, pmap,
				 (bg == tmp_bg) ? STATE_CLICKED : STATE_NORMAL,
				 x, 0, mini_w + 8, mini_h + 8);

	   if (BackgroundIsNone(bg))
	     {
		TextClass          *tc;

		tc = TextclassFind("DIALOG", 1);
		if (tc)
		  {
		     int                 tw, th;

		     TextSize(tc, 0, 0, STATE_NORMAL,
			      _("No\nBackground"), &tw, &th, 17);
		     TextDraw(tc, win, pmap, 0, 0, STATE_NORMAL,
			      _("No\nBackground"), x + 4,
			      4 + ((mini_h - th) / 2), mini_w, mini_h, 17, 512);
		  }
	     }
	   else
	     {
		im = BackgroundCacheMini(bg, 1, 0);
		if (im)
		  {
		     EImageRenderOnDrawable(im, win, pmap, 0, x + 4, 4,
					    mini_w, mini_h);
		     EImageFree(im);
		  }
	     }
	}
      x += (mini_w + 8);
   }

   EClearWindow(win);
}

static void
CB_BGAreaSlide(Dialog * d __UNUSED__, int val __UNUSED__, void *data __UNUSED__)
{
   if (tmp_bg_sel_sliderval == tmp_bg_sel_sliderval_old)
      return;
   BG_RedrawView();
   tmp_bg_sel_sliderval_old = tmp_bg_sel_sliderval;
}

static void
CB_BGScan(Dialog * d, int val __UNUSED__, void *data __UNUSED__)
{
   int                 num;

   SoundPlay(SOUND_WAIT);

   /* Forcing re-scan should not be necessary but provides the progress bars
    * so it actually looks like something is going on */
   Mode.backgrounds.force_scan = 1;
   ScanBackgroundMenu();

   num = ecore_list_count(bg_list);
   DialogItemSliderSetBounds(bg_sel_slider, 0, num * 4);
   DialogItemCallCallback(d, bg_sel_slider);
}

static void
CB_BGAreaEvent(DItem * di, int val __UNUSED__, void *data)
{
   int                 x, num, w, h;
   Background         *bg;
   XEvent             *ev = (XEvent *) data;
   int                 mini_w = Mode.backgrounds.mini_w;

   DialogItemAreaGetSize(di, &w, &h);

   switch (ev->type)
     {
     case ButtonPress:
	switch (ev->xbutton.button)
	  {
	  case 1:
	     num = ecore_list_count(bg_list);
	     x = (num * (mini_w + 8) - w) * tmp_bg_sel_sliderval / (4 * num) +
		ev->xbutton.x;
	     bg =
		(Background *) ecore_list_index_goto(bg_list, x / (mini_w + 8));
	     if (!bg || bg == DeskBackgroundGet(DesksGetCurrent()))
		break;
	     BgDialogSetNewCurrent(bg);
	     DeskBackgroundSet(DesksGetCurrent(), bg);
	     autosave();
	     break;
	  case 4:
	     tmp_bg_sel_sliderval += 4;
	     goto do_slide;
	  case 5:
	     tmp_bg_sel_sliderval -= 4;
	     goto do_slide;
	   do_slide:
	     DialogItemSliderSetVal(bg_sel_slider, tmp_bg_sel_sliderval);
	     CB_BGAreaSlide(NULL, 0, NULL);
	     break;
	  }
     }
}

static void
CB_DesktopTimeout(Dialog * d __UNUSED__, int val __UNUSED__, void *data)
{
   DItem              *di;
   char                s[256];

   di = (DItem *) data;
   Esnprintf(s, sizeof(s), _("Unused backgrounds freed after %2i:%02i:%02i"),
	     tmp_bg_timeout / 3600,
	     (tmp_bg_timeout / 60) - (60 * (tmp_bg_timeout / 3600)),
	     (tmp_bg_timeout) - (60 * (tmp_bg_timeout / 60)));
   DialogItemSetText(di, s);
}

static void
BGSettingsGoTo(Background * bg)
{
   int                 i, num;

   if (!bg_sel_slider)
      return;

   bg = (Background *) ecore_list_goto(bg_list, bg);
   if (!bg)
      return;

   i = ecore_list_index(bg_list);
   num = ecore_list_count(bg_list);
   i = ((4 * num + 20) * i) / num - 8;
   if (i < 0)
      i = 0;
   else if (i > 4 * num)
      i = 4 * num;
   DialogItemSliderSetVal(bg_sel_slider, i);
   BgDialogSetNewCurrent(bg);
}

static void
CB_BGNext(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   Background         *bg;

   bg = (Background *) ecore_list_goto(bg_list, tmp_bg);
   if (!bg)
      return;

   bg =
      (Background *) ecore_list_index_goto(bg_list,
					   ecore_list_index(bg_list) + val);
   if (!bg)
      return;

   BGSettingsGoTo(bg);
   DeskBackgroundSet(DesksGetCurrent(), bg);
}

static int
BG_SortFileCompare(const void *_bg1, const void *_bg2)
{
   const Background   *bg1 = (const Background *)_bg1;
   const Background   *bg2 = (const Background *)_bg2;
   const char         *name1, *name2;

   /* return < 0 is b1 <  b2 */
   /* return > 0 is b1 >  b2 */
   /* return   0 is b1 == b2 */

   name1 = BackgroundGetBgFile(bg1);
   name2 = BackgroundGetBgFile(bg2);
   if (name1 && name2)
      return strcmp(name1, name2);
   if (name1)
      return 1;
   if (name2)
      return -1;
   return (bg1 < bg2) ? -1 : 1;
}

static void
CB_BGSortFile(Dialog * d __UNUSED__, int val __UNUSED__, void *data __UNUSED__)
{
   Background        **bglist;
   int                 i, num;

   bglist = (Background **) ecore_list_items_get(bg_list, &num);
   if (!bglist)
      return;

   /* remove them all from the list */
   for (i = 0; i < num; i++)
      ecore_list_node_remove(bg_list, bglist[i]);
   Quicksort((void **)bglist, 0, num - 2, BG_SortFileCompare);
   for (i = 0; i < num; i++)
      ecore_list_append(bg_list, bglist[i]);
   Efree(bglist);

   BGSettingsGoTo(tmp_bg);

   autosave();
}

static void
CB_BGSortAttrib(Dialog * d __UNUSED__, int val __UNUSED__,
		void *data __UNUSED__)
{
   Background        **bglist;
   int                 i, num;

   bglist = (Background **) ecore_list_items_get(bg_list, &num);
   if (!bglist)
      return;

   /* remove them all from the list */
   for (i = 0; i < num; i++)
      ecore_list_node_remove(bg_list, bglist[i]);
   for (i = 0; i < num; i++)
     {
	Background         *bg;

	bg = bglist[i];
	if ((bg) && (bg->bg_tile) && (bg->bg.xperc == 0) && (bg->bg.yperc == 0))
	  {
	     ecore_list_prepend(bg_list, bg);
	     bglist[i] = NULL;
	  }
     }
   for (i = 0; i < num; i++)
     {
	Background         *bg;

	bg = bglist[i];
	if (bg)
	  {
	     ecore_list_prepend(bg_list, bg);
	     bglist[i] = NULL;
	  }
     }
   Efree(bglist);

   BGSettingsGoTo(tmp_bg);

   autosave();
}

#if 0				/* Doesn't do anything useful */
static void
CB_BGSortContent(Dialog * d __UNUSED__, int val __UNUSED__,
		 void *data __UNUSED__)
{
   Background        **bglist;
   int                 i, num;

   bglist = (Background **) ecore_list_items_get(bg_list, &num);
   if (!bglist)
      return;

   /* remove them all from the list */
   for (i = 0; i < num; i++)
      ecore_list_node_remove(bg_list, bglist[i]);
   for (i = 0; i < num; i++)
      ecore_list_prepend(bg_list, bglist[i]);
   Efree(bglist);

   autosave();
}
#endif

#if 0				/* Remove? */
static void
CB_DesktopMiniDisplayAreaInit(DItem * di, int val __UNUSED__,
			      void *data __UNUSED__)
{
   CB_DesktopMiniDisplayRedraw(DialogItemGetDialog(di), 1, di);
}
#endif

static void
CB_InitView(DItem * di __UNUSED__, int val __UNUSED__, void *data __UNUSED__)
{
   tmp_bg_sel_sliderval_old = tmp_bg_sel_sliderval = -1;
   BGSettingsGoTo(tmp_bg);
}

static void
_DlgFillBackground(Dialog * d, DItem * table, void *data)
{
   Background         *bg = (Background *) data;
   DItem              *di, *table2, *table3, *area, *label;
   int                 i, num;
   char                s[1024];
   int                 mini_w = Mode.backgrounds.mini_w;
   int                 mini_h = Mode.backgrounds.mini_h;

   if (!Conf.backgrounds.no_scan)
      ScanBackgroundMenu();

   if (!bg)
      bg = DeskBackgroundGet(DesksGetCurrent());
   if (!bg)
      bg = BackgroundFind("NONE");
   tmp_bg = bg;

   BG_GetValues();

   tmp_hiq = Conf.backgrounds.hiquality;
   tmp_userbg = Conf.backgrounds.user;
   tmp_root_hint = Conf.hints.set_xroot_info_on_root_window;
   tmp_bg_timeout = Conf.backgrounds.timeout;

   DialogItemTableSetOptions(table, 1, 0, 0, 0);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table2, 2, 0, 1, 0);

   di = bg_filename = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetFill(di, 1, 0);
   BG_DialogSetFileName(bg_filename);

   table3 = DialogAddItem(table2, DITEM_TABLE);

   di = tmp_w[0] = DialogAddItem(table3, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Use background image"));
   DialogItemCheckButtonSetPtr(di, &tmp_bg_image);

   di = tmp_w[1] = DialogAddItem(table3, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Keep aspect on scale"));
   DialogItemCheckButtonSetPtr(di, &tmp_bg_keep_aspect);

   di = tmp_w[2] = DialogAddItem(table3, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Tile image across background"));
   DialogItemCheckButtonSetPtr(di, &tmp_bg_tile);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table2, 4, 0, 1, 0);
   DialogItemSetFill(table2, 0, 0);
   DialogItemSetAlign(table2, 512, 0);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetText(di, _("Move to Front"));
   DialogItemSetCallback(di, CB_ConfigureFrontBG, 0, NULL);
   DialogBindKey(d, "F", CB_ConfigureFrontBG, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetText(di, _("Duplicate"));
   DialogItemSetCallback(di, CB_ConfigureNewBG, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetText(di, _("Unlist"));
   DialogItemSetCallback(di, CB_ConfigureDelBG, 0, NULL);
   DialogBindKey(d, "D", CB_ConfigureDelBG, 0, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetText(di, _("Delete File"));
   DialogItemSetCallback(di, CB_ConfigureDelBG, 1, NULL);
   DialogBindKey(d, "Delete", CB_ConfigureDelBG, 1, NULL);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table2, 3, 0, 1, 0);

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 512, 512);
   DialogItemSetText(di,
		     _("Background\n" "Image\n" "Scaling\n" "and\n"
		       "Alignment\n"));

   table3 = DialogAddItem(table2, DITEM_TABLE);
   DialogItemTableSetOptions(table3, 3, 0, 0, 0);

   DialogAddItem(table3, DITEM_NONE);

   di = tmp_w[6] = DialogAddItem(table3, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, mini_w);
   DialogItemSliderSetValPtr(di, &tmp_bg_xjust);

   DialogAddItem(table3, DITEM_NONE);

   di = tmp_w[7] = DialogAddItem(table3, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetOrientation(di, 0);
   DialogItemSetFill(di, 0, 1);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, mini_w);
   DialogItemSliderSetValPtr(di, &tmp_bg_yjust);

   di = bg_mini_disp = area = DialogAddItem(table3, DITEM_AREA);
   DialogItemAreaSetSize(di, mini_w, mini_h);
#if 0				/* Remove? */
   DialogItemAreaSetInitFunc(di, CB_DesktopMiniDisplayAreaInit);
#endif

   di = tmp_w[8] = DialogAddItem(table3, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetOrientation(di, 0);
   DialogItemSetFill(di, 0, 1);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, mini_w);
   DialogItemSliderSetValPtr(di, &tmp_bg_yperc);

   DialogAddItem(table3, DITEM_NONE);

   di = tmp_w[9] = DialogAddItem(table3, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, mini_w);
   DialogItemSliderSetValPtr(di, &tmp_bg_xperc);

   table3 = DialogAddItem(table2, DITEM_TABLE);
   DialogItemTableSetOptions(table3, 2, 0, 0, 0);

   di = DialogAddItem(table3, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 512, 512);
   DialogItemSetText(di, _("BG Colour"));

   di = DialogAddItem(table3, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Red:"));

   di = tmp_w[3] = DialogAddItem(table3, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 255);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 16);
   DialogItemSliderSetValPtr(di, &tmp_bg_r);

   di = DialogAddItem(table3, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Green:"));

   di = tmp_w[4] = DialogAddItem(table3, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 255);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 16);
   DialogItemSliderSetValPtr(di, &tmp_bg_g);

   di = DialogAddItem(table3, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Blue:"));

   di = tmp_w[5] = DialogAddItem(table3, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 255);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 16);
   DialogItemSliderSetValPtr(di, &tmp_bg_b);

   for (i = 0; i < 10; i++)
      DialogItemSetCallback(tmp_w[i], CB_DesktopMiniDisplayRedraw, 0, area);

   DialogAddItem(table, DITEM_SEPARATOR);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table2, 3, 0, 0, 0);

   table3 = DialogAddItem(table2, DITEM_TABLE);
   DialogItemTableSetOptions(table3, 2, 0, 0, 0);

   di = DialogAddItem(table3, DITEM_BUTTON);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetText(di, "<-");
   DialogItemSetCallback(di, CB_BGNext, -1, NULL);
   DialogBindKey(d, "Left", CB_BGNext, -1, NULL);

   di = DialogAddItem(table3, DITEM_BUTTON);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetText(di, "->");
   DialogItemSetCallback(di, CB_BGNext, 1, NULL);
   DialogBindKey(d, "Right", CB_BGNext, 1, NULL);

   di = DialogAddItem(table2, DITEM_BUTTON);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetText(di, _("Pre-scan BG's"));
   DialogItemSetCallback(di, CB_BGScan, 0, NULL);

   table3 = DialogAddItem(table2, DITEM_TABLE);
   DialogItemTableSetOptions(table3, 3, 0, 0, 0);

   di = DialogAddItem(table3, DITEM_BUTTON);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetText(di, _("Sort by File"));
   DialogItemSetCallback(di, CB_BGSortFile, 0, NULL);

   di = DialogAddItem(table3, DITEM_BUTTON);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetText(di, _("Sort by Attr."));
   DialogItemSetCallback(di, CB_BGSortAttrib, 0, NULL);

#if 0				/* Doesn't do anything useful */
   di = DialogAddItem(table3, DITEM_BUTTON);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetText(di, _("Sort by Image"));
   DialogItemSetCallback(di, CB_BGSortContent, 0, NULL);
#endif

   di = bg_sel = DialogAddItem(table, DITEM_AREA);
   DialogItemAreaSetSize(di, 160, 8 + Mode.backgrounds.mini_h);
   DialogItemAreaSetEventFunc(di, CB_BGAreaEvent);
   DialogItemAreaSetInitFunc(di, CB_InitView);

   num = ecore_list_count(bg_list);
   di = bg_sel_slider = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, num * 4);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 9);
   DialogItemSliderSetValPtr(di, &tmp_bg_sel_sliderval);
   DialogItemSetCallback(bg_sel_slider, CB_BGAreaSlide, 0, NULL);

   DialogAddItem(table, DITEM_SEPARATOR);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Use dithering in Hi-Colour"));
   DialogItemCheckButtonSetPtr(di, &tmp_hiq);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Background overrides theme"));
   DialogItemCheckButtonSetPtr(di, &tmp_userbg);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di,
		     _("Enable background transparency compatibility mode"));
   DialogItemCheckButtonSetPtr(di, &tmp_root_hint);

   DialogAddItem(table, DITEM_SEPARATOR);

   di = label = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 512, 512);
   Esnprintf(s, sizeof(s), _("Unused backgrounds freed after %2i:%02i:%02i"),
	     tmp_bg_timeout / 3600,
	     (tmp_bg_timeout / 60) - (60 * (tmp_bg_timeout / 3600)),
	     (tmp_bg_timeout) - (60 * (tmp_bg_timeout / 60)));
   DialogItemSetText(di, s);

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetBounds(di, 0, 60 * 60 * 4);
   DialogItemSliderSetUnits(di, 30);
   DialogItemSliderSetJump(di, 60);
   DialogItemSliderSetValPtr(di, &tmp_bg_timeout);
   DialogItemSetCallback(di, CB_DesktopTimeout, 0, label);
}

const DialogDef     DlgBackground = {
   "CONFIGURE_BG",
   N_("Background"),
   N_("Desktop Background Settings"),
   SOUND_SETTINGS_BG,
   "pix/bg.png",
   N_("Enlightenment Desktop\n" "Background Settings Dialog"),
   _DlgFillBackground,
   DLG_OAC, CB_ConfigureBG,
};

#endif /* ENABLE_DIALOGS */

/*
 * IPC functions
 */

static void
BackgroundSet1(const char *name, const char *params)
{
   const char         *p = params;
   char                type[FILEPATH_LEN_MAX];
   int                 len, value;
   Background         *bg;
   unsigned int        color;

   if (!p || !p[0])
      return;

   bg = BackgroundFind(name);
   if (!bg)
     {
	COLOR32_FROM_RGB(color, 0, 0, 0);
	bg = BackgroundCreate(name, color, NULL, 0, 0, 0,
			      0, 0, 0, NULL, 0, 0, 0, 0, 0);
	if (!bg)
	  {
	     IpcPrintf("Error: could not create background '%s'\n", name);
	     return;
	  }
     }

   type[0] = '\0';
   len = 0;
   sscanf(p, "%400s %n", type, &len);
   p += len;
   value = atoi(p);

   if (!strcmp(type, "bg.solid"))
     {
	int                 r, b, g;

	r = g = b = 0;
	sscanf(p, "%i %i %i", &r, &g, &b);
	COLOR32_FROM_RGB(bg->bg_solid, r, g, b);
     }
   else if (!strcmp(type, "bg.file"))
     {
	Efree(bg->bg.file);
	bg->bg.file = Estrdup(p);
     }
   else if (!strcmp(type, "bg.tile"))
     {
	bg->bg_tile = value;
     }
   else if (!strcmp(type, "bg.keep_aspect"))
     {
	bg->bg.keep_aspect = value;
     }
   else if (!strcmp(type, "bg.xjust"))
     {
	bg->bg.xjust = value;
     }
   else if (!strcmp(type, "bg.yjust"))
     {
	bg->bg.yjust = value;
     }
   else if (!strcmp(type, "bg.xperc"))
     {
	bg->bg.xperc = value;
     }
   else if (!strcmp(type, "bg.yperc"))
     {
	bg->bg.yperc = value;
     }
   else if (!strcmp(type, "top.file"))
     {
	Efree(bg->top.file);
	bg->top.file = Estrdup(p);
     }
   else if (!strcmp(type, "top.keep_aspect"))
     {
	bg->top.keep_aspect = value;
     }
   else if (!strcmp(type, "top.xjust"))
     {
	bg->top.xjust = value;
     }
   else if (!strcmp(type, "top.yjust"))
     {
	bg->top.yjust = value;
     }
   else if (!strcmp(type, "top.xperc"))
     {
	bg->top.xperc = value;
     }
   else if (!strcmp(type, "top.yperc"))
     {
	bg->top.yperc = value;
     }
   else
     {
	IpcPrintf("Error: unknown background value type '%s'\n", type);
     }
   autosave();
}

static void
BackgroundSet2(const char *name, const char *params)
{
   Background         *bg;
   unsigned int        color;
   int                 r, g, b;
   char                bgf[FILEPATH_LEN_MAX], topf[FILEPATH_LEN_MAX];
   int                 tile, keep_aspect, tkeep_aspect;
   int                 xjust, yjust, xperc, yperc;
   int                 txjust, tyjust, txperc, typerc;

   if (!params)
      return;

   bgf[0] = topf[0] = '\0';
   r = g = b = 99;
   sscanf(params,
	  "%i %i %i %4000s %i %i %i %i %i %i %4000s %i %i %i %i %i",
	  &r, &g, &b,
	  bgf, &tile, &keep_aspect, &xjust, &yjust, &xperc, &yperc,
	  topf, &tkeep_aspect, &txjust, &tyjust, &txperc, &typerc);
   COLOR32_FROM_RGB(color, r, g, b);

   bg = BackgroundFind(name);
   if (bg)
     {
	BackgroundModify(bg, color, bgf, tile, keep_aspect, xjust,
			 yjust, xperc, yperc, topf, tkeep_aspect,
			 txjust, tyjust, txperc, typerc);
     }
   else
     {
	BackgroundCreate(name, color, bgf, tile, keep_aspect, xjust,
			 yjust, xperc, yperc, topf, tkeep_aspect,
			 txjust, tyjust, txperc, typerc);
     }
}

static void
BackgroundsIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[128], buf[4096];
   int                 i, len, num, len2;
   Background         *bg;

   len = len2 = 0;
   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	sscanf(p, "%100s %n%100s %n", cmd, &len2, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	for (i = 0; i < (int)DesksGetNumber(); i++)
	  {
	     bg = DeskBackgroundGet(DeskGet(i));
	     if (bg)
		IpcPrintf("%i %s\n", i, BackgroundGetName(bg));
	     else
		IpcPrintf("%i %s\n", i, "-NONE-");
	  }
     }
   else if (!strncmp(cmd, "apply", 2))
     {
	Window              xwin;
	Win                 win;

	bg = BackgroundFind(prm);
	if (!bg)
	   return;

	xwin = None;
	sscanf(p, "%lx", &xwin);

	win = ECreateWinFromXwin(xwin);
	if (!win)
	   return;
	BackgroundApplyWin(bg, win);
	EDestroyWin(win);
     }
   else if (!strncmp(cmd, "del", 2))
     {
	BackgroundDestroyByName(prm);
     }
   else if (!strncmp(cmd, "list", 2))
     {
	ECORE_LIST_FOR_EACH(bg_list, bg) IpcPrintf("%s\n", bg->name);
     }
   else if (!strncmp(cmd, "load", 2))
     {
	bg = BackgroundFind(prm);
	if (bg)
	  {
	     IpcPrintf("Background already defined\n");
	  }
	else
	  {
	     BrackgroundCreateFromImage(prm, p, NULL, 0);
	  }
     }
   else if (!strncmp(cmd, "set", 2))
     {
	BackgroundSet1(prm, p);
     }
   else if (!strncmp(cmd, "show", 2))
     {
	bg = BackgroundFind(prm);

	if (bg)
	  {
	     BackgroundGetInfoString1(bg, buf, sizeof(buf));
	     IpcPrintf("%s\n", buf);
	  }
	else
	   IpcPrintf("Error: background '%s' does not exist\n", prm);
     }
   else if (!strcmp(cmd, "use"))
     {
	if (!strcmp(prm, "-"))
	   bg = NULL;
	else
	   bg = BackgroundFind(prm);

	num = DesksGetCurrentNum();
	sscanf(p, "%d %n", &num, &len);
	DeskBackgroundSet(DeskGet(num), bg);
	autosave();
     }
   else if (!strncmp(cmd, "xget", 2))
     {
	bg = BackgroundFind(prm);

	if (bg)
	  {
	     BackgroundGetInfoString2(bg, buf, sizeof(buf));
	     IpcPrintf("%s\n", buf);
	  }
	else
	   IpcPrintf("Error: background '%s' does not exist\n", prm);
     }
   else if (!strncmp(cmd, "xset", 2))
     {
	BackgroundSet2(prm, p);
     }
   else
     {
	/* Compatibility with pre- 0.16.8 clients */
	BackgroundSet1(cmd, params + len2);
     }
}

static void
IPC_BackgroundUse(const char *params)
{
   char                name[1024];
   const char         *p;
   Background         *bg;
   int                 i, l;

   p = params;
   name[0] = '\0';
   l = 0;
   sscanf(p, "%1000s %n", name, &l);
   p += l;

   bg = BackgroundFind(name);
   if (!bg)
      return;

   for (;;)
     {
	i = l = -1;
	sscanf(p, "%d %n", &i, &l);
	p += l;
	if (i < 0)
	   break;
	DeskBackgroundSet(DeskGet(i), bg);
     }

   autosave();
}

static const IpcItem BackgroundsIpcArray[] = {
   {
    BackgroundsIpc,
    "background", "bg",
    "Background commands",
    "  background                       Show current background\n"
    "  background apply <name> <win>    Apply background to window\n"
    "  background del <name>            Delete background\n"
    "  background list                  Show all background\n"
    "  background load <name> <file>    Load new wallpaper from file\n"
    "  background set <name> ...        Set background parameters\n"
    "  background show <name>           Show background info\n"
    "  background use <name> <desks...> Switch to background <name>\n"
    "  background xget <name>           Special show background parameters\n"
    "  background xset <name> ...       Special set background parameters\n"}
   ,
   {
    IPC_BackgroundUse, "use_bg", NULL, "Deprecated - do not use", NULL}
   ,
};
#define N_IPC_FUNCS (sizeof(BackgroundsIpcArray)/sizeof(IpcItem))

/*
 * Configuration items
 */
static const CfgItem BackgroundsCfgItems[] = {
   CFG_ITEM_BOOL(Conf.backgrounds, hiquality, 1),
   CFG_ITEM_BOOL(Conf.backgrounds, user, 1),
   CFG_ITEM_BOOL(Conf.backgrounds, no_scan, 0),
   CFG_ITEM_INT(Conf.backgrounds, timeout, 240),
};
#define N_CFG_ITEMS (sizeof(BackgroundsCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModBackgrounds;

const EModule       ModBackgrounds = {
   "backgrounds", "bg",
   BackgroundsSighan,
   {N_IPC_FUNCS, BackgroundsIpcArray},
   {N_CFG_ITEMS, BackgroundsCfgItems}
};

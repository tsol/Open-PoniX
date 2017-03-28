/*
 * Copyright (C) 2006-2009 Kim Woelders
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
#include "E.h"
#include "tclass.h"

#ifdef USE_XFT
#include "xwin.h"
#include <X11/extensions/Xrender.h>
#include <X11/Xft/Xft.h>

/*
 * Xft
 */
__EXPORT__ extern const FontOps FontOps_xft;

typedef struct {
   XftFont            *font;
   Win                 win;
   Drawable            draw;
   XftDraw            *xftd;
   XftColor            xftc;
} FontCtxXft;

static int
_xft_Load(TextState * ts, const char *name)
{
   XftFont            *font;
   FontCtxXft         *fdc;

   if (name[0] == '-')
      font = XftFontOpenXlfd(disp, Dpy.screen, name);
   else
      font = XftFontOpenName(disp, Dpy.screen, name);

   if (!font)
      return -1;

#if 0				/* Debug */
   {
      FT_Face             ftf = XftLockFace(font);

      if (ftf == NULL)
	 return -1;
      Eprintf("Font %s family_name=%s style_name=%s\n", name,
	      ftf->family_name, ftf->style_name);
      XftUnlockFace(font);
   }
#endif

   fdc = EMALLOC(FontCtxXft, 1);
   if (!fdc)
      return -1;
   fdc->font = font;
   ts->fdc = fdc;
   ts->need_utf8 = 1;
   ts->type = FONT_TYPE_XFT;
   ts->ops = &FontOps_xft;
   return 0;
}

static void
_xft_Unload(TextState * ts)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;

   XftFontClose(disp, fdc->font);
}

static void
_xft_TextSize(TextState * ts, const char *text, int len,
	      int *width, int *height, int *ascent)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;
   XGlyphInfo          gi;

   if (len == 0)
      len = strlen(text);
   XftTextExtentsUtf8(disp, fdc->font, (const XftChar8 *)text, len, &gi);
   *width = gi.xOff;
   *height = fdc->font->height;
   if (*height < fdc->font->ascent + fdc->font->descent)
      *height = fdc->font->ascent + fdc->font->descent;
   *ascent = fdc->font->ascent;
#if 0
   Eprintf("asc/dsc/h=%d/%d/%d x,y=%2d,%d wxh=%dx%d ox,y=%3d,%d: (%d)%s\n",
	   fdc->font->ascent, fdc->font->descent, fdc->font->height, gi.x, gi.y,
	   gi.width, gi.height, gi.xOff, gi.yOff, len, text);
#endif
}

static void
_xft_TextDraw(TextState * ts, int x, int y, const char *text, int len)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;

   XftDrawStringUtf8(fdc->xftd, &(fdc->xftc), fdc->font, x, y,
		     (const XftChar8 *)text, len);
}

int
_xft_FdcInit(TextState * ts, Win win, Drawable draw)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;

   fdc->win = win;
   fdc->draw = draw;

   fdc->xftd = XftDrawCreate(disp, draw, WinGetVisual(win), WinGetCmap(win));
   if (!fdc->xftd)
      return -1;
   return 0;
}

void
_xft_FdcFini(TextState * ts)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;

   XftDrawDestroy(fdc->xftd);
}

void
_xft_FdcSetDrawable(TextState * ts, unsigned long draw)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;

   if (fdc->draw == draw)
      return;
   fdc->draw = draw;
   XftDrawChange(fdc->xftd, draw);
}

void
_xft_FdcSetColor(TextState * ts, unsigned int color)
{
   FontCtxXft         *fdc = (FontCtxXft *) ts->fdc;
   XRenderColor        xrc;

   COLOR32_TO_ARGB16(color, xrc.alpha, xrc.red, xrc.green, xrc.blue);

   XftColorAllocValue(disp, WinGetVisual(fdc->win), WinGetCmap(fdc->win),
		      &xrc, &(fdc->xftc));
}

const FontOps       FontOps_xft = {
   _xft_Load, _xft_Unload, _xft_TextSize, TextstateTextFit, _xft_TextDraw,
   _xft_FdcInit, _xft_FdcFini, _xft_FdcSetDrawable, _xft_FdcSetColor
};

#endif /* FONT_TYPE_XFT */

/*
 * Copyright (C) 2007-2010 Kim Woelders
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

#ifdef USE_PANGO
#include <X11/Xft/Xft.h>
#include <pango/pangoxft.h>
#include "tclass.h"
#include "xwin.h"

/*
 * Pango-Xft
 */
__EXPORT__ extern const FontOps FontOps_pango;

static PangoContext *_pango_ctx = NULL;

/* Beware! The layout of FontCtxPangoXft must match FontCtxXft
 * in order to reuse the _xft_Fdc... functions. */
typedef struct {
   PangoFontDescription *font;
   Win                 win;
   Drawable            draw;
   XftDraw            *xftd;
   XftColor            xftc;
} FontCtxPangoXft;

static int
_pango_xft_Load(TextState * ts, const char *name)
{
   FontCtxPangoXft    *fdc;
   PangoFontDescription *font;
   PangoFontMask       flags;

   if (!_pango_ctx)
      _pango_ctx = pango_xft_get_context(disp, Dpy.screen);
   if (!_pango_ctx)
      return -1;

   font = pango_font_description_from_string(name);
   if (!font)
      return -1;

   flags = pango_font_description_get_set_fields(font);
   if ((flags & PANGO_FONT_MASK_FAMILY) == 0)
      pango_font_description_set_family(font, "sans");
   if ((flags & PANGO_FONT_MASK_SIZE) == 0)
      pango_font_description_set_size(font, 10 * PANGO_SCALE);

   fdc = EMALLOC(FontCtxPangoXft, 1);
   if (!fdc)
      return -1;
   fdc->font = font;
   ts->fdc = fdc;
   ts->need_utf8 = 1;
   ts->type = FONT_TYPE_PANGO_XFT;
   ts->ops = &FontOps_pango;
   return 0;
}

static void
_pango_xft_Unload(TextState * ts)
{
   FontCtxPangoXft    *fdc = (FontCtxPangoXft *) ts->fdc;

   pango_font_description_free(fdc->font);
}

static void
_pango_xft_TextSize(TextState * ts, const char *text, int len __UNUSED__,
		    int *width, int *height, int *ascent)
{
   FontCtxPangoXft    *fdc = (FontCtxPangoXft *) ts->fdc;
   PangoLayout        *layout;
   PangoRectangle      logical_rect;

   layout = pango_layout_new(_pango_ctx);
   pango_layout_set_text(layout, text, -1);
   pango_layout_set_font_description(layout, fdc->font);
   pango_layout_get_extents(layout, NULL, &logical_rect);

   *width = PANGO_PIXELS(logical_rect.x + logical_rect.width);
   *height = PANGO_PIXELS(logical_rect.height);
   *ascent = PANGO_PIXELS(-logical_rect.y);

   g_object_unref(layout);
}

static void
_pango_xft_TextDraw(TextState * ts, int x, int y, const char *text,
		    int len __UNUSED__)
{
   FontCtxPangoXft    *fdc = (FontCtxPangoXft *) ts->fdc;
   PangoLayout        *layout;

   layout = pango_layout_new(_pango_ctx);
   pango_layout_set_text(layout, text, -1);
   pango_layout_set_font_description(layout, fdc->font);

   pango_xft_render_layout(fdc->xftd, &(fdc->xftc), layout,
			   x * PANGO_SCALE, y * PANGO_SCALE);

   g_object_unref(layout);
}

const FontOps       FontOps_pango = {
   _pango_xft_Load, _pango_xft_Unload,
   _pango_xft_TextSize, TextstateTextFit, _pango_xft_TextDraw,
   _xft_FdcInit, _xft_FdcFini, _xft_FdcSetDrawable, _xft_FdcSetColor
};

#endif /* USE_PANGO */

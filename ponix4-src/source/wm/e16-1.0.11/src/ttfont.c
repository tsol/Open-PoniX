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
#include "E.h"
#include "tclass.h"

#if FONT_TYPE_IFT
#include "eimage.h"
#include <Imlib2.h>

/*
 * Imlib2/FreeType
 */
__EXPORT__ extern const FontOps FontOps_ift;

typedef void        EFont;

static void
EFonts_Init(void)
{
#if !TEST_TTFONT
   char                s[4096];

   Esnprintf(s, sizeof(s), "%s/ttfonts", Mode.theme.path);
   imlib_add_path_to_font_path(s);
   Esnprintf(s, sizeof(s), "%s/fonts", EDirRoot());
   imlib_add_path_to_font_path(s);
#endif
}

static EFont       *
Efont_load(const char *file, int size)
{
   static char         ttfont_path_set = 0;
   char                s[4096];
   EFont              *f;

   if (!ttfont_path_set)
     {
	EFonts_Init();
	ttfont_path_set = 1;
     }

   Esnprintf(s, sizeof(s), "%s/%d", file, size);
   f = imlib_load_font(s);

   return f;
}

static void
Efont_free(EFont * f)
{
   imlib_context_set_font(f);
   imlib_free_font();
}

static void
Efont_extents(EFont * f, const char *text, int len __UNUSED__,
	      int *width, int *height, int *ascent)
{
   int                 h, asc, dsc;

   imlib_context_set_font(f);
   imlib_get_text_advance(text, width, &h);
   asc = imlib_get_font_ascent();
   dsc = imlib_get_font_descent();
   *height = asc + dsc;
   *ascent = asc;
}

static void
EFont_draw_string(EImage * im, EFont * f, int x, int y,
		  int r, int g, int b, const char *text)
{
   imlib_context_set_image(im);
   imlib_context_set_color(r, g, b, 255);
   imlib_context_set_font(f);
   imlib_text_draw(x, y - imlib_get_maximum_font_ascent(), text);
}

typedef struct {
   EFont              *font;
   EImage             *im;
   int                 r, g, b;
} FontCtxIft;

static int
_ift_Load(TextState * ts, const char *name __UNUSED__)
{
   EFont              *font;
   FontCtxIft         *fdc;
   char                s[1024], *ss;
   int                 len;

   ss = strchr(ts->fontname, '/');
   if (!ss)
      return -1;
   len = ss - ts->fontname;
   if (len > 1000)
      return -1;

   memcpy(s, ts->fontname, len);
   s[len] = '\0';
   font = Efont_load(s, atoi(ss + 1));
   if (!font)
      return -1;

   fdc = EMALLOC(FontCtxIft, 1);
   if (!fdc)
      return -1;
   fdc->font = font;
   ts->fdc = fdc;
   ts->need_utf8 = 1;
   ts->type = FONT_TYPE_IFT;
   ts->ops = &FontOps_ift;
   return 0;
}

static void
_ift_Unload(TextState * ts)
{
   FontCtxIft         *fdc = (FontCtxIft *) ts->fdc;

   Efont_free(fdc->font);
}

static void
_ift_TextSize(TextState * ts, const char *text, int len,
	      int *width, int *height, int *ascent)
{
   FontCtxIft         *fdc = (FontCtxIft *) ts->fdc;

   Efont_extents(fdc->font, text, len, width, height, ascent);
}

static void
_ift_TextDraw(TextState * ts, int x, int y, const char *text,
	      int len __UNUSED__)
{
   FontCtxIft         *fdc = (FontCtxIft *) ts->fdc;

   EFont_draw_string(fdc->im, fdc->font, x, y, fdc->r, fdc->g, fdc->b, text);
}

static int
_ift_FdcInit(TextState * ts __UNUSED__, Win win __UNUSED__,
	     Drawable draw __UNUSED__)
{
   return 0;
}

static void
_ift_FdcSetDrawable(TextState * ts, unsigned long draw)
{
   FontCtxIft         *fdc = (FontCtxIft *) ts->fdc;

   fdc->im = (EImage *) draw;
}

static void
_ift_FdcSetColor(TextState * ts, unsigned int color)
{
   FontCtxIft         *fdc = (FontCtxIft *) ts->fdc;

   COLOR32_TO_RGB(color, fdc->r, fdc->g, fdc->b);
}

const FontOps       FontOps_ift = {
   _ift_Load, _ift_Unload, _ift_TextSize, TextstateTextFit, _ift_TextDraw,
   _ift_FdcInit, NULL, _ift_FdcSetDrawable, _ift_FdcSetColor
};

#if TEST_TTFONT
#include <time.h>

Display            *disp;

int
main(int argc, char **argv)
{
   Efont              *f;
   GC                  gc;
   XGCValues           gcv;
   Window              win;
   int                 i, j;

   disp = XOpenDisplay(NULL);

   imlib_context_set_display(disp);
   imlib_context_set_visual(DefaultVisual(disp, DefaultScreen(disp)));
   imlib_context_set_colormap(DefaultColormap(disp, DefaultScreen(disp)));

   srand(time(NULL));
   win = XCreateSimpleWindow(disp, DefaultRootWindow(disp), 0, 0, 640, 480, 0,
			     0, 0);
   XMapWindow(disp, win);
   XSync(disp, False);

   gcv.subwindow_mode = IncludeInferiors;
   gc = XCreateGC(disp, win, GCSubwindowMode, &gcv);
   for (;; j++)
     {
	for (i = 3; i < argc; i++)
	  {
	     XSetForeground(disp, gc, (rand() << 16 | rand()) & 0xffffff);
	     f = Efont_load(argv[i], atoi(argv[1]));
	     if (f)
		EFont_draw_string(win, gc, 20, atoi(argv[1]) * (i - 2),
				  argv[2], f,
				  DefaultVisual(disp, DefaultScreen(disp)),
				  DefaultColormap(disp, DefaultScreen(disp)));
	     Efont_free(f);
	     f = NULL;
	  }
     }
   return 0;
}

#endif

#endif /* FONT_TYPE_IFT */

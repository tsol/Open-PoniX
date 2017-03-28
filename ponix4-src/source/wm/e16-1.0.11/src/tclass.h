/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2010 Kim Woelders
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
#ifndef _TCLASS_H
#define _TCLASS_H

#include "eimage.h"
#include "etypes.h"
#include "xwin.h"

#define FONT_TO_RIGHT     0
#define FONT_TO_DOWN      1
#define FONT_TO_UP        2
#define FONT_TO_LEFT      3

#define FONT_TYPE_XFONT         0	/* XFontStruct     */
#define FONT_TYPE_XFS           2	/* XFontSet        */
#define FONT_TYPE_IFT           3	/* Imlib2/FreeType */
#if USE_XFT
#define FONT_TYPE_XFT           4	/* Xft             */
#endif
#if USE_PANGO
#define FONT_TYPE_PANGO_XFT     5	/* Pango-Xft       */
#endif

typedef struct {
   int                 (*Load) (TextState * ts, const char *name);
   void                (*Destroy) (TextState * ts);
   void                (*TextSize) (TextState * ts, const char *text, int len,
				    int *width, int *height, int *ascent);
   void                (*TextFit) (TextState * ts, char **ptext,
				   int *pwidth, int textwidth_limit);
   void                (*TextDraw) (TextState * ts, int x, int y,
				    const char *text, int len);
   int                 (*FdcInit) (TextState * ts, Win win, Drawable draw);
   void                (*FdcFini) (TextState * ts);
   void                (*FdcSetDrawable) (TextState * ts, unsigned long draw);
   void                (*FdcSetColor) (TextState * ts, unsigned int color);
} FontOps;

struct _textstate {
   char               *fontname;
   char                type;
   char                need_utf8;
   struct {
      char                mode;
      char                orientation;
      char                effect;
   } style;
   unsigned int        fg_col;
   unsigned int        bg_col;
   void               *fdc;
   const FontOps      *ops;
};

struct _textclass {
   char               *name;
   struct {
      TextState          *normal;
      TextState          *hilited;
      TextState          *clicked;
      TextState          *disabled;
   } norm             , active, sticky, sticky_active;
   int                 justification;
   unsigned int        ref_count;
};

/* tclass.c */
int                 TextclassConfigLoad(FILE * fs);
TextClass          *TextclassFind(const char *name, int fallback);
TextClass          *TextclassAlloc(const char *name, int fallback);
void                TextclassFree(TextClass * tc);
int                 TextclassGetJustification(TextClass * tc);
void                TextclassSetJustification(TextClass * tc, int just);

/* text.c */
TextState          *TextclassGetTextState(TextClass * tclass, int state,
					  int active, int sticky);
__EXPORT__ void     TextstateTextFit(TextState * ts, char **ptext, int *pw,
				     int textwidth_limit);
void                TextstateTextDraw(TextState * ts, Win win, Drawable draw,
				      const char *text, int x, int y, int w,
				      int h, const EImageBorder * pad,
				      int fsize, int justh, int justv);
void                TextSize(TextClass * tclass, int active, int sticky,
			     int state, const char *text, int *width,
			     int *height, int fsize);
void                TextDraw(TextClass * tclass, Win win, Drawable draw,
			     int active, int sticky, int state,
			     const char *text, int x, int y, int w, int h,
			     int fsize, int justification);

__EXPORT__ int      _xft_FdcInit(TextState * ts, Win win, Drawable draw);
__EXPORT__ void     _xft_FdcFini(TextState * ts);
__EXPORT__ void     _xft_FdcSetDrawable(TextState * ts, unsigned long draw);
__EXPORT__ void     _xft_FdcSetColor(TextState * ts, unsigned int color);

#endif /* _TCLASS_H */

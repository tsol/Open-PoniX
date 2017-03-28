/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2007-2011 Kim Woelders
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
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include "dox.h"

static char       **
TextGetLines(const char *text, int *count)
{
   int                 i, j, k;
   char              **list = NULL;

   *count = 0;
   i = 0;
   k = 0;
   if (!text)
      return NULL;
   *count = 1;
   while (text[i])
     {
	j = i;
	while ((text[j]) && (text[j] != '\n'))
	   j++;
	k++;
	list = EREALLOC(char *, list, k);
	list[k - 1] = EMALLOC(char, j - i + 1);

	strncpy(list[k - 1], &(text[i]), (j - i));
	list[k - 1][j - i] = 0;
	i = j;
	if (text[i] == '\n')
	   i++;
     }
   *count = k;
   return list;
}

void
TextStateLoadFont(TextState * ts)
{
#if USE_XFONT
   if ((ts->efont) || (ts->xfont) || (ts->xfontset))
#else
   if ((ts->efont) || (ts->xfontset))
#endif
      return;

   if (!ts->fontname)
      return;

   if (!ts->efont)
     {
	char                s[4096], w[4046], *s2, *ss;

	s2 = NULL;
	s2 = strdup(ts->fontname);
	ss = strchr(s2, '/');
	if (ss)
	  {
	     *ss = ' ';
	     word(s2, 1, s);
	     word(s2, 2, w);
	     ts->efont = Efont_load(s, atoi(w));
	     if (ts->efont)
	       {
		  int                 as, ds;

		  Efont_extents(ts->efont, " ", &as, &ds, NULL, NULL, NULL,
				NULL, NULL);
		  ts->xfontset_ascent = as;
		  ts->height = as + ds;
	       }
	  }
	Efree(s2);
	if (ts->efont)
	   return;
     }

#if USE_XFONT
   if ((!ts->xfont) && (!strchr(ts->fontname, ',')))
     {
	ts->xfont = XLoadQueryFont(disp, ts->fontname);
	if (ts->xfont)
	  {
	     ts->xfontset_ascent = ts->xfont->ascent;
	     ts->height = ts->xfont->ascent + ts->xfont->descent;
	     return;
	  }
     }
#endif

   if (!ts->xfontset)
     {
	int                 i, missing_cnt, font_cnt;
	int                 descent;
	char              **missing_list, *def_str, **fn;
	XFontStruct       **fs;

	ts->xfontset = XCreateFontSet(disp, ts->fontname, &missing_list,
				      &missing_cnt, &def_str);
	if (!ts->xfontset)
	   ts->xfontset = XCreateFontSet(disp, "fixed", &missing_list,
					 &missing_cnt, &def_str);
	if (missing_cnt)
	   XFreeStringList(missing_list);
	if (!ts->xfontset)
	   return;
	font_cnt = XFontsOfFontSet(ts->xfontset, &fs, &fn);
	ts->xfontset_ascent = 0;
	for (i = 0; i < font_cnt; i++)
	   ts->xfontset_ascent = MAX(fs[i]->ascent, ts->xfontset_ascent);
	descent = 0;
	for (i = 0; i < font_cnt; i++)
	   descent = MAX(fs[i]->descent, descent);
	ts->height = ts->xfontset_ascent + descent;
     }
}

void
TextSize(TextState * ts, const char *text, int *width, int *height)
{
   char              **lines;
   int                 i, num_lines;

   *width = 0;
   *height = 0;

   lines = TextGetLines(text, &num_lines);
   if (!lines)
      return;

   if (!ts)
      return;

   TextStateLoadFont(ts);

   if (ts->efont)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     int                 ascent, descent, wid;

	     Efont_extents(ts->efont, lines[i], &ascent, &descent, &wid,
			   NULL, NULL, NULL, NULL);
	     *height += ascent + descent;
	     if (wid > *width)
		*width = wid;
	  }
     }
   else if (ts->xfontset)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     XRectangle          ret1, ret2;

	     XmbTextExtents(ts->xfontset, lines[i], strlen(lines[i]), &ret1,
			    &ret2);
	     *height += ret2.height;
	     if (ret2.width > *width)
		*width = ret2.width;
	  }
     }
#if USE_XFONT
   else if ((ts->xfont) && (ts->xfont->min_byte1 == 0) &&
	    (ts->xfont->max_byte1 == 0))
     {
	for (i = 0; i < num_lines; i++)
	  {
	     int                 wid;

	     wid = XTextWidth(ts->xfont, lines[i], strlen(lines[i]));
	     *height += ts->xfont->ascent + ts->xfont->descent;
	     if (wid > *width)
		*width = wid;
	  }
     }
   else if ((ts->xfont))
     {
	for (i = 0; i < num_lines; i++)
	  {
	     int                 wid;

	     wid = XTextWidth16(ts->xfont, (XChar2b *) lines[i],
				strlen(lines[i]) / 2);
	     *height += ts->xfont->ascent + ts->xfont->descent;
	     if (wid > *width)
		*width = wid;
	  }
     }
#endif /* USE_XFONT */
   freestrlist(lines, num_lines);
}

void
TextDraw(TextState * ts, Window win, char *text,
	 int x, int y, int w, int h __UNUSED__, int justification)
{
   char              **lines;
   int                 i, num_lines;
   int                 xx, yy;
   XGCValues           gcv;
   static GC           gc = 0;

   lines = TextGetLines(text, &num_lines);
   if (!lines)
      return;

   if (!ts)
      return;

   TextStateLoadFont(ts);
   xx = x;
   yy = y;

   if (!gc)
      gc = XCreateGC(disp, win, 0, &gcv);

   if (ts->efont)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     int                 ascent, descent, wid;

	     Efont_extents(ts->efont, lines[i], &ascent, &descent, &wid,
			   NULL, NULL, NULL, NULL);
	     if (i == 0)
		yy += ascent;
	     xx = x + (((w - wid) * justification) >> 10);
	     if (ts->effect == 1)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  EFont_draw_string(disp, win, gc, xx + 1, yy + 1,
				    lines[i], ts->efont, VRoot.vis, VRoot.cmap);
	       }
	     else if (ts->effect == 2)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  EFont_draw_string(disp, win, gc, xx - 1, yy,
				    lines[i], ts->efont, VRoot.vis, VRoot.cmap);
		  EFont_draw_string(disp, win, gc, xx + 1, yy,
				    lines[i], ts->efont, VRoot.vis, VRoot.cmap);
		  EFont_draw_string(disp, win, gc, xx, yy - 1,
				    lines[i], ts->efont, VRoot.vis, VRoot.cmap);
		  EFont_draw_string(disp, win, gc, xx, yy + 1,
				    lines[i], ts->efont, VRoot.vis, VRoot.cmap);
	       }
	     EAllocColor(&ts->fg_col);
	     XSetForeground(disp, gc, ts->fg_col.pixel);
	     EFont_draw_string(disp, win, gc, xx, yy,
			       lines[i], ts->efont, VRoot.vis, VRoot.cmap);
	     yy += ascent + descent;
	  }
     }
   else if (ts->xfontset)
     {
	for (i = 0; i < num_lines; i++)
	  {
	     XRectangle          ret1, ret2;

	     XmbTextExtents(ts->xfontset, lines[i], strlen(lines[i]), &ret1,
			    &ret2);
	     if (i == 0)
		yy += ts->xfontset_ascent;
	     xx = x + (((w - ret2.width) * justification) >> 10);
	     if (ts->effect == 1)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XmbDrawString(disp, win, ts->xfontset, gc, xx + 1, yy + 1,
				lines[i], strlen(lines[i]));
	       }
	     else if (ts->effect == 2)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XmbDrawString(disp, win, ts->xfontset, gc, xx - 1, yy,
				lines[i], strlen(lines[i]));
		  XmbDrawString(disp, win, ts->xfontset, gc, xx + 1, yy,
				lines[i], strlen(lines[i]));
		  XmbDrawString(disp, win, ts->xfontset, gc, xx, yy - 1,
				lines[i], strlen(lines[i]));
		  XmbDrawString(disp, win, ts->xfontset, gc, xx, yy + 1,
				lines[i], strlen(lines[i]));
	       }
	     EAllocColor(&ts->fg_col);
	     XSetForeground(disp, gc, ts->fg_col.pixel);
	     XmbDrawString(disp, win, ts->xfontset, gc, xx, yy,
			   lines[i], strlen(lines[i]));
	     yy += ret2.height;
	  }
     }
#if USE_XFONT
   else if ((ts->xfont) && (ts->xfont->min_byte1 == 0) &&
	    (ts->xfont->max_byte1 == 0))
     {
	XSetFont(disp, gc, ts->xfont->fid);
	for (i = 0; i < num_lines; i++)
	  {
	     int                 wid;

	     wid = XTextWidth(ts->xfont, lines[i], strlen(lines[i]));
	     if (i == 0)
		yy += ts->xfont->ascent;
	     xx = x + (((w - wid) * justification) >> 10);
	     if (ts->effect == 1)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XDrawString(disp, win, gc, xx + 1, yy + 1,
			      lines[i], strlen(lines[i]));
	       }
	     else if (ts->effect == 2)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XDrawString(disp, win, gc, xx - 1, yy,
			      lines[i], strlen(lines[i]));
		  XDrawString(disp, win, gc, xx + 1, yy,
			      lines[i], strlen(lines[i]));
		  XDrawString(disp, win, gc, xx, yy - 1,
			      lines[i], strlen(lines[i]));
		  XDrawString(disp, win, gc, xx, yy + 1,
			      lines[i], strlen(lines[i]));
	       }
	     EAllocColor(&ts->fg_col);
	     XSetForeground(disp, gc, ts->fg_col.pixel);
	     XDrawString(disp, win, gc, xx, yy, lines[i], strlen(lines[i]));
	     yy += ts->xfont->ascent + ts->xfont->descent;
	  }
     }
   else if ((ts->xfont))
     {
	XSetFont(disp, gc, ts->xfont->fid);
	for (i = 0; i < num_lines; i++)
	  {
	     int                 wid;

	     wid = XTextWidth16(ts->xfont, (XChar2b *) lines[i],
				strlen(lines[i]) / 2);
	     if (i == 0)
		yy += ts->xfont->ascent;
	     xx = x + (((w - wid) * justification) >> 10);
	     if (ts->effect == 1)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XDrawString16(disp, win, gc, xx + 1, yy + 1,
				(XChar2b *) lines[i], strlen(lines[i]) / 2);
	       }
	     else if (ts->effect == 2)
	       {
		  EAllocColor(&ts->bg_col);
		  XSetForeground(disp, gc, ts->bg_col.pixel);
		  XDrawString16(disp, win, gc, xx - 1, yy,
				(XChar2b *) lines[i], strlen(lines[i]) / 2);
		  XDrawString16(disp, win, gc, xx + 1, yy,
				(XChar2b *) lines[i], strlen(lines[i]) / 2);
		  XDrawString16(disp, win, gc, xx, yy - 1,
				(XChar2b *) lines[i], strlen(lines[i]) / 2);
		  XDrawString16(disp, win, gc, xx, yy + 1,
				(XChar2b *) lines[i], strlen(lines[i]) / 2);
	       }
	     EAllocColor(&ts->fg_col);
	     XSetForeground(disp, gc, ts->fg_col.pixel);
	     XDrawString16(disp, win, gc, xx, yy,
			   (XChar2b *) lines[i], strlen(lines[i]) / 2);
	     yy += ts->xfont->ascent + ts->xfont->descent;
	  }
     }
#endif /* USE_XFONT */
   freestrlist(lines, num_lines);
}

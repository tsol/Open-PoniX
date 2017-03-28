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

#define DEFAULT_LINKCOLOR_R 30
#define DEFAULT_LINKCOLOR_G 50
#define DEFAULT_LINKCOLOR_B 160

typedef enum _type {
   IMG,
   BR,
   FONT,
   P,
   TEXT,
   PAGE
} Type;

typedef struct _img {
   char               *src;
   char               *src2;
   char               *src3;
   int                 x, y;
   char               *link;
   int                 w, h;
} Img_;

typedef struct _font {
   char               *face;
   int                 r, g, b;
} Font_;

typedef struct _p {
   float               align;
} P_;

typedef struct _object {
   Type                type;
   void               *object;
} Object;

typedef struct _page {
   char               *name;
   int                 count;
   Object             *obj;
   int                 columns;
   int                 padding;
   int                 linkr, linkg, linkb;
   char               *background;
} Page;

static int          num_pages = 0;
static Page        *page = NULL;
static char        *fdat_ptr = NULL;
static int          fdat_size = 0;
static char        *fdat = NULL;

static int
fdgetc(void)
{
   int                 val;

   if (fdat_ptr >= (fdat + fdat_size))
      return EOF;
   val = (int)(*fdat_ptr);
   fdat_ptr++;
   return val;
}

static void
fdjump(int count)
{
   fdat_ptr += count;
   if (fdat_ptr < fdat)
      fdat_ptr = fdat;
   if (fdat_ptr >= (fdat + fdat_size))
      fdat_ptr = (fdat + fdat_size) - 1;
}

static void
AddPage(Object * obj)
{
   num_pages++;
   page = EREALLOC(Page, page, num_pages);
   page[num_pages - 1].name = NULL;
   page[num_pages - 1].count = 0;
   page[num_pages - 1].obj = NULL;
   page[num_pages - 1].columns = 1;
   page[num_pages - 1].background = NULL;
   page[num_pages - 1].padding = 2;
   page[num_pages - 1].linkr = DEFAULT_LINKCOLOR_R;
   page[num_pages - 1].linkg = DEFAULT_LINKCOLOR_G;
   page[num_pages - 1].linkb = DEFAULT_LINKCOLOR_B;

   if ((obj) && (obj->type == PAGE))
     {
	Page               *pg;

	pg = (Page *) (obj->object);
	if (pg->name)
	   page[num_pages - 1].name = pg->name;
	page[num_pages - 1].columns = pg->columns;
	page[num_pages - 1].padding = pg->padding;
	page[num_pages - 1].linkr = pg->linkr;
	page[num_pages - 1].linkg = pg->linkg;
	page[num_pages - 1].linkb = pg->linkb;
	if (pg->background)
	   page[num_pages - 1].background = pg->background;
     }
}

static void
AddObject(Object * obj)
{
   page[num_pages - 1].count++;
   page[num_pages - 1].obj =
      EREALLOC(Object, page[num_pages - 1].obj, page[num_pages - 1].count);
   page[num_pages - 1].obj[page[num_pages - 1].count - 1].type = obj->type;
   page[num_pages - 1].obj[page[num_pages - 1].count - 1].object = obj->object;
}

static void
BuildObj(Object * obj, char *var, char *param)
{
   static Page        *pg = NULL;
   static P_          *p = NULL;
   static Font_       *fn = NULL;
   static Img_        *img = NULL;

   switch (obj->type)
     {
     case IMG:
	if (!obj->object)
	  {
	     img = EMALLOC(Img_, 1);
	     obj->object = img;
	     img->src = NULL;
	     img->src2 = NULL;
	     img->src3 = NULL;
	     img->x = 0;
	     img->y = 0;
	     img->link = NULL;
	     img->w = 0;
	     img->h = 0;
	  }
	if (!strcmp(var, "x"))
	   img->x = atoi(param);
	else if (!strcmp(var, "y"))
	   img->y = atoi(param);
	else if (!strcmp(var, "src"))
	   img->src = strdup(param);
	else if (!strcmp(var, "src2"))
	   img->src2 = strdup(param);
	else if (!strcmp(var, "src3"))
	   img->src3 = strdup(param);
	else if (!strcmp(var, "href"))
	   img->link = strdup(param);
	break;
     case BR:
	break;
     case FONT:
	if (!obj->object)
	  {
	     fn = EMALLOC(Font_, 1);
	     obj->object = fn;
	     fn->face = NULL;
	     fn->r = 0;
	     fn->g = 0;
	     fn->b = 0;
	  }
	if (!strcmp(var, "face"))
	   fn->face = strdup(param);
	else if (!strcmp(var, "color"))
	  {
	     char                hex[3] = "00";

	     if (param[0] == '#')
	       {
		  hex[0] = param[1];
		  hex[1] = param[2];
		  sscanf(hex, "%x", &(fn->r));
		  hex[0] = param[3];
		  hex[1] = param[4];
		  sscanf(hex, "%x", &(fn->g));
		  hex[0] = param[5];
		  hex[1] = param[6];
		  sscanf(hex, "%x", &(fn->b));
	       }
	  }
	break;
     case P:
	if (!obj->object)
	  {
	     p = EMALLOC(P_, 1);
	     obj->object = p;
	     p->align = 0;
	  }
	if (!strcmp(var, "align"))
	  {
	     if ((strlen(param) > 0) && (param[strlen(param) - 1] == '%'))
		param[strlen(param) - 1] = 0;
	     p->align = atof(param);
	  }
	break;
     case TEXT:
	break;
     case PAGE:
	if (!obj->object)
	  {
	     pg = EMALLOC(Page, 1);
	     obj->object = pg;
	     pg->columns = 1;
	     pg->padding = 1;
	     pg->name = NULL;
	     pg->background = NULL;
	     pg->linkr = DEFAULT_LINKCOLOR_R;
	     pg->linkg = DEFAULT_LINKCOLOR_G;
	     pg->linkb = DEFAULT_LINKCOLOR_B;
	  }
	if (!strcmp(var, "columns"))
	   pg->columns = atoi(param);
	else if (!strcmp(var, "padding"))
	   pg->padding = atoi(param);
	else if (!strcmp(var, "name"))
	   pg->name = strdup(param);
	else if (!strcmp(var, "background"))
	   pg->background = strdup(param);
	else if (!strcmp(var, "linkcolor"))
	  {
	     char                hex[3] = "00";

	     if (param[0] == '#')
	       {
		  hex[0] = param[1];
		  hex[1] = param[2];
		  sscanf(hex, "%x", &(pg->linkr));
		  hex[0] = param[3];
		  hex[1] = param[4];
		  sscanf(hex, "%x", &(pg->linkg));
		  hex[0] = param[5];
		  hex[1] = param[6];
		  sscanf(hex, "%x", &(pg->linkb));
	       }
	  }
	break;
     default:
	break;
     }
}

static int
GetNextTag(Object * obj)
{
   char                s[65536];
   int                 i = 0, wd = 0;
   int                 val;
   char                intag = 0;
   char                havobj = 0;

   for (;;)
     {
	val = fdgetc();
	if (val == EOF)
	   return 0;
	if (intag)
	  {
	     if (val == '>')
		intag = 0;
	     s[i++] = (char)val;
	     if (s[i - 1] == '\n')
		s[i - 1] = ' ';
	     if (s[i - 1] == '>')
		s[i - 1] = ' ';
	     if (s[i - 1] == ' ')
	       {
		  if (i == 1)
		     i = 0;
		  else
		    {
		       s[i - 1] = 0;
		       if (!havobj)
			 {
			    if (wd == 0)
			      {
				 if (!strcmp(s, "page"))
				    obj->type = PAGE;
				 else if (!strcmp(s, "img"))
				    obj->type = IMG;
				 else if (!strcmp(s, "br"))
				    obj->type = BR;
				 else if (!strcmp(s, "font"))
				    obj->type = FONT;
				 else if (!strcmp(s, "p"))
				    obj->type = P;
				 havobj = 1;
			      }
			    i = 0;
			 }
		       else
			 {
			    char                w1[1024];
			    char                w2[1024];
			    int                 j = 0;

			    w1[0] = 0;
			    w2[0] = 0;
			    while ((s[j]) && (s[j] != '='))
			      {
				 w1[j] = s[j];
				 j++;
			      }
			    w1[j] = 0;
			    if (j < (int)strlen(s))
			       strcpy(w2, &(s[j + 1]));
			    BuildObj(obj, w1, w2);
			    i = 0;
			 }
		       wd++;
		    }
	       }
	     if (!intag)
		return 1;
	  }
	if (val == '<')
	   intag = 1;
     }
   return 1;
}

static char        *
GetTextUntilTag(void)
{
   char                s[65536];
   int                 i = 0;
   int                 val;

   for (;;)
     {
	val = fdgetc();
	if (val == EOF)
	  {
	     s[i] = 0;
	     if (strlen(s) < 1)
		return NULL;
	     return strdup(s);
	  }
	s[i++] = (char)val;
	if (s[i - 1] == '\n')
	   s[i - 1] = ' ';
	if ((i == 1) && (s[0] == ' '))
	   i--;
	else if (s[i - 1] == '<')
	  {
	     s[i - 1] = 0;
	     fdjump(-1);
	     if (strlen(s) < 1)
		return NULL;
	     return strdup(s);
	  }
	if ((i > 2) && (s[i - 2] == ' ') && (s[i - 1] == ' '))
	   i--;
	if (i > 65530)
	   return NULL;
     }
   return NULL;
}

int
GetObjects(FILE * f)
{
   static char         have_font = 0;
   static char         in_para = 0;
   Object              obj;
   char               *txt;
   char                buf[4096];
   int                 count;

   fdat = NULL;
   fdat_size = 0;
   while ((count = fread(buf, 1, 4096, f)) > 0)
     {
	if (!fdat)
	   fdat = EMALLOC(char, count);

	else
	   fdat = EREALLOC(char, fdat, fdat_size + count);

	memcpy(fdat + fdat_size, buf, count);
	fdat_size += count;
     }
   fdat_ptr = fdat;

   if (page)
     {
	int                 i;

	for (i = 0; i < num_pages; i++)
	  {
	     int                 j;

	     Efree(page[i].name);
	     Efree(page[i].background);
	     for (j = 0; j < page[i].count; j++)
	       {
		  switch (page[i].obj[j].type)
		    {
		    case IMG:
		       Efree(((Img_ *) page[i].obj[j].object)->src);
		       Efree(((Img_ *) page[i].obj[j].object)->src2);
		       Efree(((Img_ *) page[i].obj[j].object)->src3);
		       Efree(((Img_ *) page[i].obj[j].object)->link);
		       break;
		    case BR:
		       break;
		    case FONT:
		       Efree(((Font_ *) page[i].obj[j].object)->face);
		       break;
		    case P:
		       break;
		    case TEXT:
		       break;
		    case PAGE:
		       break;
		    }
		  Efree(page[i].obj[j].object);
	       }
	     Efree(page[i].obj);
	  }
	Efree(page);
	num_pages = 0;
	page = NULL;
	have_font = 0;
	in_para = 0;
     }

   obj.object = NULL;
   for (;;)
     {
	if ((have_font) && (in_para))
	  {
	     txt = GetTextUntilTag();
	     if (txt)
	       {
		  obj.type = TEXT;
		  obj.object = (void *)txt;
	       }
	     else
	       {
		  if (!GetNextTag(&obj))
		    {
		       Efree(fdat);
		       return 0;
		    }
	       }
	  }
	else
	  {
	     if (!GetNextTag(&obj))
	       {
		  Efree(fdat);
		  return 0;
	       }
	  }
	if (obj.type == PAGE)
	  {
	     in_para = 0;
	     have_font = 0;
	     AddPage(&obj);
	  }
	else if (page)
	   AddObject(&obj);
	if (obj.type == IMG)
	   in_para = 0;
	if (obj.type == P)
	   in_para = 1;
	if (obj.type == FONT)
	   have_font = 1;
	obj.object = NULL;
     }
   Efree(fdat);
}

int
FixPage(int p)
{
   if (p < 0)
      return 0;
   if (p >= num_pages)
      return num_pages - 1;
   return p;
}

int
GetPage(char *name)
{
   int                 i;

   for (i = 0; i < num_pages; i++)
     {
	if ((page[i].name) && (!strcmp(name, page[i].name)))
	   return i;
     }
   return -1;
}

void
GetLinkColors(int page_num, int *r, int *g, int *b)
{
   if (page_num < 0)
     {
	*r = DEFAULT_LINKCOLOR_R;
	*g = DEFAULT_LINKCOLOR_G;
	*b = DEFAULT_LINKCOLOR_B;
     }
   else
     {
	*r = page[page_num].linkr;
	*g = page[page_num].linkg;
	*b = page[page_num].linkb;
     }
}

static void
CalcOffset(Page * pg, int col_w, int x, int y, int th, int *pxspace, int *poff)
{
   int                 xspace, off;
   int                 sx, sy, ssx, ssy;
   Img_               *img;
   int                 j;

   xspace = col_w;
   off = 0;
   sx = x + off;
   sy = y;
   ssx = sx + col_w - 1;
   ssy = sy + th - 1;
   for (j = 0; j < pg->count; j++)
     {
	if (pg->obj[j].type != IMG)
	   continue;

	img = (Img_ *) pg->obj[j].object;
	if ((img->w > 0) && (img->h > 0))
	  {
	     int                 ix, iy, iix, iiy;

	     ix = img->x;
	     iy = img->y;
	     iix = img->x + img->w - 1;
	     iiy = img->y + img->h - 1;

	     if ((iy <= ssy) && (iiy >= sy))
	       {
		  if ((ix >= sx) && (ix <= ssx))
		    {
		       if ((iix >= sx) && (iix <= ssx))
			 {
			    if (((ix + iix) / 2) > ((sx + ssx) / 2))
			       ssx = ix - 1;
			    else
			       sx = iix + 1;
			 }
		       else
			 {
			    ssx = ix - 1;
			 }
		    }
		  else if ((iix >= sx) && (iix <= ssx))
		    {
		       sx = iix + 1;
		    }
	       }
	  }
     }
   off = sx - x;
   xspace = (ssx - sx) + 1;
   if (xspace < 0)
      xspace = 0;

   *pxspace = xspace;
   *poff = off;
}

Link               *
RenderPage(Window win, int page_num, int w, int h)
{
   Link               *ll = NULL;
   Page               *pg;
   TextState           ts;
   int                 i, col_w;
   int                 x, y;
   int                 justification = 0;
   int                 firstp = 1;
   Imlib_Image         im;
   int                 wastext = 0;

   memset(&ts, 0, sizeof(ts));
   pg = &(page[page_num]);
   x = pg->padding;
   y = pg->padding;
   col_w = ((w - (pg->padding * (pg->columns + 1))) / pg->columns);

   if (pg->background)
     {
	im = ImageLoadDoc(pg->background);
	if (im)
	  {
	     imlib_context_set_image(im);
	     imlib_context_set_drawable(win);
	     imlib_render_image_on_drawable_at_size(0, 0, w, h);
	     imlib_free_image();
	  }
     }
   for (i = 0; i < pg->count; i++)
     {
	char                s[32768], ss[32768], wd[4096], *txt;
	Img_               *img;
	Font_              *fn;
	P_                 *p;
	int                 wc, eol, eot;
	int                 islink = 0, lx, lw;

	switch (pg->obj[i].type)
	  {
	  case IMG:
	     img = (Img_ *) pg->obj[i].object;
	     if (img->src)
	       {
		  im = ImageLoadDoc(img->src);
		  if (im)
		    {
		       imlib_context_set_image(im);
		       img->w = imlib_image_get_width();
		       img->h = imlib_image_get_height();
		       imlib_context_set_drawable(win);
		       imlib_render_image_on_drawable_at_size(img->x, img->y,
							      img->w, img->h);
		       imlib_free_image();
		    }
		  if (img->link)
		    {
		       Link               *l;

		       l = EMALLOC(Link, 1);
		       l->name = strdup(img->link);
		       l->x = img->x;
		       l->y = img->y;
		       l->w = img->w;
		       l->h = img->h;
		       l->next = ll;
		       ll = l;
		    }
	       }
	     break;
	  case BR:
	     if (!wastext)
		y += ts.height;
	     wastext = 0;
	     break;
	  case FONT:
	     fn = (Font_ *) pg->obj[i].object;
	     ts.fontname = NULL;
	     ESetColor(&ts.fg_col, 0, 0, 0);
	     ESetColor(&ts.bg_col, 0, 0, 0);
	     ts.effect = 0;
	     if (ts.efont)
		Efont_free(ts.efont);
	     ts.efont = NULL;
#if USE_XFONT
	     if (ts.xfont)
		XFreeFont(disp, ts.xfont);
	     ts.xfont = NULL;
#endif
	     if (ts.xfontset)
		XFreeFontSet(disp, ts.xfontset);
	     ts.xfontset = NULL;
	     ts.xfontset_ascent = 0;
	     ts.height = 0;
	     ts.fontname = fn->face;
	     ESetColor(&ts.fg_col, fn->r, fn->g, fn->b);
	     TextStateLoadFont(&ts);
	     break;
	  case P:
	     p = (P_ *) pg->obj[i].object;
	     if (p)
		justification = (int)((p->align / 100) * 1024);
	     else
		justification = 0;
	     if (!firstp)
		y += ts.height;
	     else
		firstp = 0;
	     break;
	  case TEXT:
	     txt = (char *)pg->obj[i].object;
	     wc = 1;
	     ss[0] = 0;
	     s[0] = 0;
	     eol = 0;
	     eot = 0;
	     for (;;)
	       {
		  char               *txt_disp;
		  int                 tw, th, xspace;
		  int                 off, j;
		  char                link_txt[1024];
		  char                link_link[1024];
		  int                 spaceflag, oldwc = 0, linkwc;

		  wd[0] = 0;
#ifdef USE_WORD_MB
		  if (MB_CUR_MAX > 1)	/* If multibyte locale,... */
		     word_mb(txt, wc, wd, &spaceflag);
		  else
#endif
		    {
		       word(txt, wc, wd);
		       spaceflag = 1;
		    }
		  if (!wd[0])
		     eol = 1;

		  wc++;
		  eot++;
		  strcpy(ss, s);
		  if ((eot != 1) && spaceflag)
		     strcat(s, " ");

		  linkwc = -1;
		  if (wd[0] == '_')
		    {
		       link_txt[0] = '\0';
		       link_link[0] = '\0';
		       islink = 1;
		       oldwc = wc;
		       TextSize(&ts, s, &lx, &th);
		    }

		  if (islink == 1)
		    {
		       if (eol || ((wd[0] != '_') && spaceflag))	/* if NO link tag, ... */
			 {
			    link_txt[0] = '\0';
			    link_link[0] = '\0';
			    islink = 0;
			    wc = oldwc;
#ifdef USE_WORD_MB
			    if (MB_CUR_MAX > 1)
			       word_mb(txt, wc - 1, wd, &spaceflag);
			    else
#endif
			      {
				 word(txt, wc - 1, wd);
				 spaceflag = 1;
			      }
			 }
		       else
			 {
			    int                 k, linkflg;

			    j = 0;
			    linkflg = 0;
			    if (wd[0] == '_')
			      {
				 j++;
				 linkflg++;
			      }

			    k = strlen(link_txt);
			    for (; wd[j] != '(' && wd[j] != '\0'; j++, k++)
			      {
				 if (wd[j] == '_')
				    link_txt[k] = ' ';
				 else
				    link_txt[k] = wd[j];
				 if (linkflg)
				    wd[j - 1] = link_txt[k];
				 else
				    wd[j] = link_txt[k];
			      }
			    link_txt[k] = '\0';
			    if (linkflg)
			       wd[j - 1] = '\0';

			    if (wd[j] == '(')
			      {
				 wd[j++] = '\0';
				 strcpy(link_link, wd + j);
				 k = strlen(link_link) - 1;
				 if (k <= 0)
				    continue;
				 for (j = k; j > 0 && link_link[j] != ')'; j--)
				    ;
				 link_link[j] = '\0';
				 strcpy(wd, link_txt);
				 if (j < k)
				    strcat(wd, link_link + j + 1);
				 islink = 2;
				 linkwc = wc;
			      }
			    else
			       continue;
			 }
		    }

		  strcat(s, wd);

		  CalcOffset(pg, col_w, x, y, ts.height, &xspace, &off);

		  TextSize(&ts, s, &tw, &th);
		  txt_disp = ss;
		  if (eot == 1)
		     txt_disp = s;
		  if (((tw > xspace) || (eol)) && (strlen(txt_disp) > 0))
		    {
		       if (txt_disp[strlen(txt_disp) - 1] == ' ')
			  txt_disp[strlen(txt_disp) - 1] = 0;

		       if ((eot == 1) && (tw > xspace))
			 {
			    char                p1[4096];
			    int                 point = 0, cnt = 0, ii, len;

			    while (txt_disp[(point + cnt)])
			      {
				 len =
				    mblen(txt_disp + point + cnt, MB_CUR_MAX);
				 if (len < 0)
				   {
				      cnt++;
				      continue;
				   }
				 else
				    for (ii = 0; ii < len; ii++, cnt++)
				       p1[cnt] = txt_disp[point + cnt];
				 p1[cnt] = 0;
				 TextSize(&ts, p1, &tw, &th);
				 if ((tw > xspace)
				     || (!txt_disp[(point + cnt)]))
				   {
				      if (txt_disp[(point + cnt)])
					{
					   point = point + cnt - len;
					   p1[cnt - len] = 0;
					   cnt = 0;
					}
				      else
					{
					   point = point + cnt;
					   p1[cnt] = 0;
					   cnt = 0;
					}
				      wastext = 1;
				      TextDraw(&ts, win, p1, x + off, y,
					       xspace, 99999, justification);
				      y += ts.height;
				      if (y >=
					  (h -
					   (pg->padding + ts.height -
					    (ts.height - ts.xfontset_ascent))))
					{
					   y = pg->padding;
					   x += col_w + pg->padding;
					}
				      CalcOffset(pg, col_w, x, y, ts.height,
						 &xspace, &off);
				   }
			      }
			 }
		       else
			 {
			    if ((tw > xspace) && (eot != 1))
			       wc--;
			    wastext = 1;
			    TextDraw(&ts, win, txt_disp, x + off, y,
				     xspace, 99999, justification);
			    if (islink > 1 && linkwc > wc)
			      {
				 islink = 0;
				 link_link[0] = '\0';
				 link_txt[0] = '\0';
				 wc = oldwc - 1;
			      }

			    if (islink > 1)
			      {
				 int                 rr, gg, bb;
				 int                 extra;
				 GC                  gc;
				 XGCValues           gcv;

				 gc = XCreateGC(disp, win, 0, &gcv);
				 EGetColor(&ts.fg_col, &rr, &gg, &bb);
				 ESetColor(&ts.fg_col, pg->linkr, pg->linkg,
					   pg->linkb);
				 EAllocColor(&ts.fg_col);
				 XSetForeground(disp, gc, ts.fg_col.pixel);
				 TextSize(&ts, txt_disp, &tw, &th);
				 extra = ((xspace - tw) * justification) >> 10;
				 TextDraw(&ts, win, link_txt,
					  x + off + lx + extra, y, 99999, 99999,
					  0);
				 TextSize(&ts, link_txt, &lw, &th);
				 XDrawLine(disp, win, gc,
					   x + off + lx + extra,
					   y + ts.xfontset_ascent,
					   x + off + lx + lw + extra,
					   y + ts.xfontset_ascent);
				 ESetColor(&ts.fg_col, rr, gg, bb);
				 islink = 0;
				 XFreeGC(disp, gc);
				 {
				    Link               *l;

				    l = EMALLOC(Link, 1);
				    l->name = strdup(link_link);
				    l->x = x + off + lx + extra;
				    l->y = y;
				    l->w = lw;
				    l->h = ts.height;
				    l->next = ll;
				    ll = l;
				 }
				 link_link[0] = '\0';
				 link_txt[0] = '\0';
			      }
			    y += ts.height;
			    if (y >=
				(h -
				 (pg->padding + ts.height -
				  (ts.height - ts.xfontset_ascent))))
			      {
				 y = pg->padding;
				 x += col_w + pg->padding;
			      }
			 }
		       eot = 0;
		       s[0] = 0;
		    }
		  if (eol)
		     break;
	       }

	     break;
	  default:
	     break;
	  }
	if (y >=
	    (h - (pg->padding + ts.height - (ts.height - ts.xfontset_ascent))))
	  {
	     y = pg->padding;
	     x += col_w + pg->padding;
	  }
     }

   if (ts.efont)
      Efont_free(ts.efont);
#if USE_XFONT
   if (ts.xfont)
      XFreeFont(disp, ts.xfont);
#endif
   if (ts.xfontset)
      XFreeFontSet(disp, ts.xfontset);

   return ll;
}

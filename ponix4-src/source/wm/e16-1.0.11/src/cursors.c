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
#include "E.h"
#include "conf.h"
#include "cursors.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "xwin.h"
#include <X11/cursorfont.h>
#if USE_XRENDER
#include <X11/extensions/Xrender.h>
#endif

struct _ecursor {
   char               *name;
   Cursor              cursor;
   unsigned int        ref_count;
   char               *file;
   unsigned int        bg;
   unsigned int        fg;
   int                 native_id;
};

static Ecore_List  *cursor_list = NULL;

#if USE_XRENDER
/* Assuming we have XRenderCreateCursor (render >= 0.5) */
static              Cursor
ECreatePixmapCursor(Pixmap cpmap, Pixmap cmask, unsigned int w, unsigned int h,
		    int xh, int yh, unsigned int fg, unsigned int bg)
{
   Cursor              curs;
   Pixmap              pmap;
   XGCValues           gcv;
   GC                  gc;
   Picture             pict;
   XRenderPictFormat  *pictfmt;

   pictfmt = XRenderFindStandardFormat(disp, PictStandardARGB32);

   pmap = XCreatePixmap(disp, WinGetXwin(VROOT), w, h, 32);

   gcv.foreground = 0;
   gc = EXCreateGC(pmap, GCForeground, &gcv);
   XFillRectangle(disp, pmap, gc, 0, 0, w, h);
   gcv.fill_style = FillOpaqueStippled;
   gcv.stipple = cpmap;
   gcv.clip_mask = cmask;
   gcv.foreground = fg;
   gcv.background = bg;
   XChangeGC(disp, gc,
	     GCForeground | GCBackground | GCFillStyle | GCStipple | GCClipMask,
	     &gcv);
   XFillRectangle(disp, pmap, gc, 0, 0, w, h);
   EXFreeGC(gc);

   pict = XRenderCreatePicture(disp, pmap, pictfmt, 0, 0);
   curs = XRenderCreateCursor(disp, pict, xh, yh);

   XFreePixmap(disp, pmap);
   XRenderFreePicture(disp, pict);

   return curs;
}
#else
static              Cursor
ECreatePixmapCursor(Pixmap cpmap, Pixmap cmask,
		    unsigned int w __UNUSED__, unsigned int h __UNUSED__,
		    int xh, int yh, unsigned int fg, unsigned int bg)
{
   Cursor              curs;
   XColor              fgxc, bgxc;

   COLOR32_TO_RGB16(fg, fgxc.red, fgxc.green, fgxc.blue);
   COLOR32_TO_RGB16(bg, bgxc.red, bgxc.green, bgxc.blue);
   XAllocColor(disp, WinGetCmap(VROOT), &fgxc);
   XAllocColor(disp, WinGetCmap(VROOT), &bgxc);

   curs = XCreatePixmapCursor(disp, cpmap, cmask, &fgxc, &bgxc, xh, yh);

   return curs;
}
#endif

static void
ECursorCreate(const char *name, const char *image, int native_id,
	      unsigned int fg, unsigned int bg)
{
   ECursor            *ec;

   if ((!name) || (!image && native_id == -1))
      return;

   ec = ECALLOC(ECursor, 1);
   if (!ec)
      return;

   ec->name = Estrdup(name);

   ec->file = Estrdup(image);
   ec->fg = fg;
   ec->bg = bg;
   ec->native_id = native_id;

   if (!cursor_list)
      cursor_list = ecore_list_new();
   ecore_list_prepend(cursor_list, ec);
}

static void
ECursorDestroy(ECursor * ec)
{
   if (!ec)
      return;

   if (ec->ref_count > 0)
     {
	DialogOK("ECursor Error!", _("%u references remain"), ec->ref_count);
	return;
     }

   ecore_list_node_remove(cursor_list, ec);

   Efree(ec->name);
   Efree(ec->file);

   Efree(ec);
}

static ECursor     *
ECursorRealize(ECursor * ec)
{
   Pixmap              pmap, mask;
   int                 xh, yh;
   unsigned int        w, h, ww, hh;
   char               *img, msk[FILEPATH_LEN_MAX];

   if (ec->file)
     {
	img = ThemeFileFind(ec->file, FILE_TYPE_CURSOR);
	_EFREE(ec->file);	/* Ok or not - we never need file again */
	if (!img)
	   goto done;

	Esnprintf(msk, sizeof(msk), "%s.mask", img);
	pmap = 0;
	mask = 0;
	xh = 0;
	yh = 0;
	XReadBitmapFile(disp, WinGetXwin(VROOT), msk, &w, &h, &mask, &xh, &yh);
	XReadBitmapFile(disp, WinGetXwin(VROOT), img, &w, &h, &pmap, &xh, &yh);
	XQueryBestCursor(disp, WinGetXwin(VROOT), w, h, &ww, &hh);
	if ((w <= ww) && (h <= hh) && (pmap))
	  {
	     if (xh < 0 || xh >= (int)w)
		xh = (int)w / 2;
	     if (yh < 0 || yh >= (int)h)
		yh = (int)h / 2;
	     ec->cursor =
		ECreatePixmapCursor(pmap, mask, w, h, xh, yh, ec->fg, ec->bg);
	  }

	if (ec->cursor == None)
	  {
	     Eprintf("*** Failed to create cursor \"%s\" from %s,%s\n",
		     ec->name, img, msk);
	  }

	if (pmap)
	   EFreePixmap(pmap);
	if (mask)
	   EFreePixmap(mask);
	Efree(img);
     }
   else
     {
	ec->cursor = (ec->native_id == 999) ?
	   None : XCreateFontCursor(disp, ec->native_id);
     }

 done:
   if (ec->cursor == None)
     {
	ECursorDestroy(ec);
	ec = NULL;
     }

   return ec;
}

static int
_ECursorMatchName(const void *data, const void *match)
{
   return strcmp(((const ECursor *)data)->name, (const char *)match);
}

static ECursor     *
ECursorFind(const char *name)
{
   if (!name || !name[0])
      return NULL;
   return (ECursor *) ecore_list_find(cursor_list, _ECursorMatchName, name);
}

ECursor            *
ECursorAlloc(const char *name)
{
   ECursor            *ec;

   if (!name)
      return NULL;

   ec = ECursorFind(name);
   if (!ec)
      return NULL;

   if (ec->cursor == None)
      ec = ECursorRealize(ec);
   if (!ec)
      return NULL;

   ec->ref_count++;

   return ec;
}

void
ECursorFree(ECursor * ec)
{
   if (ec)
      ec->ref_count--;
}

int
ECursorConfigLoad(FILE * fs)
{
   int                 err = 0;
   unsigned int        clr, clr2;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char               *p2;
   int                 i1, i2, r, g, b;
   char                name[FILEPATH_LEN_MAX], *pname;
   char                file[FILEPATH_LEN_MAX], *pfile;
   int                 native_id = -1;

   COLOR32_FROM_RGB(clr, 0, 0, 0);
   COLOR32_FROM_RGB(clr2, 255, 255, 255);

   pname = pfile = NULL;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, &p2, NULL);
	switch (i1)
	  {
	  case CONFIG_CURSOR:
	     err = -1;
	     i2 = atoi(s2);
	     if (i2 != CONFIG_OPEN)
		goto done;
	     COLOR32_FROM_RGB(clr, 0, 0, 0);
	     COLOR32_FROM_RGB(clr2, 255, 255, 255);
	     pname = pfile = NULL;
	     native_id = -1;
	     break;
	  case CONFIG_CLOSE:
	     ECursorCreate(pname, pfile, native_id, clr, clr2);
	     err = 0;
	     break;

	  case CONFIG_CLASSNAME:
	     if (ECursorFind(s2))
	       {
		  SkipTillEnd(fs);
		  goto done;
	       }
	     strcpy(name, s2);
	     pname = name;
	     break;
	  case CURS_BG_RGB:
	     r = g = b = 0;
	     sscanf(p2, "%d %d %d", &r, &g, &b);
	     COLOR32_FROM_RGB(clr, r, g, b);
	     break;
	  case CURS_FG_RGB:
	     r = g = b = 255;
	     sscanf(p2, "%d %d %d", &r, &g, &b);
	     COLOR32_FROM_RGB(clr2, r, g, b);
	     break;
	  case XBM_FILE:
	     strcpy(file, s2);
	     pfile = file;
	     break;
	  case NATIVE_ID:
	     native_id = atoi(s2);
	     break;
	  default:
	     break;
	  }
     }

 done:
   if (err)
      ConfigAlertLoad("Cursor");

   return err;
}

void
ECursorApply(ECursor * ec, Win win)
{
   if (!ec)
      return;
   XDefineCursor(disp, WinGetXwin(win), ec->cursor);
}

static              Cursor
ECursorGetByName(const char *name, const char *name2, unsigned int fallback)
{
   ECursor            *ec;

   ec = ECursorAlloc(name);
   if (!ec && name2)
      ec = ECursorAlloc(name2);
   if (ec)
      return ec->cursor;

   return XCreateFontCursor(disp, fallback);
}

typedef struct {
   const char         *pri;
   const char         *sec;
   unsigned int        fallback;
} ECDataRec;

static const ECDataRec ECData[ECSR_COUNT] = {
   {"DEFAULT", NULL, XC_left_ptr},
   {"GRAB", NULL, XC_crosshair},
   {"PGRAB", NULL, XC_X_cursor},
   {"GRAB_MOVE", NULL, XC_fleur},
   {"GRAB_RESIZE", NULL, XC_sizing},
   {"RESIZE_H", NULL, XC_sb_h_double_arrow},
   {"RESIZE_V", NULL, XC_sb_v_double_arrow},
   {"RESIZE_TL", "RESIZE_BR", XC_top_left_corner},
   {"RESIZE_TR", "RESIZE_BL", XC_top_right_corner},
   {"RESIZE_BL", "RESIZE_TR", XC_bottom_left_corner},
   {"RESIZE_BR", "RESIZE_TL", XC_bottom_right_corner},
};

static Cursor       ECsrs[ECSR_COUNT] = {
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

Cursor
ECsrGet(int which)
{
   if (which < 0 || which >= ECSR_COUNT)
      return None;
   if (ECsrs[which] == 1)
      ECsrs[which] = ECursorGetByName(ECData[which].pri, ECData[which].sec,
				      ECData[which].fallback);

   return ECsrs[which];
}

void
ECsrApply(int which, Window win)
{
   XDefineCursor(disp, win, ECsrGet(which));
}

static void
CursorsIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;
   ECursor            *ec;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
     }

   if (!strncmp(cmd, "list", 2))
     {
	ECORE_LIST_FOR_EACH(cursor_list, ec) IpcPrintf("%s\n", ec->name);
     }
}

static const IpcItem CursorIpcArray[] = {
   {
    CursorsIpc,
    "cursor", "csr",
    "Cursor functions",
    "  cursor list                       Show all cursors\n"}
};
#define N_IPC_FUNCS (sizeof(CursorIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
extern const EModule ModCursors;

const EModule       ModCursors = {
   "cursor", "csr",
   NULL,
   {N_IPC_FUNCS, CursorIpcArray}
   ,
   {0, NULL}
};

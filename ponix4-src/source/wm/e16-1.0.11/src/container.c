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
#include "E.h"
#include "container.h"
#include "dialog.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "eobj.h"
#include "ewins.h"
#include "hints.h"
#include "iclass.h"
#include "icons.h"
#include "menus.h"

extern const ContainerOps IconboxOps;
extern const ContainerOps SystrayOps;

static void         ContainersConfigSave(void);

static void         ContainerLayout(Container * ct, int *px, int *py, int *pw,
				    int *ph);
static void         ContainerDraw(Container * ct);

static void         ContainerEventScrollWin(Win win, XEvent * ev, void *prm);
static void         ContainerEventScrollbarWin(Win win, XEvent * ev, void *prm);
static void         ContainerEventCoverWin(Win win, XEvent * ev, void *prm);
static void         ContainerEventArrow1Win(Win win, XEvent * ev, void *prm);
static void         ContainerEventArrow2Win(Win win, XEvent * ev, void *prm);
static void         ContainerEventIconWin(Win win, XEvent * ev, void *prm);

/* Widget state */
#define WS_HILITE 0x01
#define WS_CLICK  0x02

static const unsigned char _ic_state[4] =
   { STATE_NORMAL, STATE_HILITED, STATE_CLICKED, STATE_CLICKED };
#define ST(b) _ic_state[b]

ContainerCfg        Conf_containers;

static Ecore_List  *container_list = NULL;

static int
_ContainerMatchName(const void *data, const void *match)
{
   return strcmp(((const Container *)data)->name, (const char *)match);
}

static Container   *
ContainerFind(const char *name)
{
   return (Container *) ecore_list_find(container_list, _ContainerMatchName,
					name);
}

static Container   *
ContainerCreate(const char *name)
{
   Container          *ct;

   if (ContainerFind(name))
      return NULL;

   ct = ECALLOC(Container, 1);
   if (!ct)
      return NULL;

   if (!container_list)
      container_list = ecore_list_new();
   ecore_list_append(container_list, ct);

   ct->name = Estrdup(name);
   ct->type = (name && !strcmp(name, "_ST_")) ?
      IB_TYPE_SYSTRAY : IB_TYPE_ICONBOX;
   ct->orientation = 0;
   ct->scrollbar_side = 1;
   ct->arrow_side = 1;
   ct->nobg = 0;
   ct->shownames = 1;
   ct->iconsize = 48;
   ct->icon_mode = 2;
   ct->auto_resize = 0;
   ct->draw_icon_base = 0;
   ct->scrollbar_hide = 0;
   ct->cover_hide = 0;
   ct->auto_resize_anchor = 0;
   /* FIXME: need to have theme settable params for this and get them */
   ct->scroll_thickness = 12;
   ct->arrow_thickness = 12;
   ct->bar_thickness = 8;
   ct->knob_length = 8;

   ct->w = 0;
   ct->h = 0;
   ct->pos = 0;
   ct->iwin_maxl = 1;

   ct->scrollbar_state = 0;
   ct->arrow1_state = 0;
   ct->arrow2_state = 0;
   ct->scrollbox_clicked = 0;
   ct->icon_clicked = 0;

   ct->win = ECreateClientWindow(VROOT, 0, 0, 1, 1);
   ct->icon_win = ECreateWindow(ct->win, 0, 0, 128, 26, 0);
   EventCallbackRegister(ct->icon_win, ContainerEventIconWin, ct);
   ct->cover_win = ECreateWindow(ct->win, 0, 0, 128, 26, 0);
   EventCallbackRegister(ct->cover_win, ContainerEventCoverWin, ct);
   ct->scroll_win = ECreateWindow(ct->win, 6, 26, 116, 6, 0);
   EventCallbackRegister(ct->scroll_win, ContainerEventScrollWin, ct);
   ct->arrow1_win = ECreateWindow(ct->win, 0, 26, 6, 6, 0);
   EventCallbackRegister(ct->arrow1_win, ContainerEventArrow1Win, ct);
   ct->arrow2_win = ECreateWindow(ct->win, 122, 26, 6, 6, 0);
   EventCallbackRegister(ct->arrow2_win, ContainerEventArrow2Win, ct);
   ct->scrollbar_win = ECreateWindow(ct->scroll_win, 122, 26, 6, 6, 0);
   EventCallbackRegister(ct->scrollbar_win, ContainerEventScrollbarWin, ct);
   ct->scrollbarknob_win = ECreateWindow(ct->scrollbar_win, -20, -20, 4, 4, 0);

   ESelectInput(ct->icon_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask | PointerMotionMask);
   ESelectInput(ct->scroll_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   ESelectInput(ct->cover_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   ESelectInput(ct->arrow1_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   ESelectInput(ct->arrow2_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   ESelectInput(ct->scrollbar_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask | PointerMotionMask);

   EMapWindow(ct->icon_win);
   EMapWindow(ct->scrollbar_win);

   ct->ewin = NULL;
   ct->num_objs = 0;
   ct->objs = NULL;

   if (ct->type == IB_TYPE_ICONBOX)
      ct->ops = &IconboxOps;
   else if (ct->type == IB_TYPE_SYSTRAY)
      ct->ops = &SystrayOps;

   ct->ops->Init(ct);

   return ct;
}

static void
ContainerDestroy(Container * ct, int exiting)
{
   ecore_list_node_remove(container_list, ct);

   ct->ops->Exit(ct, exiting);

   Efree(ct->name);
   Efree(ct->objs);

   Efree(ct);

   if (!exiting)
      ContainersConfigSave();
}

static void
ContainerReconfigure(Container * ct)
{
   unsigned int        wmin, hmin, wmax, hmax;

   wmax = hmax = 16384;

   if (ct->orientation)
     {
	wmax = wmin = ct->iwin_fixh + ct->scroll_thickness_set;
	hmin = ct->iwin_maxl_min;
     }
   else
     {
	hmax = hmin = ct->iwin_fixh + ct->scroll_thickness_set;
	wmin = ct->iwin_maxl_min;
     }

   ICCCM_SetSizeConstraints(ct->ewin, wmin, hmin, wmax, hmax, 0, 0, 1, 1,
			    0.0, 65535.0);
}

static void
_ContainerEwinInit(EWin * ewin)
{
   Container          *ct = (Container *) ewin->data;

   ct->ewin = ewin;

   EwinSetTitle(ewin, ct->wm_name);
   EwinSetClass(ewin, ct->name, "Enlightenment_IconBox");

   ewin->props.skip_ext_task = 1;
   ewin->props.skip_ext_pager = 1;
   ewin->props.skip_focuslist = 1;
   ewin->props.skip_winlist = 1;
   EwinInhSetWM(ewin, focus, 1);
   EwinInhSetWM(ewin, iconify, 1);
   ewin->props.autosave = 1;

   EoSetSticky(ewin, 1);
}

static void
_ContainerEwinLayout(EWin * ewin, int *px, int *py, int *pw, int *ph)
{
   Container          *ct = (Container *) ewin->data;
   int                 w, h;

   w = ct->w;
   h = ct->h;
   ContainerLayout(ct, px, py, pw, ph);

   if (*pw != w || *ph != h)
      ct->do_update = 1;
}

static void
_ContainerEwinMoveResize(EWin * ewin, int resize)
{
   Container          *ct = (Container *) ewin->data;

   if (!resize && !ct->do_update && !TransparencyUpdateNeeded())
      return;

   ct->w = ewin->client.w;
   ct->h = ewin->client.h;

   ContainerReconfigure(ct);
   ContainerDraw(ct);
   ct->do_update = 0;
}

static void
_ContainerEwinClose(EWin * ewin)
{
   ContainerDestroy((Container *) ewin->data, 0);
   ewin->data = NULL;
}

static const EWinOps _ContainerEwinOps = {
   _ContainerEwinInit,
   _ContainerEwinLayout,
   _ContainerEwinMoveResize,
   _ContainerEwinClose,
};

static void
ContainerShow(Container * ct)
{
   EWin               *ewin;

   if (!ct)
      return;

   ewin = AddInternalToFamily(ct->win, "ICONBOX", EWIN_TYPE_ICONBOX,
			      &_ContainerEwinOps, ct);
   if (!ewin)
      return;

   if (ewin->state.placed)
     {
	EwinMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
		       ewin->client.w, ewin->client.h, 0);
     }
   else
     {
	/* The first one */
	EwinResize(ewin, 128, 32, 0);
	EwinMove(ewin, WinGetW(VROOT) - EoGetW(ewin),
		 WinGetH(VROOT) - EoGetH(ewin), 0);
     }

   EwinShow(ewin);
}

/*
 * Return index, -1 if not found.
 */
int
ContainerObjectFind(Container * ct, void *obj)
{
   int                 i;

   for (i = 0; i < ct->num_objs; i++)
      if (ct->objs[i].obj == obj)
	 return i;

   return -1;
}

int
ContainerObjectAdd(Container * ct, void *obj)
{
   /* Not if already there */
   if (ContainerObjectFind(ct, obj) >= 0)
      return -1;

   ct->num_objs++;
   ct->objs = EREALLOC(ContainerObject, ct->objs, ct->num_objs);
   ct->objs[ct->num_objs - 1].obj = obj;

   return ct->num_objs - 1;	/* Success */
}

int
ContainerObjectDel(Container * ct, void *obj)
{
   int                 i, j;

   /* Quit if not there */
   i = ContainerObjectFind(ct, obj);
   if (i < 0)
      return -1;

   for (j = i; j < ct->num_objs - 1; j++)
      ct->objs[j] = ct->objs[j + 1];
   ct->num_objs--;
   if (ct->num_objs > 0)
      ct->objs = EREALLOC(ContainerObject, ct->objs, ct->num_objs);
   else
     {
	Efree(ct->objs);
	ct->objs = NULL;
     }

   return 0;			/* Success */
}

void               *
ContainerObjectFindByXY(Container * ct, int px, int py)
{
   int                 i;
   ContainerObject    *cto;

   for (i = 0; i < ct->num_objs; i++)
     {
	cto = &ct->objs[i];

	if (px >= cto->xo - 1 && py >= cto->yo - 1 &&
	    px < cto->xo + cto->wo + 1 && py < cto->yo + cto->ho + 1)
	   return cto->obj;
     }

   return NULL;
}

static void
ContainerLayoutImageWin(Container * ct)
{
   int                 i, xo, yo;
   int                 item_pad, padl, padr, padt, padb;
   ContainerObject    *cto;
   EImageBorder       *pad, *pad_base;

   if (ct->orientation)
      ct->ic_box = ImageclassFind("ICONBOX_VERTICAL", 0);
   else
      ct->ic_box = ImageclassFind("ICONBOX_HORIZONTAL", 0);

   if (ct->draw_icon_base && !ct->im_item_base)
     {
	ct->ic_item_base = ImageclassFind("DEFAULT_ICON_BUTTON", 0);
	if (ct->ic_item_base)
	   ct->im_item_base =
	      ImageclassGetImage(ct->ic_item_base, 0, 0, STATE_NORMAL);
	if (!ct->im_item_base)
	  {
	     ct->ic_item_base = NULL;
	     ct->draw_icon_base = 0;
	  }
     }

   if (ct->draw_icon_base)
     {
	pad_base = ImageclassGetPadding(ct->ic_item_base);
	padl = pad_base->left;
	padr = pad_base->right;
	padt = pad_base->top;
	padb = pad_base->bottom;

	item_pad = 0;
     }
   else
     {
	pad_base = NULL;
	padl = padr = padt = padb = 0;

	item_pad = 2;
     }

   pad = (ct->ic_box) ? ImageclassGetPadding(ct->ic_box) : NULL;

   xo = 0;
   yo = 0;
   if (pad)
     {
	xo += pad->left;
	yo += pad->top;
     }

   for (i = 0; i < ct->num_objs; i++)
     {
	cto = &ct->objs[i];

	/* Inner size */
	ct->ops->ObjSizeCalc(ct, cto);

	/* Outer size */
	if (ct->draw_icon_base && ct->im_item_base)
	  {
	     if (cto->wi > 0 && cto->hi > 0)
	       {
		  cto->wo = ct->iconsize + padl + padr;
		  cto->ho = ct->iconsize + padt + padb;
	       }
	     else
	       {
		  cto->wo = cto->ho = 0;
	       }
	  }
	else
	  {
	     if (cto->wi > 0 && cto->hi > 0)
	       {
		  if (ct->orientation)
		    {
		       cto->wo = ct->iconsize;
		       cto->ho = cto->hi;
		    }
		  else
		    {
		       cto->wo = cto->wi;
		       cto->ho = ct->iconsize;
		    }
	       }
	     else
	       {
		  cto->wo = cto->ho = 0;
	       }
	  }

	cto->xo = xo;
	cto->yo = yo;
	cto->xi = xo + (cto->wo - cto->wi) / 2;
	cto->yi = yo + (cto->ho - cto->hi) / 2;
#if 0
	Eprintf("xo,yo=%d,%d wo,ho=%d,%d  xi,yi=%d,%d wi,hi=%d,%d\n",
		cto->xo, cto->yo, cto->wo, cto->ho, cto->xi, cto->yi, cto->wi,
		cto->hi);
#endif

	if (ct->orientation)
	   yo += cto->ho + item_pad;
	else
	   xo += cto->wo + item_pad;
     }

   if (pad)
     {
	xo += pad->right;
	yo += pad->bottom;
     }

   if (ct->orientation)
      ct->iwin_maxl = yo - item_pad;
   else
      ct->iwin_maxl = xo - item_pad;

   if (ct->orientation)
     {
	ct->iwin_fixh = ct->iconsize;
	if (pad)
	   ct->iwin_fixh += pad->left + pad->right;
	if (ct->draw_icon_base)
	   ct->iwin_fixh += padl + padr;
	ct->iwin_maxl_min = 8;
     }
   else
     {
	ct->iwin_fixh = ct->iconsize;
	if (pad)
	   ct->iwin_fixh += pad->top + pad->bottom;
	if (ct->draw_icon_base)
	   ct->iwin_fixh += padt + padb;
	ct->iwin_maxl_min = 8;
     }

   if (ct->iwin_maxl < ct->iwin_maxl_min)
      ct->iwin_maxl = ct->iwin_maxl_min;
}

static void
ContainerLayoutScroll(Container * ct)
{
   ImageClass         *ic, *ic_sbb;
   EImageBorder       *pad;
   int                 arrow_mode = ct->arrow_side;
   int                 bs, bw, bx;

   switch (ct->orientation)
     {
     default:
	if (ct->h < 2 * ct->arrow_thickness + ct->knob_length)
	   arrow_mode = 3;	/* No arrows */

	ic_sbb = ImageclassFind("ICONBOX_SCROLLBAR_BASE_VERTICAL", 1);
	pad = ImageclassGetPadding(ic_sbb);
	if (arrow_mode < 3)
	   bs = ct->h - (ct->arrow_thickness * 2);
	else
	   bs = ct->h;
	if (pad)
	   bs -= pad->top + pad->bottom;
	bw = (ct->h * bs) / ct->iwin_maxl;
	if (bs < 1)
	   bs = 1;
	if (bw > bs)
	   bw = bs;
	if (bw < 1)
	   bw = 1;
	bx = (ct->pos * bs) / ct->iwin_maxl;
	if (pad)
	   bx += pad->top;
	if ((ct->scrollbar_hide) && (bw == bs))
	   goto do_hide_sb;

	EMapWindow(ct->scroll_win);
	if (arrow_mode < 3)
	  {
	     EMapWindow(ct->arrow1_win);
	     EMapWindow(ct->arrow2_win);
	  }
	else
	  {
	     EUnmapWindow(ct->arrow1_win);
	     EUnmapWindow(ct->arrow2_win);
	  }

	/* fix this area */
	if (ct->scrollbar_side == 1)
	   /* right */
	  {
	     /* start */
	     if (arrow_mode == 0)
	       {
		  EMoveResizeWindow(ct->arrow1_win,
				    ct->w - ct->scroll_thickness, 0,
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->arrow2_win,
				    ct->w - ct->scroll_thickness,
				    ct->arrow_thickness,
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->scroll_win,
				    ct->w - ct->scroll_thickness,
				    ct->arrow_thickness * 2,
				    ct->scroll_thickness,
				    ct->h - (ct->arrow_thickness * 2));
	       }
	     /* both ends */
	     else if (arrow_mode == 1)
	       {
		  EMoveResizeWindow(ct->arrow1_win,
				    ct->w - ct->scroll_thickness, 0,
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->arrow2_win,
				    ct->w - ct->scroll_thickness,
				    ct->h - ct->arrow_thickness,
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->scroll_win,
				    ct->w - ct->scroll_thickness,
				    ct->arrow_thickness,
				    ct->scroll_thickness,
				    ct->h - (ct->arrow_thickness * 2));
	       }
	     /* end */
	     else if (arrow_mode == 2)
	       {
		  EMoveResizeWindow(ct->arrow1_win,
				    ct->w - ct->scroll_thickness,
				    ct->h - (ct->arrow_thickness * 2),
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->arrow2_win,
				    ct->w - ct->scroll_thickness,
				    ct->h - ct->arrow_thickness,
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->scroll_win,
				    ct->w - ct->scroll_thickness, 0,
				    ct->scroll_thickness,
				    ct->h - (ct->arrow_thickness * 2));
	       }
	     /* no arrows */
	     else
	       {
		  EMoveResizeWindow(ct->scroll_win,
				    ct->w - ct->scroll_thickness, 0,
				    ct->scroll_thickness, ct->h);
	       }
	  }
	else
	   /* left */
	  {
	     /* start */
	     if (arrow_mode == 0)
	       {
		  EMoveResizeWindow(ct->arrow1_win, 0, 0,
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->arrow2_win, 0,
				    ct->arrow_thickness,
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->scroll_win, 0,
				    ct->arrow_thickness * 2,
				    ct->scroll_thickness,
				    ct->h - (ct->arrow_thickness * 2));
	       }
	     /* both ends */
	     else if (arrow_mode == 1)
	       {
		  EMoveResizeWindow(ct->arrow1_win, 0, 0,
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->arrow2_win, 0,
				    ct->h - ct->arrow_thickness,
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->scroll_win, 0,
				    ct->arrow_thickness,
				    ct->scroll_thickness,
				    ct->h - (ct->arrow_thickness * 2));
	       }
	     /* end */
	     else if (arrow_mode == 2)
	       {
		  EMoveResizeWindow(ct->arrow1_win, 0,
				    ct->h - (ct->arrow_thickness * 2),
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->arrow2_win, 0,
				    ct->h - ct->arrow_thickness,
				    ct->scroll_thickness, ct->arrow_thickness);
		  EMoveResizeWindow(ct->scroll_win, 0, 0,
				    ct->scroll_thickness,
				    ct->h - (ct->arrow_thickness * 2));
	       }
	     /* no arrows */
	     else
	       {
		  EMoveResizeWindow(ct->scroll_win, 0, 0,
				    ct->scroll_thickness, ct->h);
	       }
	  }

	EMoveResizeWindow(ct->scrollbar_win,
			  (ct->scroll_thickness - ct->bar_thickness) / 2, bx,
			  ct->bar_thickness, bw);

	ic = ImageclassFind("ICONBOX_SCROLLKNOB_VERTICAL", 0);
	if ((ic) && (bw > ct->knob_length))
	  {
	     EMapWindow(ct->scrollbarknob_win);
	     EMoveResizeWindow(ct->scrollbarknob_win, 0,
			       (bw - ct->knob_length) / 2, ct->bar_thickness,
			       ct->knob_length);
	  }
	else
	  {
	     EUnmapWindow(ct->scrollbarknob_win);
	  }
	ct->scroll_thickness_set = ct->scroll_thickness;
	break;

     case 0:
	if (ct->w < 2 * ct->arrow_thickness + ct->knob_length)
	   arrow_mode = 3;	/* No arrows */

	ic_sbb = ImageclassFind("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 1);
	pad = ImageclassGetPadding(ic_sbb);
	if (arrow_mode < 3)
	   bs = ct->w - (ct->arrow_thickness * 2);
	else
	   bs = ct->w;
	if (pad)
	   bs -= pad->left + pad->right;
	bw = (ct->w * bs) / ct->iwin_maxl;
	if (bs < 1)
	   bs = 1;
	if (bw > bs)
	   bw = bs;
	if (bw < 1)
	   bw = 1;
	bx = (ct->pos * bs) / ct->iwin_maxl;
	if (pad)
	   bx += pad->left;
	if ((ct->scrollbar_hide) && (bw == bs))
	   goto do_hide_sb;

	EMapWindow(ct->scroll_win);
	if (arrow_mode < 3)
	  {
	     EMapWindow(ct->arrow1_win);
	     EMapWindow(ct->arrow2_win);
	  }
	else
	  {
	     EUnmapWindow(ct->arrow1_win);
	     EUnmapWindow(ct->arrow2_win);
	  }

	if (ct->scrollbar_side == 1)
	   /* bottom */
	  {
	     /* start */
	     if (arrow_mode == 0)
	       {
		  EMoveResizeWindow(ct->arrow1_win, 0,
				    ct->h - ct->scroll_thickness,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->arrow2_win,
				    ct->arrow_thickness,
				    ct->h - ct->scroll_thickness,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->scroll_win,
				    ct->arrow_thickness * 2,
				    ct->h - ct->scroll_thickness,
				    ct->w - (ct->arrow_thickness * 2),
				    ct->scroll_thickness);
	       }
	     /* both ends */
	     else if (arrow_mode == 1)
	       {
		  EMoveResizeWindow(ct->arrow1_win, 0,
				    ct->h - ct->scroll_thickness,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->arrow2_win,
				    ct->w - ct->arrow_thickness,
				    ct->h - ct->scroll_thickness,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->scroll_win,
				    ct->arrow_thickness,
				    ct->h - ct->scroll_thickness,
				    ct->w - (ct->arrow_thickness * 2),
				    ct->scroll_thickness);
	       }
	     /* end */
	     else if (arrow_mode == 2)
	       {
		  EMoveResizeWindow(ct->arrow1_win,
				    ct->w - (ct->arrow_thickness * 2),
				    ct->h - ct->scroll_thickness,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->arrow2_win,
				    ct->w - ct->arrow_thickness,
				    ct->h - ct->scroll_thickness,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->scroll_win, 0,
				    ct->h - ct->scroll_thickness,
				    ct->w - (ct->arrow_thickness * 2),
				    ct->scroll_thickness);
	       }
	     /* no arrows */
	     else
	       {
		  EMoveResizeWindow(ct->scroll_win, 0,
				    ct->h - ct->scroll_thickness, ct->w,
				    ct->scroll_thickness);
	       }
	  }
	else
	   /* top */
	  {
	     /* start */
	     if (arrow_mode == 0)
	       {
		  EMoveResizeWindow(ct->arrow1_win, 0, 0,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->arrow2_win,
				    ct->arrow_thickness, 0,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->scroll_win,
				    ct->arrow_thickness * 2, 0,
				    ct->w - (ct->arrow_thickness * 2),
				    ct->scroll_thickness);
	       }
	     /* both ends */
	     else if (arrow_mode == 1)
	       {
		  EMoveResizeWindow(ct->arrow1_win, 0, 0,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->arrow2_win,
				    ct->w - ct->arrow_thickness, 0,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->scroll_win,
				    ct->arrow_thickness, 0,
				    ct->w - (ct->arrow_thickness * 2),
				    ct->scroll_thickness);
	       }
	     /* end */
	     else if (arrow_mode == 2)
	       {
		  EMoveResizeWindow(ct->arrow1_win,
				    ct->w - (ct->arrow_thickness * 2), 0,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->arrow2_win,
				    ct->w - ct->arrow_thickness, 0,
				    ct->arrow_thickness, ct->scroll_thickness);
		  EMoveResizeWindow(ct->scroll_win, 0, 0,
				    ct->w - (ct->arrow_thickness * 2),
				    ct->scroll_thickness);
	       }
	     /* no arrows */
	     else
	       {
		  EMoveResizeWindow(ct->scroll_win, 0, 0, ct->w,
				    ct->scroll_thickness);
	       }
	  }

	EMoveResizeWindow(ct->scrollbar_win, bx,
			  (ct->scroll_thickness - ct->bar_thickness) / 2, bw,
			  ct->bar_thickness);

	ic = ImageclassFind("ICONBOX_SCROLLKNOB_HORIZONTAL", 0);
	if ((ic) && (bw > ct->knob_length))
	  {
	     EMapWindow(ct->scrollbarknob_win);
	     EMoveResizeWindow(ct->scrollbarknob_win,
			       (bw - ct->knob_length) / 2, 0, ct->knob_length,
			       ct->bar_thickness);
	  }
	else
	  {
	     EUnmapWindow(ct->scrollbarknob_win);
	  }
	ct->scroll_thickness_set = ct->scroll_thickness;
	break;

      do_hide_sb:
	EUnmapWindow(ct->scroll_win);
	EUnmapWindow(ct->arrow1_win);
	EUnmapWindow(ct->arrow2_win);
	ct->scroll_thickness_set = 0;
	break;
     }
}

static void
ContainerDrawScroll(Container * ct)
{
   ImageClass         *ic_base, *ic_knob, *ic_snob, *ic_arr1, *ic_arr2;

   ContainerLayoutScroll(ct);

   if (!WinIsMapped(ct->scroll_win))
      return;

   ic_snob = ic_arr1 = ic_arr2 = NULL;
   if (ct->orientation)
     {
	ic_base = ImageclassFind("ICONBOX_SCROLLBAR_BASE_VERTICAL", 1);
	ic_knob = ImageclassFind("ICONBOX_SCROLLBAR_KNOB_VERTICAL", 1);
	if (WinIsMapped(ct->scrollbarknob_win))
	   ic_snob = ImageclassFind("ICONBOX_SCROLLKNOB_VERTICAL", 0);
	if (WinIsMapped(ct->arrow1_win))
	  {
	     ic_arr1 = ImageclassFind("ICONBOX_ARROW_UP", 1);
	     ic_arr2 = ImageclassFind("ICONBOX_ARROW_DOWN", 1);
	  }
     }
   else
     {
	ic_base = ImageclassFind("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 1);
	ic_knob = ImageclassFind("ICONBOX_SCROLLBAR_KNOB_HORIZONTAL", 1);
	if (WinIsMapped(ct->scrollbarknob_win))
	   ic_snob = ImageclassFind("ICONBOX_SCROLLKNOB_HORIZONTAL", 0);
	if (WinIsMapped(ct->arrow1_win))
	  {
	     ic_arr1 = ImageclassFind("ICONBOX_ARROW_LEFT", 1);
	     ic_arr2 = ImageclassFind("ICONBOX_ARROW_RIGHT", 1);
	  }
     }

   ImageclassApply(ic_base, ct->scroll_win, 0, 0, STATE_NORMAL, ST_ICONBOX);
   ImageclassApply(ic_knob, ct->scrollbar_win, 0, 0, ST(ct->scrollbar_state),
		   ST_ICONBOX);
   if (WinIsMapped(ct->scrollbarknob_win))
      ImageclassApply(ic_snob, ct->scrollbarknob_win, 0, 0,
		      ST(ct->scrollbar_state), ST_ICONBOX);

   if (WinIsMapped(ct->arrow1_win))
     {
	ImageclassApply(ic_arr1, ct->arrow1_win, 0, 0, ST(ct->arrow1_state),
			ST_ICONBOX);
	ImageclassApply(ic_arr2, ct->arrow2_win, 0, 0, ST(ct->arrow2_state),
			ST_ICONBOX);
     }
}

static void
ContainerFixPos(Container * ct)
{
   int                 v;

   if (ct->orientation)
      v = ct->iwin_maxl - ct->h;
   else
      v = ct->iwin_maxl - ct->w;

   if (ct->pos > v)
      ct->pos = v;
   if (ct->pos < 0)
      ct->pos = 0;
}

static void
ContainerLayout(Container * ct, int *px, int *py, int *pw, int *ph)
{
   int                 x, y, w, h;
   EWin               *ewin = ct->ewin;

   x = *px;
   y = *py;
   w = *pw;
   h = *ph;

   ContainerLayoutImageWin(ct);	/* Find iwin_maxl, iwin_fixh */

   /* Possibly change "length" if autosizing */
   if (ct->auto_resize)
     {
	int                 bl, br, bt, bb;

	EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);

	if (ct->orientation)
	  {
	     ct->h = h = ct->iwin_maxl;
	     if (ewin->border)
	       {
		  if ((bt + bb + h) > WinGetH(VROOT))
		     h = WinGetH(VROOT) - (bt + bb);
	       }
	     y += (((ewin->client.h - h) * ct->auto_resize_anchor) >> 10);
	     if (ewin->border)
	       {
		  if ((EoGetY(ewin) + bt + bb + h) > WinGetH(VROOT))
		     y = WinGetH(VROOT) - (bt + bb + h);
	       }
	  }
	else
	  {
	     ct->w = w = ct->iwin_maxl;
	     if (ewin->border)
	       {
		  if ((bl + br + w) > WinGetW(VROOT))
		     w = WinGetW(VROOT) - (bl + br);
	       }
	     x += (((ewin->client.w - w) * ct->auto_resize_anchor) >> 10);
	     if (ewin->border)
	       {
		  if ((EoGetX(ewin) + bl + br + w) > WinGetW(VROOT))
		     x = WinGetW(VROOT) - (bl + br + w);
	       }
	  }
     }
   else
     {
	if (ct->orientation)
	  {
	     if (h < ct->iwin_maxl_min)
		h = ct->iwin_maxl_min;
	     ct->h = h;
	  }
	else
	  {
	     if (w < ct->iwin_maxl_min)
		w = ct->iwin_maxl_min;
	     ct->w = w;
	  }
     }

   ContainerLayoutScroll(ct);	/* Find scroll_thickness_set, uses h/w, iwin_maxl */
   ContainerFixPos(ct);

   /* Possibly change "height" if hiding scrollbar */
   if (ct->orientation)
     {
	ct->w = w = ct->iwin_fixh + ct->scroll_thickness_set;
     }
   else
     {
	ct->h = h = ct->iwin_fixh + ct->scroll_thickness_set;
     }

   *px = x;
   *py = y;
   *pw = w;
   *ph = h;
}

static void
ContainerDraw(Container * ct)
{
   int                 i, w, h;
   ImageClass         *ib_ic_cover;
   int                 ib_xlt, ib_ylt, ib_ww, ib_hh;
   int                 ib_x0, ib_y0, ib_w0, ib_h0;
   EImage             *im;
   int                 ww, hh;

   w = ct->w;
   h = ct->h;

   ContainerDrawScroll(ct);

   /* Geometry of iconbox window, excluding scrollbar */
   ib_xlt = 0;
   ib_ylt = 0;
   ib_ww = w;
   ib_hh = h;
   if (ct->orientation)
     {
	ib_ic_cover = ImageclassFind("ICONBOX_COVER_VERTICAL", 0);
	if (ct->scrollbar_side == 0)
	   ib_xlt = ct->scroll_thickness_set;
	ib_ww -= ct->scroll_thickness_set;

	/* Geometry of icon window (including invisible parts) */
	ib_x0 = ib_xlt;
	ib_y0 = ib_ylt - ct->pos;
	ib_w0 = ib_ww;
	ib_h0 = ib_hh;
	if (ib_h0 < ct->iwin_maxl)
	   ib_h0 = ct->iwin_maxl;
     }
   else
     {
	ib_ic_cover = ImageclassFind("ICONBOX_COVER_HORIZONTAL", 0);
	if (ct->scrollbar_side == 0)
	   ib_ylt = ct->scroll_thickness_set;
	ib_hh -= ct->scroll_thickness_set;

	/* Geometry of icon window (including invisible parts) */
	ib_x0 = ib_xlt - ct->pos;
	ib_y0 = ib_ylt;
	ib_w0 = ib_ww;
	if (ib_w0 < ct->iwin_maxl)
	   ib_w0 = ct->iwin_maxl;
	ib_h0 = ib_hh;
     }

   EMoveResizeWindow(ct->icon_win, ib_x0, ib_y0, ib_w0, ib_h0);

   if (ib_ic_cover && !ct->cover_hide)
     {
	EMoveResizeWindow(ct->cover_win, ib_xlt, ib_ylt, ib_ww, ib_hh);
	EMapWindow(ct->cover_win);
	ImageclassApply(ib_ic_cover, ct->cover_win, 0, 0, STATE_NORMAL,
			ST_ICONBOX);
     }
   else
     {
	EMoveResizeWindow(ct->cover_win, -30000, -30000, 2, 2);
	EUnmapWindow(ct->cover_win);
     }

   if (ct->nobg && ct->num_objs == 0)
     {
	im = NULL;
     }
   else if (ct->ic_box &&
	    (!ct->nobg || (ct->type == IB_TYPE_SYSTRAY && !ct->draw_icon_base)))
     {
	/* Start out with iconbox image class image */
	im = ImageclassGetImageBlended(ct->ic_box, ct->icon_win, ib_w0, ib_h0,
				       0, 0, STATE_NORMAL, ST_ICONBOX);
     }
   else
     {
	/* Start out with blank image */
	im = EImageCreate(ib_w0, ib_h0);
	EImageSetHasAlpha(im, 1);
	EImageFill(im, 0, 0, ib_w0, ib_h0, 0);
     }

   for (i = 0; i < ct->num_objs; i++)
     {
	ContainerObject    *cto;

	cto = &ct->objs[i];

	if (ct->draw_icon_base && ct->im_item_base)
	  {
	     EImageGetSize(ct->im_item_base, &ww, &hh);
	     EImageBlend(im, ct->im_item_base, EIMAGE_BLEND | EIMAGE_ANTI_ALIAS,
			 0, 0, ww, hh, cto->xo, cto->yo, cto->wo, cto->ho, 1);
	  }

	ct->ops->ObjPlace(ct, cto, im);
     }

   if (im)
     {
	EMapWindow(ct->icon_win);
	EImageApplyToWin(im, ct->icon_win, EIMAGE_HIGH_MASK_THR, 0, 0);
	EImageFree(im);

	if (ct->type == IB_TYPE_SYSTRAY && ct->nobg && !ct->draw_icon_base)
	   EShapePropagate(ct->icon_win);
     }
   else
     {
	/* Transparent and no objects */
	EUnmapWindow(ct->icon_win);
     }
   EShapePropagate(ct->win);
   EwinUpdateShapeInfo(ct->ewin);
   ct->ewin->update.shape = 1;
   EwinPropagateShapes(ct->ewin);
}

void
ContainerRedraw(Container * ct)
{
   EWin               *ewin = ct->ewin;

   ct->do_update = 1;
   EwinResize(ct->ewin, ewin->client.w, ewin->client.h, 0);
}

static int
ContainerScroll(Container * ct, int dir)
{
   int                 ppos;

   ppos = ct->pos;
   ct->pos += dir;
   ContainerFixPos(ct);
   if (ct->pos == ppos)
      return 0;

   ContainerDraw(ct);
   return 1;
}

static void
ContainerShowMenu(Container * ct)
{
   Menu               *m;
   MenuItem           *mi;
   char                s[1024];

   m = MenuCreate("__ct", ct->menu_title, NULL, NULL);
   if (!m)
      return;

   MenuSetTransient(m);		/* Destroy when hidden */

   Esnprintf(s, sizeof(s), "ibox cfg %s", ct->name);
   mi = MenuItemCreate(_("Settings..."), NULL, s, NULL);
   MenuAddItem(m, mi);

   Esnprintf(s, sizeof(s), "wop %#lx cl", WinGetXwin(ct->win));
   mi = MenuItemCreate(_("Close"), NULL, s, NULL);
   MenuAddItem(m, mi);

   if (ct->type == IB_TYPE_ICONBOX)
     {
	mi = MenuItemCreate(_("Create New Iconbox"), NULL, "ibox new", NULL);
	MenuAddItem(m, mi);
     }

   EFunc(NULL, "menus show __ct");
}

static void
ContainersShow(void)
{
   Container          *ct;

   if (ecore_list_count(container_list) > 0)
     {
	ECORE_LIST_FOR_EACH(container_list, ct) ContainerShow(ct);
     }
   else if (Conf.startup.firsttime)
     {
	ct = ContainerCreate("_IB_0");
	ContainerShow(ct);
	ContainersConfigSave();
     }
}

static void
ContainersDestroy(void)
{
   Container          *ct;

   ECORE_LIST_FOR_EACH(container_list, ct) ContainerDestroy(ct, 1);
}

static void
ContainerEventScrollWin(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Container          *ct = (Container *) prm;
   int                 x, y, w, h;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	   ct->scrollbox_clicked = 1;
	else if (ev->xbutton.button == 3)
	   ContainerShowMenu(ct);
	break;

     case ButtonRelease:
	if (!ct->scrollbox_clicked)
	   break;
	ct->scrollbox_clicked = 0;
	EGetGeometry(ct->scrollbar_win, NULL, &x, &y, &w, &h, NULL, NULL);
	if (ct->orientation)
	  {
	     if (ev->xbutton.y < y)
		ContainerScroll(ct, -8);
	     else if (ev->xbutton.y > (y + h))
		ContainerScroll(ct, 8);
	  }
	else
	  {
	     if (ev->xbutton.x < x)
		ContainerScroll(ct, -8);
	     else if (ev->xbutton.x > (x + w))
		ContainerScroll(ct, 8);
	  }
	break;
     }
}

static void
ContainerEventScrollbarWin(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Container          *ct = (Container *) prm;
   static int          px, py, pos0;
   int                 bs, dp;
   ImageClass         *ic_sbb;
   EImageBorder       *pad;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	  {
	     px = ev->xbutton.x_root;
	     py = ev->xbutton.y_root;
	     pos0 = ct->pos;
	     ct->scrollbar_state |= WS_CLICK;
	  }
	else if (ev->xbutton.button == 3)
	   ContainerShowMenu(ct);
	goto draw_scoll;

     case ButtonRelease:
	ct->scrollbar_state &= ~WS_CLICK;
	goto draw_scoll;

     case EnterNotify:
	ct->scrollbar_state |= WS_HILITE;
	goto draw_scoll;

     case LeaveNotify:
	ct->scrollbar_state &= ~WS_HILITE;
	goto draw_scoll;

     case MotionNotify:
	if (!(ct->scrollbar_state & WS_CLICK))
	   break;

	if (ct->orientation)
	  {
	     ic_sbb = ImageclassFind("ICONBOX_SCROLLBAR_BASE_VERTICAL", 1);
	     pad = ImageclassGetPadding(ic_sbb);
	     bs = ct->h - (ct->arrow_thickness * 2);
	     if (pad)
		bs -= pad->top + pad->bottom;
	     if (bs < 1)
		bs = 1;
	     dp = ev->xmotion.y_root - py;
	  }
	else
	  {
	     ic_sbb = ImageclassFind("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 1);
	     pad = ImageclassGetPadding(ic_sbb);
	     bs = ct->w - (ct->arrow_thickness * 2);
	     if (pad)
		bs -= pad->left + pad->right;
	     if (bs < 1)
		bs = 1;
	     dp = ev->xmotion.x_root - px;
	  }
	dp = pos0 + (dp * ct->iwin_maxl) / bs - ct->pos;
	if (dp)
	   ContainerScroll(ct, dp);
	break;

      draw_scoll:
	ContainerDrawScroll(ct);
	break;
     }
}

static void
ContainerEventCoverWin(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Container          *ct = (Container *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	ContainerShowMenu(ct);
	break;
     case ButtonRelease:
	break;
     }
}

static void
ContainerEventArrow1Win(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Container          *ct = (Container *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	   ct->arrow1_state |= WS_CLICK;
	else if (ev->xbutton.button == 3)
	   ContainerShowMenu(ct);
	goto draw_scoll;

     case ButtonRelease:
	if (!(ct->arrow1_state & WS_CLICK))
	   goto draw_scoll;
	ct->arrow1_state &= ~WS_CLICK;
	if (ContainerScroll(ct, -8))
	   return;
	goto draw_scoll;

     case EnterNotify:
	ct->arrow1_state |= WS_HILITE;
	goto draw_scoll;

     case LeaveNotify:
	ct->arrow1_state &= ~WS_HILITE;
	goto draw_scoll;

      draw_scoll:
	ContainerDrawScroll(ct);
	break;
     }
}

static void
ContainerEventArrow2Win(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Container          *ct = (Container *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	   ct->arrow2_state |= WS_CLICK;
	else if (ev->xbutton.button == 3)
	   ContainerShowMenu(ct);
	goto draw_scoll;

     case ButtonRelease:
	if (!(ct->arrow2_state & WS_CLICK))
	   goto draw_scoll;
	ct->arrow2_state &= ~WS_CLICK;
	if (ContainerScroll(ct, 8))
	   return;
	goto draw_scoll;

     case EnterNotify:
	ct->arrow2_state |= WS_HILITE;
	goto draw_scoll;

     case LeaveNotify:
	ct->arrow2_state &= ~WS_HILITE;
	goto draw_scoll;

      draw_scoll:
	ContainerDrawScroll(ct);
	break;
     }
}

static void
ContainerEventIconWin(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Container          *ct = (Container *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button != 3)
	   break;
	ContainerShowMenu(ct);
	return;
     }

   if (ct->ops->Event)
      ct->ops->Event(ct, ev);
}

#if ENABLE_DIALOGS
/*
 * Configuration dialog
 */

typedef struct {
   Container          *ct;
   char                nobg;
   char                shownames;
   int                 vert;
   int                 side;
   int                 arrows;
   int                 iconsize;
   int                 icon_mode;
   char                auto_resize;
   char                draw_icon_base;
   char                scrollbar_hide;
   char                cover_hide;
   int                 autoresize_anchor;
   int                 anim_mode;
   int                 anim_speed;
} ContainerDlgData;

static void
CB_ConfigureContainer(Dialog * d, int val, void *data __UNUSED__)
{
   ContainerDlgData   *dd = DLG_DATA_GET(d, ContainerDlgData);
   Container          *ct;

   if (val >= 2)
      return;

   ct = (Container *) ecore_list_goto(container_list, dd->ct);
   if (!ct)
      return;

   ct->nobg = dd->nobg;
   ct->shownames = dd->shownames;
   ct->orientation = dd->vert;
   ct->scrollbar_side = dd->side;
   ct->arrow_side = dd->arrows;
   ct->iconsize = dd->iconsize;
   ct->icon_mode = dd->icon_mode;
   ct->auto_resize = dd->auto_resize;
   ct->draw_icon_base = dd->draw_icon_base;
   ct->scrollbar_hide = dd->scrollbar_hide;
   ct->cover_hide = dd->cover_hide;
   ct->auto_resize_anchor = dd->autoresize_anchor;
   ct->anim_mode = dd->anim_mode;
   Conf_containers.anim_time = 1000 - dd->anim_speed;

   ContainerRedraw(ct);

   ContainersConfigSave();
   autosave();
}

static void
CB_IconSizeSlider(Dialog * d, int val __UNUSED__, void *data)
{
   ContainerDlgData   *dd = DLG_DATA_GET(d, ContainerDlgData);
   DItem              *di = (DItem *) data;
   char                s[256];

   Esnprintf(s, sizeof(s), _("Icon size: %2d"), dd->iconsize);
   DialogItemSetText(di, s);
}

static void
_DlgFillContainer(Dialog * d, DItem * table, void *data)
{
   ContainerDlgData   *dd;
   Container          *ct = (Container *) data;
   DItem              *di, *table2, *radio, *label;
   char                s[256];

   dd = DLG_DATA_SET(d, ContainerDlgData);
   if (!dd)
      return;

   if (!ct)
      return;

   dd->ct = ct;
   dd->nobg = ct->nobg;
   dd->shownames = ct->shownames;
   dd->vert = ct->orientation;
   dd->side = ct->scrollbar_side;
   dd->arrows = ct->arrow_side;
   dd->iconsize = ct->iconsize;
   dd->icon_mode = ct->icon_mode;
   dd->auto_resize = ct->auto_resize;
   dd->draw_icon_base = ct->draw_icon_base;
   dd->scrollbar_hide = ct->scrollbar_hide;
   dd->cover_hide = ct->cover_hide;
   dd->autoresize_anchor = ct->auto_resize_anchor;
   dd->anim_mode = ct->anim_mode;
   dd->anim_speed = 1000 - Conf_containers.anim_time;

   DialogSetTitle(d, ct->dlg_title);

   DialogItemTableSetOptions(table, 1, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Transparent background"));
   DialogItemCheckButtonSetPtr(di, &dd->nobg);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Hide inner border"));
   DialogItemCheckButtonSetPtr(di, &dd->cover_hide);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Draw base image behind Icons"));
   DialogItemCheckButtonSetPtr(di, &dd->draw_icon_base);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Hide scrollbar when not needed"));
   DialogItemCheckButtonSetPtr(di, &dd->scrollbar_hide);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Automatically resize to fit Icons"));
   DialogItemCheckButtonSetPtr(di, &dd->auto_resize);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di,
		     _("Alignment of anchoring when automatically resizing:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 8);
   DialogItemSliderSetValPtr(di, &dd->autoresize_anchor);

   DialogAddItem(table, DITEM_SEPARATOR);

   label = di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   Esnprintf(s, sizeof(s), _("Icon size: %2d"), dd->iconsize);
   DialogItemSetText(di, s);

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 4, 128);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 8);
   DialogItemSliderSetValPtr(di, &dd->iconsize);
   DialogItemSetCallback(di, CB_IconSizeSlider, 0, label);

   DialogAddItem(table, DITEM_SEPARATOR);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table2, 5, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Orientation:"));

   radio = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Horizontal"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Vertical"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->vert);

   DialogAddItem(table2, DITEM_NONE);
   DialogAddItem(table2, DITEM_NONE);

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Scrollbar side:"));

   radio = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Left / Top"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Right / Bottom"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->side);

   DialogAddItem(table2, DITEM_NONE);
   DialogAddItem(table2, DITEM_NONE);

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Scrollbar arrows:"));

   radio = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Start"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Both ends"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("End"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("None"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->arrows);

   DialogAddItem(table, DITEM_SEPARATOR);

   if (ct->type == IB_TYPE_ICONBOX)
     {
	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetText(di, _("Show icon names"));
	DialogItemCheckButtonSetPtr(di, &dd->shownames);

	table2 = DialogAddItem(table, DITEM_TABLE);
	DialogItemTableSetOptions(table2, 4, 0, 1, 0);

	di = DialogAddItem(table2, DITEM_TEXT);
	DialogItemSetFill(di, 0, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Animation mode:"));

	radio = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
	DialogItemSetText(di, _("Off"));
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, 0);

	di = DialogAddItem(table2, DITEM_RADIOBUTTON);
	DialogItemSetText(di, _("Whirl"));
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, 1);

	di = DialogAddItem(table2, DITEM_RADIOBUTTON);
	DialogItemSetText(di, _("Box"));
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, 2);
	DialogItemRadioButtonGroupSetValPtr(radio, &dd->anim_mode);

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetFill(di, 0, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Animation speed:"));

	di = DialogAddItem(table, DITEM_SLIDER);
	DialogItemSliderSetBounds(di, 0, 1000);
	DialogItemSliderSetUnits(di, 10);
	DialogItemSliderSetJump(di, 50);
	DialogItemSliderSetValPtr(di, &dd->anim_speed);

	DialogAddItem(table, DITEM_SEPARATOR);

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetFill(di, 0, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di,
			  _
			  ("Icon image display policy (if one operation fails, try the next):"));

	radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
	DialogItemSetText(di,
			  _
			  ("Snapshot Windows, Use application icon, Use Enlightenment Icon"));
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, EWIN_ICON_MODE_SNAP);

	di = DialogAddItem(table, DITEM_RADIOBUTTON);
	DialogItemSetText(di,
			  _
			  ("Use application icon, Use Enlightenment Icon, Snapshot Window"));
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, EWIN_ICON_MODE_APP_IMG_SNAP);

	di = DialogAddItem(table, DITEM_RADIOBUTTON);
	DialogItemSetText(di, _("Use Enlightenment Icon, Snapshot Window"));
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, EWIN_ICON_MODE_IMG_APP_SNAP);
	DialogItemRadioButtonGroupSetValPtr(radio, &dd->icon_mode);
     }
}

static const DialogDef DlgContainer = {
   "CONFIGURE_ICONBOX",
   NULL,
   NULL,
   SOUND_SETTINGS_ICONBOX,
   "pix/iconbox.png",
   N_("Enlightenment Iconbox\n" "Settings Dialog"),
   _DlgFillContainer,
   DLG_OAC, CB_ConfigureContainer,
};
#endif /* ENABLE_DIALOGS */

/*
 * Configuration load/save
 */
#include "conf.h"

static int
_ContainersConfigLoad(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1, i2;
   Container          *ct, ct_dummy;

   ct = &ct_dummy;
   while (fgets(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, NULL, NULL);
	i2 = atoi(s2);
	switch (i1)
	  {
	  case CONFIG_IBOX:
	     err = -1;
	     if (i2 != CONFIG_OPEN)
		goto done;
	     break;
	  case CONFIG_CLOSE:
	     ct = &ct_dummy;
	     err = 0;
	     break;

	  case CONFIG_CLASSNAME:	/* __NAME %s */
	     ct = ContainerFind(s2);
	     if (ct)
		EwinHide(ct->ewin);
	     ct = ContainerCreate(s2);
	     break;
	  case TEXT_ORIENTATION:	/* __ORIENTATION [ __HORIZONTAL | __VERTICAL ] */
	     ct->orientation = i2;
	     break;
	  case CONFIG_TRANSPARENCY:	/* __TRANSPARENCY [ __ON | __OFF ] */
	     ct->nobg = i2;
	     break;
	  case CONFIG_SHOW_NAMES:	/* __SHOW_NAMES [ __ON | __OFF ] */
	     ct->shownames = i2;
	     break;
	  case CONFIG_ICON_SIZE:	/* __ICON_SIZE %i */
	     ct->iconsize = i2;
	     break;
	  case CONFIG_ICON_MODE:	/* __ICON_MODE [ 0 | 1 | 2 ] */
	     ct->icon_mode = i2;
	     break;
	  case CONFIG_SCROLLBAR_SIDE:	/* __SCROLLBAR_SIDE [ __BAR_LEFT/__BAR_TOP | __BAR_RIGHT/__BAR_BOTTOM ] */
	     ct->scrollbar_side = i2;
	     break;
	  case CONFIG_SCROLLBAR_ARROWS:	/* __SCROLLBAR_ARROWS [ __START | __BOTH | __FINISH | __NEITHER ] */
	     ct->arrow_side = i2;
	     break;
	  case CONFIG_AUTOMATIC_RESIZE:	/* __AUTOMATIC_RESIZE [ __ON | __OFF ] */
	     ct->auto_resize = i2;
	     break;
	  case CONFIG_SHOW_ICON_BASE:	/* __SHOW_ICON_BASE [ __ON | __OFF ] */
	     ct->draw_icon_base = i2;
	     break;
	  case CONFIG_SCROLLBAR_AUTOHIDE:	/* __SCROLLBAR_AUTOHIDE [ __ON | __OFF ] */
	     ct->scrollbar_hide = i2;
	     break;
	  case CONFIG_COVER_HIDE:	/* __COVER_HIDE [ __ON | __OFF ] */
	     ct->cover_hide = i2;
	     break;
	  case CONFIG_RESIZE_ANCHOR:	/* __RESIZE_ANCHOR 0-1024 */
	     ct->auto_resize_anchor = i2;
	     break;
	  case CONFIG_IB_ANIMATE:	/* __ICONBOX_ANIMATE [ 0 | 1 | 2 ] */
	     ct->anim_mode = i2;
	     break;
	  default:
	     Eprintf("Warning: Iconbox configuration, ignoring: %s\n", s);
	     break;
	  }
     }

 done:
   if (err)
      Eprintf("Error: Iconbox configuration file load problem.\n");

   return err;
}

static void
ContainersConfigLoad(void)
{
   char                s[4096];

   Esnprintf(s, sizeof(s), "%s.ibox", EGetSavePrefix());

   ConfigFileLoad(s, NULL, _ContainersConfigLoad, 0);
}

static void
ContainersConfigSave(void)
{
   char                s[FILEPATH_LEN_MAX];
   FILE               *fs;
   Container          *ct;

   Esnprintf(s, sizeof(s), "%s.ibox", EGetSavePrefix());
   fs = fopen(s, "w");
   if (!fs)
      return;

   /* We should check for errors... */
   ECORE_LIST_FOR_EACH(container_list, ct)
   {
      fprintf(fs, "19 999\n");
      fprintf(fs, "100 %s\n", ct->name);
      fprintf(fs, "200 %i\n", ct->orientation);
      fprintf(fs, "2001 %i\n", ct->nobg);
      fprintf(fs, "2002 %i\n", ct->shownames);
      fprintf(fs, "2003 %i\n", ct->iconsize);
      fprintf(fs, "2004 %i\n", ct->icon_mode);
      fprintf(fs, "2005 %i\n", ct->scrollbar_side);
      fprintf(fs, "2006 %i\n", ct->arrow_side);
      fprintf(fs, "2007 %i\n", ct->auto_resize);
      fprintf(fs, "2008 %i\n", ct->draw_icon_base);
      fprintf(fs, "2009 %i\n", ct->scrollbar_hide);
      fprintf(fs, "2010 %i\n", ct->cover_hide);
      fprintf(fs, "2011 %i\n", ct->auto_resize_anchor);
      fprintf(fs, "2012 %i\n", ct->anim_mode);
      fprintf(fs, "1000\n");
   }

   fclose(fs);
}

/*
 * Containers Module
 */

static void
ContainersSighan(int sig, void *prm)
{
   switch (sig)
     {
     case ESIGNAL_CONFIGURE:
	break;
     case ESIGNAL_START:
	ContainersConfigLoad();
	ContainersShow();
	break;
     case ESIGNAL_EXIT:
	ContainersDestroy();
	break;
     }

#if 0				/* FIXME */
   ECORE_LIST_FOR_EACH(container_list, ct)
   {
      if (ct->ops)
	 ct->ops->Signal(ct, sig, prm);
   }
#else
   IconboxOps.Signal(NULL, sig, prm);
#endif
}

#if ENABLE_DIALOGS
static void
ContainersConfigure(const char *params)
{
   Container          *ct;

   if (!params || !params[0])
      params = "DEFAULT";

   ct = ContainerFind(params);
   if (ct)
      DialogShowSimple(&DlgContainer, ct);
}
#endif /* ENABLE_DIALOGS */

Container          *
ContainersIterate(ContainerIterator * cti, int type, void *data)
{
   Container          *ct;

   ECORE_LIST_FOR_EACH(container_list, ct)
   {
      if (ct->type != type)
	 continue;
      if (cti(ct, data))
	 return ct;
   }

   return NULL;
}

Container         **
ContainersGetList(int *pnum)
{
   return (Container **) ecore_list_items_get(container_list, pnum);
}

/*
 * IPC functions
 */
static void
ContainerIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[128];
   int                 len, num;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %100s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	/* List iconboxes */
     }
#if ENABLE_DIALOGS
   else if (!strncmp(cmd, "cfg", 3))
     {
	ContainersConfigure(prm);
     }
#endif
   else if (!strncmp(cmd, "new", 3))
     {
	Container          *ct;

	if (!prm[0])
	  {
	     num = ecore_list_count(container_list);
	     Esnprintf(prm, sizeof(prm), "_IB_%i", num);
	  }
	ct = ContainerCreate(prm);
	ContainerShow(ct);
	ContainersConfigSave();
     }
}

static const IpcItem ContainersIpcArray[] = {
   {
    ContainerIpc,
    "iconbox", "ibox",
    "Iconbox functions",
    "  iconbox new <name>   Create new iconbox\n"
    "  iconbox cfg          Configure iconboxes\n"}
};
#define N_IPC_FUNCS (sizeof(ContainersIpcArray)/sizeof(IpcItem))

/*
 * Configuration items
 */
static const CfgItem ContainersCfgItems[] = {
   CFG_ITEM_INT(Conf_containers, anim_time, 250),
};
#define N_CFG_ITEMS (sizeof(ContainersCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModIconboxes;

const EModule       ModIconboxes = {
   "iconboxes", "ibox",
   ContainersSighan,
   {N_IPC_FUNCS, ContainersIpcArray},
   {N_CFG_ITEMS, ContainersCfgItems}
};

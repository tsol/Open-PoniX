/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2011 Kim Woelders
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
#include "desktops.h"
#include "emodule.h"
#include "eobj.h"
#include "ewins.h"
#include "hints.h"
#include "icons.h"
#include "timers.h"
#include "tooltips.h"
#include "xwin.h"
#include <math.h>

static Container   *SelectIconboxForEwin(EWin * ewin);

/* Silly hack to avoid name clash warning when using -Wshadow */
#define y1 y1_

#define IB_ANIM_TIME    Conf_containers.anim_time
#define IB_ANIM_STEP    Conf.animation.step

static void
IB_Animate_Sleep(unsigned int t0, double a)
{
   unsigned int        t;
   double              dt;

   t = GetTimeMs() - t0;
   dt = 1e-3 * (t - a * IB_ANIM_TIME);
   dt = 1e-3 * IB_ANIM_STEP - dt;
   if (dt > 0)
      usleep((unsigned long)(1e6 * dt));
}

static void
IB_Animate_A(char iconify, EWin * ewin, EWin * ibox)
{
   EWin               *fr, *to;
   unsigned int        t0;
   double              a, aa, spd;
   int                 x, y, x1, y1, x2, y2, x3, y3, x4, y4, w, h;
   int                 fx, fy, fw, fh, tx, ty, tw, th;
   Window              root = WinGetXwin(VROOT);
   GC                  gc;
   XGCValues           gcv;

   /* Window: Extents, Iconbox: Center */
   if (iconify)
     {
	fr = ewin;
	to = ibox;
	fw = EoGetW(fr) + 4;
	fh = EoGetH(fr) + 4;
	fx = EoGetX(fr) - 2;
	fy = EoGetY(fr) - 2;
	tw = 4;
	th = 4;
	tx = EoGetX(to) + EoGetW(to) / 2 - 2;
	ty = EoGetY(to) + EoGetH(to) / 2 - 2;
     }
   else
     {
	fr = ibox;
	to = ewin;
	fw = 4;
	fh = 4;
	fx = EoGetX(fr) + EoGetW(fr) / 2 - 2;
	fy = EoGetY(fr) + EoGetH(fr) / 2 - 2;
	tw = EoGetW(to) + 4;
	th = EoGetH(to) + 4;
	tx = EoGetX(to) + 2;
	ty = EoGetY(to) + 2;
     }
   fx += EoGetX(EoGetDesk(fr));
   fy += EoGetY(EoGetDesk(fr));
   tx += EoGetX(EoGetDesk(to));
   ty += EoGetY(EoGetDesk(to));

   gcv.subwindow_mode = IncludeInferiors;
   gcv.function = GXxor;
   gcv.line_width = 2;
   gcv.foreground = Dpy.pixel_white;
   if (gcv.foreground == 0)
      gcv.foreground = Dpy.pixel_black;
   gc = EXCreateGC(root,
		   GCFunction | GCForeground | GCSubwindowMode | GCLineWidth,
		   &gcv);

   spd = (1. * IB_ANIM_STEP) / IB_ANIM_TIME;

   t0 = GetTimeMs();
   for (a = 0.0; a < 1.0; a += spd)
     {
	aa = 1.0 - a;

	x = (int)((fx * aa) + (tx * a));
	y = (int)((fy * aa) + (ty * a));
	w = (int)((fw * aa) + (tw * a));
	h = (int)((fh * aa) + (th * a));

	x = (2 * x + w) / 2;	/* x middle */
	y = (2 * y + h) / 2;	/* y middle */
	w /= 2;			/* width/2 */
	h /= 2;			/* height/2 */

	x1 = (int)(x + w * (1 - .5 * sin(3.14159 + a * 6.2831853072)));
	y1 = (int)(y + h * cos(a * 6.2831853072));
	x2 = (int)(x + w * (1 - .5 * sin(a * 6.2831853072)));
	y2 = (int)(y - h * cos(a * 6.2831853072));
	x3 = (int)(x - w * (1 - .5 * sin(3.14159 + a * 6.2831853072)));
	y3 = (int)(y - h * cos(a * 6.2831853072));
	x4 = (int)(x - w * (1 - .5 * sin(a * 6.2831853072)));
	y4 = (int)(y + h * cos(a * 6.2831853072));

	XDrawLine(disp, root, gc, x1, y1, x2, y2);
	XDrawLine(disp, root, gc, x2, y2, x3, y3);
	XDrawLine(disp, root, gc, x3, y3, x4, y4);
	XDrawLine(disp, root, gc, x4, y4, x1, y1);

	ESync(0);
	IB_Animate_Sleep(t0, a);

	XDrawLine(disp, root, gc, x1, y1, x2, y2);
	XDrawLine(disp, root, gc, x2, y2, x3, y3);
	XDrawLine(disp, root, gc, x3, y3, x4, y4);
	XDrawLine(disp, root, gc, x4, y4, x1, y1);
     }

   EXFreeGC(gc);
}

static void
IB_Animate_B(char iconify, EWin * ewin, EWin * ibox)
{
   EWin               *fr, *to;
   unsigned int        t0;
   double              a, spd;
   int                 x, y, w, h;
   int                 fx, fy, fw, fh, tx, ty, tw, th;
   Window              root = WinGetXwin(VROOT);
   GC                  gc;
   XGCValues           gcv;

   if (iconify)
     {
	fr = ewin;
	to = ibox;
     }
   else
     {
	fr = ibox;
	to = ewin;
     }

   fx = EoGetX(fr) - 2;
   fy = EoGetY(fr) - 2;
   fw = EoGetW(fr) + 3;
   fh = EoGetH(fr) + 3;

   tx = EoGetX(to) - 2;
   ty = EoGetY(to) - 2;
   tw = EoGetW(to) + 3;
   th = EoGetH(to) + 3;

   fx += EoGetX(EoGetDesk(fr));
   fy += EoGetY(EoGetDesk(fr));
   tx += EoGetX(EoGetDesk(to));
   ty += EoGetY(EoGetDesk(to));

   gcv.subwindow_mode = IncludeInferiors;
   gcv.function = GXxor;
   gcv.fill_style = FillOpaqueStippled;
   gcv.foreground = Dpy.pixel_white;
   if (gcv.foreground == 0)
      gcv.foreground = Dpy.pixel_black;
   gc = EXCreateGC(root,
		   GCFunction | GCForeground | GCSubwindowMode | GCFillStyle,
		   &gcv);

   XDrawLine(disp, root, gc, fx, fy, tx, ty);
   XDrawLine(disp, root, gc, fx + fw, fy, tx + tw, ty);
   XDrawLine(disp, root, gc, fx, fy + fh, tx, ty + th);
   XDrawLine(disp, root, gc, fx + fw, fy + fh, tx + tw, ty + th);
   XDrawRectangle(disp, root, gc, tx, ty, tw, th);
   XDrawRectangle(disp, root, gc, fx, fy, fw, fh);

   spd = (1. * IB_ANIM_STEP) / IB_ANIM_TIME;

   t0 = GetTimeMs();
   for (a = 0.0; a < 1.0; a += spd)
     {
	x = (int)(fx + a * (tx - fx));
	w = (int)(fw + a * (tw - fw));
	y = (int)(fy + a * (ty - fy));
	h = (int)(fh + a * (th - fh));
	XDrawRectangle(disp, root, gc, x, y, w, h);

	ESync(0);
	IB_Animate_Sleep(t0, a);

	XDrawRectangle(disp, root, gc, x, y, w, h);
     }

   XDrawLine(disp, root, gc, fx, fy, tx, ty);
   XDrawLine(disp, root, gc, fx + fw, fy, tx + tw, ty);
   XDrawLine(disp, root, gc, fx, fy + fh, tx, ty + th);
   XDrawLine(disp, root, gc, fx + fw, fy + fh, tx + tw, ty + th);
   XDrawRectangle(disp, root, gc, tx, ty, tw, th);
   XDrawRectangle(disp, root, gc, fx, fy, fw, fh);

   EXFreeGC(gc);
}

static void
IB_Animate(Container * ct, int iconify, EWin * ewin)
{
   if (Mode.wm.startup || ct->anim_mode <= 0)
      return;

   if (Conf_containers.anim_time < 10 || Conf_containers.anim_time > 10000)
      Conf_containers.anim_time = 250;

   EobjsRepaint();
   EGrabServer();

   switch (ct->anim_mode)
     {
     default:
	break;
     case 1:
	IB_Animate_A(iconify, ewin, ct->ewin);
	break;
     case 2:
	IB_Animate_B(iconify, ewin, ct->ewin);
	break;
     }

   EUngrabServer();
}

static int
IconboxObjEwinFind(Container * ct, EWin * ewin)
{
   return ContainerObjectFind(ct, ewin);
}

static void
IconboxObjEwinAdd(Container * ct, EWin * ewin)
{
   int                 i;

   i = ContainerObjectAdd(ct, ewin);
   if (i < 0)
      return;

   ct->objs[i].im = EwinIconImageGet(ewin, ct->iconsize, ct->icon_mode);
   ContainerRedraw(ct);
}

static void
IconboxObjEwinDel(Container * ct, EWin * ewin)
{
   int                 i;

   i = IconboxObjEwinFind(ct, ewin);
   if (i < 0)
      return;

   if (ct->objs[i].im)
      EImageFree(ct->objs[i].im);

   ContainerObjectDel(ct, ewin);
}

static void
IconboxesEwinIconify(EWin * ewin)
{
   Container          *ct;

   ct = SelectIconboxForEwin(ewin);
   if (!ct)
      return;

   SoundPlay(SOUND_ICONIFY);

   if (EoIsShown(ewin) && ct->anim_mode && !ewin->state.showingdesk)
      IB_Animate(ct, 1, ewin);

   IconboxObjEwinAdd(ct, ewin);
}

static void
IconboxesEwinDeIconify(EWin * ewin)
{
   Container          *ct;

   ct = SelectIconboxForEwin(ewin);
   if (!ct)
      return;

   SoundPlay(SOUND_DEICONIFY);

   if (ct->anim_mode && !ewin->state.showingdesk)
      IB_Animate(ct, 0, ewin);

   IconboxObjEwinDel(ct, ewin);
   ContainerRedraw(ct);
   EobjsRepaint();
}

static void
RemoveMiniIcon(EWin * ewin)
{
   Container          *ct;

   ct = SelectIconboxForEwin(ewin);
   if (!ct)
      return;

   IconboxObjEwinDel(ct, ewin);
   ContainerRedraw(ct);
}

static int
IconboxFindEwin(Container * ct, void *data)
{
   EWin               *ewin = (EWin *) data;

   return IconboxObjEwinFind(ct, ewin) >= 0;

}

static Container   *
SelectIconboxForEwin(EWin * ewin)
{
   /* find the appropriate iconbox from all available ones for this app */
   /* if it is to be iconified, or if it is alreayd return which iconbox */
   /* it's in */
   Container          *ct, *ib_sel = NULL;

   if (!ewin)
      return NULL;

   if (ewin->state.iconified)
     {
	/* find the iconbox this window got iconifed into */
	ib_sel = ContainersIterate(IconboxFindEwin, IB_TYPE_ICONBOX, ewin);
     }
   else
     {
	/* pick the closest iconbox physically on screen to put it in */
	int                 min_dist;
	int                 dx, dy, dist;
	int                 i, num;
	Container         **lst;

	lst = ContainersGetList(&num);
	min_dist = 0x7fffffff;
	for (i = 0; i < num; i++)
	  {
	     ct = lst[i];
	     if (!ct->ewin || ct->type != IB_TYPE_ICONBOX)
		continue;

	     dx = (EoGetX(ct->ewin) + (EoGetW(ct->ewin) / 2)) -
		(EoGetX(ewin) + (EoGetW(ewin) / 2));
	     dy = (EoGetY(ct->ewin) + (EoGetH(ct->ewin) / 2)) -
		(EoGetY(ewin) + (EoGetH(ewin) / 2));
	     dist = (dx * dx) + (dy * dy);
	     if ((!EoIsSticky(ct->ewin)) &&
		 (EoGetDesk(ct->ewin) != EoGetDesk(ewin)))
		dist += (WinGetW(VROOT) * WinGetW(VROOT)) +
		   (WinGetH(VROOT) * WinGetH(VROOT));
	     if (dist < min_dist)
	       {
		  min_dist = dist;
		  ib_sel = ct;
	       }
	  }
	Efree(lst);
     }

   return ib_sel;
}

static void
IconboxUpdateEwinIcon(Container * ct, EWin * ewin, int icon_mode)
{
   int                 i;

   if (ct->icon_mode != icon_mode)
      return;

   i = IconboxObjEwinFind(ct, ewin);
   if (i < 0)
      return;

   if (ct->objs[i].im)
      EImageFree(ct->objs[i].im);
   ct->objs[i].im = EwinIconImageGet(ewin, ct->iconsize, icon_mode);

   ContainerRedraw(ct);
}

static void
IconboxesUpdateEwinIcon(EWin * ewin, int icon_mode)
{
   int                 i, num;
   Container         **lst, *ct;

   lst = ContainersGetList(&num);
   for (i = 0; i < num; i++)
     {
	ct = lst[i];
	IconboxUpdateEwinIcon(ct, ewin, icon_mode);
     }
   Efree(lst);
}

static void
IconboxFindIconSize(EImage * im, int *pw, int *ph, int size)
{
   int                 w, h, minsz, maxwh;

   EImageGetSize(im, &w, &h);

   maxwh = (w > h) ? w : h;
   if (maxwh <= 1)
      goto done;

   minsz = (size * 3) / 4;

   if (maxwh < minsz || maxwh > size)
     {
	w = (w * size) / maxwh;
	h = (h * size) / maxwh;
     }

 done:
   *pw = w;
   *ph = h;
}

static void
IconboxInit(Container * ct)
{
   ct->wm_name = "Iconbox";
   ct->menu_title = _("Iconbox Options");
   ct->dlg_title = _("Iconbox Settings");
   ct->iconsize = 48;
   ct->anim_mode = 1;
}

static void
IconboxExit(Container * ct, int wm_exit)
{
   while (ct->num_objs)
     {
	if (!wm_exit)
	   EwinDeIconify((EWin *) ct->objs[0].obj);
	IconboxObjEwinDel(ct, (EWin *) ct->objs[0].obj);
     }
}

static void
IconboxSighan(Container * ct __UNUSED__, int sig, void *prm)
{
   EWin               *ewin;

   switch (sig)
     {
     case ESIGNAL_EWIN_ICONIFY:
	ewin = (EWin *) prm;
	IconboxesEwinIconify(ewin);
	break;
     case ESIGNAL_EWIN_DEICONIFY:
	ewin = (EWin *) prm;
	IconboxesEwinDeIconify(ewin);
	break;
     case ESIGNAL_EWIN_DESTROY:
	ewin = (EWin *) prm;
	if (ewin->state.iconified)
	   RemoveMiniIcon(ewin);
	break;
     case ESIGNAL_EWIN_CHANGE_ICON:
	ewin = (EWin *) prm;
	if (ewin->state.iconified)
	   IconboxesUpdateEwinIcon(ewin, 1);
	break;
     }
}

static void
IconboxEvent(Container * ct, XEvent * ev)
{
   static EWin        *name_ewin = NULL;
   ToolTip            *tt;
   EWin               *ewin;
   int                 x, y;
   const char         *name;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	   ct->icon_clicked = 1;
	break;

     case ButtonRelease:
	if (!ct->icon_clicked)
	   break;
	ct->icon_clicked = 0;

	ewin =
	   (EWin *) ContainerObjectFindByXY(ct, ev->xbutton.x, ev->xbutton.y);
	if (!ewin)
	   break;

	tt = TooltipFind("ICONBOX");
	if (tt)
	   TooltipHide(tt);

	EwinOpIconify(ewin, OPSRC_USER, 0);
	break;

     case MotionNotify:
	x = ev->xmotion.x;
	y = ev->xmotion.y;
	goto do_motion;

     case EnterNotify:
	x = ev->xcrossing.x;
	y = ev->xcrossing.y;
	goto do_motion;

      do_motion:
	if (!ct->shownames)
	   break;

	ewin = (EWin *) ContainerObjectFindByXY(ct, x, y);
	if (ewin == name_ewin)
	   break;
	name_ewin = ewin;

	tt = TooltipFind("ICONBOX");
	if (!tt)
	   break;

	TooltipHide(tt);
	if (!ewin)
	   break;

	name = EwinGetTitle(ewin);
	if (name)
	   TooltipShow(tt, name, NULL, Mode.events.cx, Mode.events.cy);
	break;

     case LeaveNotify:
	tt = TooltipFind("ICONBOX");
	if (tt)
	  {
	     TooltipHide(tt);
	     name_ewin = NULL;
	  }
	break;
     }
}

static void
IconboxObjSizeCalc(Container * ct, ContainerObject * cto)
{
   /* Inner size */
   cto->wi = cto->hi = 8;
   if (cto->im)
      IconboxFindIconSize(cto->im, &cto->wi, &cto->hi, ct->iconsize);
}

static void
IconboxObjPlace(Container * ct __UNUSED__, ContainerObject * cto, EImage * im)
{
   int                 w, h;

   if (!cto->im)
      return;

   EImageGetSize(cto->im, &w, &h);
   EImageBlend(im, cto->im, EIMAGE_BLEND | EIMAGE_ANTI_ALIAS, 0, 0, w, h,
	       cto->xi, cto->yi, cto->wi, cto->hi, 1);
}

extern const ContainerOps IconboxOps;

const ContainerOps  IconboxOps = {
   IconboxInit,
   IconboxExit,
   IconboxSighan,
   IconboxEvent,
   IconboxObjSizeCalc,
   IconboxObjPlace,
};

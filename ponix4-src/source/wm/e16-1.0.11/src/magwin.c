/*
 * Copyright (C) 2007-2012 Kim Woelders
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
#include "cursors.h"
#include "ecompmgr.h"
#include "eimage.h"
#include "emodule.h"
#include "eobj.h"
#include "events.h"
#include "ewins.h"
#include "grabs.h"
#include "hints.h"
#include "tclass.h"
#include "timers.h"
#include "util.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/keysym.h>

/* Magnifier window */
typedef struct {
   EWin               *ewin;
   const char         *title;
   int                 cx, cy;	/* Center */
   int                 stroke_cx, stroke_cy;
   int                 stroke_mx, stroke_my;
   int                 scale;	/* Zoom level */
   int                 sx, sy;	/* Scene x,y */
   int                 sw, sh;	/* Scene wxh */
   Time                grab_time;
   char                disable_text;
   char                configured;
   char                btn_down;
   char                stroke;
   char                filter;
   char                grabbing;
   char                step;
   unsigned int        damage_count;
   char                update;
   char                update_always;
   char                update_trig;
} MagWindow;

static void         MagwinDestroy(MagWindow * mw);

static MagWindow   *MagWin = NULL;

static void
MagwinDrawText(MagWindow * mw, int x, int y, const char *txt)
{
   TextClass          *tc;
   int                 cw, ch;

   tc = TextclassFind("COORDS", 1);
   if (!tc)
      return;

   TextSize(tc, 0, 0, 0, txt, &cw, &ch, 17);
   TextDraw(tc, EwinGetClientWin(mw->ewin), None, 0, 0, 0,
	    txt, x, y, cw, ch, 17, 0);
}

static unsigned int
MagwinGetPixel(Drawable draw, unsigned int x, unsigned int y)
{
   EImage             *im;
   unsigned int       *pd, pixel = 0;

   im = EImageGrabDrawable(draw, None, x, y, 1, 1, 0);
   if (im)
     {
	pd = (unsigned int *)EImageGetData(im);
	pixel = *pd;
	EImageFree(im);
     }

   return pixel;
}

static void
MagwinRedraw(MagWindow * mw, int paint)
{
   int                 ww, wh;
   int                 sx, sy, sw, sh;
   double              scale;
   int                 zoom_res;
   Drawable            draw;
   char                buf[128];
   int                 px, py;
   int                 qx, qy;
   int                 out;
   unsigned int        len;

   ww = mw->ewin->client.w;
   wh = mw->ewin->client.h;

   zoom_res = Conf.magwin.zoom_res;
   if (mw->scale < -6 * zoom_res)
      mw->scale = -6 * zoom_res;
   else if (mw->scale > 6 * zoom_res)
      mw->scale = 6 * zoom_res;
   scale = pow(2., (double)(mw->scale) / zoom_res);
   sw = (int)((ww + .999 * scale) / scale);
   if (sw > WinGetW(VROOT))
      scale = (double)ww / (double)WinGetW(VROOT);
   sh = (int)((wh + .999 * scale) / scale);
   if (sh > WinGetH(VROOT) && scale < (double)wh / (double)WinGetH(VROOT))
      scale = (double)wh / (double)WinGetH(VROOT);
   sw = (int)((ww + .999 * scale) / scale);
   sh = (int)((wh + .999 * scale) / scale);
   sx = mw->cx - sw / 2;
   sy = mw->cy - sh / 2;
   if (sx < 0)
      sx = 0;
   else if (sx + sw > WinGetW(VROOT))
      sx = WinGetW(VROOT) - sw;
   if (sy < 0)
      sy = 0;
   else if (sy + sh > WinGetH(VROOT))
      sy = WinGetH(VROOT) - sh;

   mw->sx = sx;
   mw->sy = sy;
   mw->sw = sw;
   mw->sh = sh;

   if (paint)
     {
	int                 dw, dh;

	dw = (int)(sw * scale + .5);
	dh = (int)(sh * scale + .5);
	draw = ECompMgrGetRootBuffer();
	ScaleRect(VROOT, draw, EwinGetClientWin(mw->ewin),
		  EwinGetClientXwin(mw->ewin), sx, sy, sw, sh,
		  0, 0, dw, dh, (mw->filter) ? EIMAGE_ANTI_ALIAS : 0);
     }

   if (mw->disable_text)
      return;

   /* Check if pointer is in magnifier window */
   EQueryPointer(EwinGetClientWin(mw->ewin), &px, &py, NULL, NULL);
   out = px < 0 || px >= mw->ewin->client.w ||
      py < 0 || py >= mw->ewin->client.h;

   /* Show magnified area coordinates */
   len = Esnprintf(buf, sizeof(buf), "x%.2f: %d,%d %dx%d",
		   scale, sx, sy, sw, sh);

   if (out)
      goto done;

   /* Show info about pixel at cursor (if in magnifier) */
   qx = (int)(px / scale);
   qy = (int)(py / scale);
   if (qx > WinGetW(VROOT) - 1)
      qx = WinGetW(VROOT) - 1;
   if (qy > WinGetH(VROOT) - 1)
      qy = WinGetH(VROOT) - 1;
   Esnprintf(buf + len, sizeof(buf) - len, "\n%d,%d: pixel=%#08x",
	     sx + qx, sy + qy,
	     MagwinGetPixel(EwinGetClientXwin(mw->ewin), px, py));
 done:
   MagwinDrawText(mw, 10, 10, buf);
}

static int
_MagwinUpdate(MagWindow * mw)
{
   if (mw != MagWin)
      return 0;

   /* Validate ewin */
   if (!EwinFindByPtr(mw->ewin))
      return 0;

   /* When switching CM off do a delayed repaint. This will catch up on
    * at least some clients having processed expose events. */
   if (Mode.events.damage_count == 0 && mw->damage_count != 0)
      mw->update_trig = 10;
   if (mw->update_trig && --mw->update_trig == 0)
      mw->update = 1;

   if (!(mw->update ||
	 (Mode.events.damage_count == 0 && mw->update_always) ||
	 (Mode.events.damage_count != mw->damage_count)))
      return 1;

   mw->damage_count = Mode.events.damage_count;

   /* FIXME - Check damage */

   MagwinRedraw(mw, 1);

   mw->update = 0;

   return 1;
}

static int
_MagwinAnimator(void *data)
{
   MagWindow          *mw = (MagWindow *) data;

   return _MagwinUpdate(mw);
}

static void
_MagwinGrabSet(MagWindow * mw)
{
   GrabPointerSet(EwinGetClientWin(mw->ewin), ECSR_GRAB, 0);
   GrabKeyboardSet(EwinGetClientWin(mw->ewin));
   mw->grabbing = 1;
   mw->grab_time = Mode.events.time;
}

static void
_MagwinGrabRelease(MagWindow * mw)
{
   GrabPointerRelease();
   GrabKeyboardRelease();
   mw->grabbing = 0;
}

static int
MagwinKeyPress(MagWindow * mw, KeySym key)
{
   switch (key)
     {
     case XK_q:		/* Quit */
     case XK_Escape:
	return 1;
     case XK_g:		/* Toggle grabs */
	if (mw->grabbing)
	   _MagwinGrabRelease(mw);
	else
	   _MagwinGrabSet(mw);
	break;
     case XK_t:		/* Toggle text */
	mw->disable_text = !mw->disable_text;
	break;
     case XK_u:		/* Toggle update always */
	mw->update_always = !mw->update_always;
	break;
     case XK_f:		/* Toggle filter */
	mw->filter += 1;
	mw->filter %= 2;
	break;
     case XK_i:		/* Zoom in */
     case XK_Page_Up:
	mw->scale += 1;
	break;
     case XK_o:		/* Zoom out */
     case XK_Page_Down:
	mw->scale -= 1;
	if (mw->scale < -20)
	   mw->scale = -20;
	break;

     case XK_Left:
	mw->cx -= mw->step;
	if (mw->cx < mw->sw / 2)
	   mw->cx = mw->sw / 2;
	break;
     case XK_Right:
	mw->cx += mw->step;
	if (mw->cx > WinGetW(VROOT) - mw->sw / 2)
	   mw->cx = WinGetW(VROOT) - mw->sw / 2;
	break;
     case XK_Up:
	mw->cy -= mw->step;
	if (mw->cy < mw->sh / 2)
	   mw->cy = mw->sh / 2;
	break;
     case XK_Down:
	mw->cy += mw->step;
	if (mw->cy > WinGetH(VROOT) - mw->sh / 2)
	   mw->cy = WinGetH(VROOT) - mw->sh / 2;
	break;

     case XK_r:		/* Switch render mode */
	Conf.testing.use_render_for_scaling =
	   !Conf.testing.use_render_for_scaling;
	break;

     case XK_s:		/* x/y move step size */
	mw->step = (mw->step == 1) ? 4 : 1;
	break;
     }

   return 0;
}

static void
MagwinEvent(Win win __UNUSED__, XEvent * ev, void *prm)
{
   MagWindow          *mw = (MagWindow *) prm;
   KeySym              key;
   int                 done = 0;

   switch (ev->type)
     {
     default:
	break;

     case KeyPress:
	key = XLookupKeysym(&ev->xkey, 0);
	done = MagwinKeyPress(mw, key);
	mw->update = 1;
	break;

     case ButtonPress:
	switch (ev->xbutton.button)
	  {
	  default:
	     break;
	  case 1:
	     if (!mw->grabbing)
		_MagwinGrabSet(mw);
	     mw->stroke_mx = Mode.events.mx;
	     mw->stroke_my = Mode.events.my;
	     mw->stroke_cx = mw->cx;
	     mw->stroke_cy = mw->cy;
	     mw->btn_down = 1;
	     break;
	  case 3:
	     MagwinKeyPress(mw, XK_f);
	     break;
	  case 4:
	     MagwinKeyPress(mw, XK_i);
	     break;
	  case 5:
	     MagwinKeyPress(mw, XK_o);
	     break;
	  }
	mw->update = 1;
	break;
     case ButtonRelease:
	switch (ev->xbutton.button)
	  {
	  default:
	     break;
	  case 1:
	     if (mw->grabbing && (ev->xbutton.time - mw->grab_time > 250))
		_MagwinGrabRelease(mw);
	     mw->btn_down = 0;
	     mw->stroke = 0;
	  }
	break;

     case MotionNotify:
	if (mw->btn_down)
	  {
	     int                 dx, dy;
	     double              scale;

	     dx = Mode.events.mx - mw->stroke_mx;
	     dy = Mode.events.my - mw->stroke_my;

	     if ((abs(dx) > 10) || (abs(dy) > 10))
		mw->stroke = 1;
	     if (!mw->stroke)
		break;

	     scale = pow(2., (double)(mw->scale) / Conf.magwin.zoom_res);
	     mw->cx = mw->stroke_cx - (int)(dx / scale);
	     mw->cy = mw->stroke_cy - (int)(dy / scale);
	     mw->update = 1;
	  }
	else if (mw->grabbing)
	  {
	     mw->cx = Mode.events.mx;
	     mw->cy = Mode.events.my;
	     mw->update = 1;
	  }
	else
	  {
	     mw->update = 1;
	  }
	break;

     case MapNotify:
	mw->update = 1;
	if (mw->configured)
	   break;
	mw->configured = 1;
	_MagwinGrabSet(mw);
	AnimatorAdd(_MagwinAnimator, mw);
	break;
     }

   if (done)
      EwinHide(mw->ewin);
}

static void
_MagEwinInit(EWin * ewin)
{
   MagWindow          *mw = (MagWindow *) ewin->data;

   mw->ewin = ewin;

   EwinSetTitle(ewin, mw->title);
   EwinSetClass(ewin, "Magnifier", "Enlightenment_Magnifier");

   EoSetSticky(ewin, 1);
   EoSetShadow(ewin, 0);
}

static void
_MagEwinClose(EWin * ewin)
{
   MagwinDestroy((MagWindow *) ewin->data);
   ewin->data = NULL;
   MagWin = NULL;
}

static const EWinOps _MagEwinOps = {
   _MagEwinInit,
   NULL,
   NULL,
   _MagEwinClose,
};

static MagWindow   *
MagwinCreate(const char *title, int width, int height)
{
   MagWindow          *mw;
   Win                 win;
   int                 x, y, w, h;

   mw = ECALLOC(MagWindow, 1);
   if (!mw)
      return NULL;

   win = VROOT;
   w = width;
   h = height;
   x = (win->w - w) / 2;
   y = (win->h - h) / 2;

   win = ECreateClientWindow(VROOT, x, y, w, h);

   mw->title = title;
   mw->ewin = AddInternalToFamily(win, NULL, EWIN_TYPE_MISC, &_MagEwinOps, mw);
   if (!mw->ewin)
     {
	Efree(mw);
	return NULL;
     }

   mw->filter = 1;
   mw->disable_text = 1;

   mw->ewin->o.ghost = 1;
   EoSetLayer(mw->ewin, 10);
   EwinMoveResize(mw->ewin, EoGetX(mw->ewin), EoGetY(mw->ewin),
		  mw->ewin->client.w, mw->ewin->client.h, 0);

   mw->ewin->client.event_mask |=
      KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
      StructureNotifyMask;
   ESelectInput(win, mw->ewin->client.event_mask);

   EventCallbackRegister(win, MagwinEvent, mw);

   EQueryPointer(VROOT, &mw->cx, &mw->cy, NULL, NULL);
   mw->scale = Conf.magwin.zoom_res;
   mw->step = 4;

   return mw;
}

static void
MagwinDestroy(MagWindow * mw)
{
   Efree(mw);
}

static void
MagwinShow(void)
{
   if (MagWin)
      goto done;

   MagWin = MagwinCreate(_("Magnifier"),
			 WinGetW(VROOT) / 4, WinGetH(VROOT) / 4);
   if (!MagWin)
      return;

 done:
   EwinShow(MagWin->ewin);
}

static void
MagwinHide(void)
{
   if (!MagWin)
      return;
   EwinHide(MagWin->ewin);
}

/*
 * MagWin Module
 */

static void
MagwinIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len, show;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   show = MagWin && EoIsShown(MagWin->ewin);
   if (!p)
     {
	show = !show;
     }
   else if (!strcmp(cmd, "show"))
     {
	show = 1;
     }
   else if (!strcmp(cmd, "hide"))
     {
	show = 0;
     }

   if (show)
      MagwinShow();
   else
      MagwinHide();
}

static const IpcItem MagwinIpcArray[] = {
   {
    MagwinIpc,
    "magwin", "mag",
    "Magnifier functions",
    "  mag [show|hide]\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(MagwinIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
extern const EModule ModMagwin;

const EModule       ModMagwin = {
   "magwin", NULL,
   NULL,
   {N_IPC_FUNCS, MagwinIpcArray},
   {0, NULL}
};

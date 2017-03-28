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
#include "borders.h"
#include "ewins.h"
#include "focus.h"
#include "hints.h"
#include "xwin.h"
#if USE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif

#define DEBUG_ZOOM 0
#if DEBUG_ZOOM
#define Dprintf(fmt...) if(EDebug(EDBUG_TYPE_ZOOM))Eprintf(fmt)
#else
#define Dprintf(fmt...)
#endif

static EWin        *zoom_last_ewin = NULL;
static signed char  zoom_can = 0;

#if USE_ZOOM_XF86VM
#include <X11/extensions/xf86vmode.h>

static int          std_vid_modes_num = 0;
static int          std_vid_mode_cur = 0;
static XF86VidModeModeInfo **std_vid_modes = NULL;

static XF86VidModeModeInfo *
FindMode(int w, int h)
{
   XF86VidModeModeInfo *chosen = NULL;
   int                 i, closest = 0x7fffffff;

#if USE_XRANDR
   if ((Mode.screen.rotation == RR_Rotate_90) ||
       (Mode.screen.rotation == RR_Rotate_270))
     {
	i = w;
	w = h;
	h = i;
     }
#endif
   for (i = 0; i < std_vid_modes_num; i++)
     {
	int                 value = 0x7fffffff;

	if ((std_vid_modes[i]->hdisplay >= w) &&
	    (std_vid_modes[i]->vdisplay >= h))
	   value = ((std_vid_modes[i]->hdisplay - w) +
		    (std_vid_modes[i]->vdisplay - h));
	if (value < closest)
	  {
	     closest = value;
	     chosen = std_vid_modes[i];
	  }
     }
   return chosen;
}

static int
GetModeIndex(unsigned int dotclock, XF86VidModeModeLine * line)
{
   int                 i;
   const XF86VidModeModeInfo *info;

   for (i = 0; i < std_vid_modes_num; i++)
     {
	info = std_vid_modes[i];
	if (info->dotclock == dotclock &&
	    info->hdisplay == line->hdisplay &&
	    info->vdisplay == line->vdisplay)
	   return i;
     }
   return 0;
}

static int
SwitchRes(char inout, int x, int y, int w, int h, int *dw, int *dh)
{
   static int          vp_x, vp_y;
   XF86VidModeModeInfo *mode;
   int                 scr;

   scr = Dpy.screen;

   if (inout)
     {
	XF86VidModeModeLine curmode;
	int                 dotclock;
	int                 rx, ry;

	if (!XF86VidModeGetModeLine(disp, scr, &dotclock, &curmode))
	   return 0;
	XF86VidModeGetViewPort(disp, scr, &vp_x, &vp_y);

	mode = FindMode(w, h);
	if (mode)
	  {
#if USE_XRANDR
	     int                 vw, vh;

	     vw = WinGetW(VROOT);
	     vh = WinGetH(VROOT);
	     /* x and y relative to unrotated display */
	     if (Mode.screen.rotation == RR_Rotate_90)
	       {
		  rx = y;
		  ry = vw - mode->vdisplay - x;
	       }
	     else if (Mode.screen.rotation == RR_Rotate_270)
	       {
		  rx = vh - mode->hdisplay - y;
		  ry = x;
	       }
	     else if (Mode.screen.rotation == RR_Rotate_180)
	       {
		  rx = vw - mode->hdisplay - x;
		  ry = vh - mode->vdisplay - y;
	       }
	     else
#endif
	       {
		  rx = x;
		  ry = y;
	       }
#if USE_XRANDR
	     if ((Mode.screen.rotation == RR_Rotate_90) ||
		 (Mode.screen.rotation == RR_Rotate_270))
	       {
		  *dw = mode->vdisplay;
		  *dh = mode->hdisplay;
	       }
	     else
#endif
	       {
		  *dw = mode->hdisplay;
		  *dh = mode->vdisplay;
	       }
	     XF86VidModeLockModeSwitch(disp, scr, 0);
	     std_vid_mode_cur = GetModeIndex(dotclock, &curmode);
	     XF86VidModeSwitchToMode(disp, scr, mode);
	     XF86VidModeSetViewPort(disp, scr, rx, ry);
	     XF86VidModeLockModeSwitch(disp, scr, 1);
	     return 1;
	  }
     }
   else
     {
	mode = std_vid_modes[std_vid_mode_cur];
	XF86VidModeLockModeSwitch(disp, scr, 0);
	XF86VidModeSwitchToMode(disp, scr, mode);
	XF86VidModeSetViewPort(disp, scr, vp_x, vp_y);
#if 0				/* No, don't lock or we can't switch resolution */
	XF86VidModeLockModeSwitch(disp, scr, 1);
#endif
     }
   return 0;
}

static void
ZoomInit(void)
{
   int                 ev_base, er_base;

   zoom_can = -1;

   if (!XF86VidModeQueryExtension(disp, &ev_base, &er_base))
      return;

   XF86VidModeGetAllModeLines(disp, Dpy.screen,
			      &std_vid_modes_num, &std_vid_modes);

   if (std_vid_modes_num > 1)
      zoom_can = 1;
}

#else /* USE_ZOOM_XF86VM */

static int
SetMode(XRRScreenResources * xsr, RRCrtc crtc, RRMode mode, Rotation rot)
{
   return !XRRSetCrtcConfig(disp, xsr, crtc, EGetTimestamp(),
			    0, 0, mode, rot, xsr->outputs, 1);
}

static void
SetPanning(XRRScreenResources * xsr, RRCrtc crtc, int on)
{
   XRRPanning         *xpa = NULL;

   xpa = XRRGetPanning(disp, xsr, crtc);
   if (!xpa)
      return;

   Dprintf("Panning-A: %d,%d %dx%d trk: %d,%d %dx%d bdr: %d,%d,%d,%d\n",
	   xpa->left, xpa->top, xpa->width, xpa->height,
	   xpa->track_left, xpa->track_top,
	   xpa->track_width, xpa->track_height,
	   xpa->border_left, xpa->border_top,
	   xpa->border_right, xpa->border_bottom);

   xpa->timestamp = EGetTimestamp();
   xpa->left = xpa->top = 0;
   if (on)
     {
	xpa->width = WinGetW(VROOT);
	xpa->height = WinGetH(VROOT);
	xpa->track_width = xpa->track_height = 1;
     }
   else
     {
	xpa->width = xpa->height = 0;
	xpa->track_width = xpa->track_height = 0;
     }
   xpa->track_left = xpa->track_top = 0;
   xpa->border_left = xpa->border_top = 0;
   xpa->border_right = xpa->border_bottom = 0;

   Dprintf("Panning-B: %d,%d %dx%d trk: %d,%d %dx%d bdr: %d,%d,%d,%d\n",
	   xpa->left, xpa->top, xpa->width, xpa->height,
	   xpa->track_left, xpa->track_top,
	   xpa->track_width, xpa->track_height,
	   xpa->border_left, xpa->border_top,
	   xpa->border_right, xpa->border_bottom);

   XRRSetPanning(disp, xsr, crtc, xpa);
   XRRFreePanning(xpa);
}

#define SWAP(a, b) \
   do { int tmp; tmp = a; a = b; b = tmp; } while(0)

static RRMode       ss_mode;
static Rotation     ss_rot;

static              RRMode
FindMode(XRRScreenResources * xsr, int w, int h, int *dw, int *dh)
{
   int                 i, in, norm, best;
   RRMode              mode;

#if DEBUG_ZOOM
   int                 ic = 0;
#endif

   if (Mode.screen.rotation & (RR_Rotate_90 | RR_Rotate_270))
      SWAP(w, h);

   mode = 0;
   best = 0x7fffffff;
   in = -1;
   for (i = 0; i < xsr->nmode; i++)
     {
	Dprintf("Sz%2d: %dx%d\n", i, xsr->modes[i].width, xsr->modes[i].height);

#if DEBUG_ZOOM
	if (ss_mode == xsr->modes[i].id)
	   ic = i;
#endif
	if ((int)xsr->modes[i].width < w || (int)xsr->modes[i].height < h)
	   continue;
	norm = xsr->modes[i].width - w + xsr->modes[i].height - h;
	if (norm >= best)
	   continue;
	in = i;			/* new best */
	best = norm;
	mode = xsr->modes[i].id;
     }

   Dprintf("Cur i=%d mode=%#x WxH=%dx%d rot=%d\n", ic,
	   (unsigned int)xsr->modes[ic].id,
	   xsr->modes[ic].width, xsr->modes[ic].height, ss_rot);

   if (in < 0)
      return 0;

   /* Found a new mode */
   Dprintf("New i=%d mode=%#x WxH=%dx%d rot=%d\n", in,
	   (unsigned int)xsr->modes[in].id,
	   xsr->modes[in].width, xsr->modes[in].height, ss_rot);

   w = xsr->modes[in].width;
   h = xsr->modes[in].height;

   if (Mode.screen.rotation & (RR_Rotate_90 | RR_Rotate_270))
      SWAP(w, h);

   *dw = w;
   *dh = h;

   return mode;
}

static int
SwitchRes(char inout, int x __UNUSED__, int y __UNUSED__, int w, int h,
	  int *dw, int *dh)
{
   XRRScreenResources *xsr;
   XRRCrtcInfo        *xci;
   RRCrtc              crtc;
   RRMode              ss_mode_new;
   int                 ok = 0;

   Dprintf("%s: inout=%d\n", __func__, inout);

   xsr = XRRGetScreenResourcesCurrent(disp, WinGetXwin(VROOT));
   if (!xsr)
      goto done;
   crtc = xsr->crtcs[0];	/* FIXME - Which crtc? */

   if (inout)
     {
	/* Save current setup */
	xci = XRRGetCrtcInfo(disp, xsr, crtc);
	if (!xci)
	   goto done;
	ss_mode = xci->mode;
	ss_rot = xci->rotation;
	XRRFreeCrtcInfo(xci);

	/* Select zoomed setup */
	ss_mode_new = FindMode(xsr, w, h, dw, dh);

	/* Set zoomed setup */
	SetPanning(xsr, crtc, 1);

	ok = SetMode(xsr, crtc, ss_mode_new, ss_rot);
     }
   else
     {
	/* Revert to original setup */
	ok = SetMode(xsr, crtc, ss_mode, ss_rot);

	SetPanning(xsr, crtc, 0);
     }

 done:
   if (xsr)
      XRRFreeScreenResources(xsr);

   Dprintf("%s: ok=%d\n", __func__, ok);
   return ok;
}

static void
ZoomInit(void)
{
   zoom_can = 1;
}

#endif /* USE_ZOOM_XF86VM */

static int          zw, zh;

static void
_ZoomEwinRestore(EWin * ewin)
{
   EwinBorderSetTo(ewin, ewin->normal_border);
   ewin->state.zoomed = 0;
   EwinMoveResize(ewin, ewin->save_fs.x, ewin->save_fs.y,
		  ewin->client.w, ewin->client.h, 0);
}

/* outstanding BUG: zooming on shaped windows leaves stuff exposed beneath them..... */
void
Zoom(EWin * ewin, int on)
{
   if (Mode.wm.window)
      return;

   if (!ewin)
      ewin = zoom_last_ewin;
   if (!ewin)
      return;

   if (zoom_can == 0)
      ZoomInit();

   if (zoom_can <= 0)
      return;

   Dprintf("%s: on=%d\n", __func__, on);
   if (!on)
     {
	/* Unzoom */

	if (ewin != zoom_last_ewin)
	   return;

	_ZoomEwinRestore(ewin);
	SwitchRes(0, 0, 0, 0, 0, NULL, NULL);
	zw = zh = 0;
	zoom_last_ewin = NULL;
     }
   else if (ewin == zoom_last_ewin)
     {
	/* Already zoomed */
	return;
     }
   else
     {
	/* Zoom */

	if (ewin->state.fullscreen)
	   return;

	if (!zoom_last_ewin)	/* first zoom */
	  {
	     on = SwitchRes(1, 0, 0, ewin->client.w, ewin->client.h, &zw, &zh);
	  }
	else			/* we are zoomed in on another window already.... */
	  {
	     _ZoomEwinRestore(zoom_last_ewin);
	     if ((ewin->client.w <= zw) && (ewin->client.h <= zh) &&
		 ((ewin->client.w >= zw / 2) || (ewin->client.h >= zh / 2)))
	       {
		  /* YAY no need to change resolution :-D */
	       }
	     else
	       {
		  /* SwitchRes only tracks the LAST mode, so we have to switch back to
		   * the original mode before switching to new target mode
		   * so that we can restore the original vid mode when we zoom
		   * out of the last window ;( */
		  SwitchRes(0, 0, 0, 0, 0, NULL, NULL);
		  on = SwitchRes(1, 0, 0, ewin->client.w, ewin->client.h,
				 &zw, &zh);
	       }
	  }

	Dprintf("%s: SwitchRes=%d - client size %dx%d -> screen %dx%d\n",
		__func__, on, ewin->client.w, ewin->client.h, zw, zh);
	if (!on)
	   return;

	ewin->save_fs.x = EoGetX(ewin);
	ewin->save_fs.y = EoGetY(ewin);
	EwinRaise(ewin);
	EwinBorderSetTo(ewin, BorderCreateFiller(ewin->client.w,
						 ewin->client.h, zw, zh));
	EwinMoveResize(ewin, 0, 0, ewin->client.w, ewin->client.h, 0);
	ewin->state.zoomed = 1;
	FocusToEWin(ewin, FOCUS_SET);
	zoom_last_ewin = ewin;
     }

   EwinWarpTo(ewin, 1);
   ESync(0);
   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);
}

void
ReZoom(EWin * ewin __UNUSED__)
{
#if 0				/* Move/resize is disabled while zoomed */
   if (zoom_last_ewin && ewin == zoom_last_ewin)
     {
	Zoom(ewin, 0);
	Zoom(ewin, 1);
     }
#endif
}

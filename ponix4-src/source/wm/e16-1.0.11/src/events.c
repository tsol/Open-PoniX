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
#include "aclass.h"
#include "emodule.h"
#include "events.h"
#include "timers.h"
#include "tooltips.h"
#include "xwin.h"
#include <sys/time.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#if USE_XSYNC
#include <X11/extensions/sync.h>
#endif
#if USE_XSCREENSAVER
#include <X11/extensions/scrnsaver.h>
#endif
#if USE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif
#if USE_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#if USE_COMPOSITE
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrender.h>
#endif
#if USE_GLX
#include <GL/glx.h>
#endif
#if USE_XI2
#include <X11/extensions/XInput2.h>
#endif

#if ENABLE_DEBUG_EVENTS
static const char  *EventName(unsigned int type);
#endif

/*
 * Server extension handling
 */

typedef struct {
   int                 major, minor;
   int                 major_op, event_base, error_base;
} EServerExtData;

typedef struct {
   const char         *name;
   unsigned int        ix;
   int                 (*query_ver) (Display * dpy, int *major, int *minor);
   void                (*init) (int avaliable);
} EServerExt;

static EServerExtData ExtData[11];

#define event_base_shape ExtData[XEXT_SHAPE].event_base
#define event_base_randr ExtData[XEXT_RANDR].event_base
#define event_base_damage ExtData[XEXT_DAMAGE].event_base
#define event_base_saver  ExtData[XEXT_SCRSAVER].event_base
#define event_base_glx    ExtData[XEXT_GLX].event_base
#define major_op_xi       ExtData[XEXT_XI].major_op

static void
ExtInitShape(int available)
{
   if (available)
      return;

   AlertX(_("X server setup error"), _("OK"), NULL, NULL,
	  _("FATAL ERROR:\n" "\n"
	    "This Xserver does not support the Shape extension.\n"
	    "This is required for Enlightenment to run.\n" "\n"
	    "Your Xserver probably is too old or mis-configured.\n" "\n"
	    "Exiting.\n"));
   EExit(1);
}

#if USE_XSYNC
static void
ExtInitSync(int available)
{
   int                 i, num;
   XSyncSystemCounter *xssc;

   if (!available)
      return;

   xssc = XSyncListSystemCounters(disp, &num);
   for (i = 0; i < num; i++)
     {
	if (!strcmp(xssc[i].name, "SERVERTIME"))
	   Mode.display.server_time = xssc[i].counter;
	if (EDebug(EDBUG_TYPE_VERBOSE))
	   Eprintf(" Sync counter %2d: %10s %#lx %#x:%#x\n", i,
		   xssc[i].name, xssc[i].counter,
		   XSyncValueHigh32(xssc[i].resolution),
		   XSyncValueLow32(xssc[i].resolution));
     }
   XSyncFreeSystemCounterList(xssc);

   if (Mode.display.server_time == None)
      Conf.movres.enable_sync_request = 0;
}
#endif

#if USE_XSCREENSAVER
static void
ExtInitSS(int available)
{
   if (!available)
      return;

   if (EDebug(EDBUG_TYPE_VERBOSE))
     {
	XScreenSaverInfo   *xssi = XScreenSaverAllocInfo();

	XScreenSaverQueryInfo(disp, WinGetXwin(VROOT), xssi);
	Eprintf(" Screen saver window=%#lx\n", xssi->window);
	XFree(xssi);
     }
   XScreenSaverSelectInput(disp, WinGetXwin(VROOT),
			   ScreenSaverNotifyMask | ScreenSaverCycleMask);
}
#endif

#if USE_XRANDR
static void
ExtInitRR(int available)
{
   Rotation            rot;

   if (!available)
      return;

   /* Listen for RandR events */
   XRRSelectInput(disp, WinGetXwin(VROOT), RRScreenChangeNotifyMask);
   XRRRotations(disp, Dpy.screen, &rot);
   Mode.screen.rotation = rot;

#if 0				/* Debug */
   if (EDebug(EDBUG_TYPE_VERBOSE))
     {
	XRRScreenResources *psr;
	XRRCrtcInfo        *pci;
	XRROutputInfo      *poi;
	int                 i;

	psr = XRRGetScreenResources(disp, WinGetXwin(VROOT));
	if (!psr)
	   return;

	Eprintf("CRTC  ID      X,Y         WxH     mode   rot   nout\n");
	for (i = 0; i < psr->ncrtc; i++)
	  {
	     pci = XRRGetCrtcInfo(disp, psr, psr->crtcs[i]);
	     if (!pci)
		break;
	     Eprintf("%3d  %#04lx  %4d,%4d   %4ux%4u  %#04lx %4d %5d\n",
		     i, psr->crtcs[i],
		     pci->x, pci->y, pci->width, pci->height,
		     pci->mode, pci->rotation, pci->noutput);
	     XRRFreeCrtcInfo(pci);
	  }

	Eprintf("OUTP  ID  Name            WxH     crtc  ncrtc nclon nmode\n");
	for (i = 0; i < psr->noutput; i++)
	  {
	     poi = XRRGetOutputInfo(disp, psr, psr->outputs[i]);
	     if (!poi)
		break;
	     Eprintf("%3d  %#04lx %-8s     %4lux%4lu  %#04lx %4d %5d %5d\n",
		     i, psr->outputs[i],
		     poi->name, poi->mm_width, poi->mm_height,
		     poi->crtc, poi->ncrtc, poi->nclone, poi->nmode);
	     XRRFreeOutputInfo(poi);
	  }

	XRRFreeScreenResources(psr);
     }
#endif
}
#endif

#if USE_XI2
static              Status
EInputQueryVersion(Display * dpy,
		   int *major_version_return, int *minor_version_return)
{
   *major_version_return = XI_2_Major;
   *minor_version_return = XI_2_Minor;

   return XIQueryVersion(dpy, major_version_return, minor_version_return);
}

#include "X11/extensions/XInput.h"

static void
ExtInitInput(int available)
{
   int                 i, j, nd;
   XDeviceInfo        *dvi, *dvis;
   char               *p;

   if (!available)
      return;

   if (!EDebug(EDBUG_TYPE_VERBOSE))
      return;

   dvis = XListInputDevices(disp, &nd);
   Eprintf("Dev  id nc use name\n");
   for (i = 0; i < nd; i++)
     {
	dvi = dvis + i;
	Eprintf(" %2d %#3lx  %d  %d %-32s", i,
		dvi->id, dvi->num_classes, dvi->use, dvi->name);
	p = (char *)dvi->inputclassinfo;
	for (j = 0; j < dvi->num_classes; j++)
	  {
	     XAnyClassPtr        pcany = (XAnyClassPtr) p;

	     printf("  %#3lx %3d", pcany->class, pcany->length);
	     p += pcany->length;
	  }
	printf("\n");

     }
   XFreeDeviceList(dvis);
}
#endif

static const EServerExt Extensions[] = {
   {"SHAPE", XEXT_SHAPE, XShapeQueryVersion, ExtInitShape},
#if USE_XSYNC
   {"SYNC", XEXT_SYNC, XSyncInitialize, ExtInitSync},
#endif
#if USE_XSCREENSAVER
   {"MIT-SCREEN-SAVER", XEXT_SCRSAVER, XScreenSaverQueryVersion, ExtInitSS},
#endif
#if USE_XRANDR
   {"RANDR", XEXT_RANDR, XRRQueryVersion, ExtInitRR},
#endif
#if USE_XINERAMA
   {"XINERAMA", XEXT_XINERAMA, XineramaQueryVersion, NULL},
#endif
#if USE_COMPOSITE
   {"Composite", XEXT_COMPOSITE, XCompositeQueryVersion, NULL},
   {"DAMAGE", XEXT_DAMAGE, XDamageQueryVersion, NULL},
   {"XFIXES", XEXT_FIXES, XFixesQueryVersion, NULL},
   {"RENDER", XEXT_RENDER, XRenderQueryVersion, NULL},
#endif
#if USE_GLX
   {"GLX", XEXT_GLX, glXQueryVersion, NULL},
#endif
#if USE_XI2
   {"XInputExtension", XEXT_XI, EInputQueryVersion, ExtInitInput},
#endif
};

static void
ExtQuery(const EServerExt * ext)
{
   int                 available;
   EServerExtData     *exd = ExtData + ext->ix;

   available = XQueryExtension(disp, ext->name, &(exd->major_op),
			       &(exd->event_base), &(exd->error_base));

   if (available)
     {
	Mode.server.extensions |= 1 << ext->ix;

	ext->query_ver(disp, &(exd->major), &(exd->minor));

	if (EDebug(EDBUG_TYPE_VERBOSE))
	   Eprintf("Extension %-15s version %d.%d -"
		   " req/evt/err base = %3d/%3d/%3d\n", ext->name,
		   exd->major, exd->minor,
		   exd->major_op, exd->event_base, exd->error_base);
     }

   if (ext->init)
      ext->init(available);
}

/*
 * File descriptor handling
 */

struct _EventFdDesc {
#if 0				/* Unused */
   const char         *name;
#endif
   int                 fd;
   void                (*handler) (void);
};

static int          nfds = 0;
static EventFdDesc *pfds = NULL;

EventFdDesc        *
EventFdRegister(int fd, EventFdHandler * handler)
{
   nfds++;
   pfds = EREALLOC(EventFdDesc, pfds, nfds);
   pfds[nfds - 1].fd = fd;
   pfds[nfds - 1].handler = handler;

   return pfds + (nfds - 1);
}

void
EventFdUnregister(EventFdDesc * efd)
{
   efd->fd = -1;
}

/*
 * Event handling
 */

#define DOUBLE_CLICK_TIME 250	/* Milliseconds */

void
EventsInit(void)
{
   unsigned int        i;

   memset(ExtData, 0, sizeof(ExtData));

   for (i = 0; i < sizeof(Extensions) / sizeof(EServerExt); i++)
      ExtQuery(Extensions + i);

#if USE_COMPOSITE
#define XEXT_MASK_CM_ALL ((1 << XEXT_COMPOSITE) | (1 << XEXT_DAMAGE) | \
                          (1 << XEXT_FIXES) | (1 << XEXT_RENDER))
   if (((Mode.server.extensions & XEXT_MASK_CM_ALL) == XEXT_MASK_CM_ALL) &&
       (ExtData[XEXT_COMPOSITE].major > 0 ||
	ExtData[XEXT_COMPOSITE].minor >= 2))
      Mode.server.extensions |= 1 << XEXT_CM_ALL;
#endif

   EventFdRegister(ConnectionNumber(disp), NULL);
}

static const char  *
EventsGetExtensionName(int req)
{
   unsigned int        i;
   EServerExtData     *exd;

   for (i = 0; i < sizeof(Extensions) / sizeof(EServerExt); i++)
     {
	exd = ExtData + Extensions[i].ix;
	if (req == exd->major_op)
	   return Extensions[i].name;
     }

   return "?";
}

void
EventShowError(const XErrorEvent * ev)
{
   Display            *dpy = disp;
   char                buf[64], buf1[64];

   if (ev->request_code < 128)
      Esnprintf(buf, sizeof(buf), "%d", ev->request_code);
   else
      Esnprintf(buf, sizeof(buf), "%s.%d",
		EventsGetExtensionName(ev->request_code), ev->minor_code);
   XGetErrorDatabaseText(dpy, "XRequest", buf, "", buf1, sizeof(buf1));
   XGetErrorText(dpy, ev->error_code, buf, sizeof(buf));
   Eprintf("*** ERROR: xid=%#lx req=%i/%i err=%i: %s: %s\n",
	   ev->resourceid, ev->request_code, ev->minor_code,
	   ev->error_code, buf1, buf);
}

int
EventsUpdateXY(int *px, int *py)
{
   int                 ss;

   ss = EQueryPointer(NULL, &Mode.events.cx, &Mode.events.cy, NULL, NULL);
   if (px)
      *px = Mode.events.cx;
   if (py)
      *py = Mode.events.cy;

   return ss;
}

static void
ModeGetXY(int rx, int ry)
{
   /* Mode.wm.win_x/y should always be 0 if not in window mode */
   Mode.events.cx = rx - Mode.wm.win_x;
   Mode.events.cy = ry - Mode.wm.win_y;
   if (Mode.wm.window)
     {
	if (rx < Mode.wm.win_x || rx >= Mode.wm.win_x + Mode.wm.win_w ||
	    ry < Mode.wm.win_y || ry >= Mode.wm.win_y + Mode.wm.win_h)
	   Mode.events.on_screen = 0;
     }
}

static void
HandleEvent(XEvent * ev)
{
   Win                 win;

#if ENABLE_DEBUG_EVENTS
   if (EDebug(ev->type))
      EventShow(ev);
#endif

   win = ELookupXwin(ev->xany.window);

   switch (ev->type)
     {
     case KeyPress:
	Mode.events.last_keycode = ev->xkey.keycode;
	Mode.events.last_keystate = ev->xkey.state;
	/* FALLTHROUGH */
     case KeyRelease:
	Mode.events.time = ev->xkey.time;
	ModeGetXY(ev->xkey.x_root, ev->xkey.y_root);
#if 0				/* FIXME - Why? */
	if (ev->xkey.root != WinGetXwin(VROOT))
	  {
	     XSetInputFocus(disp, ev->xkey.root, RevertToPointerRoot,
			    CurrentTime);
	     ESync();
	     ev->xkey.time = CurrentTime;
	     EXSendEvent(ev->xkey.root, 0, ev);
	     return;
	  }
#endif
	Mode.events.on_screen = ev->xkey.same_screen;
	goto do_stuff;

     case ButtonPress:
     case ButtonRelease:
	Mode.events.time = ev->xbutton.time;
	ModeGetXY(ev->xbutton.x_root, ev->xbutton.y_root);
	Mode.events.on_screen = ev->xbutton.same_screen;
	TooltipsHide();
	goto do_stuff;

     case MotionNotify:
	Mode.events.time = ev->xmotion.time;
	Mode.events.px = Mode.events.mx;
	Mode.events.py = Mode.events.my;
	ModeGetXY(ev->xmotion.x_root, ev->xmotion.y_root);
	Mode.events.mx = Mode.events.cx;
	Mode.events.my = Mode.events.cy;
	Mode.events.on_screen = ev->xmotion.same_screen;
	break;

     case EnterNotify:
	Mode.context_win = win;
	Mode.events.time = ev->xcrossing.time;
	Mode.events.on_screen = ev->xcrossing.same_screen;
	if (ev->xcrossing.mode == NotifyGrab &&
	    ev->xcrossing.detail == NotifyInferior)
	  {
	     Mode.grabs.pointer_grab_window = ev->xany.window;
	     if (!Mode.grabs.pointer_grab_active)
		Mode.grabs.pointer_grab_active = 2;
	  }
	ModeGetXY(ev->xcrossing.x_root, ev->xcrossing.y_root);
	TooltipsHide();
	goto do_stuff;

     case LeaveNotify:
	Mode.events.time = ev->xcrossing.time;
	Mode.events.on_screen = ev->xcrossing.same_screen;
	if (ev->xcrossing.mode == NotifyGrab &&
	    ev->xcrossing.detail == NotifyInferior)
	  {
	     Mode.grabs.pointer_grab_window = None;
	     Mode.grabs.pointer_grab_active = 0;
	  }
	ModeGetXY(ev->xcrossing.x_root, ev->xcrossing.y_root);
	TooltipsHide();
	goto do_stuff;

     case PropertyNotify:
	Mode.events.time = ev->xproperty.time;
	break;

      do_stuff:
	if (ev->xany.window == WinGetXwin(VROOT))
	   ActionclassesGlobalEvent(ev);
	break;
     }

   switch (ev->type)
     {
     case KeyPress:		/*  2 */
     case KeyRelease:		/*  3 */
	/* Unfreeze keyboard in case we got here by keygrab */
	XAllowEvents(disp, AsyncKeyboard, CurrentTime);
	break;

     case ButtonPress:		/*  4 */
	SoundPlay(SOUND_BUTTON_CLICK);

	Mode.events.double_click =
	   ((ev->xbutton.time - Mode.events.last_btime < DOUBLE_CLICK_TIME) &&
	    ev->xbutton.button == Mode.events.last_button &&
	    ev->xbutton.window == Mode.events.last_bpress2);

	Mode.events.last_bpress = ev->xbutton.window;
	Mode.events.last_bpress2 = ev->xbutton.window;
	Mode.events.last_btime = ev->xbutton.time;
	Mode.events.last_button = ev->xbutton.button;
	break;
     case ButtonRelease:	/*  5 */
	SoundPlay(SOUND_BUTTON_RAISE);
	break;
     }

   /* The new event dispatcher */
   EventCallbacksProcess(win, ev);

   /* Post-event stuff TBD */
   switch (ev->type)
     {
     case ButtonRelease:	/*  5 */
	Mode.events.last_bpress = 0;
	break;

#if 1				/* Do this here? */
     case DestroyNotify:
	EUnregisterXwin(ev->xdestroywindow.window);
	break;
#endif

     case MappingNotify:
	XRefreshKeyboardMapping(&ev->xmapping);
	if (Conf.testing.bindings_reload)
	   ActionclassesReload();
	break;
     }
}

static void
EventsCompress(XEvent * evq, int count)
{
   XEvent             *ev, *ev2;
   int                 i, j, n;
   int                 xa, ya, xb, yb;
   int                 type;

#if ENABLE_DEBUG_EVENTS
   /* Debug - should be taken out */
   if (EDebug(EDBUG_TYPE_COMPRESSION))
      for (i = 0; i < count; i++)
	 if (evq[i].type)
	    Eprintf("EventsCompress-1 %3d %s w=%#lx\n", i,
		    EventName(evq[i].type), evq[i].xany.window);
#endif

   /* Loop through event list, starting with latest */
   for (i = count - 1; i >= 0; i--)
     {
	ev = evq + i;

	type = ev->type;
	switch (type)
	  {
	  case 0:
	     /* Already thrown away */
	  default:
	     break;

	  case MotionNotify:
	     /* Discard all but last motion event */
	     j = i - 1;
	     n = 0;
	     for (; j >= 0; j--)
	       {
		  ev2 = evq + j;
		  if (ev2->type == type)
		    {
		       n++;
		       ev2->type = 0;
		    }
	       }
#if ENABLE_DEBUG_EVENTS
	     if (n && EDebug(EDBUG_TYPE_COMPRESSION))
		Eprintf("EventsCompress n=%4d %s %#lx x,y = %d,%d\n",
			n, EventName(type), ev->xmotion.window,
			ev->xmotion.x, ev->xmotion.y);
#endif
	     break;

	  case LeaveNotify:
	     for (j = i - 1; j >= 0; j--)
	       {
		  ev2 = evq + j;
		  if (ev2->type == EnterNotify)
		    {
		       if (ev2->xcrossing.window == ev->xcrossing.window)
			  goto do_enter_leave_nuked;
		    }
	       }
	     break;
	   do_enter_leave_nuked:
	     ev2->type = ev->type = 0;
	     for (n = i - 1; n > j; n--)
	       {
		  ev2 = evq + n;
		  if (ev2->type == MotionNotify)
		    {
		       if (ev2->xmotion.window != ev->xmotion.window)
			  continue;
		       ev2->type = 0;
		    }
	       }
#if ENABLE_DEBUG_EVENTS
	     if (EDebug(EDBUG_TYPE_COMPRESSION))
		Eprintf("EventsCompress n=%4d %s %#lx\n",
			1, EventName(type), ev->xcrossing.window);
#endif
	     break;

	  case DestroyNotify:
	     for (j = i - 1; j >= 0; j--)
	       {
		  ev2 = evq + j;
		  switch (ev2->type)
		    {
		    case CreateNotify:
		       if (ev2->xcreatewindow.window !=
			   ev->xdestroywindow.window)
			  continue;
		       ev2->type = EX_EVENT_CREATE_GONE;
		       j = -1;	/* Break for() */
		       break;
		    case DestroyNotify:
		       break;
		    case UnmapNotify:
		       if (ev2->xunmap.window != ev->xdestroywindow.window)
			  continue;
		       ev2->type = EX_EVENT_UNMAP_GONE;
		       break;
		    case MapNotify:
		       if (ev2->xmap.window != ev->xdestroywindow.window)
			  continue;
		       ev2->type = EX_EVENT_MAP_GONE;
		       break;
		    case MapRequest:
		       if (ev2->xmaprequest.window != ev->xdestroywindow.window)
			  continue;
		       ev2->type = EX_EVENT_MAPREQUEST_GONE;
		       break;
		    case ReparentNotify:
		       if (ev2->xreparent.window != ev->xdestroywindow.window)
			  continue;
		       ev2->type = EX_EVENT_REPARENT_GONE;
		       break;
		    case ConfigureRequest:
		       if (ev2->xconfigurerequest.window !=
			   ev->xdestroywindow.window)
			  continue;
		       ev2->type = 0;
		       break;
		    default:
		       /* Nuke all other events on a destroyed window */
		       if (ev2->xany.window != ev->xdestroywindow.window)
			  continue;
		       ev2->type = 0;
		       break;
		    }
	       }
	     break;

	  case Expose:
	     n = 0;
	     xa = ev->xexpose.x;
	     xb = xa + ev->xexpose.width;
	     ya = ev->xexpose.y;
	     yb = ya + ev->xexpose.height;
	     for (j = i - 1; j >= 0; j--)
	       {
		  ev2 = evq + j;
		  if (ev2->type == type &&
		      ev2->xexpose.window == ev->xexpose.window)
		    {
		       n++;
		       ev2->type = 0;
		       if (xa > ev2->xexpose.x)
			  xa = ev2->xexpose.x;
		       if (xb < ev2->xexpose.x + ev2->xexpose.width)
			  xb = ev2->xexpose.x + ev2->xexpose.width;
		       if (ya > ev2->xexpose.y)
			  ya = ev2->xexpose.y;
		       if (yb < ev2->xexpose.y + ev2->xexpose.height)
			  yb = ev2->xexpose.y + ev2->xexpose.height;
		    }
	       }
	     if (n)
	       {
		  ev->xexpose.x = xa;
		  ev->xexpose.width = xb - xa;
		  ev->xexpose.y = ya;
		  ev->xexpose.height = yb - ya;
	       }
#if ENABLE_DEBUG_EVENTS
	     if (EDebug(EDBUG_TYPE_COMPRESSION))
		Eprintf("EventsCompress n=%4d %s %#lx x=%4d-%4d y=%4d-%4d\n",
			n, EventName(type), ev->xexpose.window, xa, xb, ya, yb);
#endif
	     break;

	  case EX_EVENT_SHAPE_NOTIFY:
	     n = 0;
	     for (j = i - 1; j >= 0; j--)
	       {
		  ev2 = evq + j;
		  if (ev2->type == type && ev2->xany.window == ev->xany.window)
		    {
		       n++;
		       ev2->type = 0;
		    }
	       }
#if ENABLE_DEBUG_EVENTS
	     if (n && EDebug(EDBUG_TYPE_COMPRESSION))
		Eprintf("EventsCompress n=%4d %s %#lx\n",
			n, EventName(type), ev->xmotion.window);
#endif
	     break;

	  case GraphicsExpose:
	  case NoExpose:
	     /* Not using these */
	     ev->type = 0;
	     break;
	  }
     }

#if ENABLE_DEBUG_EVENTS
   /* Debug - should be taken out */
   if (EDebug(EDBUG_TYPE_COMPRESSION))
      for (i = 0; i < count; i++)
	 if (evq[i].type)
	    Eprintf("EventsCompress-2 %3d %s w=%#lx\n", i,
		    EventName(evq[i].type), evq[i].xany.window);
#endif
}

#if USE_XI2
static void
EventFetchXI2(XEvent * ev)
{
   XIDeviceEvent      *xie;

   if (!XGetEventData(disp, &ev->xcookie))
      return;

   xie = (XIDeviceEvent *) ev->xcookie.data;

   if (EDebug(EDBUG_TYPE_XI2))
      Eprintf("%s: %#lx: XI2 ext=%d type=%d devid=%d srcid=%d\n",
	      "EventsFetch", xie->event, xie->extension,
	      xie->evtype, xie->deviceid, xie->sourceid);

   switch (ev->xcookie.evtype)
     {
     default:
	break;
     case XI_ButtonPress:
     case XI_ButtonRelease:
     case XI_Motion:
	ev->type = ev->xcookie.evtype;
	ev->xbutton.window = xie->event;
	ev->xbutton.root = xie->root;
	ev->xbutton.subwindow = xie->child;
	ev->xbutton.time = xie->time;
	ev->xbutton.x = (int)xie->event_x;
	ev->xbutton.y = (int)xie->event_y;
	ev->xbutton.x_root = (int)xie->root_x;
	ev->xbutton.y_root = (int)xie->root_y;
	ev->xbutton.button = xie->detail;
	ev->xbutton.state = xie->mods.effective;
	ev->xbutton.same_screen = 1;	/* FIXME */
	break;
     }
   XFreeEventData(disp, &ev->xcookie);
}
#endif

static int
EventsFetch(XEvent ** evq_p, int *evq_n)
{
   int                 i, n, count;
   XEvent             *evq = *evq_p, *ev;
   int                 qsz = *evq_n;

   /* Fetch the entire event queue */
   for (i = count = 0; (n = XPending(disp)) > 0;)
     {
	count += n;
	if (count > qsz)
	  {
	     qsz = count;
	     evq = EREALLOC(XEvent, evq, qsz);
	  }
	ev = evq + i;
	for (; i < count; i++, ev++)
	  {
	     XNextEvent(disp, ev);
#if USE_XI2
	     if (ev->type == GenericEvent)
	       {
		  if (ev->xcookie.extension == major_op_xi)
		     EventFetchXI2(ev);
		  continue;
	       }
#endif /* USE_XI2 */

	     /* Map some event types to E internals */
	     if (ev->type == event_base_shape + ShapeNotify)
		ev->type = EX_EVENT_SHAPE_NOTIFY;
#if USE_XRANDR
	     else if (ev->type == event_base_randr + RRScreenChangeNotify)
		ev->type = EX_EVENT_SCREEN_CHANGE_NOTIFY;
#endif
#if USE_COMPOSITE
	     else if (ev->type == event_base_damage + XDamageNotify)
		ev->type = EX_EVENT_DAMAGE_NOTIFY;
#endif
#if USE_XSCREENSAVER
	     else if (ev->type == event_base_saver + ScreenSaverNotify)
		ev->type = EX_EVENT_SAVER_NOTIFY;
#endif
#if USE_GLX
	     else if (ev->type == event_base_glx + GLX_BufferSwapComplete)
		ev->type = EX_EVENT_GLX_FLIP;
#endif
	  }
     }

   EventsCompress(evq, count);

   *evq_p = evq;
   *evq_n = qsz;

   return count;
}

static int
EventsProcess(XEvent ** evq_p, int *evq_n, int *evq_f)
{
   int                 i, n, count;
   XEvent             *evq;

   /* Fetch the entire event queue */
   n = EventsFetch(evq_p, evq_n);
   evq = *evq_p;

   if (EDebug(EDBUG_TYPE_EVENTS) > 1)
      Eprintf("EventsProcess-B %d\n", n);

   for (i = count = 0; i < n; i++)
     {
	if (evq[i].type == 0)
	   continue;

	if (EDebug(EDBUG_TYPE_EVENTS) > 2)
	   EventShow(evq + i);

	count++;
	HandleEvent(evq + i);
	evq[i].type = 0;
     }

   if (EDebug(EDBUG_TYPE_EVENTS) > 1)
      Eprintf("EventsProcess-E %d/%d\n", count, n);

   if (n > *evq_f)
      *evq_f = n;

   return count;
}

/*
 * This is the primary event loop.  Everything that is going to happen in the
 * window manager has to start here at some point.  This is where all the
 * events from the X server are interpreted, timer events are inserted, etc
 */
void
EventsMain(void)
{
   static int          evq_alloc = 0;
   static int          evq_fetch = 0;
   static XEvent      *evq_ptr = NULL;
   fd_set              fdset;
   struct timeval      tval;
   unsigned int        time1, time2;
   int                 dt;
   int                 count, pfetch;
   int                 fdsize, fd, i;

   time1 = GetTimeMs();

   for (;;)
     {
	pfetch = 0;
	count = EventsProcess(&evq_ptr, &evq_alloc, &pfetch);

	if (pfetch)
	  {
	     evq_fetch =
		(pfetch > evq_fetch) ? pfetch : (3 * evq_fetch + pfetch) / 4;
	     if (EDebug(EDBUG_TYPE_EVENTS) > 1)
		Eprintf("EventsMain - Alloc/fetch/pfetch/peak=%d/%d/%d/%d)\n",
			evq_alloc, evq_fetch, pfetch, count);
	     if ((evq_ptr) && ((evq_alloc - evq_fetch) > 64))
	       {
		  evq_alloc = 0;
		  Efree(evq_ptr);
		  evq_ptr = NULL;
	       }
	  }

	/* time2 = current time */
	time2 = GetTimeMs();
	dt = time2 - time1;
	Mode.events.time_ms = time1 = time2;
	/* dt = time spent since we last were here */

	/* Run all expired timers */
	TimersRun(time2);

	/* Run idlers */
	IdlersRun();

	if (Mode.wm.exit_mode)
	   break;

	if (XPending(disp))
	   continue;

	FD_ZERO(&fdset);
	fdsize = -1;
	for (i = 0; i < nfds; i++)
	  {
	     fd = pfds[i].fd;
	     if (fd < 0)
		continue;
	     if (fdsize < fd)
		fdsize = fd;
	     FD_SET(fd, &fdset);
	  }
	fdsize++;

	/* Get time to first non-expired (0 means none) */
	time2 = TimersRunNextIn(time2);
	if (time2 > 0.)
	  {
	     tval.tv_sec = (long)time2 / 1000;
	     tval.tv_usec = ((long)time2 - tval.tv_sec * 1000) * 1000;
	     count = select(fdsize, &fdset, NULL, NULL, &tval);
	  }
	else
	  {
	     count = select(fdsize, &fdset, NULL, NULL, NULL);
	  }

	if (EDebug(EDBUG_TYPE_EVENTS))
	   Eprintf
	      ("EventsMain - count=%d xfd=%d:%d dt=%.6lf time2=%.6lf\n",
	       count, pfds[0].fd, FD_ISSET(pfds[0].fd, &fdset), dt * 1e-3,
	       time2 * 1e-3);

	if (count <= 0)
	   continue;		/* Timeout (or error) */

	/* Excluding X fd */
	for (i = 1; i < nfds; i++)
	  {
	     fd = pfds[i].fd;
	     if ((fd >= 0) && (FD_ISSET(fd, &fdset)))
	       {
		  if (EDebug(EDBUG_TYPE_EVENTS) > 1)
		     Eprintf("Event fd %d\n", i);
		  pfds[i].handler();
	       }
	  }
     }
}

#if ENABLE_DEBUG_EVENTS
/*
 * Event debug stuff
 */

static const char  *const TxtEventNames[] = {
   "Error", "Reply", "KeyPress", "KeyRelease", "ButtonPress",
   "ButtonRelease", "MotionNotify", "EnterNotify", "LeaveNotify", "FocusIn",
   "FocusOut", "KeymapNotify", "Expose", "GraphicsExpose", "NoExpose",
   "VisibilityNotify", "CreateNotify", "DestroyNotify", "UnmapNotify",
   "MapNotify",
   "MapRequest", "ReparentNotify", "ConfigureNotify", "ConfigureRequest",
   "GravityNotify",
   "ResizeRequest", "CirculateNotify", "CirculateRequest", "PropertyNotify",
   "SelectionClear",
   "SelectionRequest", "SelectionNotify", "ColormapNotify", "ClientMessage",
   "MappingNotify"
};
#define N_EVENT_NAMES (sizeof(TxtEventNames)/sizeof(char*))

static const char  *
EventName(unsigned int type)
{
   static char         buf[16];

   if (type < N_EVENT_NAMES)
      return TxtEventNames[type];

   switch (type)
     {
     case EX_EVENT_CREATE_GONE:
	return "Create-Gone";
     case EX_EVENT_UNMAP_GONE:
	return "Unmap-Gone";
     case EX_EVENT_MAP_GONE:
	return "Map-Gone";
     case EX_EVENT_MAPREQUEST_GONE:
	return "MapRequest-Gone";
     case EX_EVENT_REPARENT_GONE:
	return "Reparent-Gone";
     case EX_EVENT_SHAPE_NOTIFY:
	return "ShapeNotify";
#if USE_XSCREENSAVER
     case EX_EVENT_SAVER_NOTIFY:
	return "ScreenSaverNotify";
#endif
#if USE_XRANDR
     case EX_EVENT_SCREEN_CHANGE_NOTIFY:
	return "ScreenChangeNotify";
#endif
#if USE_COMPOSITE
     case EX_EVENT_DAMAGE_NOTIFY:
	return "DamageNotify";
#endif
     }

   sprintf(buf, "%d", type);
   return buf;
}

static const char  *const TxtEventNotifyModeNames[] = {
   "NotifyNormal", "NotifyGrab", "NotifyUngrab", "NotifyWhileGrabbed"
};
#define N_EVENT_NOTIFY_MODE_NAMES (sizeof(TxtEventNotifyModeNames)/sizeof(char*))

static const char  *
EventNotifyModeName(unsigned int mode)
{
   if (mode < N_EVENT_NOTIFY_MODE_NAMES)
      return TxtEventNotifyModeNames[mode];

   return "Unknown";
}

static const char  *const TxtEventNotifyDetailNames[] = {
   "NotifyAncestor", "NotifyVirtual", "NotifyInferior", "NotifyNonlinear",
   "NotifyNonlinearVirtual", "NotifyPointer", "NotifyPointerRoot",
   "NotifyDetailNone"
};
#define N_EVENT_NOTIFY_DETAIL_NAMES (sizeof(TxtEventNotifyDetailNames)/sizeof(char*))

static const char  *
EventNotifyDetailName(unsigned int detail)
{
   if (detail < N_EVENT_NOTIFY_DETAIL_NAMES)
      return TxtEventNotifyDetailNames[detail];

   return "Unknown";
}

void
EventShow(const XEvent * ev)
{
   char               *txt, buf[64];

   Esnprintf(buf, sizeof(buf), "%#08lx %cEV-%s ev=%#lx",
	     ev->xany.serial, (ev->xany.send_event) ? '*' : ' ',
	     EventName(ev->type), ev->xany.window);

   switch (ev->type)
     {
     case KeyPress:
     case KeyRelease:
	Eprintf("%s sub=%#lx x,y=%d,%d state=%#x keycode=%#x ss=%d\n", buf,
		ev->xkey.subwindow, ev->xkey.x, ev->xkey.y,
		ev->xkey.state, ev->xkey.keycode, ev->xkey.same_screen);
	break;
     case ButtonPress:
     case ButtonRelease:
	Eprintf("%s sub=%#lx x,y=%d,%d state=%#x button=%#x ss=%d\n", buf,
		ev->xbutton.subwindow, ev->xbutton.x, ev->xbutton.y,
		ev->xbutton.state, ev->xbutton.button, ev->xbutton.same_screen);
	break;
     case MotionNotify:
	Eprintf("%s sub=%#lx x,y=%d,%d rx,ry=%d,%d ss=%d\n", buf,
		ev->xmotion.subwindow, ev->xmotion.x, ev->xmotion.y,
		ev->xmotion.x_root, ev->xmotion.y_root,
		ev->xmotion.same_screen);
	break;
     case EnterNotify:
     case LeaveNotify:
	Eprintf("%s sub=%#lx x,y=%d,%d m=%s d=%s ss=%d focus=%d\n", buf,
		ev->xcrossing.subwindow, ev->xcrossing.x, ev->xcrossing.y,
		EventNotifyModeName(ev->xcrossing.mode),
		EventNotifyDetailName(ev->xcrossing.detail),
		ev->xcrossing.same_screen, ev->xcrossing.focus);
	break;
     case FocusIn:
     case FocusOut:
	Eprintf("%s m=%s d=%s\n", buf, EventNotifyModeName(ev->xfocus.mode),
		EventNotifyDetailName(ev->xfocus.detail));
	break;
     case KeymapNotify:
     case Expose:
     case GraphicsExpose:
	Eprintf("%sx %d+%d %dx%d\n", buf,
		ev->xexpose.x, ev->xexpose.y,
		ev->xexpose.width, ev->xexpose.height);
	break;
     case VisibilityNotify:
	Eprintf("%s state=%d\n", buf, ev->xvisibility.state);
	break;
     case CreateNotify:
     case DestroyNotify:
     case UnmapNotify:
     case MapRequest:
     case EX_EVENT_CREATE_GONE:
     case EX_EVENT_UNMAP_GONE:
     case EX_EVENT_MAPREQUEST_GONE:
	Eprintf("%s win=%#lx\n", buf, ev->xcreatewindow.window);
	break;
     case MapNotify:
     case EX_EVENT_MAP_GONE:
	Eprintf("%s win=%#lx or=%d\n", buf, ev->xmap.window,
		ev->xmap.override_redirect);
	break;
     case ReparentNotify:
     case EX_EVENT_REPARENT_GONE:
	Eprintf("%s win=%#lx parent=%#lx %d+%d\n", buf,
		ev->xreparent.window, ev->xreparent.parent,
		ev->xreparent.x, ev->xreparent.y);
	break;
     case ConfigureNotify:
	Eprintf("%s win=%#lx %d+%d %dx%d bw=%d above=%#lx\n", buf,
		ev->xconfigure.window, ev->xconfigure.x,
		ev->xconfigure.y, ev->xconfigure.width, ev->xconfigure.height,
		ev->xconfigure.border_width, ev->xconfigure.above);
	break;
     case ConfigureRequest:
	Eprintf("%s win=%#lx m=%#lx %d+%d %dx%d bw=%d above=%#lx stk=%d\n",
		buf, ev->xconfigurerequest.window,
		ev->xconfigurerequest.value_mask, ev->xconfigurerequest.x,
		ev->xconfigurerequest.y, ev->xconfigurerequest.width,
		ev->xconfigurerequest.height,
		ev->xconfigurerequest.border_width, ev->xconfigurerequest.above,
		ev->xconfigurerequest.detail);
	break;
     case GravityNotify:
	goto case_common;
     case ResizeRequest:
	Eprintf("%s %dx%d\n", buf,
		ev->xresizerequest.width, ev->xresizerequest.height);
	break;
     case CirculateNotify:
     case CirculateRequest:
	goto case_common;
     case PropertyNotify:
	txt = XGetAtomName(disp, ev->xproperty.atom);
	Eprintf("%s Atom=%s(%ld)\n", buf, txt, ev->xproperty.atom);
	XFree(txt);
	break;
     case SelectionClear:
     case SelectionRequest:
     case SelectionNotify:
     case ColormapNotify:
	goto case_common;
     case ClientMessage:
	txt = XGetAtomName(disp, ev->xclient.message_type);
	Eprintf("%s ev_type=%s(%ld) data: %08lx %08lx %08lx %08lx %08lx\n",
		buf, txt, ev->xclient.message_type,
		ev->xclient.data.l[0], ev->xclient.data.l[1],
		ev->xclient.data.l[2], ev->xclient.data.l[3],
		ev->xclient.data.l[4]);
	XFree(txt);
	break;
     case MappingNotify:
	Eprintf("%s req=%d first=%d count=%d\n",
		buf, ev->xmapping.request,
		ev->xmapping.first_keycode, ev->xmapping.count);
	break;

     case EX_EVENT_SHAPE_NOTIFY:
#define se ((XShapeEvent *)ev)
	Eprintf("%s kind=%d shaped=%d %d,%d %dx%d\n", buf,
		se->kind, se->shaped, se->x, se->y, se->width, se->height);
#undef se
	break;
#if USE_XSCREENSAVER
     case EX_EVENT_SAVER_NOTIFY:
#define se ((XScreenSaverNotifyEvent *)ev)
	Eprintf("%s state=%d kind=%d\n", buf, se->state, se->kind);
#undef se
	break;
#endif
#if USE_XRANDR
     case EX_EVENT_SCREEN_CHANGE_NOTIFY:
	Eprintf("%s\n", buf);
	break;
#endif
#if USE_COMPOSITE
#define de ((XDamageNotifyEvent *)ev)
     case EX_EVENT_DAMAGE_NOTIFY:
	Eprintf("%s level=%d more=%x %d+%d %dx%d\n", buf,
		de->level, de->more,
		de->area.x, de->area.y, de->area.width, de->area.height);
	break;
#undef de
#endif
     default:
      case_common:
	Eprintf("%s\n", buf);
	break;
     }
}
#endif /* ENABLE_DEBUG_EVENTS */

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
#include "aclass.h"
#include "buttons.h"
#include "cursors.h"
#include "desktops.h"
#include "e16-ecore_list.h"
#include "eimage.h"
#include "emodule.h"
#include "file.h"
#include "grabs.h"
#include "iclass.h"
#include "tclass.h"
#include "tooltips.h"
#include "xwin.h"

#define BUTTON_EVENT_MASK \
  (KeyPressMask | KeyReleaseMask | \
   ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | \
   PointerMotionMask)

typedef struct {
   int                 width_min, width_max;
   int                 height_min, height_max;
   int                 xorigin, yorigin;
   int                 xabs, xrel;
   int                 yabs, yrel;
   int                 xsizerel, xsizeabs;
   int                 ysizerel, ysizeabs;
   char                size_from_image;
} BGeometry;

struct _button {
   EObj                o;
   BGeometry           geom;
   ImageClass         *iclass;
   ActionClass        *aclass;
   TextClass          *tclass;
   char               *label;
   int                 id;
   int                 flags;
   char                internal;
   char                default_show;
   EObj               *owner;
   ButtonCbFunc       *func;

   int                 state;
   Window              inside_win;
#if 0				/* Unused */
   Window              event_win;
#endif
   char                left;
   unsigned int        ref_count;
};

static Ecore_List  *button_list = NULL;

static struct {
   Button             *button;
   char                loading_user;
   char                move_pending;
   char                action_inhibit;
   int                 start_x, start_y;
} Mode_buttons;

static void         ButtonHandleEvents(Win win, XEvent * ev, void *btn);

#if 0				/* Unused */
void
ButtonIncRefcount(Button * b)
{
   b->ref_count++;
}

void
ButtonDecRefcount(Button * b)
{
   b->ref_count--;
}
#endif

static int
ButtonIsFixed(const Button * b)
{
   return b->flags & FLAG_FIXED;
}

static int
ButtonIsInternal(const Button * b)
{
   return b->internal;
}

Button             *
ButtonCreate(const char *name, int id, const char *iclass,
	     const char *aclass, const char *tclass, const char *label,
	     int ontop, int flags, int minw, int maxw, int minh, int maxh,
	     int xo, int yo, int xa, int xr, int ya, int yr, int xsr, int xsa,
	     int ysr, int ysa, char simg, int desk, char sticky)
{
   Button             *b;

   if (desk < 0 || desk >= (int)DesksGetNumber())
      return NULL;

   if (sticky && ontop == 1)
      desk = 0;

   b = ECALLOC(Button, 1);
   if (!b)
      return b;

   if (!button_list)
      button_list = ecore_list_new();
   ecore_list_append(button_list, b);

   b->id = id;

   if (label && *label)
      b->label = Estrdup(label);

   b->iclass = ImageclassAlloc(iclass, 1);
   b->aclass = ActionclassAlloc(aclass);
   if (b->label)
      b->tclass = TextclassAlloc(tclass, 1);

   b->flags = flags;
   b->geom.width_min = minw;
   b->geom.width_max = maxw;
   b->geom.height_min = minh;
   b->geom.height_max = maxh;
   b->geom.xorigin = xo;
   b->geom.yorigin = yo;
   b->geom.xabs = xa;
   b->geom.xrel = xr;
   b->geom.yabs = ya;
   b->geom.yrel = yr;
   b->geom.xsizeabs = xsa;
   b->geom.xsizerel = xsr;
   b->geom.ysizeabs = ysa;
   b->geom.ysizerel = ysr;
   b->geom.size_from_image = simg;
   b->default_show = 1;

   EoSetSticky(b, sticky);
   EoSetDesk(b, DeskGet(desk));
   EoInit(b, EOBJ_TYPE_BUTTON, None, -100, -100, 50, 50, 0, name);
   EoSetLayer(b, ontop);
   EoSetFade(b, 1);

   ESelectInput(EoGetWin(b), BUTTON_EVENT_MASK);
   EventCallbackRegister(EoGetWin(b), ButtonHandleEvents, b);

   return b;
}

void
ButtonDestroy(Button * b)
{
   if (!b)
      return;

   if (b->ref_count > 0)
     {
	DialogOK("Button Error!", _("%u references remain"), b->ref_count);
	return;
     }

   ecore_list_node_remove(button_list, b);

   EoFini(b);

   ImageclassFree(b->iclass);
   ActionclassFree(b->aclass);
   TextclassFree(b->tclass);
   Efree(b->label);

   Efree(b);
}

static int
_ButtonMatchName(const void *data, const void *match)
{
   return strcmp(EoGetName((const Button *)data), (const char *)match);
}

Button             *
ButtonFind(const char *name)
{
   return (Button *) ecore_list_find(button_list, _ButtonMatchName, name);
}

static void
ButtonCalc(Button * b)
{
   int                 w, h, x, y, xo, yo;
   EImage             *im;

   w = 32;
   h = 32;
   if (b->geom.size_from_image)
     {
	im = ImageclassGetImage(b->iclass, 0, 0, 0);
	if (im)
	  {
	     EImageGetSize(im, &w, &h);
	     EImageFree(im);
	  }
	else
	  {
	     if (!b->iclass)
		b->iclass = ImageclassAlloc(NULL, 1);
	     w = 32;
	     h = 32;
	  }
     }
   else
     {
	w = ((b->geom.xsizerel * WinGetW(VROOT)) >> 10) + b->geom.xsizeabs;
	h = ((b->geom.ysizerel * WinGetH(VROOT)) >> 10) + b->geom.ysizeabs;
     }
   if (w > b->geom.width_max)
      w = b->geom.width_max;
   else if (w < b->geom.width_min)
      w = b->geom.width_min;
   if (h > b->geom.height_max)
      h = b->geom.height_max;
   else if (h < b->geom.height_min)
      h = b->geom.height_min;
   xo = (w * b->geom.xorigin) >> 10;
   yo = (h * b->geom.yorigin) >> 10;
   x = ((b->geom.xrel * WinGetW(VROOT)) >> 10) + b->geom.xabs - xo;
   y = ((b->geom.yrel * WinGetH(VROOT)) >> 10) + b->geom.yabs - yo;

   EoMoveResize(b, x, y, w, h);
}

static void
ButtonDraw(Button * b)
{
   ITApply(EoGetWin(b), b->iclass, NULL,
	   b->state, 0, 0, ST_BUTTON, b->tclass, NULL, b->label, 0);
   EoShapeUpdate(b, 0);
}

#if 0				/* Unused */
void
ButtonDrawWithState(Button * b, int state)
{
   b->state = state;
   ButtonDraw(b);
}
#endif

void
ButtonShow(Button * b)
{
   ButtonCalc(b);
   ButtonDraw(b);
   EoMap(b, 0);
}

void
ButtonSwallowInto(Button * b, EObj * eo)
{
   b->internal = 1;
   b->default_show = 0;
   b->flags |= FLAG_FIXED;
   b->owner = eo;
   b->ref_count++;
   EobjReparent(EoObj(b), eo, 0, 0);
   ButtonCalc(b);
   ButtonDraw(b);
   EMapWindow(EoGetWin(b));
}

void
ButtonSetCallback(Button * b, ButtonCbFunc * func, EObj * eo)
{
   b->owner = eo;
   b->func = func;
}

static void
ButtonMoveToDesktop(Button * b, Desk * dsk)
{
   if (EoIsSticky(b) && EoGetLayer(b) == 1)
      dsk = DeskGet(0);

   if (!dsk)
      return;

   if (EoGetDesk(b) != dsk)
      EoReparent(b, EoObj(dsk), EoGetX(b), EoGetY(b));
}

void
ButtonHide(Button * b)
{
   EoUnmap(b);
}

static void
ButtonToggle(Button * b)
{
   if (b->internal)
      return;

   if (EoIsShown(b))
      ButtonHide(b);
   else
      ButtonShow(b);
}

void
ButtonMoveToCoord(Button * b, int x, int y)
{
   int                 rx, ry, relx, rely, absx, absy;

   if (ButtonIsFixed(b))
      return;

   if ((x + (EoGetW(b) >> 1)) < (WinGetW(VROOT) / 3))
      relx = 0;
   else if ((x + (EoGetW(b) >> 1)) > ((WinGetW(VROOT) * 2) / 3))
      relx = 1024;
   else
      relx = 512;
   rx = (relx * WinGetW(VROOT)) >> 10;
   absx = x - rx;
   if ((y + (EoGetH(b) >> 1)) < (WinGetH(VROOT) / 3))
      rely = 0;
   else if ((y + (EoGetH(b) >> 1)) > ((WinGetH(VROOT) * 2) / 3))
      rely = 1024;
   else
      rely = 512;
   ry = (rely * WinGetH(VROOT)) >> 10;
   absy = y - ry;
   if (!(b->flags & FLAG_FIXED_HORIZ))
     {
	b->geom.xorigin = 0;
	b->geom.xabs = absx;
	b->geom.xrel = relx;
     }
   if (!(b->flags & FLAG_FIXED_VERT))
     {
	b->geom.yorigin = 0;
	b->geom.yabs = absy;
	b->geom.yrel = rely;
     }

   ButtonCalc(b);
}

void
ButtonMoveRelative(Button * b, int dx, int dy)
{
   ButtonMoveToCoord(b, EoGetX(b) + dx, EoGetY(b) + dy);
}

int
ButtonDoShowDefault(const Button * b)
{
   return !b->internal && b->default_show;
}

#if 0				/* Unused */
int
ButtonEmbedWindow(Button * b, Window WindowToEmbed)
{

   int                 w, h;

   EReparentWindow(WindowToEmbed, EoGetWin(b), 0, 0);
   b->inside_win = WindowToEmbed;
   EGetGeometry(WindowToEmbed, NULL, NULL, NULL, &w, &h, NULL, NULL);
   EMoveWindow(b->inside_win, (EoGetW(b) - w) >> 1, (EoGetH(b) - h) >> 1);
   b->event_win = ECreateEventWindow(EoGetWin(b), 0, 0, w, h);
   EventCallbackRegister(b->event_win, ButtonHandleEvents, b);

   ESelectInput(b->event_win,
		ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
		LeaveWindowMask | ButtonMotionMask);

   EMoveWindow(b->event_win, (EoGetW(b) - w) >> 1, (EoGetH(b) - h) >> 1);
   EMapRaised(b->event_win);

   return 0;
}
#endif

static void
ButtonDragStart(Button * b)
{
   if (ButtonIsFixed(b))
      return;

   GrabPointerSet(EoGetWin(b), ECSR_GRAB, 0);
   Mode.mode = MODE_BUTTONDRAG;
   Mode_buttons.move_pending = 1;
   Mode_buttons.start_x = Mode.events.cx;
   Mode_buttons.start_y = Mode.events.cy;
}

static void
ButtonDragEnd(Button * b)
{
   Desk               *dsk;

   Mode.mode = MODE_NONE;

   if (!Mode_buttons.move_pending)
     {
	dsk = DesktopAt(Mode.events.mx, Mode.events.my);
	ButtonMoveToDesktop(b, dsk);
	dsk = EoGetDesk(b);
	ButtonMoveRelative(b, -EoGetX(dsk), -EoGetY(dsk));
     }
   else
      Mode_buttons.move_pending = 0;

   autosave();
}

void
ButtonsForeach(int id, Desk * dsk, void (*func) (Button * b))
{
   Button             *b;

   ECORE_LIST_FOR_EACH(button_list, b)
   {
      if (id >= 0 && id != b->id)
	 continue;
      if (dsk && dsk != EoGetDesk(b))
	 continue;
      func(b);
   }
}

void
ButtonsMoveStickyToDesk(Desk * dsk)
{
   Button             *b;

   ECORE_LIST_FOR_EACH(button_list, b)
   {
      if (!EoIsSticky(b) || ButtonIsInternal(b))
	 continue;

      ButtonMoveToDesktop(b, dsk);
   }
}

/*
 * Button event handlers
 */

static void
ButtonDoAction(Button * b, XEvent * ev)
{
   if (b->owner && b->func)
      b->func(b->owner, ev, b->aclass);
   else
      ActionclassEvent(b->aclass, ev, NULL);
}

static void
ButtonEventMouseDown(Button * b, XEvent * ev)
{
   Mode_buttons.button = b;

   GrabPointerSet(EoGetWin(b), ECSR_GRAB, 0);

   if (b->inside_win)
     {
	Window              win = ev->xbutton.window;

	ev->xbutton.window = b->inside_win;
	EXSendEvent(b->inside_win, ButtonPressMask, ev);
	ev->xbutton.window = win;
     }

   b->state = STATE_CLICKED;
   ButtonDraw(b);

   if (!ButtonIsInternal(b))
     {
	ActionClass        *ac;

	ac = ActionclassFind("ACTION_BUTTON_DRAG");
	if (ac && !Mode_buttons.action_inhibit)
	   ActionclassEvent(ac, ev, NULL);
     }

   if (b->aclass && !Mode_buttons.action_inhibit)
      ButtonDoAction(b, ev);
}

static void
ButtonEventMouseUp(Button * b, XEvent * ev)
{
   if (b->inside_win && !Mode_buttons.action_inhibit)
     {
	Window              win = ev->xbutton.window;

	ev->xbutton.window = b->inside_win;
	EXSendEvent(b->inside_win, ButtonReleaseMask, ev);
	ev->xbutton.window = win;
     }

   if ((b->state == STATE_CLICKED) && (!b->left))
      b->state = STATE_HILITED;
   else
      b->state = STATE_NORMAL;
   ButtonDraw(b);

   GrabPointerRelease();

   b->left = 0;

   if (Mode.mode == MODE_BUTTONDRAG)
      ButtonDragEnd(Mode_buttons.button);
   Mode_buttons.button = NULL;

   if (b->aclass && !b->left && !Mode_buttons.action_inhibit)
      ButtonDoAction(b, ev);
   Mode_buttons.action_inhibit = 0;
}

static void
ButtonEventMotion(Button * b, XEvent * ev __UNUSED__)
{
   int                 dx, dy;

   if (Mode.mode != MODE_BUTTONDRAG)
      return;

   dx = Mode.events.mx - Mode.events.px;
   dy = Mode.events.my - Mode.events.py;

   if (Mode_buttons.move_pending)
     {
	int                 x, y;

	x = Mode.events.mx - Mode_buttons.start_x;
	y = Mode.events.my - Mode_buttons.start_y;
	if (x < 0)
	   x = -x;
	if (y < 0)
	   y = -y;
	if ((x > Conf.buttons.move_resistance) ||
	    (y > Conf.buttons.move_resistance))
	   Mode_buttons.move_pending = 0;
	Mode_buttons.action_inhibit = 1;
     }
   if (!Mode_buttons.move_pending)
      ButtonMoveRelative(b, dx, dy);
}

static void
ButtonEventMouseIn(Button * b, XEvent * ev)
{
   if (b->state == STATE_CLICKED)
      b->left = 0;
   else
     {
	b->state = STATE_HILITED;
	ButtonDraw(b);
	if (b->aclass && !Mode_buttons.action_inhibit)
	   ActionclassEvent(b->aclass, ev, NULL);
     }
}

static void
ButtonEventMouseOut(Button * b, XEvent * ev)
{
   if (b->state == STATE_CLICKED)
      b->left = 1;
   else
     {
	b->state = STATE_NORMAL;
	ButtonDraw(b);
	if (b->aclass && !Mode_buttons.action_inhibit)
	   ActionclassEvent(b->aclass, ev, NULL);
     }
}

static ActionClass *
ButtonGetAclass(void *data)
{
   Button             *b = (Button *) data;

   /* Validate button */
   if (!ecore_list_goto(button_list, b))
      return NULL;

   return b->aclass;
}

static void
ButtonHandleEvents(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Button             *b = (Button *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	ButtonEventMouseDown(b, ev);
	break;
     case ButtonRelease:
	ButtonEventMouseUp(b, ev);
	break;
     case MotionNotify:
	ButtonEventMotion(b, ev);
	if (b->aclass)
	   TooltipsSetPending(0, ButtonGetAclass, b);
	break;
     case EnterNotify:
	ButtonEventMouseIn(b, ev);
	break;
     case LeaveNotify:
	ButtonEventMouseOut(b, ev);
	break;
     }

   if (b->func)
      b->func(b->owner, ev, NULL);
}

/*
 * Configuration load/save
 */
#include "conf.h"

int
ButtonsConfigLoad(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char               *p2;
   int                 i1, i2;
   char                name[64], label[64];
   char                iclass[64], aclass[64], tclass[64];
   Button             *bt = NULL;
   Button             *pbt = NULL;
   int                 ontop = 0;
   int                 flags = 0, minw = 1, maxw = 99999, minh = 1;
   int                 maxh = 99999, xo = 0, yo = 0, xa = 0;
   int                 xr = 0, ya = 0, yr = 0;
   int                 xsr = 0, xsa = 0, ysr = 0, ysa = 0;
   char                simg = 0;
   int                 desk = 0;
   char                sticky = 0;
   char                show = 1;
   char                internal = 0;

   name[0] = label[0] = '\0';
   iclass[0] = aclass[0] = tclass[0] = '\0';

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, &p2, NULL);
	i2 = atoi(s2);
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if (!pbt && !Mode_buttons.loading_user)
	       {
		  bt = ButtonCreate(name, 0, iclass, aclass, tclass, label,
				    ontop, flags, minw, maxw, minh, maxh,
				    xo, yo, xa, xr, ya, yr, xsr, xsa, ysr, ysa,
				    simg, desk, sticky);
		  bt->default_show = show;
		  bt->internal = internal;
	       }
	     else if (pbt)
	       {
		  if (label[0])
		     _EFDUP(pbt->label, label);
		  EoSetLayer(pbt, ontop);
		  EoSetSticky(pbt, sticky);
		  ButtonMoveToDesktop(pbt, DeskGet(desk));
		  if (iclass[0])
		     pbt->iclass = ImageclassFind(iclass, 1);
		  if (aclass[0])
		     pbt->aclass = ActionclassFind(aclass);
		  if (tclass[0])
		     pbt->tclass = TextclassFind(tclass, 1);
		  pbt->flags = flags;
		  pbt->internal = internal;
		  pbt->default_show = show;
		  pbt->geom.width_min = minw;
		  pbt->geom.width_max = maxw;
		  pbt->geom.height_min = minh;
		  pbt->geom.height_max = maxh;
		  pbt->geom.xorigin = xo;
		  pbt->geom.yorigin = yo;
		  pbt->geom.xabs = xa;
		  pbt->geom.xrel = xr;
		  pbt->geom.yabs = ya;
		  pbt->geom.yrel = yr;
		  pbt->geom.xsizerel = xsr;
		  pbt->geom.xsizeabs = xsa;
		  pbt->geom.ysizerel = ysr;
		  pbt->geom.ysizeabs = ysa;
		  pbt->geom.size_from_image = simg;
	       }
	     goto done;
	  case CONFIG_CLASSNAME:
	     STRCPY(name, s2);
	     pbt = ButtonFind(name);
	     break;
	  case BUTTON_LABEL:
	     STRCPY(label, s2);
	     break;
	  case CONFIG_IMAGECLASS:
	     STRCPY(iclass, s2);
	     break;
	  case CONFIG_ACTIONCLASS:
	     STRCPY(aclass, s2);
	     break;
	  case CONFIG_TEXT:
	     STRCPY(tclass, s2);
	     break;
	  case BORDERPART_ONTOP:
	     ontop = i2;
	     break;
	  case BORDERPART_WMIN:
	     minw = i2;
	     break;
	  case BORDERPART_WMAX:
	     maxw = i2;
	     break;
	  case BORDERPART_HMIN:
	     minh = i2;
	     break;
	  case BORDERPART_FLAGS:
	     flags = i2;
	     break;
	  case BORDERPART_HMAX:
	     maxh = i2;
	     break;
	  case BUTTON_XO:
	     xo = i2;
	     break;
	  case BUTTON_YO:
	     yo = i2;
	     break;
	  case BUTTON_XA:
	     xa = i2;
	     break;
	  case BUTTON_XR:
	     xr = i2;
	     break;
	  case BUTTON_YA:
	     ya = i2;
	     break;
	  case BUTTON_YR:
	     yr = i2;
	     break;
	  case BUTTON_XSR:
	     xsr = i2;
	     break;
	  case BUTTON_XSA:
	     xsa = i2;
	     break;
	  case BUTTON_YSR:
	     ysr = i2;
	     break;
	  case BUTTON_YSA:
	     ysa = i2;
	     break;
	  case BUTTON_SIMG:
	     simg = i2;
	     break;
	  case BUTTON_DESK:
	     desk = i2;
	     break;
	  case BUTTON_STICKY:
	     sticky = i2;
	     break;
	  case BUTTON_INTERNAL:
	     internal = i2;
	     break;
	  case BUTTON_SHOW:
	     show = i2;
	     break;
	  default:
	     break;
	  }
     }
   err = -1;

 done:
   return err;
}

static void
ButtonsConfigLoadUser(void)
{
   char                s[4096];

   Esnprintf(s, sizeof(s), "%s.buttons", EGetSavePrefix());

   Mode_buttons.loading_user = 1;
   ConfigFileLoad(s, NULL, ConfigFileRead, 0);
   Mode_buttons.loading_user = 0;
}

static void
ButtonsConfigSave(void)
{
   char                s[FILEPATH_LEN_MAX], st[FILEPATH_LEN_MAX];
   FILE               *fs;
   Button             *b;
   int                 flags;

   if (ecore_list_count(button_list) <= 0)
      return;

   Etmp(st);
   fs = fopen(st, "w");
   if (!fs)
      return;

   ECORE_LIST_FOR_EACH(button_list, b)
   {
      if (b->id != 0 || b->internal)
	 continue;

      fprintf(fs, "4 999\n");
      fprintf(fs, "100 %s\n", EoGetName(b));
#if 0				/* Remove? */
      if (b->iclass)
	 fprintf(fs, "12 %s\n", ImageclassGetName(b->iclass));
      if (b->aclass)
	 fprintf(fs, "11 %s\n", ActionclassGetName(b->aclass));
      if (EoGetLayer(b) >= 0)
	 fprintf(fs, "453 %i\n", EoGetLayer(b));
#endif
      fprintf(fs, "456 %i\n", b->geom.width_min);
      fprintf(fs, "457 %i\n", b->geom.width_max);
      fprintf(fs, "468 %i\n", b->geom.height_min);
      fprintf(fs, "469 %i\n", b->geom.height_max);
      fprintf(fs, "528 %i\n", b->geom.xorigin);
      fprintf(fs, "529 %i\n", b->geom.yorigin);
      fprintf(fs, "530 %i\n", b->geom.xabs);
      fprintf(fs, "531 %i\n", b->geom.xrel);
      fprintf(fs, "532 %i\n", b->geom.yabs);
      fprintf(fs, "533 %i\n", b->geom.yrel);
      fprintf(fs, "534 %i\n", b->geom.xsizerel);
      fprintf(fs, "535 %i\n", b->geom.xsizeabs);
      fprintf(fs, "536 %i\n", b->geom.ysizerel);
      fprintf(fs, "537 %i\n", b->geom.ysizeabs);
      fprintf(fs, "538 %i\n", b->geom.size_from_image);
      fprintf(fs, "539 %i\n", EoGetDeskNum(b));
      fprintf(fs, "540 %i\n", EoIsSticky(b));
      fprintf(fs, "542 %i\n", EoIsShown(b));

      if (b->flags)
	{
	   flags = 0;
	   if (((b->flags & FLAG_FIXED_HORIZ) &&
		(b->flags & FLAG_FIXED_VERT)) || (b->flags & FLAG_FIXED))
	      flags = 2;
	   else if (b->flags & FLAG_FIXED_HORIZ)
	      flags = 3;
	   else if (b->flags & FLAG_FIXED_VERT)
	      flags = 4;
	   else if (b->flags & FLAG_TITLE)
	      flags = 0;
	   else if (b->flags & FLAG_MINIICON)
	      flags = 1;
	   fprintf(fs, "454 %i\n", flags);
	}
      fprintf(fs, "1000\n");
   }

   fclose(fs);

   Esnprintf(s, sizeof(s), "%s.buttons", EGetSavePrefix());
   E_mv(st, s);
}

/*
 * Buttons Module
 */

static void
ButtonsSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	memset(&Mode_buttons, 0, sizeof(Mode_buttons));
	break;

     case ESIGNAL_CONFIGURE:
	ButtonsConfigLoadUser();
	break;

     case ESIGNAL_EXIT:
	if (Mode.wm.save_ok)
	   ButtonsConfigSave();
	break;
     }
}

typedef struct {
   int                 id;
   int                 match;
   const char         *regex;
} button_match_data;

static void
_ButtonHideShow(void *data, void *prm)
{
   Button             *b = (Button *) data;
   button_match_data  *bmd = (button_match_data *) prm;
   int                 match;

   if (bmd->id >= 0 && bmd->id != b->id)
      return;

   if (bmd->regex)
     {
	match = matchregexp(bmd->regex, EoGetName(b));
	if ((match && !bmd->match) || (!match && bmd->match))
	   return;
#if ENABLE_DESKRAY
	if (!strcmp(EoGetName(b), "_DESKTOP_DESKRAY_DRAG_CONTROL"))
	   return;
#endif
     }

   ButtonToggle(b);
}

static void
doHideShowButton(const char *params)
{
   char                s[1024];
   const char         *ss;
   int                 len;
   button_match_data   bmd = { -1, 1, NULL };
   Button             *b;

   if (!params)
     {
	bmd.id = 0;
	ecore_list_for_each(button_list, _ButtonHideShow, &bmd);
	goto done;
     }

   s[0] = '\0';
   len = 0;
   sscanf(params, "%1000s %n", s, &len);
   ss = (len > 0) ? params + len : NULL;

   if (!strcmp(s, "button"))
     {
	sscanf(params, "%*s %1000s", s);
	b = ButtonFind(s);
	if (b)
	   ButtonToggle(b);
     }
   else if (!strcmp(s, "buttons"))
     {
	if (!ss)
	   return;

	bmd.regex = ss;
	ecore_list_for_each(button_list, _ButtonHideShow, &bmd);
     }
   else if (!strcmp(s, "all_buttons_except"))
     {
	if (!ss)
	   return;

	bmd.id = 0;
	bmd.match = 0;
	bmd.regex = ss;
	ecore_list_for_each(button_list, _ButtonHideShow, &bmd);
     }
   else if (!strcmp(s, "all"))
     {
	ecore_list_for_each(button_list, _ButtonHideShow, &bmd);
     }

 done:
   autosave();
}

static void
ButtonsIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;
   Button             *b;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
     }
   else if (!strncmp(cmd, "list", 2))
     {
	IpcPrintf("Win       d  s  l     x     y     w     h name\n");
	ECORE_LIST_FOR_EACH(button_list, b)
	   IpcPrintf("%#lx %2d %2d %2d %5d+%5d %5dx%5d %s\n",
		     EoGetXwin(b), EoGetDeskNum(b), EoIsSticky(b),
		     EoGetLayer(b), EoGetX(b), EoGetY(b), EoGetW(b), EoGetH(b),
		     EoGetName(b));
     }
   else if (!strncmp(cmd, "move", 2))
     {
	if (Mode_buttons.button)
	   ButtonDragStart(Mode_buttons.button);
     }
}

static void
IPC_ButtonShow(const char *params)
{
   doHideShowButton(params);
}

static const IpcItem ButtonsIpcArray[] = {
   {
    ButtonsIpc,
    "button", "btn",
    "Button functions",
    "  button list               List buttons\n"},
   {
    IPC_ButtonShow,
    "button_show", NULL,
    "Show or Hide buttons on desktop",
    "use \"button_show <button/buttons/all_buttons_except/all> "
    "<BUTTON_STRING>\"\nexamples: \"button_show buttons all\" "
    "(removes all buttons and the dragbar)\n\"button_show\" "
    "(removes all buttons)\n \"button_show buttons CONFIG*\" "
    "(removes all buttons with CONFIG in the start)\n"},
};
#define N_IPC_FUNCS (sizeof(ButtonsIpcArray)/sizeof(IpcItem))

#if 0
static const CfgItem ButtonsCfgItems[] = {
   CFG_ITEM_BOOL(Conf.buttons, enable, 1),
};
#define N_CFG_ITEMS (sizeof(ButtonsCfgItems)/sizeof(CfgItem))
#endif

/*
 * Module descriptor
 */
extern const EModule ModButtons;

const EModule       ModButtons = {
   "buttons", "btn",
   ButtonsSighan,
   {N_IPC_FUNCS, ButtonsIpcArray},
   {0, NULL}
};

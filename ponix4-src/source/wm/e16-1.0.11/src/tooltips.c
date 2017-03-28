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
#include "aclass.h"
#include "conf.h"
#include "dialog.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "eobj.h"
#include "iclass.h"
#include "settings.h"
#include "tclass.h"
#include "timers.h"
#include "tooltips.h"
#include "xwin.h"

static Ecore_List  *tt_list = NULL;
static Timer       *tt_timer = NULL;

static struct {
   char                enable;
   char                showroottooltip;
   int                 delay;	/* milliseconds */
} Conf_tooltips;

static struct {
   int                 inhibit;
   char                root_motion_mask_set;
   CB_GetAclass       *ac_func;
   void               *ac_data;
} Mode_tooltips;

struct _tooltip {
   const char         *name;
   ImageClass         *iclass[5];
   TextClass          *tclass;
   int                 dist;
   Win                 iwin;
   EObj               *win[5];
   ImageClass         *tooltippic;
};

#define TTWIN win[4]
#define TTICL iclass[4]

static void
TooltipRealize(ToolTip * tt)
{
   int                 i, wh;
   EObj               *eo;

   for (i = 0; i < 5; i++)
     {
	if (!tt->iclass[i])
	   continue;

	wh = (i + 1) * 8;
	eo = EobjWindowCreate(EOBJ_TYPE_MISC, -50, -100, wh, wh, 1, tt->name);
	eo->fade = eo->shadow = 1;
	EobjChangeOpacity(eo, OpacityFromPercent(Conf.opacity.tooltips));
	tt->win[i] = eo;
     }
   tt->iwin = ECreateWindow(EobjGetWin(tt->TTWIN), 0, 0, 1, 1, 0);
}

static ToolTip     *
TooltipCreate(const char *name, const char *ic0, const char *ic1,
	      const char *ic2, const char *ic3, const char *ic4,
	      const char *tclass, int dist, const char *tooltippic)
{
   ToolTip            *tt;
   ImageClass         *ic;

   if (!ic0 || !tclass)
      return NULL;

   ic = ImageclassAlloc(ic0, 0);
   if (!ic)
      return NULL;

   tt = ECALLOC(ToolTip, 1);
   if (!tt)
      return NULL;

   tt->name = Estrdup(name);
   tt->iclass[0] = ImageclassAlloc(ic1, 0);
   tt->iclass[1] = ImageclassAlloc(ic2, 0);
   tt->iclass[2] = ImageclassAlloc(ic3, 0);
   tt->iclass[3] = ImageclassAlloc(ic4, 0);
   tt->iclass[4] = ic;
   tt->tclass = TextclassAlloc(tclass, 1);
   tt->tooltippic = ImageclassAlloc(tooltippic, 0);

   tt->dist = dist;

   if (!tt_list)
      tt_list = ecore_list_new();
   ecore_list_prepend(tt_list, tt);

   return tt;
}

#if 0				/* Not used */
static void
TooltipDestroy(ToolTip * tt)
{
   if (!tt)
      return;

   if (tt->ref_count > 0)
     {
	DialogOK("ToolTip Error!", _("%u references remain"), tt->ref_count);
     }
}
#endif

int
TooltipConfigLoad(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char                name[64];
   char                iclass[64];
   char                bubble1[64], bubble2[64], bubble3[64], bubble4[64];
   char                tclass[64];
   char                tooltiphelppic[64];
   int                 i1;
   int                 distance = 0;

   name[0] = iclass[0] = tclass[0] = '\0';
   bubble1[0] = bubble2[0] = bubble3[0] = bubble4[0] = '\0';
   tooltiphelppic[0] = '\0';

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, NULL, NULL);
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if (iclass[0] && tclass[0] && name[0])
		TooltipCreate(name, iclass, bubble1, bubble2,
			      bubble3, bubble4, tclass, distance,
			      tooltiphelppic);
	     goto done;

	  case CONFIG_CLASSNAME:
	     if (TooltipFind(s2))
	       {
		  SkipTillEnd(fs);
		  goto done;
	       }
	     STRCPY(name, s2);
	     break;
	  case TOOLTIP_DRAWICLASS:
	  case CONFIG_IMAGECLASS:
	     STRCPY(iclass, s2);
	     break;
	  case TOOLTIP_BUBBLE1:
	     STRCPY(bubble1, s2);
	     break;
	  case TOOLTIP_BUBBLE2:
	     STRCPY(bubble2, s2);
	     break;
	  case TOOLTIP_BUBBLE3:
	     STRCPY(bubble3, s2);
	     break;
	  case TOOLTIP_BUBBLE4:
	     STRCPY(bubble4, s2);
	     break;
	  case CONFIG_TEXT:
	     STRCPY(tclass, s2);
	     break;
	  case TOOLTIP_DISTANCE:
	     distance = atoi(s2);
	     break;
	  case TOOLTIP_HELP_PIC:
	     STRCPY(tooltiphelppic, s2);
	     break;
	  default:
	     ConfigParseError("ToolTip", s);
	     break;
	  }
     }
   err = -1;

 done:
   return err;
}

static ImageClass  *
TooltipCreateIclass(const char *name, const char *file, int *pw, int *ph)
{
   ImageClass         *ic;
   EImage             *im;
   int                 w, h;

   ic = ImageclassFind(name, 0);
   if (!ic)
      ic = ImageclassCreateSimple(name, file);
   im = ImageclassGetImage(ic, 0, 0, 0);

   if (im)
     {
	EImageGetSize(im, &w, &h);
	if (*pw < w)
	   *pw = w;
	if (*ph < h)
	   *ph = h;
     }

   return ic;
}

static void
TooltipIclassPaste(ToolTip * tt, const char *ic_name, int x, int y, int *px)
{
   ImageClass         *ic;
   EImage             *im;
   int                 w, h;

   ic = ImageclassFind(ic_name, 0);
   im = ImageclassGetImage(ic, 0, 0, 0);
   if (!ic || !im)
      return;

   EImageGetSize(im, &w, &h);
   EImageRenderOnDrawable(im, EobjGetWin(tt->TTWIN), None, EIMAGE_BLEND, x, y,
			  w, h);

   *px = x + w;
}

void
TooltipShow(ToolTip * tt, const char *text, ActionClass * ac, int x, int y)
{
   int                 i, w, h, ix, iy, iw, ih, dx, dy, xx, yy;
   int                 ww, hh, adx, ady, dist;
   int                 headline_h = 0, headline_w = 0, icons_width =
      0, labels_width = 0, double_w = 0;
   EImage             *im;
   int                *heights = NULL;
   ImageClass         *ic;
   EImageBorder       *pad;
   int                 cols[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   int                 num, modifiers;
   Action             *aa;
   const char         *tts;
   EObj               *eo;

   if (!tt || Mode.mode != MODE_NONE)
      return;

   if (!tt->TTWIN)
     {
	TooltipRealize(tt);
	if (!tt->TTWIN)
	   return;
     }

   /* if we get an actionclass, look for tooltip action texts */
   h = 0;
   if (ac)
     {
	num = ActionclassGetActionCount(ac);
	heights = EMALLOC(int, num);

	for (i = 0; i < num; i++)
	  {
	     int                 temp_w, temp_h;

	     temp_w = 0;
	     temp_h = 0;

	     aa = ActionclassGetAction(ac, i);
	     if (!aa)
		continue;

	     tts = ActionGetTooltipString(aa);
	     if (!tts)
		continue;
	     tts = _(tts);

	     TextSize(tt->tclass, 0, 0, STATE_NORMAL, tts, &temp_w, &temp_h,
		      17);
	     if (temp_w > labels_width)
		labels_width = temp_w;
	     temp_w = 0;

	     if (ActionGetEvent(aa) == EVENT_DOUBLE_DOWN)
	       {
		  TextSize(tt->tclass, 0, 0, STATE_NORMAL, "2x", &double_w,
			   &temp_h, 17);
		  if (cols[0] < double_w)
		     cols[0] = double_w;
	       }

	     if (ActionGetAnybutton(aa))
	       {
		  TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_ANY",
				      "pix/mouse_any.png", &cols[1], &temp_h);
	       }
	     else
		switch (ActionGetButton(aa))
		  {
		  case 1:
		     TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_1",
					 "pix/mouse_1.png", &cols[1], &temp_h);
		     break;
		  case 2:
		     TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_2",
					 "pix/mouse_2.png", &cols[1], &temp_h);
		     break;
		  case 3:
		     TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_3",
					 "pix/mouse_3.png", &cols[1], &temp_h);
		     break;
		  case 4:
		     TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_4",
					 "pix/mouse_4.png", &cols[1], &temp_h);
		     break;
		  case 5:
		     TooltipCreateIclass("TOOLTIP_MOUSEBUTTON_5",
					 "pix/mouse_5.png", &cols[1], &temp_h);
		     break;
		  case 0:
		  default:
		     break;
		  }

	     modifiers = ActionGetModifiers(aa);
	     if (modifiers)
	       {
		  if (modifiers & ShiftMask)
		     TooltipCreateIclass("TOOLTIP_KEY_SHIFT",
					 "pix/key_shift.png",
					 &cols[2], &temp_h);
		  if (modifiers & LockMask)
		     TooltipCreateIclass("TOOLTIP_KEY_LOCK",
					 "pix/key_lock.png", &cols[3], &temp_h);
		  if (modifiers & ControlMask)
		     TooltipCreateIclass("TOOLTIP_KEY_CTRL",
					 "pix/key_ctrl.png", &cols[4], &temp_h);
		  if (modifiers & Mod1Mask)
		     TooltipCreateIclass("TOOLTIP_KEY_MOD1",
					 "pix/key_mod1.png", &cols[5], &temp_h);
		  if (modifiers & Mod2Mask)
		     TooltipCreateIclass("TOOLTIP_KEY_MOD2",
					 "pix/key_mod2.png", &cols[6], &temp_h);
		  if (modifiers & Mod3Mask)
		     TooltipCreateIclass("TOOLTIP_KEY_MOD3",
					 "pix/key_mod3.png", &cols[7], &temp_h);
		  if (modifiers & Mod4Mask)
		     TooltipCreateIclass("TOOLTIP_KEY_MOD4",
					 "pix/key_mod4.png", &cols[8], &temp_h);
		  if (modifiers & Mod5Mask)
		     TooltipCreateIclass("TOOLTIP_KEY_MOD5",
					 "pix/key_mod5.png", &cols[9], &temp_h);
	       }

	     temp_w = cols[0] + cols[1] + cols[2] + cols[3] + cols[4] +
		cols[5] + cols[6] + cols[7] + cols[8] + cols[9];

	     if (temp_w > icons_width)
		icons_width = temp_w;
	     heights[i] = temp_h;
	     h += temp_h;
	  }
     }

   TextSize(tt->tclass, 0, 0, STATE_NORMAL, text, &headline_w, &headline_h, 17);
   if (headline_w < icons_width + labels_width)
      w = icons_width + labels_width;
   else
      w = headline_w;
   h += headline_h;

   ic = tt->TTICL;
   pad = ImageclassGetPadding(ic);
   iw = 0;
   ih = 0;
   if (tt->tooltippic)
     {
	im = ImageclassGetImage(tt->tooltippic, 0, 0, 0);
	if (im)
	  {
	     EImageGetSize(im, &iw, &ih);
	     EImageFree(im);
	  }
	w += iw;
	if (h < ih)
	   h = ih;
     }
   w += pad->left + pad->right;
   h += pad->top + pad->bottom;

   if ((tt->tooltippic) && (iw > 0) && (ih > 0))
     {
	ix = pad->left;
	iy = (h - ih) / 2;
	EMoveResizeWindow(tt->iwin, ix, iy, iw, ih);
	EMapWindow(tt->iwin);
	ImageclassApply(tt->tooltippic, tt->iwin, 0, 0, STATE_NORMAL, ST_SOLID);
     }
   else
      EUnmapWindow(tt->iwin);

   dx = x - WinGetW(VROOT) / 2;
   dy = y - WinGetH(VROOT) / 2;

   if ((dy == 0) && (dx == 0))
      dy = -1;

   adx = dx;
   if (adx < 0)
      adx = -adx;
   ady = dy;
   if (ady < 0)
      ady = -ady;
   if (adx < ady)
      /*   +-------+   */
      /*   |\#####/|   */
      /*   | \###/ |   */
      /*   |  \#/  |   */
      /*   |  /#\  |   */
      /*   | /###\ |   */
      /*   |/#####\|   */
      /*   +-------+   */
     {
	if (dy == 0)
	  {
	     dy = 1;
	     ady = 1;
	  }
	dist = tt->dist;
	ady = ady / dy;

	if (tt->win[0])
	  {
	     yy = y - ((ady * 10 * dist) / 100);
	     xx = x - (dist * 10 * dx) / (100 * WinGetW(VROOT) / 2);
	     EobjMove(tt->win[0], xx - 4, yy - 4);
	  }

	if (tt->win[1])
	  {
	     yy = y - ((ady * 30 * dist) / 100);
	     xx = x - (dist * 30 * dx) / (100 * WinGetW(VROOT) / 2);
	     EobjMove(tt->win[1], xx - 8, yy - 8);
	  }

	if (tt->win[2])
	  {
	     yy = y - ((ady * 50 * dist) / 100);
	     xx = x - (dist * 50 * dx) / (100 * WinGetW(VROOT) / 2);
	     EobjMove(tt->win[2], xx - 12, yy - 12);
	  }

	if (tt->win[3])
	  {
	     yy = y - ((ady * 80 * dist) / 100);
	     xx = x - (dist * 80 * dx) / (100 * WinGetW(VROOT) / 2);
	     EobjMove(tt->win[3], xx - 16, yy - 16);
	  }

	yy = y - ((ady * 100 * dist) / 100);
	xx = x - (dist * 100 * dx) / (100 * WinGetW(VROOT) / 2);
	if (ady < 0)
	   hh = 0;
	else
	   hh = h;
	ww = (w / 2) + ((dx * w) / (WinGetW(VROOT) / 2));
     }
   else
      /*   +-------+   */
      /*   |\     /|   */
      /*   |#\   /#|   */
      /*   |##\ /##|   */
      /*   |##/ \##|   */
      /*   |#/   \#|   */
      /*   |/     \|   */
      /*   +-------+   */
     {
	if (dx == 0)
	  {
	     dx = 1;
	     adx = 1;
	  }
	dist = tt->dist;
	adx = adx / dx;

	if (tt->win[0])
	  {
	     xx = x - ((adx * 10 * dist) / 100);
	     yy = y - (dist * 10 * dy) / (100 * WinGetH(VROOT) / 2);
	     EobjMove(tt->win[0], xx - 4, yy - 4);
	  }

	if (tt->win[1])
	  {
	     xx = x - ((adx * 30 * dist) / 100);
	     yy = y - (dist * 30 * dy) / (100 * WinGetH(VROOT) / 2);
	     EobjMove(tt->win[1], xx - 8, yy - 8);
	  }

	if (tt->win[2])
	  {
	     xx = x - ((adx * 50 * dist) / 100);
	     yy = y - (dist * 50 * dy) / (100 * WinGetH(VROOT) / 2);
	     EobjMove(tt->win[2], xx - 12, yy - 12);
	  }

	if (tt->win[3])
	  {
	     xx = x - ((adx * 80 * dist) / 100);
	     yy = y - (dist * 80 * dy) / (100 * WinGetH(VROOT) / 2);
	     EobjMove(tt->win[3], xx - 16, yy - 16);
	  }

	xx = x - ((adx * 100 * dist) / 100);
	yy = y - (dist * 100 * dy) / (100 * WinGetH(VROOT) / 2);
	if (adx < 0)
	   ww = 0;
	else
	   ww = w;
	hh = (h / 2) + ((dy * h) / (WinGetH(VROOT) / 2));
     }

   EobjMoveResize(tt->TTWIN, xx - ww, yy - hh, w, h);

   for (i = 0; i < 5; i++)
     {
	eo = tt->win[i];
	if (!eo)
	   continue;
	ImageclassApply(tt->iclass[i], EobjGetWin(eo), 0, 0, STATE_NORMAL,
			ST_TOOLTIP);
	EobjShapeUpdate(eo, 0);
	EobjMap(eo, 0);
     }

   xx = pad->left + iw;

   /* draw the ordinary tooltip text */
   TextDraw(tt->tclass, EobjGetWin(tt->TTWIN), None, 0, 0, STATE_NORMAL, text,
	    xx, pad->top, headline_w, headline_h, 17, 512);

   /* draw the icons and labels, if any */
   if (ac)
     {
	num = ActionclassGetActionCount(ac);
	y = pad->top + headline_h;
	xx = pad->left + double_w;

	for (i = 0; i < num; i++)
	  {
	     x = xx + iw;

	     aa = ActionclassGetAction(ac, i);
	     if (!aa)
		continue;

	     tts = ActionGetTooltipString(aa);
	     if (!tts)
		continue;
	     tts = _(tts);

	     if (ActionGetEvent(aa) == EVENT_DOUBLE_DOWN)
	       {
		  TextDraw(tt->tclass, EobjGetWin(tt->TTWIN), None, 0, 0,
			   STATE_NORMAL, "2x", xx + iw - double_w, y, double_w,
			   heights[i], 17, 0);
	       }

	     if (ActionGetAnybutton(aa))
	       {
		  TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_ANY", x, y, &x);
	       }
	     else
		switch (ActionGetButton(aa))
		  {
		  case 1:
		     TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_1", x, y, &x);
		     break;
		  case 2:
		     TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_2", x, y, &x);
		     break;
		  case 3:
		     TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_3", x, y, &x);
		     break;
		  case 4:
		     TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_4", x, y, &x);
		     break;
		  case 5:
		     TooltipIclassPaste(tt, "TOOLTIP_MOUSEBUTTON_5", x, y, &x);
		     break;
		  default:
		     break;
		  }

	     modifiers = ActionGetModifiers(aa);
	     if (modifiers)
	       {
		  if (modifiers & ShiftMask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_SHIFT", x, y, &x);
		  if (modifiers & LockMask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_LOCK", x, y, &x);
		  if (modifiers & ControlMask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_CTRL", x, y, &x);
		  if (modifiers & Mod1Mask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_MOD1", x, y, &x);
		  if (modifiers & Mod2Mask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_MOD2", x, y, &x);
		  if (modifiers & Mod3Mask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_MOD3", x, y, &x);
		  if (modifiers & Mod4Mask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_MOD4", x, y, &x);
		  if (modifiers & Mod5Mask)
		     TooltipIclassPaste(tt, "TOOLTIP_KEY_MOD5", x, y, &x);
	       }

	     TextDraw(tt->tclass, EobjGetWin(tt->TTWIN), None, 0, 0,
		      STATE_NORMAL, tts, pad->left + icons_width + iw, y,
		      labels_width, heights[i], 17, 0);
	     y += heights[i];

	  }
     }

   Efree(heights);
}

void
TooltipHide(ToolTip * tt)
{
   int                 i;

   if (!tt || !tt->TTWIN || !tt->TTWIN->shown)
      return;

   for (i = 4; i >= 0; i--)
      if (tt->win[i])
	 EobjUnmap(tt->win[i]);
}

static int
_TooltipMatchName(const void *data, const void *match)
{
   return strcmp(((const ToolTip *)data)->name, (const char *)match);
}

ToolTip            *
TooltipFind(const char *name)
{
   return (ToolTip *) ecore_list_find(tt_list, _TooltipMatchName, name);
}

/*
 * Tooltips
 */

void
TooltipsHide(void)
{
   ToolTip            *tt;

   TooltipsSetPending(0, NULL, NULL);

   ECORE_LIST_FOR_EACH(tt_list, tt) TooltipHide(tt);
}

void
TooltipsEnable(int enable)
{
   if (enable)
     {
	if (Mode_tooltips.inhibit > 0)
	   Mode_tooltips.inhibit--;
     }
   else
     {
	Mode_tooltips.inhibit++;
	TooltipsHide();
     }
}

static ToolTip     *ttip = NULL;

static int
ToolTipTimeout(void *data __UNUSED__)
{
   int                 x, y;
   unsigned int        mask;
   ActionClass        *ac;
   const char         *tts;

   if (!ttip)
      ttip = TooltipFind("DEFAULT");
   if (!ttip)
      goto done;

   /* In the case of multiple screens, check to make sure
    * the root window is still where the mouse is... */
   if (!EQueryPointer(NULL, &x, &y, NULL, &mask))
      goto done;

   /* In case this is a virtual root */
   if (x < 0 || y < 0 || x >= WinGetW(VROOT) || y >= WinGetH(VROOT))
      goto done;

   /* dont pop up tooltip is mouse button down */
   if (mask &
       (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask))
      goto done;

   if (!Mode_tooltips.ac_func)
      goto done;
   ac = Mode_tooltips.ac_func(Mode_tooltips.ac_data);
   if (!ac)
      goto done;

   tts = ActionclassGetTooltipString(ac);
   if (!tts)
      goto done;

   TooltipShow(ttip, _(tts), ac, x, y);

 done:
   tt_timer = NULL;
   return 0;
}

/*
 * We want this on
 * ButtonPress, ButtonRelease, MotionNotify, EnterNotify, LeaveNotify
 */
void
TooltipsSetPending(int type, CB_GetAclass * func, void *data)
{
   if (!Mode_tooltips.ac_func && !func)
      return;

   Mode_tooltips.ac_func = func;
   Mode_tooltips.ac_data = data;

   TooltipHide(ttip);

   TIMER_DEL(tt_timer);

   if (Conf_tooltips.showroottooltip)
     {
	if (!Mode_tooltips.root_motion_mask_set)
	  {
	     Mode_tooltips.root_motion_mask_set = 1;
	     ESelectInputChange(VROOT, PointerMotionMask, 0);
	  }
     }
   else
     {
	if (Mode_tooltips.root_motion_mask_set)
	  {
	     Mode_tooltips.root_motion_mask_set = 0;
	     ESelectInputChange(VROOT, 0, PointerMotionMask);
	  }
     }

   if (!func)
      return;
   if (Mode_tooltips.inhibit || !Conf_tooltips.enable)
      return;
   if (type && !Conf_tooltips.showroottooltip)
      return;

   TIMER_ADD(tt_timer, Conf_tooltips.delay, ToolTipTimeout, NULL);
}

/*
 * Tooltips Module
 */

static void
TooltipsSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	memset(&Mode_tooltips, 0, sizeof(Mode_tooltips));
	break;
     case ESIGNAL_AREA_SWITCH_START:
     case ESIGNAL_DESK_SWITCH_START:
     case ESIGNAL_EWIN_CHANGE:
	TooltipsHide();
	break;
     }
}

#if ENABLE_DIALOGS
/*
 * Configuration dialog
 */
static char         tmp_tooltips;
static int          tmp_tooltiptime;
static char         tmp_roottip;

static void
CB_ConfigureTooltips(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf_tooltips.enable = tmp_tooltips;
	Conf_tooltips.delay = tmp_tooltiptime * 10;
	Conf_tooltips.showroottooltip = tmp_roottip;
     }
   autosave();
}

static void
_DlgFillTooltips(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di;

   tmp_tooltips = Conf_tooltips.enable;
   tmp_tooltiptime = Conf_tooltips.delay / 10;
   tmp_roottip = Conf_tooltips.showroottooltip;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Display Tooltips"));
   DialogItemCheckButtonSetPtr(di, &tmp_tooltips);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Display Root Window Tips"));
   DialogItemCheckButtonSetPtr(di, &tmp_roottip);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Tooltip Delay:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 300);
   DialogItemSliderSetUnits(di, 10);
   DialogItemSliderSetJump(di, 25);
   DialogItemSliderSetValPtr(di, &tmp_tooltiptime);
}

const DialogDef     DlgTooltips = {
   "CONFIGURE_TOOLTIPS",
   N_("Tooltips"),
   N_("Tooltip Settings"),
   SOUND_SETTINGS_TOOLTIPS,
   "pix/tips.png",
   N_("Enlightenment Tooltip\n" "Settings Dialog"),
   _DlgFillTooltips,
   DLG_OAC, CB_ConfigureTooltips,
};
#endif /* ENABLE_DIALOGS */

static const CfgItem TooltipsCfgItems[] = {
   CFG_ITEM_BOOL(Conf_tooltips, enable, 1),
   CFG_ITEM_BOOL(Conf_tooltips, showroottooltip, 1),
   CFG_ITEM_INT(Conf_tooltips, delay, 1500),
};
#define N_CFG_ITEMS (sizeof(TooltipsCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModTooltips;

const EModule       ModTooltips = {
   "tooltips", "tt",
   TooltipsSighan,
   {0, NULL},
   {N_CFG_ITEMS, TooltipsCfgItems}
};

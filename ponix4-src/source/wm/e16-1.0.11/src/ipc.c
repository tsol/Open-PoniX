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
#include "borders.h"		/* FIXME - Should not be here */
#include "desktops.h"
#include "emodule.h"
#include "eobj.h"
#include "ewins.h"
#include "ewin-ops.h"
#include "focus.h"
#include "grabs.h"
#include "hints.h"		/* FIXME - Should not be here */
#include "screen.h"
#include "session.h"
#include "settings.h"
#include "snaps.h"
#include "timers.h"
#include "xwin.h"

#define ENABLE_IPC_INSERT_KEYS  0

#define SS(s) ((s) ? (s) : NoText)
static const char   NoText[] = "-NONE-";

static char        *ipc_bufptr = NULL;
static size_t       ipc_bufsiz = 0;
static char         ipc_active = 0;

static void
IpcPrintInit(void)
{
   ipc_bufptr = NULL;
   ipc_bufsiz = 0;
   ipc_active = 1;
}

static void
IpcPrintDone(void)
{
   Efree(ipc_bufptr);
   ipc_bufptr = NULL;
   ipc_bufsiz = 0;
   ipc_active = 0;
}

static const char  *
IpcPrintGetBuffer(void)
{
   if (!ipc_bufptr)
      return NULL;
   ipc_bufptr[ipc_bufsiz] = '\0';
   return ipc_bufptr;
}

void
IpcPrintf(const char *fmt, ...)
{
   char                tmp[FILEPATH_LEN_MAX];
   int                 len;
   va_list             args;

   if (!ipc_active)
      return;

   va_start(args, fmt);
   len = Evsnprintf(tmp, sizeof(tmp), fmt, args);
   va_end(args);

   ipc_bufptr = EREALLOC(char, ipc_bufptr, ipc_bufsiz + len + 1);

   memcpy(ipc_bufptr + ipc_bufsiz, tmp, len);
   ipc_bufsiz += len;
}

static int
SetEwinBoolean(const char *txt, char *item, const char *value, int set)
{
   int                 vold, vnew;

   vnew = vold = *item != 0;	/* Remember old value */

   if (!value || value[0] == '\0')
      vnew = !vold;
   else if (!strcmp(value, "on"))
      vnew = 1;
   else if (!strcmp(value, "off"))
      vnew = 0;
   else if (!strcmp(value, "?"))
      IpcPrintf("%s: %s\n", txt, (vold) ? "on" : "off");
   else
      IpcPrintf("Error: %s\n", value);

   if (vnew != vold)
     {
	if (set)
	   *item = vnew;
	return 1;
     }

   return 0;
}

/* The IPC functions */

static void
IPC_Screen(const char *params)
{
   char                param[1024];
   int                 l;
   const char         *p;

   param[0] = '\0';
   p = params;
   if (p)
     {
	l = 0;
	sscanf(p, "%1000s %n", param, &l);
	p += l;
     }

   if (!p || !strncmp(param, "list", 2))
     {
	ScreenShowInfo(p);
     }
   else if (!strncmp(param, "size", 2))
     {
	IpcPrintf("Screen %d  size %dx%d\n", Dpy.screen,
		  WinGetW(VROOT), WinGetH(VROOT));
     }
   else if (!strcmp(param, "split"))
     {
	unsigned int        nx, ny;

	nx = 2;
	ny = 1;
	sscanf(p, "%u %u\n", &nx, &ny);
	ScreenSplit(nx, ny);
     }
}

static void
IPC_Nop(const char *params __UNUSED__)
{
   IpcPrintf("nop\n");
}

/* Should be elsewhere */
static void
IPC_Border_CB_List(Border * b, void *data __UNUSED__)
{
   IpcPrintf("%s\n", BorderGetName(b));
}

static void
IPC_Border(const char *params)
{
   if (!params)
     {
	IpcPrintf("Please specify...\n");
	return;
     }

   if (!strncmp(params, "list", 2))
     {
	BordersForeach(IPC_Border_CB_List, NULL);
     }
}

static void
IPC_DialogOK(const char *params)
{
   if (params)
      DialogOKstr(_("Message"), params);
   else
      IpcPrintf("Error: No text for dialog specified\n");
}

static int
CfgStrlistIndex(const char *const *list, const char *str)
{
   int                 i;

   for (i = 0; list[i]; i++)
      if (!strcmp(list[i], str))
	 return i;
   return -1;
}

static const char  *const MovResCfgMoveResizeModeNames[] = {
   "opaque", "lined", "box", "shaded", "semi-solid", "translucent", "techop",
   NULL
};

static const char  *const MovResCfgInfoModes[] = {
   "never", "center", "corner", NULL
};

static void
IPC_MoveResize(const char *params)
{
   char                param1[32];
   char                param2[32];
   int                 i;

   if (!params)
     {
	IpcPrintf("Please specify...\n");
	return;
     }

   param1[0] = param2[0] = '\0';
   sscanf(params, "%31s %31s", param1, param2);

   if (!strncmp(param1, "move", 2))
     {
	if (param2[0] == '\n' || param2[0] == '?')
	  {
	     Conf.movres.mode_move =
		MoveResizeModeValidateMove(Conf.movres.mode_move);
	     IpcPrintf("Move mode: %s\n",
		       MovResCfgMoveResizeModeNames[Conf.movres.mode_move]);
	     return;
	  }

	i = CfgStrlistIndex(MovResCfgMoveResizeModeNames, param2);
	if (i >= 0)
	  {
	     Conf.movres.mode_move = i;
	  }
	else
	  {
	     IpcPrintf("Move mode not found: %s\n", param2);
	  }

     }
   else if (!strncmp(param1, "resize", 2))
     {
	if (param2[0] == '\n' || param2[0] == '?')
	  {
	     Conf.movres.mode_resize =
		MoveResizeModeValidateResize(Conf.movres.mode_resize);
	     IpcPrintf("Resize mode: %s\n",
		       MovResCfgMoveResizeModeNames[Conf.movres.mode_resize]);
	     return;
	  }

	i = CfgStrlistIndex(MovResCfgMoveResizeModeNames, param2);
	if (i >= 0)
	  {
	     Conf.movres.mode_resize = i;
	  }
	else
	  {
	     IpcPrintf("Resize mode not found: %s\n", param2);
	  }

     }
   else if (!strncmp(param1, "info", 2))
     {
	if (param2[0] == '\n' || param2[0] == '?')
	  {
	     if (Conf.movres.mode_info < 0 || Conf.movres.mode_info > 2)
		Conf.movres.mode_info = 1;
	     IpcPrintf("Info mode: %s\n",
		       MovResCfgInfoModes[Conf.movres.mode_info]);
	     return;
	  }

	i = CfgStrlistIndex(MovResCfgInfoModes, param2);
	if (i >= 0)
	  {
	     Conf.movres.mode_info = i;
	  }
	else
	  {
	     IpcPrintf("Info mode not found: %s\n", param2);
	  }

     }
}

static void
IPC_WinList(const char *params)
{
   static const char  *const TxtPG[] = { "NW", "NE", "SW", "SE" };
   char                format[8];
   const char         *match;
   EWin              **lst, *e;
   int                 num, i;

   format[0] = '\0';
   match = params;
   if (match)
     {
	num = 0;
	sscanf(params, "%8s %n", format, &num);
	match += num;
     }
   if (!match || !match[0])
      match = "all";

   lst = EwinsFindByExpr(match, &num, NULL);
   if (!lst)
     {
	IpcPrintf("No windows matching %s\n", match);
	return;
     }

   for (i = 0; i < num; i++)
     {
	e = lst[i];
	switch (format[0])
	  {
	  case '\0':
	     IpcPrintf("%#lx : %s\n", EwinGetClientXwin(e),
		       SS(EwinGetIcccmName(e)));
	     break;

	  default:
	     IpcPrintf("%#lx : %s :: %d : %d %d : %d %d %dx%d\n",
		       EwinGetClientXwin(e), SS(EwinGetIcccmName(e)),
		       (EoIsSticky(e)) ? -1 : (int)EoGetDeskNum(e), e->area_x,
		       e->area_y, EoGetX(e), EoGetY(e), EoGetW(e), EoGetH(e));
	     break;

	  case 'a':
	     IpcPrintf("%#10lx : %5d %5d %4dx%4d :: %2d : %d %d : %s\n",
		       EwinGetClientXwin(e), EoGetX(e), EoGetY(e), EoGetW(e),
		       EoGetH(e), (EoIsSticky(e)) ? -1 : (int)EoGetDeskNum(e),
		       e->area_x, e->area_y, SS(EwinGetIcccmName(e)));
	     break;

	  case 'g':
	     IpcPrintf
		("%#10lx : %5d %5d %4dx%4d :: %2d : %s %4d,%4d %2d,%2d : %s\n",
		 EwinGetClientXwin(e), EoGetX(e), EoGetY(e), EoGetW(e),
		 EoGetH(e), (EoIsSticky(e)) ? -1 : (int)EoGetDeskNum(e),
		 TxtPG[e->place.gravity & 3], e->place.gx, e->place.gy,
		 e->place.ax, e->place.ay, SS(EwinGetIcccmName(e)));
	     break;

	  case 'p':
	     IpcPrintf
		("%#10lx : %5d %5d %4dx%4d :: %2d : \"%s\" \"%s\" : \"%s\" : \"%s\"\n",
		 EwinGetClientXwin(e), EoGetX(e), EoGetY(e), EoGetW(e),
		 EoGetH(e), (EoIsSticky(e)) ? -1 : (int)EoGetDeskNum(e),
		 SS(EwinGetIcccmCName(e)), SS(EwinGetIcccmClass(e)),
		 SS(EwinGetIcccmName(e)), SS(e->icccm.wm_role));
	     break;
	  }
     }
   Efree(lst);
}

#if 0				/* TBD */
static int
doMoveConstrained(EWin * ewin, const char *params)
{
   return MoveResizeMoveStart(ewin, 0, params, 1, 0);
}

static int
doMoveNoGroup(EWin * ewin, const char *params)
{
   return MoveResizeMoveStart(ewin, 0, params, 0, 1);
}

static int
doSwapMove(EWin * ewin, const char *params)
{
   Mode.move.swap = 1;
   return MoveResizeMoveStart(ewin, 0, params, 0, 0);
}

static int
doMoveConstrainedNoGroup(EWin * ewin, const char *params)
{
   return MoveResizeMoveStart(ewin, 0, params, 1, 1);
}
#endif

static Timer       *op_timer = NULL;

static int
OpacityTimeout(void *data)
{
   EWin               *ewin = (EWin *) data;

   if (!EwinFindByPtr(ewin))	/* May be gone */
      goto done;

   if (ewin->state.active)
      EoChangeOpacity(ewin, ewin->props.focused_opacity);

 done:
   ewin->state.show_coords = 0;
   op_timer = NULL;
   return 0;
}

static void
IpcWinop(const WinOp * wop, EWin * ewin, const char *prm)
{
   char                param1[128], param2[128];
   unsigned int        val;
   char                on;
   int                 a, b;

   param1[0] = param2[0] = '\0';
   sscanf(prm, "%128s %128s", param1, param2);

   switch (wop->op)
     {
     default:
	/* We should not get here */
	IpcPrintf("Error: unknown operation\n");
	return;

     case EWIN_OP_BORDER:
	if (!param1[0])
	  {
	     IpcPrintf("Error: no border specified\n");
	     goto done;
	  }
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window border: %s\n", BorderGetName(ewin->border));
	     goto done;
	  }
	EwinOpSetBorder(ewin, OPSRC_USER, param1);
	break;

     case EWIN_OP_TITLE:
	if (!prm[0])
	  {
	     IpcPrintf("Error: no title specified\n");
	     goto done;
	  }
	if (!strcmp(prm, "?"))
	  {
	     IpcPrintf("title: %s\n", EwinGetIcccmName(ewin));
	     goto done;
	  }
	HintsSetWindowName(EwinGetClientWin(ewin), prm);
	break;

     case EWIN_OP_CLOSE:
	EwinOpClose(ewin, OPSRC_USER);
	break;

     case EWIN_OP_KILL:
	EwinOpKill(ewin, OPSRC_USER);
	break;

     case EWIN_OP_ICONIFY:
	on = ewin->state.iconified;
	if (SetEwinBoolean("window iconified", &on, param1, 1))
	   EwinOpIconify(ewin, OPSRC_USER, on);
	break;

     case EWIN_OP_ALONE:
	EwinAlone(ewin);
	break;

     case EWIN_OP_SHADE:
	on = ewin->state.shaded;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EwinOpShade(ewin, OPSRC_USER, on);
	break;

     case EWIN_OP_STICK:
	on = EoIsSticky(ewin);
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EwinOpStick(ewin, OPSRC_USER, on);
	break;

     case EWIN_OP_FOCUS:
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("focused: %s\n", (ewin == GetFocusEwin())? "yes" : "no");
	     goto done;
	  }
	EwinOpActivate(ewin, OPSRC_USER, 1);
	break;

     case EWIN_OP_DESK:
	if (!param1[0])
	  {
	     IpcPrintf("Error: no desktop supplied\n");
	     goto done;
	  }
	if (!strncmp(param1, "next", 1))
	  {
	     EwinOpMoveToDesk(ewin, OPSRC_USER, EoGetDesk(ewin), 1);
	  }
	else if (!strncmp(param1, "prev", 1))
	  {
	     EwinOpMoveToDesk(ewin, OPSRC_USER, EoGetDesk(ewin), -1);
	  }
	else if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window desk: %d\n", EoGetDeskNum(ewin));
	  }
	else
	  {
	     EwinOpMoveToDesk(ewin, OPSRC_USER, NULL, atoi(param1));
	  }
	break;

     case EWIN_OP_AREA:
	if (!param1[0])
	  {
	     IpcPrintf("Error: no area supplied\n");
	     goto done;
	  }
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window area: %d %d\n", ewin->area_x, ewin->area_y);
	  }
	else if (!strcmp(param1, "move"))
	  {
	     a = b = 0;
	     sscanf(prm, "%*s %i %i", &a, &b);
	     EwinMoveToArea(ewin, ewin->area_x + a, ewin->area_y + b);
	  }
	else
	  {
	     a = ewin->area_x;
	     b = ewin->area_y;
	     sscanf(param1, "%i", &a);
	     sscanf(param2, "%i", &b);
	     EwinMoveToArea(ewin, a, b);
	  }
	break;

     case EWIN_OP_MOVE:
	if (!param1[0])
	  {
	     IpcPrintf("Error: no coords supplied\n");
	     goto done;
	  }
	if (!strcmp(param1, "ptr"))
	  {
	     MoveResizeMoveStart(ewin, 0, 0, Mode.nogroup);
	  }
	else if (!strcmp(param1, "kbd"))
	  {
	     MoveResizeMoveStart(ewin, 1, 0, Mode.nogroup);
	  }
	else if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window location: %d %d\n", EoGetX(ewin), EoGetY(ewin));
	  }
	else if (!strcmp(param1, "??"))
	  {
	     IpcPrintf("client location: %d %d\n",
		       EoGetX(ewin) + ewin->border->border.left,
		       EoGetY(ewin) + ewin->border->border.top);
	  }
	else
	  {
	     a = EoGetX(ewin);
	     b = EoGetY(ewin);
	     sscanf(param1, "%i", &a);
	     sscanf(param2, "%i", &b);
	     EwinOpMove(ewin, OPSRC_USER, a, b);
	  }
	break;

     case EWIN_OP_SIZE:
	if (!param1[0])
	   goto done;

	if (!strcmp(param1, "ptr"))
	  {
	     MoveResizeResizeStart(ewin, 0, MODE_RESIZE);
	  }
	else if (!strcmp(param1, "ptr-h"))
	  {
	     MoveResizeResizeStart(ewin, 0, MODE_RESIZE_H);
	  }
	else if (!strcmp(param1, "ptr-v"))
	  {
	     MoveResizeResizeStart(ewin, 0, MODE_RESIZE_V);
	  }
	else if (!strcmp(param1, "kbd"))
	  {
	     MoveResizeResizeStart(ewin, 1, MODE_RESIZE);
	  }
	else if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window size: %d %d\n", ewin->client.w, ewin->client.h);
	  }
	else if (!strcmp(param1, "??"))
	  {
	     IpcPrintf("frame size: %d %d\n", EoGetW(ewin), EoGetH(ewin));
	  }
	else
	  {
	     a = ewin->client.w;
	     b = ewin->client.h;
	     sscanf(param1, "%i", &a);
	     sscanf(param2, "%i", &b);
	     EwinOpResize(ewin, OPSRC_USER, a, b);
	  }
	break;

     case EWIN_OP_MOVE_REL:
	if (!param1[0])
	   goto done;
	a = b = 0;
	sscanf(prm, "%i %i", &a, &b);
	a += EoGetX(ewin);
	b += EoGetY(ewin);
	EwinOpMove(ewin, OPSRC_USER, a, b);
	break;

     case EWIN_OP_SIZE_REL:
	if (!param1[0])
	   goto done;
	a = b = 0;
	sscanf(prm, "%i %i", &a, &b);
	a += ewin->client.w;
	b += ewin->client.h;
	EwinOpResize(ewin, OPSRC_USER, a, b);
	break;

     case EWIN_OP_MAX_WIDTH:
	MaxSizeHV(ewin, param1, 1, 0);
	break;

     case EWIN_OP_MAX_HEIGHT:
	MaxSizeHV(ewin, param1, 0, 1);
	break;

     case EWIN_OP_MAX_SIZE:
	MaxSizeHV(ewin, param1, 1, 1);
	break;

     case EWIN_OP_FULLSCREEN:
	on = ewin->state.fullscreen;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EwinOpFullscreen(ewin, OPSRC_USER, on);
	break;

     case EWIN_OP_ZOOM:
	on = ewin->state.zoomed;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   Zoom(ewin, on);
	break;

     case EWIN_OP_LAYER:
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("window layer: %d\n", EoGetLayer(ewin));
	     goto done;
	  }
	val = atoi(param1);
	EwinOpSetLayer(ewin, OPSRC_USER, val);
	break;

     case EWIN_OP_RAISE:
	EwinOpRaise(ewin, OPSRC_USER);
	break;

     case EWIN_OP_LOWER:
	EwinOpLower(ewin, OPSRC_USER);
	break;

     case EWIN_OP_OPACITY:
	a = OpacityToPercent(ewin->ewmh.opacity);
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("opacity: %u\n", a);
	     goto done;
	  }
	b = a;
	sscanf(param1, "%i", &b);
	if ((param1[0] == '+') || (param1[0] == '-'))
	   b += a;
	a = (b < 0) ? 1 : (b > 100) ? 100 : b;
	EwinOpSetOpacity(ewin, OPSRC_USER, a);
	if (a && ewin->state.active)
	  {
	     EoChangeOpacity(ewin, OpacityFromPercent(a));
	     TIMER_DEL(op_timer);
	     if (ewin->props.focused_opacity)
		TIMER_ADD(op_timer, 700, OpacityTimeout, ewin);
	  }
	if (ewin->state.in_action)
	   CoordsShowOpacity(ewin);
	break;

     case EWIN_OP_FOCUSED_OPACITY:
	a = OpacityToPercent(ewin->props.focused_opacity);
	if (!strcmp(param1, "?"))
	  {
	     IpcPrintf("focused_opacity: %u\n", a);
	     goto done;
	  }
	b = a;
	sscanf(param1, "%i", &b);
	if ((param1[0] == '+') || (param1[0] == '-'))
	   b += a;
	a = (b < 0) ? 0 : (b > 100) ? 100 : b;
	EwinOpSetFocusedOpacity(ewin, OPSRC_USER, a);
	if (ewin->state.in_action)
	   CoordsShowOpacity(ewin);
	break;

     case EWIN_OP_SNAP:
	SnapshotEwinParse(ewin, prm);
	break;

     case EWIN_OP_SKIP_LISTS:
	on = ewin->props.skip_ext_task;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EwinOpSkipLists(ewin, OPSRC_USER, on);
	break;

     case EWIN_OP_IGNORE_ARRANGE:
	on = ewin->props.ignorearrange;
	SetEwinBoolean(wop->name, &on, param1, 1);
	ewin->props.ignorearrange = on;
	goto ewin_update_snap_flags;

     case EWIN_OP_NEVER_USE_AREA:
	on = ewin->props.never_use_area;
	SetEwinBoolean(wop->name, &on, param1, 1);
	ewin->props.never_use_area = on;
	goto ewin_update_snap_flags;

     case EWIN_OP_FOCUS_CLICK:
	on = ewin->props.focusclick;
	SetEwinBoolean(wop->name, &on, param1, 1);
	ewin->props.focusclick = on;
	goto ewin_update_snap_flags;

     case EWIN_OP_AUTOSHADE:
	on = ewin->props.autoshade;
	SetEwinBoolean(wop->name, &on, param1, 1);
	ewin->props.autoshade = on;
	goto ewin_update_snap_flags;

     case EWIN_OP_NO_BUTTON_GRABS:
	on = ewin->props.no_button_grabs;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	  {
	     ewin->props.no_button_grabs = on;
	     if (ewin->props.no_button_grabs)
		UnGrabButtonGrabs(EoGetWin(ewin));
	     else
		GrabButtonGrabs(EoGetWin(ewin));
	  }
	goto ewin_update_snap_flags;

     case EWIN_OP_INH_APP_FOCUS:
	on = EwinInhGetApp(ewin, focus);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetApp(ewin, focus, on);
	goto ewin_update_snap_flags;

     case EWIN_OP_INH_APP_MOVE:
	on = EwinInhGetApp(ewin, move);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetApp(ewin, move, on);
	goto ewin_update_snap_flags;

     case EWIN_OP_INH_APP_SIZE:
	on = EwinInhGetApp(ewin, size);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetApp(ewin, size, on);
	goto ewin_update_snap_flags;

     case EWIN_OP_INH_USER_CLOSE:
	on = EwinInhGetUser(ewin, close);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetUser(ewin, close, on);
	goto ewin_update_state_hints;

     case EWIN_OP_INH_USER_MOVE:
	on = EwinInhGetUser(ewin, move);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetUser(ewin, move, on);
	goto ewin_update_state_hints;

     case EWIN_OP_INH_USER_SIZE:
	on = EwinInhGetUser(ewin, size);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetUser(ewin, size, on);
	goto ewin_update_state_hints;

     case EWIN_OP_INH_WM_FOCUS:
	on = EwinInhGetWM(ewin, focus);
	SetEwinBoolean(wop->name, &on, param1, 1);
	EwinInhSetWM(ewin, focus, on);
	goto ewin_update_state;

#if USE_COMPOSITE
     case EWIN_OP_FADE:
	on = EoGetFade(ewin);
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EoSetFade(ewin, on);
	break;

     case EWIN_OP_SHADOW:
	on = EoGetShadow(ewin);
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EoChangeShadow(ewin, on);
	break;

     case EWIN_OP_NO_REDIRECT:
	on = EoGetNoRedirect(ewin);
	on = ewin->o.noredir;
	if (SetEwinBoolean(wop->name, &on, param1, 1))
	   EoSetNoRedirect(ewin, on);
	break;
#endif
      ewin_update_snap_flags:
	SnapshotEwinUpdate(ewin, SNAP_USE_FLAGS);
	break;

      ewin_update_state:
	EwinStateUpdate(ewin);
	break;

      ewin_update_state_hints:
	EwinStateUpdate(ewin);
	HintsSetWindowState(ewin);
	break;
     }

 done:
   return;
}

static void
IPC_WinOps(const char *params)
{
   char                match[128];
   char                operation[128];
   const char         *p;
   EWin              **lst;
   int                 i, num, flags;
   const WinOp        *wop;

   if (!params)
     {
	IpcPrintf("Error: no window specified\n");
	return;
     }

   match[0] = operation[0] = '\0';
   num = 0;
   sscanf(params, "%128s %128s %n", match, operation, &num);
   p = params + num;

   if (!operation[0])
     {
	IpcPrintf("Error: no operation specified\n");
	return;
     }

   wop = EwinOpFind(operation);
   if (!wop)
     {
	IpcPrintf("Error: unknown operation\n");
	return;
     }

   lst = EwinsFindByExpr(match, &num, &flags);
   if (!lst)
     {
	IpcPrintf("No windows matching %s\n", match);
	return;
     }

   if (flags)
      Mode.nogroup = 1;

   for (i = 0; i < num; i++)
      IpcWinop(wop, lst[i], p);

   Mode.nogroup = 0;

   Efree(lst);
}

static void
IPC_Remember(const char *params)
{
   int                 window, l;
   EWin               *ewin;

   if (!params)
     {
	IpcPrintf("Error: no parameters\n");
	goto done;
     }

   l = 0;
   window = 0;
   sscanf(params, "%x %n", &window, &l);
   if (l <= 0)
      return;

   ewin = EwinFindByClient(window);
   if (!ewin)
     {
	IpcPrintf("Error: Window not found: %#x\n", window);
	goto done;
     }

   SnapshotEwinParse(ewin, params + l);

 done:
   return;
}

static void
IPC_ForceSave(const char *params __UNUSED__)
{
   autosave();
}

static void
IPC_Exec(const char *params)
{
   if (params)
      EspawnApplication(params, EXEC_SET_LANG | EXEC_SET_STARTUP_ID);
   else
      IpcPrintf("exec what?\n");
}

static void
IPC_Restart(const char *params __UNUSED__)
{
   SessionExit(EEXIT_RESTART, NULL);
}

static void
IPC_Exit(const char *params)
{
   char                param1[1024];
   const char         *p2;
   int                 l;

   param1[0] = 0;
   l = 0;
   if (params)
      sscanf(params, "%1000s %n", param1, &l);
   p2 = (l > 0) ? params + l : NULL;

   if (!param1[0])
      SessionExit(EEXIT_EXIT, NULL);
   else if (!strcmp(param1, "logout"))
      SessionExit(EEXIT_LOGOUT, NULL);
   else if (!strcmp(param1, "restart"))
      SessionExit(EEXIT_RESTART, NULL);
   else if (!strcmp(param1, "theme"))
      SessionExit(EEXIT_THEME, p2);
   else if (!strcmp(param1, "exec"))
      SessionExit(EEXIT_EXEC, p2);
}

#if ENABLE_DIALOGS
static void
IPC_About(const char *params __UNUSED__)
{
   About();
}
#endif

static void
IPC_Version(const char *params __UNUSED__)
{
   IpcPrintf("%s %s\n", e_wm_name, e_wm_version);
}

static void
IPC_Debug(const char *params)
{
   char                param[1024];
   int                 l;
   const char         *p;

   if (!params)
      return;

   p = params;
   l = 0;
   sscanf(p, "%1000s %n", param, &l);
   p += l;

   if (!strncmp(param, "event", 2))
     {
	EDebugInit(p);
     }
   else if (!strncmp(param, "grab", 2))
     {
	l = 0;
	sscanf(p, "%1000s %n", param, &l);
	p += l;

	if (!strcmp(param, "?"))
	  {
	     IpcPrintf("Pointer grab on=%d win=%#lx\n",
		       Mode.grabs.pointer_grab_active,
		       Mode.grabs.pointer_grab_window);
	  }
	else if (!strncmp(param, "allow", 2))
	  {
	     l = 0;
	     sscanf(p, "%d", &l);
	     XAllowEvents(disp, l, CurrentTime);
	     IpcPrintf("XAllowEvents\n");
	  }
	else if (!strncmp(param, "unset", 2))
	  {
	     GrabPointerRelease();
	     IpcPrintf("Ungrab\n");
	  }
     }
   else if (!strncmp(param, "sync", 2))
     {
	l = 0;
	sscanf(p, "%1000s %n", param, &l);
	if (!strncmp(param, "on", 2))
	  {
	     XSynchronize(disp, True);
	     IpcPrintf("Sync on\n");
	  }
	else if (!strncmp(param, "off", 2))
	  {
	     XSynchronize(disp, False);
	     IpcPrintf("Sync off\n");
	  }
     }
}

static void
IPC_Set(const char *params)
{
   ConfigurationSet(params);
}

static void
IPC_Show(const char *params)
{
   ConfigurationShow(params);
}

static void
EwinShowInfo(const EWin * ewin)
{
   int                 bl, br, bt, bb;

   EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);

   IpcPrintf("WM_NAME                 %s\n"
	     "_NET_WM_NAME            %s\n"
	     "WM_ICON_NAME            %s\n"
	     "WM_CLASS name.class     %s.%s\n"
	     "WM_WINDOW_ROLE          %s\n"
	     "WM_COMMAND              %s\n"
	     "WM_CLIENT_MACHINE       %s\n"
	     "Client window           %#10lx   x,y %4i,%4i   wxh %4ix%4i\n"
	     "Container window        %#10lx\n"
	     "Frame window            %#10lx   x,y %4i,%4i   wxh %4ix%4i\n"
#if USE_COMPOSITE
	     "Named pixmap            %#10lx\n"
#endif
	     "Border                  %s   lrtb %i,%i,%i,%i\n"
	     "Icon window, pixmap, mask %#10lx, %#10lx, %#10lx\n"
	     "Is group leader  %i  Window group leader %#lx   Client leader %#10lx\n"
	     "Has transients   %i  Transient type  %i  Transient for %#10lx\n"
	     "No resize H/V    %i/%i       Shaped      %i\n"
	     "Base, min, max, inc w/h %ix%i, %ix%i, %ix%i %ix%i\n"
	     "Aspect min, max         %5.5f, %5.5f\n"
	     "Struts                  lrtb %i,%i,%i,%i\n"
	     "MWM border %i resizeh %i title %i menu %i minimize %i maximize %i\n"
	     "NeedsInput   %i   TakeFocus    %i   FocusNever   %i   FocusClick   %i\n"
	     "NeverUseArea %i   FixedPos     %i   FixedSize    %i\n"
	     "Desktop      %i   Layer        %i(%i)\n"
	     "Iconified    %i   Sticky       %i   Shaded       %i   Docked       %i\n"
	     "State        %i   Shown        %i   Visibility   %i   Active       %i\n"
	     "Member of groups        %i\n"
#if USE_COMPOSITE
	     "Opacity    %3i(%x)  Focused Opacity     %3i\n"
	     "Shadow       %i   Fade         %i   NoRedirect   %i\n"
#else
	     "Opacity    %3i\n"
#endif
	     ,
	     SS(EwinGetIcccmName(ewin)),
	     SS(ewin->ewmh.wm_name),
	     SS(ewin->icccm.wm_icon_name),
	     SS(EwinGetIcccmCName(ewin)), SS(EwinGetIcccmClass(ewin)),
	     SS(ewin->icccm.wm_role),
	     SS(ewin->icccm.wm_command),
	     SS(ewin->icccm.wm_machine),
	     EwinGetClientXwin(ewin),
	     ewin->client.x, ewin->client.y, ewin->client.w, ewin->client.h,
	     EwinGetContainerXwin(ewin),
	     EoGetXwin(ewin),
	     EoGetX(ewin), EoGetY(ewin), EoGetW(ewin), EoGetH(ewin),
#if USE_COMPOSITE
	     EoGetPixmap(ewin),
#endif
	     EwinBorderGetName(ewin), bl, br, bt, bb,
	     ewin->icccm.icon_win,
	     ewin->icccm.icon_pmap, ewin->icccm.icon_mask,
	     EwinIsWindowGroupLeader(ewin), EwinGetWindowGroup(ewin),
	     ewin->icccm.client_leader, EwinGetTransientCount(ewin),
	     EwinIsTransient(ewin), EwinGetTransientFor(ewin),
	     ewin->props.no_resize_h, ewin->props.no_resize_v,
	     ewin->state.shaped, ewin->icccm.base_w, ewin->icccm.base_h,
	     ewin->icccm.width_min, ewin->icccm.height_min,
	     ewin->icccm.width_max, ewin->icccm.height_max,
	     ewin->icccm.w_inc, ewin->icccm.h_inc,
	     ewin->icccm.aspect_min, ewin->icccm.aspect_max,
	     ewin->strut.left, ewin->strut.right,
	     ewin->strut.top, ewin->strut.bottom,
	     ewin->mwm.decor_border, ewin->mwm.decor_resizeh,
	     ewin->mwm.decor_title, ewin->mwm.decor_menu,
	     ewin->mwm.decor_minimize, ewin->mwm.decor_maximize,
	     ewin->icccm.need_input, ewin->icccm.take_focus,
	     EwinInhGetWM(ewin, focus), ewin->props.focusclick,
	     ewin->props.never_use_area, EwinInhGetUser(ewin, move),
	     EwinInhGetUser(ewin, size), EoGetDeskNum(ewin),
	     EoGetLayer(ewin), ewin->o.ilayer,
	     ewin->state.iconified, EoIsSticky(ewin), ewin->state.shaded,
	     ewin->state.docked, ewin->state.state, EoIsShown(ewin),
	     ewin->state.visibility, ewin->state.active, ewin->num_groups,
	     OpacityToPercent(ewin->ewmh.opacity)
#if USE_COMPOSITE
	     , EoGetOpacity(ewin),
	     OpacityToPercent(ewin->props.focused_opacity), EoGetShadow(ewin),
	     EoGetFade(ewin), EoGetNoRedirect(ewin)
#endif
      );
}

static void
IPC_EwinInfo(const char *params)
{
   char                match[FILEPATH_LEN_MAX];
   EWin              **lst;
   int                 i, num;

   if (!params)
     {
	IpcPrintf("Error: no window specified\n");
	return;
     }

   sscanf(params, "%1000s", match);

   lst = EwinsFindByExpr(match, &num, NULL);
   if (!lst)
     {
	IpcPrintf("No windows matching %s\n", match);
	return;
     }

   for (i = 0; i < num; i++)
     {
	EwinShowInfo(lst[i]);
	if (i != num - 1)
	   IpcPrintf("\n");
     }

   Efree(lst);
}

static void
IPC_ObjInfo(const char *params __UNUSED__)
{
   int                 i, num;
   EObj               *const *lst, *eo;

   lst = EobjListStackGet(&num);

   IpcPrintf
      ("Num    Window De T V Shape  Dsk S  F   L     Pos       Size    C R Name\n");
   for (i = 0; i < num; i++)
     {
	eo = lst[i];
	IpcPrintf
	   (" %2d %#9lx %2d %d %d %2d/%2d  %3d %d  %d %3d %5d,%5d %4dx%4d %d %d %s\n",
	    i, EobjGetXwin(eo), WinGetDepth(EobjGetWin(eo)), eo->type,
	    eo->shown, eo->shaped, EShapeCheck(EobjGetWin(eo)), eo->desk->num,
	    eo->sticky, eo->floating, eo->ilayer,
	    EobjGetX(eo), EobjGetY(eo), EobjGetW(eo), EobjGetH(eo),
#if USE_COMPOSITE
	    (eo->cmhook) ? 1 : 0, !eo->noredir
#else
	    0, 0
#endif
	    , EobjGetName(eo));
     }
}

static void
IPC_Reparent(const char *params)
{
   char                param1[FILEPATH_LEN_MAX];
   char                param2[FILEPATH_LEN_MAX];
   EWin               *ewin, *enew;

   if (!params)
      return;

   sscanf(params, "%100s %100s", param1, param2);

   ewin = EwinFindByExpr(param1);
   enew = EwinFindByExpr(param2);
   if (!ewin || !enew)
      IpcPrintf("No matching client or target EWin found\n");
   else
      EwinReparent(ewin, EwinGetClientWin(enew));
}

static void
IPC_Warp(const char *params)
{
   int                 x, y;

   if (!params)
      return;

   x = y = 0;
   if (!strcmp(params, "?"))
     {
	EQueryPointer(NULL, &x, &y, NULL, NULL);
	IpcPrintf("Pointer location: %d %d\n", x, y);
     }
   else if (!strncmp(params, "abs", 3))
     {
	sscanf(params, "%*s %i %i", &x, &y);
	EWarpPointer(VROOT, x, y);
     }
   else if (!strncmp(params, "rel", 3))
     {
	sscanf(params, "%*s %i %i", &x, &y);
	EWarpPointer(NULL, x, y);
     }
   else if (!strncmp(params, "scr", 3))
     {
	x = (Dpy.screen + 1) % ScreenCount(disp);
	sscanf(params, "%*s %i", &x);
	FocusScreen(x);
     }
   else
     {
	sscanf(params, "%i %i", &x, &y);
	EWarpPointer(NULL, x, y);
     }
}

#if ENABLE_IPC_INSERT_KEYS
struct _keyset {
   const char         *sym;
   int                 state;
   const char         *ch;
};

static const struct _keyset ks[] = {
   {"a", 0, "a"},
   {"b", 0, "b"},
   {"c", 0, "c"},
   {"d", 0, "d"},
   {"e", 0, "e"},
   {"f", 0, "f"},
   {"g", 0, "g"},
   {"h", 0, "h"},
   {"i", 0, "i"},
   {"j", 0, "j"},
   {"k", 0, "k"},
   {"l", 0, "l"},
   {"m", 0, "m"},
   {"n", 0, "n"},
   {"o", 0, "o"},
   {"p", 0, "p"},
   {"q", 0, "q"},
   {"r", 0, "r"},
   {"s", 0, "s"},
   {"t", 0, "t"},
   {"u", 0, "u"},
   {"v", 0, "v"},
   {"w", 0, "w"},
   {"x", 0, "x"},
   {"y", 0, "y"},
   {"z", 0, "z"},
   {"a", ShiftMask, "A"},
   {"b", ShiftMask, "B"},
   {"c", ShiftMask, "C"},
   {"d", ShiftMask, "D"},
   {"e", ShiftMask, "E"},
   {"f", ShiftMask, "F"},
   {"g", ShiftMask, "G"},
   {"h", ShiftMask, "H"},
   {"i", ShiftMask, "I"},
   {"j", ShiftMask, "J"},
   {"k", ShiftMask, "K"},
   {"l", ShiftMask, "L"},
   {"m", ShiftMask, "M"},
   {"n", ShiftMask, "N"},
   {"o", ShiftMask, "O"},
   {"p", ShiftMask, "P"},
   {"q", ShiftMask, "Q"},
   {"r", ShiftMask, "R"},
   {"s", ShiftMask, "S"},
   {"t", ShiftMask, "T"},
   {"u", ShiftMask, "U"},
   {"v", ShiftMask, "V"},
   {"w", ShiftMask, "W"},
   {"x", ShiftMask, "X"},
   {"y", ShiftMask, "Y"},
   {"z", ShiftMask, "Z"},
   {"grave", 0, "`"},
   {"1", 0, "1"},
   {"2", 0, "2"},
   {"3", 0, "3"},
   {"4", 0, "4"},
   {"5", 0, "5"},
   {"6", 0, "6"},
   {"7", 0, "7"},
   {"8", 0, "8"},
   {"9", 0, "9"},
   {"0", 0, "0"},
   {"minus", 0, "-"},
   {"equal", 0, "="},
   {"bracketleft", 0, "["},
   {"bracketright", 0, "]"},
   {"backslash", 0, "\\\\"},
   {"semicolon", 0, "\\s"},
   {"apostrophe", 0, "\\a"},
   {"comma", 0, ","},
   {"period", 0, "."},
   {"slash", 0, "/"},
   {"grave", ShiftMask, "~"},
   {"1", ShiftMask, "!"},
   {"2", ShiftMask, "@"},
   {"3", ShiftMask, "#"},
   {"4", ShiftMask, "$"},
   {"5", ShiftMask, "%"},
   {"6", ShiftMask, "^"},
   {"7", ShiftMask, "&"},
   {"8", ShiftMask, "*"},
   {"9", ShiftMask, "("},
   {"0", ShiftMask, ")"},
   {"minus", ShiftMask, "_"},
   {"equal", ShiftMask, "+"},
   {"bracketleft", ShiftMask, "{"},
   {"bracketright", ShiftMask, "}"},
   {"backslash", ShiftMask, "|"},
   {"semicolon", ShiftMask, ":"},
   {"apostrophe", ShiftMask, "\\q"},
   {"comma", ShiftMask, "<"},
   {"period", ShiftMask, ">"},
   {"slash", ShiftMask, "?"},
   {"space", ShiftMask, " "},
   {"Return", ShiftMask, "\\n"},
   {"Tab", ShiftMask, "\\t"}
};

static void
IPC_InsertKeys(const char *params, Client * c __UNUSED__)
{
   Window              win = 0;
   int                 i, rev;
   const char         *s;
   XKeyEvent           ev;

   if (!params)
      return;

   s = params;
   XGetInputFocus(disp, &win, &rev);
   if (win == None)
      return;

   SoundPlay(SOUND_INSERT_KEYS);
   ev.window = win;
   for (i = 0; i < (int)strlen(s); i++)
     {
	int                 j;

	ev.x = Mode.events.cx;
	ev.y = Mode.events.cy;
	ev.x_root = Mode.events.cx;
	ev.y_root = Mode.events.cy;
	for (j = 0; j < (int)(sizeof(ks) / sizeof(struct _keyset)); j++)
	  {
	     if (strncmp(ks[j].ch, &(s[i]), strlen(ks[j].ch)))
		continue;

	     i += strlen(ks[j].ch) - 1;
	     ev.keycode = EKeynameToKeycode(ks[j].sym);
	     ev.state = ks[j].state;
	     ev.type = KeyPress;
	     EXSendEvent(win, 0, (XEvent *) & ev);
	     ev.type = KeyRelease;
	     EXSendEvent(win, 0, (XEvent *) & ev);
	     break;
	  }
     }
}
#endif /* ENABLE_IPC_INSERT_KEYS */

/*
 * Compatibility stuff - DO NOT USE
 */
static int
IPC_Compat(const char *params)
{
   int                 ok = 0;
   char                param1[128];
   const char         *p;
   int                 len;

   if (!params)
      goto done;

   len = 0;
   param1[0] = '\0';
   sscanf(params, "%127s %n", param1, &len);
   p = params + len;

   ok = 1;
   if (!strcmp(param1, "goto_desktop"))
     {
	if (*p == '?')
	   IpcPrintf("Current Desktop: %d\n", DesksGetCurrentNum());
     }
   else if (!strcmp(param1, "num_desks"))
     {
	if (*p == '?')
	   IpcPrintf("Number of Desks: %d\n", DesksGetNumber());
     }
#if !USE_COMPOSITE
   else if (!strcmp(param1, "cm"))
     {
	DialogOK(_("Message"), _("e16 was built without %s support"),
		 _("composite"));
     }
#endif
#if !HAVE_SOUND
   else if (!strcmp(param1, "sound"))
     {
	DialogOK(_("Message"), _("e16 was built without %s support"),
		 _("sound"));
     }
#endif
   else
     {
	ok = 0;
     }

 done:
   return ok;
}

/* the IPC Array */

/* the format of an IPC member of the IPC array is as follows:
 * {
 *    NameOfMyFunction,
 *    "command_name",
 *    "quick-help explanation",
 *    "extended help data"
 *    "may go on for several lines, be sure\n"
 *    "to add line feeds when you need them and to \"quote\"\n"
 *    "properly"
 * }
 *
 * when you add a function into this array, make sure you also add it into
 * the declarations above and also put the function in this file.  PLEASE
 * if you add a new function in, add help to it also.  since my end goal
 * is going to be to have this whole IPC usable by an end-user or to your
 * scripter, it should be easy to learn to use without having to crack
 * open the source code.
 * --Mandrake
 */
static void         IPC_Help(const char *params);

static const IpcItem IPCArray[] = {
   {
    IPC_Help,
    "help", "?",
    "Gives you this help screen",
    "Additional parameters will retrieve help on many topics - "
    "\"help <command>\"." "\n" "use \"help all\" for a list of commands.\n"},
   {
    IPC_Version,
    "version", "ver",
    "Displays the current version of Enlightenment running",
    NULL},
   {
    IPC_Nop,
    "nop", NULL,
    "IPC No-operation - returns nop",
    NULL},
#if ENABLE_DIALOGS
   {
    IPC_About, "about", NULL, "Show E info", NULL},
   {
    IPC_Cfg, "configure", "cfg", "Configuration dialogs", NULL},
#endif
   {
    IPC_Exec,
    "exec", NULL,
    "Execute program",
    "  exec <command>       Execute command\n"},
   {
    IPC_Restart,
    "restart", NULL,
    "Restart Enlightenment",
    NULL},
   {
    IPC_Exit,
    "exit", "q",
    "Exit Enlightenment",
    "  exit                 Exit immediately\n"
    "  exit logout          Show logout dialog\n"
    "  exit restart         Restart\n"
    "  exit theme <theme>   Restart with new theme\n"
    "  exit exec <program>  Exit and start program\n"},
   {
    IPC_ForceSave,
    "save_config", "s",
    "Force Enlightenment to save settings now",
    NULL},
   {
    IPC_WinOps,
    "win_op", "wop",
    "Change a property of a specific window",
    "Use \"win_op <windowid> <property> <value>\" to change the property of a window\n"
    "You can use the \"window_list\" command to retrieve a list of available windows\n"
    "You can use ? after most of these commands to receive the current\n"
    "status of that flag\n"
    "Available win_op commands are:\n"
    "  win_op <windowid> border <BORDERNAME>\n"
    "  win_op <windowid> title <title>\n"
    "  win_op <windowid> <close/kill>\n"
    "  win_op <windowid> <focus/iconify/alone/shade/stick>\n"
#if USE_COMPOSITE
    "  win_op <windowid> <fade/shadow>\n"
#endif
    "  win_op <windowid> desk <desktochangeto/next/prev>\n"
    "  win_op <windowid> area <x> <y>\n"
    "  win_op <windowid> <move/size> <x> <y> or <kbd/ptr>\n"
    "          (you can use ? and ?? to retreive client and frame locations)\n"
    "  win_op <windowid> <mr/sr> <x> <y>   (incremental move/size)\n"
    "  win_op <windowid> toggle_<width/height/size> <conservative/available/xinerama>\n"
    "  win_op <windowid> <fullscreen/zoom>\n"
    "  win_op <windowid> layer <0-100,4=normal>\n"
    "  win_op <windowid> <raise/lower>\n"
    "  win_op <windowid> opacity [+|-]<1-100(100=opaque)>   (+/-: incremental change)\n"
#if USE_COMPOSITE
    "  win_op <windowid> focused_opacity [+|-]<0-100(0=follow opacity, 100=opaque)>\n"
#endif
    "  win_op <windowid> snap <what>\n"
    "         <what>: all, none, border, command, desktop, dialog, group, icon,\n"
    "                 layer, location, opacity, shade, shadow, size, sticky\n"
    "  win_op <windowid> <focusclick/never_use_area/no_button_grabs/skiplists>\n"
    "  win_op <windowid> <no_app_focus/move/size>\n"
    "  win_op <windowid> <no_user_close/move/size>\n"
    "  win_op <windowid> <no_wm_focus>\n"
    "<windowid> may be substituted with \"current\" to use the current window\n"},
   {
    IPC_WinList,
    "window_list", "wl",
    "Get a list of managed application windows",
    "The window list can be shown in a number of different formats:\n"
    "  window_list       \"windowid : title\"\n"
    "  window_list all   \"windowid : x y w x h :: desk : area_x area_y : title\"\n"
    "  window_list ext   \"windowid : title :: desk : area_x area_y : x y wxh\"\n"
    "  window_list prop  \"windowid : x y w x h :: desk : name class Title\"\n"},
   {
    IPC_MoveResize,
    "movres", "mr",
    "Show/set Window move/resize/geometry info modes",
    "  movres move   <?/opaque/lined/box/shaded/semi-solid/translucent>\n"
    "  movres resize <?/opaque/lined/box/shaded/semi-solid>\n"
    "  movres info   <?/never/center/corner>\n"},
   {
    IPC_DialogOK,
    "dialog_ok", "dok",
    "Pop up a dialog box with an OK button",
    "Use \"dialog_ok <message>\" to pop up a dialog box\n"},
   {
    IPC_Border, "border", NULL, "List available borders", NULL},
   {
    IPC_Screen, "screen", NULL, "Return screen information",
    "  screen list         List screens\n"
    "  screen size         Show current screen size\n"
    "  screen split nx ny  Simulate xinerama by subdividing screen\n"},
   {
    SnapshotsIpcFunc,
    "list_remember", "rl",
    "Retrieve a list of remembered windows and their attributes",
    SnapshotsIpcText},
   {
    IPC_Debug,
    "debug", NULL,
    "Set debug options",
    "  debug events <EvNo>:<EvNo>...\n"},
   {
    IPC_Set, "set", NULL, "Set configuration parameter", NULL},
   {
    IPC_Show, "show", "sh", "Show configuration parameter(s)", NULL},
   {
    IPC_EwinInfo, "win_info", "wi", "Show client window info", NULL},
   {
    IPC_ObjInfo, "obj_info", "oi", "Show window object info", NULL},
   {
    IPC_Reparent,
    "reparent", "rep",
    "Reparent window",
    "  reparent <windowid> <new parent>\n"},
   {
    IPC_Remember,
    "remember", NULL,
    "Remembers parameters for client windows (obsolete)",
    "  remember <windowid> <parameter>...\n"
    "For compatibility with epplets only. In stead use\n"
    "  wop <windowid> snap <parameter>...\n"},
   {
    IPC_Warp,
    "warp", NULL,
    "Warp/query pointer",
    "  warp ?               Get pointer position\n"
    "  warp abs <x> <y>     Set pointer position\n"
    "  warp rel <x> <y>     Move pointer relative to current position\n"
    "  warp scr [<i>]       Move pointer to other screen (default next)\n"
    "  warp <x> <y>         Same as \"warp rel\"\n"},
#if ENABLE_IPC_INSERT_KEYS
   {
    IPC_InsertKeys,
    "keys", NULL,
    "Send key events to focused window",
    "  keys <string>\n"},
#endif
};

static int          ipc_item_count = 0;
static const IpcItem **ipc_item_list = NULL;

static const IpcItem **
IPC_GetList(int *pnum)
{
   int                 i, num;
   const IpcItem     **lst;

   if (ipc_item_list)
     {
	/* Must be re-generated if modules are ever added/removed */
	*pnum = ipc_item_count;
	return ipc_item_list;
     }

   num = sizeof(IPCArray) / sizeof(IpcItem);
   lst = EMALLOC(const IpcItem *, num);

   for (i = 0; i < num; i++)
      lst[i] = &IPCArray[i];

   ModulesGetIpcItems(&lst, &num);

   ipc_item_count = num;
   ipc_item_list = lst;
   *pnum = num;
   return lst;
}

/* The IPC Handler */
/* this is the function that actually loops through the IPC array
 * and finds the command that you were trying to run, and then executes it.
 * you shouldn't have to touch this function
 * - Mandrake
 */
static int
IpcExec(const char *params)
{
   int                 i, num, ok;
   char                cmd[128];
   const char         *prm;
   const IpcItem     **lst, *ipc;

   if (EDebug(EDBUG_TYPE_IPC))
      Eprintf("IpcExec: %s\n", params);

   cmd[0] = 0;
   num = 0;
   if (params)
      sscanf(params, "%100s %n", cmd, &num);
   prm = (num > 0 && params[num]) ? params + num : NULL;

   lst = IPC_GetList(&num);

   ok = 0;
   for (i = 0; i < num; i++)
     {
	ipc = lst[i];
	if (!(ipc->nick && !strcmp(cmd, ipc->nick)) && strcmp(cmd, ipc->name))
	   continue;

	ipc->func(prm);

	ok = 1;
	break;
     }

   if (!ok && params)
      ok = IPC_Compat(params);

   return ok;
}

int
IpcExecReply(const char *params, IpcReplyFunc * reply, void *data)
{
   int                 ok;

   IpcPrintInit();
   ok = IpcExec(params);
   reply(data, IpcPrintGetBuffer());
   IpcPrintDone();

   return ok;
}

int
EFunc(EWin * ewin, const char *params)
{
   int                 ok;

   SetContextEwin(ewin);
   ok = IpcExec(params);
   SetContextEwin(NULL);

   return ok;
}

static int
doEFuncDeferred(void *data)
{
   void              **prm = (void **)data;
   EWin               *ewin;

   ewin = (EWin *) prm[0];
   if (ewin && !EwinFindByPtr(ewin))
      goto done;

   EFunc(ewin, (const char *)prm[1]);

 done:
   Efree(prm[1]);
   Efree(data);

   return 0;
}

void
EFuncDefer(EWin * ewin, const char *cmd)
{
   void              **prm;

   prm = EMALLOC(void *, 2);

   if (!prm)
      return;
   prm[0] = ewin;
   prm[1] = Estrdup(cmd);

   TIMER_ADD_NP(0, doEFuncDeferred, prm);
}

static int
ipccmp(const void *p1, const void *p2)
{
   return strcmp(((const IpcItem *)p1)->name, ((const IpcItem *)p2)->name);
}

static void
IPC_Help(const char *params)
{
   int                 i, num;
   const IpcItem     **lst, *ipc;
   const char         *nick;

   lst = IPC_GetList(&num);

   IpcPrintf(_("Enlightenment IPC Commands Help\n"));

   if (!params)
     {
	IpcPrintf(_("Use \"help all\" for descriptions of each command\n"
		    "Use \"help <command>\" for an individual description\n\n"));
	IpcPrintf(_("Commands currently available:\n"));

	Quicksort((void **)lst, 0, num - 1, ipccmp);

	for (i = 0; i < num; i++)
	  {
	     ipc = lst[i];
	     nick = (ipc->nick) ? ipc->nick : "";
	     IpcPrintf("  %-16s %-4s ", ipc->name, nick);
	     if ((i % 3) == 2)
		IpcPrintf("\n");
	  }
	if (i % 3)
	   IpcPrintf("\n");
     }
   else if (!strcmp(params, "all"))
     {
	IpcPrintf(_
		  ("Use \"help full\" for full descriptions of each command\n"));
	IpcPrintf(_("Use \"help <command>\" for an individual description\n"));
	IpcPrintf(_("Commands currently available:\n"));
	IpcPrintf(_("         <command>     : <description>\n"));

	for (i = 0; i < num; i++)
	  {
	     ipc = lst[i];
	     nick = (ipc->nick) ? ipc->nick : "";
	     IpcPrintf("%18s %4s: %s\n", ipc->name, nick, ipc->help_text);
	  }
     }
   else if (!strcmp(params, "full"))
     {
	IpcPrintf(_("Commands currently available:\n"));
	IpcPrintf(_("         <command>     : <description>\n"));

	for (i = 0; i < num; i++)
	  {
	     IpcPrintf("----------------------------------------\n");
	     ipc = lst[i];
	     nick = (ipc->nick) ? ipc->nick : "";
	     IpcPrintf("%18s %4s: %s\n", ipc->name, nick, ipc->help_text);
	     if (ipc->extended_help_text)
		IpcPrintf("%s", ipc->extended_help_text);
	  }
     }
   else
     {
	for (i = 0; i < num; i++)
	  {
	     ipc = lst[i];
	     if (strcmp(params, ipc->name) &&
		 (!ipc->nick || strcmp(params, ipc->nick)))
		continue;

	     nick = (ipc->nick) ? ipc->nick : "";
	     IpcPrintf("----------------------------------------\n");
	     IpcPrintf("%18s %4s: %s\n", ipc->name, nick, ipc->help_text);
	     IpcPrintf("----------------------------------------\n");
	     if (ipc->extended_help_text)
		IpcPrintf("%s", ipc->extended_help_text);
	  }
     }
}

/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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

static Window       root_win;
static Window       my_win;

void
CommsInit(void)
{
   char               *str;

   str = getenv("ENL_WM_ROOT");
   root_win = (str) ? strtoul(str, NULL, 0) : DefaultRootWindow(disp);
}

Window
CommsSetup(Window win __UNUSED__)
{
   XSetWindowAttributes attr;

   attr.override_redirect = False;
   my_win = XCreateWindow(disp, root_win, -100, -100, 5, 5, 0, 0, InputOnly,
			  DefaultVisual(disp, DefaultScreen(disp)),
			  CWOverrideRedirect, &attr);

   return my_win;
}

Window
CommsFindCommsWindow(void)
{
   unsigned char      *s;
   Atom                a, ar;
   unsigned long       num, after;
   int                 format;
   Window              rt, comms_win;
   int                 dint;
   unsigned int        duint;

   comms_win = None;

   a = XInternAtom(disp, "ENLIGHTENMENT_COMMS", True);
   if (a == None)
      return None;

   s = NULL;
   XGetWindowProperty(disp, root_win, a, 0, 14, False, AnyPropertyType,
		      &ar, &format, &num, &after, &s);
   if (!s)
      return None;

   sscanf((char *)s, "%*s %lx", &comms_win);
   XFree(s);
   if (comms_win == None)
      return None;

   if (!XGetGeometry(disp, comms_win, &rt, &dint, &dint,
		     &duint, &duint, &duint, &duint))
      return None;

   s = NULL;
   XGetWindowProperty(disp, comms_win, a, 0, 14, False,
		      AnyPropertyType, &ar, &format, &num, &after, &s);
   if (!s)
      return None;
   XFree(s);

   XSelectInput(disp, comms_win, StructureNotifyMask | SubstructureNotifyMask);

   return comms_win;
}

void
CommsSend(Client * c, const char *s)
{
   char                ss[20];
   int                 i, j, k, len;
   XEvent              ev;
   Atom                a;

   if ((!s) || (!c) || (c->win == None))
      return;

   a = XInternAtom(disp, "ENL_MSG", True);

   ev.xclient.type = ClientMessage;
   ev.xclient.serial = 0;
   ev.xclient.send_event = True;
   ev.xclient.window = c->win;
   ev.xclient.message_type = a;
   ev.xclient.format = 8;

   len = strlen(s);
   for (i = 0; i < len + 1; i += 12)
     {
	sprintf(ss, "%8x", (int)my_win);
	for (j = 0; j < 12; j++)
	  {
	     ss[8 + j] = s[i + j];
	     if (!s[i + j])
		j = 12;
	  }
	for (k = 0; k < 20; k++)
	   ev.xclient.data.b[k] = ss[k];
	XSendEvent(disp, c->win, False, 0, &ev);
     }
}

char               *
CommsGet(Client * c, XEvent * ev)
{
   char                s[13], s2[9], *msg;
   unsigned int        i;
   Window              win;

   if ((!ev) || (!c))
      return NULL;
   if (ev->type != ClientMessage)
      return NULL;

   s[12] = 0;
   s2[8] = 0;
   msg = NULL;

   for (i = 0; i < 8; i++)
      s2[i] = ev->xclient.data.b[i];
   for (i = 0; i < 12; i++)
      s[i] = ev->xclient.data.b[i + 8];

   sscanf(s2, "%lx", &win);

   /* append text to end of msg */
   i = (c->msg) ? strlen(c->msg) : 0;
   c->msg = EREALLOC(char, c->msg, i + strlen(s) + 1);

   if (!c->msg)
      return NULL;
   strcpy(c->msg + i, s);

   if (strlen(s) < 12)
     {
	msg = c->msg;
	c->msg = NULL;
     }

   return msg;
}

Client             *
ClientCreate(Window win)
{
   Client             *c;

   c = EMALLOC(Client, 1);
   if (!c)
      return NULL;

   c->win = win;
   c->msg = NULL;

   return c;
}

void
ClientDestroy(Client * c)
{
   if (!c)
      return;

   Efree(c->msg);
   Efree(c);
}

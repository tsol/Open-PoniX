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
#include "comms.h"
#include "hints.h"
#include "ipc.h"
#include "e16-ecore_hints.h"
#include "e16-ecore_list.h"
#include "xwin.h"

typedef struct {
   char               *name;
   Window              xwin;
   char               *msg;
   char               *clientname;
   char               *version;
   char               *info;
   char                replied;
} Client;

static void         CommsSend(Client * c, const char *s);

static Ecore_List  *client_list = NULL;

static Win          comms_win = NULL;

static Client      *
ClientCreate(Window xwin)
{
   Client             *c;
   char                st[32];

   c = ECALLOC(Client, 1);
   if (!c)
      return NULL;

   Esnprintf(st, sizeof(st), "%8x", (int)xwin);
   c->name = Estrdup(st);
   c->xwin = xwin;

   if (!client_list)
      client_list = ecore_list_new();
   ecore_list_prepend(client_list, c);

   return c;
}

static void
ClientDestroy(Client * c)
{
   if (!c)
      return;

   ecore_list_node_remove(client_list, c);

   Efree(c->name);
   Efree(c->msg);
   Efree(c->clientname);
   Efree(c->version);
   Efree(c->info);

   Efree(c);
}

static int
ClientConfigure(Client * c, const char *str)
{
   char                param[64];
   const char         *value;
   int                 len;

   len = 0;
   sscanf(str, "%*s %60s %n", param, &len);
   value = str + len;

   if (!strcmp(param, "clientname"))
     {
	Efree(c->clientname);
	c->clientname = Estrdup(value);
     }
   else if (!strcmp(param, "version"))
     {
	Efree(c->version);
	c->version = Estrdup(value);
     }
   else if (!strcmp(param, "author"))
     {
     }
   else if (!strcmp(param, "email"))
     {
     }
   else if (!strcmp(param, "web"))
     {
     }
   else if (!strcmp(param, "address"))
     {
     }
   else if (!strcmp(param, "info"))
     {
	Efree(c->info);
	c->info = Estrdup(value);
     }
   else if (!strcmp(param, "pixmap"))
     {
     }
   else
     {
	return -1;
     }

   return 0;
}

static int
ClientMatchWindow(const void *data, const void *match)
{
   return ((const Client *)data)->xwin != (Window) match;
}

static Client      *
ClientFind(Window xwin)
{
   return (Client *) ecore_list_find(client_list, ClientMatchWindow,
				     (void *)xwin);
}

static char        *
ClientCommsGet(Client ** c, XClientMessageEvent * ev)
{
   char                s[13], s2[9], *msg;
   unsigned int        i;
   Window              xwin;
   Client             *cl;

   if ((!ev) || (!c))
      return NULL;
   if (ev->message_type != E16_ATOM_COMMS_MSG)
      return NULL;

   s[12] = 0;
   s2[8] = 0;
   for (i = 0; i < 8; i++)
      s2[i] = ev->data.b[i];
   for (i = 0; i < 12; i++)
      s[i] = ev->data.b[i + 8];
   xwin = None;
   sscanf(s2, "%lx", &xwin);
   if (xwin == None)
      return NULL;
   cl = ClientFind(xwin);
   if (!cl)
     {
	cl = ClientCreate(xwin);
	if (!cl)
	   return NULL;
     }

   /* append text to end of msg */
   i = (cl->msg) ? strlen(cl->msg) : 0;
   cl->msg = EREALLOC(char, cl->msg, i + strlen(s) + 1);
   if (!cl->msg)
      return NULL;
   strcpy(cl->msg + i, s);

   msg = NULL;
   if (strlen(s) < 12)
     {
	msg = cl->msg;
	cl->msg = NULL;
	*c = cl;
     }

   return msg;
}

static void
ClientIpcReply(void *data, const char *str)
{
   Client             *c = (Client *) data;

   if (!str)
     {
	/* Don't send empty replies (ack's) if we ever have replied to this
	 * client. Without this hack communication with e.g. epplets fails. */
	if (c->replied)
	   return;
	str = "";
     }
   CommsSend(c, str);
   c->replied = 1;
}

static void
ClientHandleComms(XClientMessageEvent * ev)
{
   Client             *c;
   char               *s;

   s = ClientCommsGet(&c, ev);
   if (!s)
      return;

   if (EDebug(EDBUG_TYPE_IPC))
      Eprintf("ClientHandleComms: %s\n", s);

   if (!strncmp(s, "set ", 4))
     {
	/* The old Client set command (used by epplets) */
	if (ClientConfigure(c, s) == 0)
	   goto done;
     }

   if (!IpcExecReply(s, ClientIpcReply, c))
     {
#if ENABLE_DIALOGS
	const char         *s1, *s2;

	s1 = (c->clientname) ? c->clientname : "UNKNOWN";
	s2 = (c->version) ? c->version : "UNKNOWN";
	DialogOK(_("E IPC Error"),
		 _("Received Unknown Client Message.\n"
		   "Client Name:    %s\n" "Client Version: %s\n"
		   "Message Contents:\n\n" "%s\n"), s1, s2, s);
#endif
	SoundPlay(SOUND_ERROR_IPC);
     }

 done:
   Efree(s);
}

static void
ClientHandleRootEvents(Win win __UNUSED__, XEvent * ev, void *prm __UNUSED__)
{
   Client             *c;

#if 0
   Eprintf("ClientHandleRootEvents: type=%d win=%#lx\n", ev->type,
	   ev->xany.window);
#endif
   switch (ev->type)
     {
     case DestroyNotify:
	c = ClientFind(ev->xdestroywindow.window);
	if (!c)
	   break;
	ClientDestroy(c);
	break;
     }
}

static void
ClientHandleCommsEvents(Win win __UNUSED__, XEvent * ev, void *prm __UNUSED__)
{
#if 0
   Eprintf("ClientHandleCommsEvents: type=%d win=%#lx\n", ev->type,
	   ev->xany.window);
#endif
   switch (ev->type)
     {
     case ClientMessage:
	ClientHandleComms(&(ev->xclient));
	break;
     }
}

void
CommsInit(void)
{
   char                s[1024];

   comms_win = ECreateEventWindow(VROOT, -100, -100, 5, 5);
   ESelectInput(comms_win, StructureNotifyMask | SubstructureNotifyMask);
   EventCallbackRegister(comms_win, ClientHandleCommsEvents, NULL);
   EventCallbackRegister(VROOT, ClientHandleRootEvents, NULL);

   Esnprintf(s, sizeof(s), "WINID %8lx", WinGetXwin(comms_win));
   ecore_x_window_prop_string_set(WinGetXwin(comms_win), E16_ATOM_COMMS_WIN, s);
   ecore_x_window_prop_string_set(WinGetXwin(VROOT), E16_ATOM_COMMS_WIN, s);
}

static void
CommsDoSend(Window win, const char *s)
{
   char                ss[21];
   int                 i, j, k, len;
   XEvent              ev;

   if ((!win) || (!s))
      return;

   len = strlen(s);
   ev.xclient.type = ClientMessage;
   ev.xclient.serial = 0;
   ev.xclient.send_event = True;
   ev.xclient.window = win;
   ev.xclient.message_type = E16_ATOM_COMMS_MSG;
   ev.xclient.format = 8;
   for (i = 0; i < len + 1; i += 12)
     {
	Esnprintf(ss, sizeof(ss), "%8lx", WinGetXwin(comms_win));
	for (j = 0; j < 12; j++)
	  {
	     ss[8 + j] = s[i + j];
	     if (!s[i + j])
		j = 12;
	  }
	ss[20] = 0;
	for (k = 0; k < 20; k++)
	   ev.xclient.data.b[k] = ss[k];
	EXSendEvent(win, 0, (XEvent *) & ev);
     }
}

static void
CommsSend(Client * c, const char *s)
{
   if (!c)
      return;

   CommsDoSend(c->xwin, s);
}

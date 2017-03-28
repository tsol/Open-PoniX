/*
 * Copyright (C) 2007-2008 Kim Woelders
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
#include "edbus.h"
#include "events.h"
#include "ipc.h"
#include "xwin.h"
#include <dbus/dbus.h>

#define ENABLE_INTROSPECTION 1

#define DEBUG_DBUS 1
#if DEBUG_DBUS
#define Dprintf(fmt...)  if(EDebug(EDBUG_TYPE_DBUS))Eprintf(fmt)
#define D2printf(fmt...) if(EDebug(EDBUG_TYPE_DBUS)>1)Eprintf(fmt)
#else
#define Dprintf(fmt...)
#define D2printf(fmt...)
#endif

typedef struct {
   char               *name;
   DBusConnection     *conn;
   DBusWatch          *watch;
   int                 fd;
} DbusData;

static DbusData     dbus_data;

static EventFdDesc *db_efd = NULL;

static              dbus_bool_t
DbusWatchAdd(DBusWatch * watch, void *data __UNUSED__)
{
   dbus_data.watch = watch;
   dbus_data.fd = dbus_watch_get_unix_fd(watch);

   D2printf("DbusWatchAdd fd=%d flags=%d\n", dbus_data.fd,
	    dbus_watch_get_flags(dbus_data.watch));

   return TRUE;
}

static void
DbusWatchRemove(DBusWatch * watch __UNUSED__, void *data __UNUSED__)
{
   D2printf("DbusWatchRemove\n");
}

#if 0				/* Don't need this */
static void
DbusWatchToggle(DBusWatch * watch __UNUSED__, void *data __UNUSED__)
{
   D2printf("DbusWatchToggle\n");
}
#else
#define DbusWatchToggle NULL
#endif

static void
DbusReplyString(DBusConnection * conn, DBusMessage * msg, const char *str)
{
   DBusMessage        *reply;
   DBusMessageIter     args;

   reply = dbus_message_new_method_return(msg);

   dbus_message_iter_init_append(reply, &args);
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &str))
      goto done;
   if (!dbus_connection_send(conn, reply, NULL))
      goto done;
   dbus_connection_flush(conn);

 done:
   dbus_message_unref(reply);
}

static void
DbusIpcReply(void *data, const char *str)
{
   DBusMessage        *msg = (DBusMessage *) data;

   if (!str)
      str = "ok";
   DbusReplyString(dbus_data.conn, msg, str);
}

static void
DbusMethodCommand(DBusConnection * conn, DBusMessage * msg)
{
   DBusMessageIter     args;
   const char         *param = "";

   if (!dbus_message_iter_init(msg, &args) ||
       dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_STRING)
     {
	DbusReplyString(conn, msg, "String arg required\n");
	return;
     }
   else
      dbus_message_iter_get_basic(&args, &param);

   IpcExecReply(param, DbusIpcReply, msg);
}

#if ENABLE_INTROSPECTION
/* *INDENT-OFF* */
static const char   dbus_introspect_data[] =
   "<!DOCTYPE node PUBLIC '-//freedesktop//DTD D-BUS Object Introspection 1.0//EN'\n"
   "'http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd'>\n"
   "<node>\n"
   " <interface name='org.freedesktop.DBus.Introspectable'>\n"
   "  <method name='Introspect'>\n"
   "   <arg name='data' direction='out' type='s'/>\n"
   "  </method>\n"
   " </interface>\n"
   " <interface name='%s'>\n"
   "  <method name='Command'>\n"
   "   <arg name='data' direction='out' type='s'/>\n"
   "  </method>\n"
   "  <signal name='Signal'>\n"
   "   <arg name='data' direction='in' type='s'/>\n"
   "  </signal>\n"
   " </interface>\n"
   "</node>";
/* *INDENT-ON* */

static void
DbusMsgIntrospect(DBusConnection * conn, DBusMessage * msg)
{
   char                buf[1024];

   D2printf("Introspect\n");

   Esnprintf(buf, sizeof(buf), dbus_introspect_data, dbus_data.name);
   if (!strcmp(dbus_message_get_path(msg), "/"))
      DbusReplyString(conn, msg, buf);
   else
      DbusReplyString(conn, msg, "");
}
#endif

static              DBusHandlerResult
DbusMsgHandler(DBusConnection * conn, DBusMessage * msg,
	       void *user_data __UNUSED__)
{
   int                 msg_type;
   const char         *msg_dest, *msg_ifc, *msg_memb;

   D2printf("DbusMsgHandler\n");

   msg_type = dbus_message_get_type(msg);
   msg_dest = dbus_message_get_destination(msg);
   msg_ifc = dbus_message_get_interface(msg);
   msg_memb = dbus_message_get_member(msg);

   switch (msg_type)
     {
     default:			/* Should not be possible */
	Dprintf("MESSAGE type=%d\n", msg_type);
	break;

     case DBUS_MESSAGE_TYPE_METHOD_CALL:
	Dprintf("METHOD %s %s\n", msg_ifc, msg_memb);
	if (strcmp(msg_dest, dbus_data.name))
	   break;
	if (!strcmp(msg_ifc, "org.e16"))
	  {
	     if (!strcmp(msg_memb, "Command"))
		DbusMethodCommand(conn, msg);
	     else
		DbusReplyString(conn, msg, "Error");
	  }
#if ENABLE_INTROSPECTION
	else if (!strcmp(msg_ifc, "org.freedesktop.DBus.Introspectable"))
	  {
	     if (!strcmp(msg_memb, "Introspect"))
		DbusMsgIntrospect(conn, msg);
	  }
#endif
	break;

     case DBUS_MESSAGE_TYPE_SIGNAL:
	Dprintf("SIGNAL %s %s\n", msg_ifc, msg_memb);
	if (!strcmp(msg_ifc, "org.e16"))
	  {
	     Dprintf("... for me!\n");
	  }
	break;
     }

   D2printf("sender    = %s\n", dbus_message_get_sender(msg));
   D2printf("dest      = %s\n", dbus_message_get_destination(msg));
   D2printf("path      = %s\n", dbus_message_get_path(msg));
   D2printf("interface = %s\n", dbus_message_get_interface(msg));
   D2printf("member    = %s\n", dbus_message_get_member(msg));

   return DBUS_HANDLER_RESULT_HANDLED;
}

static void
DbusHandleFd(void)
{
   int                 rc;

   D2printf("DbusHandleFd flags=%d\n", dbus_watch_get_flags(dbus_data.watch));
   dbus_watch_handle(dbus_data.watch,
		     DBUS_WATCH_READABLE | DBUS_WATCH_WRITABLE);

   for (;;)
     {
	D2printf("DbusHandleFd: Dispatch flags=%d\n",
		 dbus_watch_get_flags(dbus_data.watch));
	rc = dbus_connection_dispatch(dbus_data.conn);
	if (rc == DBUS_DISPATCH_COMPLETE)
	   break;
     }
}

void
DbusInit(void)
{
   DBusError           dberr;
   int                 err;
   char                buf[128];

   if (Mode.wm.window)
     {
	sprintf(buf, "org.e16.wm.p%u", (unsigned int)Mode.wm.pid);
     }
   else
     {
	const char         *s;

	s = strchr(Dpy.name, ':');
	if (!s)
	   return;
	sprintf(buf, "org.e16.wm.d%ds%d", atoi(s + 1), Dpy.screen);
     }
   dbus_data.name = Estrdup(buf);
   Esetenv("ENL_DBUS_NAME", dbus_data.name);

   dbus_error_init(&dberr);

   dbus_data.fd = -1;

   dbus_data.conn = dbus_bus_get(DBUS_BUS_SESSION, &dberr);
   if (dbus_error_is_set(&dberr))
      goto bail_out;
   dbus_connection_set_exit_on_disconnect(dbus_data.conn, FALSE);

   err = dbus_bus_request_name(dbus_data.conn, dbus_data.name,
			       DBUS_NAME_FLAG_DO_NOT_QUEUE, &dberr);
   if (dbus_error_is_set(&dberr))
      goto bail_out;
   if (err != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
     {
	Eprintf("*** DbusInit error: Not Primary Owner (%d)\n", err);
	return;
     }

   Esnprintf(buf, sizeof(buf), "type='signal',destination='%s'",
	     dbus_data.name);
   if (EDebug(EDBUG_TYPE_DBUS) > 1)	/* Catch all signals if extra debug is enabled */
      dbus_bus_add_match(dbus_data.conn, "type='signal'", &dberr);
   else
      dbus_bus_add_match(dbus_data.conn, buf, &dberr);
   if (dbus_error_is_set(&dberr))
      goto bail_out;
   Esnprintf(buf, sizeof(buf), "type='method_call',destination='%s'",
	     dbus_data.name);
   dbus_bus_add_match(dbus_data.conn, buf, &dberr);
   if (dbus_error_is_set(&dberr))
      goto bail_out;
#if 0				/* Debug */
   dbus_bus_add_match(dbus_data.conn, "type='method_return'", &dberr);
   if (dbus_error_is_set(&dberr))
      goto bail_out;
   dbus_bus_add_match(dbus_data.conn, "type='error'", &dberr);
   if (dbus_error_is_set(&dberr))
      goto bail_out;
#endif

   if (!dbus_connection_add_filter(dbus_data.conn, DbusMsgHandler, NULL, NULL))
      return;

   err = dbus_connection_set_watch_functions(dbus_data.conn,
					     DbusWatchAdd, DbusWatchRemove,
					     DbusWatchToggle, NULL, NULL);

   /* Handle pending D-Bus stuff */
   DbusHandleFd();
   db_efd = EventFdRegister(dbus_data.fd, DbusHandleFd);
   return;

 bail_out:
   if (dbus_error_is_set(&dberr))
     {
	Eprintf("*** DbusInit error: %s\n", dberr.message);
	dbus_error_free(&dberr);
     }
   return;
}

#if 0				/* No need? */
void
DbusExit(void)
{
}
#endif

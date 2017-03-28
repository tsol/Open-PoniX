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
#include "dialog.h"
#include "e16-ecore_hints.h"
#include "emodule.h"
#include "events.h"
#include "ewins.h"
#include "hints.h"
#include "session.h"
#include "settings.h"
#include "snaps.h"
#include "user.h"
#include "xwin.h"

#ifdef USE_EXT_INIT_WIN
static Window       new_init_win_ext = None;
#endif

/* True if we are saving state for a doExit("restart") */
static int          restarting = False;

#if USE_SM

#include <fcntl.h>
#include <X11/SM/SMlib.h>

/*
 * NB! If the discard property is revived, the dual use of buf must be fixed.
 */
#define USE_DISCARD_PROPERTY 0

static char        *sm_client_id = NULL;
static SmcConn      sm_conn = NULL;

static EventFdDesc *sm_efd = NULL;

static void
set_save_props(SmcConn smc_conn, int master_flag)
{
   const char         *s;
   const char         *user;
   const char         *program;
   char                priority = 10;
   char                style;
   int                 i, n;
   SmPropValue         programVal;
   SmPropValue         userIDVal;

#if USE_DISCARD_PROPERTY
   const char         *sh = "sh";
   const char         *c = "-c";
   const char         *sm_file;
   SmPropValue         discardVal[3];
   SmProp              discardProp;
#endif
#ifdef USE_EXT_INIT_WIN
   char                bufx[32];
#endif
   SmPropValue         restartVal[32];
   SmPropValue         styleVal;
   SmPropValue         priorityVal;
   SmProp              programProp;
   SmProp              userIDProp;
   SmProp              restartProp;
   SmProp              cloneProp;
   SmProp              styleProp;
   SmProp              priorityProp;
   SmProp             *props[7];
   char                bufs[32], bufm[32];

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("set_save_props\n");

   programProp.name = (char *)SmProgram;
   programProp.type = (char *)SmARRAY8;
   programProp.num_vals = 1;
   programProp.vals = &programVal;

   userIDProp.name = (char *)SmUserID;
   userIDProp.type = (char *)SmARRAY8;
   userIDProp.num_vals = 1;
   userIDProp.vals = &userIDVal;

#if USE_DISCARD_PROPERTY
   discardProp.name = (char *)SmDiscardCommand;
   discardProp.type = (char *)SmLISTofARRAY8;
   discardProp.num_vals = 3;
   discardProp.vals = discardVal;
#endif

   restartProp.name = (char *)SmRestartCommand;
   restartProp.type = (char *)SmLISTofARRAY8;
   restartProp.vals = restartVal;

   cloneProp.name = (char *)SmCloneCommand;
   cloneProp.type = (char *)SmLISTofARRAY8;
   cloneProp.vals = restartVal;

   styleProp.name = (char *)SmRestartStyleHint;
   styleProp.type = (char *)SmCARD8;
   styleProp.num_vals = 1;
   styleProp.vals = &styleVal;

   priorityProp.name = (char *)"_GSM_Priority";
   priorityProp.type = (char *)SmCARD8;
   priorityProp.num_vals = 1;
   priorityProp.vals = &priorityVal;

   if (master_flag)
      /* Master WM restarts immediately for a doExit("restart") */
      style = restarting ? SmRestartImmediately : SmRestartIfRunning;
   else
      /* Slave WMs never restart */
      style = SmRestartNever;

   user = username();
   /* The SM specs state that the SmProgram should be the argument passed
    * to execve. Passing argv[0] is close enough. */
   program = Mode.wm.exec_name;

   userIDVal.length = (user) ? strlen(user) : 0;
   userIDVal.value = (char *)user;
   programVal.length = strlen(program);
   programVal.value = (char *)program;
   styleVal.length = 1;
   styleVal.value = &style;
   priorityVal.length = 1;
   priorityVal.value = &priority;

#if USE_DISCARD_PROPERTY
   /* Tell session manager how to clean up our old data */
   sm_file = EGetSavePrefix();
   Esnprintf(buf, sizeof(buf), "rm %s*.clients.*", sm_file);

   discardVal[0].length = strlen(sh);
   discardVal[0].value = sh;
   discardVal[1].length = strlen(c);
   discardVal[1].value = c;
   discardVal[2].length = strlen(buf);
   discardVal[2].value = buf;	/* ??? Also used in restartVal ??? */
#endif

   n = 0;
   restartVal[n++].value = (char *)program;
   if (Mode.wm.single)
     {
	Esnprintf(bufs, sizeof(bufs), "%i", Mode.wm.master_screen);
	restartVal[n++].value = (char *)"-s";
	restartVal[n++].value = (char *)bufs;
     }
   else if (restarting && !Mode.wm.master)
     {
	Esnprintf(bufm, sizeof(bufm), "%i", Mode.wm.master_screen);
	restartVal[n++].value = (char *)"-m";
	restartVal[n++].value = bufm;
     }
#ifdef USE_EXT_INIT_WIN
   if (restarting)
     {
	Esnprintf(bufx, sizeof(bufx), "%#lx", new_init_win_ext);
	restartVal[n++].value = (char *)"-X";
	restartVal[n++].value = bufx;
     }
#endif
#if 0
   restartVal[n++].value = (char *)smfile;
   restartVal[n++].value = (char *)sm_file;
#endif
   s = Mode.conf.name;
   if (s)
     {
	restartVal[n++].value = (char *)"-p";
	restartVal[n++].value = (char *)s;
     }
   s = Mode.conf.dir;
   if (s)
     {
	restartVal[n++].value = (char *)"-P";
	restartVal[n++].value = (char *)s;
     }
   s = Mode.conf.cache_dir;
   if (s)
     {
	restartVal[n++].value = (char *)"-Q";
	restartVal[n++].value = (char *)s;
     }
   s = sm_client_id;
   restartVal[n++].value = (char *)"-S";
   restartVal[n++].value = (char *)s;

   for (i = 0; i < n; i++)
      restartVal[i].length = strlen((const char *)restartVal[i].value);

   restartProp.num_vals = n;

   /* SM specs require SmCloneCommand excludes "--sm-client-id" option */
   cloneProp.num_vals = restartProp.num_vals - 2;

   if (EDebug(EDBUG_TYPE_SESSION))
      for (i = 0; i < restartProp.num_vals; i++)
	 Eprintf("restartVal[i]: %2d: %s\n", restartVal[i].length,
		 (char *)restartVal[i].value);

   n = 0;
   props[n++] = &programProp;
   props[n++] = &userIDProp;
#if USE_DISCARD_PROPERTY
   props[n++] = &discardProp;
#endif
   props[n++] = &restartProp;
   props[n++] = &cloneProp;
   props[n++] = &styleProp;
   props[n++] = &priorityProp;

   SmcSetProperties(smc_conn, n, props);
}

/* This function is usually exclusively devoted to saving data.
 * However, E sometimes wants to save state and exit immediately afterwards
 * so that the SM will restart it in a different theme. Therefore, we include
 * a suicide clause at the end.
 */
static void
callback_save_yourself2(SmcConn smc_conn, SmPointer client_data __UNUSED__)
{
   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("callback_save_yourself2\n");

   set_save_props(smc_conn, Mode.wm.master);
   SmcSaveYourselfDone(smc_conn, True);
   if (restarting)
      EExit(0);
}

static void
callback_save_yourself(SmcConn smc_conn, SmPointer client_data __UNUSED__,
		       int save_style __UNUSED__, Bool shutdown __UNUSED__,
		       int interact_style __UNUSED__, Bool fast __UNUSED__)
{
   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("callback_save_yourself\n");

   SmcRequestSaveYourselfPhase2(smc_conn, callback_save_yourself2, NULL);
}

static void
callback_die(SmcConn smc_conn __UNUSED__, SmPointer client_data __UNUSED__)
{
   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("callback_die\n");

   SessionExit(EEXIT_EXIT, NULL);
}

static void
callback_save_complete(SmcConn smc_conn __UNUSED__,
		       SmPointer client_data __UNUSED__)
{
   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("callback_save_complete\n");
}

static void
callback_shutdown_cancelled(SmcConn smc_conn, SmPointer client_data __UNUSED__)
{
   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("callback_shutdown_cancelled\n");

   SmcSaveYourselfDone(smc_conn, False);
}

#if 0				/* Unused */
static Atom         atom_sm_client_id;
#endif

static IceConn      ice_conn;

static void
ice_io_error_handler(IceConn connection __UNUSED__)
{
   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("ice_io_error_handler\n");

   /* The less we do here the better - the default handler does an
    * exit(1) instead of closing the losing connection. */
}

static void
ice_exit(void)
{
   SmcCloseConnection(sm_conn, 0, NULL);
   sm_conn = NULL;
   EventFdUnregister(sm_efd);
}

static void
ice_msgs_process(void)
{
   IceProcessMessagesStatus status;

   status = IceProcessMessages(ice_conn, NULL, NULL);
   if (status == IceProcessMessagesIOError)
     {
	/* Less of the hope.... E survives */
	Alert(_("ERROR!\n" "\n"
		"Lost the Session Manager that was there?\n"
		"Here here session manager... come here... want a bone?\n"
		"Oh come now! Stop sulking! Bugger. Oh well. "
		"Will continue without\n" "a session manager.\n" "\n"
		"I'll survive somehow.\n" "\n" "\n" "... I hope.\n"));
	ice_exit();
     }
}

static void
ice_init(void)
{
   static SmPointer    context;
   SmcCallbacks        callbacks;
   char                error_string_ret[4096];
   char               *client_id;
   char                style[2];
   SmPropValue         styleVal;
   SmProp              styleProp;
   SmProp             *props[1];
   int                 sm_fd;

   if (!getenv("SESSION_MANAGER"))
      return;

   IceSetIOErrorHandler(ice_io_error_handler);

   callbacks.save_yourself.callback = callback_save_yourself;
   callbacks.die.callback = callback_die;
   callbacks.save_complete.callback = callback_save_complete;
   callbacks.shutdown_cancelled.callback = callback_shutdown_cancelled;

   callbacks.save_yourself.client_data = callbacks.die.client_data =
      callbacks.save_complete.client_data =
      callbacks.shutdown_cancelled.client_data = (SmPointer) NULL;

   client_id = Estrdup(sm_client_id);

   error_string_ret[0] = '\0';

   sm_conn =
      SmcOpenConnection(NULL, &context, SmProtoMajor, SmProtoMinor,
			SmcSaveYourselfProcMask | SmcDieProcMask |
			SmcSaveCompleteProcMask |
			SmcShutdownCancelledProcMask, &callbacks,
			client_id, &sm_client_id, 4096, error_string_ret);
   Efree(client_id);

   if (error_string_ret[0])
      Eprintf("While connecting to session manager: %s.", error_string_ret);

   if (!sm_conn)
      return;

   style[0] = SmRestartIfRunning;
   style[1] = 0;

   styleVal.length = 1;
   styleVal.value = style;

   styleProp.name = (char *)SmRestartStyleHint;
   styleProp.type = (char *)SmCARD8;
   styleProp.num_vals = 1;
   styleProp.vals = &styleVal;

   props[0] = &styleProp;

   ice_conn = SmcGetIceConnection(sm_conn);
   sm_fd = IceConnectionNumber(ice_conn);
   /* Just in case we are a copy of E created by a doExit("restart") */
   SmcSetProperties(sm_conn, 1, props);
   fcntl(sm_fd, F_SETFD, fcntl(sm_fd, F_GETFD, 0) | FD_CLOEXEC);

   sm_efd = EventFdRegister(sm_fd, ice_msgs_process);
}

#endif /* USE_SM */

void
SessionInit(void)
{
   if (Mode.wm.window)
      return;

#if 0				/* Unused */
   atom_sm_client_id = EInternAtom("SM_CLIENT_ID");
#endif

#if USE_SM
   ice_init();
#endif

   if (!Conf.session.script)
      Conf.session.script = Estrdup("$EROOT/scripts/session.sh");
   if (!Conf.session.cmd_reboot)
      Conf.session.cmd_reboot = Estrdup("reboot");
   if (!Conf.session.cmd_halt)
      Conf.session.cmd_halt = Estrdup("poweroff");
}

void
SessionGetInfo(EWin * ewin __UNUSED__)
{
#if 0				/* Unused */
#if USE_SM
   if (atom_sm_client_id == None)
      return;
   _EFREE(ewin->session_id);
   if (ewin->icccm.client_leader != None)
      ewin->session_id =
	 ecore_x_window_prop_string_get(ewin->icccm.client_leader,
					atom_sm_client_id);
#else
   ewin = NULL;
#endif /* USE_SM */
#endif
}

#if USE_SM
void
SetSMID(const char *smid)
{
   sm_client_id = Estrdup(smid);
}
#else
void
SetSMID(const char *smid __UNUSED__)
{
}
#endif /* USE_SM */

static void
SessionSave(int shutdown)
{
   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("SessionSave(%d)\n", shutdown);

   SnapshotsSaveReal();

#if USE_SM
   if (shutdown && sm_conn)
      ice_exit();
#endif /* USE_SM */
}

/*
 * Normally, the SM will throw away all the session data for a client
 * that breaks its connection unexpectedly. In order to avoid this we 
 * have to let the SM handle the restart (by setting a SmRestartStyleHint
 * of SmRestartImmediately). Rather than forcing all SM clients to do a
 * checkpoint (which would be a bit cleaner) we just save our own state
 * and then restore it on restart. We grab X input via the ext_init_win
 * so the our clients remain frozen while we are down.
 */
__NORETURN__ static void
doSMExit(int mode, const char *params)
{
   int                 l;
   char                s[1024];
   const char         *ss;

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("doSMExit: mode=%d prm=%p\n", mode, params);

   restarting = True;

   SessionSave(1);

   if (mode != EEXIT_THEME && mode != EEXIT_RESTART)
      SessionHelper(ESESSION_STOP);

   LangExit();

   if (disp)
     {
	/* We may get here from HandleXIOError */
	EwinsSetFree();
	ESelectInput(VROOT, 0);
	ExtInitWinKill();
	ESync(0);

	/* Forget about cleaning up if no disp */
	ModulesSignal(ESIGNAL_EXIT, NULL);
     }

   ss = NULL;
   switch (mode)
     {
     case EEXIT_EXEC:
	SoundPlay(SOUND_EXIT);
	EDisplayClose();

	if (EDebug(EDBUG_TYPE_SESSION))
	   Eprintf("doSMExit: exec %s\n", params);
	EexecCmd(params);
	break;

     case EEXIT_THEME:
	ss = params;
	/* FALLTHROUGH */
     case EEXIT_RESTART:
	SoundPlay(SOUND_WAIT);
#ifdef USE_EXT_INIT_WIN
	if (disp && !Mode.wm.window)
	   new_init_win_ext = ExtInitWinCreate();
#endif
	EDisplayClose();

	l = 0;
	l += Esnprintf(s + l, sizeof(s) - l, "%s -f", Mode.wm.exec_name);
	if (Mode.wm.single)
	   l += Esnprintf(s + l, sizeof(s) - l, " -s %d", Dpy.screen);
	else if (!Mode.wm.master)
	   l +=
	      Esnprintf(s + l, sizeof(s) - l, " -m %d", Mode.wm.master_screen);
	if (Mode.wm.window)
	   l += Esnprintf(s + l, sizeof(s) - l, " -w %dx%d",
			  WinGetW(VROOT), WinGetH(VROOT));
#if USE_SM
	if (sm_client_id)
	   l += Esnprintf(s + l, sizeof(s) - l, " -S %s", sm_client_id);
#endif
#ifdef USE_EXT_INIT_WIN
	if (new_init_win_ext != None)
	   l += Esnprintf(s + l, sizeof(s) - l, " -X %#lx", new_init_win_ext);
#endif
	if (ss)
	   l += Esnprintf(s + l, sizeof(s) - l, " -t %s", ss);

	if (EDebug(EDBUG_TYPE_SESSION))
	   Eprintf("doSMExit: exec %s\n", s);

	EexecCmd(s);
	break;
     }

   restarting = False;
   SoundPlay(SOUND_EXIT);
   EExit(0);
}

static void
SessionLogout(void)
{
#if USE_SM
   if (sm_conn)
     {
	SmcRequestSaveYourself(sm_conn, SmSaveBoth, True, SmInteractStyleAny,
			       False, True);
     }
   else
#endif /* USE_SM */
     {
	SessionExit(EEXIT_EXIT, NULL);
     }
}

#if ENABLE_DIALOGS
static void
LogoutCB(Dialog * d, int val, void *data __UNUSED__)
{
#if USE_SM
   if (sm_conn)
     {
	SessionLogout();
     }
   else
#endif /* USE_SM */
     {
	/* 0:LogOut -: No    -or-        */
	/* 0:Halt 1:Reboot 2:LogOut -:No */
	switch (val)
	  {
	  default:
	     break;
	  case 1:
	     SessionExit(EEXIT_EXIT, NULL);
	     break;
	  case 2:
	     SessionExit(EEXIT_EXEC, Conf.session.cmd_reboot);
	     break;
	  case 3:
	     SessionExit(EEXIT_EXEC, Conf.session.cmd_halt);
	     break;
	  }
     }

   DialogClose(d);
}

static void
SessionLogoutConfirm(void)
{
   Dialog             *d;
   DItem              *table, *di;

   d = DialogFind("LOGOUT_DIALOG");
   if (!d)
     {
	SoundPlay(SOUND_LOGOUT);
	d = DialogCreate("LOGOUT_DIALOG");
	table = DialogInitItem(d);
	DialogSetTitle(d, _("Are you sure?"));
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetText(di, _("Are you sure you wish to log out ?"));
	table = DialogAddItem(table, DITEM_TABLE);
	DialogItemSetAlign(table, 512, 0);
	DialogItemSetFill(table, 0, 0);
	DialogItemTableSetOptions(table, 2, 0, 1, 0);
	if (Conf.session.enable_reboot_halt)
	  {
	     DialogItemTableSetOptions(table, 4, 0, 1, 0);
	     DialogItemAddButton(table, _("Yes, Shut Down"), LogoutCB, 3,
				 1, DLG_BUTTON_OK);
	     DialogItemAddButton(table, _("Yes, Reboot"), LogoutCB, 2,
				 1, DLG_BUTTON_OK);
	  }
	DialogItemAddButton(table, _("Yes, Log Out"), LogoutCB, 1,
			    1, DLG_BUTTON_OK);
	DialogItemAddButton(table, _("No"), NULL, 0, 1, DLG_BUTTON_CANCEL);
	DialogBindKey(d, "Escape", DialogCallbackClose, 0, NULL);
	DialogBindKey(d, "Return", LogoutCB, 1, NULL);
     }

   DialogShowCentered(d);
}
#endif /* ENABLE_DIALOGS */

void
SessionExit(int mode, const char *param)
{
   /* We do not want to be exited by children. */
   if (getpid() != Mode.wm.pid)
      return;

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("SessionExit: mode=%d(%d) prm=%s\n", mode, Mode.wm.exit_mode,
	      param);

   if (Mode.wm.exiting)
      return;

   if (Mode.wm.startup || Mode.wm.exit_now)
      goto done;

   switch (mode)
     {
     default:
	/* In event loop - Set exit mode */
	Mode.wm.exit_mode = mode;
	Mode.wm.exit_param = Estrdup(param);
	return;

     case EEXIT_QUIT:
	mode = Mode.wm.exit_mode;
	param = Mode.wm.exit_param;
	break;

     case EEXIT_ERROR:
	if (!Mode.wm.exiting)
	   break;
	/* This may be possible during nested signal handling */
	Eprintf("SessionExit already in progress ... now exiting\n");
	exit(1);
	break;

     case EEXIT_LOGOUT:
#if ENABLE_DIALOGS
	if (Conf.session.enable_logout_dialog)
	   SessionLogoutConfirm();
	else
#endif
	   SessionLogout();
	return;
     }

 done:
   Mode.wm.exiting++;
   doSMExit(mode, param);
}

static void
SessionRunProg(const char *prog, const char *params)
{
   char                buf[4096];
   const char         *s;

   if (params)
     {
	Esnprintf(buf, sizeof(buf), "%s %s", prog, params);
	s = buf;
     }
   else
     {
	s = prog;
     }
   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("SessionRunProg: %s\n", s);
   Esystem(s);
}

void
SessionHelper(int when)
{
   switch (when)
     {
     case ESESSION_INIT:
	if (Conf.session.enable_script && Conf.session.script)
	   SessionRunProg(Conf.session.script, "init");
	break;
     case ESESSION_START:
	if (Conf.session.enable_script && Conf.session.script)
	   SessionRunProg(Conf.session.script, "start");
	break;
     case ESESSION_STOP:
	if (Conf.session.enable_script && Conf.session.script)
	   SessionRunProg(Conf.session.script, "stop");
	break;
     }
}

#if ENABLE_DIALOGS
/*
 * Session dialog
 */
static char         tmp_session_script;
static char         tmp_logout_dialog;
static char         tmp_reboot_halt;

static void
CB_ConfigureSession(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.session.enable_script = tmp_session_script;
	Conf.session.enable_logout_dialog = tmp_logout_dialog;
	Conf.session.enable_reboot_halt = tmp_reboot_halt;
     }
   autosave();
}

static void
_DlgFillSession(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di;

   tmp_session_script = Conf.session.enable_script;
   tmp_logout_dialog = Conf.session.enable_logout_dialog;
   tmp_reboot_halt = Conf.session.enable_reboot_halt;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Session Script"));
   DialogItemCheckButtonSetPtr(di, &tmp_session_script);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Logout Dialog"));
   DialogItemCheckButtonSetPtr(di, &tmp_logout_dialog);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Reboot/Halt on Logout"));
   DialogItemCheckButtonSetPtr(di, &tmp_reboot_halt);
}

const DialogDef     DlgSession = {
   "CONFIGURE_SESSION",
   N_("Session"),
   N_("Session Settings"),
   SOUND_SETTINGS_SESSION,
   "pix/miscellaneous.png",
   N_("Enlightenment Session\n" "Settings Dialog"),
   _DlgFillSession,
   DLG_OAC, CB_ConfigureSession,
};
#endif /* ENABLE_DIALOGS */

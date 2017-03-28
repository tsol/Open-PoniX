/* 
 *  Matchbox Window Manager - A lightweight window manager not for the
 *                            desktop.
 *
 *  Authored By Matthew Allum <mallum@o-hand.com>
 *
 *  Copyright (c) 2002, 2004 OpenedHand Ltd - http://o-hand.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include "session.h"

#if USE_SM

/* mostly based on twm/metacity code */

static void
sm_save_yourself_cb (SmcConn   smcConn,
		     SmPointer clientData,
		     int       saveType,
		     Bool      shutdown,
		     int       interactStyle,
		     Bool      fast)
{
  /* Wm *w = (Wm *)clientData; */

  /* XXX This needs much work */

  SmProp      prop1, prop2, prop3, prop4, prop5, prop6, *props[6];
  SmPropValue prop1val, prop2val, prop3val, prop4val, prop5val, prop6val;

  static int  first_time = 1;

  dbg("%s() session mark\n", __func__);

  if (first_time)
    {
      char userId[20], pid[20];
      char hint     = SmRestartImmediately;
      char priority = 20;

      dbg("%s() session mark\n", __func__);

      prop1.name      = SmProgram;
      prop1.type      = SmARRAY8;
      prop1.num_vals  = 1;
      prop1.vals      = &prop1val;
      prop1val.value  = "matchbox-window-manager"; /* Argv[0]; */ 
      prop1val.length = strlen ("matchbox-window-manager");

      snprintf (userId, 20, "%s", getenv("USER") ? getenv("USER") : "unknown");
      prop2.name      = SmUserID;
      prop2.type      = SmARRAY8;
      prop2.num_vals  = 1;
      prop2.vals      = &prop2val;
      prop2val.value  = (SmPointer) userId;
      prop2val.length = strlen (userId);

      prop3.name      = SmRestartStyleHint;
      prop3.type      = SmCARD8;
      prop3.num_vals  = 1;
      prop3.vals      = &prop3val;
      prop3val.value  = (SmPointer) &hint;
      prop3val.length = 1;

      snprintf (pid, 20, "%d", getpid ());
      prop4.name = SmProcessID;
      prop4.type = SmARRAY8;
      prop4.num_vals = 1;
      prop4.vals = &prop4val;
      prop4val.value = pid;
      prop4val.length = strlen (prop4val.value);

      /* Always start in home directory */
      prop5.name = SmCurrentDirectory;
      prop5.type = SmARRAY8;
      prop5.num_vals = 1;
      prop5.vals = &prop5val;
      prop5val.value = (char*) ( getenv("HOME") ? getenv("HOME") : "unknown" );
      prop5val.length = strlen (prop5val.value);

      prop6.name = "_GSM_Priority";
      prop6.type = SmCARD8;
      prop6.num_vals = 1;
      prop6.vals = &prop6val;
      prop6val.value = &priority;
      prop6val.length = 1;

      props[0] = &prop1;
      props[1] = &prop2;
      props[2] = &prop3;
      props[3] = &prop4;
      props[4] = &prop5;
      props[5] = &prop6;

      SmcSetProperties (smcConn, 6, props);

      first_time = 0;
    }

  /* Below is obviously wrong and needs work.
   * Need to rebuild command line arguments
  */

  prop1.name     = SmRestartCommand;
  prop1.type     = SmLISTofARRAY8;
  prop1.vals     = &prop1val;
  prop1.num_vals = 1;
  prop1val.value = (SmPointer) "matchbox-window-manager" ;
  prop1val.length = strlen ("matchbox-window-manager");

  props[0] = &prop1;

  SmcSetProperties (smcConn, 1, props);

  SmcSaveYourselfDone (smcConn, True);
}


static void
sm_die_cb (SmcConn   smcConn,
	   SmPointer clientData)
{
  Wm *w = (Wm*)clientData;

  dbg("%s() session mark\n", __func__);

  SmcCloseConnection (smcConn, 0, NULL);

  w->sm_ice_fd = -1;
}

static void
sm_save_complete_cb (SmcConn   smcConn,
		     SmPointer clientData)
{
  dbg("%s() session mark\n", __func__);

}

static void
sm_shutdown_cancelled_cb (SmcConn   smcConn,
			  SmPointer clientData)
{
  dbg("%s() session mark\n", __func__);
  /*
  if (!sent_save_done)
    {
      SmcSaveYourselfDone (smcConn, False);
      sent_save_done = 1;
    }
  */
}

void
sm_process_event(Wm *w)
{
  dbg("%s() session mark\n", __func__);

  IceProcessMessages (w->ice_conn, NULL, NULL);
}

Bool
sm_connect(Wm *w)
{
  char          error[256], *mb_client_id;
  unsigned long mask;
  SmcCallbacks  callbacks;
  SmcConn       smc_conn = NULL;

  mask = SmcSaveYourselfProcMask | SmcDieProcMask |
    SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask;

  callbacks.save_yourself.callback    = sm_save_yourself_cb;
  callbacks.save_yourself.client_data = (SmPointer)w;

  callbacks.die.callback    = sm_die_cb;
  callbacks.die.client_data = (SmPointer)w;

  callbacks.save_complete.callback = sm_save_complete_cb;
  callbacks.save_complete.client_data = (SmPointer)w;
    
  callbacks.shutdown_cancelled.callback = sm_shutdown_cancelled_cb;
  callbacks.shutdown_cancelled.client_data = (SmPointer) w;

  w->sm_ice_fd = -1;

  smc_conn = SmcOpenConnection ( NULL,   /* use SESSION_MANAGER env */
				 NULL,
				 SmProtoMajor,
				 SmProtoMinor,
				 mask,
				 &callbacks,
				 w->config->sm_client_id,
				 &mb_client_id,
				 256, error);

    if (smc_conn == NULL)
      {
	fprintf(stderr, "matchbox-window-manager: Failed to connect to session manager\n");
	return False;
      }

    w->ice_conn = SmcGetIceConnection (smc_conn);

    w->sm_ice_fd = IceConnectionNumber (w->ice_conn);

    dbg("connected to session manager\n");

    return True;
}

#endif

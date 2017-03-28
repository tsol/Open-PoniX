#include <pjsua-lib/pjsua.h>
#include <pjmedia/sound.h>

#define THIS_FILE	"thinpj_app.c"
#define NO_LIMIT	(int)0x7FFFFFFF

extern "C"
{
#include "tcpserv.h"
}

#include "thash.h"

#define	THINPJ_VERSION	"1.42"

#define NULL_SND_DEV_ID         -99  /* same as pjsua-lib/pjmedia.c */

#define OFFHOOK_FREQ	438
#define DTMF_CNT		10
#define DTMF_ON			20

#define BUSY_FREQ1	    438
#define BUSY_ON		    500
#define BUSY_OFF	    200
#define BUSY_CNT	    3

/* Ringtones		    US	       UK  */
#define RINGBACK_FREQ1	    440	    /* 400 */
#define RINGBACK_FREQ2	    480	    /* 450 */
#define RINGBACK_ON	    2000    /* 400 */
#define RINGBACK_OFF	    4000    /* 200 */
#define RINGBACK_CNT	    1	    /* 2   */
#define RINGBACK_INTERVAL   4000    /* 2000 */

#define MAX_DIALED_CHARS				16
#define DIAL_INACTIVITY_DELAY			2 // secs
#define DIAL_HANGUP_INACTIVITY_TIMES	3
#define MIN_CHARS_TO_DIAL				3

#define MAX_URL_SIZE					80

#define RING_FILE "/thinpj/ringin.wav"

#define HOOK_NOSOUND	0x1		// for hook up/down functions. Don't start sound on offhook
#define HOOK_NOSTATE	0x2		// do not send cstate message to GUI part

#define	XFER_RETRY_TIME			10	// seconds
#define XFER_TIMEOUT			60  // seconds

/* Call specific data */
struct call_data
{
    pj_bool_t				ring_on;
    pj_bool_t				ringback_on;

	pjsua_call_id			call_id; // self number used as user_data on make_call

	char					xfer_want_back; // back to operator
	char					xfer_to[MAX_URL_SIZE];
    pj_timer_entry			xfer_total_timer;
	pj_timer_entry			xfer_retry_timer;

};


/* Pjsua application data */
static struct app_config
{
    pjsua_config			cfg;
    pjsua_logging_config    log_cfg;
    pjsua_media_config	    media_cfg;

	pjsua_transport_config  udp_cfg;
    pjsua_transport_config  rtp_cfg;

    struct call_data	    call_data[PJSUA_MAX_CALLS];

    pj_pool_t		   *pool;
    /* Compatibility with older pjsua */

    pjmedia_port    	    *dtmf_media_port;
    pjsua_conf_port_id	    dtmf_conf_port;

    pjmedia_port			*ring_wav_file_media_port;
	pjmedia_snd_port		*ring_wav_snd_port;

/*
	pjsua_player_id			ring_wav_id;
    pjsua_conf_port_id	    ring_wav_port;
*/
    float		    mic_level,
			    speaker_level;

    int			    capture_dev, playback_dev, ring_dev;
    unsigned		capture_lat, playback_lat;

    int			    ring_cnt;

    int			    ringback_slot;
    int			    ringback_cnt;
    pjmedia_port	*ringback_port;


// my
	int				xfer_retry_time;
	int				xfer_timeout;
	pj_bool_t		smart_transfer;

	int			    offhook_slot;
    int			    offhook_cnt;
    pjmedia_port	*offhook_port;
    pj_bool_t		offhook;

	char 			dialed_chars[MAX_DIALED_CHARS+1];
    int				dc_index;

    char			dialed_chars_postfix[80];
    char			dialed_chars_prefix[80];

    pj_timer_entry		dial_inactivity_timer;
    int					dial_inactivity_count;

} app_config;

//static pjsua_acc_id	current_acc;
#define current_acc		pjsua_acc_get_default()

static pjsua_call_id	current_call = PJSUA_INVALID_ID;
static pjsua_call_id	incoming_call = PJSUA_INVALID_ID;
static pjsua_call_id	hold_call = PJSUA_INVALID_ID;

static char some_buf[2048];


pj_status_t app_destroy(void);

static void ringback_start(pjsua_call_id call_id);
static void ring_start(pjsua_call_id call_id);
static void ring_stop(pjsua_call_id call_id);

static void busy_sound();

static void cmd_hook_down(char silent);
static void cmd_hook_up(char silent);

static void prn_cstate();

static void xfer_retry_timer_callback(pj_timer_heap_t *timer_heap, struct pj_timer_entry *entry);
static void xfer_total_timer_callback(pj_timer_heap_t *timer_heap, struct pj_timer_entry *entry);
static void xfer_cleanup_timers(pjsua_call_id xfer_call);
static void xfer_cancel_outgoing_call_for(pjsua_call_id xfer_call);

pj_str_t gen_url(char *dst, int size, char *num);
static char inject_holded_call_as_incoming();

/*****************************************************************************
 * Configuration manipulation
 */

/* Set default config. */
static void default_config(struct app_config *cfg)
{
    char tmp[80];
	int i;

    pjsua_config_default(&cfg->cfg);

    pj_ansi_sprintf(tmp, "THINPJ v%s/%s/%s", THINPJ_VERSION, pj_get_version(), PJ_OS_NAME);
    pj_strdup2_with_null(app_config.pool, &cfg->cfg.user_agent, tmp);

    pjsua_logging_config_default(&cfg->log_cfg);
    pjsua_media_config_default(&cfg->media_cfg);
    pjsua_transport_config_default(&cfg->udp_cfg);
    cfg->udp_cfg.port = 5060;
    pjsua_transport_config_default(&cfg->rtp_cfg);
    cfg->rtp_cfg.port = 4000;

    cfg->ringback_slot = PJSUA_INVALID_ID;
/*
	cfg->ring_wav_id = PJSUA_INVALID_ID;
	cfg->ring_wav_port = PJSUA_INVALID_ID;
*/
    cfg->mic_level = cfg->speaker_level = 1.0;
    cfg->capture_dev = PJSUA_INVALID_ID;
    cfg->playback_dev = PJSUA_INVALID_ID;
	cfg->ring_dev = PJSUA_INVALID_ID;
	cfg->capture_lat = PJMEDIA_SND_DEFAULT_REC_LATENCY;
    cfg->playback_lat = PJMEDIA_SND_DEFAULT_PLAY_LATENCY;


	cfg->offhook_slot = PJSUA_INVALID_ID;
    cfg->dtmf_conf_port = PJSUA_INVALID_ID;

	for (i=0; i<PJSUA_MAX_CALLS; i++)
	{
		cfg->call_data[i].call_id = i;

		cfg->call_data[i].xfer_total_timer.id = PJSUA_INVALID_ID;
		pj_timer_entry_init(&cfg->call_data[i].xfer_total_timer, 0, NULL, &xfer_total_timer_callback);

		cfg->call_data[i].xfer_retry_timer.id = PJSUA_INVALID_ID;
		pj_timer_entry_init(&cfg->call_data[i].xfer_retry_timer, 0, NULL, &xfer_retry_timer_callback);

		cfg->call_data[i].xfer_want_back = 0;
		memset(&cfg->call_data[i].xfer_to,0,MAX_URL_SIZE);
	}

// my config
	memset(cfg->dialed_chars_postfix,0,80);
	strcpy(cfg->dialed_chars_prefix,"sip:");

	cfg->xfer_retry_time = XFER_RETRY_TIME;
	cfg->xfer_timeout = XFER_TIMEOUT;
	cfg->smart_transfer = PJ_FALSE;

	cfg->log_cfg.level = 3;
	cfg->log_cfg.console_level = 3;

    cfg->media_cfg.no_vad = PJ_TRUE;
//    cfg->media_cfg.ec_tail_len = 0;
//	  cfg->media_cfg.ec_options = PJMEDIA_ECHO_SPEEX;
	
}


static void ringback_start(pjsua_call_id call_id)
{    
    if (app_config.call_data[call_id].ringback_on)
	return;

    app_config.call_data[call_id].ringback_on = PJ_TRUE;

    if (++app_config.ringback_cnt==1 && 
	app_config.ringback_slot!=PJSUA_INVALID_ID) 
    {
	pjsua_conf_connect(app_config.ringback_slot, 0);
    }
}

static void ring_stop(pjsua_call_id call_id)
{
    pj_status_t status;

	if (app_config.call_data[call_id].ringback_on) {
	app_config.call_data[call_id].ringback_on = PJ_FALSE;

	pj_assert(app_config.ringback_cnt>0);
	if (--app_config.ringback_cnt == 0 && 
	    app_config.ringback_slot!=PJSUA_INVALID_ID) 
	{
	    pjsua_conf_disconnect(app_config.ringback_slot, 0);
	    pjmedia_tonegen_rewind(app_config.ringback_port);
	}
    }
	
	if (! app_config.call_data[call_id].ring_on)
		{ return ; }

	app_config.call_data[call_id].ring_on = PJ_FALSE;

	pj_assert(app_config.ring_cnt>0);

	if (--app_config.ring_cnt == 0 ) {
		status = pjmedia_snd_port_disconnect(app_config.ring_wav_snd_port);
		pj_assert(status == PJ_SUCCESS);
	}

	/*
	if (--app_config.ring_cnt == 0 && 
	    app_config.ring_wav_port!=PJSUA_INVALID_ID) 
	{
		pjsua_conf_disconnect(app_config.ring_wav_port, 0);
	}
*/
}

static void ring_start(pjsua_call_id call_id)
{
    pj_status_t status;

    if (app_config.call_data[call_id].ring_on)
	return;

    app_config.call_data[call_id].ring_on = PJ_TRUE;

    if (++app_config.ring_cnt==1 )
    {
		pjmedia_wav_player_port_set_pos(app_config.ring_wav_file_media_port,0);
		status = pjmedia_snd_port_connect( app_config.ring_wav_snd_port, app_config.ring_wav_file_media_port);
		pj_assert(status == PJ_SUCCESS);
	}
}

/*
 * Handler when invite state has changed.
 */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info	call_info;

	pj_time_val		delay;
	struct call_data		*cd;
	pjsua_call_id	*p_xfer_call = (pjsua_call_id*)(pjsua_call_get_user_data(call_id));

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &call_info);

    if (call_info.state == PJSIP_INV_STATE_DISCONNECTED)
	{
		/* Stop all ringback for this call */
		ring_stop(call_id);

		PJ_LOG(3,(THIS_FILE, "Call %d is DISCONNECTED [reason=%d (%s)]", 
		  call_id,
		  call_info.last_status,
		  call_info.last_status_text.ptr));

		if (call_id == current_call) {

			if (
				(app_config.offhook) &&
				(call_info.last_status != 200)  &&
				(call_info.last_status != 410)
			)
				{ busy_sound();}

			cmd_hook_down(0);
		}
		else if (call_id == incoming_call)
		{
			incoming_call = PJSUA_INVALID_ID;
		}
		else if (call_id == hold_call)
		{
			hold_call = PJSUA_INVALID_ID;
		}
		else   // this call has probably something to do with transfer
		{      // might be a call waiting transfer or a outgoing call to transferee

			cd = (call_data*)&app_config.call_data[call_id];			

			if (cd->xfer_to[0] != '\0') 			// check if it was call waiting xfer
			{
				PJ_LOG(3,(THIS_FILE, "Call %d was call waiting transfer. Canceling outgoing xfer calls and timers.", call_id));
				xfer_cleanup_timers(call_id);
				xfer_cancel_outgoing_call_for(call_id);

			} else			
			if (p_xfer_call != NULL) // this was transfer outgoing call
			{
				PJ_LOG(3,(THIS_FILE, "Call %d was outgoing tranfer call for call %d, scheduling next attempt", call_id, *p_xfer_call));

				// schedule next attempt
				cd = (call_data*)&app_config.call_data[*p_xfer_call];

				if (cd->xfer_to[0] != '\0') // check if call still needed to be transfered
				{
					delay.sec = app_config.xfer_retry_time;
					delay.msec = 0;

					// stop retry timer	
					if (cd->xfer_retry_timer.id != PJSUA_INVALID_ID)
					{
						pjsip_endpt_cancel_timer(pjsua_get_pjsip_endpt(), &cd->xfer_retry_timer);
						cd->xfer_retry_timer.id = PJSUA_INVALID_ID;
					}

					// start retry timer
					cd->xfer_retry_timer.id = *p_xfer_call;
					pjsip_endpt_schedule_timer(pjsua_get_pjsip_endpt(), &cd->xfer_retry_timer, &delay);
				}


			}


		}
	                                                      
		tcpserv_pf( "THINPJ act='callover', callid='%d', reason='%d', status='%s'\n\r",
	                call_id, call_info.last_status, call_info.last_status_text.ptr );

		prn_cstate();
		
/*
		if (1) {
			pjsua_call_dump(call_id, PJ_TRUE, some_buf, 
				    sizeof(some_buf), "  ");
			PJ_LOG(5,(THIS_FILE, 
				  "Call %d disconnected, dumping media stats\n%s", 
				  call_id, some_buf));
		}
*/		
		return;
    }

	if (call_info.state == PJSIP_INV_STATE_EARLY)
	{
	    int code;
	    pj_str_t reason;
	    pjsip_msg *msg;

	    /* This can only occur because of TX or RX message */
	    pj_assert(e->type == PJSIP_EVENT_TSX_STATE);

	    if (e->body.tsx_state.type == PJSIP_EVENT_RX_MSG) {
		msg = e->body.tsx_state.src.rdata->msg_info.msg;
	    } else {
		msg = e->body.tsx_state.src.tdata->msg;
	    }

	    code = msg->line.status.code;
	    reason = msg->line.status.reason;

		if (call_id == current_call) {
			/* Start ringback for 180 for UAC unless there's SDP in 180 */
			if (call_info.role==PJSIP_ROLE_UAC && code==180 && 
			msg->body == NULL && 
			call_info.media_status==PJSUA_CALL_MEDIA_NONE) 
			{
				ringback_start(call_id);
			}
		}

	    PJ_LOG(3,(THIS_FILE, "Call %d state changed to %s (%d %.*s)", 
		      call_id, call_info.state_text.ptr,
		      code, (int)reason.slen, reason.ptr));

		return;
	}

 //	By this point we might get CALLING, CONNECTING, CONFIRMED states

	PJ_LOG(3,(THIS_FILE, "Call %d state changed to %s", 
		      call_id,
		      call_info.state_text.ptr));

	if (p_xfer_call != NULL)	// this call_id is outgoing transfer for p_xfer_call
	{
		if (call_info.state == PJSIP_INV_STATE_CONFIRMED) // and it got connected!
		{
			PJ_LOG(3,(THIS_FILE, "Call %d was outgoing transfer call for call %d. Doing xfer with replaces!", call_id, *p_xfer_call));

				// cancel transfer process timers and variables
				xfer_cleanup_timers(*p_xfer_call);
				pjsua_call_set_user_data(call_id, NULL); // unbind xfer_call from outgoing call

				if ( pjsua_call_is_active ( *p_xfer_call ) ) // check if xfer_call still waits
				{
					// perform xfer with replace
					pjsua_msg_data	msg_data;
					pjsua_msg_data_init(&msg_data);
					pjsua_call_xfer_replaces(call_id, *p_xfer_call, PJSUA_XFER_NO_REQUIRE_REPLACES, &msg_data);
				}
				else
				{
					PJ_LOG(3,(THIS_FILE, "Call %d: xfer_call %d was no longer active!", call_id, *p_xfer_call));
					pjsua_call_hangup(call_id, PJSIP_SC_GONE, NULL, NULL);
				}
		}

	}
	else if (current_call == PJSUA_INVALID_ID)
	{
			current_call = call_id;
			prn_cstate();
	}


}


/**
 * Handler when there is incoming call.
 */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
			     pjsip_rx_data *rdata)
{
    pjsua_call_info call_info;

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &call_info);

//  if (current_call == PJSUA_INVALID_ID)
    if (!app_config.offhook && (incoming_call==-1))
    {
	/* Start ringing */
		ring_start(call_id);
		incoming_call = call_id;
    }
    else
    {
		pjsua_call_answer(call_id, 486, NULL, NULL); // busy
    }

    tcpserv_pf("THINPJ act='call', account='%d', callid='%d', from='%s'\n\r",
		  acc_id,
		  call_id,
		  call_info.remote_info.ptr);
	
	prn_cstate();
	
}


/*
 * Handler when a transaction within a call has changed state.
 */
static void on_call_tsx_state(pjsua_call_id call_id,
			      pjsip_transaction *tsx,
			      pjsip_event *e)
{
    const pjsip_method info_method = 
    {
	PJSIP_OTHER_METHOD,
	{ "INFO", 4 }
    };

    if (pjsip_method_cmp(&tsx->method, &info_method)==0) {
	/*
	 * Handle INFO method.
	 */
	if (tsx->role == PJSIP_ROLE_UAC && 
	    (tsx->state == PJSIP_TSX_STATE_COMPLETED ||
	       (tsx->state == PJSIP_TSX_STATE_TERMINATED &&
	        e->body.tsx_state.prev_state != PJSIP_TSX_STATE_COMPLETED))) 
	{
	    /* Status of outgoing INFO request */
	    if (tsx->status_code >= 200 && tsx->status_code < 300) {
		PJ_LOG(4,(THIS_FILE, 
			  "Call %d: DTMF sent successfully with INFO",
			  call_id));
	    } else if (tsx->status_code >= 300) {
		PJ_LOG(4,(THIS_FILE, 
			  "Call %d: Failed to send DTMF with INFO: %d/%.*s",
			  call_id,
		          tsx->status_code,
			  (int)tsx->status_text.slen,
			  tsx->status_text.ptr));
	    }
	} else if (tsx->role == PJSIP_ROLE_UAS &&
		   tsx->state == PJSIP_TSX_STATE_TRYING)
	{
	    /* Answer incoming INFO with 200/OK */
	    pjsip_rx_data *rdata;
	    pjsip_tx_data *tdata;
	    pj_status_t status;

	    rdata = e->body.tsx_state.src.rdata;

	    if (rdata->msg_info.msg->body) {
		status = pjsip_endpt_create_response(tsx->endpt, rdata,
						     200, NULL, &tdata);

               if (status == PJ_SUCCESS)
                                   status = pjsip_tsx_send_msg(tsx, tdata);
                                   
		PJ_LOG(3,(THIS_FILE, "Call %d: incoming INFO:\n%.*s", 
			  call_id,
			  (int)rdata->msg_info.msg->body->len,
			  rdata->msg_info.msg->body->data));
	    } else {
		status = pjsip_endpt_create_response(tsx->endpt, rdata,
						     400, NULL, &tdata);
		if (status == PJ_SUCCESS)
		    status = pjsip_tsx_send_msg(tsx, tdata);
	    }
	}
    }
}


/*
 * Callback on media state changed event.
 * The action may connect the call to sound device, to file, or
 * to loop the call.
 */
static void on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info call_info;
    pjsua_call_get_info(call_id, &call_info);
	
	// check if the call is outgoing transfer
	pjsua_call_id	*p_xfer_call = (pjsua_call_id*)(pjsua_call_get_user_data(call_id));
	if (p_xfer_call != NULL)
	{
	    PJ_LOG(3,(THIS_FILE, "Media for call %d changed, but it was OUTGOING-XFER. Ignoring.", call_id));
		return;
	}

	// check if it is a transferee
	if (app_config.call_data[call_id].xfer_to[0] != '\0')
	{
	    PJ_LOG(3,(THIS_FILE, "Media for call %d changed, but it was TRANSFEREE. Ignoring.", call_id));
		return;
	}

    ring_stop(call_id);

    if (call_info.media_status == PJSUA_CALL_MEDIA_ACTIVE) {

	    PJ_LOG(3,(THIS_FILE, "Media for call %d is active", call_id));

		pjsua_conf_connect(call_info.conf_slot, 0);
	    pjsua_conf_connect(0, call_info.conf_slot);


    } else if (call_info.media_status == PJSUA_CALL_MEDIA_LOCAL_HOLD) {

		PJ_LOG(3,(THIS_FILE, "Media for call %d is suspended (hold) by local", call_id));

	} else if (call_info.media_status == PJSUA_CALL_MEDIA_REMOTE_HOLD) {

		PJ_LOG(3,(THIS_FILE,  "Media for call %d is suspended (hold) by remote", call_id));

    } else if (call_info.media_status == PJSUA_CALL_MEDIA_ERROR) {
	
		pj_str_t reason = pj_str("ICE negotiation failed");

		PJ_LOG(1,(THIS_FILE, "Media has reported error, disconnecting call"));

		pjsua_call_hangup(call_id, 500, &reason, NULL);

    } else {
		PJ_LOG(3,(THIS_FILE,  "Media for call %d is inactive", call_id));
    }
}

/*
 * DTMF callback.
 */
static void call_on_dtmf_callback(pjsua_call_id call_id, int dtmf)
{
    PJ_LOG(3,(THIS_FILE, "Incoming DTMF on call %d: %c", call_id, dtmf));
}


/**
 * Call transfer request status.
 */
static void on_call_transfer_status(pjsua_call_id call_id,
				    int status_code,
				    const pj_str_t *status_text,
				    pj_bool_t final,
				    pj_bool_t *p_cont)
{
    PJ_LOG(3,(THIS_FILE, "Call %d: transfer status=%d (%.*s) %s",
	      call_id, status_code,
	      (int)status_text->slen, status_text->ptr,
	      (final ? "[final]" : "")));

    if (status_code/100 == 2) {
	PJ_LOG(3,(THIS_FILE, 
	          "Call %d: call transfered successfully, disconnecting call",
		  call_id));
	pjsua_call_hangup(call_id, PJSIP_SC_GONE, NULL, NULL);
	*p_cont = PJ_FALSE;
    }
}




static void offhook_sound_on()
{
			if (app_config.offhook_cnt==0 &&
				app_config.offhook_slot!=PJSUA_INVALID_ID)
			{
				pjsua_conf_connect(app_config.offhook_slot, 0);
                                app_config.offhook_cnt=1;
			}		       
}


static void offhook_sound_off()
{
			if (app_config.offhook_slot!=PJSUA_INVALID_ID &&
				app_config.offhook_cnt == 1)
			{
				pjsua_conf_disconnect(app_config.offhook_slot, 0);
				pjmedia_tonegen_rewind(app_config.offhook_port); // no need cause it's same and looped
				app_config.offhook_cnt = 0;
			}
}

static void digit_sound(char d)
{
    pj_status_t status;
    pjmedia_tone_digit digits[1];
    digits[0].digit = d;
    digits[0].on_msec = 200;
    pjmedia_tonegen_stop(app_config.dtmf_media_port);
    status = pjmedia_tonegen_play_digits(app_config.dtmf_media_port, 1, digits, 0);
    pj_assert(status == PJ_SUCCESS);
}

static void busy_sound()
{
    int i;
    pjmedia_tone_desc tone[BUSY_CNT];
      
    pj_bzero(&tone, sizeof(tone));
    for (i=0; i<BUSY_CNT; ++i) {
          tone[i].freq1 = BUSY_FREQ1;
          tone[i].on_msec = BUSY_ON;
          tone[i].off_msec = BUSY_OFF;
   }
                                                                                      
    pjmedia_tonegen_stop(app_config.dtmf_media_port);
    pjmedia_tonegen_play(app_config.dtmf_media_port, BUSY_CNT, tone, 0);
                                                                                                                           
}


static void dial_inactivity_timer_stop()
{
      /* Cancel timer, if any */
      if (app_config.dial_inactivity_timer.id != PJSUA_INVALID_ID)
      {
          app_config.dial_inactivity_timer.id = PJSUA_INVALID_ID;
          pjsip_endpt_cancel_timer(pjsua_get_pjsip_endpt(), &app_config.dial_inactivity_timer);
      }
}


static void dial_inactivity_timer_restart()
{
      pj_time_val delay;

	  dial_inactivity_timer_stop();

      /* start a new one */
  
      app_config.dial_inactivity_timer.id = 1;
      delay.sec = DIAL_INACTIVITY_DELAY;
      delay.msec = 0;

      pjsip_endpt_schedule_timer(pjsua_get_pjsip_endpt(), &app_config.dial_inactivity_timer, &delay);
                                                                                                
}

static void prn_cstate()
{
	pjsua_call_info call_info;

	char s_cur[256];
	char s_hld[256];
	char s_inc[256];
	
	memset(&s_cur,0,256);
	memset(&s_hld,0,256);
	memset(&s_inc,0,256);

	long cur_duration = 0;
	
	if (current_call != PJSUA_INVALID_ID)
	{
		pjsua_call_get_info(current_call, &call_info);
		strcpy((char*)&s_cur, call_info.remote_info.ptr);
		cur_duration = call_info.connect_duration.sec;
	}

	if (hold_call != PJSUA_INVALID_ID)
	{
		pjsua_call_get_info(hold_call, &call_info);
		strcpy((char*)&s_hld, call_info.remote_info.ptr);
	}

	if (incoming_call != PJSUA_INVALID_ID)
	{
		pjsua_call_get_info(incoming_call, &call_info);
		strcpy((char*)&s_inc, call_info.remote_info.ptr);
	}

	tcpserv_pf("THINPJ act='cstate', hook='%s', cur='%s', hld='%s', inc='%s' cur_d='%d'\n\r",
			(app_config.offhook ? "up" : "down"),
			(char*)&s_cur,
			(char*)&s_hld,
			(char*)&s_inc,
			cur_duration
	);
}

static void cmd_hook_down(char silent)
{
	if (current_call != -1)
	{
		pjsua_call_hangup(current_call, 0, NULL, NULL);
		current_call = -1;
	}

	if (incoming_call != -1)
	{
		pjsua_call_hangup(incoming_call, 0, NULL, NULL);
		incoming_call = -1;
	}

	if ( app_config.offhook )
	{
		app_config.offhook = PJ_FALSE;
		offhook_sound_off();
		dial_inactivity_timer_stop();

		if (! (silent & HOOK_NOSTATE) )
		{
			inject_holded_call_as_incoming();
			prn_cstate();				
		}
	}

}

static void cmd_hook_up(char silent)
{
	pj_status_t status;

	if (! app_config.offhook)
	{
		app_config.offhook = PJ_TRUE;

		if ((current_call==-1)&&(incoming_call != -1)) // answer incomming call
			{
				pjsua_msg_data msg_data;
				pjsua_msg_data_init(&msg_data);

				if (app_config.call_data[incoming_call].xfer_want_back==2) // ringing from hold
				{
					app_config.call_data[incoming_call].xfer_want_back = 0;
					ring_stop(incoming_call);
					status = pjsua_call_reinvite(incoming_call, PJ_TRUE, NULL);
				}
				else
				{
					status = pjsua_call_answer(incoming_call, 200, NULL, &msg_data);
				}
                      
				current_call = incoming_call;
				incoming_call = PJSUA_INVALID_ID;      

				tcpserv_pf("THINPJ act='answer', res='%s'\n\r",(status==PJ_SUCCESS ? "ok" : "fail"));

			}
			else if (! (silent & HOOK_NOSOUND) ) 	// just lift up handset
			{
				pj_bzero(&app_config.dialed_chars, MAX_DIALED_CHARS);
				app_config.dc_index = 0;
		  
				offhook_sound_on();
				dial_inactivity_timer_restart();
				app_config.dial_inactivity_count = 0;
			}

			if (! (silent & HOOK_NOSTATE) )
				{ prn_cstate(); }
			
	}	  

}

// inject call waiting

static char inject_holded_call_as_incoming()
{
	pjsua_call_info call_info;

	if ( (current_call == -1)&&(incoming_call == -1))
	{
		pjsua_call_id ids[PJSUA_MAX_CALLS];
		unsigned int maxids = PJSUA_MAX_CALLS;
		pjsua_enum_calls(ids,&maxids);

		for (unsigned int i=0;i<maxids;i++)
		{
			if (app_config.call_data[ids[i]].xfer_want_back == 1)
			{
				incoming_call = ids[i];
				app_config.call_data[ids[i]].xfer_want_back = 2;

				pjsua_call_get_info(incoming_call, &call_info);
				ring_start(incoming_call);

				tcpserv_pf("THINPJ act='call', callid='%d', from='%s', status='Back from xfer.'\n\r",
					incoming_call,
					call_info.remote_info.ptr);
	
				return 1;
			}
		}

	}

	return 0;
}

static void xfer_cancel_outgoing_call_for(pjsua_call_id xfer_call)
{
	pjsua_call_id	*p_xfer_call;
	pjsua_call_id ids[PJSUA_MAX_CALLS];
	unsigned int maxids = PJSUA_MAX_CALLS;

	pjsua_enum_calls(ids,&maxids);

	for (unsigned int i=0;i<maxids;i++)
	{
		p_xfer_call = (pjsua_call_id*)(pjsua_call_get_user_data(ids[i]));

		if (p_xfer_call != NULL)
		{
			if (*p_xfer_call == xfer_call)
			{
				pjsua_call_set_user_data(ids[i],NULL);
				PJ_LOG(3,(THIS_FILE, "Found outgoing-xfer for %d it is %d. Hanging up.",xfer_call,ids[i]));
				pjsua_call_hangup(ids[i], 200, NULL, NULL);
				break;
			}
		}
	}

}



static void xfer_cleanup_timers(pjsua_call_id xfer_call)
{
	if (xfer_call == PJSUA_INVALID_ID) // WTF! Shouldn't Be.
		{ return; }

	call_data *cd = (call_data*)&app_config.call_data[xfer_call];

	// stop retry timer	
	if (cd->xfer_retry_timer.id != PJSUA_INVALID_ID)
	{
		pjsip_endpt_cancel_timer(pjsua_get_pjsip_endpt(), &cd->xfer_retry_timer);
		cd->xfer_retry_timer.id = PJSUA_INVALID_ID;
	}

	// stop total timer	
	if (cd->xfer_total_timer.id != PJSUA_INVALID_ID)
	{
		pjsip_endpt_cancel_timer(pjsua_get_pjsip_endpt(), &cd->xfer_total_timer);
		cd->xfer_total_timer.id = PJSUA_INVALID_ID;
	}

	// clear xfer_to string
	memset(&cd->xfer_to,0,MAX_URL_SIZE);

}

// xfer total timer
static void xfer_total_timer_callback(pj_timer_heap_t *timer_heap, struct pj_timer_entry *entry)
{
    PJ_UNUSED_ARG(timer_heap);

	pjsua_call_id xfer_call = entry->id;
    PJ_LOG(3,(THIS_FILE, "Xfer_timer_total_callback: call %d",xfer_call));

	if (xfer_call == PJSUA_INVALID_ID) // WTF! Shouldn't Be.
		{ return; }

	xfer_cleanup_timers(xfer_call);
	xfer_cancel_outgoing_call_for(xfer_call);

	// queue or imm. ring to operator
	app_config.call_data[xfer_call].xfer_want_back = 1;
	if (inject_holded_call_as_incoming())
	{
		prn_cstate();
	}

}

// xfer retry timer
static void xfer_retry_timer_callback(pj_timer_heap_t *timer_heap, struct pj_timer_entry *entry)
{
	pjsua_call_id xfer_call = entry->id;
	call_data	*cd = (call_data*)&app_config.call_data[xfer_call];
	pj_str_t	pj_url_to = pj_str((char*)&cd->xfer_to);

	PJ_LOG(3,(THIS_FILE, "Xfer_timer_retry_callback: call %d",xfer_call));

	PJ_UNUSED_ARG(timer_heap);

	if ( pjsua_call_make_call( current_acc, &pj_url_to, 0,
		(void*)&cd->call_id, NULL, NULL) != PJ_SUCCESS )
	{
		PJ_LOG(3,(THIS_FILE, "Transfer dial attempt failed on make_call"));
		return;
	}
  
}


static void dial_with_dialed_chars()
{
          char url[MAX_URL_SIZE];
          pj_str_t uri_to_call;

		  uri_to_call = gen_url((char*)&url,MAX_URL_SIZE,app_config.dialed_chars);
          
          // makecall goes here                        
          tcpserv_pf("THINPJ act='dial', data='%s'\n\r",url);

          offhook_sound_off();
          dial_inactivity_timer_stop();

                
          if ( pjsua_call_make_call( current_acc, &uri_to_call, 0, NULL, NULL, NULL) == PJ_SUCCESS ) {
			tcpserv_puts("THINPJ act='makecall', res='ok'");
          } else {
			tcpserv_puts("THINPJ act='makecall', res='fail'");
			cmd_hook_down(0);
          }
}

/* Auto hangup timer callback */
static void dial_inactivity_callback(pj_timer_heap_t *timer_heap, struct pj_timer_entry *entry)
{
    PJ_UNUSED_ARG(timer_heap);
    PJ_UNUSED_ARG(entry);

    app_config.dial_inactivity_timer.id = PJSUA_INVALID_ID;

    if (current_call != -1)
    {
        tcpserv_puts("THINPJ act='unexpected', status='Inactivity callback while call is active!'");
        return;
    };

    if (app_config.dial_inactivity_count++ == DIAL_HANGUP_INACTIVITY_TIMES)
    {
		cmd_hook_down(0);
		app_config.dial_inactivity_count = 0;
		return;
    }

    if (app_config.dc_index >= MIN_CHARS_TO_DIAL)
    {        
        dial_with_dialed_chars();    
        return;
    }

    dial_inactivity_timer_restart();
    
}


int cmd_dialdigit(char d, char bulk_mode)
{
	pj_str_t pj_digit;
    pj_status_t status;

	if (! app_config.offhook)
	{
		tcpserv_puts("THINPJ act='digit', res='fail', status='Onhook now!' ");
		return -1;
	}

	if ( ! ( ((d>='0')&&(d<='9'))||(d=='*')||(d=='#') ) )
	{
		tcpserv_puts("THINPJ act='digit', res='fail', status='Invalid digit'");
		return -2;
	}

	if (! bulk_mode)
	{
		offhook_sound_off();
		digit_sound(d);
	}

	if (current_call == -1) // No current call - means we wanna dial somewhere not send dtmf
	{
		if (! bulk_mode)
		{
			dial_inactivity_timer_restart();
			app_config.dial_inactivity_count = 0;

			if (d == '#')
			{
				dial_with_dialed_chars();
				return 1;
			}
		}

		if (app_config.dc_index>=MAX_DIALED_CHARS)
		{
			tcpserv_puts("THINPJ act='digit', res='fail', status='Number too long.'");
			return -3;
		}

		app_config.dialed_chars[app_config.dc_index++] = d;
		
		if (! bulk_mode)
		{
			tcpserv_puts("THINPJ act='digit', res='ok', status='Stored.'");
		}
                
		return 1;
	}
                    
	// have call, sending dtfm to remote party

	if (pjsua_call_has_media(current_call))
	{
		char digit[2];
		digit[0] = d; digit[1] = '\0';                        
		pj_digit = pj_str(digit);
                    
		status = pjsua_call_dial_dtmf(current_call, &pj_digit);

		if (status == PJ_SUCCESS) {
			if (! bulk_mode)
				{ tcpserv_puts("THINPJ act='digit', res='ok', status='Sent to remote party.'"); }
			return 3;
		}

	}

	tcpserv_puts("THINPJ act='digit', res='fail', status='Failed sending to remote party!'");
	return -4;
}

int cmd_account(char *a)
{
        int rc;
        pjsua_acc_id id = (pjsua_acc_id)atoi(a);
        if ( (id >= 0) && (id < PJSUA_MAX_ACC) ) {
                PJ_LOG(3,(THIS_FILE, "Current account change to %d", id));
                return pjsua_acc_set_default(id);                
        }

        PJ_LOG(3,(THIS_FILE, "FAILED Current account change to %d", id));
        
        return -1;        
}

int cmd_dialnumber(char *n)
{
	int rc;

	if (current_call == -1)
	{
		cmd_hook_down(0);
		cmd_hook_up(1);

		pj_bzero(&app_config.dialed_chars, MAX_DIALED_CHARS);
		app_config.dc_index = 0;
	}

	while (((*n>='0')&&(*n<='9'))||(*n=='*')||(*n=='#'))
	{
		rc = cmd_dialdigit(*n,1);
		if (rc <= 0)
		{
			cmd_hook_down(0);
			return rc;
		}
		n++;
	}
	
	if (current_call == -1)	
	{
		dial_with_dialed_chars();
	}

	return 1;
}

pj_str_t gen_url(char *dst, int size, char *num)
{
// todo: if n allreay a url - do nothing
	pj_ansi_snprintf(dst, size, "%s%s%s", app_config.dialed_chars_prefix,
                       num, app_config.dialed_chars_postfix);
	return pj_str(dst);
}


int cmd_transfer(char *n)
{
	pjsua_msg_data	msg_data;
	char			url_to[MAX_URL_SIZE];
	pj_str_t		pj_url_to;
	pjsua_call_id	xfer_call;
	pj_time_val		delay;

	pjsua_msg_data_init(&msg_data);

	if (*n == '\0') // no number specified. transfer holded call to current call
	{
		if ((hold_call == -1)||(current_call == -1))
		{
			tcpserv_puts("THINPJ act='transfer', res='fail'");
			return -1;
		}

		pjsua_call_xfer_replaces(current_call, hold_call, PJSUA_XFER_NO_REQUIRE_REPLACES, &msg_data);

		tcpserv_pf("THINPJ act='transfer', res='ok', to_callid='%d'\n\r",hold_call);
		return 1;
	}

	// transfer to specified number
    // if there's current call we transfer it, if no - we transfer holded call

	// check if src call exists
	if ((current_call == -1)&&(hold_call == -1))
	{
		PJ_LOG(3,(THIS_FILE, "No current or holded call"));
		tcpserv_puts("THINPJ act='transfer', res='fail', status='No current or holded call'");
		return -2;
	}

	if (app_config.smart_transfer == PJ_FALSE)
	{
		if (current_call == -1)
			{ xfer_call = hold_call; }
		else
			{ xfer_call = current_call; }

		// create url from prefixes and number
		pj_url_to = gen_url((char*)&url_to,MAX_URL_SIZE,n);
		pjsua_call_xfer( xfer_call, &pj_url_to, &msg_data);

		tcpserv_pf("THINPJ act='transfer', to='%s', res='ok', status='simple xfer attempt'\n\r",url_to);

	}
	else // smart_transfer=yes
	{

	// we do a guaranteed tranfer here. we set the call on hold, try to 
	// setup a successful connection with tranferee and then do a xfer with replace.
	// if timeout occurs we should ring back to operator
		
	// select which call to transfer: current or holded
	// set that call on hold

		if (current_call == -1)
			{ xfer_call = hold_call; }
		else
		{
			xfer_call = current_call;
			pjsua_call_set_hold(xfer_call, NULL);
		}

		// create url from prefixes and number
		pj_url_to = gen_url((char*)&url_to,MAX_URL_SIZE,n);

		// mark this call as call-in-tranfer
		memcpy(&app_config.call_data[xfer_call].xfer_to,&url_to,strlen(url_to));

		// start a total tranfer timeout timer
		app_config.call_data[xfer_call].xfer_total_timer.id = xfer_call;
		delay.sec = app_config.xfer_timeout;
		delay.msec = 0;
		pjsip_endpt_schedule_timer(pjsua_get_pjsip_endpt(),
				&app_config.call_data[xfer_call].xfer_total_timer, &delay);

		// start retry timer with minimal delay - schedule first attempt
		app_config.call_data[xfer_call].xfer_retry_timer.id = xfer_call;
		delay.sec = 1;
		pjsip_endpt_schedule_timer(pjsua_get_pjsip_endpt(),
				&app_config.call_data[xfer_call].xfer_retry_timer, &delay);

		// forget about this call for a while
		if (xfer_call == current_call)
		{
				current_call = -1;
				cmd_hook_down(0);			
		}
		else
			{ hold_call = -1; }

		tcpserv_pf("THINPJ act='transfer', to='%s', res='ok', status='Setup for auto xfer'\n\r",url_to);
		prn_cstate();
	}

	return 1;
}




int cmd_hold()
{
	if (hold_call == PJSUA_INVALID_ID)  // have place for hold
	{
		if (current_call == PJSUA_INVALID_ID) // no current call
			{ return -1; }

		hold_call = current_call;
		pjsua_call_set_hold(hold_call, NULL);

		current_call = PJSUA_INVALID_ID;
		cmd_hook_down(0);

		return 1;
	}
	
	if (current_call != PJSUA_INVALID_ID) // exchange calls 
	{
		pjsua_call_id	tmp_call;
		tmp_call = current_call;
		pjsua_call_set_hold(tmp_call, NULL);
		current_call = PJSUA_INVALID_ID;
		cmd_hook_down(HOOK_NOSTATE);

		pjsua_call_reinvite(hold_call, PJ_TRUE, NULL);
		current_call = hold_call;
		hold_call = tmp_call;
		cmd_hook_up(HOOK_NOSOUND);
		return 2;
	}
	
	cmd_hook_down(HOOK_NOSTATE);
	cmd_hook_up(HOOK_NOSOUND | HOOK_NOSTATE);

	pjsua_call_reinvite(hold_call, PJ_TRUE, NULL);
	current_call = hold_call;
	hold_call = PJSUA_INVALID_ID;

	prn_cstate();

	return 3;

}

int cmd_register(char *cmd)
{
	char buf[128];
	pj_status_t status;
	pjsua_acc_config acc_cfg;
	pjsua_acc_info info;
	char *p;
			
	Thash *cmdh = new Thash();
	cmdh->parseString(cmd);

	pjsua_acc_id reg_acc_id = current_acc;

	p = cmdh->get("acc_id");
	if (*p!='\0')
	{
		reg_acc_id = (pjsua_acc_id)atoi(p);
                PJ_LOG(3,(THIS_FILE, "Registering account GUI ID = %d", reg_acc_id));
	}

/*
	pjsua_acc_get_info(reg_acc_id, &info);
	pj_ansi_snprintf((char*)&buf,128,"%.*s",(int)info.acc_uri.slen, info.acc_uri.ptr);

	if (strstr(cmdh->get("user_url"),buf)!=NULL)
	{
		if (info.status == 200)
		{
			tcpserv_pf("THINPJ act='reg', res='ok', status='%s'\n",info.status_text.ptr);
			delete cmdh;
			return 2;
		}		
		pjsua_acc_del(reg_acc_id);
	}

*/
	pjsua_acc_config_default(&acc_cfg);

	acc_cfg.id = pj_str(cmdh->get("user_url"));
	acc_cfg.reg_uri = pj_str(cmdh->get("registrar_url"));
	acc_cfg.cred_count = 1;
	acc_cfg.cred_info[0].scheme = pj_str("Digest");
	acc_cfg.cred_info[0].realm = pj_str(cmdh->get("authrealm"));
	acc_cfg.cred_info[0].username = pj_str(cmdh->get("username"));
	acc_cfg.cred_info[0].data_type = 0;
	acc_cfg.cred_info[0].data = pj_str(cmdh->get("password"));

	pjsua_acc_id new_id;
	status = pjsua_acc_add(&acc_cfg, PJ_TRUE, &new_id);

        PJ_LOG(3,(THIS_FILE, "Accounted added, new id = %d", new_id));

	//if (prev_acc_id != PJSUA_INVALID_ID) {
        //    pjsua_acc_set_default(prev_acc_id);	
	//}

	delete cmdh;

	return ( status == PJ_SUCCESS ? 1 : 0);
}



int cmd_unregister()
{
	if (!pjsua_acc_is_valid(1))
	{
		return 0;
	}

	pjsua_acc_del(1);
	return 1;
}


int cmd_config(char *cmd)
{
	Thash *cmdh = new Thash();
	cmdh->parseString(cmd);
	char *err;
	char *p;
	
	p = cmdh->get("vol_spk");
	if (*p!='\0')
	{
			app_config.speaker_level = (float)strtod(p, &err);
			pjsua_conf_adjust_tx_level(0, app_config.speaker_level);
	}

	p = cmdh->get("vol_mic");
	if (*p!='\0')
	{
			app_config.mic_level = (float)strtod(p, &err);
			pjsua_conf_adjust_rx_level(0, app_config.mic_level);
	}

	p = cmdh->get("dial_prefix");
	if (*p!='\0')
	{
		strcpy(app_config.dialed_chars_prefix,p);
	}

	p = cmdh->get("dial_postfix");
	if (*p!='\0')
	{
		strcpy(app_config.dialed_chars_postfix,p);
	}

	p = cmdh->get("vp_xfer_retry_time");
	if (*p!='\0')
	{
		app_config.xfer_retry_time = atoi(p);
	}

	p = cmdh->get("vp_xfer_timeout");
	if (*p!='\0')
	{
		app_config.xfer_timeout = atoi(p);
	}

	p = cmdh->get("smart_transfer");
	if (strstr(p,"yes")!=NULL)
	{
		app_config.smart_transfer=PJ_TRUE;
	}
		
	delete cmdh;
	return 1;
}


/*
 * Main "user interface" loop.
 */
void console_app_main()
{
    char menuin[TCPSERV_BUFSIZE];

	int rc;
	int sound_reset_counter = 0;
	pj_status_t status;

	tcpserv_init();
	tcpserv_listen_and_accept(9998);
	

	for (;;) {

	rc = tcpserv_read_to_buf();

	if (rc <= 0)
	{
		printf ("Disconnect restarting...\n");
		goto on_exit;
	}

//printf ("Read bytes: %d\n", rc);

	while (1)
	{
		char *line = tcpserv_getline();
		if (line == NULL)
			{ break; }

		strcpy((char*)&menuin,line);

//		printf ("Got line='%s'\n",line);

		switch (menuin[0]) {
	
		case 'p':	// keepalive poll
					// if handset is down inject holded calls if any

			if (! app_config.offhook )
			{
					inject_holded_call_as_incoming();
			}

			prn_cstate();
			
			if ( (++sound_reset_counter > 1) &&
                             (current_call   == PJSUA_INVALID_ID) &&
                             (hold_call	== PJSUA_INVALID_ID) && 
                             (incoming_call  == PJSUA_INVALID_ID) &&
                             (!app_config.offhook)
                           ) {
                             
                                   sound_reset_counter = 0;

                                   PJ_LOG(3,(THIS_FILE, "Performing sound device reset (capture_dev=%d, playback_dev=%d)",app_config.capture_dev,app_config.playback_dev));

                                   status = pjsua_set_snd_dev(NULL_SND_DEV_ID,NULL_SND_DEV_ID);
                                   if (status != PJ_SUCCESS) {
                                        PJ_LOG(1,(THIS_FILE, "Error on sound device reset 1"));
                                        goto on_exit;
                                   }
                                   
                                   status = pjsua_set_snd_dev(app_config.capture_dev, app_config.playback_dev);
                                   if (status != PJ_SUCCESS) {
                                        PJ_LOG(1,(THIS_FILE, "Error on sound device reset 2"));
                                        goto on_exit;
                                   }
                        }
			

			break;

		case 'v':
				tcpserv_pf("THINPJ act='levels', spk='%f', mic='%f'\n\r",
					app_config.speaker_level, app_config.mic_level);
				break;


		case 'h':
			if (menuin[1] == 'u')
				{ cmd_hook_up(0); }
			else if (menuin[1] == 'd')
			{
				cmd_hook_down(0);
			}
			break;

		case 'd':
			if (menuin[1]=='i')
			{
				rc = cmd_dialnumber((char*)&menuin[2]);
				break;
			}

			rc = cmd_dialdigit(menuin[1],0);

        break;

                case 'a':
                    rc = cmd_account((char*)&menuin[1]);
                    break;

		case 't':
			rc = cmd_transfer((char*)&menuin[1]);
			break;

		case 'o':
			rc = cmd_hold();		
			break;

		case 'r':
			rc = cmd_register((char*)&menuin);

			if (rc==0)
			{
				tcpserv_puts("THINPJ act='reg', res='fail', status='fail'");
			}
			else if (rc==1)
			{
				tcpserv_puts("THINPJ act='reg_send', res='ok'");
			}

			break;

		case 'u':
			rc = cmd_unregister();
			tcpserv_pf("THINPJ act='unreg', res='%s'\n\r",(rc?"ok":"fail"));

			break;

		case 'c':
			rc = cmd_config((char*)&menuin);
		break;

			break;

		case 'q':
		    goto on_exit;


		default:
		break;

	} // while getline
	} // for read buf

}

    PJ_LOG(1,(THIS_FILE, "Error on sound"));

on_exit: ;
    
}




/* Show usage */
static void usage(void)
{
    puts  ("Usage:");
    puts  ("  thinpj [options] [SIP URL to call]");
    puts  ("");
    puts  ("General options:");
    puts  ("  --help              Display this help screen");
    puts  ("  --version           Display version info");
    puts  ("");
    puts  ("Logging options:");
    puts  ("  --log-file=fname    Log to filename (default stderr)");
    puts  ("  --log-level=N       Set log max level to N (0(none) to 6(trace)) (default=5)");
    puts  ("  --app-log-level=N   Set log max level for stdout display (default=4)");

    puts  ("");
    puts  ("Media Options:");
    puts  ("  --clock-rate=N      Override conference bridge clock rate");
    puts  ("  --snd-clock-rate=N  Override sound device clock rate");
    puts  ("  --quality=N         Specify media quality (0-10, default=6)");
    puts  ("  --no-vad            Disable VAD/silence detector (default=vad enabled)");
    puts  ("  --ec-tail=MSEC      Set echo canceller tail length (default=256)");
    puts  ("  --ec-opt=OPT        Select echo canceller algorithm (0=default, ");
    puts  ("                        1=speex, 2=suppressor)");
    puts  ("  --capture-dev=id    Audio capture device ID (default=-1)");
    puts  ("  --playback-dev=id   Audio playback device ID (default=-1)");
    puts  ("  --ring-dev=id	      Incoming call ring device ID (default=-1)");
	puts  ("  --capture-lat=N     Audio capture latency, in ms (default=100)");
    puts  ("  --playback-lat=N    Audio playback latency, in ms (default=100)");
    puts  ("  --jb-max-size       Specify jitter buffer maximum size, in frames (default=-1)");

    puts  ("");
    puts  ("Media Transport Options:");
    puts  ("  --rtp-port=N        Base port to try for RTP (default=4000)");
    puts  ("");

	fflush(stdout);
}
/*
 * Handler registration status has changed.
 */
static void on_reg_state(pjsua_acc_id acc_id)
{
    
	pjsua_acc_info info;
    pjsua_acc_get_info(acc_id, &info);

	tcpserv_pf("THINPJ act='reg', res='%s', status='%s'\n\r",
			( info.status == 200 ? "ok" : "fail"),
			info.status_text.ptr);
}



static int my_atoi(const char *cs)
{
    pj_str_t s;

    pj_cstr(&s, cs);
    if (cs[0] == '-') {
	s.ptr++, s.slen--;
	return 0 - (int)pj_strtoul(&s);
    } else if (cs[0] == '+') {
	s.ptr++, s.slen--;
	return pj_strtoul(&s);
    } else {
	return pj_strtoul(&s);
    }
}


/* Parse arguments. */
static pj_status_t parse_args(int argc, char *argv[],
			      struct app_config *cfg)
{
    int c;
    int option_index;

    enum {
		OPT_LOG_FILE=128,
        OPT_LOG_LEVEL,
        OPT_APP_LOG_LEVEL,
        OPT_HELP,
        OPT_VERSION,   /* version */
        OPT_CLOCK_RATE,
        OPT_SND_CLOCK_RATE,
        OPT_LOCAL_PORT,   /* local-port */
        OPT_IP_ADDR, /* ip-addr */
        OPT_RTP_PORT,
        OPT_NO_VAD,
        OPT_EC_TAIL,
        OPT_EC_OPT,
        OPT_QUALITY,
        OPT_MAX_CALLS,
        OPT_CAPTURE_DEV,
        OPT_PLAYBACK_DEV,
        OPT_CAPTURE_LAT,
		OPT_RING_DEV,
        OPT_PLAYBACK_LAT,
        OPT_SND_AUTO_CLOSE,
        OPT_JB_MAX_SIZE
	};
	
	struct pj_getopt_option long_options[] = {
	{ "log-file",	1, 0, OPT_LOG_FILE},
	{ "log-level",	1, 0, OPT_LOG_LEVEL},
	{ "app-log-level",1,0,OPT_APP_LOG_LEVEL},
	{ "help",	0, 0, OPT_HELP},
	{ "version",	0, 0, OPT_VERSION},
	{ "clock-rate",	1, 0, OPT_CLOCK_RATE},
	{ "snd-clock-rate",	1, 0, OPT_SND_CLOCK_RATE},
	{ "local-port", 1, 0, OPT_LOCAL_PORT},
	{ "ip-addr",	1, 0, OPT_IP_ADDR},
	{ "quality",	1, 0, OPT_QUALITY},
	{ "no-vad",     0, 0, OPT_NO_VAD},
	{ "ec-tail",    1, 0, OPT_EC_TAIL},
	{ "ec-opt",	1, 0, OPT_EC_OPT},
	{ "capture-dev",    1, 0, OPT_CAPTURE_DEV},
	{ "playback-dev",   1, 0, OPT_PLAYBACK_DEV},
	{ "ring-dev",		1, 0, OPT_RING_DEV},
	{ "capture-lat",    1, 0, OPT_CAPTURE_LAT},
	{ "playback-lat",   1, 0, OPT_PLAYBACK_LAT},
	{ "jb-max-size", 1, 0, OPT_JB_MAX_SIZE},
	{ NULL, 0, 0, 0}
    };

    pj_optind = 0;

	while((c=pj_getopt_long(argc,argv, "", long_options,&option_index))!=-1) {
	pj_str_t tmp;
	long lval;

	switch (c) {
	
	case OPT_LOG_FILE:
	    cfg->log_cfg.log_filename = pj_str(pj_optarg);
	    break;

	case OPT_LOG_LEVEL:
	    c = pj_strtoul(pj_cstr(&tmp, pj_optarg));
	    if (c < 0 || c > 6) {
		PJ_LOG(1,(THIS_FILE, 
			  "Error: expecting integer value 0-6 "
			  "for --log-level"));
		return PJ_EINVAL;
	    }
	    cfg->log_cfg.level = c;
	    pj_log_set_level( c );
	    break;

	case OPT_APP_LOG_LEVEL:
	    cfg->log_cfg.console_level = pj_strtoul(pj_cstr(&tmp, pj_optarg));
	    if (cfg->log_cfg.console_level < 0 || cfg->log_cfg.console_level > 6) {
		PJ_LOG(1,(THIS_FILE, 
			  "Error: expecting integer value 0-6 "
			  "for --app-log-level"));
		return PJ_EINVAL;
	    }
	    break;

	case OPT_HELP:
	    usage();
	    return PJ_EINVAL;

	case OPT_VERSION:   /* version */
	    pj_dump_config();
	    return PJ_EINVAL;

	case OPT_CLOCK_RATE:
	    lval = pj_strtoul(pj_cstr(&tmp, pj_optarg));
	    if (lval < 8000 || lval > 192000) {
		PJ_LOG(1,(THIS_FILE, "Error: expecting value between "
				     "8000-192000 for conference clock rate"));
		return PJ_EINVAL;
	    }
	    cfg->media_cfg.clock_rate = lval; 
	    break;

	case OPT_SND_CLOCK_RATE:
	    lval = pj_strtoul(pj_cstr(&tmp, pj_optarg));
	    if (lval < 8000 || lval > 192000) {
		PJ_LOG(1,(THIS_FILE, "Error: expecting value between "
				     "8000-192000 for sound device clock rate"));
		return PJ_EINVAL;
	    }
	    cfg->media_cfg.snd_clock_rate = lval; 
	    break;

	case OPT_LOCAL_PORT:   /* local-port */
	    lval = pj_strtoul(pj_cstr(&tmp, pj_optarg));
	    if (lval < 0 || lval > 65535) {
		PJ_LOG(1,(THIS_FILE, 
			  "Error: expecting integer value for "
			  "--local-port"));
		return PJ_EINVAL;
	    }
	    cfg->udp_cfg.port = (pj_uint16_t)lval;
	    break;

	case OPT_IP_ADDR: /* ip-addr */
	    cfg->udp_cfg.public_addr = pj_str(pj_optarg);
	    cfg->rtp_cfg.public_addr = pj_str(pj_optarg);
	    break;


	case OPT_RTP_PORT:
	    cfg->rtp_cfg.port = my_atoi(pj_optarg);
	    if (cfg->rtp_cfg.port == 0) {
		enum { START_PORT=4000 };
		unsigned range;

		range = (65535-START_PORT-PJSUA_MAX_CALLS*2);
		cfg->rtp_cfg.port = START_PORT + 
				    ((pj_rand() % range) & 0xFFFE);
	    }

	    if (cfg->rtp_cfg.port < 1 || cfg->rtp_cfg.port > 65535) {
		PJ_LOG(1,(THIS_FILE,
			  "Error: rtp-port argument value "
			  "(expecting 1-65535"));
		return -1;
	    }
	    break;

	case OPT_NO_VAD:
	    cfg->media_cfg.no_vad = PJ_TRUE;
	    break;

	case OPT_EC_TAIL:
	    cfg->media_cfg.ec_tail_len = my_atoi(pj_optarg);
	    if (cfg->media_cfg.ec_tail_len > 1000) {
		PJ_LOG(1,(THIS_FILE, "I think the ec-tail length setting "
			  "is too big"));
		return -1;
	    }
	    break;

	case OPT_EC_OPT:
	    cfg->media_cfg.ec_options = my_atoi(pj_optarg);
	    break;

	case OPT_QUALITY:
	    cfg->media_cfg.quality = my_atoi(pj_optarg);
	    if (cfg->media_cfg.quality < 0 || cfg->media_cfg.quality > 10) {
		PJ_LOG(1,(THIS_FILE,
			  "Error: invalid --quality (expecting 0-10"));
		return -1;
	    }
	    break;

	case OPT_CAPTURE_DEV:
	    cfg->capture_dev = my_atoi(pj_optarg);
	    break;

	case OPT_PLAYBACK_DEV:
	    cfg->playback_dev = my_atoi(pj_optarg);
	    break;

	case OPT_RING_DEV:
		cfg->ring_dev = my_atoi(pj_optarg);
	    break;

	case OPT_CAPTURE_LAT:
	    cfg->capture_lat = atoi(pj_optarg);
	    break;

	case OPT_PLAYBACK_LAT:
	    cfg->playback_lat = atoi(pj_optarg);
	    break;

	case OPT_SND_AUTO_CLOSE:
	    cfg->media_cfg.snd_auto_close_time = atoi(pj_optarg);
	    break;

	case OPT_JB_MAX_SIZE:
	    cfg->media_cfg.jb_max = atoi(pj_optarg);
	    break;

	default:
	    PJ_LOG(1,(THIS_FILE, 
		      "Argument \"%s\" is not valid. Use --help to see help",
		      argv[pj_optind-1]));
	    return -1;
	}
    }

    if (pj_optind != argc) {
		PJ_LOG(1,(THIS_FILE, "Error: unknown options %s", argv[pj_optind]));
		return PJ_EINVAL;
    }

    return PJ_SUCCESS;
}


/*****************************************************************************
 * Public API
 */

pj_status_t app_init(int argc, char *argv[])
{
    pjsua_transport_id transport_id = -1;
    pj_status_t status;

	PJ_UNUSED_ARG(argc);
	PJ_UNUSED_ARG(argv);


    PJ_LOG(1,(THIS_FILE, "Thinpj voice part version [%s]", THINPJ_VERSION));

    /* Create pjsua */
    status = pjsua_create();
    if (status != PJ_SUCCESS)
	return status;

    /* Create pool for application */
    app_config.pool = pjsua_pool_create("pjsua", 1000, 1000);

    /* Initialize default config */
    default_config(&app_config);

    /* Parse the arguments */
    status = parse_args(argc, argv, &app_config);
    if (status != PJ_SUCCESS)
	return status;

	app_config.cfg.max_calls = PJSUA_MAX_CALLS / 2;

    /* Initialize application callbacks */
    app_config.cfg.cb.on_call_state = &on_call_state;
    app_config.cfg.cb.on_call_media_state = &on_call_media_state;
    app_config.cfg.cb.on_incoming_call = &on_incoming_call;
    app_config.cfg.cb.on_call_tsx_state = &on_call_tsx_state;
    app_config.cfg.cb.on_dtmf_digit = &call_on_dtmf_callback;
    app_config.cfg.cb.on_reg_state = &on_reg_state;
    app_config.cfg.cb.on_call_transfer_status = &on_call_transfer_status;

    /* Initialize pjsua */
    status = pjsua_init(&app_config.cfg, &app_config.log_cfg,
			&app_config.media_cfg);
    if (status != PJ_SUCCESS)
	return status;

	/* Add UDP transport */
	pjsua_acc_id aid;
	
	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP,
					&app_config.udp_cfg,
					&transport_id);

	if (status != PJ_SUCCESS)
	    goto on_error;

	/* Add local account */
	pjsua_acc_add_local(transport_id, PJ_TRUE, &aid);
//	pjsua_acc_set_transport(aid, transport_id);
	pjsua_acc_set_online_status(current_acc, PJ_TRUE);

	if (app_config.udp_cfg.port == 0) {
	    pjsua_transport_info ti;
	    pj_sockaddr_in *a;

	    pjsua_transport_get_info(transport_id, &ti);
	    a = (pj_sockaddr_in*)&ti.local_addr;
	}

    if (transport_id == -1) {
		PJ_LOG(3,(THIS_FILE, "Error: no transport is configured"));
		status = -1;
		goto on_error;
    }

    /* Add RTP transports */
    status = pjsua_media_transports_create(&app_config.rtp_cfg);
    if (status != PJ_SUCCESS)
	goto on_error;

    /* Set sound device latency */
    pjmedia_snd_set_latency(app_config.capture_lat, app_config.playback_lat);

    if (app_config.capture_dev  != PJSUA_INVALID_ID ||
        app_config.playback_dev != PJSUA_INVALID_ID) 
    {
	status = pjsua_set_snd_dev(app_config.capture_dev, 
				   app_config.playback_dev);
	if (status != PJ_SUCCESS)
	    goto on_error;
    }

    /* dial inavtivity timer init */
    pj_timer_entry_init(&app_config.dial_inactivity_timer, 0, NULL, &dial_inactivity_callback);
    app_config.dial_inactivity_timer.id = PJSUA_INVALID_ID;
    app_config.dial_inactivity_count = 0;
        
    /* dialed chars */
    pj_bzero(&app_config.dialed_chars, MAX_DIALED_CHARS);
    app_config.dc_index = 0;

	/* Generate Tones */

	unsigned i, samples_per_frame;
	pjmedia_tone_desc tone[RINGBACK_CNT];
    pj_str_t name;


    /* DTMF player */
	name = pj_str("dtmf");
    status = pjmedia_tonegen_create2(app_config.pool, &name,
					 16000, 1, 160, 16, 
					 0,  &app_config.dtmf_media_port);
    if (status != PJ_SUCCESS) {
	    pjsua_perror(THIS_FILE, "Unable to create tone generator", status);
	    goto on_error;
    }

    status = pjsua_conf_add_port(app_config.pool, app_config.dtmf_media_port, &app_config.dtmf_conf_port);
    pj_assert(status == PJ_SUCCESS);

    pjsua_conf_connect(app_config.dtmf_conf_port, 0);
	/* todo: is it right? i connect dtmf port staticly */

	/* offhoook tone */
	samples_per_frame = app_config.media_cfg.audio_frame_ptime * 
			    app_config.media_cfg.clock_rate *
			    app_config.media_cfg.channel_count / 1000;

	name = pj_str("offhook");
	status = pjmedia_tonegen_create2(app_config.pool, &name, 
					 app_config.media_cfg.clock_rate,
					 app_config.media_cfg.channel_count, 
					 samples_per_frame,
					 16, PJMEDIA_TONEGEN_LOOP, 
					 &app_config.offhook_port);

	if (status != PJ_SUCCESS)
	    goto on_error;

	pj_bzero(&tone, sizeof(tone));

	tone[0].freq1 = OFFHOOK_FREQ;
	tone[0].on_msec = 5000;

	pjmedia_tonegen_play(app_config.offhook_port, 1, 
			     tone, PJMEDIA_TONEGEN_LOOP);

	status = pjsua_conf_add_port(app_config.pool, app_config.offhook_port,
				     &app_config.offhook_slot);

	if (status != PJ_SUCCESS)
	    goto on_error;

	/* Ringback tone (call is ringing) */
	name = pj_str("ringback");
	status = pjmedia_tonegen_create2(app_config.pool, &name, 
					 app_config.media_cfg.clock_rate,
					 app_config.media_cfg.channel_count, 
					 samples_per_frame,
					 16, PJMEDIA_TONEGEN_LOOP, 
					 &app_config.ringback_port);
	if (status != PJ_SUCCESS)
	    goto on_error;

	pj_bzero(&tone, sizeof(tone));
	for (i=0; i<RINGBACK_CNT; ++i) {
	    tone[i].freq1 = RINGBACK_FREQ1;
	    tone[i].freq2 = RINGBACK_FREQ2;
	    tone[i].on_msec = RINGBACK_ON;
	    tone[i].off_msec = RINGBACK_OFF;
	}
	tone[RINGBACK_CNT-1].off_msec = RINGBACK_INTERVAL;

	pjmedia_tonegen_play(app_config.ringback_port, RINGBACK_CNT, tone,
			     PJMEDIA_TONEGEN_LOOP);

	status = pjsua_conf_add_port(app_config.pool, app_config.ringback_port,
				     &app_config.ringback_slot);
	if (status != PJ_SUCCESS)
	    goto on_error;


	/* init ring wav */
    
//	pj_str_t wav_file;
//	wav_file = pj_str(RING_FILE);

    /* Create file media port from the WAV file */
    status = pjmedia_wav_player_port_create(  app_config.pool,	/* memory pool	    */
					      (const char *)RING_FILE,	/* file to play	    */
					      20,	/* ptime.	    */
					      0,	/* flags	    */
					      0,	/* default buffer   */
					      &app_config.ring_wav_file_media_port /* returned port    */
					      );


    /* Create sound player port. */
    status = pjmedia_snd_port_create_player( 
		 app_config.pool,				    /* pool		    */
		 app_config.ring_dev,
		 app_config.ring_wav_file_media_port->info.clock_rate,	    /* clock rate.	    */
		 app_config.ring_wav_file_media_port->info.channel_count,	    /* # of channels.	    */
		 app_config.ring_wav_file_media_port->info.samples_per_frame, /* samples per frame.   */
		 app_config.ring_wav_file_media_port->info.bits_per_sample,   /* bits per sample.	    */
		 0,				    /* options		    */
		 &app_config.ring_wav_snd_port			    /* returned port	    */
		 );

    if (status != PJ_SUCCESS) {
		pjsua_perror(THIS_FILE, "Error loading ringin.wav file!", status);
	    goto on_error;
    }

//    status = pjmedia_snd_port_connect( snd_port, file_port);
//    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

/*	
	pj_str_t wav_file;
	wav_file = pj_str(RING_FILE);
	status = pjsua_player_create((const pj_str_t*)&wav_file, 0,  &app_config.ring_wav_id);

	if (status != PJ_SUCCESS)
	    goto on_error;

	app_config.ring_wav_port = pjsua_player_get_conf_port(app_config.ring_wav_id);
*/		
	return PJ_SUCCESS;

on_error:
    app_destroy();
    return status;
}


pj_status_t app_main(void)
{
    pj_status_t status;

    /* Start pjsua */
    status = pjsua_start();
    if (status != PJ_SUCCESS) {
	app_destroy();
	return status;
    }

    console_app_main();

 

    return PJ_SUCCESS;
}

pj_status_t app_destroy(void)
{
    pj_status_t status;

	/* wav player destroy */

    status = pjmedia_snd_port_disconnect(app_config.ring_wav_snd_port);

    /* Without this sleep, Windows/DirectSound will repeteadly
     * play the last frame during destroy.
     */
    pj_thread_sleep(100);

    /* Destroy sound device */
    status = pjmedia_snd_port_destroy( app_config.ring_wav_snd_port );
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

    /* Destroy file port */
    status = pjmedia_port_destroy( app_config.ring_wav_file_media_port );
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

/*
	if (app_config.ring_wav_port && app_config.ring_wav_id != PJSUA_INVALID_ID) {
		pjsua_player_destroy(app_config.ring_wav_id);
		app_config.ring_wav_id = PJSUA_INVALID_ID;
	}
*/



	/* Close offhook port */
    if (app_config.offhook_port && app_config.offhook_slot != PJSUA_INVALID_ID) {
		pjsua_conf_remove_port(app_config.offhook_slot);
		app_config.offhook_slot = PJSUA_INVALID_ID;
		pjmedia_port_destroy(app_config.offhook_port);
		app_config.offhook_port = NULL;
    }

    /* Close ringback port */
    if (app_config.ringback_port && 
	app_config.ringback_slot != PJSUA_INVALID_ID) 
    {
		pjsua_conf_remove_port(app_config.ringback_slot);
		app_config.ringback_slot = PJSUA_INVALID_ID;
	
		pjmedia_port_destroy(app_config.ringback_port);
		app_config.ringback_port = NULL;
    }

    /* Close DTMF tone generators */
	if (app_config.dtmf_media_port && app_config.dtmf_conf_port!=PJSUA_INVALID_ID)
	{
		pjsua_conf_disconnect(app_config.dtmf_conf_port, 0);
		pjsua_conf_remove_port(app_config.dtmf_conf_port);
		app_config.dtmf_conf_port = PJSUA_INVALID_ID;
    
		pjmedia_port_destroy(app_config.dtmf_media_port);
		app_config.dtmf_media_port = NULL;
	}
                
    if (app_config.pool) {
		pj_pool_release(app_config.pool);
		app_config.pool = NULL;
    }

    status = pjsua_destroy();
    pj_bzero(&app_config, sizeof(app_config));

    return status;
}


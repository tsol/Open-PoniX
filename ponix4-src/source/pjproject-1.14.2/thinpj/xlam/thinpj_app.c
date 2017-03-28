#include <pjsua-lib/pjsua.h>
#include "net/tcpserv.h"

#define THIS_FILE	"thinpj_app.c"
#define NO_LIMIT	(int)0x7FFFFFFF

/* Ringtones		    US	       UK  */
#define RINGBACK_FREQ1	    440	    /* 400 */
#define RINGBACK_FREQ2	    480	    /* 450 */
#define RINGBACK_ON	    2000    /* 400 */
#define RINGBACK_OFF	    4000    /* 200 */
#define RINGBACK_CNT	    1	    /* 2   */
#define RINGBACK_INTERVAL   4000    /* 2000 */

#define OFFHOOK_FREQ	438
#define DTMF_CNT	10
#define DTMF_ON		20

#define RING_FREQ1	    800
#define RING_FREQ2	    640
#define RING_ON		    200
#define RING_OFF	    100
#define RING_CNT	    3
#define RING_INTERVAL	    3000

#define MAX_DIALED_CHARS		16
#define DIAL_INACTIVITY_DELAY		3 // secs
#define DIAL_HANGUP_INACTIVITY_TIMES	3
#define MIN_CHARS_TO_DIAL		3

/* Call specific data */
struct call_data
{
    pj_timer_entry	    timer;
    pj_bool_t		    ringback_on;
    pj_bool_t		    ring_on;
};


/* Pjsua application data */
static struct app_config
{
    pjsua_config	    cfg;
    pjsua_logging_config    log_cfg;
    pjsua_media_config	    media_cfg;
    pj_bool_t		    no_refersub;
    pj_bool_t		    no_tcp;
    pj_bool_t		    no_udp;
    pj_bool_t		    use_tls;
    pjsua_transport_config  udp_cfg;
    pjsua_transport_config  rtp_cfg;

    unsigned		    acc_cnt;
    pjsua_acc_config	    acc_cfg[PJSUA_MAX_ACC];

    unsigned		    buddy_cnt;
    pjsua_buddy_config	    buddy_cfg[PJSUA_MAX_BUDDIES];

    struct call_data	    call_data[PJSUA_MAX_CALLS];

    pj_pool_t		   *pool;
    /* Compatibility with older pjsua */

    unsigned		    codec_cnt;
    pj_str_t		    codec_arg[32];
    unsigned		    codec_dis_cnt;
    pj_str_t                codec_dis[32];
    pj_bool_t		    null_audio;
    unsigned		    wav_count;
    pj_str_t		    wav_files[32];

    pjmedia_port    	    *dtmf_media_port;
    pjsua_conf_port_id	    dtmf_conf_port;

    pjsua_player_id	    wav_id;
    pjsua_conf_port_id	    wav_port;
    pj_bool_t		    auto_play;
    pj_bool_t		    auto_play_hangup;
    pj_timer_entry	    auto_hangup_timer;
    pj_bool_t		    auto_loop;
    pj_bool_t		    auto_conf;
    pj_str_t		    rec_file;
    pj_bool_t		    auto_rec;
    pjsua_recorder_id	    rec_id;
    pjsua_conf_port_id	    rec_port;
    unsigned		    auto_answer;
    unsigned		    duration;

    float		    mic_level,
			    speaker_level;

    int			    capture_dev, playback_dev;
    unsigned		    capture_lat, playback_lat;

    pj_bool_t		    no_tones;
    int			    ringback_slot;
    int			    ringback_cnt;
    pjmedia_port	   *ringback_port;
    int			    ring_slot;
    int			    ring_cnt;
    pjmedia_port	   *ring_port;

    int			    offhook_slot;
    int			    offhook_cnt;
    pjmedia_port	   *offhook_port;

    pj_bool_t			offhook;
    char 			dialed_chars[MAX_DIALED_CHARS+1];
    int				dc_index;

    char			dialed_chars_postfix[80];
    char			dialed_chars_prefix[80];

    pj_timer_entry		dial_inactivity_timer;
    int				dial_inactivity_count;

} app_config;


//static pjsua_acc_id	current_acc;
#define current_acc	pjsua_acc_get_default()
static pjsua_call_id	current_call = PJSUA_INVALID_ID;
static pj_bool_t	cmd_echo;
static int		stdout_refresh = -1;
static const char      *stdout_refresh_text = "STDOUT_REFRESH";
static pj_bool_t	stdout_refresh_quit = PJ_FALSE;
static pj_str_t		uri_arg;

static char some_buf[2048];

pj_status_t app_destroy(void);

static void ringback_start(pjsua_call_id call_id);
static void ring_start(pjsua_call_id call_id);
static void ring_stop(pjsua_call_id call_id);


/*****************************************************************************
 * Configuration manipulation
 */


/* Set default config. */
static void default_config(struct app_config *cfg)
{
    char tmp[80];
    unsigned i;

    pjsua_config_default(&cfg->cfg);

    pj_ansi_sprintf(tmp, "PJSUA v%s/%s", pj_get_version(), PJ_OS_NAME);
    pj_strdup2_with_null(app_config.pool, &cfg->cfg.user_agent, tmp);

    pjsua_logging_config_default(&cfg->log_cfg);
    pjsua_media_config_default(&cfg->media_cfg);
    pjsua_transport_config_default(&cfg->udp_cfg);
    cfg->udp_cfg.port = 5060;
    pjsua_transport_config_default(&cfg->rtp_cfg);
    cfg->rtp_cfg.port = 4000;
    cfg->duration = NO_LIMIT;
    cfg->wav_id = PJSUA_INVALID_ID;
    cfg->rec_id = PJSUA_INVALID_ID;
    cfg->wav_port = PJSUA_INVALID_ID;
    cfg->rec_port = PJSUA_INVALID_ID;
    cfg->mic_level = cfg->speaker_level = 1.0;
    cfg->capture_dev = PJSUA_INVALID_ID;
    cfg->playback_dev = PJSUA_INVALID_ID;
    cfg->capture_lat = PJMEDIA_SND_DEFAULT_REC_LATENCY;
    cfg->playback_lat = PJMEDIA_SND_DEFAULT_PLAY_LATENCY;
    cfg->ringback_slot = PJSUA_INVALID_ID;
    cfg->ring_slot = PJSUA_INVALID_ID;
    cfg->offhook_slot = PJSUA_INVALID_ID;

    cfg->dtmf_conf_port = PJSUA_INVALID_ID;
                
    for (i=0; i<PJ_ARRAY_SIZE(cfg->acc_cfg); ++i)
	pjsua_acc_config_default(&cfg->acc_cfg[i]);

    for (i=0; i<PJ_ARRAY_SIZE(cfg->buddy_cfg); ++i)
	pjsua_buddy_config_default(&cfg->buddy_cfg[i]);

// my config

    strcpy(cfg->dialed_chars_postfix,"@192.168.4.57");
    strcpy(cfg->dialed_chars_prefix,"sip:");

    cfg->media_cfg.no_vad = PJ_TRUE;
    cfg->media_cfg.ec_tail_len = 0;

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


/*
 * Dump application states.
 */
static void app_dump(pj_bool_t detail)
{
    pjsua_dump(detail);
}


/*****************************************************************************
 * Console application
 */

static void ringback_start(pjsua_call_id call_id)
{
    if (app_config.no_tones)
	return;

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
    if (app_config.no_tones)
	return;

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

    if (app_config.call_data[call_id].ring_on) {
	app_config.call_data[call_id].ring_on = PJ_FALSE;

	pj_assert(app_config.ring_cnt>0);
	if (--app_config.ring_cnt == 0 && 
	    app_config.ring_slot!=PJSUA_INVALID_ID) 
	{
	    pjsua_conf_disconnect(app_config.ring_slot, 0);
	    pjmedia_tonegen_rewind(app_config.ring_port);
	}
    }
}

static void ring_start(pjsua_call_id call_id)
{
    if (app_config.no_tones)
	return;

    if (app_config.call_data[call_id].ring_on)
	return;

    app_config.call_data[call_id].ring_on = PJ_TRUE;

    if (++app_config.ring_cnt==1 && 
	app_config.ring_slot!=PJSUA_INVALID_ID) 
    {
	pjsua_conf_connect(app_config.ring_slot, 0);
    }
}

/*
 * Find next call when current call is disconnected or when user
 * press ']'
 */
static pj_bool_t find_next_call(void)
{
    int i, max;

    max = pjsua_call_get_max_count();
    for (i=current_call+1; i<max; ++i) {
	if (pjsua_call_is_active(i)) {
	    current_call = i;
	    return PJ_TRUE;
	}
    }

    for (i=0; i<current_call; ++i) {
	if (pjsua_call_is_active(i)) {
	    current_call = i;
	    return PJ_TRUE;
	}
    }

    current_call = PJSUA_INVALID_ID;
    return PJ_FALSE;
}


/*
 * Find previous call when user press '['
 */
static pj_bool_t find_prev_call(void)
{
    int i, max;

    max = pjsua_call_get_max_count();
    for (i=current_call-1; i>=0; --i) {
	if (pjsua_call_is_active(i)) {
	    current_call = i;
	    return PJ_TRUE;
	}
    }

    for (i=max-1; i>current_call; --i) {
	if (pjsua_call_is_active(i)) {
	    current_call = i;
	    return PJ_TRUE;
	}
    }

    current_call = PJSUA_INVALID_ID;
    return PJ_FALSE;
}

/*
 * Handler when invite state has changed.
 */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info call_info;

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &call_info);

    if (call_info.state == PJSIP_INV_STATE_DISCONNECTED) {

	/* Stop all ringback for this call */
	ring_stop(call_id);

	/* Rewind play file when hangup automatically, 
	 * since file is not looped
	 */
	if (app_config.auto_play_hangup)
	    pjsua_player_set_pos(app_config.wav_id, 0);


	tcpserv_pf( "THINPJ act='disconnect', call='%d', reason='%d', status='%s'",
	                call_id, call_info.last_status, call_info.last_status_text.ptr );
	                                                      
	PJ_LOG(3,(THIS_FILE, "Call %d is DISCONNECTED [reason=%d (%s)]", 
		  call_id,
		  call_info.last_status,
		  call_info.last_status_text.ptr));

	if (call_id == current_call) {
	    find_next_call();
	}

	/* Dump media state upon disconnected */
	if (1) {
	    pjsua_call_dump(call_id, PJ_TRUE, some_buf, 
			    sizeof(some_buf), "  ");
	    PJ_LOG(5,(THIS_FILE, 
		      "Call %d disconnected, dumping media stats\n%s", 
		      call_id, some_buf));
	}

    } else {

	if (call_info.state == PJSIP_INV_STATE_EARLY) {
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

	    /* Start ringback for 180 for UAC unless there's SDP in 180 */
	    if (call_info.role==PJSIP_ROLE_UAC && code==180 && 
		msg->body == NULL && 
		call_info.media_status==PJSUA_CALL_MEDIA_NONE) 
	    {
		ringback_start(call_id);
	    }

	    PJ_LOG(3,(THIS_FILE, "Call %d state changed to %s (%d %.*s)", 
		      call_id, call_info.state_text.ptr,
		      code, (int)reason.slen, reason.ptr));
	} else {
	    PJ_LOG(3,(THIS_FILE, "Call %d state changed to %s", 
		      call_id,
		      call_info.state_text.ptr));
	}

	if (current_call==PJSUA_INVALID_ID)
	    current_call = call_id;

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

    /* Start ringback */
    ring_start(call_id);

    tcpserv_pf("THINPJ act='call', account='%d', from='%s', to='%s'\n",
		  acc_id,
		  call_info.remote_info.ptr,
		  call_info.local_info.ptr);
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

    /* Stop ringback */
    ring_stop(call_id);

    if (call_info.media_status == PJSUA_CALL_MEDIA_ACTIVE) {

	    pjsua_conf_connect(call_info.conf_slot, 0);
	    pjsua_conf_connect(0, call_info.conf_slot);

	    PJ_LOG(3,(THIS_FILE, "Media for call %d is active", call_id));

    } else if (call_info.media_status == PJSUA_CALL_MEDIA_LOCAL_HOLD) {
	PJ_LOG(3,(THIS_FILE, "Media for call %d is suspended (hold) by local",
		  call_id));
    } else if (call_info.media_status == PJSUA_CALL_MEDIA_REMOTE_HOLD) {
	PJ_LOG(3,(THIS_FILE, 
		  "Media for call %d is suspended (hold) by remote",
		  call_id));
    } else if (call_info.media_status == PJSUA_CALL_MEDIA_ERROR) {
	pj_str_t reason = pj_str("ICE negotiation failed");

	PJ_LOG(1,(THIS_FILE,
		  "Media has reported error, disconnecting call"));

	pjsua_call_hangup(call_id, 500, &reason, NULL);

    } else {
	PJ_LOG(3,(THIS_FILE, 
		  "Media for call %d is inactive",
		  call_id));
    }
}

/*
 * DTMF callback.
 */
static void call_on_dtmf_callback(pjsua_call_id call_id, int dtmf)
{
    PJ_LOG(3,(THIS_FILE, "Incoming DTMF on call %d: %c", call_id, dtmf));
}

/*
 * Handler registration status has changed.
 */
static void on_reg_state(pjsua_acc_id acc_id)
{
    PJ_UNUSED_ARG(acc_id);
    // Log already written.
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


/*
 * Print account status.
 */
static void print_acc_status(int acc_id)
{
    char buf[80];
    pjsua_acc_info info;

    pjsua_acc_get_info(acc_id, &info);

    if (!info.has_registration) {
	pj_ansi_snprintf(buf, sizeof(buf), "%.*s", 
			 (int)info.status_text.slen,
			 info.status_text.ptr);

    } else {
	pj_ansi_snprintf(buf, sizeof(buf),
			 "%d/%.*s (expires=%d)",
			 info.status,
			 (int)info.status_text.slen,
			 info.status_text.ptr,
			 info.expires);

    }

    tcpserv_pf(" %c[%2d] %.*s: %s\n", (acc_id==current_acc?'*':' '),
	   acc_id,  (int)info.acc_uri.slen, info.acc_uri.ptr, buf);
    tcpserv_pf("       Online status: %.*s\n", 
	(int)info.online_status_text.slen,
	info.online_status_text.ptr);
}

/*
 * Input simple string
 */
static pj_bool_t simple_input(const char *title, char *buf, pj_size_t len)
{
    char *p;

    tcpserv_pf("%s (empty to cancel): ", title); fflush(stdout);
    tcpserv_gets(buf, len);

    /* Remove trailing newlines. */
    for (p=buf; ; ++p) {
	if (*p=='\r' || *p=='\n') *p='\0';
	else if (!*p) break;
    }

    if (!*buf)
	return PJ_FALSE;
    
    return PJ_TRUE;
}


#define NO_NB	-2
struct input_result
{
    int	  nb_result;
    char *uri_result;
};


/*
 * Input URL.
 */
static void ui_input_url(const char *title, char *buf, int len, 
			 struct input_result *result)
{
    result->nb_result = NO_NB;
    result->uri_result = NULL;

    tcpserv_puts("Url: ");
    tcpserv_gets(buf, len);
    len = strlen(buf);

    /* Left trim */
    while (pj_isspace(*buf)) {
	++buf;
	--len;
    }

    /* Remove trailing newlines */
    while (len && (buf[len-1] == '\r' || buf[len-1] == '\n'))
	buf[--len] = '\0';

    if (len == 0 || buf[0]=='q')
	return;

    pj_status_t status;

    if ((status=pjsua_verify_sip_url(buf)) != PJ_SUCCESS) {
	    pjsua_perror(THIS_FILE, "Invalid URL", status);
	    return;
    }

    result->uri_result = buf;
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
			if (app_config.ringback_slot!=PJSUA_INVALID_ID &&
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
      dial_inactivity_timer_stop();

      /* start a new one */

      pj_time_val delay;
  
      app_config.dial_inactivity_timer.id = 1;
      delay.sec = DIAL_INACTIVITY_DELAY;
      delay.msec = 0;

      pjsip_endpt_schedule_timer(pjsua_get_pjsip_endpt(), &app_config.dial_inactivity_timer, &delay);
                                                                                                
}


static void hook_down()
{
		  app_config.offhook = PJ_FALSE;
                  offhook_sound_off();
                  dial_inactivity_timer_stop();
		  
		  if (current_call != -1)
		  {
		    pjsua_call_hangup(current_call, 0, NULL, NULL);		    
		  }
}



static void dial_with_dialed_chars()
{
          char url[80];

          pj_ansi_snprintf(url, sizeof(url), "%s%s%s", app_config.dialed_chars_prefix,
                        app_config.dialed_chars,
                        app_config.dialed_chars_postfix);
          
          // makecall goes here                        
          tcpserv_pf("THINPJ act='dial', data='%s'\n",url);

          offhook_sound_off();
          dial_inactivity_timer_stop();

          pj_str_t uri_to_call;
          uri_to_call = pj_str(url);
                
          if ( pjsua_call_make_call( current_acc, &uri_to_call, 0, NULL, NULL, NULL) == PJ_SUCCESS ) {
			tcpserv_puts("THINPJ act='makecall', res='ok'");
          } else {
			tcpserv_puts("THINPJ act='makecall', res='fail'");
			hook_down();
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
        tcpserv_puts("\rTHINPJ act='unexpected', status='Inactivity callback while call is active!!'");
        return;
    };

    if (app_config.dial_inactivity_count++ == DIAL_HANGUP_INACTIVITY_TIMES)
    {
		  hook_down();
                  tcpserv_puts("\rTHINPJ act='hookdown', res='ok', status='Inactivity hookdown.'");
                  app_config.dial_inactivity_count = 0;
                  return;
    }

    if (app_config.dc_index >= MIN_CHARS_TO_DIAL)
    {
        tcpserv_puts("\r");
        dial_with_dialed_chars();    
        return;
    }

    dial_inactivity_timer_restart();
    
}


/*
 * Main "user interface" loop.
 */
void console_app_main()
{
    char menuin[32];
    char buf[128];
    char text[128];
    int i, count;
    char *uri;
    pj_str_t tmp;
    struct input_result result;
    pjsua_call_info call_info;
    pjsua_acc_info acc_info;

    pj_status_t status;


    for (;;) {

	tcpserv_pf(">>> ");
	tcpserv_gets( menuin, sizeof(menuin) );

	switch (menuin[0]) {

	case 'h':
		if (menuin[1] == 'u')
		{
		  if (app_config.offhook)
		  {
		    tcpserv_puts("THINPJ act='hookup', res='fail', status='Hook allready up.' ");
		    break;
		  }

                  pj_bzero(&app_config.dialed_chars, MAX_DIALED_CHARS);
                  app_config.dc_index = 0;
		  
		  app_config.offhook = PJ_TRUE;
		  offhook_sound_on();
		  dial_inactivity_timer_restart();
		  app_config.dial_inactivity_count = 0;
		  
                  tcpserv_puts("THINPJ act='hookup', res='ok'");
		}
		else if (menuin[1] == 'd')
		{
		  if (! app_config.offhook)
		  {
		    tcpserv_puts("THINPJ act='hookdown', res='fail', status='Hook allready down.' ");
		    break;
		  }

		  hook_down();
		  
                  tcpserv_puts("THINPJ act='hookdown', res='ok'");
		}

		break;

        case 'd':

            if (  ((menuin[1]>='0') && (menuin[1]<='9')) || (menuin[1]=='*') || (menuin[1]=='#') )
            {
                if (! app_config.offhook)
                {
		    tcpserv_puts("THINPJ act='digit', res='fail', status='Offhook now!' ");
		    break;
                }
                
                offhook_sound_off();
                dial_inactivity_timer_restart();
                app_config.dial_inactivity_count = 0;

                digit_sound(menuin[1]);

                if (current_call == -1) // No current call - means we wanna dial somewhere not send dtmf
                {
                    if (menuin[1] == '#')
                    {
                        dial_with_dialed_chars();
                        
                        break;
                    }
                    else
                    {
                      if (app_config.dc_index<MAX_DIALED_CHARS)
                      {
                        app_config.dialed_chars[app_config.dc_index++] = menuin[1];                      
                        tcpserv_puts("THINPJ act='digit', res='ok', status='Stored.'");
                      }
                      else
                      {
                        tcpserv_puts("THINPJ act='digit', res='fail', status='Number too long.'");
                      }                    
                    }
                
                }
                else
                {
                    // send dtmf digit to the remote party
                    tcpserv_puts("THINPJ act='digit', res='ok', status='Sent to remote party.'");
                }
                
            }

        break;


/*
	case 'a':

	    if (current_call != -1) {
		pjsua_call_get_info(current_call, &call_info);
	    } else {
		call_info.role = PJSIP_ROLE_UAC;
		call_info.state = PJSIP_INV_STATE_DISCONNECTED;
	    }

	    if (current_call == -1 || 
		call_info.role != PJSIP_ROLE_UAS ||
		call_info.state >= PJSIP_INV_STATE_CONNECTING)
	    {
		tcpserv_puts("THINPJ act='answer', res='fail', status='No pending call'");
		continue;

	    }
	    
	    int st_code;
	    pjsua_msg_data msg_data;
	    pjsua_msg_data_init(&msg_data);
	    pjsua_call_answer(current_call, 200, NULL, &msg_data);

	    break;


	case 'h':

	    if (current_call == -1) {
		tcpserv_puts("THINPJ act='hangup', res='fail', status='No current call'");
		continue;
	    }

//	    pjsua_call_hangup_all();
	    pjsua_call_hangup	(current_call, 0, NULL, NULL);
	    break;


	case '#':
	    if (current_call == -1) {
		
		PJ_LOG(3,(THIS_FILE, "No current call"));

	    } else if (!pjsua_call_has_media(current_call)) {

		PJ_LOG(3,(THIS_FILE, "Media is not established yet!"));

	    } else {
		pj_str_t digits;
		int call = current_call;
		pj_status_t status;

		if (!simple_input("DTMF strings to send (0-9*#A-B)", buf, 
				  sizeof(buf)))
		{
			break;
		}

		if (call != current_call) {
		    tcpserv_puts("Call has been disconnected");
		    continue;
		}

		digits = pj_str(buf);
		status = pjsua_call_dial_dtmf(current_call, &digits);
		if (status != PJ_SUCCESS) {
		    pjsua_perror(THIS_FILE, "Unable to send DTMF", status);
		} else {
		    tcpserv_puts("DTMF digits enqueued for transmission");
		}
	    }
	    break;
*/
	case '+':
	    if (menuin[1] == 'a') {

		char id[80], registrar[80], realm[80], uname[80], passwd[30];
		pjsua_acc_config acc_cfg;
		pj_status_t status;

		    strcpy(id,"sip:401@192.168.4.57");
		    strcpy(registrar,"sip:192.168.4.57");
		    strcpy(realm,"192.168.4.57");
		    strcpy(uname,"401");
		    strcpy(passwd,"pass");
/*
		if (!simple_input("Your SIP URL:", id, sizeof(id)))
		    break;
		if (!simple_input("URL of the registrar:", registrar, sizeof(registrar)))
		    break;
		if (!simple_input("Auth Realm:", realm, sizeof(realm)))
		    break;
		if (!simple_input("Auth Username:", uname, sizeof(uname)))
		    break;
		if (!simple_input("Auth Password:", passwd, sizeof(passwd)))
		    break;
*/

		pjsua_acc_config_default(&acc_cfg);
		acc_cfg.id = pj_str(id);
		acc_cfg.reg_uri = pj_str(registrar);
		acc_cfg.cred_count = 1;
		acc_cfg.cred_info[0].scheme = pj_str("Digest");
		acc_cfg.cred_info[0].realm = pj_str(realm);
		acc_cfg.cred_info[0].username = pj_str(uname);
		acc_cfg.cred_info[0].data_type = 0;
		acc_cfg.cred_info[0].data = pj_str(passwd);

		status = pjsua_acc_add(&acc_cfg, PJ_TRUE, NULL);

		if (status != PJ_SUCCESS) {
		    tcpserv_puts("THINPJ act='addaccount', res='fail'");
		}

	    }
	    break;


	case 'q':
	    goto on_exit;


	default:
	    break;
	}
    }

on_exit:
    ;
}


/*****************************************************************************
 * Public API
 */

pj_status_t app_init(int argc, char *argv[])
{
    pjsua_transport_id transport_id = -1;
    pjsua_transport_config tcp_cfg;
    unsigned i;
    pj_status_t status;

    /* Create pjsua */
    status = pjsua_create();
    if (status != PJ_SUCCESS)
	return status;

    /* Create pool for application */
    app_config.pool = pjsua_pool_create("pjsua", 1000, 1000);

    /* Initialize default config */
    default_config(&app_config);
    
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

    /* Initialize calls data */
    for (i=0; i<PJ_ARRAY_SIZE(app_config.call_data); ++i) {
	app_config.call_data[i].timer.id = PJSUA_INVALID_ID;
	app_config.call_data[i].timer.cb = NULL;
    }

    /* dial inavtivity timer init */
    pj_timer_entry_init(&app_config.dial_inactivity_timer, 0, NULL, &dial_inactivity_callback);
    app_config.dial_inactivity_timer.id = PJSUA_INVALID_ID;
    app_config.dial_inactivity_count = 0;
        
    /* dialed chars */
    pj_bzero(&app_config.dialed_chars, MAX_DIALED_CHARS);
    app_config.dc_index = 0;

    /* DTMF player */

    pj_str_t name;
    name = pj_str("dtmf");
    status = pjmedia_tonegen_create2(app_config.pool, &name,
					 8000, 1, 160, 16, 
					 0,  &app_config.dtmf_media_port);
    if (status != PJ_SUCCESS) {
	    pjsua_perror(THIS_FILE, "Unable to create tone generator", status);
	    goto on_error;
    }

    status = pjsua_conf_add_port(app_config.pool, app_config.dtmf_media_port, &app_config.dtmf_conf_port);
    pj_assert(status == PJ_SUCCESS);

    pjsua_conf_connect(app_config.dtmf_conf_port, 0);

    /* over */

    pj_memcpy(&tcp_cfg, &app_config.udp_cfg, sizeof(tcp_cfg));

    /* Create ringback tones */
    if (app_config.no_tones == PJ_FALSE) {
	unsigned i, samples_per_frame;
	pjmedia_tone_desc tone[RING_CNT+RINGBACK_CNT];


	samples_per_frame = app_config.media_cfg.audio_frame_ptime * 
			    app_config.media_cfg.clock_rate *
			    app_config.media_cfg.channel_count / 1000;

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

	/* Ring (to alert incoming call) */
	name = pj_str("ring");
	status = pjmedia_tonegen_create2(app_config.pool, &name, 
					 app_config.media_cfg.clock_rate,
					 app_config.media_cfg.channel_count, 
					 samples_per_frame,
					 16, PJMEDIA_TONEGEN_LOOP, 
					 &app_config.ring_port);
	if (status != PJ_SUCCESS)
	    goto on_error;

	for (i=0; i<RING_CNT; ++i) {
	    tone[i].freq1 = RING_FREQ1;
	    tone[i].freq2 = RING_FREQ2;
	    tone[i].on_msec = RING_ON;
	    tone[i].off_msec = RING_OFF;
	    tone[i].volume = 16383; // make it loud!
	}

	tone[RING_CNT-1].off_msec = RING_INTERVAL;

	pjmedia_tonegen_play(app_config.ring_port, RING_CNT, 
			     tone, PJMEDIA_TONEGEN_LOOP);

	status = pjsua_conf_add_port(app_config.pool, app_config.ring_port,
				     &app_config.ring_slot);
	if (status != PJ_SUCCESS)
	    goto on_error;

	/* offhoook tone */

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

    }

    /* Add UDP transport unless it's disabled. */
    if (!app_config.no_udp) {
	pjsua_acc_id aid;

	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP,
					&app_config.udp_cfg, 
					&transport_id);
	if (status != PJ_SUCCESS)
	    goto on_error;

	/* Add local account */
	pjsua_acc_add_local(transport_id, PJ_TRUE, &aid);
	//pjsua_acc_set_transport(aid, transport_id);
	pjsua_acc_set_online_status(current_acc, PJ_TRUE);

	if (app_config.udp_cfg.port == 0) {
	    pjsua_transport_info ti;
	    pj_sockaddr_in *a;

	    pjsua_transport_get_info(transport_id, &ti);
	    a = (pj_sockaddr_in*)&ti.local_addr;

	    tcp_cfg.port = pj_ntohs(a->sin_port);
	}
    }

    /* Add TCP transport unless it's disabled */
    if (!app_config.no_tcp) {
	status = pjsua_transport_create(PJSIP_TRANSPORT_TCP,
					&tcp_cfg, 
					&transport_id);
	if (status != PJ_SUCCESS)
	    goto on_error;

	/* Add local account */
	pjsua_acc_add_local(transport_id, PJ_TRUE, NULL);
	pjsua_acc_set_online_status(current_acc, PJ_TRUE);

    }

    if (transport_id == -1) {
	PJ_LOG(3,(THIS_FILE, "Error: no transport is configured"));
	status = -1;
	goto on_error;
    }

    /* Add accounts */
    for (i=0; i<app_config.acc_cnt; ++i) {
	status = pjsua_acc_add(&app_config.acc_cfg[i], PJ_TRUE, NULL);
	if (status != PJ_SUCCESS)
	    goto on_error;
	pjsua_acc_set_online_status(current_acc, PJ_TRUE);
    }

    /* Optionally set codec orders */
    for (i=0; i<app_config.codec_cnt; ++i) {
	pjsua_codec_set_priority(&app_config.codec_arg[i],
				 (pj_uint8_t)(PJMEDIA_CODEC_PRIO_NORMAL+i+9));
    }

    /* Optionally disable some codec */
    for (i=0; i<app_config.codec_dis_cnt; ++i) {
	pjsua_codec_set_priority(&app_config.codec_dis[i],PJMEDIA_CODEC_PRIO_DISABLED);
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

    return PJ_SUCCESS;

on_error:
    app_destroy();
    return status;
}


pj_status_t app_main(void)
{
    pj_thread_t *stdout_refresh_thread = NULL;
    pj_status_t status;

    /* Start pjsua */
    status = pjsua_start();
    if (status != PJ_SUCCESS) {
	app_destroy();
	return status;
    }

    console_app_main(&uri_arg);
    return PJ_SUCCESS;
}

pj_status_t app_destroy(void)
{
    pj_status_t status;
    unsigned i;

    /* Close ringback port */
    if (app_config.ringback_port && 
	app_config.ringback_slot != PJSUA_INVALID_ID) 
    {
	pjsua_conf_remove_port(app_config.ringback_slot);
	app_config.ringback_slot = PJSUA_INVALID_ID;
	pjmedia_port_destroy(app_config.ringback_port);
	app_config.ringback_port = NULL;
    }

    /* Close ring port */
    if (app_config.ring_port && app_config.ring_slot != PJSUA_INVALID_ID) {
	pjsua_conf_remove_port(app_config.ring_slot);
	app_config.ring_slot = PJSUA_INVALID_ID;
	pjmedia_port_destroy(app_config.ring_port);
	app_config.ring_port = NULL;
    }

    /* Close offhook port */
    if (app_config.offhook_port && app_config.offhook_slot != PJSUA_INVALID_ID) {
	pjsua_conf_remove_port(app_config.offhook_slot);
	app_config.offhook_slot = PJSUA_INVALID_ID;
	pjmedia_port_destroy(app_config.offhook_port);
	app_config.offhook_port = NULL;
    }



    /* Close DTMF tone generators */
    
    pjsua_conf_remove_port(app_config.dtmf_conf_port);
    app_config.dtmf_conf_port = PJSUA_INVALID_ID;
    
    pjmedia_port_destroy(app_config.dtmf_media_port);
    app_config.dtmf_media_port = NULL;
                
    if (app_config.pool) {
	pj_pool_release(app_config.pool);
	app_config.pool = NULL;
    }

    status = pjsua_destroy();

    pj_bzero(&app_config, sizeof(app_config));

    return status;
}

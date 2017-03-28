
#include "gamix.h"
#include <math.h>

static gchar *label_3d[]={
	"wide","volume","center","space","depth","delay","feedback","depth rear"};
static gchar *label_tone[]={"B","T"};
static gchar *pc1_ptc1[]={"L","F","B"};
static gchar *pc1_ptc2[]={"R","R","U"};
static gchar *sw3_vce[]={"Uu","Mono","L","R","C","R_L","R_R","Woo"};
gint wsize_min=60;

static void close_callback(GtkWidget *,s_mixer_t *);
static void volume1_callback(GtkAdjustment *,s_element_t *);
static void volume1_sw_callback(GtkToggleButton *,s_element_t *);
static void switch1_callback(GtkToggleButton *,s_element_t *);
static void switch2_callback(GtkToggleButton *,s_element_t *);
static void switch3_callback(GtkToggleButton *,s_element_t *);
static void switch3_ss_callback(GtkWidget *,s_element_t *);
static void chain_callback(GtkToggleButton *,s_obj_t *);
static void accu3_callback(GtkAdjustment *,s_element_t *);
static void mux1_callback(GtkItem *,s_element_t *);
static void mux2_callback(GtkItem *,s_element_t *);
static void sw_3d_callback(GtkToggleButton *,s_element_t *);
static void vol_3d_callback(GtkAdjustment *,s_element_t *);
static void sw_tone_callback(GtkToggleButton *,s_element_t *);
static void vol_tone_callback(GtkAdjustment *,s_element_t *);
static void pc1_callback(GtkAdjustment *,s_element_t *);
static void pc1_ss_callback(GtkWidget *,s_element_t *);
static void csw_bool_callback(GtkToggleButton  *,s_sw_t *);
static gint mk_element(s_element_t *,GtkBox *);
static gint mk_sw(s_sw_t *,GtkBox *);
static void cv_name(unsigned char *,int);

static void close_callback(GtkWidget *w,s_mixer_t *mixer) {
	s_obj_t *obj;

	/*
	for( i=0 ; i<mixer->groups.groups ; i++ ) {
		g=&mixer->group[i];
		g->enabled=FALSE;
	}
	for( i=0 ; i<mixer->ee_n ; i++ ) {
		ee=&mixer->ee[i];
		ee->enabled=FALSE;
	}
	for( g=mixer->group; g != NULL ; g=g->next ) g->enabled=FALSE;
	for( ee=mixer->ee; ee != NULL ; ee=ee->next ) ee->enabled=FALSE;
	*/
	for( obj=mixer->obj ; obj != NULL ; obj=obj->next ) obj->enabled=FALSE;
		
	snd_mixer_close(mixer->handle);
	mixer->handle=NULL;
}

static void volume1_sw_callback(GtkToggleButton *b,s_element_t *e) {
	int i,j,value,err;

	for( i=0 ; i<e->Evol1(e.) ; i++ ) {
		if( b == GTK_TOGGLE_BUTTON(e->w[i]) ) break;
	}
	value=b->active?1:0;
	if( e->Epvol1(e.)[i] == value ) return;
	if( e->Evol1(e.) > 1 && *e->chain ) {
		for( j=0 ; j<e->Evol1(e.) ; j++ ) {
			e->Epvol1(e.)[j]=value;
			if( j!= i ) {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(e->w[j]),b->active);
			}
		}
	} else {
		e->Epvol1(e.)[i]=value;
	}
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
	if( err < 0 ) {
		sprintf(err_msg,_("mixer element write error: %s\n"),snd_strerror(err));
		pr_err(err_msg);
	}
}

static void volume1_callback(GtkAdjustment *adj,s_element_t *e) {
	int i,j,value,err;

	for( i=0 ; i<e->Evol1(e.) ; i++ ) {
		if( adj == e->adj[i] ) break;
	}
	value=-(int)adj->value;
	if( e->Epvol1(e.)[i] == value ) return;
	if( e->Evol1(e.) > 1 && *e->chain ) {
		for( j=0 ; j<e->Evol1(e.) ; j++ ) {
			e->Epvol1(e.)[j]=value;
			if( j!= i ) {
				e->adj[j]->value=adj->value;
				gtk_signal_emit_by_name(GTK_OBJECT(e->adj[j]),"value_changed");
			}
		}
	} else {
		e->Epvol1(e.)[i]=value;
	}
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
	if( err < 0 ) {
		sprintf(err_msg,_("mixer element write error: %s\n"),snd_strerror(err));
		pr_err(err_msg);
	}
}

static void switch1_callback(GtkToggleButton *b,s_element_t *e ) {
	int i,j;

	for( i=0 ; i<e->e.data.switch1.sw; i++ ) {
		if( b == (GtkToggleButton *)e->w[i] ) break;
	}
	if(	(snd_mixer_get_bit(e->e.data.switch1.psw,i)?TRUE:FALSE) == b->active )
		return;
	if( e->e.data.switch1.sw > 1 && *e->chain ) {
		for( j=0 ; j<e->e.data.switch1.sw; j++ ) {
			snd_mixer_set_bit(e->e.data.switch1.psw,j,b->active);
			if( j != i )
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(e->w[j]),b->active);
		}
	} else {
		snd_mixer_set_bit(e->e.data.switch1.psw,i,b->active);
	}
	snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
}

static void switch2_callback(GtkToggleButton *b,s_element_t *e ) {
	int err;

	e->e.data.switch2.sw=b->active;
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
}

static void switch3_callback(GtkToggleButton *b,s_element_t *e ) {
	int i;

	for( i=0 ; i < e->e.data.switch3.rsw ; i++ ) {
		if( b == (GtkToggleButton *)e->w[i] ) break;
	}
	if(	(snd_mixer_get_bit(e->e.data.switch1.psw,i)?TRUE:FALSE) == b->active )
		return;
	snd_mixer_set_bit(e->e.data.switch3.prsw,i,b->active);
	snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
}

static void switch3_ss_callback(GtkWidget *w,s_element_t *e ) {
	int i,j,k,l;
	for( i=e->e.data.switch3.rsw ; i<e->e.data.switch3.rsw+3 ; i++ ) {
		if( w == e->w[i] ) break;
	}
	switch( i-e->e.data.switch3.rsw ) {
	case 0:
		for( i=0 ; i<e->info.data.switch3.voices ; i++ ) {
			for( j=0 ; j<e->info.data.switch3.voices ; j++ ) {
				if( i==j ) k=1; else k=0;
				l=i*e->info.data.switch3.voices+j;
				snd_mixer_set_bit(e->e.data.switch3.prsw,l,k);
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(e->w[l]),k);
			}
		}
		break;
	case 1:
		for( i=0 ; i<e->info.data.switch3.voices ; i++ ) {
			for( j=0 ; j<e->info.data.switch3.voices ; j++ ) {
				if( i==(e->info.data.switch3.voices-1-j) ) k=1; else k=0;
				l=i*e->info.data.switch3.voices+j;
				snd_mixer_set_bit(e->e.data.switch3.prsw,l,k);
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(e->w[l]),k);
			}
		}
		break;
	case 2:
		for( i=0 ; i<e->e.data.switch3.rsw ; i++ ) {
			snd_mixer_set_bit(e->e.data.switch3.prsw,i,0);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(e->w[i]),00);
		}
		break;
	default:
		return;
	}
	snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
}
static void chain_callback(GtkToggleButton *b,s_obj_t *obj ) {
	obj->chain = b->active;
	/*
	printf("obj ");
	if( obj->g ) printf("gid '%s',%d ",obj->g->g.gid.name,obj->g->g.gid.index);
	if( obj->e ) printf("eid '%s',%d,%d ",obj->e->e.e.eid.name,
						obj->e->e.e.eid.index,obj->e->e.e.eid.type);
	printf(" %s\n",obj->chain?"TRUE":"FALSE");
	*/
}

static void accu3_callback(GtkAdjustment *adj,s_element_t *e) {
	int i,j,value,err;

	for( i=0 ; i<e->Eaccu3(e.) ; i++ ) {
		if( adj == e->adj[i] ) break;
	}
	value=-(int)adj->value;
	if( e->Epaccu3(e.)[i] == value ) return;
	if( e->Eaccu3(e.) > 1 && *e->chain ) {
		for( j=0 ; j<e->Eaccu3(e.) ; j++ ) {
			e->Epaccu3(e.)[j]=value;
			if( j!= i ) {
				e->adj[j]->value=adj->value;
				gtk_signal_emit_by_name(GTK_OBJECT(e->adj[j]),"value_changed");
			}
		}
	} else {
		e->Epaccu3(e.)[i]=value;
	}
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
	if( err < 0 ) {
		sprintf(err_msg,_("mixer element write error: %s\n"),snd_strerror(err));
		pr_err(err_msg);
	}
}

static void mux1_callback(GtkItem *item,s_element_t *e ) {
	int i,ch,no,err;

	ch=(int)gtk_object_get_data(GTK_OBJECT(item),"ch");
	no=(int)gtk_object_get_data(GTK_OBJECT(item),"no");

	if( strcmp(e->mux[no].name,e->Epmux1(e.)[ch].name) == 0 &&
		e->mux[no].index == e->Epmux1(e.)[ch].index &&
		e->mux[no].type == e->Epmux1(e.)[ch].type ) return;

	if( *e->chain ) {
		for( i=0 ; i<e->Emux1(e.) ; i++ ) {
			e->Epmux1(e.)[i]=e->mux[no];
			if( ch != i ) gtk_option_menu_set_history(
											  GTK_OPTION_MENU(e->w[i]),no);
		}
	} else {
		e->Epmux1(e.)[ch]=e->mux[no];
	}
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
	if( err< 0 ) {
		sprintf(err_msg,_("mixer mux1 element write error: %s\n"),snd_strerror(err));
		pr_err(err_msg);
	}
}

static void mux2_callback(GtkItem *item,s_element_t *e ) {
	int no,err;

	no=(int)gtk_object_get_data(GTK_OBJECT(item),"no");

	if( strcmp(e->mux[no].name,e->Emux2(e.).name) == 0 &&
		e->mux[no].index == e->Emux2(e.).index &&
		e->mux[no].type == e->Emux2(e.).type ) return;

	e->Emux2(e.)=e->mux[no];
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
	if( err< 0 ) {
		sprintf(err_msg,_("mixer mux1 element write error: %s\n"),snd_strerror(err));
		pr_err(err_msg);
	}
}

static void sw_3d_callback(GtkToggleButton *b,s_element_t *e ) {
	int err;

	if( b == (GtkToggleButton *)e->w[0] ) {
		e->e.data.teffect1.sw = b->active;
	} else {
		e->e.data.teffect1.mono_sw = b->active;
	}
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
}

static void vol_3d_callback(GtkAdjustment *adj,s_element_t *e) {
	int i,err,*v,value;

	for( i=0 ; i<7 ; i++ ) {
		if( adj == e->adj[i] ) break;
	}
	v=NULL;
	switch( i ) {
	case 0:
		v=&e->e.data.teffect1.wide;
		break;
	case 1:
		v=&e->e.data.teffect1.volume;
		break;
	case 2:
		v=&e->e.data.teffect1.center;
		break;
	case 3:
		v=&e->e.data.teffect1.space;
		break;
	case 4:
		v=&e->e.data.teffect1.depth;
		break;
	case 5:
		v=&e->e.data.teffect1.delay;
		break;
	case 6:
		v=&e->e.data.teffect1.feedback;
		break;
	case 7:
		v=&e->e.data.teffect1.depth_rear;
		break;
	}
	value=(int)adj->value;
	if( v ) {
		if( value == *v ) return;
		*v=value;
	} else return;
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
	if( err<0 ) {
		sprintf(err_msg,_("3D effect write error: %s\n"),snd_strerror(err));
		pr_err(err_msg);
	}
}

static void sw_tone_callback(GtkToggleButton *b,s_element_t *e ) {
	int err;

	e->e.data.tc1.sw = b->active;
	e->e.data.tc1.tc=SND_MIXER_TC1_SW;
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
}

static void vol_tone_callback(GtkAdjustment *adj,s_element_t *e) {
	int i,err,*v,value;

	for( i=0 ; i<2 ; i++ ) {
		if( adj == e->adj[i] ) break;
	}
	v=NULL;
	switch( i ) {
	case 0:
		v=&e->e.data.tc1.bass;
		e->e.data.tc1.tc=SND_MIXER_TC1_BASS;
		break;
	case 1:
		v=&e->e.data.tc1.treble;
		e->e.data.tc1.tc=SND_MIXER_TC1_TREBLE;
		break;
	}
	value=-(int)adj->value;
	if( v ) {
		if( value == *v ) return;
		*v=value;
	} else return;
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
	if( err<0 ) {
		sprintf(err_msg,_("Tone controll write error: %s\n"),snd_strerror(err));
		pr_err(err_msg);
	}
}

static void pc1_callback(GtkAdjustment *adj,s_element_t *e) {
	int i,err,value;

	value=(int)adj->value;
	for( i=0 ; i<e->e.data.pc1.pan; i++ ) {
		if( adj == e->adj[i] ) break;
	}
	if( i==e->e.data.pc1.pan ) {
		pr_err(_("Pan err.\n"));
		return;
	}
	if( e->e.data.pc1.ppan[i]==value ) return;

	e->e.data.pc1.ppan[i]=value;
	err=snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
	if( err<0 ) {
		sprintf(err_msg,_("PAN controll write error: %s\n"),snd_strerror(err));
		pr_err(err_msg);
	}
}

static void pc1_ss_callback(GtkWidget *w,s_element_t *e) {
	int i,j,k;

	j=1;
	for( i=0 ; i<e->e.data.pc1.pan; i++ ) {
		if( w == e->w[j] ) {
			k=e->info.data.pc1.prange[i].min;
			break;
		}
		j++;
		if( w == e->w[j] ) {
			k=(e->info.data.pc1.prange[i].min+e->info.data.pc1.prange[i].max)/2;
			break;
		}
		j++;
		if( w == e->w[j] ) {
			k=e->info.data.pc1.prange[i].max;
			break;
		}
		j+=2;
	}
	if( i<e->e.data.pc1.pan ) {
		if( e->e.data.pc1.ppan[i] == k ) return;
		e->adj[i]->value=(gfloat)k;
		gtk_signal_emit_by_name(GTK_OBJECT(e->adj[i]),"value_changed");
	}
}
static void csw_bool_callback(GtkToggleButton  *b,s_sw_t *sw) {
	int err;

	sw->sw.value.enable=b->active;
#if SND_LIB_VERSION < SND_PROTOCOL_VERSION(0,5,5)
	err=snd_ctl_mixer_switch_write(cards[sw->card].chandle,sw->mdev,&sw->sw);
#else
	err=snd_ctl_switch_write(cards[sw->card].chandle,&sw->sw);
#endif
#if DISP_DEBUG
	if( err<0 ) printf("err %d '%s' to %d\n",err,sw->sw.name,b->active);
#endif
}

#define make_frame \
	obj->v_frame=frame=gtk_frame_new(NULL); \
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_OUT); \
	gtk_box_pack_start(GTK_BOX(mh_box),frame, \
					   mixer->p_e,mixer->p_f,0); \
	iv_box=gtk_vbox_new(FALSE,0); \
	gtk_container_add(GTK_CONTAINER(frame),iv_box); \
	obj->title_b=gtk_label_new(NULL); \
	gtk_box_pack_start(GTK_BOX(iv_box),obj->title_b,FALSE,FALSE,0); \
	obj->chain_en=FALSE;

#define chk_name \
	gtk_widget_size_request(iv_box,&rq2); \
	c_l=gtk_label_new(gname); \
	gtk_widget_show(c_l); \
	if( rq2.width < wsize_min ) rq2.width = wsize_min; \
	gtk_widget_size_request(c_l,&rq1); \
	gtk_widget_destroy(c_l); \
	if( rq1.width > rq2.width ) { \
		cv_name(gname,rq2.width); \
		for( j=0; gname[j]!='\n' ; j++); \
		gtk_label_set(GTK_LABEL(obj->title_b),gname+j+1); \
		gtk_widget_show(obj->title_b); \
		gname[j]=0; \
	}

GtkWidget *make_mixer( gint c_n , gint m_n ) {
	int j,k,err;
	GtkWidget *mv_box,*m_name;
	GtkWidget *s_win;
	GtkWidget *mh_box;
	GtkWidget *frame;
	GtkWidget *iv_box;
	GtkWidget *ih_box;
	GtkWidget *c_l;
	GtkRequisition rq1,rq2;
	char gname[128];
	s_mixer_t *mixer;
	s_group_t *group=NULL;
	s_element_t *e;
	s_eelements_t *ee;
	s_sw_t *sw;
	s_obj_t *obj;

	if( cards[c_n].mixer[m_n].handle ) {
		snd_mixer_close(cards[c_n].mixer[m_n].handle);
	}
	if( (err=snd_mixer_open(&cards[c_n].mixer[m_n].handle,c_n,m_n)) < 0 ) {
		return NULL;
	}
	if( cards[c_n].chandle ) {
		snd_ctl_close(cards[c_n].chandle);
	}
	if( (err=snd_ctl_open(&cards[c_n].chandle,c_n)) < 0 ) {
		return NULL;
	}

	mixer = &cards[c_n].mixer[m_n];

	mv_box=gtk_vbox_new(FALSE,0);
	gtk_widget_show(mv_box);

	sprintf(gname,"%s:%s",cards[c_n].info.name,
			cards[c_n].mixer[m_n].info.name);
	m_name=gtk_label_new(gname);
	gtk_box_pack_start(GTK_BOX(mv_box),m_name,FALSE,FALSE,0);
	gtk_widget_show(m_name);

	mh_box=gtk_hbox_new(FALSE,2);
	if( conf.scroll ) {
		s_win=gtk_scrolled_window_new(NULL,NULL);
		gtk_box_pack_start(GTK_BOX(mv_box),s_win,TRUE,TRUE,0);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(s_win),
									   GTK_POLICY_AUTOMATIC,
									   GTK_POLICY_NEVER);
		gtk_widget_show(s_win);
		gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(s_win),
											  mh_box);
		//gtk_container_add(GTK_CONTAINER(s_win),mh_box);
	} else {
		gtk_box_pack_start(GTK_BOX(mv_box),mh_box,TRUE,TRUE,4);
	}
	gtk_widget_show(mh_box);

	for( obj=mixer->obj ; obj != NULL ; obj=obj->next ) {
		if( obj->g ) {
			group=obj->g;
			k=0;
			for( j=0 ; j<group->g.elements ; j++ ) {
				if( group->e[j].e.eid.type ) k++;
			}
			if( k==0 && obj->dyn_e == 0) obj->enable=FALSE;
			if( obj->enable && (obj->dyn_e == 0 || obj->dyn_e == 3) ) {
				make_frame;
				for( j=0 ; j<group->g.elements ; j++ ) {
					e=&group->e[j];
					e->chain = &obj->chain;
					e->chain_en = &obj->chain_en;
					if( mk_element(e,GTK_BOX(iv_box))<0 ) return NULL;
				}
				if( group->g.gid.index > 0 ) {
					sprintf(gname,"%s %d",group->g.gid.name,
							group->g.gid.index);
				} else {
					sprintf(gname,"%s",group->g.gid.name);
				}
				ih_box=gtk_hbox_new(FALSE,2);
				gtk_box_pack_end(GTK_BOX(iv_box),ih_box,FALSE,FALSE,0);
				if( obj->chain_en ) {
					obj->cwb=gtk_toggle_button_new();
					gtk_box_pack_start(GTK_BOX(ih_box),obj->cwb,
									   FALSE,FALSE,4);
					gtk_widget_set_usize(obj->cwb,10,10);
					gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(obj->cwb)
												,obj->chain);
					gtk_widget_show(obj->cwb);
					gtk_signal_connect(GTK_OBJECT(obj->cwb),"toggled",
									   GTK_SIGNAL_FUNC(chain_callback),
									   (gpointer)obj);
					c_l=gtk_label_new(_("Lock"));
					gtk_box_pack_start(GTK_BOX(ih_box),c_l,FALSE,FALSE,0);
					gtk_widget_show(c_l);
					gtk_widget_show(ih_box);
				} else {
					c_l=gtk_label_new(" ");
					gtk_box_pack_start(GTK_BOX(ih_box),c_l,FALSE,FALSE,0);
					gtk_widget_show(c_l);
				}
				gtk_widget_show(ih_box);
				gtk_widget_show(iv_box);

				chk_name;
				gtk_frame_set_label(GTK_FRAME(frame),gname);
				gtk_widget_show(frame);

				obj->enabled=TRUE;
			} else {
				obj->enabled=FALSE;
			}
		}
		if( obj->e ) {
			if( obj->enable && (obj->dyn_e == 0 || obj->dyn_e == 3)) {
				ee=obj->e;
				e=&ee->e;
				make_frame;
				e->chain=&obj->chain;
				e->chain_en=&obj->chain_en;
				if( mk_element(e,GTK_BOX(iv_box))<0 ) return NULL;
				ih_box=gtk_hbox_new(FALSE,2);
				gtk_box_pack_end(GTK_BOX(iv_box),ih_box,FALSE,FALSE,0);
				if( e->e.eid.index > 0 ) {
					sprintf(gname,"%s%d",e->e.eid.name,e->e.eid.index);
				} else {
					sprintf(gname,"%s",e->e.eid.name);
				}
				if( obj->chain_en ) {
					obj->cwb=gtk_toggle_button_new();
					gtk_box_pack_start(GTK_BOX(ih_box),obj->cwb,FALSE,FALSE,4);
					gtk_widget_set_usize(obj->cwb,10,10);
					gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(obj->cwb)
												,obj->chain);
					gtk_widget_show(obj->cwb);
					gtk_signal_connect(GTK_OBJECT(obj->cwb),"toggled",
									   GTK_SIGNAL_FUNC(chain_callback),
									   (gpointer)obj);
					c_l=gtk_label_new(_("Lock"));
					gtk_box_pack_start(GTK_BOX(ih_box),c_l,FALSE,FALSE,0);
					gtk_widget_show(c_l);
					gtk_widget_show(ih_box);
				} else {
					c_l=gtk_label_new(" ");
					gtk_box_pack_start(GTK_BOX(ih_box),c_l,FALSE,FALSE,0);
					gtk_widget_show(c_l);
				}
				gtk_widget_show(ih_box);
				gtk_widget_show(iv_box);

				chk_name;
				gtk_frame_set_label(GTK_FRAME(frame),gname);
				gtk_widget_show(frame);
				obj->enabled=TRUE;
			} else {
				obj->enabled=FALSE;
			}
		}
		if( obj->sw ) {
			if( obj->enable && (obj->dyn_e == 0 || obj->dyn_e == 3)) {
				sw=obj->sw;
				make_frame;
				if( mk_sw(sw,GTK_BOX(iv_box))<0 ) return NULL;

				sprintf(gname,"%s",sw->sw.name);
				gtk_widget_show(iv_box);
				chk_name;
				gtk_frame_set_label(GTK_FRAME(frame),gname);
				gtk_widget_show(frame);
				obj->enabled=TRUE;
			} else {
				obj->enabled=FALSE;
			}
		}
	}
	gtk_signal_connect(GTK_OBJECT(mv_box),"destroy",
					   GTK_SIGNAL_FUNC(close_callback),(gpointer)mixer);
	mixer->enabled=TRUE;
	return mv_box;
}

#define MIX_3D_VOL(NO,name,min_name,max_name,sname) \
  if( e->info.data.teffect1.effect & sname ) { \
    ih_box=gtk_hbox_new(FALSE,2); \
    gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,0); \
    c_l=gtk_label_new(label_3d[NO]); \
	gtk_box_pack_start(GTK_BOX(ih_box),c_l,FALSE,FALSE,0); \
	gtk_widget_show(c_l); \
	gtk_widget_show(ih_box); \
	e->adj[NO]=(GtkAdjustment *)gtk_adjustment_new( \
	  (gfloat)e->e.data.teffect1.name, \
	  (gfloat)e->info.data.teffect1.min_name-0.5, \
	  (gfloat)e->info.data.teffect1.max_name+1.0, \
	  1.0,1.0,1.0); \
	gtk_signal_connect(GTK_OBJECT(e->adj[NO]), \
	  "value_changed",GTK_SIGNAL_FUNC(vol_3d_callback),(gpointer)e);\
	e->w[NO+2]=gtk_hscale_new(GTK_ADJUSTMENT(e->adj[NO])); \
	gtk_scale_set_draw_value(GTK_SCALE(e->w[NO+2]),FALSE); \
	gtk_box_pack_start(GTK_BOX(iv_box), e->w[NO+2],FALSE,FALSE,4); \
	gtk_widget_show(e->w[NO+2]); \
  } else { ;\
	e->w[NO+2]=NULL; \
	e->adj[NO]=NULL; \
  }
#define MIX_TONE_VOL(NO,name,min_name,max_name,sname) \
  if( e->info.data.tc1.tc & sname ) { \
    tv_box = gtk_vbox_new(FALSE,2); \
    gtk_box_pack_start(GTK_BOX(ih_box),tv_box,TRUE,TRUE,0); \
    c_l=gtk_label_new(label_tone[NO]); \
    gtk_box_pack_start(GTK_BOX(tv_box),c_l,FALSE,FALSE,0); \
	gtk_widget_show(c_l); \
	e->adj[NO]=(GtkAdjustment *)gtk_adjustment_new( \
	  -(gfloat)e->e.data.tc1.name, \
	  -(gfloat)e->info.data.tc1.max_name-0.5, \
	  -(gfloat)e->info.data.tc1.min_name+0.5, \
	  1.0,4.0,1.0); \
	gtk_signal_connect(GTK_OBJECT(e->adj[NO]), \
	  "value_changed",GTK_SIGNAL_FUNC(vol_tone_callback),(gpointer)e);\
	e->w[NO+1]=gtk_vscale_new(GTK_ADJUSTMENT(e->adj[NO])); \
	gtk_scale_set_draw_value(GTK_SCALE(e->w[NO+1]),FALSE); \
	gtk_box_pack_start(GTK_BOX(tv_box), e->w[NO+1],FALSE,FALSE,4); \
	gtk_widget_show(e->w[NO+1]); \
	gtk_widget_show(tv_box); \
  } else { ;\
	e->w[NO+1]=NULL; \
	e->adj[NO]=NULL; \
  }

gint mk_element(s_element_t *e,GtkBox *iv_box) {
	int i,j,k;
	GtkWidget *ih_box,*tv_box;
	GtkWidget *menu,*c_l,*item;
	GtkWidget *tbl;
	GtkTooltips *tp;

	ih_box=gtk_hbox_new(TRUE,0);
	switch( e->e.eid.type) {
	case SND_MIXER_ETYPE_VOLUME1:
		if( (e->info.data.volume1.prange[0].max-
			 e->info.data.volume1.prange[0].min) == 1 ) {
			gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,0);
		} else
			gtk_box_pack_start(iv_box,ih_box,TRUE,TRUE,0);
		if( e->Evol1(e.) > 1 ) {
			*e->chain_en=TRUE;
			*e->chain=TRUE;
		}
		if( e->w == NULL ) {
			e->w = (GtkWidget **)g_malloc( e->Evol1(e.)*sizeof(GtkWidget *));
		}
		if( e->w == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		if( e->adj == NULL ) {
			e->adj=(GtkAdjustment **)g_malloc(e->Evol1(e.)*sizeof(GtkAdjustment *));
		}
		if( e->adj==NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		for( i=0 ; i<e->Evol1(e.) ; i++ ) {
			if( (e->info.data.volume1.prange[i].max-
				 e->info.data.volume1.prange[i].min) == 1 ) {
				e->adj[i]=NULL;
				e->w[i]=gtk_toggle_button_new_with_label("V");
				gtk_box_pack_start(GTK_BOX(ih_box),e->w[i],
								   FALSE,FALSE,0);
				gtk_toggle_button_set_state(
									GTK_TOGGLE_BUTTON(e->w[i]),
									e->Epvol1(e.)[i]);
				gtk_signal_connect(GTK_OBJECT(e->w[i]),"toggled",
								   GTK_SIGNAL_FUNC(volume1_sw_callback),
								   (gpointer)e);
				gtk_widget_show(e->w[i]);
			} else {
				e->adj[i]=(GtkAdjustment *)gtk_adjustment_new(
						-(gfloat)e->Epvol1(e.)[i],
						-(gfloat)e->info.data.volume1.prange[i].max-0.5,
						-(gfloat)e->info.data.volume1.prange[i].min+0.5,
						1.0,4.0,1.0);
				gtk_signal_connect(GTK_OBJECT(e->adj[i]),"value_changed",
								   GTK_SIGNAL_FUNC(volume1_callback),
								   (gpointer)e);
				e->w[i]=gtk_vscale_new(GTK_ADJUSTMENT(e->adj[i]));
				gtk_scale_set_draw_value(GTK_SCALE(e->w[i]),FALSE);
				gtk_box_pack_start(GTK_BOX(ih_box),e->w[i],FALSE,FALSE,4);
				gtk_widget_show(e->w[i]);
			}
		}
		break;
	case SND_MIXER_ETYPE_SWITCH1:
		gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,4);
		if( e->e.data.switch1.sw > 1 ) {
			*e->chain_en=TRUE;
			*e->chain=TRUE;
		}
		if( e->w == NULL ) {
			e->w = (GtkWidget **)g_malloc( e->e.data.switch1.sw *
										   sizeof(GtkWidget *));
		}
		if( e->w == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		for( i=0 ; i<e->e.data.switch1.sw ; i++ ) {
			e->w[i]=gtk_toggle_button_new();
			gtk_box_pack_start(GTK_BOX(ih_box),e->w[i],FALSE,FALSE,0);
			gtk_widget_set_usize(e->w[i],10,10);
			gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(e->w[i]),
									snd_mixer_get_bit(e->e.data.switch1.psw,i)
										);
			gtk_signal_connect(GTK_OBJECT(e->w[i]),"toggled",
							   GTK_SIGNAL_FUNC(switch1_callback),(gpointer)e);
			tp=gtk_tooltips_new();
			gtk_tooltips_set_tip(tp,e->w[i],e->e.eid.name,NULL);
			//gtk_tooltips_set_colors(tp,,);
			gtk_widget_show(e->w[i]);
		}
		break;
	case SND_MIXER_ETYPE_SWITCH2:
		gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,4);
		if( e->w == NULL ) {
			e->w = (GtkWidget **)g_malloc(sizeof(GtkWidget *));
		}
		if( e->w == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		e->w[0]=gtk_toggle_button_new();
		gtk_box_pack_start(GTK_BOX(ih_box),e->w[0],FALSE,FALSE,0);
		gtk_widget_set_usize(e->w[0],10,10);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(e->w[0]),
									e->e.data.switch2.sw);
		gtk_signal_connect(GTK_OBJECT(e->w[0]),"toggled",
						   GTK_SIGNAL_FUNC(switch2_callback),
						   (gpointer)e);
		tp=gtk_tooltips_new();
		gtk_tooltips_set_tip(tp,e->w[0],e->e.eid.name,NULL);
		//gtk_tooltips_set_colors(tp,,);
		gtk_widget_show(e->w[0]);
		break;
	case SND_MIXER_ETYPE_SWITCH3:
		if (e->w == NULL) {
			e->w = (GtkWidget **)g_malloc( (e->e.data.switch3.rsw+3) *
										   sizeof(GtkWidget *));
		}
		if (e->w == NULL) {
			fprintf(stderr,nomem_msg);
			return -1;
		}
		
		j = e->info.data.switch3.voices;
		tbl = gtk_table_new(j+2,j+2,FALSE);
		for (i = 0; i < e->info.data.switch3.voices ; i++) {
			snd_mixer_voice_t voice;
			char s[256];
			GtkWidget *label;
			voice = e->info.data.switch3.pvoices[i];
			if( voice.vindex ) {
				sprintf(s,"%i",voice.voice);
			} else {
				strcpy(s,sw3_vce[voice.voice]);
			}
			label=gtk_label_new(s);
			gtk_table_attach(GTK_TABLE(tbl),label,i+2,i+3,1,2,
							 GTK_EXPAND,GTK_EXPAND,2,2);
			gtk_widget_show(label);
			label=gtk_label_new(s);
			gtk_table_attach(GTK_TABLE(tbl),label,1,2,i+2,i+3,
							 GTK_EXPAND,GTK_EXPAND,2,2);
			gtk_widget_show(label);
			for (j = 0; j < e->info.data.switch3.voices ; j++) {
				k=i*e->info.data.switch3.voices+j;
				e->w[k]=gtk_toggle_button_new();
				gtk_table_attach(GTK_TABLE(tbl),e->w[k],j+2,j+3,i+2,i+3,
								 GTK_EXPAND,GTK_EXPAND,2,2);
				gtk_widget_set_usize(e->w[k],8,8);
				gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(e->w[k]),
											snd_mixer_get_bit(e->e.data.switch3.prsw, k)
											);
				gtk_signal_connect(GTK_OBJECT(e->w[k]),"toggled",
								   GTK_SIGNAL_FUNC(switch3_callback),(gpointer)e);
#if 0
				tp=gtk_tooltips_new();
				gtk_tooltips_set_tip(tp,e->w[k],e->e.eid.name,NULL);
#endif
				gtk_widget_show(e->w[k]);
			}
		}
		gtk_widget_show(tbl);
		gtk_box_pack_start(iv_box,tbl,FALSE,FALSE,0);

		i=e->e.data.switch3.rsw;
		e->w[i]=gtk_button_new_with_label("=");
		gtk_signal_connect(GTK_OBJECT(e->w[i]),"clicked",
						   GTK_SIGNAL_FUNC(switch3_ss_callback),(gpointer)e);
		gtk_widget_show(e->w[i]);
		gtk_box_pack_start(GTK_BOX(ih_box),e->w[i++],FALSE,FALSE,4);

		e->w[i]=gtk_button_new_with_label("X");
		gtk_signal_connect(GTK_OBJECT(e->w[i]),"clicked",
						   GTK_SIGNAL_FUNC(switch3_ss_callback),(gpointer)e);
		gtk_widget_show(e->w[i]);
		gtk_box_pack_start(GTK_BOX(ih_box),e->w[i++],FALSE,FALSE,4);

		e->w[i]=gtk_button_new_with_label("CL");
		gtk_signal_connect(GTK_OBJECT(e->w[i]),"clicked",
						   GTK_SIGNAL_FUNC(switch3_ss_callback),(gpointer)e);
		gtk_widget_show(e->w[i]);
		gtk_box_pack_start(GTK_BOX(ih_box),e->w[i++],FALSE,FALSE,4);

		gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,4);
		break;
	case SND_MIXER_ETYPE_ACCU3:
		gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,0);
		if( e->Eaccu3(e.) > 1 ) {
			*e->chain_en=TRUE;
			*e->chain=TRUE;
		}
		if( e->w == NULL ) {
			e->w = (GtkWidget **)g_malloc(e->Eaccu3(e.) *
										  sizeof(GtkWidget *));
		}
		if( e->w == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		if( e->adj == NULL ) {
			e->adj=(GtkAdjustment **)g_malloc(e->Eaccu3(e.)*sizeof(GtkAdjustment *));
		}
		if( e->adj==NULL ) {
			printf(nomem_msg);
			return -1;
		}
		for( i=0 ; i<e->Eaccu3(e.) ; i++ ) {
			e->adj[i]=(GtkAdjustment *)gtk_adjustment_new(
								-(gfloat)e->Epaccu3(e.)[i],
								-(gfloat)e->info.data.accu3.prange[i].max-0.5,
								-(gfloat)e->info.data.accu3.prange[i].min+0.5,
								1.0,1.0,1.0);
			gtk_signal_connect(GTK_OBJECT(e->adj[i]),"value_changed",
							   GTK_SIGNAL_FUNC(accu3_callback),(gpointer)e);
			e->w[i]=gtk_vscale_new(GTK_ADJUSTMENT(e->adj[i]));
			gtk_scale_set_draw_value(GTK_SCALE(e->w[i]),FALSE);
			gtk_box_pack_start(GTK_BOX(ih_box),e->w[i],FALSE,FALSE,4);
			gtk_widget_show(e->w[i]);
		}
		break;
	case SND_MIXER_ETYPE_MUX1:
		if( e->Emux1(e.) > 1 ) {
			*e->chain_en=TRUE;
			*e->chain=TRUE;
		}
		if( e->w == NULL ) {
			e->w = (GtkWidget **)g_malloc(e->Emux1(e.) * sizeof(GtkWidget *));
		}
		if( e->w == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		for( i=0 ; i<e->Emux1(e.) ; i++ ) {
			e->w[i]=gtk_option_menu_new();
			menu=gtk_menu_new();
			k=0;
			for( j=0 ; j<e->mux_n; j++ ) {
				if( strcmp(e->mux[j].name,e->Epmux1(e.)[i].name)==0 &&
					e->mux[j].index == e->Epmux1(e.)[i].index &&
					e->mux[j].type == e->Epmux1(e.)[i].type ) k=j;
				item=gtk_menu_item_new_with_label(e->mux[j].name);
				gtk_object_set_data(GTK_OBJECT(item),"ch",(gpointer)i);
				gtk_object_set_data(GTK_OBJECT(item),"no",(gpointer)j);
				gtk_signal_connect(GTK_OBJECT(item),"activate",
								   GTK_SIGNAL_FUNC(mux1_callback),(gpointer)e);
				gtk_menu_append(GTK_MENU(menu),item);
				gtk_widget_show(item);
			}
			gtk_option_menu_set_menu(GTK_OPTION_MENU(e->w[i]),menu);
			gtk_box_pack_start(iv_box,e->w[i],FALSE,FALSE,4);
			gtk_widget_show(e->w[i]);
			gtk_option_menu_set_history(GTK_OPTION_MENU(e->w[i]),k);
		}
		break;
	case SND_MIXER_ETYPE_MUX2:
		if( e->w == NULL ) {
			e->w = (GtkWidget **)g_malloc(sizeof(GtkWidget *));
		}
		if( e->w == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		e->w[0]=gtk_option_menu_new();
		menu=gtk_menu_new();
		k=0;
		for( j=0 ; j<e->mux_n; j++ ) {
			if( strcmp(e->mux[j].name,e->Emux2(e.).name)==0 &&
				e->mux[j].index == e->Emux2(e.).index &&
				e->mux[j].type == e->Emux2(e.).type ) k=j;
			item=gtk_menu_item_new_with_label(e->mux[j].name);
			gtk_object_set_data(GTK_OBJECT(item),"no",(gpointer)j);
			gtk_signal_connect(GTK_OBJECT(item),"activate",
							   GTK_SIGNAL_FUNC(mux2_callback),(gpointer)e);
			gtk_menu_append(GTK_MENU(menu),item);
			gtk_widget_show(item);
		}
		gtk_option_menu_set_menu(GTK_OPTION_MENU(e->w[0]),menu);
		gtk_box_pack_start(iv_box,e->w[0],FALSE,FALSE,4);
		gtk_widget_show(e->w[0]);
		gtk_option_menu_set_history(GTK_OPTION_MENU(e->w[0]),k);
		break;
	case SND_MIXER_ETYPE_3D_EFFECT1:
		if( e->w == NULL ) {
			e->w = (GtkWidget **)g_malloc(10*sizeof(GtkWidget *));
		}
		if( e->w == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		if( e->adj == NULL ) {
			e->adj=(GtkAdjustment **)g_malloc(8*sizeof(GtkAdjustment *));
		}
		if( e->adj==NULL ) {
			printf(nomem_msg);
			return -1;
		}
		if( e->info.data.teffect1.effect & SND_MIXER_EFF1_SW ) {
			ih_box=gtk_hbox_new(FALSE,2);
			gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,0);
			e->w[0]=gtk_toggle_button_new();
			gtk_box_pack_start(GTK_BOX(ih_box),e->w[0],FALSE,FALSE,4);
			gtk_widget_set_usize(e->w[0],10,10);
			gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(e->w[0])
										,e->e.data.teffect1.sw);
			gtk_widget_show(e->w[0]);
			gtk_signal_connect(GTK_OBJECT(e->w[0]),"toggled",
							   GTK_SIGNAL_FUNC(sw_3d_callback),(gpointer)e);
			c_l=gtk_label_new(_("Enable"));
			gtk_box_pack_start(GTK_BOX(ih_box),c_l,FALSE,FALSE,0);
			gtk_widget_show(c_l);
			gtk_widget_show(ih_box);
		} else {
			e->w[0]=NULL;
		}
		if( e->info.data.teffect1.effect & SND_MIXER_EFF1_MONO_SW ) {
			ih_box=gtk_hbox_new(FALSE,2);
			gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,0);
			e->w[1]=gtk_toggle_button_new();
			gtk_box_pack_start(GTK_BOX(ih_box),e->w[1],FALSE,FALSE,4);
			gtk_widget_set_usize(e->w[1],10,10);
			gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(e->w[1])
										,e->e.data.teffect1.mono_sw);
			gtk_widget_show(e->w[1]);
			gtk_signal_connect(GTK_OBJECT(e->w[1]),"toggled",
							   GTK_SIGNAL_FUNC(sw_3d_callback),(gpointer)e);
			c_l=gtk_label_new(_("MONO"));
			gtk_box_pack_start(GTK_BOX(ih_box),c_l,FALSE,FALSE,0);
			gtk_widget_show(c_l);
			gtk_widget_show(ih_box);
		} else {
			e->w[1]=NULL;
		}
		MIX_3D_VOL(0,wide,min_wide,max_wide,SND_MIXER_EFF1_WIDE);
		MIX_3D_VOL(1,volume,min_volume,max_volume,SND_MIXER_EFF1_VOLUME);
		MIX_3D_VOL(2,center,min_center,max_center,SND_MIXER_EFF1_CENTER);
		MIX_3D_VOL(3,space,min_space,max_space,SND_MIXER_EFF1_SPACE);
		MIX_3D_VOL(4,depth,min_depth,max_depth,SND_MIXER_EFF1_DEPTH);
		MIX_3D_VOL(5,delay,min_delay,max_delay,SND_MIXER_EFF1_DELAY);
		MIX_3D_VOL(6,feedback,min_feedback,max_feedback,SND_MIXER_EFF1_FEEDBACK);
		MIX_3D_VOL(7,depth_rear,min_depth_rear,max_depth_rear,SND_MIXER_EFF1_DEPTH_REAR);
		break;
	case SND_MIXER_ETYPE_TONE_CONTROL1:
		if( e->w == NULL ) {
			e->w = (GtkWidget **)g_malloc(3*sizeof(GtkWidget *));
		}
		if( e->w == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		if( e->adj == NULL ) {
			e->adj=(GtkAdjustment **)g_malloc(2*sizeof(GtkAdjustment *));
		}
		if( e->adj==NULL ) {
			printf(nomem_msg);
			return -1;
		}
		e->e.data.tc1.tc=e->info.data.tc1.tc;
		snd_mixer_element_read(cards[e->card].mixer[e->mdev].handle,&e->e);
		if( e->info.data.tc1.tc &
			(SND_MIXER_TC1_BASS | SND_MIXER_TC1_TREBLE ) ) {
			gtk_box_pack_start(iv_box,ih_box,TRUE,TRUE,0);
			MIX_TONE_VOL(0,bass,min_bass,max_bass,SND_MIXER_TC1_BASS);
			MIX_TONE_VOL(1,treble,min_treble,max_treble,SND_MIXER_TC1_TREBLE);
		}
		if( e->info.data.tc1.tc & SND_MIXER_TC1_SW ) {
			if( e->info.data.tc1.tc &
				(SND_MIXER_TC1_BASS | SND_MIXER_TC1_TREBLE ) )
				ih_box=gtk_hbox_new(FALSE,2);
			gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,0);
			e->w[0]=gtk_toggle_button_new();
			gtk_box_pack_start(GTK_BOX(ih_box),e->w[0],FALSE,FALSE,4);
			gtk_widget_set_usize(e->w[0],10,10);
			gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(e->w[0])
										,e->e.data.tc1.sw);
			gtk_widget_show(e->w[0]);
			gtk_signal_connect(GTK_OBJECT(e->w[0]),"toggled",
							   GTK_SIGNAL_FUNC(sw_tone_callback),(gpointer)e);
			c_l=gtk_label_new(_("Enable"));
			gtk_box_pack_start(GTK_BOX(ih_box),c_l,FALSE,FALSE,0);
			gtk_widget_show(c_l);
			gtk_widget_show(ih_box);
		} else {
			e->w[0]=NULL;
		}
		break;
	case SND_MIXER_ETYPE_PAN_CONTROL1:
		if( e->w == NULL ) {
			e->w = (GtkWidget **)g_malloc(e->e.data.pc1.pan*4*
										  sizeof(GtkWidget *));
		}
		if( e->w == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		if( e->adj == NULL ) {
			e->adj=(GtkAdjustment **)g_malloc(e->e.data.pc1.pan*
											  sizeof(GtkAdjustment *));
		}
		if( e->adj==NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		for( i=0 ; i<e->e.data.pc1.pan ; i++ ) {
			j=i*4;
			e->adj[i]=(GtkAdjustment *)gtk_adjustment_new(
								(gfloat)e->e.data.pc1.ppan[i],
								(gfloat)e->info.data.pc1.prange[i].min-0.5,
								(gfloat)e->info.data.pc1.prange[i].max+1.5,
								1.0,4.0,1.0);
			gtk_signal_connect(GTK_OBJECT(e->adj[i]),"value_changed",
						   GTK_SIGNAL_FUNC(pc1_callback),(gpointer)e);
			e->w[j]=gtk_hscale_new(GTK_ADJUSTMENT(e->adj[i]));
			gtk_scale_set_draw_value(GTK_SCALE(e->w[j]),FALSE);
			gtk_box_pack_start(iv_box,e->w[j],FALSE,FALSE,4);
			gtk_widget_show(e->w[j]);

			gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,4);
			j++;
			e->w[j]=gtk_button_new_with_label(pc1_ptc1[e->info.data.pc1.prange[i].pan_type]);
			gtk_signal_connect(GTK_OBJECT(e->w[j]),"clicked",
						   GTK_SIGNAL_FUNC(pc1_ss_callback),(gpointer)e);
			gtk_box_pack_start(GTK_BOX(ih_box),e->w[j],FALSE,FALSE,2);
			gtk_widget_show(e->w[j++]);
			e->w[j]=gtk_button_new_with_label("C");
			gtk_signal_connect(GTK_OBJECT(e->w[j]),"clicked",
							   GTK_SIGNAL_FUNC(pc1_ss_callback),(gpointer)e);
			gtk_box_pack_start(GTK_BOX(ih_box),e->w[j],FALSE,FALSE,2);
			gtk_widget_show(e->w[j++]);
			e->w[j]=gtk_button_new_with_label(pc1_ptc2[e->info.data.pc1.prange[i].pan_type]);
			gtk_signal_connect(GTK_OBJECT(e->w[j]),"clicked",
							   GTK_SIGNAL_FUNC(pc1_ss_callback),(gpointer)e);
			gtk_box_pack_start(GTK_BOX(ih_box),e->w[j],FALSE,FALSE,2);
			gtk_widget_show(e->w[j]);
		}
		break;
	}
	gtk_widget_show(ih_box);
	return 0;
}

static gint mk_sw(s_sw_t *sw,GtkBox *iv_box) {
	GtkWidget *ih_box;

	ih_box=gtk_hbox_new(TRUE,0);
	switch( sw->sw.type ) {
	case SND_SW_TYPE_BOOLEAN:
		gtk_box_pack_start(iv_box,ih_box,FALSE,FALSE,4);
		if( sw->w == NULL )
			sw->w = (GtkWidget **)g_malloc(sizeof(GtkWidget *));
		if( sw->w == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		sw->w[0]=gtk_toggle_button_new();
		gtk_box_pack_start(GTK_BOX(ih_box),sw->w[0],FALSE,FALSE,0);
		gtk_widget_set_usize(sw->w[0],20,20);
		gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(sw->w[0]),
									sw->sw.value.enable);
		gtk_signal_connect(GTK_OBJECT(sw->w[0]),"toggled",
						   GTK_SIGNAL_FUNC(csw_bool_callback),
						   (gpointer)sw);
		gtk_widget_show(sw->w[0]);
		break;
	}
	gtk_widget_show(ih_box);
	return 0;
}

static void cv_name(unsigned char *name,int w) {
	GtkWidget *lb;
	GtkRequisition rq;
	int i,l,max_l,x;
	unsigned char *s,*s_st,*s_max,buf[128];

	for( i=0 ; name[i] !=' ' ; i++ );
	name[i]='\n';
	lb=gtk_label_new(name);
	gtk_widget_show(lb);
	gtk_widget_size_request(lb,&rq);
	gtk_widget_destroy(lb);
	while( w < rq.width) {
		s=name;
		s_max=s;
		max_l=0;
		while( *s ) {
			l=0;
			s_st=s;
			while( *s >= ' ' ) {
				l++;
				s++;
			}
			if( l>max_l ) {
				max_l=l;
				s_max=s_st;
			}
			if( *s ) s++;
		}
		x = (rq.width - w)/(rq.width/max_l)+2;
		memcpy(buf,s_max,max_l);
		buf[max_l]=0;
		s=s_max+max_l-x;
		strcpy(buf,s);
		if( *(s-1) !=' ' && *s !=' ' ) *(s++)='-';
		*(s++)='\n';
		if( *buf == ' ' ) strcpy(s,buf+1); else strcpy(s,buf);
		lb=gtk_label_new(name);
		gtk_widget_show(lb);
		gtk_widget_size_request(lb,&rq);
		gtk_widget_destroy(lb);
	}
}

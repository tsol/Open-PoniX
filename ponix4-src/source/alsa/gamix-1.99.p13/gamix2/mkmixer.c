
#include "gamix.h"
#include <math.h>

static int card_no;
gint wsize_min=60;

static void close_callback(GtkWidget *w,s_card_t *cards ) {
	s_obj_t *obj;
	for( obj=cards->objs ; obj != NULL ; obj=obj->next ) obj->enabled=FALSE;
	snd_ctl_close(cards->handle);
	cards->handle=NULL;
}

#define CHAIN_CB GTK_SIGNAL_FUNC(chain_callback)
static void chain_callback(GtkToggleButton *b,s_obj_t *obj) {
	obj->chain=b->active;
}

#define BOOLEAN_CB GTK_SIGNAL_FUNC(boolean_callback)
static void boolean_callback(GtkToggleButton *b,s_cb_t *cb) {
	int i=cb->i,j,err;
	s_element_t *e=cb->e,*ee;
	s_ctl_t *ctl=e->ctl;
	struct s_ep *ep;
	snd_ctl_elem_value_t *val=ctl->ctl;
	snd_ctl_elem_info_t *info=ctl->info;
	int count=snd_ctl_elem_info_get_count(info);

	if( snd_ctl_elem_value_get_boolean(val,i) == b->active ) return;
	if( *e->chain && count > 1 ) {
		for( j=0 ; j<count ; j++ ) {
			snd_ctl_elem_value_set_boolean(val,j,b->active);
			if( j != i )
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(e->w[j]),
											 b->active);
		}
	} else
		snd_ctl_elem_value_set_boolean(val,i,b->active);
	for( ep=ctl->el ; ep != NULL ; ep=ep->next ) {
		if( ep->e != e ) {
			ee=ep->e;
			if( *e->chain ) {
				for( j=0 ; j<count ; j++ ) {
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ee->w[j]),
												 b->active);
				}
			} else {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ee->w[i]),
											 b->active);
			}
		}
	}
	err=snd_ctl_elem_write(cards[e->card].handle,val);
}

#define INTEGER_V_CB GTK_SIGNAL_FUNC(integer_v_callback)
static void integer_v_callback(GtkAdjustment *adj,s_cb_t *cb) {
	s_element_t *e=cb->e,*ee;
	int i=cb->i,j,value=-(int)adj->value,err,atype;
	s_ctl_t *ctl=e->ctl;
	struct s_ep *ep;
	snd_ctl_elem_value_t *val=ctl->ctl;
	snd_ctl_elem_info_t *info=ctl->info;
	int count=snd_ctl_elem_info_get_count(info);

	if( snd_ctl_elem_value_get_integer(val,i) == value ) return;
	if( count > 1 && *e->chain ) {
		for( j=0 ; j<count ; j++ ) {
			snd_ctl_elem_value_set_integer(val,j,value);
			if( j != i ) {
				e->adj[j]->value=adj->value;
				gtk_signal_emit_by_name(GTK_OBJECT(e->adj[j]),"value_changed");
			}
		}
	} else
		snd_ctl_elem_value_set_integer(val,i,value);
	//printf("V %d,'%s',%d %d:%d[%ld-%ld]\n",ctl->ctl.id.numid,ctl->ctl.id.name,ctl->ctl.id.index,i,value,ctl->info.value.integer.min,ctl->info.value.integer.max);
	atype=e->atype&1;
	for( ep=ctl->el ; ep != NULL ; ep=ep->next ) {
		if( ep->e != e ) {
			if( ep->e->atype == 3 ) {
#if 0
				char txt[10];
				sprintf(txt,"%d",value);
				gtk_entry_set_text(GTK_ENTRY(ep->e->w[i]),txt);
#endif
			} else {
				gfloat v=(atype==(ep->e->atype&1))?adj->value:-adj->value;
				ee=ep->e;
				if( *e->chain ) {
					for( j=0 ; j<count ; j++ ) {
						ee->adj[j]->value=v;
						gtk_signal_emit_by_name(GTK_OBJECT(ee->adj[j]),
												"value_changed");
					}
				} else {
					ee->adj[i]->value=v;
					gtk_signal_emit_by_name(GTK_OBJECT(ee->adj[i]),
											"value_changed");
				}
			}
		}
	}
	err=snd_ctl_elem_write(cards[e->card].handle,val);
}

#define INTEGER_H_CB GTK_SIGNAL_FUNC(integer_h_callback)
static void integer_h_callback(GtkAdjustment *adj,s_cb_t *cb) {
	s_element_t *e=cb->e,*ee;
	int i=cb->i,j,value=(int)adj->value,err,atype;
	s_ctl_t *ctl=e->ctl;
	struct s_ep *ep;
	snd_ctl_elem_value_t *val=ctl->ctl;
	snd_ctl_elem_info_t *info=ctl->info;
	int count=snd_ctl_elem_info_get_count(info);

	if( snd_ctl_elem_value_get_integer(val,i) == value ) return;
	if( count > 1 && *e->chain ) {
		for( j=0 ; j<count ; j++ ) {
			snd_ctl_elem_value_set_integer(val,j,value);
			if( j != i ) {
				e->adj[j]->value=adj->value;
				gtk_signal_emit_by_name(GTK_OBJECT(e->adj[j]),"value_changed");
			}
		}
	} else
		snd_ctl_elem_value_set_integer(val,i,value);
	//printf("H %d,'%s',%d %d:%d[%d-%d]\n",ctl->ctl.id.numid,ctl->ctl.id.name,ctl->ctl.id.index,i,value,ctl->info.value.integer.min,ctl->info.value.integer.max);
	atype=e->atype&1;
	for( ep=ctl->el ; ep != NULL ; ep=ep->next ) {
		if( ep->e != e ) {
			if( ep->e->atype == 3 ) {
#if 0
				char txt[10];
				sprintf(txt,"%d",value);
				gtk_entry_set_text(GTK_ENTRY(ep->e->w[i]),txt);
#endif
			} else {
				gfloat v=(atype==(ep->e->atype&1))?adj->value:-adj->value;
				ee=ep->e;
				if( *e->chain ) {
					for( j=0 ; j<count ; j++ ) {
						ee->adj[j]->value=v;
						gtk_signal_emit_by_name(GTK_OBJECT(ee->adj[j]),
												"value_changed");
					}
				} else {
					ee->adj[i]->value=v;
					gtk_signal_emit_by_name(GTK_OBJECT(ee->adj[i]),
											"value_changed");
				}
			}
		}
	}
	err=snd_ctl_elem_write(cards[e->card].handle,val);
}

static void pan_cb(s_element_t *e,s_ctl_t *ctl,int v) {
	if( snd_ctl_elem_value_get_integer(ctl->ctl,0) == v ) return;
	e->adj[0]->value=(double)v;
	gtk_signal_emit_by_name(GTK_OBJECT(e->adj[0]),"value_changed");
}
#define PAN_L_CB GTK_SIGNAL_FUNC(pan_l_callback)
static void pan_l_callback(GtkButton *b,s_cb_t *cb) {
	s_element_t *e=cb->e;
	s_ctl_t *ctl=e->ctl;
	pan_cb(e,ctl,snd_ctl_elem_info_get_min(ctl->info));
}
#define PAN_C_CB GTK_SIGNAL_FUNC(pan_c_callback)
static void pan_c_callback(GtkButton *b,s_cb_t *cb) {
	s_element_t *e=cb->e;
	s_ctl_t *ctl=e->ctl;
	pan_cb(e,ctl,(snd_ctl_elem_info_get_max(ctl->info) +
				  snd_ctl_elem_info_get_min(ctl->info))/2);
}
#define PAN_R_CB GTK_SIGNAL_FUNC(pan_r_callback)
static void pan_r_callback(GtkButton *b,s_cb_t *cb) {
	s_element_t *e=cb->e;
	s_ctl_t *ctl=e->ctl;
	pan_cb(e,ctl,snd_ctl_elem_info_get_max(ctl->info));
}

#define ENUMERATED_CB GTK_SIGNAL_FUNC(enumerated_callback)
static void enumerated_callback(GtkItem *item,s_cb_t *cb) {
	int i=cb->i,j,n,err;
	s_element_t *e=cb->e,*ee;
	s_ctl_t *ctl=e->ctl;
	struct s_ep *ep;
	snd_ctl_elem_value_t *val=ctl->ctl;
	snd_ctl_elem_info_t *info=ctl->info;
	int count=snd_ctl_elem_info_get_count(info);

	n=(int)gtk_object_get_data(GTK_OBJECT(item),"no");
	if( snd_ctl_elem_value_get_enumerated(val,i) == n ) return;
	if( *e->chain ) {
		for( j=0 ; j<count ; j++ ) {
			snd_ctl_elem_value_set_enumerated(val,j,n);
			if( j != i )
				gtk_option_menu_set_history(GTK_OPTION_MENU(e->w[j]),n);
		}
	} else
		snd_ctl_elem_value_set_enumerated(val,i,n);

	for( ep=ctl->el ; ep != NULL ; ep=ep->next ) {
		if( ep->e != e ) {
			ee=ep->e;
			if( *e->chain ) {
				for( j=0 ; j<count ; j++ ) {
					gtk_option_menu_set_history(GTK_OPTION_MENU(ee->w[j]),n);
				}
			} else {
				gtk_option_menu_set_history(GTK_OPTION_MENU(ee->w[i]),n);
			}
		}
	}
	err=snd_ctl_elem_write(cards[e->card].handle,val);
}

static void cv_name(unsigned char *name,int w) {
	GtkWidget *lb;
	GtkRequisition rq;
	int i,l,max_l,x,sp;
	unsigned char *s,*s_st,*s_max,buf[128];

	if( conf.g_l == 0 ) return;
	sp=0;
	for( i=0 ; name[i] ; i++ ) if( name[i]==' ' ) sp++;

	s=name;
	while( sp ) {
		while( *s != ' ' ) s++;
		*s='\n';
		lb=gtk_label_new(name);
		gtk_widget_show(lb);
		gtk_widget_size_request(lb,&rq);
		gtk_widget_destroy(lb);
		if( w >= rq.width ) return;
		sp--;
	}
	if( conf.g_l == 1 ) return;
#ifdef DISP_DEBUG
	printf("w %d rq.width %d\n",w,rq.width);
#endif
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
		while( *s ) {
			if( *s == '\n' ) {
				*s=' ';
				break;
			}
			s++;
		}
		lb=gtk_label_new(name);
		gtk_widget_show(lb);
		gtk_widget_size_request(lb,&rq);
		gtk_widget_destroy(lb);
	}
}

enum {
	VSCALE,
	HSCALE
};

int is_access(snd_ctl_elem_info_t *info) {
	if( !snd_ctl_elem_info_is_writable(info) ) return FALSE;
	if( snd_ctl_elem_info_is_inactive(info) ) return FALSE;
	return TRUE;
}

static void disp_scale(GtkBox *hbox,s_element_t *e,int i,int smode) {
	GtkAdjustment **adj=e->adj;
	GtkWidget **w=e->w;
	s_ctl_t *ctl=e->ctl;
	snd_ctl_elem_value_t *val=ctl->ctl;
	snd_ctl_elem_info_t *info=ctl->info;

	e->gp[i].i=i;
	e->gp[i].e=e;
	switch(smode) {
	case HSCALE:
		adj[i]=(GtkAdjustment *)gtk_adjustment_new(
							(gfloat)snd_ctl_elem_value_get_integer(val,i),
							(gfloat)snd_ctl_elem_info_get_min(info)-0.5,
							(gfloat)snd_ctl_elem_info_get_max(info)+1.0,
							(gfloat)(snd_ctl_elem_info_get_step(info)+1),
							(gfloat)(snd_ctl_elem_info_get_step(info)+1)*4,
							(gfloat)snd_ctl_elem_info_get_step(info)+1);
		gtk_signal_connect(GTK_OBJECT(adj[i]),
						   "value_changed",INTEGER_H_CB,
						   (gpointer)(e->gp+i));
		w[i]=gtk_hscale_new(adj[i]);
		break;
	case VSCALE:
		adj[i]=(GtkAdjustment *)gtk_adjustment_new(
							-(gfloat)snd_ctl_elem_value_get_integer(val,i),
							-(gfloat)snd_ctl_elem_info_get_max(info)-0.5,
							-(gfloat)snd_ctl_elem_info_get_min(info)+0.5,
							(gfloat)(snd_ctl_elem_info_get_step(info)+1),
							(gfloat)(snd_ctl_elem_info_get_step(info)+1)*4,
							(gfloat)snd_ctl_elem_info_get_step(info)+1);
		gtk_signal_connect(GTK_OBJECT(adj[i]),
						   "value_changed",INTEGER_V_CB,
						   (gpointer)(e->gp+i));
		w[i]=gtk_vscale_new(adj[i]);
		break;
	}
	gtk_scale_set_draw_value(GTK_SCALE(w[i]),FALSE);
	gtk_box_pack_start(hbox,w[i],FALSE,FALSE,4);
	gtk_widget_show(w[i]);
	if( !is_access(info) ) {
		gtk_widget_set_sensitive(w[i],FALSE);
	}
}

#define chk_e_w_gp(n) \
		if( e->w == NULL ) { \
			e->w=(GtkWidget **)malloc((count+(n))* \
									  sizeof(GtkWidget *)); \
		} \
		if( e->w == NULL ) { \
			pr_err(nomem_msg); \
			return ; \
		} \
		if( e->gp == NULL ) { \
			e->gp=(s_cb_t *)malloc(count*sizeof(s_cb_t)); \
		} \
		if( e->gp == NULL ) { \
			pr_err(nomem_msg); \
			return ; \
		} \
		if( count > 1 ) *e->chain=TRUE; 

static void disp_elem(s_element_t *e,GtkBox *vbox) {
	int i,j;
	GtkWidget *hbox,*box;
	GtkWidget *menu,*item;
	GtkWidget **w;
	GtkAdjustment **adj;
	s_ctl_t *ctl=e->ctl;
	gint b_fill=FALSE,b_exp=FALSE,b_pad=0;
	GtkTooltips *tp;
	snd_ctl_elem_value_t *val=ctl->ctl;
	snd_ctl_elem_info_t *info=ctl->info;
	int count=snd_ctl_elem_info_get_count(info);

	e->card=card_no;
	hbox=gtk_hbox_new(TRUE,0);
	switch( snd_ctl_elem_info_get_type(info) ) {
	case SND_CTL_ELEM_TYPE_BOOLEAN:
		chk_e_w_gp(0);
		w=e->w;
		for( i=0 ; i<count ; i++ ) {
			w[i]=gtk_toggle_button_new();
			gtk_box_pack_start(GTK_BOX(hbox),w[i],FALSE,FALSE,0);
			gtk_widget_set_usize(w[i],10,10);
			gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(w[i]),
							snd_ctl_elem_value_get_boolean(val,i)?TRUE:FALSE);
			e->gp[i].i=i;
			e->gp[i].e=e;
			gtk_signal_connect(GTK_OBJECT(w[i]),"toggled",BOOLEAN_CB,
							   (gpointer)(e->gp+i));
#if 1
			tp=gtk_tooltips_new();
			gtk_tooltips_set_tip(tp,w[i],snd_ctl_elem_id_get_name(ctl->id),NULL);
#endif
			gtk_widget_show(w[i]);
			if( !is_access(info) ) {
				gtk_widget_set_sensitive(w[i],FALSE);
			}
			b_pad=2;
		}
		break;
	case SND_CTL_ELEM_TYPE_INTEGER:
		chk_e_w_gp((e->atype==2)?3:0);
		if( e->adj == NULL ) {
			e->adj=(GtkAdjustment **)malloc(count*sizeof(GtkAdjustment *));
		}
		if( e->adj == NULL ) {
			pr_err(nomem_msg);
			return ;
		}
		w=e->w;
		adj=e->adj;
		switch( e->atype ) {
		case 0:
			box=gtk_vbox_new(FALSE,0);
			for( i=0 ; i<count ; i++ ) {
				disp_scale(GTK_BOX(box),e,i,HSCALE);
			}
			gtk_widget_show(box);
			gtk_box_pack_start(GTK_BOX(hbox),box,FALSE,FALSE,0);
			break;
		case 1:
			for( i=0 ; i<count ; i++ ) {
				disp_scale(GTK_BOX(hbox),e,i,VSCALE);
			}
			b_exp=TRUE;b_fill=TRUE;
			break;
		case 2:
			box=gtk_vbox_new(FALSE,0);
			for( i=0 ; i<count ; i++ ) {
				disp_scale(GTK_BOX(box),e,i,HSCALE);
			}
			{
				GtkWidget *xbox=gtk_hbox_new(TRUE,0);
				i=count;
				
				w[i]=gtk_button_new_with_label(_("L"));
				gtk_signal_connect(GTK_OBJECT(w[i]),"clicked",PAN_L_CB,
								   (gpointer)e->gp);
				gtk_widget_show(w[i]);
				gtk_box_pack_start(GTK_BOX(xbox),w[i],FALSE,FALSE,0);
				if( !is_access(info) ) {
					gtk_widget_set_sensitive(w[i],FALSE);
				}
				i++;
				w[i]=gtk_button_new_with_label(_("C"));
				gtk_signal_connect(GTK_OBJECT(w[i]),"clicked",PAN_C_CB,
								   (gpointer)e->gp);
				gtk_widget_show(w[i]);
				gtk_box_pack_start(GTK_BOX(xbox),w[i],FALSE,FALSE,0);
				if( !is_access(info) ) {
					gtk_widget_set_sensitive(w[i],FALSE);
				}
				i++;

				w[i]=gtk_button_new_with_label(_("R"));
				gtk_signal_connect(GTK_OBJECT(w[i]),"clicked",PAN_R_CB,
								   (gpointer)e->gp);
				gtk_widget_show(w[i]);
				gtk_box_pack_start(GTK_BOX(xbox),w[i],FALSE,FALSE,0);
				if( !is_access(info) ) {
					gtk_widget_set_sensitive(w[i],FALSE);
				}
				i++;

				gtk_widget_show(xbox);
				gtk_box_pack_start(GTK_BOX(box),xbox,FALSE,FALSE,0);
			}
			gtk_widget_show(box);
			gtk_box_pack_start(GTK_BOX(hbox),box,FALSE,FALSE,0);
			break;
		case 3:
			{
				long max=snd_ctl_elem_info_get_max(info);
				gchar st[10];
				GtkRequisition rq;
				j=1;i=10;
				while( max>i ) {
					j++;
					i*=10;
				}
				for( i=0 ; i<count ; i++ ) {
					e->gp[i].i=i;
					e->gp[i].e=e;
					w[i]=gtk_entry_new_with_max_length(j);
					gtk_entry_set_editable(GTK_ENTRY(w[i]),FALSE);
					sprintf(st,"%ld",snd_ctl_elem_value_get_integer(val,i));
					gtk_entry_set_text(GTK_ENTRY(w[i]),st);
					gtk_widget_show(w[i]);
					gtk_widget_size_request(w[i],&rq);
					gtk_widget_set_usize(w[i],scfont_w*j+4,rq.height);
					gtk_box_pack_start(GTK_BOX(hbox),w[i],FALSE,FALSE,0);
				}
				b_exp=FALSE;b_fill=FALSE;
			}
			break;
		}
		break;
	case SND_CTL_ELEM_TYPE_ENUMERATED:
		chk_e_w_gp(0);
		box=gtk_vbox_new(FALSE,0);
		w=e->w;
		for( i=0 ; i<count ; i++ ) {
			w[i]=gtk_option_menu_new();
			menu=gtk_menu_new();
			e->gp[i].i=i;
			e->gp[i].e=e;
			for( j=0 ; j<snd_ctl_elem_info_get_items(info) ; j++ ) {
				char nm[65];
				nm[64]=0;
				memcpy(nm,ctl->inames+j*64,64);
				item=gtk_menu_item_new_with_label(nm);
				gtk_object_set_data(GTK_OBJECT(item),"no",(gpointer)j);
				gtk_signal_connect(GTK_OBJECT(item),"activate",
								   ENUMERATED_CB,(gpointer)(e->gp+i));
				gtk_menu_append(GTK_MENU(menu),item);
				gtk_widget_show(item);
			}
			gtk_option_menu_set_menu(GTK_OPTION_MENU(w[i]),menu);
			gtk_box_pack_start(GTK_BOX(box),w[i],FALSE,FALSE,4);
			gtk_widget_show(w[i]);
			gtk_option_menu_set_history(GTK_OPTION_MENU(w[i]),
									snd_ctl_elem_value_get_enumerated(val,i));
			if( !is_access(info) ) {
				gtk_widget_set_sensitive(w[i],FALSE);
			}
		}
		gtk_widget_show(box);
		gtk_box_pack_start(GTK_BOX(hbox),box,FALSE,FALSE,0);
		break;
	default:
		break;
	}

	gtk_widget_show(hbox);
	gtk_box_pack_start(vbox,hbox,b_exp,b_fill,b_pad);
	e->box=hbox;
}
static void disp_obj(s_obj_t *obj) {
	GtkWidget *f;
	GtkWidget *box,*hbox;
	GtkWidget *tl=NULL;
	GtkWidget *l;
	gchar s[256];
	s_eos_t *es;
	int i;

	obj->w=NULL;
	obj->enabled=FALSE;
	if( !obj->enable ) return;
#ifdef DISP_DEBUG
	fprintf(stderr,"  obj %s\n",obj->name);
#endif
	f=gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(f),GTK_SHADOW_OUT);
	obj->chain_en=obj->chain=FALSE;
	es=obj->es;
	if( es->type == EO_OBJ ) {
		box=gtk_hbox_new(FALSE,2);
	} else {
		box=gtk_vbox_new(FALSE,2);
		tl=gtk_label_new(NULL);
		gtk_box_pack_start(GTK_BOX(box),tl,FALSE,FALSE,0);
	}

	for( i=0 ; i<obj->es_num ; i++ ) {
		switch(es->type) {
		case EO_OBJ:
			disp_obj(es->eo.o);
			if( es->eo.o->w ) {
				gtk_box_pack_start(GTK_BOX(box),es->eo.o->w,TRUE,TRUE,0);
			}
			break;
		case EO_ELEM:
			disp_elem(es->eo.e,GTK_BOX(box));
			break;
		case EO_LABEL:
			hbox=gtk_hbox_new(FALSE,0);
			l=gtk_label_new(es->eo.l);
			gtk_widget_show(l);
			gtk_box_pack_start(GTK_BOX(hbox),l,FALSE,FALSE,4);
			gtk_widget_show(hbox);
			gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
			break;
		}
		es++;
	}
	if( obj->es->type != EO_OBJ ) {
		GtkWidget *hbox;
		GtkWidget *l;

		hbox=gtk_hbox_new(TRUE,0);
		if( obj->chain ) {
			GtkWidget *hhbox,*b;
			obj->chain_en=TRUE;
			hhbox=gtk_hbox_new(FALSE,0);
			b=gtk_toggle_button_new();
			gtk_box_pack_start(GTK_BOX(hhbox),b,FALSE,FALSE,4);
			gtk_widget_set_usize(b,10,10);
			gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(b),obj->chain);
			gtk_widget_show(b);
			gtk_signal_connect(GTK_OBJECT(b),"toggled",CHAIN_CB,(gpointer)obj);
			l=gtk_label_new(_("Sync"));
			gtk_box_pack_start(GTK_BOX(hhbox),l,FALSE,FALSE,0);
			gtk_widget_show(l);
			gtk_widget_show(hhbox);
			gtk_box_pack_start(GTK_BOX(hbox),hhbox,FALSE,FALSE,0);
		} else {
			l=gtk_label_new(" ");
			gtk_widget_show(l);
			gtk_box_pack_start(GTK_BOX(hbox),l,FALSE,FALSE,0);
		}
		gtk_widget_show(hbox);
		gtk_box_pack_end(GTK_BOX(box),hbox,FALSE,FALSE,0);
	}
	gtk_widget_show(box);
	if( obj->name[0] ) {
		GtkRequisition rq1,rq2;
		GtkWidget *l;
		strcpy(s,obj->name);

		gtk_widget_size_request(box,&rq2);
		l=gtk_label_new(s);
		gtk_widget_show(l);
		if( rq2.width < wsize_min ) rq2.width = wsize_min;
		gtk_widget_size_request(l,&rq1);
		gtk_widget_destroy(l);
		if( rq1.width > rq2.width ) {
			cv_name(s,rq2.width);
			for( i=0; s[i]!='\n' && s[i] ; i++);
			if( s[i] ) {
				gtk_label_set(GTK_LABEL(tl),s+i+1);
				gtk_widget_show(tl);
				s[i]=0;
			}
		}
		gtk_frame_set_label(GTK_FRAME(f),s);
	}
	gtk_container_add(GTK_CONTAINER(f),box);
	gtk_widget_show(f);
	obj->w=f;
	obj->enabled=TRUE;
	pr_mtime();
}

GtkWidget *make_mixer( gint c_n ) {
	int i,err;
	GtkWidget *mv_box,*m_name;
	GtkWidget *s_win;
	GtkWidget *mh_box;
	char gname[128];
	s_obj_t *obj;
	s_card_t *sc=cards+c_n;


	pr_mtime();
	card_no=c_n;
	if( cards[c_n].handle ) {
		snd_ctl_close(sc->handle);
	}
	sprintf(gname,"hw:%d",c_n);
#ifdef SNDCTLOPEN2
	if( (err=snd_ctl_open(&sc->handle,gname)) < 0 )
#else
	if( (err=snd_ctl_open(&sc->handle,gname,0)) < 0 )
#endif
	{
		return NULL;
	}
	if( (err=snd_ctl_poll_descriptors(sc->handle,&sc->pfd,1))< 0 ) {
		snd_ctl_close(sc->handle);
		sc->handle=NULL;
		return NULL;
	}
	//printf("desc %d\n",sc->pfd.fd);
	snd_ctl_subscribe_events(sc->handle,1);

	mv_box=gtk_vbox_new(FALSE,0);
	gtk_widget_show(mv_box);

	sprintf(gname,"%s",snd_ctl_card_info_get_longname(sc->info));
	for(i=0; gname[i]!=0 ; i++ ) {
		if( strncmp(gname+i,"at",2)==0 ) {
			gname[i]=0;
			gname[i+1]=0;
		}
	}
	sprintf(gname,"%s: %s",gname,snd_ctl_card_info_get_mixername(sc->info));
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
	} else {
		gtk_box_pack_start(GTK_BOX(mv_box),mh_box,TRUE,TRUE,4);
	}
	gtk_widget_show(mh_box);

	for( obj=sc->objs ; obj != NULL ; obj=obj->next ) {
		disp_obj(obj);
		if( obj->w ) {
			gtk_box_pack_start(GTK_BOX(mh_box),obj->w,sc->p_e,sc->p_f,2);
		}
		pr_mtime();
	}
	gtk_signal_connect(GTK_OBJECT(mv_box),"destroy",
					   GTK_SIGNAL_FUNC(close_callback),(gpointer)sc);
	sc->enabled=TRUE;
	pr_mtime();
	return mv_box;
}


#include "gamix.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

s_conf conf;

static GtkWidget *c_win;

typedef struct c_cobj {
	gboolean en;
	GtkCTreeNode *node;
	GtkCTreeNode *pre;
	int num_next;
	struct c_cobj **nexts;
} c_cobj_t;

typedef struct {
	gboolean m_en;
	c_cobj_t *cobjs;
	GSList *gp;
	gboolean p_e;
	gboolean p_f;
	GtkCTree *ct;
	int *ord_l;
	int o_nums;
} c_card_t;

static c_card_t *ccard;
static gboolean scrolled;
static gboolean ok_pushed;
static gboolean Esaved;
static gboolean Tosave;
static gboolean sv_wsized;
static gboolean Idyn;
static gint g_l;
static GSList *G_l;

typedef struct {
	int numid;
	char name[44];
	int index;
	char *opt;
} c_relem_t;
typedef struct c_robj {
	char name[44];
	//int index;
	gboolean en;
	struct c_reos *es;
} c_robj_t;

typedef struct c_reos {
	int type;
	union {
		c_robj_t *o;
		c_relem_t *e;
		char *l;
	} eo;
	struct c_reos *next;
} c_reos_t;
static s_card_t *c_card=NULL;
static int c_card_no=0;
#ifdef DISP_DEBUG
static int c_idlv=0;
#endif

GtkWidget *gtk_vlabel(const gchar *str) {
#if GTK_CHECK_VERSION(2,0,0)
	PangoLayout *layout;	
#else
	int lb,rb,as,ds;
#endif
	GtkStyle *st=gtk_widget_get_style(window);
	GtkWidget *pm;
	GdkVisual *vs=gtk_widget_get_default_visual();
	GdkPixmap *p;
	GdkImage *i1,*i2;
	GdkGC *gc,*gc1=gdk_gc_new(window->window);
	int x,y,w,h;
	unsigned char *d1,*d2;

#if GTK_CHECK_VERSION(2,0,0)
	layout=gtk_widget_create_pango_layout(window,lconv(str));
	pango_layout_get_pixel_size(layout,&w,&h);
#ifdef DISP_DEBUG
	printf("l '%s' %p size %d %d\n",str,layout,w,h);
#endif
	p=gdk_pixmap_new(window->window,w,h,-1);
#else
	gdk_string_extents(st->font,str,&lb,&rb,&w,&as,&ds);
	h=as+ds;
	p=gdk_pixmap_new(window->window,w,h,-1);
#endif
	gdk_gc_set_background(gc1,&st->bg[0]);
	if( st->bg_gc[0] )
		gc=st->bg_gc[0];
	else {
#if GTK_CHECK_VERSION(2,0,0)
#else
		gdk_gc_set_font(gc1,st->font);
#endif
		gdk_gc_set_foreground(gc1,&st->bg[0]);
		gc=gc1;
	}
	gdk_draw_rectangle(p,gc,TRUE,0,0,w,h);
	if( st->fg_gc[0] )
		gc=st->fg_gc[0];
	else {
		gdk_gc_set_foreground(gc1,&st->fg[0]);
		//gdk_gc_set_foreground(gc1,&st->white);
		gc=gc1;
	}
#if GTK_CHECK_VERSION(2,0,0)
	gdk_draw_layout(p,gc,0,0,layout);
#else
	gdk_draw_string(p,st->font,gc,0,as,str);
#endif
	i1=gdk_image_get(p,0,0,w,h);
	i2=gdk_image_new(i1->type,vs,h,w);
#if 0
#ifdef DISP_DEBUG
	printf("bpp %d %d bpl %d %d\n",i1->bpp,i2->bpp,i1->bpl,i2->bpl);
#endif
#endif
	d1=i1->mem;
	d2=i2->mem;
	d2+=i2->bpl*(w-1);
	switch( i1->bpp ) {
	case 1:
	case 8:
		{
			unsigned char *d1_2,*d2_2;
			int w1=i1->bpl,w2=i2->bpl;
			for( y=0 ; y<h ; y++ ) {
				d2_2=d2++;
				d1_2=d1;
				for( x=0 ; x<w ; x++ ) {
					*d2_2=*d1_2++;
					d2_2-=w2;
				}
				d1+=w1;
			}
		}
		break;
	case 2:
	case 16:
	case 15:
		{
			unsigned short *d1_1,*d1_2,*d2_1,*d2_2;
			int w1=i1->bpl/2,w2=i2->bpl/2;
			d1_1=(unsigned short *)d1;
			d2_1=(unsigned short *)d2;
			for( y=0 ; y<h ; y++ ) {
				d1_2=d1_1;
				d2_2=d2_1++;
				for( x=0 ; x<w ; x++ ) {
					*d2_2=*d1_2++;
					d2_2-=w2;
				}
				d1_1+=w1;
			}
		}
		break;
	case 3:
	case 24:
		break;
	case 4:
	case 32:
		{
			unsigned long *d1_1,*d1_2,*d2_1,*d2_2;
			int w1=i1->bpl/4,w2=i2->bpl/4;
			d1_1=(unsigned long *)d1;
			d2_1=(unsigned long *)d2;
			for( y=0 ; y<h ; y++ ) {
				d1_2=d1_1;
				d2_2=d2_1++;
				for( x=0 ; x<w ; x++ ) {
					*d2_2=*d1_2++;
					d2_2-=w2;
				}
				d1_1+=w1;
			}
		}
		break;
	default:
		break;
	}
#if GTK_CHECK_VERSION(2,0,0)
	g_object_unref (layout);
#endif
	gdk_pixmap_unref(p);
	p=gdk_pixmap_new(window->window,h,w,-1);
	gdk_draw_image(p,gc,i2,0,0,0,0,h,w);
	pm=gtk_pixmap_new(p,NULL);
	gdk_pixmap_unref(p);
	gdk_image_destroy(i1);
	gdk_image_destroy(i2);
	gdk_gc_destroy(gc1);
	return pm;
}

static void close_win(GtkWidget *w,gpointer data) {
	gtk_grab_remove(c_win);
	gtk_main_quit();
}
static void cancel_b(GtkWidget *w,gpointer data) {
	gtk_widget_destroy(c_win);
}
static void ok_b(GtkWidget *w,gpointer data) {
	int i,j,k;
	GSList *n;
	GtkWidget *b;
	gchar *s;
	GtkCTreeNode *node;
	Tosave=(gboolean)data;

	for( j=0 ; (n = g_slist_nth(G_l,j)) != NULL ; j++ ) {
		b=(GtkWidget *)n->data;
		if( GTK_TOGGLE_BUTTON(b)->active ) break;
	}
	g_l=2-j;

	ok_pushed=TRUE;
	for( i=0 ; i<card_last ; i++ ) {
		if( cards[i].controls ) {
			for( j=0 ; (n = g_slist_nth(ccard[i].gp,j)) != NULL ; j++ ) {
				b=(GtkWidget *)n->data;
				if( GTK_TOGGLE_BUTTON(b)->active ) break;
			}
			switch(j) {
			case 2:
				ccard[i].p_e=FALSE;
				ccard[i].p_f=FALSE;
				break;
			case 1:
				ccard[i].p_e=TRUE;
				ccard[i].p_f=FALSE;
				break;
			case 0:
				ccard[i].p_e=TRUE;
				ccard[i].p_f=TRUE;
				break;
			}
#if 0
			for( j=0 ; j<ccard[i].o_nums ; j++ ) {
				gtk_clist_get_text(ccard[i].cl,j,0,&s);
				ccard[i].ord_l[j]=atoi(s)-1;
			}
#endif
			j=0;k=0;
			while( (node=gtk_ctree_node_nth(ccard[i].ct,j) ) ) {
				gtk_ctree_node_get_text(ccard[i].ct,node,1,&s);
				if( *s != ' ' ) {
					ccard[i].ord_l[k++]=atoi(s)-1;
					gtk_ctree_node_get_text(ccard[i].ct,node,2,&s);
					//printf("%d:%d %s\n",j,ccard[i].ord_l[k-1],s);
				}
				j++;
			}
		}
	}
			
	gtk_widget_destroy(c_win);
}
static void tb_callback(GtkToggleButton *b,gint *c) {
	*c=b->active;
}
#define SEL_CB GTK_SIGNAL_FUNC(sel_callback)
static void sel_callback(GtkWidget *w,GtkCTreeNode *node,int col,
						 GtkCTree *ct) {
	c_cobj_t *cobj=(c_cobj_t *)gtk_ctree_node_get_row_data(ct,node);

	cobj->en=TRUE;
	if( cobj->pre ) gtk_ctree_select(ct,cobj->pre);
}
#define UNSEL_CB GTK_SIGNAL_FUNC(usel_callback)
static void usel_callback(GtkWidget *w,GtkCTreeNode *node,int col,
						  GtkCTree *ct) {
	c_cobj_t *cobj=(c_cobj_t *)gtk_ctree_node_get_row_data(ct,node);

	cobj->en=FALSE;
	gtk_ctree_unselect_recursive(ct,node);
}
static gboolean dc_func(GtkCTree *ct,GtkCTreeNode *s_node,
						GtkCTreeNode *new_parent,GtkCTreeNode *new_sibling) {
	
	gchar *s;

	if( new_parent != NULL ) return FALSE;
	gtk_ctree_node_get_text(ct,s_node,1,&s);
	if( *s < '0' || *s > '9' ) return FALSE;
	//gtk_ctree_node_moveto(ct,new_sibling,0,0.5,0);
	printf("hoe %d\n",gtk_ctree_node_is_visible(ct,new_sibling));
	return TRUE;
}

static void comk_obj(c_cobj_t *pcobj,s_eos_t *es,int num,GtkCTree *ct) {
	int i;
	gchar *cl_data[3],gname[44];
	s_obj_t *obj;
	c_cobj_t *cobj;
	gboolean ct_lf,ct_ex;

	pcobj->num_next=num;
	pcobj->nexts=(c_cobj_t **)malloc(num*sizeof(c_cobj_t*));
	if( pcobj->nexts == NULL ) return;
	for( i=0 ; i<num ; i++ ) {
		if( es[i].type == EO_OBJ ) {
			obj=es[i].eo.o;
			if( obj->name[0] ) {
				strcpy(gname,obj->name);
			} else {
				strcpy(gname,"[NONE]");
			}
			//printf("gname %s\n",gname);
			cl_data[0]=" ";
			cl_data[1]=" ";
			cl_data[2]=gname;
			if( obj->es[0].type == EO_OBJ ) {
				ct_lf=FALSE;
				ct_ex=TRUE;
			} else {
				ct_lf=TRUE;
				ct_ex=FALSE;
			}
			cobj=(c_cobj_t *)malloc(sizeof(c_cobj_t));
			if( cobj == NULL ) {
				pr_err(nomem_msg);
				return;
			}
			memset(cobj,0,sizeof(c_cobj_t));
			cobj->node=gtk_ctree_insert_node(ct,pcobj->node,NULL,cl_data,0,
											 NULL,NULL,NULL,NULL,ct_lf,ct_ex);
			cobj->pre=pcobj->node;
			pcobj->nexts[i]=cobj;
			cobj->en=es[i].eo.o->enable;
			gtk_ctree_node_set_row_data(ct,cobj->node,(gpointer)cobj);
			if( obj->enable )
				gtk_ctree_select(ct,cobj->node);
			if( ct_ex == EO_OBJ )
				comk_obj(cobj,obj->es,obj->es_num,ct);
		}
	}
}
static void cobj_en(c_cobj_t *cobj,s_obj_t *obj,gboolean *ch) {
	int i;
	c_cobj_t *p;
	s_eos_t *es=obj->es;
	s_obj_t *o;

	for( i=0 ; i<cobj->num_next ; i++ ) {
		p=cobj->nexts[i];
		o=es->eo.o;
		if( !(*ch) )
			if( o->enable != p->en ) *ch=TRUE;
		o->enable=p->en;
		if( o->es[0].type == EO_OBJ )
			cobj_en(p,o,ch);
		es++;
	}
	free(cobj->nexts);
}

gint conf_win( void ) {
	int i,j,k,l,m,sf;
	gint changed,*o_l;
	GtkWidget *b;
	GtkWidget *vbox,*vvbox,*box,*frame,*hbox;
	//GtkWidget *clist;
	GtkWidget *nb,*n_label;
	GtkWidget *vscbar;
	GtkWidget *ctree;
	//GtkWidget *v1,*ve1;
	GtkAdjustment *adj;
	//unsigned char vs[4];
	GtkStyle *style;
	unsigned char gname[44];
	GSList *gp;
	s_obj_t *obj,*obj_b,*obj2,*obj2_b;
	gchar *cl_data[3],cl_num[6];
	GtkRequisition rq;
	gboolean ct_lf,ct_ex;
	c_card_t *c;
	c_cobj_t *cobj;

	ok_pushed=FALSE;

	c_win=gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_signal_connect(GTK_OBJECT(c_win),"destroy",GTK_SIGNAL_FUNC(close_win),
					   NULL);
	style=gtk_widget_get_style(c_win);

	vbox=gtk_vbox_new(FALSE,10);
	gtk_container_add(GTK_CONTAINER(c_win),vbox);
	
	/* options */
	nb=gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(nb),GTK_POS_TOP);
	gtk_box_pack_start(GTK_BOX(vbox),nb,FALSE,FALSE,0);
	
	/*  OPT */
	frame=gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_IN);
	gtk_container_set_border_width(GTK_CONTAINER(frame),20);
	box=gtk_vbox_new(FALSE,10);
	gtk_container_set_border_width(GTK_CONTAINER(box),10);
	gtk_container_add(GTK_CONTAINER(frame),box);

	hbox=gtk_hbox_new(FALSE,4);
	gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
	scrolled=conf.scroll;
	b=gtk_toggle_button_new();
	gtk_widget_set_usize(b,10,10);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),conf.scroll);
	gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
	gtk_signal_connect(GTK_OBJECT(b),"toggled",GTK_SIGNAL_FUNC(tb_callback),
					   (gpointer)&scrolled);
	gtk_widget_show(b);
	n_label=gtk_label_new(lconv(_("Scroll window enable")));
	gtk_box_pack_start(GTK_BOX(hbox),n_label,FALSE,FALSE,0);
	gtk_widget_show(n_label);
	gtk_widget_show(hbox);

	hbox=gtk_hbox_new(FALSE,4);
	gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
	Esaved=conf.Esave;
	b=gtk_toggle_button_new();
	gtk_widget_set_usize(b,10,10);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),conf.Esave);
	gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
	gtk_signal_connect(GTK_OBJECT(b),"toggled",GTK_SIGNAL_FUNC(tb_callback),
					   (gpointer)&Esaved);
	gtk_widget_show(b);
	n_label=gtk_label_new(lconv(_("Config save when exit")));
	gtk_box_pack_start(GTK_BOX(hbox),n_label,FALSE,FALSE,0);
	gtk_widget_show(n_label);
	gtk_widget_show(hbox);

	hbox=gtk_hbox_new(FALSE,4);
	gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
	sv_wsized=conf.sv_wsize;
	b=gtk_toggle_button_new();
	gtk_widget_set_usize(b,10,10);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),conf.sv_wsize);
	gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
	gtk_signal_connect(GTK_OBJECT(b),"toggled",GTK_SIGNAL_FUNC(tb_callback),
					   (gpointer)&sv_wsized);
	gtk_widget_show(b);
	n_label=gtk_label_new(lconv(_("Save window size")));
	gtk_box_pack_start(GTK_BOX(hbox),n_label,FALSE,FALSE,0);
	gtk_widget_show(n_label);
	gtk_widget_show(hbox);

	hbox=gtk_hbox_new(FALSE,4);
	gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
	Idyn=conf.i_dyn;
	b=gtk_toggle_button_new();
	gtk_widget_set_usize(b,10,10);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),conf.i_dyn);
	gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
	gtk_signal_connect(GTK_OBJECT(b),"toggled",GTK_SIGNAL_FUNC(tb_callback),
					   (gpointer)&Idyn);
	gtk_widget_show(b);
	n_label=gtk_label_new(lconv(_("Initial value of dynamic element is enable")));
	gtk_box_pack_start(GTK_BOX(hbox),n_label,FALSE,FALSE,0);
	gtk_widget_show(n_label);
	gtk_widget_show(hbox);

	hbox=gtk_hbox_new(FALSE,4);
	gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
	n_label=gtk_label_new(lconv(_("Group frame label adjust level:")));
	gtk_widget_show(n_label);
	gtk_box_pack_start(GTK_BOX(hbox),n_label,FALSE,FALSE,0);
	gtk_widget_show(hbox);
	g_l=conf.g_l;
	hbox=gtk_hbox_new(FALSE,4);
	gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
	b=gtk_radio_button_new_with_label(NULL,lconv(_("None")));
	gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
	gtk_widget_show(b);
	if( g_l==0 ) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),TRUE);
	gp=gtk_radio_button_group(GTK_RADIO_BUTTON(b));
	b=gtk_radio_button_new_with_label(gp,lconv(_("Mid")));
	gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
	gtk_widget_show(b);
	if( g_l==1 ) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),TRUE);
	gp=gtk_radio_button_group(GTK_RADIO_BUTTON(b));
	b=gtk_radio_button_new_with_label(gp,lconv(_("MAX")));
	gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
	gtk_widget_show(b);
	if( g_l==2 ) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),TRUE);
	G_l=gtk_radio_button_group(GTK_RADIO_BUTTON(b));
	gtk_widget_show(hbox);

#if 0
	Asetv1=conf.aset_vol1;
	hbox=gtk_hbox_new(FALSE,4);
	gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
	n_label=gtk_label_new(lconv(_("Volume1 of auto unmute")));
	gtk_box_pack_start(GTK_BOX(hbox),n_label,FALSE,FALSE,0);
	gtk_widget_show(n_label);
	gtk_widget_show(hbox);
	hbox=gtk_hbox_new(FALSE,4);
	gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
	adj=(GtkAdjustment *)gtk_adjustment_new((gfloat)(conf.aset_vol1),
											0,101,1.0,4.0,1.0);
	ve1=gtk_entry_new_with_max_length(3);
	gtk_signal_connect(GTK_OBJECT(adj),"value_changed",
					   GTK_SIGNAL_FUNC(aset_callback),
					   (gpointer)ve1);
	v1=gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_draw_value(GTK_SCALE(v1),FALSE);
	gtk_box_pack_start(GTK_BOX(hbox),v1,TRUE,TRUE,0);
	gtk_widget_show(v1);
	gtk_entry_set_editable(GTK_ENTRY(ve1),FALSE);
	sprintf(vs,"%3d",conf.aset_vol1);
	gtk_entry_set_text(GTK_ENTRY(ve1),vs);
	gtk_box_pack_start(GTK_BOX(hbox),ve1,FALSE,FALSE,0);
	gtk_widget_show(ve1);
	gtk_widget_size_request(ve1,&rq);
	gtk_widget_set_usize(ve1,rq.height+rq.height/2-4,rq.height);
	n_label=gtk_label_new(lconv("%"));
	gtk_box_pack_start(GTK_BOX(hbox),n_label,FALSE,FALSE,0);
	gtk_widget_show(n_label);
	gtk_widget_show(hbox);
#endif

	n_label=gtk_label_new(lconv(_("OPT")));
	gtk_widget_show(box);
	gtk_widget_show(frame);
	gtk_notebook_append_page(GTK_NOTEBOOK(nb),frame,n_label);
	
	/* Mixer */
	ccard=(c_card_t *)g_malloc(card_last*sizeof(c_card_t));
	if( ccard == NULL ) {
			pr_err(nomem_msg);
			return -1;
	}
	for( i=0 ; i<card_last ; i++ ) {
		if( !cards[i].controls ) continue;

		c=ccard+i;
		n_label=gtk_label_new(lconv(snd_ctl_card_info_get_name(cards[i].info)));
		frame=gtk_frame_new(NULL);
		gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_IN);
		gtk_container_set_border_width(GTK_CONTAINER(frame),20);
		gtk_notebook_append_page(GTK_NOTEBOOK(nb),frame,n_label);

		box=gtk_vbox_new(FALSE,2);
		gtk_container_set_border_width(GTK_CONTAINER(box),10);
		gtk_container_add(GTK_CONTAINER(frame),box);

		hbox=gtk_hbox_new(FALSE,4);
		gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
			
		c->m_en=cards[i].enable;
		b=gtk_toggle_button_new();
		gtk_widget_set_usize(b,10,10);
		gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),
									 c->m_en);
		gtk_signal_connect(GTK_OBJECT(b),"toggled",
						   GTK_SIGNAL_FUNC(tb_callback),
						   (gpointer)&c->m_en);
		gtk_widget_show(b);
		n_label=gtk_label_new(lconv(snd_ctl_card_info_get_name(cards[i].info)));
		gtk_box_pack_start(GTK_BOX(hbox),n_label,FALSE,FALSE,0);
		gtk_widget_show(n_label);
		gtk_widget_show(hbox);
			
		if( cards[i].p_e ) {
			if( cards[i].p_f ) k=2; else k=1;
		} else k=0;
		hbox=gtk_hbox_new(FALSE,4);
		gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,0);
		n_label=gtk_label_new(lconv(_("Spacing: ")));
		gtk_box_pack_start(GTK_BOX(hbox),n_label,FALSE,FALSE,0);
		gtk_widget_show(n_label);

		b=gtk_radio_button_new_with_label(NULL,lconv(_("NONE")));
		gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
		gtk_widget_show(b);
		if( k==0 ) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),
												TRUE);

		gp=gtk_radio_button_group(GTK_RADIO_BUTTON(b));

		b=gtk_radio_button_new_with_label(gp,lconv(_("space")));
		gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
		gtk_widget_show(b);
		if( k==1 ) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),
												TRUE);
		gp=gtk_radio_button_group(GTK_RADIO_BUTTON(b));

		b=gtk_radio_button_new_with_label(gp,lconv(_("expand")));
		gtk_box_pack_start(GTK_BOX(hbox),b,FALSE,FALSE,0);
		gtk_widget_show(b);
		if( k==2 ) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(b),
												TRUE);
		c->gp=gtk_radio_button_group(GTK_RADIO_BUTTON(b));
		gtk_widget_show(hbox);

		hbox=gtk_hbox_new(FALSE,0);
		vvbox=gtk_vbox_new(FALSE,0);
		n_label=gtk_vlabel(_("Left"));
		gtk_box_pack_start(GTK_BOX(vvbox),n_label,FALSE,FALSE,0);
		gtk_widget_show(n_label);
		n_label=gtk_vlabel(_("->"));
		gtk_box_pack_start(GTK_BOX(vvbox),n_label,TRUE,FALSE,0);
		gtk_widget_show(n_label);
		n_label=gtk_vlabel(_("Order"));
		gtk_box_pack_start(GTK_BOX(vvbox),n_label,FALSE,FALSE,0);
		gtk_widget_show(n_label);
		n_label=gtk_vlabel(_("<-"));
		gtk_box_pack_start(GTK_BOX(vvbox),n_label,TRUE,FALSE,0);
		gtk_widget_show(n_label);
		n_label=gtk_vlabel(_("Right"));
		gtk_box_pack_start(GTK_BOX(vvbox),n_label,FALSE,FALSE,0);
		gtk_widget_show(n_label);
		

		gtk_box_pack_start(GTK_BOX(hbox),vvbox,FALSE,FALSE,2);
		gtk_widget_show(vvbox);

		/* ctree obj list */
		ctree=gtk_ctree_new(3,0);
		c->ct=GTK_CTREE(ctree);
		gtk_ctree_set_drag_compare_func(GTK_CTREE(ctree),
										(GtkCTreeCompareDragFunc)dc_func);
		gtk_clist_set_column_auto_resize (GTK_CLIST (ctree), 0, TRUE);
		gtk_clist_freeze(GTK_CLIST(ctree));
		gtk_clist_set_selection_mode(GTK_CLIST(ctree),
									 GTK_SELECTION_MULTIPLE);
		gtk_clist_set_column_width(GTK_CLIST(ctree),0,6);
		gtk_clist_set_column_width(GTK_CLIST(ctree),1,18);
		gtk_clist_set_reorderable(GTK_CLIST(ctree),TRUE);
		gtk_clist_set_column_justification(GTK_CLIST(ctree),
										   1,GTK_JUSTIFY_RIGHT);
		gtk_clist_set_column_justification(GTK_CLIST(ctree),
										   2,GTK_JUSTIFY_LEFT);

		for( j=0,obj=cards[i].objs ; obj != NULL ; obj=obj->next,j++);

		c->o_nums=j;
		c->cobjs=(c_cobj_t *)g_malloc(j*sizeof(c_cobj_t));
		c->ord_l=(gint *)g_malloc(j*sizeof(gint));
		if( c->cobjs == NULL || c->ord_l == NULL ) {
			pr_err(nomem_msg);
			g_free(ccard);
			return -1;
		}
		memset(c->cobjs,0,j*sizeof(c_cobj_t));
		obj=cards[i].objs;
		cobj=c->cobjs;
		for( k=0 ; k<j ; k++ ) {
			sprintf(cl_num,"%d",k+1);
			strcpy(gname,obj->name);
			cl_data[0]=" ";
			cl_data[1]=cl_num;
			cl_data[2]=gname;
			if( obj->es[0].type == EO_OBJ ) {
				ct_lf=FALSE;
				ct_ex=TRUE;
			} else {
				ct_lf=TRUE;
				ct_ex=FALSE;
			}
			cobj->node = gtk_ctree_insert_node(GTK_CTREE(ctree),NULL,
											   NULL,cl_data,0,
											   NULL,NULL,NULL,NULL,
											   ct_lf,ct_ex);
			gtk_ctree_node_set_row_data(c->ct,cobj->node,
										(gpointer)cobj);
			if( !ct_lf )
				comk_obj(cobj,obj->es,obj->es_num,GTK_CTREE(ctree));
			c->cobjs[k].en=obj->enable;
			if( obj->enable )
				gtk_ctree_select(GTK_CTREE(ctree),cobj->node);
			obj=obj->next;
			cobj++;
		}
		adj=(GtkAdjustment *)gtk_adjustment_new(0.0,0.0,10.0,1.0,4.0,4.0);
		gtk_clist_set_vadjustment(GTK_CLIST(ctree),adj);
		gtk_widget_size_request(hbox,&rq);
		gtk_widget_set_usize(ctree,rq.width,150);
		gtk_signal_connect(GTK_OBJECT(ctree),"tree_select_row",
						   SEL_CB,(gpointer)c->ct);
		gtk_signal_connect(GTK_OBJECT(ctree),"tree_unselect_row",
						   UNSEL_CB,(gpointer)c->ct);
		gtk_widget_show(ctree);
		gtk_box_pack_start(GTK_BOX(hbox),ctree,TRUE,TRUE,0);
		vscbar=gtk_vscrollbar_new(adj);
		gtk_widget_show(vscbar);
		gtk_box_pack_start(GTK_BOX(hbox),vscbar,FALSE,FALSE,0);
		gtk_widget_show(hbox);
		gtk_box_pack_start(GTK_BOX(box),hbox,TRUE,TRUE,0);
		gtk_clist_thaw(GTK_CLIST(ctree));
		gtk_widget_show(box);
		gtk_widget_show(frame);
	}
	
	gtk_widget_show(nb);
	/* buttons */
	box=gtk_hbutton_box_new();
	gtk_button_box_set_layout(GTK_BUTTON_BOX(box),GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(box),5);
	gtk_box_pack_end(GTK_BOX(vbox),box,FALSE,FALSE,0);

	b=gtk_button_new_with_label(lconv(_("OK")));
	gtk_box_pack_start(GTK_BOX(box),b,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT(b),"clicked",GTK_SIGNAL_FUNC(ok_b),
					   (gpointer)FALSE);
	GTK_WIDGET_SET_FLAGS(b,GTK_CAN_DEFAULT);
	gtk_widget_show(b);
	gtk_widget_grab_default(b);

	Tosave=FALSE;
	b=gtk_button_new_with_label(lconv(_("SAVE")));
	gtk_box_pack_start(GTK_BOX(box),b,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT(b),"clicked",GTK_SIGNAL_FUNC(ok_b),
					   (gpointer)TRUE);
	GTK_WIDGET_SET_FLAGS(b,GTK_CAN_DEFAULT);
	gtk_widget_show(b);

	b=gtk_button_new_with_label(lconv(_("CANCEL")));
	gtk_box_pack_start(GTK_BOX(box),b,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT(b),"clicked",GTK_SIGNAL_FUNC(cancel_b),NULL);
	GTK_WIDGET_SET_FLAGS(b,GTK_CAN_DEFAULT);
	gtk_widget_show(b);

	gtk_widget_show(box);

	gtk_widget_show(vbox);
	gtk_widget_show(c_win);
	
	gtk_grab_add(c_win);
	gtk_main();

	changed=FALSE;

	if( ok_pushed ) {
		if ( conf.scroll != scrolled ) changed=TRUE;
		conf.scroll=scrolled;
		conf.Esave = Esaved;
		conf.sv_wsize=sv_wsized;
		conf.i_dyn=Idyn;
		if( conf.g_l != g_l ) changed=TRUE;
		conf.g_l=g_l;
		//conf.aset_vol1=Asetv1;
	}	
	for( i=0 ; i<card_last ; i++ ) {
		if( !cards[i].controls ) continue;
		c=ccard+i;
		if( ok_pushed ) {
			if( !changed ) {
				if( cards[i].enable != c->m_en )
					changed = TRUE;
			}
			cards[i].enable=c->m_en;
			cards[i].enabled=FALSE;
			if( !changed ) {
				if( cards[i].p_e != c->p_e ||
					cards[i].p_f != c->p_f )
					changed=TRUE;
			}
			cards[i].p_e=c->p_e;
			cards[i].p_f=c->p_f;

			j=0;
			sf=0;
			for( obj=cards[i].objs ; obj != NULL ; obj=obj->next ) {
				if( !changed ) {
					if( c->ord_l[j] != j ) changed=TRUE;
					if( obj->enable != c->cobjs[c->ord_l[j]].en ) {
						changed = TRUE;
					}
				}
				if( c->ord_l[j] != j ) sf=1;
				obj->enable=c->cobjs[j].en;
				obj->enabled=FALSE;
				if( c->cobjs[j].nexts ) cobj_en(c->cobjs+j,obj,&changed);
				j++;
			}
			if( sf ) {
				o_l=(gint *)g_malloc(sizeof(gint)*c->o_nums);
				if( o_l != NULL ) {
					for( j=0 ; j<c->o_nums ; j++ ) o_l[j]=j;
					obj_b=NULL;
					obj=cards[i].objs;
					for( j=0 ; j<c->o_nums ; j++ ) {
						if( c->ord_l[j] != o_l[j] ) {
							obj2=obj;
							for( l=j ; c->ord_l[j]!=o_l[l] ; l++ ) {
								obj2_b=obj2;
								obj2=obj2->next;
							}
							for( m=l ; m>j ; m-- ) o_l[m]=o_l[m-1];
							o_l[m]=j;
							if( obj_b == NULL ) {
								cards[i].objs=obj2;
							} else {
								obj_b->next=obj2;
							}
							obj2_b->next=obj2->next;
							obj2->next=obj;
							obj=obj2;
						}
						obj_b=obj;
						obj=obj->next;
					}
					g_free(o_l);
				}
			}
		}
		g_free(c->cobjs);
		g_free(c->ord_l);
	}
	g_free(ccard);
	if( Tosave ) {
		conf_write();
		conf.F_save=FALSE;
	}

	return changed;
}

static void chk_cfile( void ) {
	int i,j,k,err;
	gchar *name;

	k=strlen(g_get_home_dir());
	name=g_strdup(conf.fna);
	i=1;
	j=strlen(name)-1;
	err=-1;
	while( i>0 ) {
		if( err<0 ) {
			while( name[j] != '/' ) j--;
			name[j]=0;
			if( j <= k ) {
				pr_err(_("Can not make dir ~/.gamix\n"));
				g_free(name);
				return;
			}
		} else {
			while( name[j] != 0 ) j++;
			name[j]='/';
		}
		err=mkdir(name,S_IRUSR|S_IWUSR|S_IXUSR|
				       S_IRGRP|S_IXGRP| S_IROTH|S_IXOTH);
		if( err<0 ) {
			if( errno == ENOENT ) {
				i++;
			} else {
				sprintf(err_msg,"Can not make dir %s\n",name);
				pr_err(err_msg);
				g_free(name);
				return;
			}
		} else {
			i--;
		}
	}
}

#define REOS_ST_MAX 10
static c_reos_t *reos_st[REOS_ST_MAX];
static int reos_st_lv=0;
static int c_o_push(c_reos_t *obj) {
	if( reos_st_lv==REOS_ST_MAX ) return -1;
	reos_st[reos_st_lv++]=obj;
	//printf("Push %p\n",obj);
	return reos_st_lv;
}
static c_reos_t *c_o_pop(void) {
	if( reos_st == 0 ) return NULL;
	//printf("Pop  %p\n",reos_st[reos_st_lv-1]);
	return reos_st[--reos_st_lv];
}
static c_reos_t *c_o_new(c_reos_t **objs,int type) {
	c_reos_t *obj;
	obj=(c_reos_t *)malloc(sizeof(c_reos_t));
	if( obj == NULL ) {
		pr_err(nomem_msg);
		return NULL;
	}
	memset(obj,0,sizeof(c_reos_t));
	obj->type=type;
	*objs=obj;
	return obj;
}
static int getusc(char *s,int *sc) {
	int i,j;
	if( *s < '0' || *s > '9' ) return -1;
	i=0;j=0;
	while( *s >='0' && *s <= '9' ) {
		j=j*10+(*(s++)-'0');
		i++;
	}
	*sc=j;
	return i;
}
static s_element_t *cmk_elem(c_relem_t *re) {
	s_element_t *e;

#ifdef DISP_DEBUG
	{
		int i;
		for(i=c_idlv ; i>0 ; i-- ) printf("  ");
		printf("E %d,'%s',%d\n",re->numid,re->name,re->index);
	}
#endif
	e=(s_element_t *)malloc(sizeof(s_element_t));
	if( e==NULL ) {
		pr_err(nomem_msg);
		return NULL;
	}
	memset(e,0,sizeof(s_element_t));
	e->card=c_card_no;
	e->ctl=c_card->controls+re->numid-1;
	if( add_ep(e->ctl,e)<0 ) return NULL;
	c_card->ctl_used[re->numid-1]=TRUE;
	return e;
}
static s_obj_t *cmk_obj(c_robj_t *robj) {
	int i;
	s_obj_t *obj;
	s_eos_t *ep;
	c_reos_t *es,*ess;
	s_element_t *e;

#ifdef DISP_DEBUG
	for(i=c_idlv ; i>0 ; i-- ) printf("  ");
	printf("G '%s' %d\n",robj->name,robj->en);
	c_idlv++;
#endif
	obj=(s_obj_t *)malloc(sizeof(s_obj_t));
	if( obj == NULL ) {
		pr_err(nomem_msg);
		return NULL;
	}
	memset(obj,0,sizeof(s_obj_t));
	memcpy(obj->name,robj->name,sizeof(obj->name));
	obj->enable=robj->en;
	for(i=0,es=robj->es ; es !=NULL ; i++,es=es->next );
	if( i==0 ) {
		printf("%s %s:%d\n",__FILE__, __FUNCTION__,__LINE__);
		free(obj);
		return NULL;
	}
	obj->es_num=i;
	ep=obj->es=(s_eos_t *)malloc(i*sizeof(s_eos_t));
	if( ep == NULL ) {
		pr_err(nomem_msg);
		return NULL;
	}
	for( i=obj->es_num,es=robj->es ; i>0 ; i-- ) {
		ep->type=es->type;
		switch(es->type) {
		case EO_OBJ:
			if( (ep->eo.o=cmk_obj(es->eo.o))==NULL ) return NULL;
			free(es->eo.o);
			break;
		case EO_ELEM:
			if( (ep->eo.e=cmk_elem(es->eo.e))==NULL ) return NULL;
			e=ep->eo.e;
			if( snd_ctl_elem_info_get_type(e->ctl->info) == 
				SND_CTL_ELEM_TYPE_INTEGER && es->eo.e->opt ) {
				int j,k;
				j=getusc(es->eo.e->opt+1,&k);
				if( j>0 ) {
					e->atype = k;
				}
			}
			if( es->eo.e->opt ) free(es->eo.e->opt);
			free(es->eo.e);
			e->chain=&obj->chain;
			e->chain_en=&obj->chain_en;
			e->enabled=&obj->enabled;
			break;
		case EO_LABEL:
			ep->eo.l=strdup(es->eo.l);
			if( ep->eo.l == NULL ) {
				pr_err(nomem_msg);
				return NULL;
			}
			free(es->eo.l);
			break;
		}
		ess=es->next;
		free(es);
		es=ess;
		ep++;
	}
	//printf(__FILE__ " " __FUNCTION__ ":%d\n",__LINE__);
#ifdef DISP_DEBUG
	c_idlv--;
#endif
	return obj;
}
static int chk_obj(c_robj_t *obj) {
	int i=0,rt=0;
	c_reos_t *es,*ess;

	es=obj->es;
	while( es ) {
		switch( es->type ) {
		case EO_OBJ:
			i=chk_obj(es->eo.o);
			if( i == 0 ) {
				es->eo.o=NULL;
			} else
				rt+=i;
			break;
		case EO_ELEM:
			rt++;
			break;
		}
		es=es->next;
	}
	if( rt==0 ) {
		es=obj->es;
		while( es ) {
			ess=es;
			es=es->next;
			switch( ess->type ) {
			case EO_OBJ:
				if( ess->eo.o ) free(ess->eo.o);
				break;
			case EO_LABEL:
				if( ess->eo.l ) free(ess->eo.l);
				break;
			}
			free(ess);
		}
	} else {
		c_reos_t **e;
		void *p=NULL;
		e=&obj->es;
		while( *e ) {
			switch( (*e)->type ) {
			case EO_OBJ:
				p=(*e)->eo.o;
				break;
			case EO_ELEM:
				p=(*e)->eo.e;
				break;
			case EO_LABEL:
				p=(*e)->eo.l;
				break;
			}
			if( !p ) {
				es=*e;
				*e=es->next;
				free(es);
			} else
				e=&((*e)->next);
		}
	}
	return rt;
}
static void cread_err(gchar *s,int n ) {
	sprintf(err_msg,_("config %d:%s\n"),n,s);
	pr_err(err_msg);
}
static c_reos_t *creos_free(c_reos_t *o) {
	c_reos_t *rt=o->next;
	switch( o->type ) {
	case EO_OBJ:
#if DISP_DEBUG
		printf("free G %s\n",o->eo.o->name);
#endif
		{
			c_reos_t *p=o->eo.o->es;
			while( p ) {
				p=creos_free(p);
			}
		}
		break;
	case EO_ELEM:
#if DISP_DEBUG
		printf("free E %d,'%s',%d\n",o->eo.e->numid,o->eo.e->name,
			   o->eo.e->index);
#endif
		if( o->eo.e->opt ) free(o->eo.e->opt);
		break;
	case EO_LABEL:
#if DISP_DEBUG
		printf("free L %s\n",o->eo.l);
#endif
		free(o->eo.l);
		break;
	}
	free(o);
	return rt;
}

static void get_id_name(char **src,char *dst,int len) {
	char *st=*src,*s;
	const char sep=*(st++);
	int l;

	s=st+strlen(st);
	while( *s != sep && s>=st ) s--;
	l=s-st;
	if( s >= st && *s == sep && l<len ) {
		memcpy(dst,st,l);
		dst[l]=0;
		s++;
		*src=s;
		return;
	}
	**src=0;
}

void conf_read( void ) {
	int i,j,err,ln,olv=0;
	FILE *fp;
	gchar rbuf[256],*s;
	s_card_t *c=NULL;
	c_reos_t *mtree=NULL,**objs=&mtree,*obj;
	c_robj_t *robj;
	c_relem_t *relem;
	char *ver_st=NULL;
	char id;

	fp=fopen(conf.fna,"rt");
	if( fp == NULL ) {
		conf.F_save=TRUE;
		return;
	}
	ln=1;
	err=0;
	while( !feof(fp) && err>-5 ) {
		rbuf[0]=0;
		fgets(rbuf,255,fp);
		rbuf[255]=0;
		s=rbuf;
		err=0;
		while( *s && (*s ==' ' || *s=='\t' ) ) s++;
		id=*s;
		s+=2;
		switch( id ) {
		case '#':
			if( ln > 1 ) break;
			ver_st=strdup(s);
			s=ver_st;
			while( *s >=' ' ) s++;
			if( *s ) *s=0;
			if( strcmp(ver_st,VERSION) == 0 ) {
				break;
			}
			conf.F_save=TRUE;
			s=ver_st;
			i=0;
			while( *s>='0' && *s<='9' ) {
				i++;
				s++;
			}
			j=0;
#ifdef DISP_DEBUG
			printf(" ver %s %s %d\n",ver_st,VERSION,i);
#endif
			if( i>0 && strncmp(ver_st,VERSION,i) == 0 ) {
				s=g_strdup_printf(_("Warning: conf file %s is same major,\nbut different minor version."),conf.fna);
				dwin_01(s,_("OK"),NULL);
				g_free(s);
				j=0;
			} else {
				s=g_strdup_printf(_("No version or different major version in conf file %s.\nContinue to read conf file?"),conf.fna);
				j=dwin_01(s,_("YES"),_("NO"));
				g_free(s);
			}
			if( j ) err=-6;
#ifdef DISP_DEBUG
			printf("err %d\n",err);
#endif
			break;
		case 'D':
			conf.i_dyn=atoi(s)?TRUE:FALSE;
			break;
		case 'S':
			conf.scroll=atoi(s)?TRUE:FALSE;
			break;
		case 'C':
			i=atoi(s);
			if( i<0 || i>2 ) {
				err=-1;
			} else conf.wmode=i;
			break;
		case 'Y':
			conf.sv_wsize=atoi(s)?TRUE:FALSE;
			break;
		case 'W':
			sscanf(s,"%d,%d\n",&conf.width,&conf.height);
			break;
		case 'A':
			conf.Esave=atoi(s)?TRUE:FALSE;
			break;
		case 'B':
			i=atoi(s);
			if( i<0 || i>2 ) {
				err=-1;
			} else
				conf.g_l=i;
			break;
#if 0
		case 'V':
			x=atoi(s);
			if( x >= 0 && x<= 100 ) {
				conf.aset_vol1=x;
			} else {
				cread_err(_("Invalied volume1 value."),ln);
				err=-1;
			}
			break;
#endif
		case 'M':
			sscanf(s,"%d=%d\n",&i,&j);
			if( i<0 || i>=card_last ) {
				cread_err(_("Invalied card No."),ln);
				err=-10;
				break;
			}
			if( mtree ) {
				obj=mtree;
				while( obj ) {
					obj=creos_free(obj);
				}
			}
			c=cards+i;
			if( c->controls == NULL ) {
				c=NULL;
				cread_err(_("Invalied card No."),ln);
				err=-10;
				break;
			}
			c->enable=j?TRUE:FALSE;

			c_card=c;
			c_card_no=i;

			olv=0;
			objs=&mtree;
			break;
		case 'X':
			if( c == NULL ) {
				cread_err(_("No mixer selected"),ln);
				err=-1;
			}
			switch(atoi(s)) {
			case 0:
				c->p_e=FALSE;
				c->p_f=FALSE;
				break;
			case 1:
				c->p_e=TRUE;
				c->p_f=FALSE;
				break;
			case 2:
				c->p_e=TRUE;
				c->p_f=TRUE;
				break;
			default:
				cread_err(_("Invalied value for X"),ln);
				err=-1;
				break;
			}
			break;
		case 'G':
			if( c == NULL ) {
				cread_err(_("No mixer selected"),ln);
				err=-1;
				break;
			}
			if( (obj=c_o_new(objs,EO_OBJ)) == NULL ) {
				err=-10;
				break;
			}
			robj=obj->eo.o=(c_robj_t *)malloc(sizeof(c_robj_t));
			if( robj == NULL ) {
				pr_err(nomem_msg);
				free(obj);
				err=-10;
				break;
			}
			memset(robj,0,sizeof(c_robj_t));
			get_id_name(&s,robj->name,44);
			if( *s == 0 ) {
				cread_err(_("Invalied argument"),ln);
				free(robj);
				free(obj);
				err=-1;
				*objs=NULL;
				break;
			}
			sscanf(s,"=%d\n",&i);
			robj->en=(i==0)?FALSE:TRUE;
			c_o_push(obj);
			objs=&(obj->eo.o->es);
			olv++;
			break;
		case 'E':
			if( c == NULL ) {
				cread_err(_("No mixer selected"),ln);
				err=-1;
				break;
			}
			if( olv == 0 ) {
				cread_err(_("Wrong E"),ln);
				err=-1;
				break;
			}
			if( (obj=c_o_new(objs,EO_ELEM)) == NULL ) {
				err=-10;
				break;
			}
			relem=obj->eo.e=(c_relem_t *)malloc(sizeof(c_relem_t));
			if( relem == NULL ) {
				pr_err(nomem_msg);
				free(obj);
				err=-10;
				break;
			}
			memset(relem,0,sizeof(c_relem_t));

			i=getusc(s,&j);
			if( i<0 ) {
				cread_err(_("Wrong E"),ln);
				free(obj->eo.e);
				free(obj);
				*objs=NULL;
				err=-1;
				break;
			}
			s+=i;
			if( *s != ',' ) {
				cread_err(_("Wrong E"),ln);
				free(obj->eo.e);
				free(obj);
				*objs=NULL;
				err=-1;
				break;
			}
			relem->numid=j;
			s++;
			get_id_name(&s,relem->name,44);
			if( *s == 0 ) {
				cread_err(_("Wrong E"),ln);
				free(obj->eo.e);
				free(obj);
				*objs=NULL;
				err=-1;
				break;
			}
			s++;
			i=getusc(s,&j);
			if( i<0 ) {
				cread_err(_("Wrong E"),ln);
				free(obj->eo.e);
				free(obj);
				*objs=NULL;
				err=-1;
				break;
			}
			relem->index=j;
			s+=i;
			if( *s != 0 && *s != '\n' ) {
				relem->opt=strdup(s);
				if( relem->opt==NULL ) {
					pr_err(nomem_msg);
					err=-10;
					break;
				}
			}
			if( relem->numid > c->ctls_num ) {
				cread_err(_("Wrong E (no control numid) "),ln);
				free(obj->eo.e);
				free(obj);
				*objs=NULL;
				err=-1;
				break;
			}
			{
				s_ctl_t *ctl = c->controls+relem->numid-1;
				snd_ctl_elem_id_t *id=ctl->id;

				if( relem->numid != snd_ctl_elem_id_get_numid(id) ||
					relem->index != snd_ctl_elem_id_get_index(id) ||
					strcmp(relem->name,snd_ctl_elem_id_get_name(id)) != 0 ) {
					cread_err(_("Wrong E (not match control_id) "),ln);
					if( obj->eo.e->opt ) free(obj->eo.e->opt);
					free(obj->eo.e);
					free(obj);
					*objs=NULL;
					err=-1;
					break;
				}
			}
			objs=&(obj->next);
			break;
		case 'L':
			if( c == NULL ) {
				cread_err(_("No mixer selected"),ln);
				err=-1;
				break;
			}
			if( olv == 0 ) {
				cread_err(_("Wrong L"),ln);
				err=-1;
				break;
			}
			if( (obj=c_o_new(objs,EO_LABEL)) == NULL ) {
				err=-10;
				break;
			}
			obj->eo.l=strdup(s);
			s=obj->eo.l;
			while( *s )
				if( *s == '\n' ) *s=0; else s++;
			objs=&(obj->next);
			break;
		case '}':
			if( olv == 0 ) {
				if( c == NULL ) {
					cread_err(_("M not start"),ln);
					err=-1;
					break;
				}
				{
					s_obj_t **cobjs=&(c->objs);
					c_reos_t *o;
					for( obj=mtree ; obj != NULL ;  ) {
						if( chk_obj(obj->eo.o) ) {
							*cobjs=cmk_obj(obj->eo.o);
							if( *cobjs == NULL ) exit(0);
							cobjs=&((*cobjs)->next);
						}
						o=obj->next;
						free(obj);
						obj=o;
					}
				}
				mtree=NULL;
				//printf("HOe\n");
				//exit(0);
				break;
			}
			obj=c_o_pop();
#if 0
			if( obj == NULL ) {
				cread_err(_("G not start"),ln);
				err=-1;
				break;
			}
#endif
			olv--;
			objs=&(obj->next);
			break;
		}
		if( err<0 ) conf.F_save=TRUE;
		ln++;
	}
	fclose(fp);
	if( ver_st ) free(ver_st);
}
static void write_objs(s_obj_t *obj,int il,FILE *fp) {
	int i,j;
	s_eos_t *es;

	while( obj != NULL ) {
		for(j=il;j>0 ; j--) fputc(' ',fp);
		il++;
		fprintf(fp,"G '%s'=%d {\n",obj->name,obj->enable);
		es=obj->es;
		for( i=0 ; i<obj->es_num ; i++ ) {
			switch(es[i].type) {
			case EO_OBJ:
				write_objs(es[i].eo.o,il+1,fp);
				break;
			case EO_ELEM:
				{
					s_ctl_t *ctl=es[i].eo.e->ctl;
					snd_ctl_elem_id_t *id=ctl->id;
					for(j=il;j>0 ; j--) fputc(' ',fp);
					if( snd_ctl_elem_info_get_type(ctl->info)
						== SND_CTL_ELEM_TYPE_INTEGER )
						fprintf(fp,"E %d,'%s',%d,%d\n",
								snd_ctl_elem_id_get_numid(id),
								snd_ctl_elem_id_get_name(id),
								snd_ctl_elem_id_get_index(id),
								es[i].eo.e->atype);
					else
						fprintf(fp,"E %d,'%s',%d\n",
								snd_ctl_elem_id_get_numid(id),
								snd_ctl_elem_id_get_name(id),
								snd_ctl_elem_id_get_index(id));
				}
				break;
			case EO_LABEL:
				for(j=il;j>0 ; j--) fputc(' ',fp);
				fprintf(fp,"L %s\n",es[i].eo.l);
				break;
			}
		}
		il--;
		for(j=il;j>0 ; j--) fputc(' ',fp);
		fprintf(fp,"}\n");
		obj=obj->next;
	}
}

void conf_write( void ) {
	int i,j,k;
	FILE *fp;

	fp=fopen(conf.fna,"wt");
	if( fp == NULL ) {
		chk_cfile();
		fp=fopen(conf.fna,"wt");
	}
	if( fp == NULL ) {
		sprintf(err_msg,_("gamix: config file not saved.\n"));
		pr_err(err_msg);
		return;
	}

	fprintf(fp,"# %s\n",VERSION);
	fprintf(fp,"# OPT\n");
	fprintf(fp,"D %d\n",conf.i_dyn);
	fprintf(fp,"S %d\n",conf.scroll);
	fprintf(fp,"C %d\n",conf.wmode);
	fprintf(fp,"B %d\n",conf.g_l);
	fprintf(fp,"A %d\n",conf.Esave);
	fprintf(fp,"Y %d\n",conf.sv_wsize);
	//fprintf(fp,"V %d\n",conf.aset_vol1);
	if( conf.sv_wsize ) {
		gdk_window_get_size(window->window,&i,&j);
		fprintf(fp,"W %d,%d\n",i,j);
	}
	for( i=0 ; i<card_last ; i++ ) {
		if( cards[i].controls == NULL ) continue;
		fprintf(fp,"# Card: %s\n#   Mixer: %s\n",
				snd_ctl_card_info_get_name(cards[i].info),
				snd_ctl_card_info_get_longname(cards[i].info));
		fprintf(fp,"M %d=%d {\n",i,cards[i].enable);
		if( cards[i].p_e ) {
			if( cards[i].p_f ) k=2; else k=1;
		} else k=0;
		fprintf(fp,"X %d\n",k);
		write_objs(cards[i].objs,1,fp);
		fprintf(fp,"}\n");
	}
	fclose(fp);
}

static GtkWidget *win;

static void dwin_bt0(GtkWidget *w,int *rt) {
	*rt=0;
	gtk_widget_destroy(win);
}
static void dwin_bt1(GtkWidget *w,int *rt) {
	*rt=1;
	gtk_widget_destroy(win);
}

int dwin_01(char *msg,char *s1,char *s2) {
	GtkWidget *vbox,*hbox;
	GtkWidget *lmsg;
	GtkWidget *bt1,*bt2;
	GtkRequisition rq1,rq2;
	int rt=0;

	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(win),"destroy",
					   GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
	gtk_signal_connect(GTK_OBJECT(win),"delete_event",
					   GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
	vbox=gtk_vbox_new(FALSE,4);
	gtk_container_add(GTK_CONTAINER(win),vbox);
	hbox=gtk_hbox_new(FALSE,4);
#if GTK_CHECK_VERSION(1,3,0)
	lmsg=gtk_label_new(lconv(msg));
#else
	lmsg=gtk_text_new(NULL,NULL);
	gtk_text_set_editable(GTK_TEXT(lmsg),FALSE);
	gtk_text_set_word_wrap(GTK_TEXT(lmsg),TRUE);
	gtk_text_insert(GTK_TEXT(lmsg),NULL,NULL,NULL,msg,-1);
#endif
	gtk_widget_show(lmsg);
	gtk_box_pack_start(GTK_BOX(hbox),lmsg,TRUE,TRUE,10);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,FALSE,8);
	gtk_widget_size_request(lmsg,&rq1);
	lmsg=gtk_hseparator_new();
	gtk_widget_show(lmsg);
	gtk_widget_size_request(lmsg,&rq2);
	gtk_widget_set_usize(lmsg,(rq1.width*2)+16,rq2.height);
	gtk_box_pack_start(GTK_BOX(vbox),lmsg,FALSE,FALSE,0);
	hbox=gtk_hbox_new(TRUE,0);
	bt1=gtk_button_new_with_label(lconv(s1));
	gtk_signal_connect(GTK_OBJECT(bt1),"clicked",GTK_SIGNAL_FUNC(dwin_bt0),
								  (gpointer)(&rt));
	gtk_widget_show(bt1);
	gtk_box_pack_start(GTK_BOX(hbox),bt1,TRUE,TRUE,16);
	if( s2 ) {
		bt2=gtk_button_new_with_label(lconv(s2));
		gtk_signal_connect(GTK_OBJECT(bt2),"clicked",GTK_SIGNAL_FUNC(dwin_bt1),
						   (gpointer)(&rt));
		gtk_widget_show(bt2);
		gtk_box_pack_start(GTK_BOX(hbox),bt2,TRUE,TRUE,16);
	}
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,4);
	gtk_widget_show(vbox);
	gtk_widget_show(win);
	gtk_main();

	return rt;
}

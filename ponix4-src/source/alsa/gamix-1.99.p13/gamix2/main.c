
#include "gamix.h"

GtkWidget *window;
GtkWidget *main_vbox;
GtkWidget *mixer_container;
GtkWidget *exit_item;
unsigned char *nomem_msg = N_("No enough memory.\n");
unsigned char err_msg[256];
int scfont_w=1;
#ifdef DISP_DEBUG
struct timeval tv1,tv2;
#endif

int main(int , char **);
void disp_toolbar(void);
static void exit_gtk(GtkWidget *,gpointer);
static void sel_mctype(GtkWidget *,gpointer);
static void conf_callback(GtkWidget *,gpointer);
static void aset_callback(GtkWidget *,gpointer);
static void aset_element(s_element_t *);

static void exit_gtk(GtkWidget *w,gpointer data) {
	gtk_main_quit();
}

int main( int argc , char **argv ) {
	int h,i;
	gchar *dirname,*filename,*cname=NULL;

#ifdef DISP_DEBUG
	gettimeofday(&tv1,NULL);
#endif
	//pr_mtime();
	i=probe_mixer();
	if( i < 0 ) {
		pr_err(_("Can not make mixer.\n"));
		return -1;
	}
	if( (i=time_init())<0 ) {
		pr_err(nomem_msg);
		return -1;
	}

#ifdef ENABLE_NLS
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif
	gtk_set_locale();
	gtk_init( &argc,&argv);

	h=0;
	while( (i=getopt(argc,argv,"c:h")) != -1 ) {
		switch(i) {
		case 'c':
			cname = g_strdup(optarg);
			break;
		case 'h':
			h=1;
			break;
		case ':':
			pr_err("hoe?\n");
			break;
		case '?':
			//fprintf(stderr,_("unknown option: %c\n"),optopt);
			h=1;
			break;
		}
	}

	if( h ) {
		printf("gamix ");
		if( strcmp(PACKAGE,"alsa-utils") == 0 ) {
			printf(" alsa utils version.");
		} else if( strcmp(PACKAGE,"gamix") == 0 ) {
			printf("%s original version.",VERSION);
		}
		putchar('\n');
		printf(_("Usage: gamix [OPTION]\n"));
		printf(_("  -h        print this help.\n"));
		printf(_("  -c [file] change config file.\n"));
		exit(0);
	}

	dirname = g_strconcat(g_get_home_dir(),"/.gamix",NULL);
	filename = g_strconcat(dirname, "/gtkrc", NULL);
	gtk_rc_init();
	gtk_rc_parse(filename);
	g_free(filename);

	conf.scroll=TRUE;
	conf.wmode=1;
	conf.F_save=FALSE;
	conf.Esave=FALSE;
	conf.g_l=2;
	if( cname ) {
		if( cname[0] == '/' ) {
			conf.fna = g_strdup(cname);
		} else {
			conf.fna = g_strconcat(dirname,"/",cname,NULL);
		}
	} else {
#ifdef ADD_CONFHOSTNAME
		{
			char hn[32];
			gethostname(hn,31);
			conf.fna=g_strconcat(dirname,"/Config_",hn,NULL);
		}
#else
		conf.fna = g_strconcat(dirname,"/Config",NULL);
#endif
	}
	conf.sv_wsize=TRUE;
	conf.width=0;
	conf.height=0;
	conf.aset_vol1=80;

	g_free(dirname);

	conf_read();
	group_check();

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect(GTK_OBJECT(window),"destroy",
					   GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
	gtk_signal_connect(GTK_OBJECT(window),"delete_event",
					   GTK_SIGNAL_FUNC(gtk_main_quit),NULL);
	main_vbox=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),main_vbox);

	disp_toolbar();

	gtk_timeout_add(100,(GtkFunction)time_callback,NULL);

	{
		GtkWidget *t;
		GtkRequisition rq;
		t=gtk_label_new("0123456789");
		gtk_widget_show(t);
		gtk_widget_size_request(t,&rq);
		gtk_widget_destroy(t);
		scfont_w=rq.width/10+1;
	}
	gtk_widget_show(main_vbox);
	if( disp_mixer()<0 ) return 0;
	gtk_widget_show(window);

	gtk_main();
	if( conf.F_save || conf.Esave ) {
		conf_write();
	}
	g_free(conf.fna);
	return 0;
}

void disp_toolbar(void) {
	GtkWidget *menu,*sub_menu,*sub_item;
	GtkWidget *frame;

	frame=gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_OUT);
	gtk_box_pack_start(GTK_BOX(main_vbox),frame,FALSE,FALSE,0);
	gtk_widget_show(frame);

	menu=gtk_menu_bar_new();
	gtk_container_add(GTK_CONTAINER(frame),menu);
	gtk_widget_show(menu);

	/* Prg menu */
	sub_menu=gtk_menu_new();

	sub_item=gtk_menu_item_new_with_label(_("config"));
	exit_item=sub_item;
	gtk_menu_append(GTK_MENU(sub_menu),sub_item);
	gtk_signal_connect(GTK_OBJECT(sub_item),"activate",
					   GTK_SIGNAL_FUNC(conf_callback),NULL);
	gtk_widget_show(sub_item);

#if 0
	sub_item=gtk_menu_item_new_with_label(_("Auto unmute"));
	exit_item=sub_item;
	gtk_menu_append(GTK_MENU(sub_menu),sub_item);
	gtk_signal_connect(GTK_OBJECT(sub_item),"activate",
					   GTK_SIGNAL_FUNC(aset_callback),NULL);
	gtk_widget_show(sub_item);
#endif

	sub_item=gtk_menu_item_new_with_label(_("exit"));
	exit_item=sub_item;
	gtk_menu_append(GTK_MENU(sub_menu),sub_item);
	gtk_signal_connect(GTK_OBJECT(sub_item),"activate",
					   GTK_SIGNAL_FUNC(exit_gtk),NULL);
	gtk_widget_show(sub_item);

	sub_item=gtk_menu_item_new_with_label(_("Prog"));
	gtk_widget_show(sub_item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(sub_item),sub_menu);
	gtk_menu_bar_append(GTK_MENU_BAR(menu),sub_item);

	/* mixer container type menu*/
	if( mdev_num > 1 ) {
		sub_menu=gtk_menu_new();

		sub_item=gtk_menu_item_new_with_label(_("Horizontal"));
		gtk_menu_append(GTK_MENU(sub_menu),sub_item);
		gtk_signal_connect(GTK_OBJECT(sub_item),"activate",
						   GTK_SIGNAL_FUNC(sel_mctype),0);
		gtk_widget_show(sub_item);

		sub_item=gtk_menu_item_new_with_label(_("Vertical"));
		gtk_menu_append(GTK_MENU(sub_menu),sub_item);
		gtk_signal_connect(GTK_OBJECT(sub_item),"activate",
						   GTK_SIGNAL_FUNC(sel_mctype),(gpointer)1);
		gtk_widget_show(sub_item);

		sub_item=gtk_menu_item_new_with_label(_("note book"));
		gtk_menu_append(GTK_MENU(sub_menu),sub_item);
		gtk_signal_connect(GTK_OBJECT(sub_item),"activate",
						   GTK_SIGNAL_FUNC(sel_mctype),(gpointer)2);
		gtk_widget_show(sub_item);

		sub_item=gtk_menu_item_new_with_label(_("C-type"));
		gtk_widget_show(sub_item);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(sub_item),sub_menu);
		gtk_menu_bar_append(GTK_MENU_BAR(menu),sub_item);
	}
}

static void sel_mctype(GtkWidget *w,gpointer n) {
	int i;

	i=(int)n;
	if( i == conf.wmode ) return;
	conf.wmode=i;
	conf.width=0;
	conf.height=0;
	gtk_container_remove(GTK_CONTAINER(main_vbox),mixer_container);
	if( (i=disp_mixer()) < 0 ) gtk_signal_emit_by_name(GTK_OBJECT(exit_item),
													   "activate");
}

int disp_mixer( void ) {
	int i,j;
	GtkWidget *n_label;
	GtkWidget *frame;
	GtkRequisition rq;

	//pr_mtime();
	switch( conf.wmode ) {
	case 0: /* H */
		if( conf.scroll ) {
			mixer_container=gtk_hbox_new(TRUE,0);
		} else {
			mixer_container=gtk_hbox_new(FALSE,0);
		}
		for( i=0 ; i<card_last ; i++ ) {
			if( cards[i].enable ) {
				cards[i].w=gtk_frame_new(NULL);
				gtk_frame_set_shadow_type(GTK_FRAME(cards[i].w),
										  GTK_SHADOW_ETCHED_IN);
				gtk_widget_show(cards[i].w);
				frame=make_mixer(i);
				if( !frame ) return -1;
				gtk_container_add(GTK_CONTAINER(cards[i].w),
								  frame);
				gtk_box_pack_start(GTK_BOX(mixer_container),
								   cards[i].w,TRUE,TRUE,2);
			}
		}
		break;
	case 1: /* V */
		mixer_container=gtk_vbox_new(FALSE,0);
		for( i=0 ; i<card_last ; i++ ) {
			if( cards[i].enable ) {
				cards[i].w=make_mixer(i);
				if( !cards[i].w ) return -1;
				gtk_box_pack_start(GTK_BOX(mixer_container),cards[i].w,
								   TRUE,TRUE,0);
			}
		}
		break;
	case 2: /* NoteBook */
		mixer_container=gtk_notebook_new();
		gtk_notebook_set_tab_pos(GTK_NOTEBOOK(mixer_container),GTK_POS_TOP);
		for( i=0 ; i<card_last ; i++ )
			if( cards[i].enable ) {
				cards[i].w=make_mixer(i);
				if( !cards[i].w ) return -1;
				n_label=gtk_label_new(snd_ctl_card_info_get_id(cards[i].info));
				gtk_notebook_append_page(GTK_NOTEBOOK(mixer_container),
										 cards[i].w,n_label);
			}
		break;
	}
	gtk_box_pack_start(GTK_BOX(main_vbox),mixer_container,TRUE,TRUE,0);
	gtk_widget_show(mixer_container);

	gtk_widget_size_request(window,&rq);
#ifdef DISP_DEBUG
	printf("window size w %d h %d\n",rq.width,rq.height);
	printf("conf.size w %d h %d\n",conf.width,conf.height);
#endif
	//if( (conf.width>=rq.width || conf.height >=rq.height) && !conf.F_save ) {
	if( (conf.width>=rq.width || conf.height >=rq.height) ) {
		if( conf.width<rq.width ) conf.width=rq.width;
		if( conf.height<rq.height ) conf.height=rq.height;
			gtk_window_set_default_size(GTK_WINDOW(window),
										conf.width,conf.height);
#ifdef DISP_DEBUG
			printf("resize to w %d h %d\n",conf.width,conf.height);
#endif
			//conf.width=0;
			//conf.height=0;
	}
	pr_mtime();
	return 0;
}

static void conf_callback(GtkWidget *w ,gpointer data) {
	gint err;
	//GtkRequisition rq;

	err=conf_win();
	if( err < 0 )  gtk_signal_emit_by_name(GTK_OBJECT(exit_item),"activate");
	if( err ) {
		gtk_container_remove(GTK_CONTAINER(main_vbox),mixer_container);
		if( disp_mixer() < 0 ) gtk_signal_emit_by_name(
										GTK_OBJECT(exit_item),"activate");
		//gtk_widget_size_request(window,&rq);
		//gdk_window_resize(window->window,rq.width,rq.height);
	}
}

static void aset_callback(GtkWidget *w ,gpointer data) {
	int i,j,k;
	s_obj_t *obj;

	for( i=0 ; i<card_last ; i++ ) {
		if( cards[i].enable ) {
			for( obj=cards[i].objs ; obj!=NULL ; obj=obj->next ) {
				if( obj->enable ) {
#ifdef DISP_DEBUG
					//printf(" obj: g %p e %p sw %p\n",obj->g,obj->e,obj->sw);
#endif
#if 0
					if( obj->g ) {
						group=obj->g;
						for( k=0 ; k<group->g.elements ; k++ ) {
							aset_element(group->e+k);
						}
					} else if( obj->e ) {
						aset_element(&obj->e->e);
					}
#endif
				}
			}
		}
	}
}

static void aset_element(s_element_t *e) {
	int i,v,f=0;

#if 0
	switch(e->e.eid.type) {
	case SND_MIXER_ETYPE_SWITCH1:
		for( i=0 ; i<e->e.data.switch1.sw ; i++ ) {
			snd_mixer_set_bit(e->e.data.switch1.psw,i,TRUE);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(e->w[i]),TRUE);
		}
		f=1;
		break;
	case SND_MIXER_ETYPE_SWITCH2:
		e->e.data.switch2.sw=TRUE;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(e->w[0]),TRUE);
		f=1;
		break;
	case SND_MIXER_ETYPE_VOLUME1:
		if( (e->info.data.volume1.prange[0].max - 
			 e->info.data.volume1.prange[0].min ) == 1 ) return;
		for( i=0 ; i<e->Evol1(e.) ; i++ ) {
			v=( e->info.data.volume1.prange[i].max - 
				e->info.data.volume1.prange[i].min ) * conf.aset_vol1/100;
			e->Epvol1(e.)[i]=v;
			e->adj[i]->value=(gfloat)(-v);
			gtk_signal_emit_by_name(GTK_OBJECT(e->adj[i]),"value_changed");
		}
		f=1;
		break;
	}
	if( f ) {
		snd_mixer_element_write(cards[e->card].mixer[e->mdev].handle,&e->e);
#ifdef DISP_DEBUG
		printf("ASET '%s',%d,%d\n",e->e.eid.name,e->e.eid.index,e->e.eid.type);
#endif
	}
#endif
}

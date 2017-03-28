
#include "gamix.h"
#include <err.h>
#include <errno.h>

static snd_ctl_event_t *ev=NULL;
static snd_ctl_elem_id_t *e_id=NULL;

static void cb_info(s_card_t *c,snd_ctl_elem_id_t *eid) {
	s_ctl_t *ctl;
	snd_ctl_elem_info_t *info;
	int err,i,j;
	long n;
	struct s_ep *p;
	GtkWidget **w;
	gboolean ss;

	n=snd_ctl_elem_id_get_numid(eid);
	ctl=&c->controls[n-1];
	if( snd_ctl_elem_id_get_numid(ctl->id) == n && 
		strcmp(snd_ctl_elem_id_get_name(eid),
			   snd_ctl_elem_id_get_name(ctl->id)) == 0 &&
		snd_ctl_elem_id_get_index(eid) == snd_ctl_elem_id_get_index(ctl->id)
		) {
		info=ctl->info;
		if( (err=snd_ctl_elem_info(c->handle,info))< 0 ) return;
		ss=is_access(ctl->info);
		p=ctl->el;
		while( p ) {
			if( *p->e->enabled ) {
				w=p->e->w;
				j=snd_ctl_elem_info_get_count(info)+((p->e->atype==2)?3:0);
				for( i=0 ; i<j; i++ ) {
					gtk_widget_set_sensitive(w[i],ss);
				}
			}
			p=p->next;
		}
	} else {
	}
}
static void cb_value(s_card_t *c,snd_ctl_elem_id_t *eid) {
	s_ctl_t *ctl;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_value_t *val;
	int err,i,count;
	long n;
	struct s_ep *p;
	GtkWidget **w;
	GtkAdjustment **adj;

	n=snd_ctl_elem_id_get_numid(eid);
	ctl=&c->controls[n-1];
	if( snd_ctl_elem_id_get_numid(ctl->id) == n && 
		strcmp(snd_ctl_elem_id_get_name(eid),
			   snd_ctl_elem_id_get_name(ctl->id)) == 0 &&
		snd_ctl_elem_id_get_index(eid) == snd_ctl_elem_id_get_index(ctl->id)
		) {
		info=ctl->info;
		if( is_access(info) ) {
			val=ctl->ctl;
			if( (err=snd_ctl_elem_read(c->handle,val)) < 0 ) {
				pr_err(_("control read err"));
				return;
			}
			p=ctl->el;
			count=snd_ctl_elem_info_get_count(info);
			while( p ) {
				if( *p->e->enabled ) {
					w=p->e->w;
					switch(snd_ctl_elem_info_get_type(info)) {
					case SND_CTL_ELEM_TYPE_BOOLEAN:
						for( i=0 ; i<count ; i++ )
							gtk_toggle_button_set_active(
												 GTK_TOGGLE_BUTTON(w[i]),
									 snd_ctl_elem_value_get_boolean(val,i));
						break;
					case SND_CTL_ELEM_TYPE_INTEGER:
						if( p->e->atype == 3 ) {
							char txt[10];
							for( i=0 ; i<count ; i++ ) {
								sprintf(txt,"%ld",snd_ctl_elem_value_get_integer(val,i));
								gtk_entry_set_text(GTK_ENTRY(p->e->w[i]),txt);
							}
							break;
						}
						adj=p->e->adj;
						for( i=0 ; i<count ; i++ ) {
							//printf(" %ld",ctl->ctl.value.integer.value[i]);
							if( (p->e->atype&1) == 0 )
								adj[i]->value=(gfloat)snd_ctl_elem_value_get_integer(val,i);
							else
								adj[i]->value=-(gfloat)snd_ctl_elem_value_get_integer(val,i);
							gtk_signal_emit_by_name(GTK_OBJECT(adj[i]),
													"value_changed");
						}
						break;
					case SND_CTL_ELEM_TYPE_ENUMERATED:
						for( i=0 ; i<count ; i++ )
							gtk_option_menu_set_history(GTK_OPTION_MENU(w[i]),
									snd_ctl_elem_value_get_enumerated(val,i));
						break;
					default:
						break;
					}
				}
				p=p->next;
			}
		}
	} else {
	}
}

int time_init(void) {
	if( snd_ctl_event_malloc(&ev)<0 ) return -1;
	if( snd_ctl_elem_id_malloc(&e_id)<0 ) return -1;
	return 0;
}

gint time_callback(gpointer data) {
	int i,n,err;
	unsigned int mask;

	for( i=0 ; i<card_last ; i++ ) {
		if( cards[i].handle == NULL ) continue;
#if 0
		err=read(cards[i].pfd.fd,ev,snd_ctl_event_sizeof());
		if( err< 0 ) {
			printf("hoe %d: %d\n",err,errno);
		}
		snd_ctl_event_elem_get_id(ev,e_id);
		printf("%d: type %d id '%s',%d",i,
			   snd_ctl_event_get_type(ev),
			   snd_ctl_elem_id_get_name(e_id),
			   snd_ctl_elem_id_get_index(e_id));
#endif
		while((n=poll(&cards[i].pfd,1,0))> 0 ) {
			if( (err=snd_ctl_read(cards[i].handle,ev))<0 ) {
				printf("hoe %d: %s\n",err,snd_strerror(err));
				continue;
			}
			snd_ctl_event_elem_get_id(ev,e_id);
			mask=snd_ctl_event_elem_get_mask(ev);
#ifdef DISP_DEBUG
			printf("%d: type %s mask %02x id '%s',%d\n",i,
				   snd_ctl_event_type_name(snd_ctl_event_get_type(ev)),
				   mask,
				   snd_ctl_elem_id_get_name(e_id),
				   snd_ctl_elem_id_get_index(e_id));
#endif
			if( (mask&SND_CTL_EVENT_MASK_INFO) ) cb_info(cards+i,e_id);
			if( (mask&SND_CTL_EVENT_MASK_VALUE) ) cb_value(cards+i,e_id);
		}
	}
	
	return 1;
}

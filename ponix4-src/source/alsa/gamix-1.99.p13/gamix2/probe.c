
#include "gamix.h"

int card_last,mdev_num;
s_card_t *cards;

typedef struct n_tree {
	char *name;
	int index;
	int len;
	int mlen;
	s_ctl_t *ctl;
	struct n_tree *a_next;
	struct n_tree *b_next;
} n_tree_t;
static n_tree_t *name_tree=NULL;
#define sct_last 5
static int sct_n[sct_last];

gint probe_mixer( void ) {
	int err,i,j,k;
	snd_ctl_t *chandle;
	snd_hctl_t *handle;
	snd_ctl_elem_list_t *clist;
	snd_hctl_elem_t *elem;
	snd_ctl_elem_id_t *e_id=NULL;
	snd_ctl_elem_info_t *einfo;
	snd_ctl_elem_value_t *eval;
	s_card_t *c;
	s_ctl_t *ctl;
	gchar *cstr;

	card_last=-1;
	for(i=0 ; i<32 ; i++ ) {
		if( snd_card_load(i) == 0 ) {
			card_last=i;
		}
	}
	if( card_last < 0 ) {
		pr_err(_("No ALSA device.\n"));
		return -1;
	}
	card_last++;
	cards=(s_card_t *)g_malloc(sizeof(s_card_t)*card_last);
	if( cards == NULL ) {
		pr_err(nomem_msg);
		return -1;
	}
	memset(cards,0,sizeof(s_card_t)*card_last);
	mdev_num=0;
	for( i = 0 ; i<card_last ; i++ ) {
		//pr_mtime();
		c=cards+i;
		cstr=g_strdup_printf("hw:%d",i);
#ifdef SNDCTLOPEN2
		if((err=snd_ctl_open(&chandle,cstr))<0 )
#else
		if((err=snd_ctl_open(&chandle,cstr,0))<0 )
#endif
		{
#if 0
			sprintf(err_msg,_("open failed: %s\n"),snd_strerror(err));
			pr_err(err_msg);
			return -1;
#endif
			continue;
		}
		if( (err=snd_ctl_card_info_malloc(&c->info)) < 0 ) {
			pr_err(nomem_msg);
			snd_ctl_close(chandle);
			return -1;
		}
		memset(c->info,0,snd_ctl_card_info_sizeof());
		if( (err=snd_ctl_card_info(chandle, c->info)) <0 ) {
			sprintf(err_msg,_("hw info failed: %s\n"),snd_strerror(err));
			pr_err(err_msg);
			snd_ctl_close(chandle);
			return -1;
		}
		if( (err=snd_ctl_elem_list_malloc(&clist))<0 ) {
			pr_err(nomem_msg);
			snd_ctl_close(chandle);
			return -1;
		}
		memset(clist,0,snd_ctl_elem_list_sizeof());
		if( (err=snd_ctl_elem_list(chandle,clist))<0 ) {
			sprintf(err_msg,_("ctl_elem list get failed: %s\n"),snd_strerror(err));
			pr_err(err_msg);
			snd_ctl_close(chandle);
			continue;
		}

		snd_ctl_close(chandle);
#ifdef DISP_DEBUG
		printf("Card %s/%s\n",
			   snd_ctl_card_info_get_id(c->info),
			   snd_ctl_card_info_get_longname(c->info));
#if 0
		printf("   mix %s/%s\n",
			   snd_ctl_card_info_get_mixerid(c->info),
			   snd_ctl_card_info_get_mixername(c->info));
#endif
		printf("   mix %s\n",snd_ctl_card_info_get_mixername(c->info));
#endif
		c->enable=TRUE;
		c->enabled=FALSE;
		c->handle=NULL;
		c->objs=NULL;

		if( (err=snd_hctl_open(&handle,cstr,0))< 0 ) {
#if 0
			sprintf(err_msg,_("hctl open failed: %s\n"),snd_strerror(err));
			pr_err(err_msg);
			return -1;
#endif
			continue;
		}
		if( (err=snd_hctl_load(handle))<0 ) {
			sprintf(err_msg,_("hctl load failed: %s\n"),snd_strerror(err));
			pr_err(err_msg);
			return -1;
		}
		mdev_num++;

		c->ctls_num=snd_ctl_elem_list_get_count(clist);
		snd_ctl_elem_list_free(clist);

		c->controls=(s_ctl_t *)malloc(c->ctls_num*sizeof(s_ctl_t));
		if( c->controls == NULL ) {
			pr_err(nomem_msg);
			snd_hctl_close(handle);
			return -1;
		}
		memset(c->controls,0,c->ctls_num*sizeof(s_ctl_t));
		c->ctl_used=(gboolean *)malloc(c->ctls_num*sizeof(gboolean));
		if( c->ctl_used == NULL ) {
			pr_err(nomem_msg);
			snd_hctl_close(handle);
			return -1;
		}
#ifdef DISP_DEBUG
		printf("clist %d\n",c->ctls_num);
#endif
		elem=snd_hctl_first_elem(handle);
		for( j=0 ; j<c->ctls_num ; j++ ) {
			if( (err=snd_ctl_elem_id_malloc(&e_id))<0 ) {
				pr_err(nomem_msg);
				snd_hctl_close(handle);
				return -1;
			}
			snd_hctl_elem_get_id(elem,e_id);
#ifdef DISP_DEBUG
			printf("#%d '%s'%d\n",
				   snd_ctl_elem_id_get_numid(e_id),
				   snd_ctl_elem_id_get_name(e_id),
				   snd_ctl_elem_id_get_index(e_id));
#endif
			ctl=c->controls+j;
			if( (err=snd_ctl_elem_info_malloc(&einfo))<0 ) {
				pr_err(nomem_msg);
				snd_hctl_close(handle);
				return -1;
			}
			memset(einfo,0,snd_ctl_elem_info_sizeof());
			snd_ctl_elem_info_set_id(einfo,e_id);
			if( (err=snd_hctl_elem_info(elem,einfo)) < 0 ) {
				sprintf(err_msg,"control info read failed: %s\n",snd_strerror(err));
				pr_err(err_msg);
			}
			if( snd_ctl_elem_info_get_type(einfo) == 
				SND_CTL_ELEM_TYPE_ENUMERATED ) {
				ctl->inames = malloc(64*snd_ctl_elem_info_get_items(einfo));
				if( ctl->inames == NULL ) {
					pr_err(nomem_msg);
					snd_hctl_close(handle);
					return -1;
				}
				for( k=0 ; k<snd_ctl_elem_info_get_items(einfo) ;
					 k++ ) {
					snd_ctl_elem_info_set_item(einfo,k);
					if( (err=snd_hctl_elem_info(elem,einfo))<0 ) {
						sprintf(err_msg,"control info read failed: %s\n",snd_strerror(err));
						ctl->inames[k*64]=0;
					} else 
						memcpy(ctl->inames+k*64,
							   snd_ctl_elem_info_get_item_name(einfo),64);
				}
			}
			if( (err=snd_ctl_elem_value_malloc(&eval))<0 ) {
				pr_err(nomem_msg);
				snd_hctl_close(handle);
				return -1;
			}
			memset(eval,0,snd_ctl_elem_value_sizeof());
			snd_ctl_elem_value_set_id(eval,e_id);
			if( is_access(einfo) ) 
				if( (err=snd_hctl_elem_read(elem,eval))<0 ) {
					sprintf(err_msg,"control read failed: %s\n",snd_strerror(err));
					pr_err(err_msg);
				}
			ctl->id=e_id;
			ctl->ctl=eval;
			ctl->info=einfo;
			ctl->el=NULL;
			c->ctl_used[j]=FALSE;
			elem=snd_hctl_elem_next(elem);
		}
		snd_hctl_close(handle);
		ctl=c->controls;
		for( j=1 ; j<=c->ctls_num ; j++ ) {
			if( snd_ctl_elem_id_get_numid(ctl->id) != j ) {
				for( k=j; k<c->ctls_num ; k++ ) {
					if( snd_ctl_elem_id_get_numid(c->controls[k].id) == j )
						break;
				}
				if( k<c->ctls_num ) {
					s_ctl_t cc;
					cc=*ctl;
					*ctl=c->controls[k];
					c->controls[k]=cc;
				}
			}
			ctl++;
		}
#ifdef DISP_DEBUG
		printf("sorted\n");
		ctl=c->controls;
		for( j=0 ; j<c->ctls_num ; j++ ) {
			printf("#%d '%s'%d\n",
			   snd_ctl_elem_id_get_numid(ctl->id),
			   snd_ctl_elem_id_get_name(ctl->id),
			   snd_ctl_elem_id_get_index(ctl->id));
			
			ctl++;
		}
#endif

	}
	return 0;
}

static void free_node(n_tree_t *t) {
	if( t->a_next ) free_node(t->a_next);
	if( t->b_next ) free_node(t->b_next);
	free(t->name);
	free(t);
}
static void free_ntree(void) {
	if( name_tree ) free_node(name_tree);
	name_tree=NULL;
}
static void pr_node(n_tree_t *p,int n) {
	int i=n;
	while( (i--)>0 ) printf("    ");
	if( p->ctl ) printf("*");
	if( p->index )
		printf("'%s',%d\n",p->name+p->mlen,p->index);
	else
		printf("'%s'\n",p->name+p->mlen);
	if( p->a_next ) pr_node(p->a_next,n+1);
	if( p->b_next ) pr_node(p->b_next,n);
}

static int cs_node(n_tree_t *n,n_tree_t *pre,s_ctl_t *ctl) {
	n_tree_t *q;
	int n1,n2;
	const char *cname=snd_ctl_elem_id_get_name(ctl->id);

	n1=n2=n->mlen;
	while( n->name[n1] !=0 && cname[n1]!=0 &&
		   n->name[n1] == cname[n1] ) {
		if( n->name[n1]==' ' ) n2=n1;
		n1++;
	}
	if( n->name[n2]==' ' ) n2++;
#if 0
	{
		char s[256];
		strncpy(s,n->name,n2);
		s[n2]=0;
		printf("i1 %d n1 %d n2 %d nlen %d\n",n->mlen,n1,n2,n->len);
		printf("match '%s' diff '%s' '%s'\n",s,n->name+n2,cname+n2);
	}
#endif
	if( n->len == n2 ) return cs_node(n->a_next,n,ctl);
	if( n2 > n->mlen ) {
		q=(n_tree_t *)malloc(sizeof(n_tree_t));
		if( q==NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		q->name=malloc(n2+1);
		if( q->name== NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		memcpy(q->name,n->name,n2);
		q->name[n2]=0;
		q->len=n2;
		q->mlen=n->mlen;
		q->index=n->index;
		q->ctl=NULL;
		q->a_next=n;
		q->b_next=n->b_next;
		n->b_next=NULL;
		n->mlen=n2;
		if( pre ) {
			if( pre->a_next == n )
				pre->a_next=q;
			else
				pre->b_next=q;
		} else {
			name_tree=q;
		}
		return cs_node(n,q,ctl);
	}
	q=(n_tree_t *)malloc(sizeof(n_tree_t));
	if( q==NULL ) {
		pr_err(nomem_msg);
		return -1;
	}
	q->name=strdup(cname);
	if( q->name== NULL ) {
		pr_err(nomem_msg);
		return -1;
	}
	q->len=strlen(q->name);
	q->mlen=n->mlen;
	q->index=n->index;
	q->ctl=ctl;
	q->a_next=NULL;
	q->b_next=n;
	n->mlen=n2;
	if( pre ) {
		if( pre->a_next == n )
			pre->a_next=q;
		else
			pre->b_next=q;
	} else {
		name_tree=q;
	}
	return 0;
}

static void chk_name(s_ctl_t *ctl) {
	n_tree_t *p=name_tree,*r=NULL;
	int i1,i2;
	snd_ctl_elem_id_t *id=ctl->id;
	const char *cname=snd_ctl_elem_id_get_name(id);

#ifdef DISP_DEBUG
	printf("'%s',%d\n",cname,snd_ctl_elem_id_get_index(id));
#endif
	if( name_tree==NULL ) {
		name_tree=(n_tree_t *)malloc(sizeof(n_tree_t));
		name_tree->name=strdup(snd_ctl_elem_id_get_name(id));
		name_tree->len=strlen(name_tree->name);
		name_tree->mlen=0;
		name_tree->index=snd_ctl_elem_id_get_index(id);
		name_tree->ctl=ctl;
		name_tree->a_next=NULL;
		name_tree->b_next=NULL;
	} else {
		while( p ) {
			if( p->index == snd_ctl_elem_id_get_index(id) ) {
				i1=0;
				while(p->name[i1]!=0 && p->name[i1]!=' ' ) i1++;
				i2=0;
				while( cname[i2]!=0 && cname[i2]!=' ' ) i2++;
				if( i1==i2 && strncmp(p->name,cname,i1) == 0 ) {
					cs_node(p,r,ctl);
					return;
				}
			}
			r=p;
			p=p->b_next;
		}
		r->b_next=p=(n_tree_t *)malloc(sizeof(n_tree_t));
		p->name=strdup(cname);
		p->len=strlen(p->name);
		p->mlen=0;
		p->index=snd_ctl_elem_id_get_index(id);
		p->ctl=ctl;
		p->a_next=NULL;
		p->b_next=NULL;
	}
}

static int mk_elem(s_obj_t *obj,s_eos_t *es,s_ctl_t *ctl) {
	s_element_t *e;
	snd_ctl_elem_id_t *id=ctl->id;

	es->type = EO_ELEM;
	e=es->eo.e = (s_element_t *)malloc(sizeof(s_element_t));
	if( e == NULL ) {
		pr_err(nomem_msg);
		return -1;
	}
	memset(e,0,sizeof(s_element_t));
	e->ctl=ctl;
	e->chain_en=&obj->chain_en;
	e->chain=&obj->chain;
	e->enabled=&obj->enabled;
	if( snd_ctl_elem_info_get_type(ctl->info) ==
		SND_CTL_ELEM_TYPE_INTEGER ) {
		const char *s,*s2;
		s=s2=snd_ctl_elem_id_get_name(id);
		while( *s ) if( *(s++) == ' ' ) s2=s;
		s=snd_ctl_elem_id_get_name(id);
		if( strcmp(s2,"Volume") == 0 ) {
			e->atype=1;
		} else if( strncmp(s+4,"Pan",3) == 0 ) {
			e->atype=2;
		} else {
			e->atype=0;
		}
			
		//exit(0);
	}
	add_ep(ctl,es->eo.e);
	return 0;
}
static int mk_label(n_tree_t *obj,s_eos_t *es) {
	int i;
	char *s,*s1;
	s_ctl_t *ctl=obj->ctl;
	switch(snd_ctl_elem_info_get_type(ctl->info)) {
	case SND_CTL_ELEM_TYPE_BOOLEAN: i=sct_n[0]; break;
	case SND_CTL_ELEM_TYPE_INTEGER: i=sct_n[1]; break;
	case SND_CTL_ELEM_TYPE_ENUMERATED: i=sct_n[2]; break;
	case SND_CTL_ELEM_TYPE_BYTES: i=sct_n[3]; break;
	default:
		i=0;
	}
	if( i<2 ) return 0;
	s=strdup(snd_ctl_elem_id_get_name(ctl->id)+obj->mlen);
	//printf("label %s %s\n",s,ctl->ctl.id.name);
	s1=s;
	while( *s1 && *s1 !=' ' ) s1++;
	if( *s1 == ' ' ) *s1=0;
	s1=strdup(_(s));
	free(s);
	es->type=EO_LABEL;
	es->eo.l=s1;
	return 1;
}
static void sct_add(snd_ctl_elem_type_t type) {
	switch(type) {
	case SND_CTL_ELEM_TYPE_BOOLEAN: sct_n[0]++; break;
	case SND_CTL_ELEM_TYPE_INTEGER: sct_n[1]++; break;
	case SND_CTL_ELEM_TYPE_ENUMERATED: sct_n[2]++; break;
	case SND_CTL_ELEM_TYPE_BYTES: sct_n[3]++; break;
	default: break;
	}
}
static int sct_plus(void) {
	int i=0,n=0;
	for(i=0 ; i<sct_last ; i++ )
		if( sct_n[i]>1 ) n+=sct_n[i];
	return n;
}
static int mk_obj(s_obj_t **objs,n_tree_t *n) {
	int i,j;
	n_tree_t *p;
	s_obj_t *obj;
	s_eos_t *es;

	if( n->a_next ) {
		obj=(s_obj_t *)malloc(sizeof(s_obj_t));
		if( obj==NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		memset(obj,0,sizeof(s_obj_t));
		*objs=obj;
		if( n->index > 0 && n->name[0] !=0 )
			sprintf(obj->name,"%s %d",n->name+n->mlen,n->index);
		else
			strcpy(obj->name,n->name+n->mlen);
		i=0;
		while( obj->name[i] ) i++;
		i--;
		if( obj->name[i] == ' ' ) obj->name[i]=0;
		//obj->index=n->index;
		obj->enable=TRUE;
		obj->enabled=FALSE;

		p=n->a_next;
		i=0;
		j=0;
		memset(sct_n,0,sizeof(sct_n));
		while( p != NULL ) {
			if( !p->ctl ) {
				j=1;
				break;
			}
			sct_add(snd_ctl_elem_info_get_type(p->ctl->info));
			i++;
			p=p->b_next;
		}
		if( j ) {
			p=n->a_next;
			i=0;
			j=1;
			while( p!= NULL ) {
				if( !p->ctl ) {
					i++;
					j=1;
				} else {
					if( j != 0 ) i++;
					j=0;
				}
				p=p->b_next;
			}
			//printf("'%s' %d\n",obj->name,i);
			obj->es_num=i;
			obj->es=(s_eos_t *)malloc(i*sizeof(s_eos_t));
			if( obj->es == NULL ) {
				pr_err(nomem_msg);
				return -1;
			}
			es=obj->es;
			p=n->a_next;
			while( p ) {
				es->type=EO_OBJ;
				if( p->ctl ) {
					n_tree_t *q;
					s_eos_t *es2;

					obj=(s_obj_t *)malloc(sizeof(s_obj_t));
					if( obj == NULL ) {
						pr_err(nomem_msg);
						return -1;
					}
					memset(obj,0,sizeof(s_obj_t));
					es->eo.o=obj;
					obj->enable=TRUE;
					q=p;
					i=0;
					memset(sct_n,0,sizeof(sct_n));
					while( q != NULL && q->ctl != NULL ) {
						sct_add(snd_ctl_elem_info_get_type(q->ctl->info));
						i++;
						q=q->b_next;
					}
					i+=sct_plus();
					obj->es_num=i;
					obj->es=(s_eos_t *)malloc(i*sizeof(s_eos_t));
					if( obj->es == NULL ) {
						pr_err(nomem_msg);
						return -1;
					}
					es2=obj->es;
					while( i>0 ) {
						if( mk_label(p,es2)>0 ) {
							es2++;
							i--;
						}
						if( mk_elem(obj,es2,p->ctl)<0 ) return -1;
						i--;
						p=p->b_next;
						es2++;
					}
				} else {
					if( mk_obj(&es->eo.o,p ) < 0 ) return -1;
					p=p->b_next;
				}
				es++;
			}
			return 0;
		}
		i+=sct_plus();
		obj->es_num=i;
		obj->es=(s_eos_t *)malloc(i*sizeof(s_eos_t));
		if( obj->es == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		es=obj->es;
		p=n->a_next;
		while( i>0 ) {
			if( mk_label(p,es)>0 ) {
				es++;
				i--;
			}
			if( mk_elem(obj,es,p->ctl)<0 ) return -1;
			p=p->b_next;
			es++;
			i--;
		}
		return 0;
	}
	if( n->ctl ) {
		obj=(s_obj_t *)malloc(sizeof(s_obj_t));
		if( obj==NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		memset(obj,0,sizeof(s_obj_t));
		if( snd_ctl_elem_id_get_index(n->ctl->id) > 0 ) 
			sprintf(obj->name,"%s %d",
					snd_ctl_elem_id_get_name(n->ctl->id),
					snd_ctl_elem_id_get_index(n->ctl->id));
		else
			strcpy(obj->name,snd_ctl_elem_id_get_name(n->ctl->id));
		obj->es_num=1;
		obj->es=(s_eos_t *)malloc(sizeof(s_eos_t));
		if( obj->es == NULL ) {
			pr_err(nomem_msg);
			return -1;
		}
		obj->enable=TRUE;
		*objs=obj;
		return mk_elem(obj,obj->es,n->ctl);
	} else
		printf("hoe\n");
	return 0;
}

void group_check(void) {
	int i,j,k;
	gboolean *p;
	s_ctl_t *r;
	n_tree_t *q;
	s_obj_t **objs;

	for( i=0 ; i<card_last ; i++ ) {
		k=0;
		p=cards[i].ctl_used;
		r=cards[i].controls;
		free_ntree();
		for( j=0 ; j<cards[i].ctls_num ; j++ ) {
			if( !p[j] ) {
				chk_name(r+j);
				k++;
			}
		}
		free(p);
		cards[i].ctl_used=NULL;
		if( k==0 ) continue;
#ifdef DISP_DEBUG
		pr_node(name_tree,0);
#endif
		q=name_tree;
		objs=&cards[i].objs;
		while( *objs != NULL ) objs=&((*objs)->next);
		while( q != NULL ) {
			mk_obj(objs,q);
			{
				char *s,*s1;
				s1=s=(*objs)->name;
				while( *s1 && *s1 != ' ' ) s1++;
				if( *s1==' ' ) s1++;
				while( *s1 ) {
					if( *s1 == '-' ) {
						*s1=0;*(s-1)=0;
					} else if( strncmp(s1,"Playback",8)==0 ) {
						*s1=0;*(s-1)=0;
					} else if( strncmp(s1,"Switch",6)==0 ) {
						*s1=0;*(s-1)=0;
					} else if( strncmp(s1,"Volume",6)==0 ) {
						*s1=0;*(s-1)=0;
					}
					while( *s1 && *s1 != ' ' ) s1++;
					if( *s1==' ' ) s1++;
				}
			}
			objs=&((*objs)->next);
			q=q->b_next;
		}
	}
}

int add_ep(s_ctl_t *ctl,s_element_t *e) {
	struct s_ep **ep=&ctl->el,*p;

	while( *ep != NULL ) ep=&(*ep)->next;
	p=(struct s_ep *)malloc(sizeof(struct s_ep));
	if( p == NULL ) {
		pr_err(nomem_msg);
		return -1;
	}
	p->e=e;
	p->next=NULL;
	*ep=p;
	return 0;
}

#ifdef HAVE_CONFIG_H
# include "../config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

#if GTK_CHECK_VERSION(1,3,0)
#define GTK_WINDOW_DIALOG GTK_WINDOW_TOPLEVEL
#define gtk_rc_init(a) //gtk_rc_init(a)
#endif

#ifdef GTK2
gchar *lconv(const char *);
#else
#define lconv(s) s
#endif

//#include <sys/asoundlib.h>
#include <alsa/asoundlib.h>

#ifdef ENABLE_NLS
# include <libintl.h>
# undef _
# undef N_
# define _(String) dgettext(PACKAGE,String)
# ifdef gettext_noop
#  define N_(String) gettext_noop(String)
# else
#  define N_(String) (String)
# endif
#else
# define textdomain(String) (String)
# define gettext(String) (String)
# define dgettext(Domain,Message) (Message)
# define dcgettext(Domain,Message,Type) (Message)
# define bindtextdomain(Domain,Directory) (Domain)
# define _(String) (String)
# define N_(String) (String)
#endif

#define pr_err(msg) fprintf(stderr,__FILE__ " %d: %s",__LINE__,msg)
#define preid(eid) printf("'%s',%d,%d\n",(eid).name,(eid).index,(eid).type)

#define ACCESS_RW (SND_CONTROL_ACCESS_READ|SND_CONTROL_ACCESS_WRITE)

typedef struct {
	struct s_ctl *ctl;
	int card;
	gboolean *chain_en;
	gboolean *chain;
	gboolean *enabled;
	GtkWidget *box;
	GtkWidget **w;
	GtkAdjustment **adj;
	struct s_cb *gp;
	int atype;
	char *sfmt;
} s_element_t;

typedef struct s_cb {
	int i;
	s_element_t *e;
} s_cb_t;

enum eo_type {
	EO_ELEM,
	EO_OBJ,
	EO_LABEL
};

typedef struct {
	int type;
	union {
		s_element_t *e;
		struct s_obj *o;
		char *l;
	} eo;
} s_eos_t;

typedef struct s_obj s_obj_t;
struct s_obj {
	char name[44];
	//int index;
	gboolean enable;
	gboolean enabled;
	gboolean chain_en;
	gboolean chain;
	int es_num;
	s_eos_t *es;
	GtkWidget *w;
	struct s_obj *next;
};

struct s_ep {
	s_element_t *e;
	struct s_ep *next;
};

typedef struct s_ctl {
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_value_t *ctl;
	snd_ctl_elem_info_t *info;
	char *inames;
	struct s_ep *el;
} s_ctl_t;

typedef struct {
	s_obj_t *objs;
	int ctls_num;
	s_ctl_t *controls;
	gboolean *ctl_used;
	gboolean enable;
	gboolean enabled;
	gboolean p_e,p_f;
	GtkWidget *w;
	snd_ctl_t *handle;
	snd_ctl_card_info_t *info;
	struct pollfd pfd;
} s_card_t;

typedef struct {
	gint wmode;
	gboolean scroll;
	gboolean i_dyn;
	gchar *fna;
	gboolean F_save;
	gboolean Esave;
	gboolean sv_wsize;
	gint g_l;
	gint width;
	gint height;
	gint aset_vol1;
} s_conf;

extern GtkWidget *window;
extern GtkStyle *dstyle;
extern int card_last,mdev_num;
extern gint card,mdev;
extern s_card_t *cards;
extern s_conf conf;
extern unsigned char *nomem_msg,err_msg[];
extern int scfont_w;

#ifdef DISP_DEBUG
extern struct timeval tv1,tv2;
static inline void pr_mt(char *fl,char *fn,int line) {
	long tm;
	gettimeofday(&tv2,NULL);
	tm=(1000L*tv2.tv_sec+tv2.tv_usec/1000L)-(1000L*tv1.tv_sec+tv1.tv_usec/1000L);
	tv1=tv2;
	fprintf(stderr,"%s %s %d: %ldms\n",fl,fn,line,tm);
}
#define pr_mtime() pr_mt(__FILE__,__FUNCTION__,__LINE__)
#else
#define pr_mtime() /* __LINE__ */
#endif

/* probe.c */
gint probe_mixer( void );
void group_check( void );
int add_ep(s_ctl_t *,s_element_t *);

/* mkmixer.c */
GtkWidget *make_mixer( gint );
int is_access(snd_ctl_elem_info_t *);

/* catch.c */
gint time_callback(gpointer);
int time_init(void);

/* conf_w.c */
gint conf_win( void );
void conf_read( void );
void conf_write( void );
int dwin_01(char *,char *,char *);

/* main.c */
int disp_mixer( void );

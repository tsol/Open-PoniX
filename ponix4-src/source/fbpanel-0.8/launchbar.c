
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>


#include <gdk-pixbuf/gdk-pixbuf.h>

#include "fbpanel.h"

typedef struct btn {
    GtkWidget *button, *pixmap;
    gchar *action;
}
btn;

#define MAXBUTTONS 20
typedef struct launchbar {
    GtkWidget *box;
    GtkTooltips *tips;
    btn btns[MAXBUTTONS];
    int btn_num;
}
launchbar;


#define CFGFILE ".fbpanel/launchbar"
#define ICONSIZE 20
launchbar lb;

#define ERRSTR "launchbar:config file error:"

void read_button(FILE *fp);

void sigchld(int num)
{
    int status;

    /* collect all dead children */
    do {
	if ((wait(&status) == -1) && (errno != EINTR))
	    return;
    } while(1);
    
}

static void
clicked( GtkWidget *widget, gpointer data )
{
    int pid, no = (int) data;

    pid = fork();
    if (pid == -1)
	return;
    if (pid == 0) {
	char *argv[4];
	argv[0] = "sh";
	argv[1] = "-c";
	argv[2] = lb.btns[no].action;
	argv[3] = 0;
	execv("/bin/sh", argv);
	exit(127);
    }
}



static void
read_file(FILE *fp)
{
    gchar str[512];
    
    while (myfgets(str, 512, fp)) {
	if (strcmp(str, "[")) {
	    g_warning(ERRSTR "expected '[' but got %s\n", str);
	    return;
	}
	read_button(fp);

	/*
	if (strcmp(str, "]")) {
	    g_warning(ERRSTR "expected ']' but got %s\n", str);
	    return;
	    }
	*/
	if (lb.btn_num >= MAXBUTTONS) {
	    g_print(ERRSTR "max number of buttons (%d) was reached. skipping the rest\n",
		lb.btn_num );
	    return;
	}
    }
}

void read_button(FILE *fp)
{
    gchar str[512];
    gchar fname[512];
    gchar tooltip[512];
    gchar action[512];
    gchar *token;
    GdkPixbuf *gp, *gps;
    GdkPixmap *pix;
    GdkBitmap *mask;
    GtkWidget *pixmap, *button;
  

    token = "Image::";
    if (!myfgets(str, 512, fp) || strncmp(str, token, strlen(token))) {
	g_warning(ERRSTR "can't read 'Image::<filename>'\n");
	return;
    }
    strcpy(fname, str + strlen(token));

    token = "Tooltip::";
    if (!myfgets(str, 512, fp) || strncmp(str, token, strlen(token))) {
	g_warning(ERRSTR "can't read 'Tooltip::<string>'\n");
	return;
    }
    strcpy(tooltip, str + strlen(token));

    token = "Action::";
    if (!myfgets(str, 512, fp) || strncmp(str, token, strlen(token))) {
	g_warning(ERRSTR "can't read 'Action::<shell command>'\n");
	return;
    }
    strcpy(action, str + strlen(token));

    myfgets(str, 512, fp);
    
    gp = gdk_pixbuf_new_from_file(fname);
    if (!gp) {
	g_warning(ERRSTR "can't read image %s\n", fname);
	button = gtk_button_new_with_label("?");
    } else {
	gps =  gdk_pixbuf_scale_simple (gp, ICONSIZE, ICONSIZE, GDK_INTERP_TILES);
	gdk_pixbuf_render_pixmap_and_mask(gps, &pix, &mask, 127);
	gdk_pixbuf_unref(gp);
	gdk_pixbuf_unref(gps);
	button = gtk_button_new();
    }
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button), 0);
    
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
	GTK_SIGNAL_FUNC (clicked), (gpointer) lb.btn_num);
    gtk_widget_show(button);
    gtk_tooltips_set_tip(GTK_TOOLTIPS (lb.tips), button,
	tooltip, tooltip);
    if (gp) {
	pixmap = gtk_pixmap_new(pix, mask);
	lb.btns[lb.btn_num].pixmap = pixmap;
	gtk_widget_show(pixmap);
	gtk_container_add(GTK_CONTAINER(button), pixmap);
    }
    gtk_box_pack_start(GTK_BOX(lb.box), button, FALSE, FALSE, 0);
    lb.btns[lb.btn_num].button = button; 
    lb.btns[lb.btn_num].action = g_strdup(action);
    lb.btn_num++;
}



GtkWidget *
lb_init(gchar *cfgfile)
{
    FILE *fp;
    gchar *f = NULL;
    GtkWidget *frame;

    lb.box = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (lb.box), 0);
    gtk_widget_show(lb.box);
    lb.tips = gtk_tooltips_new();
    
    if (cfgfile)
	f = g_strdup(cfgfile);
    else
	f = g_strdup_printf("%s/%s", getenv("HOME"),  CFGFILE);

    if ((fp = fopen(f, "r")) != NULL)
	read_file(fp);
    g_free(f);

    signal(SIGCHLD, sigchld);
    return  lb.box;
}

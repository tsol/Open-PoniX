

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <locale.h>
#include <string.h>
#include <signal.h>

#include "plugin.h"
#include "panel.h"
#include "misc.h"

static gchar *cfgfile = NULL;
static gchar version[] = "2.2";
static gchar *cprofile = "default";


//#define DEBUG
#include "dbg.h"




static FILE *pfp; /* current profile FP */
static panel *p;

/****************************************************
 *         panel's handlers for WM events           *
 ****************************************************/
static void
print_wmdata(panel *p)
{
    int i;

    ENTER;
    DBG("desktop %d/%d\n", p->curdesk, p->desknum);
    DBG("workarea\n");
    for (i = 0; i < p->wa_len/4; i++)
        DBG("(%d, %d) x (%d, %d)\n",
              p->workarea[4*i + 0],
              p->workarea[4*i + 1],
              p->workarea[4*i + 2],
              p->workarea[4*i + 3]);
    RET();
}


static GdkFilterReturn
panel_wm_events(GdkXEvent *xevent, GdkEvent *event, gpointer *data)
{
    Atom at;
    Window win;
    XEvent *ev = (XEvent *) xevent;
    panel *p = (panel *) data;

    ENTER;
    DBG("panel_wm_events: win = 0x%x\n", ev->xproperty.window);
    if (ev->type != PropertyNotify )
        RET(GDK_FILTER_CONTINUE);
    
    at = ev->xproperty.atom;
    win = ev->xproperty.window;
    if (win == GDK_ROOT_WINDOW()) {
	if (at == a_NET_CLIENT_LIST) {
            DBG("A_NET_CLIENT_LIST\n");
	} else if (at == a_NET_CURRENT_DESKTOP) {
            p->curdesk = get_net_current_desktop();
            DBG("A_NET_CURRENT_DESKTOP\n");
	} else if (at == a_NET_NUMBER_OF_DESKTOPS) {
            p->desknum = get_net_number_of_desktops();
            DBG("A_NET_NUMBER_OF_DESKTOPS\n");
	} else if (at == a_NET_ACTIVE_WINDOW) {
            DBG("A_NET_ACTIVE_WINDOW\n");
	} else if (at == a_NET_WORKAREA) {
            DBG("A_NET_WORKAREA\n");
            p->workarea = get_xaproperty (GDK_ROOT_WINDOW(), a_NET_WORKAREA, XA_CARDINAL, &p->wa_len);
        }
    } 
    RET(GDK_FILTER_CONTINUE);
}

/****************************************************
 *         panel's handlers for GTK events          *
 ****************************************************/

  
static gint
panel_delete_event(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    ENTER;
    RET(FALSE);
}

static gint
panel_destroy_event(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    //panel *p = (panel *) data;

    ENTER;  
    //if (!p->self_destroy)
    gtk_main_quit();
    RET(FALSE);
}



static gint
panel_size_req(GtkWidget *widget, GtkRequisition *req, panel *p)
{
    ENTER;
    DBG("panel_size_req: IN req=(%d, %d)\n", req->width, req->height);
    if (p->widthtype == WIDTH_REQUEST)
        p->width = (p->orientation == ORIENT_HORIZ) ? req->width : req->height;
    if (p->heighttype == HEIGHT_REQUEST)
        p->height = (p->orientation == ORIENT_HORIZ) ? req->height : req->width;
    calculate_position(p);
    req->width  = p->aw;
    req->height = p->ah;
    DBG("panel_size_req: OUT req=(%d, %d)\n", req->width, req->height);
    RET( TRUE );
}

static gint
panel_size_alloc(GtkWidget *widget, GtkAllocation *allocation, panel *p)
{
    ENTER;
    DBG("panel_size_alloc: alloc=(%d, %d)\n", allocation->width, allocation->height);
    if (p->widthtype == WIDTH_REQUEST)
        p->width = (p->orientation == ORIENT_HORIZ) ? allocation->width : allocation->height;
    if (p->heighttype == HEIGHT_REQUEST)
        p->height = (p->orientation == ORIENT_HORIZ) ? allocation->height : allocation->width;
    calculate_position(p);
    gtk_widget_set_uposition(p->topgwin, p->ax, p->ay);
    RET(TRUE);
}




/****************************************************
 *         panel creation                           *
 ****************************************************/


void
panel_start_gui(panel *p)
{
    Atom state[3];
    XWMHints wmhints;
    unsigned int val;
    
    ENTER;
    p->topgwin =  gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(p->topgwin), 1);
    gtk_window_set_policy(GTK_WINDOW(p->topgwin), FALSE, FALSE, FALSE);
    gtk_window_set_wmclass(GTK_WINDOW(p->topgwin), "panel", "fbpanel");
    gtk_window_set_title(GTK_WINDOW(p->topgwin), "panel");
    gtk_signal_connect(GTK_OBJECT(p->topgwin), "delete-event",
          GTK_SIGNAL_FUNC(panel_delete_event), p);
    gtk_signal_connect(GTK_OBJECT(p->topgwin), "destroy-event",
          GTK_SIGNAL_FUNC(panel_destroy_event), p);
    gtk_signal_connect (GTK_OBJECT (p->topgwin), "size-request",
          (GtkSignalFunc) panel_size_req, p);
    gtk_signal_connect (GTK_OBJECT (p->topgwin), "size-allocate",
          (GtkSignalFunc) panel_size_alloc, p);

    gtk_widget_realize(p->topgwin);
    gdk_window_set_decorations(p->topgwin->window, 0);
    /* OLD */

    p->box = p->my_box_new(FALSE, 1);

    /* NEW */
    /*
    p->box = gtk_table_new(1, 1, FALSE);
    */
    gtk_container_add(GTK_CONTAINER(p->topgwin), p->box);

    p->topxwin = GDK_WINDOW_XWINDOW(GTK_WIDGET(p->topgwin)->window);

    /* make our window unfocusable */
    wmhints.flags = InputHint;
    wmhints.input = 0;
    XSetWMHints (GDK_DISPLAY(), p->topxwin, &wmhints); 
    if (p->setdocktype) {
        state[0] = a_NET_WM_WINDOW_TYPE_DOCK;
        XChangeProperty(GDK_DISPLAY(), p->topxwin, a_NET_WM_WINDOW_TYPE, XA_ATOM,
              32, PropModeReplace, (unsigned char *) state, 1);
    }

    
    
#define WIN_HINTS_SKIP_FOCUS      (1<<0)	/* "alt-tab" skips this win */
    val = WIN_HINTS_SKIP_FOCUS;
    XChangeProperty(GDK_DISPLAY(), p->topxwin,
          XInternAtom(GDK_DISPLAY(), "_WIN_HINTS", False), XA_CARDINAL, 32,
          PropModeReplace, (unsigned char *) &val, 1);

    Xclimsg(p->topxwin, a_NET_WM_DESKTOP, 0xFFFFFFFF, 0, 0, 0, 0);

    /************************/
    /* Window Mapping Point */
    gtk_widget_show_all(p->topgwin);
    Xclimsg(p->topxwin, a_NET_WM_DESKTOP, 0xFFFFFFFF, 0, 0, 0, 0);
    
    state[0] = a_NET_WM_STATE_SKIP_PAGER;
    state[1] = a_NET_WM_STATE_SKIP_TASKBAR;
    state[2] = a_NET_WM_STATE_STICKY;
    XChangeProperty(GDK_DISPLAY(), p->topxwin, a_NET_WM_STATE, XA_ATOM,
          32, PropModeReplace, (unsigned char *) state, 3);

 
  
    XSelectInput (GDK_DISPLAY(), GDK_ROOT_WINDOW(), PropertyChangeMask);
    /*
      XSelectInput (GDK_DISPLAY(), topxwin, PropertyChangeMask | FocusChangeMask |
      StructureNotifyMask);
    */
    gdk_window_add_filter(GDK_ROOT_PARENT(), (GdkFilterFunc)panel_wm_events, p);

    calculate_position(p);
    gdk_window_move_resize(p->topgwin->window, p->ax, p->ay, p->aw, p->ah);
    RET();
}

static int
panel_parse_global(panel *p, FILE *fp)
{
    line s;
    s.len = 256;
    
    ENTER;
    while (get_line(fp, &s) != LINE_NONE) {
        if (s.type == LINE_VAR) {
            if (!g_strcasecmp(s.t[0], "edge")) {
                p->edge = str2num(edge_pair, s.t[1], EDGE_NONE);
            } else if (!g_strcasecmp(s.t[0], "allign")) {
                p->allign = str2num(allign_pair, s.t[1], ALLIGN_NONE);
            } else if (!g_strcasecmp(s.t[0], "margin")) {
                p->margin = atoi(s.t[1]);
            } else if (!g_strcasecmp(s.t[0], "widthtype")) {
                p->widthtype = str2num(width_pair, s.t[1], WIDTH_NONE);
            } else if (!g_strcasecmp(s.t[0], "width")) {
                p->width = atoi(s.t[1]);
            } else if (!g_strcasecmp(s.t[0], "heighttype")) {
                p->heighttype = str2num(height_pair, s.t[1], HEIGHT_NONE);
            } else if (!g_strcasecmp(s.t[0], "height")) {
                p->height = atoi(s.t[1]);
            } else if (!g_strcasecmp(s.t[0], "SetDockType")) {
                p->setdocktype = str2num(bool_pair, s.t[1], 0);
            } else {
                ERR( "fbpanel: %s - unknown var in Global section\n", s.t[0]);
                RET(0);
            }
        } else if (s.type == LINE_BLOCK_END) {
            break;
        } else {
            ERR( "fbpanel: illegal in this context %s\n", s.str);
            RET(0);
        }
    }
    p->orientation = (p->edge == EDGE_TOP || p->edge == EDGE_BOTTOM)
        ? ORIENT_HORIZ : ORIENT_VERT;
    if (p->orientation == ORIENT_HORIZ) {
        p->my_box_new = gtk_hbox_new;
        p->my_separator_new = gtk_vseparator_new;
    } else {
        p->my_box_new = gtk_vbox_new;
        p->my_separator_new = gtk_hseparator_new;
    }
    if (p->width < 0)
        p->width = 100;
    if (p->widthtype == WIDTH_PERCENT && p->width > 100)
        p->width = 100;
    p->heighttype = HEIGHT_PIXEL;
    if (p->heighttype == HEIGHT_PIXEL) {
        if (p->height < PANEL_HEIGHT_MIN)
            p->height = PANEL_HEIGHT_MIN;
        else if (p->height > PANEL_HEIGHT_MAX)
            p->height = PANEL_HEIGHT_MAX;
    }
    p->curdesk = get_net_current_desktop();
    p->desknum = get_net_number_of_desktops();
    p->workarea = get_xaproperty (GDK_ROOT_WINDOW(), a_NET_WORKAREA, XA_CARDINAL, &p->wa_len);
    panel_start_gui(p);
    RET(1);
}

static int
panel_parse_plugin(panel *p, FILE *fp)
{
    line s;
    plugin *plug = NULL;
    gchar *type = NULL;
    FILE *tmpfp;
    int expand , padding;
    
    ENTER;
    s.len = 256;
    if (!(tmpfp = tmpfile())) {
        ERR( "can't open temporary file with tmpfile()\n");
        RET(0);
    }
    expand = padding = 0;
    while (get_line(fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_NONE)
            goto error;

        if (s.type == LINE_VAR) {
            if (!g_strcasecmp(s.t[0], "type"))
                type = g_strdup(s.t[1]);
            else if (!g_strcasecmp(s.t[0], "expand"))
                expand = str2num(bool_pair,  s.t[1], 0);
            else if (!g_strcasecmp(s.t[0], "padding"))
                padding = atoi(s.t[1]);
            else {
                ERR( "fbpanel: unknown var %s\n", s.t[0]);
                goto error;
            }
        } else if (s.type == LINE_BLOCK_START) {
            if (!g_strcasecmp(s.t[0], "Config")) {
                int pno = 1;
                while (pno) {
                    get_line_as_is(fp, &s);
                    if (s.type == LINE_NONE) {
                        ERR( "fbpanel: unexpected eof\n");
                        goto error;
                    } else if (s.type == LINE_BLOCK_START) {
                        pno++;
                    } else if (s.type == LINE_BLOCK_END) {
                        pno--;
                    } 
                    fprintf(tmpfp, "%s\n", s.str);
                }              
            } else {
                ERR( "fbpanel: unknown block %s\n", s.t[0]);
                goto error;
            }
        } else {
            ERR( "fbpanel: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    
    if (!type || !(plug = plugin_load(type))) {
        ERR( "fbpanel: can't load %s plugin\n", type);
        goto error;
    }
    plug->panel = p;
    plug->fp = tmpfp;
    plug->expand = expand;
    plug->padding = padding;
    fprintf(tmpfp, "}\n");
    fseek(tmpfp, 0, SEEK_SET);
    if (!plugin_start(plug)) {
        ERR( "fbpanel: can't start plugin %s\n", type);
        goto error;
    }
    p->plugins = g_list_append(p->plugins, plug);
    RET(1);
    
 error:
    fclose(tmpfp);
    g_free(type);
    if (plug)
          plugin_put(plug);
    RET(0);
    
}


int
panel_start(panel *p, FILE *fp)
{
    line s;

    /* parse global section */
    ENTER;
    s.len = 256;
    memset(p, 0, sizeof(panel));
    p->allign = ALLIGN_CENTER;
    p->edge = EDGE_BOTTOM;
    p->widthtype = WIDTH_PERCENT;
    p->width = 100;
    p->heighttype = HEIGHT_PIXEL;
    p->height = PANEL_HEIGHT_DEFAULT;
    p->setdocktype = 1;
    if ((get_line(fp, &s) != LINE_BLOCK_START) || g_strcasecmp(s.t[0], "Global")) {
        ERR( "fbpanel: config file must start from Global section\n");
        RET(0);
    }
    if (!panel_parse_global(p, fp))
        RET(0);


 
    while (get_line(fp, &s) != LINE_NONE) {
        if ((s.type  != LINE_BLOCK_START) || g_strcasecmp(s.t[0], "Plugin")) {
            ERR( "fbpanel: expecting Plugin section\n");
            RET(0);
        }
        if (!panel_parse_plugin(p, fp)) 
            RET(0);
    }
    gtk_widget_show_all(p->topgwin);
    print_wmdata(p);
    RET(1);
}

static void
delete_plugin(gpointer data, gpointer udata)
{
    ENTER;
    plugin_stop((plugin *)data);
    plugin_put((plugin *)data);
    RET();
    
}

void panel_stop(panel *p)
{
    ENTER;

    g_list_foreach(p->plugins, delete_plugin, NULL);
    g_list_free(p->plugins);
    p->plugins = NULL;
    XSelectInput (GDK_DISPLAY(), GDK_ROOT_WINDOW(), NoEventMask);
    gdk_window_remove_filter(GDK_ROOT_PARENT(), (GdkFilterFunc)panel_wm_events, p);
    gtk_widget_destroy(p->topgwin);
    g_free(p->workarea);
    RET();
}


void
usage()
{
    ENTER;
    printf("fbpanel %s - lightwight GTK panel for UNIX destops\n", version);
    printf("Command line options:\n");
    printf(" -h  -- print this help and exit:\n");
    printf(" -v  -- print version and exit:\n");
    printf(" -p <name> -- use named profile. File ~/.fbpanel/<name> must exist\n");
}

FILE *
open_profile(gchar *profile)
{
    gchar *tmp;
    struct stat buf;
    FILE *fp;

    ENTER;
    /* check private configuration directory */
    tmp = g_strdup_printf("%s/.fbpanel", getenv("HOME"));
    if (stat(tmp, &buf)) {
        if (errno != ENOENT) {
            ERR( "fbpanel: can't fstat %s\n", tmp);
            perror(NULL);
            RET(NULL);
        }

        
        ERR( "Directory %s does  not exists. Creating...\n", tmp);
        if (mkdir(tmp, (S_IRUSR | S_IWUSR | S_IXUSR))) {
            ERR( "fbpanel: can't make directory %s\n", tmp);
            perror(NULL);
            RET(NULL);
        }
    } else if (!S_ISDIR(buf.st_mode)) {
        ERR( "fbpanel: %s is not directory. Exiting...\n", tmp);
        RET(NULL);
    }

    /* check profile */
    g_free(tmp);
    tmp = g_strdup_printf("%s/.fbpanel/%s", getenv("HOME"), profile);
    if (!(fp = fopen(tmp, "r+")) && !(fp = fopen(tmp, "w+"))) {
        ERR( "fbpanel: can't open profile %s\n", tmp);
        perror(NULL);
        RET(NULL);
    }
    cfgfile = tmp;
    RET(fp);
}

void
handle_error(Display * d, XErrorEvent * ev)
{
    char buf[256];

    ENTER;
    XGetErrorText(GDK_DISPLAY(), ev->error_code, buf, 256);
    //ERR( "fbpanel : X error: %s\n", buf);
    RET();
}

static void
sig_usr(int signum)
{
    if (signum != SIGUSR1)
        return;
    gtk_main_quit();
}

int
main(int argc, char *argv[], char *env[])
{
    int i;
    int quit = 0;

    ENTER;
    setlocale(LC_CTYPE, "");
    gtk_set_locale();
    gtk_init(&argc, &argv);
    XSetLocaleModifiers("");
    XSetErrorHandler((XErrorHandler) handle_error);
    resolve_atoms();
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-h")) {
            usage();
            exit(0);
        } else if (!strcmp(argv[i], "-v")) {
            printf("fbpanel %s\n", version);
            exit(0);
        } else if (!strcmp(argv[i], "-p")) {
            i++;
            if (i == argc) {
                ERR( "fbpanel: missing profile name\n");
                usage();
                exit(1);
            } else {
                cprofile = g_strdup(argv[i]);
            }
        } else {
            printf("fbpanel: unknown option - %s\n", argv[i]);
            usage();
            exit(1);
        }
    }
    signal(SIGUSR1, sig_usr);
    do {
        if (!(pfp = open_profile(cprofile)))
            exit(1);
        p = g_new0(panel, 1);
        g_return_val_if_fail (p != NULL, 1);
        if (!panel_start(p, pfp)) {
            ERR( "fbpanel: can start panel\n");
            exit(1);
        }
        fclose(pfp);
        gtk_main();
        panel_stop(p);
        g_free(p);
    } while (quit == 0);
    
    exit(0);
}


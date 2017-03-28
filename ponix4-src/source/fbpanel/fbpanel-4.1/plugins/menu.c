#include <stdlib.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

#include "panel.h"
#include "misc.h"
#include "plugin.h"
#include "bg.h"

//#define DEBUG
#include "dbg.h"

/*
 * SuxPanel version 0.1
 * Copyright (c) 2003 Leandro Pereira <leandro@linuxmag.com.br>
 */

/*
 * menu style code was taken from suxpanel
 */


typedef struct {
    GtkTooltips *tips;
    GtkWidget *mainw, *mbar;
    int iconsize, paneliconsize;
} menup;

static gchar *menu_rc = "style 'menubar-style'\n"
"{\n"
"GtkMenuBar::shadow-type = none\n"
"GtkMenuBar::internal-padding = 0\n"
"GtkMenu::vertical-padding = 0\n"
"}\n"
"class 'GtkMenuBar' style 'menubar-style'\n"
"class 'GtkMenu'    style 'menubar-style'\n";
/*
static gchar *transparent_rc = "style 'transparent-style'\n"
"{\n"
"bg_pixmap[NORMAL] = \"<parent>\"\n"
"bg_pixmap[INSENSITIVE] = \"<parent>\"\n"
"bg_pixmap[PRELIGHT] = \"<parent>\"\n"
"bg_pixmap[SELECTED] = \"<parent>\"\n"
"bg_pixmap[ACTIVE] = \"<parent>\"\n"
"}\n"
"widget '*.mainmenu' style 'transparent-style'\n";
*/

static void
menu_destructor(plugin *p)
{
    menup *m = (menup *)p->priv;

    ENTER;
    gtk_widget_destroy(m->mainw);
  
    //
    g_free(m);
    RET();
}

static void
spawn_app(GtkWidget *widget, gpointer data)
{
    GError *error = NULL;

    ENTER;    
    if (data) {
        if (! g_spawn_command_line_async(data, &error) ) {
            ERR("can't spawn %s\nError is %s\n", data, error->message);
            g_error_free (error);
        }
    }
    RET();
}


static void
run_command(GtkWidget *widget, void (*cmd)(void))
{
    ENTER;    
    cmd();
    RET();
}

static FILE *
read_include(FILE *fp)
{
    FILE *fi;
    line s;
    gchar *name;
    
    ENTER;
    s.len = 256;
    name = 0;
    while (get_line(fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_VAR) {
            if (!g_ascii_strcasecmp(s.t[0], "name")) 
                name = expand_tilda(s.t[1]);
            else {
                ERR( "menu/include: unknown var %s\n", s.t[0]);
                goto error;
            }
        } 
    }

    if ((fi = fopen(name, "r"))) {
        ERR("Including %s\n", name);
        g_free(name);
    }
    else {
        ERR("Can't include %s\n", name);
	goto error;
    }
      
    RET(fi);

 error:
    g_free(name);
    RET(NULL);
}

static GtkWidget *
read_item(plugin *p)
{
    line s;
    gchar *name, *fname, *action;
    GtkWidget *item;
    menup *m = (menup *)p->priv;
    void (*cmd)(void);
    
    ENTER;
    s.len = 256;
    name = fname = action = 0;
    cmd = NULL;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_VAR) {
            if (!g_ascii_strcasecmp(s.t[0], "image")) 
                fname = expand_tilda(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "name"))
                name = g_strdup(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "action"))
                action = g_strdup(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "command")) {
                command *tmp;
                
                for (tmp = commands; tmp->name; tmp++) {
                    if (!g_ascii_strcasecmp(s.t[1], tmp->name)) {
                        cmd = tmp->cmd;
                        break;
                    }
                }
            } else {
                ERR( "menu/item: unknown var %s\n", s.t[0]);
                goto error;
            }
        } 
    }
    /* menu button */
    item = gtk_image_menu_item_new_with_label(name ? name : "");
    gtk_container_set_border_width(GTK_CONTAINER(item), 0);
    if (name)
        g_free(name);
    if (fname) {
        GtkWidget *img;
        img = gtk_image_new_from_file_scaled(fname, m->iconsize, m->iconsize, TRUE);
        gtk_widget_show(img);
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), img);
        g_free(fname);
    }
    if (cmd) {
        g_signal_connect(G_OBJECT(item), "activate", (GCallback)run_command, cmd);
    } else if (action) {
        g_signal_connect(G_OBJECT(item), "activate", (GCallback)spawn_app, action);
    }
    RET(item);

 error:
    g_free(fname);
    g_free(name);
    g_free(action);
    RET(NULL);
}

static GtkWidget *
read_separator(plugin *p)
{
    line s;

    ENTER;
    s.len = 256;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        ERR("menu: error - separator can not have paramteres\n");
        RET(NULL);
    }
    RET(gtk_separator_menu_item_new());
}

static GtkWidget *
read_submenu(plugin *p, int level)
{
    line s;
    GtkWidget *mi, *menu;
    gchar *name, *fname;
    menup *m = (menup *)p->priv;
    FILE *fbackup;
    int isinclude;
    
    ENTER;
    s.len = 256;
    menu = gtk_menu_new ();
    gtk_container_set_border_width(GTK_CONTAINER(menu), 0);
    gtk_widget_set_name(GTK_WIDGET(menu), "mainmenu");
    //gtk_rc_parse_string(menu_rc);
    
    name = fname = 0;
    isinclude = 0;
    fbackup = 0;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {       
        if (s.type == LINE_BLOCK_START) {
            mi = NULL;
            if (!g_ascii_strcasecmp(s.t[0], "item")) {
                mi = read_item(p);
            } else if (!g_ascii_strcasecmp(s.t[0], "separator")) {
                mi = read_separator(p);
            } else if (!g_ascii_strcasecmp(s.t[0], "menu")) {
                mi = read_submenu(p, level+1);
            } else if (!g_ascii_strcasecmp(s.t[0], "include")) {
                if (isinclude)
		   {
		     ERR("menu: illegal include in this context\n");
                     goto error;
		   }
                fbackup = p->fp;
                if ((p->fp = read_include(p->fp))) {
		    isinclude = 1;
		    continue;
		} else {
                    p->fp = fbackup;
                    goto error;
		}
            } else {
                ERR("menu: unknown block %s\n", s.t[0]);
                goto error;
            }
            if (!mi) {
                ERR("menu: can't create menu item\n");
                goto error;
            }
            gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
        } else if (s.type == LINE_VAR) {
            if (!g_ascii_strcasecmp(s.t[0], "image")) 
                fname = expand_tilda(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "name"))
                name = g_strdup(s.t[1]);
             else {
                ERR("menu: unknown var %s\n", s.t[0]);
                goto error;
            }
        } else if (s.type == LINE_NONE && isinclude) {
            isinclude = 0;
            fclose(p->fp);
            p->fp = fbackup;
        } else {
            ERR("menu: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    mi = gtk_image_menu_item_new_with_label(name ? name : "");
    if (name) 
        g_free(name);
    if (fname) {
        GtkWidget *img;

        if (level == 0) {
            //bg_set_transparent_background(mi, p->panel);
            img = gtk_image_new_from_file_scaled(fname,
                  m->paneliconsize, m->paneliconsize, TRUE);
            gtk_widget_set_name(mi, "mainmenu");
        } else
            img = gtk_image_new_from_file_scaled(fname, m->iconsize, m->iconsize, TRUE);
        gtk_widget_show(img);
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(mi), img);
        g_free(fname);
    }
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (mi), menu);
    RET(mi);

 error:
    // FIXME: we need to recursivly destroy all child menus and their items
    gtk_widget_destroy(menu);
    g_free(fname);
    g_free(name);
    RET(0);
}




static int
menu_constructor(plugin *p)
{
    menup *m;
    GtkWidget *mi;
    line s;

    ENTER;
    s.len = 256;
    m = g_new0(menup, 1);
    g_return_val_if_fail(m != NULL, 0);
    p->priv = m;
  
    if  (p->panel->orientation == ORIENT_HORIZ) 
        m->paneliconsize = GTK_WIDGET(p->panel->box)->allocation.height
            - 2* GTK_WIDGET(p->panel->box)->style->ythickness;
    else
        m->paneliconsize = GTK_WIDGET(p->panel->box)->allocation.width
            - 2* GTK_WIDGET(p->panel->box)->style->xthickness;
    m->iconsize = 22;       
    /* menubar */
    /* holds button to pop menu */
    m->mainw = gtk_menu_bar_new();
    //bg_set_transparent_background(m->mainw, p->panel);
    gtk_widget_set_name(m->mainw, "mainmenu");
    //gtk_widget_set_name(GTK_WIDGET(m->mainw), "sux-menu");
    gtk_rc_parse_string(menu_rc);
    /*
    if (p->panel->transparent) 
        gtk_rc_parse_string(transparent_rc);
    */  
    gtk_container_set_border_width(GTK_CONTAINER(m->mainw), 0);
    gtk_container_add(GTK_CONTAINER(p->pwid), m->mainw);
    gtk_widget_show(m->mainw);

    /* menu button */
    mi = read_submenu(p, 0);
    //bg_set_transparent_background(mi, p->panel);
    if (!mi) {
        ERR("menu: plugin init failde\n");
        goto error;
    }
    gtk_menu_shell_append (GTK_MENU_SHELL (m->mainw), mi);
    RET(1);

 error:
    menu_destructor(p);
    RET(0);
}


plugin_class menu_plugin_class = {
    fname: NULL,
    count: 0,

    type : "menu",
    name : "menu",
    version: "1.0",
    description : "Provide Menu",

    constructor : menu_constructor,
    destructor  : menu_destructor,
};

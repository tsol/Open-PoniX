#include <stdlib.h>
#include <string.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "panel.h"
#include "misc.h"
#include "plugin.h"
#include "bg.h"
#include "gtkbgbox.h"

//#define DEBUG
#include "dbg.h"


static const char desktop_ent[] = "Desktop Entry";
static const gchar app_dir_name[] = "applications";
static GHashTable *ht;
static GtkWidget *read_submenu(plugin *p, gboolean as_item);

typedef struct {
    gchar *name;
    gchar *icon;
    gchar *local_name;
    GtkWidget *menu;
} cat_info;

static cat_info main_cats[] = {
    { "AudioVideo", "gnome-multimedia", "Audio & Video" },
    { "Development","gnome-devel" },
    { "Education",  "gnome-applications" },
    { "Game",       "gnome-joystick" },
    { "Graphics",   "gnome-graphics" },
    { "Network",    "gnome-globe" },
    { "Office",     "gnome-applications" },
    { "Settings",   "gnome-settings" },
    { "System",     "gnome-system" },
    { "Utility",    "gnome-util" },
};
typedef struct {
    GtkTooltips *tips;
    GtkWidget *menu, *box, *bg, *label;
    gulong handler_id;
    int iconsize, paneliconsize;
    GSList *files;
} menup;

static void
menu_destructor(plugin *p)
{
    menup *m = (menup *)p->priv;

    ENTER;
    g_signal_handler_disconnect(G_OBJECT(m->bg), m->handler_id);
    if (m->menu) {
        DBG("destroy(m->menu)\n");
        gtk_widget_destroy(m->menu);
    }
    DBG("destroy(m->box)\n");
    gtk_widget_destroy(m->box);
    DBG("here\n");
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
            ERR("can't spawn %s\nError is %s\n", (char *)data, error->message);
            g_error_free (error);
        }
    }
    RET();
}

static gint
_menu_shell_insert_sorted(GtkMenuShell *menu_shell, GtkWidget *mi, const gchar *name)
{
    GList *items;
    gint i;
    gchar *cmpname;

    //TRACE("dummy");

    items = gtk_container_get_children(GTK_CONTAINER(menu_shell));
    for(i=0; items; items=items->next, i++)  {
        cmpname = (gchar *)g_object_get_data(G_OBJECT(items->data), "item-name");
        if(cmpname && g_ascii_strcasecmp(name, cmpname) < 0)
            break;
    }
    gtk_menu_shell_insert(menu_shell, mi, i);
    return i;
}

static void
do_app_dir(plugin *p, const gchar *path)
{
    GDir* dir;
    const gchar* name;
    gchar *cwd, **cats, **tmp, *exec, *title, *icon, *dot;
    GKeyFile*  file;

    ENTER;
    DBG("path: %s\n", path);
    dir = g_dir_open(path, 0, NULL);
    if (!dir)
        RET();
    cwd = g_get_current_dir();
    g_chdir(path);
    file = g_key_file_new();
    while ((name = g_dir_read_name(dir))) {
        DBG("name: %s\n", name);
        if (g_file_test(name, G_FILE_TEST_IS_DIR)) {
            do_app_dir(p, name);
            continue;
        }
        if (!g_str_has_suffix(name, ".desktop"))
            continue;
        if (!g_key_file_load_from_file(file, name, 0, NULL))
            continue;
        if (g_key_file_get_boolean(file, desktop_ent, "NoDisplay", NULL))
            continue;
        if (g_key_file_has_key(file, desktop_ent, "OnlyShowIn", NULL))
            continue;
        if (!(cats = g_key_file_get_string_list(file, desktop_ent, "Categories", NULL, NULL)))
            continue;
        if (!(exec = g_key_file_get_string(file, desktop_ent, "Exec", NULL)))
            goto free_cats;
        while ((dot = strchr(exec, '%'))) {
            if (dot[1] != '\0')
                dot[0] = dot[1] = ' ';
        }
        DBG("exec: %s\n", exec);
        if (!(title = g_key_file_get_locale_string(file, desktop_ent, "Name", NULL, NULL)))
            goto free_exec;
        DBG("title: %s\n", title);
        icon = g_key_file_get_string(file, desktop_ent, "Icon", NULL);
        if (icon) {
            dot = strchr( icon, '.' );
            if(icon[0] !='/' && dot )
                *dot = '\0';
        }
        DBG("icon: %s\n", icon);
        for (tmp = cats; *tmp; tmp++) {
            GtkWidget **menu, *mi;

            DBG("cat: %s\n", *tmp);
            if (!(menu = g_hash_table_lookup(ht, tmp[0])))
                continue;

            mi = gtk_image_menu_item_new_with_label(title);
            gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(mi),
                  gtk_fbimage_new(icon, icon, 22, 22, TRUE));
            g_signal_connect(G_OBJECT(mi), "activate", (GCallback)spawn_app,g_strdup(exec));
            if (!(*menu))
                *menu = gtk_menu_new();
            g_object_set_data_full(G_OBJECT(mi), "item-name", title, g_free);
            _menu_shell_insert_sorted(GTK_MENU_SHELL(*menu), mi, title);
            //gtk_menu_shell_prepend(GTK_MENU_SHELL(*menu), mi);
            gtk_widget_show_all(mi);
            DBG("added =======================================\n");
            break;
        }
        g_free(icon);
    free_exec:
        g_free(exec);
    free_cats:
        g_strfreev(cats);
    }
    g_chdir(cwd);
    g_free(cwd);
    RET();
}


void
make_fdo_menu(plugin *p, GtkWidget *menu)
{
    const char** sys_dirs = (const char**)g_get_system_data_dirs();
    int i;
    gchar *path;
    menup *m = (menup *)p->priv;

    ENTER;
    ht = g_hash_table_new(g_str_hash, g_str_equal);
    for (i = 0; i < G_N_ELEMENTS(main_cats); i++) {
        g_hash_table_insert(ht, main_cats[i].name, &main_cats[i].menu);
        main_cats[i].menu = NULL;
        if (g_hash_table_lookup(ht, &main_cats[i].name))
            DBG("%s not found\n", main_cats[i].name);
    }

    for (i = 0; i < g_strv_length((gchar **)sys_dirs); ++i)    {
        path = g_build_filename(sys_dirs[i], app_dir_name, NULL );
        do_app_dir(p, path);
        g_free(path);
    }
    path = g_build_filename(g_get_user_data_dir(), app_dir_name, NULL);
    do_app_dir(p, path);
    g_free(path);
    //build menu
    for (i = 0; i < G_N_ELEMENTS(main_cats); i++) {
        GtkWidget *mi;
        gchar *name;

        if (main_cats[i].menu) {
            name = main_cats[i].local_name ? main_cats[i].local_name : main_cats[i].name;
            mi = gtk_image_menu_item_new_with_label(name);
            gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(mi),
                gtk_fbimage_new(main_cats[i].icon, NULL, m->iconsize, m->iconsize, TRUE));
            gtk_menu_item_set_submenu (GTK_MENU_ITEM (mi), main_cats[i].menu);
            gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
            gtk_widget_show_all(mi);
            gtk_widget_show_all(main_cats[i].menu);
        }
    }
    g_hash_table_destroy(ht);
    RET();
}


static void
run_command(GtkWidget *widget, void (*cmd)(void))
{
    ENTER;
    cmd();
    RET();
}

static gboolean
delayed_menu_creation(plugin *p)
{
    menup *m;

    ENTER;
    m = (menup *)p->priv;
    if (!m->menu) {
        fseek(p->fp, 0, SEEK_SET);
        read_submenu(p, TRUE);
    }
    RET(FALSE); /* run once */
}

static gboolean
my_button_pressed(GtkWidget *widget, GdkEventButton *event, plugin *p)
{
    menup *m;

    ENTER;
    m = (menup *)p->priv;
    if ((event->type == GDK_BUTTON_PRESS)
          && (event->x >=0 && event->x < widget->allocation.width)
          && (event->y >=0 && event->y < widget->allocation.height)) {
        if (!m->menu) {
            DBG("building menu\n");
            fseek(p->fp, 0, SEEK_SET);
            read_submenu(p, TRUE);
        }
        gtk_menu_popup(GTK_MENU(m->menu),
              NULL, NULL, (GtkMenuPositionFunc)menu_pos, widget,
              event->button, event->time);
    }
    RET(TRUE);
}


static GtkWidget *
make_button(plugin *p, gchar *iname, gchar *fname, gchar *name, GtkWidget *menu)
{
    int w, h;
    menup *m;

    ENTER;
    m = (menup *)p->priv;
    if (p->panel->orientation == ORIENT_HORIZ) {
        w = 10000;
        h = p->panel->ah;
    } else {
        w = p->panel->aw;
        h = 10000;
    }
    DBG("iname=%s\n", iname);
    m->bg = fb_button_new_from_icon_file_with_label(iname, fname, w, h, 0x702020, TRUE, name);
    gtk_widget_show(m->bg);
    gtk_box_pack_start(GTK_BOX(m->box), m->bg, FALSE, FALSE, 0);
    if (p->panel->transparent)
        gtk_bgbox_set_background(m->bg, BG_ROOT, p->panel->tintcolor, p->panel->alpha);


    m->handler_id = g_signal_connect (G_OBJECT (m->bg), "button-press-event",
          G_CALLBACK (my_button_pressed), p);
    g_object_set_data(G_OBJECT(m->bg), "plugin", p);

    RET(m->bg);
}

static GtkWidget *
read_item(plugin *p)
{
    line s;
    gchar *name, *fname, *iname, *action;
    GtkWidget *item;
    menup *m = (menup *)p->priv;
    void (*cmd)(void);

    ENTER;
    s.len = 256;
    name = fname = action = iname = NULL;
    cmd = NULL;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_VAR) {
            if (!g_ascii_strcasecmp(s.t[0], "image"))
                fname = expand_tilda(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "name"))
                name = g_strdup(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "icon"))
                iname = g_strdup(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "action"))
                action = expand_tilda(s.t[1]);
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
    if (fname || iname) {
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item),
            gtk_fbimage_new(iname, fname, m->iconsize, m->iconsize, TRUE));
        g_free(fname);
        g_free(iname);
    }
    if (cmd) {
        g_signal_connect(G_OBJECT(item), "activate", (GCallback)run_command, cmd);
    } else if (action) {
        g_signal_connect(G_OBJECT(item), "activate", (GCallback)spawn_app, action);
        g_object_set_data_full(G_OBJECT(item), "activate", action, g_free);
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


static void
read_system_menu(plugin *p, GtkWidget *menu)
{
    line s;

    ENTER;
    s.len = 256;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) ;
    make_fdo_menu(p, menu);
    RET();
}

static void
read_include(plugin *p)
{
    gchar *name;
    line s;
    menup *m = (menup *)p->priv;
    FILE *fp = NULL;

    ENTER;
    s.len = 256;
    name = NULL;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_VAR) {
            if (!g_ascii_strcasecmp(s.t[0], "name")) {
                g_free(name);
                name = expand_tilda(s.t[1]);
            } else {
                ERR( "menu/include: unknown var %s\n", s.t[0]);
            }
        }
    }
    fp = fopen(name, "r");
    g_free(name);
    if (fp) {
        m->files = g_slist_prepend(m->files, p->fp);
        p->fp = fp;
    } else {
        ERR("Can't include '%s'\n", name);
    }
    RET();
}

static GtkWidget *
read_submenu(plugin *p, gboolean as_item)
{
    line s;
    GtkWidget *mi, *menu;
    gchar name[256], *fname, *iname;
    menup *m = (menup *)p->priv;


    ENTER;
    s.len = 256;
    menu = gtk_menu_new ();
    gtk_container_set_border_width(GTK_CONTAINER(menu), 0);

    iname = fname = NULL;
    name[0] = 0;
    DBG("here\n");
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_BLOCK_START) {
            if (!as_item)
                break;
            mi = NULL;
            if (!g_ascii_strcasecmp(s.t[0], "item")) {
                mi = read_item(p);
            } else if (!g_ascii_strcasecmp(s.t[0], "separator")) {
                mi = read_separator(p);
            } else if (!g_ascii_strcasecmp(s.t[0], "menu")) {
                mi = read_submenu(p, TRUE);
            } else if (!g_ascii_strcasecmp(s.t[0], "systemmenu")) {
                read_system_menu(p, menu);
                continue;
            } else if (!g_ascii_strcasecmp(s.t[0], "include")) {
                read_include(p);
                continue;
            } else {
                ERR("menu: unknown block %s\n", s.t[0]);
                goto error;
            }
            if (!mi) {
                ERR("menu: can't create menu item\n");
                goto error;
            }
            gtk_widget_show(mi);
            gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
        } else if (s.type == LINE_VAR) {
            DBG("here\n");
            if (!g_ascii_strcasecmp(s.t[0], "image"))
                fname = expand_tilda(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "name"))
                strcpy(name, s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "icon")) {
                iname = g_strdup(s.t[1]);
                DBG("icon\n");
            } else {
                ERR("menu: unknown var %s\n", s.t[0]);
                goto error;
            }
            DBG("here\n");
        } else if (s.type == LINE_NONE) {
            if (m->files) {
                fclose(p->fp);
                p->fp = m->files->data;
                m->files = g_slist_delete_link(m->files, m->files);
            }
        }  else {
            ERR("menu: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    DBG("here\n");
    if (as_item) {
        mi = gtk_image_menu_item_new_with_label(name ? name : "");
        if (fname) {
            GtkWidget *img;
            img = gtk_image_new_from_file_scaled(fname, m->iconsize, m->iconsize, TRUE);
            gtk_widget_show(img);
            gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(mi), img);
            g_free(fname);
        }
        gtk_menu_item_set_submenu (GTK_MENU_ITEM (mi), menu);
        m->menu = menu;
        RET(mi);
    } else {
        mi = make_button(p, iname, fname, name, menu);
        if (fname)
            g_free(fname);
        RET(mi);
    }

 error:
    // FIXME: we need to recursivly destroy all child menus and their items
    DBG("destroy(menu)\n");
    gtk_widget_destroy(menu);
    g_free(fname);
    g_free(name);
    RET(NULL);
}


static void
menu_icon_theme_changed(GtkIconTheme *icon_theme, plugin *p)
{
    menup *m;

    ENTER;
    m = (menup *)p->priv;
    if (m->menu) {
        DBG("destroy(m->menu)\n");
        gtk_widget_destroy(m->menu);
    }
    m->menu = NULL;
    g_timeout_add(3000, (GSourceFunc) delayed_menu_creation, p);
}


static int
menu_constructor(plugin *p)
{
    menup *m;

    ENTER;
    m = g_new0(menup, 1);
    g_return_val_if_fail(m != NULL, 0);
    p->priv = m;
    m->iconsize = 22;
    m->box = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(m->box), 0);
    gtk_container_add(GTK_CONTAINER(p->pwid), m->box);

    if (!read_submenu(p, FALSE)) {
        ERR("menu: plugin init failed\n");
        goto error;
    }
    g_timeout_add(3000, (GSourceFunc) delayed_menu_creation, p);
    g_signal_connect (G_OBJECT(gtk_icon_theme_get_default()),
        "changed", (GCallback) menu_icon_theme_changed, p);
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


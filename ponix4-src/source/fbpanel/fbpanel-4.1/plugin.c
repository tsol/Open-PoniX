
#include "plugin.h"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
#include <gdk/gdk.h>
#include <string.h>
#include <stdlib.h>


//#define DEBUG
#include "dbg.h"
#include "misc.h"
#include "bg.h"
#include "gtkbgbox.h"

static GList *pcl = NULL;


/* counter for static (built-in) plugins must be greater then zero
 * so fbpanel will not try to unload them */

#define REGISTER_PLUGIN_CLASS(pc, dynamic) \
do {\
    extern plugin_class pc;\
    register_plugin_class(&pc, dynamic);\
} while (0)


static void
register_plugin_class(plugin_class *pc, int dynamic)
{
    pcl = g_list_append(pcl, pc);
    pc->dynamic = dynamic;
    if (!pc->dynamic)
        pc->count++;
    /* reloading tray results in segfault due to registering static type in dll.
     * so keep it always onboard until bug fix */
    if (!strcmp(pc->type, "tray"))
        pc->count++; 
}

static void
init_plugin_class_list()
{
    ENTER;
#ifdef STATIC_SEPARATOR
    REGISTER_PLUGIN_CLASS(separator_plugin_class, 0);
#endif
    
#ifdef STATIC_IMAGE
    REGISTER_PLUGIN_CLASS(image_plugin_class, 0);
#endif
      
#ifdef STATIC_LAUNCHBAR
    REGISTER_PLUGIN_CLASS(launchbar_plugin_class, 0);
#endif
  
#ifdef STATIC_DCLOCK
    REGISTER_PLUGIN_CLASS(dclock_plugin_class, 0);
#endif
    
#ifdef STATIC_WINCMD
    REGISTER_PLUGIN_CLASS(wincmd_plugin_class, 0);
#endif
    
#ifdef STATIC_TEST
    REGISTER_PLUGIN_CLASS(test_plugin_class, 0);
#endif

#ifdef STATIC_TASKBAR
    REGISTER_PLUGIN_CLASS(taskbar_plugin_class, 0);
#endif

#ifdef STATIC_PAGER
    REGISTER_PLUGIN_CLASS(pager_plugin_class, 0);
#endif

#ifdef STATIC_TRAY
    REGISTER_PLUGIN_CLASS(tray_plugin_class, 0);
#endif

#ifdef STATIC_MENU
    REGISTER_PLUGIN_CLASS(menu_plugin_class, 0);
#endif

#ifdef STATIC_SPACE
    REGISTER_PLUGIN_CLASS(space_plugin_class, 0);
#endif

#ifdef STATIC_ICONS
    REGISTER_PLUGIN_CLASS(icons_plugin_class, 0);
#endif
    
    RET();
}





plugin *
plugin_load(char *type)
{
    GList *tmp;
    plugin_class *pc = NULL;
    plugin *plug = NULL;
    static GString *str = NULL;
    
    ENTER;
    if (!pcl)
        init_plugin_class_list();

    for (tmp = pcl; tmp; tmp = g_list_next(tmp)) {
        pc = (plugin_class *) tmp->data;
        if (!g_ascii_strcasecmp(type, pc->type)) {
            break;
        }
    }
#ifndef STATIC_PLUGINS
    if (!tmp && g_module_supported()) {
        GModule *m;

        DBG("loading dll %s\n", type);
        if (!str)
            str = g_string_sized_new(PATH_MAX);
        g_string_printf(str, "%s/.fbpanel/plugins/%s.so", getenv("HOME"), type);
        m = g_module_open(str->str, G_MODULE_BIND_LOCAL | G_MODULE_BIND_LAZY);
        DBG("opening %s\n", str->str);
        if (!m) {
            DBG("%s\n", g_module_error());
            g_string_printf(str, "%s/share/fbpanel/plugins/%s.so", PREFIX, type);
            m = g_module_open(str->str, G_MODULE_BIND_LOCAL | G_MODULE_BIND_LAZY);
            DBG("opening %s\n", str->str);
        }
        if (!m) {
            ERR("%s\n", g_module_error());
            RET(NULL);
        }
        if (verbose)
            ERR("loading %s\n", str->str);
        g_string_printf(str, "%s_plugin_class", type);
        if (!g_module_symbol(m, str->str, &pc) || pc == NULL
              || strcmp(type, pc->type)) {
            g_module_close(m);
            ERR("%s.so is not a fbpanel plugin\n", type);
            RET(NULL);
        }
        pc->gmodule = m;
        register_plugin_class(pc, 1);
    
    }
#endif

    /* nothing was found */
    if (!pc)
        RET(NULL);
    
    plug = g_new0(plugin, 1);
    g_return_val_if_fail (plug != NULL, NULL);
    plug->class = pc;
    pc->count++;
    RET(plug);
}


void plugin_put(plugin *this)
{
    plugin_class *pc = this->class;

    ENTER;
    pc->count--;
    if (pc->count == 0 && pc->dynamic) {
        pcl = g_list_remove(pcl, pc);
        g_module_close(pc->gmodule);           
        DBG("unloading plugin %s\n", pc->type);
    }
    g_free(this);
    RET();
}

int
plugin_start(plugin *this)
{
    ENTER;

    if (!this->class->invisible) {
        this->pwid = gtk_bgbox_new();
        gtk_widget_set_name(this->pwid, this->class->type);
        gtk_box_pack_start(GTK_BOX(this->panel->box), this->pwid, this->expand, TRUE,
              this->padding);
        if (this->panel->transparent) 
            gtk_bgbox_set_background(this->pwid, BG_ROOT, this->panel->tintcolor, this->panel->alpha);        
        gtk_widget_show(this->pwid);
    }
    if (!this->class->constructor(this)) {
        if (!this->class->invisible)
            gtk_widget_destroy(this->pwid);
        RET(0);
    }
    RET(1);
}


void plugin_stop(plugin *this)
{
    ENTER;
    DBG("%s\n", this->class->type);
    this->class->destructor(this);
    this->panel->plug_num--;
    if (!this->class->invisible) 
        gtk_widget_destroy(this->pwid);
    RET();
}


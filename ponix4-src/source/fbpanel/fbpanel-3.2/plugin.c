
#include "plugin.h"


//#define DEBUG
#include "dbg.h"


static GList *pcl = NULL;


/* counter for static (built-in) plugins must be greater then zero
 * so fbpanel will not try to unload them */
#define STATIC_PLUGIN_CLASS(spc) \
do { \
extern plugin_class spc;\
pcl = g_list_append(pcl, &spc);\
spc.count++;\
} while (0)

 
static void
init_plugin_class_list()
{
    ENTER;
#ifdef STATIC_SEPARATOR
    STATIC_PLUGIN_CLASS(separator_plugin_class);
#endif
    
#ifdef STATIC_IMAGE
    STATIC_PLUGIN_CLASS(image_plugin_class);
#endif
      
#ifdef STATIC_LAUNCHBAR
    STATIC_PLUGIN_CLASS(launchbar_plugin_class);
#endif

  
#ifdef STATIC_DCLOCK
    STATIC_PLUGIN_CLASS(dclock_plugin_class);
#endif
    
#ifdef STATIC_WINCMD
    STATIC_PLUGIN_CLASS(wincmd_plugin_class);
#endif
    
#ifdef STATIC_TEST
    STATIC_PLUGIN_CLASS(test_plugin_class);
#endif

#ifdef STATIC_TASKBAR
    STATIC_PLUGIN_CLASS(taskbar_plugin_class);
#endif

#ifdef STATIC_PAGER
    STATIC_PLUGIN_CLASS(pager_plugin_class);
#endif

#ifdef STATIC_TRAY
    STATIC_PLUGIN_CLASS(tray_plugin_class);
#endif

    RET();
}





plugin *
plugin_load(char *type)
{
    GList *tmp;
    plugin_class *pc = NULL;
    plugin *plug = NULL;
    
    ENTER;
    if (!pcl)
        init_plugin_class_list();

    for (tmp = pcl; tmp; tmp = g_list_next(tmp)) {
        pc = (plugin_class *) tmp->data;
        if (!g_ascii_strcasecmp(type, pc->type)) {
            break;
        }
    }
    if (!tmp) {
        /* try to load external plugin */
        pc = NULL;

    }

    /* nothing was found */
    if (!pc)
        return NULL;
    
    plug = g_new0(plugin, 1);
    g_return_val_if_fail (plug != NULL, NULL);
    plug->class = pc;
    pc->count++;
    return plug;
}


void plugin_put(plugin *this)
{
    ENTER;
    this->class->count--;
    if (this->class->count == 0)
        ERR( "unloading plugin %s\n", this->class->type);
    g_free(this);
    RET();
}


int
plugin_start(plugin *this)
{
    //panel *p = this->panel;
    ENTER;
    this->pwid = gtk_event_box_new();
    if (!this->class->constructor(this)) {
        gtk_widget_destroy(this->pwid);
        RET(0);
    }
    gtk_widget_show(this->pwid);
    /* OLD */

    gtk_box_pack_start(GTK_BOX(this->panel->box), this->pwid, this->expand, TRUE,
          this->padding);

    /* NEW */
    /*
    p->plug_num++;
    if (p->orientation == ORIENT_HORIZ) {
        gtk_table_attach(GTK_TABLE(p->box), this->pwid,
              p->plug_num - 1, p->plug_num,
              0, 1,
              (this->expand) ? (GTK_EXPAND | GTK_SHRINK | GTK_FILL) : 0,
              (GTK_EXPAND | GTK_SHRINK),
              0, 0);
        gtk_table_resize(GTK_TABLE(p->box), p->plug_num, 1);
    } else {
        gtk_table_attach(GTK_TABLE(p->box), this->pwid,
              0, 1,
              p->plug_num - 1, p->plug_num,
              (GTK_EXPAND | GTK_SHRINK),
              (this->expand) ? (GTK_EXPAND | GTK_SHRINK | GTK_FILL) : 0,
              0, 0);
        gtk_table_resize(GTK_TABLE(p->box), 1, p->plug_num);
        }
    */
    gtk_widget_show(this->pwid);
    RET(1);
}


void plugin_stop(plugin *this)
{
    ENTER;
    this->class->destructor(this);
    this->panel->plug_num--;
    gtk_widget_destroy(this->pwid);
    RET();
}





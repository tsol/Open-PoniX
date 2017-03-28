

#include "plugin.h"
#include "panel.h"
#include "misc.h"
#include "bg.h"
#include "gtkbgbox.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

//#define DEBUGPRN
#include "dbg.h"

void configure(void);
void restart(void);
void quit(void);

command commands[] = {
    { "configure", configure },
    { "restart", restart },
    { "quit", quit },
    { NULL, NULL },
};


#define FRAME_BORDER   6
#define INDENT_SIZE    20


GtkWidget *default_plugin_edit_config(plugin_instance *pl);

static GtkWidget *dialog = NULL;
static GtkSizeGroup *sg;

//width
static GtkWidget *width_spinb, *width_opt;
static GtkAdjustment *width_adj;

//height
static GtkWidget *height_spinb, *height_opt;
static GtkAdjustment *height_adj;

//margin
static GtkWidget *margin_label, *margin_spinb;
static GtkAdjustment *margin_adj;

//allign
static GtkWidget *allign_opt;

//edge
static GtkWidget *edge_opt;

//effects
static GtkWidget *tr_checkb, *tr_colorb;;
static GtkWidget *tr_box;
static GtkWidget *prop_round_corners_checkb, *round_corners_box, *round_corners_spinb;
static GtkAdjustment *round_corners_adj;


//properties
static GtkWidget *prop_dt_checkb, *prop_st_checkb, *prop_autohide_checkb;
static GtkWidget *height_when_hidden_box, *height_when_hidden_spinb;
static GtkAdjustment *height_when_hidden_adj;

//list of available plugins
static GtkListStore* apl;

static GtkWidget *prop_layer_checkb, *layer_box, *layer_opt;

extern panel *the_panel;
extern gchar *cprofile;
extern int config;
extern FILE *pconf;

static void global_config_save(FILE *fp);
static void plugin_config_save_all(FILE *fp);

static void
add_hindent_box(GtkWidget *box)
{
    GtkWidget  *indent_box;
    
    indent_box = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX (box), indent_box, FALSE, TRUE, 0);
    gtk_widget_set_size_request(indent_box, INDENT_SIZE, 1);
}

static void
update_toggle_button(GtkWidget *w, gboolean val)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), !val);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), val);
}


static int
mk_profile_dir()
{
    gchar fname[1024];
    struct stat buf;
    int ret;

    ENTER;
    sprintf(fname, "%s/.fbpanel", getenv("HOME"));
    if ((ret = stat(fname, &buf))) {
        LOG(LOG_INFO, "creating %s\n", fname);
        mkdir(fname, 0755);
        ret = stat(fname, &buf);
    }
    if (ret)
        RET(0);
    if (!(S_ISDIR(buf.st_mode) && (S_IWUSR & buf.st_mode) && (S_IXUSR & buf.st_mode)))
        RET(0);
    RET(1);
}


static void
save_config()
{
    gchar fname[1024];
    FILE *fp;

    ENTER;
    if (!mk_profile_dir()) {
        ERR("can't make ~/.fbpanel direcory\n");
        RET();
    }
    sprintf(fname, "%s/.fbpanel/%s", getenv("HOME"), cprofile);
    LOG(LOG_WARN, "saving profile %s as %s\n", cprofile, fname);
    if (!(fp = fopen(fname, "w"))) {
        ERR("can't open for write %s:", fname);
        perror(NULL);
        RET();
    }
    global_config_save(fp);
    plugin_config_save_all(fp);
    fclose(fp);
    RET();
}

static FILE*
make_empty_fp()
{
    FILE *fp;

    ENTER;
    if (!(fp = tmpfile())) {
        ERR( "can't open temporary file with tmpfile()\n");
        RET(NULL);
    }
    fprintf(fp, "}\n");
    fseek(fp, 0, SEEK_SET);
    RET(fp);
}
  
static void
set_edge(GtkComboBox *widget, gpointer bp)
{
    int edge;
    
    ENTER;    
    edge = gtk_combo_box_get_active(widget) + 1;
    the_panel->edge = edge;
    save_config();
    gtk_widget_queue_draw(dialog);
    gtk_main_quit();
    RET();
}


static void
set_allign(GtkComboBox *widget, gpointer bp)
{
    int allign;
    gboolean t;
    
    ENTER;	
    allign = gtk_combo_box_get_active(widget) + 1;
    DBG2("allign=%d\n", allign);
    t = (allign != ALLIGN_CENTER);
    gtk_widget_set_sensitive(margin_label, t);
    gtk_widget_set_sensitive(margin_spinb, t);
    the_panel->allign = allign;
    calculate_position(the_panel);
    gtk_window_move(GTK_WINDOW(the_panel->topgwin), the_panel->ax, the_panel->ay);
    RET();
}

static void
set_margin(GtkSpinButton* spin, gpointer user_data )
{
    ENTER;
    the_panel->margin = (int)gtk_spin_button_get_value(spin);
    calculate_position(the_panel);
    gtk_window_move(GTK_WINDOW(the_panel->topgwin), the_panel->ax, the_panel->ay);
    RET();
}


static void
set_width( GtkSpinButton* spin, gpointer user_data )
{
    ENTER;
    the_panel->width = (int)gtk_spin_button_get_value(spin);
    gtk_widget_queue_resize(the_panel->topgwin);
    RET();
}

static void
set_height( GtkSpinButton* spin, gpointer user_data )
{
    ENTER;
    the_panel->height = (int)gtk_spin_button_get_value(spin);
    gtk_widget_queue_resize(the_panel->topgwin);
    RET();
}

static void
set_width_type(GtkWidget *item, gpointer bp)
{
    int widthtype;
    gboolean t;
    
    ENTER;    
    widthtype = gtk_combo_box_get_active(GTK_COMBO_BOX(item)) + 1;
    DBG("widthtype=%d\n", widthtype);
    t = (widthtype != WIDTH_REQUEST);
    gtk_widget_set_sensitive(width_spinb, t);
    if (widthtype == WIDTH_PERCENT) {
        width_adj->upper = 100;
        width_adj->value = width_adj->upper;
    } else if  (widthtype == WIDTH_PIXEL) {
        width_adj->upper = gdk_screen_width();
        width_adj->value = width_adj->upper;
    } else
        RET();
    
    gtk_adjustment_changed(width_adj);
    gtk_adjustment_value_changed(width_adj);
    the_panel->widthtype = widthtype;
    gtk_widget_queue_resize(the_panel->topgwin);
    RET();
}

static void
set_round_corners(GtkWidget *b, gpointer bp)
{
    gboolean t;

    ENTER;
    t = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
    the_panel->round_corners = t;
    save_config();
    gtk_widget_queue_draw(dialog);
    gtk_main_quit();
    RET();
}

static void
set_transparency(GtkWidget *b, gpointer bp)
{
    gboolean t;

    ENTER;
    t = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
    //gtk_widget_set_sensitive(tr_colorl, t);
    the_panel->transparent = t;
    save_config();
    gtk_widget_queue_draw(dialog);
    gtk_main_quit();
    RET();
}

static void
set_transparency_color(GtkWidget *b, gpointer bp)
{
    ENTER2;
    gtk_color_button_get_color(GTK_COLOR_BUTTON(b), &the_panel->gtintcolor);
    the_panel->tintcolor = gcolor2rgb24(&the_panel->gtintcolor);
    the_panel->alpha = gtk_color_button_get_alpha(GTK_COLOR_BUTTON(b)) * 0xff / 0xffff;
    save_config();
    gtk_widget_queue_draw(dialog);
    gtk_main_quit();
    RET2();
}
#if 0
static void
set_dock_type(GtkToggleButton* toggle, gpointer user_data)
{
    Atom state;
    
    ENTER;
    the_panel->setdocktype = gtk_toggle_button_get_active(toggle) ? 1 : 0;
    state = (the_panel->setdocktype) ?
        a_NET_WM_WINDOW_TYPE_DOCK : a_NET_WM_WINDOW_TYPE_NORMAL;
    XChangeProperty(GDK_DISPLAY(), the_panel->topxwin, a_NET_WM_WINDOW_TYPE, XA_ATOM,
          32, PropModeReplace, (unsigned char *) &state, 1);
    gtk_widget_queue_resize(the_panel->topgwin);
    RET();
}
static void
set_strut(GtkToggleButton* toggle, gpointer user_data)
{
    ENTER;
    the_panel->setstrut = gtk_toggle_button_get_active(toggle) ? 1 : 0;
    if (!the_panel->setstrut) {
        DBG("remove strut\n");
        XDeleteProperty(GDK_DISPLAY(), the_panel->topxwin, a_NET_WM_STRUT_PARTIAL);
        XDeleteProperty(GDK_DISPLAY(), the_panel->topxwin, a_NET_WM_STRUT);
    } else
        panel_set_wm_strut(the_panel);
    gtk_widget_queue_resize(the_panel->topgwin);
    RET();
}

#endif

GtkWidget *
mk_position()
{
    GtkWidget *hbox, *hbox2, *label, *frame;
    GtkWidget *t;
   
   
    ENTER;
    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME (frame), GTK_SHADOW_NONE);
    gtk_container_set_border_width (GTK_CONTAINER (frame), FRAME_BORDER);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL (label),"<span weight=\"bold\">Position</span>");
    gtk_frame_set_label_widget(GTK_FRAME (frame), label);    

    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hbox2), 0);
    gtk_container_add (GTK_CONTAINER (frame), hbox2);
    
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_set_size_request(hbox, INDENT_SIZE, 1);
    gtk_box_pack_start(GTK_BOX (hbox2), hbox, FALSE, TRUE, 0);
    
    t = gtk_table_new(5, 2, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(t), 3);
    gtk_table_set_col_spacings(GTK_TABLE(t), 5);
    gtk_box_pack_start(GTK_BOX (hbox2), t, FALSE, TRUE, 0);
    
    //Edge
    label = gtk_label_new("Edge:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(t), label, 0, 1, 0, 1, GTK_FILL, 0, 0, 0);
    gtk_size_group_add_widget(sg, label);
    
    edge_opt = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(edge_opt), "Left");
    gtk_combo_box_append_text(GTK_COMBO_BOX(edge_opt), "Right");
    gtk_combo_box_append_text(GTK_COMBO_BOX(edge_opt), "Top");
    gtk_combo_box_append_text(GTK_COMBO_BOX(edge_opt), "Bottom");
    gtk_combo_box_set_active(GTK_COMBO_BOX(edge_opt), the_panel->edge - 1);
    g_signal_connect(G_OBJECT(edge_opt), "changed", G_CALLBACK(set_edge), NULL);
    
    gtk_widget_show(edge_opt);
  
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX (hbox), edge_opt, FALSE, TRUE, 0);
    gtk_table_attach(GTK_TABLE(t), hbox, 1, 2, 0, 1, GTK_FILL, 0, 0, 0);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), edge_opt);

    //Allignment
    label = gtk_label_new("Allignment:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_widget_show(label);
    gtk_size_group_add_widget(sg, label);
    
    gtk_table_attach(GTK_TABLE(t), label, 0, 1, 1, 2, GTK_FILL, 0, 0, 0);
        
    allign_opt = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(allign_opt), "Left");
    gtk_combo_box_append_text(GTK_COMBO_BOX(allign_opt), "Center");
    gtk_combo_box_append_text(GTK_COMBO_BOX(allign_opt), "Right");
    gtk_combo_box_set_active(GTK_COMBO_BOX(allign_opt), the_panel->allign - 1);
    g_signal_connect(G_OBJECT(allign_opt), "changed", G_CALLBACK(set_allign), NULL);
    gtk_widget_show(allign_opt);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX (hbox), allign_opt, FALSE, TRUE, 0);
    gtk_table_attach(GTK_TABLE(t), hbox, 1, 2, 1, 2, GTK_FILL, 0, 0, 0);                
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), allign_opt);


    //Margin
    margin_label = gtk_label_new("Margin:");
    gtk_misc_set_alignment(GTK_MISC(margin_label), 0.0, 0.5);
    gtk_widget_show(margin_label);

    gtk_table_attach(GTK_TABLE(t), margin_label, 2, 3, 1, 2, GTK_FILL, 0, 0, 0);

    margin_adj = (GtkAdjustment *) gtk_adjustment_new (0.0, 0.0, gdk_screen_width(), 1.0, 5.0, 5.0);
    margin_spinb = gtk_spin_button_new (margin_adj, 1.0, 0);
    g_signal_connect(margin_spinb, "value-changed", G_CALLBACK(set_margin), NULL);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(margin_spinb), the_panel->margin);
    gtk_table_attach(GTK_TABLE(t), margin_spinb, 3, 4, 1, 2, GTK_FILL, 0, 0, 0);
    gtk_table_set_col_spacing(GTK_TABLE(t), 1, 20);

    RET(frame);
}



GtkWidget *
mk_size()
{
    GtkWidget *hbox, *hbox2, *label, *frame;
    GtkWidget *t;
  
   
    ENTER;
    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME (frame), GTK_SHADOW_NONE);
    gtk_container_set_border_width (GTK_CONTAINER (frame), FRAME_BORDER);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL (label),"<span weight=\"bold\">Size</span>");
    gtk_frame_set_label_widget(GTK_FRAME (frame), label);    

    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hbox2), 0);
    gtk_container_add (GTK_CONTAINER (frame), hbox2);
    
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_set_size_request(hbox, INDENT_SIZE, 1);
    gtk_box_pack_start(GTK_BOX (hbox2), hbox, FALSE, TRUE, 0);
    
    t = gtk_table_new(3, 2, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(t), 3);
    gtk_table_set_col_spacings(GTK_TABLE(t), 5);
    gtk_box_pack_start(GTK_BOX (hbox2), t, FALSE, TRUE, 0);
    
    //width
    label = gtk_label_new("Width:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(t), label, 0, 1, 0, 1, GTK_FILL, 0, 0, 0);
    gtk_size_group_add_widget(sg, label);
    
    width_adj = (GtkAdjustment *) gtk_adjustment_new (20.0, 0.0, 2100.0, 1.0, 5.0, 5.0);
    gtk_adjustment_set_value(width_adj, the_panel->width);
    width_spinb = gtk_spin_button_new (width_adj, 1.0, 0);
    gtk_table_attach(GTK_TABLE(t), width_spinb, 1, 2, 0, 1, GTK_FILL, 0, 0, 0);
    g_signal_connect(width_spinb, "value-changed", G_CALLBACK(set_width), NULL);

    width_opt = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(width_opt), "dynamic");
    gtk_combo_box_append_text(GTK_COMBO_BOX(width_opt), "pixels");
    gtk_combo_box_append_text(GTK_COMBO_BOX(width_opt), "% of edge");
    gtk_combo_box_set_active(GTK_COMBO_BOX(width_opt), the_panel->widthtype - 1);
    g_signal_connect(G_OBJECT(width_opt), "changed", G_CALLBACK(set_width_type), NULL);
    gtk_widget_show(width_opt);

    
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX (hbox), width_opt, FALSE, TRUE, 0);
    gtk_table_attach(GTK_TABLE(t), hbox, 2, 3, 0, 1, GTK_FILL, 0, 0, 0);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), width_opt);


    //height
    label = gtk_label_new("Height:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_widget_show(label);
    gtk_table_attach(GTK_TABLE(t), label, 0, 1, 1, 2, GTK_FILL, 0, 0, 0);
    gtk_size_group_add_widget(sg, label);
    
    height_adj = (GtkAdjustment *) gtk_adjustment_new (30.0, 0.0, 300.0, 1.0, 5.0, 5.0);
    gtk_adjustment_set_value(height_adj, the_panel->height);
    height_spinb = gtk_spin_button_new (height_adj, 1.0, 0);
    gtk_table_attach(GTK_TABLE(t), height_spinb, 1, 2, 1, 2, GTK_FILL, 0, 0, 0);
    g_signal_connect( height_spinb, "value-changed", G_CALLBACK(set_height), NULL );

    
    height_opt = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(height_opt), "pixels");
    gtk_combo_box_set_active(GTK_COMBO_BOX(height_opt), the_panel->heighttype - 1);
    //g_signal_connect(G_OBJECT(height_opt), "changed", G_CALLBACK(set_height), NULL);
    gtk_widget_show(height_opt);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX (hbox), height_opt, FALSE, TRUE, 0);
    gtk_table_attach(GTK_TABLE(t), hbox, 2, 3, 1, 2, GTK_FILL, 0, 0, 0);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), height_opt);

    RET(frame);
}

static void
sensitive_toggle(GtkWidget *b, GtkWidget *w)
{
    gboolean t;

    ENTER;
    t = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b));
    gtk_widget_set_sensitive(w, t);
    RET();
}

GtkWidget *
mk_effects()
{
    GtkWidget *hbox, *label, *frame, *vbox;
   
    ENTER;
    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME (frame), GTK_SHADOW_NONE);
    gtk_container_set_border_width (GTK_CONTAINER (frame), FRAME_BORDER);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL (label),"<span weight=\"bold\">Effects</span>");
    gtk_frame_set_label_widget(GTK_FRAME (frame), label);    

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 0);
    gtk_container_add (GTK_CONTAINER (frame), hbox);
    
    add_hindent_box(hbox);
    
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX (hbox), vbox, FALSE, TRUE, 0);

    
    tr_checkb = gtk_check_button_new_with_label("Transparency");
    gtk_box_pack_start(GTK_BOX (vbox), tr_checkb, FALSE, FALSE, 0);

    tr_box = gtk_hbox_new(FALSE, 0);
    g_signal_connect(G_OBJECT(tr_checkb), "toggled", G_CALLBACK(sensitive_toggle), tr_box);
    update_toggle_button(tr_checkb, the_panel->transparent);
    g_signal_connect(G_OBJECT(tr_checkb), "toggled", G_CALLBACK(set_transparency), tr_box);
    gtk_box_pack_start(GTK_BOX (vbox), tr_box, FALSE, FALSE, 0);

    add_hindent_box(tr_box);
    
    label = gtk_label_new("Color settings:");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX (tr_box), label, FALSE, FALSE, 0);
    
    tr_colorb = gtk_color_button_new();
    gtk_color_button_set_color(GTK_COLOR_BUTTON(tr_colorb), &the_panel->gtintcolor);
    gtk_color_button_set_alpha (GTK_COLOR_BUTTON(tr_colorb), 256*the_panel->alpha);
    gtk_color_button_set_use_alpha(GTK_COLOR_BUTTON(tr_colorb), TRUE);
    g_signal_connect(G_OBJECT(tr_colorb), "color-set", G_CALLBACK(set_transparency_color), tr_box);
    gtk_box_pack_start(GTK_BOX (tr_box), tr_colorb, FALSE, FALSE, 5);
    //gtk_widget_set_sensitive(tr_colorb, FALSE);


    prop_round_corners_checkb = gtk_check_button_new_with_label("Round Corners");
    gtk_box_pack_start(GTK_BOX (vbox), prop_round_corners_checkb, FALSE, FALSE, 0);

    round_corners_box = gtk_hbox_new(FALSE, 0);

    g_signal_connect(G_OBJECT(prop_round_corners_checkb), "toggled", G_CALLBACK(sensitive_toggle),
          round_corners_box);
    update_toggle_button(prop_round_corners_checkb, the_panel->round_corners);
    g_signal_connect(G_OBJECT(prop_round_corners_checkb), "toggled", G_CALLBACK(set_round_corners), tr_box);
    gtk_box_pack_start(GTK_BOX (vbox), round_corners_box, FALSE, FALSE, 0);
    
    add_hindent_box(round_corners_box);
    
    label = gtk_label_new("Radius is ");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX (round_corners_box), label, FALSE, TRUE, 0);

    round_corners_adj = (GtkAdjustment *) gtk_adjustment_new (
        the_panel->round_corners_radius, 0, MIN(the_panel->cw, the_panel->ch)/2, 1.0, 1.0, 1.0);
    round_corners_spinb = gtk_spin_button_new (round_corners_adj, 1.0, 0);
    gtk_box_pack_start(GTK_BOX (round_corners_box), round_corners_spinb, FALSE, TRUE, 5);

    label = gtk_label_new("pixels");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX (round_corners_box), label, FALSE, TRUE, 0);



    
     
    RET(frame);
}


GtkWidget *
mk_properties()
{
    GtkWidget *vbox, *hbox, *label, *frame;
   
    ENTER;
    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME (frame), GTK_SHADOW_NONE);
    gtk_container_set_border_width (GTK_CONTAINER (frame), FRAME_BORDER);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL (label),"<span weight=\"bold\">Properties</span>");
    gtk_frame_set_label_widget(GTK_FRAME (frame), label);    

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (frame), hbox);

    add_hindent_box(hbox);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX (hbox), vbox, FALSE, TRUE, 0);
    
    prop_dt_checkb = gtk_check_button_new_with_label("Set Dock Type");
    update_toggle_button(prop_dt_checkb, the_panel->setdocktype);
    gtk_box_pack_start(GTK_BOX (vbox), prop_dt_checkb, FALSE, FALSE, 0);

    prop_st_checkb = gtk_check_button_new_with_label("Do not cover by maximized windows");
    update_toggle_button(prop_st_checkb, the_panel->setstrut);
    gtk_box_pack_start(GTK_BOX (vbox), prop_st_checkb, FALSE, FALSE, 0);

    prop_autohide_checkb = gtk_check_button_new_with_label("Autohide");
    gtk_box_pack_start(GTK_BOX (vbox), prop_autohide_checkb, FALSE, FALSE, 0);

    height_when_hidden_box = gtk_hbox_new(FALSE, 0);
    g_signal_connect(G_OBJECT(prop_autohide_checkb), "toggled", G_CALLBACK(sensitive_toggle),
          height_when_hidden_box);
     update_toggle_button(prop_autohide_checkb, the_panel->autohide);
    gtk_box_pack_start(GTK_BOX (vbox), height_when_hidden_box, FALSE, FALSE, 0);

    add_hindent_box(height_when_hidden_box);
    
    label = gtk_label_new("Height when hidden");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX (height_when_hidden_box), label, FALSE, TRUE, 0);

    height_when_hidden_adj = (GtkAdjustment *) gtk_adjustment_new (2.0, 1.0, 10.0, 1.0, 1.0, 1.0);
    gtk_adjustment_set_value(height_when_hidden_adj, the_panel->height_when_hidden);
    height_when_hidden_spinb = gtk_spin_button_new (height_when_hidden_adj, 1.0, 0);
    gtk_box_pack_start(GTK_BOX (height_when_hidden_box), height_when_hidden_spinb, FALSE, TRUE, 5);

    label = gtk_label_new("pixels");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX (height_when_hidden_box), label, FALSE, TRUE, 0);

    /* layer */
    prop_layer_checkb = gtk_check_button_new_with_label("Layer");
 
    gtk_box_pack_start(GTK_BOX (vbox), prop_layer_checkb, FALSE, FALSE, 0);
    
    layer_box = gtk_hbox_new(FALSE, 0);
    g_signal_connect(G_OBJECT(prop_layer_checkb), "toggled", G_CALLBACK(sensitive_toggle), layer_box);
    update_toggle_button(prop_layer_checkb, the_panel->layer);
    gtk_box_pack_start(GTK_BOX (vbox), layer_box, FALSE, FALSE, 0);

    add_hindent_box(layer_box);
    
    label = gtk_label_new("Panel is ");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX (layer_box), label, FALSE, TRUE, 0);

    layer_opt = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(layer_opt), "above");
    gtk_combo_box_append_text(GTK_COMBO_BOX(layer_opt), "below");
    gtk_box_pack_start(GTK_BOX (layer_box), layer_opt, FALSE, TRUE, 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(layer_opt),
          the_panel->layer ? the_panel->layer -1 : 0);
    
    label = gtk_label_new(" all windows");
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX (layer_box), label, FALSE, TRUE, 0);
    
    RET(frame);
}

static void
dialog_response_event(GtkDialog *widget, gint arg1, gpointer user_data)
{
    ENTER;
    switch (arg1) {
    case GTK_RESPONSE_DELETE_EVENT:
        DBG("GTK_RESPONSE_DELETE_EVENT\n");        
    case GTK_RESPONSE_CLOSE:
        DBG("GTK_RESPONSE_CLOSE\n");
    case GTK_RESPONSE_APPLY:
        DBG("GTK_RESPONSE_APPLY\n");
        save_config();
        gtk_widget_destroy(dialog);
        dialog = NULL;
        //gtk_main_quit();
        break;
    }
    RET();
}

static void
dialog_destroy_event(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    ENTER;  
    dialog = NULL;
    RET();
}

static gint
dialog_delete_event( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{

    ENTER;  
    //if (!the_panel->self_destroy)
    RET(FALSE);
}

enum {
    PLUGIN_TYPE, PLUGIN_NAME, PLUGIN_DESC, PLUGIN_PTR, PLUGIN_N_COLUMNS
};



static void
plug_moveup_plugin(GtkButton* btn, GtkTreeView* view)
{
    GList *el;
    GtkTreeIter it, prev;
    GtkTreeModel* model = gtk_tree_view_get_model(view);
    GtkTreeSelection* tree_sel = gtk_tree_view_get_selection(view);
    int i;
    plugin_instance* pl;
    
    ENTER;
    if (gtk_tree_selection_get_selected(tree_sel, &model, &it)) {
        gtk_tree_model_get (model, &it, PLUGIN_PTR, &pl, -1);

        for (i = 0, el = the_panel->plugins;
             el && (el->data != pl);
             el = el->next, i++);
        if (!el->prev)
            RET();
        el->data = el->prev->data;
        el->prev->data = pl;
        if (!gtk_tree_model_iter_nth_child(model, &prev, NULL, i-1)) {
            ERR("BUG: prev elem not found\n");
            RET();
        }
        gtk_list_store_move_before(GTK_LIST_STORE(model), &it, &prev);
        DBG("box=%p pwid=%p ind=%d\n",  the_panel->box, pl->pwid, i-1);
        gtk_box_reorder_child(GTK_BOX(the_panel->box), pl->pwid, i-1);
    }
    RET();
}    


static void
plug_movedown_plugin(GtkButton* btn, GtkTreeView* view)
{
    GList *el;
    GtkTreeIter it, prev;
    GtkTreeModel* model = gtk_tree_view_get_model(view);
    GtkTreeSelection* tree_sel = gtk_tree_view_get_selection(view);
    int i;
    plugin_instance* pl;
    
    ENTER;
    if (gtk_tree_selection_get_selected(tree_sel, &model, &it)) {
        gtk_tree_model_get (model, &it, PLUGIN_PTR, &pl, -1);

        for (i = 0, el = the_panel->plugins;
             el && (el->data != pl);
             el = el->next, i++);
        if (!el->next)
            RET();
        el->data = el->next->data;
        el->next->data = pl;
        if (!gtk_tree_model_iter_nth_child(model, &prev, NULL, i+1)) {
            ERR("BUG: next elem not found\n");
            RET();
        }
        gtk_list_store_move_after(GTK_LIST_STORE(model), &it, &prev);
        DBG("box=%p pwid=%p ind=%d\n",  the_panel->box, pl->pwid, i+1);
        gtk_box_reorder_child(GTK_BOX(the_panel->box), pl->pwid, i+1);
    }
    RET();
}    



static void
plug_remove_plugin(GtkButton* btn, GtkTreeView* view)
{   
    GtkTreeIter it;
    GtkTreeModel* model;
    GtkTreeSelection* tree_sel = gtk_tree_view_get_selection(view);
    plugin_instance* pl;

    ENTER;
    if (gtk_tree_selection_get_selected(tree_sel, &model, &it)) {
        gtk_tree_model_get(model, &it, PLUGIN_PTR, &pl, -1);
        plugin_stop(pl); /* free the plugin_instance widget & its data */
        plugin_put(pl); /* free the lib if necessary */
        the_panel->plugins = g_list_remove(the_panel->plugins, pl);
        gtk_list_store_remove( GTK_LIST_STORE(model), &it);
    }
    RET();
}




static void
tree_selection_changed_cb (GtkTreeSelection *selection, GtkWidget *vbox)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    plugin_instance* pl;
    GtkWidget *edit;
    
    ENTER;
    if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
        
        gtk_widget_set_sensitive(vbox, TRUE);
        gtk_tree_model_get (model, &iter, PLUGIN_PTR, &pl, -1);
        DBG("pl=%s\n", pl->class->name);
        edit = g_object_get_data(G_OBJECT(vbox), "edit");
        gtk_widget_set_sensitive(edit, pl->class->edit_config != NULL);
    } else {
        gtk_widget_set_sensitive(vbox, FALSE);
    }
    RET();
}

static void
plug_populate_apl()
{
    gchar *path, *file, *type, *tmp;
    GDir* dir;

    ENTER;
    apl = gtk_list_store_new(PLUGIN_N_COLUMNS,
          G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
    path = g_build_filename(PREFIX, "share/fbpanel/plugins", NULL);
    DBG("path: %s\n", path);
    dir = g_dir_open(path, 0, NULL);
    if (!dir)
        RET();
    while ((file = (gchar *)g_dir_read_name(dir))) {
        type = g_strdup(file);
        if ((tmp = strstr(type, "."))) {
            GModule *m;
            
            file = g_build_filename(path, file, NULL);
            DBG("file: %s\n", file);
            m = g_module_open(file, G_MODULE_BIND_LAZY);
            if (m) {
                gpointer tmpsym;
                *tmp = 0;    
                tmp = g_strdup_printf("%s_plugin_class", type);
                DBG("class: %s\n", tmp);
                if (g_module_symbol(m, tmp, &tmpsym)) {
                    plugin_class *pc = tmpsym;
                    GtkTreeIter it;
                    
                    DBG("name:%s desc:%s\n", pc->name, pc->description);
                    gtk_list_store_append(apl, &it);
                    gtk_list_store_set(apl, &it,
                          PLUGIN_TYPE, pc->type,
                          PLUGIN_NAME, pc->name,
                          PLUGIN_DESC, pc->description,
                          -1);
                }
                g_module_close(m);
                g_free(tmp);
            }
            g_free(file);
        }
        g_free(type);
    }    
    g_dir_close(dir);
    g_free(path);
    RET();
}
  
static void
plug_add_plugin_response(GtkDialog *dlg, int response, GtkTreeView* _view)
{
    GtkTreeView* view;
    GtkTreeSelection* tree_sel;
    GtkTreeIter it;
    GtkTreeModel* model;
    char* type = NULL;
    plugin_instance* pl;

    ENTER;
    if (response != GTK_RESPONSE_OK)
        goto close;
    view = g_object_get_data(G_OBJECT(dlg), "avail-plugins" );
    tree_sel = gtk_tree_view_get_selection(view);
    if (!gtk_tree_selection_get_selected(tree_sel, &model, &it))
        goto close;

    gtk_tree_model_get(model, &it,
          PLUGIN_TYPE, &type, -1);
    if ((pl = plugin_load(type))) {
        GtkTreePath* tree_path;

        pl->panel = the_panel;
        pl->fp = make_empty_fp();
        plugin_start(pl);
        the_panel->plugins = g_list_append(the_panel->plugins, pl);
        /* FIXME: will show all cause problems? */
        //gtk_widget_show_all(pl->pwid);

        model = gtk_tree_view_get_model( _view );
        gtk_list_store_append(GTK_LIST_STORE(model), &it);
        gtk_list_store_set(GTK_LIST_STORE(model), &it,
              PLUGIN_TYPE, pl->class->type,
              PLUGIN_NAME, pl->class->name,
              PLUGIN_DESC, pl->class->description,
              PLUGIN_PTR, pl, 
              -1);
        tree_sel = gtk_tree_view_get_selection( _view );
        gtk_tree_selection_select_iter( tree_sel, &it );
        if ((tree_path = gtk_tree_model_get_path( model, &it))) {
            gtk_tree_view_scroll_to_cell( _view, tree_path, NULL, FALSE, 0, 0 );
            gtk_tree_path_free( tree_path );
        }
    }
    g_free( type );
    
close:
    gtk_widget_destroy(GTK_WIDGET(dlg));
    RET();
}

static void
plug_edit_plugin(GtkButton* btn, GtkTreeView* view)
{
    GtkWidget *dlg, *w;
    GtkTreeIter it;
    GtkTreeModel* model;
    GtkTreeSelection* tree_sel = gtk_tree_view_get_selection(view);
    plugin_instance* pl;
    gchar name[60];
    
    ENTER;
    if (!gtk_tree_selection_get_selected(tree_sel, &model, &it))
        RET();
    gtk_tree_model_get(model, &it, PLUGIN_PTR, &pl, -1);
    DBG("Edit %s\n", pl->class->type);
    g_snprintf(name, 60, "%s settings", pl->class->name);
    dlg = gtk_dialog_new_with_buttons(name, 
          GTK_WINDOW(dialog),
          GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
          GTK_STOCK_CLOSE,
          GTK_RESPONSE_CLOSE,
          NULL );
    //gtk_window_set_default_size(GTK_WINDOW(dlg), 300, 500);
    w = pl->class->edit_config ? pl->class->edit_config(pl)
        : default_plugin_edit_config(pl);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox), w, TRUE, TRUE, 4);
    g_signal_connect(G_OBJECT(dlg), "response",  (GCallback) gtk_widget_destroy, NULL);
    
    gtk_widget_show_all(dlg);
    RET();
}
       
static void
plug_add_plugin(GtkButton* btn, GtkTreeView* _view )
{
    GtkWidget* dlg, *scroll, *view;
    GtkTreeViewColumn* col;
    GtkCellRenderer* render;
    GtkTreeSelection* tree_sel;

    if (!apl)
        plug_populate_apl();

    dlg = gtk_dialog_new_with_buttons("Add plugin to panel",
          GTK_WINDOW(dialog),
          GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
          GTK_STOCK_CANCEL,
          GTK_RESPONSE_CANCEL,
          GTK_STOCK_ADD,
          GTK_RESPONSE_OK, NULL );
    /* gtk_widget_set_sensitive( parent_win, FALSE ); */
    scroll = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_IN);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
          GTK_POLICY_AUTOMATIC,
          GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->vbox), scroll, TRUE, TRUE, 4);
    view = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scroll), view);
    tree_sel = gtk_tree_view_get_selection(GTK_TREE_VIEW (view));
    gtk_tree_selection_set_mode(tree_sel, GTK_SELECTION_BROWSE);

    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(
        "Name",
        render, "text",
        PLUGIN_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
    
    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(
        "Description",
        render, "text",
        PLUGIN_DESC, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
    if (1) {
        GtkTreeModel *sort_model;
        sort_model = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(apl));
        gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (sort_model),
              PLUGIN_NAME, GTK_SORT_ASCENDING);
        gtk_tree_view_set_model(GTK_TREE_VIEW(view), sort_model);
    }
    g_signal_connect(G_OBJECT(dlg), "response",  (GCallback) plug_add_plugin_response, _view );
    g_object_set_data(G_OBJECT(dlg), "avail-plugins", view );
    
    gtk_window_set_default_size(GTK_WINDOW(dlg), 380, 470);
    gtk_widget_show_all(dlg);
}


static GtkWidget *
mk_tab_plugins_new()
{
   
    GtkWidget *view, *scroll, *hbox, *vbox, *vbox2, *button;
    GtkListStore *store;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkTreeSelection *select;
    GList* l;

    ENTER;
    hbox = gtk_hbox_new(FALSE, 0);

    // 1. scrolled window with plugin list
    store = gtk_list_store_new(PLUGIN_N_COLUMNS,
          G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
    for (l = the_panel->plugins; l; l = l->next) {
        GtkTreeIter iter;
        plugin_instance* pl = (plugin_instance*)l->data;

        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter,
              PLUGIN_TYPE, pl->class->type,
              PLUGIN_NAME, pl->class->name,
              PLUGIN_DESC, pl->class->description,
              PLUGIN_PTR, pl, 
              -1);
    }
    view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    select = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
    gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
 

#if 0
    // type
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes ("Type",
          renderer,
          "text", PLUGIN_TYPE,
          NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
#endif
    // name
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes ("Name",
          renderer,
          "text", PLUGIN_NAME,
          NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
    // description
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes ("Description",
          renderer,
          "text", PLUGIN_DESC,
          NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll),
          GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_IN );
    gtk_container_add(GTK_CONTAINER(scroll), view );
    gtk_box_pack_start(GTK_BOX(hbox), scroll, TRUE, TRUE, 0);
    
    // 2. buttons to add, del, edit, move up and down
    vbox = gtk_vbox_new(FALSE, 6);
    // space
    button = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 14);
    // add
    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button),
          gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_BUTTON));
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    g_signal_connect( button, "clicked", G_CALLBACK(plug_add_plugin), view);
    // space
    button = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), button, TRUE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, TRUE, 13);

    vbox2 = gtk_vbox_new(FALSE, 6);
    gtk_box_pack_start(GTK_BOX(vbox), vbox2, FALSE, TRUE, 3);
    // edit
    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button),
          gtk_image_new_from_stock(GTK_STOCK_EDIT, GTK_ICON_SIZE_BUTTON));
    gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(plug_edit_plugin), view);
    g_object_set_data(G_OBJECT(vbox2), "edit", button);
    // remove
    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button),
          gtk_image_new_from_stock(GTK_STOCK_REMOVE, GTK_ICON_SIZE_BUTTON));
    gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);
    g_signal_connect( button, "clicked", G_CALLBACK(plug_remove_plugin), view);
    // up
    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button),
          gtk_image_new_from_stock(GTK_STOCK_GO_UP, GTK_ICON_SIZE_BUTTON));
    gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);
    g_signal_connect( button, "clicked", G_CALLBACK(plug_moveup_plugin), view);
    // down
    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button),
          gtk_image_new_from_stock(GTK_STOCK_GO_DOWN, GTK_ICON_SIZE_BUTTON));
    gtk_box_pack_start(GTK_BOX(vbox2), button, FALSE, FALSE, 0);
    g_signal_connect( button, "clicked", G_CALLBACK(plug_movedown_plugin), view);
    // plugin selection
    gtk_widget_set_sensitive(vbox2, FALSE);
    g_signal_connect (G_OBJECT (select), "changed",
          G_CALLBACK (tree_selection_changed_cb), vbox2);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 4);
    RET(hbox);
}

#if 0
static GtkWidget *
mk_tab_plugins()
{
    GtkWidget *hbox, *label;
    gchar *msg;

    ENTER;
    hbox = gtk_vbox_new(FALSE, 0);
    msg = g_strdup_printf("Graphical plugin_instance configuration is not implemented yet.\n"
          "Please edit manually\n\t~/.fbpanel/%s\n\n"
          "You can use as example files in \n\t%s/share/fbpanel/\n"
          "or visit\n"
          "\thttp://fbpanel.sourceforge.net/docs.html", cprofile, PREFIX);
    label = gtk_label_new(msg);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    gtk_box_pack_end(GTK_BOX(hbox), label, TRUE, TRUE, 5);
    g_free(msg);
    
    RET(hbox);
}
#endif


static GtkWidget *
mk_tab_general()
{
    GtkWidget *frame, *page;

    ENTER;
    /*
    sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw), GTK_SHADOW_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(sw), 0);
    */
    page = gtk_vbox_new(FALSE, 1);
        
    sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
    
    //position 
    frame = mk_position();
    DBG("here\n");
    gtk_box_pack_start(GTK_BOX (page), frame, FALSE, TRUE, 0);
    
    //size 
    frame = mk_size();
    gtk_box_pack_start(GTK_BOX (page), frame, FALSE, TRUE, 0);
    
    //effects 
    frame = mk_effects();
    gtk_box_pack_start(GTK_BOX (page), frame, FALSE, TRUE, 0);
    
    //properties 
    frame = mk_properties();
    gtk_box_pack_start(GTK_BOX (page), frame, FALSE, TRUE, 0);
    /*
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW (sw), page);
    */
    RET(page);
}


static GtkWidget *
mk_dialog()
{
    GtkWidget *sw, *nb, *label;
    gchar *name;
    

    ENTER;
    DBG("creating dialog\n");
    name = g_strdup_printf("fbpanel settings: <%s> profile", cprofile);  
    dialog = gtk_dialog_new_with_buttons (name,
          NULL,
          GTK_DIALOG_NO_SEPARATOR, //GTK_DIALOG_DESTROY_WITH_PARENT,
          GTK_STOCK_CLOSE,
          GTK_RESPONSE_CLOSE,
          NULL);
    g_free(name);
    DBG("connecting sugnal to %p\n",  dialog);
    g_signal_connect (G_OBJECT(dialog), "response",     (GCallback) dialog_response_event, NULL);
    g_signal_connect (G_OBJECT(dialog), "destroy",      (GCallback) dialog_destroy_event, NULL);
    g_signal_connect (G_OBJECT(dialog), "delete_event", (GCallback) dialog_delete_event,  NULL);
    gtk_window_set_modal(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 500);
    gtk_window_set_icon_from_file(GTK_WINDOW(dialog), IMGPREFIX "/star.png", NULL);
      
    nb = gtk_notebook_new();
    gtk_notebook_set_show_border (GTK_NOTEBOOK(nb), FALSE);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), nb);

    sw = mk_tab_general();
    label = gtk_label_new("General");
    gtk_misc_set_padding(GTK_MISC(label), 4, 1);
    gtk_notebook_append_page(GTK_NOTEBOOK(nb), sw, label);

    sw = mk_tab_plugins_new();
    label = gtk_label_new("Plugins");
    gtk_misc_set_padding(GTK_MISC(label), 4, 1);
    gtk_notebook_append_page(GTK_NOTEBOOK(nb), sw, label);
    
    g_object_unref(sg);
    
    //gtk_widget_show_all(page);
    gtk_widget_show_all(dialog);
    
    RET(dialog);
}



void
configure(void)
{
    ENTER;
    DBG("dialog %p\n",  dialog);
    if (!dialog) 
        dialog = mk_dialog();
    gtk_widget_show(dialog);

    ENTER;
#if 0    
    update_opt_menu(edge_opt, the_panel->edge - 1);
    update_opt_menu(allign_opt, the_panel->allign - 1);
    //gtk_adjustment_set_value(margin_adj, the_panel->margin);
    //gtk_adjustment_value_changed(margin_adj);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(margin_spinb), the_panel->margin);
    
    update_opt_menu(width_opt, the_panel->widthtype - 1);
    gtk_adjustment_set_value(width_adj, the_panel->width);
    update_opt_menu(height_opt, HEIGHT_PIXEL - 1);
    gtk_adjustment_set_value(height_adj, the_panel->height);

    update_toggle_button(tr_checkb, the_panel->transparent);
    gtk_color_button_set_color(GTK_COLOR_BUTTON(tr_colorb), &the_panel->gtintcolor);
    gtk_color_button_set_alpha (GTK_COLOR_BUTTON(tr_colorb), 256*the_panel->alpha);
    gtk_widget_show(dialog);

    update_toggle_button(prop_dt_checkb, the_panel->setdocktype);
    update_toggle_button(prop_st_checkb, the_panel->setstrut);
    update_toggle_button(prop_autohide_checkb, the_panel->autohide);
    gtk_adjustment_set_value(height_when_hidden_adj, the_panel->height_when_hidden);
    update_toggle_button(prop_round_corners_checkb, the_panel->round_corners);
    gtk_adjustment_set_value(round_corners_adj, the_panel->round_corners_radius);
    update_toggle_button(prop_layer_checkb, the_panel->layer);
    update_opt_menu(layer_opt, MAX(the_panel->layer - 1, 0));
#endif   
    RET();
}

void
global_config_save(FILE *fp)
{
    GdkColor c;
    int layer;
    
    fprintf(fp, "# fbpanel <profile> config file\n");
    fprintf(fp, "# see http://fbpanel.sf.net/docs.html for complete configuration guide\n");
    fprintf(fp, "\n\n");
    fprintf(fp, "Global {\n");
    fprintf(fp, "    edge = %s\n",
          num2str(edge_pair, gtk_combo_box_get_active(GTK_COMBO_BOX(edge_opt)) + 1, "none"));
    fprintf(fp, "    allign = %s\n",
          num2str(allign_pair, gtk_combo_box_get_active(GTK_COMBO_BOX(allign_opt)) + 1, "none"));
    fprintf(fp, "    margin = %d\n", (int) margin_adj->value);
    fprintf(fp, "    widthtype = %s\n",
          num2str(width_pair, gtk_combo_box_get_active(GTK_COMBO_BOX(width_opt)) + 1, "none"));
    fprintf(fp, "    width = %d\n", (int) width_adj->value);
    fprintf(fp, "    height = %d\n", (int) height_adj->value);
    fprintf(fp, "    transparent = %s\n",
          num2str(bool_pair, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tr_checkb)), "false"));
    gtk_color_button_get_color(GTK_COLOR_BUTTON(tr_colorb), &c);
    fprintf(fp, "    tintcolor = #%06x\n", gcolor2rgb24(&c));
    fprintf(fp, "    alpha = %d\n", gtk_color_button_get_alpha(GTK_COLOR_BUTTON(tr_colorb)) * 0xff / 0xffff);
    fprintf(fp, "    setdocktype = %s\n",
          num2str(bool_pair, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prop_dt_checkb)), "true"));
    fprintf(fp, "    setpartialstrut = %s\n",
          num2str(bool_pair, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prop_st_checkb)), "true"));
    fprintf(fp, "    autohide = %s\n",
          num2str(bool_pair, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prop_autohide_checkb)), "false"));
    fprintf(fp, "    heightWhenHidden = %d\n", (int) height_when_hidden_adj->value);
    fprintf(fp, "    roundcorners = %s\n",
          num2str(bool_pair, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prop_round_corners_checkb)), "false"));
    fprintf(fp, "    roundcornersradius = %d\n", (int) round_corners_adj->value);    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prop_layer_checkb))) 
        layer = gtk_combo_box_get_active(GTK_COMBO_BOX(layer_opt)) + 1;
    else
        layer = 0;
    fprintf(fp, "    layer = %s\n", num2str(layer_pair, layer, "None"));
                

    fprintf(fp, "}\n\n");
    RET();
}


static void
default_plugin_save_config(plugin_instance *pl, FILE *fp)
{
    gchar str[PATH_MAX];
    
    ENTER;
    fseek(pl->fp, 0, SEEK_SET);
    while (fgets(str, PATH_MAX, pl->fp))
        fprintf(fp, "%s", str);
    RET();
}

void
plugin_config_save_all(FILE *fp)
{
    GList *el;
    plugin_instance *pl;
    
    ENTER;
    for (el = the_panel->plugins; el; el = el->next) {
        pl = el->data;
        fprintf(fp, "plugin {\n");
        fprintf(fp, "    type = %s\n", pl->class->type);
        fprintf(fp, "    expand = %s\n", num2str(bool_pair, pl->expand, "false"));
        fprintf(fp, "    padding = %d\n", pl->padding);
        fprintf(fp, "    config {\n");
        if (pl->class->save_config)
            pl->class->save_config(pl, fp);
        else
            default_plugin_save_config(pl, fp);
        /* config's closing bracket '}' must be added by plugin */
        //fprintf(fp, "    }\n");
        fprintf(fp, "}\n\n");
    }
    RET();
}

void
restart(void)
{
    ENTER;
    gtk_main_quit();
    RET();
}


void
quit(void)
{
    ENTER;
    gtk_main_quit();
    force_quit = 1;
    RET();
}


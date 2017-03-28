#include <gtk/gtk.h>
#include <stdio.h>

gboolean delete_event(gpointer object, GdkEventAny *event, gpointer data);

gboolean destroy_event(gpointer object, GdkEventAny *event, gpointer data);

gboolean exit_ok(gpointer object, gpointer data);

gboolean exit_cancel(gpointer object, gpointer data);

gboolean exit_keypress(gpointer object, GdkEventKey *event, gpointer data);

gboolean exit_help(gpointer object, gpointer data);

gboolean exit_previous(gpointer object, gpointer data);

gboolean checked(GtkObject *button, gpointer data);

gboolean timeout_exit(gpointer data);

gboolean infobox_timeout_exit(gpointer data);

gboolean infobox_timeout(gpointer data);

gboolean gauge_timeout(gpointer data);

gboolean progress_timeout(gpointer data);

gboolean tailbox_timeout(gpointer data);

gboolean tailbox_keypress(GtkObject *text, GdkEventKey *event, gpointer data);

gboolean logbox_timeout(gpointer data);

gboolean inputbox_ok(gpointer object, gpointer data);

gboolean input_keypress(GtkObject *entry, GdkEventKey *event, gpointer data);

gboolean inputbox_timeout(gpointer data);

gboolean hide_passwords(GtkObject *button, gpointer data);

gboolean editbox_ok(gpointer object, gpointer data);

gboolean print_text(gpointer object, gpointer data);

gboolean rangebox_exit(GtkButton *button, gpointer data);

gboolean rangebox_timeout(gpointer data);

gboolean spinbox_exit(GtkButton *button, gpointer data);

gboolean spinbox_timeout(gpointer data);

gint double_click_event(GtkObject *object, GdkEventButton *event, gpointer data);

void item_toggle(GtkObject *item, int i);

gboolean print_items(GtkButton *button, gpointer data);

gboolean itemlist_timeout(gpointer data);

void item_select(GtkObject *clist, gint row, gint column, GdkEventButton *event,
		 gpointer data);

gboolean print_selection(GtkButton *button, gpointer data);

#ifdef USE_GTK2
gboolean print_tree_selection(GtkButton *button, gpointer data);
#endif

gboolean menu_timeout(gpointer data);

gboolean move_to_row_timeout(gpointer data);

#if 0
gboolean select_timeout(gpointer data);
#endif

void cb_selection_changed(GtkObject *tree);

void sensitive_buttons(void);

gboolean add_to_list(GtkButton *button, gpointer data);

gboolean remove_from_list(GtkButton *button, gpointer data);

gboolean print_list(GtkButton *button, gpointer data);

gboolean buildlist_timeout(gpointer data);

gboolean filesel_exit(GtkObject *filesel, gpointer client_data);

gboolean colorsel_exit(GtkObject *colorsel, gpointer client_data);

gboolean fontsel_exit(GtkObject *filesel, gpointer client_data);

gboolean calendar_exit(gpointer object, gpointer data);

gboolean calendar_timeout(gpointer data);

gboolean timebox_exit(gpointer object, gpointer data);

gboolean timebox_timeout(gpointer data);

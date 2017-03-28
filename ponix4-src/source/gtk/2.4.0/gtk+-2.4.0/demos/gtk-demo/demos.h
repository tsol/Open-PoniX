typedef	GtkWidget *(*GDoDemoFunc) (GtkWidget *do_widget);

typedef struct _Demo Demo;

struct _Demo 
{
  gchar *title;
  gchar *filename;
  GDoDemoFunc func;
  Demo *children;
};

GtkWidget *do_appwindow (GtkWidget *do_widget);
GtkWidget *do_button_box (GtkWidget *do_widget);
GtkWidget *do_changedisplay (GtkWidget *do_widget);
GtkWidget *do_colorsel (GtkWidget *do_widget);
GtkWidget *do_dialog (GtkWidget *do_widget);
GtkWidget *do_drawingarea (GtkWidget *do_widget);
GtkWidget *do_editable_cells (GtkWidget *do_widget);
GtkWidget *do_entry_completion (GtkWidget *do_widget);
GtkWidget *do_hypertext (GtkWidget *do_widget);
GtkWidget *do_images (GtkWidget *do_widget);
GtkWidget *do_list_store (GtkWidget *do_widget);
GtkWidget *do_menus (GtkWidget *do_widget);
GtkWidget *do_panes (GtkWidget *do_widget);
GtkWidget *do_pixbufs (GtkWidget *do_widget);
GtkWidget *do_sizegroup (GtkWidget *do_widget);
GtkWidget *do_stock_browser (GtkWidget *do_widget);
GtkWidget *do_textview (GtkWidget *do_widget);
GtkWidget *do_tree_store (GtkWidget *do_widget);
GtkWidget *do_ui_manager (GtkWidget *do_widget);

Demo child0[] = {
  { "Editable Cells", "editable_cells.c", do_editable_cells, NULL },
  { "List Store", "list_store.c", do_list_store, NULL },
  { "Tree Store", "tree_store.c", do_tree_store, NULL },
  { NULL } 
};

Demo child1[] = {
  { "Hypertext", "hypertext.c", do_hypertext, NULL },
  { "Multiple Views", "textview.c", do_textview, NULL },
  { NULL } 
};

Demo testgtk_demos[] = {
  { "Application main window", "appwindow.c", do_appwindow, NULL }, 
  { "Button Boxes", "button_box.c", do_button_box, NULL }, 
  { "Change Display", "changedisplay.c", do_changedisplay, NULL }, 
  { "Color Selector", "colorsel.c", do_colorsel, NULL }, 
  { "Dialog and Message Boxes", "dialog.c", do_dialog, NULL }, 
  { "Drawing Area", "drawingarea.c", do_drawingarea, NULL }, 
  { "Entry Completion", "entry_completion.c", do_entry_completion, NULL }, 
  { "Images", "images.c", do_images, NULL }, 
  { "Menus", "menus.c", do_menus, NULL }, 
  { "Paned Widgets", "panes.c", do_panes, NULL }, 
  { "Pixbufs", "pixbufs.c", do_pixbufs, NULL }, 
  { "Size Groups", "sizegroup.c", do_sizegroup, NULL }, 
  { "Stock Item and Icon Browser", "stock_browser.c", do_stock_browser, NULL }, 
  { "Text Widget", NULL, NULL, child1 }, 
  { "Tree View", NULL, NULL, child0 }, 
  { "UI Manager", "ui_manager.c", do_ui_manager, NULL },
  { NULL } 
};

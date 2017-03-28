/*
 *  plymout-log-viewer.c - Display boot messages
 *
 *  Copyright (C) 2008 Red Hat, Inc.
 *
 *  Author: Matthias Clasen
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <locale.h>

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#ifndef GETTEXT_PACKAGE
#define GETTEXT_PACKAGE "bootmessages"
#endif

#ifndef DEFAULT_LOG
#define DEFAULT_LOG "/var/log/boot.log"
#endif

#ifndef DEFAULT_SPOOL_FILE
#define DEFAULT_SPOOL_FILE "/var/spool/plymouth/boot.log"
#endif

static gboolean show_icon = FALSE;
static gboolean force = FALSE;

static GOptionEntry entries[] = 
{
  { "icon", 0, 0, G_OPTION_ARG_NONE, &show_icon, N_("Show a status icon if there are errors"), NULL },
  { "force", 0, 0, G_OPTION_ARG_NONE, &force, N_("Show the icon even without errors"), NULL },
  { NULL }
};

static void
popup_menu (GtkStatusIcon *icon, 
            guint          button,
            guint          activate_time,
            gpointer       user_data)
{
  GtkWidget *menu = user_data;
  
  gtk_menu_popup (GTK_MENU (menu), NULL, NULL, 
                  gtk_status_icon_position_menu, icon, 
                  button, activate_time);
}

static void
activate_icon (GtkStatusIcon *icon, 
               gpointer       user_data)
{
  GtkWidget *window = user_data;

  gtk_window_present (GTK_WINDOW (window));
}

static gboolean
check_for_errors (const char *file)
{
  struct stat log_info, spool_file_info;

  if (stat (file, &log_info) < 0)
    return FALSE;

  if (stat (DEFAULT_SPOOL_FILE, &spool_file_info) < 0)
    return FALSE;

  return spool_file_info.st_dev == log_info.st_dev &&
         spool_file_info.st_ino == log_info.st_ino;
}

static GtkTextBuffer *
read_boot_log (const char  *file, 
               int         *seen_errors, 
               GError     **error)
{
  char *content;
  char *content_utf8;
  gsize length;
  char *p, *q;
  GtkTextBuffer *buffer;
  GtkTextIter iter;
  GString *partial;

  if (!g_file_get_contents (file, &content, &length, error))
    return NULL;

  content_utf8 = g_locale_to_utf8 (content, length, NULL, NULL, NULL);
  if (content_utf8)
    {
      g_free (content);
      content = content_utf8;   
    }
  
  if (check_for_errors (file))
    *seen_errors = 2;
  else
    *seen_errors = 0;

  buffer = gtk_text_buffer_new (NULL);
  gtk_text_buffer_create_tag (buffer, "blue", "foreground", "blue", NULL);
  gtk_text_buffer_create_tag (buffer, "green", "foreground", "green", NULL);
  gtk_text_buffer_create_tag (buffer, "red", "foreground", "red", NULL);
  gtk_text_buffer_create_tag (buffer, "yellow", "foreground", "yellow", NULL);

  partial = g_string_new ("");

  p = content;
  while (*p) 
    {
      switch (*p) 
        {
          case '\r': 
            /* keep isolated \r */
            if (p[1] != '\r' && p[-1] != '\r' &&
                p[1] != '\n' && p[-1] != '\n')
              {
                gtk_text_buffer_get_end_iter (buffer, &iter);
                gtk_text_buffer_insert (buffer, &iter, p, 1);
              }
            p++;
            break;
          case '\t':
            gtk_text_buffer_get_end_iter (buffer, &iter);
            gtk_text_buffer_insert (buffer, &iter, "        ", 8); 
            p++;
            break;
          case '\033':
            if (strncmp (p, "\033[0;34m", 7) == 0 && (q = strstr (p, "\033[0;39m")))
              {
                p += 7;
                gtk_text_buffer_get_end_iter (buffer, &iter);
                gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, p, q - p, "blue", NULL); 
                p = q + 7; 
              }
            else if (strncmp (p, "\033[60G", 5) == 0) 
              {
                gtk_text_buffer_get_end_iter (buffer, &iter);
                gtk_text_buffer_insert (buffer, &iter, "\t", 1);
                p += 5;
              }
            else if (strncmp (p, "\033[0;31m", 7) == 0 && (q = strstr (p, "\033[0;39m")))
              {
                p += 7;
                gtk_text_buffer_get_end_iter (buffer, &iter);
                gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, p, q - p, "red", NULL); 
                p = q + 7; 
              }
            else if (strncmp (p, "\033[0;32m", 7) == 0 && (q = strstr (p, "\033[0;39m")))
              {
                p += 7;
                gtk_text_buffer_get_end_iter (buffer, &iter);
                gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, p, q - p, "green", NULL); 
                p = q + 7; 
              }
            else if (strncmp (p, "\033[0;33m", 7) == 0 && (q = strstr (p, "\033[0;39m")))
              {
                p += 7;
                gtk_text_buffer_get_end_iter (buffer, &iter);
                gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, p, q - p, "yellow", NULL); 
                p = q + 7; 
              }
            else if (strncmp (p, "\033%G", 3) == 0) 
              p += 3;
            else
              p++;
            break;
          default:
            /* GtkTextBuffer doesn't let us insert partial utf-8 characters */
            g_string_append_c (partial, *p);
            if (g_utf8_get_char_validated (partial->str, partial->len) != (gunichar)-2)
              {
                gtk_text_buffer_get_end_iter (buffer, &iter);
                gtk_text_buffer_insert (buffer, &iter, partial->str, partial->len);
                g_string_truncate (partial, 0);
              }
            p++;
            break;
        }
    }

  g_string_free (partial, TRUE);
  g_free (content);

  return buffer;
}

static void
close_window (GtkWidget *window)
{
  if (show_icon)
    gtk_widget_hide (window);
  else
    gtk_main_quit ();
}

static GtkWidget *
create_window (GtkTextBuffer *buffer)
{
  GtkWidget *window;
  GtkWidget *scrolledwin;
  GtkWidget *box;
  GtkWidget *terminal;
  GtkWidget *bbox;
  GtkWidget *close_button;
  PangoFontDescription *description;
  PangoTabArray *tabs;
  int width, height;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  gtk_window_set_title (GTK_WINDOW (window), _("Boot messages"));
  gtk_container_set_border_width (GTK_CONTAINER (window), 12);

  width = MIN (800, 0.75 * gdk_screen_get_width (gdk_screen_get_default ()));
  height = MIN (600, 0.75 * gdk_screen_get_height (gdk_screen_get_default ()));
  gtk_window_set_default_size (GTK_WINDOW (window), width, height);

  box = gtk_vbox_new (FALSE, 0);
  scrolledwin = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwin), 
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwin), 
                                       GTK_SHADOW_IN);
  terminal = gtk_text_view_new_with_buffer (buffer);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (terminal), FALSE);

  description = pango_font_description_from_string ("monospace");
  gtk_widget_modify_font (terminal, description);
  pango_font_description_free (description);

  tabs = pango_tab_array_new_with_positions (1, TRUE, PANGO_TAB_LEFT, width - 130);
  gtk_text_view_set_tabs (GTK_TEXT_VIEW (terminal), tabs);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (terminal), 12);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (terminal), 12);

  bbox = gtk_hbutton_box_new ();
  gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_END);
  close_button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);

  gtk_container_add (GTK_CONTAINER (window), box);
  gtk_box_pack_start (GTK_BOX (box), scrolledwin, TRUE, TRUE, 6);
  gtk_container_add (GTK_CONTAINER (scrolledwin), terminal);
  gtk_box_pack_start (GTK_BOX (box), bbox, FALSE, TRUE, 6);
  gtk_box_pack_start (GTK_BOX (bbox), close_button, FALSE, TRUE, 6);

  g_signal_connect (window, "delete-event",
                    G_CALLBACK (close_window), NULL);
  g_signal_connect_swapped (close_button, "clicked", 
                            G_CALLBACK (close_window), window);

  gtk_widget_show_all (box);

  return window;
}

int
main (int argc, char *argv[])
{
  GtkStatusIcon *icon;
  GtkWidget *menu;
  GtkWidget *quit_item;
  GtkWidget *window;
  GtkTextBuffer *buffer;
  GError *error = NULL;
  int seen_errors;
  gchar *file;

  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  if (!gtk_init_with_args (&argc, &argv, 
                           _("[FILE]"), entries, GETTEXT_PACKAGE, &error))
    {
      g_print ("%s\n", error ? error->message : "Beep");
      exit (1);
    }

  if (argc > 1)
    file = argv[1];
  else
    file = DEFAULT_LOG;

  buffer = read_boot_log (file, &seen_errors, &error);
  if (buffer == NULL)
    {
      g_print ("%s\n", error ? error->message : "Blop");
      exit (1);
    }

  window = create_window (buffer);

  if (seen_errors == 2)
    gtk_window_set_icon_name (GTK_WINDOW (window), GTK_STOCK_DIALOG_WARNING);
  else
    gtk_window_set_icon_name (GTK_WINDOW (window), GTK_STOCK_INFO);

  if (show_icon)
    {
      menu = gtk_menu_new ();
      quit_item = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);
      gtk_menu_append (GTK_MENU (menu), quit_item);
      gtk_widget_show_all (menu);
      g_signal_connect (quit_item, "activate", G_CALLBACK (gtk_main_quit), NULL);
  
      icon = gtk_status_icon_new ();
      if (seen_errors == 2)
        gtk_status_icon_set_from_stock (icon, GTK_STOCK_DIALOG_WARNING);
      else if (seen_errors == 1 || force)
        gtk_status_icon_set_from_stock (icon, GTK_STOCK_INFO);
      else
        exit (0);

      gtk_status_icon_set_tooltip (icon, _("Console output from services during system startup"));
      gtk_status_icon_set_title (icon, _("Boot messages"));

      g_signal_connect (icon, "activate", G_CALLBACK (activate_icon), window);
      g_signal_connect (icon, "popup-menu", G_CALLBACK (popup_menu), menu);
    }
  else
    gtk_window_present (GTK_WINDOW (window));

  gtk_main ();

  return 0;
}

/*
 * Callback functions for Xdialog.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#ifdef STDC_HEADERS
#	include <stdlib.h>
#	include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif
#include <time.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

extern Xdialog_data Xdialog;

/* This function is called when a "delete_event" is received from the window
 * manager. It is used to trigger a "destroy" event by returning FALSE
 * (provided the "--no-close" option was not given).
 */
gboolean delete_event(gpointer object, GdkEventAny *event, gpointer data)
{
	return Xdialog.no_close;
}

/* This function is called when a "destroy" event is received 
 * by the top level window.
 */
gboolean destroy_event(gpointer object, GdkEventAny *event, gpointer data)
{
	if (Xdialog.timer != 0) {
		gtk_timeout_remove(Xdialog.timer);
		Xdialog.timer = 0;
	}
	if (Xdialog.timer2 != 0) {
		gtk_timeout_remove(Xdialog.timer2);
		Xdialog.timer2 = 0;
	}
	gtk_main_quit();
	Xdialog.window = Xdialog.widget1 = Xdialog.widget2 = Xdialog.widget3 = NULL;

	if (Xdialog.file != NULL) {
		if (Xdialog.file != stdin)
			fclose(Xdialog.file);
		Xdialog.file = NULL;
	}
	if (Xdialog.array != NULL) {
		g_free(Xdialog.array);
		Xdialog.array = NULL;
	}

	if (Xdialog.beep & BEEP_AFTER && Xdialog.exit_code != 2)
		gdk_beep();

	return FALSE;
}

/* These are the normal termination callback routines that are used by the
 * OK, Cancel and Help buttons. They set the exit_code global variable (for
 * use as the exit() parameter by main.c), and then call the destroy_event
 * function that will cleanup everything, destroy the top level window and
 * set the gtk main loop exit flag. They are used "as is" by most Xdialog
 * widgets but care must be taken that they are connected with the
 * gtk_signal_connect_after() function so that any other signal callbacks
 * are executed BEFORE them (after them, the widget does not exists anymore).
 */
gboolean exit_ok(gpointer object, gpointer data)
{
	if (Xdialog.check) {
		if (Xdialog.checked)
			fprintf(Xdialog.output, "checked\n");
		else
			fprintf(Xdialog.output, "unchecked\n");
	}
	gtk_widget_destroy(Xdialog.window);
	Xdialog.exit_code = 0;
	return FALSE;
}

gboolean exit_cancel(gpointer object, gpointer data)
{
	Xdialog.exit_code = 1;
	gtk_widget_destroy(Xdialog.window);
	return FALSE;
}

gint exit_keypress(gpointer object, GdkEventKey *event, gpointer data)
{
	if (event->type == GDK_KEY_PRESS && (event->keyval == GDK_Escape)) {
		return exit_cancel(object, data);
	}
	return TRUE;
}

gboolean exit_help(gpointer object, gpointer data)
{
	Xdialog.exit_code = 2;
	gtk_widget_destroy(Xdialog.window);
	return FALSE;
}

gboolean exit_previous(gpointer object, gpointer data)
{
	Xdialog.exit_code = 3;
	gtk_widget_destroy(Xdialog.window);
	return FALSE;
}

gboolean checked(GtkObject *button, gpointer data)
{
	Xdialog.checked = GTK_TOGGLE_BUTTON(button)->active;
	return TRUE;
}

gboolean timeout_exit(gpointer data)
{
	Xdialog.exit_code = 255;
	gtk_widget_destroy(Xdialog.window);
	return FALSE;
}

/* This function is called within the timeout functions so to force
 * the processing of all queued GTK events.
 */
gboolean empty_gtk_queue(void)
{
	while(gtk_events_pending()) {
		gtk_main_iteration();
		/* If the timeout function has been removed, return immediately */
		if (Xdialog.timer == 0)
			return FALSE;
	}
	/* All events have been taken into account... */
	return TRUE;
}

/* infobox callbacks: the infobox_timeout_exit() is responsible for
 * closing the infobox once the timeout is over (it therefore calls
 * exit_ok()). The infobox_timeout() function is responsible for
 * reading the stdin and changing the infobox label and/or exiting the
 * infobox by calling exit_ok().
 */

gboolean infobox_timeout_exit(gpointer data)
{
	return exit_ok(NULL, NULL);
}

gboolean infobox_timeout(gpointer data)
{
	char temp[256];
	int ret;

	if (!empty_gtk_queue())
		return FALSE;

	/* Read the the data from stdin */
#ifdef USE_SCANF
	ret = scanf("%255s", temp);
#else
	ret = my_scanf(temp);
#endif
	if ((ret == EOF && !Xdialog.ignore_eof) || (strcmp(temp, "XXXX") == 0))
		return exit_ok(NULL, NULL);
	if (ret != 1)
		return TRUE;

	if (strcmp(temp, "XXX") == 0) {
		/* If this is a new label delimiter, then check to see if it's the
		 * start or the end of the label. */
		if (Xdialog.new_label) {
			gtk_label_set_text(GTK_LABEL(Xdialog.widget1),
					   Xdialog.label_text);
			Xdialog.label_text[0] = 0 ;
			Xdialog.new_label = FALSE;
		} else {
			Xdialog.new_label = TRUE;
		}
	} else {
		/* Add this text to the new label text */
		if (strlen(Xdialog.label_text)+strlen(temp)+2 < MAX_LABEL_LENGTH) {
			if (strcmp(temp, "\\n") == 0) {
				strcat(Xdialog.label_text, "\n");
			} else {
				strcat(Xdialog.label_text, " ");
				strcat(Xdialog.label_text, temp);
			}
		}
	}

	/* As this is a timeout function, return TRUE so that it
	 * continues to get called */
	return TRUE;
}

/* gauge timeout callback */

gboolean gauge_timeout(gpointer data)
{
	gfloat new_val;
	GtkAdjustment *adj;
	char temp[256];
	int ret;

	if (!empty_gtk_queue())
		return FALSE;

	/* Read the new progress bar value or the new label from stdin */
#ifdef USE_SCANF
	ret = scanf("%255s", temp);
#else
	ret = my_scanf(temp);
#endif
	if (ret == EOF && !Xdialog.ignore_eof)
		return exit_ok(NULL, NULL);
	if (ret != 1)
		return TRUE;

	if (!Xdialog.new_label && strcmp(temp, "XXX")) {
		/* Try to convert the string into an integer for use as the new
	 	 * progress bar value... */
		new_val = (gfloat) atoi(temp);
		adj = GTK_PROGRESS(Xdialog.widget1)->adjustment;
		if ((new_val > adj->upper) || (new_val < adj->lower))
			return exit_ok(NULL, NULL);
		/* Set the new value */
		gtk_progress_set_value(GTK_PROGRESS(Xdialog.widget1), new_val);
	} else {
		if (strcmp(temp, "XXX") == 0) {
			/* If this is a new label delimiter, then check to see if it's the
			 * start or the end of the label. */
			if (Xdialog.new_label) {
				gtk_label_set_text(GTK_LABEL(Xdialog.widget2),
						   Xdialog.label_text);
				Xdialog.label_text[0] = 0 ;
				Xdialog.new_label = FALSE;
			} else {
				Xdialog.new_label = TRUE;
			}
		} else {
			/* Add this text to the new label text */
			if (strlen(Xdialog.label_text)+strlen(temp)+2 < MAX_LABEL_LENGTH) {
				if (strcmp(temp, "\\n") == 0) {
					strcat(Xdialog.label_text, "\n");
				} else {
					strcat(Xdialog.label_text, " ");
					strcat(Xdialog.label_text, temp);
				}
			}
		}
	}

	/* As this is a timeout function, return TRUE so that it
	 * continues to get called */
	return TRUE;
}

/* progress timeout callback */

gboolean progress_timeout(gpointer data)
{
	gfloat new_val;
	GtkAdjustment *adj;
	char temp[256];
	int ret;

	if (!empty_gtk_queue())
		return FALSE;

	adj = GTK_PROGRESS(Xdialog.widget1)->adjustment;

	temp[255] = '\0';

	/* Read the new progress bar value or the new "dot" from stdin,
	 * skipping any control character.
	 */
	do {
		ret = fgetc(stdin);
		if (ret == EOF)
			return exit_ok(NULL, NULL);
		temp[0] = (char) ret;
	} while (temp[0] <= ' ');

	if (temp[0] >= '0' && temp[0] <= '9') {
		/* Get and convert a string into an integer for use as
		 * the new progress bar value... */
		ret = scanf("%254s", temp + 1);
		if (ret == EOF)
			return exit_ok(NULL, NULL);
		if (ret != 1)
			return TRUE;
		new_val = (gfloat) atoi(temp);
	} else {
		/* Increment the number of "dots" */
		new_val = adj->value + 1;
	}

	if ((new_val > adj->upper) || (new_val < adj->lower))
		return exit_ok(NULL, NULL);
	/* Set the new value */
	gtk_progress_set_value(GTK_PROGRESS(Xdialog.widget1), new_val);

	/* As this is a timeout function, return TRUE so that it
	 * continues to get called */
	return TRUE;
}

/* tailbox and logbox callbacks */

#ifdef USE_GTK2
gboolean tailbox_timeout(gpointer data)
{
	gchar buffer[1024];
	int nchars = 0;
	GtkTextIter end_iter;

	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Xdialog.widget1));

	do {
		if (!empty_gtk_queue() && (Xdialog.file_init_size <= 0))
			return FALSE;

		nchars = fread(buffer, sizeof(gchar), 1024, Xdialog.file);
		if (nchars == 0)
			break;

		gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
		gtk_text_buffer_insert(text_buffer, &end_iter, buffer, nchars);

		if (Xdialog.file_init_size > 0) 
			Xdialog.file_init_size -= nchars;
	} while (nchars == 1024);

	if (nchars > 0) {
		GtkTextMark *mark;
		gtk_text_buffer_get_end_iter(text_buffer, &end_iter);

		mark = gtk_text_buffer_create_mark(text_buffer, "end",
			&end_iter, TRUE);

		gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(Xdialog.widget1),
			mark, 0, FALSE, 0, 0);
	}

	return TRUE;
}
#else
gboolean tailbox_timeout(gpointer data)
{
	GtkAdjustment *adj;
	gchar buffer[1024];
	int nchars;
	gboolean flag = FALSE;

	adj = GTK_TEXT(Xdialog.widget1)->vadj;

	if (Xdialog.file_init_size > 0)
		gtk_text_freeze(GTK_TEXT(Xdialog.widget1));

	do {
		if (!empty_gtk_queue() && (Xdialog.file_init_size <= 0))
			return FALSE;

		nchars = fread(buffer, sizeof(gchar), 1024, Xdialog.file);

		if (nchars == 0)
			break;

		if (Xdialog.file_init_size > 0) {
			Xdialog.file_init_size -= nchars;
			if (Xdialog.file_init_size <= 0)
				flag = TRUE;
		} else {
			if (!Xdialog.smooth)
				gtk_text_freeze(GTK_TEXT(Xdialog.widget1));
		}

		gtk_text_insert(GTK_TEXT(Xdialog.widget1), NULL, NULL,
				NULL, buffer, nchars);

		if ((!Xdialog.smooth || flag) && Xdialog.file_init_size <= 0) {
			gtk_text_thaw(GTK_TEXT(Xdialog.widget1));
			flag = FALSE;
		}

		gtk_adjustment_set_value(adj, adj->upper);

	} while (nchars == 1024);

	return TRUE;
}
#endif

gint tailbox_keypress(GtkObject *text, GdkEventKey *event,
		      gpointer data)
{
	if (event->type == GDK_KEY_PRESS && (event->keyval == GDK_Return ||
					     event->keyval == GDK_KP_Enter)) {
		if (Xdialog.default_no)
			Xdialog.exit_code = 1;
		else
			Xdialog.exit_code = 0;

		return(FALSE);
	}
	return(TRUE);
}

#ifdef HAVE_STRSTR

static void vt_to_gdk_color(gint color, GdkColor **fgcolor, GdkColor **bgcolor)
{
	static const GdkColor BLACK     = { 0, 0x0000, 0x0000, 0x0000 };
	static const GdkColor RED       = { 0, 0xffff, 0x0000, 0x0000 };
	static const GdkColor GREEN     = { 0, 0x0000, 0xffff, 0x0000 };
	static const GdkColor BLUE      = { 0, 0x0000, 0x0000, 0xffff };
	static const GdkColor MAGENTA   = { 0, 0xffff, 0x0000, 0xffff };
	static const GdkColor YELLOW    = { 0, 0xffff, 0xffff, 0x0000 };
	static const GdkColor CYAN      = { 0, 0x0000, 0xffff, 0xffff };
	static const GdkColor WHITE     = { 0, 0xffff, 0xffff, 0xffff };

	switch (color) {
		case 30:
			*fgcolor = (GdkColor *) &BLACK;
			break;
		case 31:
			*fgcolor = (GdkColor *) &RED;
			break;
		case 32:
			*fgcolor = (GdkColor *) &GREEN;
			break;
		case 33:
			*fgcolor = (GdkColor *) &YELLOW;
			break;
		case 34:
			*fgcolor = (GdkColor *) &BLUE;
			break;
		case 35:
			*fgcolor = (GdkColor *) &MAGENTA;
			break;
		case 36:
			*fgcolor = (GdkColor *) &CYAN;
			break;
		case 37:
			*fgcolor = (GdkColor *) &WHITE;
			break;
		case 38:
			*fgcolor = NULL;
			break;
		case 40:
			*bgcolor = (GdkColor *) &BLACK;
			break;
		case 41:
			*bgcolor = (GdkColor *) &RED;
			break;
		case 42:
			*bgcolor = (GdkColor *) &GREEN;
			break;
		case 43:
			*bgcolor = (GdkColor *) &YELLOW;
			break;
		case 44:
			*bgcolor = (GdkColor *) &BLUE;
			break;
		case 45:
			*bgcolor = (GdkColor *) &MAGENTA;
			break;
		case 46:
			*bgcolor = (GdkColor *) &CYAN;
			break;
		case 47:
			*bgcolor = (GdkColor *) &WHITE;
			break;
		case 48:
			*bgcolor = NULL;
			break;
	}
}

static void remove_vt_sequences(char *str)
{
	char *p, *q;

	while ((p = strstr(str, "\033[")) != NULL) {
		q = p;
		while (++p < str + strlen(str)) {
			if (*p == 'm')
				break;
		}
		strcpy(q, ++p);
	}
}

static GdkColor *old_fgcolor = NULL;
static GdkColor *old_bgcolor = NULL;

gboolean logbox_timeout(gpointer data)
{
	GtkCList *clist = GTK_CLIST(Xdialog.widget1);
	GdkColor *fgcolor, *bgcolor;
	gchar buffer[MAX_LABEL_LENGTH], *p;
	static gchar *null_single_row[] = { NULL };
	static gchar *null_double_row[] = { NULL, NULL };
	gint rownum;
	int color, len;
	struct tm *localdate = NULL;
	time_t curr_time;
	gboolean flag = FALSE;

	if (!Xdialog.smooth && (Xdialog.file_init_size > 0))
		gtk_clist_freeze(clist);
	else {
		if (!empty_gtk_queue())
			return FALSE;
	}

	while (fgets(buffer, MAX_LABEL_LENGTH, Xdialog.file) != NULL) {

		len = strlen(buffer);

		if (Xdialog.file_init_size > 0) {
			Xdialog.file_init_size -= len;
			if (Xdialog.file_init_size <= 0)
				flag = TRUE;
		}

		if ((len > 0) && (buffer[len - 1] == '\n'))
			buffer[--len] = 0;

		if (Xdialog.time_stamp) {
			time(&curr_time);
			localdate = localtime(&curr_time);
		}

		if (Xdialog.keep_colors) {
			fgcolor = old_fgcolor;
			bgcolor = old_bgcolor;
		} else
			fgcolor = bgcolor = NULL;

		if ((p = strstr(buffer, "\033[1;")) != NULL) {
			p += 4;
			color = atoi(p);
			vt_to_gdk_color(color, &fgcolor, &bgcolor);

			while (++p < buffer + len) {
				if (*p == ';') {
					color = atoi(++p);
					vt_to_gdk_color(color, &fgcolor, &bgcolor);
					p += 2;
				}
				if (*p == 'm')
					break;
			}

			if (Xdialog.keep_colors) {
				old_fgcolor = fgcolor;
				old_bgcolor = bgcolor;
			}

			remove_vt_sequences(buffer);
		}

		if (Xdialog.reverse)
			rownum = gtk_clist_prepend(clist,
						   Xdialog.time_stamp ? null_double_row : null_single_row);
		else
			rownum = gtk_clist_append(clist,
						  Xdialog.time_stamp ? null_double_row : null_single_row);
		gtk_clist_set_selectable(clist, rownum, FALSE);
		if (fgcolor != NULL)
			gtk_clist_set_foreground(clist, rownum, fgcolor);
		if (bgcolor != NULL)
			gtk_clist_set_background(clist, rownum, bgcolor);
		if (Xdialog.time_stamp) {
			gtk_clist_set_text(clist, rownum, 1, buffer);
			if (Xdialog.date_stamp)
				sprintf(buffer, "%02d/%02d/%d %02d:%02d:%02d ",
					localdate->tm_mday, localdate->tm_mon+1, localdate->tm_year+1900,
					localdate->tm_hour, localdate->tm_min, localdate->tm_sec);
			else
				sprintf(buffer, "%02d:%02d:%02d",
					localdate->tm_hour, localdate->tm_min, localdate->tm_sec);
			gtk_clist_set_text(clist, rownum, 0, buffer);
		} else
			gtk_clist_set_text(clist, rownum, 0, buffer);
		gtk_clist_columns_autosize(clist);

		if ((Xdialog.file_init_size <= 0) || Xdialog.smooth) {
			if (flag) {
				gtk_clist_thaw(clist);
				flag = FALSE;
			}

			if (Xdialog.reverse)
				gtk_clist_moveto(clist, rownum, 0, 0.0, 0.0);
			else
				gtk_clist_moveto(clist, rownum, 0, 1.0, 0.0);
		}

		if (!empty_gtk_queue())
			return FALSE;
	}

	return TRUE;
}

#else
#error strstr() function is needed by Xdialog !
#endif

/* Here are the callback functions for the inputboxes and combobox (OK button
 * callback and entry RETURN/ENTER keypress callback).
 */
gint inputbox_ok(gpointer object, gpointer data)
{
	fprintf(Xdialog.output, "%s",
		gtk_entry_get_text(GTK_ENTRY(Xdialog.widget1)));
	if (Xdialog.widget2 != NULL)
		fprintf(Xdialog.output, "%s%s", Xdialog.separator,
			gtk_entry_get_text(GTK_ENTRY(Xdialog.widget2)));
	if (Xdialog.widget3 != NULL)
		fprintf(Xdialog.output, "%s%s", Xdialog.separator,
			gtk_entry_get_text(GTK_ENTRY(Xdialog.widget3)));
	fprintf(Xdialog.output, "\n");
		
	return TRUE;
}

gboolean inputbox_timeout(gpointer data)
{
	return inputbox_ok(NULL, NULL);
}


gint input_keypress(GtkObject *entry, GdkEventKey *event, gpointer data)
{
	if (event->type == GDK_KEY_PRESS && (event->keyval == GDK_Return ||
					     event->keyval == GDK_KP_Enter)) {
		if (Xdialog.default_no) {
			Xdialog.exit_code = 1;
		} else {
			inputbox_ok(NULL, NULL);
			Xdialog.exit_code = 0;
		}
		if (Xdialog.check) {
			if (Xdialog.checked)
				fprintf(Xdialog.output, "checked\n");
			else
				fprintf(Xdialog.output, "unchecked\n");
		}
		gtk_widget_destroy(Xdialog.window);
		return FALSE;
	}
	return TRUE;
}

gboolean hide_passwords(GtkObject *button, gpointer data)
{
	gint entries;
	gboolean visible;

	entries = 1 + (Xdialog.widget2 != NULL ? 1 : 0) + (Xdialog.widget3 != NULL ? 1 : 0);

	visible = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)) == FALSE);

	if ((Xdialog.passwd < 10 && Xdialog.passwd >= entries) || Xdialog.passwd == 11)
		gtk_entry_set_visibility(GTK_ENTRY(Xdialog.widget1), visible);
	if (entries > 1 && ((Xdialog.passwd < 10 && Xdialog.passwd >= entries-1) || Xdialog.passwd == 12))
		gtk_entry_set_visibility(GTK_ENTRY(Xdialog.widget2), visible);
	if (entries > 2 && ((Xdialog.passwd < 10 && Xdialog.passwd > 0) || Xdialog.passwd == 13))
		gtk_entry_set_visibility(GTK_ENTRY(Xdialog.widget3), visible);

	return TRUE;
}

/* editbox callback */

#ifdef USE_GTK2
gboolean editbox_ok(gpointer object, gpointer data)
{
	GtkTextIter start_iter, end_iter;

	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Xdialog.widget1));
	gtk_text_buffer_get_bounds(text_buffer, &start_iter, &end_iter);
	fputs(gtk_text_buffer_get_text(text_buffer, &start_iter, &end_iter,
		FALSE), Xdialog.output);

	return TRUE;
}
#else
gboolean editbox_ok(gpointer object, gpointer data)
{
	int length, i;

	length = gtk_text_get_length(GTK_TEXT(Xdialog.widget1));
	for (i = 0; i < length; i++)
		fputc(GTK_TEXT_INDEX(GTK_TEXT(Xdialog.widget1), i),
		      Xdialog.output);

	return TRUE;
}
#endif

/* The print button callback (used by editbox, textbox and tailbox) */

#ifdef USE_GTK2
gboolean print_text(gpointer object, gpointer data)
{
	gint length;
	char cmd[MAX_PRTCMD_LENGTH];
	FILE * temp;
	GtkTextIter start_iter, end_iter;
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Xdialog.widget1));
	
	gtk_text_buffer_get_bounds(text_buffer, &start_iter, &end_iter);

	strcpysafe(cmd, PRINTER_CMD, MAX_PRTCMD_LENGTH);
	if (strlen(Xdialog.printer) != 0) {
		strcatsafe(cmd, " "PRINTER_CMD_OPTION, MAX_PRTCMD_LENGTH);
		strcatsafe(cmd, Xdialog.printer, MAX_PRTCMD_LENGTH);
	}

	length = gtk_text_buffer_get_char_count(text_buffer);

	temp = popen(cmd, "w");
	if (temp != NULL) {
		fwrite(gtk_text_buffer_get_text(text_buffer, &start_iter,
			&end_iter, FALSE), sizeof(gchar), length, temp);
		pclose(temp);
	}

	return TRUE;
}
#else
gboolean print_text(gpointer object, gpointer data)
{
	int length, i;
	gchar * buffer;
	char cmd[MAX_PRTCMD_LENGTH];
	FILE * temp;

	strcpysafe(cmd, PRINTER_CMD, MAX_PRTCMD_LENGTH);
	if (strlen(Xdialog.printer) != 0) {
		strcatsafe(cmd, " "PRINTER_CMD_OPTION, MAX_PRTCMD_LENGTH);
		strcatsafe(cmd, Xdialog.printer, MAX_PRTCMD_LENGTH);
	}

	length = gtk_text_get_length(GTK_TEXT(Xdialog.widget1));
	buffer = g_malloc((length+1)*sizeof(gchar));
	for (i = 0; i < length; i++)
		buffer[i] = GTK_TEXT_INDEX(GTK_TEXT(Xdialog.widget1), i);

	temp = popen(cmd, "w");
	if (temp != NULL) {
		i = fwrite(buffer, sizeof(gchar), length, temp);
		pclose(temp);
	}
	g_free(buffer);

	return TRUE;
}
#endif

/* rangebox callbacks */

gboolean rangebox_exit(GtkButton *button, gpointer data)
{
	GtkAdjustment *adj;

	adj = GTK_ADJUSTMENT((GtkObject *) Xdialog.widget1);
	fprintf(Xdialog.output, "%d", (gint) adj->value);
	if (Xdialog.widget2 != NULL) {
		adj = GTK_ADJUSTMENT((GtkObject *) Xdialog.widget2);
		fprintf(Xdialog.output, "%s%d", Xdialog.separator,
			(gint) adj->value);
	}
	if (Xdialog.widget3 != NULL) {
		adj = GTK_ADJUSTMENT((GtkObject *) Xdialog.widget3);
		fprintf(Xdialog.output, "%s%d", Xdialog.separator,
			(gint) adj->value);
	}
	fprintf(Xdialog.output, "\n");

	return TRUE;
}


gboolean rangebox_timeout(gpointer data)
{
	return rangebox_exit(NULL, NULL);
}

/* spin boxes callbacks */

gboolean spinbox_exit(GtkButton *button, gpointer data)
{
	fprintf(Xdialog.output, "%d",
		gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(Xdialog.widget1)));

	if (Xdialog.widget2 != NULL)
		fprintf(Xdialog.output, "%s%d", Xdialog.separator,
			gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(Xdialog.widget2)));

	if (Xdialog.widget3 != NULL)
		fprintf(Xdialog.output, "%s%d", Xdialog.separator,
			gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(Xdialog.widget3)));

	fprintf(Xdialog.output, "\n");

	return TRUE;
}

gboolean spinbox_timeout(gpointer data)
{
	return spinbox_exit(NULL, NULL);
}

/* Double-click event is processed as a button click in menubox, radiolist and
 * checklist... The button widget is to be passed as "data".
 */
gint double_click_event(GtkObject *object, GdkEventButton *event,
			gpointer data)
{
	if (event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS)
		gtk_signal_emit_by_name(GTK_OBJECT(data), "clicked");

	return FALSE;
}

/* radiolist and checklist callbacks */

void item_toggle(GtkObject *item, int i)
{
	if (GTK_TOGGLE_BUTTON(item)->active) {
		Xdialog.array[i].state = 1;
	} else
		Xdialog.array[i].state = 0;
}

gboolean print_items(GtkButton *button, gpointer data)
{
	int i;
	gboolean flag = FALSE;

	for (i = 0 ; Xdialog.array[i].state != -1 ; i++) {
		if (Xdialog.array[i].state) {
			if (flag)
				fprintf(Xdialog.output, "%s", Xdialog.separator);
			fprintf(Xdialog.output, "%s", Xdialog.array[i].tag);
			flag = TRUE;
		}
	}
	if (flag)
		fprintf(Xdialog.output, "\n");

	return TRUE;
}

gboolean itemlist_timeout(gpointer data)
{
	return print_items(NULL, NULL);
}

/* menubox callback */

void item_select(GtkObject *clist, gint row, gint column,
		 GdkEventButton *event, gpointer data)
{
	/* If the tag is empty, then this is an unavailable item:
	 * select back the last selected row and exit.
	 */
	if (strlen(Xdialog.array[row].tag) == 0) {
		gtk_clist_select_row(GTK_CLIST(clist), Xdialog.array[0].state, 0);
		return;
	}

	/* Just remember which row was last selected (we use the first
	 * row status variable IOT do so)...
	 */
	Xdialog.array[0].state = row;

	if (Xdialog.tips == 1) {
		gtk_statusbar_pop(GTK_STATUSBAR(Xdialog.widget1), Xdialog.status_id);
		gtk_statusbar_push(GTK_STATUSBAR(Xdialog.widget1), Xdialog.status_id,
				   Xdialog.array[row].tips);
	}
}

/* menubox and treeview callback */

gboolean print_selection(GtkButton *button, gpointer data)
{
	if ( Xdialog.array[0].state >= 0) {
		fprintf(Xdialog.output, "%s\n", Xdialog.array[Xdialog.array[0].state].tag);
		return TRUE;
	} else
		return FALSE;
}

#ifdef USE_GTK2
gboolean print_tree_selection(GtkButton *button, gpointer data)
{
	int i = 0;
	for (i = 0 ; Xdialog.array[i].state != -1 ; i++) {
		if (Xdialog.array[i].state > 0) {
			fprintf(Xdialog.output, "%s\n", Xdialog.array[i].tag);
 			return TRUE;
		}
	}
	return FALSE;
}
#endif

gboolean menu_timeout(gpointer data)
{
	return print_selection(NULL, NULL);
}

gboolean move_to_row_timeout(gpointer data)
{
	gtk_clist_moveto(GTK_CLIST(Xdialog.widget2),
			 Xdialog.array[0].state, 0, 0.5, 0.0);

	/* Run this timeout function only once ! */
	return FALSE;	
}

#ifdef USE_GTK2
void cb_selection_changed(GtkObject *tree)
{
	GtkTreeIter tree_iter;
	GtkTreeModel *model;
	GtkTreeSelection* selection;
	gchar *name, *tag;
	int i = 0;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(Xdialog.widget1));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(Xdialog.widget1));

	if (gtk_tree_selection_get_selected(selection, &model, &tree_iter))
		gtk_tree_model_get(model, &tree_iter, 0, &name, 1, &tag, -1);

	for (i = 0 ; Xdialog.array[i].state != -1 ; i++) {
		if (!strncmp(Xdialog.array[i].name, name, strlen(name)))
			Xdialog.array[i].state = 1;
		else
			Xdialog.array[i].state = 0;
	}

	g_free(name);
	g_free(tag);
}
#else
void cb_selection_changed(GtkObject *tree)
{
	GList *list;
	GtkWidget *item;
	int i;

	list = GTK_TREE_SELECTION(tree);
	if (list != NULL) {
		item = GTK_WIDGET(list->data);
		for (i = 0 ; Xdialog.array[i].state != -1 ; i++) {
			if (Xdialog.array[i].widget == item) {
				Xdialog.array[0].state = i;
				break;
			}
		}
	}
}
#endif

/* buildlist callbacks */

/* the following function is to be called after each glist update so to
 * set the proper (in)sensitive status onto the Add/Remove buttons.
 */
#ifdef USE_GTK2
void sensitive_buttons(void)
{
	GtkTreeIter tree_iter;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(Xdialog.widget1));

	if (gtk_tree_model_get_iter_first(model, &tree_iter)) {
		gtk_widget_set_sensitive(Xdialog.widget3, TRUE);
	} else {
		gtk_widget_set_sensitive(Xdialog.widget3, FALSE);
	}

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(Xdialog.widget2));

	if (gtk_tree_model_get_iter_first(model, &tree_iter)) {
		gtk_widget_set_sensitive(Xdialog.widget4, TRUE);
	} else {
		gtk_widget_set_sensitive(Xdialog.widget4, FALSE);
	}
}
#else
void sensitive_buttons(void)
{
	gtk_widget_set_sensitive(Xdialog.widget3,
				 g_list_length(GTK_LIST(Xdialog.widget1)->children) != 0);
	gtk_widget_set_sensitive(Xdialog.widget4,
				 g_list_length(GTK_LIST(Xdialog.widget2)->children) != 0);
}
#endif

#ifdef USE_GTK2
gboolean add_to_list(GtkButton *button, gpointer data)
{
	GtkTreeIter tree_iter1, tree_iter2;
	GtkTreeModel *model1, *model2;
	GtkTreeSelection* selection1;

	model1 = gtk_tree_view_get_model(GTK_TREE_VIEW(Xdialog.widget1));
	selection1 = gtk_tree_view_get_selection(GTK_TREE_VIEW(Xdialog.widget1));
	model2 = gtk_tree_view_get_model(GTK_TREE_VIEW(Xdialog.widget2));

	if (gtk_tree_selection_get_selected(selection1, &model1, &tree_iter1)) {
		gchar *name, *tag;
		gtk_tree_model_get(model1, &tree_iter1, 0, &name, 1, &tag, -1);

		gtk_list_store_append(GTK_LIST_STORE(model2), &tree_iter2);
		gtk_list_store_set(GTK_LIST_STORE(model2), &tree_iter2, 0,
			 name, 1, tag, -1);
		gtk_list_store_remove(GTK_LIST_STORE(model1), &tree_iter1);

		g_free(name);
		g_free(tag);
	}
  
	sensitive_buttons(); 

	return TRUE;
}
#else
gboolean add_to_list(GtkButton *button, gpointer data)
{
	GList *selected;

	selected = g_list_copy(GTK_LIST(Xdialog.widget1)->selection);
	gtk_list_remove_items_no_unref(GTK_LIST(Xdialog.widget1), selected);
	gtk_list_append_items(GTK_LIST(Xdialog.widget2), selected);

	sensitive_buttons();

	return TRUE;
}
#endif

#ifdef USE_GTK2
gboolean remove_from_list(GtkButton *button, gpointer data)
{
	GtkTreeIter tree_iter1, tree_iter2;
	GtkTreeModel *model1, *model2;
	GtkTreeSelection* selection2;

	model1 = gtk_tree_view_get_model(GTK_TREE_VIEW(Xdialog.widget1));
	model2 = gtk_tree_view_get_model(GTK_TREE_VIEW(Xdialog.widget2));
	selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(Xdialog.widget2));

	if (gtk_tree_selection_get_selected(selection2, &model2, &tree_iter2)) {
		gchar *name, *tag;
		gtk_tree_model_get(model2, &tree_iter2, 0, &name, 1, &tag, -1);

		gtk_list_store_append(GTK_LIST_STORE(model1), &tree_iter1);
		gtk_list_store_set(GTK_LIST_STORE(model1), &tree_iter1, 0, name, 1, tag, -1);
		gtk_list_store_remove(GTK_LIST_STORE(model2), &tree_iter2);

		g_free(name);
		g_free(tag);
	}

	sensitive_buttons();

	return TRUE;
}
#else
gboolean remove_from_list(GtkButton *button, gpointer data)
{
	GList *selected;

	selected = g_list_copy(GTK_LIST(Xdialog.widget2)->selection);
	gtk_list_remove_items_no_unref(GTK_LIST(Xdialog.widget2), selected);
	gtk_list_append_items(GTK_LIST(Xdialog.widget1), selected);

	sensitive_buttons();

	return TRUE;
}
#endif

#ifdef USE_GTK2
gboolean print_list(GtkButton *button, gpointer data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	gboolean valid;
	gboolean flag = FALSE;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(Xdialog.widget2));
	valid = gtk_tree_model_get_iter_first(model, &iter);

	while (valid) {
		gchar *name_data, *tag_data;

		gtk_tree_model_get(model, &iter, 0, &name_data, 1, &tag_data, -1);

		if (flag)
			fprintf(Xdialog.output, "%s", Xdialog.separator);

		fprintf(Xdialog.output, "%s", tag_data);
		flag = TRUE;

		g_free(name_data);
		g_free(tag_data);

		valid = gtk_tree_model_iter_next(model, &iter);
	}

	if (flag)
		fprintf(Xdialog.output, "\n");

	return TRUE;
}
#else
gboolean print_list(GtkButton *button, gpointer data)
{
	GList *children;
	int i;
	gboolean flag = FALSE;

	children = GTK_LIST(Xdialog.widget2)->children;
	while (children != NULL) {
		for (i = 0 ; Xdialog.array[i].state != -1 ; i++) {
			if (Xdialog.array[i].widget == children->data) {
				if (flag)
					fprintf(Xdialog.output, "%s", Xdialog.separator);
				fprintf(Xdialog.output, "%s", Xdialog.array[i].tag);
				flag = TRUE;
				break;
			}
		}
		children = g_list_next(children);
	}
	if (flag)
		fprintf(Xdialog.output, "\n");

	return TRUE;
}
#endif

gboolean buildlist_timeout(gpointer data)
{
	return print_list(NULL, NULL);
}

/* fselect and dselect callback */

gboolean filesel_exit(GtkObject *filesel, gpointer client_data)
{
	fprintf(Xdialog.output, "%s\n",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION(client_data)));
	return exit_ok(NULL, NULL);
}

/* colorsel callback */

gboolean colorsel_exit(GtkObject *colorsel, gpointer client_data)
{
	gdouble colors[4];

	gtk_color_selection_get_color(GTK_COLOR_SELECTION(client_data), colors);
	fprintf(Xdialog.output, "%d %d %d\n",
                (int) (255.0 * colors[0]), (int) (255.0 * colors[1]), (int) (255.0 * colors[2]));
	return exit_ok(NULL, NULL);
}

/* fontsel callback */

gboolean fontsel_exit(GtkObject *fontsel, gpointer client_data)
{
	fprintf(Xdialog.output, "%s\n",
                gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(client_data)));
	return exit_ok(NULL, NULL);
}

/* calendar callbacks */

gboolean calendar_exit(gpointer object, gpointer data)
{
	gint day, month, year;

	gtk_calendar_get_date(GTK_CALENDAR(Xdialog.widget1), &year, &month, &day);
	fprintf(Xdialog.output, "%02d/%02d/%d\n", day, month+1, year);

	return TRUE;
}

gboolean calendar_timeout(gpointer data)
{
	return calendar_exit(NULL, NULL);
}

/* timebox callbacks */

gboolean timebox_exit(gpointer object, gpointer data)
{
	fprintf(Xdialog.output, "%02d:%02d:%02d\n",
		gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(Xdialog.widget1)),
		gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(Xdialog.widget2)),
		gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(Xdialog.widget3)));

	return TRUE;
}

gboolean timebox_timeout(gpointer data)
{
	return timebox_exit(NULL, NULL);
}

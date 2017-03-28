/* gtk.c:  GTK+ interface for aumix
 * (c) 1998-2000 the authors (see AUTHORS file)
 *
 * based on:
 * Freestyle DJ sample playing tool
 * Revision 1.11  1998/06/18 18:17:39
 * (C) 1998 Pete Hollobon hollobon@bigfoot.com
 * http://www.york.ac.uk/~ph116/freestyle.html
 *
 * also based on:
 * gmixer 0.98 (22.2.1999)
 * Copyleft (C) 1998, 1999 Sergey Kiselev
 * sergey@junior.technion.ac.il
 *
 * also based on:
 * filesel.c and pixmap.c examples
 * GTK v1.2 Tutorial
 * Tony Gale <gale@gtk.org>, Ian Main <imain@gtk.org>
 * February 23rd, 2000
 */

#include "common.h"
#ifdef HAVE_GTK
#include "gtk.h"
#include "../data/aumix.xpm"
#include "record.xpm"
#include "play.xpm"
#include "interactive.h"
#include <signal.h>		/* SIGALRM */

int             items[SOUND_MIXER_NRDEVICES], *itemp[SOUND_MIXER_NRDEVICES];
GtkToggleButton *rpbuttons[SOUND_MIXER_NRDEVICES];

void            SaveSettingsGTK(void)
{
	(void) SaveSettings();
}

void            LoadSettingsGTK(void)
{
	(void) LoadSettings();
}

void            SwitchRecordPlayGTK(int *device)
{
	int             i;
	/* xxx This loop is an ugly hack.  We only want the device number. */
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++)
		if (rpbuttons[i] == (GtkToggleButton *) device)
			break;
	SwitchRecordPlay(i);
	return;
}

void            AdjustLevelGTK(int *device)
{
	int             i;
	/* xxx This loop is an ugly hack.  We only want the device number. */
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++)
		if (adjustment[i] == (GtkAdjustment *) device)
			break;
	AdjustLevel(i, 0, adjustment[i]->value);
	return;
}

void            AdjustBalanceGTK(int *device)
{
	int             i;
	/* xxx This loop is an ugly hack.  We only want the device number. */
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++)
		if (balance[i] == (GtkAdjustment *) device)
			break;
	AdjustBalance(i, 0, balance[i]->value);
	return;
}

void            aumix_destroy(GtkWidget * widget, gpointer * data)
{
	gtk_widget_destroy(widget);
}

void            InitScreenGTK(void)
{
	GdkColormap    *colormap;
	GdkBitmap      *bitmap;
	GdkPixmap      *icon_pixmap;
	GtkStyle       *style;
	GtkWidget      *window;
	GtkWidget      *levels[SOUND_MIXER_NRDEVICES];
	GtkWidget      *baltracks[SOUND_MIXER_NRDEVICES];
	GtkWidget      *mainbox;
	GtkWidget      *maintable;
	GtkWidget      *filemenu;
	GtkWidget      *filemenuitems[5];
	GtkWidget      *mutemenu;
	GtkWidget      *muteallitem;
	GtkWidget      *menubar;
	GtkWidget      *menubarlabel;
	GtkAccelGroup  *accel;
	GtkWidget      *label;
	GtkWidget      *channellabel[SOUND_MIXER_NRDEVICES];
	GtkWidget      *control_box;
	int             balset, max, i, y, tmp, left, right;
	GtkSignalFunc   filefuncs[] = {&LoadSettingsGTK, &SaveSettingsGTK, &LoadDialog, &SaveDialog, &CloseScreenGTK};
	char           *filemenutext[] = {LOCAL_TEXT_NOOP("Load"), LOCAL_TEXT_NOOP("Save"), LOCAL_TEXT_NOOP("Load From"), LOCAL_TEXT_NOOP("Save To"), LOCAL_TEXT_NOOP("Quit")};
/* XXX The accelerator keys don't get translated. */
	char           *fileacceltext[5] = {LOCAL_TEXT_NOOP('l'), LOCAL_TEXT_NOOP('s'), LOCAL_TEXT_NOOP('f'), LOCAL_TEXT_NOOP('t'), LOCAL_TEXT_NOOP('q')};
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "aumix");
	gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
	gtk_container_border_width(GTK_CONTAINER(window), BORDER);
	mainbox = gtk_vbox_new(FALSE, BORDER);
	gtk_widget_show(mainbox);
	gtk_container_add(GTK_CONTAINER(window), mainbox);
	accel = gtk_accel_group_new();
	gtk_accel_group_attach(accel, GTK_OBJECT(window));
	menubar = gtk_menu_bar_new();
	gtk_widget_show(menubar);
	gtk_box_pack_start(GTK_BOX(mainbox), menubar, FALSE, TRUE, 0);
	filemenu = gtk_menu_new();
	mutemenu = gtk_menu_new();
	for (i = 0; i < 5; i++) {
		filemenuitems[i] = gtk_menu_item_new_with_label(LOCAL_TEXT(filemenutext[i]));
		gtk_menu_append(GTK_MENU(filemenu), filemenuitems[i]);
		gtk_widget_show(filemenuitems[i]);
		gtk_widget_add_accelerator(filemenuitems[i], "activate", accel, (guint)fileacceltext[i], 0, GTK_ACCEL_VISIBLE);
		gtk_signal_connect(GTK_OBJECT(filemenuitems[i]), "activate", filefuncs[i], NULL);
	}
	muteallitem = gtk_check_menu_item_new_with_label(LOCAL_TEXT("Mute All"));
	gtk_menu_append(GTK_MENU(mutemenu), muteallitem);
/* XXX need to translate the "m" */
	gtk_widget_add_accelerator(muteallitem, "activate", accel, 'm', 0, GTK_ACCEL_VISIBLE);
	gtk_widget_show(muteallitem);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(muteallitem), TRUE);
	menubarlabel = gtk_menu_item_new_with_label(LOCAL_TEXT("File"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubarlabel), filemenu);
	gtk_menu_bar_append(GTK_MENU_BAR(menubar), menubarlabel);
	gtk_widget_show(menubarlabel);
	menubarlabel = gtk_menu_item_new_with_label(LOCAL_TEXT("Mute"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubarlabel), mutemenu);
	gtk_menu_bar_append(GTK_MENU_BAR(menubar), menubarlabel);
	gtk_widget_show(menubarlabel);
	gtk_signal_connect(GTK_OBJECT(muteallitem), "activate", GTK_SIGNAL_FUNC(ToggleMuting), NULL);
	maintable = gtk_table_new(CountChannels(SOUND_MIXER_NRDEVICES) + 2,	/* 1 row per mixing channel and two for headings */
				  8,	/* 8 columns:  record/play, "0", "Level", "100", channel names, "L", "balance", and "R" */
				  FALSE);	/* not homogeneous:  all cells not same size */
	gtk_widget_show(maintable);
	gtk_container_add(GTK_CONTAINER(mainbox), maintable);
	style = gtk_widget_get_style(window);
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		if ((1 << i) & devmask) {
			/* Try to read mixer. */
			if (SOUND_IOCTL(mixer_fd, MIXER_READ(i), &tmp) == -1)
				gtk_exit(EREADMIX);
			right = tmp >> 8;
			left = tmp & 0xFF;
			max = (left > right) ? left : right;
			/* See rangewidgets example. */
			adjustment[i] = GTK_ADJUSTMENT(gtk_adjustment_new(((right + left) / 2), 0, MAXLEVEL, 1, 1, 0));
			colormap = gtk_widget_get_colormap(window);
			record_pixmap = gdk_pixmap_colormap_create_from_xpm_d(window->window, colormap, &rmask, &style->bg[GTK_STATE_NORMAL], (gchar **) record_xpm);
			play_pixmap = gdk_pixmap_colormap_create_from_xpm_d(window->window, colormap, &pmask, &style->bg[GTK_STATE_NORMAL], (gchar **) play_xpm);
			if ((1 << i) & recmask) {
				ExitIfError(ReadRecSrc());
				/* a pixmap widget to contain the pixmap */
				pixmapwid[i] = gtk_pixmap_new((1 << i) & recsrc ? record_pixmap : play_pixmap, (1 << i) & recsrc ? rmask : pmask);
				gtk_widget_show(pixmapwid[i]);
				rpbuttons[i] = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
				gtk_container_add(GTK_CONTAINER(rpbuttons[i]), pixmapwid[i]);
				gtk_widget_show(GTK_WIDGET(rpbuttons[i]));
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rpbuttons[i]), (1 << i) & recsrc ? TRUE : FALSE);
				rpcallback[i] = (gint *) gtk_signal_connect(GTK_OBJECT(rpbuttons[i]), "clicked", GTK_SIGNAL_FUNC(SwitchRecordPlayGTK), &rpbuttons[i]);
			}
			if ((1 << i) & stereodevs) {
				if (tmp) {
					balset = (left > right) ? (MAXLEVEL / 2) * right / max : MAXLEVEL - ((MAXLEVEL / 2) * left / max);
				} else {
					balset = (MAXLEVEL / 2);
				}
				balance[i] = GTK_ADJUSTMENT(gtk_adjustment_new(balset, 0, MAXLEVEL, 1, 1, 0));
				balcallback[i] = (gint *) gtk_signal_connect(GTK_OBJECT(balance[i]), "value_changed", GTK_SIGNAL_FUNC(AdjustBalanceGTK), &balance[i]);
				baltracks[i] = gtk_hscale_new(balance[i]);
				gtk_scale_set_value_pos(GTK_SCALE(baltracks[i]), GTK_POS_RIGHT);
				gtk_scale_set_digits(GTK_SCALE(baltracks[i]), 0);
				gtk_widget_show(baltracks[i]);
				gtk_range_set_update_policy(GTK_RANGE(baltracks[i]), GTK_UPDATE_CONTINUOUS);
			}
			items[i] = i;
			itemp[i] = &items[i];
			lvlcallback[i] = (gint *) gtk_signal_connect(GTK_OBJECT(adjustment[i]), "value_changed", GTK_SIGNAL_FUNC(AdjustLevelGTK), &adjustment[i]);
			levels[i] = gtk_hscale_new(adjustment[i]);
			gtk_scale_set_value_pos(GTK_SCALE(levels[i]), GTK_POS_LEFT);
			gtk_scale_set_digits(GTK_SCALE(levels[i]), 0);
			gtk_widget_show(levels[i]);
			gtk_range_set_update_policy(GTK_RANGE(levels[i]), GTK_UPDATE_CONTINUOUS);
		}
	}
	control_box = gtk_hbox_new(FALSE, BORDER);
	gtk_widget_show(control_box);
	label = gtk_label_new(LOCAL_TEXT("Rec"));
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(maintable), label, 0, 1, 0, 1);
	label = gtk_label_new(LOCAL_TEXT("Level"));
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(maintable), label, 2, 3, 0, 1);
	label = gtk_label_new(LOCAL_TEXT("Balance"));
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(maintable), label, 6, 7, 0, 1);
	y = 0;
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		if ((1 << i) & devmask) {
			if ((1 << i) & recmask)
				gtk_table_attach_defaults(GTK_TABLE(maintable), GTK_WIDGET(rpbuttons[i]), 0, 1, y + 1, y + 2);
			channellabel[i] = gtk_label_new(LOCAL_TEXT(dev_label[i]));
			gtk_widget_show(channellabel[i]);
			gtk_table_attach_defaults(GTK_TABLE(maintable), channellabel[i], 4, 5, y + 1, y + 2);
			gtk_table_attach_defaults(GTK_TABLE(maintable), levels[i], 1, 4, y + 1, y + 2);
			if ((1 << i) & stereodevs)
				gtk_table_attach_defaults(GTK_TABLE(maintable), baltracks[i], 5, 8, y + 1, y + 2);
			y++;
		}
	}
	label = gtk_label_new(LOCAL_TEXT("L"));
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(maintable), label, 5, 6, y + 1, y + 2);
	label = gtk_label_new(LOCAL_TEXT("R"));
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(maintable), label, 7, 8, y + 1, y + 2);
	label = gtk_label_new(LOCAL_TEXT("0"));
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(maintable), label, 1, 2, y + 1, y + 2);
	label = gtk_label_new(LOCAL_TEXT("100"));
	gtk_widget_show(label);
	gtk_table_attach_defaults(GTK_TABLE(maintable), label, 3, 4, y + 1, y + 2);
	gtk_box_pack_start(GTK_BOX(mainbox), control_box, TRUE, TRUE, 0);
	gtk_widget_realize(window);
	icon_pixmap = gdk_pixmap_create_from_xpm_d(window->window, &bitmap, &style->bg[GTK_STATE_NORMAL], aumix_xpm);
	gdk_window_set_icon(window->window, NULL, icon_pixmap, bitmap);
	gtk_widget_show(window);
	signal(SIGALRM, (void *) AumixSignalHandlerGTK);
	alarm(REFRESH_PERIOD);
}

void            CloseScreenGTK(void)
{
	gtk_exit(0);
	return;
}

void            AumixSignalHandlerGTK(int signal_number)
{
	/* Handle SIGALRM. */
	int             max, balset, i, tmp, left, right;
	signal(SIGALRM, (void *) AumixSignalHandlerGTK);	/* Reset the signal handler. */
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		if ((1 << i) & devmask) {
			/* Try to read mixer. */
			if (SOUND_IOCTL(mixer_fd, MIXER_READ(i), &tmp) == -1)
				gtk_exit(EREADMIX);
			right = tmp >> 8;
			left = tmp & 0xFF;
			gtk_signal_handler_block(GTK_OBJECT(adjustment[i]), (gint) lvlcallback[i]);
			gtk_adjustment_set_value(adjustment[i], (right + left) / 2);
			gtk_signal_handler_unblock(GTK_OBJECT(adjustment[i]), (gint) lvlcallback[i]);
			max = (left > right) ? left : right;
			if ((1 << i) & recmask) {
				gtk_signal_handler_block(GTK_OBJECT(rpbuttons[i]), (gint) rpcallback[i]);
				ExitIfError(ReadRecSrc());
				gtk_widget_show(pixmapwid[i]);
				gtk_pixmap_set(GTK_PIXMAP(pixmapwid[i]), (1 << i) & recsrc ? record_pixmap : play_pixmap, NULL);
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rpbuttons[i]), (1 << i) & recsrc ? TRUE : FALSE);
				gtk_widget_show(pixmapwid[i]);
				gtk_signal_handler_unblock(GTK_OBJECT(rpbuttons[i]), (gint) rpcallback[i]);
			}
			if ((1 << i) & stereodevs) {
				if (tmp) {
					balset = (left > right) ? (MAXLEVEL / 2) * right / max : MAXLEVEL - ((MAXLEVEL / 2) * left / max);
				} else {
					balset = (MAXLEVEL / 2);
				}
				gtk_signal_handler_block(GTK_OBJECT(balance[i]), (gint) balcallback[i]);
				gtk_adjustment_set_value(balance[i], balset);
				gtk_signal_handler_unblock(GTK_OBJECT(balance[i]), (gint) balcallback[i]);
			}
		}
	}
	alarm(REFRESH_PERIOD);
}

void            FileOKLoad(GtkWidget * w, GtkFileSelection * fs)
/* Get the selected filename and copy it into the global save_filename. */
{
	save_filename = malloc(strlen(gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs))) + 1);
	strcpy(save_filename, gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
	ShowWarning(LoadSettings());
}

void            FileOKSave(GtkWidget * w, GtkFileSelection * fs)
/* Get the selected filename and copy it into the global save_filename. */
{
	save_filename = malloc(strlen(gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs))) + 1);
	strcpy(save_filename, gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
	ExitIfError(SaveSettings());
}

void            LoadDialog(void)
{
	GtkWidget      *filew;
	/* Create a new file selection widget */
	filew = gtk_file_selection_new("File selection");
	gtk_signal_connect(GTK_OBJECT(filew), "destroy", (GtkSignalFunc) aumix_destroy, &filew);
	/* Connect the ok_button to FileOKLoad function */
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filew)->ok_button), "clicked", (GtkSignalFunc) FileOKLoad, filew);
	/* Connect the cancel_button to destroy the widget */
	gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(filew)->cancel_button), "clicked", (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT(filew));
	/* Let's set the filename, as a default. */
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(filew), ".aumixrc");
	gtk_widget_show(filew);
}

void            SaveDialog(void)
{
	GtkWidget      *filew;
	/* Create a new file selection widget */
	filew = gtk_file_selection_new("File selection");
	gtk_signal_connect(GTK_OBJECT(filew), "destroy", (GtkSignalFunc) aumix_destroy, &filew);
	/* Connect the ok_button to FileOKSave function */
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filew)->ok_button), "clicked", (GtkSignalFunc) FileOKSave, filew);
	/* Connect the cancel_button to destroy the widget */
	gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(filew)->cancel_button), "clicked", (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT(filew));
	/* Let's set the filename, as a default. */
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(filew), ".aumixrc");
	gtk_widget_show(filew);
}
#endif				/* HAVE_GTK */

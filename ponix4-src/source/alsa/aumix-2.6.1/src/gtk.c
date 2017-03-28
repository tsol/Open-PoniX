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

void            SaveSettingsGTK(void)
{
	(void) SaveSettings();
}

void            LoadSettingsGTK(void)
{
	(void) LoadSettings();
}

void            HideShowGTK(int device)
{
	if (visible[device]) {
		/* Hide the record/play button, if any. */
		if ((1 << device) & recmask)
			gtk_widget_hide(GTK_WIDGET(rpbuttons[device]));
		/* Hide the level track. */
		gtk_scale_set_draw_value(GTK_SCALE(levels[device]), FALSE);
		gtk_widget_hide(GTK_WIDGET(levels[device]));
/* Hide the name of the channel. */
		gtk_widget_hide(channellabel[device]);
		/* Hide the balance track, if any. */
		if ((1 << device) & stereodevs) {
/* Hide the balance track, if any. */
			gtk_scale_set_draw_value(GTK_SCALE(baltracks[device]), FALSE);
			gtk_widget_hide(baltracks[device]);
		}
	} else {
		/* Show the record/play button, if any. */
		if ((1 << device) & recmask)
			gtk_widget_show(GTK_WIDGET(rpbuttons[device]));
		/* Show the level track. */
		gtk_scale_set_draw_value(GTK_SCALE(levels[device]), TRUE);
		gtk_widget_show(GTK_WIDGET(levels[device]));
		/* Show the name of the channel. */
		gtk_widget_show(channellabel[device]);
		/* Show the balance track, if any. */
		if ((1 << device) & stereodevs) {
			/* Show the balance track, if any. */
			gtk_scale_set_draw_value(GTK_SCALE(baltracks[device]), TRUE);
			gtk_widget_show(baltracks[device]);
		}
	}
/* Toggle visibility flag for the device. */
	visible[device] = !visible[device];
}

void            AdjustLevelGTK(int device)
{
	AdjustLevel(device, 0, adjustment[device]->value);
	return;
}

void            AdjustBalanceGTK(int device)
{
	AdjustBalance(device, 0, balance[device]->value);
	return;
}

void            InitScreenGTK(void)
{
	GdkColormap    *colormap;
	GdkBitmap      *bitmap;
	GdkPixmap      *icon_pixmap;
	GtkStyle       *style;
	GtkWidget      *window;
	GtkWidget      *mainbox;
	GtkWidget      *filemenu;
	GtkWidget      *filemenuitems[5];
	GtkWidget      *viewmenu;
	GtkWidget      *viewmenuitems[SOUND_MIXER_NRDEVICES];
	GtkWidget      *mutemenu;
	GtkWidget      *muteallitem;
	GtkWidget      *menubar;
	GtkWidget      *menubarlabel;
	GtkAccelGroup  *accel;
	GtkWidget      *label;
	GtkWidget      *control_box;
	int             balset, max, y, tmp, left, right;
	long int        i;
	GtkSignalFunc   filefuncs[] = {&LoadSettingsGTK, &SaveSettingsGTK, &LoadDialog, &SaveDialog, &CloseScreenGTK};
	char           *filemenutext[] = {LOCAL_TEXT_NOOP("Load"), LOCAL_TEXT_NOOP("Save"), LOCAL_TEXT_NOOP("Load From"), LOCAL_TEXT_NOOP("Save To"), LOCAL_TEXT_NOOP("Quit")};
	char           *fileacceltext[5] = {"l", "s", "f", "t", "q"};
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
	viewmenu = gtk_menu_new();
	for (i = 0; i < 5; i++) {
		filemenuitems[i] = gtk_menu_item_new_with_label(LOCAL_TEXT(filemenutext[i]));
		gtk_menu_append(GTK_MENU(filemenu), filemenuitems[i]);
		gtk_widget_show(filemenuitems[i]);
		gtk_widget_add_accelerator(filemenuitems[i], "activate", accel, *LOCAL_TEXT(fileacceltext[i]), 0, GTK_ACCEL_VISIBLE);
		gtk_signal_connect(GTK_OBJECT(filemenuitems[i]), "activate", filefuncs[i], NULL);
	}
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		visible[i] = TRUE;
		if ((1 << i) & devmask) {
			viewmenuitems[i] = gtk_check_menu_item_new_with_label(LOCAL_TEXT(dev_label[i]));
			gtk_menu_append(GTK_MENU(viewmenu), viewmenuitems[i]);
			gtk_widget_show(viewmenuitems[i]);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(viewmenuitems[i]), TRUE);
		}
	}
	muteallitem = gtk_check_menu_item_new_with_label(LOCAL_TEXT("Mute All"));
	gtk_menu_append(GTK_MENU(mutemenu), muteallitem);
/* XXX The "m" needs to get translated. */
	gtk_widget_add_accelerator(muteallitem, "activate", accel, 'm', 0, GTK_ACCEL_VISIBLE);
	gtk_widget_show(muteallitem);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(muteallitem), TRUE);
	menubarlabel = gtk_menu_item_new_with_label(LOCAL_TEXT("File"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubarlabel), filemenu);
	gtk_menu_bar_append(GTK_MENU_BAR(menubar), menubarlabel);
	gtk_widget_show(menubarlabel);
	menubarlabel = gtk_menu_item_new_with_label(LOCAL_TEXT("View"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menubarlabel), viewmenu);
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
				ErrorExitWarn(ReadRecSrc(), 'e');
				/* a pixmap widget to contain the pixmap */
				pixmapwid[i] = gtk_pixmap_new((1 << i) & recsrc ? record_pixmap : play_pixmap, (1 << i) & recsrc ? rmask : pmask);
				gtk_widget_show(pixmapwid[i]);
				rpbuttons[i] = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
				gtk_container_add(GTK_CONTAINER(rpbuttons[i]), pixmapwid[i]);
				gtk_widget_show(GTK_WIDGET(rpbuttons[i]));
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rpbuttons[i]), (1 << i) & recsrc ? TRUE : FALSE);
				rpcallback[i] = gtk_signal_connect_object(GTK_OBJECT(rpbuttons[i]), "clicked", GTK_SIGNAL_FUNC(SwitchRecordPlay), (gpointer) i);
			}
			if ((1 << i) & stereodevs) {
				if (tmp) {
					balset = (left > right) ? (MAXLEVEL / 2) * right / max : MAXLEVEL - ((MAXLEVEL / 2) * left / max);
				} else {
					balset = (MAXLEVEL / 2);
				}
				balance[i] = GTK_ADJUSTMENT(gtk_adjustment_new(balset, 0, MAXLEVEL, 1, 1, 0));
				balcallback[i] = gtk_signal_connect_object(GTK_OBJECT(balance[i]), "value_changed", GTK_SIGNAL_FUNC(AdjustBalanceGTK), (gpointer) i);
				baltracks[i] = gtk_hscale_new(balance[i]);
				gtk_scale_set_value_pos(GTK_SCALE(baltracks[i]), GTK_POS_RIGHT);
				gtk_scale_set_digits(GTK_SCALE(baltracks[i]), 0);
				gtk_widget_show(baltracks[i]);
				gtk_range_set_update_policy(GTK_RANGE(baltracks[i]), GTK_UPDATE_CONTINUOUS);
			}
			lvlcallback[i] = gtk_signal_connect_object(GTK_OBJECT(adjustment[i]), "value_changed", GTK_SIGNAL_FUNC(AdjustLevelGTK), (gpointer) i);
			levels[i] = gtk_hscale_new(GTK_ADJUSTMENT(adjustment[i]));
			gtk_signal_connect_object(GTK_OBJECT(viewmenuitems[i]), "activate", GTK_SIGNAL_FUNC(HideShowGTK), (gpointer) i);
			gtk_scale_set_value_pos(GTK_SCALE(levels[i]), GTK_POS_LEFT);
			gtk_scale_set_digits(GTK_SCALE(levels[i]), 0);
			gtk_widget_show(GTK_WIDGET(levels[i]));
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
			gtk_table_attach_defaults(GTK_TABLE(maintable), GTK_WIDGET(levels[i]), 1, 4, y + 1, y + 2);
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
	signal(SIGALRM, AumixSignalHandler);
	alarm(REFRESH_PERIOD);
}

void            CloseScreenGTK(void)
{
	gtk_exit(0);
	return;
}

void            WakeUpGTK(void)
{
	/* things to do on SIGALRM */
	int             max, balset, i, tmp, left, right;
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		if ((1 << i) & devmask) {
			/* Try to read mixer. */
			if (SOUND_IOCTL(mixer_fd, MIXER_READ(i), &tmp) == -1)
				gtk_exit(EREADMIX);
			right = tmp >> 8;
			left = tmp & 0xFF;
			gtk_signal_handler_block(GTK_OBJECT(adjustment[i]), lvlcallback[i]);
			gtk_adjustment_set_value(adjustment[i], (right + left) / 2);
			gtk_signal_handler_unblock(GTK_OBJECT(adjustment[i]), lvlcallback[i]);
			max = (left > right) ? left : right;
			if ((1 << i) & recmask) {
				gtk_signal_handler_block(GTK_OBJECT(rpbuttons[i]), rpcallback[i]);
				ErrorExitWarn(ReadRecSrc(), 'e');
				gtk_widget_show(pixmapwid[i]);
				gtk_pixmap_set(GTK_PIXMAP(pixmapwid[i]), (1 << i) & recsrc ? record_pixmap : play_pixmap, NULL);
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rpbuttons[i]), (1 << i) & recsrc ? TRUE : FALSE);
				gtk_widget_show(pixmapwid[i]);
				gtk_signal_handler_unblock(GTK_OBJECT(rpbuttons[i]), rpcallback[i]);
			}
			if ((1 << i) & stereodevs) {
				if (tmp) {
					balset = (left > right) ? (MAXLEVEL / 2) * right / max : MAXLEVEL - ((MAXLEVEL / 2) * left / max);
				} else {
					balset = (MAXLEVEL / 2);
				}
				gtk_signal_handler_block(GTK_OBJECT(balance[i]), balcallback[i]);
				gtk_adjustment_set_value(balance[i], balset);
				gtk_signal_handler_unblock(GTK_OBJECT(balance[i]), balcallback[i]);
			}
		}
	}
}

void            FileOKLoad(GtkWidget * w, GtkFileSelection * fs)
/* Get the selected filename and copy it into the global save_filename. */
{
	save_filename = malloc(strlen(gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs))) + 1);
	strcpy(save_filename, gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
	ErrorExitWarn(LoadSettings(), 'w');
}

void            FileOKSave(GtkWidget * w, GtkFileSelection * fs)
/* Get the selected filename and copy it into the global save_filename. */
{
	save_filename = malloc(strlen(gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs))) + 1);
	strcpy(save_filename, gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
	ErrorExitWarn(SaveSettings(), 'e');
}

void            LoadDialog(void)
{
	GtkWidget      *filew;
	/* Create a new file selection widget */
	filew = gtk_file_selection_new("File selection");
	gtk_signal_connect(GTK_OBJECT(filew), "destroy", (GtkSignalFunc) gtk_widget_destroy, &filew);
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
	gtk_signal_connect(GTK_OBJECT(filew), "destroy", (GtkSignalFunc) gtk_widget_destroy, &filew);
	/* Connect the ok_button to FileOKSave function */
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filew)->ok_button), "clicked", (GtkSignalFunc) FileOKSave, filew);
	/* Connect the cancel_button to destroy the widget */
	gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(filew)->cancel_button), "clicked", (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT(filew));
	/* Let's set the filename, as a default. */
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(filew), ".aumixrc");
	gtk_widget_show(filew);
}
#endif				/* HAVE_GTK */

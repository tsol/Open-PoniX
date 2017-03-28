/* $Aumix: aumix/src/gtk.c,v 1.11 2010/04/26 10:24:07 trevor Exp $
 *
 * gtk.c:  GTK+ interface for aumix
 * (c) 1998-2001 the authors (see AUTHORS file)
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
 *
 * This file is part of aumix.
 *
 * Aumix is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * Aumix is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * aumix; if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA 02111-1307, USA.
 */

#include "common.h"
#if defined (HAVE_GTK)
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

void            HideShowNumbersGTK(void)
{
	int             ii;
	if (valuesvisible) {
		for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
			if ((1 << ii) & devmask && visible[ii]) {
				gtk_scale_set_draw_value(GTK_SCALE(levels[ii]), FALSE);
				if ((1 << ii) & stereodevs)
					gtk_scale_set_draw_value(GTK_SCALE(baltracks[ii]), FALSE);
			}
		}
		gtk_widget_hide(labelzero);
		gtk_widget_hide(labelcent);
	} else {
		for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
			if ((1 << ii) & devmask && visible[ii]) {
				gtk_scale_set_draw_value(GTK_SCALE(levels[ii]), TRUE);
				if ((1 << ii) & stereodevs)
					gtk_scale_set_draw_value(GTK_SCALE(baltracks[ii]), TRUE);
			}
		}
		gtk_widget_show(labelzero);
		gtk_widget_show(labelcent);
	}
	valuesvisible = !valuesvisible;
}

void            HideShowBalancesGTK(void)
{
	int             ii;
	if (balancevisible) {
		gtk_widget_hide(labelbalance);
		gtk_widget_hide(labelleft);
		gtk_widget_hide(labelright);
		for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
			if (visible[ii] && (1 << ii) & stereodevs)
				gtk_widget_hide(baltracks[ii]);
		}
	} else {
		gtk_widget_show(labelbalance);
		gtk_widget_show(labelleft);
		gtk_widget_show(labelright);
		for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
			if (visible[ii] && (1 << ii) & stereodevs)
				gtk_widget_show(baltracks[ii]);
		}
	}
	balancevisible = !balancevisible;
}

void            HideShowMenuGTK(GtkWidget * menu)
{
	if (menuvisible) {
		gtk_widget_hide(GTK_WIDGET(menu));
	} else {
		gtk_widget_show(GTK_WIDGET(menu));
	}
	menuvisible = !menuvisible;
}

void            HideShowChannelGTK(int device)
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
		if ((1 << device) & stereodevs && balancevisible) {
			/* Show the balance track, if any. */
			gtk_scale_set_draw_value(GTK_SCALE(baltracks[device]), TRUE);
			gtk_widget_show(baltracks[device]);
		}
	}
/* Toggle visibility flag for the device. */
	visible[device] = !visible[device];
}

void            HideShowRecPlayGTK(void)
{
	int             ii;
	recplayvisible = !recplayvisible;
	if (recplayvisible) {
		gtk_widget_show(labelrec);
	} else {
		gtk_widget_hide(labelrec);
	}
	for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
		if (recplayvisible) {
			/* Show the record/play button, if any. */
			if ((1 << ii) & recmask)
				gtk_widget_show(GTK_WIDGET(rpbuttons[ii]));
		} else {
			/* Hide the record/play button, if any. */
			if ((1 << ii) & recmask)
				gtk_widget_hide(GTK_WIDGET(rpbuttons[ii]));
		}
	}
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
	GtkWidget      *viewmenuitems[SOUND_MIXER_NRDEVICES + 4];
	GtkWidget      *separator;
	GtkWidget      *mutemenu;
	GtkWidget      *muteallitem;
	GtkWidget      *menubar;
	GtkWidget      *menubarlabel;
	GtkAccelGroup  *accel;
	GtkWidget      *labellevel;
	GtkWidget      *control_box;
	int             balset, max, y, tmp, left, right;
	long int        ii;
	GtkSignalFunc   filefuncs[] = {&LoadSettingsGTK, &SaveSettingsGTK, &LoadDialog, &SaveDialog, &CloseScreenGTK};
	char           *filemenutext[] = {LOCAL_TEXT_NOOP("Load"), LOCAL_TEXT_NOOP("Save"), LOCAL_TEXT_NOOP("Load From"), LOCAL_TEXT_NOOP("Save To"), LOCAL_TEXT_NOOP("Quit")};
	char           *fileacceltext[5] = {LOCAL_TEXT_NOOP("l"), LOCAL_TEXT_NOOP("s"), LOCAL_TEXT_NOOP("f"), LOCAL_TEXT_NOOP("t"), LOCAL_TEXT_NOOP("q")};
	char           *viewmenutext[] = {LOCAL_TEXT_NOOP("Menu"), LOCAL_TEXT_NOOP("Balance"), LOCAL_TEXT_NOOP("Record/Play"), LOCAL_TEXT_NOOP("Numbers")};
	char           *viewacceltext[] = {LOCAL_TEXT_NOOP("v"), LOCAL_TEXT_NOOP("b"), LOCAL_TEXT_NOOP("r"), LOCAL_TEXT_NOOP("n")};
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "aumix");
	gtk_signal_connect(GTK_OBJECT(window), "destroy", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
	gtk_container_border_width(GTK_CONTAINER(window), BORDER);
	mainbox = gtk_vbox_new(FALSE, BORDER);
	gtk_widget_show(mainbox);
	gtk_container_add(GTK_CONTAINER(window), mainbox);
	accel = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(window), accel);
	menubar = gtk_menu_bar_new();
	gtk_widget_show(menubar);
	gtk_box_pack_start(GTK_BOX(mainbox), menubar, FALSE, TRUE, 0);
	filemenu = gtk_menu_new();
	mutemenu = gtk_menu_new();
	viewmenu = gtk_menu_new();
	for (ii = 0; ii < 5; ii++) {
		filemenuitems[ii] = gtk_menu_item_new_with_label(LOCAL_TEXT(filemenutext[ii]));
		gtk_menu_append(GTK_MENU(filemenu), filemenuitems[ii]);
		gtk_widget_show(filemenuitems[ii]);
		gtk_widget_add_accelerator(filemenuitems[ii], "activate", accel, *LOCAL_TEXT(fileacceltext[ii]), 0, GTK_ACCEL_VISIBLE);
		gtk_signal_connect(GTK_OBJECT(filemenuitems[ii]), "activate", filefuncs[ii], NULL);
	}
	for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
		visible[ii] = TRUE;
		if ((1 << ii) & (devmask | recmask)) {
			viewmenuitems[ii] = gtk_check_menu_item_new_with_label(LOCAL_TEXT(dev_label[ii]));
			gtk_menu_append(GTK_MENU(viewmenu), viewmenuitems[ii]);
			gtk_widget_show(viewmenuitems[ii]);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(viewmenuitems[ii]), TRUE);
		}
	}
	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_menu_append(GTK_MENU(viewmenu), separator);
	menuvisible = TRUE;
	balancevisible = TRUE;
	recplayvisible = TRUE;
	valuesvisible = TRUE;
	viewmenuitems[SOUND_MIXER_NRDEVICES] = gtk_check_menu_item_new_with_label(LOCAL_TEXT("Menu"));
	viewmenuitems[SOUND_MIXER_NRDEVICES + 1] = gtk_check_menu_item_new_with_label(LOCAL_TEXT("Balance"));
	viewmenuitems[SOUND_MIXER_NRDEVICES + 2] = gtk_check_menu_item_new_with_label(LOCAL_TEXT("Record/Play"));
	viewmenuitems[SOUND_MIXER_NRDEVICES + 3] = gtk_check_menu_item_new_with_label(LOCAL_TEXT("Numbers"));
	for (ii = 0; ii < 4; ii++) {
		gtk_menu_append(GTK_MENU(viewmenu), viewmenuitems[SOUND_MIXER_NRDEVICES + ii]);
	}
	gtk_widget_add_accelerator(viewmenuitems[SOUND_MIXER_NRDEVICES], "activate", accel, *LOCAL_TEXT("v"), 0, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(viewmenuitems[SOUND_MIXER_NRDEVICES + 1], "activate", accel, *LOCAL_TEXT("b"), 0, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(viewmenuitems[SOUND_MIXER_NRDEVICES + 2], "activate", accel, *LOCAL_TEXT("r"), 0, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(viewmenuitems[SOUND_MIXER_NRDEVICES + 3], "activate", accel, *LOCAL_TEXT("n"), 0, GTK_ACCEL_VISIBLE);
	for (ii = 0; ii < 4; ii++) {
		gtk_widget_show(viewmenuitems[SOUND_MIXER_NRDEVICES + ii]);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(viewmenuitems[SOUND_MIXER_NRDEVICES + ii]), TRUE);
	}
	muteallitem = gtk_check_menu_item_new_with_label(LOCAL_TEXT("Mute All"));
	gtk_menu_append(GTK_MENU(mutemenu), muteallitem);
/* XXX The "m" needs to get translated. */
	gtk_widget_add_accelerator(muteallitem, "activate", accel, 'm', 0, GTK_ACCEL_VISIBLE);
	gtk_widget_show(muteallitem);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(muteallitem), FALSE);
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
	for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
		if ((1 << ii) & devmask) {
			/* Try to read mixer. */
			if (SOUND_IOCTL(mixer_fd, MIXER_READ(ii), &tmp) == -1)
				gtk_exit(EREADMIX);
			right = tmp >> 8;
			left = tmp & 0xFF;
			max = (left > right) ? left : right;
			/* See rangewidgets example. */
			adjustment[ii] = GTK_ADJUSTMENT(gtk_adjustment_new(((right + left) / 2), 0, MAXLEVEL, 1, 1, 0));
			if ((1 << ii) & stereodevs) {
				if (tmp) {
					balset = (left > right) ? (MAXLEVEL / 2) * right / max : MAXLEVEL - ((MAXLEVEL / 2) * left / max);
				} else {
					balset = (MAXLEVEL / 2);
				}
				balance[ii] = GTK_ADJUSTMENT(gtk_adjustment_new(balset, 0, MAXLEVEL, 1, 1, 0));
				balcallback[ii] = gtk_signal_connect_object(GTK_OBJECT(balance[ii]), "value_changed", GTK_SIGNAL_FUNC(AdjustBalanceGTK), (gpointer) ii);
				baltracks[ii] = gtk_hscale_new(balance[ii]);
				gtk_scale_set_value_pos(GTK_SCALE(baltracks[ii]), GTK_POS_RIGHT);
				gtk_scale_set_digits(GTK_SCALE(baltracks[ii]), 0);
#ifdef HAVE_GTK
				gtk_widget_set_size_request(GTK_WIDGET (baltracks[ii]), 84, -1);
#endif
				gtk_widget_show(baltracks[ii]);
				gtk_range_set_update_policy(GTK_RANGE(baltracks[ii]), GTK_UPDATE_CONTINUOUS);
			}
			lvlcallback[ii] = gtk_signal_connect_object(GTK_OBJECT(adjustment[ii]), "value_changed", GTK_SIGNAL_FUNC(AdjustLevelGTK), (gpointer) ii);
			levels[ii] = gtk_hscale_new(GTK_ADJUSTMENT(adjustment[ii]));
			gtk_signal_connect_object(GTK_OBJECT(viewmenuitems[ii]), "activate", GTK_SIGNAL_FUNC(HideShowChannelGTK), (gpointer) ii);
			gtk_scale_set_value_pos(GTK_SCALE(levels[ii]), GTK_POS_LEFT);
			gtk_scale_set_digits(GTK_SCALE(levels[ii]), 0);
#ifdef HAVE_GTK
			gtk_widget_set_size_request(GTK_WIDGET (levels[ii]), 84, -1);
#endif
			gtk_widget_show(GTK_WIDGET(levels[ii]));
			gtk_range_set_update_policy(GTK_RANGE(levels[ii]), GTK_UPDATE_CONTINUOUS);
		}
		if ((1 << ii) & recmask) {
			colormap = gtk_widget_get_colormap(window);
			record_pixmap = gdk_pixmap_colormap_create_from_xpm_d(window->window, colormap, &rmask, &style->bg[GTK_STATE_NORMAL], (gchar **) record_xpm);
			play_pixmap = gdk_pixmap_colormap_create_from_xpm_d(window->window, colormap, &pmask, &style->bg[GTK_STATE_NORMAL], (gchar **) play_xpm);
			ErrorExitWarn(ReadRecSrc(), 'e');
			/* a pixmap widget to contain the pixmap */
			pixmapwid[ii] = gtk_pixmap_new((1 << ii) & recsrc ? record_pixmap : play_pixmap, (1 << ii) & recsrc ? rmask : pmask);
			gtk_widget_show(pixmapwid[ii]);
			rpbuttons[ii] = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
			gtk_container_add(GTK_CONTAINER(rpbuttons[ii]), pixmapwid[ii]);
			gtk_widget_show(GTK_WIDGET(rpbuttons[ii]));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rpbuttons[ii]), (1 << ii) & recsrc ? TRUE : FALSE);
			rpcallback[ii] = gtk_signal_connect_object(GTK_OBJECT(rpbuttons[ii]), "clicked", GTK_SIGNAL_FUNC(SwitchRecordPlay), (gpointer) ii);
		}
	}
	gtk_signal_connect_object(GTK_OBJECT(viewmenuitems[SOUND_MIXER_NRDEVICES]), "activate", GTK_SIGNAL_FUNC(HideShowMenuGTK), GTK_OBJECT(menubar));
	gtk_signal_connect_object(GTK_OBJECT(viewmenuitems[SOUND_MIXER_NRDEVICES + 1]), "activate", GTK_SIGNAL_FUNC(HideShowBalancesGTK), 0);
	gtk_signal_connect_object(GTK_OBJECT(viewmenuitems[SOUND_MIXER_NRDEVICES + 2]), "activate", GTK_SIGNAL_FUNC(HideShowRecPlayGTK), 0);
	gtk_signal_connect_object(GTK_OBJECT(viewmenuitems[SOUND_MIXER_NRDEVICES + 3]), "activate", GTK_SIGNAL_FUNC(HideShowNumbersGTK), 0);
	control_box = gtk_hbox_new(FALSE, BORDER);
	gtk_widget_show(control_box);
	labelrec = gtk_label_new(LOCAL_TEXT("Rec"));
	gtk_widget_show(labelrec);
	gtk_table_attach_defaults(GTK_TABLE(maintable), labelrec, 0, 1, 0, 1);
	labellevel = gtk_label_new(LOCAL_TEXT("Level"));
	gtk_widget_show(labellevel);
	gtk_table_attach_defaults(GTK_TABLE(maintable), labellevel, 2, 3, 0, 1);
	labelbalance = gtk_label_new(LOCAL_TEXT("Balance"));
	gtk_widget_show(labelbalance);
	gtk_table_attach_defaults(GTK_TABLE(maintable), labelbalance, 6, 7, 0, 1);
	y = 0;
	for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
		if ((1 << ii) & (devmask | recmask)) {
			if ((1 << ii) & recmask)
				gtk_table_attach_defaults(GTK_TABLE(maintable), GTK_WIDGET(rpbuttons[ii]), 0, 1, y + 1, y + 2);
			channellabel[ii] = gtk_label_new(LOCAL_TEXT(dev_label[ii]));
			gtk_widget_show(channellabel[ii]);
			gtk_table_attach_defaults(GTK_TABLE(maintable), channellabel[ii], 4, 5, y + 1, y + 2);
			if ((1 << ii) & devmask) {
				gtk_table_attach_defaults(GTK_TABLE(maintable), GTK_WIDGET(levels[ii]), 1, 4, y + 1, y + 2);
				if ((1 << ii) & stereodevs)
					gtk_table_attach_defaults(GTK_TABLE(maintable), baltracks[ii], 5, 8, y + 1, y + 2);
			}
			y++;
		}
	}
	labelleft = gtk_label_new(LOCAL_TEXT("L"));
	gtk_widget_show(labelleft);
	gtk_table_attach_defaults(GTK_TABLE(maintable), labelleft, 5, 6, y + 1, y + 2);
	labelright = gtk_label_new(LOCAL_TEXT("R"));
	gtk_widget_show(labelright);
	gtk_table_attach_defaults(GTK_TABLE(maintable), labelright, 7, 8, y + 1, y + 2);
	labelzero = gtk_label_new(LOCAL_TEXT("0"));
	gtk_widget_show(labelzero);
	gtk_table_attach_defaults(GTK_TABLE(maintable), labelzero, 1, 2, y + 1, y + 2);
	labelcent = gtk_label_new(LOCAL_TEXT("100"));
	gtk_widget_show(labelcent);
	gtk_table_attach_defaults(GTK_TABLE(maintable), labelcent, 3, 4, y + 1, y + 2);
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
	int             max, balset, ii, tmp, left, right;
	for (ii = 0; ii < SOUND_MIXER_NRDEVICES; ii++) {
		if ((1 << ii) & devmask) {
			/* Try to read mixer. */
			if (SOUND_IOCTL(mixer_fd, MIXER_READ(ii), &tmp) == -1)
				gtk_exit(EREADMIX);
			right = tmp >> 8;
			left = tmp & 0xFF;
			gtk_signal_handler_block(GTK_OBJECT(adjustment[ii]), lvlcallback[ii]);
			gtk_adjustment_set_value(adjustment[ii], (right + left) / 2);
			gtk_signal_handler_unblock(GTK_OBJECT(adjustment[ii]), lvlcallback[ii]);
			max = (left > right) ? left : right;
			if ((1 << ii) & stereodevs) {
				if (tmp) {
					balset = (left > right) ? (MAXLEVEL / 2) * right / max : MAXLEVEL - ((MAXLEVEL / 2) * left / max);
				} else {
					balset = (MAXLEVEL / 2);
				}
				gtk_signal_handler_block(GTK_OBJECT(balance[ii]), balcallback[ii]);
				gtk_adjustment_set_value(balance[ii], balset);
				gtk_signal_handler_unblock(GTK_OBJECT(balance[ii]), balcallback[ii]);
			}
		}
		if ((1 << ii) & recmask) {
			gtk_signal_handler_block(GTK_OBJECT(rpbuttons[ii]), rpcallback[ii]);
			ErrorExitWarn(ReadRecSrc(), 'e');
			gtk_widget_show(pixmapwid[ii]);
			gtk_pixmap_set(GTK_PIXMAP(pixmapwid[ii]), (1 << ii) & recsrc ? record_pixmap : play_pixmap, NULL);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rpbuttons[ii]), (1 << ii) & recsrc ? TRUE : FALSE);
			gtk_widget_show(pixmapwid[ii]);
			gtk_signal_handler_unblock(GTK_OBJECT(rpbuttons[ii]), rpcallback[ii]);
		}
	}
}

void            FileOKLoad(GtkWidget * w, GtkFileSelection * fs)
/* Get the selected filename and copy it into the global save_filename. */
{
	save_filename = g_strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
	gtk_widget_destroy(fs);
	ErrorExitWarn(LoadSettings(), 'w');
}

void            FileOKSave(GtkWidget * w, GtkFileSelection * fs)
/* Get the selected filename and copy it into the global save_filename. */
{
	save_filename = g_strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs)));
	gtk_widget_destroy(fs);
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

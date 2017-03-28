/* clock.c -- clock module of fbpanel project
 *
 * Copyright (C) 2002-2003 Anatoly Asviyan <aanatoly@users.sf.net>
 *                    2003 tooar <tooar@users.sf.net>
 * This file is part of fbpanel.
 *
 * fbpanel is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * fbpanel is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sawfish; see the file COPYING.   If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "fbpanel.h"



#define DEFAULT_TIP_FORMAT    "%A %x"
#define DEFAULT_CLOCK_FORMAT  "%R"

typedef struct {
    GtkWidget *main;
    GtkWidget *clockw;
    GtkTooltips *tip;
    char *tfmt;
    char *cfmt;
    char *action;
    short lastDay;
} dclock;

static dclock me;

static gint
clock_update( gpointer data )
{
    char output [40] ;
    time_t now ;
    struct tm * detail ;

    time(&now);
    detail = localtime(&now);
    strftime(output, sizeof(output), me.cfmt, detail) ;
    gtk_label_set_text (GTK_LABEL(me.clockw), output) ;

    if (detail->tm_mday != me.lastDay) {
	me.lastDay = detail->tm_mday ;

	strftime (output, sizeof(output), me.tfmt, detail) ;
	gtk_tooltips_set_tip(me.tip, me.main, output, NULL) ;
    }
    return TRUE ;
}

static void
load_config(FILE *fp)
{
    gchar str[512];
    
    while (myfgets(str, 512, fp)) {
        if (!strncmp(str, "Clock::", strlen("Clock::"))) 
	    me.cfmt = g_strdup(str + strlen("Clock::"));
        else if (!strncmp(str, "Tooltip::", strlen("Tooltip::"))) 
	    me.tfmt = g_strdup(str + strlen("Tooltip::"));
        else if (!strncmp(str, "Action::", strlen("Action::"))) 
	    me.action = g_strdup(str + strlen("Action::"));
        else
            printf("unknown token: %s\n", str);
    }
   
               
}

static void
complete_config()
{
    if (!me.cfmt)  
        me.cfmt = g_strdup(DEFAULT_CLOCK_FORMAT);
    if (!me.tfmt)  
        me.tfmt = g_strdup(DEFAULT_TIP_FORMAT);
}

static  void
clicked( GtkWidget *widget, gpointer data )
{
    system(me.action);
}



GtkWidget *
clock_init(FILE *fp)
{
    if (fp)
        load_config(fp);
    complete_config();
    
    me.main = gtk_button_new();
    if (me.action)
        gtk_signal_connect (GTK_OBJECT (me.main), "clicked",
                            GTK_SIGNAL_FUNC (clicked), NULL);
    {
	char output [40] ;
	time_t now ;
	struct tm * detail ;

	time(&now);
	detail = localtime(&now);
	strftime(output, sizeof(output), me.cfmt, detail) ;
	me.clockw = gtk_label_new(output);
    }
    gtk_widget_show(me.clockw);
    gtk_container_add(GTK_CONTAINER(me.main), me.clockw);
    gtk_widget_show(me.main);
    me.tip = gtk_tooltips_new();
    gtk_timeout_add(1000, clock_update, NULL);
    
    return me.main;
}

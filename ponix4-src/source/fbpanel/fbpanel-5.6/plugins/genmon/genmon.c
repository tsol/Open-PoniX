/* genmon_priv.c -- Generic monitor plugin for fbpanel
 *
 * Copyright (C) 2007 Davide Truffa <davide@catoblepa.org>
 * 
 * This plugin is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 dated June, 1991.
 * 
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "panel.h"
#include "misc.h"
#include "plugin.h"

//#define DEBUG
#include "dbg.h"

typedef struct {
    guint time;
    int timer;
    int max_text_len;
    char *command;
    char *textsize;
    char *textcolor;
    GtkWidget *main;
} genmon_priv;


static int
text_update(genmon_priv *gm)
{
    FILE *fp;  
    char text[256];
    char *markup;
    int len;

    ENTER;
    fp = popen(gm->command, "r");
    fgets(text, sizeof(text), fp);
    pclose(fp);
    len = strlen(text) - 1;
    if (len >= 0) {
        if (text[len] == '\n')
            text[len] = 0;

        markup = g_markup_printf_escaped ("<span size='%s' foreground='%s'>%s</span>", 
                gm->textsize, gm->textcolor, text);
        gtk_label_set_markup (GTK_LABEL(gm->main), markup);
        g_free(markup);
    }
    RET(TRUE);
}

  
static void
genmon_destructor(plugin_instance *p)
{
    genmon_priv *gm = p->priv;

    ENTER;
    if (gm->timer) {
        g_source_remove(gm->timer);
    }
    g_free(gm->command);
    g_free(gm->textsize);
    g_free(gm->textcolor);
    g_free(gm);
    RET();
}


static int
genmon_constructor(plugin_instance *p)
{
    genmon_priv *gm;
    line s;

    ENTER;
    gm = g_new0(genmon_priv, 1);
    p->priv = gm;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_NONE) {
            ERR( "genmon-plugin: illegal token %s\n", s.str);
            goto error;
        }
        if (s.type == LINE_VAR) {
            if (!g_ascii_strcasecmp(s.t[0], "Command")) {
                gm->command = g_strdup(s.t[1]);
            } else if (!g_ascii_strcasecmp(s.t[0], "PollingTime")) {
                gm->time = strtol(s.t[1],NULL,10);
            } else if (!g_ascii_strcasecmp(s.t[0], "MaxTextLength")) {
                gm->max_text_len = strtol(s.t[1],NULL,10);
            } else if (!g_ascii_strcasecmp(s.t[0], "TextSize")) {
                gm->textsize = g_strdup(s.t[1]);
            } else if (!g_ascii_strcasecmp(s.t[0], "TextColor")) {
                gm->textcolor = g_strdup(s.t[1]);
            } else {
                ERR( "genmon-plugin: unknown var %s\n", s.t[0]);
                goto error;
            }
        }
    }
    if (!gm->time) 
        gm->time = 1;
    if (!gm->max_text_len)
        gm->max_text_len = 30;
    if (!gm->textsize)
        gm->textsize = g_strdup("medium");
    if (!gm->textcolor)
        gm->textcolor = g_strdup("darkblue");
    if (!gm->command)
        gm->command = g_strdup("date +%R");
    gm->main = gtk_label_new(NULL);
    gtk_label_set_max_width_chars(GTK_LABEL(gm->main), gm->max_text_len);
    text_update(gm);
    gtk_container_set_border_width (GTK_CONTAINER (p->pwid), 1);
    gtk_container_add(GTK_CONTAINER(p->pwid), gm->main);
    gtk_widget_show_all(p->pwid);
    gm->timer = g_timeout_add(gm->time*1000, (GSourceFunc) text_update, (gpointer)gm);
    
    RET(1);

error:
    genmon_destructor(p);
    RET(0);
}


static plugin_class class = {
    .count       = 0,
    .type        = "genmon",
    .name        = "Generic Monitor",
    .version     = "0.3",
    .description = "Display the output of a program/script into the panel",
    .priv_size   = sizeof(genmon_priv),

    .constructor = genmon_constructor,
    .destructor  = genmon_destructor,
};
static plugin_class *class_ptr = (plugin_class *) &class;

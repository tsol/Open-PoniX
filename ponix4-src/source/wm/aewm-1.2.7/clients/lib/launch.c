/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <stdlib.h>
#include <unistd.h>
#include "parser.h"
#include "launch.h"

/* FIXME: this needs some error handling. and documentation. */

static void make_launch_menu_helper(FILE *rc, menu_t menu,
    add_item_func add_item_cb, add_submenu_func add_submenu_cb)
{
    char buf[BUF_SIZE], token[BUF_SIZE], *p;

    while (fgets(buf, sizeof buf, rc)) {
        /* comments and blank lines are skipped */
        if (buf[0] == '#' || buf[0] == '\n') continue;

        p = buf;
        while (get_token(&p, token)) {

            /* start a new sub-menu */
            if (strcmp(token, "menu") == 0) {
                menu_t newmenu;
                if (get_token(&p, token)) {
                    newmenu = add_submenu_cb(menu, token);
                    make_launch_menu_helper(rc, newmenu, add_item_cb, add_submenu_cb);
                } else {
                    /* err */
                }
            }

            /* add a menu item. FIXME: We free the label token,
             * because once the tookit does its thing with it, we
             * assume it has its own copy. However, we do -not- free
             * the command token, because all we will do with it is
             * hand a pointer to the toolkit, that the toolkit will
             * then give us back later when the appropriate menu item
             * is clicked (which is at some unspecified time in the
             * future, of course, and the pointer still needs to point
             * to something valid when that happens). This is
             * problematic because I want to be able to re-build the
             * menu on SIGHUP, but that would cause leaks unless a
             * list of all such strings we are holding on to is
             * maintained so that they can be freed later on. Sigh. I
             * should have picked a language with garbage collection.
             *
             * XXX: perhaps we can use the toolkit to get at this? it
             * obviously has to know where all the pointers are, and
             * should be able to walk the menu and enumerate them
             * without too much trouble, before destroying the menu.
             * GTK+ might even allow a "destroy" event callback, or
             * something like that... but we still have to deal with
             * Xaw... ramble ramble ramble... */
            if (strcmp(token, "cmd") == 0) {
                if (get_token(&p, token)) {
                    char *label = strdup(token);
                    if (get_token(&p, token))
                        add_item_cb(menu, label, strdup(token));
//                  /* PLEASE NOTE THAT THE FOLLOWING FUNNY LINES OF CODE
//                   * DO NOT IN FACT WORK */
//                  if (get_token(&p, token)) {
//                      char *cmd = strdup(token);
//                      add_item_cb(menu, label, cmd);
//                      free(cmd);
//                  }
                    free(label);
                } else {
                    /* err */
                }
            }

            /* include another menu file */
            if (strcmp(token, "include") == 0) {
                if (get_token(&p, token)) {
                    FILE *f = fopen(token, "r");
                    if (f) {
                        make_launch_menu_helper(f, menu, add_item_cb, add_submenu_cb);
                        fclose(f);
                    } else {
                        /* err */
                    }
                } else {
                    /* err */
                }
            }

            /* back out of this sub-menu if we are done */
            if (strcmp(token, "end") == 0) return;
        }
    }
}

void make_launch_menu(char *rcfile, menu_t menu, add_item_func add_item_cb,
    add_submenu_func add_submenu_cb)
{
    char defrc_buf[BUF_SIZE];
    FILE *rc;

    if (rcfile) {
        rc = fopen(rcfile, "r");
    } else {
        snprintf(defrc_buf, sizeof defrc_buf,
            "%s/.aewm/clientsrc", getenv("HOME"));
        rc = fopen(defrc_buf, "r");
        if (!rc) rc = fopen(DEF_RC, "r");
    }

    if (rc) {
        make_launch_menu_helper(rc, menu, add_item_cb, add_submenu_cb);
        fclose(rc);
    } else {
        fprintf(stderr, "can't find any rc files\n");
        exit(1);
    }
}

void fork_exec(char *cmd)
{
    pid_t pid = fork();

    switch (pid) {
        case 0:
            execlp("/bin/sh", "sh", "-c", cmd, NULL);
            fprintf(stderr, "exec failed, cleaning up child\n");
            exit(1);
        case -1:
            fprintf(stderr, "can't fork\n");
    }
}

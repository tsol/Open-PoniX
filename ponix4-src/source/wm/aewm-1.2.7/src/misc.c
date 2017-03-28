/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "aewm.h"

static void quit_nicely(void);

void err(const char *fmt, ...)
{
    va_list argp;

    fprintf(stderr, "aewm: ");
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    fprintf(stderr, "\n");
}

void fork_exec(char *cmd)
{
    pid_t pid = fork();

    switch (pid) {
        case 0:
            setsid();
            execlp("/bin/sh", "sh", "-c", cmd, NULL);
            err("exec failed, cleaning up child");
            exit(1);
        case -1:
            err("can't fork");
    }
}

void sig_handler(int signum)
{
    switch (signum) {
        case SIGINT:
        case SIGTERM:
        case SIGHUP:
            quit_nicely(); break;
        case SIGCHLD:
            wait(NULL); break;
    }
}

int handle_xerror(Display *dpy, XErrorEvent *e)
{
    /* FIXME: should this be here or not? */
    /* client_t *c = find_client(e->resourceid, MATCH_WINDOW); */

    if (e->error_code == BadAccess && e->resourceid == root) {
        err("root window unavailible (maybe another wm is running?)");
        exit(1);
    } else {
        char msg[255];
        XGetErrorText(dpy, e->error_code, msg, sizeof msg);
        err("X error (%#lx): %s", e->resourceid, msg);
    }

    /* if (c) remove_client(c, REMOVE_WITHDRAW); */
    return 0;
}

/* Ick. Argh. You didn't see this function. */

int ignore_xerror(Display *dpy, XErrorEvent *e)
{
    return 0;
}

/* Currently, only send_wm_delete uses this one... */

int send_xmessage(Window w, Atom a, long x)
{
    XClientMessageEvent e;

    e.type = ClientMessage;
    e.window = w;
    e.message_type = a;
    e.format = 32;
    e.data.l[0] = x;
    e.data.l[1] = CurrentTime;

    return XSendEvent(dpy, w, False, NoEventMask, (XEvent *)&e);
}

void get_mouse_position(int *x, int *y)
{
    Window mouse_root, mouse_win;
    int win_x, win_y;
    unsigned int mask;

    XQueryPointer(dpy, root, &mouse_root, &mouse_win,
        x, y, &win_x, &win_y, &mask);
}

#ifdef DEBUG

/* Bleh, stupid macro names. I'm not feeling creative today. */

#define SHOW_EV(name, memb) \
    case name: ev_type = #name; w = e.memb.window; break;
#define SHOW(name) \
    case name: return #name;

void show_event(XEvent e)
{
    char *ev_type;
    Window w;
    client_t *c;

    switch (e.type) {
        SHOW_EV(ButtonPress, xbutton)
        SHOW_EV(ButtonRelease, xbutton)
        SHOW_EV(ClientMessage, xclient)
        SHOW_EV(ColormapNotify, xcolormap)
        SHOW_EV(ConfigureNotify, xconfigure)
        SHOW_EV(ConfigureRequest, xconfigurerequest)
        SHOW_EV(CreateNotify, xcreatewindow)
        SHOW_EV(DestroyNotify, xdestroywindow)
        SHOW_EV(EnterNotify, xcrossing)
        SHOW_EV(Expose, xexpose)
        SHOW_EV(MapNotify, xmap)
        SHOW_EV(MapRequest, xmaprequest)
        SHOW_EV(MappingNotify, xmapping)
        SHOW_EV(MotionNotify, xmotion)
        SHOW_EV(PropertyNotify, xproperty)
        SHOW_EV(ReparentNotify, xreparent)
        SHOW_EV(ResizeRequest, xresizerequest)
        SHOW_EV(UnmapNotify, xunmap)
        default:
        if (shape && e.type == shape_event) {
            ev_type = "ShapeNotify"; w = ((XShapeEvent *)&e)->window;
        } else {
            ev_type = "unknown event"; w = None;
        }
        break;
    }

    if ((c = find_client(w, MATCH_WINDOW))) {
        dump_title(c, ev_type, 'w');
    } else if ((c = find_client(w, MATCH_FRAME))) {
        dump_title(c, ev_type, 'f');
    } else if (w == root) {
        dump_win(w, ev_type, '?');
    }

}

static const char *show_state(client_t *c)
{
    switch (get_wm_state(c)) {
        SHOW(WithdrawnState)
        SHOW(NormalState)
        SHOW(IconicState)
        default: return "unknown state";
    }
}

static const char *show_grav(client_t *c)
{
    if (!c->size || !(c->size->flags & PWinGravity))
        return "no grav (NW)";

    switch (c->size->win_gravity) {
        SHOW(UnmapGravity)
        SHOW(NorthWestGravity)
        SHOW(NorthGravity)
        SHOW(NorthEastGravity)
        SHOW(WestGravity)
        SHOW(CenterGravity)
        SHOW(EastGravity)
        SHOW(SouthWestGravity)
        SHOW(SouthGravity)
        SHOW(SouthEastGravity)
        SHOW(StaticGravity)
        default: return "unknown grav";
    }
}

void dump_title(client_t *c, const char *label, char flag)
{
    err("%15.15s: %#010lx [%c] %-40.40s", label, c->window, flag, c->name);
}

void dump_win(Window w, const char *label, char flag)
{
    err("%15.15s: %#010lx [%c] %-40.40s", label, w, flag,
        w == root ? "(root window)": "(unknown window)");
}

void dump_info(client_t *c)
{
    err("%28s[i] frame %#0lx, ignore_unmap %d", "",
        c->frame, c->ignore_unmap);
    err("%28s[i] %s, %s", "",
        show_state(c), show_grav(c));
}

void dump_geom(client_t *c, const char *label)
{
    err("%28s[g] %s %dx%d+%d+%d", "",
        label, c->width, c->height, c->x, c->y);
}

void dump_removal(client_t *c, int mode)
{
    err("%28s[r] %s, %d pending", "",
        mode == REMOVE_WITHDRAW ? "withdraw" : "remap", XPending(dpy));
}

void dump_clients()
{
    client_t *c;

    for (c = head_client; c; c = c->next) {
        dump_title(c, "dump", 'd');
        dump_geom(c, "current");
        dump_info(c);
    }
}
#endif

/* We use XQueryTree here to preserve the window stacking order,
 * since the order in our linked list is different. */

static void quit_nicely(void)
{
    unsigned int nwins, i;
    Window dummyw1, dummyw2, *wins;
    client_t *c;

    XQueryTree(dpy, root, &dummyw1, &dummyw2, &wins, &nwins);
    for (i = 0; i < nwins; i++) {
        c = find_client(wins[i], MATCH_FRAME);
        if (c) remove_client(c, REMOVE_REMAP);
    }
    XFree(wins);

    XFreeFont(dpy, font);
#ifdef XFT
    XftFontClose(dpy, xftfont);
#endif
    XFreeCursor(dpy, move_curs);
    XFreeCursor(dpy, resize_curs);
    XFreeGC(dpy, invert_gc);
    XFreeGC(dpy, border_gc);
    XFreeGC(dpy, string_gc);

    XInstallColormap(dpy, DefaultColormap(dpy, screen));
    XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);

    XCloseDisplay(dpy);
    exit(0);
}

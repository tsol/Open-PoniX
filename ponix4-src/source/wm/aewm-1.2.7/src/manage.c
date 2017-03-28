/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include "aewm.h"

static void drag(client_t *);
static void sweep(client_t *);
static void recalc_sweep(client_t *, int, int, int, int);
static void draw_outline(client_t *);
static int get_incsize(client_t *, int *, int *, int);

void move(client_t *c)
{
    drag(c);
    XMoveWindow(dpy, c->frame, c->x, c->y - theight(c));
    send_config(c);
}

void resize(client_t *c)
{
    sweep(c);
    XMoveResizeWindow(dpy, c->frame,
        c->x, c->y - theight(c), c->width, c->height + theight(c));
    XMoveResizeWindow(dpy, c->window,
        0, theight(c), c->width, c->height);
    send_config(c);
}

void hide(client_t *c)
{
    if (!c->ignore_unmap) c->ignore_unmap++;
    XUnmapWindow(dpy, c->frame);
    XUnmapWindow(dpy, c->window);
    set_wm_state(c, IconicState);
}

/* The name of this function is a bit misleading: if the client
 * doesn't listen to WM_DELETE then we just terminate it with extreme
 * prejudice. */

void send_wm_delete(client_t *c)
{
    int i, n, found = 0;
    Atom *protocols;

    if (XGetWMProtocols(dpy, c->window, &protocols, &n)) {
        for (i=0; i<n; i++) if (protocols[i] == wm_delete) found++;
        XFree(protocols);
    }
    if (found) send_xmessage(c->window, wm_protos, wm_delete);
    else XKillClient(dpy, c->window);
}

static void drag(client_t *c)
{
    XEvent ev;
    int x1, y1;
    int old_cx = c->x;
    int old_cy = c->y;

    if (!grab(root, MouseMask, move_curs)) return;
    XGrabServer(dpy);
    get_mouse_position(&x1, &y1);

    draw_outline(c);
    for (;;) {
        XMaskEvent(dpy, MouseMask, &ev);
        switch (ev.type) {
            case MotionNotify:
                draw_outline(c); /* clear */
                c->x = old_cx + (ev.xmotion.x - x1);
                c->y = old_cy + (ev.xmotion.y - y1);
                draw_outline(c);
                break;
            case ButtonRelease:
                draw_outline(c); /* clear */
                XUngrabServer(dpy);
                ungrab();
                return;
        }
    }
}

static void sweep(client_t *c)
{
    XEvent ev;
    int old_cx = c->x;
    int old_cy = c->y;

    if (!grab(root, MouseMask, resize_curs)) return;
    XGrabServer(dpy);

    draw_outline(c);
    setmouse(c->window, c->width, c->height);
    for (;;) {
        XMaskEvent(dpy, MouseMask, &ev);
        switch (ev.type) {
            case MotionNotify:
                draw_outline(c); /* clear */
                recalc_sweep(c, old_cx, old_cy, ev.xmotion.x, ev.xmotion.y);
                draw_outline(c);
                break;
            case ButtonRelease:
                draw_outline(c); /* clear */
                XUngrabServer(dpy);
                ungrab();
                return;
        }
    }
}

static void recalc_sweep(client_t *c, int x1, int y1, int x2, int y2)
{
    c->width = abs(x1 - x2) - BW(c);
    c->height = abs(y1 - y2) - BW(c);

    get_incsize(c, &c->width, &c->height, SIZE_PIXELS);

    if (c->size->flags & PMinSize) {
        if (c->width < c->size->min_width) c->width = c->size->min_width;
        if (c->height < c->size->min_height) c->height = c->size->min_height;
    }

    if (c->size->flags & PMaxSize) {
        if (c->width > c->size->max_width) c->width = c->size->max_width;
        if (c->height > c->size->max_height) c->height = c->size->max_height;
    }

    c->x = (x1 <= x2) ? x1 : x1 - c->width;
    c->y = (y1 <= y2) ? y1 : y1 - c->height;
}

static void draw_outline(client_t *c)
{
    char buf[32];
    int width, height;

    XDrawRectangle(dpy, root, invert_gc,
        c->x + BW(c)/2, c->y - theight(c) + BW(c)/2,
        c->width + BW(c), c->height + theight(c) + BW(c));
#ifdef MWM_HINTS
    if (c->has_title)
#endif
    XDrawLine(dpy, root, invert_gc, c->x + BW(c), c->y + BW(c)/2,
        c->x + c->width + BW(c), c->y + BW(c)/2);

    if (!get_incsize(c, &width, &height, SIZE_LOGICAL)) {
        width = c->width;
        height = c->height;
    }

    gravitate(c, GRAV_UNDO);
    snprintf(buf, sizeof buf, "%dx%d%+d%+d", width, height, c->x, c->y);
    gravitate(c, GRAV_APPLY);
    XDrawString(dpy, root, invert_gc,
        c->x + c->width - XTextWidth(font, buf, strlen(buf)) - opt_pad,
        c->y + c->height - opt_pad,
        buf, strlen(buf));
}

/* If the window in question has a ResizeInc hint, then it wants to be
 * resized in multiples of some (x,y). If we are calculating a new
 * window size, we set mode == SIZE_PIXELS and get the correct
 * width and height back. If we are drawing a friendly label on the
 * screen for the user, we set mode == SIZE_LOGICAL so that they
 * see the geometry in human-readable form (80x25 for xterm, etc). */

static int get_incsize(client_t *c, int *x_ret, int *y_ret, int mode)
{
    int width_inc, height_inc;
    int base_width, base_height;

    if (c->size->flags & PResizeInc) {
        width_inc = c->size->width_inc ? c->size->width_inc : 1;
        height_inc = c->size->height_inc ? c->size->height_inc : 1;
        base_width = (c->size->flags & PBaseSize) ? c->size->base_width :
            (c->size->flags & PMinSize) ? c->size->min_width : 0;
        base_height = (c->size->flags & PBaseSize) ? c->size->base_height :
            (c->size->flags & PMinSize) ? c->size->min_height : 0;

        if (mode == SIZE_PIXELS) {
            *x_ret = c->width - ((c->width - base_width) % width_inc);
            *y_ret = c->height - ((c->height - base_height) % height_inc);
        } else if (mode == SIZE_LOGICAL) {
            *x_ret = (c->width - base_width) / width_inc;
            *y_ret = (c->height - base_height) / height_inc;
        }
        return 1;
    }

    return 0;
}

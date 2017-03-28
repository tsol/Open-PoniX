/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <X11/Xmd.h>
#include "aewm.h"

client_t *find_client(Window w, int mode)
{
    client_t *c;

    if (mode == MATCH_FRAME) {
        for (c = head_client; c; c = c->next)
            if (c->frame == w) return c;
    } else if (mode == MATCH_WINDOW) {
        for (c = head_client; c; c = c->next)
            if (c->window == w) return c;
    }

    return NULL;
}

/* For a regular window, c->trans is None (false), and we include
 * enough space to draw the title. For a transient window we just make
 * a tiny strip. */

int theight(client_t *c)
{
    int font_height;

    if (!c) return 0;

#ifdef MWM_HINTS
    if (!c->has_title) return 0;
#endif

#ifdef XFT
    font_height = xftfont->ascent + xftfont->descent;
#else
    font_height = font->ascent + font->descent;
#endif
    return c->trans ? font_height / 2 : font_height + 2*opt_pad + BW(c);
}

/* Attempt to follow the ICCCM by explicity specifying 32 bits for
 * this property. Does this goof up on 64 bit systems? */

void set_wm_state(client_t *c, int state)
{
    CARD32 data[2];

    data[0] = state;
    data[1] = None; /* Icon? We don't need no steenking icon. */

    XChangeProperty(dpy, c->window, wm_state, wm_state,
        32, PropModeReplace, (unsigned char *)data, 2);
}

/* If we can't find a WM_STATE we're going to have to assume
 * Withdrawn. This is not exactly optimal, since we can't really
 * distinguish between the case where no WM has run yet and when the
 * state was explicitly removed (clients are allowed to either set the
 * atom to Withdrawn or just remove it... yuck.) */

long get_wm_state(client_t *c)
{
    Atom real_type; int real_format;
    long state = WithdrawnState;
    unsigned long items_read, bytes_left;
    unsigned char *data;

    if (XGetWindowProperty(dpy, c->window, wm_state, 0L, 2L, False,
            wm_state, &real_type, &real_format, &items_read, &bytes_left,
            &data) == Success && items_read) {
        state = *(long *)data;
        XFree(data);
    }
    return state;
}

/* This will need to be called whenever we update our client_t stuff.
 * Yeah, yeah, stop yelling at me about OO. */

void send_config(client_t *c)
{
    XConfigureEvent ce;

    ce.type = ConfigureNotify;
    ce.event = c->window;
    ce.window = c->window;
    ce.x = c->x;
    ce.y = c->y;
    ce.width = c->width;
    ce.height = c->height;
    ce.border_width = 0;
    ce.above = None;
    ce.override_redirect = 0;

    XSendEvent(dpy, c->window, False, StructureNotifyMask, (XEvent *)&ce);
}

/* After pulling my hair out trying to find some way to tell if a
 * window is still valid, I've decided to instead carefully ignore any
 * errors raised by this function. We know that the X calls are, and
 * we know the only reason why they could fail -- a window has removed
 * itself completely before the Unmap and Destroy events get through
 * the queue to us. It's not absolutely perfect, but it works.
 *
 * The 'withdrawing' argument specifes if the client is actually
 * (destroying itself||being destroyed by us) or if we are merely
 * cleaning up its data structures when we exit mid-session. */

void remove_client(client_t *c, int mode)
{
    client_t *p;

    XGrabServer(dpy);
    XSetErrorHandler(ignore_xerror);

#ifdef DEBUG
    dump_title(c, "removing", 'r');
    dump_removal(c, mode);
#endif

    if (mode == REMOVE_WITHDRAW) set_wm_state(c, WithdrawnState);
    else if (mode == REMOVE_REMAP) XMapWindow(dpy, c->window);

    gravitate(c, GRAV_UNDO);
    XReparentWindow(dpy, c->window, root, c->x, c->y);
#ifdef MWM_HINTS
    if (c->has_border) XSetWindowBorderWidth(dpy, c->window, 1);
#else
    XSetWindowBorderWidth(dpy, c->window, 1);
#endif
#ifdef XFT
    if (c->xftdraw) XftDrawDestroy(c->xftdraw);
#endif
    XRemoveFromSaveSet(dpy, c->window);
    XDestroyWindow(dpy, c->frame);

    if (head_client == c) head_client = c->next;
    else for (p = head_client; p && p->next; p = p->next)
        if (p->next == c) p->next = c->next;

    if (c->name) XFree(c->name);
    if (c->size) XFree(c->size);
    free(c);

    XSync(dpy, False);
    XSetErrorHandler(handle_xerror);
    XUngrabServer(dpy);
}

/* I've changed this to just clear the window every time. The amount
 * of 'flicker' is basically imperceptable. Also, we might be drawing
 * an anti-aliased font with Xft, in which case we always have to
 * clear to draw the text properly. This allows us to simplify
 * handle_property_change as well. */

void redraw(client_t *c)
{
#ifdef MWM_HINTS
    if (!c->has_title) return;
#endif

    XClearWindow(dpy, c->frame);
    XDrawLine(dpy, c->frame, border_gc,
        0, theight(c) - BW(c) + BW(c)/2,
        c->width, theight(c) - BW(c) + BW(c)/2);
    XDrawLine(dpy, c->frame, border_gc,
        c->width - theight(c) + BW(c)/2, 0,
        c->width - theight(c) + BW(c)/2, theight(c));
    if (!c->trans && c->name) {
#ifdef XFT
        XftDrawString8(c->xftdraw, &xft_fg,
            xftfont, opt_pad, opt_pad + xftfont->ascent,
            c->name, strlen(c->name));
#else
        XDrawString(dpy, c->frame, string_gc,
            opt_pad, opt_pad + font->ascent,
            c->name, strlen(c->name));
#endif
    }
}

/* Window gravity is a mess to explain, but we don't need to do much
 * about it since we're using X borders. For NorthWest et al, the top
 * left corner of the window when there is no WM needs to match up
 * with the top left of our fram once we manage it, and likewise with
 * SouthWest and the bottom right (these are the only values I ever
 * use, but the others should be obvious.) Our titlebar is on the top
 * so we only have to adjust in the first case. */

void gravitate(client_t *c, int mode)
{
    int dy = 0;
    int gravity = (c->size->flags & PWinGravity) ?
        c->size->win_gravity : NorthWestGravity;

    switch (gravity) {
        case NorthWestGravity:
        case NorthEastGravity:
        case NorthGravity: dy = theight(c); break;
        case CenterGravity: dy = theight(c)/2; break;
    }

    if (mode == GRAV_APPLY)
        c->y += dy;
    else if (mode == GRAV_UNDO)
        c->y -= dy;
}

/* Well, the man pages for the shape extension say nothing, but I was
 * able to find a shape.PS.Z on the x.org FTP site. What we want to do
 * here is make the window shape be a boolean OR (or union, if you
 * prefer) of the client's shape and our titlebar. The titlebar
 * requires both a bound and a clip because it has a border -- the X
 * server will paint the border in the region between the two. (I knew
 * that using X borders would get me eventually... ;-)) */

#ifdef SHAPE
void set_shape(client_t *c)
{
    int n, order;
    XRectangle temp, *dummy;

    dummy = XShapeGetRectangles(dpy, c->window, ShapeBounding, &n, &order);
    if (n > 1) {
        XShapeCombineShape(dpy, c->frame, ShapeBounding,
            0, theight(c), c->window, ShapeBounding, ShapeSet);
        temp.x = -BW(c);
        temp.y = -BW(c);
        temp.width = c->width + 2*BW(c);
        temp.height = theight(c) + BW(c);
        XShapeCombineRectangles(dpy, c->frame, ShapeBounding,
            0, 0, &temp, 1, ShapeUnion, YXBanded);
        temp.x = 0;
        temp.y = 0;
        temp.width = c->width;
        temp.height = theight(c) - BW(c);
        XShapeCombineRectangles(dpy, c->frame, ShapeClip,
            0, theight(c), &temp, 1, ShapeUnion, YXBanded);
        c->has_been_shaped = 1;
    } else if (c->has_been_shaped) {
        /* I can't find a 'remove all shaping' function... */
        temp.x = -BW(c);
        temp.y = -BW(c);
        temp.width = c->width + 2*BW(c);
        temp.height = c->height + theight(c) + 2*BW(c);
        XShapeCombineRectangles(dpy, c->frame, ShapeBounding,
            0, 0, &temp, 1, ShapeSet, YXBanded);
    }
    XFree(dummy);
}
#endif

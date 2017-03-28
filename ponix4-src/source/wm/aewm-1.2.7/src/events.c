/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <X11/Xatom.h>
#include "aewm.h"

static void handle_button_press(XButtonEvent *);
static void handle_button_release(XButtonEvent *);
static void handle_configure_request(XConfigureRequestEvent *);
static void handle_circulate_request(XCirculateRequestEvent *);
static void handle_map_request(XMapRequestEvent *);
static void handle_unmap_event(XUnmapEvent *);
static void handle_destroy_event(XDestroyWindowEvent *);
static void handle_client_message(XClientMessageEvent *);
static void handle_property_change(XPropertyEvent *);
static void handle_enter_event(XCrossingEvent *);
static void handle_colormap_change(XColormapEvent *);
static void handle_expose_event(XExposeEvent *);
#ifdef SHAPE
static void handle_shape_change(XShapeEvent *);
#endif

static int root_button_pressed = 0;

/* We may want to put in some sort of check for unknown events at some
 * point. TWM has an interesting and different way of doing this... */

void do_event_loop(void)
{
    XEvent ev;

    for (;;) {
        XNextEvent(dpy, &ev);
#ifdef DEBUG
        show_event(ev);
#endif
        switch (ev.type) {
            case ButtonPress:
                handle_button_press(&ev.xbutton); break;
            case ButtonRelease:
                handle_button_release(&ev.xbutton); break;
            case ConfigureRequest:
                handle_configure_request(&ev.xconfigurerequest); break;
            case CirculateRequest:
                handle_circulate_request(&ev.xcirculaterequest); break;
            case MapRequest:
                handle_map_request(&ev.xmaprequest); break;
            case UnmapNotify:
                handle_unmap_event(&ev.xunmap); break;
            case DestroyNotify:
                handle_destroy_event(&ev.xdestroywindow); break;
            case ClientMessage:
                handle_client_message(&ev.xclient); break;
            case ColormapNotify:
                handle_colormap_change(&ev.xcolormap); break;
            case PropertyNotify:
                handle_property_change(&ev.xproperty); break;
            case EnterNotify:
                handle_enter_event(&ev.xcrossing); break;
            case Expose:
                handle_expose_event(&ev.xexpose); break;
#ifdef SHAPE
            default:
                if (shape && ev.type == shape_event)
                    handle_shape_change((XShapeEvent *)&ev);
#endif
        }
    }
}

/* Someone clicked a button. If they clicked on a window, we want the
 * button press, but if they clicked on the root, we're only
 * interested in the button release. Thus, two functions.
 *
 * If it was on the root, we get the click by default. If it's on a
 * window frame, we get it as well. If it's on a client window, it may
 * still fall through to us if the client doesn't select for
 * mouse-click events. The upshot of this is that you should be able
 * to click on the blank part of a GTK window with Button2 to move it.
 *
 * If you have a hankering to change the button bindings, they're
 * right here. However(!), I highly reccomend that 2-button mouse
 * users try xmodmap -e 'pointer = 1 3 2' first, as mentioned in the
 * README. */

static void handle_button_press(XButtonEvent *e)
{
    client_t *c = find_client(e->window, MATCH_FRAME);
    int in_box;

    if (e->window == root) {
        root_button_pressed = 1;
    } else if (c) {
        in_box = (e->x >= c->width - theight(c)) && (e->y <= theight(c));
        switch (e->button) {
            case Button1:
                in_box ? hide(c) : raise_win(c); break;
            case Button2:
                in_box ? resize(c) : move(c); break;
            case Button3:
                in_box ? send_wm_delete(c) : lower_win(c); break;
        }
    }
}

static void handle_button_release(XButtonEvent *e)
{
    if (e->window == root && root_button_pressed) {
#ifdef DEBUG
        dump_clients();
#endif
        switch (e->button) {
            case Button1: fork_exec(opt_new1); break;
            case Button2: fork_exec(opt_new2); break;
            case Button3: fork_exec(opt_new3); break;
        }

        root_button_pressed = 0;
    }
}

/* Because we are redirecting the root window, we get ConfigureRequest
 * events from both clients we're handling and ones that we aren't.
 * For clients we manage, we need to fiddle with the frame and the
 * client window, and for unmanaged windows we have to pass along
 * everything unchanged. Thankfully, we can reuse (a) the
 * XWindowChanges struct and (c) the code to configure the client
 * window in both cases.
 *
 * Most of the assignments here are going to be garbage, but only the
 * ones that are masked in by e->value_mask will be looked at by the X
 * server. */

static void handle_configure_request(XConfigureRequestEvent *e)
{
    client_t *c = find_client(e->window, MATCH_WINDOW);
    XWindowChanges wc;

    if (c) {
        gravitate(c, GRAV_UNDO);
        if (e->value_mask & CWX) c->x = e->x;
        if (e->value_mask & CWY) c->y = e->y;
        if (e->value_mask & CWWidth) c->width = e->width;
        if (e->value_mask & CWHeight) c->height = e->height;
        gravitate(c, GRAV_APPLY);
#ifdef DEBUG
        dump_geom(c, "moving to");
#endif
        /* configure the frame */
        wc.x = c->x;
        wc.y = c->y - theight(c);
        wc.width = c->width;
        wc.height = c->height + theight(c);
        wc.border_width = BW(c);
        wc.sibling = e->above;
        wc.stack_mode = e->detail;
        XConfigureWindow(dpy, c->frame, e->value_mask, &wc);
#ifdef SHAPE
        if (e->value_mask & (CWWidth|CWHeight)) set_shape(c);
#endif
        send_config(c);
        /* start setting up the next call */
        wc.x = 0;
        wc.y = theight(c);
    } else {
        wc.x = e->x;
        wc.y = e->y;
    }

    wc.width = e->width;
    wc.height = e->height;
    wc.sibling = e->above;
    wc.stack_mode = e->detail;
    XConfigureWindow(dpy, e->window, e->value_mask, &wc);
}

/* The only window that we will circulate children for is the root
 * (because nothing else would make sense). After a client requests that
 * the root's children be circulated, the server will determine which
 * window needs to be raised or lowered, and so all we have to do is
 * make it so. */

static void handle_circulate_request(XCirculateRequestEvent *e)
{
    if (e->parent == root) {
        if (e->place == PlaceOnBottom) XLowerWindow(dpy, e->window);
        else /* e->place == PlaceOnTop */ XRaiseWindow(dpy, e->window);
    }
}

/* Two possiblilies if a client is asking to be mapped. One is that
 * it's a new window, so we handle that if it isn't in our clients
 * list anywhere. The other is that it already exists and wants to
 * de-iconify, which is simple to take care of. */

static void handle_map_request(XMapRequestEvent *e)
{
    client_t *c = find_client(e->window, MATCH_WINDOW);

    if (!c) {
        make_new_client(e->window);
    } else {
        XMapWindow(dpy, c->window);
        XMapRaised(dpy, c->frame);
        set_wm_state(c, NormalState);
    }
}

/* See aewm.h for the intro to this one. If this is a window we
 * unmapped ourselves, decrement c->ignore_unmap and casually go on as
 * if nothing had happened. If the window unmapped itself from under
 * our feet, however, get rid of it.
 *
 * If you spend a lot of time with -DDEBUG on, you'll realize that
 * because most clients unmap and destroy themselves at once, they're
 * gone before we even get the Unmap event, never mind the Destroy
 * one. This will necessitate some extra caution in remove_client.
 *
 * Personally, I think that if Map events are intercepted, Unmap
 * events should be intercepted too. No use arguing with a standard
 * that's almost as old as I am though. :-( */

static void handle_unmap_event(XUnmapEvent *e)
{
    client_t *c = find_client(e->window, MATCH_WINDOW);

    if (!c) return;

    if (c->ignore_unmap) c->ignore_unmap--;
    else remove_client(c, REMOVE_WITHDRAW);
}

/* This happens when a window is iconified and destroys itself. An
 * Unmap event wouldn't happen in that case because the window is
 * already unmapped. */

static void handle_destroy_event(XDestroyWindowEvent *e)
{
    client_t *c = find_client(e->window, MATCH_WINDOW);

    if (!c) return;
    remove_client(c, REMOVE_WITHDRAW);
}

/* If a client wants to iconify itself (boo! hiss!) it must send a
 * special kind of ClientMessage. We might set up other handlers here
 * but there's nothing else required by the ICCCM. */

static void handle_client_message(XClientMessageEvent *e)
{
    client_t *c = find_client(e->window, MATCH_WINDOW);

    if (c && e->message_type == wm_change_state &&
        e->format == 32 && e->data.l[0] == IconicState) hide(c);
}

/* All that we have cached is the name and the size hints, so we only
 * have to check for those here. A change in the name means we have to
 * immediately wipe out the old name and redraw size hints only get
 * used when we need them. Note that the actual redraw call both
 * clears and draws; before Xft, the XClearWindow call was only made
 * in this function. */

static void handle_property_change(XPropertyEvent *e)
{
    client_t *c = find_client(e->window, MATCH_WINDOW);
    long dummy;

    if (!c) return;

    switch (e->atom) {
        case XA_WM_NAME:
            if (c->name) XFree(c->name);
            XFetchName(dpy, c->window, &c->name);
            redraw(c);
            break;
        case XA_WM_NORMAL_HINTS:
            XGetWMNormalHints(dpy, c->window, c->size, &dummy);
            break;
    }
}

/* X's default focus policy is follows-mouse, but we have to set it
 * anyway because some sloppily written clients assume that (a) they
 * can set the focus whenever they want or (b) that they don't have
 * the focus unless the keyboard is grabbed to them. OTOH it does
 * allow us to keep the previous focus when pointing at the root,
 * which is nice.
 *
 * We also implement a colormap-follows-mouse policy here. That, on
 * the third hand, is *not* X's default. */

static void handle_enter_event(XCrossingEvent *e)
{
    client_t *c = find_client(e->window, MATCH_FRAME);

    if (!c) return;
    XSetInputFocus(dpy, c->window, RevertToPointerRoot, CurrentTime);
    XInstallColormap(dpy, c->cmap);
}

/* Here's part 2 of our colormap policy: when a client installs a new
 * colormap on itself, set the display's colormap to that. Arguably,
 * this is bad, because we should only set the colormap if that client
 * has the focus. However, clients don't usually set colormaps at
 * random when you're not interacting with them, so I think we're
 * safe. If you have an 8-bit display and this doesn't work for you,
 * by all means yell at me, but very few people have 8-bit displays
 * these days. */

static void handle_colormap_change(XColormapEvent *e)
{
    client_t *c = find_client(e->window, MATCH_WINDOW);

    if (c && e->new) {
        c->cmap = e->colormap;
        XInstallColormap(dpy, c->cmap);
    }
}

/* If we were covered by multiple windows, we will usually get
 * multiple expose events, so ignore them unless e->count (the number
 * of outstanding exposes) is zero. */

static void handle_expose_event(XExposeEvent *e)
{
    client_t *c = find_client(e->window, MATCH_FRAME);

    if (c && e->count == 0) redraw(c);
}

#ifdef SHAPE
static void handle_shape_change(XShapeEvent *e)
{
    client_t *c = find_client(e->window, MATCH_WINDOW);

    if (c) set_shape(c);
}
#endif

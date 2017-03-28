/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#ifndef AEWM_H
#define AEWM_H

#define VERSION "1.2.7"

#include "common.h"
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef SHAPE
#include <X11/extensions/shape.h>
#endif
#ifdef MWM_HINTS
#include <Xm/MwmUtil.h>
#endif
#ifdef XFT
#include <X11/Xft/Xft.h>
#endif

/* Here are the default settings; they can be overriden in aewmrc,
 * but you can also change them here at compile time. */

#ifdef XFT
#define DEF_FONT "-*-freesans-medium-r-normal-*-*-80-*-*-p-*-iso10646-1"
#else
#define DEF_FONT "fixed"
#endif

#define DEF_RC   "/etc/X11/aewm/aewmrc"
#define DEF_FG   "white"
#define DEF_BG   "slategray"
#define DEF_BD   "black"
#define DEF_NEW1 "aemenu"
#define DEF_NEW2 "aemenu --switch"
#define DEF_NEW3 "xterm"

#define DEF_BW   1
#define DEF_PAD  3

/* A few useful masks made up out of X's basic ones. `ChildMask' is a
 * silly name, but oh well. */

#define ChildMask (SubstructureRedirectMask|SubstructureNotifyMask)
#define ButtonMask (ButtonPressMask|ButtonReleaseMask)
#define MouseMask (ButtonMask|PointerMotionMask)

/* Shorthand for wordy function calls */

#define setmouse(w, x, y) XWarpPointer(dpy, None, w, 0, 0, 0, 0, x, y)
#define ungrab() XUngrabPointer(dpy, CurrentTime)
#define grab(w, mask, curs) (XGrabPointer(dpy, w, False, mask, \
    GrabModeAsync, GrabModeAsync, None, curs, CurrentTime) == GrabSuccess)

/* I wanna know who the morons who prototyped these functions as
 * implicit int are...  */

#define lower_win(c) ((void) XLowerWindow(dpy, (c)->frame))
#define raise_win(c) ((void) XRaiseWindow(dpy, (c)->frame))

/* Border width accessor to handle hints/no hints */

#ifdef MWM_HINTS
#define BW(c) ((c)->has_border ? opt_bw : 0)
#else
#define BW(c) (opt_bw)
#endif

/* Multipliers for calling gravitate */

enum {
    GRAV_APPLY,
    GRAV_UNDO
};

/* Modes to call get_incsize with */

enum {
    SIZE_PIXELS,
    SIZE_LOGICAL
};

/* Modes for find_client */

enum {
    MATCH_WINDOW,
    MATCH_FRAME
};

/* And finally modes for remove_client. */

enum {
    REMOVE_WITHDRAW,
    REMOVE_REMAP
};

/* This structure keeps track of top-level windows (hereinafter
 * 'clients'). The clients we know about (i.e. all that don't set
 * override-redirect) are kept track of in linked list starting at the
 * global pointer called, appropriately, 'clients'. 
 *
 * window and parent refer to the actual client window and the larget
 * frame into which we will reparent it respectively. trans is set to
 * None for regular windows, and the window's 'owner' for a transient
 * window. Currently, we don't actually do anything with the owner for
 * transients; it's just used as a boolean.
 *
 * ignore_unmap is for our own purposes and doesn't reflect anything
 * from X. Whenever we unmap a window intentionally, we increment
 * ignore_unmap. This way our unmap event handler can tell when it
 * isn't supposed to do anything. */

typedef struct client client_t;

struct client {
    client_t   *next;
    char       *name;
    XSizeHints *size;
    Window     window, frame, trans;
    Colormap   cmap;
    int        x, y, width, height;
    int        ignore_unmap;
#ifdef SHAPE
    Bool       has_been_shaped;
#endif
#ifdef MWM_HINTS
    Bool       has_title, has_border;
#endif
#ifdef XFT
    XftDraw    *xftdraw;
#endif
};

/* Below here are (mainly generated with cproto) declarations and
 * prototypes for each file. */

/* init.c */
extern Display *dpy;
extern Window root;
extern int screen;
extern client_t *head_client;
extern XFontStruct *font;
#ifdef XFT
extern XftFont *xftfont;
extern XftColor xft_fg;
#endif
extern GC invert_gc;
extern GC string_gc;
extern GC border_gc;
extern XColor fg;
extern XColor bg;
extern XColor bd;
extern Cursor move_curs;
extern Cursor resize_curs;
extern Atom wm_state;
extern Atom wm_change_state;
extern Atom wm_protos;
extern Atom wm_delete;
#ifdef GNOME_PDA
extern Atom gnome_pda;
#endif
#ifdef MWM_HINTS
extern Atom mwm_hints;
#endif
extern char *opt_new1;
extern char *opt_new2;
extern char *opt_new3;
extern int opt_bw;
extern int opt_pad;
#ifdef SHAPE
extern Bool shape;
extern int shape_event;
#endif

/* events.c */
extern void do_event_loop(void);

/* client.c */
extern client_t *find_client(Window, int);
extern int theight(client_t *);
extern void set_wm_state(client_t *, int);
extern long get_wm_state(client_t *);
extern void send_config(client_t *);
extern void remove_client(client_t *, int);
extern void redraw(client_t *);
extern void gravitate(client_t *, int);
#ifdef SHAPE
extern void set_shape(client_t *);
#endif

/* new.c */
extern void make_new_client(Window);

/* manage.c */
extern void move(client_t *);
extern void resize(client_t *);
extern void hide(client_t *);
extern void send_wm_delete(client_t *);

/* misc.c */
extern void err(const char *, ...);
extern void fork_exec(char *);
extern void sig_handler(int);
extern int handle_xerror(Display *, XErrorEvent *);
extern int ignore_xerror(Display *, XErrorEvent *);
extern int send_xmessage(Window, Atom, long);
extern void get_mouse_position(int *, int *);
#ifdef DEBUG
extern void show_event(XEvent);
extern void dump_title(client_t *, const char *, char);
extern void dump_win(Window, const char *, char);
extern void dump_info(client_t *);
extern void dump_geom(client_t *, const char *);
extern void dump_removal(client_t *, int);
extern void dump_clients(void);
#endif

#endif /* AEWM_H */

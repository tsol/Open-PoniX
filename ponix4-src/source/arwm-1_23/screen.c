/*
 * - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <6809.org.uk> see README for license and other details.
 */
#include "arwm.h"
#include <string.h>
#include "screen.h"

#ifdef WITHRESIZEBORDER
static void 
draw_outline(Client * c)
{
	Client ca = *c;
	ScreenInfo sa = *ca.screen;
	GC gc=sa.invert_gc;
	int x, y;
	unsigned int width, height;
	Display * dpy = arwm.X.dpy;

#ifdef INFORMATION_ON_OUTLINE
	char buf[27];
	int buf_len=0;
#ifdef USE_XFT
	XClearWindow(dpy, sa.root);
#endif /* USE_XFT */
#endif	/* INFORMATION_ON_OUTLINE */

	x=ca.geometry.x - ca.border + 1;
	y=ca.geometry.y - ca.border
#ifdef TITLEBAR
		-(
#ifdef SHAPE
		(c->flags & AR_CLIENT_SHAPED) ? 0 :
#endif
		TITLEBAR_HEIGHT)
#endif /* TITLEBAR */
		;
	width=ca.geometry.width + ca.border;
	height=ca.geometry.height + ca.border
#ifdef TITLEBAR
		+ (
#ifdef SHAPE
		(c->flags & AR_CLIENT_SHAPED) ? 0 :
#endif /* SHAPE */
		(((c->flags & AR_CLIENT_SHADED)?-TITLEBAR_HEIGHT:0)
		+ TITLEBAR_HEIGHT))
#endif /* TITLEBAR */
		;

#ifdef FILLED_DRAG
	XFillRectangle
#else /* ! FILLED_DRAG */
	XDrawRectangle
#endif /* FILLED_DRAG */
		(dpy, sa.root, gc, x, y, width, height);

#ifdef INFORMATION_ON_OUTLINE
	snprintf(buf, sizeof(buf), "%dx%d+%d+%d",
		(ca.geometry.width - ca.base_width) / ca.width_inc,
		(ca.geometry.height - ca.base_height) / ca.height_inc, 
		ca.geometry.x, ca.geometry.y);
	buf_len=strlen(buf);
	x=ca.geometry.x+ca.geometry.width;
	y=ca.geometry.y+ca.geometry.height - SPACE;
#ifndef USE_XFT
	x+= -XTextWidth(arwm.X.font, buf, strlen(buf)) - SPACE;

	XDrawString(dpy, sa.root, sa.invert_gc, x, y, buf, strlen(buf));
#else /* USE_XFT */
	{
		XGlyphInfo extents;
		XftDraw * draw;
		XftColor color;
#ifndef TITLEBAR
		Visual * visual;
		Colormap cmap;
#endif /* ! TITLEBAR */

		XftTextExtentsUtf8(dpy, arwm.X.font,
			(XftChar8 *)buf, buf_len, &extents);

		x-=(extents.width+c->border);
		y-=extents.height;
#ifdef TITLEBAR
		draw=arwm.titlebar.draw;
		color=arwm.titlebar.black;
		XftDrawChange(draw, sa.root);
#else /* ! TITLEBAR */
		{
			int screen;

			screen=c->screen->screen;
			visual=DefaultVisual(dpy, screen);
			cmap=DefaultColormap(dpy, screen);
			draw=XftDrawCreate(dpy, sa.root, visual, cmap);
			XftColorAllocName(dpy, visual, cmap, "black", &color);
		}
#endif /* TITLEBAR */

		XftDrawStringUtf8(draw, &color, arwm.X.font, x, y, 
				(XftChar8 *)buf, buf_len);
#ifndef TITLEBAR
		XftDrawDestroy(draw);
		XftColorFree(dpy, visual, cmap, &color);
#endif /* ! TITLEBAR */
	}
#endif /* ! USE_XFT */
#endif	/* INFORMATION_ON_OUTLINE */
}
#endif	/* WITHRESIZEBORDER */

static void
recalculate_size(Client * c, int x1, int y1, int x2, int y2)
{
	{
		c->geometry.width=abs(x1 - x2);
		c->geometry.height=abs(y1 - y2);
		c->geometry.width-=(c->geometry.width - c->base_width) 
			% c->width_inc;
		c->geometry.height-=(c->geometry.height - c->base_height) 
			% c->height_inc;
	}
	{
		if (c->min_width && c->geometry.width < c->min_width)
			c->geometry.width = c->min_width;
		if (c->min_height && c->geometry.height < c->min_height)
			c->geometry.height = c->min_height;
		if (c->max_width && c->geometry.width > c->max_width)
			c->geometry.width = c->max_width;
		if (c->max_height && c->geometry.height > c->max_height)
			c->geometry.height = c->max_height;
	}
}

static void
recalculate_position(Client * c, int x1, int y1, int x2, int y2)
{
	c->geometry.x = (x1 <= x2) ? x1 : x1 - c->geometry.width;
	c->geometry.y = (y1 <= y2) ? y1 : y1 - c->geometry.height;
}

static void
recalculate_sweep(Client * c, int x1, int y1, int x2, int y2)
{
	/* This is a quick hack for gshterm.  */
	c->min_height=c->min_width=0;

	recalculate_size(c, x1, y1, x2, y2);
	recalculate_position(c, x1, y1, x2, y2);

	SET_CLIENT_CE(c);
}

void 
sweep(Client * c)
{
	XEvent ev;
	int old_cx = c->geometry.x;
	int old_cy = c->geometry.y;

	if (!grab_pointer(c->screen->root, MouseMask, arwm.X.cursors.resize))
		return;

	XGrabServer(arwm.X.dpy);
#ifdef WITHRESIZEBORDER
	draw_outline(c);
#endif	/* WITHRESIZEBORDER */
	setmouse(c->window, c->geometry.width, c->geometry.height);
	for (;;)
	{
		XMaskEvent(arwm.X.dpy, MouseMask, &ev);
		switch (ev.type)
		{
		case MotionNotify:
#ifdef WITHRESIZEBORDER
			draw_outline(c);	/* clear */
#endif	/* WITHRESIZEBORDER */
			XUngrabServer(arwm.X.dpy);
			recalculate_sweep(c, old_cx, old_cy,
				ev.xmotion.x, ev.xmotion.y);
			XSync(arwm.X.dpy, False);
			XGrabServer(arwm.X.dpy);
#ifdef WITHRESIZEBORDER
			draw_outline(c);
#endif	/* WITHRESIZEBORDER */
			break;
		case ButtonRelease:
#ifdef WITHRESIZEBORDER
			draw_outline(c);	/* clear */
#endif	/* WITHRESIZEBORDER */
			XUngrabServer(arwm.X.dpy);
			XUngrabPointer(arwm.X.dpy, CurrentTime);
			moveresize(c);
			return;
		default:
			break;
		}
	}
}

#ifdef SNAP
static int 
absmin(int a, int b)
{
	if (abs(a) < abs(b))
		return a;
	return b;
}

static void
snap_client_to_screen_border(Client * c)
{
	int display_width = DisplayWidth(arwm.X.dpy, c->screen->screen);
	int display_height = DisplayHeight(arwm.X.dpy, c->screen->screen);

	/* snap to screen border */
	if (abs(c->geometry.x - c->border) < arwm.options.snap)
		c->geometry.x = c->border;
	if (abs(c->geometry.x + c->geometry.width + c->border - display_width) 
		< arwm.options.snap)
		c->geometry.x = display_width - c->geometry.width - c->border;
	if (abs(c->geometry.y - c->border) < arwm.options.snap)
		c->geometry.y = c->border;
	if (abs(c->geometry.y + c->geometry.height + c->border 
		- display_height) < arwm.options.snap)
		c->geometry.y = display_height - c->geometry.height 
			- c->border;
}

static void 
snap_client(Client * c)
{
	int dx, dy;
	Client *ci;

	snap_client_to_screen_border(c);
	/* snap to other windows */
	dx = dy = arwm.options.snap;
	for (ci = head_client; ci; ci = ci->next)
	{
		if (ci != c
			&& (ci->screen == c->screen)
			&& (ci->vdesk == c->vdesk)
			)
		{
			if (ci->geometry.y - ci->border - c->border 
				- c->geometry.height - c->geometry.y
				<= arwm.options.snap && c->geometry.y 
				- c->border - ci->border - ci->geometry.height 
				- ci->geometry.y <= arwm.options.snap)
			{
				dx = absmin(dx, ci->geometry.x 
					+ ci->geometry.width 
					- c->geometry.x + c->border 
					+ ci->border);
				dx = absmin(dx, ci->geometry.x 
					+ ci->geometry.width 
					- c->geometry.x
					- c->geometry.width);
				dx = absmin(dx, ci->geometry.x 
					- c->geometry.x 
					- c->geometry.width
					- c->border - ci->border);
				dx = absmin(dx, ci->geometry.x 
					- c->geometry.x);
			}
			if (ci->geometry.x - ci->border - c->border 
				- c->geometry.width - c->geometry.x
				<= arwm.options.snap && c->geometry.x 
				- c->border - ci->border - ci->geometry.width 
				- ci->geometry.x 
				<= arwm.options.snap)
			{
				dy = absmin(dy, ci->geometry.y 
					+ ci->geometry.height 
					- c->geometry.y + c->border 
					+ ci->border);
				dy = absmin(dy, ci->geometry.y 
					+ ci->geometry.height 
					- c->geometry.y - c->geometry.height);
				dy = absmin(dy, ci->geometry.y - c->geometry.y 
					- c->geometry.height - c->border 
					- ci->border);
				dy = absmin(dy, ci->geometry.y - c->geometry.y);
			}
		}
	}
	if (abs(dx) < arwm.options.snap)
		c->geometry.x += dx;
	if (abs(dy) < arwm.options.snap)
		c->geometry.y += dy;
}
#endif	/* SNAP */

static void
handle_drag_type(Client * c)
{
#ifdef SOLIDDRAG
	if (!arwm.options.solid_drag)
	{
#endif /* SOLIDDRAG */
		XSync(arwm.X.dpy, False);
		XGrabServer(arwm.X.dpy); 
#ifdef WITHRESIZEBORDER
		draw_outline(c);
#endif	/* WITHRESIZEBORDER */
#ifdef SOLIDDRAG
	}
	else
	{
		Client ca = *c;

		XMoveWindow(arwm.X.dpy, ca.parent,
			ca.geometry.x - ca.border,
			ca.geometry.y - ca.border);
		SEND_CONFIG(c);
#ifdef TITLEBAR
		/* This call allows expose events to be processed,
		   properly rendering the titlebars when uncovered.  */
#ifdef SHAPE
	if(!(c->flags & AR_CLIENT_SHAPED))
#endif /* SHAPE */
		arwm_process_events();
#endif /* TITLEBAR */
	}
#endif /* SOLIDDRAG */
}

static void
drag_motion(Client * c, XEvent ev, int x1, int y1,
	int old_cx, int old_cy)
{
#ifdef SOLIDDRAG
	if (!arwm.options.solid_drag)
	{
#endif /* SOLIDDRAG */
#ifdef WITHRESIZEBORDER
		draw_outline(c);	/* clear */
#endif	/* WITHRESIZEBORDER */
		XUngrabServer(arwm.X.dpy);
#ifdef SOLIDDRAG
	}
#endif /* SOLIDDRAG */
	c->geometry.x = old_cx + (ev.xmotion.x - x1);
	c->geometry.y = old_cy + (ev.xmotion.y - y1);
	SET_CLIENT_CE(c);
#ifdef SNAP
	if (arwm.options.snap)
		snap_client(c);
#endif
	handle_drag_type(c);
}

static void
drag_button_release(Client * c)
{
	LOG_DEBUG("drag_button_release(c);\n");
#ifdef SOLIDDRAG
	if (!arwm.options.solid_drag)
	{
#endif /* SOLIDDRAG */
#ifdef WITHRESIZEBORDER
		draw_outline(c);	/* clear */
#endif	/* WITHRESIZEBORDER */
		XUngrabServer(arwm.X.dpy);
#ifdef SOLIDDRAG 
	}
#endif /* SOLIDDRAG */
	XUngrabPointer(arwm.X.dpy, CurrentTime);
#ifdef SOLIDDRAG
	if (!arwm.options.solid_drag)
#endif /* SOLIDDRAG */
		moveresize(c);
#if defined(INFORMATION_ON_OUTLINE) && defined(USE_XFT)
	XClearWindow(arwm.X.dpy, c->screen->root);
#endif
}

static void
drag_event_loop(Client * c, int x1, int y1,
	int old_cx, int old_cy)
{
	XEvent ev;
	for (;;)
	{
		XMaskEvent(arwm.X.dpy, MouseMask, &ev);
		switch (ev.type)
		{
		case MotionNotify:
			drag_motion(c, ev,
				x1, y1,
				old_cx,
				old_cy);
			break;
		case ButtonRelease:
			drag_button_release(c);
			return;
		default:
			break;
		}
	}

}
void 
drag(Client * c)
{
	int x1, y1;
	int old_cx = c->geometry.x;
	int old_cy = c->geometry.y;

	if (!grab_pointer(c->screen->root, MouseMask, arwm.X.cursors.move))
		return;
	/* XRaiseWindow(arwm.X.dpy, c->parent); */
	get_mouse_position(&x1, &y1, c->screen->root);

#ifdef SOLIDDRAG
	if (!arwm.options.solid_drag)
	{
#endif /* SOLIDDRAG */
		XGrabServer(arwm.X.dpy);
#ifdef WITHRESIZEBORDER
		draw_outline(c);
#endif	/* WITHRESIZEBORDER */
#ifdef SOLIDDRAG
	}
#endif /* SOLIDDRAG */
	drag_event_loop(c, x1, y1, old_cx, old_cy);
}

void 
moveresize(Client * c)
{

	const unsigned int parent_height = c->geometry.height
#ifdef TITLEBAR
		+ (((c->flags & AR_CLIENT_SHADED) 
#ifdef SHAPE
		|| (c->flags & AR_CLIENT_SHAPED) 
#endif /* SHAPE */
		?  0 : TITLEBAR_HEIGHT))
#endif	/* TITLEBAR */
	;

	LOG_DEBUG("moveresize(c);\n");
	XMoveResizeWindow(arwm.X.dpy, c->parent, 
		c->geometry.x - c->border, 
		c->geometry.y - c->border
#ifdef TITLEBAR
		- (
#ifdef SHAPE
	//	(c->flags & AR_CLIENT_SHAPED) ? 0 :
#endif
		TITLEBAR_HEIGHT)
#endif /* TITLEBAR */		
		, c->geometry.width, parent_height);
	XMoveResizeWindow(arwm.X.dpy, c->window, 0,
#ifdef TITLEBAR
		(
#ifdef SHAPED
		(c->flags & AR_CLIENT_SHAPED) ? -TITLEBAR_HEIGHT :
#endif /* SHAPED */
		TITLEBAR_HEIGHT),
#else	/* not TITLEBAR */
		0,
#endif	/* TITLEBAR */
		c->geometry.width, 
		c->geometry.height
#ifdef SHAPED
		+ ((c->flags & AR_CLIENT_SHAPED) ? TITLEBAR_HEIGHT : 0)
#endif /* SHAPED */
		);
	send_config(c);
#ifdef TITLEBAR
#ifdef SHAPE
	if(c->flags & AR_CLIENT_SHAPED)
		return;
#endif /* SHAPE */
	LOG_DEBUG("moveresize(c)--\tw:%d\tow:%d\n", c->geometry.width,
		c->exposed_width);
	/* Only update the titlebar if the width has changed.  */
	if((c->geometry.width != c->exposed_width))
		update_info_window(c);
	/* Store width value for above test.  */
	c->exposed_width=c->geometry.width;
#endif /* TITLEBAR */
}

#define ARWM_MAXIMIZE_DIR(pos_dir, siz_dir, cmd, c)\
{\
	if (c->old_geometry.siz_dir)\
	{\
		c->geometry.pos_dir = c->old_geometry.pos_dir;\
		c->geometry.siz_dir = c->old_geometry.siz_dir;\
		c->old_geometry.siz_dir = 0;\
	}\
	else\
	{\
		c->old_geometry.pos_dir = c->geometry.pos_dir;\
		c->old_geometry.siz_dir = c->geometry.siz_dir;\
		c->geometry.pos_dir = 0;\
		c->geometry.siz_dir= cmd(arwm.X.dpy, c->screen->screen);\
	}\
}

void
maximise_client(Client * c, int hv)
{

	if (hv & MAXIMISE_HORZ)
		ARWM_MAXIMIZE_DIR(x, width, DisplayWidth, c);
	if (hv & MAXIMISE_VERT)
		ARWM_MAXIMIZE_DIR(y, height, DisplayHeight, c);
	recalculate_sweep(c, c->geometry.x, c->geometry.y,
		c->geometry.x + c->geometry.width,
		c->geometry.y + c->geometry.height);
	moveresize(c);
	discard_enter_events();
#ifdef TITLEBAR
	select_client(c);
#endif	/* TITLEBAR */
}

void 
hide(Client * c)
{
	/* This will generate an unmap event.  Tell event handler
	 * to ignore it.  */
	c->ignore_unmap++;
	LOG_XDEBUG("screen:XUnmapWindow(parent); ");
	XUnmapWindow(arwm.X.dpy, c->parent);
	set_wm_state(c, IconicState);
}

void 
unhide(Client * c, int raise_win)
{
	raise_win ? XMapRaised(arwm.X.dpy, c->parent) 
		: XMapWindow(arwm.X.dpy, c->parent);
	set_wm_state(c, NormalState);
}

void 
next(void)
{
	Client *newc = current;

	do
	{
		if (newc)
		{
			newc = newc->next;
			if (!newc && !current)
				return;
		}
		if (!newc)
			newc = head_client;
		if (!newc)
			return;
		if (newc == current)
			return;
	}
	/*
	 * NOTE: Checking against newc->screen->vdesk implies we can Alt+Tab
	 * across screen boundaries.  Is this what we want?
	 */
	while (newc->vdesk != newc->screen->vdesk);
	if (!newc)
		return;
	unhide(newc, RAISE);
	select_client(newc);
	setmouse(newc->window, 0, 0);
	setmouse(newc->window, newc->geometry.width + newc->border - 1,
		newc->geometry.height + newc->border - 1);
	discard_enter_events();
}

void 
switch_vdesk(ScreenInfo * s, int v)
{
	Client *c;

	if (v == s->vdesk)
		return;
	if (current && !is_sticky(current))
		select_client(NULL);
	for (c = head_client; c; c = c->next)
	{
		if (c->screen != s)
			continue;
		if (is_sticky(c) && c->vdesk != v)
		{
			c->vdesk = v;
			ARWM_UPDATE_NET_WM_DESKTOP(c);
		}
		if (c->vdesk == s->vdesk)
			hide(c);
		else if (c->vdesk == v)
		{
			unhide(c, NO_RAISE);
		}
	}
	s->vdesk = v;
}

ScreenInfo *
find_screen(Window root)
{
	int i;

	for (i = 0; i < arwm.X.num_screens; i++)
		if (arwm.X.screens[i].root == root)
			return &arwm.X.screens[i];

	return NULL;
}

ScreenInfo *
find_current_screen(void)
{
	Window cur_root, dw;
	int di;
	unsigned int dui;

	/* XQueryPointer is useful for getting the current pointer root */
	XQueryPointer(arwm.X.dpy, arwm.X.screens[0].root, &cur_root,
		&dw, &di, &di, &di, &di, &dui);

	return find_screen(cur_root);
}

static void 
grab_keysym(Window w, unsigned int mask, KeySym keysym)
{
	KeyCode keycode = XKeysymToKeycode(arwm.X.dpy, keysym);

	XGrabKey(arwm.X.dpy, keycode, mask, w, True,
		GrabModeAsync, GrabModeAsync);
	XGrabKey(arwm.X.dpy, keycode, mask | LockMask, w, True,
		GrabModeAsync, GrabModeAsync);
	if (arwm.keymasks.numlock)
	{
		XGrabKey(arwm.X.dpy, keycode, mask | arwm.keymasks.numlock, 
			w, True, GrabModeAsync, GrabModeAsync);
		XGrabKey(arwm.X.dpy, keycode, mask | arwm.keymasks.numlock 
			| LockMask, w, True, GrabModeAsync, GrabModeAsync);
	}
}

void 
grab_keys_for_screen(ScreenInfo * s)
{
	KeySym *keysym;

	/* The key lists are split to macros, 
           as one may wish to define which
	   keys to grab quickly when adding key bindings.  */

	/* Release any previous grabs */
	XUngrabKey(arwm.X.dpy, AnyKey, AnyModifier, s->root);
	/* Grab key combinations we're interested in */
	{
		KeySym keys_to_grab[] = ARWM_KEYS_TO_GRAB;

		for (keysym = keys_to_grab; *keysym; keysym++)
			grab_keysym(s->root, 
				arwm.keymasks.grab1, 
				*keysym);
	}
	{
		KeySym alt_keys_to_grab[] = ARWM_ALT_KEYS_TO_GRAB;

		for (keysym = alt_keys_to_grab; *keysym; keysym++)
			grab_keysym(s->root, arwm.keymasks.grab1 
				| arwm.keymasks.alt, *keysym);
	}
	grab_keysym(s->root, arwm.keymasks.grab2, KEY_NEXT);
}


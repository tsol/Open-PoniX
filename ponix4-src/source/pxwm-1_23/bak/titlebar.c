#ifdef TITLEBAR

#include "arwm.h"
/* Include the title button bitmaps.  */
#ifdef USE_XPM
#include <X11/xpm.h>
#include "close_button.xpm"
#include "close_button_inactive.xpm"
#include "resize_button.xpm"
#include "gradient.xpm"
#include "shade.xpm"
#elif USE_XBM
#include "close.xbm"
#include "resize.xbm"
#endif /* USE_XPM */

#ifdef USE_XFT
#include <X11/Xft/Xft.h>

#endif /* USE_XFT */

static void
setup_titlebar(Client * c)
{
	Display * dpy = arwm.X.dpy;
	Window info_window;

#ifdef SHAPE
	if(c->flags & AR_CLIENT_SHAPED)
		return;
#endif /* SHAPE */

	info_window=c->info_window 
		= XCreateSimpleWindow(dpy, c->parent, 0, 0,
			c->geometry.width, TITLEBAR_HEIGHT,
			TB_BRDR_WDTH, 
			0, 
			arwm_get_pixel_for_color(TITLEBAR_BG));
	XSelectInput(arwm.X.dpy, info_window, ExposureMask);
	XMapRaised(dpy, info_window);
	arwm_grab_button(info_window, 0, AnyButton);
}

static unsigned int
draw_info_strings(Client * c, char * name)
{
	/* Validate inputs.  */
	if (name && c)
	{
#ifndef USE_XFT
		const 
#endif /* ! USE_XFT */
		 unsigned int name_length = strlen(name);
		const int x = BUTTON_WIDTH+TITLEBUTTON_PADDING;
		const int y = TITLE_FONT_Y;

		Window info_window = c->info_window;
#ifdef USE_XFT
		XftDrawChange(arwm.titlebar.draw, info_window);
		{
			/* Prevent the text from going over the resize button.
			*/
			XGlyphInfo extents;
			int max_width = c->geometry.width - 4*TITLEBAR_HEIGHT;

			XftTextExtentsUtf8(arwm.X.dpy, arwm.X.font,
				(XftChar8 *)name, name_length, &extents);
			if(extents.width > max_width)
			{
				name_length=name_length*max_width/extents.width;
			}
		}
		XftDrawStringUtf8(arwm.titlebar.draw, &(arwm.titlebar.black), 
			arwm.X.font, x, y, (XftChar8 *)name, name_length);
#else /* ! USE_XFT */
		Display * dpy = arwm.X.dpy;
		GC text_gc;
		XGCValues values;

		XGetGCValues(arwm.X.dpy, c->screen->invert_gc, 
			GCFont, &values);
		values.foreground=BlackPixel(dpy, DefaultScreen(dpy));
		text_gc=XCreateGC(dpy, info_window, 
			GCFont | GCForeground, &values);
		XDrawString(dpy, info_window, text_gc, x, y, 
			name, name_length);
		XFree(name);
		XFreeGC(dpy, text_gc);
#endif /* USE_XFT */
		return name_length;
	}
	else
		return -1;
}



void
arwm_draw_close_button(Client * c)
{
	ARWMButton * close = arwm.titlebar.buttons.close;

#ifdef SHAPE
	if(c->flags & AR_CLIENT_SHAPED)
		return;
#endif /* SHAPE */

#ifdef TITLEBAR_DEBUG
	LOG_DEBUG("arwm_draw_close_button(c)\n");
	LOG_DEBUG("x:%d\ty:%d\tw:%d\th:%d\n", 
		close->geometry->x, close->geometry->y, 
		close->geometry->width, close->geometry->height);
#endif /* TITLEBAR_DEBUG */
#ifdef USE_XPM
	close->image=(c->flags & AR_CLIENT_ACTIVE) 
		? arwm.titlebar.close : arwm.titlebar.close_inactive;
#endif /* USE_XPM */
	close->parent=c->info_window;
	$(close, draw);
}

static void
draw_handles(Client * c)
{
	Window w = c->info_window;
	ARWMButton *close, *resize, *shade, *handle;
	const unsigned int width = c->geometry.width;
	const unsigned int resize_offset=width-AR_RESIZE_DELTA;
	const unsigned int shade_offset=width-AR_SHADE_DELTA;

	close=arwm.titlebar.buttons.close; 
	resize=arwm.titlebar.buttons.resize;
	shade=arwm.titlebar.buttons.shade;
	handle=arwm.titlebar.buttons.handle;
	
	handle->parent=w;
	handle->geometry->width=width;
	$(handle, draw);

	arwm_draw_close_button(c); 

	shade->parent=w;
	shade->geometry->x=shade_offset;
	$(shade, draw); 

	resize->parent=w;
	resize->geometry->x=resize_offset;
	$(resize, draw);
}

static void
arwm_ARWMTitlebarData_destroy(ARWMTitlebarData * titlebar);

#if defined(USE_XPM) || defined(USE_XBM)
static void
initialize_images(ARWMTitlebarData * titlebar)
{

#ifdef USE_XPM
	Display * dpy = arwm.X.dpy;

#ifdef TITLEBAR_DEBUG
	LOG_DEBUG("Load pixmaps...");
#endif /* TITLEBAR_DEBUG */
	XpmCreateImageFromData(dpy, gradient_xpm, 
		&(titlebar->gradient), NULL, NULL);
	XpmCreateImageFromData(dpy, resize_button_xpm, 
		&(titlebar->resize), NULL, NULL);
	XpmCreateImageFromData(dpy, close_button_xpm, 
		&(titlebar->close), NULL, NULL);
	XpmCreateImageFromData(dpy, close_button_inactive_xpm, 
		&(titlebar->close_inactive), NULL, NULL);
	XpmCreateImageFromData(dpy, shade_xpm, 
		&(titlebar->shade), NULL, NULL);
	titlebar->handles_gc=arwm_new_gc_for_color(0xa7,0xa8,0xa7, 0,0,0);
#elif USE_XBM
	titlebar->resize=arwm_get_XImage_for_XBM(resize_bits, 
		resize_width, resize_height);
	titlebar->close=arwm_get_XImage_for_XBM(close_bits, 
		close_width, close_height);

#endif	/* USE_XPM */
}
#endif /* USE_XPM || USE_XBM */

static void
initialize_buttons(ARWMTitlebarData * titlebar, Display * dpy)
{
	Window root = DefaultRootWindow(dpy);
	ARWMButton *close, *resize, *shade, *handle;

	titlebar->buttons.close_gc=arwm_new_gc_for_color(
		0xdd,0xb2,0xc3, 0,0,0);
	close=titlebar->buttons.close=new_ARWMButton(root, 
		&(titlebar->buttons.close_gc), NULL, 
#if defined(USE_XPM) || defined(USE_XBM)
		titlebar->close
#else
		NULL
#endif
		);
	close->geometry->height=TITLEBAR_HEIGHT;
	close->geometry->width=TITLEBAR_HEIGHT;
#ifdef USE_XBM
	close->image_height=close_height;
	close->image_width=close_width;
#endif

	titlebar->buttons.resize_gc=arwm_new_gc_for_color(
		0xae,0xdd,0xc3, 0,0,0);
	resize=titlebar->buttons.resize=new_ARWMButton(root,
		&(titlebar->buttons.resize_gc), NULL, 
#if defined(USE_XPM) || defined(USE_XBM)
		titlebar->resize
#else
		NULL
#endif
		);
	resize->geometry->height=TITLEBAR_HEIGHT;
	resize->geometry->width=
#ifdef USE_XPM
		24
#else 
		TITLEBAR_HEIGHT
#endif
		;
#ifdef USE_XBM
	resize->image_height=resize_height;
	resize->image_width=resize_width;
#endif

	titlebar->buttons.shade_gc=arwm_new_gc_for_color(
		0xae,0xb2,0xdd, 0,0,0);
	shade=titlebar->buttons.shade=new_ARWMButton(root,
		&(titlebar->buttons.shade_gc), NULL,
#ifdef USE_XPM
		titlebar->shade
#else /* ! USE_XPM */
		NULL
#endif /* USE_XPM */
		);
	shade->geometry->width=TITLEBAR_HEIGHT;
	shade->geometry->height=TITLEBAR_HEIGHT;

	arwm.titlebar.buttons.handle_gc=arwm_new_gc_for_color(
		0xae,0xb2,0xc3, 0,0,0);
	handle=arwm.titlebar.buttons.handle=new_ARWMButton(root,
		&(arwm.titlebar.buttons.handle_gc), NULL,
#ifdef USE_XPM
		titlebar->gradient
#else /* ! USE_XPM */
		NULL
#endif /* USE_XPM */
		);
	handle->geometry->height=TITLEBAR_HEIGHT;
}

#ifdef USE_XFT
static void
initialize_font_data(ARWMTitlebarData * titlebar, Display * dpy)
{
	const int scr = DefaultScreen(dpy);
	Visual * visual = DefaultVisual(dpy, scr);
	Colormap colormap = DefaultColormap(dpy, scr);

	XftColorAllocName(dpy, visual, colormap, "black", 
		&(titlebar->black));
	titlebar->draw=XftDrawCreate(dpy, DefaultRootWindow(dpy), 
		visual, colormap);
}
#endif /* USE_XFT */

void
arwm_ARWMTitlebarData_init(ARWMTitlebarData * titlebar)
{
	Display * dpy = arwm.X.dpy;

	if(titlebar->initialized)
		return;

#if defined(USE_XPM) || defined(USE_XBM)
	initialize_images(titlebar);
#endif /* USE_XPM || USE_XBM */
	initialize_buttons(titlebar, dpy);
#ifdef USE_XFT
	initialize_font_data(titlebar, dpy);
#endif /* USE_XFT */
	titlebar->delete=&arwm_ARWMTitlebarData_destroy;
	titlebar->initialized=True;
}

static void
delete_buttons(ARWMTitlebarData * titlebar)
{
	$(titlebar->buttons.close, delete); 
	$(titlebar->buttons.resize, delete);
	$(titlebar->buttons.shade, delete);
	$(titlebar->buttons.handle, delete);
}

static void
free_GCs(ARWMTitlebarData * titlebar)
{
#if defined(USE_XPM) || defined(USE_XBM)
	XDestroyImage(titlebar->close);
	XFreeGC(arwm.X.dpy, titlebar->buttons.close_gc);

	XDestroyImage(titlebar->resize);
	XFreeGC(arwm.X.dpy, titlebar->buttons.resize_gc);
#ifdef USE_XPM
	XDestroyImage(titlebar->gradient);
	XFreeGC(arwm.X.dpy, titlebar->buttons.handle_gc);
	XDestroyImage(titlebar->shade);
	XFreeGC(arwm.X.dpy, titlebar->buttons.shade_gc);
	XFreeGC(arwm.X.dpy, titlebar->handles_gc);
#endif /* USE_XPM */
#endif /* USE_XPM || USE_XBM */
}

#ifdef USE_XFT
static void
free_Xft_data(ARWMTitlebarData * titlebar)
{
	XftDraw * draw = titlebar->draw;

	XftColorFree(arwm.X.dpy, XftDrawVisual(draw), 
		XftDrawColormap(draw), &(titlebar->black));
	XftDrawDestroy(draw);
}
#endif /* USE_XFT */

/* This assumes that the memory of the ARWMTitlebarData instance
   passed is managed externally.  */
static void
arwm_ARWMTitlebarData_destroy(ARWMTitlebarData * titlebar)
{
	if(!titlebar->initialized)
		return;
	free_GCs(titlebar);
#ifdef USE_XFT
	free_Xft_data(titlebar);
#endif /* USE_XFT */
	delete_buttons(titlebar);
}

void
update_info_window(Client * c)
{
	char *name;

#ifdef SHAPE
	if(c)
		set_shape(c);
#endif /* SHAPE */

	if(!c || (c->flags & AR_CLIENT_DONT_USE_TITLEBAR)
		|| (c->flags & AR_CLIENT_DONT_MANAGE))
	{
		remove_info_window(c);
		return;
	}
	if (!c->info_window)
		setup_titlebar(c);

	/* Client specific data.  */
	XFetchName(arwm.X.dpy, c->window, &name);

	XMoveResizeWindow(arwm.X.dpy, c->info_window, 0, 0, 
		c->geometry.width,
		TITLEBAR_HEIGHT);	
	XClearWindow(arwm.X.dpy, c->info_window);
	/* Depending on common data.  */
	arwm_ARWMTitlebarData_init(&(arwm.titlebar));
	draw_handles(c);
	draw_info_strings(c, name);
}

void
remove_info_window(Client * c)
{
	if (c && c->info_window)
		XDestroyWindow(arwm.X.dpy, c->info_window);
	c->info_window = None;
}

#endif /* TITLEBAR */


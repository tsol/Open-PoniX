/* Copyright 2008, Jeffrey E. Bedard <antiright@gmail.com> */

#include "arwm.h"

#if 0

XImage * 
arwm_render_gradient(unsigned char * data)
{
	XImage * image
	
}

#endif

XImage *
arwm_get_XImage_for_XBM(unsigned char * data, 
	unsigned int width, unsigned int height)
{
	XImage * image;
	Display * dpy = arwm.X.dpy;

	image=XCreateImage(dpy, (Visual *)CopyFromParent,
		1, XYBitmap, 0, (char *)data, width, height, 8, 0);
	assert(image);
	return image;
}

unsigned long
arwm_get_pixel_for_color(const int r, const int g, const int b)
{
	XColor color;
	Display * dpy = arwm.X.dpy;
	const int screen = arwm.X.screens ? arwm.X.screens->screen
		: DefaultScreen(dpy);

	color.red=r*255; 
	color.green=g*255;
	color.blue=b*255;

	XAllocColor(dpy, DefaultColormap(dpy, screen), &color);

	return color.pixel;
}

GC
arwm_new_gc_for_color(	int fg_r, int fg_g, int fg_b, 
	int bg_r, int bg_g, int bg_b)
{
	unsigned long valuemask;
	XGCValues values;
	Display * dpy = arwm.X.dpy;
	GC gc;

	values.foreground=arwm_get_pixel_for_color(fg_r,fg_g,fg_b);
	values.background=arwm_get_pixel_for_color(bg_r,bg_g,bg_b);
	valuemask = GCForeground | GCBackground;
	gc=XCreateGC(dpy, DefaultRootWindow(dpy), 
		valuemask, &values);

	return gc;	
}


static void
delete_ARWMButton(ARWMButton * button)
{
#ifdef GRAPHICS_DEBUG
	LOG_DEBUG("delete_ARWMButton()\n");
#endif
	if(button->__flags & __ARWMBUTTON_ALLOCATED_GEOMETRY)
	{
#ifdef GRAPHICS_DEBUG
		LOG_DEBUG("\t\tfree(geometry)\n");
#endif
		free(button->geometry);
	}
	if(button->__flags & __ARWMBUTTON_ALLOCATED_GC)
	{
#ifdef GRAPHICS_DEBUG
		LOG_DEBUG("\t\tfree(gc)\n");
#endif
		XFreeGC(arwm.X.dpy, button->gc);
	}
#ifdef GRAPHICS_DEBUG
	LOG_DEBUG("\tfree(button)\n");
#endif
	free(button);
}

#ifndef USE_XPM
static void
frame_button(Window info_window, int x, int y, int w, int h,
	Bool outset)
{
	Display * dpy = arwm.X.dpy;
	GC gc;
	GC highlight=arwm_new_gc_for_color(0xff,0xff,0xff, 0,0,0);
	GC shadow = arwm_new_gc_for_color(0,0,0, 0,0,0);

	gc=outset?highlight:shadow;
	XDrawLine(dpy, info_window, gc, x, y, x+w, y);
	XDrawLine(dpy, info_window, gc, x, y, x, y+h);
	XFreeGC(dpy, gc);

	gc=outset?shadow:highlight;
	XDrawLine(dpy, info_window, gc, x+w, y, x+w, y+h);
	XDrawLine(dpy, info_window, gc, x, y+h, x+w, y+h);
	XFreeGC(dpy, gc);
}
#endif /* !USE_XPM */

static void
ARWMButton_draw(ARWMButton * button)
{
	const int x = button->geometry->x;
	const int y = button->geometry->y;
	const unsigned int width = button->geometry->width;
	const unsigned int height = button->geometry->height;

#ifdef GRAPHICS_DEBUG
	LOG_DEBUG("ARWMButton_draw()\n");
#endif /* GRAPHICS_DEBUG */
#ifndef USE_XPM
	XFillRectangle(arwm.X.dpy, button->parent, button->gc, 
		x, y, width, height);
#endif /* ! USE_XPM */
#if defined(USE_XPM) || defined(USE_XBM)
	if(button->image)
	{
		int image_x=x, image_y=y;

#ifdef GRAPHICS_DEBUG
		LOG_DEBUG("\tXPutImage()\n");
#endif /* GRAPHICS_DEBUG */
		if(button->image_width)
			image_x+=(button->geometry->width
				-button->image_width)/2;
		if(button->image_height)
			image_y+=(button->geometry->height
				-button->image_width)/2;
		XPutImage(arwm.X.dpy, button->parent, button->gc, 
			button->image, 0, 0, image_x, image_y, width, height);
	}
#endif /* USE_XPM || USE_XBM */
#ifndef USE_XPM
	frame_button(button->parent, x, y, width, height, True);
#endif /* ! USE_XPM */
}

ARWMButton *
new_ARWMButton(Window parent, GC * gc, XRectangle * geometry, void * image)
{
	ARWMButton * button;

#ifdef GRAPHICS_DEBUG
	LOG_DEBUG("new_ARWMButton()\n");
#endif /* GRAPHICS_DEBUG */

	button=malloc(sizeof(ARWMButton));

	button->delete=&delete_ARWMButton;
	button->draw=&ARWMButton_draw;
	button->__flags=None;
	button->parent=parent;
#if defined(USE_XPM) || defined(USE_XBM)
	button->image_width=button->image_height=0;
#endif /* USE_XPM || USE_XBM */
	if(gc)
		button->gc=*gc;
	else
	{
		button->gc=arwm_new_gc_for_color(0xae,0xb2,0xc3, 0,0,0);
		button->__flags |= __ARWMBUTTON_ALLOCATED_GC;
	}
	
	if(geometry)
		button->geometry=geometry;
	else
	{
		button->geometry=calloc(1, sizeof(XRectangle));
		button->__flags |= __ARWMBUTTON_ALLOCATED_GEOMETRY;
	}

#if defined(USE_XPM) || defined(USE_XBM)
	button->image=image;
#else
	(void)image;
#endif /* USE_XPM || USE_XBM */

	return button;
}



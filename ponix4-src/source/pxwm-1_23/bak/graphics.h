#ifndef ARWM_GRAPHICS_H
#define ARWM_GRAPHICS_H

typedef struct _ARWMButton
{
	Window parent;
	GC gc;
	XRectangle * geometry;

#if defined(USE_XPM) || defined(USE_XBM)
	XImage * image;
	unsigned int image_width, image_height;
#endif /* USE_XPM || USE_XBM */

	unsigned int __flags;
#define __ARWMBUTTON_ALLOCATED_GC (1<<0)
#define __ARWMBUTTON_ALLOCATED_GEOMETRY (1<<1)

	void (*draw)(struct _ARWMButton *);
	void (*delete)(struct _ARWMButton *);
} ARWMButton;


ARWMButton *
new_ARWMButton(Window parent, GC * gc, XRectangle * geometry, void * image);

unsigned long
arwm_get_pixel_for_color(const int r, const int g, const int b);

GC
arwm_new_gc_for_color(int fg_r, int fg_g, int fg_b, 
	int bg_r, int bg_g, int bg_b);

XImage *
arwm_get_XImage_for_XBM(unsigned char * data, 
	unsigned int width, unsigned int height);


#endif /* ARWM_GRAPHICS_H */


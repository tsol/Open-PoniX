#ifndef ARWM_TITLEBAR_H
#define ARWM_TITLEBAR_H

#define TB_BRDR_WDTH 0

#include "graphics.h"

typedef struct _ARWMTitlebarData
{
	Bool initialized;

	struct
	{
		GC close_gc, resize_gc, shade_gc, handle_gc;
		ARWMButton *close, *resize, *shade, *handle;
	} buttons;

#ifdef USE_XFT
	XftDraw * draw;
	XftColor black;
#endif /* USE_XFT */
#ifdef USE_XPM
	GC handles_gc;
	XImage * close, * close_inactive, * resize, * gradient, * shade;
#elif USE_XBM
	XImage * close, * resize;
#endif

	void (*delete)(struct _ARWMTitlebarData *);

} ARWMTitlebarData;

void
arwm_ARWMTitlebarData_init(ARWMTitlebarData * titlebar);

void
update_info_window(Client * c);

void 
remove_info_window(Client * c);

void
arwm_draw_close_button(Client * c);

#define AR_BUTTON_WIDTH TITLEBAR_HEIGHT
#define AR_BUTTON_HEIGHT TITLEBAR_HEIGHT
#ifdef USE_XPM
#define AR_RESIZE_BUTTON_WIDTH 24
#else
#define AR_RESIZE_BUTTON_WIDTH AR_BUTTON_WIDTH
#endif
#define AR_RESIZE_DELTA (AR_RESIZE_BUTTON_WIDTH+TITLEBUTTON_PADDING)
#define AR_SHADE_DELTA (AR_RESIZE_DELTA+AR_BUTTON_WIDTH+TITLEBUTTON_PADDING)

#endif /* ARWM_TITLEBAR_H */


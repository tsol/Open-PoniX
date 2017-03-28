
#ifndef ARWM_ENVIRONMENT_H
#define ARWM_ENVIRONMENT_H
#ifdef TITLEBAR
#include "titlebar.h"
#endif /* TITLEBAR */

typedef struct
_ARWMNetWMWindowType
{
	Atom desktop, dock, toolbar, menu, utility, splash,
		dialog, dropdown_menu, popup_menu, tooltip,
		notification, combo, dnd, normal;
} ARWMNetWMWindowType;

struct 
ARWMEnvironment
{
	volatile Window initialising;

	struct
	{
		struct
		{
			/* Standard X protocol atoms */
			Atom state, protos, delete, cmapwins;
		} wm;
		struct
		{
			Atom desktop, state, sticky;
		} vwm;
		Atom mwm_hints;
		ARWMNetWMWindowType net_wm_window_type;
	} atoms;
	struct
	{
		unsigned int numlock, grab1, grab2, alt;
	} keymasks;
	struct
	{
		/* Commonly used X information */
		Display *dpy;
#ifdef USE_XFT
		XftFont * font;
#else /* ! USE_XFT */
		XFontStruct *font;
#endif /* USE_XFT */
		int num_screens;
		ScreenInfo *screens;

		struct
		{
			Cursor move;
			Cursor resize;
		} cursors;
#ifdef SHAPE
		int have_shape, shape_event;
#endif /* SHAPE */
	} X;
	struct
	{
		int bw;
	/*	char * terminal[3];
		const char * display=""; */
#ifdef SNAP
		int snap;
#endif /* SNAP */
#ifdef SOLIDDRAG
		Bool solid_drag;
#endif /* SOLIDDRAG */
/*		struct
		{
			char * fg;
			char * bg;
			char * fc;
		} colors; */
	} options;

#ifdef TITLEBAR
	ARWMTitlebarData titlebar;
#endif /* TITLEBAR */
};
typedef struct ARWMEnvironment ARWMEnvironment;
#endif /* not ARWM_ENVIRONMENT_H */

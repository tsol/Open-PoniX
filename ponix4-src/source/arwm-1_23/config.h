/* Copyright 2007, Jeffrey E. Bedard */
/*
 * This file is ment to hold configuration values and to resolve
 * configuration dependencies.
 */

#ifndef ARWM_CONFIG_H
#define ARWM_CONFIG_H


/* These are the default settings.  */
#ifdef USE_XFT
#define DEF_FONT "Sans 10"
#define DEF_FONT_ALT1 "Helvetica 10"
#define DEF_FONT_ALT2 "fixed"
#else /* ! USE_XFT */
#define DEF_FONT "7x14bold"
#define DEF_FONT_ALT1   "7x13bold"
#define DEF_FONT_ALT2	"7x14" 
#endif /* USE_XFT */
#define DEF_FG          "#d4d5dd"
#define DEF_BG          "#9c9fa2"
#define TITLEBAR_BG	0xb5, 0xb6, 0xbe
#define DEF_BW          3
#define DEF_FC          "#aeb2c3"
#define SPACE           3
#ifdef DEBIAN
#define DEF_TERM        "x-terminal-emulator"
#else
#define DEF_TERM        "xterm"
#endif

#include "macros.h"

/* Values.  */
#define TITLEBUTTON_PADDING (arwm.options.bw)
#ifdef USE_XFT
#define TITLE_FONT_HEIGHT arwm.X.font->height
#else /* ! USE_XFT */
#define TITLE_FONT_HEIGHT (arwm.X.font->ascent+arwm.X.font->descent)
#endif /* USE_XFT */
#ifdef USE_XFT
#define TITLE_FONT_Y (TITLE_FONT_HEIGHT - TITLEBUTTON_PADDING)
#else /* ! USE_XFT */
#define TITLE_FONT_Y (TITLE_FONT_HEIGHT + TITLEBUTTON_PADDING)
#endif /* USE_XFT */
/* Ensure that the title text fits within the titlebar.  */
#define TITLEBAR_HEIGHT MAX(22,(TITLE_FONT_HEIGHT+TITLEBUTTON_PADDING))

#define BUTTON_WIDTH (TITLEBAR_HEIGHT-TITLEBUTTON_PADDING)
#define ARWM_RESIZE_INCREMENT TITLEBAR_HEIGHT

#ifdef SNAP
#define ARWM_DEFAULT_SNAP 4
#endif /* SNAP */

#endif	/* ARWM_CONFIG_H */


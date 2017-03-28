/* 
 *  Matchbox Window Manager - A lightweight window manager not for the
 *                            desktop.
 *
 *  Authored By Matthew Allum <mallum@o-hand.com>
 *
 *  Copyright (c) 2002, 2004 OpenedHand Ltd - http://o-hand.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#ifndef _MBTHEME_H_
#define _MBTHEME_H_

#include "structs.h"
#include "wm.h"
#include "list.h"

/***

 Feel Free to experiment with below defines, to get a standalone 'theme' 
 to suit your tastes. Also see _draw_button() and theme_frame_paint() for
 more extreme changes


 ***/

/* Various frame sizes in pixels. */

#define FRAME_MAIN_HEIGHT        20
#define FRAME_DIALOG_HEIGHT      20
#define FRAME_DIALOG_BORDER_SIZE 1
#define FRAME_TOOLBAR_MAX_SIZE   16
#define FRAME_TOOLBAR_MIN_SIZE   16

/* Line width for buttons  */

#define THEME_LINE_WIDTH         2

/* Frame Colors */

#define THEME_FG_COLOR           "#496179"
#define THEME_FG_HIGHLIGHT_COLOR "#697d96"
#define THEME_FG_LOWLIGHT_COLOR  "#384961"

/* Text / button color - white  */

#define THEME_TEXT_COLOR         "#ffffff"

/* Fonts, notice you can list alternates seperated by a '|' */

#ifdef USE_XFT
#define THEME_FONT_MAIN    "sans serif-10:bold"
#define THEME_FONT_TOOLBAR "sans serif-8:bold"
#else
#define THEME_FONT_MAIN    "-*-helvetica-bold-*-normal-*-12-*-*-*-*-*-*-*|fixed"
#define THEME_FONT_TOOLBAR "6x10|5x7|fixed"
#endif


/* Simple theme struct */

typedef struct _mbtheme 
{
  XColor       col_fg;
  XColor       col_fg_lowlight;
  XColor       col_fg_highlight;
  XColor       col_text;
  GC           gc, mask_gc, band_gc;

#ifdef USE_XFT
  XftFont     *font;
  XftFont     *font_toolbar;
  XftColor     xftcol;
#else
  XFontStruct *font;
  XFontStruct *font_toolbar;
#endif


  struct _wm  *wm;
   
} MBTheme;


/* font stuff */

#define font_height(font) (((font)->ascent) + ((font)->descent))
#define font_ascent(font) ((font)->ascent)
#define font_ref(font) ((font))

#ifdef USE_XFT
Bool
font_load (Wm                   *w, 
	   char                 *spec, 
           XftFont              **font);
#else
Bool
font_load (Wm                   *w, 
	   char                 *spec, 
           XFontStruct          **font);
#endif

#ifdef USE_XFT
void
font_paint (Wm                   *w, 
	    XftDraw              *drawable,
	    XftColor             *color,
	    XftFont              *font,
	    unsigned char        *text,
	    int                   text_len,
	    Bool                  text_is_utf8,
	    int                   x,
	    int                   y);
#else
void
font_paint (Wm                   *w, 
	    Drawable              drawable,
	    XColor               *color,
	    XFontStruct          *font,
	    GC                    gc,
	    unsigned char        *text,
	    int                   text_len,
	    int                   x,
	    int                   y);
#endif


#ifdef USE_XFT
int
font_get_str_width (Wm                   *w, 
		    XftFont              *font,
		    unsigned char        *text,
		    int                   text_len,
		    Bool                  text_is_utf8);
#else
int
font_get_str_width (Wm                   *w, 
		    XFontStruct          *font,
		    unsigned char        *text,
		    int                   text_len,
		    Bool                  text_is_utf8);
#endif


/* funcs, some are just stubs  */

void 
theme_paint_rgba_icon (MBTheme       *t,
		       Client        *c,
		       Drawable       drw,
		       int            x,
		       int            y,
		       int           *data);

Bool 
theme_frame_wants_shaped_window (MBTheme       *theme,
				 int            frame_type);

Bool 
theme_frame_supports_button_type (MBTheme       *theme,
				  int            frame_type,
				  int            button_type);

int 
theme_frame_defined_width_get (MBTheme       *theme,
			       int            frame_type );

int 
theme_frame_defined_height_get (MBTheme       *theme,
				int            frame_ref );

Bool
theme_has_frame_type_defined (MBTheme      *theme, 
			      int           frame_type);


void     
theme_img_cache_clear (MBTheme       *theme,
		       int            frame_ref );

void     
theme_img_cache_clear_all (MBTheme       *theme );

void     
theme_frame_button_paint (MBTheme       *theme,
			  Client        *c,
			  int            action,
			  int            state,
			  int            frame_type,
			  int            dest_w,
			  int            dest_h );

Bool     
theme_frame_paint (MBTheme       *theme,
		   Client        *c,
		   int            frame_ref,
		   int            dw,
		   int            dh );

void 
theme_frame_menu_paint (MBTheme       *theme,
			Client        *c);

Bool     
theme_frame_menu_get_dimentions (MBTheme       *theme,
				  int           *w,
				  int           *h);

void
theme_frame_menu_highlight_entry(Client *c, 
				 MBClientButton *button, 
				 int mode);

Bool
theme_has_message_decor( MBTheme *theme );

Bool
theme_has_borders_only_decor( MBTheme *theme );

MBTheme* 
mbtheme_new (Wm            *w);

void     
mbtheme_switch (Wm            *w,
		char          *new_theme_conf);

void     
mbtheme_init (Wm            *w,
	      char          *theme_conf);

int      
theme_frame_button_get_x_pos (MBTheme       *theme,
			      int            frame_type,
			      int            button_type,
			      int            width);

Bool
mbtheme_get_titlebar_panel_rect(MBTheme    *theme, 
				XRectangle *rect,
				Client     *ignore_client);

Bool
mbtheme_has_titlebar_panel(MBTheme *theme);

void
theme_pixmap_cache_clear_all( MBTheme *theme );


#endif

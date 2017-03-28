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
#include "xml.h"
#include "wm.h"
#include "list.h"

#define ERROR_MISSING_PARAMS   -1
#define ERROR_INCORRECT_PARAMS -2
#define ERROR_LOADING_RESOURCE -3

typedef struct _mb_theme_param 
{
   enum { 
     pixel, 
     percentage, 
     object, 
     textx, 
     textw, 
     MBParamLabelEnd, 
     MBParamTotalLabelWidth, 
   } unit;
  signed int value;
  signed int offset;

} MBThemeParam;

enum {
  LAYER_GRADIENT_HORIZ = 1,
  LAYER_GRADIENT_VERT,
  LAYER_LABEL,
  LAYER_PLAIN,
  LAYER_PIXMAP,
  LAYER_PIXMAP_TILED,
  LAYER_PICTURE,
  LAYER_PICTURE_TILED,
  LAYER_ICON,
  LAYER_SUB_LABEL,

} MBThemeLayerType;

typedef struct _mb_theme_label 
{
  Alignment  justify;
  MBColor   *col;
  MBColor   *bgcol;
  MBFont    *font;
  MBThemeParam  *sublabel_label_clip_w;
} MBThemeLabel;

typedef struct _mb_theme_button {
   
  int action;

  Bool inputonly;
  Bool press_activates; 
  Bool wants_dbl_click;
   
  MBThemeParam *x;
  MBThemeParam *y;
  MBThemeParam *w;
  MBThemeParam *h;

  MBPixbufImage *img_active;
  MBPixbufImage *img_inactive;

  int img_active_blend;
  int img_inactive_blend;
   
} MBThemeButton;

typedef struct _mb_theme_layer {

  MBThemeParam *x;
  MBThemeParam *y;
  MBThemeParam *w;
  MBThemeParam *h;
  
  MBColor  *color;
  MBPixbufImage *img;
  MBThemeLabel  *label;  
  
  MBColor  *color_end; 	/* for gradients */
  
} MBThemeLayer;


typedef struct _mb_theme_frame 
{
  int                     type;
  struct list_item       *layers;
  struct list_item       *buttons;

  int                     set_height;
  int                     set_width;
  
  char                   *options;
  Bool                    wants_shape;

  /*  FIXME: 
   *    Can save some bytes by do some C style OO casting magic on 
   *    MBThemeFrame instances to save wastage of bytes below on 
   *    frames that dont need this infomation.
   */

  /* Only for frames display text  */

  int                     label_w;
  int                     label_h; 
  int                     label_x;

  int                     sublabel_x;
  int                     sublabel_w;


  MBFont                 *font;
  MBColor                *color;
  MBColor                *hl_color;

  /* Only for menu frame */

  int                    border_n;
  int                    border_e;
  int                    border_s;
  int                    border_w;

  int                   fixed_width;
  int                   fixed_x;
   
} MBThemeFrame;

typedef struct _mbtheme {

  struct list_item* frames;
  struct list_item* images;
  struct list_item* pictures;
  struct list_item* colors;
  struct list_item* fonts;

  GC                gc, mask_gc, band_gc; /* for drag window  */

  MBPixbufImage* img_caches[N_FRAME_TYPES];

  /* For toolbar in panel */
  Bool          have_toolbar_panel;
  MBThemeParam *toolbar_panel_x;
  MBThemeParam *toolbar_panel_y;
  MBThemeParam *toolbar_panel_w;
  MBThemeParam *toolbar_panel_h;

  /* For image substitution */
  char           subst_char;
  MBPixbufImage *subst_img;

  /* App side decoration pixmap cache */

  Pixmap app_win_pxm_cache[3];

  /* disable cacheing, not recommened */
  Bool           disable_pixbuf_cache;

  struct _wm    *wm;
   
} MBTheme;


void     
theme_paint_rgba_icon (MBTheme       *t,
		       Client        *c,
		       Drawable       drw,
		       int            x,
		       int            y,
		       int           *data);

void     
theme_frame_icon_paint (MBTheme       *t,
			Client        *c,
			MBPixbufImage *img_dest,
			int            x,
			int            y);

Bool     
theme_frame_wants_shaped_window  (MBTheme *theme,
				  int      frame_type);

Bool     
theme_frame_supports_button_type (MBTheme *theme,
				  int      frame_type,
				  int      button_type);

Bool
theme_has_frame_type_defined (MBTheme *theme, 
			      int      frame_type);

int      
theme_frame_defined_width_get (MBTheme *theme,
			       int      frame_type );

int      
theme_frame_defined_height_get (MBTheme       *theme,
				int            frame_ref );

Bool
theme_has_message_decor( MBTheme *theme );

Bool
theme_has_borders_only_decor( MBTheme *theme );

void     
theme_img_cache_clear (MBTheme       *theme,
		       int            frame_ref );

void     
theme_img_cache_clear_all (MBTheme       *theme );

void
theme_pixmap_cache_clear_all( MBTheme *theme );


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
theme_frame_menu_highlight_entry(Client *c, 
				 MBClientButton *button, 
				 int mode);

void     
theme_frame_menu_paint (MBTheme       *theme,
			Client        *c);

Bool     
theme_frame_menu_get_dimentions  (MBTheme       *theme,
				  int           *w,
				  int           *h);

MBTheme* 
mbtheme_new  (Wm            *w);

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

Bool
mbtheme_button_press_activates(MBThemeButton* button);


#endif

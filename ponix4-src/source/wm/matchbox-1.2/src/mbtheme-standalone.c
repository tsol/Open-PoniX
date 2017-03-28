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

#include "mbtheme-standalone.h"

#ifdef USE_PANGO
#error Standalone builds do not currently support Pango. 
#endif

#define BUTTON_CROSS 1
#define BUTTON_ARROW 2

typedef struct MBMiniDrawable 
{
  Pixmap   pxm;
  
#ifdef USE_XFT
  XftDraw *xftdraw;
#endif

} MBMiniDrawable;


/* Simple font abstraction - previously in font.c*/

#ifdef USE_XFT
Bool
font_load (Wm                   *w, 
	   char                 *spec, 
           XftFont              **font)
#else
Bool
font_load (Wm                   *w, 
	   char                 *spec, 
           XFontStruct          **font)
#endif
{
   const char delim[] = "|";
   char *orig = NULL, *str = NULL, *token = NULL;

   orig = str = strdup(spec);
      
   while( (token = strsep (&str, delim)) != NULL )
     {
#ifdef USE_XFT
       if ((*font = XftFontOpenName(w->dpy, w->screen, token)) != NULL)
	 { 
	   if (orig) free(orig); 
	   return True; 
	 }
#else
       if ((*font = XLoadQueryFont(w->dpy, token)) != NULL)
	 { 
	   if (orig) free(orig);  
	   return True; 
	 }
#endif
     }
      
   if (orig) free(orig);
   fprintf(stderr, "Matchbox: *warning* unable to load font : %s\n", spec);
      
   return False;
}


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
	    int                   y)
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
	    int                   y)
#endif
{
#ifdef USE_XFT
      if (text_is_utf8)
	XftDrawStringUtf8(drawable,
			  color, 
			  font,
			  x,
			  y,
			  text, 
			  ewmh_utf8_get_byte_cnt(text, text_len));
      else XftDrawString8(drawable,
			  color, 
			  font,
			  x,
			  y,
			  text, 
			  text_len);
#else
      XSetFont(w->dpy, gc, font->fid);
      XSetForeground(w->dpy, gc, color->pixel);
      XDrawString(w->dpy, drawable, gc, x, y, text, text_len);
#endif
}


#ifdef USE_XFT
int
font_get_str_width (Wm                   *w, 
		    XftFont              *font,
		    unsigned char        *text,
		    int                   text_len,
		    Bool                  text_is_utf8)
#else
int
font_get_str_width (Wm                   *w, 
		    XFontStruct          *font,
		    unsigned char        *text,
		    int                   text_len,
		    Bool                  text_is_utf8)
#endif
{
#ifdef USE_XFT   
  XGlyphInfo extents;
  if (text_is_utf8)
     XftTextExtentsUtf8(w->dpy, font, (unsigned char *)text,
			ewmh_utf8_get_byte_cnt(text, text_len), 
			&extents);
   else
     XftTextExtents8(w->dpy, font, (unsigned char *)text, text_len, &extents);
   return extents.width;
#else
   return XTextWidth(font, text, text_len);
#endif
}


static void
_draw_button(MBTheme *theme, Drawable drw, int type, int size, int state)
{
  Pixmap pxm_backing;

  dbg("%s() called \n", __func__);

  pxm_backing = XCreatePixmap(theme->wm->dpy, theme->wm->root, size, size, 
			      DefaultDepth(theme->wm->dpy, theme->wm->screen));

  /* Draw button background */

  XSetForeground(theme->wm->dpy, theme->gc, theme->col_fg.pixel);
  XFillRectangle(theme->wm->dpy, pxm_backing, theme->gc, 0, 0, size, size);

  XSetLineAttributes(theme->wm->dpy, theme->gc, THEME_LINE_WIDTH, 
		     LineSolid, CapRound, JoinRound);

  if (state == ACTIVE)
    XSetForeground(theme->wm->dpy, theme->gc, 
		   BlackPixel(theme->wm->dpy, theme->wm->screen));
  else
    XSetForeground(theme->wm->dpy, theme->gc, theme->col_text.pixel);

  if (type == BUTTON_CROSS)
    {
      size--;
      XDrawLine(theme->wm->dpy, pxm_backing, theme->gc, 0, 0, size, size);
      XDrawLine(theme->wm->dpy, pxm_backing, theme->gc, size, 0, 0, size);
    }
  else
    {
      int i;
      for (i=0; i<6; i++)
	{
	  XFillRectangle(theme->wm->dpy, 
			 pxm_backing, 
			 theme->gc, 
			 i, i+2, 
			 size-(2*i), 1);
	}
    }

  XSetWindowBackgroundPixmap(theme->wm->dpy, drw, pxm_backing);
  XClearWindow(theme->wm->dpy, drw);   

  XFlush(theme->wm->dpy);

  XFreePixmap(theme->wm->dpy, pxm_backing);
}


void
theme_paint_rgba_icon(MBTheme *t, Client *c, 
		      Drawable drw, int x, int y, 
		      int *data)
{
  return;
}

Bool theme_frame_wants_shaped_window( MBTheme *theme, int frame_type)
{
  return False;
}

Bool theme_frame_supports_button_type(MBTheme *theme, 
				      int frame_type, 
				      int button_type)
{
  return True; 			/* XXX THIS ISN'T RIGHT */
}

int theme_frame_defined_width_get( MBTheme *theme, int frame_type )
{
  if (frame_type == FRAME_UTILITY_MAX) return FRAME_TOOLBAR_MAX_SIZE;
  if (frame_type == FRAME_DIALOG_EAST || frame_type == FRAME_DIALOG_WEST
      || frame_type == FRAME_MAIN_EAST || frame_type == FRAME_MAIN_WEST)
    return FRAME_DIALOG_BORDER_SIZE;
  return 0;
}

int theme_frame_defined_height_get( MBTheme *theme, int frame_type )
{
  switch (frame_type)
    {
    case FRAME_MAIN:
      return FRAME_MAIN_HEIGHT;
    case FRAME_DIALOG:
      return FRAME_DIALOG_HEIGHT;
    case FRAME_UTILITY_MIN:
      return FRAME_TOOLBAR_MIN_SIZE;
    case FRAME_DIALOG_SOUTH:
    case FRAME_DIALOG_NORTH:
    case FRAME_MAIN_SOUTH:
      return FRAME_DIALOG_BORDER_SIZE;
    }
  return 0;
}

Bool
theme_has_frame_type_defined (MBTheme      *theme, 
			      int           frame_type)
{
  switch (frame_type)
    {
    case FRAME_MAIN:
    case FRAME_MAIN_EAST:
    case FRAME_MAIN_WEST:
    case FRAME_MAIN_SOUTH:
    case FRAME_DIALOG:
    case FRAME_UTILITY_MAX:
    case FRAME_UTILITY_MIN:
    case FRAME_DIALOG_NORTH:
    case FRAME_DIALOG_EAST:
    case FRAME_DIALOG_WEST:
    case FRAME_DIALOG_SOUTH:
      return True;
    }
  return False;
}

void 
theme_img_cache_clear( MBTheme *theme,  int frame_ref )
{
  return;
}

void 
theme_img_cache_clear_all( MBTheme *theme )
{
  return;
}

void theme_frame_button_paint(MBTheme *theme, 
			      Client  *c, 
			      int      action,
			      int      state, 
			      int      frame_type, 
			      int      dest_w, 
			      int      dest_h )
{

  MBClientButton *button = NULL;
  int button_size       = FRAME_MAIN_HEIGHT - 8;
  int button_small_size = FRAME_TOOLBAR_MAX_SIZE - 6;

  struct list_item* client_button_obj = c->buttons;

  while (client_button_obj != NULL)
    {
      if (client_button_obj->id == action) 
	{
	  button = (MBClientButton*)client_button_obj->data;
	  break;
	}
      client_button_obj = client_button_obj->next;
    }

  switch(frame_type)
    {
      case FRAME_MAIN:
	switch (action)
	  {

	  case BUTTON_ACTION_CLOSE:
	    if (button == NULL)
	      {
		button = client_button_new (c, 
					    client_title_frame(c),
					    dest_w - button_size - 4,
					    (FRAME_MAIN_HEIGHT-button_size)/2,
					    button_size,
					    button_size,
					    False,
					    NULL );
		list_add(&c->buttons, NULL, action, (void *)button);
	      }
	    _draw_button(theme, button->win, BUTTON_CROSS, 
			 button_size, state);
	    break;

	  case BUTTON_ACTION_MENU:
	    dbg("%s() painting menu button %p\n", __func__, button );
	    if (button == NULL)
	      {
		dbg("%s() button is NULL\n", __func__ );
		button = client_button_new (c, 
					    client_title_frame(c),
					    4,
					    (FRAME_MAIN_HEIGHT-button_size)/2,
					    button_size,
					    button_size,
					    False,
					    NULL );
		list_add(&c->buttons, NULL, action, (void *)button);
	      }
	    else if (button->win == None)
	      {
		/* XXX Hack, to work round desktop window issues
		             removing the button window..
		*/
		XSetWindowAttributes attr;
		attr.override_redirect = True;
		attr.event_mask = ExposureMask;
	      
		button->win = XCreateWindow(c->wm->dpy, client_title_frame(c),
					    4,
					    (FRAME_MAIN_HEIGHT-button_size)/2,
					    button_size,
					    button_size,
					    0,
					    CopyFromParent, 
					    CopyFromParent,  
					    CopyFromParent,
					    CWOverrideRedirect|CWEventMask,
					    &attr);
		XMapWindow(c->wm->dpy, button->win);
	      }
	      
	    _draw_button(theme, button->win, BUTTON_ARROW, 
			 button_size, state);
	    dbg("%s() painting close button at %i\n", __func__, dest_w - 20 );
	    break;
	  }
      break;
    case FRAME_DIALOG:
	if (action == BUTTON_ACTION_CLOSE)
	  {
	    if (button == NULL)
	      {
		button = client_button_new (c, 
					    client_title_frame(c),
					    dest_w - button_size - 4,
					    (FRAME_MAIN_HEIGHT-button_size)/2,
					    button_size,
					    button_size,
					    False,
					    NULL );
		list_add(&c->buttons, NULL, action, (void *)button);
	      }
	    _draw_button(theme, button->win, BUTTON_CROSS, button_size, state);

	  }
	break;
    case FRAME_UTILITY_MAX:
	if (action == BUTTON_ACTION_MIN)
	  {
	    if (button == NULL)
	      {
		button = client_button_new (c, 
					    client_title_frame(c),
					    2,
					    2,
					    button_small_size,
					    button_small_size,
					    False,
					    NULL );
		list_add(&c->buttons, NULL, action, (void *)button);
	      }
	    _draw_button(theme, button->win, BUTTON_ARROW, 
			 button_small_size, state);
	    dbg("%s() painting min button at %i\n", __func__, dest_w - 20 );
	  }
	break;
    case FRAME_UTILITY_MIN:
	if (action == BUTTON_ACTION_CLOSE)
	  {
	    if (button == NULL)
	      {
		button = client_button_new (c, 
					    client_title_frame(c),
					    dest_w - FRAME_TOOLBAR_MIN_SIZE + 2,
					    2,
					    button_small_size,
					    button_small_size,
					    False,
					    NULL );
		list_add(&c->buttons, NULL, action, (void *)button);
	      }
	    _draw_button(theme, button->win, BUTTON_CROSS, 
			 button_small_size, state);
	    dbg("%s() painting close button at %i\n", __func__, dest_w - 20 );
	  }
	else if (action == BUTTON_ACTION_MAX)
	  {
	    if (button == NULL)
	      {
		button = client_button_new (c, 
					    client_title_frame(c),
					    0,
					    2,
					    dest_w - FRAME_TOOLBAR_MIN_SIZE,
					    dest_h,
					    True,
					    NULL );
		list_add(&c->buttons, NULL, action, (void *)button);
	      }
	  }

	break;
    default: break;
    }

}

static void
_theme_paint_pixmap_border( MBTheme *theme,
			    Pixmap   pxm_backing,
			    int      dw,
			    int      dh)
{
  XSetForeground(theme->wm->dpy, theme->gc, 
		 BlackPixel(theme->wm->dpy, theme->wm->screen));
  XFillRectangle(theme->wm->dpy, pxm_backing, theme->gc, 
		 0, 0, dw, dh);

  XSetForeground(theme->wm->dpy, theme->gc, theme->col_fg_highlight.pixel);
  XFillRectangle(theme->wm->dpy, pxm_backing, theme->gc, 
		 1, 1, dw-2, dh-2);
  XSetForeground(theme->wm->dpy, theme->gc, theme->col_fg_lowlight.pixel);
  XFillRectangle(theme->wm->dpy, pxm_backing, theme->gc, 
		 2, 2, dw-3, dh-3);
  XSetForeground(theme->wm->dpy, theme->gc, theme->col_fg.pixel);
  XFillRectangle(theme->wm->dpy, pxm_backing, theme->gc, 
		 2, 2, dw-4, dh-4);

}

Bool 
theme_frame_paint( MBTheme *theme, 		   
		   Client  *c, 
		   int frame_ref, 
		   int dw, 
		   int dh )
{
  Wm    *w = theme->wm;
  int    decor_idx = 0;
  Pixmap pxm_backing  = None; 

  pxm_backing = XCreatePixmap(theme->wm->dpy, theme->wm->root, dw, dh, 
			      DefaultDepth(theme->wm->dpy, theme->wm->screen));

  switch(frame_ref)
    {
    case FRAME_MAIN_SOUTH:
    case FRAME_DIALOG_SOUTH:
    case FRAME_MSG_SOUTH:
      decor_idx = SOUTH;
      break;
    case FRAME_MAIN_EAST: 
    case FRAME_DIALOG_EAST:
    case FRAME_MSG_EAST:
      decor_idx = EAST;
      break;
    case FRAME_MAIN_WEST: 
    case FRAME_DIALOG_WEST:
    case FRAME_MSG_WEST:
      decor_idx = WEST;
      break;
      /* FRAME_MAIN, FRAME_DIALOG, FRAME_MSG, FRAME_DIALOG_NORTH: */
    default:
      decor_idx = NORTH;
      break;
    }

  if (frame_ref == FRAME_DIALOG_EAST 
      || frame_ref == FRAME_DIALOG_WEST
      || frame_ref == FRAME_DIALOG_SOUTH
      || frame_ref == FRAME_DIALOG_NORTH
      || frame_ref == FRAME_MAIN_EAST 
      || frame_ref == FRAME_MAIN_WEST
      || frame_ref == FRAME_MAIN_SOUTH)
    {
      XSetForeground(theme->wm->dpy, theme->gc, 
		     BlackPixel(theme->wm->dpy, theme->wm->screen));
      XFillRectangle(theme->wm->dpy, pxm_backing, theme->gc, 0, 0, dw, dh);

      goto SetBackground;
    }
 
  _theme_paint_pixmap_border( theme, pxm_backing, dw, dh);

  if (c->name && ( frame_ref == FRAME_MAIN 
		   || frame_ref == FRAME_DIALOG 
		   || frame_ref == FRAME_UTILITY_MIN ) )
    {
      int xoffset = 0;
      int tmp_w = 0, space_avail;
#ifdef USE_XFT
      XftFont *font;
      XftDraw *xftdraw;
#else
      XFontStruct* font;
#endif
      int title_bytes = (c->name_is_utf8) 
	? ewmh_utf8_len(c->name) : strlen(c->name);


      if (frame_ref == FRAME_MAIN) 
	xoffset = FRAME_MAIN_HEIGHT;
      else 
	xoffset = 4;

      if (frame_ref == FRAME_UTILITY_MIN)
	font = theme->font_toolbar;
      else
	font = theme->font;
  
      space_avail 
	= c->width - ( (frame_ref == FRAME_MAIN ? 2 : 1 ) * FRAME_MAIN_HEIGHT);

      while ((tmp_w = font_get_str_width (theme->wm, 
					  font, 
					  c->name, 
					  title_bytes, 
					  c->name_is_utf8)) > space_avail
	     && title_bytes > 0)
	title_bytes--;

#ifdef USE_XFT

      xftdraw = XftDrawCreate(w->dpy, (Drawable) pxm_backing,
			      DefaultVisual(w->dpy, w->screen),
			      DefaultColormap(w->dpy, w->screen));

      if (xftdraw != NULL)
	{
	  font_paint (theme->wm,  
		      xftdraw,
		      &theme->xftcol, 
		      font,
		      c->name,
		      title_bytes,
		      c->name_is_utf8,
		      xoffset, 
		      font->ascent + 3);
	  
	  XftDrawDestroy(xftdraw);
	}
#else
      font_paint (theme->wm,
		  pxm_backing, 
		  &theme->col_text,
		  font,
		  theme->gc,
		  c->name, 
		  title_bytes,
		  xoffset,
		  font->ascent + 2);
#endif		  
    }

 SetBackground:

  XSetWindowBackgroundPixmap(w->dpy, c->frames_decor[decor_idx], 
			     pxm_backing);
  XClearWindow(w->dpy, c->frames_decor[decor_idx]);
  XSync(w->dpy, False);

  XFreePixmap(w->dpy, pxm_backing);

  return True;
}

static void
_theme_frame_menu_paint_text_entry(MBTheme* theme, 
				   Client *c, Client *entry, 
				   MBMiniDrawable *drw,
				   int y)
{
  MBClientButton* button = NULL;
  int offset = ( 16 + theme->font->ascent ) / 2;
  int str_len = 0;
  int item_h = theme->font->ascent + theme->font->descent + MENU_ENTRY_PADDING;

  if (entry->name_is_utf8)
    str_len = ewmh_utf8_len(entry->name);
  else
    str_len = strlen(entry->name);

  while (font_get_str_width (theme->wm, 
			     theme->font, 
			     entry->name, 
			     str_len, 
			     entry->name_is_utf8) > c->width 
	 && str_len > 0)
	       str_len--;

#ifdef USE_XFT
      font_paint (theme->wm,  
		  drw->xftdraw,
		  &theme->xftcol, 
		  theme->font,
		  entry->name,
		  str_len,
		  entry->name_is_utf8,
		  MENU_ENTRY_PADDING, 
		  y + offset);
#else
      font_paint (theme->wm,
		  drw->pxm, 
		  &theme->col_text,
		  theme->font,
		  theme->gc,
		  entry->name, 
		  str_len,
		  MENU_ENTRY_PADDING,
		  y + offset);
#endif		  

      /* We also create a button here */
    button = client_button_new(c, c->frame, 0, y, 
			       c->width, item_h,
			       True, (void* )entry );
	
    list_add(&c->buttons, NULL, 0, (void *)button);
}


void 
theme_frame_menu_paint(MBTheme* theme, Client *c)
{
  Wm     *w = c->wm;
  Client *p;
  MBList *item;
  int item_h = 0, item_current_y = 0;
  MBMiniDrawable drw;

  item_h = theme->font->ascent + theme->font->descent + MENU_ENTRY_PADDING;

   /* Now render fonts */
   item_current_y = 0;

  drw.pxm = XCreatePixmap(w->dpy, w->root, c->width, c->height, 
			  DefaultDepth(w->dpy, w->screen));

#ifdef USE_XFT
  drw.xftdraw = XftDrawCreate(w->dpy, (Drawable) drw.pxm,
			      DefaultVisual(w->dpy, w->screen),
			      DefaultColormap(w->dpy, w->screen));

#endif

  _theme_paint_pixmap_border( theme, drw.pxm, c->width, c->height);

  list_enumerate(w->client_age_list, item)
    {
      p = (Client*)item->data;

      if (p->type == MBCLIENT_TYPE_APP && p->name
	  && client_get_state(p) == NormalState )
      {
	_theme_frame_menu_paint_text_entry(theme, c, p, &drw,
					   item_current_y);
	item_current_y += item_h;
      }
    }

  list_enumerate(w->client_age_list, item)
    {
      p = (Client*)item->data;

      if (p->type == MBCLIENT_TYPE_APP && p->name
	  && client_get_state(p) == IconicState )
      {
	_theme_frame_menu_paint_text_entry(theme, c, p, &drw,
					   item_current_y);
	item_current_y += item_h;
      }
    }

   if ((p = wm_get_desktop(c->wm)) != NULL) 
     {
       _theme_frame_menu_paint_text_entry(theme, c, p, &drw,
					  item_current_y);
     }

#ifdef USE_XFT
   XftDrawDestroy(drw.xftdraw);   
#endif

   XSetWindowBackgroundPixmap(w->dpy, c->frame, drw.pxm);

   XClearWindow(w->dpy, c->frame);
   XSync(w->dpy, False);

   XFreePixmap(w->dpy, drw.pxm);
   return;
}

Bool 
theme_frame_menu_get_dimentions(MBTheme* theme, int* w, int *h)
{
  Client *p = NULL;
  int width = 0, height = 0, tmp_w = 0, str_len = 0, space_avail = 0;

  space_avail = theme->wm->dpy_width - 16;

  stack_enumerate(theme->wm, p) 
  {
    if ((p->type == MBCLIENT_TYPE_APP || p->type == MBCLIENT_TYPE_DESKTOP) 
	&& p->name)
      {
	unsigned char *name = p->name;
	 
	if (p->name_is_utf8)
	   str_len = ewmh_utf8_len(p->name);
	 else
	   str_len = strlen(p->name);

	while ((tmp_w = font_get_str_width (theme->wm, 
					    theme->font, 
					    name, 
					    str_len, 
					    p->name_is_utf8)) > space_avail
	       && str_len > 0)
	  str_len--;
	
	tmp_w += MENU_ENTRY_PADDING;
	height += theme->font->ascent + theme->font->descent + MENU_ENTRY_PADDING;
	if (tmp_w > width) width = tmp_w;
      }
   }
    
  width += MENU_ENTRY_PADDING;
    
  *w = width;
  *h = height;
  
  return True;
}

MBTheme* 
mbtheme_new(Wm *w)
{
  XGCValues gv;
#ifdef USE_XFT   
  XRenderColor colortmp;
#endif

  MBTheme *t = (MBTheme *)malloc(sizeof(MBTheme));
  memset(t, 0, sizeof(MBTheme));

  t->wm = w;

#ifndef STANDALONE
  t->have_toolbar_panel = False;
#endif  

  gv.graphics_exposures = False;
  gv.function   = GXcopy;
  t->gc = XCreateGC(w->dpy, w->root,
		       GCGraphicsExposures|GCFunction, &gv);
  

  gv.function = GXinvert;
  gv.subwindow_mode = IncludeInferiors;
  gv.line_width = 1;
  t->band_gc = XCreateGC(w->dpy, w->root,
			 GCFunction|GCSubwindowMode|GCLineWidth, &gv);
  
  t->mask_gc = None;

  /* XXX Condense The Below XXX */

  if (!XParseColor(w->dpy, DefaultColormap(w->dpy, w->screen),
		   THEME_FG_COLOR, &t->col_fg))
    {
      fprintf(stderr, "matchbox: failed to parse color %s\n", THEME_FG_COLOR);
      exit(1);
    } else {
      XAllocColor(w->dpy, DefaultColormap(w->dpy, w->screen), &t->col_fg);
    }

  if (!XParseColor(w->dpy, DefaultColormap(w->dpy, w->screen),
		   THEME_FG_HIGHLIGHT_COLOR, &t->col_fg_highlight))
    {
      fprintf(stderr, "matchbox: failed to parse color %s\n", 
	      THEME_FG_HIGHLIGHT_COLOR);
      exit(1);
    } else {
      XAllocColor(w->dpy, DefaultColormap(w->dpy, w->screen), 
		  &t->col_fg_highlight);
    }

  if (!XParseColor(w->dpy, DefaultColormap(w->dpy, w->screen),
		   THEME_FG_LOWLIGHT_COLOR, &t->col_fg_lowlight))
    {
      fprintf(stderr, "matchbox: failed to parse color %s\n", 
	      THEME_FG_LOWLIGHT_COLOR);
      exit(1);
    } else {
      XAllocColor(w->dpy, DefaultColormap(w->dpy, w->screen), 
		  &t->col_fg_lowlight);
    }

  if (!XParseColor(w->dpy, DefaultColormap(w->dpy, w->screen),
		   THEME_TEXT_COLOR, &t->col_text))
    {
      fprintf(stderr, "matchbox: failed to parse color %s\n", 
	      THEME_TEXT_COLOR);
      exit(1);
    } else {
      XAllocColor(w->dpy, DefaultColormap(w->dpy, w->screen), &t->col_text);
    }

#ifdef USE_XFT      
   colortmp.red   = t->col_text.red;
   colortmp.green = t->col_text.green;
   colortmp.blue  = t->col_text.blue;
   colortmp.alpha = 0xffff; 
   XftColorAllocValue(w->dpy,
		       DefaultVisual(w->dpy, w->screen), 
		       DefaultColormap(w->dpy, w->screen),
		       &colortmp,
		       &t->xftcol);
#endif

  if (!font_load (w, THEME_FONT_MAIN, &t->font))
    {
      fprintf(stderr, "matchbox: failed to load font %s\n", 
	      THEME_FONT_TOOLBAR);
      exit(1);
    };

  if (!font_load (w, THEME_FONT_TOOLBAR, &t->font_toolbar))
    {
      fprintf(stderr, "matchbox: failed to load font %s\n", 
	      THEME_FONT_TOOLBAR);
      exit(1);
    };

  return t;
}

void
theme_frame_menu_highlight_entry(Client *c, 
				 MBClientButton *button, 
				 int mode)
{
  MBTheme *t = c->wm->mbtheme;

  XDrawLine(t->wm->dpy, c->frame, t->band_gc,
	    button->x + 3, button->y, button->x + button->w - 4, button->y);
  XDrawLine(t->wm->dpy, c->frame, t->band_gc,
	    button->x + 3, button->y + button->h, 
	    button->x + button->w - 4, button->y + button->h);
  XDrawLine(t->wm->dpy, c->frame, t->band_gc,
	    button->x + 2, button->y + 1, 
	    button->x + 2, button->y + button->h);
  XDrawLine(t->wm->dpy, c->frame, t->band_gc,
	    button->x + button->w - 4, button->y + 1, 
	    button->x + button->w - 4, button->y + button->h);
}

void 
mbtheme_switch(Wm *w, char *new_theme_conf)
{
  return;
}

void 
mbtheme_init(Wm *w, char *theme_conf)
{
  w->mbtheme = mbtheme_new(w);
}

int 
theme_frame_button_get_x_pos(MBTheme *theme, 
			     int frame_type, 
			     int button_type,
			     int width)
{
  return 0;
}

Bool
mbtheme_has_titlebar_panel(MBTheme *theme)
{
  return theme->wm->have_toolbar_panel;
}

Bool
mbtheme_get_titlebar_panel_rect(MBTheme    *theme, 
				XRectangle *rect,
				Client     *ignore_client)
{
  if (!theme->wm->have_toolbar_panel) return False;

  rect->x      = theme->wm->toolbar_panel_x;
  rect->y      = theme->wm->toolbar_panel_y;
  rect->width  = theme->wm->toolbar_panel_w;
  rect->height = theme->wm->toolbar_panel_h;

  return True;
}

Bool
theme_has_message_decor( MBTheme *theme )
{
  return False;
}

Bool
theme_has_borders_only_decor( MBTheme *theme )
{
  return False;
}

void
theme_pixmap_cache_clear_all( MBTheme *theme )
{
  return; /* No caching here */
}

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

#include "mbtheme.h" 

#ifdef HAVE_XCURSOR
#include <X11/Xcursor/Xcursor.h>
#endif

#define GET_INT_ATTR(n,k,v) \
    { if (get_attr((n), (k))) (v) = atoi(get_attr((n), (k))); else (v) = 0; }

static struct layer_lookup_t
{
  char* name;
  int   id;
} 
layer_lookup[] = {
  { "plain",  LAYER_PLAIN },
  { "gradient-horiz", LAYER_GRADIENT_HORIZ },
  { "gradient-vert",  LAYER_GRADIENT_VERT  },
  { "pixmap",         LAYER_PIXMAP        },
  { "pixmap-tiled",   LAYER_PIXMAP_TILED   },
  { "picture",        LAYER_PICTURE       },
  { "picture-tiled",  LAYER_PICTURE_TILED  },
  { "label",          LAYER_LABEL         },
  { "sublabel",       LAYER_SUB_LABEL     },
  { "icon",           LAYER_ICON          }
};

#define layer_lookup_cnt (sizeof(layer_lookup)/sizeof(struct layer_lookup_t))

static struct button_lookup_t
{
  char* name;
  int   id;
} 
button_lookup[] = {
  { "next",     BUTTON_ACTION_NEXT   },
  { "prev",     BUTTON_ACTION_PREV   },
  { "close",    BUTTON_ACTION_CLOSE  },
  { "minimize", BUTTON_ACTION_MIN    },
  { "maximize", BUTTON_ACTION_MAX    },
  { "menu",     BUTTON_ACTION_MENU   },
  { "hide",     BUTTON_ACTION_HIDE   },
  { "accept",   BUTTON_ACTION_ACCEPT },
  { "help",     BUTTON_ACTION_HELP   },
  { "desktop",  BUTTON_ACTION_DESKTOP },
  { "custom",   BUTTON_ACTION_CUSTOM  },
};

#define button_lookup_cnt (sizeof(button_lookup)/sizeof(struct button_lookup_t))

static struct frame_lookup_t
{
  char* name;
  int   id;
} frame_lookup[] = {
  { "main",          FRAME_MAIN         },
  { "main-east",     FRAME_MAIN_EAST    },
  { "main-west",     FRAME_MAIN_WEST    },
  { "main-south",    FRAME_MAIN_SOUTH   },
  { "dialog",        FRAME_DIALOG       },
  { "dialog-north",  FRAME_DIALOG_NORTH },
  { "dialog-east",   FRAME_DIALOG_EAST  },
  { "dialog-west",   FRAME_DIALOG_WEST  },
  { "dialog-south",  FRAME_DIALOG_SOUTH },
  { "dialog-nt-north",  FRAME_DIALOG_NT_NORTH },
  { "dialog-nt-east",   FRAME_DIALOG_NT_EAST  },
  { "dialog-nt-west",   FRAME_DIALOG_NT_WEST  },
  { "dialog-nt-south",  FRAME_DIALOG_NT_SOUTH },
  { "utility-min",   FRAME_UTILITY_MIN  },
  { "utility-max",   FRAME_UTILITY_MAX  },
  { "menu",          FRAME_MENU         },
  { "message",       FRAME_MSG         },
  { "message-east",  FRAME_MSG_EAST    },
  { "message-west",  FRAME_MSG_WEST    },
  { "message-south", FRAME_MSG_SOUTH   },
};

#define frame_lookup_cnt (sizeof(frame_lookup)/sizeof(struct frame_lookup_t))

static int
parse_panel_tag (MBTheme *theme, 
		 XMLNode *node);


static int param_get( MBThemeFrame *frame, MBThemeParam *p, int max );

static void show_parse_error(Wm *w, XMLNode *node, 
			     char *theme_file, int err_num);


/* ---------------------------------------------------- Painting Code -- */

static int 
lookup_frame_type(char *name)
{
  int i;
  for (i=0; i < frame_lookup_cnt; i++)
    if (!strcmp(name, frame_lookup[i].name))
      return frame_lookup[i].id;
  return -1;
}
 
void theme_frame_icon_paint(MBTheme *t, Client *c, 
			    MBPixbufImage *img_dest, 
			    int x, int y)
{
  MBPixbufImage *img = NULL;
  int           *data = NULL;

  if (c->icon_rgba_data != NULL)
    {
      data = c->icon_rgba_data;

      img = mb_pixbuf_img_new_from_int_data(t->wm->pb,
					    (data+2),
					    data[0], data[1]);
    }
  else 
    {
      if (c->icon)
	{
	  Window       win_foo;
	  int          foo;
	  unsigned int icon_w, icon_h, ufoo;
	  XGetGeometry(t->wm->dpy, c->icon, &win_foo, &foo, &foo, 
		       &icon_w, &icon_h, &ufoo, &ufoo);
	  img = mb_pixbuf_img_new_from_drawable(t->wm->pb, 
						c->icon, 
						c->icon_mask, 
						0, 0, 
						(int)icon_w, (int)icon_h);
	} 
    }

  if( img == NULL)
    img = mb_pixbuf_img_clone(t->wm->pb, c->wm->img_generic_icon);

  if (img->width != 16 || img->height != 16) 
    {
      MBPixbufImage *tmp_img;
      tmp_img = mb_pixbuf_img_scale(t->wm->pb, img, 16, 16); 
      mb_pixbuf_img_free(t->wm->pb, img);
      img = tmp_img;
    }

  mb_pixbuf_img_composite(t->wm->pb, img_dest, img, x, y);

  if (img != NULL) mb_pixbuf_img_free(t->wm->pb, img);
}


void
theme_frame_button_paint(MBTheme *theme, 
			 Client  *c, 
			 int      action,
			 int      state, 
			 int      frame_type, 
			 int      dest_w, 
			 int      dest_h )
{
  Wm           *w = c->wm;
  MBThemeFrame *frame = NULL;
  MBList       *client_button_obj = c->buttons; /* Client 'WM' buttons */
  MBList       *theme_button_list  = NULL;      /* Theme button defs   */
  int           button_x, button_y, button_w, button_h;

  frame = (MBThemeFrame *)list_find_by_id(theme->frames, frame_type);

  dbg("%s called\n", __func__);

  if (frame == NULL) { dbg("%s failed to find frame\n", __func__); return; }

  theme_button_list = frame->buttons;

  while (theme_button_list != NULL)
    {
      /* Go through defined theme frames button list, find matching action */

      if (theme_button_list->id == action)
	{
	  MBThemeButton *button = (MBThemeButton *)theme_button_list->data;
	  Window         button_xid = None;
	  Bool           found = False;

	  /* Figure out actual wm button geometry from the defined theme
           * button.
	  */

	  /* HACK. param_get() has no client* ref so need to  
           * do stuff here to get position relevant to 
           * label end point :(. 
	   */
	  if (button->x->unit == MBParamLabelEnd)
	    {
	      button_x = c->name_rendered_end_pos + button->x->offset;
	    }	  
	  else button_x = param_get(frame, button->x, dest_w);

	  button_y = param_get(frame, button->y, dest_h);

	  if (button->w->unit == MBParamTotalLabelWidth)
	    {
	      button_w = c->name_total_width + button->w->offset;
	    }
	  else button_w = param_get(frame, button->w, dest_w);

	  button_h = param_get(frame, button->h, dest_h);

	  dbg("%s() button x is %i, destw %i\n", __func__, button_x, dest_w);

	  /* See if the clien already has a wm button object created */

	  while (client_button_obj != NULL)
	    {
	      if (client_button_obj->id == action) 
		{
		  found = True;
		  button_xid = ((MBClientButton*)client_button_obj->data)->win;
		  dbg("%s found button action %i, win id is %li\n", 
		      __func__, action, button_xid);
		  break;
		}
	      client_button_obj = client_button_obj->next;
	    }

	  /* Did we find a valid window ID for the button */

	  if (button_xid == None)
	    {
	      MBClientButton *b = NULL;

	      if (!found)
		{
		  /*  We didn't so create a new client button 'on demand'. */

		  b = client_button_new(c, client_title_frame(c),
					button_x, button_y,
					button_w, button_h,
					button->inputonly,
					NULL);

		  if (button->press_activates)
		    b->press_activates = True;

		  list_add(&c->buttons, NULL, action, (void *)b);
		}
	      else
		{
		  /* We found a button with no win ID ( win probably removed  
		   * so it can be updated ). So we simply re-init it.   
		  */

		  b = (MBClientButton *)client_button_obj->data;
		  client_button_init(c, client_title_frame(c), b,
				     button_x, button_y,
				     button_w, button_h,
				     button->inputonly,
				     b->data);
		}

	      button_xid = b->win;
	    }
    
	  if (!button->inputonly)
	    {
	      /* Now paint the actual button if required */
	      int            copy_w, copy_h;
	      MBPixbufImage *img_backing = NULL;
	      Pixmap         pxm_button;
	      MBPixbuf      *pb = w->pb;

#ifdef USE_COMPOSITE
	      /* 32 ARGB wins require 'special' pixbuf ref */
	      if (c->is_argb32)
		pb = w->argb_pb;
#endif
	      pxm_button = XCreatePixmap(w->dpy, w->root, button_w, button_h, 
					 pb->depth);

	      /* Grab any background from caches so can composite to it */

	      if (c->type == MBCLIENT_TYPE_APP 
		  || c->type == MBCLIENT_TYPE_TOOLBAR 
		  || c->type == MBCLIENT_TYPE_DIALOG)
		{
		  img_backing = mb_pixbuf_img_rgb_new(pb, button_w, button_h);

		  if (!theme->disable_pixbuf_cache)
		    mb_pixbuf_img_copy(pb, img_backing,
				       theme->img_caches[frame_type],
				       button_x, button_y,
				       button_w, button_h,
				       0, 0 );

		}

	      /* Now actually paint depending on button state */

	      if (state == ACTIVE)
		{
		  if (button->img_active->width > button_w)
		    copy_w = button_w;
		  else
		    copy_w = button->img_active->width;

		  if (button->img_active->height > button_h)
		    copy_h = button_h;
		  else
		    copy_h = button->img_active->height;

		  mb_pixbuf_img_copy_composite_with_alpha(pb, img_backing,
							  button->img_active, 
							  0, 0, copy_w, copy_h,
							  0, 0, 
							  button->img_active_blend);

		} 
	      else 
		{
		  if (button->img_inactive->width > button_w)
		    copy_w = button_w;
		  else
		    copy_w = button->img_inactive->width;
		  
		  if (button->img_inactive->height > button_h)
		    copy_h = button_h;
		  else
		    copy_h = button->img_inactive->height;

		  mb_pixbuf_img_copy_composite_with_alpha(pb, img_backing,
							  button->img_inactive, 
							  0, 0, copy_w, copy_h,
							  0, 0,
							  button->img_inactive_blend);
		}

	      mb_pixbuf_img_render_to_drawable(pb, img_backing, pxm_button, 
					       0, 0);
	      
	      XSetWindowBackgroundPixmap(w->dpy, button_xid, pxm_button);
	      XClearWindow(w->dpy, button_xid);   

	      XFreePixmap(w->dpy, pxm_button);
	      mb_pixbuf_img_free(pb, img_backing);
	      
	    }

	  XMapWindow(w->dpy, button_xid);
	}
      theme_button_list = theme_button_list->next;
    }
}

Bool 
theme_frame_wants_shaped_window( MBTheme *theme, int frame_type)
{
  MBThemeFrame *frame = NULL;

  if ((frame = (MBThemeFrame *)list_find_by_id(theme->frames, frame_type)) == NULL)
    return False;

  return frame->wants_shape;
}

Bool
theme_has_message_decor( MBTheme *theme )
{
  MBThemeFrame *frame = (MBThemeFrame *)list_find_by_id(theme->frames, 
							FRAME_MSG);
  if (frame == NULL) 
    return False;
  else
    return True;
}

Bool
theme_has_borders_only_decor( MBTheme *theme )
{
  MBThemeFrame *frame = (MBThemeFrame *)list_find_by_id(theme->frames, 
							FRAME_DIALOG_NT_NORTH);
  if (frame == NULL) 
    return False;
  else
    return True;
}

static void
_theme_paint_gradient(MBTheme*       theme, 
		      MBThemeLayer*  layer_cur, 
		      MBPixbufImage* img_dest, 
		      int            w, 
		      int            h, 
		      int            direction)
{
  int tx, ty, r, rs, re, b, bs, be, g, gs, ge, a, as, ae;

  rs = mb_col_red(layer_cur->color);
  re = mb_col_red(layer_cur->color_end);
  gs = mb_col_green(layer_cur->color);
  ge = mb_col_green(layer_cur->color_end);
  bs = mb_col_blue(layer_cur->color);
  be = mb_col_blue(layer_cur->color_end);
  as = mb_col_alpha(layer_cur->color);
  ae = mb_col_alpha(layer_cur->color_end);

  if (rs == re && gs == ge && bs == be && as == ae)
    {
      /* If start and end points are the same, fastpath it */

      mb_pixbuf_img_fill(theme->wm->pb, img_dest, rs, gs, bs, as);
      return;
    }

  if (direction == VERTICAL)
    {
      for(ty=0; ty<h; ty++)
	{
	  r = rs + (( ty * (re - rs) ) / h); 
	  g = gs + (( ty * (ge - gs) ) / h); 
	  b = gs + (( ty * (be - bs) ) / h); 
	  a = as + (( ty * (ae - as) ) / h); 
	  
	  for(tx=0; tx<w; tx++)
	    {
	      mb_pixbuf_img_plot_pixel(theme->wm->pb, img_dest, tx, ty, 
				       r, g, b);
	      mb_pixbuf_img_set_pixel_alpha(img_dest, tx, ty, a);
	    }
	}
    } else {
      for(tx=0; tx<w; tx++)
	{
	  r = rs + (( tx * (re - rs) ) / w); 
	  g = gs + (( tx * (ge - gs) ) / w); 
	  b = gs + (( tx * (be - bs) ) / w); 
	  a = as + (( tx * (ae - as) ) / w); 
	  
	  for(ty=0; ty<h; ty++)
	    {
	      mb_pixbuf_img_plot_pixel(theme->wm->pb, img_dest, tx, ty, 
				       r, g, b);
	      mb_pixbuf_img_set_pixel_alpha(img_dest, tx, ty, a);
	    }
	}
    }
}

static void
_theme_paint_core( MBTheme       *theme, 
		   Client        *c, 
		   MBThemeFrame  *frame,
		   MBPixbufImage *img, 
		   int            dx, 
		   int            dy, 
		   int            dw, 
		   int            dh )
{
  /* Basically render a frame layers to 'img' */

  MBThemeLayer *layer_cur = NULL;
  MBList       *layer_list_item = frame->layers; 

  while (layer_list_item != NULL)
    {
      MBPixbufImage *img_tmp = NULL;
      int            tx, ty, tw, th, x, y, w, h;

      layer_cur = (MBThemeLayer *)layer_list_item->data;

      /* Get Layer Rect */

      x = param_get(frame, layer_cur->x, dw);
      y = param_get(frame, layer_cur->y, dh);
      w = param_get(frame, layer_cur->w, dw);
      h = param_get(frame, layer_cur->h, dh);

      /* Minor hack to handle 'object' size attribute */

      if ( layer_list_item->id == LAYER_PIXMAP 
	   || layer_list_item->id == LAYER_PIXMAP_TILED)
	{
	  if ( layer_cur->w->unit == object) w = layer_cur->img->width;
	  if ( layer_cur->h->unit == object) h = layer_cur->img->height;
	}

      /* Clip if calculated sizes are bigger than dest */

      if (w > dw) w = dw;
      if (h > dh) h = dh;
	
      /* ..And more safety */

      if (w <= 0) w = 1;
      if (h <= 0) h = 1;
	
      switch (layer_list_item->id)
	{

	case LAYER_PLAIN:
	  img_tmp = mb_pixbuf_img_new(theme->wm->pb, w, h);
	  mb_pixbuf_img_fill(theme->wm->pb, img_tmp, 
			     mb_col_red(layer_cur->color),
			     mb_col_green(layer_cur->color),
			     mb_col_blue(layer_cur->color),
			     mb_col_alpha(layer_cur->color) );
	    break;

	case LAYER_GRADIENT_HORIZ:
	  img_tmp = mb_pixbuf_img_new(theme->wm->pb, w, h);
	  _theme_paint_gradient(theme, layer_cur, img_tmp, w, h, HORIZONTAL);
	  break;

	case LAYER_GRADIENT_VERT:
	  img_tmp = mb_pixbuf_img_new(theme->wm->pb, w, h);
	  _theme_paint_gradient(theme, layer_cur, img_tmp, w, h, VERTICAL);
	  break;

	case LAYER_PIXMAP:
	  img_tmp = mb_pixbuf_img_scale(theme->wm->pb, layer_cur->img, w, h);
	  dbg("%s() Layer is pixmap\n", __func__);
	  break;

	case LAYER_PIXMAP_TILED:
	  dbg("%s() Layer is pixmap tiled %i x %i\n", __func__, w, h);
	  img_tmp = mb_pixbuf_img_new(theme->wm->pb, w, h);
	  
	  for (ty=0; ty < h; ty += layer_cur->img->height)
	    for (tx=0; tx < w; tx += layer_cur->img->width)
	      {
		if ( (tx + layer_cur->img->width) > w )
		  tw = layer_cur->img->width - ((tx+layer_cur->img->width)-w);
		else
		  tw = layer_cur->img->width;
		
		if ( (ty + layer_cur->img->height) > h )
		  th = layer_cur->img->height-((ty+layer_cur->img->height)-h);
		else
		  th = layer_cur->img->height;
		
		dbg("%s() Tiling %i x %i, +%i+%i\n", __func__, tw, th, tx, ty);
		mb_pixbuf_img_copy_composite(theme->wm->pb, img, 
					     layer_cur->img,
					     0, 0, tw, th, tx + x, ty + y);
	      }
	  break;

	case LAYER_ICON:
	  /* We cant cache the icon, so we dont paint it here  */
	  break;
	}
      
      if (img_tmp)
	{
	  /* Clip image if needed ( more safety ) -*/

	  if (w > img->width)  w = img->width; 
	  if (h > img->height) h = img->height; 

	  mb_pixbuf_img_copy_composite(theme->wm->pb, img, img_tmp,
				       0, 0, w, h, x, y); 
	  mb_pixbuf_img_free(theme->wm->pb, img_tmp);
	}
      
      layer_list_item = layer_list_item->next;
    }
}

Bool
theme_frame_paint( MBTheme *theme, 
		   Client  *c, 
		   int      frame_type, 
		   int      dw, 
		   int      dh )
{
  Wm *w = c->wm;

  MBFontRenderOpts  text_render_opts = MB_FONT_RENDER_OPTS_CLIP_TRAIL;
  Bool              have_img_cached = False, free_img = False;
  MBThemeFrame     *frame;
  MBPixbufImage    *img;
  MBThemeLayer     *layer_label = NULL, *layer_icon = NULL;
  struct list_item *layer_list_item;
  int               label_rendered_width;
  int               decor_idx = 0;
  MBDrawable       *drawable = NULL;
  MBPixbuf         *pixbuf = w->pb;

  if (dw == 0 || dh == 0)
    return False;

  frame = (MBThemeFrame *)list_find_by_id(theme->frames, frame_type);

  if (frame == NULL) return False;

#ifdef USE_COMPOSITE
   if (c->is_argb32)
     pixbuf = w->argb_pb;
#endif

   /* Figure out what frame we are painting to */

  switch(frame_type)
    {
    case FRAME_MAIN_SOUTH:
    case FRAME_DIALOG_SOUTH:
    case FRAME_DIALOG_NT_SOUTH:
    case FRAME_MSG_SOUTH:
      decor_idx = SOUTH;
      break;
    case FRAME_MAIN_EAST: 
    case FRAME_DIALOG_EAST:
    case FRAME_DIALOG_NT_EAST:
    case FRAME_MSG_EAST:
      decor_idx = EAST;
      break;
    case FRAME_MAIN_WEST: 
    case FRAME_DIALOG_WEST:
    case FRAME_DIALOG_NT_WEST:
    case FRAME_MSG_WEST:
      decor_idx = WEST;
      break;
      /* FRAME_MAIN, FRAME_DIALOG, FRAME_MSG, FRAME_DIALOG_NORTH: */
    default:
      decor_idx = NORTH;
      break;
    }

  /* 
   *  The pixmaps for side decorations ( east, west, south ) rarely
   *  change for app wins and dont have text. We can therefore cache
   *  the generated pixmaps to save both mem and cpu
   */
  if (frame_type == FRAME_MAIN_SOUTH 
      || frame_type == FRAME_MAIN_EAST
      || frame_type == FRAME_MAIN_WEST)
    {
      if (theme->app_win_pxm_cache[decor_idx] != None)
	{
	  dbg("%s() getting pixmap frame from cache\n", __func__);

	  XSetWindowBackgroundPixmap(w->dpy, c->frames_decor[decor_idx], 
				     theme->app_win_pxm_cache[decor_idx]);
	  XClearWindow(w->dpy, c->frames_decor[decor_idx]);
	  XSync(w->dpy, False);
	  return True;
	}
    }

   drawable = mb_drawable_new(pixbuf, dw, dh);

   /* Cacheing */

  if (frame_type == FRAME_MAIN && theme->img_caches[frame_type] != NULL)
    {
      /* We only reuse app titlebar images. 
       */
      img = theme->img_caches[frame_type];
      have_img_cached = True;
    }
  else
    {
      /* Other window decors are just kept around whilst the client exists 
       * so things like buttons can composite onto them.  
      */
      theme_img_cache_clear (theme, frame_type);

      if (c->backing_masks[MSK_NORTH] != None  /* Need alpha chan for shape */
	  || c->backing_masks[MSK_SOUTH]!= None
	  || c->backing_masks[MSK_EAST] != None
	  || c->backing_masks[MSK_WEST] != None )
	theme->img_caches[frame_type] = mb_pixbuf_img_rgba_new(theme->wm->pb,
							       dw,dh);
      else
	theme->img_caches[frame_type] = mb_pixbuf_img_rgb_new(theme->wm->pb,
							      dw,dh);
      img = theme->img_caches[frame_type];
    }

  layer_list_item = frame->layers;

  layer_label = (MBThemeLayer*)list_find_by_id(frame->layers, LAYER_LABEL);

  /* Figure out text alignment + positioning */

  if (layer_label && c->name)
    {
      if (layer_label->label->justify == ALIGN_CENTER)
	text_render_opts |= MB_FONT_RENDER_ALIGN_CENTER;
      else if (layer_label->label->justify == ALIGN_RIGHT)
	text_render_opts |= MB_FONT_RENDER_ALIGN_RIGHT;

      frame->label_x = param_get(frame, layer_label->x, dw);
      frame->label_w = param_get(frame, layer_label->w, dw);

      label_rendered_width = mb_font_render_simple_get_width (layer_label->label->font, 
							      frame->label_w,
							      (unsigned char*) c->name,
							      (c->name_is_utf8) ? MB_ENCODING_UTF8 : MB_ENCODING_LATIN,
							      text_render_opts );
      
      dbg("%s() label_rendered_width = %i, frame clip width = %i\n",
	  __func__, label_rendered_width, frame->label_w);

      /* Has text changed or first render */
      
      if (c->name_rendered_end_pos != (frame->label_x + label_rendered_width))
	{
	  c->name_rendered_end_pos = (frame->label_x + label_rendered_width);
	}

      c->name_total_width = label_rendered_width;
    }

  /* Paint the acout pixbuf image, if not cached */
  if (!have_img_cached)
    _theme_paint_core( theme, c, frame, img, 0, 0, dw, dh );
  
  /* Icons - are a pain as we cant cache them */
  
  if ((layer_icon = (MBThemeLayer*)list_find_by_id(frame->layers, 
						   LAYER_ICON)) != NULL)
    {
      MBPixbufImage *img_tmp = NULL;
      dbg("%s() painting icon\n", __func__);
      
      if (have_img_cached)
	{
	  img_tmp = mb_pixbuf_img_clone(theme->wm->pb, 
					theme->img_caches[frame_type]);
	  
	  theme_frame_icon_paint(theme, c, img_tmp, 
				 param_get(frame, layer_icon->x, dw), 
				 param_get(frame, layer_icon->y, dh));
	  img = img_tmp;
	}
      else
	{
	  img = mb_pixbuf_img_clone(theme->wm->pb, img);
	}
      free_img = True;
      theme_frame_icon_paint(theme, c, img, 
			     param_get(frame, layer_icon->x, dw), 
			     param_get(frame, layer_icon->y, dh));
    } 
  
  /* Finally paint to the pixmap. */
  
  mb_pixbuf_img_render_to_drawable(pixbuf, img, 
				   mb_drawable_pixmap(drawable), 
				   0, 0);
  
  /* and masks for shaping */
  
  if (c->backing_masks[MSK_NORTH] != None &&
      ( frame_type == FRAME_MAIN || frame_type == FRAME_DIALOG 
	|| frame_type == FRAME_MSG || frame_type == FRAME_DIALOG_NORTH
	|| frame_type == FRAME_DIALOG_NT_NORTH)
      )
    mb_pixbuf_img_render_to_mask(theme->wm->pb, img, 
				 c->backing_masks[MSK_NORTH],
				 0, 0);
  
  if (c->backing_masks[MSK_SOUTH] != None  &&
      ( frame_type == FRAME_MAIN_SOUTH || frame_type == FRAME_DIALOG_SOUTH 
	|| frame_type == FRAME_MSG_SOUTH 
	|| frame_type == FRAME_DIALOG_NT_SOUTH)
      )
    mb_pixbuf_img_render_to_mask(theme->wm->pb, img, 
				 c->backing_masks[MSK_SOUTH],
				 0, 0);
  
  if (c->backing_masks[MSK_EAST] != None  &&
      ( frame_type == FRAME_MAIN_EAST || frame_type == FRAME_DIALOG_EAST 
	|| frame_type == FRAME_MSG_EAST || frame_type == FRAME_DIALOG_NT_EAST)
      )
    mb_pixbuf_img_render_to_mask(theme->wm->pb, img, 
				 c->backing_masks[MSK_EAST],
				 0, 0);
  
  if (c->backing_masks[MSK_WEST] != None  &&
      ( frame_type == FRAME_MAIN_WEST || frame_type == FRAME_DIALOG_WEST 
	|| frame_type == FRAME_MSG_WEST || frame_type == FRAME_DIALOG_NT_WEST )
      )
    mb_pixbuf_img_render_to_mask(theme->wm->pb, img, 
				 c->backing_masks[MSK_WEST],
				 0, 0);
  
  /* If we've painted an icon we need to free up our temporary image */
  
  if (free_img)
    mb_pixbuf_img_free(theme->wm->pb, img);
  
  /* No point caching frame images which dont have buttons */
  
  if (decor_idx != NORTH || theme->disable_pixbuf_cache == True)
    theme_img_cache_clear (theme, frame_type);
  
  /* Now paint text onto pixmap */
  
  if (layer_label && c->name && !(c->flags & CLIENT_BORDERS_ONLY_FLAG))
    {
      int fy = param_get(frame, layer_label->y, dh); 
      
      dbg("%s() rendering '%s' text\n", __func__, c->name);
      
      mb_font_set_color (layer_label->label->font, layer_label->label->col);
      
      dbg("%s() painting text '%s' with r: %i, g: %i, b: %i, a: %i\n",
	  __func__, 
	  (unsigned char*) c->name,
	  mb_col_red(layer_label->label->col),
	  mb_col_green(layer_label->label->col),
	  mb_col_blue(layer_label->label->col),
	  mb_col_alpha(layer_label->label->col));
      
      
      mb_font_render_simple (layer_label->label->font, 
			     drawable, 
			     frame->label_x,
			     fy,
			     frame->label_w,
			     (unsigned char*) c->name,
			     (c->name_is_utf8) ? MB_ENCODING_UTF8 : MB_ENCODING_LATIN,
			     text_render_opts);
      dbg("%s() rendered text\n", __func__);

      /* FIXME: we really need an mb_font_unset_color() here..? */
      mb_col_unref(layer_label->label->col);
      layer_label->label->font->col = NULL;
    }

  /* Finally put the drawable on the decoration frame background */
  
  XSetWindowBackgroundPixmap(w->dpy, c->frames_decor[decor_idx], 
			     mb_drawable_pixmap(drawable));
  XClearWindow(w->dpy, c->frames_decor[decor_idx]);
  XSync(c->wm->dpy, False);

  /* Cache the pixmaps of these frame types.  
   * ( note we copy so xft part of drawable gets freed ok ).
  */
  if (frame_type == FRAME_MAIN_SOUTH 
      || frame_type == FRAME_MAIN_EAST
      || frame_type == FRAME_MAIN_WEST)
    {
      dbg("%s() cacheing pixmap frame\n", __func__);

      theme->app_win_pxm_cache[decor_idx] 
	= XCreatePixmap(w->dpy, mb_drawable_pixmap(drawable), 
			dw, dh, DefaultDepth(w->dpy, w->screen));
      XCopyArea(w->dpy, mb_drawable_pixmap(drawable), 
		theme->app_win_pxm_cache[decor_idx], theme->gc, 
		0, 0, dw, dh, 0, 0);
    }

  mb_drawable_unref(drawable);

  return True;
}

/**** Task list painting *******/

Bool
theme_frame_menu_get_dimentions(MBTheme* theme, int *w, int *h)
{
  Client       *p     = NULL;
  MBThemeFrame *frame = NULL;
  int           width = 0, height = 0, space_avail = 0;

  space_avail = theme->wm->dpy_width - theme->wm->config->use_icons - 16;

  frame =  (MBThemeFrame*)list_find_by_id(theme->frames, FRAME_MENU );

  if (frame == NULL)       return False; 
  if (frame->font == NULL) return False;

  stack_enumerate(theme->wm, p)
  {
    if ((p->type == MBCLIENT_TYPE_APP || p->type == MBCLIENT_TYPE_DESKTOP) 
	&& p->name 
	/* && p->mapped */  /* Include iconsized clients in sizing */
	&& p != wm_get_visible_main_client(theme->wm))
      {
	int this_width = mb_font_get_txt_width (frame->font,
						(unsigned char *)p->name, 
						strlen(p->name), 
						(p->name_is_utf8) ? MB_ENCODING_UTF8 : MB_ENCODING_LATIN);

	this_width += ( MENU_ENTRY_PADDING + theme->wm->config->use_icons );
	
	height += MBMAX(theme->wm->config->use_icons,
			mb_font_get_height(frame->font) + MENU_ENTRY_PADDING );
	
	if (this_width > width) width = this_width;
      }
   }

  if (!height) return False; 	/* No clients */
    
  width += MENU_ENTRY_PADDING;

  width  += frame->border_e + frame->border_w;
  height += frame->border_s + frame->border_n;

  if (width > space_avail)
    width = space_avail;

  if (frame->fixed_width)
    *w = frame->fixed_width;
  else
    *w = width;

  *h = height;
  
  return True;
}

static void
_theme_frame_menu_paint_text_entry(MBTheme      *theme, 
				   MBFont       *font, 
				   MBColor      *color, 
				   Client       *c, 
				   Client       *entry, 
				   MBDrawable   *dest,
				   int           x,
				   int           y)
{
  int text_height, text_width, y_offset;

  text_height = MBMAX( c->wm->config->use_icons + MENU_ICON_PADDING,
		      mb_font_get_height(font) + MENU_ENTRY_PADDING );

  text_width  = c->width - c->wm->config->use_icons - MENU_ENTRY_PADDING;

  y_offset    = y + (text_height - (mb_font_get_height(font)))/2;

  mb_font_set_color (font, color);

  mb_font_render_simple (font, dest, x, y_offset, text_width,
			 (unsigned char*)entry->name,
			 (entry->name_is_utf8) ? 
			    MB_ENCODING_UTF8 : MB_ENCODING_LATIN,
			 MB_FONT_RENDER_OPTS_CLIP_TRAIL);

  mb_col_unref(color); 	/* set_color refs */
}

void
theme_frame_menu_highlight_entry(Client         *c, 
				 MBClientButton *button, 
				 int             mode)
{
  Wm            *w = c->wm;
  MBTheme       *theme = w->mbtheme;
  MBPixbufImage *img = NULL;
  int            xx, yy;
  unsigned char  r=0x99, g = 0x99, b = 0x99;
  MBDrawable    *drw;
  MBThemeFrame  *frame;
  MBFont        *font;
  MBColor       *color;
  Client        *entry = (Client *)button->data;
  int            offset, item_h;

  frame = (MBThemeFrame *)list_find_by_id(theme->frames, FRAME_MENU);

  if (frame == NULL) 
    return;

  font  = frame->font;
  color = frame->color;

  if (frame->hl_color)
    {
      r = mb_col_red(frame->hl_color);
      g = mb_col_green(frame->hl_color);
      b = mb_col_blue(frame->hl_color);
    } 

  dbg("%s() painting +%i+%i %ix%i\n", __func__, 
      button->x, button->y, button->w, button->h);

  if (mode == INACTIVE)
    {
      XClearWindow(w->dpy, c->frame);
    }
  else
    {
      img = mb_pixbuf_img_rgba_new(c->wm->pb, button->w, button->h);
      
      mb_pixbuf_img_copy (w->pb, img, w->mbtheme->img_caches[FRAME_MENU],
			  button->x, button->y, button->w, button->h, 0, 0);
      
      for ( xx=4; xx < (button->w - 4); xx++)
	{
	  mb_pixbuf_img_plot_pixel(w->pb, img, xx, 0,
				   r, g, b);
	  mb_pixbuf_img_plot_pixel(w->pb, img, xx, button->h-2,
				   r, g, b);
	}
      
      for ( xx=3; xx < (button->w - 3); xx++)
	{
	  mb_pixbuf_img_plot_pixel(w->pb, img, xx, 1,
				   r, g, b);
	  mb_pixbuf_img_plot_pixel(w->pb, img, xx, button->h-3,
				   r, g, b);
	}
      
      for ( xx=2; xx < button->w-2; xx++)
	for ( yy=2; yy < button->h-3; yy++)
	  mb_pixbuf_img_plot_pixel(w->pb, img, xx, yy,r, g, b);
      
      drw = mb_drawable_new(w->pb, button->w, button->h);

      theme_frame_icon_paint(w->mbtheme, (Client *)button->data, img, 
			     MENU_ENTRY_PADDING/2,  
			     (button->h - w->config->use_icons)/2); 

      mb_pixbuf_img_render_to_drawable(w->pb, img, mb_drawable_pixmap(drw), 
				       0, 0);
      
      /* Now repaint font  */

      item_h = MBMAX( c->wm->config->use_icons + MENU_ICON_PADDING,
		      mb_font_get_height(font) + MENU_ENTRY_PADDING );

      offset = (item_h - (mb_font_get_height(font))) / 2;

      mb_font_render_simple (font, 
			     drw,
			     c->wm->config->use_icons + MENU_ENTRY_PADDING,
			     offset,
			     c->width - c->wm->config->use_icons - MENU_ENTRY_PADDING,
			     (unsigned char*)entry->name,
			     (entry->name_is_utf8) ? MB_ENCODING_UTF8 : MB_ENCODING_LATIN,
			     MB_FONT_RENDER_OPTS_CLIP_TRAIL);
      
      XCopyArea(w->dpy, mb_drawable_pixmap(drw), 
		c->frame, w->mbtheme->gc, 0, 0, 
		button->w, button->h, button->x, button->y);

      mb_pixbuf_img_free(w->pb, img);
      mb_drawable_unref(drw);

    }

  XSync(w->dpy, False);

  /* XXX
   *   This really shouldn't need to go below. 
   */
#ifdef USE_COMPOSITE
  comp_engine_client_show(c->wm, c); 

  if (w->all_damage)
    {
      comp_engine_render(w, w->all_damage);
      XFixesDestroyRegion (w->dpy, w->all_damage);
      w->all_damage = None;
    }
#endif

}

void
theme_frame_menu_paint(MBTheme* theme, Client *c)
{
  Wm             *w = c->wm;
  Client         *p;
  MBThemeFrame   *frame;
  MBPixbufImage  *img;
  MBFont         *font ;
  MBColor        *color;
  MBClientButton *button = NULL;
  MBList         *item;
  MBDrawable     *drawable;
  int             item_h, item_x, item_current_y, item_text_w, icon_offset = 0;


  frame = (MBThemeFrame *)list_find_by_id(theme->frames, FRAME_MENU);

  if (frame == NULL) return;

  font  = frame->font;
  color = frame->color;

  item_h = MBMAX( c->wm->config->use_icons + MENU_ICON_PADDING,
		  mb_font_get_height(font) + MENU_ENTRY_PADDING );

  icon_offset = (item_h - c->wm->config->use_icons) / 2;

  if (icon_offset < 0) icon_offset = 0;

  drawable = mb_drawable_new(w->pb, c->width, c->height);
    
  img = mb_pixbuf_img_new(theme->wm->pb, c->width, c->height);

  /* render background */
  _theme_paint_core( theme, c, frame, img, 0, 0, c->width, c->height );
  
  /* render icons */
  item_current_y = frame->border_n;

  list_enumerate(w->client_age_list, item)
    {
      p = (Client*)item->data;
      if (p->type == MBCLIENT_TYPE_APP 
	  && p->name && !(p->flags & CLIENT_IS_DESKTOP_FLAG)
	  && p->mapped 
	  && p != wm_get_visible_main_client(w))
	{
	  theme_frame_icon_paint(theme, p, img, 
				 frame->border_w + MENU_ENTRY_PADDING/2, 
				 item_current_y  + icon_offset);
	  item_current_y += item_h;
	}
    }

  list_enumerate(w->client_age_list, item)
    {
      p = (Client*)item->data;

      if (p->type == MBCLIENT_TYPE_APP 
	  && p->name && !(p->flags & CLIENT_IS_DESKTOP_FLAG)
	  && !p->mapped /* client_get_state(p) == IconicState */
	  && p != wm_get_visible_main_client(w))
      {
	theme_frame_icon_paint(theme, p, img, 
			       frame->border_w + MENU_ENTRY_PADDING/2, 
			       item_current_y + icon_offset); 
	item_current_y += item_h;
      }
    }

   if ((p = wm_get_desktop(c->wm)) != NULL) 
     {
       dbg("%s() got desktop menu entry\n", __func__);
       theme_frame_icon_paint(theme, p, img, 
			      frame->border_w + MENU_ENTRY_PADDING/2, 
			      item_current_y + icon_offset); 
     }

   /* render the pixbuf */

   mb_pixbuf_img_render_to_drawable(theme->wm->pb, img, 
				    mb_drawable_pixmap(drawable), 0, 0);

  if (c->backing_masks[MSK_NORTH] != None)
    mb_pixbuf_img_render_to_mask(theme->wm->pb, img, 
				 c->backing_masks[MSK_NORTH], 0, 0);

  if (theme->img_caches[FRAME_MENU] != NULL)
    mb_pixbuf_img_free(theme->wm->pb, theme->img_caches[FRAME_MENU]);

  theme->img_caches[FRAME_MENU] = img;    

   /* Now render fonts */

  item_current_y = frame->border_n;
  item_x = MENU_ENTRY_PADDING + c->wm->config->use_icons + frame->border_w;
  item_text_w = c->width - (frame->border_e + frame->border_w);

  /* Visible apps */

  list_enumerate(w->client_age_list, item)
    {
      p = (Client*)item->data;

      if (p->type == MBCLIENT_TYPE_APP 
	  && p->name && !(p->flags & CLIENT_IS_DESKTOP_FLAG)
	  && p->mapped
	  && p != wm_get_visible_main_client(w))
	{
	  _theme_frame_menu_paint_text_entry(theme, font, color, 
					     c, p, drawable, 
					     item_x, item_current_y);
	  
	  button = client_button_new(c, c->frame, frame->border_w, 
				     item_current_y, 
				     item_text_w, 
				     item_h,
				     True, (void* )p );
	  
	  list_add(&c->buttons, NULL, 0, (void *)button);
	  
	  item_current_y += item_h;
	}
    }
    
  /* Iconized / hidden apps */

  list_enumerate(w->client_age_list, item)
    {
      p = (Client*)item->data;

      if (p->type == MBCLIENT_TYPE_APP 
	  && p->name && !(p->flags & CLIENT_IS_DESKTOP_FLAG)
	  && !p->mapped
	  && p != wm_get_visible_main_client(w))
	{
	  _theme_frame_menu_paint_text_entry(theme, font, color, 
					     c, p, drawable, 
					     item_x, item_current_y);
	  
	  button = client_button_new(c, c->frame, frame->border_w, 
				     item_current_y, 
				     item_text_w, 
				     item_h,
				   True, (void* )p );
	  
	  list_add(&c->buttons, NULL, 0, (void *)button);
	  
	  item_current_y += item_h;
	}
    }
  
  /* The desktop at the bottom, if there is one. */

  if ((p = wm_get_desktop(c->wm)) != NULL) 
    {
      _theme_frame_menu_paint_text_entry(theme, font, color, 
					 c, p, drawable, 
					 item_x, item_current_y);

      button = client_button_new(c, c->frame, frame->border_w, 
				 item_current_y, 
				 item_text_w, 
				 item_h,
				 True, (void* )p );
      
      list_add(&c->buttons, NULL, 0, (void *)button);
    }
  

  XSetWindowBackgroundPixmap(w->dpy, c->frame, mb_drawable_pixmap(drawable));
  XClearWindow(w->dpy, c->frame);
  XSync(c->wm->dpy, False);

  mb_drawable_unref(drawable);
  return;
}


/* ------- General Utils ------------------------------------------  */

Bool 
theme_frame_supports_button_type(MBTheme *theme, 
				 int frame_type, 
				 int button_type)
{
  MBThemeFrame* frame = (MBThemeFrame*)list_find_by_id(theme->frames, 
						       frame_type );
  if (frame == NULL) return False;

  if (list_find_by_id(frame->buttons, button_type))
    return True;
  else
    return False;
}

Bool
theme_has_frame_type_defined(MBTheme *theme, int frame_type)
{
  if (list_find_by_id (theme->frames, frame_type))
    return True;
  else
    return False;
}

int
theme_frame_button_get_x_pos(MBTheme *theme, 
			     int frame_type, 
			     int button_type,
			     int width)
{
  MBThemeFrame *frame;
  MBThemeButton *button;

  frame = (MBThemeFrame*)list_find_by_id(theme->frames, frame_type );

  if (frame)
    {
      button = (MBThemeButton *)list_find_by_id(frame->buttons, 
						button_type );
      return param_get( frame, button->x, width);
    }

  return 0;
}

int
theme_frame_defined_width_get( MBTheme *theme, int frame_type )
{
  MBThemeFrame *frame = NULL;

  if (theme->wm->config->use_title == False
      && ( frame_type == FRAME_MAIN_EAST || frame_type == FRAME_MAIN_WEST))
    return 0;

  frame = (MBThemeFrame *)list_find_by_id(theme->frames, frame_type);
  if (frame) 
    {
      return frame->set_width;
    }

  return 0;
}

int
theme_frame_defined_height_get( MBTheme *theme, int frame_type )
{
  MBThemeFrame *frame = NULL;

  if (theme->wm->config->use_title == False && frame_type == FRAME_MAIN_SOUTH )
    return 0;


  frame = (MBThemeFrame *)list_find_by_id(theme->frames, frame_type);
  if (frame) 
    {
      return frame->set_height;
    }

  return 0;
}

void
theme_img_cache_clear( MBTheme *theme,  int frame_ref )
{
  if (theme->img_caches[frame_ref] != NULL) 
    mb_pixbuf_img_free(theme->wm->pb, theme->img_caches[frame_ref]);
  theme->img_caches[frame_ref] = NULL;
} 

void
theme_pixmap_cache_clear_all( MBTheme *theme )
{
  int i;

  for (i=0; i < 3; i++)
    if (theme->app_win_pxm_cache[i] != None)
      {
	dbg("%s() clearing pixmap cache\n", __func__);
	XFreePixmap(theme->wm->dpy, theme->app_win_pxm_cache[i]);
	theme->app_win_pxm_cache[i] = None;
      }
}

void
theme_img_cache_clear_all( MBTheme *theme )
{
  int i;
  for (i=0; i < N_FRAME_TYPES; i++)
    theme_img_cache_clear( theme, i );
}


/* ------------------------------------------ Creation / Parsing Code -- */

static char *
get_attr(XMLNode *node, char *key)
{
   Params *p;
   for(p = node->attr; p != NULL; p = p->next)
   {
      if (!(strcmp(p->key, key)))
	 return p->value;
   }
   return NULL;
}


/* Parsing calls */

static MBThemeParam *
param_parse(char *def_str)
{
   MBThemeParam *g;
   char *p = def_str;
   
   if (def_str == NULL) return NULL;

   g = (MBThemeParam *)malloc(sizeof(MBThemeParam));
   g->offset = 0;
   
   if (!strncmp(def_str,"object",5))
   {
        g->unit = object;
	g->value = 1;
	/* TODO pass offset */
	return g;
   }

   if (!strncmp(def_str,"labelx",6))
   {
        g->unit = textx;
	g->value = 1;
	if (def_str[6] != '\0')
	   g->offset = strtol(&def_str[6], (char **)NULL, 10);
	return g;
   }

   if (!strncmp(def_str,"labelw",6))
   {
      g->unit = textw;
      g->value = 1;
      if (def_str[6] != '\0')
	 g->offset = strtol(&def_str[6], (char **)NULL, 10);
      return g;
   }

   if (!strncmp(def_str,"labelend",8))
   {
      g->unit = MBParamLabelEnd;
      g->value = 1;
      if (def_str[8] != '\0')
	 g->offset = strtol(&def_str[8], (char **)NULL, 10);
      return g;
   }

   if (!strncmp(def_str,"totallabelw",11))
   {
      g->unit = MBParamTotalLabelWidth;
      g->value = 1;
      if (def_str[11] != '\0')
	 g->offset = strtol(&def_str[11], (char **)NULL, 10);
      return g;
   }

   while(*p != '%' && *p != '\0') { p++; }

   if (*p == '%')
   {
      *p = '\0';
      g->unit = percentage;
      g->value = strtol(def_str, (char **)NULL, 10);

      if (*(p+1) != '\0')
      {
	 g->offset = strtol(p+1, (char **)NULL, 10);
      }
   } else {
      g->unit = pixel;
      g->value = strtol(def_str, (char **)NULL, 10);
   }

   return g;
}


static int 
param_get( MBThemeFrame *frame, MBThemeParam *p, int max )
{

   if (p->unit == pixel && p->value >= 0) return p->value;
   if (p->unit == textx) return frame->label_x + p->offset;
   if (p->unit == textw) return frame->label_w + p->offset;

   if (p->unit == pixel)
     return max + p->value;  /* Note value is negaive  */

   if (p->unit == percentage)
     {
       int tmp = ( p->value / 100.00 ) * max ; /* XXX REMOVE FP */
       return ( p->value > 0 ? tmp : max + tmp ) + p->offset;
     }

   return 0;
}


static int 
lookup_button_action(char *name)
{
  int i;
  for (i=0; i < button_lookup_cnt; i++)
    if (!strcmp(name, button_lookup[i].name))
      return button_lookup[i].id;
  return -1;
}

Bool
mbtheme_button_press_activates(MBThemeButton* button)
{
  return button->press_activates;
}


MBThemeButton*
mbtheme_button_new (MBTheme *theme, 
		    char  *x, 
		    char *y, 
		    char *w, 
		    char *h, 
		    char *img_active_id, 
		    char *img_inactive_id,
		    int  active_blend,
		    int  inactive_blend,
		    char *options)
{
  MBThemeButton *button = malloc(sizeof(MBThemeButton));
  memset(button, 0, sizeof(MBThemeButton));

  if ( (button->x = param_parse(x)) == NULL) return NULL;
  if ( (button->y = param_parse(y)) == NULL) return NULL;
  if ( (button->w = param_parse(w)) == NULL) return NULL;
  if ( (button->h = param_parse(h)) == NULL) return NULL;

  dbg("%s() params parsed ok\n", __func__);

  button->inputonly          = False;
  button->wants_dbl_click    = False;
  button->img_active_blend   = active_blend;
  button->img_inactive_blend = inactive_blend;
  button->press_activates    = False;

  if (options != NULL)
    {
      if (strstr(options, "inputonly"))
	{
	  button->inputonly = True;
	  if (strstr(options, "doubleclick"))
	    button->wants_dbl_click = True;
	}

      if (strstr(options, "pressonly"))
	{
	  dbg("%s(), pressonly : %s\n", __func__, options);
	  button->press_activates = True;
	}

    }

  button->img_active   = (MBPixbufImage*)list_find_by_name(theme->images, 
							   img_active_id);
  button->img_inactive = (MBPixbufImage*)list_find_by_name(theme->images, 
							   img_inactive_id);

  if (button->img_inactive == NULL && button->inputonly == False) 
    {
      return NULL;
    }
  
  if (button->img_active == NULL) button->img_active = button->img_inactive;

  return button;
}
  
static int 
parse_frame_button_tag(MBTheme *theme, MBThemeFrame *frame, XMLNode *inode)
{
  MBThemeButton* button_new = NULL;

  char *x       = get_attr(inode, "x");
  char *y       = get_attr(inode, "y");
  char *w       = get_attr(inode, "w");
  char *h       = get_attr(inode, "h");
  char *action  = get_attr(inode, "action");
  char *options = get_attr(inode, "options"); 

  char *active_spec = NULL, *inactive_spec = NULL;

  int active_blend = 0, inactive_blend = 0;

  int action_id = 0;

  dbg("%s() parsing button tag\n", __func__);

  if ( x == NULL || y == NULL || w == NULL || h == NULL || action == NULL)
    return ERROR_MISSING_PARAMS;

  dbg("%s() params ok\n", __func__);

  if ((action_id = lookup_button_action(action)) == 0)
    return ERROR_INCORRECT_PARAMS;
  
  if (inode->kids) 
    {
      Nlist *nn;
      XMLNode *iinode;
      for(nn = inode->kids; nn != NULL; nn = nn->next)
	{
	  iinode = nn->data;
	  if (!(strcmp("active", iinode->tag)))
	    {
	      active_spec = get_attr(iinode, "pixmap"); 
	      if (active_spec == NULL) get_attr(iinode, "picture");
	      GET_INT_ATTR(iinode, "blend", active_blend);
	      continue;
	    }
	  if (!(strcmp("inactive", iinode->tag)))
	    {
	      inactive_spec = get_attr(iinode, "pixmap"); 
	      if (inactive_spec == NULL) get_attr(iinode, "picture");
	      GET_INT_ATTR(iinode, "blend", inactive_blend);
	      continue;
	    }
	}
    }

  button_new = mbtheme_button_new(theme, x, y, w, h, 
				  active_spec, inactive_spec,
				  active_blend, inactive_blend,
				  options);

  if (button_new == NULL) return ERROR_INCORRECT_PARAMS;

  dbg("%s() adding new button with action : %i\n", __func__, action_id);

  list_add(&frame->buttons, NULL, action_id, (void *)button_new);

  return 1;

}

MBThemeLabel *
mbtheme_label_new(MBTheme* theme, 
		  char*    font_id,
		  char*    col_id, 
		  char*    justify)
{
   MBThemeLabel *label = malloc(sizeof(MBThemeLabel));
   memset(label, 0, sizeof(MBThemeLabel));

   if      (!strcmp(justify, "left"))   { label->justify = ALIGN_LEFT;   }
   else if (!strcmp(justify, "center")) { label->justify = ALIGN_CENTER; }
   else if (!strcmp(justify, "right"))  { label->justify = ALIGN_RIGHT;  }
   else                                 { label->justify = ALIGN_LEFT;   }

   label->font = (MBFont *)list_find_by_name(theme->fonts, font_id);
   if (label->font == NULL) return NULL;

   label->col = (MBColor *)list_find_by_name(theme->colors, col_id);
   if (label->col == NULL) return NULL;

   return label;
}

MBThemeLabel *
mbtheme_sublabel_new(MBTheme* theme, char* sublabel_label_clip_w)
{
   MBThemeLabel *label = malloc(sizeof(MBThemeLabel));
   memset(label, 0, sizeof(MBThemeLabel));

   label->sublabel_label_clip_w = param_parse(sublabel_label_clip_w);

   return label;
}


static int 
lookup_layer_type(char *name)
{
  int i;
  for (i=0; i < layer_lookup_cnt; i++)
      if (!strcmp(name, layer_lookup[i].name))
	  return layer_lookup[i].id;
  return -1;
}

MBThemeLayer *
mbtheme_layer_new (MBTheme *theme, 
		   char    *x, 
		   char    *y, 
		   char    *w, 
		   char    *h )
{
  MBThemeLayer *layer = malloc(sizeof(MBThemeLayer));
  memset(layer, 0, sizeof(MBThemeLayer));

  if ( (layer->x = param_parse(x)) == NULL) return NULL;
  if ( (layer->y = param_parse(y)) == NULL) return NULL;
  if ( (layer->w = param_parse(w)) == NULL) return NULL;
  if ( (layer->h = param_parse(h)) == NULL) return NULL;

  return layer;
}

static int 
parse_frame_layer_tag (MBTheme      *theme, 
		       MBThemeFrame *frame, 
		       XMLNode      *inode)
{
  MBThemeLayer *layer_new;
  int type_id;
  char *attr = NULL;

  char *x    = get_attr(inode, "x");
  char *y    = get_attr(inode, "y");
  char *w    = get_attr(inode, "w");
  char *h    = get_attr(inode, "h");
  char *type = get_attr(inode, "type");

  if ( x == NULL || y == NULL || w == NULL || h == NULL || type == NULL)
    return ERROR_MISSING_PARAMS;

  layer_new = mbtheme_layer_new( theme, x, y, w, h );
  type_id   = lookup_layer_type(type);

  if (layer_new == NULL || type_id == 0) return ERROR_INCORRECT_PARAMS;
  
  list_add(&frame->layers, NULL, type_id, (void *)layer_new);

  switch (type_id)
    {
    case LAYER_PLAIN:
      attr = get_attr(inode, "color");

      layer_new->color = (MBColor*)list_find_by_name(theme->colors, attr);
      if (layer_new->color == NULL) return ERROR_INCORRECT_PARAMS;

      break;

    case LAYER_GRADIENT_HORIZ:
    case LAYER_GRADIENT_VERT:
      attr = get_attr(inode, "startcol");
      layer_new->color = (MBColor*)list_find_by_name(theme->colors, attr);

      attr = get_attr(inode, "endcol");
      layer_new->color_end = (MBColor*)list_find_by_name(theme->colors,
							      attr);
      if (layer_new->color == NULL || layer_new->color_end == NULL) 
	return ERROR_INCORRECT_PARAMS;
      break;

    case LAYER_PIXMAP:
    case LAYER_PIXMAP_TILED:
      attr = get_attr(inode, "pixmap");
      layer_new->img = (MBPixbufImage*)list_find_by_name(theme->images, attr);

      if (layer_new->img == NULL) return ERROR_INCORRECT_PARAMS;
      break;

    case LAYER_PICTURE_TILED:
    case LAYER_PICTURE:
      attr = get_attr(inode, "picture");
      layer_new->img = (MBPixbufImage*)list_find_by_name(theme->images, attr);
      if (layer_new->img == NULL) return ERROR_INCORRECT_PARAMS;
      break;

    case LAYER_LABEL:
      layer_new->label = mbtheme_label_new( theme, 
					    get_attr(inode, "font"),
					    get_attr(inode, "color"),
					    get_attr(inode, "justify") );
      if (layer_new->label == NULL) return ERROR_INCORRECT_PARAMS;
      break;
      
    }

  return 1;

}


MBThemeFrame *
mbtheme_frame_new (MBTheme *theme, 
		   char    *name, 
		   int      width, 
		   int      height, 
		   char    *options)
{
  MBThemeFrame *frame = NULL;

  frame = malloc(sizeof(MBThemeFrame));
  memset(frame, 0, sizeof(MBThemeFrame));

  frame->type = lookup_frame_type(name);

  frame->set_height  = height;
  frame->set_width   = width;
  frame->wants_shape = False;

  if (options) 
    {
      frame->options = strdup(options);
      if (strstr(options, "shaped")) frame->wants_shape = True;
    }

  return frame;
}

void
mbtheme_frame_free (MBTheme      *theme, 
		    MBThemeFrame *frame)
{
  struct list_item* next = NULL, *cur = NULL;

  cur = frame->layers;
  while (cur != NULL)
    {
      MBThemeLayer *layer;
      next = cur->next;
      
      layer = (MBThemeLayer *)cur->data;

      if (layer->label) free(layer->label);
      free(layer->x);
      free(layer->y);
      free(layer->w);
      free(layer->h);
      free(layer);
      free(cur);

      cur = next;
    }
  frame->layers = NULL;

  cur = frame->buttons;
  while (cur != NULL)
    {
      MBThemeButton *button;
      next = cur->next;
      
      button = (MBThemeButton *)cur->data;
      free(button->x);
      free(button->y);
      free(button->w);
      free(button->h);
      free(button);
      free(cur);

      cur = next;
    }

  frame->buttons = NULL;

  if (frame->options) free(frame->options);

  free(frame);
}

static int
parse_frame_tag (MBTheme *theme, 
		 XMLNode *node, 
		 char    *theme_filename)
{
  MBThemeFrame* frame_new;

  Nlist *n;
  XMLNode *inode;
   
  int size, wanted_width, wanted_height, result;
  char* id      = get_attr(node, "id");
  char* options = get_attr(node, "options");
  char *font_def = NULL, *color_def = NULL;

  int frame_type = 0;

  dbg("%s() Frame id is %s\n", __func__, id);

   GET_INT_ATTR(node, "size",   size);
   GET_INT_ATTR(node, "width",  wanted_width);
   GET_INT_ATTR(node, "height", wanted_height);

   dbg("%s size is %i\n", __func__, wanted_width);

   if (wanted_height == 0) wanted_height = size; /* Workaround depreciated 
						    size attribute.
						 */

   frame_new = mbtheme_frame_new(theme, id, wanted_width, 
				 wanted_height, options); 
   
   if (frame_new == NULL) return ERROR_INCORRECT_PARAMS;
   
   frame_type = lookup_frame_type(id);

   if (!frame_type) return ERROR_INCORRECT_PARAMS;

   if (frame_type == FRAME_MENU) /* HACK HACK HACK */
     {
       dbg("%s() frame_type is MENU\n", __func__);
       
       font_def = get_attr(node, "font");
       if (font_def == NULL) return ERROR_MISSING_PARAMS;
       
       frame_new->font = list_find_by_name(theme->fonts, font_def);
       if (frame_new->font == NULL) return ERROR_INCORRECT_PARAMS;
       
       color_def = get_attr(node, "color");
       if (color_def == NULL) return ERROR_MISSING_PARAMS;
   
       frame_new->color = list_find_by_name(theme->colors, color_def);
       if (frame_new->color == NULL) return ERROR_MISSING_PARAMS;

       /* XXX Hack around for highlight color */
       color_def = get_attr(node, "highlight");
       if (color_def != NULL)
	 {
          frame_new->hl_color = list_find_by_name(theme->colors, color_def);
	  if (frame_new->hl_color == NULL) return ERROR_INCORRECT_PARAMS;
	 }
       else frame_new->hl_color = NULL;
       
       GET_INT_ATTR(node, "border_east",  frame_new->border_e);
       GET_INT_ATTR(node, "border_west",  frame_new->border_w);
       GET_INT_ATTR(node, "border_north", frame_new->border_n);
       GET_INT_ATTR(node, "border_south", frame_new->border_s);

       GET_INT_ATTR(node, "fixed_width",  frame_new->fixed_width);

       frame_new->fixed_x = -1;

       if (get_attr(node, "fixed_x"))
	 {
	   GET_INT_ATTR(node, "fixed_x",  frame_new->fixed_x);
	 }
     }

   list_add(&theme->frames, NULL, frame_type, (void *)frame_new);
   
   for(n = node->kids; n != NULL; n = n->next)
     {
       inode = n->data;
       if (!(strcmp("layer", inode->tag)))
	 {
	   result = parse_frame_layer_tag(theme, frame_new, inode);
	   if (result < 0) 
	     {
	       show_parse_error(theme->wm, inode, 
				theme_filename, result);
	       return -1;
	     }
	   continue;
	 }
       
       if (!(strcmp("button", inode->tag)))
	 {
	   result = parse_frame_button_tag(theme, frame_new, inode);
	   if (result < 0) 
	     {
	       show_parse_error(theme->wm, inode, 
				theme_filename, result);
	       return -1;
	     }
	   
	   continue;
	 }
       if (!strcmp("panel",inode->tag))
	 {
	   if ((result = parse_panel_tag(theme, inode)) < 0)
	     {
	       show_parse_error(theme->wm, inode, theme_filename, result);
	       return -1;
	     }
	   continue;
	 }


     }

   return 1;
}

static int
parse_color_tag (MBTheme *theme, 
		 XMLNode *node)
{
  MBColor *color = NULL;
  int alpha;
  char *id     = get_attr(node, "id");
  char *spec   = get_attr(node, "def");

  if (get_attr(node, "alpha"))
    {
      fprintf(stderr, "matchbox *warning*: alpha attribute in theme.xml color tar is depreciated\n                    Use def='rrggbbaa' format instead to specify alpha\n"); 
    }
  else alpha = 0xff;

  dbg("%s() id : %s , def : %s\n", __func__, id, spec);

   if ( id == NULL || spec == NULL ) return ERROR_MISSING_PARAMS;
   
   if ((color = mb_col_new_from_spec(theme->wm->pb, spec)) == NULL)
     return ERROR_INCORRECT_PARAMS;

   dbg("%s() got color r: %i, g: %i, b: %i, a: %i\n",
       __func__, 
       mb_col_red(color),
       mb_col_green(color),
       mb_col_blue(color),
       mb_col_alpha(color));

   list_add(&theme->colors, id, 0, (void *)color);

   return 1;
}


static int
parse_font_tag (MBTheme *theme, 
		XMLNode *node)
{
  MBFont *font = NULL;

  char *id     = get_attr(node, "id");
  char *spec   = get_attr(node, "def");

  if ( id == NULL || spec == NULL ) 
    return ERROR_MISSING_PARAMS;
  
  if ((font = mb_font_new_from_string(theme->wm->dpy, spec)) == NULL)
    return ERROR_INCORRECT_PARAMS;

  dbg("%s() got font family: %s size: %i\n", 
      __func__, mb_font_get_family(font), mb_font_get_point_size(font));

  list_add(&theme->fonts, id, 0, (void *)font);

  return 1;
}

static int
parse_pixmap_tag (MBTheme *theme, 
		  XMLNode *node)
{
  MBPixbufImage *img = NULL;
  char *id         = get_attr(node, "id");
  char *filename   = get_attr(node, "filename");

  if ( id == NULL || filename == NULL ) return ERROR_MISSING_PARAMS;

  if ((img = mb_pixbuf_img_new_from_file(theme->wm->pb, filename)) == NULL)
    return ERROR_LOADING_RESOURCE;

  list_add(&theme->images, id, 0, (void *)img);  

  return 1;
}

static int
parse_panel_tag (MBTheme *theme, 
		  XMLNode *node)
{
  Wm *wm = theme->wm;

  char *x       = get_attr(node, "x");
  char *y       = get_attr(node, "y");
  char *w       = get_attr(node, "w");
  char *h       = get_attr(node, "h");

  dbg("%s() parsing panel tag\n", __func__);

  if ( x == NULL || y == NULL || wm == NULL || h == NULL)
    return ERROR_MISSING_PARAMS;

  theme->have_toolbar_panel = True;

  theme->toolbar_panel_x = param_parse(x);
  theme->toolbar_panel_y = param_parse(y);
  theme->toolbar_panel_w = param_parse(w);
  theme->toolbar_panel_h = param_parse(h);

  return 1;
}

static int
parse_lowlight_tag (MBTheme *theme, 
		    XMLNode *node)
{
  MBColor *color = NULL;
  char *color_attr     = get_attr(node, "color");

  /* matchbox not running with lowlight switch */
#ifndef USE_COMPOSITE
  if (!theme->wm->config->dialog_shade) return 1; 
#endif

  if ( color_attr == NULL ) return ERROR_MISSING_PARAMS;

  color = (MBColor*)list_find_by_name(theme->colors, color_attr);

  if (color == NULL) return ERROR_INCORRECT_PARAMS;

  theme->wm->config->lowlight_params[0] = mb_col_red(color);
  theme->wm->config->lowlight_params[1] = mb_col_green(color);
  theme->wm->config->lowlight_params[2] = mb_col_blue(color);
  theme->wm->config->lowlight_params[3] = mb_col_alpha(color);
  
  return 1;
}

static int
parse_app_icon_tag (MBTheme *theme, 
		    XMLNode *node)
{
  MBPixbufImage *img = NULL;
  char *pixmap_attr     = get_attr(node, "pixmap");

  if ( pixmap_attr == NULL ) return ERROR_MISSING_PARAMS;

  img = (MBPixbufImage*)list_find_by_name(theme->images, pixmap_attr);

  if (img == NULL) return ERROR_INCORRECT_PARAMS;

  if (theme->wm->img_generic_icon) 
    mb_pixbuf_img_free(theme->wm->pb, theme->wm->img_generic_icon);
  
  theme->wm->img_generic_icon 
    = mb_pixbuf_img_clone(theme->wm->pb, img);

  misc_scale_wm_app_icon(theme->wm);

  return 1;
}


#ifdef USE_COMPOSITE
static int
parse_shadow_tag (MBTheme *theme, 
		  XMLNode *node)
{
  /* 
     attributes are;
       style : off ?, none, simple, gaussian * only needed one *
       color : ref to defined color 
          dx : x offset of shadow ( can be -ive )
          dy : y offset of shadow ( can be -ive )
       width : extra width of shadow   - only used for simple shadows ?
      height : extra height of shadow
  */
  MBColor *color = NULL;
  Wm           *w     = theme->wm;

  char *style         = get_attr(node, "style");
  char *color_attr    = get_attr(node, "color");

  if ( style == NULL ) return ERROR_MISSING_PARAMS;

  if (!strcasecmp(style, "simple")) 
    {
      w->config->shadow_style = SHADOW_STYLE_SIMPLE;
    } 
  else if (!strcasecmp(style, "gaussian")) 
    {
      w->config->shadow_style = SHADOW_STYLE_GAUSSIAN;
    }      
  else if (!strcasecmp(style, "none")) 
    {
      w->config->shadow_style = SHADOW_STYLE_NONE;
      return 1;
    }
  else return ERROR_INCORRECT_PARAMS;

  if (color_attr)
    {
      color = (MBColor*)list_find_by_name(theme->colors, color_attr);

      if (color == NULL) return ERROR_INCORRECT_PARAMS;

      w->config->shadow_color[0] = mb_col_red(color);
      w->config->shadow_color[1] = mb_col_green(color);
      w->config->shadow_color[2] = mb_col_blue(color);
      w->config->shadow_color[3] = mb_col_alpha(color);
    }
  
  if (get_attr(node, "dx"))
    {
      GET_INT_ATTR(node, "dx", w->config->shadow_dx);   
    }

  if (get_attr(node, "dy"))
    {
      GET_INT_ATTR(node, "dy", w->config->shadow_dy);   
    }

  if (get_attr(node, "width"))
    {
      GET_INT_ATTR(node, "width", w->config->shadow_padding_width);   
    }

  if (get_attr(node, "height"))
    {
      GET_INT_ATTR(node, "height", w->config->shadow_padding_height);   
    } 

  return 1;
}
#endif

MBTheme *
mbtheme_new (Wm *w)
{
  XGCValues gv;
  int       i;
  MBTheme *t = (MBTheme *)malloc(sizeof(MBTheme));
  memset(t, 0, sizeof(MBTheme));

  for (i=0; i<3; i++)
    t->app_win_pxm_cache[i] = None;

  gv.graphics_exposures = False;
  gv.function           = GXcopy;
  t->gc = XCreateGC(w->dpy, w->root, GCGraphicsExposures|GCFunction, &gv);

  XSetForeground(w->dpy, t->gc, w->grey_col.pixel); 
  
  gv.function       = GXinvert;
  gv.subwindow_mode = IncludeInferiors;
  gv.line_width     = 1;
  t->band_gc = XCreateGC(w->dpy, w->root, 
			 GCFunction|GCSubwindowMode|GCLineWidth, &gv);
  t->mask_gc = None;
  t->wm = w;
  
  t->frames   = NULL;
  t->images   = NULL;
  t->pictures = NULL;
  t->colors   = NULL;
  t->fonts    = NULL;
  
  t->have_toolbar_panel = False;
  

  return t;
}

void
mbtheme_free (Wm      *w, 
	      MBTheme *theme)
{
  struct list_item* next = NULL, *cur = NULL;

  if (theme->toolbar_panel_x) free(theme->toolbar_panel_x);
  if (theme->toolbar_panel_y) free(theme->toolbar_panel_y);
  if (theme->toolbar_panel_w) free(theme->toolbar_panel_w);
  if (theme->toolbar_panel_h) free(theme->toolbar_panel_h);

  cur = theme->frames;
  while (cur != NULL)
    {
      next = cur->next;
      if (cur->name) free(cur->name);
      mbtheme_frame_free (theme, (MBThemeFrame*)cur->data);
      free(cur);
      cur = next;
    }
  theme->frames = NULL;

  cur = theme->images;
  while (cur != NULL)
    {
      next = cur->next;
      if (cur->name) free(cur->name);
      mb_pixbuf_img_free(w->pb, (MBPixbufImage *)cur->data);
      free(cur);
      cur = next;
    }
  theme->images = NULL;

  cur = theme->colors;
  while (cur != NULL)
    {
      next = cur->next;
      if (cur->name) free(cur->name);
      mb_col_unref((MBColor*)cur->data);
      free(cur);
      cur = next;
    }
  theme->colors = NULL;

  cur = theme->fonts;
  while (cur != NULL)
    {
      next = cur->next;
      if (cur->name) free(cur->name);
      mb_font_unref((MBFont*)cur->data);
      free(cur);
      cur = next;
    }
  theme->fonts = NULL;

  if (theme->gc) XFreeGC(w->dpy, theme->gc);
  if (theme->band_gc) XFreeGC(w->dpy, theme->band_gc);
  if (theme->mask_gc) XFreeGC(w->dpy, theme->mask_gc);

  theme_img_cache_clear_all (theme);

  theme_pixmap_cache_clear_all( theme );

  free(theme);

  w->mbtheme = NULL;
}

void
mbtheme_switch (Wm   *w, 
		char *new_theme_name)
{
  Client *p = NULL;

  XGrabServer(w->dpy);

  /* now the fun part */
  mbtheme_free(w, w->mbtheme);

  /* load the new theme */
  mbtheme_init(w, new_theme_name);

  theme_img_cache_clear( w->mbtheme, FRAME_MAIN );

  /* sort having titlebar panel, no theme defintion */
  if (w->have_titlebar_panel) 
    {
      if (mbtheme_has_titlebar_panel(w->mbtheme))
	{
	  dbg("%s() mapping titlebar\n", __func__ );
	  XMapWindow(w->dpy, w->have_titlebar_panel->frame);
	}
      else
	{
	  w->have_titlebar_panel->ignore_unmap++;
	  XUnmapWindow(w->dpy, w->have_titlebar_panel->frame);
	}
    }

  /* Now resize ( due to new frames ) + repaint everything */

  stack_enumerate(w, p)
    {
      client_buttons_delete_all(p);
      
      if (p->type == MBCLIENT_TYPE_DIALOG)
	{
	  XRectangle rect;
	  Region     xregion;
	  
	  /*  Old theme may have been shaped and this one not. 
	   *  Therefore we 'clear' any possible shapes set on 
	   *  the window frames.  
	   */
	  
	  xregion = XCreateRegion ();
	  
	  rect.x      = 0;
	  rect.y      = 0;
	  rect.width  = p->width  + 100;
	  rect.height = p->height + 100;
	  
	  XUnionRectWithRegion (&rect, xregion, xregion);
	  
	  XShapeCombineRegion (w->dpy, client_title_frame(p), 
			       ShapeBounding, 0, 0, 
			       xregion, ShapeSet);
	  
	  XShapeCombineRegion (w->dpy, p->frame,ShapeBounding, 0, 0, 
			       xregion, ShapeSet);
	  
	  XDestroyRegion (xregion);
	}
      
      p->configure(p);
      p->move_resize(p);
      p->redraw(p, False);
    }

  ewmh_update_rects(w); /* theme *could* affect this */
    
  XSync(w->dpy, False);

  XUngrabServer(w->dpy);

  comp_engine_render(w, None);
}


static void
show_parse_error (Wm *w,
		  XMLNode *node, 
		  char    *theme_file, 
		  int      err_num)
{
  Params *p;

  if (w->mbtheme == NULL) return; /* We've already been called no doubt */

  fprintf(stderr, "Matchbox: error parsing %s\n", theme_file);

  switch (err_num)
    {
    case ERROR_MISSING_PARAMS:
      fprintf(stderr, "Missing Params in ");
      break;
    case ERROR_INCORRECT_PARAMS:
      fprintf(stderr, "Incorrect Params in ");
      break;
    case ERROR_LOADING_RESOURCE:
      fprintf(stderr, "Error loading resource from ");
      break;
    default :
      fprintf(stderr, "Unknown error in ");
      break;
    }  

  fprintf(stderr, "<%s", node->tag);
  for(p = node->attr; p != NULL; p = p->next)
    fprintf(stderr, " %s='%s'", p->key, p->value);
  fprintf(stderr, "/>\n");

  if (!strncmp(theme_file, DEFAULTTHEME, 255))
    exit(1); 
  
  fflush(stderr);

  mbtheme_free(w, w->mbtheme);
}

static Bool 
file_exists(char *filename)
{
  struct stat st;
  if (stat(filename, &st)) return False;
  return True;
}

void
mbtheme_init (Wm   *w, 
	      char *theme_name)
{
  int err = 0;
  XMLNode *root_node, *cnode;
  Nlist *n;

  XMLParser *parser = xml_parser_new();

  char theme_filename[255] = DEFAULTTHEME;
  char *theme_path = NULL;

#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

  char orig_wd[MAXPATHLEN];
  
  if (theme_name != NULL) { 
    if (theme_name[0] == '/')
      strncpy(theme_filename, theme_name, 255);
    else
      {
	snprintf(theme_filename, 255, "%s/.themes/%s/matchbox/theme.xml",
		 getenv("HOME"), theme_name);
	if (!file_exists(theme_filename))
	  {
	    snprintf(theme_filename, 255, "%s/themes/%s/matchbox/theme.xml",
		     DATADIR, theme_name);
	  }
      }
  } else { theme_name = DEFAULTTHEMENAME; }
  
  if (!file_exists(theme_filename))
    {
      fprintf(stderr, "matchbox-wm: unable to open theme: %s\n", 
	      theme_filename); 
      /* If the defualt is not openable either - give up :( */
      if (!file_exists(DEFAULTTHEME)) exit(1);
      strncpy(theme_filename, DEFAULTTHEME, 255);
    }
  
  if (getcwd(orig_wd, MAXPATHLEN) == (char *)NULL)
    {
      fprintf(stderr, "matchbox: cant get current directory\n");
      exit(1);
    }
  
  theme_path = strdup(theme_filename);
  theme_path[strlen(theme_filename)-9] = '\0';
  
  chdir(theme_path);

  XChangeProperty(w->dpy, w->root, w->atoms[_MB_THEME], XA_STRING, 8,
		  PropModeReplace, (unsigned char*)theme_path, strlen(theme_path));

  XChangeProperty(w->dpy, w->root, w->atoms[_MB_THEME_NAME], XA_STRING, 8,
		  PropModeReplace, (unsigned char*)theme_name, strlen(theme_name));

  free(theme_path);

  comp_engine_set_defualts(w);

  root_node = xml_parse_file_dom(parser, theme_filename);

  if (root_node == NULL)
    {
     fprintf(stderr, "matchbox-wm: Failed to parse theme file: %s\n", 
	     theme_filename);
     fprintf(stderr, "matchbox-wm: Please check this file contains valid XML\n") ;

     if (!strncmp(theme_filename, DEFAULTTHEME, 255))
       exit(1); 		    /* give up, the defualt theme is corrupt */
     fprintf(stderr, "matchbox-wm: switching to default\n");
     xml_parser_free(parser, root_node); 
     return mbtheme_init (w, NULL); /* try again with defualt */
   }

   /* Check version info */
   if (!get_attr(root_node, "engine_version")
       || (strcmp(get_attr(root_node, "engine_version"), "1") != 0))
     {
       fprintf(stderr, "matchbox-wm: %s is not valid for this version of matchbox.\n", theme_filename );
       if (!strncmp(theme_filename, DEFAULTTHEME, 255))
	 exit(1); 	   /* give up, the defualt theme is corrupt */
       fprintf(stderr, "matchbox: switching to default\n");
       xml_parser_free(parser, root_node); 
       return mbtheme_init (w, NULL); 
     }       

   w->mbtheme = mbtheme_new(w);

   if (get_attr(root_node, "cache") 
       && !strcasecmp(get_attr(root_node, "cache"), "false"))
     {
       if (!getenv("MB_THEME_ALWAYS_CACHE"))
	 w->mbtheme->disable_pixbuf_cache = True;
     }
   
   for(n = root_node->kids; n != NULL; n = n->next)
   {
      cnode = n->data;
      if (!strcmp("color",cnode->tag))
      {
	 if ((err = parse_color_tag(w->mbtheme, cnode)) < 0)
	   {
	     show_parse_error(w, cnode, theme_filename, err);
	     xml_parser_free(parser, root_node); 
	     return mbtheme_init (w, NULL); 
	   }

	 continue;
      }
      if (!strcmp("font",cnode->tag))
      {
	 if ((err = parse_font_tag(w->mbtheme, cnode)) < 0)
	   {
	     show_parse_error(w, cnode, theme_filename, err);
	     xml_parser_free(parser, root_node); 
	     return mbtheme_init (w, NULL); 
	   }
	 continue;
      }

      if (!strcmp("frame", cnode->tag))
      {
	 if ((err = parse_frame_tag(w->mbtheme, cnode, theme_filename)) < 0)
	   {
	     show_parse_error(w, cnode, theme_filename, err);
	     xml_parser_free(parser, root_node); 
	     return mbtheme_init (w, NULL); 
	   }
	 continue;
      }

      if (!strcmp("pixmap",cnode->tag))
      {
	 if ((err =parse_pixmap_tag(w->mbtheme, cnode)) < 0)
	   {
	     show_parse_error(w, cnode, theme_filename, err);
	     xml_parser_free(parser, root_node); 
	     return mbtheme_init (w, NULL); 
	   }
	 continue;
      }

      if (!strcmp("lowlight",cnode->tag))
      {
	if ((err =parse_lowlight_tag(w->mbtheme, cnode)) < 0)
	  {
	    show_parse_error(w, cnode, theme_filename, err);
	    xml_parser_free(parser, root_node); 
	    return mbtheme_init (w, NULL); 
	  }
	 continue;
      }

      if (!strcmp("appicon",cnode->tag))
      {
	if ((err =parse_app_icon_tag(w->mbtheme, cnode)) < 0)
	  {
	    show_parse_error(w, cnode, theme_filename, err);
	    xml_parser_free(parser, root_node); 
	    return mbtheme_init (w, NULL); 
	  }
	 continue;
      }

#ifdef USE_COMPOSITE
      if (!strcmp("shadow", cnode->tag))
      {
	if ((err =parse_shadow_tag(w->mbtheme, cnode)) < 0)
	  {
	    show_parse_error(w, cnode, theme_filename, err);
	    xml_parser_free(parser, root_node); 
	    return mbtheme_init (w, NULL); 
	  }
	 continue;
      }
#endif
#ifdef HAVE_XCURSOR
      if (!strcmp("cursor", cnode->tag))
      {
	char *cursor_theme = NULL;
	int   cursor_size = -1;

	cursor_theme = get_attr(cnode, "theme");
	GET_INT_ATTR(cnode, "size", cursor_size);

	dbg ("Got cursor theme:%s size:%i\n", cursor_theme, cursor_size);

	if (cursor_theme)
	  XcursorSetTheme (w->dpy, cursor_theme);

	if (cursor_size > -1)
	  XcursorSetDefaultSize (w->dpy, cursor_size);

	dbg ("cursor size is %i\n", XcursorGetDefaultSize (w->dpy));

	 continue;
      }
#endif
   }

   chdir(orig_wd);

   xml_parser_free(parser, root_node); 

   comp_engine_theme_init(w);

}

Bool
mbtheme_has_titlebar_panel(MBTheme *theme)
{
  return theme->have_toolbar_panel;
}


Bool
mbtheme_get_titlebar_panel_rect(MBTheme    *theme, 
				XRectangle *rect,
				Client     *ignore_client)
{
  MBThemeFrame *frame;
  int width, height;

  if (!theme->have_toolbar_panel) return False;

  frame = (MBThemeFrame*)list_find_by_id(theme->frames, FRAME_MAIN );

  if (!frame) return False;

  height = theme_frame_defined_height_get(theme, FRAME_MAIN);
  width  = theme->wm->dpy_width 
    - wm_get_offsets_size(theme->wm, EAST,  ignore_client, True)
    - wm_get_offsets_size(theme->wm, WEST,  ignore_client, True);

  rect->x = param_get( frame, theme->toolbar_panel_x, width );
  rect->y = param_get( frame, theme->toolbar_panel_y, height );
  rect->width  = param_get( frame, theme->toolbar_panel_w, width );
  rect->height = param_get( frame, theme->toolbar_panel_h, height );

  return True;
}



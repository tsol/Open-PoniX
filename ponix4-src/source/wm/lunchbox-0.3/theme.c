/**************************************************************************
    Lunchbox Window Manager
    Copyright (C) 2008 Alysander Stanley

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

#include "lunchbox.h"
#include "theme.h"
#include "xcheck.h"
#include "util.h"

/**
@file     theme.c
@brief    Contains functions relevant to the theming system. It works by using the stored co-ordinates in a set of region files to identify and copy out sections of corresponding image files for each individual widget.
@author   Alysander Stanley
**/


enum Splash_background_tile {
  tile_splash_parent
};

enum Menubar_background_tile {
  tile_menubar_parent /* tile_menubar_parent must be last */
};

enum Popup_menu_background_tile {
  tile_popup_t_edge,
  tile_popup_l_edge,
  tile_popup_b_edge,
  tile_popup_r_edge,
  tile_popup_parent /*tile_popup_parent must be last */
};

enum Frame_background_tile {
  tile_t_edge,
  tile_titlebar,
  tile_l_edge,
  tile_b_edge,
  tile_r_edge,
  tile_title_menu_text, 
  tile_title_menu_icon, //TODO
  tile_frame_parent /* tile_frame_parent must be last */
};

static struct Widget_theme *create_component_theme(Display *display, char *type);

static void create_font_themes(struct Themes *restrict themes);
static void swap_widget_theme(struct Widget_theme *from, struct Widget_theme *to);
static void swap_tiled_widget_themes(char *type, struct Widget_theme *themes, struct Widget_theme *tiles);

static void create_widget_theme_pixmap(Display *display, struct Widget_theme *widget_theme, cairo_surface_t **theme_images);

static Pixmap create_text_background_pixmap(Display *display, const char *restrict text
, const struct Font_theme *restrict font_theme, Pixmap background_p, int b_w, int b_h);

static void remove_widget_themes (Display *display, struct Widget_theme *themes, int length);
static void create_mode_menu_text(Display *display, struct Themes *themes);

#if 0
//internal pixmap testing function
static void show_pixmap (Display *display, Pixmap pixmap); 

static void show_pixmap (Display *display, Pixmap pixmap) {
  Window root = DefaultRootWindow(display);
  Screen *screen    = DefaultScreenOfDisplay(display);
  int black = BlackPixelOfScreen(screen);	  
  Window temp = XCreateSimpleWindow(display, root
  , 20, 20
  , 120, 120, 0, black, black); 

  XSetWindowBackgroundPixmap(display, temp, pixmap);
  XMapWindow(display, temp);
  XFlush(display);
}
#endif

/**
@brief    create_themes opens the theme in the theme folder with the name specified by theme_name.
          It changes the current working directory before calling create_component_theme each window type.
          If an error occurs when opening the theme a NULL pointer is returned. 
@return   The completed Themes struct or NULL if an error has occured.
**/
struct Themes *
create_themes(Display *display, char *theme_name) {
  struct Themes *themes = NULL;
  char *path = NULL;
  
  /*
  char *home = getenv("HOME");

  if(!home) {
    fprintf(stderr, "Could not access HOME environmental variable\n");
    return NULL;
  }
  */
  path = malloc(PATH_SIZE * sizeof(char));
  if(!path) {
    fprintf(stderr, "Out of memory\n");
    return NULL;
  }
  
  //strnadd(path, home, "/.lunchbox/themes/", PATH_SIZE);
  strnadd(path, "/usr", "/local/lunchbox/themes/", PATH_SIZE);
  
  strncat(path, theme_name, PATH_SIZE);
  printf("The theme path is: %s\n", path);
  if(chdir(path)) {
    fprintf(stderr, "Error opening theme path: %s\n", path);
    goto error;
  }

  themes = calloc(1, sizeof(struct Themes));
  if(themes == NULL) {
    fprintf(stderr, "Error: insufficient memory\n");
    goto error;
  }

//TODO make the file name come from another theme file in case things are being reused.
  themes->window_type[unknown]        = create_component_theme(display, "program_frame");
  if(!themes->window_type[unknown]) goto error;

//  themes->window_type[splash]         = create_component_theme(display, "splash_frame");
  themes->window_type[file]           = create_component_theme(display, "file_frame");
  themes->window_type[program]        = create_component_theme(display, "program_frame");
  themes->window_type[dialog]         = create_component_theme(display, "dialog_frame");
  themes->window_type[modal_dialog]   = create_component_theme(display, "modal_dialog_frame"); 
  themes->window_type[utility]        = create_component_theme(display, "utility_frame"); 
  themes->window_type[status]         = create_component_theme(display, "status_frame");
  themes->window_type[system_program] = create_component_theme(display, "system_program_frame");
  themes->window_type[panel]          = create_component_theme(display, "panel_frame");
//  themes->window_type[popup_menubar]  = create_component_theme(display, "program_frame");  


/****
TODO Verify that: 
  t_edge                         x >= 0, y >= 0, w <=0,  h >  0
  l_edge                         x >= 0, y >= 0, w > 0,  h <= 0
  b_edge                         x > 0,  y < 0,  w <= 0, h >  0
  r_edge                         x < 0,  y >= 0, w > 0,  h <= 0
  title_menu_text: all states exist for all defined window types
  window                         x >= 0, y >= 0, w <= 0, h <= 0
  frame_parent                   x == 0, y == 0, w == 0, h == 0
  title_menu_rhs                 w > 0
  mode_dropdown_text              w > 0, h > 0
  all menubar item width and heights are greater than zero
  title menu must be just before the mode menu.
  Assumes that the total mode menu text is less that the w of the mode menu in the theme.
****/
  
  themes->popup_menu = create_component_theme(display, "popup_menu");
  if(!themes->popup_menu) goto error;
  themes->menubar = create_component_theme(display, "menubar");
  if(!themes->menubar) goto error;

  free(path);
  create_font_themes(themes);
  create_mode_menu_text(display, themes);
  return themes;

  error:
  free(path);
  if(themes) free(themes);
  return NULL;
}

/**
@brief    This function creates the different states for the mode pulldown list (depending on the mode chosen for each frame type 
@todo     Need to draw a good background and then tile the "text" bit. 
@return   void
**/
static void
create_mode_menu_text(Display *display, struct Themes *themes) {
  /** This is the custom create mode menu LHS section **/
  themes->mode_pulldown_width = get_text_width(display, "Floating", &themes->font_theme[active]) + MODE_ICON_SIZE;
  for(int i = 0; i <= system_program; i++) {
    for(int j = 0; j <= inactive; j++) {
      if(themes->window_type[i] != NULL
      && themes->window_type[i][mode_dropdown_text].state_p[j]) {

        themes->window_type[i][mode_dropdown_text_floating].state_p[j] = create_text_background_pixmap(display, "Floating"
        , &themes->font_theme[active], themes->window_type[i][mode_dropdown_text].state_p[j]
        , themes->window_type[i][mode_dropdown_text].w
        , themes->window_type[i][mode_dropdown_text].h);

        themes->window_type[i][mode_dropdown_text_tiling].state_p[j] = create_text_background_pixmap(display, "Tiling"
        , &themes->font_theme[active], themes->window_type[i][mode_dropdown_text].state_p[j]
        , themes->window_type[i][mode_dropdown_text].w
        , themes->window_type[i][mode_dropdown_text].h);

        themes->window_type[i][mode_dropdown_text_desktop].state_p[j] = create_text_background_pixmap(display, "Desktop"
        , &themes->font_theme[active], themes->window_type[i][mode_dropdown_text].state_p[j]
        , themes->window_type[i][mode_dropdown_text].w
        , themes->window_type[i][mode_dropdown_text].h);
      }
    }
  }
  //show_pixmap (display,  themes->window_type[unknown][mode_dropdown_text_floating].state_p[normal]);
}

/**
@brief    This function creates the complete set of pixmaps for a particular component of a theme such as the menubar or dialog frame.
@return   the completed Widget_theme or NULL if an error has occured.
**/
static struct Widget_theme *
create_component_theme(Display *display, char *type) {
  struct Widget_theme *themes = NULL;
  struct Widget_theme *tiles  = NULL;
  char *filename = NULL;
  FILE *regions = NULL;

  unsigned int nwidgets = 0;
  unsigned int ntiles = 0;
  cairo_surface_t *theme_images[inactive + 1];
  for(int i = 0; i <= inactive; i++) theme_images[i] = NULL;

  /*Check that the theme set name is valid */
  if(strcmp(type, "program_frame")
  && strcmp(type, "file_frame")
  && strcmp(type, "dialog_frame")
  && strcmp(type, "modal_dialog_frame")
  && strcmp(type, "utility_frame")
  && strcmp(type, "unknown_frame")
  && strcmp(type, "panel_frame")  
  && strcmp(type, "popup_menu")
  && strcmp(type, "popup_menubar")
  && strcmp(type, "splash")
  && strcmp(type, "menubar"))  {
    fprintf(stderr, "Unknown theme component \"%s\" specified\n", type);
    return NULL;
  }

  /* Establish the number of widgets in set */
  if(strstr(type, "frame")) {
    nwidgets = frame_parent + 1;
    ntiles = tile_frame_parent + 1;
  }
  else if(strstr(type, "menubar")) {
    nwidgets = menubar_parent + 1; 
    ntiles =   tile_menubar_parent + 1;
  }
  else if(!strcmp(type, "popup_menu")) {
    nwidgets = popup_menu_parent + 1; 
    ntiles =   tile_popup_parent + 1;
  }
  //TODO splash ?

  /* Allocate memory and open files */
  themes = calloc(nwidgets, sizeof(struct Widget_theme));
  if(!themes)   goto error;

  tiles =  calloc(ntiles,   sizeof(struct Widget_theme));  
  if(!tiles)    goto error;

  filename = calloc(PATH_SIZE, sizeof(char));
  if(!filename) goto error;

  regions = fopen(strnadd(filename, type, "_regions", WIDGET_NAME_SIZE), "r");
  if(regions == NULL) {
    fprintf(stderr, "Error:  A required theme file \"%s_regions\" could not be accessed\n", type);
    goto error;
  }

  /***
  To add another tiled background the following steps need to be followed:
  1) Add item to enumerators.
  2) Add tile name in strcmp below.  This should be the widget's name prepended with "tile_".
  3) Add in appropriate swap_widget command to be used before and after the pixmaps have been created.
  ***/
  
  /* Read in entries from the theme's region file into the theme array. */
  while(!feof(regions)) {
    int returned = 0;
    unsigned int current_widget;
    unsigned int current_tile;
    char widget_name[WIDGET_NAME_SIZE];
    int x,y,w,h;
    int was_tile = 0;
    returned = fscanf(regions, "%s %d %d %d %d\n", widget_name, &x, &y, &w, &h);
    if(returned != 5) {
      fprintf(stderr, "Error in theme format, required string int int int int\n");
      fprintf(stderr, "which represent the widget name, x position, y position, width and height repectively\n");
      goto error;
    }
    //else fprintf(stderr, "Loading theme for %s\n", widget_name);
    if(strstr(type, "frame")) { 
      if(!strcmp(widget_name, "window"))                   current_widget = window;
      else if(!strcmp(widget_name, "titlebar"))            current_widget = titlebar;
      else if(!strcmp(widget_name, "t_edge"))              current_widget = t_edge;
      else if(!strcmp(widget_name, "l_edge"))              current_widget = l_edge;
      else if(!strcmp(widget_name, "b_edge"))              current_widget = b_edge;
      else if(!strcmp(widget_name, "r_edge"))              current_widget = r_edge;
      else if(!strcmp(widget_name, "tl_corner"))           current_widget = tl_corner;
      else if(!strcmp(widget_name, "tr_corner"))           current_widget = tr_corner;
      else if(!strcmp(widget_name, "bl_corner"))           current_widget = bl_corner;
      else if(!strcmp(widget_name, "br_corner"))           current_widget = br_corner;
      else if(!strcmp(widget_name, "selection_indicator")) current_widget = selection_indicator;
      else if(!strcmp(widget_name, "selection_indicator_hotspot")) current_widget = selection_indicator_hotspot;
      else if(!strcmp(widget_name, "title_menu_lhs"))      current_widget = title_menu_lhs;
    //else if(!strcmp(widget_name, "title_menu_icon"))     current_widget = title_menu_icon; /* this can have the icon*/
      else if(!strcmp(widget_name, "title_menu_text"))     current_widget = title_menu_text;
      else if(!strcmp(widget_name, "title_menu_rhs"))      current_widget = title_menu_rhs;
      else if(!strcmp(widget_name, "title_menu_hotspot"))  current_widget = title_menu_hotspot;
      else if(!strcmp(widget_name, "mode_dropdown_lhs"))    current_widget = mode_dropdown_lhs;
      else if(!strcmp(widget_name, "mode_dropdown_text"))   current_widget = mode_dropdown_text;
      else if(!strcmp(widget_name, "mode_dropdown_rhs"))    current_widget = mode_dropdown_rhs;
      else if(!strcmp(widget_name, "mode_dropdown_hotspot"))current_widget = mode_dropdown_hotspot;
      else if(!strcmp(widget_name, "close_button"))         current_widget = close_button;
      else if(!strcmp(widget_name, "close_button_hotspot")) current_widget = close_button_hotspot ;
      else if(!strcmp(widget_name, "frame_parent"))         current_widget = frame_parent;
      else if(!strcmp(widget_name, "tile_titlebar")) {         was_tile = 1; current_tile = tile_titlebar;  }
      else if(!strcmp(widget_name, "tile_t_edge")) {           was_tile = 1; current_tile = tile_t_edge;    }
      else if(!strcmp(widget_name, "tile_l_edge")) {           was_tile = 1; current_tile = tile_l_edge;    }
      else if(!strcmp(widget_name, "tile_b_edge")) {           was_tile = 1; current_tile = tile_b_edge;    }
      else if(!strcmp(widget_name, "tile_r_edge")) {           was_tile = 1; current_tile = tile_r_edge;    }
      else if(!strcmp(widget_name, "tile_title_menu_text")) {  was_tile = 1; current_tile = tile_title_menu_text; }
      else if(!strcmp(widget_name, "tile_frame_parent")) {     was_tile = 1; current_tile = tile_frame_parent;    }

      else goto name_error;
    }
    else if(strstr(type, "menubar")) {
      if(!strcmp(widget_name, "program_menu"))        current_widget = program_menu;
      else if(!strcmp(widget_name, "window_menu"))    current_widget = window_menu;
      else if(!strcmp(widget_name, "options_menu"))   current_widget = options_menu;
      else if(!strcmp(widget_name, "links_menu"))     current_widget = links_menu;
      else if(!strcmp(widget_name, "tool_menu"))      current_widget = tool_menu;
      else if(!strcmp(widget_name, "menubar_parent")) current_widget = menubar_parent;
      else if(!strcmp(widget_name, "tile_menubar_parent")) {was_tile = 1; current_tile = tile_menubar_parent;}
      else goto name_error;
    }
    else if(!strcmp(type, "popup_menu")) {
//      printf("%s x %d, y %d, w %d, h %d\n", widget_name, x, y, w, h);
           if(!strcmp(widget_name, "menu_item_lhs"))   current_widget = menu_item_lhs;
      else if(!strcmp(widget_name, "menu_item_mid"))   current_widget = menu_item_mid;
      else if(!strcmp(widget_name, "menu_item_rhs"))   current_widget = menu_item_rhs;
      else if(!strcmp(widget_name, "popup_t_edge"))      current_widget = popup_t_edge;
      else if(!strcmp(widget_name, "popup_l_edge"))      current_widget = popup_l_edge;
      else if(!strcmp(widget_name, "popup_b_edge"))      current_widget = popup_b_edge;
      else if(!strcmp(widget_name, "popup_r_edge"))      current_widget = popup_r_edge;
      else if(!strcmp(widget_name, "popup_tl_corner"))   current_widget = popup_tl_corner;
      else if(!strcmp(widget_name, "popup_tr_corner"))   current_widget = popup_tr_corner;
      else if(!strcmp(widget_name, "popup_bl_corner"))   current_widget = popup_bl_corner;
      else if(!strcmp(widget_name, "popup_br_corner"))   current_widget = popup_br_corner;
      else if(!strcmp(widget_name, "popup_menu_parent")) current_widget = popup_menu_parent;
      else if(!strcmp(widget_name, "tile_popup_t_edge")) { was_tile = 1; current_tile = tile_popup_t_edge; }
      else if(!strcmp(widget_name, "tile_popup_l_edge")) { was_tile = 1; current_tile = tile_popup_l_edge; }
      else if(!strcmp(widget_name, "tile_popup_b_edge")) { was_tile = 1; current_tile = tile_popup_b_edge; }
      else if(!strcmp(widget_name, "tile_popup_r_edge")) { was_tile = 1; current_tile = tile_popup_r_edge; }
      else if(!strcmp(widget_name, "tile_popup_parent")) { was_tile = 1; current_tile = tile_popup_parent; }
      else goto name_error;
    }
    
    if(strstr(widget_name, "tile_")) {
      tiles[current_tile].exists = 1;
      tiles[current_tile].x = x;
      tiles[current_tile].y = y;
      tiles[current_tile].w = w;
      tiles[current_tile].h = h;
    }
    else {
      if(strstr(widget_name, "_hotspot")) themes[current_widget].exists = -1;
      else themes[current_widget].exists = 1;
      themes[current_widget].x = x;
      themes[current_widget].y = y;
      themes[current_widget].w = w;
      themes[current_widget].h = h;
    }
    continue;
    name_error:
    fprintf(stderr, "Error loading theme - widget name \"%s\" not recognized\n", widget_name);        
    goto error;
  }
  fclose(regions); regions = NULL;
  
  /* Load the different state image files */
  theme_images[normal]  
  = cairo_image_surface_create_from_png(strnadd(filename, type, "_normal.png", WIDGET_NAME_SIZE));
  theme_images[active]  
  = cairo_image_surface_create_from_png(strnadd(filename, type, "_active.png", WIDGET_NAME_SIZE));
  theme_images[normal_hover] 
  = cairo_image_surface_create_from_png(strnadd(filename, type, "_normal_hover.png", WIDGET_NAME_SIZE));
  theme_images[active_hover] 
  = cairo_image_surface_create_from_png(strnadd(filename, type, "_active_hover.png", WIDGET_NAME_SIZE));
  theme_images[normal_focussed] 
  = cairo_image_surface_create_from_png(strnadd(filename, type, "_normal_focussed.png", WIDGET_NAME_SIZE));
  theme_images[active_focussed] 
  = cairo_image_surface_create_from_png(strnadd(filename, type, "_active_focussed.png", WIDGET_NAME_SIZE));
  theme_images[normal_focussed_hover] 
  = cairo_image_surface_create_from_png(strnadd(filename, type, "_normal_focussed_hover.png", WIDGET_NAME_SIZE));
  theme_images[active_focussed_hover] 
  = cairo_image_surface_create_from_png(strnadd(filename, type, "_active_focussed_hover.png", WIDGET_NAME_SIZE));
  theme_images[inactive] 
  = cairo_image_surface_create_from_png(strnadd(filename, type, "_inactive.png", WIDGET_NAME_SIZE));

  //TODO perhaps check to make sure that they have the same dimensions
  for(int i = 0; i <= inactive; i++) {
    if(cairo_surface_status(theme_images[i]) == CAIRO_STATUS_FILE_NOT_FOUND) {
//      if(i != normal  &&  i != active  &&  i != inactive) {  /* Focussed variations are optional */
        cairo_surface_destroy(theme_images[i]);
        theme_images[i] = NULL;
//      }
//      else {
        fprintf(stderr, "Warning:  Image file for theme component %s - %d\n not found\n", type, i);
        //goto error;
  //    }
    }
    else 
    if(cairo_surface_status(theme_images[i]) == CAIRO_STATUS_NO_MEMORY ) {
      fprintf(stderr, "Error: surface %d no memory\n", i);
      goto error;
    }
    else 
    if(cairo_surface_status(theme_images[i]) == CAIRO_STATUS_READ_ERROR) {
      fprintf(stderr, "Error: surface %d read error\n", i);
      goto error;
    }
  }

  //backup the position and size of the widget before replacing with the corresponding tiles region
  swap_tiled_widget_themes(type, themes, tiles);

  
  for(unsigned int i = 0; i < nwidgets; i++) {
    create_widget_theme_pixmap(display, &themes[i], theme_images);
  }

  //save the widgets region data and cleanup the tiled background region data as it isn't required anymore.
  swap_tiled_widget_themes(type, themes, tiles);

  //destroy the images of the whole frame sine they've been copied into images for individual widgets.
  for(int i = 0; i <= inactive; i++) if(theme_images[i]) cairo_surface_destroy(theme_images[i]);

  free(tiles);
  free(filename);

  return themes;

  error:
  if(tiles)       free(tiles);
  if(regions)     fclose(regions);  
  if(filename)    free(filename);
  if(themes)      free(themes);
  for(int i = 0; i <= inactive; i++) if(theme_images[i]) cairo_surface_destroy(theme_images[i]);
  return NULL;
}

/**
@brief    This loads the various font settings that are used by functions that draw the text eventually it will load these from a file.
@return   void
**/
static void 
create_font_themes(struct Themes *restrict themes) {
  struct Font_theme font_theme = { .font_name = "Sans", .size = 13.5, .r = 1, .g = 1, .b = 1, .a = 1
  , .x = 3, .y = 15, .slant = CAIRO_FONT_SLANT_NORMAL, .weight = CAIRO_FONT_WEIGHT_NORMAL };

  for(int i = 0; i <= inactive; i++) {
    memcpy(&themes->font_theme[i], &font_theme, sizeof(struct Font_theme));
  }

  themes->font_theme[active].weight = CAIRO_FONT_WEIGHT_BOLD;
  themes->font_theme[active_hover].weight = CAIRO_FONT_WEIGHT_BOLD;
  themes->font_theme[inactive].r = 0.17;
  themes->font_theme[inactive].g = 0.17;
  themes->font_theme[inactive].b = 0.17;

}

/**
@brief    This copies all the details about the widget theme, but not the pixmaps. 
          This is so that the region of tile can be used to create the pixmaps for a widget that itself has a different region 
@return   void
**/
static void 
swap_widget_theme(struct Widget_theme *from, struct Widget_theme *to) {
  struct Widget_theme original_region;
  /*Problem, this also copies the pixmaps.*/
  if(from->exists &&  to->exists) {
    original_region = *to;
    to->x = from->x;
    to->y = from->y;
    to->w = from->w;
    to->h = from->h;
    from->x = original_region.x;
    from->y = original_region.y;
    from->w = original_region.w;
    from->h = original_region.h;
  }
  else fprintf(stderr, "Warning: background tile missing during theme load.\n");
}

/**
@brief    This function is used just before and after creating the the pixmaps for the widgets
          because some widgets need a tiled image (or image subsection) that isn't
          the same region that the widget itself will be on.  So the region the widget
          will be at and the image itself need to be temporarily swapped.
@return   void
**/
static void 
swap_tiled_widget_themes(char *type, struct Widget_theme *themes, struct Widget_theme *tiles) {
  if(strstr(type, "frame")) {
    swap_widget_theme(&tiles[tile_titlebar], &themes[titlebar]);
    swap_widget_theme(&tiles[tile_t_edge], &themes[t_edge]);
    swap_widget_theme(&tiles[tile_r_edge], &themes[r_edge]);
    swap_widget_theme(&tiles[tile_b_edge], &themes[b_edge]);
    swap_widget_theme(&tiles[tile_l_edge], &themes[l_edge]);
    swap_widget_theme(&tiles[tile_title_menu_text], &themes[title_menu_text]);
    swap_widget_theme(&tiles[tile_frame_parent],    &themes[frame_parent]);
  }
  else if(strstr(type, "menubar")) {
    swap_widget_theme(&tiles[tile_menubar_parent], &themes[menubar_parent]);
  }
  else if(!strcmp(type, "popup_menu")) {
    swap_widget_theme(&tiles[tile_popup_t_edge], &themes[popup_t_edge]);
    swap_widget_theme(&tiles[tile_popup_l_edge], &themes[popup_l_edge]);
    swap_widget_theme(&tiles[tile_popup_b_edge], &themes[popup_b_edge]);
    swap_widget_theme(&tiles[tile_popup_r_edge], &themes[popup_r_edge]);
    swap_widget_theme(&tiles[tile_popup_parent], &themes[popup_menu_parent]);
  }
}


/**
@brief    This function looks at the co-ordinates in the widget_theme and uses them to generate the backgrounds from the theme images and saves them onto multiple state windows
@pre      widget theme has x,y,w,h set correctly
@return   void
**/
static void 
create_widget_theme_pixmap(Display *display,  struct Widget_theme *widget_theme, cairo_surface_t **theme_images) {
  Window root = DefaultRootWindow(display); 
  int screen_number = DefaultScreen (display);
  Visual *colours   = DefaultVisual(display, screen_number);

  cairo_surface_t *surface;
  cairo_t *cr;

  int x = widget_theme->x;
  int y = widget_theme->y;
  int w = widget_theme->w;
  int h = widget_theme->h;

  int surface_width = cairo_image_surface_get_width(theme_images[0]);
  int surface_height = cairo_image_surface_get_height(theme_images[0]);

  if(widget_theme->exists <= 0) return; //the exists variable is -1 for hotspots
  //TODO get the size of the window from somewhere else
  if(x < 0)  x +=  surface_width;
  if(y < 0)  y +=  surface_height; 
  if(w <= 0) w +=  surface_width;
  if(h <= 0) h +=  surface_height;

  if(x < 0
  || y < 0
  || w <= 0
  || h <= 0
  || w > surface_width
  || h > surface_height) return;

  for(int i = 0; i <= inactive; i++) if(theme_images[i]) {
    widget_theme->state_p[i] = XCreatePixmap(display, root, w, h, XDefaultDepth(display, screen_number));
    surface = cairo_xlib_surface_create(display, widget_theme->state_p[i], colours, w, h);
    cr = cairo_create(surface);

    //paint a default background
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
    cairo_rectangle(cr, 0, 0, w, h);
    cairo_fill(cr);

    //paint the section of the image onto the widget pixmap
    cairo_set_source_surface(cr, theme_images[i], -x, -y);
    cairo_rectangle(cr, 0, 0, w, h);
    cairo_fill(cr);
    cairo_surface_flush(surface);
    cairo_destroy (cr);  
    cairo_surface_destroy(surface);
  }
}


/**
@brief    This function frees the pixmaps in an array of widget_theme s.  length is the number of elements in the array. 
@return   void
**/
static void 
remove_widget_themes (Display *display, struct Widget_theme *themes, int length) {

  if(themes != NULL) {
    for(int i = 0; i < length; i++)
    if(themes[i].exists) {
      for(int j = 0; j <= inactive; j++) if(themes[i].state_p[j]) XFreePixmap(display, themes[i].state_p[j]);
    }
    free(themes);
  }
}


/**
@brief    This is dependent on struct Themes, so if that changes make corresponding updates here 
@return   void
**/
void 
remove_themes(Display *display, struct Themes *themes) {

  for(int i = 0; i <= system_program; i++)  remove_widget_themes(display, themes->window_type[i], frame_parent + 1);
  remove_widget_themes(display, themes->popup_menu, popup_menu_parent + 1);

  remove_widget_themes(display, themes->menubar, menubar_parent + 1);
  free(themes);
  XFlush(display);
}


/**
@pre      Display is open, font_theme is not NULL, background_p is valid
@post     pixmap using supplied background with the text in the specified style set as background to the window.
@brief    this function uses the supplied pixmap as the background and draw the supplied text over it.
@note     this doesn't use the themes struct so that the caller can more easily specify which pixmap to use
@return   void
**/
void 
create_text_background(Display *display, Window window, const char *restrict text
, const struct Font_theme *restrict font_theme, Pixmap background_p, int b_w, int b_h) {

  Pixmap pixmap = create_text_background_pixmap(display, text, font_theme, background_p, b_w, b_h);

  if(!pixmap || pixmap == BadPixmap) return;

  XUnmapWindow(display, window);
  XSetWindowBackgroundPixmap(display, window, pixmap);
  XSync(display, False); 
  XMapWindow(display, window);
  XFreePixmap(display, pixmap);
  XFlush(display);

}


/**
@brief    This function attempts to put the ICCCM (not the newer EWMH w/ alpha channel) icon onto a background.  It is currently broken.
@return   void
**/
void 
create_icon_background(Display *display, Window window
, Pixmap icon_p, Pixmap icon_mask_p, int b_w, int b_h) {

  Window root       = DefaultRootWindow(display); 
  int screen_number = DefaultScreen(display);
  Screen *screen    = DefaultScreenOfDisplay(display);
  Visual *colours   = DefaultVisual(display, screen_number);

  unsigned int width = XWidthOfScreen(screen);
  unsigned int height = XHeightOfScreen(screen);

  cairo_surface_t *background;
  cairo_pattern_t *background_pattern;
  
  Pixmap pixmap = XCreatePixmap(display, root, width, height, XDefaultDepth(display, screen_number));
  cairo_surface_t *surface = cairo_xlib_surface_create(display, pixmap, colours,  width, height);
  cairo_surface_t *mask =  cairo_xlib_surface_create_for_bitmap(display, icon_mask_p, screen, b_w, b_h);
  cairo_t *cr = cairo_create(surface);

  /*Draw a backup background */
  cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 1);
  cairo_rectangle(cr, 0, 0, width, height);
  cairo_fill(cr);

  /* Draw the background first
  background = cairo_xlib_surface_create(display, background_p, colours, b_w, b_h);
  background_pattern = cairo_pattern_create_for_surface(background);
  cairo_pattern_set_extend(background_pattern, CAIRO_EXTEND_REPEAT);
  cairo_set_source(cr, background_pattern);
  cairo_rectangle(cr, 0, 0, width, height);
  cairo_fill(cr);
  cairo_surface_flush(surface);
  cairo_surface_destroy(background);
  cairo_pattern_destroy(background_pattern);
  */
  
  //so the surface is the pixmap, we create a pattern based on the surface and then set that as the source for the renderer
  background = cairo_xlib_surface_create(display, icon_p, colours, b_w, b_h);
  background_pattern = cairo_pattern_create_for_surface(background);
  cairo_pattern_set_extend(background_pattern, CAIRO_EXTEND_NONE);
  cairo_set_source(cr, background_pattern);
  //need to mask it first
                                                           

  /*mask draws the current surface */
  //cairo_mask_surface (cr, mask, 0, 0);
  
  cairo_rectangle(cr, 0, 0, width, height);
  cairo_fill(cr);
  cairo_surface_flush(surface);
  cairo_surface_destroy(mask);
  cairo_surface_destroy(background);
  cairo_pattern_destroy(background_pattern);
      
  /********** ***********/
  cairo_destroy (cr);  
  cairo_surface_destroy(surface);    

  XUnmapWindow(display, window);
  XSetWindowBackgroundPixmap(display, window, pixmap);
  XSync(display, False); 
  XMapWindow(display, window);
  XFreePixmap(display, pixmap);
  XFlush(display);
}


/**
@brief    This function combines text and a background into a pixmap. If the text is Tiling, Floating, Desktop or Hidden, it also draws an icon.
@param    text          a null terminated UTF8 string
@param    background_p  the background pixmap
@param    b_w           the width of the background
@param    b_h           the height of the background
@return   The resulting pixmap.
**/
static Pixmap
create_text_background_pixmap(Display *display, const char *restrict text
, const struct Font_theme *restrict font_theme, Pixmap background_p, int b_w, int b_h) {
  Window root = DefaultRootWindow(display); 
  int screen_number = DefaultScreen (display);
  Screen* screen = DefaultScreenOfDisplay(display);
  Visual *colours =  DefaultVisual(display, screen_number);

  if(b_w <= 0) return 0;
  if(b_h <= 0) return 0;
  if(!background_p || !font_theme) return 0;
  //printf("Creating text pixmap %s, b_w %d b_h %d\n", text, b_w, b_h);

  unsigned int width = XWidthOfScreen(screen);
  unsigned int height = b_h;  
  Pixmap pixmap = XCreatePixmap(display, root, width, height, XDefaultDepth(display, screen_number));
  cairo_surface_t *surface = cairo_xlib_surface_create(display, pixmap, colours,  width, height);
  cairo_surface_t *background = cairo_xlib_surface_create(display, background_p, colours, b_w, b_h);
  cairo_t *cr = cairo_create(surface);

  cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 1);
  cairo_rectangle(cr, 0, 0, width, height);
  cairo_fill(cr);

  cairo_pattern_t *background_pattern = cairo_pattern_create_for_surface(background);
  cairo_pattern_set_extend(background_pattern, CAIRO_EXTEND_REPEAT);
  cairo_set_source(cr, background_pattern);
  cairo_rectangle(cr, 0, 0, width, height);
  cairo_fill(cr);
  cairo_surface_flush(surface);
  cairo_surface_destroy(background);
  cairo_pattern_destroy(background_pattern);
  
  cairo_select_font_face(cr, font_theme->font_name, font_theme->slant, font_theme->weight);
  cairo_set_font_size(cr, font_theme->size);
  cairo_set_source_rgba(cr, font_theme->r, font_theme->g, font_theme->b, font_theme->a);
  
  if(text && (!strcmp(text, "Floating") || !strcmp(text, "Tiling") ||  !strcmp(text, "Desktop") || !strcmp(text, "Hidden"))) {
    cairo_move_to(cr, font_theme->x + MODE_ICON_SIZE, font_theme->y);
    if(!strcmp(text, "Floating")) {
      cairo_show_text(cr, "Floating");
      
      cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD); //means don't fill areas that are filled twice.
      cairo_rectangle(cr, 4 - 1, 4, 9, 9);
      cairo_rectangle(cr, 5 - 1, 6, 7, 6);
      
      cairo_rectangle(cr, 8 - 1, 8, 5, 5); //cut off the corner for the 2nd window icon
      cairo_rectangle(cr, 8 - 1, 8, 4, 4);
      cairo_fill(cr);

      cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
      cairo_rectangle(cr, 8 - 1, 8, 9, 9);
      cairo_rectangle(cr, 9 - 1, 10, 7, 6);
      cairo_fill(cr); 
    }
    else
    if(!strcmp(text, "Tiling")) {
      cairo_show_text(cr, "Tiling");

      cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
      cairo_rectangle(cr, 5- 1, 7, 4, 7);
      cairo_rectangle(cr, 4- 1, 5, 6, 10);
      cairo_fill(cr);

      cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
      cairo_rectangle(cr, 12- 1, 7, 4, 7);
      cairo_rectangle(cr, 11- 1, 5, 6, 10);
      cairo_fill(cr);    
    }
    else 
    if(!strcmp(text, "Desktop")) {
      cairo_show_text(cr, "Desktop");

      cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD); //means don't fill areas that are filled twice.
      cairo_rectangle(cr, 4- 1, 4, 11, 11);
      cairo_rectangle(cr, 5- 1, 5, 9, 9);
      cairo_fill(cr); 
    }
    else 
    if(!strcmp(text, "Hidden")) {
      cairo_show_text(cr, "Hidden");
      cairo_rectangle(cr, 4 - 1, 12, 8, 2);
      cairo_fill(cr);
    }
  }
  else 
  if(text) {
    cairo_move_to(cr, font_theme->x, font_theme->y);
    cairo_show_text(cr, text);
  }
  cairo_destroy (cr);  
  cairo_surface_destroy(surface);
  XFlush(display);
  return pixmap;
}

/** 
@brief    This function calculates the width of a title when drawn using the specified font theme in pixels. It is used to calculate popup menu widths.
          It never returns a length larger than the width of the screen. 
@return   the number of pixels the text takes up on the screen.
**/
unsigned int 
get_text_width(Display* display, const char *title, struct Font_theme *font_theme) {
  int screen_number = DefaultScreen (display);
  Screen* screen = DefaultScreenOfDisplay(display);
  Visual *colours =  DefaultVisual(display, screen_number);
  Window temp;
  cairo_surface_t *surface;
  cairo_t *cr;
  cairo_text_extents_t extents;
  int width = 0;
  
  temp = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, PIXMAP_SIZE, PIXMAP_SIZE, 0
  , WhitePixelOfScreen(screen), BlackPixelOfScreen(screen));
  surface = cairo_xlib_surface_create(display, temp, colours,  PIXMAP_SIZE, PIXMAP_SIZE);
  cr = cairo_create(surface);

  cairo_select_font_face (cr, font_theme->font_name, font_theme->slant, font_theme->weight);
  cairo_set_font_size(cr, font_theme->size); 
  cairo_text_extents(cr, title, &extents);

  XDestroyWindow(display, temp); 
  XFlush(display);
  cairo_destroy (cr);  
  cairo_surface_destroy(surface);

  if(extents.x_bearing < 0) extents.x_advance -=  extents.x_bearing;

  width = extents.x_advance + font_theme->x;
  if(width > XWidthOfScreen(screen)) width = XWidthOfScreen(screen);
  
  return (unsigned int)width;
}


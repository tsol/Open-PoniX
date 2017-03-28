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
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>

#include "lunchbox.h"
#include "space.h"


/**
@file     space.c
@brief    Functions for identifying and manipulating free space rectangles.
@author   Alysander Stanley
**/

/**
@brief    Finds the available free spaces on the screen as a Rectangle_list.
@return   A rectangle_list structure.

@param    display  The currently open X11 connection.
@param    frames   All the currently open frames - used to establish the currently occupied spaces.
@param    themes   the current theme.
@param    only_panels  Only consider tiling panels and not other tiling windows if True, consider all tiling windows including panels if False.

@pre      All tiled windows are not overlapping.
**/
struct Rectangle_list 
get_free_screen_spaces (Display *display, Bool only_panels, struct Workspace *frames, struct Themes *themes) {
  //the start variable is used to skip windows at the start 
  //in the handle_frame_retile function these are the intersecting tiled windows.
  
  struct Rectangle_list used_spaces = {.used = 0, .max = 8, .list = NULL};
  struct Rectangle_list free_spaces = {.used = 0, .max = 8, .list = NULL};
  
  Screen* screen = DefaultScreenOfDisplay(display);
  
  used_spaces.list = malloc(used_spaces.max * sizeof(struct Rectangle));
  if(used_spaces.list == NULL) {
    //probably could do something more intelligent here
    perror("Error: out of memory for calculating free spaces on the screen.\n");
    return used_spaces; //i.e., NULL
  }
  
  //ideally we would be certain that all tiled windows are not overlapping.  However, this is hard so we have to double check.
  for(int i = 0; i < frames->used; i++) {
    if( ((only_panels == False) || (only_panels  &&  frames->list[i]->type == panel))  &&  frames->list[i]->mode == tiling  &&  frames->list[i]->state == none) {
      struct Rectangle current = {.x = frames->list[i]->x, .y = frames->list[i]->y, .w = frames->list[i]->w, .h = frames->list[i]->h};

      #ifdef SHOW_FREE_SPACE_STEPS   
      printf("Tiled window %s, x %d, y %d, w %d, h %d\n", frames->list[i]->window_name
      , frames->list[i]->x, frames->list[i]->y, frames->list[i]->w, frames->list[i]->h);
      #endif
      for(unsigned int j = 0; j < used_spaces.used; j++) {
        //printf("%d, %d, %d, %d vs %d, %d, %d, %d\n", current.x, current.y, current.w, current.h,  used_spaces.list[j].x, used_spaces.list[j].y, used_spaces.list[j].w, used_spaces.list[j].h);
        if(INTERSECTS(current.x, current.w, used_spaces.list[j].x, used_spaces.list[j].w)
        && INTERSECTS(current.y, current.h, used_spaces.list[j].y, used_spaces.list[j].h)) {
          
          //This should never occur
          //Enforce precondition of largest_available spaces algorithm.
          #ifdef SHOW_FREE_SPACE_STEPS   
          printf("intersects the current window");
          #endif        
          free(used_spaces.list);
          used_spaces.list = NULL;
          used_spaces.used = 0;
          return used_spaces;
        }
      }
      add_rectangle(&used_spaces, current);
    }
  }
  #ifdef SHOW_FREE_SPACE_STEPS   
  printf("Finding free spaces. Width %d, Height %d\n", XWidthOfScreen(screen), XHeightOfScreen(screen) - themes->menubar[menubar_parent].h);
  #endif
  free_spaces = largest_available_spaces(&used_spaces, XWidthOfScreen(screen), XHeightOfScreen(screen) - themes->menubar[menubar_parent].h);
  return free_spaces;
}

/**
@brief    This implements "Free space modeling for placing rectangles without overlapping" 
          by Marc Bernard and Francois Jacquenet. 
@return   returns a Rectangle_list with all largest free spaces
**/
struct Rectangle_list 
largest_available_spaces (struct Rectangle_list *used_spaces, int w, int h) {

  struct Rectangle_list free_spaces  = {.used = 0, .max = 8, .list = NULL};
  struct Rectangle_list new_spaces   = {.used = 0, .max = 8, .list = NULL};
  struct Rectangle_list old_spaces   = {.used = 0, .max = 8, .list = NULL};  
  struct Rectangle screen_space      = {.x = 0, .y = 0, .w = w, .h = h};
  
  if(used_spaces->list == NULL) return free_spaces;
  free_spaces.list = malloc(free_spaces.max * sizeof(struct Rectangle));  
  if(free_spaces.list == NULL) {
    free(used_spaces->list);
    //probably could do something more intelligent here
    perror("Error: out of memory for calculating free spaces on the screen.\n");
    return free_spaces;
  }
  
  add_rectangle(&free_spaces, screen_space ); //define the intitial space.
  
  //for all used spaces (already tiled windows on the screen)
  for(unsigned int i = 0; i < used_spaces->used; i++) {  
    unsigned int j;
    new_spaces.list = malloc(new_spaces.max * sizeof(struct Rectangle));
    old_spaces.list = malloc(old_spaces.max * sizeof(struct Rectangle));

    if(new_spaces.list == NULL  ||  old_spaces.list == NULL) {
      if(new_spaces.list  != NULL) free(new_spaces.list);
      if(old_spaces.list  != NULL) free(old_spaces.list);
      free(free_spaces.list);
      free(used_spaces->list);
      used_spaces->list = NULL;
      //probably could do something more intelligent here
      perror("Error: out of memory for calculating free spaces on the screen.\n");
      return free_spaces;
    }
    #ifdef SHOW_FREE_SPACE_STEPS     
    printf("\nPlacing next rectangle\n");    
    #endif
    for(j = 0; j < free_spaces.used; j++) {
      struct Rectangle new = {0, 0, 0, 0};
      #ifdef SHOW_FREE_SPACE_STEPS
      printf("j %d, free_spaces.used %d, free_spaces.max %d\n", j, free_spaces.used, free_spaces.max);
      #endif

      //if an edge intersects, modify opposing edge.  add modified to new_spaces and original to old_spaces
      if(INTERSECTS_OUTSIDE(free_spaces.list[j].y, free_spaces.list[j].h, used_spaces->list[i].y, used_spaces->list[i].h)
      && INTERSECTS_OUTSIDE(free_spaces.list[j].x, free_spaces.list[j].w, used_spaces->list[i].x, used_spaces->list[i].w) ) {
        #ifdef SHOW_FREE_SPACE_STEPS
        printf("Complete occlusion in both axis");
        #endif
        add_rectangle(&old_spaces, free_spaces.list[j]);
        continue;
      }
      
      if(INTERSECTS(free_spaces.list[j].y, free_spaces.list[j].h, used_spaces->list[i].y, used_spaces->list[i].h) ) {
        #ifdef SHOW_FREE_SPACE_STEPS
        //printf("i %d intersects j %d in y\n", i, j);
        #endif
        //All left reduced rectangles which are modified by the right edge of the placed rectangle        
        if(INTERSECTS_WITHIN(free_spaces.list[j].x, free_spaces.list[j].w, used_spaces->list[i].x, used_spaces->list[i].w)) {
          #ifdef SHOW_FREE_SPACE_STEPS
          printf("LHS and RHS of free space modified\n");
          #endif
          new = free_spaces.list[j];
          new.w = used_spaces->list[i].x - new.x;  
          add_rectangle(&new_spaces, new);
          new = free_spaces.list[j];
          new.w += new.x;
          new.x = used_spaces->list[i].x + used_spaces->list[i].w;
          new.w -= new.x;
          add_rectangle(&new_spaces, new);
          add_rectangle(&old_spaces, free_spaces.list[j]);
        }
        else if(INTERSECTS_BEFORE(free_spaces.list[j].x, free_spaces.list[j].w, used_spaces->list[i].x, used_spaces->list[i].w) ) {
          //the free space comes after the placed rectangle, modify it's LHS
          #ifdef SHOW_FREE_SPACE_STEPS         
          printf("RHS of free space modified\n");
          #endif
          new = free_spaces.list[j];
          new.w = used_spaces->list[i].x - new.x;          
          add_rectangle(&new_spaces, new);
          add_rectangle(&old_spaces, free_spaces.list[j]);
        }

        //All right reduced rectangles which are modified by the left edge of the placed rectangle    
        else if(INTERSECTS_AFTER(free_spaces.list[j].x, free_spaces.list[j].w, used_spaces->list[i].x, used_spaces->list[i].w) ) {
          //the free space comes before the placed rectangle, modify it's RHS
          #ifdef SHOW_FREE_SPACE_STEPS
          printf("LHS of free space modified\n");
          #endif
          new = free_spaces.list[j];
          new.w += new.x;
          new.x = used_spaces->list[i].x + used_spaces->list[i].w;
          new.w -= new.x;
          add_rectangle(&new_spaces, new);
          add_rectangle(&old_spaces, free_spaces.list[j]);
        }
      }
      if(INTERSECTS(free_spaces.list[j].x, free_spaces.list[j].w, used_spaces->list[i].x, used_spaces->list[i].w) ) {
        #ifdef SHOW_FREE_SPACE_STEPS
        printf("i %d intersects j %d in y\n", i, j);
        #endif
        if(INTERSECTS_WITHIN(free_spaces.list[j].y, free_spaces.list[j].h, used_spaces->list[i].y, used_spaces->list[i].h) ) {
          #ifdef SHOW_FREE_SPACE_STEPS
          printf("TOP and BTM of free space modified\n");
          #endif
          new = free_spaces.list[j];
          new.h = used_spaces->list[i].y - new.y;          
          add_rectangle(&new_spaces, new);
          new = free_spaces.list[j];
          new.h += new.y;
          new.y = used_spaces->list[i].y + used_spaces->list[i].h;
          new.h -= new.y;
          add_rectangle(&new_spaces, new);
          add_rectangle(&old_spaces, free_spaces.list[j]); 
        }
        else if(INTERSECTS_BEFORE(free_spaces.list[j].y, free_spaces.list[j].h, used_spaces->list[i].y, used_spaces->list[i].h) ) {
          //the free space comes after the placed rectangle, modify it's LHS
          #ifdef SHOW_FREE_SPACE_STEPS         
          printf("TOP of free space modified\n");
          #endif
          new = free_spaces.list[j];
          new.h = used_spaces->list[i].y - new.y;          
          add_rectangle(&new_spaces, new);
          add_rectangle(&old_spaces, free_spaces.list[j]); 
        }    
        else if(INTERSECTS_AFTER(free_spaces.list[j].y, free_spaces.list[j].h, used_spaces->list[i].y, used_spaces->list[i].h) ) {
          #ifdef SHOW_FREE_SPACE_STEPS
          printf("BTM of free space modified\n");
          #endif
          new = free_spaces.list[j];
          new.h += new.y;
          new.y = used_spaces->list[i].y + used_spaces->list[i].h;
          new.h -= new.y;
          add_rectangle(&new_spaces, new);
          add_rectangle(&old_spaces, free_spaces.list[j]);
        }
      }
      
      if(new_spaces.list == NULL  ||  old_spaces.list == NULL) {
        #ifdef SHOW_FREE_SPACE_STEPS
        printf("One of the lists was NULL!\n");
        #endif
        break;
      }
    }
    #ifdef SHOW_FREE_SPACE_STEPS     
    printf("Integrating Changes\n");
    #endif

    //remove the spaces which were modified. O(N*M)
    for(unsigned int k = 0; k < old_spaces.used; k++) remove_rectangle(&free_spaces, old_spaces.list[k]);
      
    //add new_spaces to the free spaces O(N*M)
    for(unsigned int k = 0; k < new_spaces.used; k++) add_rectangle(&free_spaces, new_spaces.list[k]);

    if(new_spaces.list == NULL  ||  old_spaces.list == NULL) {
      #ifdef SHOW_FREE_SPACE_STEPS
      printf("An error occured when calculating free space, j %d, free_spaces.used %d\n", j, free_spaces.used);
      #endif
      if(free_spaces.list!= NULL) free(free_spaces.list);
      if(new_spaces.list != NULL) free(new_spaces.list);
      if(old_spaces.list != NULL) free(old_spaces.list);
      free_spaces.list = NULL;
      break;
    }
    else {
      if(new_spaces.list != NULL) free(new_spaces.list);
      if(old_spaces.list != NULL) free(old_spaces.list);
      new_spaces.list = NULL;
      new_spaces.used = 0;
      new_spaces.max  = 8;
      old_spaces.list = NULL;
      old_spaces.used = 0;
      old_spaces.max  = 8;
    }
  }
  if(used_spaces->list != NULL) free(used_spaces->list);
  used_spaces->list = NULL;

  return free_spaces;
}

/**
@brief    Adds rectangles to the list.  Rectangles are checked for validity and independence/inclusion. O(n) 
@return   void
**/
void 
add_rectangle(struct Rectangle_list *list, struct Rectangle new) {

  if(list->list == NULL) {
    #ifdef SHOW_FREE_SPACE_STEPS     
    printf("NULL list was passed to add_rectangle\n");
    #endif
    return;
  }
  
  if(new.h <= 0  ||  new.w <= 0  ||  new.x < 0  ||  new.y < 0) return;
  
  if(list->max == list->used) {
    struct Rectangle *temp = NULL;
    temp = realloc(list->list, sizeof(struct Rectangle) * list->max * 2);
    if(temp != NULL) list->list = temp;
    else {
      list->list = NULL;
      return;
    }
    list->list = temp;
    list->max *= 2;
  }

  //start from the end in case it was just added. Don't add rectangles which are already covered/included

  for(int i = list->used - 1; i >= 0; i--) {
    if(list->list[i].x <= new.x 
    && list->list[i].y <= new.y
    && list->list[i].w + list->list[i].x >= new.w + new.x
    && list->list[i].h + list->list[i].y >= new.h + new.y) {
      #ifdef SHOW_FREE_SPACE_STEPS     
      printf("list %p\n", (void *)list->list);
      printf("adding space: x %d, y %d, w %d, h %d\n", new.x, new.y, new.w, new.h);
      printf("but container %d: x %d, y %d, w %d, h %d\n", i, list->list[i].x, list->list[i].y, list->list[i].w, list->list[i].h);
      #endif
      return;
    }
  }
  #ifdef SHOW_FREE_SPACE_STEPS     
  printf("Have added space: x %d, y %d, w %d, h %d\n", new.x, new.y, new.w, new.h);
  #endif
  list->list[list->used] = new;
  list->used++;
}

/**
@brief    Removes the rectangle from the list if it exists. O(n)
@return   void
**/
void 
remove_rectangle(struct Rectangle_list *list, struct Rectangle old) {
  unsigned int i;
  if(list->list == NULL) {
    #ifdef SHOW_FREE_SPACE_STEPS     
    printf("NULL list was passed to remove_rectangle\n");
    #endif
    return;
  }

  for(i = 0; i < list->used; i++)
  if(list->list[i].x == old.x
  && list->list[i].y == old.y
  && list->list[i].w == old.w
  && list->list[i].h == old.h) {
    //replace the item with the last item and remove the last item
    if((list->used != 1) && (i != list->used - 1)) { //not the first or the last
      list->list[i] = list->list[list->used - 1];    
    }
    #ifdef SHOW_FREE_SPACE_STEPS     
    printf("rem'd space: w %d, h %d\n", old.w, old.h);    
    #endif
    list->used--;
    return;
  }
}

/**
@brief    Calculate the displacement squared in each axis and use pythagoras to calculate the net displacement
@pre      Rectangles a and b must not be overlapping.
@return   -1 if source is larger than dest
**/
double 
calculate_displacement(struct Rectangle source, struct Rectangle dest, int *dx, int *dy) {
  double hypotenuse_squared;
  
  if(source.w > dest.w
  || source.h > dest.h) {
    #ifdef SHOW_FREE_SPACE_STEPS     
    printf("Doesn't fit: source w %d, dest w %d, source h %d, dest h %d\n", source.w, dest.w, source.h, dest.h);
    #endif
    return -1;
  }
   
  if(source.x > dest.x  
  && source.x + source.w < dest.x + dest.w) *dx = 0;
  else *dx = dest.x - source.x;
  
  if(*dx < 0) *dx += dest.w - source.w; //move it to the nearest edge if required.

  if(source.y > dest.y  
  && source.y + source.h < dest.y + dest.h) *dy = 0;
  else *dy = dest.y - source.y;
  
  if(*dy < 0) *dy += dest.h - source.h; //move it to the nearest edge if required.
  
  hypotenuse_squared = (*dx) * (*dx) + (*dy) * (*dy); //major optimization if we don't use the sqrt
 // printf("dest x %d, dest y %d, dx %d, dy %d, hyp %f\n", dest.x, dest.y, *dx, *dy, hypotenuse);

  return hypotenuse_squared;
}


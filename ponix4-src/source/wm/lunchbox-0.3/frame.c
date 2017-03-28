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
#include <stdint.h>
#include <X11/extensions/shape.h>
#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xatom.h>

#include "xcheck.h"
#include "lunchbox.h"
#include "menus.h"
#include "theme.h"
#include "frame.h"
#include "frame-actions.h"



/**
@file     frame.c
@brief    Functions for creating and removing frames. A frame is the interactive border placed around another applications top level windows.
@author   Alysander Stanley
**/

static void 
create_frame_subwindows (Display *display, struct Frame *frame, struct Themes *themes, struct Cursors *cursors);

static void 
get_frame_type_and_mode (Display *display, struct Frame *frame, struct Atoms *atoms, struct Themes *themes);

static void 
get_frame_state         (Display *display, struct Frame *frame, struct Atoms *atoms);

static void 
get_frame_wm_hints      (Display *display, struct Frame *frame);

static void 
frame_type_settings     (Display *display, struct Frame *frame);

static void
save_frame_initial_state(struct Frame *frame);

/** 
@brief    Sometimes a client window is killed before it gets unmapped, we only get the unmapnotify event, but there is no way to tell so we just supress the error.  This is passed to the XSetErrorHandler function;
**/
int 
supress_xerror(Display *display, XErrorEvent *event) {
  (void) display;
  (void) event;
  //printf("Caught an X error\n");
  return 0;
}

/**
@brief  Save the initial state in case the frame will be used in other workspaces and can appear in it's prefered spot.
        
@return void
**/
static void
save_frame_initial_state(struct Frame *frame) {
  frame->initial_state.x = frame->x;
  frame->initial_state.y = frame->y;
  frame->initial_state.w = frame->w;
  frame->initial_state.h = frame->h;
  frame->initial_state.mode = frame->mode;
}

/**
@brief    Reparents the specified framed_window to a newly created frame.
@return   returns 1 if successful or 0 if no window was created.
@todo     A the moment each pointer is totally valid, in the future it will merely be an alias from another datastructure.
**/
int
create_frame(Display *display, struct Frame* frame
, Window framed_window, struct Popup_menu *window_menu, struct Separators *seps, struct Themes *themes
, struct Cursors *cursors, struct Atoms *atoms) {
  Screen* screen = DefaultScreenOfDisplay(display); 
  XWindowAttributes get_attributes;
  
  //printf("Creating frames->list[%d] with window %lu, connection %lu\n"
  //, frames->used, (unsigned long)framed_window, (unsigned long)display);
  //add this window to the save set as soon as possible so that if an error occurs it is still available

  XAddToSaveSet(display, framed_window); 
  XSync(display, False);
  XGetWindowAttributes(display, framed_window, &get_attributes);

  /*** Set up defaults ***/
  frame->focussed = False;
  frame->sticky = False;
  frame->window_name = NULL;
  frame->framed_window = framed_window;
  frame->type = unknown;
  frame->theme_type = unknown;
  frame->mode = unset;
  frame->state = none;
  frame->wants_attention = False;
  frame->transient = 0;
  frame->width_inc = 1;
  frame->height_inc = 1;
  frame->menu.item = 0;

  frame->w_inc_offset = 0;
  frame->h_inc_offset = 0;
  
  frame->w = get_attributes.width; 
  frame->h = get_attributes.height;
 
  get_frame_type_and_mode (display, frame, atoms, themes);

  frame->x = get_attributes.x - themes->window_type[frame->theme_type][window].x;
  frame->y = get_attributes.y - themes->window_type[frame->theme_type][window].y;
  frame->hspace = 0 - themes->window_type[frame->theme_type][window].w;
  frame->vspace = 0 - themes->window_type[frame->theme_type][window].h;
  
  //prevent overly large windows with these sensible defaults
  frame->max_width  = XWidthOfScreen(screen) + frame->hspace; 
  frame->max_height = XHeightOfScreen(screen) + frame->vspace;
  frame->min_width  = MINWIDTH + frame->hspace;;
  frame->min_height = MINHEIGHT + frame->vspace;;

  #ifdef ALLOW_OVERSIZE_WINDOWS_WITHOUT_MINIMUM_HINTS
  Screen* screen = DefaultScreenOfDisplay(display);
  /* Ugh Horrible.  */
  /* Many apps that are resizeable ask to be the size of the screen and since windows
     often don't specifiy their minimum size, we have no other way of knowing if they 
     really need to be that size or not.  In case they do, this specifies that their
     current width is their minimum size, in the hope that it is overridden by the
     size hints. This kind of behaviour causes problems on small screens like the
     eee pc. */
  if(frame->w > XWidthOfScreen(screen))  frame->min_width = frame->w;
  if(frame->h > XHeightOfScreen(screen)) frame->min_height = frame->h;
  #endif

  /* This requires hspace and vspace to be set as well as the incremental hints */
  get_frame_hints(display, frame);

  frame_type_settings(display, frame);
  
  //Don't manage splash screens, they just cause the workspace to be created and instantly destroyed
  if(frame->type == splash) {
    XMapWindow(display, framed_window);
    XFlush(display);
    return 0;
  }


  get_frame_state(display, frame, atoms);
  create_frame_subwindows(display, frame, themes, cursors);
  create_frame_name(display, window_menu, frame, themes, atoms);

  get_frame_wm_hints(display, frame);  //this might need to change the focus, it's mode (to hidden) and so on
  get_frame_strut_hints_as_normal_hints(display, frame, atoms);
  
  //_NET_FRAME_EXTENTS, left, right, top, bottom, CARDINAL[4]/32
  int32_t ewmh_frame_extents[4] = { themes->window_type[frame->theme_type][window].x
  , themes->window_type[frame->theme_type][window].y
  , - themes->window_type[frame->theme_type][window].x - themes->window_type[frame->theme_type][window].w
  , - themes->window_type[frame->theme_type][window].y - themes->window_type[frame->theme_type][window].h
  };
  
  XChangeProperty(display, framed_window, atoms->frame_extents, XA_CARDINAL
  , 32, PropModeReplace, (unsigned char *)ewmh_frame_extents, 4);
  
  XSetWindowBorderWidth(display, framed_window, 0);

  change_frame_mode(display, frame, unset, themes);
  
  #ifdef CRASH_ON_BUG
  XGrabServer(display);
  XSetErrorHandler(supress_xerror);
  #endif
  
  XSelectInput(display, framed_window,  0);

  //reparent the framed_window to frame->widgets[window].widget
  XReparentWindow(display, framed_window, frame->widgets[window].widget, 0, 0);
  //for some odd reason the reparent only reports an extra unmap event if the window was already unmapped
  XRaiseWindow(display, framed_window);
  XMapWindow(display, frame->widgets[window].widget);
  
  #ifdef CRASH_ON_BUG
  XSetErrorHandler(NULL);
  XUngrabServer(display);
  #endif

  XSelectInput(display, framed_window,  PropertyChangeMask); //Property notify is used to update titles
  XSelectInput(display, frame->widgets[window].widget
  , SubstructureRedirectMask | SubstructureNotifyMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
  
  //Some windows only send the destroy event (e.g., gimp splash screen)
  XSync(display, False);  
  
  //Intercept clicks so we can set the focus and possibly raise floating windows
  XGrabButton(display, Button1, 0, frame->widgets[window].widget
  , False, ButtonPressMask, GrabModeSync, GrabModeAsync, None, None);
  
  //do it for numlock as well
  XGrabButton(display, Button1, Mod2Mask, frame->widgets[window].widget
  , False, ButtonPressMask, GrabModeSync, GrabModeAsync, None, None);
  
  frame->w += frame->hspace;
  frame->h += frame->vspace;
  
  check_frame_limits(display, frame, themes);
  
  resize_frame(display, frame, themes);
  stack_frame(display, frame, seps);
  change_frame_state(display, frame, frame->state, seps, themes, atoms);
  XMoveResizeWindow(display, framed_window, 0, 0, frame->w - frame->hspace, frame->h - frame->vspace);  
  XMoveWindow(display, framed_window, 0, 0);
  XMapWindow(display, framed_window);
  
  XFlush(display);
  save_frame_initial_state(frame);  
  return 1;
}

/**
@brief   This function reparents a framed window to root and then destroys the frame. It is used when the framed window has been unmapped or destroyed, or is about to be.
@param   workspaces list of all workspaces.
@param   index  index of the target frame in the global list of frames
@return  void
**/
void 
remove_frame(Display* display, struct Workspace_list *workspaces, int index, int current_workspace, struct Atoms *atoms, struct Themes *themes) {

  struct Frame *frame = &workspaces->frame_list[index];

  XWindowChanges changes;
  Window root = DefaultRootWindow(display);
  unsigned int mask = CWSibling | CWStackMode;  

  changes.stack_mode = Below;
  changes.sibling = frame->widgets[frame_parent].widget;
  
  free_frame_name(frame);
  XDestroyWindow(display, frame->menu.item);  
  //make the whole window disappear before the inner frame is unmapped to make it look faster
  XUnmapWindow(display, frame->widgets[frame_parent].widget);
  XSync(display, False); 
  #ifdef CRASH_ON_BUG
  XGrabServer(display);
  XSetErrorHandler(supress_xerror);  
  #endif
  
  XDeleteProperty(display, frame->framed_window, atoms->wm_state);
  frame->state = none;
  XReparentWindow(display, frame->framed_window, root
  , frame->x + themes->window_type[frame->theme_type][window].x
  , frame->y + themes->window_type[frame->theme_type][window].y);
  //TODO need to change the frame w,h attributes here
  
  //keep the stacking order
  XConfigureWindow(display, frame->framed_window, mask, &changes);  
  
  XRemoveFromSaveSet(display, frame->framed_window); 
  
  //this will not destroy the window because it has been reparented to root
  XDestroyWindow(display, frame->widgets[frame_parent].widget);
  XSync(display, False);
  
  #ifdef CRASH_ON_BUG
  XSetErrorHandler(NULL);    
  XUngrabServer(display);
  #endif
 
 
  //we want to unconditionally remove references to the frame which is being removed in the current workspace
  if(current_workspace >= 0  &&  workspaces->list[current_workspace].list) {
    for(int i = 0; i < workspaces->list[current_workspace].used; i++) {
      if(workspaces->list[current_workspace].list[i]->framed_window == frame->framed_window) {
        int nested_last = workspaces->list[current_workspace].used - 1;
        if((workspaces->list[current_workspace].used != 1)  && (i != nested_last) ) {
          workspaces->list[current_workspace].list[i] = workspaces->list[current_workspace].list[nested_last];
        }
        workspaces->list[current_workspace].used--;
        break;
      }
    }
  }

  //make sure that the saved states are reset since it is assumed than an empty slot is zeroed by clean realloc etc.
  struct Saved_frame_state blank = { .x =0 , .y = 0 , .w = 0, .h = 0,
  .available = 0, .need_to_tile = 0, .mode = 0, .state = 0 };

  for(int i = 0; i < workspaces->used_workspaces; i++) {
    workspaces->list[i].states[index] = blank;
  }
  
  //remove it from the global frame list, and also remove it's state references.
  int last = workspaces->used_frames - 1;
  if((workspaces->used_frames != 1) && (index != last)) { //the frame is not alone or the last
    struct Frame* last_fp_pointer = &workspaces->frame_list[last];
    
    for(int i = 0; i < workspaces->used_workspaces; i++) {
      workspaces->list[i].states[index] = workspaces->list[i].states[last];
      workspaces->list[i].states[last] = blank;
    }
    
    if(current_workspace >= 0  &&  workspaces->list[current_workspace].list) {
      for(int i = 0; i < workspaces->list[current_workspace].used; i++) {
        //in anticipation of the swap of the last frame we need to update pointers to it.
        //imagine that the removed frame wasn't in the current workspace (in fact the above code guarantees it) but the moved frame was, it's pointer would become invalid.
        if(workspaces->list[current_workspace].list[i] == last_fp_pointer) workspaces->list[current_workspace].list[i] = &workspaces->frame_list[index];
      }
    }
    workspaces->frame_list[index] = workspaces->frame_list[last]; //swap the deleted item with the last item.    
  }
  
  workspaces->used_frames--;
}


/**
@brief   This function is called when the close button on the frame is pressed.
@return  void
**/
void
close_window(Display* display, Window framed_window) {
  int n;
  Bool found = False;
  Atom *protocols;

  //based on windowlab/aewm
  if (XGetWMProtocols(display, framed_window, &protocols, &n)) {
    Atom delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    for (int i = 0; i < n; i++)
      if (protocols[i] == delete_window) {
        found = True;
        break;
      }
    XFree(protocols);
  }

  if(found)  {
   //from windowlab/aewm
    XClientMessageEvent event;
    event.type = ClientMessage;
    event.window = framed_window;
    event.format = 32;
    event.message_type = XInternAtom(display, "WM_PROTOCOLS", False);
    event.data.l[0] = (long)XInternAtom(display, "WM_DELETE_WINDOW", False);
    event.data.l[1] = CurrentTime;
    XSendEvent(display, framed_window, False, NoEventMask, (XEvent *)&event);
    #ifdef SHOW_UNMAP_NOTIFY_EVENT
    printf("Sent wm_delete_window message\n");
    #endif
  }
  else {
    #ifdef SHOW_UNMAP_NOTIFY_EVENT
    printf("Killed window %lu\n", (unsigned long)framed_window);
    #endif
    XUnmapWindow(display, framed_window);
    XFlush(display);
    XKillClient(display, framed_window);
  } 
}

/**
@brief   This function is used to centre a window on a containing window. It modifieds sets x and y parameters.
@return  void
**/
void 
centre_frame(const int container_width, const int container_height, const int w, const int h, int *x, int *y) {
  *x = (container_width - w)     / 2;
  *y = (container_height - h)    / 2;
  if(*x < 0) *x = container_width  / 2;
  if(*y < 0) *y = container_height / 2;
  //BUGS for some windows this goes off the edge of the screen.
}

/**
@brief   Update frame with available resizing information 
@return  void
**/
void 
get_frame_hints(Display* display, struct Frame* frame) { //use themes
  XSizeHints specified;
  long pre_ICCCM; //pre ICCCM recovered values which are ignored.

  #ifdef SHOW_FRAME_HINTS
  printf("BEFORE: width %d, height %d, x %d, y %d, minh %d, minw %d\n", frame->w, frame->h, frame->x, frame->y, frame->min_height, frame->min_width);
  #endif

  /* whenever assigning a width, height, min/max width/height anew, always add on the h/v space. */

  if(XGetWMNormalHints(display, frame->framed_window, &specified, &pre_ICCCM) != 0) {
    #ifdef SHOW_FRAME_HINTS
    printf("Managed to recover size hints\n");
    #endif
   
    if(specified.flags & PResizeInc) { //Set this first as it might be required for the min hints
      #ifdef SHOW_FRAME_HINTS
      printf("got inc hints, w %d, h %d\n", specified.width_inc, specified.height_inc);
      #endif
      frame->width_inc = specified.width_inc;
      frame->height_inc = specified.height_inc;
    }
        
    if((specified.flags & PPosition) 
    || (specified.flags & USPosition)) {
      #ifdef SHOW_FRAME_HINTS
      if(specified.flags & PPosition) printf("PPosition specified\n");
      else printf("USPosition specified\n");
      #endif
      frame->x = specified.x;
      frame->y = specified.y;
    }

    if((specified.flags & PSize) 
    || (specified.flags & USSize)) {
      #ifdef SHOW_FRAME_HINTS    
      printf("Size specified\n");
      #endif
      frame->w = specified.width;
      frame->h = specified.height;
      frame->w += frame->hspace; 
      frame->h += frame->vspace; 

    }
    if(specified.flags & PMinSize) {
      #ifdef SHOW_FRAME_HINTS
      printf("Minimum size specified\n");
      #endif  
      frame->w_inc_offset = specified.min_width  % frame->width_inc;  
      frame->h_inc_offset = specified.min_height % frame->height_inc;
     
      if(specified.min_width  < MINWIDTH) {
        //decided to override min hints, compensate possible base case for inc_resize
        if(frame->width_inc > 1) { //inc defaults to 1  so the following might be changing the size in that case needlessly
          frame->min_width  += frame->width_inc  - (frame->min_width % frame->width_inc);
          if(frame->w_inc_offset) {
            frame->min_width  -= frame->w_inc_offset;
            frame->min_width  += frame->width_inc;
          }
        }
      }
      else {
        frame->min_width = specified.min_width;
        frame->min_width += frame->hspace;
      }

      if(specified.min_height < MINHEIGHT) {
       //decided to override min hints, compensate possible base case for inc_resize 
       if(frame->height_inc > 1) { //inc defaults to 1 so the following might be changing the size in that case needlessly
          frame->min_height += frame->height_inc - (frame->min_height % frame->height_inc);        
          if(frame->h_inc_offset) {
            frame->min_height -= frame->h_inc_offset;
            frame->min_height += frame->height_inc;
          }
        }
      }
      else {
        frame->min_height = specified.min_height;
        frame->min_height += frame->vspace;
      }
 
      if(frame->min_width > frame->max_width) frame->max_width = frame->min_width;
      if(frame->min_height > frame->max_height) frame->max_height = frame->min_height;

    }
    
    if(specified.flags & PMaxSize) {
      #ifdef SHOW_FRAME_HINTS
      printf("Maximum size specified\n");
      #endif
      //only update the maximums if they are greater than the minimums!
      if(specified.max_width >= frame->min_width - frame->hspace) {
        frame->max_height  = specified.max_height;
        frame->max_height += frame->vspace; 
      }
      if(specified.max_height >= frame->min_height - frame->vspace) {
        frame->max_width  = specified.max_width;
        frame->max_width += frame->hspace; 
      }
    }
  }
  //

  #ifdef SHOW_FRAME_HINTS      
  printf("width %d, height %d, min_width %d, max_width %d, min_height %d, max_height %d, x %d, y %d\n"
  , frame->w, frame->h, frame->min_width, frame->max_width, frame->min_height, frame->max_height
  , frame->x, frame->y);
  #endif
  
}

/**
@brief   This function is run after get_frame_type_and_mode so that there are fewer inter-dependences between that and get_frame_hints. It determines whether to centre a frame or not.
@note    If windows need to moved or resized because of their height, this is the place to do it.
@return  void
**/
static void 
frame_type_settings(Display *display, struct Frame *frame) {
  Screen* screen = DefaultScreenOfDisplay(display);
  
  /* Centre transient windows on top of their parents */
  /* This is a bit of a hack in that it uses some slow round trips but reduces coupling with other modules */
  if(frame->transient) {
    XWindowAttributes details;
    if(XGetWindowAttributes(display, frame->transient, &details)) {
      centre_frame(details.width, details.height, frame->w, frame->h, &(frame->x), &(frame->y));
      {
        Window root;
        Window parent = 0;
        Window transient_parents_toplevel = 0;
        Window *list = NULL;
        unsigned int nitems;
        XQueryTree(display, frame->transient, &root, &parent, &list, &nitems);
        if(list) XFree(list);
        if(parent) XQueryTree(display, parent, &root, &transient_parents_toplevel, &list, &nitems);
        if(list) XFree(list);
        if(transient_parents_toplevel
        &&  XGetWindowAttributes(display, transient_parents_toplevel, &details)) { 
        //get the screen x,y of the parent window 
          frame->x += details.x;
          frame->y += details.y;
        }
      }
    }
    else centre_frame(XWidthOfScreen(screen), XHeightOfScreen(screen), frame->w, frame->h, &(frame->x), &(frame->y));
  }
  else if(frame->type == dialog  ||  frame->type == splash  ) {
    centre_frame(XWidthOfScreen(screen), XHeightOfScreen(screen), frame->w, frame->h, &(frame->x), &(frame->y));
  }
}



void 
make_frame_coordinates_minmax(Display *display, struct Frame *frame) {
  frame->max_width = frame->min_width = frame->w;
  frame->max_height = frame->min_height = frame->h;
}

/**
@brief  This function gets the EWMH strut hints used for panels and sets the min/max width/height of the framed_window (which must be a panel). 
        It does this so that resize algorithms don't try and resize taskbars and so forth since this would be unexpected.
        
        At the moment this function is essentially ignored if favor of basically just allowing panels to go where they want and then tiling them as normal windows.
        
      
@return void
**/
void
get_frame_strut_hints_as_normal_hints(Display *display, struct Frame *frame, struct Atoms *atoms) {
  /*
  _NET_WM_STRUT_PARTIAL, left, right, top, bottom, left_start_y, left_end_y,
  right_start_y, right_end_y, top_start_x, top_end_x, bottom_start_x,
  bottom_end_x,CARDINAL[12]/32
  */
  Atom return_type;
  int return_format;
  long unsigned int return_nitems;
  long unsigned int return_remaining_bytes;
  
  struct { 
   uint32_t left, right, top, bottom, left_start_y, left_end_y,
   right_start_y, right_end_y, top_start_x, top_end_x, bottom_start_x,
   bottom_end_x;
  } *partial;

  
  if(frame->type != panel) { return; }
  
  return;
  
  printf("STRUT BEFORE: width %d, height %d, min_width %d, max_width %d, min_height %d, max_height %d, x %d, y %d\n"
  , frame->w, frame->h, frame->min_width, frame->max_width, frame->min_height, frame->max_height
  , frame->x, frame->y);
    
  if(XGetWindowProperty(display, frame->framed_window, atoms->wm_strut_partial, 0, 12,False, XA_CARDINAL, &return_type, &return_format
  , &return_nitems, &return_remaining_bytes, (unsigned char **)&partial) ==  Success) {
  
    if(return_type == XA_CARDINAL) {
      
      if((partial->top  && partial->bottom) || (partial->left && partial->right)) fprintf(stderr, "Warning: A panel cannot reserve space at both sides of the screen\n");
      
      if(partial->top ||  partial->bottom) {
        frame->min_height = partial->top + partial->bottom;
        frame->x = partial->bottom_start_x + partial->top_start_x;
        frame->min_width = (partial->bottom_end_x - partial->bottom_start_x) + (partial->top_end_x - partial->top_start_x);
      }
      
      if(partial->left || partial->right) {
        frame->min_width = partial->left + partial->right;
        frame->y = partial->left_start_y + partial->right_start_y;
        frame->min_height = (partial->left_end_y - partial->left_start_y) + (partial->right_end_y - partial->right_start_y);    
      }

      frame->w = frame->max_width = frame->min_width;
      frame->h = frame->max_height = frame->min_height;
       
        printf("STRUT AFTER width %d, height %d, min_width %d, max_width %d, min_height %d, max_height %d, x %d, y %d\n"
        , frame->w, frame->h, frame->min_width, frame->max_width, frame->min_height, frame->max_height
        , frame->x, frame->y);
    }
  }
  if(partial)XFree(partial);
}

/**
@brief   Determines the frame type and sets the appropriate stacking mode.
@return  void
**/
static void 
get_frame_type_and_mode(Display *display, struct Frame *frame, struct Atoms *atoms, struct Themes *themes) {
  unsigned char *contents = NULL;
  Atom return_type;
  int return_format;
  unsigned long items;
  unsigned long bytes;
  
  XGetTransientForHint(display, frame->framed_window, &frame->transient);
  
  XGetWindowProperty(display, frame->framed_window, atoms->wm_window_type, 0, 1 //long long_length?
  , False, AnyPropertyType, &return_type, &return_format,  &items, &bytes, &contents);

  frame->type = unknown;
  frame->mode = floating;
  
  if(return_type == XA_ATOM  && contents != NULL) {
    Atom *window_type = (Atom*)contents;
    #ifdef SHOW_PROPERTIES
    printf("Number of atoms %lu\n", items);
    #endif
    for(unsigned int i =0; i < items; i++) {
    //these are fairly mutually exclusive so be suprised if there are others
      if(window_type[i] == atoms->wm_window_type_desktop) {
        #ifdef SHOW_PROPERTIES
        printf("mode/type: desktop\n");
        #endif
        frame->mode = desktop;
        //frame->mode = floating;
        //There are no desktop windows - desktops are normal "programs"
        if(themes->window_type[program]) frame->type = program;
      }
      else if(window_type[i] == atoms->wm_window_type_normal) {
        #ifdef SHOW_PROPERTIES
        printf("type: normal/definately unknown\n");
        #endif
        frame->mode = tiling;
        frame->type = unknown;
      }
      else if(window_type[i] == atoms->wm_window_type_dock) {
        #ifdef SHOW_PROPERTIES
        printf("type: dock\n");
        #endif
        frame->mode = tiling;
        if(themes->window_type[panel]) frame->theme_type = panel;
        frame->type = panel;
        frame->sticky = True;
      }
      else if(window_type[i] == atoms->wm_window_type_splash) {
        #ifdef SHOW_PROPERTIES
        printf("type: splash\n");
        #endif
        //frame->mode = floating;
        //if(themes->window_type[splash]) 
        frame->type = splash; //these have been removed because splash screens aren't managed
      }
      else if(window_type[i] == atoms->wm_window_type_dialog) {
        #ifdef SHOW_PROPERTIES
        printf("type: dialog\n");
        #endif
        frame->mode = floating;
        if(themes->window_type[dialog]) frame->theme_type = dialog;
        frame->type = dialog;
      }
      else if(window_type[i] == atoms->wm_window_type_utility) {
        #ifdef SHOW_PROPERTIES
        printf("type: utility\n");
        #endif
        frame->mode = tiling;
        if(themes->window_type[utility]) frame->theme_type = utility;
        frame->type = utility;
      }
    }
  }

  if(contents) XFree(contents);
}

/**
@brief   This function gets the frame state, which is either, none, demands attention or fullscreen.  
@brief   Since EWMH considers "modal dialog box" a state, but I consider it a type, it can also change the type
@brief   and therefore must be called after get_frame_type. 
@return  void
**/
static void 
get_frame_state(Display *display, struct Frame *frame, struct Atoms *atoms) {
  unsigned char *contents = NULL;
  Atom return_type;
  int return_format;
  unsigned long items;
  unsigned long bytes;
  XGetWindowProperty(display, frame->framed_window, atoms->wm_state, 0, 1
  , False, AnyPropertyType, &return_type, &return_format,  &items, &bytes, &contents);

  //printf("loading state\n");
  frame->state = none; 
  if(return_type == XA_ATOM  && contents != NULL) {
    Atom *window_state = (Atom*)contents;
    #ifdef SHOW_PROPERTIES
    printf("Number of atoms %lu\n", items);
    #endif
    for(unsigned int i =0; i < items; i++) {
      if(window_state[i] == atoms->wm_state_demands_attention) {
        #ifdef SHOW_PROPERTIES
        printf("state: urgent\n");
        #endif
        frame->wants_attention = True;
      }
      else if(window_state[i] == atoms->wm_state_modal) {
        #ifdef SHOW_PROPERTIES
        printf("type/state: modal dialog\n");
        #endif
        frame->type = modal_dialog;
        frame->mode = floating;
      }
      else if(window_state[i] == atoms->wm_state_fullscreen) {
        #ifdef SHOW_PROPERTIES
        printf("state: fullscreen\n");
        #endif
        frame->state = fullscreen;
      }
      else if(window_state[i] == atoms->wm_state_hidden) {
        #ifdef SHOW_PROPERTIES
        printf("state: hidden/minimized \n");
        #endif
        frame->state = minimized;         
      }
    }
  }
  if(contents != NULL) XFree(contents);
}

/**
@brief   This function creates the windows which make up the actual frame and its widgets.
@return  void
**/
static void
create_frame_subwindows (Display *display, struct Frame *frame, struct Themes *themes, struct Cursors *cursors) {
  Window root = DefaultRootWindow(display);
  Screen* screen = DefaultScreenOfDisplay(display);
  int black = BlackPixelOfScreen(screen);

  frame->widgets[frame_parent].widget = XCreateSimpleWindow(display, root
  , frame->x, frame->y,  frame->w, frame->h, 0, black, black);
  XFlush(display);
  for(int j = 0; j <= inactive; j++)  frame->widgets[frame_parent].state[j] = 0;

  for(int i = 0; i < frame_parent; i++) {
    frame->widgets[i].widget = 0;
    for(int j = 0; j <= inactive; j++)  frame->widgets[i].state[j] = 0;
  }
  for(int i = 0; i < frame_parent; i++) {
    int x = themes->window_type[frame->theme_type][i].x;
    int y = themes->window_type[frame->theme_type][i].y;
    int w = themes->window_type[frame->theme_type][i].w;
    int h = themes->window_type[frame->theme_type][i].h;

    if(!themes->window_type[frame->theme_type][i].exists) {
      //printf("Skipping window component %d\n", i);
      continue;
    }
    //TODO get the size of the window from somewhere else
    if(x <  0) x += frame->w;
    if(y <  0) y += frame->h; 
    if(w <= 0) w += frame->w;
    if(h <= 0) h += frame->h;

    if(themes->window_type[frame->theme_type][i].exists < 0) { //the exists variable is -1 for hotspots
      frame->widgets[i].widget = XCreateWindow(display, frame->widgets[frame_parent].widget
      , x, y, w, h, 0, CopyFromParent, InputOnly, CopyFromParent, 0, NULL);
      XMapWindow(display, frame->widgets[i].widget);
    }
    else if(themes->window_type[frame->theme_type][i].exists) { //otherwise if it isn't an inputonly hotspot
      frame->widgets[i].widget = XCreateSimpleWindow(display, frame->widgets[frame_parent].widget
      , x, y, w, h, 0, black, black);
      XFlush(display);
      if(i != window) { //don't create state windows for the framed window 
        //OPTIMIZATION: This makes the cropped state windows large so that they don't need to be resized
        if(themes->window_type[frame->theme_type][i].w <= 0 || i == mode_dropdown_text) w = XWidthOfScreen(screen);
        if(themes->window_type[frame->theme_type][i].h <= 0) h = XWidthOfScreen(screen);
        for(int j = 0; j <= inactive; j++) {
          frame->widgets[i].state[j] = XCreateSimpleWindow(display, frame->widgets[i].widget
          , 0, 0, w, h, 0, black, black);
          XSetWindowBackgroundPixmap(display, frame->widgets[i].state[j]
          , themes->window_type[frame->theme_type][i].state_p[j]);
          XMapWindow(display, frame->widgets[i].state[j]);
        }
      }
      //Map windows
      XMapWindow(display, frame->widgets[i].widget);
    }
  }
  frame->menu.width = 0;
  //select input
  XSelectInput(display, frame->widgets[frame_parent].widget
  , Button1MotionMask | ButtonPressMask | ButtonReleaseMask);

  XDefineCursor(display, frame->widgets[frame_parent].widget, cursors->normal);

  if(themes->window_type[frame->theme_type][close_button_hotspot].exists) {
    XSelectInput(display, frame->widgets[close_button_hotspot].widget
    ,  ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
    
    XDefineCursor(display, frame->widgets[close_button_hotspot].widget, cursors->pressable);
  }

  if(themes->window_type[frame->theme_type][mode_dropdown_hotspot].exists) {
    XSelectInput(display, frame->widgets[mode_dropdown_hotspot].widget, ButtonPressMask | ButtonReleaseMask);
    XDefineCursor(display, frame->widgets[mode_dropdown_hotspot].widget, cursors->pressable);
  }

  if(themes->window_type[frame->theme_type][title_menu_hotspot].exists) {
    XSelectInput(display, frame->widgets[title_menu_hotspot].widget, ButtonPressMask | ButtonReleaseMask);
    XDefineCursor(display, frame->widgets[title_menu_hotspot].widget , cursors->pressable);
  }

  if(themes->window_type[frame->theme_type][tl_corner].exists) {
    XSelectInput(display, frame->widgets[tl_corner].widget,  ButtonPressMask | ButtonReleaseMask);
    XDefineCursor(display, frame->widgets[tl_corner].widget, cursors->resize_tl_br);
  }
  if(themes->window_type[frame->theme_type][t_edge].exists) {
    XSelectInput(display, frame->widgets[t_edge].widget,     ButtonPressMask | ButtonReleaseMask);
    XDefineCursor(display, frame->widgets[t_edge].widget , cursors->resize_v);
  }
  if(themes->window_type[frame->theme_type][tr_corner].exists) {
    XSelectInput(display, frame->widgets[tr_corner].widget,  ButtonPressMask | ButtonReleaseMask);
    XDefineCursor(display, frame->widgets[tr_corner].widget, cursors->resize_tr_bl);  
  }
  if(themes->window_type[frame->theme_type][l_edge].exists) {
    XSelectInput(display, frame->widgets[l_edge].widget,     ButtonPressMask | ButtonReleaseMask);
    XDefineCursor(display, frame->widgets[l_edge].widget , cursors->resize_h);
  }
  if(themes->window_type[frame->theme_type][bl_corner].exists) {
    XSelectInput(display, frame->widgets[bl_corner].widget,  ButtonPressMask | ButtonReleaseMask);
    XDefineCursor(display, frame->widgets[bl_corner].widget, cursors->resize_tr_bl);
  }
  if(themes->window_type[frame->theme_type][b_edge].exists) {
    XSelectInput(display, frame->widgets[b_edge].widget,     ButtonPressMask | ButtonReleaseMask);
    XDefineCursor(display, frame->widgets[b_edge].widget , cursors->resize_v);
  }
  if(themes->window_type[frame->theme_type][br_corner].exists) {
    XSelectInput(display, frame->widgets[br_corner].widget,  ButtonPressMask | ButtonReleaseMask);
    XDefineCursor(display, frame->widgets[br_corner].widget, cursors->resize_tl_br);
  }
  if(themes->window_type[frame->theme_type][r_edge].exists) {
    XSelectInput(display, frame->widgets[r_edge].widget,     ButtonPressMask | ButtonReleaseMask);
    XDefineCursor(display, frame->widgets[r_edge].widget , cursors->resize_h);
  }
  XFlush(display);
}



/**
@brief   create pixmaps with the specified name if it is available, otherwise use a default name 
@return  void 
**/
void
create_frame_name(Display* display, struct Popup_menu *window_menu, struct Frame *temp
, struct Themes *themes, struct Atoms *atoms) {
  char untitled[] = "noname";

  //struct Frame temp = *frame; 

  Screen* screen = DefaultScreenOfDisplay(display);
  int black = BlackPixelOfScreen(screen);

  /* Destroy/Free old title if it had one */
  free_frame_name(temp);
  
  { 
    /* Recover EWMH UTF8 name first. If none exists, get the ICCCM ASCII name */
    /* If this succeeds, previous names will be freed if they exists later on */
    Atom ret_type;
    int  ret_format;
    unsigned long ret_nitems;
    unsigned long ret_trailing_bytes;
    temp->window_name = NULL;
    if((XGetWindowProperty (display, temp->framed_window, atoms->name, (long)0, (long)MAX_WM_NAME_LENGTH
    , False, atoms->utf8
    , &ret_type, &ret_format, &ret_nitems, &ret_trailing_bytes
    , (unsigned char **)&temp->window_name ) != Success) || temp->window_name == NULL) {

      //Try and get the non EWMH name
      if(!XFetchName(display, temp->framed_window, &temp->window_name)) {
      
        //Generate a blank name.
        printf("Warning: unnamed window\n");
        XStoreName(display, temp->framed_window, untitled);
        XFlush(display);
        XFetchName(display, temp->framed_window, &temp->window_name);
        XFlush(display);      
      }
    }
  }
  /*
  if(temp->window_name == NULL 
  && frame->window_name != NULL
  && strcmp(frame->window_name, untitled) == 0) {
    //it was null and already has the name from untitled above.
    return;
  }
  else 
  if( (temp->window_name != NULL
  && frame->window_name != NULL )
  && (strcmp(temp->window_name, frame->window_name) == 0)) {
    XFree(temp->window_name);
    //skip this if the name hasn't changed
    return;
  }
  */
  
  if(!temp->menu.item) {
    temp->menu.item = XCreateSimpleWindow(display
    , window_menu->widgets[popup_menu_parent].widget
    , themes->popup_menu[l_edge].w, 0
    , XWidthOfScreen(screen), themes->popup_menu[menu_item_mid].h
    , 0, black, black);
    for(int i = 0; i <= inactive; i++) {
      temp->menu.state[i] = XCreateSimpleWindow(display
      , temp->menu.item
      , 0, 0
      , XWidthOfScreen(screen), themes->popup_menu[menu_item_mid].h
      , 0, black, black);
    }
    XSelectInput(display, temp->menu.item, ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
  }
  
  temp->menu.width = get_text_width(display, temp->window_name, &themes->font_theme[active]);

  //create corresponding title menu item for this frame
  for(int i = 0; i <= inactive; i++) {
    XUnmapWindow(display, temp->menu.state[i]);
    XUnmapWindow(display, temp->widgets[title_menu_text].state[i]);
    XFlush(display);
    //create the title menu item with the windows title
    create_text_background(display, temp->menu.state[i], temp->window_name
    , &themes->font_theme[i], themes->popup_menu[menu_item_mid].state_p[i]
    , XWidthOfScreen(screen), themes->popup_menu[menu_item_mid].h);
    
    //TODO make the title for unfocussed windows not bold?
    create_text_background(display, temp->widgets[title_menu_text].state[i], temp->window_name
    , &themes->font_theme[active], themes->window_type[temp->theme_type][title_menu_text].state_p[i]
    , XWidthOfScreen(screen), themes->window_type[temp->theme_type][title_menu_text].h);
    
    //If this is mapped here, it might be shown in the wrong workspace, //XMapWindow(display, temp->menu.item);
    /* Show changes to background pixmaps */
    XMapWindow(display, temp->menu.state[i]);
    XMapWindow(display, temp->widgets[title_menu_text].state[i]);    
  }
  xcheck_raisewin(display, temp->menu.state[active]);
  //these are the items for inside the menu
  //need to create all these windows.
  
  {
    XWindowAttributes attr;
    XGetWindowAttributes(display, temp->menu.item, &attr);
    
    if(attr.map_state != IsUnmapped) { //remap all the state pixmaps
      XSelectInput(display, temp->menu.item, 0);
      XSync(display, False);
      XUnmapWindow(display, temp->menu.item);
      XSelectInput(display, temp->menu.item, ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
      XFlush(display);
      XMapWindow(display, temp->menu.item);
    }
  }
  XFlush(display);

  //*frame = temp;
}

/**  
@brief   frees the frame name.  This is used to make the transition to the EWMH UTF8 names simpler, as EWMH names might need to be freed differently.
@return  void
**/
void
free_frame_name(struct Frame* frame) {
  if(frame->window_name != NULL) {
    XFree(frame->window_name);
    frame->window_name = NULL;
  }
} 

/**
@brief  determines whether to make the window availabe in all workspaces.
@return 1 if suitable, 0 otherwise.
**/
Bool
suitable_for_foreign_workspaces(struct Frame *frame) {
  if(frame->transient) return False;
  switch(frame->type) {
    case unknown:
    case file:
    case program: //this should change later on when the other types are properly detected.
    case status:
    case system_program:    
    case splash:
    case panel:  //we don't want it in the window menu.
    return True;

    case dialog:
    case modal_dialog:
    case utility:
    break;
  }
  return False;
}

/**
@brief  Recover a window chosen from the window menu or in response to a successful _NET_ACTIVE_WINDOW request
@param  frames  workspace the frame is in
@param  i       index of the frame in the workspaces list of frames.
@return void
**/
void 
recover_frame(Display *display, struct Workspace *frames, int i /*index*/, struct Separators *seps, struct Themes *themes) {
  //allow desktop windows to be recovered/tiled.  Otherwise the user has no way to recover a desktop window.
  if(frames->list[i]->mode == desktop) {
    if(drop_frame(display, frames, i, False, themes))  {
      change_frame_mode(display, frames->list[i], tiling, themes);
      resize_frame(display, frames->list[i], themes);
    }
  }
  else if(frames->list[i]->mode == tiling) {
    if(drop_frame(display, frames, i, False, themes))  {
      XMapWindow(display, frames->list[i]->widgets[frame_parent].widget);
      frames->list[i]->state = none;
    }
  }
  else if(frames->list[i]->mode == floating) {
    if(drop_frame(display, frames, i, True, themes)) {
      XMapWindow(display, frames->list[i]->widgets[frame_parent].widget);
      frames->list[i]->state = none;
    }
  }
  else {
    XMapWindow(display, frames->list[i]->widgets[frame_parent].widget);
    frames->list[i]->state = none;
  }
  stack_frame(display, frames->list[i], seps);
  reset_frame_titlebar(display, frames->list[i]);
  XFlush(display);
}


/** 
@brief   Updates the frame with available wm hints (ICCCM icon, window "group", focus wanted, urgency, withdrawn) 
@return  void
**/
void 
get_frame_wm_hints(Display *display, struct Frame *frame) {
  XWMHints *wm_hints = XGetWMHints(display, frame->framed_window);
  //WM_ICON_SIZE  in theory we can ask for set of specific icon sizes.

  //Set defaults
  Pixmap icon_p = 0;
  Pixmap icon_mask_p = 0;

  if(wm_hints != NULL) {
    if(wm_hints->flags & IconPixmapHint) {
      icon_p = wm_hints->icon_pixmap;
      //XSetWindowBackgroundPixmap(display, frame->menu_item.icon.item_icon, icon_p);
    }

    if(wm_hints->flags & IconMaskHint) {
      icon_mask_p = wm_hints->icon_mask;
      //icon_mask_p = wm_hints->icon_mask;
      //XShapeCombineMask (display, icon_p, ShapeBounding //ShapeClip or ShapeBounding
      //,0, 0, icon_mask_p, ShapeSet); 
      //Shapeset or ShapeUnion, ShapeIntersect, ShapeSubtract, ShapeInvert
      //XMapWindow(display, frame->menu_item.icon.item_icon);
      
    }
    /* This method of creating the icon is not good, a higher quality method is available in EWMH
    if(icon_p && icon_mask_p ) {
      for(int i = 0; i <= inactive; i++) {
        create_icon_background(display, frame->widgets[title_menu_lhs].state[i]
        , icon_p, icon_mask_p
        , themes->window_type[frame->theme_type][title_menu_lhs].state_p[i]
        , themes->window_type[frame->theme_type][title_menu_lhs].w, themes->window_type[frame->theme_type][title_menu_lhs].h);
        XMapWindow(display, frame->widgets[title_menu_lhs].state[i]);
      }
      
    }
    XSync(display, False);
    */
    //get the icon sizes
    //find out it is urgent
    //get the icon if it has one.
    //icon window is for the systray
    //window group is for mass minimization
    XFree(wm_hints);
  }
}

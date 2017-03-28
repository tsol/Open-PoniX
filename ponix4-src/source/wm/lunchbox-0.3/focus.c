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
#include <X11/extensions/shape.h>
#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xatom.h>

#include "lunchbox.h"

#include "frame.h"
#include "frame-actions.h"
#include "focus.h"
#include "xcheck.h"

/**
@file     focus.c
@brief    Functions for setting and recovering keyboard focus for frames.  Keyboard focus is the term used to identify the window which receives key press events.
@author   Alysander Stanley
**/

/*
(implemented in add_frame_to_workspace called from main)
Give focus to new windows if
 1) It is the only window in the workspace
 2) It is a transient window and it's parent has focus.

(implemented in UnmapNotify case in main)
Remove focus when any frame is closed 
Remove focus and change it to previously focussed window when:
 1) The currently focussed frame is closed.

(implemented in ButtonRelease case in main)
 2) The currently focussed frame is sunk
 3) A window is replaced using the title menu with another window.

(same section as "window is replaced" above)
Remove focus if any window is hidden - because giving focus shouldn't have to raise a window.

(implemented in ButtonPress) 
Change focus to another window whenever the user clicks on it.
*/

/**
@brief Adds another window (the framed window) to the focus list
@return void
**/
void add_focus(Window new, struct Focus_list* focus) {
  remove_focus(new, focus); //remove duplicates
  if(focus->used == focus->max  ||  focus->list == NULL) {
    Window *temp = NULL;
    if(focus->list != NULL) temp = realloc(focus->list, sizeof(Window) * focus->max * 2);
    else {
      temp = malloc(sizeof(Window) * focus->max);
      focus->max = focus->max / 2;
    }
    
    if(temp != NULL) {
      focus->list = temp;
      focus->max *= 2;
    }
    else if (focus->list == NULL) return;      //it doesn't really matter if there is no focus history
    else remove_focus(focus->list[0], focus);  //if out of memory overwrite oldest value
  }
  focus->list[focus->used] = new;
  focus->used++;
}

/**
@brief Removes a window from the focus list. This is called when the window has been unmapped. 
@return void
**/
void remove_focus(Window old, struct Focus_list* focus) {
  int i;
  if(focus->list == NULL || focus->used == 0) return;
  //recently added windows are more likely to be removed
  for( i = focus->used - 1; i >= 0; i--) if(focus->list[i] == old) break;
  if(i < 0) return; //not found
  focus->used--;
  //we know that i is unsigned at this stage
  for( ; (unsigned int)i < focus->used; i++) focus->list[i] = focus->list[i + 1];
}

/**
@brief Determines if newly created windows should get focussed automatically.
       This is the case if no windows are currently focussed or if it is a transient window and its parent is focussed.
       Caller then checks if the frame structure   has the "selected" member set to 1. 
       This doesn't actually focus the window in case it is in the wrong workspace the caller must determine that and then run recover focus.
@return void
**/
void check_and_set_new_frame_focus (Display *display, struct Frame *frame, struct Workspace *frames) {
  int set_focus = 0;

  if(frame->type == panel) return; /* TODO use hints to establish whether it is focussable */
    
  if(frames->focus.used == 0) set_focus=1;
  else if(frames->focus.used > 0  
  && frames->focus.list 
  && frame->transient == frames->focus.list[frames->focus.used - 1]) { //parent has focus
    unfocus_frames(display, frames); //make frames look normal
    set_focus=1;
  }
  
  if(set_focus) {
    add_focus(frame->framed_window, &frames->focus);
    frame->focussed = True;    
    for(int widget_index = 0; widget_index <= frame_parent; widget_index++) {
      change_frame_widget_state(display, frame, widget_index, normal);
    }
    XFlush(display);
  }
}


/**
@pre  This assumes that no widgets will be active on the focussed frame when this is called.
@brief Resets the appearance of all the frames. Used when changing the focussed window.
@return void
**/
void unfocus_frames(Display *display, struct Workspace *frames) {
  if(!frames->list) return;
  for(int i = 0; i < frames->used; i++) {
    if(frames->list[i]->focussed) {
      frames->list[i]->focussed = False;
      for(int widget_index = 0; widget_index <= frame_parent; widget_index++) {
        change_frame_widget_state(display, frames->list[i], widget_index, normal);
      }
      XFlush(display);
    }
  }
}

/**
@brief After a focussed window is closed, use this function to set the focus to another window. It actually does set the focus using XSetInputFocus. 
@return void
**/  

void recover_focus(Display *display, struct Workspace *frames, struct Themes *themes, struct Atoms *atoms) {
  Window root = DefaultRootWindow(display);
  if(frames->focus.used == 0) return;
  //printf("Recovering focus\n");
  for(int i = frames->used - 1; i >= 0; i--) { 
    if(frames->list[i]->framed_window == frames->focus.list[frames->focus.used - 1]) {
      //_NET_ACTIVE_WINDOW
      #ifdef CRASH_ON_BUG
      XGrabServer(display);
      XSetErrorHandler(supress_xerror);
      #endif
      //seems excessive but closing windows can cause bad window errors
      XSetInputFocus(display, frames->list[i]->framed_window, RevertToPointerRoot, CurrentTime);
      XChangeProperty(display, root, atoms->active_window, XA_WINDOW, 32, PropModeReplace, (unsigned char *)&frames->list[i]->framed_window, 1);
      #ifdef CRASH_ON_BUG
      XSync(display, False);
      XSetErrorHandler(NULL);    
      XUngrabServer(display);
      #endif
      XFlush(display);
      frames->list[i]->focussed = True;
      for(int widget_index = 0; widget_index <= frame_parent; widget_index++) {
        change_frame_widget_state(display, frames->list[i], widget_index, normal);
      }
      break;
    }
  }
}

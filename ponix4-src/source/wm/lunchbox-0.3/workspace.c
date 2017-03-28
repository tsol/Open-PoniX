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
#include <assert.h>

#include "xcheck.h"
#include "lunchbox.h"
#include "menus.h"
#include "theme.h"
#include "frame.h"
#include "frame-actions.h"
#include "workspace.h"
#include "space.h"
#include "focus.h"
#include "util.h"

static void
save_frame_state(struct Saved_frame_state* save, struct Frame *frame);

static void
load_frame_state(Display *display, struct Saved_frame_state* save, struct Frame *frame, struct Separators *seps, struct Themes *themes, struct Atoms *atoms);

static void
load_initial_states(struct Saved_frame_state* save, struct Frame *frame);

static Bool
ensure_empty_frame_slots(struct Workspace_list *workspaces);

static Bool
ensure_empty_frame_reference_slots(struct Workspace *workspace);
/**
@file     workspace.c
@brief    Contains functions for manipulating workspaces.  A workspace is a different arrangement of frames.
@author   Alysander Stanley
**/

/**
@brief    Find a frame with a matching menu item, (each frame has keeps track of it's correspoding window/title menu item)
@return   index of the frame in the global frame_list if found, -1 otherwise.  
@param    window the desired framed_window
**/
int
find_frame_with_menu_item_in_workspace(Window window, struct Workspace_list* workspaces, int current_workspace) {
  struct Workspace *frames = &workspaces->list[current_workspace];
  if(current_workspace == -1) return -1;
  for(int i = 0; i < frames->used; i += 1) {
    if(window == frames->list[i]->menu.item) return i;
  }
  return -1;
}


/**
@brief    Find a workspace with a matching menu item, (each workspace has keeps track of it's correspoding program menu item)
@return   index of the workspace if found, -1 otherwise.  
@param    window the desired framed_window
**/
int
find_workspace_with_menu_item(Window window, struct Workspace_list* workspaces) {
  for(int k = 0; k < workspaces->used_workspaces; k += 1) {
    if(window == workspaces->list[k].workspace_menu.item) return k;
  }
  return -1;
}

/**
@brief    Find a frame with a particular framed_window
@return   1 if found, -1 otherwise.  
@param    window the desired framed_window
@param    i   is set to the index of the frame in the global frame_list
**/
int
find_frame_with_framed_window(Window window, struct Workspace_list* workspaces) {
  for(int i = 0; i < workspaces->used_frames; i += 1) {
    if(window == workspaces->frame_list[i].framed_window) return i;
  }
  return -1;
}


/**
@brief    Find a frame with a particular framed_window in a specified workspace
@return   1 if found, -1 otherwise.  
@param    window the desired framed_window
@param    i   is set to the index of the frame in the workspaces frame list
**/
int
find_frame_with_framed_window_in_workspace(Window window, struct Workspace_list* workspaces, int k) {
  if(k < 0) return -1;
  struct Workspace *frames = &workspaces->list[k];
  for(int i = 0; i < frames->used; i += 1) {
    if(window == frames->list[i]->framed_window) return i;
  }
  return -1;
}


/**
@brief    Find a frame with a widget that matches a specified window in a workspace
@return   Frame_widget enum that matched or frame_parent + 1
@param    window the window to be matched.
@param    k   is the index of the workspace
@param    i   is set to the index of the frame in the workspace
**/
enum Frame_widget
find_frame_with_widget_in_workspace(Window key_window, struct Workspace_list* workspaces, int k, int *i) {
  enum Frame_widget found = frame_parent + 1;
  struct Workspace *frames = &workspaces->list[k];
  if(k == -1) return found;
  for(*i = 0; *i < frames->used; *i += 1) {
    if(key_window == frames->list[*i]->widgets[window].widget)               found = window;
    else if(key_window == frames->list[*i]->widgets[t_edge].widget)               found = t_edge;
    else if(key_window == frames->list[*i]->widgets[titlebar].widget)             found = titlebar;
    else if(key_window == frames->list[*i]->widgets[l_edge].widget)               found = l_edge;
    else if(key_window == frames->list[*i]->widgets[b_edge].widget)               found = b_edge;
    else if(key_window == frames->list[*i]->widgets[r_edge].widget)               found = r_edge;
    else if(key_window == frames->list[*i]->widgets[tl_corner].widget)            found = tl_corner;
    else if(key_window == frames->list[*i]->widgets[tr_corner].widget)            found = tr_corner;
    else if(key_window == frames->list[*i]->widgets[bl_corner].widget)            found = bl_corner;
    else if(key_window == frames->list[*i]->widgets[br_corner].widget)            found = br_corner;
    else if(key_window == frames->list[*i]->widgets[selection_indicator].widget)  found = selection_indicator;
    else if(key_window == frames->list[*i]->widgets[selection_indicator_hotspot].widget) found = selection_indicator_hotspot;
    else if(key_window == frames->list[*i]->widgets[title_menu_lhs].widget)              found = title_menu_lhs;
    else if(key_window == frames->list[*i]->widgets[title_menu_icon].widget)             found = title_menu_icon;
    else if(key_window == frames->list[*i]->widgets[title_menu_text].widget)             found = title_menu_text;
    else if(key_window == frames->list[*i]->widgets[title_menu_rhs].widget)              found = title_menu_rhs;
    else if(key_window == frames->list[*i]->widgets[title_menu_hotspot].widget)          found = title_menu_hotspot;
    else if(key_window == frames->list[*i]->widgets[mode_dropdown_lhs].widget)           found = mode_dropdown_lhs;
    else if(key_window == frames->list[*i]->widgets[mode_dropdown_text].widget)          found = mode_dropdown_text;
    else if(key_window == frames->list[*i]->widgets[mode_dropdown_text_floating].widget) found = mode_dropdown_text_floating;
    else if(key_window == frames->list[*i]->widgets[mode_dropdown_text_tiling].widget)   found = mode_dropdown_text_tiling;
    else if(key_window == frames->list[*i]->widgets[mode_dropdown_text_desktop].widget)  found = mode_dropdown_text_desktop;
    else if(key_window == frames->list[*i]->widgets[mode_dropdown_rhs].widget)           found = mode_dropdown_rhs;
    else if(key_window == frames->list[*i]->widgets[mode_dropdown_hotspot].widget)       found = mode_dropdown_hotspot;
    else if(key_window == frames->list[*i]->widgets[close_button].widget)                found = close_button;
    else if(key_window == frames->list[*i]->widgets[close_button_hotspot].widget)        found = close_button_hotspot;
    else if(key_window == frames->list[*i]->widgets[frame_parent].widget)                found = frame_parent;
    else continue;
    return found;
  }
  return found;
}

/**
@brief    Creates a new workspace, including separators used for stacking windows in different modes and the workspace menu item for the program menu. 
@return   the index of the created workspace
**/
int 
create_workspace(Display *display, struct Workspace_list* workspaces, char *workspace_name, struct Themes *themes) {
//  Window root = DefaultRootWindow(display);
  Screen* screen =  DefaultScreenOfDisplay(display);  
  int black = BlackPixelOfScreen(screen);
//  XSetWindowAttributes attributes; 
  struct Workspace *frames;

  if(workspaces->list == NULL) {
    workspaces->used_workspaces = 0;
    workspaces->max_workspaces = 16;
    workspaces->list = malloc(sizeof(struct Workspace) * workspaces->max_workspaces);
    if(workspaces->list == NULL) return -1;
  }
  else if(workspaces->used_workspaces == workspaces->max_workspaces) {
    //printf("reallocating, used_workspaces %d, max%d\n", workspaces->used_workspaces, workspaces->max);
    struct Workspace* temp = NULL;
    temp = realloc(workspaces->list, sizeof(struct Workspace) * workspaces->max_workspaces * 2);
    if(temp != NULL) workspaces->list = temp;
    else {
      perror("\nError: Not enough available memory\n");
      return -1;
    }
    workspaces->max_workspaces *= 2;
  }

  //the frame list frames is the new workspace
  frames = &workspaces->list[workspaces->used_workspaces];
  frames->workspace_name = workspace_name;

  frames->states = calloc(sizeof(struct Saved_frame_state), workspaces->max_frames);
  if(!frames->states) { perror("Error: not enough memory to allocate frame save states in a new workspace"); return -1; }
  //Create the background window
  //frames->virtual_desktop = XCreateSimpleWindow(display, root, 0, 0
  //, XWidthOfScreen(screen), XHeightOfScreen(screen), 0, black, black);
  //attributes.cursor = cursors->normal;
  //attributes.override_redirect = True;
  //attributes.background_pixmap = ParentRelative;
  //XChangeWindowAttributes(display, frames->virtual_desktop 
  //, CWOverrideRedirect | CWBackPixmap | CWCursor , &attributes);
  //XLowerWindow(display, frames->virtual_desktop);

  //TODO when a workspace is added to the list, it must also be resized to the width of that list.
  unsigned int width = workspaces->workspace_menu.inner_width;

  frames->workspace_menu.item = XCreateSimpleWindow(display
  , workspaces->workspace_menu.widgets[popup_menu_parent].widget
  , themes->popup_menu[popup_l_edge].w, themes->popup_menu[popup_t_edge].h
  , width, themes->popup_menu[menu_item_mid].h
  , 0, black, black);

  XSelectInput(display, frames->workspace_menu.item,  ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);
  
  for(int i = 0; i <= inactive; i++) { //if(themes->popup_menu[menu_item].state_p[i])
    frames->workspace_menu.state[i] = XCreateSimpleWindow(display
    , frames->workspace_menu.item
    , 0, 0
    , XWidthOfScreen(screen), themes->popup_menu[menu_item_mid].h
    , 0, black, black);

    create_text_background(display, frames->workspace_menu.state[i], frames->workspace_name
    , &themes->font_theme[i], themes->popup_menu[menu_item_mid].state_p[i]
    , themes->popup_menu[menu_item_mid].w, themes->popup_menu[menu_item_mid].h);

    XMapWindow(display, frames->workspace_menu.state[i]);
  }

  XMapWindow(display, frames->workspace_menu.item);

  frames->workspace_menu.width = get_text_width(display, frames->workspace_name, &themes->font_theme[active]);

  //Create the frame_list
  frames->used = 0;
  frames->max  = DEFAULT_STARTING_FRAMES;
  frames->list = NULL; //this is allocated when we change to this workspace.

  frames->focus.used = 0;
  frames->focus.max  = 8; //must be divisible by 2
  frames->focus.list = malloc(sizeof(struct Focus_list) * frames->focus.max); //ok if it fails.
  #ifdef SHOW_WORKSPACE
  printf("Created workspace %d\n", workspaces->used_workspaces);
  #endif

  //need to make the existing neutral windows available to new workspaces
  for(int i = 0; i < workspaces->used_frames; i++) {
    if(suitable_for_foreign_workspaces(&workspaces->frame_list[i])) {
      load_initial_states(&frames->states[i], &workspaces->frame_list[i]);
    }
  }
  
  workspaces->used_workspaces++;
  XSync(display, False);
  
  return workspaces->used_workspaces - 1;
}

/**  
@brief    This is called when the wm is exiting, it doesn't close the open windows.  It shuffles all the workspaces along to fill the gap.
@return   void
**/
void 
remove_workspace(Display *display, struct Workspace_list* workspaces, int index) {

  assert(index < workspaces->used_workspaces);
  assert(index >= 0);
  
  struct Workspace *frames = &workspaces->list[index];

  if(frames->list) {
    free(frames->list);
    frames->list = NULL;
    frames->used = 0;
  }
  if(frames->workspace_name != NULL) {
    XFree(frames->workspace_name);
    frames->workspace_name = NULL;
  }
  //remove the focus list
  if(frames->focus.list !=  NULL) {
    free(frames->focus.list);
    frames->focus.list = NULL;
  }
  if(frames->states) {
    free(frames->states);
    frames->states = NULL;
  }

  workspaces->used_workspaces--;
  //close the background window
  //XDestroyWindow(display, frames->virtual_desktop);
  //remove the entry in the program menu
  XDestroyWindow(display, frames->workspace_menu.item);
  //keep the open workspaces in order
  for(int i = index ; i < workspaces->used_workspaces; i++) workspaces->list[i] = workspaces->list[i + 1];
}

/** 
@brief    Generates a name for the program frame the XClassHint.  The returned pointer must be freed with XFree.
@return   A pointer to the null terminated string.
**/
char *
load_program_name(Display* display, Window window) {
  XClassHint program_hint;
  if(XGetClassHint(display, window, &program_hint)) {
   // printf("res_name %s, res_class %s\n", program_hint.res_name, program_hint.res_class);
    if(program_hint.res_name != NULL) XFree(program_hint.res_name);    
    if(program_hint.res_class != NULL)
      return program_hint.res_class;
  }
  return NULL;
}

/**
@brief    Used to generate a default name for the case when the XClassHints are not set correctly.
@return   void
**/
void 
make_default_program_name(Display *display, Window window, char *name) {
  XClassHint program_hint;
  program_hint.res_name = name;
  program_hint.res_class = name;
  XSetClassHint(display, window, &program_hint);
  XFlush(display);  
}

/**
@brief   Save the workspace independent frame details for recovery when the workspace changes.
@return  void
**/
static void
save_frame_state(struct Saved_frame_state* save, struct Frame *frame) {
  #ifdef SHOW_STATES
  printf("Saving x %d, y %d, w %d, h %d, state %d.  %s\n", frame->x, frame->y, frame->w, frame->h, frame->state, frame->window_name);
  #endif
  save->x = frame->x;
  save->y = frame->y;
  save->w = frame->w;
  save->h = frame->h;
  save->mode = frame->mode;
  save->state = frame->state;
  save->sticky = frame->sticky;
}

/**
@brief   Load the defaults to give a window in other workspaces when it starts up.  Saved because when new workspaces are created they should be given the same values to be consistent.
@return  void
**/
static void
load_initial_states(struct Saved_frame_state* save, struct Frame *frame) {
  save->x = frame->initial_state.x;
  save->y = frame->initial_state.y;
  save->w = frame->initial_state.w;
  save->h = frame->initial_state.h;
  save->state = minimized;
  save->available = 2;
  save->need_to_tile = 0;
  save->sticky = frame->sticky;
}

/**
@brief   Load the workspace independent frame details from a saved state.
@return  void
**/
static void
load_frame_state(Display *display, struct Saved_frame_state* save, struct Frame *frame, struct Separators *seps, struct Themes *themes, struct Atoms *atoms) {
  if(save->sticky == False) { //if the window has been sticky (but isn't any more), the saved details are wrong so just ignore them and keep the existing stuff.
    frame->x = save->x;
    frame->y = save->y;
    frame->w = save->w;
    frame->h = save->h;
    change_frame_state(display, frame, save->state, seps, themes, atoms);
    change_frame_mode(display, frame, save->mode, themes);
    #ifdef SHOW_STATES
    printf("Loading x %d, y %d, w %d, h %d, state %d.  %s\n", frame->x, frame->y, frame->w, frame->h, frame->state, frame->window_name);
    #endif
  }
}

/**
@brief  Looks at the number of used frames in all workspaces and checks if the maximum number of frames has been reached.  If max has been reached, it increases the available frame slots if possible.
@return True on success, False otherwise.
**/
static Bool
ensure_empty_frame_slots(struct Workspace_list *workspaces) {
  //increase the number of slots for frames if necessary
  if(workspaces->used_frames == workspaces->max_frames) {
    struct Frame* temp = realloc(workspaces->frame_list, sizeof(struct Frame) * workspaces->max_frames * 2);
    if(!temp) { perror("Error: could not reallocate frame list"); return False;}
    
    //all the saved pointers in all the workspaces are now invalid.  Time to update them.
    for(int k = 0; k < workspaces->used_workspaces; k++) {
      struct Workspace *frames = &workspaces->list[k];
      if(frames->list != NULL) {
        for(int i = 0; i < frames->used; i++) {
          int real_frame_index = get_offset_in_array(frames->list[i], workspaces->frame_list, sizeof(struct Frame));
          frames->list[i] = &temp[real_frame_index];
        }
      }
    }
    workspaces->frame_list = temp;

    //increase the number of states in each workspace to match the number of available frames
    for(int k = 0; k < workspaces->used_workspaces; k++) {
      struct Saved_frame_state* temp2 = clean_realloc(workspaces->list[k].states, sizeof(struct Saved_frame_state) * workspaces->max_frames, sizeof(struct Saved_frame_state) * workspaces->max_frames * 2);
      if(!temp2) { perror("Error: could not reallocate frame states list"); return False;}
      //lets say this loops fails halfway through, the window isn't mapped and there won't be any memory leaks.
      // If another windows is then added and there is more memory, it will just reallocate some to a memory slot the same size.
      workspaces->list[k].states = temp2;
    }
    workspaces->max_frames *= 2;
  }
  return True;
}

/**
@brief  Looks at the number of used pointers to frames in a specified workspace and checks if the maximum number of frames has been reached.  If max has been reached, it increases the available frame slots if possible.
@return True on success, False otherwise.
**/
static Bool
ensure_empty_frame_reference_slots(struct Workspace *workspace) {
  //number of references in the workspace reached.
  if(workspace->list  
  &&  workspace->used == workspace->max) {
    struct Frame** temp = NULL;
    temp = realloc(workspace->list, sizeof(struct Frame*) * workspace->max * 2);
    if(temp != NULL) workspace->list = temp;
    else return False;
    workspace->max *= 2;
  }
  return True;
}


/**
@brief    Adds a frame to a workspace, creating the workspace if required.
          Indirectly creates workspaces and frames.  Gets the program name from the frame using the class hints (load_program_name).
          If the program name doesn't match the name of any workspace, creates a new workspace.
          If the workspace is new, switch to the workspace.  Try and tile the frame if its mode is tiling.
          Decide whether to show the frame and whether to focus it. 
@return   the index of the workspace the frame was inserted into or -1 if an error occurred.
**/
int 
add_frame_to_workspace(Display *display, struct Workspace_list *workspaces, Window framed_window, int *current_workspace, struct Workspace **frames
, struct Popup_menu *window_menu
, struct Separators *seps
, struct Themes *themes, struct Cursors *cursors, struct Atoms *atoms) {

  int home_w;  //index of the home workspace of the frame
  int true_frame_index; //index of the frame in the global frame list in workspaces
  
  Bool new_workspace = False;  //boolean indicator to check if a workspace was just created.
  char *program_name = load_program_name(display, framed_window);
  
  if(program_name == NULL) {
    #ifdef SHOW_MAP_REQUEST_EVENT
    printf("Warning, could not load program name for window %lu. ", framed_window);  
    printf("Creating default workspace\n");
    #endif
    make_default_program_name(display, framed_window, "Other Programs");
    program_name = load_program_name(display, framed_window);
  }
  if(program_name == NULL) {perror("Error: out of memory"); return -1;} //probably out of memory
  
  for(home_w = 0; home_w < workspaces->used_workspaces; home_w++) {
    if(strcmp(program_name, workspaces->list[home_w].workspace_name) == 0) {
      XFree(program_name);
      break;
    }
  }
  if(home_w == workspaces->used_workspaces) { //create_statup workspaces only creates a workspace if there is at least one
    home_w = create_workspace(display, workspaces, program_name, themes);
    if(home_w < 0) { perror("Error: could not create new workspace\n"); return -1; }
    new_workspace = True;
  }

  if(ensure_empty_frame_slots(workspaces) == False) return -1;
  if(ensure_empty_frame_reference_slots(&workspaces->list[home_w]) == False) return -1;

  true_frame_index = workspaces->used_frames;
  if(!create_frame(display, &workspaces->frame_list[true_frame_index], framed_window, window_menu, seps, themes, cursors, atoms)) {
    //if the window wasn't created, and the workspace is now empty, remove the workspace
    if(new_workspace) { remove_workspace(display, workspaces, home_w);  }
    return -1;
  }
 
  workspaces->used_frames++;   
  #ifdef SHOW_MAP_REQUEST_EVENT
  printf("Workspace %d, real frame index %d, frame_name %s, window %lu, workspace_name %s\n", home_w, true_frame_index
  , workspaces->frame_list[true_frame_index].window_name, framed_window, workspaces->list[home_w].workspace_name);
  #endif

  check_and_set_new_frame_focus (display, &workspaces->frame_list[true_frame_index], &workspaces->list[home_w]);
  
  //check if it's neutral
  Bool show_in_other_workspaces = suitable_for_foreign_workspaces(&workspaces->frame_list[true_frame_index]);
  enum Window_state original_state = workspaces->frame_list[true_frame_index].state; //in case it is fullscreen
  
  { //save state of window in home workspace and other workspaces if it's meant to be shown in other workspaces.
    //Otherwise the saved state entry will simply zero'd.
    int workspace_index = 0;

    do {
      if(!show_in_other_workspaces) workspace_index = home_w; //only do one pass if we don't want to make it available

      workspaces->list[workspace_index].states[true_frame_index].need_to_tile = 0; //reset
       
      if(workspace_index == home_w) {
        workspaces->list[workspace_index].states[true_frame_index].available = 1;
        if(workspaces->frame_list[true_frame_index].mode == tiling  &&  home_w != *current_workspace) {
          workspaces->list[workspace_index].states[true_frame_index].need_to_tile = 1; 
        }
        change_frame_state(display, &workspaces->frame_list[true_frame_index], original_state, seps, themes, atoms);
        save_frame_state(&workspaces->list[workspace_index].states[true_frame_index], &workspaces->frame_list[true_frame_index]);   
      }
      else if (show_in_other_workspaces) {
        workspaces->list[workspace_index].states[true_frame_index].available = 2;
        change_frame_state(display, &workspaces->frame_list[true_frame_index], minimized, seps, themes, atoms);
        save_frame_state(&workspaces->list[workspace_index].states[true_frame_index], &workspaces->frame_list[true_frame_index]);   
      }

      workspace_index++;
    }
    while(show_in_other_workspaces  &&  workspace_index < workspaces->used_workspaces);
  }

  if(home_w == *current_workspace  &&  *current_workspace != -1) { //new window is in the current workspace
     //save the reference
    struct Workspace *workspace = &workspaces->list[*current_workspace];
    int frame_ref_index = workspace->used;
    workspace->list[frame_ref_index] = &workspaces->frame_list[true_frame_index];
    workspace->used++;
    
    change_frame_state(display, &workspaces->frame_list[true_frame_index], original_state, seps, themes, atoms);
    if(workspace->list[frame_ref_index]->mode == tiling  &&  workspace->list[frame_ref_index]->state != fullscreen) {
      if(!redrop_frame(display, workspace, frame_ref_index, themes)) {
        change_frame_state(display, workspace->list[frame_ref_index], minimized, seps, themes, atoms);
      }
    }
    #ifdef SHOW_MAP_REQUEST_EVENT      
    printf("Created and mapped window in workspace %d\n", *current_workspace);
    #endif
    if(workspaces->frame_list[true_frame_index].state != minimized) XMapWindow(display, workspace->list[frame_ref_index]->widgets[frame_parent].widget);
    XMapWindow(display, workspace->list[frame_ref_index]->menu.item);
    if(workspace->list[frame_ref_index]->focussed) {
      recover_focus(display, &workspaces->list[*current_workspace], themes, atoms);
    }

    update_client_list(display, &workspaces->list[*current_workspace], atoms);
  }
    
  if(home_w != *current_workspace) {
    change_to_workspace(display, workspaces, current_workspace, frames, home_w, seps, themes, atoms);
  }
  
  XFlush(display);
  return home_w;
}

/**
@brief    Adds all frames to workspaces.  Called when the window manager is starting up.
@return   True on success, False on failure.
**/
Bool
create_startup_workspaces(Display *display, struct Workspace_list *workspaces
, int *current_workspace, struct Workspace **frames
, struct Separators *seps
, struct Popup_menu *window_menu, struct Themes *themes, struct Cursors *cursors, struct Atoms *atoms) {

  unsigned int windows_length;
  Window root, parent, children, *windows;
  XWindowAttributes attributes;
  root = DefaultRootWindow(display);

  workspaces->frame_list = malloc(sizeof(struct Frame) * workspaces->max_frames);
  if(!workspaces->frame_list) return False;
  
  create_workspaces_menu(display, workspaces, themes, cursors);  

  XQueryTree(display, root, &parent, &children, &windows, &windows_length);  

  if(windows != NULL) {
    for(unsigned int i = 0; i < windows_length; i++)  {
      XGetWindowAttributes(display, windows[i], &attributes);
      if(attributes.map_state == IsViewable && !attributes.override_redirect)  {

        add_frame_to_workspace(display, workspaces, windows[i], current_workspace, frames, window_menu
        , seps, themes, cursors, atoms);
      }
    }
    XFree(windows);
  }
  return True;
}

/**
@pre      all parameters intitalized and allocated properly.
@brief    This function changes the user's workspace to the workspace at the specified index.  
          If a negative index is passed (This is done when the currently open workspace is removed), it changes to a default workspace which is currently 0.
          If a negative index is passed but no workspace is open nothing happens.
          Generally, it is expected that at least one workspace is open.
          Windows from other workspaces are unmapped.
@post     The user's workspace has visibly changed.
@return   void
**/
void 
change_to_workspace(Display *display, struct Workspace_list *workspaces, int *current_workspace, struct Workspace **frames, int index, struct Separators *seps, struct Themes *themes, struct Atoms *atoms) {
  
  struct Workspace *workspace = &workspaces->list[*current_workspace];
  if(*current_workspace != -1) *frames = workspace;
  
  struct Saved_frame_state *frame_state;
  if(workspaces->used_workspaces == 0) {
    *frames = NULL;
    *current_workspace = -1;
    return; //don't do anything if no windows are open
  }

  if(*current_workspace < workspaces->used_workspaces /*this function is sometimes called to change from an invalid workspace that has been closed */
  &&  *current_workspace >= 0) {
    //XUnmapWindow(display, workspace->virtual_desktop);
    for(int i = 0; i < workspace->used; i++) {
      //So we can figure out where to save the frame state we need to calculate the pointer offset.
      int real_frame_index = get_offset_in_array(workspace->list[i], workspaces->frame_list, sizeof(struct Frame));
      
      if(workspace->list[i]->sticky == False) {
        XUnmapWindow(display, workspace->list[i]->widgets[frame_parent].widget);
        XUnmapWindow(display, workspace->list[i]->menu.item);
      }
      //printf("real_frame_index %d, i = %d, current_workspace %d\n", real_frame_index, i, *current_workspace);
      frame_state = &workspace->states[real_frame_index];
      frame_state->need_to_tile = 0;
      save_frame_state(frame_state, workspace->list[i]);
    }
    free(workspace->list);
    workspace->list = NULL;
    workspace->used = 0;
  }
  XSync(display, False);
  
  //if index is -1, change to default workspace which is 0
  if(index < 0  &&  workspaces->used_workspaces > 0) index = 0;
  *current_workspace = index;
  
  workspace = &workspaces->list[*current_workspace];
  *frames = workspace;
  //XMapWindow(display, workspace->virtual_desktop);

  workspace->max = DEFAULT_STARTING_FRAMES;
  workspace->list = malloc(sizeof(struct Frames *) * workspace->max );
  if(!workspace->list) { perror("Couldn't allocate frame list"); return; }

  //Do all the panels because we want them to be tiled first.
  for(int i = 0; i < workspaces->used_frames; i++) {
    if(workspaces->frame_list[i].sticky == True) {
      if(ensure_empty_frame_reference_slots(workspace) == False) { perror("Couldn't allocate frame list"); return; }

      int ref_index = workspace->used;
      struct Frame *frame = workspace->list[ref_index] = &workspaces->frame_list[i];
      if(drop_frame(display, workspace, ref_index, False, themes)) { //this should be easy as they should already be non-overlapping
        change_frame_mode(display, frame, tiling, themes);
      }
      else {
        change_frame_state(display, frame, minimized, seps, themes, atoms);
        fprintf(stderr, "Couldn't tile panel! It is called %s\n", frame->window_name);
      }
      resize_frame(display, frame, themes);
      stack_frame(display, frame, seps);
      XMapWindow(display, frame->widgets[frame_parent].widget);  //Actually only needs to be mapped the first time but doesn't matter
      workspace->used++;
    }
  }
  

  for(int i = 0; i < workspaces->used_frames; i++) {
    frame_state =  &workspace->states[i];
    if(frame_state->available != 0  &&  frame_state->sticky == False) {
      if(ensure_empty_frame_reference_slots(workspace) == False) { perror("Couldn't allocate frame list"); return; }

      int ref_index = workspace->used;
      struct Frame *frame = workspace->list[ref_index] = &workspaces->frame_list[i];
      
      load_frame_state(display, frame_state, frame, seps, themes, atoms);
      if(workspace->states[i].need_to_tile) {
        if(drop_frame(display, workspace, ref_index, False, themes)) { //this should be easy as they should already be non-overlapping
          change_frame_mode(display, frame, tiling, themes);
        }
        else {
          change_frame_state(display, frame, minimized, seps, themes, atoms);
          //TODO set urgency hint
        }
        workspace->states[i].need_to_tile = 0;
      }
      else if(frame->mode == floating) {
        if(!drop_frame(display, workspace, ref_index, True, themes)) {
          change_frame_state(display, frame, minimized, seps, themes, atoms);
          //TODO set urgency hint
        }        
      }
      resize_frame(display, frame, themes);
      stack_frame(display, frame, seps);
      if(frame->state != minimized) XMapWindow(display, frame->widgets[frame_parent].widget); 
      XMapWindow(display, workspace->list[ref_index]->menu.item);
      workspace->used++;
    }
  }
  update_client_list(display, workspace, atoms);  
  // printf("changing focus to one in new workspace\n");
  recover_focus(display, workspace, themes, atoms);
  XFlush(display);

}

/**
@brief Update the list of windows managed by the window manager.
@todo  Future versions should save list of managed windows in the workspace along with the pointers to the frames to improve performance.
@todo  Stacking order is not correctly reported.
**/

void 
update_client_list(Display *display, struct Workspace *frames, struct Atoms *atoms) {
/**
  _NET_CLIENT_LIST

  _NET_CLIENT_LIST, WINDOW[]/32
  _NET_CLIENT_LIST_STACKING, WINDOW[]/32

  These arrays contain all X Windows managed by the Window Manager
  . _NET_CLIENT_LIST has initial mapping order, starting with the oldest window
  . _NET_CLIENT_LIST_STACKING has bottom-to-top stacking order.
   These properties SHOULD be set and updated by the Window Manager. 
**/ 
  Window root = DefaultRootWindow(display);
  Window *windows = malloc(sizeof(Window) * frames->used);
  for(int i = 0; i < frames->used; i++) windows[i] = frames->list[i]->framed_window;
  if(!windows) { perror("Out of memory to update client list"); return;}
  
  XChangeProperty(display, root, atoms->client_list, XA_WINDOW, 32, PropModeReplace, (unsigned char *)windows, frames->used);
  XChangeProperty(display, root, atoms->client_list_stacking, XA_WINDOW, 32, PropModeReplace, (unsigned char *)windows, frames->used);
  free(windows);
  return;
}

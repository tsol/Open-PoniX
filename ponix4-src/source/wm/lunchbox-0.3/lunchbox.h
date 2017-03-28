#include <cairo/cairo.h> //because the Font_theme struct uses cairo types
#include <cairo/cairo-xlib.h>
#include "defs.h"

#define M_PI 3.14159265359

#define M_DOUBLE_MAX 1.7e+308 

#define MAX_WM_NAME_LENGTH 200
#define PIXMAP_SIZE 16
//#define DEFAULT_MENU_ITEM_WIDTH 90 /* This is the starting width for the mode menu */
#define MODE_ICON_SIZE 14
//#define MINWIDTH 200
#define MINWIDTH 100
#define MINHEIGHT 10
#define PATH_SIZE 400
#define WIDGET_NAME_SIZE 50
#define DEFAULT_MENU_ITEM_WIDTH 100
#define DOUBLE_CLICK_MILLISECONDS 600 /* Eventually figure out a standard complient way to get this info*/
#define DEFAULT_STARTING_FRAMES 8
/****THEME DERIVED CONSTANTS******/

/**
@file     lunchbox.h
@brief    Global header file for the lunchbox window manager.
@author   Alysander Stanley
**/

enum Splash_widget {
  splash_parent
};

enum Menubar_widget {
  program_menu,
  window_menu,
  options_menu,
  links_menu,
  tool_menu,
  menubar_parent      /* menubar_parent must be last      */
}; 

enum Popup_menu_widget {
  menu_item_lhs,
  menu_item_mid, /* the middle will be tiled for wider or thinner popups */
  menu_item_rhs,
  popup_t_edge,
  popup_l_edge,
  popup_b_edge,
  popup_r_edge,
  popup_tl_corner,
  popup_tr_corner,
  popup_bl_corner,
  popup_br_corner,    
  popup_menu_parent /* popup_menu_parent must be last */
};

enum Frame_widget {
  window,
  t_edge,
  titlebar,
  l_edge,
  b_edge,
  r_edge,
  tl_corner,
  tr_corner,
  bl_corner,
  br_corner,
  selection_indicator,
  selection_indicator_hotspot,  
  title_menu_lhs,
  title_menu_icon,
  title_menu_text,   //fill -- must be before rhs and hotspot for minimum size in resize frame
  title_menu_rhs,    //includes arrow
  title_menu_hotspot,
  //mode dropdown must follow a title menu
  mode_dropdown_lhs, //sets the title and the icon
  mode_dropdown_text,
  mode_dropdown_text_floating, //this is not directly loaded from the theme, but are based on the mode_dropdown_lhs plus text and icon
  mode_dropdown_text_tiling,   //this is not directly loaded from the theme, but are based on the mode_dropdown_lhs plus text and icon
  mode_dropdown_text_desktop,  //this is not directly loaded from the theme, but are based on the mode_dropdown_lhs plus text and icon
  mode_dropdown_rhs, //includes arrow
  mode_dropdown_hotspot,
  close_button,
  close_button_hotspot,

  frame_parent  /*frame parent must be last */
};

enum Window_mode {
  floating,
  tiling,
  desktop,
  hidden, /* add new modes above this line (this line is hidden mode) */
  unset   /* must be after hidden */ /* This is required for the first change_frame_mode */
};

enum Window_state {
  none,
  fullscreen,
  minimized
  //lurking,       /* The lurking state will be used when the window attemps to tile and fails*/  
};

/**
@brief  this defines the available window types.  Some of these are interpretted from EWMH hints, others will need an additional protocol which hasn't been developed.
**/
enum Window_type {
  unknown, /**< No type could be determined, use a default style. **/
  file,    /**< The window represents a file such as a document or image. **/
  program, /**< The window is a program. **/
  dialog,  /**< Traditional dialog box/transient window. **/
  modal_dialog, /**< The window forces the user to deal with it before continuing with their task. **/
  utility, /**< The window is for interacting indirectly with another window. **/
  status,  /**< The window is not interactive, it merely communicates status information (e.g., a clock) .**/
  panel,   /**< The window is a panel and is almost unmanaged. **/
  system_program, /**< must be last, items following this will not be included in the struct Themes */
  splash /**< this is ignored in the themes as splash screens are not managed **/
};


/**
@brief These are the possible states that each Widget can be in.    Some clarification of widget state terminology.
  For a button, "active" is its pressed state.
  For a checkbox, "active" is its checked state.
  For a menu, "active" means that it is the already chosen item, or that the submenu is open.
  Therefore, the text from a drop down list should be look similar to the "active" element in the list.
  For example bold.
**/
enum Widget_state {
  normal, /**< Normal State**/
  active, /**< Pressed/checked/chosen item **/
  normal_hover, /**< for when the pointer is above a normal widget **/
  active_hover, /**< for when the pointer is above an activated widget **/ 
  normal_focussed,  /**< for when a normal widget is focussed **/
  active_focussed,  /**< for when an activated widget is focussed **/
  normal_focussed_hover, /**< for when a normal widget is focussed and has a pointer above it **/
  active_focussed_hover, /**< for when an activated widget is focussed and has a pointer above it **/
  inactive    /**< widget is unresponsive - must be last in this list **/
};

struct Widget {
  Window widget; /**<  This window is a container and is used to select events **/
  Window state[inactive + 1];   /**<  These windows can be raised to change the visible window state **/
};

struct Widget_theme {
  char exists;   /**<  used to indicate whether a theme has been loaded for this widget. **/
  int x,y,w,h;   /**<  values wrap around window.  w or h of zero is frame width or height. **/
  Pixmap state_p[inactive + 1]; /**< The stored graphical data/different backgrounds. **/
};

struct Font_theme {
  char font_name[20];
  float size;
  float r,g,b,a;
  unsigned int x,y;
  cairo_font_slant_t slant;
  cairo_font_weight_t weight;
};

struct Themes { //these are all individually malloc'd, and window type is an array of malloc'd arrays
  struct Widget_theme *window_type[system_program + 1]; 
  struct Widget_theme *menubar;
  struct Widget_theme *popup_menu;
  struct Font_theme font_theme[inactive + 1];
  int mode_pulldown_width;
};

struct Popup_menu {
  struct Widget widgets[popup_menu_parent + 1];
  unsigned int inner_width;
  unsigned int inner_height;
};

struct Menu_item {
  //these are the items for the title menu and window menu
  Window hotspot;
  Window item;
  Window state[inactive + 1];

  int width;
};

struct Mode_menu {
  struct Popup_menu menu;
  struct Menu_item items[hidden + 1];
};

struct Menubar {
  struct Widget widgets[menubar_parent + 1];
};

/**
@brief  Separators for stacking windows.  These are unmapped windows that are placeholders and used for ensuring windows are on the correct "layer".
**/
struct Separators {
  Window sinking_separator;  /**< this window is always above desktop windows. **/
  Window tiling_separator;   /**< this window is always above tiled windows. **/
  Window floating_separator; /**< this window is always above floating windows.**/
  Window panel_separator;    /**< this window is always above panels and taskbars.**/
};

struct Frame {
  char *window_name;

  int x,y,w,h;

  struct {
    int x,y,w,h;
    enum Window_mode mode;
  } initial_state;  /**< stores the starting values for the window so that when it is made available in another workspace it has meaningful defaults. **/
  
  enum Window_mode mode;  /**< the stacking mode. **/
  
  int frame_hspace, frame_vspace;  /**< amount used by the frame theme **/
  enum Window_type type;     
  enum Window_type theme_type;  /**< this is used in case the theme is actually from a different window type **/
  enum Window_state state;      /**< currently fullscreen, minimized or none **/
  Bool focussed;
  Bool sticky;
  Bool wants_attention;  //not currently being used.
  int min_width, max_width; //includes hspace
  int min_height, max_height; //includes vspace
  int vspace; //dependent on the window type
  int hspace; //dependent on the window type
  int width_inc;  //increments for incremental resize
  int height_inc; //increments for incremental resize
  int w_inc_offset;
  int h_inc_offset;
  
  Window transient; //the calling window of this dialog box - not structural

  Window framed_window; //the window which is reparented.

  struct Widget widgets[frame_parent + 1];

  struct Menu_item menu; //this contains icons used in the window menu and the title menu

  struct { //these is used during tiling resize operations.
    int new_position;
    int new_size;
  } indirect_resize;
};

/**
@brief  Focus_list contains a list of framed windows from the current workspace in the LRU order. The Windows can be focussed using XSetFocus Directly.
**/
struct Focus_list {
  unsigned int used, max;
  Window* list;
};

/**
@brief Because some frames can be in multiple workspaces we need a struct like this to save the details which differ for frames between workspaces.
**/
struct Saved_frame_state {
  int x,y,w,h;
  int available; //2 = make available in other  workspaces, 1 = in it's home workspace, 0 = not available in this workspace
  Bool need_to_tile; //1 = yes, 0 = no
  Bool sticky;
  enum Window_mode mode;
  enum Window_state state;
};

/**
@brief  A seperate arrangement of windows.  Contains references to a frame, keeps track of its focus list.
**/
struct Workspace {
  int used, max; /**< number of used frame reference and max number of frames **/
  struct Frame** list; /**< array of references to frames **/
  struct Focus_list focus; /**< keeps track of the recently focussed windows so that focus can be restored when a windows is closed in the workspace **/
  struct Saved_frame_state* states; /**< should always be the same size as the global framelist **/

  char *workspace_name; /**< UTF8 formatted name, but probably non latin characters won't be outputted correctly **/
  
  struct Menu_item workspace_menu; /**< this the workspaces menu item **/
  
  //Window virtual_desktop; /**< The background of this is the same as the root, so it's an easy way to get the root to redraw by mapping and unmapping this **/
};

/**
@brief Keeps track of all the workspaces/open programs. Be careful which list is referenced as there are two arrays here.
**/
struct Workspace_list {
  int used_workspaces, max_workspaces; /**< number of used_workspaces and number of allocated slots for workspaces. This must be an int because index may initially be set as -1 **/
  struct Workspace* list; /**< contains all the workspace structures **/
  struct Popup_menu workspace_menu; /**< the workspace menu contains the list of open programs **/
  
  int used_frames, max_frames;  /**< number of used frames and number of allocated slots for frames **/  
  struct Frame* frame_list; /**< frame_list contains all the frames structures **/
};

/**
@brief this struct contains references to XCursors that are used to change the mouse pointer when the user grabs or presses certain widgets.
**/
struct Cursors {
  Cursor normal, hand, grab, pressable, resize_h, resize_v, resize_tr_bl, resize_tl_br;
};

/**
@brief contains references to stored strings in the X server.  Used for protocol communications with programs.
**/
struct Atoms {
  Atom name                    /** "WM_NAME"  **/
  , normal_hints               /** "WM_NORMAL_HINTS"  **/

  //make sure this is the first Extended window manager hint
  , supporting_wm_check        /** "_NET_SUPPORTING_WM_CHECK"      **/
  , client_list                /** "_NET_CLIENT_LIST"  **/
  , client_list_stacking       /** "_NET_CLIENT_LIST_STACKING" **/
  , active_window              /** "_NET_ACTIVE_WINDOW" **/
  , number_of_desktops         /** "_NET_NUMBER_OF_DESKTOPS" //always 1  **/
  , current_desktop            /** "_NET_CURRENT_DESKTOP" //always 0 **/
  , showing_desktop            /** "_NET_SHOWING_DESKTOP" //always 1 **/
  , desktop_geometry           /** "_NET_DESKTOP_GEOMETRY" //this is currently the same size as the screen   **/
  , desktop_viewport           /** "_NET_DESKTOP_VIEWPORT" //this is always 0, 0 **/
  , workarea                   /** "_NET_WORKAREA" //this will be minus the menubar at the bottom of the screen - added because absense causes a netbook-launcher divide by 0 exception   **/
  , wm_full_placement          /** "_NET_WM_FULL_PLACEMENT"   **/
  , wm_icon                    /** "_NET_WM_ICON"  **/
  , frame_extents              /** "_NET_FRAME_EXTENTS"  **/
  , wm_window_type             /** "_NET_WM_WINDOW_TYPE"  **/
  , wm_window_type_normal      /** "_NET_WM_WINDOW_TYPE_NORMAL"  **/
  , wm_window_type_dock        /** "_NET_WM_WINDOW_TYPE_DOCK"  **/
  , wm_window_type_desktop     /** "_NET_WM_WINDOW_TYPE_DESKTOP"    **/
  , wm_window_type_splash      /** "_NET_WM_WINDOW_TYPE_SPLASH"    **/
  , wm_window_type_dialog      /** "_NET_WM_WINDOW_TYPE_DIALOG"  //can be transient  **/
  , wm_window_type_utility     /** "_NET_WM_WINDOW_TYPE_UTILITY" //can be transient  **/
  , wm_strut_partial
  , wm_state                   /** "_NET_WM_STATE"   **/
  , wm_state_above             /** "_NET_WM_STATE_ABOVE"   **/  
  , wm_state_below             /** "_NET_WM_STATE_ABOVE"   **/
  , wm_state_hidden            /** "_NET_WM_STATE_HIDDEN"   **/
  , wm_state_demands_attention /** "_NET_WM_STATE_DEMANDS_ATTENTION"   **/
  , wm_state_modal             /** "_NET_WM_STATE_MODAL"  //can be transient - for the specified window   **/
  , wm_state_fullscreen;       /** "_NET_WM_STATE_FULLSCREEN"  **/
  //make sure this comes last  

  Atom supported;                  /** "_NET_SUPPORTED"  **/
  //this is a type
  Atom utf8; 
};

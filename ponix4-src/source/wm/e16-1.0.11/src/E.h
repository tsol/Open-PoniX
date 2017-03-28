/*****************************************************************************/
/* Enlightenment - The Window Manager that dares to do what others don't     */
/*****************************************************************************/
/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2012 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "config.h"

#include "alert.h"
#include "edebug.h"
#include "lang.h"
#include "sounds.h"
#include "util.h"

#include <X11/X.h>

#include "xtypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FILEPATH_LEN_MAX 4096

#ifndef MAX
#define MAX(a,b)  ((a)>(b)?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b)  ((a)<(b)?(a):(b))
#endif

#define IN_RANGE(a, b, range) \
   ((((a) >  (b)) && ((a) - (b) <= (range))) || \
   (((a) <= (b)) && ((b) - (a) <= (range))))

#define IN_ABOVE(a, b, range) \
   (((a) >=  (b)) && ((a) - (b) <= (range)))

#define IN_BELOW(a, b, range) \
   (((a) <= (b)) && ((b) - (a) <= (range)))

#define SPANS_COMMON(x1, w1, x2, w2) \
   (!((((x2) + (w2)) <= (x1)) || ((x2) >= ((x1) + (w1)))))

#define STATE_NORMAL            0
#define STATE_HILITED           1
#define STATE_CLICKED           2
#define STATE_DISABLED          3

#define FLAG_BUTTON             0
#define FLAG_TITLE              1
#define FLAG_MINIICON           2
#define FLAG_FIXED              4
#define FLAG_FIXED_HORIZ        8
#define FLAG_FIXED_VERT         16

#define MODE_FOCUS_POINTER      0
#define MODE_FOCUS_SLOPPY       1
#define MODE_FOCUS_CLICK        2

#define EDGE_FLIP_OFF           0
#define EDGE_FLIP_ON            1
#define EDGE_FLIP_MOVE          2

#define DOCK_LEFT               0
#define DOCK_RIGHT              1
#define DOCK_UP                 2
#define DOCK_DOWN               3

#define ICON_LEFT               0
#define ICON_RIGHT              1
#define ICON_UP                 2
#define ICON_DOWN               3

#define MODE_NONE                 0
#define MODE_MOVE_PENDING         1
#define MODE_MOVE                 2
#define MODE_RESIZE               3
#define MODE_RESIZE_H             4
#define MODE_RESIZE_V             5
#define MODE_DESKDRAG             6
#define MODE_BUTTONDRAG           7
#define MODE_DESKRAY              8
#define MODE_PAGER_DRAG_PENDING   9
#define MODE_PAGER_DRAG          10

#define EVENT_MOUSE_DOWN  0
#define EVENT_MOUSE_UP    1
#define EVENT_MOUSE_ENTER 2
#define EVENT_MOUSE_LEAVE 3
#define EVENT_KEY_DOWN    4
#define EVENT_KEY_UP      5
#define EVENT_DOUBLE_DOWN 6
#define EVENT_FOCUS_IN    7
#define EVENT_FOCUS_OUT   8

/* Server extensions */
#define XEXT_SHAPE       0
#define XEXT_XINERAMA    1
#define XEXT_SYNC        2
#define XEXT_SCRSAVER    3
#define XEXT_RANDR       4
#define XEXT_COMPOSITE   5
#define XEXT_DAMAGE      6
#define XEXT_FIXES       7
#define XEXT_RENDER      8
#define XEXT_GLX         9
#define XEXT_XI         10

#define XEXT_CM_ALL     16

#define XEXT_AVAILABLE(ext)  (Mode.server.extensions & (1 << ext))

/*
 * Types
 */
#include "etypes.h"

/* Configuration parameters */
typedef struct {
   struct {
      unsigned int        step;	/* Animation time step, ms */
   } animation;
   struct {
      char                enable;
      int                 delay;	/* milliseconds */
   } autoraise;
   struct {
      char                hiquality;
      char                user;
      char                no_scan;
      int                 timeout;
   } backgrounds;
   struct {
      int                 move_resistance;
   } buttons;
   struct {
      unsigned int        num;
      int                 dragdir;
      int                 dragbar_width;
      int                 dragbar_length;
      int                 dragbar_ordering;
      char                desks_wraparound;
      char                slidein;
      int                 slidespeed;
      int                 areas_nx;
      int                 areas_ny;
      char                areas_wraparound;
      int                 edge_flip_mode;
      int                 edge_flip_resistance;
   } desks;
   struct {
      char                headers;
      char                button_image;
   } dialogs;
   struct {
      char                enable;	/* wmdockapp only */
      char                sticky;	/* Make dockapps sticky by default */
      int                 dirmode;
      int                 startx;
      int                 starty;
   } dock;
   struct {
      int                 mode;
      char                clickraises;
      char                transientsfollowleader;
      char                switchfortransientmap;
      char                all_new_windows_get_focus;
      char                new_windows_get_focus_if_group_focused;
      char                new_transients_get_focus;
      char                new_transients_get_focus_if_group_focused;
      char                raise_on_next;
      char                warp_on_next;
      char                warp_always;
   } focus;
   struct {
      char                set_xroot_info_on_root_window;
   } hints;
   struct {
      char                dest;
      char                difftime;
   } log;
   struct {
      int                 zoom_res;
   } magwin;
   struct {
      char                animate;
      char                onscreen;
      char                warp;
      char                show_icons;
      int                 icon_size;
      struct {
	 KeySym              left, right, up, down, escape, ret;
      } key;
   } menus;
   struct {
      int                 mode_move;
      int                 mode_resize;
      int                 mode_info;
      int                 mode_maximize_default;
      unsigned int        color;
      char                avoid_server_grab;
      char                update_while_moving;
      char                enable_sync_request;
      char                dragbar_nocover;
      char                enable_smart_max_hv;
      char                maximize_animate;
      int                 maximize_speed;
   } movres;
   struct {
      int                 movres;
      int                 menus;
      int                 tooltips;
      int                 focused;
      int                 unfocused;
   } opacity;
   struct {
      char                manual;
      char                manual_mouse_pointer;
      char                center_if_desk_full;
      char                ignore_struts;
      char                ignore_struts_fullscreen;
      char                ignore_struts_maximize;
      char                raise_fullscreen;
      char                slidein;
      char                cleanupslide;
      int                 slidemode;
      int                 slidespeedmap;
      int                 slidespeedcleanup;
#ifdef USE_XINERAMA_no		/* Not implemented */
      char                extra_head;	/* Not used */
#endif
   } place;
   struct {
      char                enable_script;
      char               *script;
      char                enable_logout_dialog;
      char                enable_reboot_halt;
      char               *cmd_reboot;
      char               *cmd_halt;
   } session;
   struct {
      char                animate;
      int                 speed;
   } shading;
   struct {
      char                enable;
      int                 edge_snap_dist;
      int                 screen_snap_dist;
   } snap;
   struct {
      char                firsttime;
      char                animate;
   } startup;
   struct {
      char                use_theme_font_cfg;
      char                use_alt_font_cfg;
      char               *name;
      char               *extra_path;
      char               *font_cfg;
   } theme;
#ifdef ENABLE_THEME_TRANSPARENCY
   struct {
      int                 alpha;
      int                 border;
      int                 widget;
      int                 iconbox;
      int                 menu;
      int                 menu_item;
      int                 tooltip;
      int                 dialog;
      int                 hilight;
      int                 pager;
      int                 warplist;
   } trans;
#endif
   struct {
      char                enable;
      char                showsticky;
      char                showshaded;
      char                showiconified;
      char                showalldesks;
      char                warpfocused;
      char                raise_on_select;
      char                warp_on_select;
      char                show_shape;
      int                 icon_mode;
   } warplist;

   struct {
      char                argb_internal_objects;
      char                argb_internal_clients;
      char                argb_clients;
      char                argb_clients_inherit_attr;
      int                 image_cache_size;
      int                 mask_alpha_threshold;
      char                enable_startup_id;
      char                use_render_for_scaling;
      char                bindings_reload;
      unsigned int        no_sync_mask;
   } testing;

   char                autosave;
   char                memory_paranoia;
   char                save_under;
} EConf;

typedef struct {
   struct {
      unsigned int        startup_id;
   } apps;
   struct {
      char                force_scan;
      unsigned int        mini_w, mini_h;
   } backgrounds;
   struct {
      char               *name;
      char               *prefix;
      char               *dir;
      char               *cache_dir;
   } conf;
   struct {
      char                dummy;	/* Prevent empty struct */
#ifdef USE_XINERAMA
      char                xinerama_active;
#endif
#if USE_XSYNC
      XID                 server_time;
#endif
   } display;
   struct {
      unsigned int        time_ms;	/* Local ms time */
      Time                time;	/* Latest X event time */
      int                 cx, cy;	/* Any detected pointer movement */
      int                 mx, my;	/* Motion event */
      int                 px, py;	/* Previous motion event */
      Time                last_btime;
      Window              last_bpress;
      Window              last_bpress2;
      unsigned int        last_button;
      unsigned int        last_keycode;
      unsigned int        last_keystate;
      char                double_click;
      char                on_screen;
#if USE_COMPOSITE
      unsigned int        damage_count;
#endif
   } events;
   struct {
      char                pointer_grab_active;
      Window              pointer_grab_window;
   } grabs;
   struct {
      const char         *lang;
      char                utf8_int;	/* Use UTF-8 internally */
      char                utf8_loc;	/* Locale is UTF-8 */
   } locale;
   struct {
      unsigned int        mod_key_mask;
      unsigned int        mod_combos[8];
   } masks;
   struct {
      char                swap;
   } move;
   struct {
      signed char         enable_features;
      char                doing_manual;
   } place;
   struct {
      int                 w_old;
      int                 h_old;
#ifdef USE_XRANDR
      unsigned short      rotation;
#endif
   } screen;
   struct {
      unsigned int        extensions;
   } server;
   struct {
      Pixmap              ext_pmap;
      char                ext_pmap_valid;
   } root;
   struct {
      char               *paths;
      char               *path;
      char               *variant;
   } theme;
   struct {
      char               *exec_name;	/* argv[0] */
      char                master;	/* We are the master E */
      char                single;	/* No slaves */
      char                window;	/* Running in virtual root window */
      pid_t               pid;
      int                 master_screen;
      char                session_start;
      char                startup;
      char                restart;
      char                exiting;
      char                in_signal_handler;
      char                exit_now;
      char                save_ok;
      char                coredump;
      int                 win_x, win_y, win_w, win_h;
      int                 exit_mode;
      char               *exit_param;
      int                 child_count;
      pid_t              *children;
      char               *machine_name;
   } wm;
   int                 mode;
   int                 op_source;
   EWin               *focuswin;
   EWin               *mouse_over_ewin;
   EWin               *context_ewin;
   Colormap            current_cmap;
   Win                 context_win;
   char                constrained;
   char                nogroup;
   char                showing_desktop;
   Window              button_proxy_win;
   char                firsttime;
   char                debug_exit;
} EMode;

/*
 * Function prototypes
 */

/* about.c */
void                About(void);

/* actions.c */
#define EXEC_SET_LANG       0x01
#define EXEC_SET_STARTUP_ID 0x02
void                EexecCmd(const char *cmd);
int                 EspawnApplication(const char *params, int flags);
void                Espawn(int argc, char **argv);
void                EspawnCmd(const char *cmd);
int                 Esystem(const char *cmd);

/* config.c */
#define FILE_TYPE_CONFIG     0
#define FILE_TYPE_BACKGROUND 1
#define FILE_TYPE_IMAGE      1
#define FILE_TYPE_CURSOR     1
#define FILE_TYPE_SOUND      1
#define FILE_TYPE_MENU       2
#define FILE_TYPE_ICON       3
void                SkipTillEnd(FILE * ConfigFile);
char               *GetLine(char *s, int size, FILE * f);
int                 ConfigParseline1(char *str, char *s2, char **p2, char **p3);
void                ConfigParseError(const char *where, const char *line);
void                ConfigAlertLoad(const char *txt);
char               *FindFile(const char *file, const char *themepath, int type);
char               *ThemeFileFind(const char *file, int type);
int                 ConfigFileLoad(const char *name, const char *themepath,
				   int (*parse) (FILE * fs), int preparse);
int                 ConfigFileRead(FILE * fs);
int                 ThemeConfigLoad(void);

/* dialog.c */
#if ENABLE_DIALOGS
void __PRINTF_2__   DialogOK(const char *title, const char *fmt, ...);
void                DialogOKstr(const char *title, const char *txt);
#else
#define DialogOK(title, fmt, ...) do {} while(0)
#define DialogOKstr(title, fmt) do {} while(0)
#endif /* ENABLE_DIALOGS */

/* draw.c (move to moveresize.h?) */
/* Move/resize modes */
#define MR_OPAQUE       0
#define MR_TECHNICAL    1
#define MR_BOX          2
#define MR_SHADED       3
#define MR_SEMI_SOLID   4
#define MR_TRANSLUCENT  5
#define MR_TECH_OPAQUE  6

int                 MoveResizeModeValidateMove(int md);
int                 MoveResizeModeValidateResize(int md);

/* econfig.c */
void                ConfigurationLoad(void);
void                ConfigurationSave(void);
void                ConfigurationSet(const char *params);
void                ConfigurationShow(const char *params);

/* edge.c */
void                EdgeCheckMotion(int x, int y);
void                EdgeWindowsShow(void);
void                EdgeWindowsHide(void);

/* extinitwin.c */
Window              ExtInitWinCreate(void);
void                ExtInitWinSet(Window win);
Window              ExtInitWinGet(void);
void                ExtInitWinKill(void);

/* fonts.c */
void                FontConfigLoad(void);
void                FontConfigUnload(void);
const char         *FontLookup(const char *name);

/* handlers.c */
void                SignalsSetup(void);
void                SignalsRestore(void);

/* main.c */
__NORETURN__ void   EExit(int exitcode);

#define EDirRoot()       ENLIGHTENMENT_ROOT
#define EDirBin()        ENLIGHTENMENT_BIN
#define EDirLib()        ENLIGHTENMENT_LIB
#define EDirUser()       Mode.conf.dir
#define EDirUserCache()  Mode.conf.cache_dir
#define EGetSavePrefix() Mode.conf.prefix

void                Etmp(char *s);

/* misc.c */
void                Quicksort(void **a, int l, int r,
			      int (*CompareFunc) (const void *d1,
						  const void *d2));
void                ETimedLoopInit(int k1, int k2, int speed);
int                 ETimedLoopNext(void);

/* mod-misc.c */
void                autosave(void);

/* regex.c */
int                 matchregexp(const char *rx, const char *s);

/* setup.c */
void                SetupX(const char *dstr);

/* slideouts.c */
int                 SlideoutsConfigLoad(FILE * fs);

/* startup.c */
void                StartupWindowsCreate(void);
void                StartupWindowsOpen(void);
void                StartupBackgroundsDestroy(void);

/* theme.c */
char               *ThemeFind(const char *theme);
char               *ThemePathName(const char *path);
void                ThemePathFind(void);
char              **ThemesList(int *num);

/*
 * Global vars
 */
extern const char   e_wm_name[];
extern const char   e_wm_version[];
__EXPORT__ extern EConf Conf;
__EXPORT__ extern EMode Mode;

#ifndef FBPANEL_H
#define FBPANEL_H

#include <X11/Xatom.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>



#define MWM_HINTS_DECORATIONS         (1L << 1)
typedef struct _mwmhints
{
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long inputMode;
    unsigned long status;
}
MWMHints;


extern Window topxwin;
extern GtkWidget *window;
extern Atom atoms[];

#define atom_KWM_WIN_ICON    atoms[0]
#define atom__MOTIF_WM_HINTS atoms[1]
#define atom__WIN_HINTS      atoms[2]
#define atom__WIN_LAYER      atoms[3]
#define atom__WIN_STATE      atoms[4]
#define atom_WM_STATE        atoms[5]
#define atpm_WM_CLASS        atoms[6]

#define atom__NET_CLIENT_LIST         atoms[7]
#define atom__NET_NUMBER_OF_DESKTOPS  atoms[8]
#define atom__NET_CURRENT_DESKTOP     atoms[9]
#define atom__NET_DESKTOP_NAMES       atoms[10]
#define atom__NET_ACTIVE_WINDOW       atoms[11]


#define atom__NET_WM_STATE               atoms[12]
#define atom__NET_WM_STATE_SKIP_TASKBAR  atoms[13]
#define atom__NET_WM_STATE_SKIP_PAGER    atoms[14]
#define atom__NET_WM_STATE_STICKY        atoms[15]

#define atom__NET_WM_DESKTOP      atoms[16]
#define atom__NET_WM_NAME         atoms[17]

#define atom__WIN_WORKSPACE_NAMES atoms[18]



#define WIN_STATE_STICKY          (1<<0)	/* everyone knows sticky */
#define WIN_STATE_MINIMIZED       (1<<1)	/* ??? */
#define WIN_STATE_MAXIMIZED_VERT  (1<<2)	/* window in maximized V state */
#define WIN_STATE_MAXIMIZED_HORIZ (1<<3)	/* window in maximized H state */
#define WIN_STATE_HIDDEN          (1<<4)	/* not on taskbar but window visible */
#define WIN_STATE_SHADED          (1<<5)	/* shaded (NeXT style) */
#define WIN_STATE_HID_WORKSPACE   (1<<6)	/* not on current desktop */
#define WIN_STATE_HID_TRANSIENT   (1<<7)	/* owner of transient is hidden */
#define WIN_STATE_FIXED_POSITION  (1<<8)	/* window is fixed in position even */
#define WIN_STATE_ARRANGE_IGNORE  (1<<9)	/* ignore for auto arranging */

#define WIN_HINTS_SKIP_FOCUS      (1<<0)	/* "alt-tab" skips this win */
#define WIN_HINTS_SKIP_WINLIST    (1<<1)	/* not in win list */
#define WIN_HINTS_SKIP_TASKBAR    (1<<2)	/* not on taskbar */
#define WIN_HINTS_GROUP_TRANSIENT (1<<3)	/* ??????? */
#define WIN_HINTS_FOCUS_ON_CLICK  (1<<4)	/* app only accepts focus when clicked */
#define WIN_HINTS_DO_NOT_COVER    (1<<5)	/* attempt to not cover this window */

#define MAX_TASK_WIDTH 150
#define TASK_PADDING 4
#define PANEL_HEIGHT 20
#define PANEL_BORDER 1



char **get_textproperty(Window win, Atom prop, int *nitems);
void *get_xaproperty (Window win, Atom prop, Atom type, int *nitems);
gchar *myfgets(gchar *str, int num, FILE *fp);

GtkWidget *tb_init();
GtkWidget *clip_init();
GtkWidget *lb_init(gchar *fname);
GtkWidget *clock_init();

#endif

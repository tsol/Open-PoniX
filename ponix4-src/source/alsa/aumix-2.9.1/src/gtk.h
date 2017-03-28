/* $Aumix: aumix/src/gtk.h,v 1.3 2002/12/12 04:38:09 trevor Exp $ */

#ifndef AUMIX_GTK_H
#define AUMIX_GTK_H
#define BORDER 2
#include <sys/stat.h>
#include <sys/mman.h>
#include <gtk/gtk.h>

GdkBitmap      *rmask;
GdkBitmap      *pmask;
GtkWidget      *pixmapwid[SOUND_MIXER_NRDEVICES];
GdkPixmap      *record_pixmap;
GdkPixmap      *play_pixmap;
GtkAdjustment  *adjustment[SOUND_MIXER_NRDEVICES];
GtkAdjustment  *balance[SOUND_MIXER_NRDEVICES];
GtkWidget      *baltracks[SOUND_MIXER_NRDEVICES];
GtkWidget      *channellabel[SOUND_MIXER_NRDEVICES];
GtkToggleButton *rpbuttons[SOUND_MIXER_NRDEVICES];
GtkWidget      *levels[SOUND_MIXER_NRDEVICES];
GtkWidget      *maintable;
gint            balcallback[SOUND_MIXER_NRDEVICES];
gint            lvlcallback[SOUND_MIXER_NRDEVICES];
gint            rpcallback[SOUND_MIXER_NRDEVICES];
gboolean        menuvisible, balancevisible, recplayvisible, valuesvisible, visible[SOUND_MIXER_NRDEVICES];
GtkWidget      *labelrec, *labelzero, *labelcent, *labelleft, *labelright, *labelbalance;
void            WakeUpGTK(void);
void            HideShowBalancesGTK(void);
void            HideShowNumbersGTK(void);
void            HideShowMenuGTK(GtkWidget * menu);
void            HideShowChannelGTK(int device);
void            HideShowRecPlayGTK(void);
void            SaveSettingsGTK(void);
void            LoadSettingsGTK(void);
void            AdjustLevelGTK(int device);
void            AdjustBalanceGTK(int device);
void            InitScreenGTK(void);
void            CloseScreenGTK(void);
void            FileOKLoad(GtkWidget * w, GtkFileSelection * fs);
void            FileOKSave(GtkWidget * w, GtkFileSelection * fs);
void            LoadDialog(void);
void            SaveDialog(void);
void            get_main_menu(GtkWidget * window, GtkWidget ** menubar);
#endif				/* AUMIX_GTK_H */

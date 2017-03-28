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
GtkToggleButton *rpbuttons[SOUND_MIXER_NRDEVICES];
gint           *balcallback[SOUND_MIXER_NRDEVICES];
gint           *lvlcallback[SOUND_MIXER_NRDEVICES];
gint           *rpcallback[SOUND_MIXER_NRDEVICES];
void            SaveSettingsGTK(void);
void            LoadSettingsGTK(void);
void            AdjustLevelGTK(int *device);
void            AdjustBalanceGTK(int *device);
void            aumix_destroy(GtkWidget * widget, gpointer * data);
void            InitScreenGTK(void);
void            CloseScreenGTK(void);
void            AumixSignalHandlerGTK(int signal_number);
void            FileOKLoad(GtkWidget * w, GtkFileSelection * fs);
void            FileOKSave(GtkWidget * w, GtkFileSelection * fs);
void            LoadDialog(void);
void            SaveDialog(void);
void            get_main_menu(GtkWidget * window, GtkWidget ** menubar);
#endif				/* AUMIX_GTK_H */

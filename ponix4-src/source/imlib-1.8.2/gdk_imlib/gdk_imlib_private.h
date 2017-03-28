#ifndef CONVERT_PATH
#define CONVERT_PATH "/usr/local/bin"
#endif

#ifndef NETPBM_PATH
#define NETPBM_PATH  "/usr/local/bin"
#endif

#ifndef CJPEG_PROG
#define CJPEG_PROG "/usr/bin/cjpeg"
#endif

#ifndef DJPEG_PROG
#define DJPEG_PROG "/usr/local/bin/djpeg"
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <netinet/in.h>
#ifdef HAVE_IPC_H
#include <sys/ipc.h>
#endif
#ifdef HAVE_SHM_H
#include <sys/shm.h>
#endif
#include <sys/time.h>
#include <sys/types.h>

#ifdef _HAVE_STRING_H
#include <string.h>
#elif _HAVE_STRINGS_H
#include <strings.h>
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#ifdef HAVE_XSHM_H
#include <X11/extensions/XShm.h>
#endif
#include <X11/extensions/shape.h>
#include <X11/cursorfont.h>
#include <gdk/gdkprivate.h>

#ifdef HAVE_LIBJPEG
#include <jpeglib.h>
#endif
#ifdef HAVE_LIBPNG
#include <png.h>
#endif
#ifdef HAVE_LIBTIFF
#include <tiffio.h>
#endif
#ifdef HAVE_LIBGIF
#include <gif_lib.h>
#endif

#define BYTE_ORD_24_RGB 0
#define BYTE_ORD_24_RBG 1
#define BYTE_ORD_24_BRG 2
#define BYTE_ORD_24_BGR 3
#define BYTE_ORD_24_GRB 4
#define BYTE_ORD_24_GBR 5

struct image_cache
  {
    gchar              *file;
    GdkImlibImage      *im;
    gint                refnum;
    gchar               dirty;
    struct image_cache *prev;
    struct image_cache *next;
  };

struct pixmap_cache
  {
    GdkImlibImage      *im;
    gchar              *file;
    gchar               dirty;
    gint                width, height;
    GdkPixmap          *pmap;
    GdkBitmap          *shape_mask;
    XImage             *xim, *sxim;
    gint                refnum;
    struct pixmap_cache *prev;
    struct pixmap_cache *next;
  };

typedef struct _xdata
  {
    Display            *disp;
    gint                screen;
    Window              root;
    Visual             *visual;
    gint                depth;
    gint                render_depth;
    Colormap            root_cmap;
    gchar               shm;
    gchar               shmp;
    gint                shm_event;
    XImage             *last_xim;
    XImage             *last_sxim;
    Window              base_window;
    GdkWindow          *gdk_win;
    GdkColormap        *gdk_cmap;
    gint                byte_order, bit_order;
#ifdef HAVE_SHM
    XShmSegmentInfo     last_shminfo;
    XShmSegmentInfo     last_sshminfo;
#endif
  }
Xdata;

typedef struct _imlibdata
  {
    gint                num_colors;
    GdkImlibColor      *palette;
    GdkImlibColor      *palette_orig;
    unsigned char      *fast_rgb;
    gint               *fast_err;
    gint               *fast_erg;
    gint               *fast_erb;
    gint                render_type;
    gint                max_shm;
    gint                byte_order;
    struct _cache
      {
	gchar               on_image;
	gint                size_image;
	gint                num_image;
	gint                used_image;
        struct image_cache *image;
        gchar               on_pixmap;
        gint                size_pixmap;
        gint                num_pixmap;
        gint                used_pixmap;
        struct pixmap_cache *pixmap;
      }
    cache;
    gchar               fastrend;
    gchar               hiq;
    GdkImlibColorModifier mod, rmod, gmod, bmod;
    unsigned char       rmap[256], gmap[256], bmap[256];
    gchar               fallback;
    gchar               ordered_dither;
    Xdata               x;
  }
ImlibData;

extern ImlibData   *_gdk_imlib_data;

gint                _gdk_imlib_index_best_color_match(gint * r, gint * g, gint * b);

void                _gdk_imlib_dirty_pixmaps(GdkImlibImage * im);
void                _gdk_imlib_dirty_images(GdkImlibImage * im);
void                _gdk_imlib_find_pixmap(GdkImlibImage * im, int width, int height, GdkPixmap ** pmap, GdkBitmap ** mask);
GdkImlibImage      *_gdk_imlib_find_image(char *file);
void                _gdk_imlib_free_pixmappmap(GdkPixmap * pmap);
void                _gdk_imlib_free_image(GdkImlibImage * im);
void                _gdk_imlib_flush_image(GdkImlibImage * im);
void                _gdk_imlib_add_image(GdkImlibImage * im, char *file);
void                _gdk_imlib_add_pixmap(GdkImlibImage * im, int width, int height, XImage * xim, XImage * sxim);
void                _gdk_imlib_clean_caches(void);
void                _gdk_imlib_nullify_image(GdkImlibImage * im);

/* char *g_SplitID(char *file); */
char               *_gdk_imlib_GetExtension(char *file);

#ifdef HAVE_LIBJPEG
unsigned char      *_gdk_imlib_LoadJPEG(FILE * f, int *w, int *h);

#endif /* HAVE_LIBJPEG */
#ifdef HAVE_LIBPNG
unsigned char      *_gdk_imlib_LoadPNG(FILE * f, int *w, int *h, int *t);

#endif /* HAVE_LIBPNG */
#ifdef HAVE_LIBTIFF
unsigned char      *_gdk_imlib_LoadTIFF(FILE *f, char *file, int *w, int *h, int *t);

#endif /* HAVE_LIBTIFF */
#ifdef HAVE_LIBGIF
unsigned char      *_gdk_imlib_LoadGIF(FILE *f, int *w, int *h, int *t);

#endif /* HAVE_LIBGIF */
unsigned char      *_gdk_imlib_LoadBMP(FILE *f, int *w, int *h, int *t);
unsigned char      *_gdk_imlib_LoadXPM(FILE *f, int *w, int *h, int *t);
unsigned char      *_gdk_imlib_LoadPPM(FILE * f, int *w, int *h);
int                 _gdk_imlib_ispnm(FILE *f);
int                 _gdk_imlib_isjpeg(FILE *f);
int                 _gdk_imlib_ispng(FILE *f);
int                 _gdk_imlib_istiff(FILE *f);
int                 _gdk_imlib_iseim(FILE *f);
int                 _gdk_imlib_isgif(FILE *f);
int                 _gdk_imlib_isxpm(FILE *f);
int                 _gdk_imlib_isbmp(FILE *f);

GdkPixmap          *gdk_imlib_pixmap_foreign_new(gint width, gint height, gint depth, Pixmap pmap);

void                _gdk_imlib_calc_map_tables(GdkImlibImage * im);

void                _gdk_imlib_PaletteAlloc(int num, int *cols);

FILE               *_gdk_imlib_open_helper(const char *, const char *, const char *);
int                 _gdk_imlib_close_helper(FILE *);

#define INDEX_RGB(r,g,b)  id->fast_rgb[(r<<10)|(g<<5)|(b)]
#define COLOR_INDEX(i)    id->palette[i].pixel
#define COLOR_RGB(r,g,b)  id->palette[INDEX_RGB(r,g,b)].pixel
#define ERROR_RED(rr,i)   rr-id->palette[i].r;
#define ERROR_GRN(gg,i)   gg-id->palette[i].g;
#define ERROR_BLU(bb,i)   bb-id->palette[i].b;

#define DITHER_ERROR(Der1,Der2,Dex,Der,Deg,Deb) \
ter=&(Der1[Dex]);\
(*ter)+=(Der*7)>>4;ter++;\
(*ter)+=(Deg*7)>>4;ter++;\
(*ter)+=(Deb*7)>>4;\
ter=&(Der2[Dex-6]);\
(*ter)+=(Der*3)>>4;ter++;\
(*ter)+=(Deg*3)>>4;ter++;\
(*ter)+=(Deb*3)>>4;ter++;\
(*ter)+=(Der*5)>>4;ter++;\
(*ter)+=(Deg*5)>>4;ter++;\
(*ter)+=(Deb*5)>>4;ter++;\
(*ter)+=Der>>4;ter++;\
(*ter)+=Deg>>4;ter++;\
(*ter)+=Deb>>4;

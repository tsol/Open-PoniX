/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2009 Kim Woelders
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
#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <X11/Xlib.h>
#include "eimage.h"

typedef struct _container Container;

typedef struct {
   unsigned int        anim_time;	/* Animation run time  (ms) */
} ContainerCfg;

typedef struct {
   void               *obj;
   int                 xo, yo, wo, ho;	/* Outer */
   int                 xi, yi, wi, hi;	/* Inner */
   EImage             *im;
} ContainerObject;

typedef struct {
   void                (*Init) (Container * ct);
   void                (*Exit) (Container * ct, int wm_exit);
   void                (*Signal) (Container * ct, int signal, void *prm);
   void                (*Event) (Container * ct, XEvent * ev);
   void                (*ObjSizeCalc) (Container * ct, ContainerObject * cto);
   void                (*ObjPlace) (Container * ct, ContainerObject * cto,
				    EImage * im);
} ContainerOps;

struct _container {
   const ContainerOps *ops;
   const char         *wm_name;
   const char         *menu_title;
   const char         *dlg_title;

   /* user settings */
   char               *name;
   char                type;
   char                orientation;
   char                scrollbar_side;
   char                arrow_side;
   char                nobg;
   int                 iconsize;
   char                auto_resize;
   char                draw_icon_base;
   char                scrollbar_hide;
   char                cover_hide;
   int                 auto_resize_anchor;
   /* Iconbox specific */
   char                shownames;
   int                 anim_mode;
   int                 icon_mode;

   /* internally set stuff */
   EWin               *ewin;
   int                 w, h;
   int                 pos;
   int                 iwin_maxl, iwin_maxl_min;
   int                 iwin_fixh;
   ImageClass         *ic_box;
   ImageClass         *ic_item_base;
   EImage             *im_item_base;

   unsigned char       scrollbar_state;
   unsigned char       arrow1_state;
   unsigned char       arrow2_state;
   char                scrollbox_clicked;
   char                icon_clicked;

   Win                 win;
   Win                 cover_win;
   Win                 icon_win;
   Win                 scroll_win;
   Win                 arrow1_win;
   Win                 arrow2_win;
   Win                 scrollbar_win;
   Win                 scrollbarknob_win;

   int                 num_objs;
   ContainerObject    *objs;

   /* these are theme-settable parameters */
   int                 scroll_thickness;
   int                 arrow_thickness;
   int                 bar_thickness;
   int                 knob_length;

   /* State flags */
   char                do_update;
   int                 scroll_thickness_set;
};

extern ContainerCfg Conf_containers;

void                ContainerRedraw(Container * ct);

typedef int         (ContainerIterator) (Container * ct, void *data);
Container          *ContainersIterate(ContainerIterator * cti, int type,
				      void *data);
Container         **ContainersGetList(int *pnum);

int                 ContainerObjectAdd(Container * ct, void *obj);
int                 ContainerObjectDel(Container * ct, void *obj);
int                 ContainerObjectFind(Container * ct, void *obj);
void               *ContainerObjectFindByXY(Container * ct, int x, int y);

/* Here? */
#define IB_TYPE_ICONBOX     0
#define IB_TYPE_SYSTRAY     1

#endif /* _CONTAINER_H_ */

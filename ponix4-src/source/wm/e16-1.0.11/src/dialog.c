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
#include "E.h"
#if ENABLE_DIALOGS
#include "dialog.h"
#include "e16-ecore_list.h"
#include "eimage.h"
#include "ewins.h"
#include "hints.h"
#include "iclass.h"
#include "tclass.h"
#include "timers.h"
#include "xwin.h"

#define DEBUG_DIALOGS 0

typedef struct {
   char                horizontal;

   char                numeric;

   int                 upper;
   int                 lower;
   int                 unit;
   int                 jump;
   int                *val_ptr;

   int                 min_length;

   int                 base_orig_w, base_orig_h;
   int                 knob_orig_w, knob_orig_h;

   int                 base_x, base_y, base_w, base_h;
   int                 knob_x, knob_y, knob_w, knob_h;
   int                 numeric_x, numeric_y, numeric_w, numeric_h;

   ImageClass         *ic_base;
   ImageClass         *ic_knob;

   char                in_drag;

   Win                 base_win;
   Win                 knob_win;
} DItemSlider;

typedef struct {
   Win                 area_win;
   int                 w, h;
   DialogItemCallbackFunc *init_func;
   DialogItemCallbackFunc *event_func;
} DItemArea;

typedef struct {
   Win                 check_win;
   int                 orig_w, orig_h;
   char                onoff;
   char               *onoff_ptr;
} DItemCheckButton;

typedef struct {
   char               *image;
} DItemImage;

typedef struct {
   char                horizontal;
} DItemSeparator;

typedef struct {
   int                 num_columns;
   char                border;
   char                homogenous_h;
   char                homogenous_v;
   int                 num_items;
   DItem             **items;
} DItemTable;

typedef struct {
   Win                 radio_win;
   int                 orig_w, orig_h;
   char                onoff;
   int                 val;
   int                *val_ptr;
   DItem              *next;
   DItem              *first;
} DItemRadioButton;

struct _ditem {
   int                 type;
   Dialog             *dlg;
   DialogCallbackFunc *func;
   int                 val;
   void               *data;
   ImageClass         *iclass;
   TextClass          *tclass;
   EImageBorder        padding;
   char                fill_h;
   char                fill_v;
   char                do_close;
   int                 align_h;
   int                 align_v;
   int                 row_span;
   int                 col_span;

   int                 x, y, w, h;
   Win                 win;
   char               *text;
   union {
      DItemCheckButton    check_button;
      DItemTable          table;
      DItemImage          image;
      DItemSeparator      separator;
      DItemRadioButton    radio_button;
      DItemSlider         slider;
      DItemArea           area;
   } item;

   char                realized;
   char                update;

   char                state;
   char                hilited;
   char                clicked;
};

typedef struct {
   KeyCode             key;
   DialogCallbackFunc *func;
   int                 val;
   void               *data;
} DKeyBind;

struct _dialog {
   EWin               *ewin;
   Win                 win;
   int                 w, h;
   char               *name;
   char               *title;
   PmapMask            pmm_bg;
   TextClass          *tclass;
   ImageClass         *iclass;
   DItem              *item;
   DialogCallbackFunc *exit_func;
   int                 exit_val;
   int                 num_bindings;
   DKeyBind           *keybindings;
   void               *data;

   char                redraw;
   char                update;
   char                resize;
   char                close;
   char                set_title;
   int                 xu1, yu1, xu2, yu2;
};

static EWin        *FindEwinByDialog(Dialog * d);
static int          FindADialog(void);

static void         DialogHandleEvents(Win win, XEvent * ev, void *prm);
static void         DItemHandleEvents(Win win, XEvent * ev, void *prm);

static void         MoveTableBy(Dialog * d, DItem * di, int dx, int dy);
static void         DialogItemsRealize(Dialog * d);
static void         DialogItemDestroy(DItem * di, int clean);
static void         DialogDrawItems(Dialog * d, DItem * di, int x, int y, int w,
				    int h);

static int          DialogItemCheckButtonGetState(DItem * di);

static void         DialogUpdate(Dialog * d);

static void         DialogAddFooter(Dialog * d, DItem * parent,
				    int flags, DialogCallbackFunc * cb);
static void         DialogAddHeader(Dialog * d, DItem * parent,
				    const char *img, const char *txt);

static Ecore_List  *dialog_list = NULL;

static char         dialog_update_pending = 0;

void
DialogBindKey(Dialog * d, const char *key, DialogCallbackFunc * func, int val,
	      void *data)
{
   d->num_bindings++;
   d->keybindings = EREALLOC(DKeyBind, d->keybindings, d->num_bindings);
   d->keybindings[d->num_bindings - 1].val = val;
   d->keybindings[d->num_bindings - 1].func = func;
   d->keybindings[d->num_bindings - 1].data = data;
   d->keybindings[d->num_bindings - 1].key = EKeynameToKeycode(key);
}

void
DialogKeybindingsDestroy(Dialog * d)
{
   Efree(d->keybindings);
   d->keybindings = NULL;
   d->num_bindings = 0;
}

Dialog             *
DialogCreate(const char *name)
{
   Dialog             *d;

   d = ECALLOC(Dialog, 1);
   if (!d)
      return NULL;

   if (!dialog_list)
      dialog_list = ecore_list_new();
   ecore_list_append(dialog_list, d);

   d->name = Estrdup(name);
   d->win = ECreateClientWindow(VROOT, -20, -20, 2, 2);
   EventCallbackRegister(d->win, DialogHandleEvents, d);

   d->iclass = ImageclassAlloc("DIALOG", 1);
   d->tclass = TextclassAlloc("DIALOG", 1);

   d->xu1 = d->yu1 = 99999;
   d->xu2 = d->yu2 = 0;

   return d;
}

static void
DialogDestroy(Dialog * d)
{
   ecore_list_node_remove(dialog_list, d);

   Efree(d->name);
   Efree(d->title);
   Efree(d->data);
   DialogKeybindingsDestroy(d);
   if (d->item)
      DialogItemDestroy(d->item, 0);
   ImageclassFree(d->iclass);
   TextclassFree(d->tclass);

   PmapMaskFree(&d->pmm_bg);

   Efree(d);
}

static int
_DialogMatchName(const void *data, const void *match)
{
   return strcmp(((const Dialog *)data)->name, (const char *)match);
}

Dialog             *
DialogFind(const char *name)
{
   return (Dialog *) ecore_list_find(dialog_list, _DialogMatchName, name);
}

void
DialogSetTitle(Dialog * d, const char *title)
{
   Efree(d->title);
   d->title = Estrdup(title);
   d->set_title = 1;
}

void
DialogSetExitFunction(Dialog * d, DialogCallbackFunc * func, int val)
{
   d->exit_func = func;
   d->exit_val = val;
}

void
DialogCallExitFunction(Dialog * d)
{
   if (d->exit_func)
      d->exit_func(d, d->exit_val, NULL);
}

void               *
DialogDataSet(Dialog * d, unsigned int size)
{
   d->data = Ecalloc(size, 1);
   return d->data;
}

void               *
DialogDataGet(Dialog * d)
{
   return d->data;
}

DItem              *
DialogItemAddButton(DItem * parent, const char *text, DialogCallbackFunc * func,
		    int val, char doclose, int image __UNUSED__)
{
   DItem              *di;

   di = DialogAddItem(parent, DITEM_BUTTON);
   DialogItemSetText(di, text);
   DialogItemSetCallback(di, func, 0, NULL);
   di->val = val;
   di->do_close = doclose;

   return di;
}

void
DialogRedraw(Dialog * d)
{
   if ((!d->tclass) || (!d->iclass))
      return;

#if DEBUG_DIALOGS
   Eprintf("DialogRedraw win=%#lx pmap=%#lx\n",
	   WinGetXwin(d->win), WinGetPmap(d->win));
#endif

   PmapMaskFree(&d->pmm_bg);
   ImageclassApplyCopy(d->iclass, d->win, d->w, d->h, 0, 0, STATE_NORMAL,
		       &(d->pmm_bg), IC_FLAG_FULL_SIZE, ST_DIALOG);
   if (d->pmm_bg.pmap == None)
      return;

   EGetWindowBackgroundPixmap(d->win);
   EXCopyArea(d->pmm_bg.pmap, WinGetPmap(d->win), 0, 0, d->w, d->h, 0, 0);

   d->redraw = 1;

   DialogDrawItems(d, d->item, 0, 0, 99999, 99999);
}

static void
_DialogEwinInit(EWin * ewin)
{
   Dialog             *d = (Dialog *) ewin->data;

   d->ewin = ewin;

   EwinSetTitle(ewin, d->title);
   EwinSetClass(ewin, d->name, "Enlightenment_Dialog");
   d->set_title = 0;

   ewin->props.focus_when_mapped = 1;
   ewin->props.ignorearrange = 1;

   EoSetSticky(ewin, 1);
   EoSetLayer(ewin, 10);
}

static void
_DialogEwinMoveResize(EWin * ewin, int resize __UNUSED__)
{
   Dialog             *d = (Dialog *) ewin->data;

   if (!d || Mode.mode != MODE_NONE || !EoIsShown(ewin))
      return;

   if (TransparencyUpdateNeeded() || ImageclassIsTransparent(d->iclass))
      DialogRedraw(d);
}

static void
_DialogEwinClose(EWin * ewin)
{
   DialogDestroy((Dialog *) ewin->data);
   ewin->data = NULL;
}

static const EWinOps _DialogEwinOps = {
   _DialogEwinInit,
   NULL,
   _DialogEwinMoveResize,
   _DialogEwinClose,
};

void
DialogArrange(Dialog * d, int resize)
{
   if (resize)
      DialogItemsRealize(d);

   if (d->set_title)
     {
	EwinSetTitle(d->ewin, d->title);
	d->set_title = 0;
     }

   ICCCM_SetSizeConstraints(d->ewin, d->w, d->h, d->w, d->h, 0, 0, 1, 1,
			    0.0, 65535.0);

   if (resize)
     {
	EwinResize(d->ewin, d->w, d->h, 0);
	d->resize = 1;
	DialogRedraw(d);
	DialogUpdate(d);
	d->resize = 0;
	ArrangeEwinCentered(d->ewin);
     }
}

static void
DialogShowArranged(Dialog * d, int center)
{
   EWin               *ewin;

   ewin = FindEwinByDialog(d);
   if (ewin)
     {
	EwinRaise(ewin);
	EwinShow(ewin);
	return;
     }

   DialogItemsRealize(d);

   ewin = AddInternalToFamily(d->win, "DIALOG", EWIN_TYPE_DIALOG,
			      &_DialogEwinOps, d);
   if (!ewin)
      return;

   DialogArrange(d, 0);

   ewin->client.event_mask |= KeyPressMask;
   ESelectInput(d->win, ewin->client.event_mask);

   if (ewin->state.placed)
     {
	EwinMoveResize(ewin, EoGetX(ewin), EoGetY(ewin), d->w, d->h, 0);
     }
   else
     {
	EwinResize(ewin, d->w, d->h, 0);
	if (center || FindADialog() == 1)
	   ArrangeEwinCentered(ewin);
	else
	   ArrangeEwin(ewin);
     }

   DialogRedraw(d);
   DialogUpdate(d);
   EwinShow(ewin);
}

void
DialogShow(Dialog * d)
{
   DialogShowArranged(d, 0);
}

void
DialogShowCentered(Dialog * d)
{
   DialogShowArranged(d, 1);
}

void
DialogClose(Dialog * d)
{
   d->close = 1;
}

static void
_DialogClose(Dialog * d)
{
   if (!d)
      return;

   DialogCallExitFunction(d);

   EwinHide(d->ewin);
}

void
DialogShowSimple(const DialogDef * dd, void *data)
{
   DialogShowSimpleWithName(dd, dd->name, data);
}

void
DialogFill(Dialog * d, DItem * parent, const DialogDef * dd, void *data)
{
   DItem              *content;

   if (Conf.dialogs.headers && (dd->header_image || dd->header_text))
      DialogAddHeader(d, parent, dd->header_image, _(dd->header_text));

   content = DialogAddItem(parent, DITEM_TABLE);
   if (!content)
      return;

   Efree(d->data);
   d->data = NULL;

   dd->fill(d, content, data);

   if (dd->func)
      DialogAddFooter(d, parent, dd->flags, dd->func);
}

void
DialogShowSimpleWithName(const DialogDef * dd, const char *name, void *data)
{
   Dialog             *d;
   DItem              *table;

   d = DialogFind(name);
   if (d)
     {
	SoundPlay(SOUND_SETTINGS_ACTIVE);
	DialogShow(d);
	return;
     }
   SoundPlay(dd->sound);

   d = DialogCreate(name);
   if (!d)
      return;

   DialogSetTitle(d, _(dd->title));

   table = DialogInitItem(d);
   if (!table)
      return;

   DialogFill(d, table, dd, data);

   DialogShow(d);
}

static DItem       *
DialogItemCreate(int type)
{
   DItem              *di;

   di = ECALLOC(DItem, 1);
   if (!di)
      return di;

   di->type = type;
   di->align_h = 512;
   di->align_v = 512;
   di->row_span = 1;
   di->col_span = 1;
   di->item.table.num_columns = 1;

   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);

   return di;
}

DItem              *
DialogInitItem(Dialog * d)
{
   DItem              *di;

   if (d->item)
      return NULL;

   di = DialogItemCreate(DITEM_TABLE);
   d->item = di;
   if (!di)
      return di;

   di->dlg = d;
   di->item.table.num_columns = 1;

   return di;
}

DItem              *
DialogAddItem(DItem * dii, int type)
{
   DItem              *di;

   di = DialogItemCreate(type);
   if (!di)
      return di;

   switch (di->type)
     {
     default:
	break;
     case DITEM_AREA:
	di->item.area.w = 32;
	di->item.area.h = 32;
	break;
     case DITEM_CHECKBUTTON:
	di->item.check_button.onoff = 0;
	di->item.check_button.onoff_ptr = &(di->item.check_button.onoff);
	di->item.check_button.orig_w = 10;
	di->item.check_button.orig_h = 10;
	break;
     case DITEM_TABLE:
	di->item.table.num_columns = 1;
	break;
     case DITEM_IMAGE:
	di->item.image.image = NULL;
	break;
     case DITEM_SEPARATOR:
	di->item.separator.horizontal = 0;
	break;
     case DITEM_RADIOBUTTON:
	di->item.radio_button.orig_w = 10;
	di->item.radio_button.orig_h = 10;
	break;
     case DITEM_SLIDER:
	di->item.slider.horizontal = 1;
	di->item.slider.upper = 100;
	di->item.slider.lower = 0;
	di->item.slider.unit = 10;
	di->item.slider.jump = 20;
	di->item.slider.min_length = 64;
	di->item.slider.base_orig_w = 10;
	di->item.slider.base_orig_h = 10;
	di->item.slider.knob_orig_w = 6;
	di->item.slider.knob_orig_h = 6;
	break;
     }

   if (dii)
     {
	dii->item.table.num_items++;
	dii->item.table.items =
	   EREALLOC(DItem *, dii->item.table.items, dii->item.table.num_items);
	dii->item.table.items[dii->item.table.num_items - 1] = di;
	di->dlg = dii->dlg;
     }

   return di;
}

static void
DialogAddHeader(Dialog * d __UNUSED__, DItem * parent, const char *img,
		const char *txt)
{
   DItem              *table, *di;

   table = DialogAddItem(parent, DITEM_TABLE);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);
   DialogItemSetAlign(table, 512, 0);
   DialogItemSetFill(table, 0, 0);

   di = DialogAddItem(table, DITEM_IMAGE);
   DialogItemImageSetFile(di, img);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetText(di, txt);

   DialogAddItem(parent, DITEM_SEPARATOR);
}

static void
DialogAddFooter(Dialog * d, DItem * parent, int flags, DialogCallbackFunc * cb)
{
   DItem              *table;
   int                 n_buttons;

   if (!(flags & DLG_NO_SEPARATOR))
      DialogAddItem(parent, DITEM_SEPARATOR);

   table = DialogAddItem(parent, DITEM_TABLE);
   DialogItemSetAlign(table, 512, 0);
   DialogItemSetFill(table, 0, 0);

   n_buttons = 0;
   if (flags & 1)
     {
	DialogItemAddButton(table, _("OK"), cb, 0, 1, DLG_BUTTON_OK);
	n_buttons++;
     }
   if (flags & 2)
     {
	DialogItemAddButton(table, _("Apply"), cb, 1, 0, DLG_BUTTON_APPLY);
	DialogBindKey(d, "Return", cb, 1, NULL);
	n_buttons++;
     }
   if (flags & 4)
     {
	DialogItemAddButton(table, _("Close"), NULL, 0, 1, DLG_BUTTON_CLOSE);
	DialogBindKey(d, "Escape", DialogCallbackClose, 0, NULL);
	n_buttons++;
     }

   DialogItemTableSetOptions(table, n_buttons, 0, 1, 0);

   DialogSetExitFunction(d, cb, 2);
}

Dialog             *
DialogItemGetDialog(DItem * di)
{
   return di->dlg;
}

void
DialogItemSetCallback(DItem * di, DialogCallbackFunc * func, int val,
		      void *data)
{
   di->func = func;
   di->val = val;
   di->data = data;
}

void
DialogItemSetPadding(DItem * di, int left, int right, int top, int bottom)
{
   di->padding.left = left;
   di->padding.right = right;
   di->padding.top = top;
   di->padding.bottom = bottom;
}

void
DialogItemSetFill(DItem * di, char fill_h, char fill_v)
{
   di->fill_h = fill_h;
   di->fill_v = fill_v;
}

void
DialogItemSetAlign(DItem * di, int align_h, int align_v)
{
   di->align_h = align_h;
   di->align_v = align_v;
}

void
DialogItemSetRowSpan(DItem * di, int row_span)
{
   di->row_span = row_span;
}

void
DialogItemSetColSpan(DItem * di, int col_span)
{
   di->col_span = col_span;
}

void
DialogItemCallCallback(Dialog * d, DItem * di)
{
   if (di->func)
      di->func(d, di->val, di->data);
}

static void
DialogRealizeItem(Dialog * d, DItem * di)
{
   const char         *iclass, *tclass;
   int                 iw = 0, ih = 0;
   int                 register_win_callback;
   EImage             *im;
   EImageBorder       *pad;

   if (di->realized && di->type != DITEM_TABLE)
      return;
   di->realized = 1;

   iclass = tclass = NULL;
   if (di->type == DITEM_BUTTON)
     {
	iclass = "DIALOG_WIDGET_BUTTON";
	tclass = iclass;
     }
   else if (di->type == DITEM_CHECKBUTTON)
     {
	iclass = "DIALOG_WIDGET_CHECK_BUTTON";
	tclass = iclass;
     }
   else if (di->type == DITEM_TEXT)
     {
	tclass = "DIALOG_WIDGET_TEXT";
     }
   else if (di->type == DITEM_SEPARATOR)
     {
	iclass = "DIALOG_WIDGET_SEPARATOR";
     }
   else if (di->type == DITEM_RADIOBUTTON)
     {
	iclass = "DIALOG_WIDGET_RADIO_BUTTON";
	tclass = iclass;
     }
#if 0
   else if (di->type == DITEM_SLIDER)
     {
	iclass = NULL;
     }
#endif
   else if (di->type == DITEM_AREA)
     {
	iclass = "DIALOG_WIDGET_AREA";
     }

   if (!di->iclass && iclass)
     {
	di->iclass = ImageclassAlloc(iclass, 1);
	if (!di->iclass)
	  {
	     di->type = DITEM_NONE;
	     return;
	  }
     }

   if (!di->tclass && tclass)
     {
	di->tclass = TextclassAlloc(tclass, 1);
	if (!di->tclass)
	  {
	     di->type = DITEM_NONE;
	     return;
	  }
     }

   if (di->type == DITEM_TABLE)
     {
	int                 i;

	for (i = 0; i < di->item.table.num_items; i++)
	   DialogRealizeItem(d, di->item.table.items[i]);
     }

   register_win_callback = 1;

   switch (di->type)
     {
     case DITEM_SLIDER:
	if (di->item.slider.numeric)
	  {
	     di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	     EMapWindow(di->win);
	     ESelectInput(di->win,
			  EnterWindowMask | LeaveWindowMask |
			  ButtonPressMask | ButtonReleaseMask);
	  }
	di->item.slider.base_win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(di->item.slider.base_win);
	di->item.slider.knob_win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(di->item.slider.knob_win);
	ESelectInput(di->item.slider.base_win,
		     EnterWindowMask | LeaveWindowMask |
		     ButtonPressMask | ButtonReleaseMask);
	EventCallbackRegister(di->item.slider.base_win, DItemHandleEvents, di);
	ESelectInput(di->item.slider.knob_win,
		     EnterWindowMask | LeaveWindowMask |
		     ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
	EventCallbackRegister(di->item.slider.knob_win, DItemHandleEvents, di);

	if (!di->item.slider.ic_base)
	  {
	     if (di->item.slider.horizontal)
		di->item.slider.ic_base =
		   ImageclassAlloc("DIALOG_WIDGET_SLIDER_BASE_HORIZONTAL", 1);
	     else
		di->item.slider.ic_base =
		   ImageclassAlloc("DIALOG_WIDGET_SLIDER_BASE_VERTICAL", 1);
	  }
	im = ImageclassGetImage(di->item.slider.ic_base, 0, 0, 0);
	if (im)
	  {
	     EImageGetSize(im, &di->item.slider.base_orig_w,
			   &di->item.slider.base_orig_h);
	     EImageFree(im);
	  }

	if (!di->item.slider.ic_knob)
	  {
	     if (di->item.slider.horizontal)
		di->item.slider.ic_knob =
		   ImageclassAlloc("DIALOG_WIDGET_SLIDER_KNOB_HORIZONTAL", 1);
	     else
		di->item.slider.ic_knob =
		   ImageclassAlloc("DIALOG_WIDGET_SLIDER_KNOB_VERTICAL", 1);
	  }
	im = ImageclassGetImage(di->item.slider.ic_knob, 0, 0, 0);
	if (im)
	  {
	     EImageGetSize(im, &di->item.slider.knob_orig_w,
			   &di->item.slider.knob_orig_h);
	     EImageFree(im);
	  }

	pad = ImageclassGetPadding(di->item.slider.ic_base);
	if (di->item.slider.horizontal)
	  {
	     iw = di->item.slider.min_length + pad->left + pad->right;
	     ih = di->item.slider.base_orig_h;
	  }
	else
	  {
	     iw = di->item.slider.base_orig_w;
	     ih = di->item.slider.min_length + pad->top + pad->bottom;
	  }
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_BUTTON:
	pad = ImageclassGetPadding(di->iclass);
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->text, &iw, &ih, 17);
	iw += pad->left + pad->right;
	ih += pad->top + pad->bottom;
	di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(di->win);
	ESelectInput(di->win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_AREA:
	pad = ImageclassGetPadding(di->iclass);
	iw = di->item.area.w;
	ih = di->item.area.h;
	iw += pad->left + pad->right;
	ih += pad->top + pad->bottom;
	di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(di->win);
	di->item.area.area_win = ECreateWindow(di->win, -20, -20, 2, 2, 0);
	EMapWindow(di->item.area.area_win);
	ESelectInput(di->item.area.area_win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask | PointerMotionMask);
	EventCallbackRegister(di->item.area.area_win, DItemHandleEvents, di);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_CHECKBUTTON:
	pad = ImageclassGetPadding(di->iclass);
	im = ImageclassGetImage(di->iclass, 0, 0, 0);
	if (im)
	  {
	     EImageGetSize(im, &di->item.check_button.orig_w,
			   &di->item.check_button.orig_h);
	     EImageFree(im);
	  }
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->text, &iw, &ih, 17);
	if (ih < di->item.check_button.orig_h)
	   ih = di->item.check_button.orig_h;
	iw += di->item.check_button.orig_w + pad->left;
	di->item.check_button.check_win =
	   ECreateWindow(d->win, -20, -20, 2, 2, 0);
	di->win = ECreateEventWindow(d->win, -20, -20, 2, 2);
	EMapWindow(di->item.check_button.check_win);
	EMapWindow(di->win);
	ESelectInput(di->win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_TEXT:
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->text, &iw, &ih, 17);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_IMAGE:
	im = ThemeImageLoad(di->item.image.image);
	if (im)
	  {
	     EImageGetSize(im, &iw, &ih);
	     EImageFree(im);
	  }
	di->w = iw;
	di->h = ih;
	register_win_callback = 0;
	break;
     case DITEM_SEPARATOR:
	pad = ImageclassGetPadding(di->iclass);
	iw = pad->left + pad->right;
	ih = pad->top + pad->bottom;
	di->win = ECreateWindow(d->win, -20, -20, 2, 2, 0);
	EMapWindow(di->win);
	di->w = iw;
	di->h = ih;
	register_win_callback = 0;
	break;
     case DITEM_RADIOBUTTON:
	pad = ImageclassGetPadding(di->iclass);
	im = ImageclassGetImage(di->iclass, 0, 0, 0);
	if (im)
	  {
	     EImageGetSize(im, &di->item.radio_button.orig_w,
			   &di->item.radio_button.orig_h);
	     EImageFree(im);
	  }
	TextSize(di->tclass, 0, 0, STATE_NORMAL, di->text, &iw, &ih, 17);
	if (ih < di->item.radio_button.orig_h)
	   ih = di->item.radio_button.orig_h;
	iw += di->item.radio_button.orig_w + pad->left;
	di->item.radio_button.radio_win =
	   ECreateWindow(d->win, -20, -20, 2, 2, 0);
	di->win = ECreateEventWindow(d->win, -20, -20, 2, 2);
	EMapWindow(di->item.radio_button.radio_win);
	EMapWindow(di->win);
	ESelectInput(di->win,
		     EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		     ButtonReleaseMask);
	di->w = iw;
	di->h = ih;
	break;
     case DITEM_TABLE:
	{
	   int                 cols, rows;

	   pad = ImageclassGetPadding(d->iclass);

	   cols = di->item.table.num_columns;
	   rows = 1;
	   if (cols > 0)
	     {
		int                 i, r, c, x, y;
		int                *col_size, *row_size;

		col_size = EMALLOC(int, cols);
		row_size = EMALLOC(int, rows);

		if (!col_size || !row_size)
		   goto bail_out;

		row_size[0] = 0;
		for (i = 0; i < cols; i++)
		   col_size[i] = 0;

		r = c = 0;
		for (i = 0; i < di->item.table.num_items; i++)
		  {
		     DItem              *dii;
		     int                 w, h, j, ww;

		     dii = di->item.table.items[i];
		     w = dii->w + (dii->padding.left + dii->padding.right);
		     h = dii->h + (dii->padding.top + dii->padding.bottom);
		     ww = 0;
		     for (j = 0; j < dii->col_span; j++)
			ww += col_size[c + j];
		     if (w > ww)
		       {
			  ww = (w + dii->col_span - 1) / dii->col_span;
			  for (j = 0; j < dii->col_span; j++)
			     if (col_size[c + j] < ww)
				col_size[c + j] = ww;
		       }
		     if (h > row_size[r])
			row_size[r] = h;
		     c += dii->col_span;
		     if (c >= cols)
		       {
			  c = 0;
			  r++;
			  rows++;
			  row_size = EREALLOC(int, row_size, rows);

			  if (!row_size)
			     goto bail_out;

			  row_size[rows - 1] = 0;
		       }
		  }
		if (di->item.table.homogenous_h)
		  {
		     int                 max = 0;

		     for (i = 0; i < cols; i++)
		       {
			  if (col_size[i] > max)
			     max = col_size[i];
		       }
		     for (i = 0; i < cols; i++)
			col_size[i] = max;
		  }
		if (di->item.table.homogenous_v)
		  {
		     int                 max = 0;

		     for (i = 0; i < rows; i++)
		       {
			  if (row_size[i] > max)
			     max = row_size[i];
		       }
		     for (i = 0; i < rows; i++)
			row_size[i] = max;
		  }

		iw = ih = 0;
		for (i = 0; i < cols; i++)
		   iw += col_size[i];
		for (i = 0; i < rows; i++)
		   ih += row_size[i];
		di->w = iw;
		di->h = ih;

		x = y = 0;
		r = c = 0;
		for (i = 0; i < di->item.table.num_items; i++)
		  {
		     DItem              *dii;
		     int                 j, sw = 0, sh = 0;

		     dii = di->item.table.items[i];

		     for (j = 0; j < dii->col_span; j++)
			sw += col_size[c + j];
		     for (j = 0; j < dii->row_span; j++)
			sh += row_size[r + j];
		     if (dii->fill_h)
			dii->w = sw - (dii->padding.left + dii->padding.right);
		     if (dii->fill_v)
			dii->h = sh - (dii->padding.top + dii->padding.bottom);
		     if (dii->w <= 0 || dii->h <= 0)
			goto skip;
		     if (dii->type == DITEM_TABLE)
		       {
			  int                 dx, dy, newx, newy;

			  newx =
			     di->x + x + pad->left +
			     dii->padding.left +
			     (((sw
				- (dii->padding.left + dii->padding.right) -
				dii->w) * dii->align_h) >> 10);
			  newy =
			     di->y + y + pad->top +
			     dii->padding.top +
			     (((sh
				- (dii->padding.top + dii->padding.bottom) -
				dii->h) * dii->align_v) >> 10);
			  dx = newx - dii->x - pad->left;
			  dy = newy - dii->y - pad->top;
			  MoveTableBy(d, dii, dx, dy);
		       }
		     else
		       {
			  dii->x =
			     di->x + x + pad->left +
			     dii->padding.left +
			     (((sw
				- (dii->padding.left + dii->padding.right) -
				dii->w) * dii->align_h) >> 10);
			  dii->y =
			     di->y + y + pad->top +
			     dii->padding.top +
			     (((sh
				- (dii->padding.top + dii->padding.bottom) -
				dii->h) * dii->align_v) >> 10);
			  if (dii->win)
			     EMoveResizeWindow(dii->win, dii->x, dii->y,
					       dii->w, dii->h);
			  if (dii->type == DITEM_CHECKBUTTON)
			     EMoveResizeWindow(dii->item.check_button.check_win,
					       dii->x,
					       dii->y +
					       (dii->h -
						dii->item.check_button.orig_h) /
					       2, dii->item.check_button.orig_w,
					       dii->item.check_button.orig_h);
			  if (dii->type == DITEM_RADIOBUTTON)
			     EMoveResizeWindow(dii->item.radio_button.radio_win,
					       dii->x,
					       dii->y +
					       (dii->h -
						dii->item.radio_button.orig_h) /
					       2, dii->item.radio_button.orig_w,
					       dii->item.radio_button.orig_h);
			  if (dii->type == DITEM_AREA)
			    {
			       pad = ImageclassGetPadding(dii->iclass);
			       dii->item.area.w =
				  dii->w - (pad->left + pad->right);
			       dii->item.area.h =
				  dii->h - (pad->top + pad->bottom);
			       EMoveResizeWindow(dii->item.area.area_win,
						 pad->left, pad->top,
						 dii->item.area.w,
						 dii->item.area.h);
			    }
			  if (dii->type == DITEM_SLIDER)
			    {
			       dii->item.slider.base_x = 0;
			       dii->item.slider.base_y = 0;
			       dii->item.slider.base_w = dii->w;
			       dii->item.slider.base_h = dii->h;
			       dii->item.slider.knob_w =
				  dii->item.slider.knob_orig_w;
			       dii->item.slider.knob_h =
				  dii->item.slider.knob_orig_h;
			       if (dii->item.slider.base_win)
				  EMoveResizeWindow(dii->item.slider.base_win,
						    dii->x +
						    dii->item.slider.base_x,
						    dii->y +
						    dii->item.slider.base_y,
						    dii->item.slider.base_w,
						    dii->item.slider.base_h);
			       if (dii->win)
				  EMoveResizeWindow(dii->win,
						    dii->x +
						    dii->item.slider.numeric_x,
						    dii->y +
						    dii->item.slider.numeric_y,
						    dii->item.slider.numeric_w,
						    dii->item.slider.numeric_h);
			    }
		       }
		   skip:
		     x += sw;
		     c += dii->col_span;
		     if (c >= cols)
		       {
			  x = 0;
			  y += row_size[r];
			  c = 0;
			  r++;
		       }
		  }
	      bail_out:
		Efree(col_size);
		Efree(row_size);
	     }
	}
	break;
     case DITEM_NONE:
     default:
	di->w = 0;
	di->h = 0;
	break;
     }

   if (di->win && register_win_callback)
      EventCallbackRegister(di->win, DItemHandleEvents, di);
}

static void
MoveTableBy(Dialog * d, DItem * di, int dx, int dy)
{
   int                 i;

   di->x += dx;
   di->y += dy;
   for (i = 0; i < di->item.table.num_items; i++)
     {
	DItem              *dii;

	dii = di->item.table.items[i];

	if (dii->type == DITEM_TABLE)
	  {
	     MoveTableBy(d, dii, dx, dy);
	     continue;
	  }

	dii->x += dx;
	dii->y += dy;

	if (dii->win)
	   EMoveWindow(dii->win, dii->x, dii->y);

	switch (dii->type)
	  {
	  case DITEM_CHECKBUTTON:
	     EMoveWindow(dii->item.check_button.check_win, dii->x,
			 dii->y +
			 ((dii->h - dii->item.check_button.orig_h) / 2));
	     break;
	  case DITEM_RADIOBUTTON:
	     EMoveWindow(dii->item.radio_button.radio_win, dii->x,
			 dii->y +
			 ((dii->h - dii->item.radio_button.orig_h) / 2));
	     break;
	  case DITEM_SLIDER:
	     {
		if (dii->item.slider.base_win)
		   EMoveResizeWindow(dii->item.slider.base_win,
				     dii->x + dii->item.slider.base_x,
				     dii->y + dii->item.slider.base_y,
				     dii->item.slider.base_w,
				     dii->item.slider.base_h);
		if (dii->item.slider.knob_win)
		   EMoveResizeWindow(dii->item.slider.knob_win,
				     dii->x + dii->item.slider.knob_x,
				     dii->y + dii->item.slider.knob_y,
				     dii->item.slider.knob_w,
				     dii->item.slider.knob_h);
		if (dii->win)
		   EMoveResizeWindow(dii->win,
				     dii->x + dii->item.slider.numeric_x,
				     dii->y + dii->item.slider.numeric_y,
				     dii->item.slider.numeric_w,
				     dii->item.slider.numeric_h);
		break;
	     }
	  }
     }
}

static void
DialogDrawItems(Dialog * d, DItem * di, int x, int y, int w, int h)
{
   d->update = 1;
   di->update = 1;

   if (d->xu1 > x)
      d->xu1 = x;
   if (d->yu1 > y)
      d->yu1 = y;
   x += w;
   y += h;
   if (d->xu2 < x)
      d->xu2 = x;
   if (d->yu2 < y)
      d->yu2 = y;

   dialog_update_pending = 1;

#if DEBUG_DIALOGS
   Eprintf("DialogDrawItems t=%d u=%d - %d,%d -> %d,%d\n", di->type, di->update,
	   d->xu1, d->yu1, d->xu2, d->yu2);
#endif
}

static void
DialogDrawItem(Dialog * d, DItem * di)
{
   int                 state, x, w, val;
   EImageBorder       *pad;
   EImage             *im;

   if (!di->update && di->type != DITEM_TABLE)
      return;

   if (di->x > d->xu2 || di->y > d->yu2 ||
       di->x + di->w <= d->xu1 || di->y + di->h <= d->yu1)
      goto done;

#if DEBUG_DIALOGS
   Eprintf("DialogDrawItem t=%d u=%d - %d,%d -> %d,%d\n", di->type, di->update,
	   d->xu1, d->yu1, d->xu2, d->yu2);
#endif

   switch (di->type)
     {
     case DITEM_TABLE:
	{
	   int                 i;
	   DItem              *dii;

	   for (i = 0; i < di->item.table.num_items; i++)
	     {
		dii = di->item.table.items[i];
		if (di->update)
		   dii->update = 1;
		DialogDrawItem(d, dii);
	     }

#if 0				/* Debug */
	   {
	      XGCValues           gcv;
	      GC                  gc;

	      pad = ImageclassGetPadding(d->iclass);
	      gcv.subwindow_mode = IncludeInferiors;
	      gc = EXCreateGC(WinGetPmap(d->win), GCSubwindowMode, &gcv);
	      XSetForeground(disp, gc, Dpy.pixel_black);
	      XDrawRectangle(disp, WinGetPmap(d->win), gc,
			     pad->left + di->x, pad->top + di->y, di->w, di->h);
	      EXFreeGC(gc);
	   }
#endif
	}
	break;

     case DITEM_SLIDER:
	val = (di->item.slider.val_ptr) ?
	   *(di->item.slider.val_ptr) : di->item.slider.lower;
	if (di->item.slider.horizontal)
	  {
	     di->item.slider.knob_x = di->item.slider.base_x +
		(((di->item.slider.base_w - di->item.slider.knob_w) *
		  (val - di->item.slider.lower)) /
		 (di->item.slider.upper - di->item.slider.lower));
	     di->item.slider.knob_y = di->item.slider.base_y +
		((di->item.slider.base_h - di->item.slider.knob_h) / 2);
	  }
	else
	  {
	     di->item.slider.knob_y = di->item.slider.base_y +
		(((di->item.slider.base_h - di->item.slider.knob_h) *
		  (val - di->item.slider.lower)) /
		 (di->item.slider.upper - di->item.slider.lower));
	     di->item.slider.knob_x = di->item.slider.base_x +
		((di->item.slider.base_w - di->item.slider.knob_w) / 2);
	  }
	if (di->item.slider.knob_win)
	   EMoveResizeWindow(di->item.slider.knob_win,
			     di->x + di->item.slider.knob_x,
			     di->y + di->item.slider.knob_y,
			     di->item.slider.knob_w, di->item.slider.knob_h);
	if (di->item.slider.base_win)
	   ImageclassApply(di->item.slider.ic_base,
			   di->item.slider.base_win,
			   0, 0, STATE_NORMAL, ST_WIDGET);
	state = STATE_NORMAL;
	if ((di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	else if ((di->hilited) && (!di->clicked))
	   state = STATE_HILITED;
	else if (!(di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	if (di->item.slider.knob_win)
	   ImageclassApply(di->item.slider.ic_knob,
			   di->item.slider.knob_win, 0, 0, state, ST_WIDGET);
	break;

     case DITEM_BUTTON:
	state = STATE_NORMAL;
	if ((di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	else if ((di->hilited) && (!di->clicked))
	   state = STATE_HILITED;
	else if (!(di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	ITApply(di->win, di->iclass, NULL, state, 0, 0,
		ST_WIDGET, di->tclass, NULL, di->text, 0);
	break;

     case DITEM_AREA:
	if (!d->redraw)
	   break;
	ImageclassApply(di->iclass, di->win, 0, 0, STATE_NORMAL, ST_DIALOG);
	if (di->item.area.init_func)
	   di->item.area.init_func(di, 0, NULL);
	break;

     case DITEM_SEPARATOR:
	if (!d->redraw)
	   break;
	if (di->item.separator.horizontal)
	   ImageclassApply(di->iclass, di->win, 0, 0, STATE_NORMAL, ST_WIDGET);
	else
	   ImageclassApply(di->iclass, di->win, 0, 0, STATE_CLICKED, ST_WIDGET);
	break;

     case DITEM_TEXT:
	state = STATE_NORMAL;
	x = di->x;
	w = di->w;
	goto draw_text;

     case DITEM_IMAGE:
	im = ThemeImageLoad(di->item.image.image);
	if (im)
	  {
	     EImageRenderOnDrawable(im, d->win, WinGetPmap(d->win),
				    EIMAGE_BLEND | EIMAGE_ANTI_ALIAS,
				    di->x, di->y, di->w, di->h);
	     EImageFree(im);
	  }
	break;

     case DITEM_CHECKBUTTON:
	state = STATE_NORMAL;
	if ((di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	else if ((di->hilited) && (!di->clicked))
	   state = STATE_HILITED;
	else if (!(di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	ImageclassApply(di->iclass, di->item.check_button.check_win,
			DialogItemCheckButtonGetState(di), 0, state, ST_WIDGET);
	if (!d->redraw &&
	    (TextclassGetTextState(di->tclass, di->state, 0, 0) ==
	     TextclassGetTextState(di->tclass, state, 0, 0)))
	   break;
	pad = ImageclassGetPadding(di->iclass);
	x = di->x + di->item.check_button.orig_w + pad->left;
	w = di->w - di->item.check_button.orig_w - pad->left;
	goto draw_text;

     case DITEM_RADIOBUTTON:
	state = STATE_NORMAL;
	if ((di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	else if ((di->hilited) && (!di->clicked))
	   state = STATE_HILITED;
	else if (!(di->hilited) && (di->clicked))
	   state = STATE_CLICKED;
	ImageclassApply(di->iclass, di->item.radio_button.radio_win,
			di->item.radio_button.onoff, 0, state, ST_WIDGET);
	if (!d->redraw &&
	    (TextclassGetTextState(di->tclass, di->state, 0, 0) ==
	     TextclassGetTextState(di->tclass, state, 0, 0)))
	   break;
	pad = ImageclassGetPadding(di->iclass);
	x = di->x + di->item.radio_button.orig_w + pad->left;
	w = di->w - di->item.radio_button.orig_w - pad->left;
	goto draw_text;

     default:
	break;

      draw_text:
	di->state = state;
	if (!di->text || !di->tclass)
	   break;
	if (!d->redraw || di->update)
	   EXCopyArea(d->pmm_bg.pmap, WinGetPmap(d->win),
		      di->x, di->y, di->w, di->h, di->x, di->y);
	TextDraw(di->tclass, d->win, WinGetPmap(d->win), 0, 0, state, di->text,
		 x, di->y, w, 99999, 17, TextclassGetJustification(di->tclass));
	break;
     }

 done:
   di->update = 0;
}

static void
DialogUpdate(Dialog * d)
{
   do
     {
	d->update = 0;
	if (d->item)
	   DialogDrawItem(d, d->item);
     }
   while (d->update);
   if (d->xu1 < d->xu2 && d->yu1 < d->yu2)
      EClearArea(d->win, d->xu1, d->yu1, d->xu2 - d->xu1, d->yu2 - d->yu1);
   d->xu1 = d->yu1 = 99999;
   d->xu2 = d->yu2 = 0;
}

static void
_DialogsCheckUpdate(void *data __UNUSED__)
{
   Dialog             *d;

   if (!dialog_update_pending)
      return;
   dialog_update_pending = 0;

   ECORE_LIST_FOR_EACH(dialog_list, d)
   {
      if (d->update)
	 DialogUpdate(d);
      d->redraw = 0;
   }
}

void
DialogsInit(void)
{
   IdlerAdd(_DialogsCheckUpdate, NULL);
}

static void
DialogItemsRealize(Dialog * d)
{
   EImageBorder       *pad;

   if (!d->item)
      return;

   DialogRealizeItem(d, d->item);
   DialogDrawItems(d, d->item, 0, 0, 99999, 99999);
   pad = ImageclassGetPadding(d->iclass);
   d->w = d->item->w + pad->left + pad->right;
   d->h = d->item->h + pad->top + pad->bottom;
   EResizeWindow(d->win, d->w, d->h);
}

void
DialogItemSetText(DItem * di, const char *text)
{
   Efree(di->text);
   di->text = Estrdup(text);

   if (di->realized)
      DialogDrawItems(di->dlg, di, di->x, di->y, di->w, di->h);
}

void
DialogItemRadioButtonSetFirst(DItem * di, DItem * first)
{
   di->item.radio_button.first = first;
   if (di == first)
      return;
   while (first->item.radio_button.next)
      first = first->item.radio_button.next;
   first->item.radio_button.next = di;
}

void
DialogItemRadioButtonGroupSetValPtr(DItem * di, int *val_ptr)
{
   while (di)
     {
	di->item.radio_button.val_ptr = val_ptr;
	if (*val_ptr == di->item.radio_button.val)
	   di->item.radio_button.onoff = 1;
	di = di->item.radio_button.next;
     }
}

void
DialogItemRadioButtonGroupSetVal(DItem * di, int val)
{
   di->item.radio_button.val = val;
}

void
DialogItemCheckButtonSetState(DItem * di, char onoff)
{
   if (*(di->item.check_button.onoff_ptr) == onoff)
      return;
   *(di->item.check_button.onoff_ptr) = onoff;

   if (di->realized)
      DialogDrawItems(di->dlg, di, di->x, di->y, di->w, di->h);
}

void
DialogItemCheckButtonSetPtr(DItem * di, char *onoff_ptr)
{
   di->item.check_button.onoff_ptr = onoff_ptr;
}

static int
DialogItemCheckButtonGetState(DItem * di)
{
   return *(di->item.check_button.onoff_ptr) ? 1 : 0;
}

void
DialogItemTableSetOptions(DItem * di, int num_columns, char border,
			  char homogenous_h, char homogenous_v)
{
   di->item.table.num_columns = num_columns;
   di->item.table.border = border;
   di->item.table.homogenous_h = homogenous_h;
   di->item.table.homogenous_v = homogenous_v;
}

void
DialogItemSeparatorSetOrientation(DItem * di, char horizontal)
{
   di->item.separator.horizontal = horizontal;
}

void
DialogItemImageSetFile(DItem * di, const char *image)
{
   Efree(di->item.image.image);
   di->item.image.image = Estrdup(image);
   di->fill_h = 0;
   di->fill_v = 0;
}

static int
_DialogItemSliderClampVal(const DItem * di, int val)
{
   if (di->item.slider.lower < di->item.slider.upper)
     {
	if (val < di->item.slider.lower)
	   val = di->item.slider.lower;
	else if (val > di->item.slider.upper)
	   val = di->item.slider.upper;
     }
   else
     {
	if (val > di->item.slider.lower)
	   val = di->item.slider.lower;
	else if (val < di->item.slider.upper)
	   val = di->item.slider.upper;
     }

   return val;
}

void
DialogItemSliderSetVal(DItem * di, int val)
{
   val = _DialogItemSliderClampVal(di, val);

   if (di->item.slider.val_ptr)
      *di->item.slider.val_ptr = val;

   if (di->realized)
      DialogDrawItems(di->dlg, di, di->x, di->y, di->w, di->h);
}

void
DialogItemSliderSetValPtr(DItem * di, int *val_ptr)
{
   di->item.slider.val_ptr = val_ptr;
   DialogItemSliderSetVal(di, *val_ptr);
}

void
DialogItemSliderSetBounds(DItem * di, int lower, int upper)
{
   if (upper == lower)
      upper = lower + 1;
   di->item.slider.lower = lower;
   di->item.slider.upper = upper;

   if (di->realized)
      DialogDrawItems(di->dlg, di, di->x, di->y, di->w, di->h);
}

void
DialogItemSliderSetUnits(DItem * di, int units)
{
   di->item.slider.unit = units;
}

void
DialogItemSliderSetJump(DItem * di, int jump)
{
   di->item.slider.jump = jump;
}

void
DialogItemSliderSetMinLength(DItem * di, int min)
{
   di->item.slider.min_length = min;
}

void
DialogItemSliderSetOrientation(DItem * di, char horizontal)
{
   di->item.slider.horizontal = horizontal;
}

void
DialogItemSliderGetBounds(const DItem * di, int *lower, int *upper)
{
   if (lower)
      *lower = di->item.slider.lower;
   if (upper)
      *upper = di->item.slider.upper;
}

void
DialogItemAreaSetSize(DItem * di, int w, int h)
{
   di->item.area.w = w;
   di->item.area.h = h;
}

Win
DialogItemAreaGetWindow(const DItem * di)
{
   return di->item.area.area_win;
}

void
DialogItemAreaGetSize(const DItem * di, int *w, int *h)
{
   *w = di->item.area.w;
   *h = di->item.area.h;
}

void
DialogItemAreaSetInitFunc(DItem * di, DialogItemCallbackFunc * func)
{
   di->item.area.init_func = func;
}

void
DialogItemAreaSetEventFunc(DItem * di, DialogItemCallbackFunc * func)
{
   di->item.area.event_func = func;
}

void
DialogItemTableEmpty(DItem * di)
{
   int                 i;

   if (di->type != DITEM_TABLE)
      return;

   for (i = 0; i < di->item.table.num_items; i++)
      DialogItemDestroy(di->item.table.items[i], 1);

   Efree(di->item.table.items);
   di->item.table.items = NULL;
   di->item.table.num_items = 0;
}

static void
DialogItemDestroy(DItem * di, int clean)
{
   if (di->type == DITEM_TABLE)
      DialogItemTableEmpty(di);

   Efree(di->text);

   switch (di->type)
     {
     default:
	break;
     case DITEM_CHECKBUTTON:
	if (!clean)
	   break;
	EDestroyWindow(di->item.check_button.check_win);
	break;
     case DITEM_IMAGE:
	Efree(di->item.image.image);
	break;
     case DITEM_RADIOBUTTON:
	if (!clean)
	   break;
	EDestroyWindow(di->item.radio_button.radio_win);
	break;
     case DITEM_SLIDER:
	ImageclassFree(di->item.slider.ic_base);
	ImageclassFree(di->item.slider.ic_knob);
	if (!clean)
	   break;
	EDestroyWindow(di->item.slider.base_win);
	EDestroyWindow(di->item.slider.knob_win);
	break;
     case DITEM_AREA:
	if (!clean)
	   break;
	EDestroyWindow(di->item.area.area_win);
	break;
     }

   if (clean && di->win)
      EDestroyWindow(di->win);
   ImageclassFree(di->iclass);
   TextclassFree(di->tclass);

   Efree(di);
}

/* Convenience callback to close dialog */
void
DialogCallbackClose(Dialog * d, int val __UNUSED__, void *data __UNUSED__)
{
   DialogClose(d);
}

/*
 * Predefined dialogs
 */

void
DialogOK(const char *title, const char *fmt, ...)
{
   char                text[10240];
   va_list             args;

   va_start(args, fmt);
   Evsnprintf(text, sizeof(text), fmt, args);
   va_end(args);

   DialogOKstr(title, text);
}

void
DialogOKstr(const char *title, const char *txt)
{
   Dialog             *d;
   DItem              *table, *di;

   d = DialogCreate("DIALOG");

   table = DialogInitItem(d);
   DialogSetTitle(d, title);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetText(di, txt);

   di = DialogItemAddButton(table, _("OK"), DialogCallbackClose, 0, 1,
			    DLG_BUTTON_OK);
   DialogItemSetFill(di, 0, 0);

   DialogBindKey(d, "Return", DialogCallbackClose, 0, NULL);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0, NULL);

   DialogShow(d);
}

/*
 * Dialog event handlers
 */
static int
_DlgPixToVal(const DItem * di, int dx, int sr)
{
   int                 vr, val;

   vr = di->item.slider.upper - di->item.slider.lower;
   dx = (int)(((float)dx / (sr * di->item.slider.unit)) * abs(vr) + .5);
   dx *= di->item.slider.unit;
   if (vr < 0)
      dx = -dx;
   val = di->item.slider.lower + dx;

   val = _DialogItemSliderClampVal(di, val);

   return val;
}

static void
DialogEventKeyPress(Dialog * d, XEvent * ev)
{
   int                 i;

   for (i = 0; i < d->num_bindings; i++)
     {
	if (ev->xkey.keycode != d->keybindings[i].key)
	   continue;
	d->keybindings[i].func(d, d->keybindings[i].val,
			       d->keybindings[i].data);
	break;
     }
}

static void
DialogHandleEvents(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Dialog             *d = (Dialog *) prm;

   switch (ev->type)
     {
     case KeyPress:
	DialogEventKeyPress(d, ev);
	break;
     }

   if (d->close)
      _DialogClose(d);
}

static void
DItemEventMotion(Win win __UNUSED__, DItem * di, XEvent * ev)
{
   int                 val;

   switch (di->type)
     {
     case DITEM_AREA:
	if (di->item.area.event_func)
	   di->item.area.event_func(di, 0, ev);
	break;

     case DITEM_SLIDER:
	if (!di->item.slider.in_drag)
	   break;
	if (ev->xmotion.window == WinGetXwin(di->item.slider.knob_win))
	  {
	     EQueryPointer(di->item.slider.knob_win,
			   &ev->xbutton.x, &ev->xbutton.y, NULL, NULL);
	     if (di->item.slider.horizontal)
	       {
		  val =
		     _DlgPixToVal(di,
				  ev->xbutton.x + di->item.slider.knob_x -
				  di->item.slider.knob_w / 2,
				  di->item.slider.base_w -
				  di->item.slider.knob_w);
	       }
	     else
	       {
		  val =
		     _DlgPixToVal(di,
				  ev->xbutton.y + di->item.slider.knob_y -
				  di->item.slider.knob_h / 2,
				  di->item.slider.base_h -
				  di->item.slider.knob_h);
	       }
	     if (di->item.slider.val_ptr)
		*di->item.slider.val_ptr = val;
	     if (di->func)
		(di->func) (di->dlg, di->val, di->data);
	  }

	DialogDrawItems(di->dlg, di, di->x, di->y, di->w, di->h);
	break;
     }
}

static void
DItemEventMouseDown(Win win, DItem * di, XEvent * ev)
{
   int                 x, y, jump, val;

   switch (di->type)
     {
     case DITEM_AREA:
	if (di->item.area.event_func)
	   di->item.area.event_func(di, 0, ev);
	break;

     case DITEM_SLIDER:
	if (ev->xbutton.window == WinGetXwin(di->item.slider.knob_win))
	  {
	     if (ev->xbutton.button >= 1 && ev->xbutton.button <= 3)
	       {
		  di->item.slider.in_drag = 1;
		  break;
	       }
	  }

	val = (di->item.slider.val_ptr) ?
	   *(di->item.slider.val_ptr) : di->item.slider.lower;
	jump = 0;

	/* Coords -> item.slider.base_win */
	ETranslateCoordinates(win, di->item.slider.base_win,
			      ev->xbutton.x, ev->xbutton.y, &x, &y, NULL);

	switch (ev->xbutton.button)
	  {
	  case 1:
	  case 3:
	     if (di->item.slider.horizontal)
	       {
		  if (ev->xbutton.x >
		      (di->item.slider.knob_x + (di->item.slider.knob_w / 2)))
		     jump = di->item.slider.jump;
		  else
		     jump = -di->item.slider.jump;
	       }
	     else
	       {
		  if (ev->xbutton.y >
		      (di->item.slider.knob_y + (di->item.slider.knob_h / 2)))
		     jump = di->item.slider.jump;
		  else
		     jump = -di->item.slider.jump;
	       }
	     break;

	  case 2:
	     if (di->item.slider.horizontal)
	       {
		  val =
		     _DlgPixToVal(di,
				  ev->xbutton.x - di->item.slider.knob_w / 2,
				  di->item.slider.base_w -
				  di->item.slider.knob_w);
	       }
	     else
	       {
		  val =
		     _DlgPixToVal(di,
				  ev->xbutton.y - di->item.slider.knob_h / 2,
				  di->item.slider.base_h -
				  di->item.slider.knob_h);
	       }
	     break;

	  case 4:
	  case 5:
	     jump = di->item.slider.jump / 2;
	     if (!jump)
		jump++;

	     if (ev->xbutton.button == 5)
		jump = -jump;
	     break;
	  }
	if (di->item.slider.lower > di->item.slider.upper)
	   jump = -jump;
	val = _DialogItemSliderClampVal(di, val + jump);
	if (di->item.slider.val_ptr)
	   *di->item.slider.val_ptr = val;
#if 0				/* Remove? */
	if (di->func)
	   (di->func) (d, di->val, di->data);
#endif
	break;
     }

   di->clicked = 1;

   DialogDrawItems(di->dlg, di, di->x, di->y, di->w, di->h);
}

static void
DItemEventMouseUp(Win win, DItem * di, XEvent * ev)
{
   DItem              *dii;

   if (ev->xbutton.window != Mode.events.last_bpress)
      return;

   switch (di->type)
     {
     case DITEM_AREA:
	if (di->item.area.event_func)
	   di->item.area.event_func(di, 0, ev);
	break;

     case DITEM_CHECKBUTTON:
	DialogItemCheckButtonSetState(di, !DialogItemCheckButtonGetState(di));
	break;

     case DITEM_RADIOBUTTON:
	dii = di->item.radio_button.first;
	while (dii)
	  {
	     if (dii->item.radio_button.onoff)
	       {
		  dii->item.radio_button.onoff = 0;
		  DialogDrawItems(di->dlg, dii, dii->x, dii->y, dii->w, dii->h);
	       }
	     dii = dii->item.radio_button.next;
	  }
	di->item.radio_button.onoff = 1;
	if (di->item.radio_button.val_ptr)
	   *di->item.radio_button.val_ptr = di->item.radio_button.val;
	break;

     case DITEM_SLIDER:
	if (win == di->item.slider.knob_win)
	   di->item.slider.in_drag = 0;
	break;
     }

   if (di->hilited && di->clicked)
     {
	if (di->func)
	   di->func(di->dlg, di->val, di->data);

	if (di->do_close)
	   di->dlg->close = 1;
     }

   di->clicked = 0;

   DialogDrawItems(di->dlg, di, di->x, di->y, di->w, di->h);
}

static void
DItemEventMouseIn(Win win __UNUSED__, DItem * di, XEvent * ev)
{
   switch (di->type)
     {
     case DITEM_AREA:
	if (di->item.area.event_func)
	   di->item.area.event_func(di, 0, ev);
	break;
     }

   di->hilited = 1;

   DialogDrawItems(di->dlg, di, di->x, di->y, di->w, di->h);
}

static void
DItemEventMouseOut(Win win __UNUSED__, DItem * di, XEvent * ev)
{
   switch (di->type)
     {
     case DITEM_AREA:
	if (di->item.area.event_func)
	   di->item.area.event_func(di, 0, ev);
	break;
     }

   if (!di->clicked)
      di->hilited = 0;

   DialogDrawItems(di->dlg, di, di->x, di->y, di->w, di->h);
}

static void
DItemHandleEvents(Win win, XEvent * ev, void *prm)
{
   DItem              *di = (DItem *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	DItemEventMouseDown(win, di, ev);
	break;
     case ButtonRelease:
	DItemEventMouseUp(win, di, ev);
	break;
     case MotionNotify:
	DItemEventMotion(win, di, ev);
	break;
     case EnterNotify:
	DItemEventMouseIn(win, di, ev);
	break;
     case LeaveNotify:
	DItemEventMouseOut(win, di, ev);
	break;
     }

   if (di->dlg->close)
      _DialogClose(di->dlg);
}

/*
 * Finders
 */

static EWin        *
FindEwinByDialog(Dialog * d)
{
   EWin               *const *ewins;
   int                 i, num;

   ewins = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if ((Dialog *) (ewins[i]->data) == d)
	   return ewins[i];
     }

   return NULL;
}

static int
FindADialog(void)
{
   EWin               *const *ewins;
   int                 i, num, n;

   ewins = EwinListGetAll(&num);
   for (i = n = 0; i < num; i++)
     {
	if (ewins[i]->type == EWIN_TYPE_DIALOG)
	   n++;
     }

   return n;
}

#endif /* ENABLE_DIALOGS */

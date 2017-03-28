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
#include "settings.h"

typedef struct {
   int                 move;
   int                 resize;
   int                 geominfo;
   int                 maximize;
   int                 maximize_speed;
   char                maximize_animate;
   char                dragbar_nocover;
   char                enable_smart_max_hv;
   char                avoid_server_grab;
   char                update_while_moving;
   char                sync_request;
} MovResDlgData;

static void
CB_ConfigureMoveResize(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   MovResDlgData      *dd = DLG_DATA_GET(d, MovResDlgData);

   if (val >= 2)
      return;

   Conf.movres.mode_move = dd->move;
   Conf.movres.mode_resize = dd->resize;
   Conf.movres.mode_info = dd->geominfo;
   Conf.movres.mode_maximize_default = dd->maximize;
   Conf.movres.avoid_server_grab = dd->avoid_server_grab;
   Conf.movres.update_while_moving = dd->update_while_moving;
   Conf.movres.enable_sync_request = dd->sync_request;
   Conf.movres.dragbar_nocover = dd->dragbar_nocover;
   Conf.movres.enable_smart_max_hv = dd->enable_smart_max_hv;
   Conf.movres.maximize_speed = dd->maximize_speed;
   Conf.movres.maximize_animate = dd->maximize_animate;

   autosave();
}

static void
_DlgFillMoveResize(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *radio1, *radio2, *radio3, *radio4;
   MovResDlgData      *dd;

   dd = DLG_DATA_SET(d, MovResDlgData);
   if (!dd)
      return;

   dd->move = Conf.movres.mode_move;
   dd->resize = Conf.movres.mode_resize;
   dd->geominfo = Conf.movres.mode_info;
   dd->maximize = Conf.movres.mode_maximize_default;
   dd->avoid_server_grab = Conf.movres.avoid_server_grab;
   dd->update_while_moving = Conf.movres.update_while_moving;
   dd->sync_request = Conf.movres.enable_sync_request;
   dd->dragbar_nocover = Conf.movres.dragbar_nocover;
   dd->enable_smart_max_hv = Conf.movres.enable_smart_max_hv;
   dd->maximize_speed = Conf.movres.maximize_speed;
   dd->maximize_animate = Conf.movres.maximize_animate;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Move Methods:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Resize Methods:"));

   radio1 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Opaque"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, MR_OPAQUE);

   radio2 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Opaque"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, MR_OPAQUE);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Technical"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, MR_TECHNICAL);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Technical"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, MR_TECHNICAL);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("TechOpaque"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, MR_TECH_OPAQUE);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("TechOpaque"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, MR_TECH_OPAQUE);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Box"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, MR_BOX);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Box"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, MR_BOX);

#if ENABLE_OLDMOVRES
   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Shaded"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, MR_SHADED);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Shaded"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, MR_SHADED);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Semi-Solid"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, MR_SEMI_SOLID);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Semi-Solid"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, MR_SEMI_SOLID);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Translucent"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, MR_TRANSLUCENT);

   DialogAddItem(table, DITEM_NONE);
#endif /* ENABLE_OLDMOVRES */

   DialogItemRadioButtonGroupSetValPtr(radio1, &dd->move);
   DialogItemRadioButtonGroupSetValPtr(radio2, &dd->resize);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Avoid server grab"));
   DialogItemCheckButtonSetPtr(di, &dd->avoid_server_grab);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Move/Resize Geometry Info Postion:"));

   radio3 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Window Center (O/T/B Methods)"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Always Screen corner"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Don't show"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 0);
   DialogItemRadioButtonGroupSetValPtr(radio3, &dd->geominfo);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Default Resize Policy:"));

   radio4 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Conservative"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Available"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Absolute"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 0);
   DialogItemRadioButtonGroupSetValPtr(radio4, &dd->maximize);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Update window while moving"));
   DialogItemCheckButtonSetPtr(di, &dd->update_while_moving);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Synchronize move/resize with application"));
   DialogItemCheckButtonSetPtr(di, &dd->sync_request);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Do not cover dragbar"));
   DialogItemCheckButtonSetPtr(di, &dd->dragbar_nocover);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable smart maximization"));
   DialogItemCheckButtonSetPtr(di, &dd->enable_smart_max_hv);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Animate window maximization"));
   DialogItemCheckButtonSetPtr(di, &dd->maximize_animate);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Maximization animation speed:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetValPtr(di, &dd->maximize_speed);
}

const DialogDef     DlgMoveResize = {
   "CONFIGURE_MOVERESIZE",
   N_("Move/Resize"),
   N_("Move & Resize Settings"),
   SOUND_SETTINGS_MOVERESIZE,
   "pix/moveres.png",
   N_("Enlightenment Move & Resize\n" "Method Settings Dialog"),
   _DlgFillMoveResize,
   DLG_OAC, CB_ConfigureMoveResize,
};

typedef struct {
   char                with_leader;
   char                switch_popup;
   char                manual_placement;
   char                manual_placement_mouse_pointer;
   char                center_if_desk_full;
   char                map_slide;
   char                cleanup_slide;
   int                 slide_mode;
   int                 map_slide_speed;
   int                 cleanup_slide_speed;
   char                animate_shading;
   int                 shade_speed;
   char                place_ignore_struts;
   char                place_ignore_struts_fullscreen;
   char                place_ignore_struts_maximize;
   char                raise_fullscreen;
#ifdef USE_XINERAMA_no		/* Not implemented */
   char                extra_head;
#endif
} PlaceDlgData;

static void
CB_ConfigurePlacement(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   PlaceDlgData       *dd = DLG_DATA_GET(d, PlaceDlgData);

   if (val >= 2)
      return;

   Conf.focus.transientsfollowleader = dd->with_leader;
   Conf.focus.switchfortransientmap = dd->switch_popup;

   Conf.place.manual = dd->manual_placement;
   Conf.place.manual_mouse_pointer = dd->manual_placement_mouse_pointer;
   Conf.place.center_if_desk_full = dd->center_if_desk_full;

   Conf.place.slidein = dd->map_slide;
   Conf.place.cleanupslide = dd->cleanup_slide;
   Conf.place.slidemode = dd->slide_mode;
   Conf.place.slidespeedmap = dd->map_slide_speed;
   Conf.place.slidespeedcleanup = dd->cleanup_slide_speed;

   Conf.shading.animate = dd->animate_shading;
   Conf.shading.speed = dd->shade_speed;

   Conf.place.ignore_struts = dd->place_ignore_struts;
   Conf.place.ignore_struts_fullscreen = dd->place_ignore_struts_fullscreen;
   Conf.place.ignore_struts_maximize = dd->place_ignore_struts_maximize;
   Conf.place.raise_fullscreen = dd->raise_fullscreen;
#ifdef USE_XINERAMA_no		/* Not implemented */
   if (Mode.display.xinerama_active)
      Conf.place.extra_head = dd->extra_head;
#endif

   autosave();
}

static void
_DlgFillPlacement(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *radio;
   PlaceDlgData       *dd;

   dd = DLG_DATA_SET(d, PlaceDlgData);
   if (!dd)
      return;

   dd->with_leader = Conf.focus.transientsfollowleader;
   dd->switch_popup = Conf.focus.switchfortransientmap;

   dd->manual_placement = Conf.place.manual;
   dd->manual_placement_mouse_pointer = Conf.place.manual_mouse_pointer;
   dd->center_if_desk_full = Conf.place.center_if_desk_full;

   dd->map_slide = Conf.place.slidein;
   dd->cleanup_slide = Conf.place.cleanupslide;
   dd->slide_mode = Conf.place.slidemode;
   dd->map_slide_speed = Conf.place.slidespeedmap;
   dd->cleanup_slide_speed = Conf.place.slidespeedcleanup;

   dd->animate_shading = Conf.shading.animate;
   dd->shade_speed = Conf.shading.speed;

   dd->place_ignore_struts = Conf.place.ignore_struts;
   dd->place_ignore_struts_fullscreen = Conf.place.ignore_struts_fullscreen;
   dd->place_ignore_struts_maximize = Conf.place.ignore_struts_maximize;
   dd->raise_fullscreen = Conf.place.raise_fullscreen;

#ifdef USE_XINERAMA_no		/* Not implemented */
   dd->extra_head = Conf.place.extra_head;
#endif

   DialogItemTableSetOptions(table, 2, 0, 1, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Dialog windows appear together with their owner"));
   DialogItemCheckButtonSetPtr(di, &dd->with_leader);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Switch to desktop where dialog appears"));
   DialogItemCheckButtonSetPtr(di, &dd->switch_popup);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Place windows manually"));
   DialogItemCheckButtonSetPtr(di, &dd->manual_placement);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Place windows under mouse"));
   DialogItemCheckButtonSetPtr(di, &dd->manual_placement_mouse_pointer);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Center windows when desk is full"));
   DialogItemCheckButtonSetPtr(di, &dd->center_if_desk_full);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Slide windows in when they appear"));
   DialogItemCheckButtonSetPtr(di, &dd->map_slide);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Slide windows around when cleaning up"));
   DialogItemCheckButtonSetPtr(di, &dd->cleanup_slide);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Slide Method:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Opaque"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, MR_OPAQUE);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Technical"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, MR_TECHNICAL);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("TechOpaque"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, MR_TECH_OPAQUE);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Box"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, MR_BOX);

#if ENABLE_OLDMOVRES
   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Shaded"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, MR_SHADED);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Semi-Solid"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, MR_SEMI_SOLID);
#endif /* ENABLE_OLDMOVRES */

   DialogItemRadioButtonGroupSetValPtr(radio, &dd->slide_mode);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Appear Slide speed:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetValPtr(di, &dd->map_slide_speed);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Cleanup Slide speed:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetValPtr(di, &dd->cleanup_slide_speed);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Animate shading and unshading of windows"));
   DialogItemCheckButtonSetPtr(di, &dd->animate_shading);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Window Shading speed:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetValPtr(di, &dd->shade_speed);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Ignore struts/panels for placing normal windows"));
   DialogItemCheckButtonSetPtr(di, &dd->place_ignore_struts);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Ignore struts/panels when windows are fullscreen"));
   DialogItemCheckButtonSetPtr(di, &dd->place_ignore_struts_fullscreen);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Ignore struts/panels when maximizing windows"));
   DialogItemCheckButtonSetPtr(di, &dd->place_ignore_struts_maximize);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raise fullscreen windows"));
   DialogItemCheckButtonSetPtr(di, &dd->raise_fullscreen);

#ifdef USE_XINERAMA_no		/* Not implemented */
   if (Mode.display.xinerama_active)
     {
	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetText(di, _("Place windows on another head when full"));
	DialogItemCheckButtonSetPtr(di, &dd->extra_head);
     }
#endif
}

const DialogDef     DlgPlacement = {
   "CONFIGURE_PLACEMENT",
   N_("Placement"),
   N_("Window Placement Settings"),
   SOUND_SETTINGS_PLACEMENT,
   "pix/place.png",
   N_("Enlightenment Window Placement\n" "Settings Dialog"),
   _DlgFillPlacement,
   DLG_OAC, CB_ConfigurePlacement,
};

typedef struct {
   char                dialog_headers;
   char                button_image;
   char                animate_startup;
   char                saveunders;
   int                 magwin_zoom_res;
} MiscDlgData;

static void
CB_ConfigureMiscellaneous(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   MiscDlgData        *dd = DLG_DATA_GET(d, MiscDlgData);

   if (val >= 2)
      return;

   Conf.dialogs.headers = dd->dialog_headers;
   Conf.dialogs.button_image = dd->button_image;
   Conf.startup.animate = dd->animate_startup;
   Conf.save_under = dd->saveunders;
   Conf.magwin.zoom_res = dd->magwin_zoom_res;

   autosave();
}

static void
_DlgFillMisc(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di;
   MiscDlgData        *dd;

   dd = DLG_DATA_SET(d, MiscDlgData);
   if (!dd)
      return;

   dd->dialog_headers = Conf.dialogs.headers;
   dd->button_image = Conf.dialogs.button_image;
   dd->animate_startup = Conf.startup.animate;
   dd->saveunders = Conf.save_under;
   dd->magwin_zoom_res = Conf.magwin.zoom_res;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Dialog Headers"));
   DialogItemCheckButtonSetPtr(di, &dd->dialog_headers);

#if 0				/* Not functional */
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Button Images"));
   DialogItemCheckButtonSetPtr(di, &dd->button_image);
#endif

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable sliding startup windows"));
   DialogItemCheckButtonSetPtr(di, &dd->animate_startup);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Use saveunders to reduce window exposures"));
   DialogItemCheckButtonSetPtr(di, &dd->saveunders);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Magwin zoom resolution"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 2);
   DialogItemSliderSetBounds(di, 1, 32);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSliderSetValPtr(di, &dd->magwin_zoom_res);
}

const DialogDef     DlgMisc = {
   "CONFIGURE_MISCELLANEOUS",
   N_("Miscellaneous"),
   N_("Miscellaneous Settings"),
   SOUND_SETTINGS_MISCELLANEOUS,
   "pix/miscellaneous.png",
   N_("Enlightenment Miscellaneous\n" "Settings Dialog"),
   _DlgFillMisc,
   DLG_OAC, CB_ConfigureMiscellaneous,
};

#if USE_COMPOSITE
#include "ecompmgr.h"

/*
 * Composite dialog
 */

static void
CB_ConfigureComposite(Dialog * d, int val, void *data __UNUSED__)
{
   cfg_composite      *dd;

   if (val >= 2)
      return;

   /* Configure and read back */
   dd = DLG_DATA_GET(d, cfg_composite);
   ECompMgrConfigSet(dd);
   ECompMgrConfigGet(dd);
}

static void
_DlgFillComposite(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *radio;
   cfg_composite      *dd;

   dd = DLG_DATA_SET(d, cfg_composite);
   if (!dd)
      return;

   /* Get current settings */
   ECompMgrConfigGet(dd);

   /* Layout */
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Composite"));
   DialogItemCheckButtonSetPtr(di, &dd->enable);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Fading"));
   DialogItemCheckButtonSetPtr(di, &dd->fading);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Fading Speed:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 100);
   DialogItemSliderSetUnits(di, 5);
   DialogItemSliderSetJump(di, 5);
   DialogItemSliderSetValPtr(di, &dd->fade_speed);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shadows Off"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shadows Sharp"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shadows Sharp2"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shadows Blurred"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->shadow);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Default focused window opacity:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 100);
   DialogItemSliderSetUnits(di, 5);
   DialogItemSliderSetJump(di, 5);
   DialogItemSliderSetValPtr(di, &dd->opacity_focused);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Default unfocused window opacity:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 100);
   DialogItemSliderSetUnits(di, 5);
   DialogItemSliderSetJump(di, 5);
   DialogItemSliderSetValPtr(di, &dd->opacity_unfocused);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Default pop-up window opacity:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 100);
   DialogItemSliderSetUnits(di, 5);
   DialogItemSliderSetJump(di, 5);
   DialogItemSliderSetValPtr(di, &dd->opacity_override);
}

const DialogDef     DlgComposite = {
   "CONFIGURE_COMPOSITE",
   N_("Composite"),
   N_("Composite Settings"),
   SOUND_SETTINGS_COMPOSITE,
   "pix/pager.png",
   N_("Enlightenment Composite\n" "Settings Dialog"),
   _DlgFillComposite,
   DLG_OAC, CB_ConfigureComposite,
};
#endif

/*
 * Combined configuration dialog
 */

static const DialogDef *dialogs[] = {
   &DlgFocus,
   &DlgMoveResize,
   &DlgPlacement,
   &DlgDesks,
   &DlgAreas,
   &DlgPagers,
   &DlgMenus,
   &DlgTooltips,
#if HAVE_SOUND
   &DlgSound,
#endif
   &DlgGroupDefaults,
   &DlgRemember,
   &DlgFx,
   &DlgBackground,
   &DlgTheme,
#ifdef ENABLE_THEME_TRANSPARENCY
   &DlgThemeTrans,
#endif
#if USE_COMPOSITE
   &DlgComposite,
#endif
   &DlgSession,
   &DlgMisc,
};
#define N_CFG_DLGS (sizeof(dialogs)/sizeof(DialogDef*))

static void
CB_DlgSelect(Dialog * d, int val, void *data)
{
   const DialogDef    *dd = dialogs[val];
   DItem              *table = (DItem *) data;

   if (!table)
      return;
   if (!dd->fill)
      return;

   DialogCallExitFunction(d);

   DialogItemTableEmpty(table);
   DialogKeybindingsDestroy(d);

   DialogSetTitle(d, _(dd->title));
   DialogFill(d, table, dd, NULL);

   DialogArrange(d, 1);
}

static void
_DlgFillConfiguration(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *buttons, *content;
   unsigned int        i;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   buttons = DialogAddItem(table, DITEM_TABLE);
   content = DialogAddItem(table, DITEM_TABLE);

   for (i = 0; i < N_CFG_DLGS; i++)
     {
	di = DialogAddItem(buttons, DITEM_BUTTON);
	DialogItemSetPadding(di, 2, 2, 0, 0);
	DialogItemSetText(di, _(dialogs[i]->label));
	DialogItemSetCallback(di, CB_DlgSelect, i, content);
     }

   DialogFill(d, content, dialogs[0], NULL);
}

static const DialogDef DlgConfiguration = {
   "CONFIGURE_ALL",
   NULL,
   N_("Enlightenment Settings"),
   SOUND_SETTINGS_ALL,
   NULL,
   NULL,
   _DlgFillConfiguration,
   0, NULL,
};

void
IPC_Cfg(const char *params)
{
   unsigned int        i;
   const char         *name;

   if (!params || !params[0])
     {
	DialogShowSimple(&DlgConfiguration, NULL);
	return;
     }

   for (i = 0; i < N_CFG_DLGS; i++)
     {
	name = dialogs[i]->label;
	if (strcasecmp(params, name))
	   continue;
	DialogShowSimple(dialogs[i], NULL);
     }
}
#endif /* ENABLE_DIALOGS */

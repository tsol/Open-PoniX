/*
 * Copyright (C) 2004-2007 Jaron Omega
 * Copyright (C) 2004-2011 Kim Woelders
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
#include "dialog.h"
#include "emodule.h"
#include "iclass.h"
#include "settings.h"
#include "timers.h"

#ifdef ENABLE_THEME_TRANSPARENCY
/*
 * Theme transparency settings
 */

#if ENABLE_DIALOGS
static Timer       *st_timer = NULL;

static int
TransparencyChangeTimeout(void *data)
{
   TransparencySet(PTR2INT(data));

   st_timer = NULL;
   return 0;
}

static void
TransparencyChange(int val)
{
   TIMER_DEL(st_timer);
   TIMER_ADD(st_timer, 10, TransparencyChangeTimeout, INT2PTR(val));
}

/*
 * Configuration dialog
 */
typedef struct {
   int                 theme_transparency;

   int                 st_border;
   int                 st_widget;
   int                 st_dialog;
   int                 st_menu;
   int                 st_tooltip;
   int                 st_hilight;
} TransDlgData;

static void
CB_ConfigureTrans(Dialog * d, int val, void *data __UNUSED__)
{
   TransDlgData       *dd = DLG_DATA_GET(d, TransDlgData);

   if (val >= 2)
      return;

   Conf.trans.border = dd->st_border;
   Conf.trans.widget = dd->st_widget;
   Conf.trans.dialog = dd->st_dialog;
   Conf.trans.menu = dd->st_menu;
   Conf.trans.tooltip = dd->st_tooltip;
   Conf.trans.hilight = Conf.trans.menu_item = dd->st_hilight;
   Conf.trans.pager = ICLASS_ATTR_BG;
   Conf.trans.iconbox = ICLASS_ATTR_BG;
   Conf.trans.warplist = ICLASS_ATTR_BG;
#if 0				/* Should not be necessary */
   TransparencyChange(dd->theme_transparency);
#endif

   autosave();
}

static void
_DlgThemeTransparencyText(DItem * di, TransDlgData * dd)
{
   char                s[256];

   Esnprintf(s, sizeof(s), _("Theme transparency: %2d"),
	     dd->theme_transparency);
   DialogItemSetText(di, s);
}

static void
CB_ThemeTransparency(Dialog * d, int val __UNUSED__, void *data)
{
   TransDlgData       *dd = DLG_DATA_GET(d, TransDlgData);
   DItem              *di = (DItem *) data;

   _DlgThemeTransparencyText(di, dd);
   TransparencyChange(dd->theme_transparency);
}

static void
_DlgFillThemeTrans(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *label;
   DItem              *radio_border, *radio_widget, *radio_menu,
      *radio_dialog, *radio_tooltip, *radio_hilight;
   TransDlgData       *dd;

   dd = DLG_DATA_SET(d, TransDlgData);
   if (!dd)
      return;

   dd->st_border = Conf.trans.border;
   dd->st_widget = Conf.trans.widget;
   dd->st_dialog = Conf.trans.dialog;
   dd->st_menu = Conf.trans.menu;
   dd->st_tooltip = Conf.trans.tooltip;
   dd->st_hilight = Conf.trans.hilight;

   dd->theme_transparency = Conf.trans.alpha;

   DialogItemTableSetOptions(table, 7, 0, 0, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 7);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Changes Might Require Restart:"));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 7);

   DialogAddItem(table, DITEM_NONE);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Borders:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Menus:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Hilights:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("E Widgets:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("E Dialogs:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Tooltips:"));

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 1024);
   DialogItemSetText(di, _("Opaque"));

   radio_border = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_border);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio_menu = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_menu);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio_hilight = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_hilight);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio_widget = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_widget);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio_dialog = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_dialog);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio_tooltip = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_tooltip);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 1024);
   DialogItemSetText(di, _("Background"));

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_border);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio_border, &dd->st_border);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_menu);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio_menu, &dd->st_menu);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_hilight);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio_hilight, &dd->st_hilight);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_widget);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio_widget, &dd->st_widget);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_dialog);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio_dialog, &dd->st_dialog);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_tooltip);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 1024);
   DialogItemSetText(di, _("Glass"));

   di = DialogAddItem(table, DITEM_NONE);
   DialogItemSetColSpan(di, 5);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemRadioButtonSetFirst(di, radio_tooltip);
   DialogItemRadioButtonGroupSetVal(di, 2);
   DialogItemRadioButtonGroupSetValPtr(radio_tooltip, &dd->st_tooltip);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 7);

   di = label = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 7);
   DialogItemSetAlign(di, 512, 512);
   _DlgThemeTransparencyText(di, dd);

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 7);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetBounds(di, 0, 255);
   DialogItemSliderSetUnits(di, 4);
   DialogItemSliderSetJump(di, 16);
   DialogItemSliderSetValPtr(di, &dd->theme_transparency);
   DialogItemSetCallback(di, CB_ThemeTransparency, 0, (void *)label);
}

const DialogDef     DlgThemeTrans = {
   "CONFIGURE_TRANS",
   N_("Transparency"),
   N_("Selective Transparency Settings"),
   SOUND_SETTINGS_TRANS,
   "pix/tips.png",
   N_("Enlightenment Selective Transparency\n" "Settings Dialog"),
   _DlgFillThemeTrans,
   DLG_OAC, CB_ConfigureTrans,
};
#endif /* ENABLE_DIALOGS */

static void
TransparencySighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_CONFIGURE:
	TransparencySet(Conf.trans.alpha);
	break;
     }
}

static const CfgItem TransCfgItems[] = {
   CFG_ITEM_INT(Conf.trans, alpha, 0),
   CFG_ITEM_INT(Conf.trans, menu, ICLASS_ATTR_BG),
   CFG_ITEM_INT(Conf.trans, menu_item, ICLASS_ATTR_BG),
   CFG_ITEM_INT(Conf.trans, tooltip, ICLASS_ATTR_GLASS),
   CFG_ITEM_INT(Conf.trans, widget, ICLASS_ATTR_BG),
   CFG_ITEM_INT(Conf.trans, hilight, ICLASS_ATTR_OPAQUE),
   CFG_ITEM_INT(Conf.trans, border, ICLASS_ATTR_BG),
   CFG_ITEM_INT(Conf.trans, iconbox, ICLASS_ATTR_BG),
   CFG_ITEM_INT(Conf.trans, dialog, ICLASS_ATTR_BG),
   CFG_ITEM_INT(Conf.trans, pager, ICLASS_ATTR_BG),
   CFG_ITEM_INT(Conf.trans, warplist, ICLASS_ATTR_BG),
};
#define N_CFG_ITEMS (sizeof(TransCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModTransparency;

const EModule       ModTransparency = {
   "transparency", "tr",
   TransparencySighan,
   {0, NULL},
   {N_CFG_ITEMS, TransCfgItems}
};

#endif /* ENABLE_THEME_TRANSPARENCY */

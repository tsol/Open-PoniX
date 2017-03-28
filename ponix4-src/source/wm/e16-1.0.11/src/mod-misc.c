/*
 * Copyright (C) 2003-2012 Kim Woelders
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
#include "eimage.h"
#include "emodule.h"
#include "settings.h"
#include "snaps.h"

/***********************************************************************
 * *** should all go elsewhere ***
 **********************************************************************/

/* Use static module list for now */
extern const EModule ModAclass;
extern const EModule ModBackgrounds;
extern const EModule ModButtons;

#if USE_COMPOSITE
extern const EModule ModCompMgr;
extern const EModule ModMagwin;
#endif
#if USE_GLX
extern const EModule ModGlwin;
#endif
extern const EModule ModCursors;
extern const EModule ModDesktops;
extern const EModule ModEffects;
extern const EModule ModEwins;
extern const EModule ModFocus;
extern const EModule ModGroups;
extern const EModule ModImageclass;
extern const EModule ModIconboxes;
extern const EModule ModLocale;
extern const EModule ModMenus;
extern const EModule ModMisc;
extern const EModule ModPagers;
extern const EModule ModSlideouts;

#if HAVE_SOUND
extern const EModule ModSound;
#endif
extern const EModule ModTextclass;
extern const EModule ModTheme;
extern const EModule ModTooltips;

#ifdef ENABLE_THEME_TRANSPARENCY
extern const EModule ModTransparency;
#endif
extern const EModule ModWarplist;
extern const EModule ModWindowMatch;

const EModule      *const p_modules[] = {
   &ModAclass,
   &ModBackgrounds,
   &ModButtons,
#if USE_COMPOSITE
   &ModCompMgr,
   &ModMagwin,
#endif
   &ModCursors,
   &ModDesktops,
   &ModEwins,
   &ModEffects,
   &ModFocus,
#if USE_GLX
   &ModGlwin,
#endif
   &ModGroups,
   &ModIconboxes,
   &ModImageclass,
   &ModLocale,
   &ModMenus,
   &ModMisc,
   &ModPagers,
   &ModSlideouts,
#if HAVE_SOUND
   &ModSound,
#endif
   &ModTextclass,
   &ModTheme,
   &ModTooltips,
#ifdef ENABLE_THEME_TRANSPARENCY
   &ModTransparency,
#endif
   &ModWarplist,
   &ModWindowMatch,
};
int                 n_modules = sizeof(p_modules) / sizeof(EModule *);

#if 0
static void
MiscSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_START:
	break;
     }
}
#endif

static void
_CfgImageCacheSize(void *item __UNUSED__, const char *value)
{
   int                 size_old, size_new;

   if (!value || !value[0])
      return;

   size_new = atoi(value);
   size_old = EImageSetCacheSize(size_new);
   IpcPrintf("Image cache size %u->%u byte\n", size_old, size_new);
}

static const CfgItem MiscCfgItems[] = {

   CFG_ITEM_INT(Conf, animation.step, 10),

   CFG_ITEM_INT(Conf, buttons.move_resistance, 10),

   CFG_ITEM_BOOL(Conf, dialogs.headers, 0),
   CFG_ITEM_BOOL(Conf, dialogs.button_image, 0),

   CFG_ITEM_BOOL(Conf, dock.enable, 1),
   CFG_ITEM_BOOL(Conf, dock.sticky, 1),
   CFG_ITEM_INT(Conf, dock.dirmode, DOCK_DOWN),
   CFG_ITEM_INT(Conf, dock.startx, 0),
   CFG_ITEM_INT(Conf, dock.starty, 0),

   CFG_ITEM_BOOL(Conf, hints.set_xroot_info_on_root_window, 0),

   CFG_ITEM_BOOL(Conf, log.difftime, 0),
   CFG_ITEM_BOOL(Conf, log.dest, 0),

   CFG_ITEM_INT(Conf, magwin.zoom_res, 4),

   CFG_ITEM_INT(Conf, movres.mode_move, MR_OPAQUE),
   CFG_ITEM_INT(Conf, movres.mode_resize, MR_BOX),
   CFG_ITEM_INT(Conf, movres.mode_info, 1),
   CFG_ITEM_INT(Conf, movres.mode_maximize_default, 1),
   CFG_ITEM_HEX(Conf, movres.color, 0x00ff0000),
   CFG_ITEM_BOOL(Conf, movres.avoid_server_grab, 1),
   CFG_ITEM_BOOL(Conf, movres.update_while_moving, 0),
   CFG_ITEM_BOOL(Conf, movres.enable_sync_request, 0),
   CFG_ITEM_BOOL(Conf, movres.dragbar_nocover, 0),
   CFG_ITEM_BOOL(Conf, movres.enable_smart_max_hv, 0),
   CFG_ITEM_BOOL(Conf, movres.maximize_animate, 0),
   CFG_ITEM_INT(Conf, movres.maximize_speed, 6000),

   CFG_ITEM_INT(Conf, opacity.menus, 85),
   CFG_ITEM_INT(Conf, opacity.movres, 60),
   CFG_ITEM_INT(Conf, opacity.tooltips, 80),
   CFG_ITEM_INT(Conf, opacity.focused, 100),
   CFG_ITEM_INT(Conf, opacity.unfocused, 100),

   CFG_ITEM_BOOL(Conf, place.manual, 0),
   CFG_ITEM_BOOL(Conf, place.manual_mouse_pointer, 0),
   CFG_ITEM_BOOL(Conf, place.center_if_desk_full, 0),
   CFG_ITEM_BOOL(Conf, place.ignore_struts, 0),
   CFG_ITEM_BOOL(Conf, place.ignore_struts_fullscreen, 0),
   CFG_ITEM_BOOL(Conf, place.ignore_struts_maximize, 0),
   CFG_ITEM_BOOL(Conf, place.raise_fullscreen, 0),
   CFG_ITEM_BOOL(Conf, place.slidein, 0),
   CFG_ITEM_BOOL(Conf, place.cleanupslide, 1),
   CFG_ITEM_INT(Conf, place.slidemode, 0),
   CFG_ITEM_INT(Conf, place.slidespeedmap, 6000),
   CFG_ITEM_INT(Conf, place.slidespeedcleanup, 8000),

   CFG_ITEM_BOOL(Conf, session.enable_script, 0),
   CFG_ITEM_STR(Conf, session.script),
   CFG_ITEM_BOOL(Conf, session.enable_logout_dialog, 1),
   CFG_ITEM_BOOL(Conf, session.enable_reboot_halt, 0),
   CFG_ITEM_STR(Conf, session.cmd_reboot),
   CFG_ITEM_STR(Conf, session.cmd_halt),

   CFG_ITEM_BOOL(Conf, shading.animate, 1),
   CFG_ITEM_INT(Conf, shading.speed, 8000),

   CFG_ITEM_BOOL(Conf, snap.enable, 1),
   CFG_ITEM_INT(Conf, snap.edge_snap_dist, 8),
   CFG_ITEM_INT(Conf, snap.screen_snap_dist, 32),

   CFG_ITEM_BOOL(Conf, startup.firsttime, 1),
   CFG_ITEM_BOOL(Conf, startup.animate, 1),

   CFG_ITEM_BOOL(Conf, testing.argb_internal_objects, 0),
   CFG_ITEM_BOOL(Conf, testing.argb_internal_clients, 0),
   CFG_ITEM_BOOL(Conf, testing.argb_clients, 0),
   CFG_ITEM_BOOL(Conf, testing.argb_clients_inherit_attr, 0),
   CFG_FUNC_INT(Conf, testing.image_cache_size, -1, _CfgImageCacheSize),
   CFG_ITEM_INT(Conf, testing.mask_alpha_threshold, 8),
   CFG_ITEM_BOOL(Conf, testing.enable_startup_id, 1),
   CFG_ITEM_BOOL(Conf, testing.use_render_for_scaling, 0),
   CFG_ITEM_BOOL(Conf, testing.bindings_reload, 1),
   CFG_ITEM_HEX(Conf, testing.no_sync_mask, 0),

   CFG_ITEM_BOOL(Conf, autosave, 1),
   CFG_ITEM_BOOL(Conf, memory_paranoia, 1),
   CFG_ITEM_BOOL(Conf, save_under, 0),
};
#define N_CFG_ITEMS ((int)(sizeof(MiscCfgItems)/sizeof(CfgItem)))

/* Stuff not elsewhere */
const EModule       ModMisc = {
   "misc", NULL,
   NULL,
   {0, NULL},
   {N_CFG_ITEMS, MiscCfgItems}
};

void
autosave(void)
{
   if (!Mode.wm.save_ok)
      return;

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("autosave\n");

   SnapshotsSaveReal();

   /* Save the configuration parameters */
   ConfigurationSave();
}

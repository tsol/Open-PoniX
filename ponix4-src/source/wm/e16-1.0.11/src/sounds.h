/*
 * Copyright (C) 2008-2010 Kim Woelders
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
#ifndef _SOUNDS_H_
#define _SOUNDS_H_

#include "config.h"

typedef enum {
   SOUND_NONE = 0,
   SOUND_ALERT,
   SOUND_BUTTON_CLICK,
   SOUND_BUTTON_RAISE,
   SOUND_DEICONIFY,
   SOUND_DESKTOP_LOWER,
   SOUND_DESKTOP_RAISE,
   SOUND_DESKTOP_SHUT,
   SOUND_ERROR_IPC,
   SOUND_EXIT,
   SOUND_FOCUS_SET,
   SOUND_ICONIFY,
   SOUND_INSERT_KEYS,
   SOUND_LOGOUT,
   SOUND_LOWER,
   SOUND_MENU_SHOW,
   SOUND_MOVE_AREA_DOWN,
   SOUND_MOVE_AREA_LEFT,
   SOUND_MOVE_AREA_RIGHT,
   SOUND_MOVE_AREA_UP,
   SOUND_MOVE_RESIST,
   SOUND_MOVE_START,
   SOUND_MOVE_STOP,
   SOUND_RAISE,
   SOUND_RESIZE_START,
   SOUND_RESIZE_STOP,
   SOUND_SCANNING,
   SOUND_SETTINGS_ACTIVE,
   SOUND_SETTINGS_ALL,
   SOUND_SETTINGS_AREA,
   SOUND_SETTINGS_AUDIO,
   SOUND_SETTINGS_AUTORAISE_unused,
   SOUND_SETTINGS_BG,
   SOUND_SETTINGS_COMPOSITE,
   SOUND_SETTINGS_DESKTOPS,
   SOUND_SETTINGS_FOCUS,
   SOUND_SETTINGS_FX,
   SOUND_SETTINGS_GROUP,
   SOUND_SETTINGS_ICONBOX,
   SOUND_SETTINGS_MENUS,
   SOUND_SETTINGS_MISCELLANEOUS,
   SOUND_SETTINGS_MOVERESIZE,
   SOUND_SETTINGS_PAGER,
   SOUND_SETTINGS_PLACEMENT,
   SOUND_SETTINGS_SESSION,
   SOUND_SETTINGS_TOOLTIPS,
   SOUND_SETTINGS_TRANS,
   SOUND_SHADE,
   SOUND_SLIDEOUT_SHOW,
   SOUND_STARTUP,
   SOUND_UNSHADE,
   SOUND_WAIT,
   SOUND_WINDOW_BORDER_CHANGE,
   SOUND_WINDOW_CHANGE_LAYER_DOWN,
   SOUND_WINDOW_CHANGE_LAYER_UP,
   SOUND_WINDOW_CLOSE,
   SOUND_WINDOW_SLIDE,
   SOUND_WINDOW_SLIDE_END,
   SOUND_WINDOW_STICK,
   SOUND_WINDOW_UNSTICK,
   SOUND_NOT_USED
} esound_e;

#if HAVE_SOUND

void                SoundPlay(int sound);

#else

#define SoundPlay(sound) do{}while(0)

#endif

#endif /* _SOUNDS_H_ */

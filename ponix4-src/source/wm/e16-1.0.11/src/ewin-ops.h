/*
 * Copyright (C) 2005-2011 Kim Woelders
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
#ifndef _EWIN_OPS_H_
#define _EWIN_OPS_H_

typedef enum {
   EWIN_OP_INVALID,

   EWIN_OP_BORDER,
   EWIN_OP_TITLE,

   EWIN_OP_CLOSE,
   EWIN_OP_KILL,
   EWIN_OP_ICONIFY,
   EWIN_OP_ALONE,
   EWIN_OP_SHADE,
   EWIN_OP_STICK,
   EWIN_OP_FOCUS,

   EWIN_OP_DESK,
   EWIN_OP_AREA,
   EWIN_OP_MOVE,
   EWIN_OP_SIZE,
   EWIN_OP_MOVE_REL,
   EWIN_OP_SIZE_REL,

   EWIN_OP_MAX_WIDTH,
   EWIN_OP_MAX_HEIGHT,
   EWIN_OP_MAX_SIZE,
   EWIN_OP_FULLSCREEN,
   EWIN_OP_ZOOM,

   EWIN_OP_LAYER,
   EWIN_OP_RAISE,
   EWIN_OP_LOWER,

   EWIN_OP_OPACITY,
   EWIN_OP_FOCUSED_OPACITY,

   EWIN_OP_SNAP,

   EWIN_OP_FOCUS_CLICK,
   EWIN_OP_IGNORE_ARRANGE,
   EWIN_OP_NEVER_USE_AREA,
   EWIN_OP_NO_BUTTON_GRABS,
   EWIN_OP_SKIP_LISTS,
   EWIN_OP_AUTOSHADE,

   EWIN_OP_INH_APP_FOCUS,
   EWIN_OP_INH_APP_MOVE,
   EWIN_OP_INH_APP_SIZE,
   EWIN_OP_INH_USER_CLOSE,
   EWIN_OP_INH_USER_MOVE,
   EWIN_OP_INH_USER_SIZE,
   EWIN_OP_INH_WM_FOCUS,

   EWIN_OP_FADE,
   EWIN_OP_SHADOW,
   EWIN_OP_NO_REDIRECT,
   EWIN_OP_NO_ARGB,
} winop_e;

typedef struct {
   const char         *name;
   char                len;
   char                ok_ipc;
   char                ok_match;
   char                op;
} WinOp;

const WinOp        *EwinOpFind(const char *op);

#endif /* _EWIN_OPS_H_ */

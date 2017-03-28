/*
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
#ifndef _ECOMPMGR_H
#define _ECOMPMGR_H

#if USE_COMPOSITE

#include "eobj.h"
#include "etypes.h"

typedef struct {
   char                enable;
   int                 shadow;
   char                fading;
   int                 fade_speed;
   int                 opacity_focused;
   int                 opacity_unfocused;
   int                 opacity_override;
} cfg_composite;

int                 ECompMgrIsActive(void);

void                ECompMgrDeskConfigure(Desk * dsk);

Pixmap              ECompMgrGetRootBuffer(void);

void                ECompMgrWinNew(EObj * eo);
void                ECompMgrWinDel(EObj * eo);
void                ECompMgrWinMap(EObj * eo);
void                ECompMgrWinUnmap(EObj * eo);
void                ECompMgrWinMoveResize(EObj * eo, int change_xy,
					  int change_wh, int change_bw);
void                ECompMgrWinDamageArea(EObj * eo, int x, int y, int w,
					  int h);
void                ECompMgrWinReparent(EObj * eo, Desk * dsk, int change_xy);
void                ECompMgrWinRaiseLower(EObj * eo, int delta);
void                ECompMgrWinChangeShape(EObj * eo);
void                ECompMgrWinSetOpacity(EObj * eo, unsigned int opacity);
void                ECompMgrWinChangeOpacity(EObj * eo, unsigned int opacity);
void                ECompMgrWinChangeShadow(EObj * eo, int shadow);
Pixmap              ECompMgrWinGetPixmap(const EObj * eo);
Picture             ECompMgrWinGetAlphaPict(const EObj * eo);
void                ECompMgrWinClipToGC(EObj * eo, GC gc);

void                ECompMgrConfigGet(cfg_composite * cfg);
void                ECompMgrConfigSet(const cfg_composite * cfg);

void                ECompMgrRepaint(void);

#else

#define ECompMgrIsActive()          0

#define ECompMgrGetRootBuffer()     WinGetXwin(VROOT)

#endif

#endif /* _ECOMPMGR_H */

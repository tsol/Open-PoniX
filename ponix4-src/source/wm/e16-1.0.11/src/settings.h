/*
 * Copyright (C) 2006-2009 Kim Woelders
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
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#if ENABLE_DIALOGS

/* settings.c */
void                IPC_Cfg(const char *params);

#ifdef _DIALOG_H_
extern const DialogDef DlgAreas;
extern const DialogDef DlgBackground;
extern const DialogDef DlgComposite;
extern const DialogDef DlgDesks;
extern const DialogDef DlgFocus;
extern const DialogDef DlgFx;
extern const DialogDef DlgGroupDefaults;
extern const DialogDef DlgMenus;
extern const DialogDef DlgMisc;
extern const DialogDef DlgMoveResize;
extern const DialogDef DlgPagers;
extern const DialogDef DlgPlacement;
extern const DialogDef DlgRemember;
extern const DialogDef DlgSession;
extern const DialogDef DlgSound;
extern const DialogDef DlgTheme;
extern const DialogDef DlgThemeTrans;
extern const DialogDef DlgTooltips;
#endif

#endif /* ENABLE_DIALOGS */

#endif /* _SETTINGS_H_ */

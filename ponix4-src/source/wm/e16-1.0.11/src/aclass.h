/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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
#ifndef _ACLASS_H
#define _ACLASS_H

#include <X11/Xlib.h>
#include "etypes.h"

/* aclass.c */
int                 AclassConfigLoad(FILE * fs);
ActionClass        *ActionclassCreate(const char *name, int global);
Action             *ActionCreate(char event, char anymod, int mod, int anybut,
				 int but, char anykey, const char *key,
				 const char *tooltipstring);
void                ActionAddTo(Action * aa, const char *params);
void                ActionclassAddAction(ActionClass * ac, Action * aa);
void                ActionclassSetTooltipString(ActionClass * ac,
						const char *tts);
ActionClass        *ActionclassFind(const char *name);
ActionClass        *ActionclassAlloc(const char *name);
void                ActionclassFree(ActionClass * ac);
const char         *ActionclassGetName(ActionClass * ac);
const char         *ActionclassGetTooltipString(ActionClass * ac);
int                 ActionclassGetActionCount(ActionClass * ac);
Action             *ActionclassGetAction(ActionClass * ac, int ix);
int                 ActionclassEvent(ActionClass * ac, XEvent * ev,
				     EWin * ewin);
int                 ActionclassesGlobalEvent(XEvent * ev);
void                ActionclassesReload(void);

const char         *ActionGetTooltipString(Action * aa);
int                 ActionGetAnybutton(Action * aa);
int                 ActionGetEvent(Action * aa);
int                 ActionGetButton(Action * aa);
int                 ActionGetModifiers(Action * aa);

void                GrabButtonGrabs(Win win);
void                UnGrabButtonGrabs(Win win);

#endif /* _ACLASS_H */

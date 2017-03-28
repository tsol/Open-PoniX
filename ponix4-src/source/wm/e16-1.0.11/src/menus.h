/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2007 Kim Woelders
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
#ifndef _MENUS_H_
#define _MENUS_H_

#include "etypes.h"

typedef struct _menu Menu;
typedef struct _menuitem MenuItem;
typedef struct _menustyle MenuStyle;

typedef int         (MenuLoader) (Menu * m);

/* menus.c */
int                 MenuStyleConfigLoad(FILE * fs);
MenuStyle          *MenuStyleFind(const char *name);

int                 MenuConfigLoad(FILE * fs);
Menu               *MenuCreate(const char *name, const char *title,
			       Menu * parent, MenuStyle * ms);
void                MenuEmpty(Menu * m, int destroying);
int                 MenuLoad(Menu * m);
Menu               *MenuFind(const char *name, const char *param);
MenuItem           *MenuItemCreate(const char *text, ImageClass * ic,
				   const char *action_params, Menu * child);
void                MenuSetInternal(Menu * m);
void                MenuSetDynamic(Menu * m);
void                MenuSetTransient(Menu * m);
void                MenuSetName(Menu * m, const char *name);
void                MenuSetAlias(Menu * m, const char *alias);
void                MenuSetStyle(Menu * m, MenuStyle * ms);
void                MenuSetTitle(Menu * m, const char *title);
void                MenuSetIconSize(Menu * m, int size);
void                MenuSetData(Menu * m, char *data);
void                MenuSetLoader(Menu * m, MenuLoader * loader);
void                MenuSetTimestamp(Menu * m, time_t t);
void                MenuAddItem(Menu * m, MenuItem * mi);
const char         *MenuGetName(const Menu * m);
const char         *MenuGetData(const Menu * m);
time_t              MenuGetTimestamp(const Menu * m);

int                 MenusActive(void);

/* menus-misc.c */
Menu               *MenusCreateInternal(const char *type, const char *name,
					const char *style, const char *prm);

#endif /* _MENUS_H_ */

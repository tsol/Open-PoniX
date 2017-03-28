/*
 * Copyright (C) 2003-2010 Kim Woelders
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
#ifndef _EMODULE_H_
#define _EMODULE_H_

#include "econfig.h"
#include "ipc.h"

typedef struct {
   const char         *name;
   const char         *nick;
   void                (*Signal) (int sig, void *prm);
   IpcItemList         ipc;
   CfgItemList         cfg;
} EModule;

typedef enum {
   ESIGNAL_NONE,
   ESIGNAL_INIT,
   ESIGNAL_CONFIGURE,
   ESIGNAL_START,
   ESIGNAL_EXIT,
   ESIGNAL_AREA_CONFIGURED,
   ESIGNAL_AREA_SWITCH_START,
   ESIGNAL_AREA_SWITCH_DONE,
   ESIGNAL_DESK_ADDED,
   ESIGNAL_DESK_REMOVED,
   ESIGNAL_DESK_SWITCH_START,
   ESIGNAL_DESK_SWITCH_DONE,
   ESIGNAL_DESK_RESIZE,
   ESIGNAL_BACKGROUND_CHANGE,
   ESIGNAL_ANIMATION_SUSPEND,
   ESIGNAL_ANIMATION_RESUME,
   ESIGNAL_EWIN_CREATE,
   ESIGNAL_EWIN_DESTROY,
   ESIGNAL_EWIN_UNMAP,
   ESIGNAL_EWIN_ICONIFY,
   ESIGNAL_EWIN_DEICONIFY,
   ESIGNAL_EWIN_CHANGE_ICON,
   ESIGNAL_EWIN_CHANGE,
   ESIGNAL_EWIN_DAMAGE,
   ESIGNAL_THEME_TRANS_CHANGE,
} esignal_e;

#if 0				/* Maybe later */
void                EModuleRegister(EModule * em);
#endif

extern const EModule *const p_modules[];
extern int          n_modules;

#define MODULE_LIST_GET(lst, num) \
    do { lst = p_modules; num = n_modules; } while (0)
#define MODULE_LIST_FREE(lst)

int                 ModuleConfigSet(const char *name, const char *item,
				    const char *params);
int                 ModuleConfigShow(const char *name, const char *item);

void                ModulesSignal(int signal, void *prm);
void                ModulesConfigShow(void);

#if 0
void                ModulesGetCfgItems(const CfgItem *** pi, int *ni);
#endif
void                ModulesGetIpcItems(const IpcItem *** pi, int *ni);

#endif /* _EMODULE_H_ */

/*
 * Copyright (C) 2003-2009 Kim Woelders
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
#include "emodule.h"

#if 0				/* No dynamic registration yet */
void
EModuleRegister(EModule * em)
{
   p_modules = EREALLOC(EModule *, p_modules, n_modules + 1);
   p_modules[n_modules++] = em;
}
#endif

static const EModule *
EModuleFind(const char *name)
{
   int                 i;
   const EModule      *pm;

   for (i = 0; i < n_modules; i++)
     {
	pm = p_modules[i];
	if (!strncmp(name, pm->name, 4) ||
	    (pm->nick && !strcmp(name, pm->nick)))
	   return pm;
     }
   return NULL;
}

int
ModuleConfigSet(const char *name, const char *item, const char *params)
{
   const EModule      *em;

   em = EModuleFind(name);
   if (!em)
      return -1;

   if (em->cfg.num <= 0)
      return -1;

   CfgItemListNamedItemSet(em->cfg.lst, em->cfg.num, item, params);

   return 0;
}

static void
EmodCfgItemShow(const EModule * em, const CfgItem * ci)
{
   char                buf[1024];

   if (!ci)
      return;

   CfgItemToString(ci, buf, sizeof(buf));
   IpcPrintf("  %s.%s = %s\n", em->name, ci->name, buf);
}

static void
EmodCfgNamedItemShow(const EModule * em, const char *item)
{
   const CfgItem      *ci;

   ci = CfgItemFind(em->cfg.lst, em->cfg.num, item);
   if (ci)
      EmodCfgItemShow(em, ci);
   else
      IpcPrintf("! %s.%s *** Not found ***\n", em->name, item);
}

int
ModuleConfigShow(const char *name, const char *item)
{
   const EModule      *em;
   int                 i;

#if 0
   Eprintf("ModuleConfigShow: %s:%s\n", name, item);
#endif
   em = EModuleFind(name);
   if (!em)
      return -1;

   if (item)
     {
	EmodCfgNamedItemShow(em, item);
     }
   else
     {
	for (i = 0; i < em->cfg.num; i++)
	   EmodCfgItemShow(em, em->cfg.lst + i);
     }

   return 0;
}

void
ModulesSignal(int sig, void *prm)
{
   int                 i;

   for (i = 0; i < n_modules; i++)
      if (p_modules[i]->Signal)
	 p_modules[i]->Signal(sig, prm);
}

#if 0
void
ModulesGetCfgItems(const CfgItem *** ppi, int *pni)
{
   int                 i, j, k, n;
   const CfgItem     **pi;

   pi = *ppi;
   k = *pni;
   for (i = 0; i < n_modules; i++)
     {
	if (p_modules[i]->cfg.lst)
	  {
	     n = p_modules[i]->cfg.num;
	     pi = EREALLOC(CfgItem *, pi, k + n);
	     for (j = 0; j < n; j++)
		pi[k++] = &(p_modules[i]->cfg.lst[j]);
	  }
     }
   *ppi = pi;
   *pni = k;
}
#endif

void
ModulesGetIpcItems(const IpcItem *** ppi, int *pni)
{
   int                 i, j, k, n;
   const IpcItem     **pi;

   pi = *ppi;
   k = *pni;
   for (i = 0; i < n_modules; i++)
     {
	if (p_modules[i]->ipc.lst)
	  {
	     n = p_modules[i]->ipc.num;
	     pi = EREALLOC(const IpcItem *, pi, k + n);

	     for (j = 0; j < n; j++)
		pi[k++] = &(p_modules[i]->ipc.lst[j]);
	  }
     }
   *ppi = pi;
   *pni = k;
}

void
ModulesConfigShow(void)
{
   int                 i, nml;
   const EModule      *const *pml;

   /* Load module configs */
   MODULE_LIST_GET(pml, nml);
   for (i = 0; i < nml; i++)
     {
	/* Somewhat inefficient but ... later */
	ModuleConfigShow(pml[i]->name, NULL);
     }
   MODULE_LIST_FREE(pml);
}

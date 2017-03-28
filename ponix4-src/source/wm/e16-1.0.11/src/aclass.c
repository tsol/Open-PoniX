/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "aclass.h"
#include "conf.h"
#include "cursors.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "ewins.h"
#include "file.h"
#include "grabs.h"
#include "timers.h"
#include <ctype.h>

typedef struct _actiontype {
   char               *params;
   struct _actiontype *next;
} ActionType;

struct _action {
   char                event;
   char                anymodifier;
   int                 modifiers;
   char                anybutton;
   int                 button;
   char                anykey;
   KeyCode             key;
   char               *key_str;
   char               *tooltipstring;
   ActionType         *action;
};

struct _actionclass {
   char               *name;
   int                 num;
   Action            **list;
   char               *tooltipstring;
   unsigned int        ref_count;
   char global;
};

static void         UnGrabActionKey(Action * aa);
static void         GrabActionKey(Action * aa);

static void         BindingsSave(void);

static Ecore_List  *aclass_list = NULL;
static Ecore_List  *aclass_list_global = NULL;

static char         mode_action_destroy = 0;
static char         mode_keybinds_changed = 0;

static void
RemoveActionType(ActionType * ActionTypeToRemove)
{
   ActionType         *ptr, *pp;

   ptr = ActionTypeToRemove;
   while (ptr)
     {
	Efree(ptr->params);
	pp = ptr;
	ptr = ptr->next;
	Efree(pp);
     }
}

Action             *
ActionCreate(char event, char anymod, int mod, int anybut, int but,
	     char anykey, const char *key, const char *tooltipstring)
{
   Action             *aa;

   aa = EMALLOC(Action, 1);
   if (!aa)
      return NULL;
   aa->action = NULL;
   aa->event = event;
   aa->anymodifier = anymod;
   aa->modifiers = mod;
   aa->anybutton = anybut;
   aa->button = but;
   aa->anykey = anykey;
   if (!key || !key[0] || (event != EVENT_KEY_DOWN && event != EVENT_KEY_UP))
      aa->key = 0;
   else
      aa->key = EKeynameToKeycode(key);
   aa->key_str = (aa->key) ? Estrdup(key) : NULL;
   aa->tooltipstring =
      (tooltipstring) ? Estrdup((tooltipstring[0]) ? tooltipstring : "?!?") :
      NULL;

   return aa;
}

static void
ActionDestroy(Action * aa)
{
   if (!aa)
      return;

   if ((aa->event == EVENT_KEY_DOWN) || (aa->event == EVENT_KEY_UP))
      UnGrabActionKey(aa);
   if (aa->action)
      RemoveActionType(aa->action);
   Efree(aa->tooltipstring);
   Efree(aa->key_str);
   Efree(aa);
}

void
ActionAddTo(Action * aa, const char *params)
{
   ActionType         *pptr, *ptr, *at;

   at = EMALLOC(ActionType, 1);
   if (!at)
      return;

   at->next = NULL;
   at->params = (params && *params) ? Estrdup(params) : NULL;
   if (!aa->action)
     {
	aa->action = at;
     }
   else
     {
	pptr = NULL;
	ptr = aa->action;
	while (ptr)
	  {
	     pptr = ptr;
	     ptr = ptr->next;
	  }
	if (pptr)
	   pptr->next = at;
     }
}

void
ActionclassAddAction(ActionClass * ac, Action * aa)
{
   ac->num++;
   ac->list = EREALLOC(Action *, ac->list, ac->num);
   ac->list[ac->num - 1] = aa;
}

ActionClass        *
ActionclassCreate(const char *name, int global)
{
   ActionClass        *ac;

   ac = ECALLOC(ActionClass, 1);
   if (!ac)
      return NULL;
   ac->name = Estrdup(name);

   if (global)
     {
	if (!aclass_list_global)
	   aclass_list_global = ecore_list_new();
	ecore_list_prepend(aclass_list_global, ac);
	ac->global = 1;
     }
   else
     {
	if (!aclass_list)
	   aclass_list = ecore_list_new();
	ecore_list_prepend(aclass_list, ac);
     }

   return ac;
}

static void
ActionclassEmpty(ActionClass * ac)
{
   int                 i;

   for (i = 0; i < ac->num; i++)
      ActionDestroy(ac->list[i]);
   ac->num = 0;
   _EFREE(ac->list);
   _EFREE(ac->tooltipstring);
}

static void
ActionclassDestroy(ActionClass * ac)
{
   if (!ac)
      return;

   if (ac->ref_count > 0)
     {
	DialogOK("ActionClass Error!", _("%u references remain"),
		 ac->ref_count);
	return;
     }

   ecore_list_node_remove(aclass_list, ac);

   ActionclassEmpty(ac);
   Efree(ac->name);

   Efree(ac);
   mode_action_destroy = 1;
}

static int
_ActionclassMatchName(const void *data, const void *match)
{
   return strcmp(((const ActionClass *)data)->name, (const char *)match);
}

static ActionClass *
ActionclassFindGlobal(const char *name)
{
   return (ActionClass *) ecore_list_find(aclass_list_global,
					  _ActionclassMatchName, name);
}

ActionClass        *
ActionclassFind(const char *name)
{
   if (!name)
      return NULL;
   return (ActionClass *) ecore_list_find(aclass_list, _ActionclassMatchName,
					  name);
}

static ActionClass *
ActionclassFindAny(const char *name)
{
   ActionClass        *ac;

   ac = (ActionClass *) ecore_list_find(aclass_list_global,
					_ActionclassMatchName, name);
   if (ac)
      return ac;
   return (ActionClass *) ecore_list_find(aclass_list, _ActionclassMatchName,
					  name);
}

int
AclassConfigLoad(FILE * fs)
{
   int                 err = 0;
   ActionClass        *ac = NULL;
   Action             *aa = NULL;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char               *p2;
   int                 i1, i2;
   char                event = 0;
   char                anymod = 0;
   int                 mod = 0;
   int                 anybut = 0;
   int                 but = 0;
   int                 first = 1;
   char                anykey = 0;
   char                key[64];
   char               *aclass_tooltipstring = NULL;
   char               *action_tooltipstring = NULL;
   char global = 0;

   key[0] = '\0';

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, &p2, NULL);
	i2 = atoi(s2);
	switch (i1)
	  {
	  case CONFIG_VERSION:
	     break;
	  case CONFIG_ACTIONCLASS:
	     err = -1;
	     if (i2 != CONFIG_OPEN)
		goto done;
	     ac = NULL;
	     aa = NULL;
	     event = 0;
	     anymod = anybut = anykey = 0;
	     mod = 0;
	     but = 0;
	     first = 1;
	     key[0] = '\0';
	     break;
	  case CONFIG_CLOSE:
	     if (ac)
		ac->tooltipstring =
		   (aclass_tooltipstring) ? Estrdup((aclass_tooltipstring[0]) ?
						    aclass_tooltipstring :
						    "?!?") : NULL;
	     err = 0;
	     goto done;

	  case CONFIG_CLASSNAME:
	  case ACLASS_NAME:
	     ac = ActionclassFindAny(s2);
	     if (ac)
	       {
		  if (!strcmp(s2, "KEYBINDINGS"))
		     mode_keybinds_changed = 1;
		  ActionclassEmpty(ac);
	       }
	     else
	       {
		  ac = ActionclassCreate(s2, 0);
	       }
	     break;
	  case CONFIG_TYPE:
	  case ACLASS_TYPE:
	     if (i2 == ACLASS_TYPE_ACLASS)
		break;
	     ecore_list_node_remove(aclass_list, ActionclassFind(s2));
	     ecore_list_prepend(aclass_list_global, ac);
	     global = 1;

	     break;
	  case CONFIG_MODIFIER:
	  case ACLASS_MODIFIER:
	     /* These are the defines that I have listed...
	      * These, therefore, are the ones that I am 
	      * going to accept by default.
	      * REMINDER: add and'ing in future!!!!
	      * #define ShiftMask       (1<<0)
	      * #define LockMask        (1<<1)
	      * #define ControlMask     (1<<2)
	      * #define Mod1Mask        (1<<3)
	      * #define Mod2Mask        (1<<4)
	      * #define Mod3Mask        (1<<5)
	      * #define Mod4Mask        (1<<6)
	      * #define Mod5Mask        (1<<7)
	      */
	     switch (i2)
	       {
	       case MASK_NONE:
		  mod = 0;
		  break;
	       case MASK_SHIFT:
		  mod |= ShiftMask;
		  break;
	       case MASK_LOCK:
		  mod |= LockMask;
		  break;
	       case MASK_CTRL:
		  mod |= ControlMask;
		  break;
	       case MASK_MOD1:
		  mod |= Mod1Mask;
		  break;
	       case MASK_MOD2:
		  mod |= Mod2Mask;
		  break;
	       case MASK_MOD3:
		  mod |= Mod3Mask;
		  break;
	       case MASK_MOD4:
		  mod |= Mod4Mask;
		  break;
	       case MASK_MOD5:
		  mod |= Mod5Mask;
		  break;
	       case MASK_CTRL_ALT:
		  mod |= ControlMask | Mod1Mask;
		  break;
	       case MASK_SHIFT_ALT:
		  mod |= ShiftMask | Mod1Mask;
		  break;
	       case MASK_CTRL_SHIFT:
		  mod |= ShiftMask | ControlMask;
		  break;
	       case MASK_CTRL_SHIFT_ALT:
		  mod |= ShiftMask | ControlMask | Mod1Mask;
		  break;
	       case MASK_SHIFT_META4:
		  mod |= Mod4Mask | ShiftMask;
		  break;
	       case MASK_CTRL_META4:
		  mod |= Mod4Mask | ControlMask;
		  break;
	       case MASK_CTRL_META4_SHIFT:
		  mod |= Mod4Mask | ControlMask | ShiftMask;
		  break;
	       case MASK_SHIFT_META5:
		  mod |= Mod5Mask | ShiftMask;
		  break;
	       case MASK_CTRL_META5:
		  mod |= Mod5Mask | ControlMask;
		  break;
	       case MASK_CTRL_META5_SHIFT:
		  mod |= Mod5Mask | ControlMask | ShiftMask;
		  break;
	       case MASK_WINDOWS_SHIFT:
		  mod |= Mod2Mask | ShiftMask;
		  break;
	       case MASK_WINDOWS_CTRL:
		  mod |= Mod2Mask | ControlMask;
		  break;
	       case MASK_WINDOWS_ALT:
		  mod |= Mod2Mask | Mod1Mask;
		  break;
	       default:
		  break;
	       }
	     break;
	  case CONFIG_ANYMOD:
	  case ACLASS_ANYMOD:
	     anymod = i2;
	     break;
	  case CONFIG_ANYBUT:
	  case ACLASS_ANYBUT:
	     anybut = i2;
	     break;
	  case CONFIG_BUTTON:
	  case ACLASS_BUT:
	     but = i2;
	     break;
	  case CONFIG_ANYKEY:
	  case ACLASS_ANYKEY:
	     anykey = i2;
	     break;
	  case ACLASS_KEY:
	     STRCPY(key, s2);
	     break;
	  case ACLASS_EVENT_TRIGGER:
	     event = i2;
	     break;
	  case CONFIG_NEXT:
	     mod = 0;
	     anymod = 0;
	     anybut = 0;
	     first = 1;
	     break;
	  case CONFIG_ACTION:
	     if (first)
	       {
		  aa = ActionCreate(event, anymod, mod, anybut, but, anykey,
				    key, action_tooltipstring);
		  /* the correct place to grab an action key */
		  Efree(action_tooltipstring);
		  action_tooltipstring = NULL;
		  key[0] = '\0';
		  if (global)
		     GrabActionKey(aa);
		  ActionclassAddAction(ac, aa);
		  first = 0;
	       }
	     ActionAddTo(aa, p2);
	     break;
	  case CONFIG_ACTION_TOOLTIP:
	     action_tooltipstring = Estrdupcat2(action_tooltipstring, "\n", p2);
	     break;
	  case CONFIG_TOOLTIP:
	     aclass_tooltipstring = Estrdupcat2(aclass_tooltipstring, "\n", p2);
	     break;
	  default:
	     ConfigParseError("ActionClass", s);
	     break;
	  }
     }

   if (ac && err)
      ActionclassDestroy(ac);

 done:
   Efree(aclass_tooltipstring);
   Efree(action_tooltipstring);

   return err;
}

static Action      *
ActionDecode(const char *line)
{
   Action             *aa;
   char                ev[16], mod[16], key[128], *s;
   int                 len, event, modifiers, button;
   char                anymod, anybut, anykey;

   len = -1;
   sscanf(line, "%15s %15s %127s %n", ev, mod, key, &len);
   if (len <= 0)
      return NULL;

   event = -1;
   if (!strcmp(ev, "KeyDown"))
      event = EVENT_KEY_DOWN;
   else if (!strcmp(ev, "MouseDown"))
      event = EVENT_MOUSE_DOWN;
   else if (!strcmp(ev, "KeyUp"))
      event = EVENT_KEY_UP;
   else if (!strcmp(ev, "MouseUp"))
      event = EVENT_MOUSE_UP;
   else if (!strcmp(ev, "MouseDouble"))
      event = EVENT_DOUBLE_DOWN;
   else if (!strcmp(ev, "MouseIn"))
      event = EVENT_MOUSE_ENTER;
   else if (!strcmp(ev, "MouseOut"))
      event = EVENT_MOUSE_LEAVE;
   else if (!strcmp(ev, "FocusIn"))
      event = EVENT_FOCUS_IN;
   else if (!strcmp(ev, "FocusOut"))
      event = EVENT_FOCUS_OUT;

   anymod = anybut = anykey = 0;
   button = 0;

   modifiers = 0;
   for (s = mod; *s; s++)
     {
	switch (*s)
	  {
	  case '*':
	     anymod = 1;
	     break;
	  case 'C':
	     modifiers |= ControlMask;
	     break;
	  case 'S':
	     modifiers |= ShiftMask;
	     break;
	  case 'A':
	     modifiers |= Mod1Mask;
	     break;
	  case '1':
	     modifiers |= Mod1Mask;
	     break;
	  case '2':
	     modifiers |= Mod2Mask;
	     break;
	  case '3':
	     modifiers |= Mod3Mask;
	     break;
	  case '4':
	     modifiers |= Mod4Mask;
	     break;
	  case '5':
	     modifiers |= Mod5Mask;
	     break;
	  }
     }

   switch (event)
     {
     case EVENT_MOUSE_DOWN:
     case EVENT_MOUSE_UP:
     case EVENT_DOUBLE_DOWN:
     case EVENT_MOUSE_ENTER:
     case EVENT_MOUSE_LEAVE:
	if (key[0] == '*')
	   anybut = 1;
	else if (isdigit(key[0]))
	   button = atoi(key);
	if (!anybut && button == 0)
	   return NULL;		/* Invalid */
	key[0] = '\0';
	break;
     }

   aa =
      ActionCreate(event, anymod, modifiers, anybut, button, anykey, key, NULL);
   ActionAddTo(aa, line + len);

   return aa;
}

static int
ActionEncode(Action * aa, char *buf, int len)
{
   const char         *event;
   char               *p, mod[32], btn[32];

   if (!aa || !aa->action)
      return 0;

   p = mod;
   if (aa->anymodifier)
      *p++ = '*';
   if (aa->modifiers & ControlMask)
      *p++ = 'C';
   if (aa->modifiers & ShiftMask)
      *p++ = 'S';
   if (aa->modifiers & Mod1Mask)
      *p++ = 'A';
   if (aa->modifiers & Mod2Mask)
      *p++ = '2';
   if (aa->modifiers & Mod3Mask)
      *p++ = '3';
   if (aa->modifiers & Mod4Mask)
      *p++ = '4';
   if (aa->modifiers & Mod5Mask)
      *p++ = '5';
   if (p == mod)
      *p++ = '-';
   *p = '\0';

   switch (aa->event)
     {
     default:
	return 0;
     case EVENT_KEY_DOWN:
	event = "KeyDown";
	goto encode_kb;
     case EVENT_KEY_UP:
	event = "KeyUp";
	goto encode_kb;
      encode_kb:
	if (!aa->key_str)
	   return 0;
	len = Esnprintf(buf, len, "%-7s %4s %8s %s\n", event, mod, aa->key_str,
			(aa->action->params) ? aa->action->params : "");
	break;

     case EVENT_MOUSE_DOWN:
	event = "MouseDown";
	goto encode_mb;
     case EVENT_MOUSE_UP:
	event = "MouseUp";
	goto encode_mb;
     case EVENT_DOUBLE_DOWN:
	event = "MouseDouble";
	goto encode_mb;
     case EVENT_MOUSE_ENTER:
	event = "MouseIn";
	goto encode_mb;
     case EVENT_MOUSE_LEAVE:
	event = "MouseOut";
	goto encode_mb;
      encode_mb:
	if (aa->anybutton)
	   strcpy(btn, "*");
	else
	   sprintf(btn, "%u", aa->button);
	len = Esnprintf(buf, len, "%-11s %4s %s %s\n", event, mod, btn,
			(aa->action->params) ? aa->action->params : "");
	break;

     case EVENT_FOCUS_IN:
	event = "FocusIn";
	goto encode_fc;
     case EVENT_FOCUS_OUT:
	event = "FocusOut";
	goto encode_fc;
      encode_fc:
	break;
     }

   return len;
}

static int
AclassEncodeTT(const char *str, char *buf, int len)
{
   char              **lst;
   int                 i, num, l, nw;

   lst = StrlistFromString(str, '\n', &num);
   nw = 0;
   for (i = 0; i < num; i++)
     {
	l = Esnprintf(buf, len, "Tooltip %s\n", lst[i]);
	nw += l;
	len -= l;
	buf += l;
     }
   StrlistFree(lst, num);
   return nw;
}

static void
AclassConfigLineParse(char *s, ActionClass ** pac, Action ** paa)
{
   char                prm1[128], prm2[128], prm3[128];
   ActionClass        *ac = *pac;
   Action             *aa = *paa;
   int                 len, len2;

   len = strcspn(s, "#\r\n");
   if (len <= 0)
      return;
   s[len] = '\0';

   prm3[0] = '\0';
   len2 = 0;
   len = sscanf(s, "%16s %n%128s %16s", prm1, &len2, prm2, prm3);
   if (len < 2)
      return;

   if (!strcmp(prm1, "Aclass"))
     {
	if (!strcmp(prm2, "KEYBINDINGS_UNCHANGABLE"))
	  {
	     /* No more "unchangable" keybindings. */
	     ac = ActionclassFindGlobal("KEYBINDINGS");
	     prm2[11] = '\0';
	  }
	else
	  {
	     ac = ActionclassFindAny(prm2);
	     if (ac)
		ActionclassEmpty(ac);
	  }

	if (!ac)
	   ac = ActionclassCreate(prm2, prm3[0] == 'g');

	mode_keybinds_changed = 1;
	aa = NULL;
     }
   else if (!strncmp(prm1, "Key", 3) || !strncmp(prm1, "Mouse", 5))
     {
	if (!ac)
	   return;

	aa = ActionDecode(s);
	if (!aa)
	   return;

	ActionclassAddAction(ac, aa);
	GrabActionKey(aa);
     }
   else if (!strcmp(prm1, "Tooltip"))
     {
	/* FIXME - Multiple line strings may break */
	if (aa)
	  {
	     aa->tooltipstring = Estrdupcat2(aa->tooltipstring, "\n", s + len2);
	  }
	else if (ac)
	  {
	     ac->tooltipstring = Estrdupcat2(ac->tooltipstring, "\n", s + len2);
	  }
     }

   *pac = ac;
   *paa = aa;
}

static int
AclassConfigLoad2(FILE * fs)
{
   char                s[FILEPATH_LEN_MAX], *ss;
   ActionClass        *ac = NULL;
   Action             *aa = NULL;

   for (;;)
     {
	ss = fgets(s, sizeof(s), fs);
	if (!ss)
	   break;

	AclassConfigLineParse(s, &ac, &aa);
     }

   return 0;
}

static void
AclassConfigParseIpc(const char *p)
{
   char               *s, *ss;
   int                 len;
   ActionClass        *ac = NULL;
   Action             *aa = NULL;

   ss = s = Estrdup(p);
   if (!s)
      return;

   for (;; s += len + 1)
     {
	len = strcspn(s, "#\r\n");
	if (len <= 0)
	   break;
	s[len] = '\0';

	AclassConfigLineParse(s, &ac, &aa);
     }

   Efree(ss);

   BindingsSave();
}

static void
AclassConfigLoadConfig(const char *name)
{
   ConfigFileLoad(name, NULL, AclassConfigLoad2, 0);
}

static void
AclassConfigWrite(const ActionClass * ac, void (*prf) (const char *fmt, ...))
{
   char                s[FILEPATH_LEN_MAX];
   Action             *aa;
   int                 i, len;

   if (!ac || ac->num <= 0)
      return;

   prf("Aclass %s %s\n", ac->name, (ac->global)? "global" : "normal");
   if (ac->tooltipstring)
     {
	AclassEncodeTT(ac->tooltipstring, s, sizeof(s));
	prf(s);
     }
   for (i = 0; i < ac->num; i++)
     {
	aa = ac->list[i];
	len = ActionEncode(aa, s, sizeof(s));
	if (len <= 0)
	   continue;
	prf(s);
	if (aa->tooltipstring)
	  {
	     AclassEncodeTT(aa->tooltipstring, s, sizeof(s));
	     prf(s);
	  }
     }
}

static FILE        *_ac_fs = NULL;	/* Ugly! Yeah well... */

static void
_ac_prf(const char *fmt, ...)
{
   va_list             args;

   va_start(args, fmt);
   vfprintf(_ac_fs, fmt, args);
   va_end(args);
}

static void
BindingsSave(void)
{
   char                s[FILEPATH_LEN_MAX], ss[FILEPATH_LEN_MAX];
   FILE               *fs;

   if (!mode_keybinds_changed)
      return;

   Etmp(ss);
   fs = fopen(ss, "w");
   if (!fs)
      return;
   _ac_fs = fs;

   AclassConfigWrite(ActionclassFind("BUTTONBINDINGS"), _ac_prf);
   AclassConfigWrite(ActionclassFind("DESKBINDINGS"), _ac_prf);
   AclassConfigWrite(ActionclassFindGlobal("KEYBINDINGS"), _ac_prf);
   AclassConfigWrite(ActionclassFindGlobal("KEYBINDINGS_UNCHANGABLE"), _ac_prf);

   fclose(fs);
   _ac_fs = NULL;

   Esnprintf(s, sizeof(s), "%s/bindings.cfg", EDirUser());
   E_mv(ss, s);
}

void
ActionclassSetTooltipString(ActionClass * ac, const char *tts)
{
   _EFDUP(ac->tooltipstring, tts);
}

ActionClass        *
ActionclassAlloc(const char *name)
{
   ActionClass        *ac;

   if (!name || !name[0])
      return NULL;

   ac = ActionclassFind(name);
   if (ac)
      ac->ref_count++;

   return ac;
}

void
ActionclassFree(ActionClass * ac)
{
   if (ac)
      ac->ref_count--;
}

const char         *
ActionclassGetName(ActionClass * ac)
{
   return (ac) ? ac->name : NULL;
}

const char         *
ActionclassGetTooltipString(ActionClass * ac)
{
   return (ac) ? ac->tooltipstring : NULL;
}

int
ActionclassGetActionCount(ActionClass * ac)
{
   return (ac) ? ac->num : 0;
}

Action             *
ActionclassGetAction(ActionClass * ac, int ix)
{
   return (ac && ix < ac->num) ? ac->list[ix] : NULL;
}

const char         *
ActionGetTooltipString(Action * aa)
{
   return (aa) ? aa->tooltipstring : NULL;
}

int
ActionGetEvent(Action * aa)
{
   return (aa) ? aa->event : 0;
}

int
ActionGetAnybutton(Action * aa)
{
   return (aa) ? aa->anybutton : 0;
}

int
ActionGetButton(Action * aa)
{
   return (aa) ? aa->button : 0;
}

int
ActionGetModifiers(Action * aa)
{
   return (aa) ? aa->modifiers : 0;
}

static void
handleAction(EWin * ewin, ActionType * action)
{
   if (ewin)
      ewin->state.in_action = 1;
   EFunc(ewin, action->params);
   if (ewin)
     {
	if (!EwinFindByPtr(ewin))
	   return;		/* ewin has been destroyed */
	ewin->state.in_action = 0;
     }

   /* Did we just hose ourselves? if so, we'd best not stick around here */
   if (mode_action_destroy)
      return;

   /* If there is another action in this series, (now that
    * we're sure we didn't already die) perform it
    */
   if (action->next)
      handleAction(ewin, action->next);
}

int
ActionclassEvent(ActionClass * ac, XEvent * ev, EWin * ewin)
{
   KeyCode             key;
   int                 i, type, button, modifiers, ok, mouse, mask, val = 0;
   Action             *aa;

   if (ewin && ewin->state.inhibit_actions)
      return 0;

   key = type = button = modifiers = mouse = 0;

   mask = Mode.masks.mod_key_mask;

   switch (ev->type)
     {
     case KeyPress:
	type = EVENT_KEY_DOWN;
	key = ev->xkey.keycode;
	modifiers = ev->xbutton.state & mask;
	mouse = 0;
	break;
     case KeyRelease:
	type = EVENT_KEY_UP;
	key = ev->xkey.keycode;
	modifiers = ev->xbutton.state & mask;
	mouse = 0;
	break;
     case ButtonPress:
	if (Mode.events.double_click)
	   type = EVENT_DOUBLE_DOWN;
	else
	   type = EVENT_MOUSE_DOWN;
	button = ev->xbutton.button;
	modifiers = ev->xbutton.state & mask;
	mouse = 1;
	break;
     case ButtonRelease:
	type = EVENT_MOUSE_UP;
	button = ev->xbutton.button;
	modifiers = ev->xbutton.state & mask;
	mouse = 1;
	break;
     case EnterNotify:
	type = EVENT_MOUSE_ENTER;
	button = -1;
	modifiers = ev->xcrossing.state & mask;
	mouse = 1;
	break;
     case LeaveNotify:
	/* If frame window, quit if pointer is still inside */
	if (ewin && ev->xcrossing.window == EoGetXwin(ewin) &&
	    (ev->xcrossing.x >= 0 && ev->xcrossing.x < EoGetW(ewin) &&
	     ev->xcrossing.y >= 0 && ev->xcrossing.y < EoGetH(ewin)))
	   return 0;
	type = EVENT_MOUSE_LEAVE;
	button = -1;
	modifiers = ev->xcrossing.state & mask;
	mouse = 1;
	break;
     case FocusIn:
	type = EVENT_FOCUS_IN;
	button = -1;
	mouse = 1;
	break;
     case FocusOut:
	type = EVENT_FOCUS_OUT;
	button = -1;
	mouse = 1;
	break;
     default:
	break;
     }

   mode_action_destroy = 0;

   for (i = 0; i < ac->num; i++)
     {
	if (!mode_action_destroy)
	  {
	     aa = ac->list[i];
	     ok = 0;
	     if ((aa->event == type) && (aa->action))
	       {
		  if (mouse)
		    {
		       if (button < 0)
			 {
			    if (aa->anymodifier)
			       ok = 1;
			    else if (aa->modifiers == modifiers)
			       ok = 1;
			 }
		       else
			 {
			    if (aa->anymodifier)
			      {
				 if (aa->anybutton)
				    ok = 1;
				 else if (aa->button == button)
				    ok = 1;
			      }
			    else if (aa->modifiers == modifiers)
			      {
				 if (aa->anybutton)
				    ok = 1;
				 else if (aa->button == button)
				    ok = 1;
			      }
			 }
		    }
		  else
		    {
		       if (aa->anymodifier)
			 {
			    if (aa->anykey)
			       ok = 1;
			    else if (aa->key == key)
			       ok = 1;
			 }
		       else if (aa->modifiers == modifiers)
			 {
			    if (aa->anykey)
			       ok = 1;
			    else if (aa->key == key)
			       ok = 1;
			 }
		    }
		  if (ok)
		    {
		       handleAction(ewin, aa->action);
		       val = 1;
		    }
	       }
	  }
	if (mode_action_destroy)
	   break;
     }

   mode_action_destroy = 0;

   return val;
}

int
ActionclassesGlobalEvent(XEvent * ev)
{
   ActionClass        *ac;
   int                 match;

   match = 0;
   ECORE_LIST_FOR_EACH(aclass_list_global, ac)
      match |= ActionclassEvent(ac, ev, GetFocusEwin());

   return match;
}

static Timer       *ac_reload_timer = NULL;

static int
_ac_reload(void *data __UNUSED__)
{
   AclassConfigLoadConfig("bindings.cfg");
   ac_reload_timer = NULL;
   return 0;
}

void
ActionclassesReload(void)
{
   TIMER_DEL(ac_reload_timer);
   TIMER_ADD(ac_reload_timer, 200, _ac_reload, NULL);
}

/*
 * Actions module
 */

static void
AclassSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	AclassConfigLoadConfig("bindings.cfg");
	break;
     }
}

static void
AclassIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;
   ActionClass        *ac;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '\0' || cmd[0] == '?')
     {
     }
   else if (!strncmp(cmd, "kb", 2))
     {
	if (!strcmp(prm, "set"))
	   AclassConfigParseIpc(p);
	else
	   AclassConfigWrite(ActionclassFindGlobal("KEYBINDINGS"), IpcPrintf);
     }
   else if (!strncmp(cmd, "list", 2))
     {
	if (prm[0] == '\0')
	  {
	     IpcPrintf("Normal:\n");
	     ECORE_LIST_FOR_EACH(aclass_list, ac) IpcPrintf("%s\n", ac->name);
	     IpcPrintf("Global:\n");
	     ECORE_LIST_FOR_EACH(aclass_list_global, ac) IpcPrintf("%s\n",
								   ac->name);
	  }
	else if (!strcmp(prm, "all"))
	  {
	     ECORE_LIST_FOR_EACH(aclass_list, ac)
	     {
		IpcPrintf("\n");
		AclassConfigWrite(ac, IpcPrintf);
	     }
	     ECORE_LIST_FOR_EACH(aclass_list_global, ac)
	     {
		IpcPrintf("\n");
		AclassConfigWrite(ac, IpcPrintf);
	     }
	  }
	else
	  {
	     AclassConfigWrite(ActionclassFindAny(prm), IpcPrintf);
	  }
     }
   else if (!strcmp(cmd, "load"))
     {
	if (*prm == '\0')
	   AclassConfigLoadConfig("bindings.cfg");
	else
	   AclassConfigLoadConfig(prm);
     }
}

static const IpcItem AclassIpcArray[] = {
   {
    AclassIpc,
    "aclass", "ac",
    "Action class functions",
    "  aclass kb                 List key bindings\n"
    "  aclass kb set ...         Set key bindings\n"
    "  aclass list [name/all]    List action class[es]\n"
    "  aclass load [name]        Reload action classes (default is bindings.cfg)\n"}
};
#define N_IPC_FUNCS (sizeof(AclassIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
extern const EModule ModAclass;

const EModule       ModAclass = {
   "aclass", "ac",
   AclassSighan,
   {N_IPC_FUNCS, AclassIpcArray},
   {0, NULL}
};

void
GrabButtonGrabs(Win win)
{
   ActionClass        *ac;
   int                 j;
   Action             *aa;
   unsigned int        mod, button, mask;

   ac = ActionclassFind("BUTTONBINDINGS");
   if (!ac)
      return;

   ac->ref_count++;
   for (j = 0; j < ac->num; j++)
     {
	aa = ac->list[j];
	if ((!aa) || ((aa->event != EVENT_MOUSE_DOWN) &&
		      (aa->event != EVENT_MOUSE_UP)))
	   continue;

	mod = (aa->anymodifier) ? AnyModifier : aa->modifiers;
	button = (aa->anybutton) ? AnyButton : aa->button;
	mask = ButtonPressMask | ButtonReleaseMask;

	GrabButtonSet(button, mod, win, mask, ECSR_PGRAB, 1);
     }
}

void
UnGrabButtonGrabs(Win win)
{
   ActionClass        *ac;
   int                 j;
   Action             *aa;
   unsigned int        mod, button;

   ac = ActionclassFind("BUTTONBINDINGS");
   if (!ac)
      return;

   ac->ref_count--;
   for (j = 0; j < ac->num; j++)
     {
	aa = ac->list[j];
	if ((!aa) || ((aa->event != EVENT_MOUSE_DOWN)
		      && (aa->event != EVENT_MOUSE_UP)))
	   continue;

	mod = (aa->anymodifier) ? AnyModifier : aa->modifiers;
	button = (aa->anybutton) ? AnyButton : aa->button;

	GrabButtonRelease(button, mod, win);
     }
}

static void
GrabActionKey(Action * aa)
{
   int                 mod;

   if (!aa->key)
      return;

   mod = (aa->anymodifier) ? AnyModifier : aa->modifiers;

   GrabKeySet(aa->key, mod, VROOT);
}

static void
UnGrabActionKey(Action * aa)
{
   int                 mod;

   if (!aa->key)
      return;

   mod = (aa->anymodifier) ? AnyModifier : aa->modifiers;

   GrabKeyRelease(aa->key, mod, VROOT);
}

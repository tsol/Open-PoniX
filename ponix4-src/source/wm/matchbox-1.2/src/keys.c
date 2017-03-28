/* 
 *  Matchbox Window Manager - A lightweight window manager not for the
 *                            desktop.
 *
 *  Authored By Matthew Allum <mallum@o-hand.com>
 *
 *  Copyright (c) 2002, 2004 OpenedHand Ltd - http://o-hand.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include "structs.h"
#include "keys.h"

#ifndef NO_KBD

static Bool
keys_keysym_needs_shift(Wm *w, KeySym keysym)
{
  int col;     /* XXX double check this is correct / easiest way  */
  int keycode;
  KeySym k;
  int min_kc, max_kc;

  XDisplayKeycodes(w->dpy, &min_kc, &max_kc);

  for (keycode = min_kc; keycode <= max_kc; keycode++) {
    for (col = 0; (k = XKeycodeToKeysym (w->dpy, keycode, col))
           != NoSymbol; col++)
      if (k == keysym && col == 1) {
        return True;
      }
  }  
  return False;
}

static KeySym
keys_keysym_get_shifted (Wm *w, KeySym keysym)
{
  int    min_kc, max_kc, keycode;
  KeySym k;

  XDisplayKeycodes (w->dpy, &min_kc, &max_kc);

  for (keycode = min_kc; keycode <= max_kc; keycode++)
    if ((k = XKeycodeToKeysym (w->dpy, keycode, 0)) == keysym)
      return XKeycodeToKeysym (w->dpy, keycode, 1);

  return NoSymbol;
}

Bool
keys_add_entry(Wm *w, char *keystr, int action, int idata, char *sdata)
{
  char *p = keystr;
  char *q;
  int i = 0, mask = 0, index = 0;
  char *keydef = NULL;
  Bool want_shift = False;
  KeySym ks;
  MBConfigKbdEntry *entry;

  MBConfigKbd *kb =  w->config->kb;

  struct {

    char *def;
    int mask;

  } lookup[] = {
    { "ctrl", ControlMask },
    { "alt", kb->AltMask },
    { "meta", kb->MetaMask },
    { "super", kb->SuperMask },
    { "hyper", kb->HyperMask },
    { "mod1", Mod1Mask },
    { "mod2", Mod2Mask },
    { "mod3", Mod3Mask },
    { "mod4", Mod4Mask },
    { "mod5", Mod5Mask },
    { NULL, 0 }
  };

  dbg("%s() called, keystr: %s action: %i sdata %s\n", 
      __func__, keystr, action, sdata);

  while (*p != '\0')
    {
      Bool found = False;
      if (*p == '<') 		/* XXX check for esacped < */
	{
	  q = ++p;
	  while (*q != '\0' && *q != '>') q++;
	  if (*q == '\0') return False; /* Parse error */
	  
	  i = 0;
	  while (lookup[i].def != NULL && !found)
	    {
	      if (!strncasecmp(p, lookup[i].def, q-p))
		{
		  if (lookup[i].mask)
		    {
		      mask |= lookup[i].mask;
		      found = True;
		    } else {
		      fprintf(stderr, "matchbox: keyboard does not appear to have a <%s> key.\n",  lookup[i].def);
		    }
		}
	      i++;
	    }
	  if (found) 
	    {
	      p = q;
	    } else {
	      if (!strncasecmp(p, "shift", 5))
		{
		  want_shift = True;
		  p = q;
		}
	      else return False;
	    }
	} 
      else if (!isspace(*p)) {
	keydef = p;
	break;
      }
      p++;
    }

  if (!keydef) return False;

  if ((ks = XStringToKeysym(keydef)) == (KeySym)NULL)
    {
      if (islower(keydef[0]))  /* Try again, changing case */
	keydef[0] = toupper(keydef[0]);
      else
	keydef[0] = tolower(keydef[0]);

      if ((ks = XStringToKeysym(keydef)) == (KeySym)NULL)
	{
	  fprintf(stderr, "matchbox: Cant find keysym for %s\n", keydef);
	  dbg("%s() can find keysym %s\n", __func__, keydef);
	  return False;
	}
    }

  if (keys_keysym_needs_shift(w, ks))
    {
      mask |= ShiftMask;
      index = 1;
    }
  else if (want_shift)
    {
      KeySym shifted;

      /* Change the keysym for case of '<shift>lowerchar' where we
       * actually want to grab the shifted version of the keysym.
       */
      if ((shifted = keys_keysym_get_shifted (w, ks)) != NoSymbol)
        {
          ks = shifted;
          index = 1;
        }

      /* Set the mask even if no shifted version - <shift>up key for
       * example.
       */
      mask |= ShiftMask;
    }


  dbg("%s() keydefinition is %s, want_shift is %i\n", __func__, keydef, mask & ShiftMask);

  /* If we grab keycode 0, we end up grabbing the entire keyboard :\ */
  if (XKeysymToKeycode(w->dpy, ks) == 0 && mask == 0)
    {
      fprintf(stderr, "matchbox: Cant find a keycode for keysym %li\n", ks);
      return False;
    }

  if (w->config->kb->entrys == NULL)
    {
      w->config->kb->entrys = malloc(sizeof(MBConfigKbdEntry));
      entry = w->config->kb->entrys;
    }
  else
    {
      entry = w->config->kb->entrys;
      while (entry->next_entry != NULL) 
	entry = entry->next_entry; 
    
      entry->next_entry = malloc(sizeof(MBConfigKbdEntry));
      entry = entry->next_entry;
    }

  entry->next_entry   = NULL;
  entry->action       = action;
  entry->ModifierMask = mask;
  entry->index        = index;
  entry->key          = ks;
  entry->idata        = idata;
  entry->sdata        = (( sdata == NULL ) ? NULL : strdup(sdata));

  dbg("added new key entry mask: %i\n", entry->ModifierMask);

  return True;
}
 

Bool
keys_load_config(Wm *w)
{

#ifdef USE_GCONF

  int   i = 0;
  char *key_def_str = NULL;
  char  gconf_key_cmd_keys[256] = { 0 };
  char  gconf_key_cmd[256]      = { 0 };


#define KEY_CMD_ENTRYS_N 12

  struct {

    char *key;
    int action;

  } lookup[] = {
    { "/apps/matchbox/global_keybindings/cycle_windows", KEY_ACTN_NEXT_CLIENT },
    { "/apps/matchbox/global_keybindings/cycle_windows_backward", KEY_ACTN_PREV_CLIENT },
    { "/apps/matchbox/window_keybindings/close", KEY_ACTN_CLOSE_CLIENT },
    { "/apps/matchbox/global_keybindings/show_desktop", KEY_ACTN_TOGGLE_DESKTOP },
    { "/apps/matchbox/window_keybindings/activate_window_menu", KEY_ACTN_TASK_MENU_ACTIVATE },
    { "/apps/matchbox/window_keybindings/toggle_fullscreen", KEY_ACTN_FULLSCREEN         },
    { "/apps/matchbox/window_keybindings/toggle_titlebar",  KEY_ACTN_HIDE_TITLEBAR      },
    { NULL, 0 }
  };

  while (lookup[i].key != NULL)
    {
      key_def_str = gconf_client_get_string(w->gconf_client, 
					    lookup[i].key, NULL);
      
      if (key_def_str && strcasecmp(key_def_str, "disabled"))
	keys_add_entry(w, key_def_str, lookup[i].action, 0, NULL);
      i++;
    }
  
  /* Add any command entrys */
  for (i=0; i<KEY_CMD_ENTRYS_N; i++)
    {
      char *cmd_value = NULL, *cmd_keys = NULL;
      int action = 0;
      
      snprintf(gconf_key_cmd, 256, 
	       "/apps/matchbox/keybinding_commands/command_%i", i+1);
      snprintf(gconf_key_cmd_keys, 256, 
	       "/apps/matchbox/global_keybindings/run_command_%i", i+1);
      
      cmd_value = gconf_client_get_string(w->gconf_client, 
					  gconf_key_cmd, NULL);
      cmd_keys = gconf_client_get_string(w->gconf_client, 
					gconf_key_cmd_keys, NULL);
      
      if (cmd_value && cmd_keys 
	  && strcasecmp(cmd_value, "disabled")
	  && strcasecmp(cmd_keys, "disabled") )
	{
#ifdef USE_LIBSN
	  if (*cmd_value == '!')
	    {
	      action = KEY_ACTN_EXEC_SN;
	      cmd_value++;
	    }
	  else if (*cmd_value == '$')
	    {
	      action = KEY_ACTN_EXEC_SINGLE;
	      cmd_value++;
	    }
	  else 
#else
	    if ((*(cmd_value+1) == '!') || (*(cmd_value+1) == '$'))
	      cmd_value++;
#endif
	  action = KEY_ACTN_EXEC;
	  
	  keys_add_entry(w, cmd_keys, action, 0, cmd_value);
	}
    }

#else  /* No Gconf, Load the shortcuts from a file */

  FILE *fp;
  char data[256];
  char *key = NULL, *val = NULL, *str = NULL;
  int i = 0;
  struct stat stat_info;
  char *conf_path = NULL;

  struct {

    char *str;
    int act;

  } lookup[] = {
    { "next",       KEY_ACTN_NEXT_CLIENT        },
    { "prev",       KEY_ACTN_PREV_CLIENT        },
    { "close",      KEY_ACTN_CLOSE_CLIENT       },
    { "desktop",    KEY_ACTN_TOGGLE_DESKTOP     },
    { "taskmenu",   KEY_ACTN_TASK_MENU_ACTIVATE },
    { "fullscreen", KEY_ACTN_FULLSCREEN         },
    { "hidetitle",  KEY_ACTN_HIDE_TITLEBAR      },
    { NULL, 0 }
  };

  if (w->config->kbd_conf_file != NULL)
    conf_path = w->config->kbd_conf_file;

  if (conf_path == NULL && getenv("HOME"))
    {
      conf_path = malloc(sizeof(char)*(strlen(getenv("HOME"))+25));
      sprintf(conf_path, "%s/.matchbox/kbdconfig", getenv("HOME"));
      if (stat(conf_path, &stat_info) == -1)
	{
	  free(conf_path);
	  conf_path = NULL;
	}
    }

  if (conf_path == NULL) conf_path = strdup(CONFDIR "/kbdconfig");

  if (!(fp = fopen(conf_path, "r"))) 
    {
      free(conf_path);
      return False;
    }

  dbg("%s() parsing keyboard config: %s\n", __func__, conf_path);

  free(conf_path);

  while(fgets(data,256,fp) != NULL)
    {
      if (data[0] == '#') 
	continue;
      str = strdup(data);
      if ( (val = strchr(str, '=')) != NULL)
	{
	  *val++ = '\0'; key = str;
	  if (*val != '\0')
	    {
	      int action = 0;
	      if (val[strlen(val)-1] == '\n') val[strlen(val)-1] = '\0';

	      if (*val == '!')
		{		/* It some kind of exec */
		  val++;
#ifdef USE_LIBSN

		  if (*val == '!')
		    {
		      action = KEY_ACTN_EXEC_SN;
		      val++;
		    }
		  else if (*val == '$')
		    {
		      action = KEY_ACTN_EXEC_SINGLE;
		      val++;
		    }
		  else 
#else
		    if ((*(val+1) == '!') || (*(val+1) == '$'))
		      val++;
#endif
		    action = KEY_ACTN_EXEC;
		}
	      else 
		{
		  i = 0;
		  while (lookup[i].str != NULL)
		    {
		      if (!strncasecmp(lookup[i].str, val, 
				       strlen(lookup[i].str)))
			{
			  action = lookup[i].act;
			  break;
			}
		      i++;
		    }
		}
	      if (!(action && keys_add_entry(w, key, action, 0, val)))
		fprintf(stderr, "matchbox: ignoring key shortcut %s\n", data);

	    }
	}
      free(str);
    }
  fclose(fp);

#endif

  
  return 1;
}

void
keys_get_modifiers(Wm *w)
{
  int mod_idx, mod_key, col, kpm;
  XModifierKeymap *mod_map = XGetModifierMapping(w->dpy);

  MBConfigKbd *kbd =  w->config->kb;

  kbd->MetaMask = 0;
  kbd->HyperMask = 0;
  kbd->SuperMask = 0;
  kbd->AltMask = 0;
  kbd->ModeMask = 0; 
  kbd->NumLockMask = 0;
  kbd->ScrollLockMask = 0;

  kpm = mod_map->max_keypermod;
  for (mod_idx = 0; mod_idx < 8; mod_idx++)
    for (mod_key = 0; mod_key < kpm; mod_key++) 
      {
	KeySym last_sym = 0;
	for (col = 0; col < 4; col += 2) 
	  {
	    KeyCode code = mod_map->modifiermap[mod_idx * kpm + mod_key];
	    KeySym sym = (code ? XKeycodeToKeysym(w->dpy, code, col) : 0);

	    if (sym == last_sym) continue;
	    last_sym = sym;

	    switch (sym) 
	      {
	      case XK_Mode_switch:
		/* XXX store_modifier("Mode_switch", mode_bit); */
		break;
	      case XK_Meta_L:
	      case XK_Meta_R:
		kbd->MetaMask |= (1 << mod_idx); 
		break;
	      case XK_Super_L:
	      case XK_Super_R:
		kbd->SuperMask |= (1 << mod_idx);
		break;
	      case XK_Hyper_L:
	      case XK_Hyper_R:
		kbd->HyperMask |= (1 << mod_idx);
		break;
	      case XK_Alt_L:
	      case XK_Alt_R:
		kbd->AltMask |= (1 << mod_idx);
		break;
	      case XK_Num_Lock:
		kbd->NumLockMask |= (1 << mod_idx);
		break;
	      case XK_Scroll_Lock:
		kbd->ScrollLockMask |= (1 << mod_idx);
		break;
	      }
	  }
      }

  /* XXX check this. assume alt <=> meta if only either set */
  if (!kbd->AltMask)  kbd->AltMask  = kbd->MetaMask; 
  if (!kbd->MetaMask) kbd->MetaMask = kbd->AltMask; 

  kbd->lock_mask = kbd->ScrollLockMask | kbd->NumLockMask | LockMask;

  if (mod_map) XFreeModifiermap(mod_map);

}

void
keys_grab(Wm *w, Bool ungrab)
{
  MBConfigKbdEntry *entry =  w->config->kb->entrys;

  while (entry != NULL)
    {
      int ignored_mask = 0;

      /* Needed to grab all ignored combo's too */
      while (ignored_mask < (int) w->config->kb->lock_mask)
	{                                       
	  if (ignored_mask & ~(w->config->kb->lock_mask))
	    {
	      ++ignored_mask;
	      continue;
	    }

	  if (ungrab)
	    {
	      dbg("keys, ungrabbing %i , %i\n", 
		  XKeysymToKeycode(w->dpy, entry->key), entry->ModifierMask);
	      XUngrabKey(w->dpy, 
			 XKeysymToKeycode(w->dpy, entry->key), 
			 entry->ModifierMask | ignored_mask,
			 w->root);
	    } else {

	      int result; 

	      misc_trap_xerrors();

	      dbg("keys, grabbing keycode: %i , mask: %i\n", 
		  XKeysymToKeycode(w->dpy, entry->key), 
		  entry->ModifierMask | ignored_mask);

	      XGrabKey(w->dpy, XKeysymToKeycode(w->dpy, entry->key), 
		       entry->ModifierMask | ignored_mask,
		       w->root, True, GrabModeAsync, GrabModeAsync);

	      result = misc_untrap_xerrors(); 
	      if (result != Success)
		{
		  if (result == BadAccess)
		    fprintf(stderr, "matchbox: Some other program is already using the key %s with modifiers %x as a binding\n",  
			    (XKeysymToString(entry->key)) ? XKeysymToString (entry->key) : "unknown", 
			    entry->ModifierMask | ignored_mask );
		  else
		    fprintf(stderr, "matchbox: Unable to grab the key %s with modifiers %x as a binding\n",  
			    (XKeysymToString(entry->key)) ? XKeysymToString (entry->key) : "unknown", 
			    entry->ModifierMask | ignored_mask );
		}
	    }
	  ++ignored_mask;
	}
      entry = entry->next_entry;
    }
}

static void 
keys_entries_free(Wm *w)
{
  MBConfigKbdEntry *cur =  w->config->kb->entrys, *entry = NULL;

  while (cur != NULL)
    {
      entry = cur;
      if (entry->sdata) free(entry->sdata);
      cur = entry->next_entry;
      free(entry);
    }

  w->config->kb->entrys = NULL;
}

static void
keys_load_and_grab(Wm *w)
{
  keys_get_modifiers(w);
  
  if (!keys_load_config(w))
    {
      fprintf(stderr, "matchbox: failed to load keyboard config\n");
      return;
    }

  keys_grab(w, False);
}

void
keys_reinit(Wm *w)
{
  keys_grab(w, True);
  keys_entries_free(w);
  keys_load_and_grab(w);
}

void
keys_init(Wm *w)
{
  w->config->kb = malloc(sizeof(MBConfigKbd));

  w->config->kb->entrys = NULL;
  
  keys_load_and_grab(w);
}

#endif 

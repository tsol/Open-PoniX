/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2010 Kim Woelders
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
#if HAVE_SOUND
#include "dialog.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "settings.h"
#include "sound.h"
#include "sounds.h"

#if HAVE_SOUND_ESD
#define SOUND_SERVER_NAME "esd"
#elif HAVE_SOUND_PA
#define SOUND_SERVER_NAME "pulseaudio"
#else
#error Invalid sound configuration
#endif

#define N_SOUNDS (SOUND_NOT_USED - 1)

typedef struct {
   char               *name;
   char               *file;
   Sample             *sample;
} SoundClass;

#define SC_NAME(sc) ((sc) ? (sc)->name : "(none)")

static struct {
   char                enable;
   char               *theme;
   unsigned int        mask1, mask2;
} Conf_sound;

static struct {
   char                cfg_loaded;
   char               *theme_path;
} Mode_sound;

#define SOUND_THEME_PATH ((Mode_sound.theme_path) ? Mode_sound.theme_path : Mode.theme.path)

static Ecore_List  *sound_list = NULL;

#if USE_MODULES
static const SoundOps *ops = NULL;
#else
#if HAVE_SOUND_ESD
extern const SoundOps SoundOps_esd;
static const SoundOps *ops = &SoundOps_esd;
#elif HAVE_SOUND_PA
extern const SoundOps SoundOps_pa;
static const SoundOps *ops = &SoundOps_pa;
#endif
#endif

static void         _SoundConfigLoad(void);

static const char  *const sound_names[N_SOUNDS] = {
   "SOUND_ALERT",		/*  0 0x00000001 */
   "SOUND_BUTTON_CLICK",
   "SOUND_BUTTON_RAISE",
   "SOUND_DEICONIFY",
   "SOUND_DESKTOP_LOWER",	/*  4 0x00000010 */
   "SOUND_DESKTOP_RAISE",
   "SOUND_DESKTOP_SHUT",
   "SOUND_ERROR_IPC",
   "SOUND_EXIT",		/*  8 0x00000100 */
   "SOUND_FOCUS_SET",
   "SOUND_ICONIFY",
   "SOUND_INSERT_KEYS",
   "SOUND_LOGOUT",		/* 12 0x00001000 */
   "SOUND_LOWER",
   "SOUND_MENU_SHOW",
   "SOUND_MOVE_AREA_DOWN",
   "SOUND_MOVE_AREA_LEFT",	/* 16 0x00010000 */
   "SOUND_MOVE_AREA_RIGHT",
   "SOUND_MOVE_AREA_UP",
   "SOUND_MOVE_RESIST",
   "SOUND_MOVE_START",		/* 20 0x00100000 */
   "SOUND_MOVE_STOP",
   "SOUND_RAISE",
   "SOUND_RESIZE_START",
   "SOUND_RESIZE_STOP",		/* 24 0x01000000 */
   "SOUND_SCANNING",
   "SOUND_SETTINGS_ACTIVE",
   "SOUND_SETTINGS_ALL",
   "SOUND_SETTINGS_AREA",	/* 28 0x10000000 */
   "SOUND_SETTINGS_AUDIO",
   NULL,
   "SOUND_SETTINGS_BG",
   "SOUND_SETTINGS_COMPOSITE",	/*  0 0x00000001 */
   "SOUND_SETTINGS_DESKTOPS",
   "SOUND_SETTINGS_FOCUS",
   "SOUND_SETTINGS_FX",
   "SOUND_SETTINGS_GROUP",	/*  4 0x00000010 */
   "SOUND_SETTINGS_ICONBOX",
   "SOUND_SETTINGS_MENUS",
   "SOUND_SETTINGS_MISCELLANEOUS",
   "SOUND_SETTINGS_MOVERESIZE",	/*  8 0x00000100 */
   "SOUND_SETTINGS_PAGER",
   "SOUND_SETTINGS_PLACEMENT",
   "SOUND_SETTINGS_SESSION",
   "SOUND_SETTINGS_TOOLTIPS",	/* 12 0x00001000 */
   "SOUND_SETTINGS_TRANS",
   "SOUND_SHADE",
   "SOUND_SLIDEOUT_SHOW",
   "SOUND_STARTUP",		/* 16 0x00010000 */
   "SOUND_UNSHADE",
   "SOUND_WAIT",
   "SOUND_WINDOW_BORDER_CHANGE",
   "SOUND_WINDOW_CHANGE_LAYER_DOWN",	/* 20 0x00100000 */
   "SOUND_WINDOW_CHANGE_LAYER_UP",
   "SOUND_WINDOW_CLOSE",
   "SOUND_WINDOW_SLIDE",
   "SOUND_WINDOW_SLIDE_END",	/* 24 0x01000000 */
   "SOUND_WINDOW_STICK",
   "SOUND_WINDOW_UNSTICK",
};

static void
_SclassSampleDestroy(void *data, void *user_data __UNUSED__)
{
   SoundClass         *sclass = (SoundClass *) data;

   if (!sclass || !sclass->sample)
      return;

   if (ops)
      ops->SampleDestroy(sclass->sample);
   sclass->sample = NULL;
}

static SoundClass  *
SclassCreate(const char *name, const char *file)
{
   SoundClass         *sclass;

   sclass = EMALLOC(SoundClass, 1);
   if (!sclass)
      return NULL;

   if (!sound_list)
      sound_list = ecore_list_new();
   ecore_list_prepend(sound_list, sclass);

   sclass->name = Estrdup(name);
   sclass->file = Estrdup(file);
   sclass->sample = NULL;

   return sclass;
}

static void
SclassDestroy(SoundClass * sclass)
{
   if (!sclass)
      return;

   ecore_list_node_remove(sound_list, sclass);
   _SclassSampleDestroy(sclass, NULL);
   Efree(sclass->name);
   Efree(sclass->file);

   Efree(sclass);
}

static void
_SclassDestroy(void *data, void *user_data __UNUSED__)
{
   SclassDestroy((SoundClass *) data);
}

static void
SclassApply(SoundClass * sclass)
{
   if (!sclass || !Conf_sound.enable)
      return;

   if (!sclass->sample)
     {
	char               *file;

	file = FindFile(sclass->file, SOUND_THEME_PATH, FILE_TYPE_SOUND);
	if (file)
	  {
	     sclass->sample = ops->SampleLoad(file);
	     Efree(file);
	  }
	if (!sclass->sample)
	  {
	     DialogOK(_("Error finding sound file"),
		      _("Warning!  Enlightenment was unable to load the\n"
			"following sound file:\n%s\n"
			"Enlightenment will continue to operate, but you\n"
			"may wish to check your configuration settings.\n"),
		      sclass->file);
	     SclassDestroy(sclass);
	     return;
	  }
     }

   ops->SamplePlay(sclass->sample);
}

static int
_SclassMatchName(const void *data, const void *match)
{
   return strcmp(((const SoundClass *)data)->name, (const char *)match);
}

static SoundClass  *
SclassFind(const char *name)
{
   return (SoundClass *) ecore_list_find(sound_list, _SclassMatchName, name);
}

static void
_SoundPlayByName(const char *name)
{
   SoundClass         *sclass;

   if (!Conf_sound.enable)
      return;

   if (!name || !*name)
      return;

   sclass = SclassFind(name);

   if (EDebug(EDBUG_TYPE_SOUND))
      Eprintf("%s: %s file=%s\n", "SclassApply", name, SC_NAME(sclass));

   SclassApply(sclass);
}

#define _SoundMasked(i) \
    (((i) <= 32) ? Conf_sound.mask1 & (1 << ((i) - 1)) : \
                   Conf_sound.mask2 & (1 << ((i) - 1)))
void
SoundPlay(int sound)
{
   if (!Conf_sound.enable)
      return;

   if (sound <= 0 || sound > N_SOUNDS)
      return;

   if (_SoundMasked(sound))
      return;

   _SoundPlayByName(sound_names[sound - 1]);
}

static int
SoundFree(const char *name)
{
   SoundClass         *sclass;

   sclass = SclassFind(name);
   SclassDestroy(sclass);

   return ! !sclass;
}

static void
SoundInit(void)
{
   int                 err;

   if (!Conf_sound.enable)
      return;

   err = -1;
#if USE_MODULES
   if (!ops)
#if HAVE_SOUND_ESD
      ops = ModLoadSym("sound", "SoundOps", "esd");
#elif HAVE_SOUND_PA
      ops = ModLoadSym("sound", "SoundOps", "pa");
#endif
#endif
   if (ops && ops->Init)
      err = ops->Init();

   if (err)
     {
	Conf_sound.enable = 0;
	AlertX(_("Error initialising sound"), _("OK"), NULL, NULL,
	       _("Audio was enabled for Enlightenment but there was an error\n"
		 "communicating with the audio server (%s).\n"
		 "Audio will now be disabled.\n"), SOUND_SERVER_NAME);
     }

   _SoundConfigLoad();
}

static void
SoundExit(void)
{
   ecore_list_for_each(sound_list, _SclassSampleDestroy, NULL);

   if (ops)
      ops->Exit();

   Conf_sound.enable = 0;
}

/*
 * Configuration load/save
 */

static int
_SoundConfigParse(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s1[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1, fields;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = -1;
	fields = sscanf(s, "%d", &i1);
	if (fields == 1)	/* Just skip the numeric config stuff */
	   continue;

	s1[0] = s2[0] = '\0';
	fields = sscanf(s, "%4000s %4000s", s1, s2);
	if (fields != 2)
	  {
	     Eprintf("*** Ignoring line: %s\n", s);
	     continue;
	  }
	SclassCreate(s1, s2);
     }
#if 0				/* Errors here are just ignored */
   if (err)
      ConfigAlertLoad("Sound");
#endif

   return err;
}

static void
_SoundConfigLoad(void)
{
   if (Mode_sound.cfg_loaded)
      return;
   Mode_sound.cfg_loaded = 1;

   Efree(Mode_sound.theme_path);
   if (Conf_sound.theme)
      Mode_sound.theme_path = ThemeFind(Conf_sound.theme);
   else
      Mode_sound.theme_path = NULL;

   ConfigFileLoad("sound.cfg", SOUND_THEME_PATH, _SoundConfigParse, 1);
}

static void
_SoundConfigUnload(void)
{
   ecore_list_for_each(sound_list, _SclassDestroy, NULL);
   Mode_sound.cfg_loaded = 0;
}

static void
_SoundThemeChange(void *item __UNUSED__, const char *theme)
{
   if (*theme == '\0')
      theme = NULL;
   _SoundConfigUnload();
   _EFDUP(Conf_sound.theme, theme);
   _SoundConfigLoad();
}

/*
 * Sound module
 */

static void
SoundSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	memset(&Mode_sound, 0, sizeof(Mode_sound));
	break;
     case ESIGNAL_CONFIGURE:
	SoundInit();
	break;
     case ESIGNAL_START:
	if (!Conf_sound.enable)
	   break;
	SoundPlay(SOUND_STARTUP);
	SoundFree("SOUND_STARTUP");
	break;
     case ESIGNAL_EXIT:
/*      if (Mode.wm.master) */
	SoundExit();
	break;
     }
}

#if ENABLE_DIALOGS
/*
 * Configuration dialog
 */
static char         tmp_audio;

static void
CB_ConfigureAudio(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf_sound.enable = tmp_audio;
	if (Conf_sound.enable)
	   SoundInit();
	else
	   SoundExit();
     }
   autosave();
}

static void
_DlgFillSound(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di;

   tmp_audio = Conf_sound.enable;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable sounds"));
   DialogItemCheckButtonSetPtr(di, &tmp_audio);
}

const DialogDef     DlgSound = {
   "CONFIGURE_AUDIO",
   N_("Sound"),
   N_("Audio Settings"),
   SOUND_SETTINGS_AUDIO,
   "pix/sound.png",
   N_("Enlightenment Audio\n" "Settings Dialog"),
   _DlgFillSound,
   DLG_OAC, CB_ConfigureAudio,
};
#endif /* ENABLE_DIALOGS */

/*
 * IPC functions
 */

static void
SoundIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;
   SoundClass         *sc;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!strncmp(cmd, "del", 3))
     {
	SoundFree(prm);
     }
   else if (!strncmp(cmd, "list", 2))
     {
	ECORE_LIST_FOR_EACH(sound_list, sc) IpcPrintf("%s\n", sc->name);
     }
   else if (!strncmp(cmd, "new", 3))
     {
	SclassCreate(prm, p);
     }
   else if (!strncmp(cmd, "off", 2))
     {
	SoundExit();
	autosave();
     }
   else if (!strncmp(cmd, "on", 2))
     {
	Conf_sound.enable = 1;
	SoundInit();
	autosave();
     }
   else if (!strncmp(cmd, "play", 2))
     {
	_SoundPlayByName(prm);
     }
}

static const IpcItem SoundIpcArray[] = {
   {
    SoundIpc,
    "sound", "snd",
    "Sound functions",
    "  sound add <classname> <filename> Create soundclass\n"
    "  sound del <classname>            Delete soundclass\n"
    "  sound list                       Show all sounds\n"
    "  sound off                        Disable sounds\n"
    "  sound on                         Enable sounds\n"
    "  sound play <classname>           Play sounds\n"}
};
#define N_IPC_FUNCS (sizeof(SoundIpcArray)/sizeof(IpcItem))

static const CfgItem SoundCfgItems[] = {
   CFG_ITEM_BOOL(Conf_sound, enable, 0),
   CFG_FUNC_STR(Conf_sound, theme, _SoundThemeChange),
   CFG_ITEM_HEX(Conf_sound, mask1, 0),
   CFG_ITEM_HEX(Conf_sound, mask2, 0),
};
#define N_CFG_ITEMS (sizeof(SoundCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModSound;

const EModule       ModSound = {
   "sound", "audio",
   SoundSighan,
   {N_IPC_FUNCS, SoundIpcArray},
   {N_CFG_ITEMS, SoundCfgItems}
};

#endif /* HAVE_SOUND */

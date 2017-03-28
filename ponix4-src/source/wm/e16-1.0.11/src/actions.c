/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "E.h"
#include "desktops.h"
#include "file.h"
#include "user.h"

void
EexecCmd(const char *cmd)
{
   char              **lst;
   int                 fd, num;

   /* Close all file descriptors except the std 3 */
   for (fd = 3; fd < 1024; fd++)
      close(fd);

   lst = StrlistFromString(cmd, ' ', &num);

   execvp(lst[0], lst);
}

static void
StartupIdExport(void)
{
   char                buf[128];
   Desk               *dsk;
   int                 ax, ay;

   dsk = DesksGetCurrent();
   DeskGetArea(dsk, &ax, &ay);

   Esnprintf(buf, sizeof(buf), "e16/%d:%d:%d,%d", Mode.apps.startup_id,
	     dsk->num, ax, ay);
   Esetenv("DESKTOP_STARTUP_ID", buf);
}

static void
ExecSetupEnv(int flags)
{
   int                 fd;

   setsid();

   /* Close all file descriptors except the std 3 */
   for (fd = 3; fd < 1024; fd++)
      close(fd);

   /* Set up env stuff */
   if (flags & EXEC_SET_LANG)
      LangExport();
   if (flags & EXEC_SET_STARTUP_ID)
      StartupIdExport();

#if USE_LIBHACK
   if (Mode.wm.window)
     {
	char                buf[1024];

	Esnprintf(buf, sizeof(buf), "%s/libhack.so", EDirLib());
	Esetenv("LD_PRELOAD", buf);
     }
#endif
}

int
EspawnApplication(const char *params, int flags)
{
   char                exe[FILEPATH_LEN_MAX];
   const char         *sh;
   char               *path;
   char               *real_exec;

   if (!params)
      return -1;

   sscanf(params, "%4000s", exe);
   if (exe[0] == '\0')
      return -1;

   Mode.apps.startup_id++;
   if (fork())
      return 0;

   ExecSetupEnv(flags);

   sh = usershell();

   if (path_canexec(exe))
     {
	real_exec = EMALLOC(char, strlen(params) + 6);

	if (!real_exec)
	   return -1;
	sprintf(real_exec, "exec %s", params);

	execl(sh, sh, "-c", real_exec, NULL);
	/* We should not get here - invalid shell? */
     }

   if (!Mode.wm.startup)
     {
	path = path_test(exe, EFILE_ANY);
	if (!path)
	  {
	     /* absolute path */
	     if (isabspath(exe))
		AlertOK(_("There was an error running the program:\n"
			  "%s\n"
			  "This program could not be executed.\n"
			  "This is because the file does not exist.\n"), exe);
	     /* relative path */
	     else
		AlertOK(_("There was an error running the program:\n"
			  "%s\n"
			  "This program could not be executed.\n"
			  "This is most probably because this "
			  "program is not in the\n"
			  "path for your shell which is %s. "
			  "I suggest you read the manual\n"
			  "page for that shell and read up how to "
			  "change or add to your\n"
			  "execution path.\n"), exe, sh);
	  }
	else
	   /* it is a node on the filing sys */
	  {
	     /* it's a file */
	     if (isfile(path))
	       {
		  /* can execute it */
		  if (canexec(path))
		     AlertOK(_("There was an error running the program:\n"
			       "%s\n"
			       "This program could not be executed.\n"
			       "I am unsure as to why you could not "
			       "do this. The file exists,\n"
			       "is a file, and you are allowed to "
			       "execute it. I suggest you look\n"
			       "into this.\n"), path);
		  /* not executable file */
		  else
		     AlertOK(_("There was an error running the program:\n"
			       "%s\n"
			       "This program could not be executed.\n"
			       "This is because the file exists, is a "
			       "file, but you are unable\n"
			       "to execute it because you do not "
			       "have execute " "access to this file.\n"), path);
	       }
	     /* it's not a file */
	     else
	       {
		  /* its a dir */
		  if (isdir(path))
		     AlertOK(_("There was an error running the program:\n"
			       "%s\n"
			       "This program could not be executed.\n"
			       "This is because the file is in fact "
			       "a directory.\n"), path);
		  /* its not a file or a dir */
		  else
		     AlertOK(_("There was an error running the program:\n"
			       "%s\n"
			       "This program could not be executed.\n"
			       "This is because the file is not a "
			       "regular file.\n"), path);
	       }
	     Efree(path);
	  }
     }
   exit(100);
}

void
Espawn(int argc __UNUSED__, char **argv)
{
   if (!argv || !argv[0])
      return;

   if (fork())
      return;

   ExecSetupEnv(EXEC_SET_LANG);

   execvp(argv[0], argv);

   AlertOK(_("There was an error running the program:\n%s"), argv[0]);
   exit(100);
}

void
EspawnCmd(const char *cmd)
{
   int                 argc;
   char              **argv;

   argv = StrlistDecodeEscaped(cmd, &argc);
   Espawn(argc, argv);
   StrlistFree(argv, argc);
}

int
Esystem(const char *cmd)
{
   return system(cmd);
}

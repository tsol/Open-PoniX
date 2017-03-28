/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2012 Kim Woelders
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
#include "file.h"
#include "user.h"
#include "util.h"
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static int          usr_uid = -1;
static const char  *usr_name = "unknown";
static const char  *usr_home = "/tmp";
static const char  *usr_shell = "/bin/sh";

static void
_user_init(void)
{
   const char         *ss;
   struct passwd      *pwd;

   usr_uid = getuid();
   pwd = getpwuid(usr_uid);

   if (!pwd)
     {
	ss = getenv("TMPDIR");
	if (ss)
	   usr_home = ss;
     }

   ss = Estrdup(pwd->pw_name);
   if (ss)
      usr_name = ss;
   ss = Estrdup(pwd->pw_dir);
   if (ss)
      usr_home = ss;
   if (canexec(pwd->pw_shell))
     {
	ss = Estrdup(pwd->pw_shell);
	if (ss)
	   usr_shell = ss;
     }
}

const char         *
username(void)
{
   if (usr_uid < 0)
      _user_init();
   return usr_name;
}

const char         *
userhome(void)
{
   if (usr_uid < 0)
      _user_init();
   return usr_home;
}

const char         *
usershell(void)
{
   if (usr_uid < 0)
      _user_init();
   return usr_shell;
}

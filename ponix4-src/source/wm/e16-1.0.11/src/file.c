/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2007-2010 Kim Woelders
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
#include "file.h"
#include "util.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char              **
E_ls(const char *dir, int *num)
{
   int                 i, dirlen;
   int                 done = 0;
   DIR                *dirp;
   char              **names;
   struct dirent      *dp;

   if ((!dir) || (!*dir))
      return NULL;
   dirp = opendir(dir);
   if (!dirp)
     {
	*num = 0;
	return NULL;
     }
   /* count # of entries in dir (worst case) */
   for (dirlen = 0; readdir(dirp); dirlen++)
      ;
   if (!dirlen)
     {
	closedir(dirp);
	*num = dirlen;
	return NULL;
     }
   names = EMALLOC(char *, dirlen);

   if (!names)
      return NULL;

   rewinddir(dirp);
   for (i = 0; i < dirlen;)
     {
	dp = readdir(dirp);
	if (!dp)
	   break;
	if ((strcmp(dp->d_name, ".")) && (strcmp(dp->d_name, "..")))
	  {
	     names[i] = Estrdup(dp->d_name);
	     i++;
	  }
     }

   if (i < dirlen)
      dirlen = i;		/* dir got shorter... */
   closedir(dirp);
   *num = dirlen;

   /* do a simple bubble sort here to alphanumberic it */
   while (!done)
     {
	done = 1;
	for (i = 0; i < dirlen - 1; i++)
	  {
	     if (strcmp(names[i], names[i + 1]) > 0)
	       {
		  char               *tmp;

		  tmp = names[i];
		  names[i] = names[i + 1];
		  names[i + 1] = tmp;
		  done = 0;
	       }
	  }
     }
   return names;
}

void
E_md(const char *s)
{
   if ((!s) || (!*s))
      return;
   mkdir(s, S_IRWXU);
}

void
E_mv(const char *s, const char *ss)
{
   if ((!s) || (!ss) || (!*s) || (!*ss))
      return;
   rename(s, ss);
}

void
E_rm(const char *s)
{
   if ((!s) || (!*s))
      return;
   unlink(s);
}

int
file_test(const char *s, unsigned int test)
{
   struct stat         st;
   int                 mode;

   if (!s || !*s)
      return 0;

#define EFILE_ALL (EFILE_ANY | EFILE_REG | EFILE_DIR)
   if (test & EFILE_ALL)
     {
	if (stat(s, &st) < 0)
	   return 0;
	if ((test & EFILE_REG) && !S_ISREG(st.st_mode))
	   return 0;
	if ((test & EFILE_DIR) && !S_ISDIR(st.st_mode))
	   return 0;
     }

#define EPERM_ALL (EPERM_R | EPERM_W | EPERM_X)
   if (test & EPERM_ALL)
     {
	mode = 0;
	if (test & EPERM_R)
	   mode |= R_OK;
	if (test & EPERM_W)
	   mode |= W_OK;
	if (test & EPERM_X)
	   mode |= X_OK;
	if (access(s, mode))
	   return 0;
     }

   return 1;
}

time_t
moddate(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   if (st.st_mtime > st.st_ctime)
      return st.st_mtime;
   return st.st_ctime;
}

#if 0				/* Unused */
int
filesize(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return (int)st.st_size;
}
#endif

int
fileinode(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return (int)st.st_ino;
}

int
filedev(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return filedev_map((int)st.st_dev);
}

int
filedev_map(int dev)
{
#ifdef __linux__
   /* device numbers in the anonymous range can't be relied
    * upon, so map them all on a single one */
   switch (dev >> 8)
     {
     default:
	return dev;
     case 0:
     case 144:
     case 145:
     case 146:
	return 1;
     }
#else
   return dev;
#endif
}

int
isabspath(const char *path)
{
   return path[0] == '/';
}

const char         *
fileext(const char *file)
{
   const char         *p;

   p = strrchr(file, '.');
   if (p)
     {
	return p + 1;
     }
   return "";
}

char               *
fileof(const char *path)
{
   const char         *s1, *s2;

   s1 = strrchr(path, '/');
   s1 = (s1) ? s1 + 1 : path;
   s2 = strrchr(s1, '.');
   if (!s2)
      return Estrdup(s1);

   return Estrndup(s1, s2 - s1);
}

const char         *
fullfileof(const char *path)
{
   const char         *s;

   if (!path)
      return NULL;
   s = strrchr(path, '/');
   return (s) ? s + 1 : path;
}

char               *
path_test(const char *file, unsigned int test)
{
   char               *cp, *ep;
   char               *s, *p;
   unsigned int        len, exelen;

   if (!file)
      return NULL;

   if (isabspath(file))
     {
	if (file_test(file, test))
	   return Estrdup(file);
	return NULL;
     }
   cp = getenv("PATH");
   if (!cp)
      return Estrdup(file);

   exelen = strlen(file);
   s = NULL;
   ep = cp;
   for (; ep; cp = ep + 1)
     {
	ep = strchr(cp, ':');
	len = (ep) ? (unsigned int)(ep - cp) : strlen(cp);
	if (len == 0)
	   continue;
	p = EREALLOC(char, s, len + exelen + 2);

	if (!p)
	   break;
	s = p;
	memcpy(s, cp, len);
	s[len] = '/';
	memcpy(s + len + 1, file, exelen + 1);
	if (file_test(s, test))
	   return s;
     }
   Efree(s);
   return NULL;
}

int
path_canexec(const char *file)
{
   char               *s;

   s = path_test(file, EFILE_REG | EPERM_X);
   if (!s)
      return 0;
   Efree(s);
   return 1;
}

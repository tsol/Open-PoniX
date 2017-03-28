/* utility functions for `patch' */

/* Copyright (C) 1986 Larry Wall

   Copyright (C) 1992, 1993, 1997, 1998, 1999, 2001, 2002, 2003, 2006,
   2009 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.
   If not, write to the Free Software Foundation,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#define XTERN extern
#include <common.h>
#include <backupfile.h>
#include <dirname.h>
#include <hash.h>
#include <quotearg.h>
#include <quotesys.h>
#undef XTERN
#define XTERN
#include <util.h>
#include <xalloc.h>

#include <maketime.h>
#include <partime.h>

#include <signal.h>
#if !defined SIGCHLD && defined SIGCLD
#define SIGCHLD SIGCLD
#endif
#if ! HAVE_RAISE && ! defined raise
# define raise(sig) kill (getpid (), sig)
#endif

#include <stdarg.h>
#include <full-write.h>

static void makedirs (char *);

typedef struct
{
  dev_t dev;
  ino_t ino;
} file_id;

/* Return an index for ENTRY into a hash table of size TABLE_SIZE.  */

static size_t
file_id_hasher (void const *entry, size_t table_size)
{
  file_id const *e = entry;
  size_t i = e->ino + e->dev;
  return i % table_size;
}

/* Do ENTRY1 and ENTRY2 refer to the same files?  */

static bool
file_id_comparator (void const *entry1, void const *entry2)
{
  file_id const *e1 = entry1;
  file_id const *e2 = entry2;
  return (e1->ino == e2->ino && e1->dev == e2->dev);
}

static Hash_table *file_id_table;

/* Initialize the hash table.  */

void
init_backup_hash_table (void)
{
  file_id_table = hash_initialize (0, NULL, file_id_hasher,
				   file_id_comparator, free);
  if (!file_id_table)
    xalloc_die ();
}

/* Insert a file with status ST into the hash table.  */

static void
insert_file (struct stat const *st)
{
   file_id *p;
   static file_id *next_slot;

   if (!next_slot)
     next_slot = xmalloc (sizeof *next_slot);
   next_slot->dev = st->st_dev;
   next_slot->ino = st->st_ino;
   p = hash_insert (file_id_table, next_slot);
   if (!p)
     xalloc_die ();
   if (p == next_slot)
     next_slot = NULL;
}

/* Has the file identified by ST already been inserted into the hash
   table?  */

bool
file_already_seen (struct stat const *st)
{
  file_id f;
  f.dev = st->st_dev;
  f.ino = st->st_ino;
  return hash_lookup (file_id_table, &f) != 0;
}

static bool
contains_slash (const char *s)
{
  for (; *s; s++)
    if (ISSLASH(*s))
      return true;
  return false;
}

static void
create_backup_copy (char const *from, char const *to, struct stat *st,
		    bool to_dir_known_to_exist)
{
  struct utimbuf utimbuf;

  copy_file (from, to, 0, 0, st->st_mode, to_dir_known_to_exist);
  utimbuf.actime = st->st_atime;
  utimbuf.modtime = st->st_mtime;
  if (utime (to, &utimbuf) != 0)
    pfatal ("Can't set timestamp on file %s",
	    quotearg (to));
  if (chmod (to, st->st_mode) != 0)
    pfatal ("Can't set timestamp on file %s",
	    quotearg (to));
}

void
create_backup (char *to, struct stat *to_st, int *to_errno,
	       bool leave_original)
{
  struct stat tmp_st;
  int tmp_errno;

  if (! to_st || ! to_errno)
    {
      to_st = &tmp_st;
      to_errno = &tmp_errno;
    }
  *to_errno = stat (to, to_st) == 0 ? 0 : errno;

  if (! *to_errno && file_already_seen (to_st))
    {
      if (debug & 4)
	say ("File %s already seen\n", quotearg (to));
    }
  else
    {
      int try_makedirs_errno = 0;
      char *bakname;

      if (origprae || origbase || origsuff)
	{
	  char const *p = origprae ? origprae : "";
	  char const *b = origbase ? origbase : "";
	  char const *s = origsuff ? origsuff : "";
	  char const *t = to;
	  size_t plen = strlen (p);
	  size_t blen = strlen (b);
	  size_t slen = strlen (s);
	  size_t tlen = strlen (t);
	  char const *o;
	  size_t olen;

	  for (o = t + tlen, olen = 0;
	       o > t && ! ISSLASH (*(o - 1));
	       o--)
	    continue;
	  olen = t + tlen - o;
	  tlen -= olen;
	  bakname = xmalloc (plen + tlen + blen + olen + slen + 1);
	  memcpy (bakname, p, plen);
	  memcpy (bakname + plen, t, tlen);
	  memcpy (bakname + plen + tlen, b, blen);
	  memcpy (bakname + plen + tlen + blen, o, olen);
	  memcpy (bakname + plen + tlen + blen + olen, s, slen + 1);

	  if ((origprae
	       && (contains_slash (origprae + FILE_SYSTEM_PREFIX_LEN (origprae))
		   || contains_slash (to)))
	      || (origbase && contains_slash (origbase)))
	    try_makedirs_errno = ENOENT;
	}
      else
	{
	  bakname = find_backup_file_name (to, backup_type);
	  if (!bakname)
	    xalloc_die ();
	}

      if (*to_errno)
	{
	  int fd;

	  if (debug & 4)
	    say ("Creating empty file %s\n", quotearg (bakname));

	  try_makedirs_errno = ENOENT;
	  unlink (bakname);
	  while ((fd = creat (bakname, 0666)) < 0)
	    {
	      if (errno != try_makedirs_errno)
		pfatal ("Can't create file %s", quotearg (bakname));
	      makedirs (bakname);
	      try_makedirs_errno = 0;
	    }
	  if (close (fd) != 0)
	    pfatal ("Can't close file %s", quotearg (bakname));
	}
      else if (leave_original)
	create_backup_copy (to, bakname, to_st, try_makedirs_errno == 0);
      else
	{
	  if (debug & 4)
	    say ("Renaming file %s to %s\n",
		 quotearg_n (0, to), quotearg_n (1, bakname));
	  while (rename (to, bakname) != 0)
	    {
	      if (errno == try_makedirs_errno)
		{
		  makedirs (bakname);
		  try_makedirs_errno = 0;
		}
	      else if (errno == EXDEV)
		{
		  create_backup_copy (to, bakname, to_st,
				      try_makedirs_errno == 0);
		  unlink (to);
		  break;
		}
	      else
		pfatal ("Can't rename file %s to %s",
			quotearg_n (0, to), quotearg_n (1, bakname));
	    }
	}
      free (bakname);
    }
}

/* Move a file FROM (where *FROM_NEEDS_REMOVAL is nonzero if FROM
   needs removal when cleaning up at the end of execution, and where
   *FROMST is FROM's status if known),
   to TO, renaming it if possible and copying it if necessary.
   If we must create TO, use MODE to create it.
   If FROM is null, remove TO.
   FROM_NEEDS_REMOVAL must be nonnull if FROM is nonnull.
   and FROMST must be nonnull if both FROM and BACKUP are nonnull.
   Back up TO if BACKUP is true.  */

void
move_file (char const *from, int volatile *from_needs_removal,
	   struct stat const *fromst,
	   char *to, mode_t mode, bool backup)
{
  struct stat to_st;
  int to_errno = -1;

  if (backup)
    create_backup (to, &to_st, &to_errno, false);

  if (from)
    {
      if (debug & 4)
	say ("Renaming file %s to %s\n",
	     quotearg_n (0, from), quotearg_n (1, to));

      if (rename (from, to) != 0)
	{
	  bool to_dir_known_to_exist = false;

	  if (errno == ENOENT
	      && (to_errno == -1 || to_errno == ENOENT))
	    {
	      makedirs (to);
	      to_dir_known_to_exist = true;
	      if (rename (from, to) == 0)
		goto rename_succeeded;
	    }

	  if (errno == EXDEV)
	    {
	      struct stat tost;
	      if (! backup)
		{
		  if (unlink (to) == 0)
		    to_dir_known_to_exist = true;
		  else if (errno != ENOENT)
		    pfatal ("Can't remove file %s", quotearg (to));
		}
	      copy_file (from, to, &tost, 0, mode, to_dir_known_to_exist);
	      insert_file (&tost);
	      return;
	    }

	  pfatal ("Can't rename file %s to %s",
		  quotearg_n (0, from), quotearg_n (1, to));
	}

    rename_succeeded:
      if (fromst)
	insert_file (fromst);
      /* Do not clear *FROM_NEEDS_REMOVAL if it's possible that the
	 rename returned zero because FROM and TO are hard links to
	 the same file.  */
      if (0 < to_errno
	  || (to_errno == 0 && to_st.st_nlink <= 1))
	*from_needs_removal = 0;
    }
  else if (! backup)
    {
      if (debug & 4)
	say ("Removing file %s\n", quotearg (to));
      if (unlink (to) != 0 && errno != ENOENT)
	pfatal ("Can't remove file %s", quotearg (to));
    }
}

/* Create FILE with OPEN_FLAGS, and with MODE adjusted so that
   we can read and write the file and that the file is not executable.
   Return the file descriptor.  */
int
create_file (char const *file, int open_flags, mode_t mode,
	     bool to_dir_known_to_exist)
{
  int try_makedirs_errno = to_dir_known_to_exist ? 0 : ENOENT;
  int fd;
  mode |= S_IRUSR | S_IWUSR;
  mode &= ~ (S_IXUSR | S_IXGRP | S_IXOTH);
  do
    {
      if (! (O_CREAT && O_TRUNC))
	close (creat (file, mode));
      fd = open (file, O_CREAT | O_TRUNC | open_flags, mode);
      if (fd < 0)
	{
	  char *f;
	  if (errno != try_makedirs_errno)
	    pfatal ("Can't create file %s", quotearg (file));
	  f = xstrdup (file);
	  makedirs (f);
	  free (f);
	  try_makedirs_errno = 0;
	}
    } while (fd < 0);
  return fd;
}

static void
copy_to_fd (const char *from, int tofd)
{
  int fromfd;
  ssize_t i;

  if ((fromfd = open (from, O_RDONLY | O_BINARY)) < 0)
    pfatal ("Can't reopen file %s", quotearg (from));
  while ((i = read (fromfd, buf, bufsize)) != 0)
    {
      if (i == (ssize_t) -1)
	read_fatal ();
      if (full_write (tofd, buf, i) != i)
	write_fatal ();
    }
  if (close (fromfd) != 0)
    read_fatal ();
}

/* Copy a file. */

void
copy_file (char const *from, char const *to, struct stat *tost,
	   int to_flags, mode_t mode, bool to_dir_known_to_exist)
{
  int tofd;

  if (debug & 4)
    say ("Copying file %s to %s\n",
	 quotearg_n (0, from), quotearg_n (1, to));
  tofd = create_file (to, O_WRONLY | O_BINARY | to_flags, mode,
		      to_dir_known_to_exist);
  copy_to_fd (from, tofd);
  if ((tost && fstat (tofd, tost) != 0)
      || close (tofd) != 0)
    write_fatal ();
}

/* Append to file. */

void
append_to_file (char const *from, char const *to)
{
  int tofd;

  if ((tofd = open (to, O_WRONLY | O_BINARY | O_APPEND)) < 0)
    pfatal ("Can't reopen file %s", quotearg (to));
  copy_to_fd (from, tofd);
  if (close (tofd) != 0)
    write_fatal ();
}

static char const DEV_NULL[] = NULL_DEVICE;

static char const RCSSUFFIX[] = ",v";
static char const CHECKOUT[] = "co %s";
static char const CHECKOUT_LOCKED[] = "co -l %s";
static char const RCSDIFF1[] = "rcsdiff %s";

static char const SCCSPREFIX[] = "s.";
static char const GET[] = "get ";
static char const GET_LOCKED[] = "get -e ";
static char const SCCSDIFF1[] = "get -p ";
static char const SCCSDIFF2[] = "|diff - %s";

static char const CLEARTOOL_CO[] = "cleartool co -unr -nc ";

static char const PERFORCE_CO[] = "p4 edit ";

/* Return "RCS" if FILENAME is controlled by RCS,
   "SCCS" if it is controlled by SCCS,
   "ClearCase" if it is controlled by Clearcase,
   "Perforce" if it is controlled by Perforce,
   and 0 otherwise.
   READONLY is true if we desire only readonly access to FILENAME.
   FILESTAT describes FILENAME's status or is 0 if FILENAME does not exist.
   If successful and if GETBUF is nonzero, set *GETBUF to a command
   that gets the file; similarly for DIFFBUF and a command to diff the file
   (but set *DIFFBUF to 0 if the diff operation is meaningless).
   *GETBUF and *DIFFBUF must be freed by the caller.  */
char const *
version_controller (char const *filename, bool readonly,
		    struct stat const *filestat, char **getbuf, char **diffbuf)
{
  struct stat cstat;
  char *dir = dir_name (filename);
  char *filebase = base_name (filename);
  char const *dotslash = *filename == '-' ? "./" : "";
  size_t dirlen = strlen (dir) + 1;
  size_t maxfixlen = sizeof "SCCS/" - 1 + sizeof SCCSPREFIX - 1;
  size_t maxtrysize = dirlen + strlen (filebase) + maxfixlen + 1;
  size_t quotelen = quote_system_arg (0, dir) + quote_system_arg (0, filebase);
  size_t maxgetsize = sizeof CLEARTOOL_CO + quotelen + maxfixlen;
  size_t maxdiffsize =
    (sizeof SCCSDIFF1 + sizeof SCCSDIFF2 + sizeof DEV_NULL - 1
     + 2 * quotelen + maxfixlen);
  char *trybuf = xmalloc (maxtrysize);
  char const *r = 0;

  sprintf (trybuf, "%s/", dir);

#define try1(f,a1)    (sprintf (trybuf + dirlen, f, a1),    stat (trybuf, &cstat) == 0)
#define try2(f,a1,a2) (sprintf (trybuf + dirlen, f, a1,a2), stat (trybuf, &cstat) == 0)

  /* Check that RCS file is not working file.
     Some hosts don't report file name length errors.  */

  if ((try2 ("RCS/%s%s", filebase, RCSSUFFIX)
       || try1 ("RCS/%s", filebase)
       || try2 ("%s%s", filebase, RCSSUFFIX))
      && ! (filestat
	    && filestat->st_dev == cstat.st_dev
	    && filestat->st_ino == cstat.st_ino))
    {
      if (getbuf)
	{
	  char *p = *getbuf = xmalloc (maxgetsize);
	  sprintf (p, readonly ? CHECKOUT : CHECKOUT_LOCKED, dotslash);
	  p += strlen (p);
	  p += quote_system_arg (p, filename);
	  *p = '\0';
	}

      if (diffbuf)
	{
	  char *p = *diffbuf = xmalloc (maxdiffsize);
	  sprintf (p, RCSDIFF1, dotslash);
	  p += strlen (p);
	  p += quote_system_arg (p, filename);
	  *p++ = '>';
	  strcpy (p, DEV_NULL);
	}

      r = "RCS";
    }
  else if (try2 ("SCCS/%s%s", SCCSPREFIX, filebase)
	   || try2 ("%s%s", SCCSPREFIX, filebase))
    {
      if (getbuf)
	{
	  char *p = *getbuf = xmalloc (maxgetsize);
	  sprintf (p, readonly ? GET : GET_LOCKED);
	  p += strlen (p);
	  p += quote_system_arg (p, trybuf);
	  *p = '\0';
	}

      if (diffbuf)
	{
	  char *p = *diffbuf = xmalloc (maxdiffsize);
	  strcpy (p, SCCSDIFF1);
	  p += sizeof SCCSDIFF1 - 1;
	  p += quote_system_arg (p, trybuf);
	  sprintf (p, SCCSDIFF2, dotslash);
	  p += strlen (p);
	  p += quote_system_arg (p, filename);
	  *p++ = '>';
	  strcpy (p, DEV_NULL);
	}

      r = "SCCS";
    }
  else if (!readonly && filestat
	   && try1 ("%s@@", filebase) && S_ISDIR (cstat.st_mode))
    {
      if (getbuf)
	{
	  char *p = *getbuf = xmalloc (maxgetsize);
	  strcpy (p, CLEARTOOL_CO);
	  p += sizeof CLEARTOOL_CO - 1;
	  p += quote_system_arg (p, filename);
	  *p = '\0';
	}

      if (diffbuf)
	*diffbuf = 0;

      r = "ClearCase";
     }
  else if (!readonly && filestat &&
           (getenv("P4PORT") || getenv("P4USER") || getenv("P4CONFIG")))
    {
      if (getbuf)
	{
	  char *p = *getbuf = xmalloc (maxgetsize);
	  strcpy (p, PERFORCE_CO);
	  p += sizeof PERFORCE_CO - 1;
	  p += quote_system_arg (p, filename);
	  *p = '\0';
	}

      if (diffbuf)
	*diffbuf = 0;

      r = "Perforce";
    }

  free (trybuf);
  free (filebase);
  free (dir);
  return r;
}

/* Get FILENAME from version control system CS.  The file already exists if
   EXISTS.  Only readonly access is needed if READONLY.
   Use the command GETBUF to actually get the named file.
   Store the resulting file status into *FILESTAT.
   Return true if successful.  */
bool
version_get (char const *filename, char const *cs, bool exists, bool readonly,
	     char const *getbuf, struct stat *filestat)
{
  if (patch_get < 0)
    {
      ask ("Get file %s from %s%s? [y] ",
	   quotearg (filename), cs, readonly ? "" : " with lock");
      if (*buf == 'n')
	return 0;
    }

  if (dry_run)
    {
      if (! exists)
	fatal ("can't do dry run on nonexistent version-controlled file %s; invoke `%s' and try again",
	       quotearg (filename), getbuf);
    }
  else
    {
      if (verbosity == VERBOSE)
	say ("Getting file %s from %s%s...\n", quotearg (filename),
	     cs, readonly ? "" : " with lock");
      if (systemic (getbuf) != 0)
	fatal ("Can't get file %s from %s", quotearg (filename), cs);
      if (stat (filename, filestat) != 0)
	pfatal ("%s", quotearg (filename));
    }

  return 1;
}

/* Allocate a unique area for a string. */

char *
savebuf (register char const *s, register size_t size)
{
  register char *rv;

  if (! size)
    return NULL;

  rv = malloc (size);

  if (! rv)
    {
      if (! using_plan_a)
	xalloc_die ();
    }
  else
    memcpy (rv, s, size);

  return rv;
}

char *
savestr (char const *s)
{
  return savebuf (s, strlen (s) + 1);
}

void
remove_prefix (char *p, size_t prefixlen)
{
  char const *s = p + prefixlen;
  while ((*p++ = *s++))
    continue;
}

char *
format_linenum (char numbuf[LINENUM_LENGTH_BOUND + 1], LINENUM n)
{
  char *p = numbuf + LINENUM_LENGTH_BOUND;
  *p = '\0';

  if (n < 0)
    {
      do
	*--p = '0' - (int) (n % 10);
      while ((n /= 10) != 0);

      *--p = '-';
    }
  else
    {
      do
	*--p = '0' + (int) (n % 10);
      while ((n /= 10) != 0);
    }

  return p;
}

/* Terminal output, pun intended. */

void
fatal (char const *format, ...)
{
  va_list args;
  fprintf (stderr, "%s: **** ", program_name);
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  putc ('\n', stderr);
  fflush (stderr);
  fatal_exit (0);
}

void
xalloc_die (void)
{
  fatal ("out of memory");
}

void
read_fatal (void)
{
  pfatal ("read error");
}

void
write_fatal (void)
{
  pfatal ("write error");
}

/* Say something from patch, something from the system, then silence . . . */

void
pfatal (char const *format, ...)
{
  int errnum = errno;
  va_list args;
  fprintf (stderr, "%s: **** ", program_name);
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  fflush (stderr); /* perror bypasses stdio on some hosts.  */
  errno = errnum;
  perror (" ");
  fflush (stderr);
  fatal_exit (0);
}

/* Tell the user something.  */

static void
vsay (char const *format, va_list args)
{
  vfprintf (stdout, format, args);
  fflush (stdout);
}

void
say (char const *format, ...)
{
  va_list args;
  va_start (args, format);
  vsay (format, args);
  va_end (args);
}

/* Get a response from the user, somehow or other. */

void
ask (char const *format, ...)
{
  static int ttyfd = -2;
  ssize_t r;
  va_list args;

  va_start (args, format);
  vfprintf (stdout, format, args);
  va_end (args);
  fflush (stdout);

  if (ttyfd == -2)
    {
      /* If standard output is not a tty, don't bother opening /dev/tty,
	 since it's unlikely that stdout will be seen by the tty user.
	 The isatty test also works around a bug in GNU Emacs 19.34 under Linux
	 which makes a call-process `patch' hang when it reads from /dev/tty.
	 POSIX.1-2001 XCU line 26599 requires that we read /dev/tty,
	 though.  */
      ttyfd = (posixly_correct || isatty (STDOUT_FILENO)
	       ? open (TTY_DEVICE, O_RDONLY)
	       : -1);
    }

  if (ttyfd < 0)
    {
      /* No terminal at all -- default it.  */
      printf ("\n");
      buf[0] = '\n';
      buf[1] = '\0';
    }
  else
    {
      size_t s = 0;
      while ((r = read (ttyfd, buf + s, bufsize - 1 - s)) == bufsize - 1 - s
	     && buf[bufsize - 2] != '\n')
	{
	  s = bufsize - 1;
	  bufsize *= 2;
	  buf = realloc (buf, bufsize);
	  if (!buf)
	    xalloc_die ();
	}
      if (r == 0)
	printf ("EOF\n");
      else if (r < 0)
	{
	  perror ("tty read");
	  fflush (stderr);
	  close (ttyfd);
	  ttyfd = -1;
	  r = 0;
	}
      buf[s + r] = '\0';
    }
}

/* Return nonzero if it OK to reverse a patch.  */

bool
ok_to_reverse (char const *format, ...)
{
  bool r = false;

  if (noreverse || ! (force && verbosity == SILENT))
    {
      va_list args;
      va_start (args, format);
      vsay (format, args);
      va_end (args);
    }

  if (noreverse)
    {
      say ("  Skipping patch.\n");
      skip_rest_of_patch = true;
    }
  else if (force)
    {
      if (verbosity != SILENT)
	say ("  Applying it anyway.\n");
    }
  else if (batch)
    {
      say (reverse ? "  Ignoring -R.\n" : "  Assuming -R.\n");
      r = true;
    }
  else
    {
      ask (reverse ? "  Ignore -R? [n] " : "  Assume -R? [n] ");
      r = *buf == 'y';
      if (! r)
	{
	  ask ("Apply anyway? [n] ");
	  if (*buf != 'y')
	    {
	      if (verbosity != SILENT)
		say ("Skipping patch.\n");
	      skip_rest_of_patch = true;
	    }
	}
    }

  return r;
}

/* How to handle certain events when not in a critical region. */

#define NUM_SIGS ((int) (sizeof (sigs) / sizeof (*sigs)))
static int const sigs[] = {
#ifdef SIGHUP
       SIGHUP,
#endif
#ifdef SIGPIPE
       SIGPIPE,
#endif
#ifdef SIGTERM
       SIGTERM,
#endif
#ifdef SIGXCPU
       SIGXCPU,
#endif
#ifdef SIGXFSZ
       SIGXFSZ,
#endif
       SIGINT
};

#if !HAVE_SIGPROCMASK
#define sigset_t int
#define sigemptyset(s) (*(s) = 0)
#ifndef sigmask
#define sigmask(sig) (1 << ((sig) - 1))
#endif
#define sigaddset(s, sig) (*(s) |= sigmask (sig))
#define sigismember(s, sig) ((*(s) & sigmask (sig)) != 0)
#ifndef SIG_BLOCK
#define SIG_BLOCK 0
#endif
#ifndef SIG_UNBLOCK
#define SIG_UNBLOCK (SIG_BLOCK + 1)
#endif
#ifndef SIG_SETMASK
#define SIG_SETMASK (SIG_BLOCK + 2)
#endif
#define sigprocmask(how, n, o) \
  ((how) == SIG_BLOCK \
   ? ((o) ? *(o) = sigblock (*(n)) : sigblock (*(n))) \
   : (how) == SIG_UNBLOCK \
   ? sigsetmask (((o) ? *(o) = sigblock (0) : sigblock (0)) & ~*(n)) \
   : (o ? *(o) = sigsetmask (*(n)) : sigsetmask (*(n))))
#if !HAVE_SIGSETMASK
#define sigblock(mask) 0
#define sigsetmask(mask) 0
#endif
#endif

static sigset_t initial_signal_mask;
static sigset_t signals_to_block;

#if ! HAVE_SIGACTION
static void fatal_exit_handler (int) __attribute__ ((noreturn));
static void
fatal_exit_handler (int sig)
{
  signal (sig, SIG_IGN);
  fatal_exit (sig);
}
#endif

void
set_signals (bool reset)
{
  int i;
#if HAVE_SIGACTION
  struct sigaction initial_act, fatal_act;
  fatal_act.sa_handler = fatal_exit;
  sigemptyset (&fatal_act.sa_mask);
  fatal_act.sa_flags = 0;
#define setup_handler(sig) sigaction (sig, &fatal_act, (struct sigaction *) 0)
#else
#define setup_handler(sig) signal (sig, fatal_exit_handler)
#endif

  if (!reset)
    {
#ifdef SIGCHLD
      /* System V fork+wait does not work if SIGCHLD is ignored.  */
      signal (SIGCHLD, SIG_DFL);
#endif
      sigemptyset (&signals_to_block);
      for (i = 0;  i < NUM_SIGS;  i++)
	{
	  bool ignoring_signal;
#if HAVE_SIGACTION
	  if (sigaction (sigs[i], (struct sigaction *) 0, &initial_act) != 0)
	    continue;
	  ignoring_signal = initial_act.sa_handler == SIG_IGN;
#else
	  ignoring_signal = signal (sigs[i], SIG_IGN) == SIG_IGN;
#endif
	  if (! ignoring_signal)
	    {
	      sigaddset (&signals_to_block, sigs[i]);
	      setup_handler (sigs[i]);
	    }
	}
    }
  else
    {
      /* Undo the effect of ignore_signals.  */
#if HAVE_SIGPROCMASK || HAVE_SIGSETMASK
      sigprocmask (SIG_SETMASK, &initial_signal_mask, (sigset_t *) 0);
#else
      for (i = 0;  i < NUM_SIGS;  i++)
	if (sigismember (&signals_to_block, sigs[i]))
	  setup_handler (sigs[i]);
#endif
    }
}

/* How to handle certain events when in a critical region. */

void
ignore_signals (void)
{
#if HAVE_SIGPROCMASK || HAVE_SIGSETMASK
  sigprocmask (SIG_BLOCK, &signals_to_block, &initial_signal_mask);
#else
  int i;
  for (i = 0;  i < NUM_SIGS;  i++)
    if (sigismember (&signals_to_block, sigs[i]))
      signal (sigs[i], SIG_IGN);
#endif
}

void
exit_with_signal (int sig)
{
  sigset_t s;
  signal (sig, SIG_DFL);
  sigemptyset (&s);
  sigaddset (&s, sig);
  sigprocmask (SIG_UNBLOCK, &s, (sigset_t *) 0);
  raise (sig);
  exit (2);
}

int
systemic (char const *command)
{
  if (debug & 8)
    say ("+ %s\n", command);
  fflush (stdout);
  return system (command);
}

/* Replace '/' with '\0' in FILENAME if it marks a place that
   needs testing for the existence of directory.  Return the address
   of the last location replaced, or 0 if none were replaced.  */
static char *
replace_slashes (char *filename)
{
  char *f;
  char *last_location_replaced = 0;
  char const *component_start;

  for (f = filename + FILE_SYSTEM_PREFIX_LEN (filename);  ISSLASH (*f);  f++)
    continue;

  component_start = f;

  for (; *f; f++)
    if (ISSLASH (*f))
      {
	char *slash = f;

	/* Treat multiple slashes as if they were one slash.  */
	while (ISSLASH (f[1]))
	  f++;

	/* Ignore slashes at the end of the path.  */
	if (! f[1])
	  break;

	/* "." and ".." need not be tested.  */
	if (! (slash - component_start <= 2
	       && component_start[0] == '.' && slash[-1] == '.'))
	  {
	    *slash = '\0';
	    last_location_replaced = slash;
	  }

	component_start = f + 1;
      }

  return last_location_replaced;
}

/* Make sure we'll have the directories to create a file.
   Ignore the last element of `filename'.  */

static void
makedirs (register char *filename)
{
  register char *f;
  register char *flim = replace_slashes (filename);

  if (flim)
    {
      /* Create any missing directories, replacing NULs by '/'s.
	 Ignore errors.  We may have to keep going even after an EEXIST,
	 since the path may contain ".."s; and when there is an EEXIST
	 failure the system may return some other error number.
	 Any problems will eventually be reported when we create the file.  */
      for (f = filename;  f <= flim;  f++)
	if (!*f)
	  {
	    mkdir (filename,
		   S_IRUSR|S_IWUSR|S_IXUSR
		   |S_IRGRP|S_IWGRP|S_IXGRP
		   |S_IROTH|S_IWOTH|S_IXOTH);
	    *f = '/';
	  }
    }
}

/* Remove empty ancestor directories of FILENAME.
   Ignore errors, since the path may contain ".."s, and when there
   is an EEXIST failure the system may return some other error number.  */
void
removedirs (char *filename)
{
  size_t i;

  for (i = strlen (filename);  i != 0;  i--)
    if (ISSLASH (filename[i])
	&& ! (ISSLASH (filename[i - 1])
	      || (filename[i - 1] == '.'
		  && (i == 1
		      || ISSLASH (filename[i - 2])
		      || (filename[i - 2] == '.'
			  && (i == 2
			      || ISSLASH (filename[i - 3])))))))
      {
	filename[i] = '\0';
	if (rmdir (filename) == 0 && verbosity == VERBOSE)
	  say ("Removed empty directory %s\n", quotearg (filename));
	filename[i] = '/';
      }
}

static time_t initial_time;

void
init_time (void)
{
  time (&initial_time);
}

/* Make filenames more reasonable. */

char *
fetchname (char *at, int strip_leading, char **ptimestr, time_t *pstamp)
{
    char *name;
    char *timestr = NULL;
    register char *t;
    int sleading = strip_leading;
    time_t stamp = (time_t) -1;

    while (ISSPACE ((unsigned char) *at))
	at++;
    if (debug & 128)
	say ("fetchname %s %d\n", at, strip_leading);

    name = at;
    /* Strip up to `strip_leading' leading slashes and null terminate.
       If `strip_leading' is negative, strip all leading slashes.  */
    for (t = at;  *t;  t++)
      {
	if (ISSLASH (*t))
	  {
	    while (ISSLASH (t[1]))
	      t++;
	    if (strip_leading < 0 || --sleading >= 0)
		name = t+1;
	  }
	else if (ISSPACE ((unsigned char) *t))
	  {
	    /* Allow file names with internal spaces,
	       but only if a tab separates the file name from the date.  */
	    char const *u = t;
	    while (*u != '\t' && ISSPACE ((unsigned char) u[1]))
	      u++;
	    if (*u != '\t' && strchr (u + 1, '\t'))
	      continue;

	    if (*u == '\n')
	      stamp = (time_t) -1;
	    else
	      {
		if (ptimestr)
		  {
		    char const *t = u + strlen (u);
		    if (t != u && *(t-1) == '\n')
		      t--;
		    if (t != u && *(t-1) == '\r')
		      t--;
		    timestr = savebuf (u, t - u + 1);
		    timestr[t - u] = 0;
		  }

		if (set_time | set_utc)
		  stamp = str2time (&u, initial_time,
				    set_utc ? 0L : TM_LOCAL_ZONE);
		else
		  {
		    /* The head says the file is nonexistent if the
		       timestamp is the epoch; but the listed time is
		       local time, not UTC, and POSIX.1 allows local
		       time offset anywhere in the range -25:00 <
		       offset < +26:00.  Match any time in that range
		       by assuming local time is -25:00 and then
		       matching any ``local'' time T in the range 0 <
		       T < 25+26 hours.  */
		    stamp = str2time (&u, initial_time, -25L * 60 * 60);
		    if (0 < stamp && stamp < (25 + 26) * 60L * 60)
		      stamp = 0;
		  }

		if (*u && ! ISSPACE ((unsigned char) *u))
		  stamp = (time_t) -1;
	      }

	    *t = '\0';
	    break;
	  }
      }

    if (!*name)
      {
	if (timestr)
	  free (timestr);
	return 0;
      }

    /* If the name is "/dev/null", ignore the name and mark the file
       as being nonexistent.  The name "/dev/null" appears in patches
       regardless of how NULL_DEVICE is spelled.  */
    if (strcmp (at, "/dev/null") == 0)
      {
	if (pstamp)
	  *pstamp = 0;
	if (timestr)
	  free (timestr);
	return 0;
      }

    /* Ignore the name if it doesn't have enough slashes to strip off.  */
    if (0 < sleading)
      {
	if (timestr)
	  free (timestr);
	return 0;
      }

    if (pstamp)
      *pstamp = stamp;
    if (timestr)
      *ptimestr = timestr;
    return savestr (name);
}

void
Fseek (FILE *stream, file_offset offset, int ptrname)
{
  if (file_seek (stream, offset, ptrname) != 0)
    pfatal ("fseek");
}

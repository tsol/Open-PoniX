/* patch - a program to apply diffs to original files */

/* Copyright (C) 1984, 1985, 1986, 1987, 1988 Larry Wall

   Copyright (C) 1989, 1990, 1991, 1992, 1993, 1997, 1998, 1999, 2002,
   2003, 2006, 2009 Free Software Foundation, Inc.

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

#define XTERN
#include <common.h>
#undef XTERN
#define XTERN extern
#include <argmatch.h>
#include <backupfile.h>
#include <exitfail.h>
#include <getopt.h>
#include <inp.h>
#include <pch.h>
#include <quotearg.h>
#include <util.h>
#include <version.h>
#include <xalloc.h>

/* procedures */

static FILE *create_output_file (char const *, int);
static LINENUM locate_hunk (LINENUM);
static bool apply_hunk (struct outstate *, LINENUM);
static bool patch_match (LINENUM, LINENUM, LINENUM, LINENUM);
static bool spew_output (struct outstate *, struct stat *);
static char const *make_temp (char);
static int numeric_string (char const *, bool, char const *);
static void cleanup (void);
static void get_some_switches (void);
static void init_output (char const *, int, struct outstate *);
static void init_reject (void);
static void reinitialize_almost_everything (void);
static void remove_if_needed (char const *, int volatile *);
static void usage (FILE *, int) __attribute__((noreturn));

static void abort_hunk (bool, bool);
static void abort_hunk_context (bool, bool);
static void abort_hunk_unified (bool, bool);

#ifdef ENABLE_MERGE
static bool merge;
#else
# define merge false
#endif

static enum diff reject_format = NO_DIFF;  /* automatic */
static bool make_backups;
static bool backup_if_mismatch;
static char const *version_control;
static char const *version_control_context;
static bool remove_empty_files;
static bool explicit_inname;

/* true if -R was specified on command line.  */
static bool reverse_flag_specified;

static char const *do_defines; /* symbol to patch using ifdef, ifndef, etc. */
static char const if_defined[] = "\n#ifdef %s\n";
static char const not_defined[] = "\n#ifndef %s\n";
static char const else_defined[] = "\n#else\n";
static char const end_defined[] = "\n#endif\n";

static int Argc;
static char * const *Argv;

static FILE *rejfp;  /* reject file pointer */

static char const *patchname;
static char *rejname;
static char const * volatile TMPREJNAME;
static int volatile TMPREJNAME_needs_removal;

static LINENUM maxfuzz = 2;

static char serrbuf[BUFSIZ];

/* Apply a set of diffs as appropriate. */

int
main (int argc, char **argv)
{
    char const *val;
    bool somefailed = false;
    struct outstate outstate;
    struct stat outst;
    char numbuf[LINENUM_LENGTH_BOUND + 1];
    bool written_to_rejname = false;
    bool apply_empty_patch = false;

    exit_failure = 2;
    program_name = argv[0];
    init_time ();

    setbuf(stderr, serrbuf);

    bufsize = 8 * 1024;
    buf = xmalloc (bufsize);

    strippath = -1;

    val = getenv ("QUOTING_STYLE");
    {
      int i = val ? argmatch (val, quoting_style_args, 0, 0) : -1;
      set_quoting_style ((struct quoting_options *) 0,
			 i < 0 ? shell_quoting_style : (enum quoting_style) i);
    }

    posixly_correct = getenv ("POSIXLY_CORRECT") != 0;
    backup_if_mismatch = ! posixly_correct;
    patch_get = ((val = getenv ("PATCH_GET"))
		 ? numeric_string (val, true, "PATCH_GET value")
		 : 0);

    val = getenv ("SIMPLE_BACKUP_SUFFIX");
    simple_backup_suffix = val && *val ? val : ".orig";

    if ((version_control = getenv ("PATCH_VERSION_CONTROL")))
      version_control_context = "$PATCH_VERSION_CONTROL";
    else if ((version_control = getenv ("VERSION_CONTROL")))
      version_control_context = "$VERSION_CONTROL";

    /* Cons up the names of the global temporary files.
       Do this before `cleanup' can possibly be called (e.g. by `pfatal').  */
    TMPOUTNAME = make_temp ('o');
    TMPINNAME = make_temp ('i');
    TMPREJNAME = make_temp ('r');
    TMPPATNAME = make_temp ('p');

    /* parse switches */
    Argc = argc;
    Argv = argv;
    get_some_switches();

    if (make_backups | backup_if_mismatch)
      backup_type = get_version (version_control_context, version_control);

    init_backup_hash_table ();
    init_output (outfile, 0, &outstate);

    /* Make sure we clean up in case of disaster.  */
    set_signals (false);

    if (inname && outfile)
      {
	apply_empty_patch = true;
	inerrno = -1;
      }
    for (
	open_patch_file (patchname);
	there_is_another_patch (! (inname || posixly_correct))
	  || apply_empty_patch;
	reinitialize_almost_everything(),
	  apply_empty_patch = false
    ) {					/* for each patch in patch file */
      int hunk = 0;
      int failed = 0;
      bool mismatch = false;
      char *outname = NULL;

      if (! skip_rest_of_patch)
	{
	  outname = outfile ? outfile : inname;
	  get_input_file (inname, outname);
	}

      if (diff_type == ED_DIFF) {
	outstate.zero_output = false;
	somefailed |= skip_rest_of_patch;
	do_ed_script (outstate.ofp);
	if (! dry_run && ! outfile && ! skip_rest_of_patch)
	  {
	    struct stat statbuf;
	    if (stat (TMPOUTNAME, &statbuf) != 0)
	      pfatal ("%s", TMPOUTNAME);
	    outstate.zero_output = statbuf.st_size == 0;
	  }
      } else {
	int got_hunk;
	bool apply_anyway = merge;  /* don't try to reverse when merging */

	/* initialize the patched file */
	if (! skip_rest_of_patch && ! outfile)
	  {
	    int exclusive = TMPOUTNAME_needs_removal ? 0 : O_EXCL;
	    TMPOUTNAME_needs_removal = 1;
	    init_output (TMPOUTNAME, exclusive, &outstate);
	  }

	/* initialize reject file */
	init_reject ();

	/* find out where all the lines are */
	if (!skip_rest_of_patch)
	    scan_input (inname);

	/* from here on, open no standard i/o files, because malloc */
	/* might misfire and we can't catch it easily */

	/* apply each hunk of patch */
	while (0 < (got_hunk = another_hunk (diff_type, reverse)))
	  {
	    LINENUM where = 0; /* Pacify `gcc -Wall'.  */
	    LINENUM newwhere;
	    LINENUM fuzz = 0;
	    LINENUM mymaxfuzz;

	    if (merge)
	      {
		/* When in merge mode, don't apply with fuzz.  */
		mymaxfuzz = 0;
	      }
	    else
	      {
		LINENUM prefix_context = pch_prefix_context ();
		LINENUM suffix_context = pch_suffix_context ();
		LINENUM context = (prefix_context < suffix_context
				   ? suffix_context : prefix_context);
		mymaxfuzz = (maxfuzz < context ? maxfuzz : context);
	      }

	    hunk++;
	    if (!skip_rest_of_patch) {
		do {
		    where = locate_hunk(fuzz);
		    if (! where || fuzz || in_offset)
		      mismatch = true;
		    if (hunk == 1 && ! where && ! (force | apply_anyway)
			&& reverse == reverse_flag_specified) {
						/* dwim for reversed patch? */
			if (!pch_swap()) {
			    say (
"Not enough memory to try swapped hunk!  Assuming unswapped.\n");
			    continue;
			}
			/* Try again.  */
			where = locate_hunk (fuzz);
			if (where
			    && (ok_to_reverse
				("%s patch detected!",
				 (reverse
				  ? "Unreversed"
				  : "Reversed (or previously applied)"))))
			  reverse = ! reverse;
			else
			  {
			    /* Put it back to normal.  */
			    if (! pch_swap ())
			      fatal ("lost hunk on alloc error!");
			    if (where)
			      {
				apply_anyway = true;
				fuzz--; /* Undo `++fuzz' below.  */
				where = 0;
			      }
			  }
		    }
		} while (!skip_rest_of_patch && !where
			 && ++fuzz <= mymaxfuzz);

		if (skip_rest_of_patch) {		/* just got decided */
		  if (outstate.ofp && ! outfile)
		    {
		      fclose (outstate.ofp);
		      outstate.ofp = 0;
		    }
		}
	    }

	    newwhere = (where ? where : pch_first()) + out_offset;
	    if (skip_rest_of_patch
		|| (merge && ! merge_hunk (hunk, &outstate, where,
					   &somefailed))
		|| (! merge
		    && ((where == 1 && pch_says_nonexistent (reverse) == 2
			 && instat.st_size)
			|| ! where
			|| ! apply_hunk (&outstate, where))))
	      {
		abort_hunk (! failed, reverse);
		failed++;
		if (verbosity == VERBOSE ||
		    (! skip_rest_of_patch && verbosity != SILENT))
		  say ("Hunk #%d %s at %s.\n", hunk,
		       skip_rest_of_patch ? "ignored" : "FAILED",
		       format_linenum (numbuf, newwhere));
	      }
	    else if (! merge &&
		     (verbosity == VERBOSE
		      || (verbosity != SILENT && (fuzz || in_offset))))
	      {
		say ("Hunk #%d succeeded at %s", hunk,
		     format_linenum (numbuf, newwhere));
		if (fuzz)
		  say (" with fuzz %s", format_linenum (numbuf, fuzz));
		if (in_offset)
		  say (" (offset %s line%s)",
		       format_linenum (numbuf, in_offset),
		       "s" + (in_offset == 1));
		say (".\n");
	      }
	  }

	if (!skip_rest_of_patch)
	  {
	    if (got_hunk < 0  &&  using_plan_a)
	      {
		if (outfile)
		  fatal ("out of memory using Plan A");
		say ("\n\nRan out of memory using Plan A -- trying again...\n\n");
		if (outstate.ofp)
		  {
		    fclose (outstate.ofp);
		    outstate.ofp = 0;
		  }
		fclose (rejfp);
		continue;
	      }

	    /* Finish spewing out the new file.  */
	    if (! spew_output (&outstate, &outst))
	      {
		say ("Skipping patch.\n");
		skip_rest_of_patch = true;
	      }
	  }
      }

      /* and put the output where desired */
      ignore_signals ();
      if (! skip_rest_of_patch && ! outfile) {
	  bool backup = make_backups
			|| (backup_if_mismatch && (mismatch | failed));

	  if (outstate.zero_output
	      && (remove_empty_files
		  || (pch_says_nonexistent (! reverse) == 2
		      && ! posixly_correct)))
	    {
	      if (verbosity == VERBOSE)
		say ("Removing file %s%s\n", quotearg (outname),
		     dry_run ? " and any empty ancestor directories" : "");
	      if (! dry_run)
		{
		  move_file (0, 0, 0, outname, 0, backup);
		  removedirs (outname);
		}
	    }
	  else
	    {
	      if (! outstate.zero_output
		  && pch_says_nonexistent (! reverse)
		  && ! (merge && somefailed))
		{
		  mismatch = true;
		  if (verbosity != SILENT)
		    say ("File %s is not empty after patch, as expected\n",
			 quotearg (outname));
		}

	      if (! dry_run)
		{
		  /* Avoid replacing files when nothing has changed.  */
		  if (failed < hunk || diff_type == ED_DIFF)
		    {
		      time_t t;

		      move_file (TMPOUTNAME, &TMPOUTNAME_needs_removal, &outst,
				 outname, instat.st_mode, backup);

		      if ((set_time | set_utc)
			  && (t = pch_timestamp (! reverse)) != (time_t) -1)
			{
			  struct utimbuf utimbuf;
			  utimbuf.actime = utimbuf.modtime = t;

			  if (! force && ! inerrno
			      && pch_says_nonexistent (reverse) != 2
			      && (t = pch_timestamp (reverse)) != (time_t) -1
			      && t != instat.st_mtime)
			    say ("Not setting time of file %s "
				 "(time mismatch)\n",
				 quotearg (outname));
			  else if (! force && (mismatch | failed))
			    say ("Not setting time of file %s "
				 "(contents mismatch)\n",
				 quotearg (outname));
			  else if (utime (outname, &utimbuf) != 0)
			    pfatal ("Can't set timestamp on file %s",
				    quotearg (outname));
			}

		      if (! inerrno)
			{
			  if (chmod (outname, instat.st_mode) != 0)
			    pfatal ("Can't set permissions on file %s",
				    quotearg (outname));
			  if (geteuid () != instat.st_gid)
			    {
			      /* Fails if we are not in group instat.st_gid.  */
			      chown (outname, -1, instat.st_gid);
			    }
			  /* FIXME: There may be other attributes to preserve.  */
			}
		    }
		  else
		    create_backup (outname, 0, 0, true);
		}
	    }
      }
      if (diff_type != ED_DIFF) {
	struct stat rejst;

	if ((failed && fstat (fileno (rejfp), &rejst) != 0)
	    || fclose (rejfp) != 0)
	    write_fatal ();
	if (failed) {
	    somefailed = true;
	    say ("%d out of %d hunk%s %s", failed, hunk, "s" + (hunk == 1),
		 skip_rest_of_patch ? "ignored" : "FAILED");
	    if (outname && (! rejname || strcmp (rejname, "-") != 0)) {
		char *rej = rejname;
		if (!rejname) {
		    /* FIXME: This should really be done differnely!  */
		    const char *s = simple_backup_suffix;
		    size_t len;
		    simple_backup_suffix = ".rej";
		    rej = find_backup_file_name (outname, simple_backups);
		    len = strlen (rej);
		    if (rej[len - 1] == '~')
		      rej[len - 1] = '#';
		    simple_backup_suffix = s;
		}
		say (" -- saving rejects to file %s\n", quotearg (rej));
		if (! dry_run)
		  {
		    if (rejname)
		      {
			if (! written_to_rejname)
			  {
			    copy_file (TMPREJNAME, rejname, 0, 0, 0666, true);
			    written_to_rejname = true;
			  }
			else
			  append_to_file (TMPREJNAME, rejname);
		      }
		    else
		      {
			struct stat oldst;
			int olderrno;

			olderrno = stat (rej, &oldst) ? errno : 0;
			if (olderrno && olderrno != ENOENT)
			  write_fatal ();
		        if (! olderrno && file_already_seen (&oldst))
			  append_to_file (TMPREJNAME, rej);
			else
			  move_file (TMPREJNAME, &TMPREJNAME_needs_removal,
				     &rejst, rej, 0666, false);
		      }
		  }
		if (!rejname)
		    free (rej);
	    } else
	      say ("\n");
	}
      }
      set_signals (true);
    }
    if (outstate.ofp && (ferror (outstate.ofp) || fclose (outstate.ofp) != 0))
      write_fatal ();
    cleanup ();
    if (somefailed)
      exit (1);
    return 0;
}

/* Prepare to find the next patch to do in the patch file. */

static void
reinitialize_almost_everything (void)
{
    re_patch();
    re_input();

    input_lines = 0;
    last_frozen_line = 0;

    if (inname && ! explicit_inname) {
	free (inname);
	inname = 0;
    }

    in_offset = 0;
    out_offset = 0;

    diff_type = NO_DIFF;

    if (revision) {
	free(revision);
	revision = 0;
    }

    reverse = reverse_flag_specified;
    skip_rest_of_patch = false;
}

static char const shortopts[] = "bB:cd:D:eEfF:g:i:l"
#if 0 && defined(ENABLE_MERGE)
				"m"
#endif
				"nNo:p:r:RstTuvV:x:Y:z:Z";

static struct option const longopts[] =
{
  {"backup", no_argument, NULL, 'b'},
  {"prefix", required_argument, NULL, 'B'},
  {"context", no_argument, NULL, 'c'},
  {"directory", required_argument, NULL, 'd'},
  {"ifdef", required_argument, NULL, 'D'},
  {"ed", no_argument, NULL, 'e'},
  {"remove-empty-files", no_argument, NULL, 'E'},
  {"force", no_argument, NULL, 'f'},
  {"fuzz", required_argument, NULL, 'F'},
  {"get", no_argument, NULL, 'g'},
  {"input", required_argument, NULL, 'i'},
  {"ignore-whitespace", no_argument, NULL, 'l'},
#ifdef ENABLE_MERGE
  {"merge", optional_argument, NULL, 'm'},
#endif
  {"normal", no_argument, NULL, 'n'},
  {"forward", no_argument, NULL, 'N'},
  {"output", required_argument, NULL, 'o'},
  {"strip", required_argument, NULL, 'p'},
  {"reject-file", required_argument, NULL, 'r'},
  {"reverse", no_argument, NULL, 'R'},
  {"quiet", no_argument, NULL, 's'},
  {"silent", no_argument, NULL, 's'},
  {"batch", no_argument, NULL, 't'},
  {"set-time", no_argument, NULL, 'T'},
  {"unified", no_argument, NULL, 'u'},
  {"version", no_argument, NULL, 'v'},
  {"version-control", required_argument, NULL, 'V'},
  {"debug", required_argument, NULL, 'x'},
  {"basename-prefix", required_argument, NULL, 'Y'},
  {"suffix", required_argument, NULL, 'z'},
  {"set-utc", no_argument, NULL, 'Z'},
  {"dry-run", no_argument, NULL, CHAR_MAX + 1},
  {"verbose", no_argument, NULL, CHAR_MAX + 2},
  {"binary", no_argument, NULL, CHAR_MAX + 3},
  {"help", no_argument, NULL, CHAR_MAX + 4},
  {"backup-if-mismatch", no_argument, NULL, CHAR_MAX + 5},
  {"no-backup-if-mismatch", no_argument, NULL, CHAR_MAX + 6},
  {"posix", no_argument, NULL, CHAR_MAX + 7},
  {"quoting-style", required_argument, NULL, CHAR_MAX + 8},
  {"reject-format", required_argument, NULL, CHAR_MAX + 9},
  {NULL, no_argument, NULL, 0}
};

static char const *const option_help[] =
{
"Input options:",
"",
"  -p NUM  --strip=NUM  Strip NUM leading components from file names.",
"  -F LINES  --fuzz LINES  Set the fuzz factor to LINES for inexact matching.",
"  -l  --ignore-whitespace  Ignore white space changes between patch and input.",
"",
"  -c  --context  Interpret the patch as a context difference.",
"  -e  --ed  Interpret the patch as an ed script.",
"  -n  --normal  Interpret the patch as a normal difference.",
"  -u  --unified  Interpret the patch as a unified difference.",
"",
"  -N  --forward  Ignore patches that appear to be reversed or already applied.",
"  -R  --reverse  Assume patches were created with old and new files swapped.",
"",
"  -i PATCHFILE  --input=PATCHFILE  Read patch from PATCHFILE instead of stdin.",
"",
"Output options:",
"",
"  -o FILE  --output=FILE  Output patched files to FILE.",
"  -r FILE  --reject-file=FILE  Output rejects to FILE.",
"",
"  -D NAME  --ifdef=NAME  Make merged if-then-else output using NAME.",
#ifdef ENABLE_MERGE
"  -m  --merge  Merge using conflict markers instead of creating reject files.",
#endif
"  -E  --remove-empty-files  Remove output files that are empty after patching.",
"",
"  -Z  --set-utc  Set times of patched files, assuming diff uses UTC (GMT).",
"  -T  --set-time  Likewise, assuming local time.",
"",
"  --quoting-style=WORD   output file names using quoting style WORD.",
"    Valid WORDs are: literal, shell, shell-always, c, escape.",
"    Default is taken from QUOTING_STYLE env variable, or 'shell' if unset.",
"",
"Backup and version control options:",
"",
"  -b  --backup  Back up the original contents of each file.",
"  --backup-if-mismatch  Back up if the patch does not match exactly.",
"  --no-backup-if-mismatch  Back up mismatches only if otherwise requested.",
"",
"  -V STYLE  --version-control=STYLE  Use STYLE version control.",
"	STYLE is either 'simple', 'numbered', or 'existing'.",
"  -B PREFIX  --prefix=PREFIX  Prepend PREFIX to backup file names.",
"  -Y PREFIX  --basename-prefix=PREFIX  Prepend PREFIX to backup file basenames.",
"  -z SUFFIX  --suffix=SUFFIX  Append SUFFIX to backup file names.",
"",
"  -g NUM  --get=NUM  Get files from RCS etc. if positive; ask if negative.",
"",
"Miscellaneous options:",
"",
"  -t  --batch  Ask no questions; skip bad-Prereq patches; assume reversed.",
"  -f  --force  Like -t, but ignore bad-Prereq patches, and assume unreversed.",
"  -s  --quiet  --silent  Work silently unless an error occurs.",
"  --verbose  Output extra information about the work being done.",
"  --dry-run  Do not actually change any files; just print what would happen.",
"  --posix  Conform to the POSIX standard.",
"",
"  -d DIR  --directory=DIR  Change the working directory to DIR first.",
"  --reject-format=FORMAT  Create 'context' or 'unified' rejects.",
"  --binary  Read and write data in binary mode.",
"",
"  -v  --version  Output version info.",
"  --help  Output this help.",
"",
"Report bugs to <" PACKAGE_BUGREPORT ">.",
0
};

static void
usage (FILE *stream, int status)
{
  char const * const *p;

  if (status != 0)
    {
      fprintf (stream, "%s: Try `%s --help' for more information.\n",
	       program_name, Argv[0]);
    }
  else
    {
      fprintf (stream, "Usage: %s [OPTION]... [ORIGFILE [PATCHFILE]]\n\n",
	       Argv[0]);
      for (p = option_help;  *p;  p++)
	fprintf (stream, "%s\n", *p);
    }

  exit (status);
}

/* Process switches and filenames.  */

static void
get_some_switches (void)
{
    register int optc;

    if (rejname)
	free (rejname);
    rejname = 0;
    if (optind == Argc)
	return;
    while ((optc = getopt_long (Argc, Argv, shortopts, longopts, (int *) 0))
	   != -1) {
	switch (optc) {
	    case 'b':
		make_backups = true;
		 /* Special hack for backward compatibility with CVS 1.9.
		    If the last 4 args are `-b SUFFIX ORIGFILE PATCHFILE',
		    treat `-b' as if it were `-b -z'.  */
		if (Argc - optind == 3
		    && strcmp (Argv[optind - 1], "-b") == 0
		    && ! (Argv[optind + 0][0] == '-' && Argv[optind + 0][1])
		    && ! (Argv[optind + 1][0] == '-' && Argv[optind + 1][1])
		    && ! (Argv[optind + 2][0] == '-' && Argv[optind + 2][1]))
		  {
		    optarg = Argv[optind++];
		    if (verbosity != SILENT)
		      say ("warning: the `-b %s' option is obsolete; use `-b -z %s' instead\n",
			   optarg, optarg);
		    goto case_z;
		  }
		break;
	    case 'B':
		if (!*optarg)
		  fatal ("backup prefix is empty");
		origprae = savestr (optarg);
		break;
	    case 'c':
		diff_type = CONTEXT_DIFF;
		break;
	    case 'd':
		if (chdir(optarg) < 0)
		  pfatal ("Can't change to directory %s", quotearg (optarg));
		break;
	    case 'D':
		do_defines = savestr (optarg);
		break;
	    case 'e':
		diff_type = ED_DIFF;
		break;
	    case 'E':
		remove_empty_files = true;
		break;
	    case 'f':
		force = true;
		break;
	    case 'F':
		maxfuzz = numeric_string (optarg, false, "fuzz factor");
		break;
	    case 'g':
		patch_get = numeric_string (optarg, true, "get option value");
		break;
	    case 'i':
		patchname = savestr (optarg);
		break;
	    case 'l':
		canonicalize = true;
		break;
#ifdef ENABLE_MERGE
	    case 'm':
		merge = true;
		if (optarg)
		  {
		    if (! strcmp (optarg, "merge"))
		      conflict_style = MERGE_MERGE;
		    else if (! strcmp (optarg, "diff3"))
		      conflict_style = MERGE_DIFF3;
		    else
		      usage (stderr, 2);
		  }
		else
		  conflict_style = MERGE_MERGE;
		break;
#endif
	    case 'n':
		diff_type = NORMAL_DIFF;
		break;
	    case 'N':
		noreverse = true;
		break;
	    case 'o':
		outfile = savestr (optarg);
		break;
	    case 'p':
		strippath = numeric_string (optarg, false, "strip count");
		break;
	    case 'r':
		rejname = savestr (optarg);
		break;
	    case 'R':
		reverse = true;
		reverse_flag_specified = true;
		break;
	    case 's':
		verbosity = SILENT;
		break;
	    case 't':
		batch = true;
		break;
	    case 'T':
		set_time = true;
		break;
	    case 'u':
		diff_type = UNI_DIFF;
		break;
	    case 'v':
		version();
		exit (0);
		break;
	    case 'V':
		version_control = optarg;
		version_control_context = "--version-control or -V option";
		break;
#if DEBUGGING
	    case 'x':
		debug = numeric_string (optarg, true, "debugging option");
		break;
#endif
	    case 'Y':
		if (!*optarg)
		  fatal ("backup basename prefix is empty");
		origbase = savestr (optarg);
		break;
	    case 'z':
	    case_z:
		if (!*optarg)
		  fatal ("backup suffix is empty");
		origsuff = savestr (optarg);
		break;
	    case 'Z':
		set_utc = true;
		break;
	    case CHAR_MAX + 1:
		dry_run = true;
		break;
	    case CHAR_MAX + 2:
		verbosity = VERBOSE;
		break;
	    case CHAR_MAX + 3:
		no_strip_trailing_cr = true;
#if HAVE_SETMODE_DOS
		binary_transput = O_BINARY;
#endif
		break;
	    case CHAR_MAX + 4:
		usage (stdout, 0);
	    case CHAR_MAX + 5:
		backup_if_mismatch = true;
		break;
	    case CHAR_MAX + 6:
		backup_if_mismatch = false;
		break;
	    case CHAR_MAX + 7:
		posixly_correct = true;
		break;
	    case CHAR_MAX + 8:
		{
		  int i = argmatch (optarg, quoting_style_args, 0, 0);
		  if (i < 0)
		    {
		      invalid_arg ("quoting style", optarg, i);
		      usage (stderr, 2);
		    }
		  set_quoting_style ((struct quoting_options *) 0,
				     (enum quoting_style) i);
		}
		break;
	    case CHAR_MAX + 9:
		if (strcmp (optarg, "context") == 0)
		  reject_format = NEW_CONTEXT_DIFF;
		else if (strcmp (optarg, "unified") == 0)
		  reject_format = UNI_DIFF;
		else
		  usage (stderr, 2);
		break;
	    default:
		usage (stderr, 2);
	}
    }

    /* Process any filename args.  */
    if (optind < Argc)
      {
	inname = savestr (Argv[optind++]);
	explicit_inname = true;
	invc = -1;
	if (optind < Argc)
	  {
	    patchname = savestr (Argv[optind++]);
	    if (optind < Argc)
	      {
		fprintf (stderr, "%s: %s: extra operand\n",
			 program_name, quotearg (Argv[optind]));
		usage (stderr, 2);
	      }
	  }
      }
}

/* Handle STRING (possibly negative if NEGATIVE_ALLOWED is nonzero)
   of type ARGTYPE_MSGID by converting it to an integer,
   returning the result.  */
static int
numeric_string (char const *string,
		bool negative_allowed,
		char const *argtype_msgid)
{
  int value = 0;
  char const *p = string;
  int sign = *p == '-' ? -1 : 1;

  p += *p == '-' || *p == '+';

  do
    {
      int v10 = value * 10;
      int digit = *p - '0';
      int signed_digit = sign * digit;
      int next_value = v10 + signed_digit;

      if (9 < (unsigned) digit)
	fatal ("%s %s is not a number", argtype_msgid, quotearg (string));

      if (v10 / 10 != value || (next_value < v10) != (signed_digit < 0))
	fatal ("%s %s is too large", argtype_msgid, quotearg (string));

      value = next_value;
    }
  while (*++p);

  if (value < 0 && ! negative_allowed)
    fatal ("%s %s is negative", argtype_msgid, quotearg (string));

  return value;
}

/* Attempt to find the right place to apply this hunk of patch. */

static LINENUM
locate_hunk (LINENUM fuzz)
{
    register LINENUM first_guess = pch_first () + in_offset;
    register LINENUM offset;
    LINENUM pat_lines = pch_ptrn_lines();
    LINENUM prefix_context = pch_prefix_context ();
    LINENUM suffix_context = pch_suffix_context ();
    LINENUM context = (prefix_context < suffix_context
		       ? suffix_context : prefix_context);
    LINENUM prefix_fuzz = fuzz + prefix_context - context;
    LINENUM suffix_fuzz = fuzz + suffix_context - context;
    LINENUM max_where = input_lines - (pat_lines - suffix_fuzz) + 1;
    LINENUM min_where = last_frozen_line + 1 - (prefix_context - prefix_fuzz);
    LINENUM max_pos_offset = max_where - first_guess;
    LINENUM max_neg_offset = first_guess - min_where;
    LINENUM max_offset = (max_pos_offset < max_neg_offset
			  ? max_neg_offset : max_pos_offset);

    if (!pat_lines)			/* null range matches always */
	return first_guess;

    /* Do not try lines <= 0.  */
    if (first_guess <= max_neg_offset)
	max_neg_offset = first_guess - 1;

    if (prefix_fuzz < 0 && pch_first () <= 1)
      {
	/* Can only match start of file.  */

	if (suffix_fuzz < 0)
	  /* Can only match entire file.  */
	  if (pat_lines != input_lines || prefix_context < last_frozen_line)
	    return 0;

	offset = 1 - first_guess;
	if (last_frozen_line <= prefix_context
	    && offset <= max_pos_offset
	    && patch_match (first_guess, offset, (LINENUM) 0, suffix_fuzz))
	  {
	    in_offset += offset;
	    return first_guess + offset;
	  }
	else
	  return 0;
      }
    else if (prefix_fuzz < 0)
      prefix_fuzz = 0;

    if (suffix_fuzz < 0)
      {
	/* Can only match end of file.  */
	offset = first_guess - (input_lines - pat_lines + 1);
	if (offset <= max_neg_offset
	    && patch_match (first_guess, -offset, prefix_fuzz, (LINENUM) 0))
	  {
	    in_offset -= offset;
	    return first_guess - offset;
	  }
	else
	  return 0;
      }

    for (offset = 0;  offset <= max_offset;  offset++) {
	char numbuf0[LINENUM_LENGTH_BOUND + 1];
	char numbuf1[LINENUM_LENGTH_BOUND + 1];
	if (offset <= max_pos_offset
	    && patch_match (first_guess, offset, prefix_fuzz, suffix_fuzz)) {
	    if (debug & 1)
	      say ("Offset changing from %s to %s\n",
		   format_linenum (numbuf0, in_offset),
		   format_linenum (numbuf1, in_offset + offset));
	    in_offset += offset;
	    return first_guess+offset;
	}
	if (0 < offset && offset <= max_neg_offset
	    && patch_match (first_guess, -offset, prefix_fuzz, suffix_fuzz)) {
	    if (debug & 1)
	      say ("Offset changing from %s to %s\n",
		   format_linenum (numbuf0, in_offset),
		   format_linenum (numbuf1, in_offset - offset));
	    in_offset -= offset;
	    return first_guess-offset;
	}
    }
    return 0;
}

static void
mangled_patch (LINENUM old, LINENUM new)
{
  char numbuf0[LINENUM_LENGTH_BOUND + 1];
  char numbuf1[LINENUM_LENGTH_BOUND + 1];
  if (debug & 1)
    say ("oldchar = '%c', newchar = '%c'\n",
        pch_char (old), pch_char (new));
  fatal ("Out-of-sync patch, lines %s,%s -- mangled text or line numbers, "
        "maybe?",
        format_linenum (numbuf0, pch_hunk_beg () + old),
        format_linenum (numbuf1, pch_hunk_beg () + new));
}

/* Output a line number range in unified format.  */

static void
print_unidiff_range (FILE *fp, LINENUM start, LINENUM count)
{
  char numbuf0[LINENUM_LENGTH_BOUND + 1];
  char numbuf1[LINENUM_LENGTH_BOUND + 1];

  switch (count)
    {
    case 0:
      fprintf (fp, "%s,0", format_linenum (numbuf0, start - 1));
      break;

    case 1:
      fprintf (fp, "%s", format_linenum (numbuf0, start));
      break;

    default:
      fprintf (fp, "%s,%s",
              format_linenum (numbuf0, start),
              format_linenum (numbuf1, count));
      break;
    }
}

static void
print_header_line (FILE *fp, const char *tag, bool reverse)
{
  const char *name = pch_name (reverse);
  const char *timestr = pch_timestr (reverse);

  /* FIXME: include timestamp as well. */
  fprintf (fp, "%s %s%s\n", tag, name ? name : "/dev/null",
	   timestr ? timestr : "");
}

/* Produce unified reject files */

static void
abort_hunk_unified (bool header, bool reverse)
{
  FILE *fp = rejfp;
  register LINENUM old = 1;
  register LINENUM lastline = pch_ptrn_lines ();
  register LINENUM new = lastline + 1;

  if (header)
    {
      if (pch_name (INDEX))
	fprintf(fp, "Index: %s\n", pch_name (INDEX));
      print_header_line (rejfp, "---", reverse);
      print_header_line (rejfp, "+++", ! reverse);
    }

  /* Add out_offset to guess the same as the previous successful hunk.  */
  fprintf (fp, "@@ -");
  print_unidiff_range (fp, pch_first () + out_offset, lastline);
  fprintf (fp, " +");
  print_unidiff_range (fp, pch_newfirst () + out_offset, pch_repl_lines ());
  fprintf (fp, " @@\n");

  while (pch_char (new) == '=' || pch_char (new) == '\n')
    new++;

  if (diff_type != UNI_DIFF)
    pch_normalize (UNI_DIFF);

  for (; ; old++, new++)
    {
      for (;  pch_char (old) == '-';  old++)
	{
	  fputc ('-', fp);
	  pch_write_line (old, fp);
	}
      for (;  pch_char (new) == '+';  new++)
	{
	  fputc ('+', fp);
	  pch_write_line (new, fp);
	}

      if (old > lastline)
	  break;

      if (pch_char (new) != pch_char (old))
	mangled_patch (old, new);

      fputc (' ', fp);
      pch_write_line (old, fp);
    }
  if (pch_char (new) != '^')
    mangled_patch (old, new);
}

/* Output the rejected patch in context format.  */

static void
abort_hunk_context (bool header, bool reverse)
{
    register LINENUM i;
    register LINENUM pat_end = pch_end ();
    /* add in out_offset to guess the same as the previous successful hunk */
    LINENUM oldfirst = pch_first() + out_offset;
    LINENUM newfirst = pch_newfirst() + out_offset;
    LINENUM oldlast = oldfirst + pch_ptrn_lines() - 1;
    LINENUM newlast = newfirst + pch_repl_lines() - 1;
    char const *stars =
      (int) NEW_CONTEXT_DIFF <= (int) diff_type ? " ****" : "";
    char const *minuses =
      (int) NEW_CONTEXT_DIFF <= (int) diff_type ? " ----" : " -----";
    char const *c_function = pch_c_function();

    if (diff_type == UNI_DIFF)
      pch_normalize (NEW_CONTEXT_DIFF);

    if (header)
      {
	if (pch_name (INDEX))
	  fprintf(rejfp, "Index: %s\n", pch_name (INDEX));
	print_header_line (rejfp, "***", reverse);
	print_header_line (rejfp, "---", ! reverse);
      }
    fprintf(rejfp, "***************%s\n", c_function ? c_function : "");
    for (i=0; i<=pat_end; i++) {
	char numbuf0[LINENUM_LENGTH_BOUND + 1];
	char numbuf1[LINENUM_LENGTH_BOUND + 1];
	switch (pch_char(i)) {
	case '*':
	    if (oldlast < oldfirst)
		fprintf(rejfp, "*** 0%s\n", stars);
	    else if (oldlast == oldfirst)
		fprintf (rejfp, "*** %s%s\n",
			 format_linenum (numbuf0, oldfirst), stars);
	    else
		fprintf (rejfp, "*** %s,%s%s\n",
			 format_linenum (numbuf0, oldfirst),
			 format_linenum (numbuf1, oldlast), stars);
	    break;
	case '=':
	    if (newlast < newfirst)
		fprintf(rejfp, "--- 0%s\n", minuses);
	    else if (newlast == newfirst)
		fprintf (rejfp, "--- %s%s\n",
			 format_linenum (numbuf0, newfirst), minuses);
	    else
		fprintf (rejfp, "--- %s,%s%s\n",
			 format_linenum (numbuf0, newfirst),
			 format_linenum (numbuf1, newlast), minuses);
	    break;
	case ' ': case '-': case '+': case '!':
	    fprintf (rejfp, "%c ", pch_char (i));
	    /* fall into */
	case '\n':
	    pch_write_line (i, rejfp);
	    break;
	default:
	    fatal ("fatal internal error in abort_hunk_context");
	}
	if (ferror (rejfp))
	  write_fatal ();
    }
}

/* Output the rejected hunk.  */

static void
abort_hunk (bool header, bool reverse)
{
  if (reject_format == UNI_DIFF
      || (reject_format == NO_DIFF && diff_type == UNI_DIFF))
    abort_hunk_unified (header, reverse);
  else
    abort_hunk_context (header, reverse);
}

/* We found where to apply it (we hope), so do it. */

static bool
apply_hunk (struct outstate *outstate, LINENUM where)
{
    register LINENUM old = 1;
    register LINENUM lastline = pch_ptrn_lines ();
    register LINENUM new = lastline+1;
    register enum {OUTSIDE, IN_IFNDEF, IN_IFDEF, IN_ELSE} def_state = OUTSIDE;
    register char const *R_do_defines = do_defines;
    register LINENUM pat_end = pch_end ();
    register FILE *fp = outstate->ofp;

    where--;
    while (pch_char(new) == '=' || pch_char(new) == '\n')
	new++;

    while (old <= lastline) {
	if (pch_char(old) == '-') {
	    assert (outstate->after_newline);
	    if (! copy_till (outstate, where + old - 1))
		return false;
	    if (R_do_defines) {
		if (def_state == OUTSIDE) {
		    fprintf (fp, outstate->after_newline + not_defined,
			     R_do_defines);
		    def_state = IN_IFNDEF;
		}
		else if (def_state == IN_IFDEF) {
		    fputs (outstate->after_newline + else_defined, fp);
		    def_state = IN_ELSE;
		}
		if (ferror (fp))
		  write_fatal ();
		outstate->after_newline = pch_write_line (old, fp);
		outstate->zero_output = false;
	    }
	    last_frozen_line++;
	    old++;
	}
	else if (new > pat_end) {
	    break;
	}
	else if (pch_char(new) == '+') {
	    if (! copy_till (outstate, where + old - 1))
		return false;
	    if (R_do_defines) {
		if (def_state == IN_IFNDEF) {
		    fputs (outstate->after_newline + else_defined, fp);
		    def_state = IN_ELSE;
		}
		else if (def_state == OUTSIDE) {
		    fprintf (fp, outstate->after_newline + if_defined,
			     R_do_defines);
		    def_state = IN_IFDEF;
		}
		if (ferror (fp))
		  write_fatal ();
	    }
	    outstate->after_newline = pch_write_line (new, fp);
	    outstate->zero_output = false;
	    new++;
	}
	else if (pch_char(new) != pch_char(old))
	  mangled_patch (old, new);
	else if (pch_char(new) == '!') {
	    assert (outstate->after_newline);
	    if (! copy_till (outstate, where + old - 1))
		return false;
	    assert (outstate->after_newline);
	    if (R_do_defines) {
	       fprintf (fp, 1 + not_defined, R_do_defines);
	       if (ferror (fp))
		write_fatal ();
	       def_state = IN_IFNDEF;
	    }

	    do
	      {
		if (R_do_defines) {
		    outstate->after_newline = pch_write_line (old, fp);
		}
		last_frozen_line++;
		old++;
	      }
	    while (pch_char (old) == '!');

	    if (R_do_defines) {
		fputs (outstate->after_newline + else_defined, fp);
		if (ferror (fp))
		  write_fatal ();
		def_state = IN_ELSE;
	    }

	    do
	      {
		outstate->after_newline = pch_write_line (new, fp);
		new++;
	      }
	    while (pch_char (new) == '!');
	    outstate->zero_output = false;
	}
	else {
	    assert(pch_char(new) == ' ');
	    old++;
	    new++;
	    if (R_do_defines && def_state != OUTSIDE) {
		fputs (outstate->after_newline + end_defined, fp);
		if (ferror (fp))
		  write_fatal ();
		outstate->after_newline = true;
		def_state = OUTSIDE;
	    }
	}
    }
    if (new <= pat_end && pch_char(new) == '+') {
	if (! copy_till (outstate, where + old - 1))
	    return false;
	if (R_do_defines) {
	    if (def_state == OUTSIDE) {
		fprintf (fp, outstate->after_newline + if_defined,
			 R_do_defines);
		def_state = IN_IFDEF;
	    }
	    else if (def_state == IN_IFNDEF) {
		fputs (outstate->after_newline + else_defined, fp);
		def_state = IN_ELSE;
	    }
	    if (ferror (fp))
	      write_fatal ();
	    outstate->zero_output = false;
	}

	do
	  {
	    if (! outstate->after_newline  &&  putc ('\n', fp) == EOF)
	      write_fatal ();
	    outstate->after_newline = pch_write_line (new, fp);
	    outstate->zero_output = false;
	    new++;
	  }
	while (new <= pat_end && pch_char (new) == '+');
    }
    if (R_do_defines && def_state != OUTSIDE) {
	fputs (outstate->after_newline + end_defined, fp);
	if (ferror (fp))
	  write_fatal ();
	outstate->after_newline = true;
    }
    out_offset += pch_repl_lines() - pch_ptrn_lines ();
    return true;
}

/* Create an output file.  */

static FILE *
create_output_file (char const *name, int open_flags)
{
  int fd = create_file (name, O_WRONLY | binary_transput | open_flags,
			instat.st_mode, true);
  FILE *f = fdopen (fd, binary_transput ? "wb" : "w");
  if (! f)
    pfatal ("Can't create file %s", quotearg (name));
  return f;
}

/* Open the new file. */

static void
init_output (char const *name, int open_flags, struct outstate *outstate)
{
  if (! name)
    outstate->ofp = (FILE *) 0;
  else if (strcmp (name, "-") != 0)
    outstate->ofp = create_output_file (name, open_flags);
  else
    {
      int stdout_dup = dup (fileno (stdout));
      outstate->ofp = fdopen (stdout_dup, "a");
      if (stdout_dup == -1 || ! outstate->ofp)
	pfatal ("Failed to duplicate standard output");
      if (dup2 (fileno (stderr), fileno (stdout)) == -1)
	pfatal ("Failed to redirect messages to standard error");
    }

  outstate->after_newline = true;
  outstate->zero_output = true;
}

/* Open a file to put hunks we can't locate. */

static void
init_reject (void)
{
  int exclusive = TMPREJNAME_needs_removal ? 0 : O_EXCL;
  TMPREJNAME_needs_removal = 1;
  rejfp = create_output_file (TMPREJNAME, exclusive);
}

/* Copy input file to output, up to wherever hunk is to be applied. */

bool
copy_till (register struct outstate *outstate, register LINENUM lastline)
{
    register LINENUM R_last_frozen_line = last_frozen_line;
    register FILE *fp = outstate->ofp;
    register char const *s;
    size_t size;

    if (R_last_frozen_line > lastline)
      {
	say ("misordered hunks! output would be garbled\n");
	return false;
      }
    while (R_last_frozen_line < lastline)
      {
	s = ifetch (++R_last_frozen_line, false, &size);
	if (size)
	  {
	    if ((! outstate->after_newline  &&  putc ('\n', fp) == EOF)
		|| ! fwrite (s, sizeof *s, size, fp))
	      write_fatal ();
	    outstate->after_newline = s[size - 1] == '\n';
	    outstate->zero_output = false;
	  }
      }
    last_frozen_line = R_last_frozen_line;
    return true;
}

/* Finish copying the input file to the output file. */

static bool
spew_output (struct outstate *outstate, struct stat *st)
{
    if (debug & 256)
      {
	char numbuf0[LINENUM_LENGTH_BOUND + 1];
	char numbuf1[LINENUM_LENGTH_BOUND + 1];
	say ("il=%s lfl=%s\n",
	     format_linenum (numbuf0, input_lines),
	     format_linenum (numbuf1, last_frozen_line));
      }

    if (last_frozen_line < input_lines)
      if (! copy_till (outstate, input_lines))
	return false;

    if (outstate->ofp && ! outfile)
      {
	if (fflush (outstate->ofp) != 0
	    || fstat (fileno (outstate->ofp), st) != 0
	    || fclose (outstate->ofp) != 0)
	  write_fatal ();
	outstate->ofp = 0;
      }

    return true;
}

/* Does the patch pattern match at line base+offset? */

static bool
patch_match (LINENUM base, LINENUM offset,
	     LINENUM prefix_fuzz, LINENUM suffix_fuzz)
{
    register LINENUM pline = 1 + prefix_fuzz;
    register LINENUM iline;
    register LINENUM pat_lines = pch_ptrn_lines () - suffix_fuzz;
    size_t size;
    register char const *p;

    for (iline=base+offset+prefix_fuzz; pline <= pat_lines; pline++,iline++) {
	p = ifetch (iline, offset >= 0, &size);
	if (canonicalize) {
	    if (!similar(p, size,
			 pfetch(pline),
			 pch_line_len(pline) ))
		return false;
	}
	else if (size != pch_line_len (pline)
		 || memcmp (p, pfetch (pline), size) != 0)
	    return false;
    }
    return true;
}

/* Do two lines match with canonicalized white space? */

bool
similar (register char const *a, register size_t alen,
	 register char const *b, register size_t blen)
{
  /* Ignore presence or absence of trailing newlines.  */
  alen  -=  alen && a[alen - 1] == '\n';
  blen  -=  blen && b[blen - 1] == '\n';

  for (;;)
    {
      if (!blen || (*b == ' ' || *b == '\t'))
	{
	  while (blen && (*b == ' ' || *b == '\t'))
	    b++, blen--;
	  if (alen)
	    {
	      if (!(*a == ' ' || *a == '\t'))
		return false;
	      do a++, alen--;
	      while (alen && (*a == ' ' || *a == '\t'));
	    }
	  if (!alen || !blen)
	    return alen == blen;
	}
      else if (!alen || *a++ != *b++)
	return false;
      else
	alen--, blen--;
    }
}

/* Make a temporary file.  */

#if HAVE_MKTEMP && ! HAVE_DECL_MKTEMP && ! defined mktemp
char *mktemp (char *);
#endif

#ifndef TMPDIR
#define TMPDIR "/tmp"
#endif

static char const *
make_temp (char letter)
{
  char *r;
#if HAVE_MKTEMP
  char const *tmpdir = getenv ("TMPDIR");	/* Unix tradition */
  if (!tmpdir) tmpdir = getenv ("TMP");		/* DOS tradition */
  if (!tmpdir) tmpdir = getenv ("TEMP");	/* another DOS tradition */
  if (!tmpdir) tmpdir = TMPDIR;
  r = xmalloc (strlen (tmpdir) + 10);
  sprintf (r, "%s/p%cXXXXXX", tmpdir, letter);

  /* It is OK to use mktemp here, since the rest of the code always
     opens temp files with O_EXCL.  It might be better to use mkstemp
     to avoid some DoS problems, but simply substituting mkstemp for
     mktemp here will not fix the DoS problems; a more extensive
     change would be needed.  */
  mktemp (r);

  if (!*r)
    pfatal ("mktemp");
#else
  r = xmalloc (L_tmpnam);
  if (! (tmpnam (r) == r && *r))
    pfatal ("tmpnam");
#endif
  return r;
}

/* Fatal exit with cleanup. */

void
fatal_exit (int sig)
{
  cleanup ();

  if (sig)
    exit_with_signal (sig);

  exit (2);
}

static void
remove_if_needed (char const *name, int volatile *needs_removal)
{
  if (*needs_removal)
    {
      unlink (name);
      *needs_removal = 0;
    }
}

static void
cleanup (void)
{
  remove_if_needed (TMPINNAME, &TMPINNAME_needs_removal);
  remove_if_needed (TMPOUTNAME, &TMPOUTNAME_needs_removal);
  remove_if_needed (TMPPATNAME, &TMPPATNAME_needs_removal);
  remove_if_needed (TMPREJNAME, &TMPREJNAME_needs_removal);
}

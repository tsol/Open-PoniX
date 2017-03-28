/*
 *  log.c - ACPI daemon logging
 *
 *  Portions Copyright (C) 2000 Andrew Henroid
 *  Portions Copyright (C) 2001 Sun Microsystems
 *  Portions Copyright (C) 2004 Tim Hockin (thockin@hockin.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>

#include "log.h"

int log_debug_to_stderr = 0;

int
#ifdef __GNUC__
__attribute__((format(printf, 2, 3)))
#endif
acpid_log(int level, const char *fmt, ...)
{
	va_list args;

	if (level == LOG_DEBUG) {
		/* if "-d" has been specified */
		if (log_debug_to_stderr) {
			/* send debug output to stderr */
			va_start(args, fmt);
			vfprintf(stderr, fmt, args);
			va_end(args);

            fprintf(stderr, "\n");
		}
	} else {
		va_start(args, fmt);
		vsyslog(level, fmt, args);
		va_end(args);
	}

	return 0;
}

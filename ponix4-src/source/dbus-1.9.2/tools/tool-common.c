/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* tool-common - common functionality for dbus-test-tool modules
 *
 * Copyright © 2003 Philip Blundell <philb@gnu.org>
 * Copyright © 2011 Nokia Corporation
 * Copyright © 2014 Collabora Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <config.h>
#include "tool-common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef DBUS_WIN
#include <windows.h>
#endif

/* a hack to avoid having to depend on the static -util version of libdbus;
 * it's useful for ancillary programs to be able to use the shared library */
void
tool_millisleep (int ms)
{
#ifdef DBUS_WIN
  Sleep (ms);
#else
  fd_set nothing;
  struct timeval tv;

  tv.tv_sec = ms / 1000;
  tv.tv_usec = (ms % 1000) * 1000;

  FD_ZERO (&nothing);
  select (1, &nothing, &nothing, &nothing, &tv);
#endif
}

void
tool_oom (const char *doing)
{
  fprintf (stderr, "OOM while %s\n", doing);
  exit (1);
}

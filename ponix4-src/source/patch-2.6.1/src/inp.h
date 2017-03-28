/* inputting files to be patched */

/* Copyright (C) 1986, 1988 Larry Wall
   Copyright (C) 1991, 1992, 1993, 1997, 1998, 1999, 2002, 2003, 2009
   Free Software Foundation, Inc.

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

XTERN LINENUM input_lines;		/* how long is input file in lines */

char const *ifetch (LINENUM, bool, size_t *);
void get_input_file (char const *, char const *);
void re_input (void);
void scan_input (char *);

/*
 *  kacpimon - Kernel ACPI Event Monitor
 *
 *  Monitors kernel ACPI events from multiple interfaces and reports them
 *  to the console.
 *
 *  Copyright (C) 2008, Ted Felix (www.tedfelix.com)
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
 *
 *  (tabs at 4)
 */

#ifndef KACPIMON_H__
#define KACPIMON_H__

/* ??? Encapsulate with accessor functions? */

/* Exit flag that can be set by any of the functions to cause the */
/* program to exit.  */
extern int exitflag;

#endif /* KACPIMON_H__ */

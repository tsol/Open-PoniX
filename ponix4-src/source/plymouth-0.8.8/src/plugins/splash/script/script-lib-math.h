/* script-lib-math.h - math script functions library
 *
 * Copyright (C) 2009 Charlie Brej <cbrej@cs.man.ac.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by: Charlie Brej <cbrej@cs.man.ac.uk>
 */
#ifndef SCRIPT_LIB_MATH_H
#define SCRIPT_LIB_MATH_H

#include "script.h"

typedef struct
{
  script_op_t      *script_main_op;
} script_lib_math_data_t;

script_lib_math_data_t *script_lib_math_setup (script_state_t *state);
void script_lib_math_destroy (script_lib_math_data_t *data);

#endif /* SCRIPT_LIB_MATH_H */

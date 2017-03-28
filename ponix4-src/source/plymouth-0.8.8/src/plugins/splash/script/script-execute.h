/* script-execute.h - execution of scripts
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
#ifndef SCRIPT_EXECUTE_H
#define SCRIPT_EXECUTE_H

#include "script.h"

script_return_t script_execute (script_state_t *state,
                                script_op_t    *op);
script_return_t script_execute_object (script_state_t    *state,
                                       script_obj_t      *function,
                                       script_obj_t      *this,
                                       script_obj_t      *first_arg,
                                       ...);

#endif /* SCRIPT_EXECUTE_H */

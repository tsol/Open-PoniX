/* ply-buffer.h - facilities for buffering data
 *
 * Copyright (C) 2008 Ray Strode <rstrode@redhat.com>
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
 */
#ifndef PLY_BUFFER_H
#define PLY_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _ply_buffer ply_buffer_t;

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_buffer_t *ply_buffer_new (void);
void ply_buffer_free (ply_buffer_t *buffer);
void ply_buffer_append_bytes (ply_buffer_t *buffer,
                              const void   *bytes,
                              size_t number_of_bytes);

void ply_buffer_append_from_fd (ply_buffer_t *buffer,
                                int           fd);
#define ply_buffer_append(buffer, format, args...)                             \
        ply_buffer_append_with_non_literal_format_string (buffer,              \
                                                          format "", ##args)
__attribute__((__format__ (__printf__, 2, 3)))
void ply_buffer_append_with_non_literal_format_string (ply_buffer_t   *buffer,
                                                       const char *format, ...);
void ply_buffer_remove_bytes (ply_buffer_t *buffer,
                              size_t        number_of_bytes);
void ply_buffer_remove_bytes_at_end (ply_buffer_t *buffer,
                                     size_t        number_of_bytes);
const char *ply_buffer_get_bytes (ply_buffer_t *buffer);
char *ply_buffer_steal_bytes (ply_buffer_t *buffer);
size_t ply_buffer_get_size (ply_buffer_t *buffer);
void ply_buffer_clear (ply_buffer_t *buffer);
#endif

#endif /* PLY_BUFFER_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */

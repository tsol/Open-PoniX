/* ply-logger.h - logging and tracing facilities
 *
 * Copyright (C) 2007 Ray Strode <rstrode@redhat.com>
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
#ifndef PLY_LOGGER_H
#define PLY_LOGGER_H

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _ply_logger ply_logger_t;

typedef enum
{
  PLY_LOGGER_FLUSH_POLICY_WHEN_ASKED = 0,
  PLY_LOGGER_FLUSH_POLICY_EVERY_TIME
} ply_logger_flush_policy_t;

typedef void (* ply_logger_filter_handler_t) (void          *user_data,
                                              const void    *in_bytes,
                                              size_t         in_size,
                                              void         **out_bytes,
                                              size_t        *out_size,
                                              ply_logger_t  *logger);

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_logger_t *ply_logger_new (void);
void ply_logger_free (ply_logger_t *logger);
bool ply_logger_open_file (ply_logger_t *logger,
                           const char   *filename,
                           bool          world_readable);
void ply_logger_close_file (ply_logger_t    *logger);
void ply_logger_set_output_fd (ply_logger_t *logger,
                               int           fd);
int ply_logger_get_output_fd (ply_logger_t *logger);
bool ply_logger_flush (ply_logger_t *logger);
void ply_logger_set_flush_policy (ply_logger_t              *logger,
                                  ply_logger_flush_policy_t  policy);
ply_logger_flush_policy_t ply_logger_get_flush_policy (ply_logger_t *logger);
void ply_logger_toggle_logging (ply_logger_t *logger);
bool ply_logger_is_logging (ply_logger_t *logger);
void ply_logger_inject_bytes (ply_logger_t *logger,
                              const void   *bytes,
                              size_t number_of_bytes);
void ply_logger_add_filter (ply_logger_t                *logger,
                            ply_logger_filter_handler_t  filter_handler,
                            void                        *user_data);
#define ply_logger_inject(logger, format, args...)                             \
        ply_logger_inject_with_non_literal_format_string (logger,              \
                                                          format "", ##args)
__attribute__((__format__ (__printf__, 2, 3)))
void ply_logger_inject_with_non_literal_format_string (ply_logger_t   *logger,
		                                       const char *format, ...);

ply_logger_t *ply_logger_get_default (void);
ply_logger_t *ply_logger_get_error_default (void);

/* tracing is a debugging facility that incurs a hefty performance hit on the
 * program, so we conditionally compile support for it
 */
#ifdef PLY_ENABLE_TRACING
void ply_logger_toggle_tracing (ply_logger_t *logger);
bool ply_logger_is_tracing_enabled (ply_logger_t *logger);

#define ply_logger_trace(logger, format, args...)                              \
do                                                                             \
  {                                                                            \
    int _old_errno;                                                            \
    _old_errno = errno;                                                        \
    if (ply_logger_is_tracing_enabled (logger))                                \
      {                                                                        \
        ply_logger_flush (logger);                                             \
        errno = _old_errno;                                                    \
        ply_logger_inject (logger,                                             \
                           "[%s] %45.45s:" format "\r\n",                      \
                           __FILE__, __func__, ##args);                        \
        ply_logger_flush (logger);                                             \
        errno = _old_errno;                                                    \
      }                                                                        \
  }                                                                            \
while (0)
#else
#define ply_logger_trace(logger, format, args...)
#define ply_logger_toggle_tracing(logger)
#define ply_logger_is_tracing_enabled(logger) (false)
#endif /* PLY_ENABLE_TRACING */

/* convenience macros
 */
#define ply_open_log_file(filename)                                            \
        ply_logger_open_file (ply_logger_get_default (), filename, false)
#define ply_close_log_file()                                                   \
        ply_logger_close_file (ply_logger_get_default ())
#define ply_flush_log()                                                        \
        ply_logger_flush (ply_logger_get_default ())
#define ply_free_log()                                                         \
        ply_logger_free (ply_logger_get_default ())
#define ply_log(format, args...)                                               \
        ply_logger_inject (ply_logger_get_default (), format "\n", ##args)
#define ply_log_without_new_line(format, args...)                              \
        ply_logger_inject (ply_logger_get_default (), format, ##args)
#define ply_error(format, args...)                                             \
        ply_logger_inject (ply_logger_get_error_default (), format "\n", ##args)
#define ply_error_without_new_line(format, args...)                            \
        ply_logger_inject (ply_logger_get_error_default (), format, ##args)
#define ply_free_error_log()                                                   \
        ply_logger_free (ply_logger_get_error_default ())

#define ply_toggle_tracing()                                                   \
        ply_logger_toggle_tracing (ply_logger_get_error_default ())
#define ply_is_tracing()                                                       \
        ply_logger_is_tracing_enabled (ply_logger_get_error_default ())
#define ply_trace(format, args...)                                             \
        ply_logger_trace (ply_logger_get_error_default (), format, ##args)

#endif

#endif /* PLY_LOGGER_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */

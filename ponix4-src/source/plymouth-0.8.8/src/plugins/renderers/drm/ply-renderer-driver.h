/* ply-renderer-driver.h
 *
 * Copyright (C) 2009 Red Hat, Inc.
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
 * Written By: Ray Strode <rstrode@redhat.com>
 */
#ifndef PLY_RENDERER_DRIVER_H
#define PLY_RENDERER_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#include "ply-list.h"
#include "ply-rectangle.h"
#include "ply-utils.h"

typedef struct _ply_renderer_driver ply_renderer_driver_t;

typedef struct
{
    ply_renderer_driver_t * (* create_driver) (int device_fd);

    void (* destroy_driver) (ply_renderer_driver_t *driver);

    uint32_t (* create_buffer) (ply_renderer_driver_t *driver,
                                unsigned long          width,
                                unsigned long          height,
                                unsigned long         *row_stride);
    bool (* fetch_buffer) (ply_renderer_driver_t *driver,
                           uint32_t               buffer_id,
                           unsigned long         *width,
                           unsigned long         *height,
                           unsigned long         *row_stride);

    bool (* map_buffer) (ply_renderer_driver_t *driver,
                         uint32_t               buffer_id);

    void (* unmap_buffer) (ply_renderer_driver_t *driver,
                           uint32_t               buffer_id);

    char * (* begin_flush) (ply_renderer_driver_t *driver,
                            uint32_t               buffer_id);
    void (* end_flush) (ply_renderer_driver_t *driver,
                        uint32_t               buffer_id);

    void (* destroy_buffer) (ply_renderer_driver_t *driver,
                             uint32_t               buffer_id);

} ply_renderer_driver_interface_t;

#endif /* PLY_RENDERER_DRIVER_H */
/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */

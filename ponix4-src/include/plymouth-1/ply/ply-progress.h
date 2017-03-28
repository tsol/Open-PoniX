/* ply-progress.h - calculats boot progress 
 *
 * Copyright (C) 2007 Red Hat, Inc.
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
 *             Soeren Sandmann <sandmann@redhat.com>
 *             Charlie Brej <cbrej@cs.man.ac.uk>
 */
#ifndef PLY_PROGRESS_H
#define PLY_PROGRESS_H


typedef struct _ply_progress ply_progress_t;

ply_progress_t *ply_progress_new (void);
void ply_progress_free (ply_progress_t* progress);
void ply_progress_load_cache (ply_progress_t* progress, const char *filename);
double ply_progress_get_percentage (ply_progress_t* progress);
void ply_progress_set_percentage (ply_progress_t* progress, double percentage);
double ply_progress_get_time (ply_progress_t* progress);
void ply_progress_pause (ply_progress_t* progress);
void ply_progress_unpause (ply_progress_t* progress);
void ply_progress_save_cache (ply_progress_t* progress, const char *filename);
void ply_progress_status_update (ply_progress_t* progress, const char  *status);

#endif /* PLY_PROGRESS_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */

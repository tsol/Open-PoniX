/*
 * Copyright 2009, 2010 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
/** \file QXLImage.c
 * \author Søren Sandmann <sandmann@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <spice/macros.h>

#include "qxl.h"
#include "murmurhash3.h"

typedef struct image_info_t image_info_t;

struct image_info_t
{
    struct QXLImage *image;
    int ref_count;
    image_info_t *next;
};

#define HASH_SIZE 4096
static image_info_t *image_table[HASH_SIZE];

static unsigned int
hash_and_copy (const uint8_t *src, int src_stride,
	       uint8_t *dest, int dest_stride,
	       int bytes_per_pixel, int width, int height,
	       uint32_t hash)
{
    int i;
  
    for (i = 0; i < height; ++i)
    {
	const uint8_t *src_line = src + i * src_stride;
	uint8_t *dest_line = dest + i * dest_stride;
	int n_bytes = width * bytes_per_pixel;
	if (n_bytes > src_stride)
	    n_bytes = src_stride;

	if (dest)
	    memcpy (dest_line, src_line, n_bytes);

	MurmurHash3_x86_32 (src_line, n_bytes, hash, &hash);
    }

    return hash;
}

static image_info_t *
lookup_image_info (unsigned int hash,
		   int width,
		   int height)
{
    struct image_info_t *info = image_table[hash % HASH_SIZE];

    while (info)
    {
	struct QXLImage *image = info->image;

	if (image->descriptor.id == hash		&&
	    image->descriptor.width == width		&&
	    image->descriptor.height == height)
	{
	    return info;
	}

	info = info->next;
    }

#if 0
    ErrorF ("lookup of %u failed\n", hash);
#endif
    
    return NULL;
}

static image_info_t *
insert_image_info (unsigned int hash)
{
    struct image_info_t *info = malloc (sizeof (image_info_t));

    if (!info)
	return NULL;

    info->next = image_table[hash % HASH_SIZE];
    image_table[hash % HASH_SIZE] = info;
    
    return info;
}

static void
remove_image_info (image_info_t *info)
{
    struct image_info_t **location = &image_table[info->image->descriptor.id % HASH_SIZE];

    while (*location && (*location) != info)
	location = &((*location)->next);

    if (*location)
	*location = info->next;

    free (info);
}

struct qxl_bo *
qxl_image_create (qxl_screen_t *qxl, const uint8_t *data,
		  int x, int y, int width, int height,
		  int stride, int Bpp, Bool fallback)
{
	uint32_t hash;
	image_info_t *info;
	struct QXLImage *image;
	struct qxl_bo *head_bo, *tail_bo;
	struct qxl_bo *image_bo;
	int dest_stride = (width * Bpp + 3) & (~3);
	int h;
	int chunk_size;

	data += y * stride + x * Bpp;

#if 0
	ErrorF ("Must create new image of size %d %d\n", width, height);
#endif
	
	/* Chunk */

	/* FIXME: Check integer overflow */

	head_bo = tail_bo = NULL;

	hash = 0;
	h = height;

	chunk_size = MAX (512 * 512, dest_stride);

#ifdef XF86DRM_MODE
	/* ensure we will not create too many pieces and overflow
	 * the command buffer (MAX_RELOCS).  if so, increase the chunk_size.
	 * each loop creates at least 2 cmd buffer entries, and
	 * we have to leave room when we're done.
	 */
	if (height / (chunk_size / dest_stride) > (MAX_RELOCS / 4)) {
		chunk_size = height / (MAX_RELOCS/4) * dest_stride;
#if 0
		ErrorF ("adjusted chunk_size to %d\n", chunk_size);
#endif
	}
#endif

	while (h)
	{
	    int n_lines = MIN ((chunk_size / dest_stride), h);
	    struct qxl_bo *bo = qxl->bo_funcs->bo_alloc (qxl, sizeof (QXLDataChunk) + n_lines * dest_stride, "image data");

	    QXLDataChunk *chunk = qxl->bo_funcs->bo_map(bo);
	    chunk->data_size = n_lines * dest_stride;
	    hash = hash_and_copy (data, stride,
				  chunk->data, dest_stride,
				  Bpp, width, n_lines, hash);
	    
	    if (tail_bo)
	    {
		qxl->bo_funcs->bo_output_bo_reloc(qxl, offsetof(QXLDataChunk, next_chunk),
					       tail_bo, bo);
		qxl->bo_funcs->bo_output_bo_reloc(qxl, offsetof(QXLDataChunk, prev_chunk),
					       bo, tail_bo);

		chunk->next_chunk = 0;
		
		tail_bo = bo;
	    }
	    else
	    {
		head_bo = tail_bo = bo;
		chunk->next_chunk = 0;
		chunk->prev_chunk = 0;
	    }

	    qxl->bo_funcs->bo_unmap(bo);
	    if (bo != head_bo)
		qxl->bo_funcs->bo_decref(qxl, bo);
	    data += n_lines * stride;
	    h -= n_lines;
	}

	/* Image */
	image_bo = qxl->bo_funcs->bo_alloc (qxl, sizeof *image, "image struct");
	image = qxl->bo_funcs->bo_map(image_bo);

	image->descriptor.id = 0;
	image->descriptor.type = SPICE_IMAGE_TYPE_BITMAP;
	
	image->descriptor.flags = 0;
	image->descriptor.width = width;
	image->descriptor.height = height;

	if (Bpp == 2)
	{
	    image->bitmap.format = SPICE_BITMAP_FMT_16BIT;
	}
	else if (Bpp == 1)
	{
	    image->bitmap.format = SPICE_BITMAP_FMT_8BIT_A;
	}
	else if (Bpp == 4)
	{
	    image->bitmap.format = SPICE_BITMAP_FMT_RGBA;
	}
	else
	{
	    abort();
	}

	image->bitmap.flags = SPICE_BITMAP_FLAGS_TOP_DOWN;
	image->bitmap.x = width;
	image->bitmap.y = height;
	image->bitmap.stride = dest_stride;
	image->bitmap.palette = 0;
	qxl->bo_funcs->bo_output_bo_reloc(qxl, offsetof(QXLImage, bitmap.data),
				       image_bo, head_bo);

	qxl->bo_funcs->bo_decref(qxl, head_bo);
	/* Add to hash table if caching is enabled */
	if ((fallback && qxl->enable_fallback_cache)	||
	    (!fallback && qxl->enable_image_cache))
	{
	    if ((info = insert_image_info (hash)))
	    {
		info->image = image;
		info->ref_count = 1;

		image->descriptor.id = hash;
		image->descriptor.flags = QXL_IMAGE_CACHE;

#if 0
		ErrorF ("added with hash %u\n", hash);
#endif
	    }
	}

	qxl->bo_funcs->bo_unmap(image_bo);
	return image_bo;
}

void
qxl_image_destroy (qxl_screen_t *qxl,
		   struct qxl_bo *image_bo)
{
    struct QXLImage *image;

    image_info_t *info;
    uint64_t chunk, prev_chunk;

    image = qxl->bo_funcs->bo_map(image_bo);
    info = lookup_image_info (image->descriptor.id,
			      image->descriptor.width,
			      image->descriptor.height);
    qxl->bo_funcs->bo_unmap(image_bo);
    if (info && info->image == image)
    {
	--info->ref_count;

	if (info->ref_count != 0)
	    return;

#if 0
	ErrorF ("removed %p from hash table\n", info->image);
#endif
	
	remove_image_info (info);
    }

    image = qxl->bo_funcs->bo_map(image_bo);
    chunk = image->bitmap.data;
    while (chunk)
    {
	struct qxl_bo *bo;
	struct QXLDataChunk *virtual;

	bo = qxl_ums_lookup_phy_addr(qxl, chunk);
	assert(bo);
	virtual = qxl->bo_funcs->bo_map(bo);
	chunk = virtual->next_chunk;
	prev_chunk = virtual->prev_chunk;

	qxl->bo_funcs->bo_unmap(bo);
	qxl->bo_funcs->bo_decref (qxl, bo);
	if (prev_chunk) {
	    bo = qxl_ums_lookup_phy_addr(qxl, prev_chunk);
	    assert(bo);
	    qxl->bo_funcs->bo_decref (qxl, bo);
	}
    }
    qxl->bo_funcs->bo_unmap(image_bo);
    qxl->bo_funcs->bo_decref (qxl, image_bo);
}

void
qxl_drop_image_cache (qxl_screen_t *qxl)
{
    memset (image_table, 0, HASH_SIZE * sizeof (image_info_t *));
}

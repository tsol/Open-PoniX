/*
 * Copyright (C) 2013 Konstantin Khlebnikov
 * Copyright (C) 2013 Luis R. Rodriguez <mcgrof@do-not-panic.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 */
#ifndef BACKPORT_LINUX_RADIX_TREE_H
#define BACKPORT_LINUX_RADIX_TREE_H

#include_next <linux/radix-tree.h>

#ifdef CPTCFG_BACKPORT_BUILD_RADIX_HELPERS

/**
 * struct radix_tree_iter - radix tree iterator state
 *
 * @index:	index of current slot
 * @next_index:	next-to-last index for this chunk
 * @tags:	bit-mask for tag-iterating
 *
 * This radix tree iterator works in terms of "chunks" of slots.  A chunk is a
 * subinterval of slots contained within one radix tree leaf node.  It is
 * described by a pointer to its first slot and a struct radix_tree_iter
 * which holds the chunk's position in the tree and its size.  For tagged
 * iteration radix_tree_iter also holds the slots' bit-mask for one chosen
 * radix tree tag.
 */
struct radix_tree_iter {
	unsigned long	index;
	unsigned long	next_index;
	unsigned long	tags;
};

#define RADIX_TREE_ITER_TAG_MASK	0x00FF	/* tag index in lower byte */
#define RADIX_TREE_ITER_TAGGED		0x0100	/* lookup tagged slots */
#define RADIX_TREE_ITER_CONTIG		0x0200	/* stop at first hole */

/**
 * radix_tree_iter_init - initialize radix tree iterator
 *
 * @iter:	pointer to iterator state
 * @start:	iteration starting index
 * Returns:	NULL
 */
static __always_inline void **
radix_tree_iter_init(struct radix_tree_iter *iter, unsigned long start)
{
	/*
	 * Leave iter->tags uninitialized. radix_tree_next_chunk() will fill it
	 * in the case of a successful tagged chunk lookup.  If the lookup was
	 * unsuccessful or non-tagged then nobody cares about ->tags.
	 *
	 * Set index to zero to bypass next_index overflow protection.
	 * See the comment in radix_tree_next_chunk() for details.
	 */
	iter->index = 0;
	iter->next_index = start;
	return NULL;
}

/**
 * radix_tree_next_chunk - find next chunk of slots for iteration
 *
 * @root:	radix tree root
 * @iter:	iterator state
 * @flags:	RADIX_TREE_ITER_* flags and tag index
 * Returns:	pointer to chunk first slot, or NULL if there no more left
 *
 * This function looks up the next chunk in the radix tree starting from
 * @iter->next_index.  It returns a pointer to the chunk's first slot.
 * Also it fills @iter with data about chunk: position in the tree (index),
 * its end (next_index), and constructs a bit mask for tagged iterating (tags).
 */
void **radix_tree_next_chunk(struct radix_tree_root *root,
			     struct radix_tree_iter *iter, unsigned flags);

/**
 * radix_tree_chunk_size - get current chunk size
 *
 * @iter:	pointer to radix tree iterator
 * Returns:	current chunk size
 */
static __always_inline unsigned
radix_tree_chunk_size(struct radix_tree_iter *iter)
{
	return iter->next_index - iter->index;
}

/**
 * radix_tree_next_slot - find next slot in chunk
 *
 * @slot:	pointer to current slot
 * @iter:	pointer to interator state
 * @flags:	RADIX_TREE_ITER_*, should be constant
 * Returns:	pointer to next slot, or NULL if there no more left
 *
 * This function updates @iter->index in the case of a successful lookup.
 * For tagged lookup it also eats @iter->tags.
 */
static __always_inline void **
radix_tree_next_slot(void **slot, struct radix_tree_iter *iter, unsigned flags)
{
	if (flags & RADIX_TREE_ITER_TAGGED) {
		iter->tags >>= 1;
		if (likely(iter->tags & 1ul)) {
			iter->index++;
			return slot + 1;
		}
		if (!(flags & RADIX_TREE_ITER_CONTIG) && likely(iter->tags)) {
			unsigned offset = __ffs(iter->tags);

			iter->tags >>= offset;
			iter->index += offset + 1;
			return slot + offset + 1;
		}
	} else {
		unsigned size = radix_tree_chunk_size(iter) - 1;

		while (size--) {
			slot++;
			iter->index++;
			if (likely(*slot))
				return slot;
			if (flags & RADIX_TREE_ITER_CONTIG) {
				/* forbid switching to the next chunk */
				iter->next_index = 0;
				break;
			}
		}
	}
	return NULL;
}

/**
 * radix_tree_for_each_chunk - iterate over chunks
 *
 * @slot:	the void** variable for pointer to chunk first slot
 * @root:	the struct radix_tree_root pointer
 * @iter:	the struct radix_tree_iter pointer
 * @start:	iteration starting index
 * @flags:	RADIX_TREE_ITER_* and tag index
 *
 * Locks can be released and reacquired between iterations.
 */
#define radix_tree_for_each_chunk(slot, root, iter, start, flags)	\
	for (slot = radix_tree_iter_init(iter, start) ;			\
	      (slot = radix_tree_next_chunk(root, iter, flags)) ;)

/**
 * radix_tree_for_each_chunk_slot - iterate over slots in one chunk
 *
 * @slot:	the void** variable, at the beginning points to chunk first slot
 * @iter:	the struct radix_tree_iter pointer
 * @flags:	RADIX_TREE_ITER_*, should be constant
 *
 * This macro is designed to be nested inside radix_tree_for_each_chunk().
 * @slot points to the radix tree slot, @iter->index contains its index.
 */
#define radix_tree_for_each_chunk_slot(slot, iter, flags)		\
	for (; slot ; slot = radix_tree_next_slot(slot, iter, flags))

/**
 * radix_tree_for_each_slot - iterate over non-empty slots
 *
 * @slot:	the void** variable for pointer to slot
 * @root:	the struct radix_tree_root pointer
 * @iter:	the struct radix_tree_iter pointer
 * @start:	iteration starting index
 *
 * @slot points to radix tree slot, @iter->index contains its index.
 */
#define radix_tree_for_each_slot(slot, root, iter, start)		\
	for (slot = radix_tree_iter_init(iter, start) ;			\
	     slot || (slot = radix_tree_next_chunk(root, iter, 0)) ;	\
	     slot = radix_tree_next_slot(slot, iter, 0))

/**
 * radix_tree_for_each_contig - iterate over contiguous slots
 *
 * @slot:	the void** variable for pointer to slot
 * @root:	the struct radix_tree_root pointer
 * @iter:	the struct radix_tree_iter pointer
 * @start:	iteration starting index
 *
 * @slot points to radix tree slot, @iter->index contains its index.
 */
#define radix_tree_for_each_contig(slot, root, iter, start)		\
	for (slot = radix_tree_iter_init(iter, start) ;			\
	     slot || (slot = radix_tree_next_chunk(root, iter,		\
				RADIX_TREE_ITER_CONTIG)) ;		\
	     slot = radix_tree_next_slot(slot, iter,			\
				RADIX_TREE_ITER_CONTIG))

/**
 * radix_tree_for_each_tagged - iterate over tagged slots
 *
 * @slot:	the void** variable for pointer to slot
 * @root:	the struct radix_tree_root pointer
 * @iter:	the struct radix_tree_iter pointer
 * @start:	iteration starting index
 * @tag:	tag index
 *
 * @slot points to radix tree slot, @iter->index contains its index.
 */
#define radix_tree_for_each_tagged(slot, root, iter, start, tag)	\
	for (slot = radix_tree_iter_init(iter, start) ;			\
	     slot || (slot = radix_tree_next_chunk(root, iter,		\
			      RADIX_TREE_ITER_TAGGED | tag)) ;		\
	     slot = radix_tree_next_slot(slot, iter,			\
				RADIX_TREE_ITER_TAGGED))

#endif /* CPTCFG_BACKPORT_BUILD_RADIX_HELPERS */

#endif /* BACKPORT_LINUX_RADIX_TREE_H */

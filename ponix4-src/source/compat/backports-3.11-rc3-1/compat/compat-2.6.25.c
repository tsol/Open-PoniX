/*
 * Copyright 2007-2012	Luis R. Rodriguez <mcgrof@winlab.rutgers.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Backport functionality introduced in Linux 2.6.25.
 */

#include <linux/miscdevice.h>
#include <linux/pci.h>

/*
 * The default behaviour of sg_alloc_table() is to use these kmalloc/kfree
 * helpers.
 */
static struct scatterlist *sg_kmalloc(unsigned int nents, gfp_t gfp_mask)
{
	if (nents == SG_MAX_SINGLE_ALLOC) {
		return (struct scatterlist *) __get_free_page(gfp_mask);
	} else
		return kmalloc(nents * sizeof(struct scatterlist), gfp_mask);
}

static void sg_kfree(struct scatterlist *sg, unsigned int nents)
{
	if (nents == SG_MAX_SINGLE_ALLOC)
		free_page((unsigned long) sg);
	else
		kfree(sg);
}


/**
 * __sg_free_table - Free a previously mapped sg table
 * @table:	The sg table header to use
 * @max_ents:	The maximum number of entries per single scatterlist
 * @free_fn:	Free function
 *
 *  Description:
 *    Free an sg table previously allocated and setup with
 *    __sg_alloc_table().  The @max_ents value must be identical to
 *    that previously used with __sg_alloc_table().
 *
 **/
void __sg_free_table(struct sg_table *table, unsigned int max_ents,
		     sg_free_fn *free_fn)
{
	struct scatterlist *sgl, *next;

	if (unlikely(!table->sgl))
		return;

	sgl = table->sgl;
	while (table->orig_nents) {
		unsigned int alloc_size = table->orig_nents;
		unsigned int sg_size;

		/*
		 * If we have more than max_ents segments left,
		 * then assign 'next' to the sg table after the current one.
		 * sg_size is then one less than alloc size, since the last
		 * element is the chain pointer.
		 */
		if (alloc_size > max_ents) {
			next = sg_chain_ptr(&sgl[max_ents - 1]);
			alloc_size = max_ents;
			sg_size = alloc_size - 1;
		} else {
			sg_size = alloc_size;
			next = NULL;
		}

		table->orig_nents -= sg_size;
		free_fn(sgl, alloc_size);
		sgl = next;
	}

	table->sgl = NULL;
}
EXPORT_SYMBOL_GPL(__sg_free_table);

/**
 * sg_free_table - Free a previously allocated sg table
 * @table:	The mapped sg table header
 *
 **/
void sg_free_table(struct sg_table *table)
{
	__sg_free_table(table, SG_MAX_SINGLE_ALLOC, sg_kfree);
}
EXPORT_SYMBOL_GPL(sg_free_table);

/**
 * __sg_alloc_table - Allocate and initialize an sg table with given allocator
 * @table:	The sg table header to use
 * @nents:	Number of entries in sg list
 * @max_ents:	The maximum number of entries the allocator returns per call
 * @gfp_mask:	GFP allocation mask
 * @alloc_fn:	Allocator to use
 *
 * Description:
 *   This function returns a @table @nents long. The allocator is
 *   defined to return scatterlist chunks of maximum size @max_ents.
 *   Thus if @nents is bigger than @max_ents, the scatterlists will be
 *   chained in units of @max_ents.
 *
 * Notes:
 *   If this function returns non-0 (eg failure), the caller must call
 *   __sg_free_table() to cleanup any leftover allocations.
 *
 **/
int __sg_alloc_table(struct sg_table *table, unsigned int nents,
		     unsigned int max_ents, gfp_t gfp_mask,
		     sg_alloc_fn *alloc_fn)
{
	struct scatterlist *sg, *prv;
	unsigned int left;

#ifndef ARCH_HAS_SG_CHAIN
	if (WARN_ON_ONCE(nents > max_ents))
		return -EINVAL;
#endif

	memset(table, 0, sizeof(*table));

	left = nents;
	prv = NULL;
	do {
		unsigned int sg_size, alloc_size = left;

		if (alloc_size > max_ents) {
			alloc_size = max_ents;
			sg_size = alloc_size - 1;
		} else
			sg_size = alloc_size;

		left -= sg_size;

		sg = alloc_fn(alloc_size, gfp_mask);
		if (unlikely(!sg)) {
			/*
			 * Adjust entry count to reflect that the last
			 * entry of the previous table won't be used for
			 * linkage.  Without this, sg_kfree() may get
			 * confused.
			 */
			if (prv)
				table->nents = ++table->orig_nents;

 			return -ENOMEM;
		}

		sg_init_table(sg, alloc_size);
		table->nents = table->orig_nents += sg_size;

		/*
		 * If this is the first mapping, assign the sg table header.
		 * If this is not the first mapping, chain previous part.
		 */
		if (prv)
			sg_chain(prv, max_ents, sg);
		else
			table->sgl = sg;

		/*
		 * If no more entries after this one, mark the end
		 */
		if (!left)
			sg_mark_end(&sg[sg_size - 1]);

		prv = sg;
	} while (left);

	return 0;
}
EXPORT_SYMBOL_GPL(__sg_alloc_table);

/**
 * sg_alloc_table - Allocate and initialize an sg table
 * @table:	The sg table header to use
 * @nents:	Number of entries in sg list
 * @gfp_mask:	GFP allocation mask
 *
 *  Description:
 *    Allocate and initialize an sg table. If @nents@ is larger than
 *    SG_MAX_SINGLE_ALLOC a chained sg table will be setup.
 *
 **/
int sg_alloc_table(struct sg_table *table, unsigned int nents, gfp_t gfp_mask)
{
	int ret;

	ret = __sg_alloc_table(table, nents, SG_MAX_SINGLE_ALLOC,
			       gfp_mask, sg_kmalloc);
	if (unlikely(ret))
		__sg_free_table(table, SG_MAX_SINGLE_ALLOC, sg_kfree);

	return ret;
}
EXPORT_SYMBOL_GPL(sg_alloc_table);


/*
 * To backport b718989d correctly pcibios_enable_device()
 * is required but we don't have access to it on modules
 * as its an architecture specific routine that is not
 * exported and as such only core kernel code has access
 * to it. We implement a sloppy work around for backporting
 * this.
 */
int pci_enable_device_mem(struct pci_dev *dev)
{
	int bars = pci_select_bars(dev, IORESOURCE_MEM);

	return pci_enable_device_bars(dev, bars);
}
EXPORT_SYMBOL_GPL(pci_enable_device_mem);

/**
 * The following things are out of ./lib/vsprintf.c
 * The new iwlwifi driver is using them.
 */

/**
 * strict_strtoul - convert a string to an unsigned long strictly
 * @cp: The string to be converted
 * @base: The number base to use
 * @res: The converted result value
 *
 * strict_strtoul converts a string to an unsigned long only if the
 * string is really an unsigned long string, any string containing
 * any invalid char at the tail will be rejected and -EINVAL is returned,
 * only a newline char at the tail is acceptible because people generally
 * change a module parameter in the following way:
 *
 * 	echo 1024 > /sys/module/e1000/parameters/copybreak
 *
 * echo will append a newline to the tail.
 *
 * It returns 0 if conversion is successful and *res is set to the converted
 * value, otherwise it returns -EINVAL and *res is set to 0.
 *
 * simple_strtoul just ignores the successive invalid characters and
 * return the converted value of prefix part of the string.
 */
int strict_strtoul(const char *cp, unsigned int base, unsigned long *res);

/**
 * strict_strtol - convert a string to a long strictly
 * @cp: The string to be converted
 * @base: The number base to use
 * @res: The converted result value
 *
 * strict_strtol is similiar to strict_strtoul, but it allows the first
 * character of a string is '-'.
 *
 * It returns 0 if conversion is successful and *res is set to the converted
 * value, otherwise it returns -EINVAL and *res is set to 0.
 */
int strict_strtol(const char *cp, unsigned int base, long *res);

#define define_strict_strtoux(type, valtype)				\
int strict_strtou##type(const char *cp, unsigned int base, valtype *res)\
{									\
	char *tail;							\
	valtype val;							\
	size_t len;							\
									\
	*res = 0;							\
	len = strlen(cp);						\
	if (len == 0)							\
		return -EINVAL;						\
									\
	val = simple_strtou##type(cp, &tail, base);			\
	if ((*tail == '\0') ||						\
		((len == (size_t)(tail - cp) + 1) && (*tail == '\n'))) {\
		*res = val;						\
		return 0;						\
	}								\
									\
	return -EINVAL;							\
}									\

#define define_strict_strtox(type, valtype)				\
int strict_strto##type(const char *cp, unsigned int base, valtype *res)	\
{									\
	int ret;							\
	if (*cp == '-') {						\
		ret = strict_strtou##type(cp+1, base, res);		\
		if (!ret)						\
			*res = -(*res);					\
	} else								\
		ret = strict_strtou##type(cp, base, res);		\
									\
	return ret;							\
}									\

define_strict_strtoux(l, unsigned long)
define_strict_strtox(l, long)

EXPORT_SYMBOL_GPL(strict_strtoul);
EXPORT_SYMBOL_GPL(strict_strtol);


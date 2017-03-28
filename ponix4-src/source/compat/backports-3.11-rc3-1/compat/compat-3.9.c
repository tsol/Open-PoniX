/*
 * Copyright (c) 2013  Luis R. Rodriguez <mcgrof@do-not-panic.com>
 *
 * Backport functionality introduced in Linux 3.9.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/scatterlist.h>
#include <linux/device.h>
#include <linux/err.h>

#ifdef __sg_page_iter_next

void __sg_page_iter_start(struct sg_page_iter *piter,
			  struct scatterlist *sglist, unsigned int nents,
			  unsigned long pgoffset)
{
	piter->__pg_advance = 0;
	piter->__nents = nents;

	piter->page = NULL;
	piter->sg = sglist;
	piter->sg_pgoffset = pgoffset;
}
EXPORT_SYMBOL_GPL(__sg_page_iter_start);

static int sg_page_count(struct scatterlist *sg)
{
	return PAGE_ALIGN(sg->offset + sg->length) >> PAGE_SHIFT;
}

bool __sg_page_iter_next(struct sg_page_iter *piter)
{
	if (!piter->__nents || !piter->sg)
		return false;

	piter->sg_pgoffset += piter->__pg_advance;
	piter->__pg_advance = 1;

	while (piter->sg_pgoffset >= sg_page_count(piter->sg)) {
		piter->sg_pgoffset -= sg_page_count(piter->sg);
		piter->sg = sg_next(piter->sg);
		if (!--piter->__nents || !piter->sg)
			return false;
	}
	piter->page = nth_page(sg_page(piter->sg), piter->sg_pgoffset);

	return true;
}
EXPORT_SYMBOL_GPL(__sg_page_iter_next);

void __iomem *devm_ioremap_resource(struct device *dev, struct resource *res)
{
	void __iomem *dest_ptr;

	dest_ptr = devm_ioremap_resource(dev, res);
	if (!dest_ptr)
		return (void __iomem *)ERR_PTR(-ENOMEM);
	return dest_ptr;
}
EXPORT_SYMBOL_GPL(devm_ioremap_resource);

#endif /* __sg_page_iter_next */

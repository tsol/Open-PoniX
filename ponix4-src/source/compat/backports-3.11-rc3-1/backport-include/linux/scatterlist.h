#ifndef __BACKPORT_SCATTERLIST_H
#define __BACKPORT_SCATTERLIST_H
#include_next <linux/scatterlist.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
struct sg_table {
	struct scatterlist *sgl;        /* the list */
	unsigned int nents;             /* number of mapped entries */
	unsigned int orig_nents;        /* original size of list */
};

#define sg_alloc_fn LINUX_BACKPORT(sg_alloc_fn)
typedef struct scatterlist *(sg_alloc_fn)(unsigned int, gfp_t);

#define sg_free_fn LINUX_BACKPORT(sg_free_fn)
typedef void (sg_free_fn)(struct scatterlist *, unsigned int);

#define __sg_free_table LINUX_BACKPORT(__sg_free_table)
void __sg_free_table(struct sg_table *table, unsigned int max_ents,
		     sg_free_fn *free_fn);
#define sg_free_table LINUX_BACKPORT(sg_free_table)
void sg_free_table(struct sg_table *);
#define __sg_alloc_table LINUX_BACKPORT(__sg_alloc_table)
int __sg_alloc_table(struct sg_table *table, unsigned int nents,
		     unsigned int max_ents, gfp_t gfp_mask,
		     sg_alloc_fn *alloc_fn);
#define sg_alloc_table LINUX_BACKPORT(sg_alloc_table)
int sg_alloc_table(struct sg_table *table, unsigned int nents, gfp_t gfp_mask);

#define SG_MAX_SINGLE_ALLOC            (PAGE_SIZE / sizeof(struct scatterlist))
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
/* backports efc42bc9 */
#define sg_alloc_table_from_pages LINUX_BACKPORT(sg_alloc_table_from_pages)
int sg_alloc_table_from_pages(struct sg_table *sgt,
			      struct page **pages, unsigned int n_pages,
			      unsigned long offset, unsigned long size,
			      gfp_t gfp_mask);
#endif /* < 3.6 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0)

/* Lets expect distributions might backport this */
#ifndef for_each_sg_page
/*
 * sg page iterator
 *
 * Iterates over sg entries page-by-page.  On each successful iteration,
 * @piter->page points to the current page, @piter->sg to the sg holding this
 * page and @piter->sg_pgoffset to the page's page offset within the sg. The
 * iteration will stop either when a maximum number of sg entries was reached
 * or a terminating sg (sg_last(sg) == true) was reached.
 */
struct sg_page_iter {
	struct page		*page;		/* current page */
	struct scatterlist	*sg;		/* sg holding the page */
	unsigned int		sg_pgoffset;	/* page offset within the sg */

	/* these are internal states, keep away */
	unsigned int		__nents;	/* remaining sg entries */
	int			__pg_advance;	/* nr pages to advance at the
						 * next step */
};

#define __sg_page_iter_next LINUX_BACKPORT(__sg_page_iter_next)
bool __sg_page_iter_next(struct sg_page_iter *piter);
#define __sg_page_iter_start LINUX_BACKPORT(__sg_page_iter_start)
void __sg_page_iter_start(struct sg_page_iter *piter,
			  struct scatterlist *sglist, unsigned int nents,
			  unsigned long pgoffset);

/**
 * for_each_sg_page - iterate over the pages of the given sg list
 * @sglist:	sglist to iterate over
 * @piter:	page iterator to hold current page, sg, sg_pgoffset
 * @nents:	maximum number of sg entries to iterate over
 * @pgoffset:	starting page offset
 */
#define for_each_sg_page(sglist, piter, nents, pgoffset)		   \
	for (__sg_page_iter_start((piter), (sglist), (nents), (pgoffset)); \
	     __sg_page_iter_next(piter);)

#endif /* for_each_sg_page assumption */
#endif /* version < 3.9 */

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0))

#define sg_page_iter_page LINUX_BACKPORT(sg_page_iter_page)
/**
 * sg_page_iter_page - get the current page held by the page iterator
 * @piter:     page iterator holding the page
 */
static inline struct page *sg_page_iter_page(struct sg_page_iter *piter)
{
	return nth_page(sg_page(piter->sg), piter->sg_pgoffset);
}

#define sg_page_iter_dma_address LINUX_BACKPORT(sg_page_iter_dma_address)
/**
 * sg_page_iter_dma_address - get the dma address of the current page held by
 * the page iterator.
 * @piter:     page iterator holding the page
 */
static inline dma_addr_t sg_page_iter_dma_address(struct sg_page_iter *piter)
{
	return sg_dma_address(piter->sg) + (piter->sg_pgoffset << PAGE_SHIFT);
}
#endif /* version < 3.10 */

#endif /* __BACKPORT_SCATTERLIST_H */

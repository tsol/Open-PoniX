#ifndef __BACKPORT_LINUX_SHMEM_FS_H
#define __BACKPORT_LINUX_SHMEM_FS_H
#include_next <linux/shmem_fs.h>


#if (RHEL_RELEASE_CODE < RHEL_RELEASE_VERSION(6,4))
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,0)
/* This pulls-in a lot of non-exported symbol backports
 * on kernels older than 2.6.32. There's no harm for not
 * making this available on kernels < 2.6.32. */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
#include <linux/pagemap.h>
/* This backports the 2nd part of:
 *
 * commit d9d90e5eb70e09903dadff42099b6c948f814050
 * Author: Hugh Dickins <hughd@google.com>
 * Date:   Mon Jun 27 16:18:04 2011 -0700
 *
 *	tmpfs: add shmem_read_mapping_page_gfp
 *
 * First part is in compat-3.0.c.
 */
#define shmem_read_mapping_page_gfp LINUX_BACKPORT(shmem_read_mapping_page_gfp)
extern struct page *shmem_read_mapping_page_gfp(struct address_space *mapping,
						pgoff_t index, gfp_t gfp);


#define shmem_read_mapping_page LINUX_BACKPORT(shmem_read_mapping_page)
static inline struct page *shmem_read_mapping_page(
                               struct address_space *mapping, pgoff_t index)
{
       return shmem_read_mapping_page_gfp(mapping, index,
                                       mapping_gfp_mask(mapping));
}
#endif
#endif
#endif

#endif /* __BACKPORT_LINUX_SHMEM_FS_H */

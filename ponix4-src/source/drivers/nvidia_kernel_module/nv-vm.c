/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 1999-2001 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */

#include "nv-misc.h"
#include "os-interface.h"
#include "nv.h"
#include "nv-linux.h"

#define NV_DEFAULT_LIST_PAGE_COUNT 10

void
nv_vm_list_page_count(nv_pte_t **page_list, NvU32 num_pages)
{
#if defined(NV_DBG_MEM)
    nv_pte_t *page_ptr = *page_list;
    unsigned int i;

    if (page_ptr == NULL)
        return;

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM:    page_table at 0x%p, %d pages\n", 
        page_ptr, num_pages);

    for (i = 0; (i < NV_DEFAULT_LIST_PAGE_COUNT) && (i < num_pages); i++)
    {
        page_ptr = page_list[i];
        nv_printf(NV_DBG_MEMINFO,
                  "NVRM: VM:       v 0x%08lx p 0x%08lx d 0x%08lx: count %d flags 0x%x\n", 
                  page_ptr->virt_addr,
                  page_ptr->phys_addr,
                  page_ptr->dma_addr,
                  NV_GET_PAGE_COUNT(page_ptr),
                  NV_GET_PAGE_FLAGS(page_ptr)
                  );
    }
#endif
}

static inline void nv_set_page_attrib_uncached(nv_pte_t *page_ptr)
{
    if (nv_update_memory_types)
    {
#if defined(NV_SET_PAGES_UC_PRESENT)
        struct page *page = NV_GET_PAGE_STRUCT(page_ptr->phys_addr);
        set_pages_uc(page, 1);
#elif defined(NV_CHANGE_PAGE_ATTR_PRESENT)
        struct page *page = NV_GET_PAGE_STRUCT(page_ptr->phys_addr);
        pgprot_t prot = PAGE_KERNEL_NOCACHE;
#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
        pgprot_val(prot) &= __nv_supported_pte_mask;
#endif
        change_page_attr(page, 1, prot);
#endif
    }
}

static inline void nv_set_page_attrib_cached(nv_pte_t *page_ptr)
{
    if (nv_update_memory_types)
    {
#if defined(NV_SET_PAGES_UC_PRESENT)
        struct page *page = NV_GET_PAGE_STRUCT(page_ptr->phys_addr);
        set_pages_wb(page, 1);
#elif defined(NV_CHANGE_PAGE_ATTR_PRESENT)
        struct page *page = NV_GET_PAGE_STRUCT(page_ptr->phys_addr);
        pgprot_t prot = PAGE_KERNEL;
#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
        pgprot_val(prot) &= __nv_supported_pte_mask;
#endif
        change_page_attr(page, 1, prot);
#endif
    }
}

#if defined(NV_SG_MAP_BUFFERS)

/* track how much memory has been remapped through the iommu/swiotlb */

#if defined(NV_NEED_REMAP_CHECK)
extern unsigned int nv_remap_count;
extern unsigned int nv_remap_limit;
#endif

static inline int nv_map_sg(struct pci_dev *dev, struct scatterlist *sg)
{
    int ret;
#if !defined(KERNEL_2_4) && defined(NV_SWIOTLB)
    if (swiotlb)
        ret = swiotlb_map_sg(&dev->dev, sg, 1, PCI_DMA_BIDIRECTIONAL);
    else
#endif
        ret = pci_map_sg(dev, sg, 1, PCI_DMA_BIDIRECTIONAL);
    return ret;
}

static inline void nv_unmap_sg(struct pci_dev *dev, struct scatterlist *sg)
{
#if !defined(KERNEL_2_4) && defined(NV_SWIOTLB)
    if (swiotlb)
        swiotlb_unmap_sg(&dev->dev, sg, 1, PCI_DMA_BIDIRECTIONAL);
    else
#endif
        pci_unmap_sg(dev, sg, 1, PCI_DMA_BIDIRECTIONAL);
}

#define NV_MAP_SG_MAX_RETRIES 16

static inline int nv_sg_map_buffer(
    struct pci_dev     *dev,
    nv_pte_t          **page_list,
    void               *base,
    unsigned int        num_pages
)
{
    struct scatterlist *sg_ptr = &page_list[0]->sg_list;
    struct scatterlist sg_tmp;
    unsigned int i;

    NV_SG_INIT_TABLE(sg_ptr, 1);

#if defined(NV_SCATTERLIST_HAS_PAGE)
    sg_ptr->page = virt_to_page(base);
#else
    sg_ptr->page_link = (NvUPtr)virt_to_page(base);
#endif
    sg_ptr->offset = ((unsigned long)base & ~NV_PAGE_MASK);
    sg_ptr->length  = num_pages * PAGE_SIZE;

#if !defined(NV_INTEL_IOMMU) && \
  !defined(NV_XEN_SUPPORT_OLD_STYLE_KERNEL)
    if (((virt_to_phys(base) + sg_ptr->length - 1) & ~dev->dma_mask) == 0)
    {
        sg_ptr->dma_address = virt_to_phys(base);
        goto done;
    }
#endif

#if defined(NV_NEED_REMAP_CHECK)
    if ((nv_remap_count + sg_ptr->length) > nv_remap_limit)
    {
        static int count = 0;
        if (count < NV_MAX_RECURRING_WARNING_MESSAGES)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: internal %s remap limit (0x%x bytes) exceeded\n",
                nv_swiotlb ? "SWIOTLB" : "IOMMU", nv_remap_limit);

        }
        if (count++ == 0)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: please see the README section on IOMMU/SWIOTLB "
                "interaction for details\n");
        }
        return 1;
    }
#endif

    i = NV_MAP_SG_MAX_RETRIES;
    do {
        if (nv_map_sg(dev, sg_ptr) == 0)
            return -1;

        if (sg_ptr->dma_address & ~NV_PAGE_MASK)
        {
            nv_unmap_sg(dev, sg_ptr);

            NV_SG_INIT_TABLE(&sg_tmp, 1);

#if defined(NV_SCATTERLIST_HAS_PAGE)
            sg_tmp.page = sg_ptr->page;
#else
            sg_tmp.page_link = sg_ptr->page_link;
#endif
            sg_tmp.offset = sg_ptr->offset;
            sg_tmp.length = 2048;

            if (nv_map_sg(dev, &sg_tmp) == 0)
                return -1;

            if (nv_map_sg(dev, sg_ptr) == 0)
            {
                nv_unmap_sg(dev, &sg_tmp);
                return -1;
            }

            nv_unmap_sg(dev, &sg_tmp);
        }
    }
    while (i-- && sg_ptr->dma_address & ~NV_PAGE_MASK);

    if (sg_ptr->dma_address & ~NV_PAGE_MASK)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: VM: nv_sg_map_buffer: failed to obtain aligned mapping\n");
        nv_unmap_sg(dev, sg_ptr);
        return -1;
    }

#if defined(NV_NEED_REMAP_CHECK)
    if (sg_ptr->dma_address != virt_to_phys(base))
        nv_remap_count += sg_ptr->length;
#endif

#if !defined(NV_INTEL_IOMMU) && \
  !defined(NV_XEN_SUPPORT_OLD_STYLE_KERNEL)
done:
#endif
    for (i = 1; i < num_pages; i++)
    {
        page_list[i]->sg_list.dma_address = sg_ptr->dma_address + (i * PAGE_SIZE);
    }

    return 0;
}

static inline int nv_sg_load(
    struct scatterlist *sg_ptr,
    nv_pte_t           *page_ptr
)
{
    page_ptr->dma_addr = sg_ptr->dma_address;

#if defined(NV_SWIOTLB)
    // with the sw io tlb, we've actually switched to different physical pages
    // wire in the new page's addresses, but save the original off to free later
    if (nv_swiotlb)
    {
        // note that we modify our local version, not the sg_ptr version that
        // will be returned to the swiotlb pool
        NV_FIXUP_SWIOTLB_VIRT_ADDR_BUG(page_ptr->dma_addr);
        page_ptr->orig_phys_addr = page_ptr->phys_addr;
        page_ptr->phys_addr      = page_ptr->dma_addr;
        page_ptr->orig_virt_addr = page_ptr->virt_addr;
        page_ptr->virt_addr      = (unsigned long) __va(page_ptr->dma_addr);
    }
#endif

    return 0;
}

// make sure we only unmap the page if it was really mapped through the iommu,
// in which case the dma_addr and phys_addr will not match.
static inline void nv_sg_unmap_buffer(
    struct pci_dev     *dev,
    struct scatterlist *sg_ptr,
    nv_pte_t           *page_ptr
)
{
#if defined(NV_SWIOTLB)
    // for sw io tlbs, dma_addr == phys_addr currently, so the check below fails
    // restore the original settings first, then the following check will work
    if (nv_swiotlb && page_ptr->orig_phys_addr)
    {
        page_ptr->phys_addr      = page_ptr->orig_phys_addr;
        page_ptr->virt_addr      = page_ptr->orig_virt_addr;
        page_ptr->orig_phys_addr = 0;
        page_ptr->orig_virt_addr = 0;
    }
#endif

    if (page_ptr->dma_addr != page_ptr->phys_addr)
    {
        nv_unmap_sg(dev, sg_ptr);
        page_ptr->dma_addr = 0;
#if defined(NV_NEED_REMAP_CHECK)
        nv_remap_count -= sg_ptr->length;
#endif
    }
}
#endif  /* NV_SG_MAP_BUFFERS */

#if !defined(NV_VMWARE)
/*
 * Cache flushes and TLB invalidation
 *
 * Allocating new pages, we may change their kernel mappings' memory types
 * from cached to UC to avoid cache aliasing. One problem with this is
 * that cache lines may still contain data from these pages and there may
 * be then stale TLB entries.
 *
 * The Linux kernel's strategy for addressing the above has varied since
 * the introduction of change_page_attr(): it has been implicit in the
 * change_page_attr() interface, explicit in the global_flush_tlb()
 * interface and, as of this writing, is implicit again in the interfaces
 * replacing change_page_attr(), i.e. set_pages_*().
 *
 * In theory, any of the above should satisfy the NVIDIA graphics driver's
 * requirements. In practise, none do reliably:
 *
 *  - some Linux 2.4 kernels (e.g. vanilla 2.4.27) did not flush caches
 *    on CPUs with Self Snoop capability, but this feature does not
 *    interact well with AGP.
 *
 *  - most Linux 2.6 kernels' implementations of the global_flush_tlb()
 *    interface fail to flush caches on all or some CPUs, for a
 *    variety of reasons.
 *
 * Due to the above, the NVIDIA Linux graphics driver is forced to perform
 * heavy-weight flush/invalidation operations to avoid problems due to
 * stale cache lines and/or TLB entries.
 */

static void nv_flush_cache(void *p)
{
    unsigned long reg0, reg1;

    CACHE_FLUSH();

    // flush global TLBs
#if defined(NVCPU_X86)
    asm volatile("movl %%cr4, %0;  \n"
                 "andl $~0x80, %0; \n"
                 "movl %0, %%cr4;  \n"
                 "movl %%cr3, %1;  \n"
                 "movl %1, %%cr3;  \n"
                 "orl  $0x80, %0;  \n"
                 "movl %0, %%cr4;  \n"
                 : "=&r" (reg0), "=&r" (reg1)
                 : : "memory");
#elif defined(NVCPU_X86_64)
    asm volatile("movq %%cr4, %0;  \n"
                 "andq $~0x80, %0; \n"
                 "movq %0, %%cr4;  \n"
                 "movq %%cr3, %1;  \n"
                 "movq %1, %%cr3;  \n"
                 "orq  $0x80, %0;  \n"
                 "movq %0, %%cr4;  \n"
                 : "=&r" (reg0), "=&r" (reg1)
                 : : "memory");
#endif
}
#endif

static void nv_flush_caches(void)
{
#if defined(NV_VMWARE) || \
  defined(NV_XEN_SUPPORT_OLD_STYLE_KERNEL)
    return;
#else
    nv_execute_on_all_cpus(nv_flush_cache, NULL);
#if !defined(KERNEL_2_4) && \
  (defined(NVCPU_X86) || defined(NVCPU_X86_64)) && \
  defined(NV_CHANGE_PAGE_ATTR_PRESENT)
    global_flush_tlb();
#endif
#endif
}

/*
 * DMA buffer management
 * nv_vm_malloc is used to allocate any memory used for dma purposes.
 * for various historical reasons, unless otherwise requested, we allocate
 * memory a page at a time.
 *
 * some cases where allocations have caused system problems:
 *
 *   allocating buffers with vmalloc can exhaust the kernel virtual address
 *     space. in some cases, the kernel's virtual address space can shrink
 *     down to < 128 megs on i386 systems.
 *     additionally, newer kernel api documents specifically forbid using
 *     vmalloc'd memory for dma.
 *
 *   allocating too many dma buffers via get_free_pages with order greater
 *     than 0 has also been shown to cause problems. too many large such
 *     allocations fragment the free_page pools, such that only single pages
 *     are left. at this point, creating new processes fails, as the task
 *     struct is 2 physical pages, which cannot be allocated. (this is not
 *     true with newer kernels, but is necessary for backwards compatibility)
 *
 * nv-linux.h defines which GFP_ flags to send to get_free_pages, based on
 * the processor platform. this is to insure we get 32-bit addresses when
 * possible.
 *
 * additionally, some platforms require us to allocate 64-bit addresses,
 * then remap them through an iommu/swiotlb to get 32-bit addresses. this
 * is handled here as well.
 */

RM_STATUS nv_vm_malloc_pages(
    nv_state_t *nv,
    nv_alloc_t *at
)
{
    RM_STATUS status;
    nv_pte_t *page_ptr = *at->page_table;
#if !defined(CONFIG_X86_UV)
    NvU64 system_memory_size;
#endif
    unsigned int i, j, gfp_mask = NV_GFP_KERNEL;
    unsigned long virt_addr = 0;
    NvU64 phys_addr;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct pci_dev *dev = nvl->dev;
#if defined(NV_SG_MAP_BUFFERS)
    int ret = -1;
#endif

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: nv_vm_malloc_pages: %d pages\n",
        at->num_pages);

#if !defined(CONFIG_X86_UV)
    system_memory_size = (os_get_system_memory_size() * RM_PAGE_SIZE);
    if (system_memory_size != 0)
    {
        if (dev->dma_mask < (system_memory_size - 1))
            gfp_mask = NV_GFP_DMA32;
    }
    else if (dev->dma_mask < 0xffffffffffULL)
    {
        gfp_mask = NV_GFP_DMA32;
    }
#endif
#if defined(__GFP_ZERO)
    if (at->flags & NV_ALLOC_TYPE_ZEROED)
        gfp_mask |= __GFP_ZERO;
#endif

    // allocate and prep contiguous pages up front if necessary
    if (NV_ALLOC_MAPPING_CONTIG(at->flags))
    {
#if defined(NV_XEN_SUPPORT_OLD_STYLE_KERNEL)
        if (at->num_pages > 1)
            return RM_ERR_NOT_SUPPORTED;
#endif
#if defined(NV_SWIOTLB)
        if (nv_swiotlb && at->num_pages > 64)
        {
            nv_printf(NV_DBG_SETUP,
                "NVRM: VM: nv_vm_malloc_pages: SWIOTLB cannot support large "
                "contiguous allocation: %d pages\n",
                at->num_pages);
            return RM_ERR_NOT_SUPPORTED;
        }
#endif
        at->order = nv_calc_order(at->num_pages * PAGE_SIZE);
        nv_printf(NV_DBG_MEMINFO, "NVRM: VM:    contiguous, order %d\n", at->order);
        NV_GET_FREE_PAGES(virt_addr, at->order, (gfp_mask | __GFP_COMP));
        if (virt_addr == 0)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: VM: nv_vm_malloc_pages: failed to allocate contiguous "
                "memory\n");
            return RM_ERR_NO_FREE_MEM;
        }
#if !defined(__GFP_ZERO)
        if (at->flags & NV_ALLOC_TYPE_ZEROED)
            memset(virt_addr, 0, (at->num_pages * PAGE_SIZE));
#endif

#if defined(NV_SG_MAP_BUFFERS)
        // for amd 64-bit platforms, remap pages to make them 32-bit addressable
        // in this case, we need the final remapping to be contiguous, so we
        // have to do the whole mapping at once, instead of page by page
        if ((ret = nv_sg_map_buffer(dev, at->page_table, (void *)virt_addr, at->num_pages)))
        {
            if (ret < 0)
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM: VM: nv_vm_malloc_pages: failed to remap contiguous "
                    "memory\n");
            }
            NV_FREE_PAGES(virt_addr, at->order);
            return RM_ERR_INSUFFICIENT_RESOURCES;
        }
#endif
    }

    for (i = 0; i < at->num_pages; i++) 
    {
        if (!NV_ALLOC_MAPPING_CONTIG(at->flags))
        { 
            NV_GET_FREE_PAGES(virt_addr, 0, gfp_mask); 
            if (virt_addr == 0)
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM: VM: nv_vm_malloc_pages: failed to allocate a page\n");
                status = RM_ERR_NO_FREE_MEM;
                goto failed;
            }
#if !defined(__GFP_ZERO)
            if (at->flags & NV_ALLOC_TYPE_ZEROED)
                memset(virt_addr, 0, PAGE_SIZE);
#endif
        }

        phys_addr = nv_get_kern_phys_address(virt_addr);
        if (phys_addr == 0)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: VM: nv_vm_malloc_pages: failed to lookup physical address\n");
            status = RM_ERR_OPERATING_SYSTEM;
            goto failed;
        }

#if defined(_PAGE_NX)
        if ((_PAGE_NX & pgprot_val(PAGE_KERNEL)) != 0 && phys_addr < 0x400000) {
            // Until a bug in change_page_attr() is fixed
            // we avoid pages with physaddr < 0x400000,
            // since splitting "jumbo" mappings results
            // in kernel pages being mapped as PAGE_KERNEL, which
            // may include _PAGE_NX, effectively making much of the
            // kernel code non-executable.
            nv_printf(NV_DBG_SETUP,
                "NVRM: VM: nv_vm_malloc_pages: discarding page @%08x\n", phys_addr);
            --i;
            continue;
        }
#endif

        page_ptr = at->page_table[i];
        page_ptr->phys_addr = phys_addr;
        page_ptr->page_count = NV_GET_PAGE_COUNT(page_ptr);
        page_ptr->virt_addr = virt_addr;
        page_ptr->dma_addr = NV_GET_DMA_ADDRESS(page_ptr->phys_addr);

        /* lock the page for dma purposes */
        NV_LOCK_PAGE(page_ptr);

#if defined(NV_SG_MAP_BUFFERS)
        if (!NV_ALLOC_MAPPING_CONTIG(at->flags))
        {
            if ((ret = nv_sg_map_buffer(dev, &at->page_table[i],
                                        __va(page_ptr->phys_addr), 1)))
            {
                if (ret < 0)
                {
                    nv_printf(NV_DBG_ERRORS,
                        "NVRM: VM: nv_vm_malloc_pages: failed to remap a page\n");
                }
                NV_UNLOCK_PAGE(page_ptr);
                NV_FREE_PAGES(virt_addr, 0);
                i--;
                status = RM_ERR_INSUFFICIENT_RESOURCES;
                goto failed;
            }
        }
        nv_sg_load(&page_ptr->sg_list, page_ptr);
#endif

        if (!NV_ALLOC_MAPPING_CACHED(at->flags))
            nv_set_page_attrib_uncached(page_ptr);

        nv_verify_page_mappings(page_ptr, NV_ALLOC_MAPPING(at->flags));
        virt_addr += PAGE_SIZE;
    }

    nv_vm_list_page_count(at->page_table, at->num_pages);

    if (!NV_ALLOC_MAPPING_CACHED(at->flags))
        nv_flush_caches();

    return RM_OK;

failed:
    for (j = 1; j <= (i+1); j++)
    {
        page_ptr = at->page_table[j-1];

        if (page_ptr && page_ptr->virt_addr)
        {
            if (!NV_ALLOC_MAPPING_CACHED(at->flags))
                nv_set_page_attrib_cached(page_ptr);
#if defined(NV_SG_MAP_BUFFERS)
            if (!NV_ALLOC_MAPPING_CONTIG(at->flags))
                nv_sg_unmap_buffer(dev, &page_ptr->sg_list, page_ptr);
#endif
            NV_UNLOCK_PAGE(page_ptr);
            if (!NV_ALLOC_MAPPING_CONTIG(at->flags))
                NV_FREE_PAGES(page_ptr->virt_addr, 0);
        }
    }

    if (!NV_ALLOC_MAPPING_CACHED(at->flags))
        nv_flush_caches();

    if (NV_ALLOC_MAPPING_CONTIG(at->flags))
    {
        page_ptr = *at->page_table;
#if defined(NV_SG_MAP_BUFFERS)
        nv_sg_unmap_buffer(dev, &at->page_table[0]->sg_list, page_ptr);
#endif
        NV_FREE_PAGES(page_ptr->virt_addr, at->order);
    }

    return status;
}

void nv_vm_free_pages(
    nv_state_t *nv,
    nv_alloc_t *at
)
{
    nv_pte_t *page_ptr;
    unsigned int i;
    nv_linux_state_t *nvl;
    struct pci_dev *dev;

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: nv_vm_free_pages: %d pages\n",
        at->num_pages);

    nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    dev = nvl->dev;

    nv_vm_list_page_count(at->page_table, at->num_pages);

    for (i = 0; i < at->num_pages; i++)
    {
        page_ptr = at->page_table[i];
        if (!NV_ALLOC_MAPPING_CACHED(at->flags))
            nv_set_page_attrib_cached(page_ptr);
#if defined(NV_SG_MAP_BUFFERS)
        if (!NV_ALLOC_MAPPING_CONTIG(at->flags))
            nv_sg_unmap_buffer(dev, &page_ptr->sg_list, page_ptr);
#endif
        if (NV_GET_PAGE_COUNT(page_ptr) != page_ptr->page_count)
        {
            static int count = 0;
            if (count++ < NV_MAX_RECURRING_WARNING_MESSAGES)
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM: VM: nv_vm_free_pages: page count != initial page count (%d,%d)\n",
                    NV_GET_PAGE_COUNT(page_ptr), page_ptr->page_count);
            }
        }
        NV_UNLOCK_PAGE(page_ptr);
        if (!NV_ALLOC_MAPPING_CONTIG(at->flags))
            NV_FREE_PAGES(page_ptr->virt_addr, 0);
    }

    if (!NV_ALLOC_MAPPING_CACHED(at->flags))
        nv_flush_caches();

    if (NV_ALLOC_MAPPING_CONTIG(at->flags))
    {
        page_ptr = *at->page_table;
#if defined(NV_SG_MAP_BUFFERS)
        nv_sg_unmap_buffer(dev, &at->page_table[0]->sg_list, page_ptr);
#endif
        NV_FREE_PAGES(page_ptr->virt_addr, at->order);
    }
}

#if defined(NV_VMAP_PRESENT) && defined(KERNEL_2_4) && defined(NVCPU_X86)
static unsigned long
nv_vmap_vmalloc(
    int count,
    struct page **pages,
    pgprot_t prot
)
{
    void *virt_addr = NULL;
    unsigned int i, size = count * PAGE_SIZE;

    NV_VMALLOC(virt_addr, size, TRUE);
    if (virt_addr == NULL)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: vmalloc() failed to allocate vmap() scratch pages!\n");
        return 0;
    }

    for (i = 0; i < (unsigned int)count; i++)
    {
        pgd_t *pgd = NULL;
        pmd_t *pmd = NULL;
        pte_t *pte = NULL;
        unsigned long address;
        struct page *page;

        address = (unsigned long)virt_addr + i * PAGE_SIZE; 

        pgd = NV_PGD_OFFSET(address, 1, NULL);
        if (!NV_PGD_PRESENT(pgd))
            goto failed;

        pmd = NV_PMD_OFFSET(address, pgd);
        if (!NV_PMD_PRESENT(pmd))
            goto failed;

        pte = NV_PTE_OFFSET(address, pmd);
        if (!NV_PTE_PRESENT(pte))
            goto failed;

        page = NV_GET_PAGE_STRUCT(pte_val(*pte) & NV_PAGE_MASK);
        get_page(pages[i]);
        set_pte(pte, mk_pte(pages[i], prot));
        put_page(page);
        NV_PTE_UNMAP(pte);
    }
    nv_flush_caches();

    return (unsigned long)virt_addr;

failed:
    NV_VFREE(virt_addr, size);

    return 0;
}

static void
nv_vunmap_vmalloc(
    void *address,
    int count
)
{
    NV_VFREE(address, count * PAGE_SIZE);
}
#endif /* NV_VMAP_PRESENT && KERNEL_2_4 && NVCPU_X86 */

void *nv_vmap(
    struct page **pages,
    int count,
    pgprot_t prot
)
{
    unsigned long virt_addr = 0;
#if defined(NV_VMAP_PRESENT)
#if defined(KERNEL_2_4) && defined(NVCPU_X86)
    /*
     * XXX Linux 2.4's vmap() checks the requested mapping's size against
     * the value of (max_mapnr << PAGESHIFT); since 'map_nr' is a 32-bit
     * symbol, the checks fails given enough physical memory. We can't solve
     * this problem by adjusting the value of 'map_nr', but we can avoid
     * vmap() by going through vmalloc().
     */
    if (max_mapnr >= 0x100000)
        virt_addr = nv_vmap_vmalloc(count, pages, prot);
    else
#endif
        NV_VMAP_KERNEL(virt_addr, pages, count, prot);
#if defined(KERNEL_2_4)
    if (virt_addr)
    {
        int i;
        /*
         * XXX Linux 2.4's vmap() increments the pages' reference counts
         * in preparation for vfree(); the latter skips the calls to
         * __free_page() if the pages are marked reserved, however, so
         * that the underlying memory is effectively leaked when we free
         * it later. Decrement the count here to avoid this leak.
         */
        for (i = 0; i < count; i++)
        {
            if (PageReserved(pages[i]))
                atomic_dec(&pages[i]->count);
        }
    }
#endif
#endif /* NV_VMAP_PRESENT */
    return (void *)virt_addr;
}

void nv_vunmap(
    void *address,
    int count
)
{
#if defined(NV_VMAP_PRESENT)
#if defined(KERNEL_2_4) && defined(NVCPU_X86)
    /*
     * XXX Linux 2.4's vmap() checks the requested mapping's size against
     * the value of (max_mapnr << PAGESHIFT); since 'map_nr' is a 32-bit
     * symbol, the checks fails given enough physical memory. We can't solve
     * this problem by adjusting the value of 'map_nr', but we can avoid
     * vmap() by going through vmalloc().
     */
    if (max_mapnr >= 0x100000)
        nv_vunmap_vmalloc(address, count);
    else
#endif
        NV_VUNMAP_KERNEL(address, count);
#endif /* NV_VMAP_PRESENT */
}

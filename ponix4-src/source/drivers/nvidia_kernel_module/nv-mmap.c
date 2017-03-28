/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 1999-2011 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */

#define  __NO_VERSION__
#include "nv-misc.h"

#include "os-interface.h"
#include "nv-linux.h"

/*
 * The 'struct vm_operations' open() callback is called by the Linux
 * kernel when the parent VMA is split or copied, close() when the
 * current VMA is about to be deleted.
 *
 * We implement these callbacks to keep track of the number of user
 * mappings of system memory allocations. This was motivated by a
 * subtle interaction problem between the driver and the kernel with
 * respect to the bookkeeping of pages marked reserved and later
 * mapped with mmap().
 *
 * Traditionally, the Linux kernel ignored reserved pages, such that
 * when they were mapped via mmap(), the integrity of their usage
 * counts depended on the reserved bit being set for as long as user
 * mappings existed.
 *
 * Since we mark system memory pages allocated for DMA reserved and
 * typically map them with mmap(), we need to ensure they remain
 * reserved until the last mapping has been torn down. This worked
 * correctly in most cases, but in a few, the RM API called into the
 * RM to free memory before calling munmap() to unmap it.
 *
 * In the past, we allowed nv_free_pages() to remove the 'at' from
 * the parent device's allocation list in this case, but didn't
 * release the underlying pages until the last user mapping had been
 * destroyed:
 *
 * In nv_kern_vma_release(), we freed any resources associated with
 * the allocation (IOMMU/SWIOTLB mappings, etc.) and cleared the
 * underlying pages' reserved bits, but didn't free them. The kernel
 * was expected to do this.
 *
 * This worked in practise, but made dangerous assumptions about the
 * kernel's behavior and could fail in some cases. We now handle
 * this case differently (see below).
 */
static void
nv_kern_vma_open(struct vm_area_struct *vma)
{
    NV_PRINT_VMA(NV_DBG_MEMINFO, vma);

    if (NV_VMA_PRIVATE(vma))
    {
        nv_alloc_t *at = (nv_alloc_t *)NV_VMA_PRIVATE(vma);
        NV_ATOMIC_INC(at->usage_count);

        if (!NV_ALLOC_MAPPING_AGP(at->flags))
        {
            NV_PRINT_AT(NV_DBG_MEMINFO, at);
            nv_vm_list_page_count(at->page_table, at->num_pages);
        }
    }
}

/*
 * (see above for additional information)
 *
 * If the 'at' usage count drops to zero with the updated logic, the
 * VMA's file pointer is saved; nv_kern_close() uses it to find
 * these allocations when the parent file descriptor is closed. This
 * will typically happen when the process exits.
 *
 * Since this is technically a workaround to handle possible fallout
 * from misbehaving clients, we addtionally print a warning.
 */
static void
nv_kern_vma_release(struct vm_area_struct *vma)
{
    NV_PRINT_VMA(NV_DBG_MEMINFO, vma);

    if (NV_VMA_PRIVATE(vma))
    {
        nv_alloc_t *at = (nv_alloc_t *) NV_VMA_PRIVATE(vma);

        if (NV_ATOMIC_DEC_AND_TEST(at->usage_count))
        {
            static int count = 0;
            if ((at->pid == os_get_current_process()) &&
                (count++ < NV_MAX_RECURRING_WARNING_MESSAGES))
            {
                nv_printf(NV_DBG_MEMINFO,
                    "NVRM: VM: %s: late unmap, comm: %s, 0x%p\n",
                    __FUNCTION__, current->comm, at);
            }
            at->file = NV_VMA_FILE(vma);
        }

        if (!NV_ALLOC_MAPPING_AGP(at->flags))
        {
            NV_PRINT_AT(NV_DBG_MEMINFO, at);
            nv_vm_list_page_count(at->page_table, at->num_pages);
        }
    }
}

#if !defined(NV_VM_INSERT_PAGE_PRESENT)
static
struct page *nv_kern_vma_nopage(
    struct vm_area_struct *vma,
    unsigned long address,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 1))
    int *type
#else
    int write_access
#endif
)
{
    struct page *page;

    page = pfn_to_page(vma->vm_pgoff);
    get_page(page);

    return page;
}
#endif

struct vm_operations_struct nv_vm_ops = {
    .open   = nv_kern_vma_open,
    .close  = nv_kern_vma_release,
#if !defined(NV_VM_INSERT_PAGE_PRESENT)
    .nopage = nv_kern_vma_nopage,
#endif
};

int nv_encode_caching(
    pgprot_t *prot,
    NvU32     cache_type,
    NvU32     memory_type
)
{
    pgprot_t tmp;

    if (prot == NULL)
    {
        tmp = __pgprot(0);
        prot = &tmp;
    }

    switch (cache_type)
    {
        case NV_MEMORY_UNCACHED_WEAK:
#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
            *prot = pgprot_noncached_weak(*prot);
            break;
#endif
        case NV_MEMORY_UNCACHED:
            *prot = pgprot_noncached(*prot);
            break;
#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
        case NV_MEMORY_WRITECOMBINED:
#if defined(NV_ENABLE_PAT_SUPPORT)
            if ((nv_pat_mode != NV_PAT_MODE_DISABLED) &&
                    (memory_type != NV_MEMORY_TYPE_REGISTERS))
            {
                pgprot_val(*prot) &= ~(_PAGE_PSE | _PAGE_PCD | _PAGE_PWT);
                *prot = __pgprot(pgprot_val(*prot) | _PAGE_PWT);
                break;
            }
#endif
            /*
             * If PAT support is unavailable and the memory space isn't
             * NV_MEMORY_TYPE_AGP, we need to return an error code to
             * the caller, but do not print a warning message.
             *
             * In the case of AGP memory, we will have attempted to add
             * a WC MTRR for the AGP aperture and aborted the AGP
             * initialization if this failed, so we can safely return
             * success here.
             *
             * For frame buffer memory, callers are expected to use the
             * UC- memory type if we report WC as unsupported, which
             * translates to the effective memory type WC if a WC MTRR
             * exists or else UC.
             */
            if (memory_type == NV_MEMORY_TYPE_AGP)
                break;
            return 1;
#endif
        case NV_MEMORY_CACHED:
#if !defined(NVCPU_X86) && !defined(NVCPU_X86_64)
            if (memory_type != NV_MEMORY_TYPE_REGISTERS)
                break;
#else
            /*
             * RAM is cached on Linux by default, we can assume there's
             * nothing to be done here. This is not the case for the
             * other memory spaces: as commented on above, we will have
             * added a WC MTRR for the AGP aperture (or else aborted
             * AGP initialization), and we will have made an attempt to
             * add a WC MTRR for the frame buffer.
             *
             * If a WC MTRR is present, we can't satisfy the WB mapping
             * attempt here, since the achievable effective memory
             * types in that case are WC and UC, if not it's typically
             * UC (MTRRdefType is UC); we could only satisfy WB mapping
             * requests with a WB MTRR.
             */
            if (memory_type == NV_MEMORY_TYPE_SYSTEM)
                break;
#endif
        default:
            nv_printf(NV_DBG_ERRORS,
                "NVRM: VM: cache type %d not supported for memory type %d!\n",
                cache_type, memory_type);
            return 1;
    }
    return 0;
}

int nv_kern_mmap(
    struct file *file,
    struct vm_area_struct *vma
)
{
    unsigned int pages;
    nv_alloc_t *at;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_FILEP(file);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nv_file_private_t *nvfp = NV_GET_FILE_PRIVATE(file);
    int status = 0;
    nv_stack_t *sp = NULL;
    NvU32 i;

    if (nv->flags & NV_FLAG_CONTROL)
        return -ENODEV;

    NV_PRINT_VMA(NV_DBG_MEMINFO, vma);

    down(&nvfp->fops_sp_lock[NV_FOPS_STACK_INDEX_MMAP]);
    sp = nvfp->fops_sp[NV_FOPS_STACK_INDEX_MMAP];

    NV_CHECK_PCI_CONFIG_SPACE(sp, nv, TRUE, TRUE, NV_MAY_SLEEP());

    pages = NV_VMA_SIZE(vma) >> PAGE_SHIFT;
    NV_VMA_PRIVATE(vma) = NULL;

    vma->vm_ops = &nv_vm_ops;

    if (IS_REG_OFFSET(nv, NV_VMA_OFFSET(vma), NV_VMA_SIZE(vma)))
    {
        if (nv_encode_caching(&vma->vm_page_prot,
                              NV_MEMORY_UNCACHED,
                              NV_MEMORY_TYPE_REGISTERS))
        {
            status = -ENXIO;
            goto done;
        }

        if (NV_IO_REMAP_PAGE_RANGE(vma->vm_start, NV_VMA_OFFSET(vma),
                    NV_VMA_SIZE(vma), vma->vm_page_prot))
        {
            status = -EAGAIN;
            goto done;
        }

        vma->vm_flags |= VM_IO;
    }
    else if (IS_FB_OFFSET(nv, NV_VMA_OFFSET(vma), NV_VMA_SIZE(vma)))
    {
        if (IS_UD_OFFSET(nv, NV_VMA_OFFSET(vma), NV_VMA_SIZE(vma)))
        {
            if (nv_encode_caching(&vma->vm_page_prot,
                                      NV_MEMORY_UNCACHED,
                                      NV_MEMORY_TYPE_FRAMEBUFFER))
            {
                status = -ENXIO;
                goto done;
            }
        }
        else if (nv_encode_caching(&vma->vm_page_prot,
                                   NV_MEMORY_WRITECOMBINED,
                                   NV_MEMORY_TYPE_FRAMEBUFFER))
        {
            if (nv_encode_caching(&vma->vm_page_prot,
                                  NV_MEMORY_UNCACHED_WEAK,
                                  NV_MEMORY_TYPE_FRAMEBUFFER))
            {
                status = -ENXIO;
                goto done;
            }
        }

        if (NV_IO_REMAP_PAGE_RANGE(vma->vm_start, NV_VMA_OFFSET(vma),
                    NV_VMA_SIZE(vma), vma->vm_page_prot))
        {
            status = -EAGAIN;
            goto done;
        }

        vma->vm_flags |= VM_IO;
    }
#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
    else if (IS_AGP_OFFSET(nv, NV_VMA_OFFSET(vma), NV_VMA_SIZE(vma)))
    {
        down(&nvl->at_lock);
        at = nv_find_alloc(nvl, NV_VMA_OFFSET(vma), NV_ALLOC_TYPE_AGP);

        if (at == NULL)
        {
            static int count = 0;
            if (count++ < NV_MAX_RECURRING_WARNING_MESSAGES)
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM: %s: invalid offset: 0x%08x @ 0x%016llx (AGP)\n",
                    __FUNCTION__, NV_VMA_SIZE(vma),
                    NV_VMA_OFFSET(vma));
            }
            up(&nvl->at_lock);
            status = -EINVAL;
            goto done;
        }

        if (nv_encode_caching(&vma->vm_page_prot,
                              NV_MEMORY_WRITECOMBINED,
                              NV_MEMORY_TYPE_AGP))
        {
            up(&nvl->at_lock);
            status = -ENXIO;
            goto done;
        }

        NV_VMA_PRIVATE(vma) = at;
        NV_ATOMIC_INC(at->usage_count);
        up(&nvl->at_lock);

        if (NV_IO_REMAP_PAGE_RANGE(vma->vm_start, NV_VMA_OFFSET(vma),
                    NV_VMA_SIZE(vma), vma->vm_page_prot))
        {
            NV_ATOMIC_DEC(at->usage_count);
            status = -EAGAIN;
            goto done;
        }

        i = ((NV_VMA_OFFSET(vma) - at->key) >> PAGE_SHIFT);

        NV_PRINT_AT(NV_DBG_MEMINFO, at);
        nv_vm_list_page_count(&at->page_table[i], pages);

        vma->vm_flags |= VM_IO;
    }
#endif
    else
    {
        unsigned long start = 0;
        unsigned int i, j;

        down(&nvl->at_lock);
        at = nv_find_alloc(nvl, NV_VMA_OFFSET(vma), NV_ALLOC_TYPE_PCI);

        if (at == NULL)
        {
            static int count = 0;
            up(&nvl->at_lock);
            if (count++ < NV_MAX_RECURRING_WARNING_MESSAGES)
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM: %s: invalid offset: 0x%08x @ 0x%016llx (PCI)\n",
                    __FUNCTION__, NV_VMA_SIZE(vma),
                    NV_VMA_OFFSET(vma));
            }
            status = -EINVAL;
            goto done;
        }

        for (i = 0; i < at->num_pages; i++)
        {
            if ((NV_VMA_OFFSET(vma) == at->page_table[i]->phys_addr) ||
                (NV_VMA_OFFSET(vma) == at->page_table[i]->dma_addr))
            {
                break;
            }
        }

        if (i == at->num_pages)
        {
            up(&nvl->at_lock);
            status = -EINVAL;
            goto done;
        }

        if ((i + pages) > at->num_pages)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: requested mapping exceeds allocation's boundary!\n");
            up(&nvl->at_lock);
            status = -EINVAL;
            goto done;
        }

        if (nv_encode_caching(&vma->vm_page_prot,
                              NV_ALLOC_MAPPING(at->flags),
                              NV_MEMORY_TYPE_SYSTEM))
        {
            up(&nvl->at_lock);
            status = -ENXIO;
            goto done;
        }

        NV_VMA_PRIVATE(vma) = at;
        NV_ATOMIC_INC(at->usage_count);
        up(&nvl->at_lock);

        start = vma->vm_start;
        for (j = i; j < (i + pages); j++)
        {
            nv_verify_page_mappings(at->page_table[j],
                    NV_ALLOC_MAPPING(at->flags));
#if defined(NV_VM_INSERT_PAGE_PRESENT)
            if (NV_VM_INSERT_PAGE(vma, start,
                    NV_GET_PAGE_STRUCT(at->page_table[j]->phys_addr)))
#else
            if (NV_REMAP_PAGE_RANGE(start, at->page_table[j]->phys_addr,
                    PAGE_SIZE, vma->vm_page_prot))
#endif
            {
                NV_ATOMIC_DEC(at->usage_count);
                status = -EAGAIN;
                goto done;
            }
            start += PAGE_SIZE;
        }

        NV_PRINT_AT(NV_DBG_MEMINFO, at);
        nv_vm_list_page_count(&at->page_table[i], pages);

        vma->vm_flags |= (VM_IO | VM_LOCKED | VM_RESERVED);

#if defined(VM_DRIVER_PAGES)
        vma->vm_flags |= VM_DRIVER_PAGES;
#endif
    }

    NV_VMA_FILE(vma) = file;

done:
    up(&nvfp->fops_sp_lock[NV_FOPS_STACK_INDEX_MMAP]);
    return status;
}

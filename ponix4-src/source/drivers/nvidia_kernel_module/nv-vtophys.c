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

NvU64 nv_get_phys_address(
    NvU64 address,
    BOOL kern
)
{
#if defined(NV_SET_PAGES_UC_PRESENT)
    nv_printf(NV_DBG_ERRORS,
        "NVRM: can't translate address in %s()!\n", __FUNCTION__);
#else
    struct mm_struct *mm;
    pgd_t *pgd = NULL;
    pmd_t *pmd = NULL;
    pte_t *pte = NULL;
    NvU64 retval;

    if (!kern)
    {
        mm = current->mm;
        down_read(&mm->mmap_sem);
    }
    else
        mm = NULL;

    pgd = NV_PGD_OFFSET(address, kern, mm);
    if (!NV_PGD_PRESENT(pgd))
        goto failed;

    pmd = NV_PMD_OFFSET(address, pgd);
    if (!NV_PMD_PRESENT(pmd))
        goto failed;

    pte = NV_PTE_OFFSET(address, pmd);
    if (!NV_PTE_PRESENT(pte))
        goto failed;

    retval = ((NV_PTE_VALUE(pte) & NV_PAGE_MASK) | (address & ~NV_PAGE_MASK));

#if defined(NVCPU_X86_64) && defined(_PAGE_NX)
    retval &= ~_PAGE_NX;
#endif

    if (!kern)
        up_read(&mm->mmap_sem);

    return retval;

failed:
    if (!kern)
        up_read(&mm->mmap_sem);
#endif
    return 0;
}

NvU64 NV_API_CALL nv_get_kern_phys_address(NvU64 address)
{
    /* make sure this address is a kernel virtual address */
#if defined(DEBUG) && !defined(CONFIG_X86_4G)
    if (address < PAGE_OFFSET)
    {
        nv_printf(NV_DBG_WARNINGS,
            "NVRM: user address passed to %s: 0x%llx!\n",
            __FUNCTION__, address);
        return 0;
    }
#endif

    /* direct-mapped kernel address */
    if ((address > PAGE_OFFSET) && (address < VMALLOC_START))
        return __pa(address);

    return nv_get_phys_address(address, TRUE);
}

NvU64 NV_API_CALL nv_get_user_address(NvU64 address)
{
    /* make sure this address is not a kernel virtual address */
#if defined(DEBUG) && !defined(CONFIG_X86_4G)
    if (address >= PAGE_OFFSET)
    {
        nv_printf(NV_DBG_WARNINGS,
            "NVRM: kernel address passed to %s: 0x%llx!\n",
            __FUNCTION__, address);
        return 0;
    }
#endif

    return nv_get_phys_address(address, FALSE);
}

/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 1999-2001 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */



/******************* Operating System AGP Routines *************************\
*                                                                           *
* Module: os-agp.c                                                          *
*   interface to the operating system's native agp support                  *
*                                                                           *
\***************************************************************************/

#define  __NO_VERSION__
#include "nv-misc.h"

#include "os-interface.h"
#include "nv-linux.h"

#ifdef AGPGART

#if !defined(KERNEL_2_4)
typedef struct agp_kern_info agp_kern_info;
typedef struct agp_memory agp_memory;
typedef struct agp_bridge_data agp_bridge_data;
#elif defined(KERNEL_2_4)
const drm_agp_t *drm_agp_p; /* functions */
#endif

#define NV_AGPGART_MODE_BITS_RATE(mode,rate) \
    (((mode) & 8) ? ((((mode) & ((rate) >> 2)) & 3) | 8) : (((mode) & (rate)) & 7))
#define NV_AGPGART_MODE_BITS_FW(fw) ((fw) ? 0x10 : 0)
#define NV_AGPGART_MODE_BITS_SBA(sba) ((sba) ? 0x200 : 0)

#if !defined(KERNEL_2_4)
#if (NV_AGP_BACKEND_ACQUIRE_ARGUMENT_COUNT == 1)
#define NV_AGPGART_BACKEND_ACQUIRE(o,b,d) ({ (b) = agp_backend_acquire(d); !(b); })
#define NV_AGPGART_BACKEND_ENABLE(o,b,mode) agp_enable(b,mode)
#define NV_AGPGART_BACKEND_RELEASE(o,b) agp_backend_release(b)
#define NV_AGPGART_COPY_INFO(o,b,p) agp_copy_info(b,p)
#define NV_AGPGART_ALLOCATE_MEMORY(o,b,count,type) agp_allocate_memory(b,count,type)
#else
#define NV_AGPGART_BACKEND_ACQUIRE(o,b,d) agp_backend_acquire()
#define NV_AGPGART_BACKEND_ENABLE(o,b,mode) agp_enable(mode)
#define NV_AGPGART_BACKEND_RELEASE(o,b) agp_backend_release()
#define NV_AGPGART_COPY_INFO(o,b,p) agp_copy_info(p)
#define NV_AGPGART_ALLOCATE_MEMORY(o,b,count,type) agp_allocate_memory(count,type)
#endif
#define NV_AGPGART_FREE_MEMORY(o,p) agp_free_memory(p)
#define NV_AGPGART_BIND_MEMORY(o,p,offset) agp_bind_memory(p,offset)
#define NV_AGPGART_UNBIND_MEMORY(o,p) agp_unbind_memory(p)
#elif defined(KERNEL_2_4)
#define NV_AGPGART_BACKEND_ACQUIRE(o,b,d) ({ (o)->acquire(); 0; })
#define NV_AGPGART_BACKEND_ENABLE(o,b,mode) (o)->enable(mode)
#define NV_AGPGART_BACKEND_RELEASE(o,b) ((o)->release())
#define NV_AGPGART_COPY_INFO(o,b,p) ({ (o)->copy_info(p); 0; })
#define NV_AGPGART_ALLOCATE_MEMORY(o,b,count,type) (o)->allocate_memory(count,type)
#define NV_AGPGART_FREE_MEMORY(o,p) (o)->free_memory(p)
#define NV_AGPGART_BIND_MEMORY(o,p,offset) (o)->bind_memory(p,offset)
#define NV_AGPGART_UNBIND_MEMORY(o,p) (o)->unbind_memory(p)
#endif

#endif /* AGPGART */

RM_STATUS KernInitAGP(
    nv_stack_t *sp,
    nv_state_t *nv,
    NvU64      *ap_phys_base,
    NvU64      *ap_limit
)
{
#ifndef AGPGART
    return RM_ERROR;
#else
    RM_STATUS status = RM_ERROR;
    nv_linux_state_t *nvl;
    void *bitmap;
    agp_kern_info agp_info;
    NvU32 bitmap_size;

    NvU32 agp_rate    = (8 | 4 | 2 | 1);
    NvU32 enable_sba  = 0;
    NvU32 enable_fw   = 0;
    NvU32 agp_mode    = 0;

#if defined(KERNEL_2_4)
    if (!(drm_agp_p = inter_module_get_request("drm_agp", "agpgart")))
        return RM_ERR_NOT_SUPPORTED;
#endif

    /* NOTE: from here down, return an error code of '-1'
     * that indicates that agpgart is loaded, but we failed to use it
     * in some way. This is so we don't try to use nvagp and lock up
     * the memory controller.
     */
    nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (NV_AGPGART_BACKEND_ACQUIRE(drm_agp_p, nvl->agp_bridge, nvl->dev))
    {
        nv_printf(NV_DBG_INFO, "NVRM: AGPGART: no backend available\n");
        status = RM_ERR_NOT_SUPPORTED;
        goto bailout;
    }

    if (NV_AGPGART_COPY_INFO(drm_agp_p, nvl->agp_bridge, &agp_info))
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: AGPGART: kernel reports chipset as unsupported\n");
        goto release;
    }

    if (nv_pat_mode == NV_PAT_MODE_DISABLED)
    {
#ifdef CONFIG_MTRR
        /*
         * Failure to set a write-combining range on the AGP aperture may
         * be due to the presence of other memory ranges with conflicting
         * caching  attributes. Play safe and fail AGP initialization.
         */
        if (mtrr_add(agp_info.aper_base, agp_info.aper_size << 20,
                MTRR_TYPE_WRCOMB, 0) < 0)
#endif
        {
            nv_printf(NV_DBG_ERRORS, 
                "NVRM: AGPGART: unable to set MTRR write-combining\n");
            goto release;
        }
    }

    // allocate and set the bitmap for tracking agp allocations
    bitmap_size = (agp_info.aper_size << 20)/PAGE_SIZE/8;
    if (os_alloc_mem(&bitmap, bitmap_size))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: AGPGART: unable to allocate bitmap\n");
        goto failed;
    }

    os_mem_set(bitmap, 0xff, bitmap_size);
    status = rm_set_agp_bitmap(sp, nv, bitmap);
    if (status != RM_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: AGPGART: unable to set bitmap\n");
        os_free_mem(bitmap);
        goto failed;
    }

    agp_mode = agp_info.mode;
    rm_read_registry_dword(sp, NULL, "NVreg", "ReqAGPRate", &agp_rate);

    agp_mode = NV_AGPGART_MODE_BITS_RATE(agp_mode, agp_rate);
    agp_mode |= 1; /* avoid 0x mode request */

    if (agp_mode & 2) agp_mode &= ~1;
    if (agp_mode & 4) agp_mode &= ~2;

    rm_read_registry_dword(sp, NULL, "NVreg", "EnableAGPSBA", &enable_sba);
    agp_mode |= NV_AGPGART_MODE_BITS_SBA(enable_sba);

    rm_read_registry_dword(sp, NULL, "NVreg", "EnableAGPFW", &enable_fw);
    agp_mode |= NV_AGPGART_MODE_BITS_FW(enable_fw);

    agp_info.mode &= (0xff000000 | agp_mode);
    NV_AGPGART_BACKEND_ENABLE(drm_agp_p, nvl->agp_bridge, agp_info.mode);

    *ap_phys_base = (unsigned)agp_info.aper_base;
    *ap_limit = (unsigned)((agp_info.aper_size << 20) - 1);

    return RM_OK;

failed:
#ifdef CONFIG_MTRR
    if (nv_pat_mode == NV_PAT_MODE_DISABLED)
        mtrr_del(-1, agp_info.aper_base, agp_info.aper_size << 20);
#endif
release:
    NV_AGPGART_BACKEND_RELEASE(drm_agp_p, nvl->agp_bridge);
bailout:
#if defined(KERNEL_2_4)
    inter_module_put("drm_agp");
#endif

    return status;

#endif /* AGPGART */
}

RM_STATUS KernTeardownAGP(
    nv_stack_t *sp,
    nv_state_t *nv
)
{
#ifndef AGPGART
    return RM_ERROR;
#else
    RM_STATUS status;
    nv_linux_state_t *nvl;
    void *bitmap;

    nvl = NV_GET_NVL_FROM_NV_STATE(nv);

#ifdef CONFIG_MTRR
    if (nv_pat_mode == NV_PAT_MODE_DISABLED)
        mtrr_del(-1, nv->agp.address, nv->agp.size);
#endif

    NV_AGPGART_BACKEND_RELEASE(drm_agp_p, nvl->agp_bridge);
#if defined(KERNEL_2_4)
    inter_module_put("drm_agp");
#endif

    status = rm_clear_agp_bitmap(sp, nv, &bitmap);
    if (status != RM_OK)
    {
        nv_printf(NV_DBG_WARNINGS, "NVRM: AGPGART: failed to clear bitmap\n");
        return status;
    }

    os_free_mem(bitmap);

    return RM_OK;

#endif /* AGPGART */
}


RM_STATUS KernAllocAGPPages(
    nv_stack_t  *sp,
    nv_state_t  *nv,
    NvU32        PageCount,
    void       **pPriv_data,
    NvU32       *Offset
)
{
#ifndef AGPGART
    return RM_ERROR;
#else
    nv_linux_state_t *nvl;
    agp_memory *ptr;

    nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (rm_alloc_agp_bitmap(sp, nv, PageCount, Offset))
    {
        nv_printf(NV_DBG_INFO, "NVRM: AGPGART: failed to allocate AGP offset\n");
        return RM_ERROR;
    }

    ptr = NV_AGPGART_ALLOCATE_MEMORY(drm_agp_p, nvl->agp_bridge,
                PageCount, AGP_NORMAL_MEMORY);
    if (ptr == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: AGPGART: no pages available\n");
        rm_free_agp_bitmap(sp, nv, PageCount, *Offset);
        return RM_ERR_NO_FREE_MEM;
    }
    
    if (NV_AGPGART_BIND_MEMORY(drm_agp_p, ptr, *Offset))
    {
        nv_printf(NV_DBG_INFO, "NVRM: AGPGART: unable to bind %u pages\n", PageCount);
        NV_AGPGART_FREE_MEMORY(drm_agp_p, ptr);
        rm_free_agp_bitmap(sp, nv, PageCount, *Offset);
        return RM_ERROR;
    }

    *pPriv_data = (void *)ptr;

    return RM_OK;
#endif /* AGPGART */
}

RM_STATUS KernLoadAGPPages(
    nv_state_t *nv,
    nv_alloc_t *at,
    void *priv_data
)
{
#ifndef AGPGART
    return RM_ERROR;
#else
    agp_memory *ptr = priv_data;
    unsigned int i;

    if (at->num_pages != ptr->page_count)
        return RM_ERROR;

    for (i = 0; i < at->num_pages; i++)
    {
         nv_pte_t *page_ptr = at->page_table[i];
#if defined(NV_AGP_MEMORY_HAS_PAGES)
         page_ptr->phys_addr = page_to_phys(ptr->pages[i]);
#else
         page_ptr->phys_addr = (ptr->memory[i] & NV_PAGE_MASK);
#endif
         page_ptr->virt_addr = (unsigned long) __va(page_ptr->phys_addr);
         page_ptr->dma_addr  = page_ptr->phys_addr;
    }

    return RM_OK;
#endif /* AGPGART */
}

RM_STATUS KernFreeAGPPages(
    nv_stack_t *sp,
    nv_state_t *nv,
    void       *priv_data
)
{
#ifndef AGPGART
    return RM_OK;
#else
    agp_memory *ptr = priv_data;

    rm_free_agp_bitmap(sp, nv, ptr->page_count, ptr->pg_start);

    NV_AGPGART_UNBIND_MEMORY(drm_agp_p, ptr);
    NV_AGPGART_FREE_MEMORY(drm_agp_p, ptr);

    return RM_OK;
#endif
}



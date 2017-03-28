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
#include "nv-linux.h"
#include "nv-p2p.h"
#include "os-agp.h"
#include "nv-reg.h"
#include "rmil.h"

#if defined(MODULE_LICENSE)
MODULE_LICENSE("NVIDIA");
#endif
#if defined(MODULE_INFO)
MODULE_INFO(supported, "external");
#endif
#if defined(MODULE_VERSION)
MODULE_VERSION(NV_VERSION_STRING);
#endif

#ifdef MODULE_ALIAS_CHARDEV_MAJOR
MODULE_ALIAS_CHARDEV_MAJOR(NV_MAJOR_DEVICE_NUMBER);
#endif

#if defined(KERNEL_2_4) && (defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE))
extern int i2c_add_adapter (struct i2c_adapter *) __attribute__ ((weak));
extern int i2c_del_adapter (struct i2c_adapter *) __attribute__ ((weak));
#endif

#include "patches.h"

/*
 * our global state; one per device
 */

static NvU32 num_nv_devices = 0;
static NvU32 num_probed_nv_devices = 0;

nv_linux_state_t *nv_linux_devices;
static nv_smu_state_t nv_smu_device;

#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
NvU64 __nv_supported_pte_mask = ~_PAGE_NX;
#endif

/*
 * And one for the control device
 */

nv_linux_state_t nv_ctl_device = { { 0 } };
wait_queue_head_t nv_ctl_waitqueue;

#if defined(NV_SWIOTLB)
static const char *__swiotlb_warning = \
    "You are probably using the kernel's SWIOTLB interface.\n\n"
    "Be very careful with this interface, as it is easy to exhaust\n"
    "its memory buffer, at which point it may panic the kernel.\n"
    "Please increase the default size of this buffer by specifying\n"
    "a larger buffer size with the \"swiotlb\" kernel parameter,\n"
    "e.g.: \"swiotlb=16384\".\n";
#endif

#if defined(NV_CHANGE_PAGE_ATTR_BUG_PRESENT)
static const char *__cpgattr_warning = \
    "Your Linux kernel has known problems in its implementation of\n"
    "the change_page_attr() kernel interface.\n\n"
    "The NVIDIA graphics driver will attempt to work around these\n"
    "problems, but system stability may be adversely affected.\n"
    "It is recommended that you update to Linux 2.6.11 (or a newer\n"
    "Linux kernel release).\n";

static const char *__cpgattr_warning_2 = \
    "Your Linux kernel's version and architecture indicate that it\n"
    "may have an implementation of the change_page_attr() kernel\n"
    "kernel interface known to have problems. The NVIDIA graphics\n"
    "driver made an attempt to determine whether your kernel is\n"
    "affected, but could not. It will assume the interface does not\n"
    "work correctly and attempt to employ workarounds.\n"
    "This may adversely affect system stability.\n"
    "It is recommended that you update to Linux 2.6.11 (or a newer\n"
    "Linux kernel release).\n";
#endif

static const char *__mmconfig_warning = \
    "Your current system configuration has known problems when\n"
    "accessing PCI Configuration Space that can lead to accesses\n"
    "to the PCI Configuration Space of the wrong PCI device. This\n"
    "is known to cause instabilities with the NVIDIA graphics driver.\n\n"
    "Please see the MMConfig section in the readme for more information\n"
    "on how to work around this problem.\n";

#if defined(NV_SG_MAP_BUFFERS)
int nv_swiotlb = 0;
#if defined(NV_NEED_REMAP_CHECK)
unsigned int nv_remap_count;
unsigned int nv_remap_limit;
#endif
#endif

int nv_update_memory_types = 1;
static int nv_mmconfig_failure_detected = 0;

void *nvidia_p2p_page_t_cache;
static void *nv_pte_t_cache;
void *nv_stack_t_cache;
static nv_stack_t *__nv_init_sp;

// allow an easy way to convert all debug printfs related to events
// back and forth between 'info' and 'errors'
#if defined(NV_DBG_EVENTS)
#define NV_DBG_EVENTINFO NV_DBG_ERRORS
#else
#define NV_DBG_EVENTINFO NV_DBG_INFO
#endif

//
// Attempt to determine if we are running into the MMCONFIG coherency
// issue and, if so, warn the user and stop attempting to verify
// and correct the BAR values (see NV_CHECK_PCI_CONFIG_SPACE()), so
// that we do not do more harm than good.
//
#define NV_CHECK_MMCONFIG_FAILURE(nv,bar,value)                            \
    {                                                                      \
        nv_linux_state_t *nvl;                                             \
        for (nvl = nv_linux_devices; nvl != NULL;  nvl = nvl->next)        \
        {                                                                  \
            nv_state_t *nv_tmp = NV_STATE_PTR(nvl);                        \
            if (((nv) != nv_tmp) &&                                        \
                (nv_tmp->bars[(bar)].address == (value)))                  \
            {                                                              \
                nv_prints(NV_DBG_ERRORS, __mmconfig_warning);              \
                nv_procfs_add_warning("mmconfig", __mmconfig_warning);     \
                nv_mmconfig_failure_detected = 1;                          \
                return;                                                    \
            }                                                              \
        }                                                                  \
    }

static void
verify_pci_bars(
    nv_state_t  *nv,
    void        *dev_handle
)
{
    NvU32 bar, bar_hi, bar_lo;

    //
    // If an MMCONFIG specific failure was detected, skip the
    // PCI BAR verification to avoid overwriting the BAR(s)
    // of a given device with those of this GPU. See above for
    // more information.
    //
    if (nv_mmconfig_failure_detected)
        return;

    for (bar = 0; bar < NV_GPU_NUM_BARS; bar++)
    {
        nv_aperture_t *tmp = &nv->bars[bar];

        bar_lo = bar_hi = 0;
        if (tmp->offset == 0)
            continue;

        bar_lo = os_pci_read_dword(dev_handle, tmp->offset);

        if ((bar_lo & NVRM_PCICFG_BAR_ADDR_MASK)
                != (tmp->address & 0xffffffff))
        {
            nv_printf(NV_DBG_USERERRORS,
                "NVRM: BAR%u(L) is 0x%08x, will restore to 0x%08llx.\n",
                bar, bar_lo, (tmp->address & 0xffffffff));

            NV_CHECK_MMCONFIG_FAILURE(nv, bar,
                    (bar_lo & NVRM_PCICFG_BAR_ADDR_MASK));

            os_pci_write_dword(dev_handle, tmp->offset, tmp->address);
        }

        if ((bar_lo & NVRM_PCICFG_BAR_MEMTYPE_MASK)
                != NVRM_PCICFG_BAR_MEMTYPE_64BIT)
            continue;

        bar_hi = os_pci_read_dword(dev_handle, (tmp->offset + 4));

        if (bar_hi != (tmp->address >> 32))
        {
            nv_printf(NV_DBG_USERERRORS,
                "NVRM: BAR%u(H) is 0x%08x, will restore to 0x%08llx.\n",
                bar, bar_hi, (tmp->address >> 32));

            os_pci_write_dword(dev_handle, (tmp->offset + 4),
                    (tmp->address >> 32));
        }
    }
}

void nv_check_pci_config_space(nv_state_t *nv, BOOL check_the_bars)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    unsigned short cmd, flag = 0;

    // don't do this on the control device, only the actual devices
    if (nv->flags & NV_FLAG_CONTROL)
        return;

    pci_read_config_word(nvl->dev, PCI_COMMAND, &cmd);
    if (!(cmd & PCI_COMMAND_MASTER))
    {
        nv_printf(NV_DBG_USERERRORS, "NVRM: restoring bus mastering!\n");
        cmd |= PCI_COMMAND_MASTER;
        flag = 1;
    }

    if (!(cmd & PCI_COMMAND_MEMORY))
    {
        nv_printf(NV_DBG_USERERRORS, "NVRM: restoring MEM access!\n");
        cmd |= PCI_COMMAND_MEMORY;
        flag = 1;
    }

    if (cmd & PCI_COMMAND_SERR)
    {
        nv_printf(NV_DBG_USERERRORS, "NVRM: clearing SERR enable bit!\n");
        cmd &= ~PCI_COMMAND_SERR;
        flag = 1;
    }

    if (cmd & PCI_COMMAND_INTX_DISABLE)
    {
        nv_printf(NV_DBG_USERERRORS, "NVRM: clearing INTx disable bit!\n");
        cmd &= ~PCI_COMMAND_INTX_DISABLE;
        flag = 1;
    }

    if (flag)
        pci_write_config_word(nvl->dev, PCI_COMMAND, cmd);

    if (check_the_bars && NV_MAY_SLEEP() && !(nv->flags & NV_FLAG_PASSTHRU))
        verify_pci_bars(nv, nvl->dev);
}

void NV_API_CALL nv_verify_pci_config(nv_state_t *nv, BOOL check_the_bars)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    nv_stack_t *sp = nvl->pci_cfgchk_sp;

    NV_CHECK_PCI_CONFIG_SPACE(sp, nv, check_the_bars, FALSE, NV_MAY_SLEEP());
}

/***
 *** STATIC functions, only in this file
 ***/

/* nvos_ functions.. do not take a state device parameter  */
static int      nvos_count_devices(nv_stack_t *);

static nv_alloc_t  *nvos_create_alloc(struct pci_dev *, int);
static int          nvos_free_alloc(nv_alloc_t *);

/* nvl_ functions.. take a linux state device pointer */
static int          nvl_add_alloc(nv_linux_state_t *, nv_alloc_t *);
static int          nvl_remove_alloc(nv_linux_state_t *, nv_alloc_t *);

/* lock-related functions that should only be called from this file */
static void nv_lock_init_locks(nv_state_t *nv);


/***
 *** EXPORTS to Linux Kernel
 ***/

int           nv_kern_open(struct inode *, struct file *);
int           nv_kern_close(struct inode *, struct file *);
unsigned int  nv_kern_poll(struct file *, poll_table *);
int           nv_kern_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
long          nv_kern_unlocked_ioctl(struct file *, unsigned int, unsigned long);
long          nv_kern_compat_ioctl(struct file *, unsigned int, unsigned long);
void          nv_kern_isr_bh(unsigned long);
#if !defined(NV_IRQ_HANDLER_T_PRESENT) || (NV_IRQ_HANDLER_T_ARGUMENT_COUNT == 3)
irqreturn_t   nv_kern_isr(int, void *, struct pt_regs *);
#else
irqreturn_t   nv_kern_isr(int, void *);
#endif
void          nv_kern_rc_timer(unsigned long);
#if defined(NV_PM_SUPPORT_OLD_STYLE_APM)
static int    nv_kern_apm_event(struct pm_dev *, pm_request_t, void *);
#endif

int           nv_kern_ctl_open(struct inode *, struct file *);
int           nv_kern_ctl_close(struct inode *, struct file *);

int nv_kern_probe(struct pci_dev *, const struct pci_device_id *);
int nv_kern_smu_probe(struct pci_dev *);

#if defined(NV_PM_SUPPORT_DEVICE_DRIVER_MODEL)
static int    nv_kern_suspend(struct pci_dev *, pm_message_t);
static int    nv_kern_smu_suspend(void);
static int    nv_kern_resume(struct pci_dev *);
static int    nv_kern_smu_resume(void);
#endif

/***
 *** see nv.h for functions exported to other parts of resman
 ***/

static struct pci_device_id nv_pci_table[] = {
    { 
        .vendor      = PCI_VENDOR_ID_NVIDIA,
        .device      = PCI_ANY_ID,
        .subvendor   = PCI_ANY_ID,
        .subdevice   = PCI_ANY_ID,
        .class       = (PCI_CLASS_DISPLAY_VGA << 8),
        .class_mask  = ~0
    },
    { 
        .vendor      = PCI_VENDOR_ID_NVIDIA,
        .device      = PCI_ANY_ID,
        .subvendor   = PCI_ANY_ID,
        .subdevice   = PCI_ANY_ID,
        .class       = (PCI_CLASS_DISPLAY_3D << 8),
        .class_mask  = ~0
    },
    { 
        .vendor      = PCI_VENDOR_ID_NVIDIA,
        .device      = NV_PCI_DEVICE_ID_SMU,
        .subvendor   = PCI_ANY_ID,
        .subdevice   = PCI_ANY_ID,
        .class       = (PCI_CLASS_PROCESSOR_CO << 8), /* SMU device class */
        .class_mask  = ~0
    },
    { 
        .vendor      = PCI_VENDOR_ID_NVIDIA,
        .device      = 0x0e00,
        .subvendor   = PCI_ANY_ID,
        .subdevice   = PCI_ANY_ID,
        .class       = (PCI_CLASS_MULTIMEDIA_OTHER << 8),
        .class_mask  = ~0
    },
    { }
};

MODULE_DEVICE_TABLE(pci, nv_pci_table);

static struct pci_driver nv_pci_driver = {
    .name     = "nvidia",
    .id_table = nv_pci_table,
    .probe    = nv_kern_probe,
#if defined(NV_PM_SUPPORT_DEVICE_DRIVER_MODEL)
    .suspend  = nv_kern_suspend,
    .resume   = nv_kern_resume,
#endif
};

/* character driver entry points */

static struct file_operations nv_fops = {
    .owner     = THIS_MODULE,
    .poll      = nv_kern_poll,
#if defined(NV_FILE_OPERATIONS_HAS_IOCTL)
    .ioctl     = nv_kern_ioctl,
#endif
#if defined(NV_FILE_OPERATIONS_HAS_UNLOCKED_IOCTL)
    .unlocked_ioctl = nv_kern_unlocked_ioctl,
#endif
#if defined(NVCPU_X86_64) && defined(NV_FILE_OPERATIONS_HAS_COMPAT_IOCTL)
    .compat_ioctl = nv_kern_compat_ioctl,
#endif
#if !defined(NV_VMWARE)
    .mmap      = nv_kern_mmap,
#endif
    .open      = nv_kern_open,
    .release   = nv_kern_close,
};

#if defined(VM_CHECKER)
/* kernel virtual memory usage/allocation information */
NvU32 vm_usage = 0;
struct mem_track_t *vm_list = NULL;
nv_spinlock_t vm_lock;
#endif

#if defined(KM_CHECKER)
/* kernel logical memory usage/allocation information */
NvU32 km_usage = 0;
struct mem_track_t *km_list = NULL;
nv_spinlock_t km_lock;
#endif


/***
 *** STATIC functions
 ***/

static 
nv_alloc_t *nvos_create_alloc(
    struct pci_dev *dev,
    int num_pages
)
{
    nv_alloc_t *at;
    unsigned int pt_size, i;

    NV_KMALLOC(at, sizeof(nv_alloc_t));
    if (at == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate alloc info\n");
        return NULL;
    }

    memset(at, 0, sizeof(nv_alloc_t));

    pt_size = num_pages *  sizeof(nv_pte_t *);
    if (os_alloc_mem((void **)&at->page_table, pt_size) != RM_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate page table\n");
        NV_KFREE(at, sizeof(nv_alloc_t));
        return NULL;
    }

    memset(at->page_table, 0, pt_size);
    at->num_pages = num_pages;
    NV_ATOMIC_SET(at->usage_count, 0);

    for (i = 0; i < at->num_pages; i++)
    {
        NV_KMEM_CACHE_ALLOC(at->page_table[i], nv_pte_t_cache, nv_pte_t);
        if (at->page_table[i] == NULL)
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: failed to allocate page table entry\n");
            nvos_free_alloc(at);
            return NULL;
        }
        memset(at->page_table[i], 0, sizeof(nv_pte_t));
    }

    at->pid = os_get_current_process();

    return at;
}

static 
int nvos_free_alloc(
    nv_alloc_t *at
)
{
    unsigned int i;

    if (at == NULL)
        return -1;

    if (NV_ATOMIC_READ(at->usage_count))
        return 1;

    for (i = 0; i < at->num_pages; i++)
    {
        if (at->page_table[i] != NULL)
            NV_KMEM_CACHE_FREE(at->page_table[i], nv_pte_t, nv_pte_t_cache);
    }
    os_free_mem(at->page_table);

    NV_KFREE(at, sizeof(nv_alloc_t));

    return 0;
}

NvU8 nv_find_pci_capability(struct pci_dev *dev, NvU8 capability)
{
    u16 status;
    u8  cap_ptr, cap_id;

    pci_read_config_word(dev, PCI_STATUS, &status);
    status &= PCI_STATUS_CAP_LIST;
    if (!status)
        return 0;

    switch (dev->hdr_type) {
        case PCI_HEADER_TYPE_NORMAL:
        case PCI_HEADER_TYPE_BRIDGE:
            pci_read_config_byte(dev, PCI_CAPABILITY_LIST, &cap_ptr);
            break;
        default:
            return 0;
    }

    do {
        cap_ptr &= 0xfc;
        pci_read_config_byte(dev, cap_ptr + PCI_CAP_LIST_ID, &cap_id);
        if (cap_id == capability)
            return cap_ptr;
        pci_read_config_byte(dev, cap_ptr + PCI_CAP_LIST_NEXT, &cap_ptr);
    } while (cap_ptr && cap_id != 0xff);

    return 0;
}

void *nv_find_alloc(
    nv_linux_state_t *nvl,
    NvU64             address,
    NvU32             flags
)
{
    nv_alloc_t *at;

    for (at = nvl->alloc_queue; at; at = at->next)
    {
        unsigned int i;

        // make sure this 'at' matches the flags the caller provided
        // ie, don't mistake a pci allocation with an agp allocation
        if (!(at->flags & flags))
            continue;

        // most mappings will be found based on the 'key'
        if (address == at->key)
            return at;

        // if agp, allow the address to fall within this range 
        if (NV_ALLOC_MAPPING_AGP(at->flags) &&
            (address >= at->key) &&
            ((address + PAGE_SIZE) <= (at->key + at->num_pages * PAGE_SIZE)))
            return at;

        for (i = 0; i < at->num_pages; i++)
        {
            nv_pte_t *page_ptr = at->page_table[i];

            if ((address >= page_ptr->phys_addr) &&
                    ((address - page_ptr->phys_addr) < PAGE_SIZE))
                return at;
            if ((address >= page_ptr->dma_addr) &&
                    ((address - page_ptr->dma_addr) < PAGE_SIZE))
                return at;
        }
    }

    /* failure is not necessarily an error if the caller
       was just probing an address */
    nv_printf(NV_DBG_INFO, "NVRM: could not find map for vm 0x%lx\n", address);
    return NULL;
}

static int nvl_add_alloc(
    nv_linux_state_t *nvl, 
    nv_alloc_t *at
)
{
    down(&nvl->at_lock);
    at->next = nvl->alloc_queue;
    nvl->alloc_queue = at;
    up(&nvl->at_lock);
    return 0;
}

static int nvl_remove_alloc(
    nv_linux_state_t *nvl, 
    nv_alloc_t *at
)
{
    nv_alloc_t *tmp, *prev;

    if (nvl->alloc_queue == at)
    {
        nvl->alloc_queue = nvl->alloc_queue->next;
        return 0;
    }

    for (tmp = prev = nvl->alloc_queue; tmp; prev = tmp, tmp = tmp->next)
    {
        if (tmp == at)
        {
            prev->next = tmp->next;
            return 0;
        }
    }

    return -1;
}

#if defined(NV_CHANGE_PAGE_ATTR_BUG_PRESENT)
/*
 * nv_verify_cpa_interface() - determine if the change_page_attr() large page
 * management accounting bug known to exist in early Linux/x86-64 kernels
 * is present in this kernel.
 *
 * There's really no good way to determine if change_page_attr() is working
 * correctly. We can't reliably use change_page_attr() on Linux/x86-64 2.6
 * kernels < 2.6.11: if we run into the accounting bug, the Linux kernel will
 * trigger a BUG() if we attempt to restore the WB memory type of a page
 * originally part of a large page.
 *
 * So if we can successfully allocate such a page, change its memory type to
 * UC and check if the accounting was done correctly, we can determine if
 * the change_page_attr() interface can be used safely.
 *
 * Return values:
 *    0 - test passed, the change_page_attr() interface works
 *    1 - test failed, the status is unclear
 *   -1 - test failed, the change_page_attr() interface is broken
 */

static inline pte_t *check_large_page(unsigned long vaddr)
{
    pgd_t *pgd = NULL;
    pmd_t *pmd = NULL;

    pgd = NV_PGD_OFFSET(vaddr, 1, NULL);
    if (!NV_PGD_PRESENT(pgd))
        return NULL;

    pmd = NV_PMD_OFFSET(vaddr, pgd);
    if (!pmd || pmd_none(*pmd))
        return NULL;

    if (!pmd_large(*pmd))
        return NULL;

    return (pte_t *) pmd;
}

#define CPA_FIXED_MAX_ALLOCS 500

int nv_verify_cpa_interface(void)
{
    unsigned int i, size;
    unsigned long large_page = 0;
    unsigned long *vaddr_list;
    size = sizeof(unsigned long) * CPA_FIXED_MAX_ALLOCS;

    NV_KMALLOC(vaddr_list, size);
    if (!vaddr_list)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_verify_cpa_interface: failed to allocate "
            "page table\n");
        return 1;
    }

    memset(vaddr_list, 0, size);

    /* try to track down an allocation from a 2M page. */
    for (i = 0; i < CPA_FIXED_MAX_ALLOCS; i++)
    {
        vaddr_list[i] =  __get_free_page(GFP_KERNEL);
        if (!vaddr_list[i])
            continue;

#if defined(_PAGE_NX)
        if ((pgprot_val(PAGE_KERNEL) & _PAGE_NX) &&
                virt_to_phys((void *)vaddr_list[i]) < 0x400000)
            continue;
#endif

        if (check_large_page(vaddr_list[i]) != NULL)
        {
            large_page = vaddr_list[i];
            vaddr_list[i] = 0;
            break;
        }
    }

    for (i = 0; i < CPA_FIXED_MAX_ALLOCS; i++)
    {
        if (vaddr_list[i])
            free_page(vaddr_list[i]);
    }
    NV_KFREE(vaddr_list, size);

    if (large_page)
    {
        struct page *page = virt_to_page(large_page);
        struct page *kpte_page;
        pte_t *kpte;
        unsigned long kpte_val;
        pgprot_t prot;

        // lookup a pointer to our pte
        kpte = check_large_page(large_page);
        kpte_val = pte_val(*kpte);
        kpte_page = virt_to_page(((unsigned long)kpte) & PAGE_MASK);

        prot = PAGE_KERNEL_NOCACHE;
        pgprot_val(prot) &= __nv_supported_pte_mask;

        // this should split the large page
        change_page_attr(page, 1, prot);

        // broken kernels may get confused after splitting the page and
        // restore the page before returning to us. detect that case.
        if (((pte_val(*kpte) & ~_PAGE_NX) == kpte_val) &&
            (pte_val(*kpte) & _PAGE_PSE))
        {
            if ((pte_val(*kpte) & _PAGE_NX) &&
                    (__nv_supported_pte_mask & _PAGE_NX) == 0)
                clear_bit(_PAGE_BIT_NX, kpte);
            // don't change the page back, as it's already been reverted
            put_page(kpte_page);
            free_page(large_page);
            return -1;  // yep, we're broken
        }

        // ok, now see if our bookkeeping is broken
        if (page_count(kpte_page) != 0)
            return -1;  // yep, we're broken

        prot = PAGE_KERNEL;
        pgprot_val(prot) &= __nv_supported_pte_mask;

        // everything's ok!
        change_page_attr(page, 1, prot);
        free_page(large_page);
        return 0;
    }

    return 1;
}
#endif /* defined(NV_CHANGE_PAGE_ATTR_BUG_PRESENT) */

/*
 * nv_verify_page_mappings() - verify that the kernel mapping of the specified
 * page matches the specified type. This is to help detect bugs in the Linux
 * kernel's change_page_attr() interface, early.
 *
 * This function relies on the ability to perform kernel virtul address to PFN
 * translations and therefore on 'init_mm'. Unfortunately, the latter is no
 * longer exported in recent Linux/x86 2.6 kernels. The export was removed at
 * roughtly the same time as the set_pages_{uc,wb}() change_page_attr()
 * replacement interfaces were introduced; hopefully, it will be sufficient to
 * check for their presence.
 */
int nv_verify_page_mappings(
    nv_pte_t *page_ptr,
    unsigned int cachetype
)
{
#if defined(NV_CHANGE_PAGE_ATTR_PRESENT)
    unsigned long retval = -1;
#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
    pgd_t *pgd = NULL;
    pmd_t *pmd = NULL;
    pte_t *pte = NULL;
    unsigned int flags, expected;
    unsigned long address;
    static int count = 0;

    if (!nv_update_memory_types)
        return 0;

    address = (unsigned long)__va(page_ptr->phys_addr);

    pgd = NV_PGD_OFFSET(address, 1, NULL);
    if (!NV_PGD_PRESENT(pgd))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: pgd not present for addr 0x%lx\n", address);
        goto failed;
    }

    pmd = NV_PMD_OFFSET(address, pgd);
    if (!pmd || pmd_none(*pmd))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: pmd not present for addr 0x%lx\n", address);
        goto failed;
    }

    // account for large pages
    if (pmd_large(*pmd))
    {
        pte = (pte_t *)pmd;
        flags = pte_val(*pte) & ~(PAGE_MASK|_PAGE_PSE);
        NV_PMD_UNMAP(pmd);
    }
    else
    {
        pte = NV_PTE_OFFSET(address, pmd);
        if (!NV_PTE_PRESENT(pte))
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: pte not present for addr 0x%lx\n",
                address);
            goto failed;
        }
        flags = NV_PTE_VALUE(pte) & ~(PAGE_MASK|_PAGE_PSE);
    }

    switch (cachetype)
    {
        case NV_MEMORY_CACHED:
            expected = pgprot_val(PAGE_KERNEL);
            if ((flags & ~_PAGE_NX) == (expected & ~_PAGE_NX))
                retval = 0;
            break;
        default:
            expected = pgprot_val(PAGE_KERNEL_NOCACHE);
            if ((flags & ~(_PAGE_NX | _PAGE_PWT)) == (expected & ~(_PAGE_NX | _PAGE_PWT)))
                retval = 0;
            break;
    }

    if (retval)
    {
        if (count < NV_MAX_RECURRING_WARNING_MESSAGES)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: bad caching on address 0x%lx: actual 0x%x != expected 0x%x\n",
                address, flags, expected);
        }

        if (count == 0)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: please see the README section on "
                "Cache Aliasing for more information\n");
        }

        count++;
    }

failed:
#endif /* defined(NVCPU_X86) || defined(NVCPU_X86_64) */
    return retval;
#else
    return 0;
#endif
}

static int __init nvidia_init_module(void)
{
    RM_STATUS status;
    int rc;
    NvU32 count, data, i;
    nv_state_t *nv = NV_STATE_PTR(&nv_ctl_device);
    nv_stack_t *sp = NULL;
    nv_linux_state_t *nvl;
    nv_smu_state_t *nv_smu = &nv_smu_device;

    nv_user_map_init();

#if defined(VM_CHECKER)
    NV_SPIN_LOCK_INIT(&vm_lock);
#endif
#if defined(KM_CHECKER)
    NV_SPIN_LOCK_INIT(&km_lock);
#endif

    NV_KMEM_CACHE_CREATE(nv_stack_t_cache, "nv_stack_t", nv_stack_t);
    if (nv_stack_t_cache == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: stack cache allocation failed!\n");
        return -ENOMEM;
    }

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        NV_KMEM_CACHE_DESTROY(nv_stack_t_cache);
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return -ENOMEM;
    }

    count = nvos_count_devices(sp);
    if (count == 0)
    {
        NV_KMEM_CACHE_FREE_STACK(sp);
        NV_KMEM_CACHE_DESTROY(nv_stack_t_cache);
        nv_printf(NV_DBG_ERRORS, "NVRM: No NVIDIA graphics adapter found!\n");
        return -ENODEV;
    }

    if (!rm_init_rm(sp))
    {
        NV_KMEM_CACHE_FREE_STACK(sp);
        NV_KMEM_CACHE_DESTROY(nv_stack_t_cache);
        nv_printf(NV_DBG_ERRORS, "NVRM: rm_init_rm() failed!\n");
        return -EIO;
    }

    nv_linux_devices = NULL;
    memset(&nv_smu_device, 0, sizeof(nv_smu_state_t));

    if (pci_register_driver(&nv_pci_driver) < 0)
    {
        rc = -ENODEV;
        nv_printf(NV_DBG_ERRORS, "NVRM: No NVIDIA graphics adapter found!\n");
        goto failed2;
    }

    if (nv_smu->handle != NULL)
    {
        /* init SMU functionality */
        rm_init_smu(sp, nv_smu);
    }

    if (num_probed_nv_devices != count)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: The NVIDIA probe routine was not called for %d device(s).\n",
            count - num_probed_nv_devices);
        nv_printf(NV_DBG_ERRORS,
            "NVRM: This can occur when a driver such as nouveau, rivafb,\n"
            "NVRM: nvidiafb, or rivatv was loaded and obtained ownership of\n"
            "NVRM: the NVIDIA device(s).\n");
        nv_printf(NV_DBG_ERRORS,
            "NVRM: Try unloading the conflicting kernel module (and/or\n"
            "NVRM: reconfigure your kernel without the conflicting\n"
            "NVRM: driver(s)), then try loading the NVIDIA kernel module\n"
            "NVRM: again.\n");
    }

    if (num_probed_nv_devices == 0)
    {
        rc = -ENODEV;
        nv_printf(NV_DBG_ERRORS, "NVRM: No NVIDIA graphics adapter probed!\n");
        goto failed2;
    }

    if (num_probed_nv_devices != num_nv_devices)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: The NVIDIA probe routine failed for %d device(s).\n",
            num_probed_nv_devices - num_nv_devices);
    }

    if (num_nv_devices == 0)
    {
        rc = -ENODEV;
        nv_printf(NV_DBG_ERRORS,
            "NVRM: None of the NVIDIA graphics adapters were initialized!\n");
        goto failed2;
    }

    nv_printf(NV_DBG_ERRORS, "NVRM: loading %s", pNVRM_ID);
    if (__nv_patches[0].short_description != NULL)
    {
        nv_printf(NV_DBG_ERRORS,
            " (applied patches: %s", __nv_patches[0].short_description);
        for (i = 1; __nv_patches[i].short_description; i++)
        {
            nv_printf(NV_DBG_ERRORS,
                ",%s", __nv_patches[i].short_description);
        }
        nv_printf(NV_DBG_ERRORS, ")");
    }
    nv_printf(NV_DBG_ERRORS, "\n");

    rc = nv_register_chrdev(NV_MAJOR_DEVICE_NUMBER, &nv_fops);
    if (rc < 0)
        goto failed2;

    // init the nvidia control device
    nv->os_state = (void *) &nv_ctl_device;
    nv_lock_init_locks(nv);

#if defined(NV_PM_SUPPORT_OLD_STYLE_APM)
    for (nvl = nv_linux_devices; nvl != NULL;  nvl = nvl->next)
    {
        nvl->apm_nv_dev = pm_register(PM_PCI_DEV, PM_SYS_VGA, nv_kern_apm_event);
    }
#endif

    NV_KMEM_CACHE_CREATE(nv_pte_t_cache, "nv_pte_t", nv_pte_t);
    if (nv_pte_t_cache == NULL)
    {
        rc = -ENOMEM;
        nv_printf(NV_DBG_ERRORS, "NVRM: pte cache allocation failed\n");
        goto failed;
    }

    NV_KMEM_CACHE_CREATE(nvidia_p2p_page_t_cache, "nvidia_p2p_page_t",
            nvidia_p2p_page_t);
    if (nvidia_p2p_page_t_cache == NULL)
    {
        rc = -ENOMEM;
        nv_printf(NV_DBG_ERRORS, "NVRM: p2p page cache allocation failed\n");
        goto failed;
    }

#if defined(NV_SG_MAP_BUFFERS) && defined(NV_NEED_REMAP_CHECK)
    rm_read_registry_dword(sp, nv, "NVreg", "RemapLimit", &nv_remap_limit);

    // allow an override, but use default if no override
    if (nv_remap_limit == 0)
        nv_remap_limit = NV_REMAP_LIMIT_DEFAULT;

    nv_remap_count = 0;
#endif

#if !defined(NV_VMWARE) && \
  (defined(NVCPU_X86_64) || (defined(NVCPU_X86) && defined(CONFIG_X86_PAE)))
    if (boot_cpu_has(X86_FEATURE_NX))
    {
        NvU32 __eax, __edx;
        rdmsr(MSR_EFER, __eax, __edx);
        if ((__eax & EFER_NX) != 0)
            __nv_supported_pte_mask |= _PAGE_NX;
    }
    if (_PAGE_NX != ((NvU64)1<<63))
    {
        /*
         * Make sure we don't strip software no-execute
         * bits from PAGE_KERNEL(_NOCACHE) before calling
         * change_page_attr().
         */
        __nv_supported_pte_mask |= _PAGE_NX;
    }
#endif

    /* create /proc/driver/nvidia/... */
    rc = nv_register_procfs();
    if (rc < 0)
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to register procfs!\n");

    /*
     * Give users an opportunity to disable the driver's use of
     * the change_page_attr() and set_pages_{uc,wb}() kernel
     * interfaces.
     */
    status = rm_read_registry_dword(sp, nv,
            "NVreg", NV_REG_UPDATE_MEMORY_TYPES, &data);
    if ((status == RM_OK) && ((int)data != ~0))
    {
        nv_update_memory_types = data;
    }
#if defined(NV_CHANGE_PAGE_ATTR_BUG_PRESENT)
    /*
     * Unless we explicitely detect that the change_page_attr()
     * inteface is fixed, disable usage of the interface on
     * this kernel. Notify the user of this problem using the
     * driver's /proc warnings interface (read by the installer
     * and the bug report script).
     */
    else
    {
        rc = nv_verify_cpa_interface();
        if (rc < 0)
        {
            nv_prints(NV_DBG_ERRORS, __cpgattr_warning);
            nv_procfs_add_warning("change_page_attr", __cpgattr_warning);
            nv_update_memory_types = 0;
        }
        else if (rc != 0)
        {
            nv_prints(NV_DBG_ERRORS, __cpgattr_warning_2);
            nv_procfs_add_warning("change_page_attr", __cpgattr_warning_2);
            nv_update_memory_types = 0;
        }
    }
#endif /* defined(NV_CHANGE_PAGE_ATTR_BUG_PRESENT) */

#if defined(NVCPU_X86_64) && defined(CONFIG_IA32_EMULATION) && \
  !defined(NV_FILE_OPERATIONS_HAS_COMPAT_IOCTL)
    rm_register_compatible_ioctls(sp);
#endif

    rc = nv_init_pat_support(sp);
    if (rc < 0)
        goto failed;

#if (defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)) && defined(KERNEL_2_4)
    // attempt to load the i2c modules for linux kernel
    // check to see if this is possible
    if((!i2c_add_adapter) || (!i2c_del_adapter))
    {
        // attempt to load the module
        request_module("i2c-core");
        
        // recheck for valid addresses
        if((!i2c_add_adapter) || (!i2c_del_adapter))
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: Your Linux 2.4 kernel was configured to include modular\n");
            nv_printf(NV_DBG_ERRORS, "NVRM: support for the Linux/i2c infrastructure, but the NVIDIA\n");
            nv_printf(NV_DBG_ERRORS, "NVRM: Linux graphics driver was unable to locate and load the\n");
            nv_printf(NV_DBG_ERRORS, "NVRM: i2c-core.o kernel module.\n");
            nv_printf(NV_DBG_ERRORS, "NVRM: \n");
            nv_printf(NV_DBG_ERRORS, "NVRM: If you wish to take advantage of the NVIDIA driver's i2c\n");
            nv_printf(NV_DBG_ERRORS, "NVRM: support feature, please make sure the Linux/i2c kernel\n");
            nv_printf(NV_DBG_ERRORS, "NVRM: modules are installed correctly.\n");
        }
    }
#endif

    __nv_init_sp = sp;

    for (nvl = nv_linux_devices; nvl != NULL;  nvl = nvl->next)
    {
#if defined(NV_PM_MESSAGE_T_HAS_EVENT)
        nvl->nv_state.flags |= NV_FLAG_S4_AVAILABLE;
#else
        nvl->nv_state.flags &= ~NV_FLAG_S4_AVAILABLE;
#endif
    }

    return 0;

failed:
    if (nvidia_p2p_page_t_cache != NULL)
        NV_KMEM_CACHE_DESTROY(nvidia_p2p_page_t_cache);

    if (nv_pte_t_cache != NULL)
        NV_KMEM_CACHE_DESTROY(nv_pte_t_cache);

#if defined(NV_PM_SUPPORT_OLD_STYLE_APM)
    for (nvl = nv_linux_devices; nvl != NULL;  nvl = nvl->next)
        if (nvl->apm_nv_dev != NULL) pm_unregister(nvl->apm_nv_dev);
#endif

    nv_unregister_chrdev(NV_MAJOR_DEVICE_NUMBER);

    while (nv_linux_devices != NULL)
    {
        nv_linux_state_t *tmp;
        if (nv_linux_devices->dev)
        {
            struct pci_dev *dev = nv_linux_devices->dev;
            release_mem_region(NV_PCI_RESOURCE_START(dev, NV_GPU_BAR_INDEX_REGS),
                               NV_PCI_RESOURCE_SIZE(dev, NV_GPU_BAR_INDEX_REGS));
            NV_PCI_DISABLE_DEVICE(dev);
        }
        tmp = nv_linux_devices;
        nv_linux_devices = nv_linux_devices->next;
        NV_KFREE(tmp, sizeof(nv_linux_state_t));
    }

failed2:
    if (nv_smu->handle != NULL)
    {
        struct pci_dev *dev = nv_smu->handle;
        rm_shutdown_smu(sp, nv_smu);
        release_mem_region(NV_PCI_RESOURCE_START(dev, 0),
                           NV_PCI_RESOURCE_SIZE(dev, 0));
        pci_disable_device(dev);
        memset(&nv_smu_device, 0, sizeof(nv_smu_state_t));
    }

    pci_unregister_driver(&nv_pci_driver);
    rm_shutdown_rm(sp);

    NV_KMEM_CACHE_FREE_STACK(sp);
    NV_KMEM_CACHE_DESTROY(nv_stack_t_cache);

    return rc;
}

static void __exit nvidia_exit_module(void)
{
    nv_linux_state_t *nvl;
    nv_state_t *nv;
    nv_smu_state_t *nv_smu = &nv_smu_device; 
    nv_stack_t *sp = __nv_init_sp;
    struct pci_dev *dev;
    NvU32 i;

    nv_printf(NV_DBG_INFO, "NVRM: nvidia_exit_module\n");

    for (nvl = nv_linux_devices; nvl != NULL;  nvl = nvl->next)
    {
        nv = NV_STATE_PTR(nvl);

        if (nv->flags & NV_FLAG_PERSISTENT_SW_STATE)
        {
            rm_disable_gpu_state_persistence(sp, nv);
            NV_SHUTDOWN_ADAPTER(sp, nv, nvl);

            NV_KMEM_CACHE_FREE_STACK(nvl->timer_sp);
            NV_KMEM_CACHE_FREE_STACK(nvl->isr_bh_sp);
            NV_KMEM_CACHE_FREE_STACK(nvl->pci_cfgchk_sp);
            NV_KMEM_CACHE_FREE_STACK(nvl->isr_sp);
        }
    }

    if ((dev = (struct pci_dev *)(nv_smu->handle)) != NULL) 
    {
        rm_shutdown_smu(sp, nv_smu);
        release_mem_region(NV_PCI_RESOURCE_START(dev, 0),
                           NV_PCI_RESOURCE_SIZE(dev, 0));
        pci_disable_device(dev);
        memset(&nv_smu_device, 0, sizeof(nv_smu_state_t));
    }

    nv_unregister_chrdev(NV_MAJOR_DEVICE_NUMBER);

    for (nvl = nv_linux_devices; nvl != NULL;  nvl = nvl->next)
    {
        nv = NV_STATE_PTR(nvl);

        if ((dev = nvl->dev) != NULL)
        {
            if (NV_IS_GVI_DEVICE(nv))
            {
                NV_TASKQUEUE_FLUSH();
                rm_gvi_detach_device(sp, nv);
                rm_gvi_free_private_state(sp, nv);
            }
            else
            {
                for (i = 0; i < NV_GPU_NUM_BARS; i++)
                {
                    if (nv->bars[i].size != 0)
                    {
                        nv_user_map_unregister(nv->bars[i].address,
                                nv->bars[i].size);
                    }
                }
                rm_i2c_remove_adapters(sp, nv);
                rm_free_private_state(sp, nv);
            }
            release_mem_region(NV_PCI_RESOURCE_START(dev, NV_GPU_BAR_INDEX_REGS),
                               NV_PCI_RESOURCE_SIZE(dev, NV_GPU_BAR_INDEX_REGS));
            NV_PCI_DISABLE_DEVICE(dev);
        }
    }

    pci_unregister_driver(&nv_pci_driver);

    /* remove /proc/driver/nvidia/... */
    nv_unregister_procfs();

#if defined(NV_PM_SUPPORT_OLD_STYLE_APM)
    for (nvl = nv_linux_devices; nvl != NULL;  nvl = nvl->next)
    {
        if (nvl->apm_nv_dev != NULL) pm_unregister(nvl->apm_nv_dev);
    }
#endif

    /*
     * Make sure we freed up all the mappings. The kernel should
     * do this automatically before calling close.
     */
    while (nv_linux_devices != NULL)
    {
        nv_linux_state_t *tmp;
        nv = NV_STATE_PTR(nv_linux_devices);

        if (nv_linux_devices->alloc_queue)
        {
            nv_alloc_t *at = nv_linux_devices->alloc_queue, *next;
            while (at)
            {
                NV_PRINT_AT(NV_DBG_ERRORS, at);
                /* nv_free_pages() will free this 'at' */
                next = at->next;
                nv_free_pages(nv, at->num_pages,
                              NV_ALLOC_MAPPING_AGP(at->flags),
                              NV_ALLOC_MAPPING_CONTIG(at->flags),
                              NV_ALLOC_MAPPING(at->flags),
                              (void *)at);
                at = next;
            }
        }
        tmp = nv_linux_devices;
        nv_linux_devices = nv_linux_devices->next;
        NV_KFREE(tmp, sizeof(nv_linux_state_t));
    }

    // Shutdown the resource manager
    rm_shutdown_rm(sp);

#if defined(NVCPU_X86_64) && defined(CONFIG_IA32_EMULATION) && \
  !defined(NV_FILE_OPERATIONS_HAS_COMPAT_IOCTL)
    rm_unregister_compatible_ioctls(sp);
#endif

    nv_teardown_pat_support();

#if defined(NV_ENABLE_MEM_TRACKING)
#if defined(VM_CHECKER)
    if (vm_usage != 0)
    {
        nv_list_mem("VM", vm_list);
        nv_printf(NV_DBG_ERRORS,
            "NVRM: final VM memory usage: 0x%x bytes\n", vm_usage);
    }
#endif
#if defined(KM_CHECKER)
    if (km_usage != 0)
    {
        nv_list_mem("KM", km_list);
        nv_printf(NV_DBG_ERRORS,
            "NVRM: final KM memory usage: 0x%x bytes\n", km_usage);
    }
#endif
#if defined(NV_SG_MAP_BUFFERS) && defined(NV_NEED_REMAP_CHECK)
    if (nv_remap_count != 0)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: final SG memory usage: 0x%x bytes\n", nv_remap_count);
    }
#endif
#endif /* NV_ENABLE_MEM_TRACKING */

    NV_KMEM_CACHE_DESTROY(nvidia_p2p_page_t_cache);
    NV_KMEM_CACHE_DESTROY(nv_pte_t_cache);

    NV_KMEM_CACHE_FREE_STACK(sp);
    NV_KMEM_CACHE_DESTROY(nv_stack_t_cache);
}

module_init(nvidia_init_module);
module_exit(nvidia_exit_module);

void *nv_alloc_file_private(void)
{
    nv_file_private_t *nvfp;
    unsigned int i;

    NV_KMALLOC(nvfp, sizeof(nv_file_private_t));
    if (!nvfp)
        return NULL;

    memset(nvfp, 0, sizeof(nv_file_private_t));

    for (i = 0; i < NV_FOPS_STACK_INDEX_COUNT; ++i)
    {
        NV_INIT_MUTEX(&nvfp->fops_sp_lock[i]);
    }
    init_waitqueue_head(&nvfp->waitqueue);
    NV_SPIN_LOCK_INIT(&nvfp->fp_lock);

    return nvfp;
}

void nv_free_file_private(nv_file_private_t *nvfp)
{
    nvidia_event_t *nvet;

    if (nvfp == NULL)
        return;

    for (nvet = nvfp->event_head; nvet != NULL; nvet = nvfp->event_head)
    {
        nvfp->event_head = nvfp->event_head->next;
        NV_KFREE(nvet, sizeof(nvidia_event_t));
    }
    NV_KFREE(nvfp, sizeof(nv_file_private_t));
}


/*
** nv_kern_open
**
** nv driver open entry point.  Sessions are created here.
*/
int nv_kern_open(
    struct inode *inode,
    struct file *file
)
{
    nv_state_t *nv = NULL;
    nv_linux_state_t *nvl = NULL;
    NvU32 devnum;
    int rc = 0;
    nv_file_private_t *nvfp = NULL;
    nv_stack_t *sp = NULL;
#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
    NvU32 msi_config = 0;
#endif
    unsigned int i;
    unsigned int k;

    nv_printf(NV_DBG_INFO, "NVRM: nv_kern_open...\n");

    nvfp = nv_alloc_file_private();
    if (nvfp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate file private!\n");
        return -ENOMEM;
    }

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_free_file_private(nvfp);
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return -ENOMEM;
    }

    for (i = 0; i < NV_FOPS_STACK_INDEX_COUNT; ++i)
    {
        NV_KMEM_CACHE_ALLOC_STACK(nvfp->fops_sp[i]);
        if (nvfp->fops_sp[i] == NULL)
        {
            NV_KMEM_CACHE_FREE_STACK(sp);
            for (k = 0; k < i; ++k)
            {
                NV_KMEM_CACHE_FREE_STACK(nvfp->fops_sp[k]);
            }
            nv_free_file_private(nvfp);
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack[%d]\n", i);
            return -ENOMEM;
        }
    }

    NV_SET_FILE_PRIVATE(file, nvfp);
    nvfp->sp = sp;

    /* for control device, just jump to its open routine */
    /* after setting up the private data */
    if (NV_IS_CONTROL_DEVICE(inode))
    {
        rc = nv_kern_ctl_open(inode, file);
        if (rc != 0)
            goto failed2;
        return rc;
    }

    /* what device are we talking about? */
    devnum = NV_DEVICE_NUMBER(inode);

    nvl = nv_linux_devices;
    while (nvl != NULL)
    {
        if (nvl->device_num == devnum)
            break;
        nvl = nvl->next;
    }

    if (nvl == NULL)
    {
        rc = -ENODEV;
        goto failed2;
    }

    nv = NV_STATE_PTR(nvl);

    nv_printf(NV_DBG_INFO, "NVRM: nv_kern_open on device %d\n", devnum);
    down(&nvl->ldata_lock);

    NV_CHECK_PCI_CONFIG_SPACE(sp, nv, TRUE, TRUE, NV_MAY_SLEEP());

    nvfp->nvptr = nvl;

    /*
     * map the memory and allocate isr on first open
     */

    if ( ! (nv->flags & NV_FLAG_OPEN))
    {
        if (nv->device_id == 0)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: open of nonexistent device %d\n",
                devnum);
            rc = -ENXIO;
            goto failed;
        }

        if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
        {
            NV_KMEM_CACHE_ALLOC_STACK(nvl->isr_sp);
            if (nvl->isr_sp == NULL)
            {
                rc = -ENOMEM;
                nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
                goto failed;
            }
    
            NV_KMEM_CACHE_ALLOC_STACK(nvl->pci_cfgchk_sp);
            if (nvl->pci_cfgchk_sp == NULL)
            {
                rc = -ENOMEM;
                nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
                goto failed;
            }
    
            NV_KMEM_CACHE_ALLOC_STACK(nvl->isr_bh_sp);
            if (nvl->isr_bh_sp == NULL)
            {
                rc = -ENOMEM;
                nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
                goto failed;
            }
    
            NV_KMEM_CACHE_ALLOC_STACK(nvl->timer_sp);
            if (nvl->timer_sp == NULL)
            {
                rc = -ENOMEM;
                nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
                goto failed;
            }
        }

#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
        if (!NV_IS_GVI_DEVICE(nv))
        {
            if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
            {
                rm_read_registry_dword(sp, nv, "NVreg", NV_REG_ENABLE_MSI,
                                       &msi_config);
                if ((msi_config == 1) && 
                        (nv_find_pci_capability(nvl->dev, PCI_CAP_ID_MSI)))
                {
                    rc = pci_enable_msi(nvl->dev);
                    if (rc == 0)
                    {
                        nv->interrupt_line = nvl->dev->irq;
                        nv->flags |= NV_FLAG_USES_MSI;
                    }
                    else
                    {
                        nv->flags &= ~NV_FLAG_USES_MSI;
                        nv_printf(NV_DBG_ERRORS,
                                  "NVRM: failed to enable MSI, \n"
                                  "using PCI-E virtual-wire interrupts!\n");
                    }
                }
            }
        }
#endif

        if (NV_IS_GVI_DEVICE(nv))
        {
            rc = request_irq(nv->interrupt_line, nv_gvi_kern_isr,
                             IRQF_SHARED, "nvidia", (void *)nvl);
            if (rc == 0)
            {
                nvl->work.data = (void *)nvl;
                NV_TASKQUEUE_INIT(&nvl->work.task, nv_gvi_kern_bh,
                                  (void *)&nvl->work);
                rm_init_gvi_device(sp, nv);
                goto done;
            }
        }
        else
        {
            rc = 0;
            if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
            {
                rc = request_irq(nv->interrupt_line, nv_kern_isr, IRQF_SHARED,
                        "nvidia", (void *)nvl);
            }
        }
        if (rc != 0)
        {
            if ((nv->interrupt_line != 0) && (rc == -EBUSY))
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM: Tried to get IRQ %d, but another driver\n",
                    (unsigned int) nv->interrupt_line);
                nv_printf(NV_DBG_ERRORS, "NVRM: has it and is not sharing it.\n");
                nv_printf(NV_DBG_ERRORS, "NVRM: You may want to verify that no audio driver");
                nv_printf(NV_DBG_ERRORS, " is using the IRQ.\n");
            }
            nv_printf(NV_DBG_ERRORS, "NVRM: request_irq() failed (%d)\n", rc);
            goto failed;
        }

        if (!rm_init_adapter(sp, nv))
        {
            free_irq(nv->interrupt_line, (void *) nvl);
            nv_printf(NV_DBG_ERRORS, "NVRM: rm_init_adapter(%d) failed\n",
                      devnum);
            rc = -EIO;
            goto failed;
        }

        if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
            tasklet_enable(&nvl->tasklet);

done:
        nv->flags |= NV_FLAG_OPEN;
    }

    NV_ATOMIC_INC(nvl->usage_count);

failed:
    up(&nvl->ldata_lock);

    if (rc != 0)
    {
#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
        if (nv->flags & NV_FLAG_USES_MSI)
        {
            nv->flags &= ~NV_FLAG_USES_MSI;              
            NV_PCI_DISABLE_MSI(nvl->dev);
        }
#endif
        if (nvl->timer_sp != NULL)
            NV_KMEM_CACHE_FREE_STACK(nvl->timer_sp);
        if (nvl->isr_bh_sp != NULL)
            NV_KMEM_CACHE_FREE_STACK(nvl->isr_bh_sp);
        if (nvl->pci_cfgchk_sp != NULL)
            NV_KMEM_CACHE_FREE_STACK(nvl->pci_cfgchk_sp);
        if (nvl->isr_sp != NULL)
            NV_KMEM_CACHE_FREE_STACK(nvl->isr_sp);

failed2:
        if (nvfp != NULL)
        {
            NV_KMEM_CACHE_FREE_STACK(sp);
            for (i = 0; i < NV_FOPS_STACK_INDEX_COUNT; ++i)
            {
                NV_KMEM_CACHE_FREE_STACK(nvfp->fops_sp[i]);
            }
            nv_free_file_private(nvfp);
            NV_SET_FILE_PRIVATE(file, NULL);
        }
    }

    return rc;
}

/*
** nv_kern_close
**
** Master driver close entry point.
*/

int nv_kern_close(
    struct inode *inode,
    struct file *file
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_FILEP(file);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nv_file_private_t *nvfp = NV_GET_FILE_PRIVATE(file);
    nv_stack_t *sp = nvfp->sp;
    unsigned int i;

    NV_CHECK_PCI_CONFIG_SPACE(sp, nv, TRUE, TRUE, NV_MAY_SLEEP());

    /* for control device, just jump to its open routine */
    /* after setting up the private data */
    if (NV_IS_CONTROL_DEVICE(inode))
        return nv_kern_ctl_close(inode, file);

    nv_printf(NV_DBG_INFO, "NVRM: nv_kern_close on device %d\n",
        NV_DEVICE_NUMBER(inode));

    rm_free_unused_clients(sp, nv, NV_GET_FILE_PRIVATE(file));

    down(&nvl->at_lock);
    if (nvl->alloc_queue != NULL)
    {
        nv_alloc_t *at = nvl->alloc_queue, *next;
        while (at != NULL)
        {
            /* nv_free_pages() will free this 'at' */
            next = at->next;
            if ((NV_ATOMIC_READ(at->usage_count) == 0) &&
                (at->file == file))
            {
                NV_ATOMIC_INC(at->usage_count);
                up(&nvl->at_lock);
                if (at->pid == os_get_current_process())
                    NV_PRINT_AT(NV_DBG_MEMINFO, at);
                nv_free_pages(nv, at->num_pages,
                              NV_ALLOC_MAPPING_AGP(at->flags),
                              NV_ALLOC_MAPPING_CONTIG(at->flags),
                              NV_ALLOC_MAPPING(at->flags),
                              (void *)at);
                down(&nvl->at_lock);
                next = nvl->alloc_queue; /* start over */
            }
            at = next;
        }
    }
    up(&nvl->at_lock);

    down(&nvl->ldata_lock);
    if (NV_ATOMIC_DEC_AND_TEST(nvl->usage_count))
    {
        if (NV_IS_GVI_DEVICE(nv))
        {
            rm_shutdown_gvi_device(sp, nv);
            NV_TASKQUEUE_FLUSH();
            free_irq(nv->interrupt_line, (void *)nvl);
        }
        else
        {
            if (nv->flags & NV_FLAG_PERSISTENT_SW_STATE)
            {
                rm_disable_adapter(sp, nv);
            }
            else
            {
                NV_SHUTDOWN_ADAPTER(sp, nv, nvl);
            }
        }

        if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
        {
            NV_KMEM_CACHE_FREE_STACK(nvl->timer_sp);
            NV_KMEM_CACHE_FREE_STACK(nvl->isr_bh_sp);
            NV_KMEM_CACHE_FREE_STACK(nvl->pci_cfgchk_sp);
            NV_KMEM_CACHE_FREE_STACK(nvl->isr_sp);
        }

        /*
         * If software state persistence is not enabled, make sure we free 
         * all memory tied to this device. Memory freed here has been leaked
         * by the core RM, warn accordingly.
         */
        if (nvl->alloc_queue && !(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
        {
            nv_alloc_t *at = nvl->alloc_queue, *next;
            while (at)
            {
                NV_PRINT_AT(NV_DBG_ERRORS, at);
                /* nv_free_pages() will free this 'at' */
                next = at->next;
                nv_free_pages(nv, at->num_pages,
                              NV_ALLOC_MAPPING_AGP(at->flags),
                              NV_ALLOC_MAPPING_CONTIG(at->flags),
                              NV_ALLOC_MAPPING(at->flags),
                              (void *)at);
                at = next;
            }
        }

        /* leave INIT flag alone so we don't reinit every time */
        nv->flags &= ~NV_FLAG_OPEN;
    }
    up(&nvl->ldata_lock);

    for (i = 0; i < NV_FOPS_STACK_INDEX_COUNT; ++i)
    {
        NV_KMEM_CACHE_FREE_STACK(nvfp->fops_sp[i]);
    }

    if (NV_GET_FILE_PRIVATE(file) != NULL)
    {
        nv_free_file_private(NV_GET_FILE_PRIVATE(file));
        NV_SET_FILE_PRIVATE(file, NULL);
    }

    NV_KMEM_CACHE_FREE_STACK(sp);

    return 0;
}

unsigned int nv_kern_poll(
    struct file *file,
    poll_table *wait
)
{
    unsigned int mask = 0;
    nv_file_private_t *nvfp;
    unsigned long eflags;

    nvfp = NV_GET_FILE_PRIVATE(file);

    if ( !(file->f_flags & O_NONBLOCK))
    {
        // add us to the list
        poll_wait(file, &nvfp->waitqueue, wait);
    }

    NV_SPIN_LOCK_IRQSAVE(&nvfp->fp_lock, eflags);

    // wake the user on any event
    if (nvfp->event_head != NULL)
    {
        nv_printf(NV_DBG_EVENTINFO, "NVRM: Hey, an event occured!\n");
        // trigger the client, when they grab the event, 
        // we'll decrement the event count
        mask |= (POLLPRI|POLLIN);
    }

    NV_SPIN_UNLOCK_IRQRESTORE(&nvfp->fp_lock, eflags);

    return mask;
}

//
// nv_kern_ioctl
//
// nv driver ioctl entry point.
//

/*
 * some ioctl's can only be done on actual device, others only on the control device
 */
#define CTL_DEVICE_ONLY(nv) { if ( ! ((nv)->flags & NV_FLAG_CONTROL)) { status = -EINVAL; goto done; } }

#define ACTUAL_DEVICE_ONLY(nv) { if ((nv)->flags & NV_FLAG_CONTROL) { status = -EINVAL; goto done; } }


/* todo:
   need ioctl to raise a thread priority that is not superuser
       set its priority to SCHED_FIFO which is simple
       priority scheduling w/ disabled timeslicing
*/

int nv_kern_ioctl(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long i_arg)
{
    int status = 0;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_FILEP(file);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nv_file_private_t *nvfp = NV_GET_FILE_PRIVATE(file);
    nv_stack_t *sp = NULL;
    nv_ioctl_xfer_t ioc_xfer;
    void *arg_ptr = (void *) i_arg;
    void *arg_copy = NULL;
    size_t arg_size;
    int arg_cmd;

    nv_printf(NV_DBG_INFO, "NVRM: ioctl(0x%x, 0x%x, 0x%x)\n",
        _IOC_NR(cmd), (unsigned int) i_arg, _IOC_SIZE(cmd));

    down(&nvfp->fops_sp_lock[NV_FOPS_STACK_INDEX_IOCTL]);
    sp = nvfp->fops_sp[NV_FOPS_STACK_INDEX_IOCTL];

    NV_CHECK_PCI_CONFIG_SPACE(sp, nv, TRUE, TRUE, NV_MAY_SLEEP());

    arg_size = _IOC_SIZE(cmd);
    arg_cmd  = _IOC_NR(cmd);

    if (arg_cmd == NV_ESC_IOCTL_XFER_CMD)
    {
        if (arg_size != sizeof(nv_ioctl_xfer_t))
        {
            nv_printf(NV_DBG_ERRORS,
                    "NVRM: invalid ioctl XFER structure size!\n");
            status = -EINVAL;
            goto done;
        }

        if (copy_from_user(&ioc_xfer, arg_ptr, sizeof(ioc_xfer)))
        {
            nv_printf(NV_DBG_ERRORS,
                    "NVRM: failed to copyin ioctl XFER data!\n");
            status = -EFAULT;
            goto done;
        }

        arg_cmd  = ioc_xfer.cmd;
        arg_size = ioc_xfer.size;
        arg_ptr  = NvP64_VALUE(ioc_xfer.ptr);

        if (arg_size > NV_ABSOLUTE_MAX_IOCTL_SIZE)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: invalid ioctl XFER size!\n");
            status = -EINVAL;
            goto done;
        }
    }

    NV_KMALLOC(arg_copy, arg_size);
    if (arg_copy == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate ioctl memory\n");
        status = -ENOMEM;
        goto done;
    }

    if (copy_from_user(arg_copy, arg_ptr, arg_size))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to copyin ioctl data!\n");
        status = -EFAULT;
        goto done;
    }

    switch (arg_cmd)
    {
        /* pass out info about the card */
        case NV_ESC_CARD_INFO:
        {
            nv_ioctl_card_info_t *ci;
            nv_linux_state_t *tnvl;
            nv_ioctl_rm_api_old_version_t *rm_api;

            CTL_DEVICE_ONLY(nv);

            /* the first element of card info passed from the client will have
             * the rm_api_version_magic value to show that the client is new
             * enough to support versioning. If the client is too old to 
             * support versioning, our mmap interfaces are probably different
             * enough to cause serious damage.
             * just copy in the one dword to check.
             */
            rm_api = arg_copy;
            switch (rm_api->magic)
            {
                case NV_RM_API_OLD_VERSION_MAGIC_REQ:
                case NV_RM_API_OLD_VERSION_MAGIC_LAX_REQ:
                case NV_RM_API_OLD_VERSION_MAGIC_OVERRIDE_REQ:
                    /* the client is using the old major-minor-patch
                     * API version check; reject it.
                     */
                    nv_printf(NV_DBG_ERRORS,
                              "NVRM: API mismatch: the client has the version %d.%d-%d, but\n"
                              "NVRM: this kernel module has the version %s.  Please\n"
                              "NVRM: make sure that this kernel module and all NVIDIA driver\n"
                              "NVRM: components have the same version.\n",
                              rm_api->major, rm_api->minor, rm_api->patch,
                              NV_VERSION_STRING);
                    status = -EINVAL;
                    break;
                
                case NV_RM_API_OLD_VERSION_MAGIC_IGNORE:
                    /* the client is telling us to ignore the old
                     * version scheme; it will do a version check via
                     * NV_ESC_CHECK_VERSION_STR
                     */
                    break;
                default:
                    nv_printf(NV_DBG_ERRORS, 
                        "NVRM: client does not support versioning!!\n");
                    status = -EINVAL;
                    break;
            }

            ci = arg_copy;
            memset(ci, 0, arg_size);
            for (tnvl = nv_linux_devices; tnvl != NULL;  tnvl = tnvl->next)
            {
                nv_state_t *tnv;
                tnv = NV_STATE_PTR(tnvl);
                if (tnv->device_id)
                {
                    ci->flags = NV_IOCTL_CARD_INFO_FLAG_PRESENT;
                    ci->domain = tnv->domain;
                    ci->bus = tnv->bus;
                    ci->slot = tnv->slot;
                    ci->vendor_id = tnv->vendor_id;
                    ci->device_id = tnv->device_id;
                    ci->gpu_id = tnv->gpu_id;
                    ci->interrupt_line = tnv->interrupt_line;
                    ci->reg_address = tnv->regs->address;
                    ci->reg_size = tnv->regs->size;
                    ci->fb_address = tnv->fb->address;
                    ci->fb_size = tnv->fb->size;
                    ci++;
                }
            }
            break;
        }

        case NV_ESC_CHECK_VERSION_STR:
        {
            nv_ioctl_rm_api_version_t *params = arg_copy;
            
            CTL_DEVICE_ONLY(nv);
            status = (rm_perform_version_check(sp, params) == RM_OK)
                        ? 0 : -EINVAL;
            break;
        }
        
        default:
            status = rm_ioctl(sp, nv, NV_GET_FILE_PRIVATE(file), arg_cmd,
                    arg_copy) ? 0 : -EINVAL;
            break;
    }

done:
    up(&nvfp->fops_sp_lock[NV_FOPS_STACK_INDEX_IOCTL]);
    if (arg_copy != NULL)
    {
        if (copy_to_user(arg_ptr, arg_copy, arg_size))
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to copyout ioctl data\n");
            status = -EFAULT;
        }
        NV_KFREE(arg_copy, arg_size);
    }

    return status;
}

long nv_kern_unlocked_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long i_arg
)
{
    return nv_kern_ioctl(file->f_dentry->d_inode, file, cmd, i_arg);
}

long nv_kern_compat_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long i_arg
)
{
    return nv_kern_ioctl(file->f_dentry->d_inode, file, cmd, i_arg);
}

/*
 * driver receives an interrupt
 *    if someone waiting, then hand it off.
 */
irqreturn_t nv_kern_isr(
    int   irq,
    void *arg
#if !defined(NV_IRQ_HANDLER_T_PRESENT) || (NV_IRQ_HANDLER_T_ARGUMENT_COUNT == 3)
    ,struct pt_regs *regs
#endif
)
{
    nv_linux_state_t *nvl = (void *) arg;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    NvU32 need_to_run_bottom_half = 0;
    BOOL ret;

    ret = rm_isr(nvl->isr_sp, nv, &need_to_run_bottom_half);
    if (need_to_run_bottom_half)
    {
        tasklet_schedule(&nvl->tasklet);
    }

    return IRQ_RETVAL(ret);
}

void nv_kern_isr_bh(
    unsigned long data
)
{
    nv_state_t *nv = (nv_state_t *) data;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    NV_CHECK_PCI_CONFIG_SPACE(nvl->isr_bh_sp, nv, TRUE, FALSE, FALSE);
    rm_isr_bh(nvl->isr_bh_sp, nv);
}

void nv_kern_rc_timer(
    unsigned long data
)
{
    nv_linux_state_t *nvl = (nv_linux_state_t *) data;
    nv_state_t *nv = NV_STATE_PTR(nvl);

    NV_CHECK_PCI_CONFIG_SPACE(nvl->timer_sp, nv, TRUE, TRUE, FALSE);

    rm_run_rc_callback(nvl->timer_sp, nv);
    mod_timer(&nvl->rc_timer, jiffies + HZ);  /* set another timeout in 1 second */
}

#if defined(NV_PM_SUPPORT_OLD_STYLE_APM)
/* kernel calls us with a power management event */
static int
nv_kern_apm_event(
    struct pm_dev *dev,
    pm_request_t rqst,
    void *data
)
{
    nv_state_t *nv;
    nv_linux_state_t *lnv;
    int status = RM_OK;
    nv_stack_t *sp = NULL;

    nv_printf(NV_DBG_INFO, "NVRM: nv_kern_apm_event: %d (0x%p)\n", rqst, data);

    for (lnv = nv_linux_devices; lnv != NULL;  lnv = lnv->next)
    {
        if (lnv->apm_nv_dev == dev)
        {
            break;
        }
    }

    if (lnv == NULL)
    {
        nv_printf(NV_DBG_WARNINGS, "NVRM: APM: invalid device!\n");
        return 1;
    }

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return 1;
    }

    nv = NV_STATE_PTR(lnv);

    NV_CHECK_PCI_CONFIG_SPACE(sp, nv, TRUE, TRUE, NV_MAY_SLEEP());

    switch (rqst)
    {
        case PM_RESUME:
            nv_printf(NV_DBG_INFO, "NVRM: APM: received resume event\n");
            nv_enable_pat_support();
            status = rm_power_management(sp, nv, 0, NV_PM_APM_RESUME);
            break;

        case PM_SUSPEND:
            nv_printf(NV_DBG_INFO, "NVRM: APM: received suspend event\n");
            status = rm_power_management(sp, nv, 0, NV_PM_APM_SUSPEND);
            nv_disable_pat_support();
            break;

        // 2.4 kernels sent a PM_SAVE_STATE request when powering down via
        // ACPI. just ignore it and return success so the power down works
        case PM_SAVE_STATE:
            status = RM_OK;
            break;

        default:
            nv_printf(NV_DBG_WARNINGS, "NVRM: APM: unsupported event: %d\n", rqst);
            status = 1;
    }

    NV_KMEM_CACHE_FREE_STACK(sp);

    if (status != RM_OK)
        nv_printf(NV_DBG_ERRORS, "NVRM: APM: failed event: %d\n", rqst);

    return status;
}
#endif /* defined(NV_PM_SUPPORT_OLD_STYLE_APM) */

/*
** nv_kern_ctl_open
**
** nv control driver open entry point.  Sessions are created here.
*/
int nv_kern_ctl_open(
    struct inode *inode,
    struct file *file
)
{
    nv_linux_state_t *nvl = &nv_ctl_device;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nv_file_private_t *nvfp = NV_GET_FILE_PRIVATE(file);
    static int count = 0;

    nv_printf(NV_DBG_INFO, "NVRM: nv_kern_ctl_open\n");

    down(&nvl->ldata_lock);

    /* save the nv away in file->private_data */
    nvfp->nvptr = nvl;

    if (NV_ATOMIC_READ(nvl->usage_count) == 0)
    {
        init_waitqueue_head(&nv_ctl_waitqueue);

        nv->flags |= (NV_FLAG_OPEN | NV_FLAG_CONTROL);

        if ((nv_acpi_init() < 0) &&
            (count++ < NV_MAX_RECURRING_WARNING_MESSAGES))
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: failed to register with the ACPI subsystem!\n");
        }
    }

    NV_ATOMIC_INC(nvl->usage_count);
    up(&nvl->ldata_lock);

    return 0;
}


/*
** nv_kern_ctl_close
*/
int nv_kern_ctl_close(
    struct inode *inode,
    struct file *file
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_FILEP(file);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nv_file_private_t *nvfp = NV_GET_FILE_PRIVATE(file);
    nv_stack_t *sp = nvfp->sp;
    static int count = 0;
    unsigned int i;

    nv_printf(NV_DBG_INFO, "NVRM: nv_kern_ctl_close\n");

    down(&nvl->ldata_lock);
    if (NV_ATOMIC_DEC_AND_TEST(nvl->usage_count))
    {
        nv->flags &= ~NV_FLAG_OPEN;

        if ((nv_acpi_uninit() < 0) &&
            (count++ < NV_MAX_RECURRING_WARNING_MESSAGES))
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: failed to unregister from the ACPI subsystem!\n");
        }
    }
    up(&nvl->ldata_lock);

    rm_free_unused_clients(sp, nv, NV_GET_FILE_PRIVATE(file));

    for (i = 0; i < NV_FOPS_STACK_INDEX_COUNT; ++i)
    {
        NV_KMEM_CACHE_FREE_STACK(nvfp->fops_sp[i]);
    }

    if (nvfp != NULL)
    {
        nv_free_file_private(nvfp);
        NV_SET_FILE_PRIVATE(file, NULL);
    }

    NV_KMEM_CACHE_FREE_STACK(sp);

    return 0;
}


void   NV_API_CALL  nv_set_dma_address_size(
    nv_state_t  *nv,
    NvU32       phys_addr_bits
)
{
    nv_linux_state_t    *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

#ifdef NV_SWIOTLB
    if (swiotlb && !nv_swiotlb && phys_addr_bits<=32)
    {
        nv_prints(NV_DBG_ERRORS, __swiotlb_warning);
        nv_procfs_add_warning("swiotlb", __swiotlb_warning);
        nv_swiotlb = 1;
    }
#endif

    nvl->dev->dma_mask = (((u64)1) << phys_addr_bits) - 1;
}

#if defined(HAVE_NV_XEN) && defined(CONFIG_XEN) && !defined(CONFIG_PARAVIRT)
#include "nv-xen.h"
#else
#if defined(NV_VMAP_PRESENT)
static unsigned long 
nv_map_guest_pages(nv_alloc_t *at,
                   NvU64 address,
                   NvU32 page_count,
                   NvU32 page_idx)
{
    struct page **pages;
    NvU32 j;
    unsigned long virt_addr;

    NV_KMALLOC(pages, sizeof(struct page *) * page_count);
    if (pages == NULL)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: failed to allocate vmap() page descriptor table!\n");
        return 0;
    }
    
    for (j = 0; j < page_count; j++)
    {
        pages[j] = NV_GET_PAGE_STRUCT(at->page_table[page_idx+j]->phys_addr);
    }
    
    NV_VMAP(virt_addr, pages, page_count, NV_ALLOC_MAPPING_CACHED(at->flags));
    NV_KFREE(pages, sizeof(struct page *) * page_count);

    return virt_addr;
}
#endif

RM_STATUS NV_API_CALL
nv_alias_pages(
    nv_state_t *nv,
    NvU32 page_cnt,
    NvU32 contiguous,
    NvU32 cache_type,
    NvU64 guest_id,
    NvU64 *pte_array,
    void **priv_data
)
{
    nv_alloc_t *at;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU32 i=0;
    nv_pte_t *page_ptr = NULL;
    
    page_cnt = RM_PAGES_TO_OS_PAGES(page_cnt);
    at = nvos_create_alloc(nvl->dev, page_cnt);

    if (at == NULL)
    {
        return RM_ERR_NO_FREE_MEM;
    }

    at->flags = nv_alloc_init_flags(cache_type, 0, contiguous, 0);
    at->flags |= NV_ALLOC_TYPE_GUEST;
                 
    at->nv = nv;
    at->order = nv_calc_order(at->num_pages * PAGE_SIZE);

    for (i=0; i < at->num_pages; ++i)
    {
        page_ptr = at->page_table[i];
        
        if (contiguous && i>0)
        {
            page_ptr->dma_addr = pte_array[0] + (i << PAGE_SHIFT);
        }
        else
        {
            page_ptr->dma_addr  = pte_array[i];
        }

        page_ptr->phys_addr = page_ptr->dma_addr; 

        /* aliased pages will be mapped on demand. */
        page_ptr->virt_addr = 0x0;
    }
    at->key = at->page_table[0]->dma_addr;
    at->guest_id    = guest_id;

    nvl_add_alloc(nvl, at);

    *priv_data = at;
    NV_ATOMIC_INC(at->usage_count);
    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    return RM_OK;
}

RM_STATUS NV_API_CALL
nv_guest_pfn_list(nv_state_t  *nv,
                  unsigned int key,
                  unsigned int pfn_count,
                  unsigned int offset_index,
                  unsigned int *user_pfn_list)
{
    return RM_OK;
}
#endif // HAVE_NV_XEN

/***
 *** EXPORTS to rest of resman
 ***/

/*
 * Given a physical address, find the associated 'at', track down
 * the actual page within the allocation and return a kernel virtual
 * mapping to it. Make sure to save the page offset if the address
 * isn't aligned.
 *
 * If the requested mapping spans more than one page, then determine
 * the individual pages and create a mapping with vmap().
 */
void* NV_API_CALL nv_alloc_kernel_mapping(
    nv_state_t *nv,
    NvU64       address,
    NvU64       size,
    void      **priv_data
)
{
    nv_alloc_t *at;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU32 i, offset, page_idx=0;

    down(&nvl->at_lock);
    at = nv_find_alloc(nvl, address, NV_ALLOC_TYPE_PCI);
    if (at != NULL)
    {
        offset = (address & ~NV_PAGE_MASK);
        address &= NV_PAGE_MASK;

        for (i = 0; i < at->num_pages; i++)
        {
            if ((address == at->page_table[i]->phys_addr)
                    || (address == at->page_table[i]->dma_addr))
            {
                page_idx = i;
                break;
            }
        }

        if (i == at->num_pages) /* not found */
        {
            up(&nvl->at_lock);
            return NULL;
        }
    }
    else
    {
        at = nv_find_alloc(nvl, address, NV_ALLOC_TYPE_AGP);
        if (at != NULL)
        {
            offset = (address - at->key);
            i = offset >> PAGE_SHIFT;
            offset = (address & ~NV_PAGE_MASK);

            if (at->page_table[i]->virt_addr == 0)
            {
                up(&nvl->at_lock);
                return NULL;
            }
        }
        else
        {
            up(&nvl->at_lock);
            return NULL; /* not found */
        }
    }
    up(&nvl->at_lock);

    if (((size + offset) <= PAGE_SIZE) && !NV_ALLOC_MAPPING_GUEST(at->flags))
    {
        *priv_data = NULL;
        return (void *)(at->page_table[i]->virt_addr + offset);
    }
    else
    {
#if defined(NV_VMAP_PRESENT)
        NvU32 j, page_count;
        unsigned long virt_addr;
        struct page **pages;

        size += offset; /* adjust mapping size */
        page_count = (size >> PAGE_SHIFT) + ((size & ~NV_PAGE_MASK) ? 1 : 0);

        if ((i + page_count) > at->num_pages)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: requested mapping exceeds allocation's boundary!\n");
            return NULL;
        }
        
        /* guest allocated pages have to mapped differently */
        if (NV_ALLOC_MAPPING_GUEST(at->flags))
        {
            virt_addr = nv_map_guest_pages(at, 
                                           nv->bars[NV_GPU_BAR_INDEX_REGS].address, 
                                           page_count, page_idx);
        }
        else
        {
            NV_KMALLOC(pages, sizeof(struct page *) * page_count);
            if (pages == NULL)
            {
                nv_printf(NV_DBG_ERRORS,
                          "NVRM: failed to allocate vmap() page descriptor table!\n");
                return NULL;
            }

            for (j = 0; j < page_count; j++)
              pages[j] = NV_GET_PAGE_STRUCT(at->page_table[i+j]->phys_addr);

            NV_VMAP(virt_addr, pages, page_count, NV_ALLOC_MAPPING_CACHED(at->flags));
            NV_KFREE(pages, sizeof(struct page *) * page_count);
        }
        
        if (virt_addr == 0)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to map pages!\n");
            return NULL;
        }

        *priv_data = (void *)(unsigned long)page_count;
        return (void *)(virt_addr + offset);
#else
        nv_printf(NV_DBG_ERRORS,
            "NVRM: This version of the Linux kernel does not provide the vmap()\n"
            "NVRM: kernel interface.  If you see this message, please update\n"
            "NVRM: your kernel to Linux 2.4.22 or install a distribution kernel\n"
            "NVRM: that supports the vmap() kernel interface.\n");
#endif
    }

    return NULL;
}

RM_STATUS NV_API_CALL nv_free_kernel_mapping(
    nv_state_t *nv,
    void       *address,
    void       *priv_data,
    NvBool      guest_mapping
)
{
#if defined(NV_VMAP_PRESENT)
    unsigned long virt_addr;
    NvU32 page_count;

    virt_addr = ((unsigned long)address & NV_PAGE_MASK);

    if (guest_mapping)
    {
        NV_IOUNMAP((void *)virt_addr, (unsigned long)priv_data * PAGE_SIZE);
    }
    else if (priv_data != NULL)
    {
        page_count = (unsigned long)priv_data;
        NV_VUNMAP(virt_addr, page_count);
    }
#endif
    return RM_OK;
}

RM_STATUS NV_API_CALL nv_alloc_pages(
    nv_state_t *nv,
    NvU32       page_count,
    NvBool      agp_memory,
    NvBool      contiguous,
    NvU32       cache_type,
    NvBool      zeroed,
    NvU64      *pte_array,
    void      **priv_data
)
{
    nv_alloc_t *at;
    RM_STATUS status = RM_ERR_NO_FREE_MEM;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    nv_stack_t *sp = NULL;
    NvU32 i, memory_type;

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: nv_alloc_pages: %d pages\n", page_count);
    nv_printf(NV_DBG_MEMINFO, "NVRM: VM:    agp %d  contig %d  cache_type %d\n",
        agp_memory, contiguous, cache_type);

#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
    memory_type = agp_memory ? NV_MEMORY_TYPE_AGP : NV_MEMORY_TYPE_SYSTEM;
#else
    memory_type = NV_MEMORY_TYPE_SYSTEM;
#endif

#if !defined(NV_VMWARE)
    if (nv_encode_caching(NULL, cache_type, memory_type))
        return RM_ERR_NOT_SUPPORTED;
#endif

    page_count = RM_PAGES_TO_OS_PAGES(page_count);
    at = nvos_create_alloc(nvl->dev, page_count);
    if (at == NULL)
        return RM_ERR_NO_FREE_MEM;

    at->flags = nv_alloc_init_flags(cache_type, agp_memory, contiguous, zeroed);
    at->nv = nv;

    if (agp_memory)
    {
        NvU32 offset;

        if (!NV_AGP_ENABLED(nv))
        {
            status = RM_ERR_NOT_SUPPORTED;
            goto failed;
        }

        NV_KMEM_CACHE_ALLOC_STACK(sp);
        if (sp == NULL)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
            status = RM_ERR_INSUFFICIENT_RESOURCES;
            goto failed;
        }

        /* allocate agp-able memory */
        if (NV_OSAGP_ENABLED(nv))
        {
            /* agpgart will allocate all of the underlying memory */
            status = KernAllocAGPPages(sp, nv, page_count, priv_data, &offset);
            if (status != RM_OK)
                goto failed;

            KernLoadAGPPages(nv, at, *priv_data);
        } else {
            status = rm_alloc_agp_pages(sp, nv, page_count, priv_data, &offset);
            if (status != RM_OK)
                goto failed;
        }

        NV_KMEM_CACHE_FREE_STACK(sp);

        at->priv_data = *priv_data;
        nvl_add_alloc(nvl, at);

        pte_array[0] = (nv->agp.address + (offset << PAGE_SHIFT));
        at->key = pte_array[0];
    }
    else
    {
        status = nv_vm_malloc_pages(nv, at);
        if (status != RM_OK)
            goto failed;

        /* 
         * must be page-aligned or mmap will fail
         * so use the first page, which is page-aligned. this way, our 
         * allocated page table does not need to be page-aligned
         */
        for (i = 0; i < ((contiguous) ? 1 : page_count); i++)
            pte_array[i] = at->page_table[i]->dma_addr;

        at->key = at->page_table[0]->phys_addr;
        nvl_add_alloc(nvl, at);
    }

    *priv_data = at;
    NV_ATOMIC_INC(at->usage_count);

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    return RM_OK;

failed:
    if (sp != NULL)
        NV_KMEM_CACHE_FREE_STACK(sp);
    nvos_free_alloc(at);

    return status;
}

RM_STATUS NV_API_CALL nv_free_pages(
    nv_state_t *nv,
    NvU32 page_count,
    NvU32 agp_memory,
    NvU32 contiguous,
    NvU32 cache_type,
    void *priv_data
)
{
    RM_STATUS rmStatus = RM_OK;
    nv_alloc_t *at = priv_data;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    nv_stack_t *sp = NULL;

    page_count = RM_PAGES_TO_OS_PAGES(page_count);
    nv_printf(NV_DBG_MEMINFO,
        "NVRM: VM: nv_free_pages: 0x%llx 0x%x\n", at->key, page_count);

    /* only lock ldata while removing 'at' from the list */
    down(&nvl->at_lock);

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    /*
     * If the 'at' usage count doesn't drop to zero here, not all of
     * the user mappings have been torn down in time - we can't
     * safely free the memory. We report success back to the RM, but
     * defer the actual free until later.
     *
     * This is described in greater detail in the comments above the
     * nv_kern_vma_(open|release)() callbacks above.
     */
    if (!NV_ATOMIC_DEC_AND_TEST(at->usage_count))
    {
        up(&nvl->at_lock);
        return RM_OK;
    }

    nvl_remove_alloc(nvl, at);
    up(&nvl->at_lock);

    if (NV_ALLOC_MAPPING_GUEST(at->flags))
    {
        goto done;
    }

    if (agp_memory)
    {
        if (!NV_AGP_ENABLED(nv))
        {
            rmStatus = RM_ERR_NOT_SUPPORTED;
            goto failed;
        }

        NV_KMEM_CACHE_ALLOC_STACK(sp);
        if (sp == NULL)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
            rmStatus = RM_ERR_NO_FREE_MEM;
            goto failed;
        }

        if (NV_OSAGP_ENABLED(nv))
            rmStatus = KernFreeAGPPages(sp, nv, at->priv_data);
        else
            rmStatus = rm_free_agp_pages(sp, nv, at->priv_data);

        NV_KMEM_CACHE_FREE_STACK(sp);
    } else
        nv_vm_free_pages(nv, at);

done:
    nvos_free_alloc(at);

failed:
    return rmStatus;
}

static void nv_lock_init_locks
( 
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl;
    nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    NV_INIT_MUTEX(&nvl->ldata_lock);
    NV_INIT_MUTEX(&nvl->at_lock);

    NV_ATOMIC_SET(nvl->usage_count, 0);
}

void NV_API_CALL nv_post_event(
    nv_state_t *nv,
    nv_event_t *event,
    NvU32       handle,
    NvU32       index
)
{
    nv_file_private_t *nvfp = event->file;
    unsigned long eflags;
    nvidia_event_t *nvet;

    nv_printf(NV_DBG_EVENTINFO, "NVRM: posting event on 0x%x:0x%x\n",
        event, nvfp);

    NV_KMALLOC_ATOMIC(nvet, sizeof(nvidia_event_t));
    if (nvet == NULL)
        return;

    NV_SPIN_LOCK_IRQSAVE(&nvfp->fp_lock, eflags);

    // Insert the event struct in the queue
    if (nvfp->event_tail != NULL)
        nvfp->event_tail->next = nvet;
    if (nvfp->event_head == NULL)
        nvfp->event_head = nvet;
    nvfp->event_tail = nvet;
    nvet->next = NULL;

    // copy the event into the queue
    nvet->event         = *event;

    // set the handle for this event
    nvet->event.hObject = handle;
    nvet->event.index   = index;

    wake_up_interruptible(&nvfp->waitqueue);
    NV_SPIN_UNLOCK_IRQRESTORE(&nvfp->fp_lock, eflags);
}

int NV_API_CALL nv_get_event(
    nv_state_t *nv,
    void       *file,
    nv_event_t *event,
    NvU32      *more_events
)
{
    nv_file_private_t *nvfp = file;
    nvidia_event_t *nvet;
    unsigned long eflags;

    NV_SPIN_LOCK_IRQSAVE(&nvfp->fp_lock, eflags);
    if (nvfp->event_head == NULL)
    {
        NV_SPIN_UNLOCK_IRQRESTORE(&nvfp->fp_lock, eflags);
        return -1;
    }

    nvet = nvfp->event_head;

    *event = nvet->event;
    
    if (nvfp->event_tail == nvet)
        nvfp->event_tail = NULL;
    nvfp->event_head = nvet->next;

    NV_KFREE(nvet, sizeof(nvidia_event_t));

    if (more_events)
        *more_events = (nvfp->event_head != NULL);

    nv_printf(NV_DBG_EVENTINFO, "NVRM: returning event: 0x%p\n", event);
    nv_printf(NV_DBG_EVENTINFO, "NVRM:     hParent: 0x%x\n", event->hParent);
    nv_printf(NV_DBG_EVENTINFO, "NVRM:     hObject: 0x%x\n", event->hObject);
    nv_printf(NV_DBG_EVENTINFO, "NVRM:     file:    0x%p\n", event->file);
    nv_printf(NV_DBG_EVENTINFO, "NVRM:     fd:      %d\n", event->fd);
    if (more_events)
        nv_printf(NV_DBG_EVENTINFO, "NVRM: more events: %d\n", *more_events);

    NV_SPIN_UNLOCK_IRQRESTORE(&nvfp->fp_lock, eflags);

    return 0;
}


RM_STATUS NV_API_CALL nv_agp_init(
    nv_state_t *nv,
    NvU64      *phys_start,
    NvU64      *agp_limit,
    NvU32       config
)
{
#if !defined(NV_XEN_SUPPORT_OLD_STYLE_KERNEL)
    RM_STATUS status = RM_ERROR;
    static int old_error = 0;
    nv_stack_t *sp = NULL;

    if (NV_AGP_ENABLED(nv))
        return RM_ERR_STATE_IN_USE;

    if (config == NVOS_AGP_CONFIG_DISABLE_AGP)
    {
        nv->agp_config = NVOS_AGP_CONFIG_DISABLE_AGP;
        nv->agp_status = NV_AGP_STATUS_DISABLED;
        return RM_OK;
    }

    nv_printf(NV_DBG_SETUP, "NVRM: nv_agp_init\n");

    nv->agp_config = NVOS_AGP_CONFIG_DISABLE_AGP;
    nv->agp_status = NV_AGP_STATUS_FAILED;

    if (config & NVOS_AGP_CONFIG_OSAGP)
    {
        NV_KMEM_CACHE_ALLOC_STACK(sp);
        if (sp == NULL)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
            return RM_ERR_NO_FREE_MEM;
        }

        status = KernInitAGP(sp, nv, phys_start, agp_limit);

        /* if enabling agpgart was successfull, register it,
         * and check about overrides
         */
        if (status == RM_OK)
        {
            nv->agp_config = NVOS_AGP_CONFIG_OSAGP;
            nv->agp_status = NV_AGP_STATUS_ENABLED;

            /* make sure we apply our overrides in this case */
            rm_update_agp_config(sp, nv);
        }

        NV_KMEM_CACHE_FREE_STACK(sp);

        if ((status != RM_OK) && !(config & NVOS_AGP_CONFIG_NVAGP) && !old_error)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: unable to initialize the Linux AGPGART driver, please \n"
                "NVRM: verify you configured your kernel to include support  \n"
                "NVRM: for AGPGART (either statically linked, or as a kernel \n"
                "NVRM: module). Please also make sure you selected support   \n"
                "NVRM: for your AGP chipset.                                 \n");
#if !defined(KERNEL_2_4)
            nv_printf(NV_DBG_ERRORS,
                "NVRM:                                                       \n"
                "NVRM: note that as of Linux 2.6 AGPGART, all chipset/vendor \n"
                "NVRM: drivers are split into independent modules; make sure \n"
                "NVRM: the correct one is loaded for your chipset.           \n");
#endif
            old_error = 1;
        }

        /* if agpgart is loaded, but we failed to initialize it,
         * we'd better not attempt nvagp, or we're likely to lock
         * the machine.
         */
        if (status != RM_ERR_NOT_SUPPORTED)
            return status;
    }

    /* we're either explicitly not using agpgart,
     * or trying to use agpgart failed
     * make sure the user did not specify "use agpgart only"
     */
    if ( (!NV_AGP_ENABLED(nv)) && (config & NVOS_AGP_CONFIG_NVAGP) )
    {
        /* make sure the user does not have agpgart loaded */
#if defined(KERNEL_2_4)
        if (inter_module_get("drm_agp"))
        {
            inter_module_put("drm_agp");
            nv_printf(NV_DBG_WARNINGS, "NVRM: not using NVAGP, AGPGART is loaded!\n");
            return status;
        }
#elif defined(AGPGART)
#if (NV_AGP_BACKEND_ACQUIRE_ARGUMENT_COUNT == 1)
        if (!list_empty(&agp_bridges))
        {
            nv_printf(NV_DBG_WARNINGS,
                      "NVRM: not using NVAGP, an AGPGART backend is loaded!\n");
            return status;
        }
#else
        int error;
        /*
         * We can only safely use NvAGP when no backend has been
         * registered with the AGPGART frontend. This condition
         * is only met when the acquire function returns -EINVAL.
         *
         * Other return codes indicate that a backend is present
         * and was either acquired, busy or else unavailable.
         */
        if ((error = agp_backend_acquire()) != -EINVAL)
        {
            if (!error) agp_backend_release();
            nv_printf(NV_DBG_WARNINGS,
                      "NVRM: not using NVAGP, an AGPGART backend is loaded!\n");
            return status;
        }
#endif
#endif /* AGPGART  */
#if defined(CONFIG_X86_64) && defined(CONFIG_GART_IOMMU)
        nv_printf(NV_DBG_WARNINGS,
            "NVRM: not using NVAGP, kernel was compiled with GART_IOMMU support!!\n");
#else

        NV_KMEM_CACHE_ALLOC_STACK(sp);
        if (sp == NULL)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
            return RM_ERR_NO_FREE_MEM;
        }

        status = rm_init_agp(sp, nv, NULL, NULL);
        if (status == RM_OK)
        {
            nv->agp_config = NVOS_AGP_CONFIG_NVAGP;
            nv->agp_status = NV_AGP_STATUS_ENABLED;
        }

        NV_KMEM_CACHE_FREE_STACK(sp);
#endif
    }

    if (NV_AGP_ENABLED(nv))
        old_error = 0; /* report new errors */

    return status;
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: AGP is not supported on Xen kernels.\n");
    return RM_ERR_NOT_SUPPORTED;
#endif
}

RM_STATUS NV_API_CALL nv_agp_teardown(
    nv_state_t *nv
)
{
    RM_STATUS status = RM_ERROR;
    nv_stack_t *sp = NULL;

    nv_printf(NV_DBG_SETUP, "NVRM: nv_agp_teardown\n");

    /* little sanity check won't hurt */
    if (!NV_AGP_ENABLED(nv))
        return RM_ERR_NOT_SUPPORTED;

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return RM_ERR_NO_FREE_MEM;
    }

    if (NV_OSAGP_ENABLED(nv))
        status = KernTeardownAGP(sp, nv);
    else if (NV_NVAGP_ENABLED(nv))
        status = rm_teardown_agp(sp, nv);

    NV_KMEM_CACHE_FREE_STACK(sp);

    nv->agp_config = NVOS_AGP_CONFIG_DISABLE_AGP;
    nv->agp_status = NV_AGP_STATUS_DISABLED;

    return status;
}

/* set a timer to go off every second */
int NV_API_CALL nv_start_rc_timer(
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (nv->rc_timer_enabled)
        return -1;

    nv_printf(NV_DBG_INFO, "NVRM: initializing rc timer\n");
    init_timer(&nvl->rc_timer);
    nvl->rc_timer.function = nv_kern_rc_timer;
    nvl->rc_timer.data = (unsigned long) nv;
    nv->rc_timer_enabled = 1;
    mod_timer(&nvl->rc_timer, jiffies + HZ); /* set our timeout for 1 second */
    nv_printf(NV_DBG_INFO, "NVRM: rc timer initialized\n");

    return 0;
}

int NV_API_CALL nv_stop_rc_timer(
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (!nv->rc_timer_enabled)
        return -1;

    nv_printf(NV_DBG_INFO, "NVRM: stopping rc timer\n");
    nv->rc_timer_enabled = 0;
    del_timer_sync(&nvl->rc_timer);
    nv_printf(NV_DBG_INFO, "NVRM: rc timer stopped\n");

    return 0;
}

/* make sure the pci_driver called probe for all of our devices.
 * we've seen cases where rivafb claims the device first and our driver
 * doesn't get called.
 */
static int
nvos_count_devices(nv_stack_t *sp)
{
    struct pci_dev *dev;
    int count = 0;

    dev = NV_PCI_GET_CLASS(PCI_CLASS_DISPLAY_VGA << 8, NULL);
    while (dev)
    {
        if ((dev->vendor == 0x10de) && (dev->device >= 0x20) &&
            !rm_is_legacy_device(sp, dev->device, TRUE))
            count++;
        dev = NV_PCI_GET_CLASS(PCI_CLASS_DISPLAY_VGA << 8, dev);
    }

    dev = NV_PCI_GET_CLASS(PCI_CLASS_DISPLAY_3D << 8, NULL);
    while (dev)
    {
        if ((dev->vendor == 0x10de) && (dev->device >= 0x20) &&
            !rm_is_legacy_device(sp, dev->device, TRUE))
            count++;
        dev = NV_PCI_GET_CLASS(PCI_CLASS_DISPLAY_3D << 8, dev);
    }

    dev = NV_PCI_GET_CLASS(PCI_CLASS_MULTIMEDIA_OTHER << 8, NULL);
    while (dev)
    {
        if ((dev->vendor == 0x10de) && (dev->device == 0x0e00))
            count++;
        dev = NV_PCI_GET_CLASS(PCI_CLASS_MULTIMEDIA_OTHER << 8, dev);
    }

    return count;
}

/* find nvidia devices and set initial state */
int
nv_kern_probe
(
    struct pci_dev *dev,
    const struct pci_device_id *id_table
)
{
    nv_state_t *nv;
    nv_linux_state_t *nvl = NULL;
    unsigned int i, j;
    int flags = 0;
    nv_stack_t *sp = NULL;

    if (NV_IS_SMU_DEVICE(dev))
    {
        return nv_kern_smu_probe(dev);
    }

    nv_printf(NV_DBG_SETUP, "NVRM: probing 0x%x 0x%x, class 0x%x\n",
        dev->vendor, dev->device, dev->class);

    if ((dev->class == (PCI_CLASS_MULTIMEDIA_OTHER << 8)) &&
        (dev->device == 0x0e00))
    {
        flags = NV_FLAG_GVI;
    }

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return -1;
    }

    if (!(flags & NV_FLAG_GVI))
    {
        if ((dev->vendor != 0x10de) || (dev->device < 0x20) || 
            ((dev->class != (PCI_CLASS_DISPLAY_VGA << 8)) &&
             (dev->class != (PCI_CLASS_DISPLAY_3D << 8))) ||
            rm_is_legacy_device(sp, dev->device, FALSE))
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: ignoring the legacy GPU %04x:%02x:%02x.%x\n",
                      NV_PCI_DOMAIN_NUMBER(dev), NV_PCI_BUS_NUMBER(dev), NV_PCI_SLOT_NUMBER(dev),
                      PCI_FUNC(dev->devfn));
            goto failed;
        }
    }

    num_probed_nv_devices++;

    if (pci_enable_device(dev) != 0)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: pci_enable_device failed, aborting\n");
        goto failed;
    }

    if (dev->irq == 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Can't find an IRQ for your NVIDIA card!\n");
        nv_printf(NV_DBG_ERRORS, "NVRM: Please check your BIOS settings.\n");
        nv_printf(NV_DBG_ERRORS, "NVRM: [Plug & Play OS] should be set to NO\n");
        nv_printf(NV_DBG_ERRORS, "NVRM: [Assign IRQ to VGA] should be set to YES \n");
        goto failed;
    }

    for (i = 0; i < (NV_GPU_NUM_BARS - 1); i++)
    {
        if (NV_PCI_RESOURCE_VALID(dev, i))
        {
            if ((NV_PCI_RESOURCE_FLAGS(dev, i) & PCI_BASE_ADDRESS_MEM_TYPE_64) &&
                ((NV_PCI_RESOURCE_START(dev, i) >> PAGE_SHIFT) > 0xfffffULL))
            {
                struct pci_dev *bridge = dev->bus->self;
                NvU32 base_upper, limit_upper;

                pci_read_config_dword(bridge, PCI_PREF_BASE_UPPER32,
                        &base_upper);
                pci_read_config_dword(bridge, PCI_PREF_LIMIT_UPPER32,
                        &limit_upper);

                if ((base_upper != 0) && (limit_upper != 0))
                    continue;

                nv_printf(NV_DBG_ERRORS,
                    "NVRM: This is a 64-bit BAR mapped above 4GB by the system\n"
                    "NVRM: BIOS or the Linux kernel, but the PCI bridge\n"
                    "NVRM: immediately upstream of this GPU does not define\n"
                    "NVRM: a matching prefetchable memory window.  This\n"
                    "NVRM: may be due to a known Linux kernel bug.  Please see\n"
                    "NVRM: the README section on 64-bit BARs for additional\n"
                    "NVRM: information.\n");

                goto failed;
            }
            else
                continue;
        }
        nv_printf(NV_DBG_ERRORS,
            "NVRM: This PCI I/O region assigned to your NVIDIA device is invalid:\n"
            "NVRM: BAR%d is %dM @ 0x%llx (PCI:%04x:%02x:%02x.%x)\n", i,
            (NV_PCI_RESOURCE_SIZE(dev, i) >> 20),
            (NvU64)NV_PCI_RESOURCE_START(dev, i),
            NV_PCI_DOMAIN_NUMBER(dev),
            NV_PCI_BUS_NUMBER(dev), NV_PCI_SLOT_NUMBER(dev), PCI_FUNC(dev->devfn));
#if defined(NVCPU_X86)
        if ((NV_PCI_RESOURCE_FLAGS(dev, i) & PCI_BASE_ADDRESS_MEM_TYPE_64) &&
            ((NV_PCI_RESOURCE_START(dev, i) >> PAGE_SHIFT) > 0xfffffULL))
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: This is a 64-bit BAR mapped above 4GB by the system\n"
                "NVRM: BIOS or the Linux kernel.  The NVIDIA Linux/x86\n"
                "NVRM: graphics driver and other system software components\n"
                "NVRM: do not support this configuration.\n");
        }
        else
#endif
        if (NV_PCI_RESOURCE_FLAGS(dev, i) & PCI_BASE_ADDRESS_MEM_TYPE_64)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: This is a 64-bit BAR, which early Linux kernels and\n"
                "NVRM: other system software components are known to\n"
                "NVRM: handle incorrectly.  Please see the README section\n"
                "NVRM: on 64-bit BARs for more information.\n");
        }
        else
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: The system BIOS may have misconfigured your GPU.\n");
        }
        goto failed;
    }

    if (!request_mem_region(NV_PCI_RESOURCE_START(dev, NV_GPU_BAR_INDEX_REGS),
                            NV_PCI_RESOURCE_SIZE(dev, NV_GPU_BAR_INDEX_REGS), "nvidia"))
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: request_mem_region failed for %dM @ 0x%llx. This can\n"
            "NVRM: occur when a driver such as rivatv is loaded and claims\n"
            "NVRM: ownership of the device's registers.\n",
            (NV_PCI_RESOURCE_SIZE(dev, NV_GPU_BAR_INDEX_REGS) >> 20),
            (NvU64)NV_PCI_RESOURCE_START(dev, NV_GPU_BAR_INDEX_REGS));
        goto failed;
    }

    NV_KMALLOC(nvl, sizeof(nv_linux_state_t));
    if (nvl == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate memory\n");
        goto err_not_supported;
    }

    os_mem_set(nvl, 0, sizeof(nv_linux_state_t));

    if (nv_linux_devices == NULL)
        nv_linux_devices = nvl;
    else
    {
        nv_linux_state_t *tnvl;
        for (tnvl = nv_linux_devices; tnvl->next != NULL;  tnvl = tnvl->next);
        tnvl->next = nvl;
    }

    nvl->device_num = num_nv_devices;
    nv  = NV_STATE_PTR(nvl);

    pci_set_drvdata(dev, (void *)nvl);

    /* default to 32-bit PCI bus address space */
    dev->dma_mask = 0xffffffffULL;

    nvl->dev          = dev;
    nv->vendor_id     = dev->vendor;
    nv->device_id     = dev->device;
    nv->subsystem_id  = dev->subsystem_device;
    nv->os_state      = (void *) nvl;
    nv->domain        = NV_PCI_DOMAIN_NUMBER(dev);
    nv->bus           = NV_PCI_BUS_NUMBER(dev);
    nv->slot          = NV_PCI_SLOT_NUMBER(dev);
    nv->handle        = dev;
    nv->flags         |= flags;

    nv_lock_init_locks(nv);

    for (i = 0, j = 0; i < NVRM_PCICFG_NUM_BARS && j < NV_GPU_NUM_BARS; i++)
    {
        if ((NV_PCI_RESOURCE_VALID(dev, i)) &&
            (NV_PCI_RESOURCE_FLAGS(dev, i) & PCI_BASE_ADDRESS_SPACE)
                == PCI_BASE_ADDRESS_SPACE_MEMORY)
        {
            nv->bars[j].address = NV_PCI_RESOURCE_START(dev, i);
            nv->bars[j].size    = NV_PCI_RESOURCE_SIZE(dev, i);
            nv->bars[j].offset  = NVRM_PCICFG_BAR_OFFSET(i);
            j++;
        }
    }
    nv->regs = &nv->bars[NV_GPU_BAR_INDEX_REGS];
    nv->fb   = &nv->bars[NV_GPU_BAR_INDEX_FB];

    nv->interrupt_line = dev->irq;

    tasklet_init(&nvl->tasklet, nv_kern_isr_bh, (NvUPtr)NV_STATE_PTR(nvl));
    tasklet_disable(&nvl->tasklet);

    pci_set_master(dev);

#if defined(CONFIG_VGA_ARB)
#if defined(VGA_DEFAULT_DEVICE)
    vga_tryget(VGA_DEFAULT_DEVICE, VGA_RSRC_LEGACY_MASK);
#endif
    vga_set_legacy_decoding(dev, VGA_RSRC_NONE);
#endif

    if (NV_IS_GVI_DEVICE(nv))
    {
        if (!rm_gvi_init_private_state(sp, nv))
        {
            nv_printf(NV_DBG_ERRORS, "NVGVI: rm_init_gvi_private_state() failed!\n");
            goto err_not_supported;
        }

        if (rm_gvi_attach_device(sp, nv) != RM_OK)
        {
            rm_gvi_free_private_state(sp, nv);
            goto err_not_supported;
        }
    }
    else
    {
        nv_check_pci_config_space(nv, FALSE);

        if (rm_is_supported_device(sp, nv) != RM_OK)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: The NVIDIA GPU %04x:%02x:%02x.%x (PCI ID: %04x:%04x) installed\n"
                "NVRM: in this system is not supported by the %s NVIDIA Linux\n"
                "NVRM: graphics driver release.  Please see 'Appendix A -\n"
                "NVRM: Supported NVIDIA GPU Products' in this release's README,\n"
                "NVRM: available on the Linux graphics driver download page at\n"
                "NVRM: www.nvidia.com.\n",
                nv->domain, nv->bus, nv->slot, PCI_FUNC(dev->devfn), nv->vendor_id, nv->device_id,
                NV_VERSION_STRING);
            goto err_not_supported;
        }

        if (!rm_init_private_state(sp, nv))
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: rm_init_private_state() failed!\n");
            goto err_zero_dev;
        }
    }

    nv_printf(NV_DBG_INFO,
            "NVRM: PCI:%04x:%02x:%02x.%x (%04x:%04x): BAR0 @ 0x%llx (%lluMB)\n",
            nv->domain, nv->bus, nv->slot, PCI_FUNC(dev->devfn),
            nv->vendor_id, nv->device_id, nv->regs->address,
            (nv->regs->size >> 20));
    nv_printf(NV_DBG_INFO,
            "NVRM: PCI:%04x:%02x:%02x.%x (%04x:%04x): BAR1 @ 0x%llx (%lluMB)\n",
            nv->domain, nv->bus, nv->slot, PCI_FUNC(dev->devfn),
            nv->vendor_id, nv->device_id, nv->fb->address,
            (nv->fb->size >> 20));

    num_nv_devices++;

    for (i = 0; i < NV_GPU_NUM_BARS; i++)
    {
        if (nv->bars[i].size != 0)
        {
            if (nv_user_map_register(nv->bars[i].address,
                        nv->bars[i].size) != 0)
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM: failed to register usermap for BAR %u!\n", i);
                for (j = 0; j < i; j++)
                {
                    nv_user_map_unregister(nv->bars[j].address,
                            nv->bars[j].size);
                }
                goto err_zero_dev;
            }
        }
    }

    NV_KMEM_CACHE_FREE_STACK(sp);

    return 0;

err_zero_dev:
    rm_free_private_state(sp, nv);
err_not_supported:
    if (nvl != NULL)
    {
        if (nvl == nv_linux_devices)
            nv_linux_devices = nvl->next;
        else
        {
            nv_linux_state_t *tnvl;
            for (tnvl = nv_linux_devices; tnvl->next != nvl;  tnvl = tnvl->next);
            tnvl->next = nvl->next;
        }
        NV_KFREE(nvl, sizeof(nv_linux_state_t));
    }
    release_mem_region(NV_PCI_RESOURCE_START(dev, NV_GPU_BAR_INDEX_REGS),
                       NV_PCI_RESOURCE_SIZE(dev, NV_GPU_BAR_INDEX_REGS));
    NV_PCI_DISABLE_DEVICE(dev);
failed:
    NV_KMEM_CACHE_FREE_STACK(sp);
    return -1;
}

int
nv_kern_smu_probe
(
    struct pci_dev *dev
)
{
    nv_smu_state_t *nv = &nv_smu_device;

    nv_printf(NV_DBG_SETUP, "NVRM: probing 0x%x 0x%x, class 0x%x\n",
        dev->vendor, dev->device, dev->class);

    if ((dev->vendor != 0x10de) || 
        (dev->class != (PCI_CLASS_PROCESSOR_CO << 8)) ||
        (dev->device != NV_PCI_DEVICE_ID_SMU))  
    {
        nv_printf(NV_DBG_INFO, "NVRM: ignoring the SMU %04x:%02x:%02x.%x\n",
                  NV_PCI_DOMAIN_NUMBER(dev), NV_PCI_BUS_NUMBER(dev),
                  NV_PCI_SLOT_NUMBER(dev), PCI_FUNC(dev->devfn));
        goto failed;
    }

    if (nv->handle != NULL)
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: More than one SMU device? Driver not yet designed to handle this case\n");
        goto failed;
    }

    if (pci_enable_device(dev) != 0)
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: pci_enable_device for SMU device failed, aborting\n");
        goto failed;
    }

    // validate BAR0 
    if (!NV_PCI_RESOURCE_VALID(dev, 0))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: This PCI I/O region assigned to the SMU device is invalid:\n"
            "NVRM: BAR0 is %dM @ 0x%08x (PCI:%04x:%02x:%02x.%x)\n", 
            NV_PCI_RESOURCE_SIZE(dev, 0) >> 20, NV_PCI_RESOURCE_START(dev, 0),
            NV_PCI_DOMAIN_NUMBER(dev), NV_PCI_BUS_NUMBER(dev),
            NV_PCI_SLOT_NUMBER(dev), PCI_FUNC(dev->devfn));
        goto failed;
    }

    if (!request_mem_region(NV_PCI_RESOURCE_START(dev, 0),
                            NV_PCI_RESOURCE_SIZE(dev, 0), "nvidia"))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: request_mem_region failed for %dM @ 0x%08x.\n",
            NV_PCI_RESOURCE_SIZE(dev, 0) >> 20,
            NV_PCI_RESOURCE_START(dev, 0));

        goto failed;
    }

    pci_set_drvdata(dev, (void *)nv);

    /* default to 32-bit PCI bus address space */
    dev->dma_mask = 0xffffffffULL;

    nv->vendor_id     = dev->vendor;
    nv->device_id     = dev->device;
    nv->domain        = NV_PCI_DOMAIN_NUMBER(dev);
    nv->bus           = NV_PCI_BUS_NUMBER(dev);
    nv->slot          = NV_PCI_SLOT_NUMBER(dev);
    nv->handle        = dev;

    if ((NV_PCI_RESOURCE_VALID(dev, 0)) &&
        (NV_PCI_RESOURCE_FLAGS(dev, 0) & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY)
    {
        nv->bar0.address = NV_PCI_RESOURCE_START(dev, 0);
        nv->bar0.size    = NV_PCI_RESOURCE_SIZE(dev, 0);
        nv->bar0.offset  = NVRM_PCICFG_BAR_OFFSET(0);
    }

    nv->regs = &nv->bar0;

    nv_printf(NV_DBG_INFO, "NVRM: %04x:%02x:%02x.%x %04x:%04x - 0x%08x [size=%dK]\n",
            nv->domain, nv->bus, nv->slot, PCI_FUNC(dev->devfn),
            nv->vendor_id, nv->device_id, nv->regs->address,
            nv->regs->size / (1024));

    return 0;

failed:
    return -1;
}

int NV_API_CALL nv_no_incoherent_mappings(void)
{
#if defined(NV_CHANGE_PAGE_ATTR_PRESENT) || defined(NV_SET_PAGES_UC_PRESENT)
    return (nv_update_memory_types);
#else
    return 0;
#endif
}

#if defined(NV_PM_SUPPORT_DEVICE_DRIVER_MODEL)

static int
nv_power_management(
    struct pci_dev *dev, 
    u32 pci_state,
    u32 power_state
)
{
    nv_state_t *nv;
    nv_linux_state_t *lnv = NULL;
    int status = RM_OK;
    nv_stack_t *sp = NULL;

    nv_printf(NV_DBG_INFO, "NVRM: nv_power_management: %d\n", pci_state);
    lnv = pci_get_drvdata(dev);

    if (!lnv || (lnv->dev != dev))
    {
        nv_printf(NV_DBG_WARNINGS, "NVRM: PM: invalid device!\n");
        return -1;
    }

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return -1;
    }

    nv = NV_STATE_PTR(lnv);
    NV_CHECK_PCI_CONFIG_SPACE(sp, nv, TRUE, TRUE, NV_MAY_SLEEP());

    switch (pci_state)
    {
#if defined(NV_PM_SUPPORT_NEW_STYLE_APM)
        case PCI_D3hot:
            nv_printf(NV_DBG_INFO, "NVRM: APM: received suspend event\n");
            status = rm_power_management(sp, nv, 0, NV_PM_APM_SUSPEND);
            nv_disable_pat_support();
            break;

        case PCI_D0:
            nv_printf(NV_DBG_INFO, "NVRM: APM: received resume event\n");
            nv_enable_pat_support();
            status = rm_power_management(sp, nv, 0, NV_PM_APM_RESUME);
            break;

#else /* end of NV_PM_SUPPORT_NEW_STYLE_APM */
         case PCI_D3hot:
            nv_printf(NV_DBG_INFO, "NVRM: ACPI: received suspend event\n");
            tasklet_disable(&lnv->tasklet);
            status = rm_power_management(sp, nv, 0, power_state);
            nv_disable_pat_support();
            break;

        case PCI_D0:
            nv_printf(NV_DBG_INFO, "NVRM: ACPI: received resume event\n");
            nv_enable_pat_support();
            status = rm_power_management(sp, nv, 0, power_state);
            tasklet_enable(&lnv->tasklet);
            break;

#endif /* End of NV_PM_SUPPORT_NEW_STYLE_APM */

        default:
            nv_printf(NV_DBG_WARNINGS, "NVRM: PM: unsupported event: %d\n", pci_state);
            status = -1;
    }

    NV_KMEM_CACHE_FREE_STACK(sp);

    if (status != RM_OK)
        nv_printf(NV_DBG_ERRORS, "NVRM: PM: failed event: %d\n", pci_state);

    return status;
}

static int nv_kern_suspend(
    struct pci_dev *dev,
    pm_message_t state
)
{
    int pci_state = -1;
    u32 power_state;
    nv_state_t *nv;
    nv_linux_state_t *lnv = NULL;

    if (NV_IS_SMU_DEVICE(dev))
    {
        return nv_kern_smu_suspend();
    }

    lnv = pci_get_drvdata(dev);

    if (!lnv || (lnv->dev != dev))
    {
        nv_printf(NV_DBG_WARNINGS, "NVRM: PM: invalid device!\n");
        return -1;
    }

    nv = NV_STATE_PTR(lnv);

    if (NV_IS_GVI_DEVICE(nv))
    {
        return nv_gvi_kern_suspend(dev, state);
    }

#if !defined(NV_PM_MESSAGE_T_PRESENT)
    pci_state = state;
#elif defined(NV_PCI_CHOOSE_STATE_PRESENT)
    pci_state = PCI_D3hot;
#endif
    
    power_state = NV_PM_ACPI_STANDBY;

#if defined(NV_PM_MESSAGE_T_HAS_EVENT)
    if (state.event == PM_EVENT_FREEZE) /* for hibernate */
        power_state = NV_PM_ACPI_HIBERNATE;
#endif

    return nv_power_management(dev, pci_state, power_state);
}

static int nv_kern_smu_suspend(void)
{
    nv_stack_t *sp = NULL;

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return RM_ERR_NO_FREE_MEM;
    }

    rm_suspend_smu(sp); 
    
    NV_KMEM_CACHE_FREE_STACK(sp);

    return 0;
}

static int nv_kern_resume(
    struct pci_dev *dev
)
{
    nv_state_t *nv;
    nv_linux_state_t *lnv = NULL;

    if (NV_IS_SMU_DEVICE(dev))
    {
        return nv_kern_smu_resume();
    }

    lnv = pci_get_drvdata(dev);

    if (!lnv || (lnv->dev != dev))
    {
        nv_printf(NV_DBG_WARNINGS, "NVRM: PM: invalid device!\n");
        return -1;
    }

    nv = NV_STATE_PTR(lnv);

    if (NV_IS_GVI_DEVICE(nv))
    {
        return nv_gvi_kern_resume(dev);
    }

    return nv_power_management(dev, PCI_D0, NV_PM_ACPI_RESUME);
}

static int nv_kern_smu_resume(void)
{
    nv_stack_t *sp = NULL;

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return RM_ERR_NO_FREE_MEM;
    }

    rm_resume_smu(sp);
    
    NV_KMEM_CACHE_FREE_STACK(sp);

    return 0;
}

#endif /* defined(NV_PM_SUPPORT_DEVICE_DRIVER_MODEL) */

void* NV_API_CALL nv_get_adapter_state(
    NvU32 domain,
    NvU16 bus,
    NvU16 slot
)
{
    nv_linux_state_t *nvl;

    for (nvl = nv_linux_devices; nvl != NULL;  nvl = nvl->next)
    {
        nv_state_t *nv = NV_STATE_PTR(nvl);
        if (nv->domain == domain && nv->bus == bus && nv->slot == slot)
            return (void *) nv;
    }

    if (bus == 255 && slot == 255)
    {
        nv_state_t *nv = NV_STATE_PTR(&nv_ctl_device);
        return (void *) nv;
    }

    return NULL;
}

void* NV_API_CALL nv_get_smu_state(void)
{
    nv_smu_state_t *nv_smu = &nv_smu_device;

    if (nv_smu->handle == NULL)
    {
        return NULL;
    }

    return nv_smu;
}

RM_STATUS NV_API_CALL nv_log_error(
    nv_state_t *nv,
    NvU32       error_number,
    const char *format,
    va_list    ap
)
{
    RM_STATUS status = RM_OK;
#if defined(CONFIG_CRAY_XT)
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    if (ap != NULL)
    {
        status = nvos_forward_error_to_cray(nvl->dev, error_number,
                format, ap);
    }
#endif
    return status;
}

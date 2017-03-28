/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 1999-2011 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */

#ifndef _NV_PROTO_H_
#define _NV_PROTO_H_

int         nv_acpi_init                (void);
int         nv_acpi_uninit              (void);

int         nv_verify_page_mappings     (nv_pte_t *, NvU32);

#if !defined(NV_IRQ_HANDLER_T_PRESENT) || (NV_IRQ_HANDLER_T_ARGUMENT_COUNT == 3)
irqreturn_t nv_gvi_kern_isr             (int, void *, struct pt_regs *);
#else
irqreturn_t nv_gvi_kern_isr             (int, void *);
#endif

#if defined(KERNEL_2_4) || (NV_INIT_WORK_ARGUMENT_COUNT == 3)
void        nv_gvi_kern_bh              (void *);
#else
void        nv_gvi_kern_bh              (struct work_struct *);
#endif

#if defined(NV_PM_SUPPORT_DEVICE_DRIVER_MODEL)
int         nv_gvi_kern_suspend         (struct pci_dev *, pm_message_t);
int         nv_gvi_kern_resume          (struct pci_dev *);
#endif

int         nv_register_chrdev          (int, struct file_operations *);
void        nv_unregister_chrdev        (int major);

NvU8        nv_find_pci_capability      (struct pci_dev *, NvU8);
void *      nv_alloc_file_private       (void);
void        nv_free_file_private        (nv_file_private_t *);
void *      nv_find_alloc               (nv_linux_state_t *, NvU64, NvU32);

void        nv_check_pci_config_space   (nv_state_t *, BOOL);

int         nv_register_procfs          (void);
void        nv_unregister_procfs        (void);
void        nv_procfs_add_warning       (const char *, const char *);

int         nv_init_pat_support         (nv_stack_t *);
void        nv_teardown_pat_support     (void);
int         nv_enable_pat_support       (void);
void        nv_disable_pat_support      (void);

#if !defined(NV_VMWARE)
int         nv_kern_mmap                (struct file *, struct vm_area_struct *);
int         nv_encode_caching           (pgprot_t *, NvU32, NvU32);
#endif

void        nv_user_map_init            (void);
int         nv_user_map_register        (NvU64, NvU64);
void        nv_user_map_unregister      (NvU64, NvU64);

void *      nv_vmap                     (struct page **, int, pgprot_t);
void        nv_vunmap                   (void *, int);

RM_STATUS   nv_vm_malloc_pages          (nv_state_t *, nv_alloc_t *);
void        nv_vm_unlock_pages          (nv_alloc_t *);
void        nv_vm_free_pages            (nv_state_t *, nv_alloc_t *);

void        nv_vm_list_page_count       (nv_pte_t **, NvU32);

#endif /* _NV_PROTO_H_ */

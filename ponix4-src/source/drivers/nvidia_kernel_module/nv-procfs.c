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

#if defined(CONFIG_PROC_FS)

#include "nv_compiler.h"
#include "nv-reg.h"
#include "patches.h"
#include "rmil.h"

static const char *__README_warning = \
    "The NVIDIA graphics driver tries to detect potential problems\n"
    "with the host system and warns about them using the system's\n"
    "logging mechanisms. Important warning message are also logged\n"
    "to dedicated text files in this directory.\n";

static const char *__README_patches = \
    "The NVIDIA graphics driver's kernel interface files can be\n"
    "patched to improve compatibility with new Linux kernels or to\n"
    "fix bugs in these files. When applied, each official patch\n"
    "provides a short text file with a short description of itself\n"
    "in this directory.\n";

static struct proc_dir_entry *proc_nvidia;
static struct proc_dir_entry *proc_nvidia_warnings;
static struct proc_dir_entry *proc_nvidia_patches;

extern nv_linux_state_t *nv_linux_devices;

extern char *NVreg_RegistryDwords;
extern char *NVreg_RmMsg;

static char nv_registry_keys[NV_MAX_REGISTRY_KEYS_LENGTH];

#if defined(NV_PROC_DIR_ENTRY_HAS_OWNER)
#define NV_SET_PROC_ENTRY_OWNER(entry) ((entry)->owner = THIS_MODULE)
#else
#define NV_SET_PROC_ENTRY_OWNER(entry)
#endif

#define NV_CREATE_PROC_ENTRY(name,mode,parent)            \
   ({                                                     \
        struct proc_dir_entry *__entry;                   \
        __entry = create_proc_entry(name, mode, parent);  \
        if (__entry != NULL)                              \
            NV_SET_PROC_ENTRY_OWNER(__entry);             \
        __entry;                                          \
    })

#define NV_CREATE_PROC_FILE(name,parent,__read_proc,           \
    __write_proc,__fops,__data)                                \
   ({                                                          \
        struct proc_dir_entry *__entry;                        \
        int __mode = (S_IFREG | S_IRUGO);                      \
        if ((__write_proc) != NULL)                            \
            __mode |= S_IWUSR;                                 \
        __entry = NV_CREATE_PROC_ENTRY(name, __mode, parent);  \
        if (__entry != NULL)                                   \
        {                                                      \
            if ((__read_proc) != NULL)                         \
                __entry->read_proc = (__read_proc);            \
            if ((__write_proc) != NULL)                        \
            {                                                  \
                __entry->write_proc = (__write_proc);          \
                __entry->proc_fops = (__fops);                 \
            }                                                  \
            __entry->data = (__data);                          \
        }                                                      \
        __entry;                                               \
    })

#define NV_CREATE_PROC_DIR(name,parent)                        \
   ({                                                          \
        struct proc_dir_entry *__entry;                        \
        int __mode = (S_IFDIR | S_IRUGO | S_IXUGO);            \
        __entry = NV_CREATE_PROC_ENTRY(name, __mode, parent);  \
        __entry;                                               \
    })

#define NV_PROC_WRITE_BUFFER_SIZE   (64 * RM_PAGE_SIZE)

static int
nv_procfs_read_gpu_info(
    char  *page,
    char **start,
    off_t  off,
    int    count,
    int   *eof,
    void  *data
)
{
    nv_state_t *nv = data;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct pci_dev *dev = nvl->dev;
    char *type, *fmt, tmpstr[NV_DEVICE_NAME_LENGTH];
    int len = 0, status;
    NvU32 vbios_rev1, vbios_rev2, vbios_rev3, vbios_rev4, vbios_rev5;
    NvU32 fpga_rev1, fpga_rev2, fpga_rev3;
    nv_stack_t *sp = NULL;

    *eof = 1;

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return 0;
    }

    if (NV_IS_GVI_DEVICE(nv))
    {
        if (rm_gvi_get_device_name(sp, nv, dev->device, NV_DEVICE_NAME_LENGTH,
                                   tmpstr) != RM_OK)
        {
            strcpy (tmpstr, "Unknown");
        }
    }
    else
    {
        if (rm_get_device_name(sp, nv, dev->device, dev->subsystem_vendor,
                    dev->subsystem_device, NV_DEVICE_NAME_LENGTH,
                    tmpstr) != RM_OK)
        {
            strcpy (tmpstr, "Unknown");
        }
    }

    len += sprintf(page+len, "Model: \t\t %s\n", tmpstr);
    len += sprintf(page+len, "IRQ:   \t\t %d\n", nv->interrupt_line);

    if (NV_IS_GVI_DEVICE(nv))
    {
        status = rm_gvi_get_firmware_version(sp, nv, &fpga_rev1, &fpga_rev2,
                                             &fpga_rev3);
        if (status != RM_OK)
            len += sprintf(page+len, "Firmware: \t ????.??.??\n");
        else
        {
            fmt = "Firmware: \t %x.%x.%x\n";
            len += sprintf(page+len, fmt, fpga_rev1, fpga_rev2, fpga_rev3);
        }
    }
    else
    {
        status = rm_get_vbios_version(sp, nv, &vbios_rev1, &vbios_rev2,
                                      &vbios_rev3, &vbios_rev4, &vbios_rev5);

        if (status < 0)
        {
            /* before rm_init_adapter() */
            len += sprintf(page+len, "Video BIOS: \t ??.??.??.??.??\n");
        }
        else
        {
            fmt = "Video BIOS: \t %02x.%02x.%02x.%02x.%02x\n";
            len += sprintf(page+len, fmt, vbios_rev1, vbios_rev2, vbios_rev3,
                                                      vbios_rev4, vbios_rev5);
        }
    }

    if (nv_find_pci_capability(dev, PCI_CAP_ID_AGP))
        type = "AGP";
    else if (nv_find_pci_capability(dev, PCI_CAP_ID_EXP))
        type = "PCI-E";
    else
        type = "PCI";
    len += sprintf(page+len, "Card Type: \t %s\n", type);

    len += sprintf(page+len, "DMA Size: \t %d bits\n",
     nv_count_bits(dev->dma_mask));
    len += sprintf(page+len, "DMA Mask: \t 0x%llx\n", dev->dma_mask);
    len += sprintf(page+len, "Bus Location: \t %04x:%02x.%02x.%x\n",
                   nv->domain, nv->bus, nv->slot, PCI_FUNC(dev->devfn));
#if defined(DEBUG)
    do
    {
        int j;
        for (j = 0; j < NV_GPU_NUM_BARS; j++)
        {
            len += sprintf(page+len, "BAR%u: \t\t 0x%llx (%lluMB)\n",
                           j, nv->bars[j].address, (nv->bars[j].size >> 20));
        }
    } while (0);
#endif

    NV_KMEM_CACHE_FREE_STACK(sp);

    return len;
}

static int
nv_procfs_read_version(
    char  *page,
    char **start,
    off_t  off,
    int    count,
    int   *eof,
    void  *data
)
{
    int len = 0;
    *eof = 1;

    len += sprintf(page+len, "NVRM version: %s\n", pNVRM_ID);
    len += sprintf(page+len, "GCC version:  %s\n", NV_COMPILER);

    return len;
}

static struct pci_dev *nv_get_agp_device_by_class(unsigned int class)
{
    struct pci_dev *dev, *fdev;
    u32 slot, func;

    dev = NV_PCI_GET_CLASS(class << 8, NULL);
    while (dev)
    {
        slot = NV_PCI_SLOT_NUMBER(dev);
        for (func = 0; func < 8; func++)
        {
            fdev = NV_GET_DOMAIN_BUS_AND_SLOT(NV_PCI_DOMAIN_NUMBER(dev),
                                              NV_PCI_BUS_NUMBER(dev),
                                              PCI_DEVFN(slot, func));
            if (!fdev)
                continue;
            if (nv_find_pci_capability(fdev, PCI_CAP_ID_AGP))
            {
                NV_PCI_DEV_PUT(dev);
                return fdev;
            }
            NV_PCI_DEV_PUT(fdev);
        }
        dev = NV_PCI_GET_CLASS(class << 8, dev);
    }

    return NULL;
}

static int
nv_procfs_read_agp_info(
    char  *page,
    char **start,
    off_t  off,
    int    count,
    int   *eof,
    void  *data
)
{
    nv_state_t *nv = data;
    nv_linux_state_t *nvl = NULL;
    struct pci_dev *dev;
    char   *fw, *sba;
    u8     cap_ptr;
    u32    status, command, agp_rate;
    int    len = 0;

    *eof = 1;

    if (nv != NULL)
    {
        nvl = NV_GET_NVL_FROM_NV_STATE(nv);
        dev = nvl->dev;
    }
    else
    {
        dev = nv_get_agp_device_by_class(PCI_CLASS_BRIDGE_HOST);
        if (!dev)
            return 0;

        len += sprintf(page+len, "Host Bridge: \t ");

#if defined(CONFIG_PCI_NAMES)
        len += sprintf(page+len, "%s\n", NV_PCI_DEVICE_NAME(dev));
#else
        len += sprintf(page+len, "PCI device %04x:%04x\n",
                dev->vendor, dev->device);
#endif
    }

    cap_ptr = nv_find_pci_capability(dev, PCI_CAP_ID_AGP);

    pci_read_config_dword(dev, cap_ptr + 4, &status);
    pci_read_config_dword(dev, cap_ptr + 8, &command);

    fw  = (status & 0x00000010) ? "Supported" : "Not Supported";
    sba = (status & 0x00000200) ? "Supported" : "Not Supported";

    len += sprintf(page+len, "Fast Writes: \t %s\n", fw);
    len += sprintf(page+len, "SBA: \t\t %s\n", sba);

    agp_rate = status & 0x7;
    if (status & 0x8)
        agp_rate <<= 2;

    len += sprintf(page+len, "AGP Rates: \t %s%s%s%s\n",
            (agp_rate & 0x00000008) ? "8x " : "",
            (agp_rate & 0x00000004) ? "4x " : "",
            (agp_rate & 0x00000002) ? "2x " : "",
            (agp_rate & 0x00000001) ? "1x " : "");

    len += sprintf(page+len, "Registers: \t 0x%08x:0x%08x\n", status, command);

    if (nvl == NULL)
        NV_PCI_DEV_PUT(dev);

    return len;
}

static int
nv_procfs_read_agp_status(
    char  *page,
    char **start,
    off_t  off,
    int    count,
    int   *eof,
    void  *data
)
{
    nv_state_t *nv = data;
    struct pci_dev *dev;
    char   *fw, *sba, *drv;
    int    len = 0;
    u8     cap_ptr;
    u32    scratch;
    u32    status, command, agp_rate;
    nv_stack_t *sp = NULL;

    *eof = 1;

    dev = nv_get_agp_device_by_class(PCI_CLASS_BRIDGE_HOST);
    if (!dev)
        return 0;
    cap_ptr = nv_find_pci_capability(dev, PCI_CAP_ID_AGP);

    pci_read_config_dword(dev, cap_ptr + 4, &status);
    pci_read_config_dword(dev, cap_ptr + 8, &command);
    NV_PCI_DEV_PUT(dev);

    dev = nv_get_agp_device_by_class(PCI_CLASS_DISPLAY_VGA);
    if (!dev)
        return 0;
    cap_ptr = nv_find_pci_capability(dev, PCI_CAP_ID_AGP);

    pci_read_config_dword(dev, cap_ptr + 4, &scratch);
    status &= scratch;
    pci_read_config_dword(dev, cap_ptr + 8, &scratch);
    command &= scratch;

    if (NV_AGP_ENABLED(nv) && (command & 0x100))
    {
        len += sprintf(page+len, "Status: \t Enabled\n");

        drv = NV_OSAGP_ENABLED(nv) ? "AGPGART" : "NVIDIA";
        len += sprintf(page+len, "Driver: \t %s\n", drv);

        agp_rate = command & 0x7;
        if (status & 0x8)
            agp_rate <<= 2;

        len += sprintf(page+len, "AGP Rate: \t %dx\n", agp_rate);

        fw = (command & 0x00000010) ? "Enabled" : "Disabled";
        len += sprintf(page+len, "Fast Writes: \t %s\n", fw);

        sba = (command & 0x00000200) ? "Enabled" : "Disabled";
        len += sprintf(page+len, "SBA: \t\t %s\n", sba);
    }
    else
    {
        int agp_config = 0;

        NV_KMEM_CACHE_ALLOC_STACK(sp);
        if (sp == NULL)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
            return 0;
        }

        len += sprintf(page+len, "Status: \t Disabled\n\n");

        /*
         * If we find AGP is disabled, but the RM registry indicates it
         * was requested, direct the user to the kernel log (we, or even
         * the kernel may have printed a warning/an error message).
         *
         * Note that the "XNvAGP" registry key reflects the user request
         * and overrides the RM "NvAGP" key, if present.
         */
        rm_read_registry_dword(sp, nv, "NVreg", "NvAGP",  &agp_config);
        rm_read_registry_dword(sp, nv, "NVreg", "XNvAGP", &agp_config);

        if (agp_config != NVOS_AGP_CONFIG_DISABLE_AGP && NV_AGP_FAILED(nv))
        {
            len += sprintf(page+len,
                  "AGP initialization failed, please check the ouput  \n"
                  "of the 'dmesg' command and/or your system log file \n"
                  "for additional information on this problem.        \n");
        }

        NV_KMEM_CACHE_FREE_STACK(sp);
    }

    NV_PCI_DEV_PUT(dev);
    return len;
}

static int
nv_procfs_open_registry(
    struct inode *inode,
    struct file  *file
)
{
    nv_file_private_t *nvfp = NULL;
    nv_stack_t *sp = NULL;

    if (0 == (file->f_mode & FMODE_WRITE))
        return 0;

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

    NV_SET_FILE_PRIVATE(file, nvfp);
    nvfp->sp = sp;

    if (RM_OK != os_alloc_mem((void **)&nvfp->data, NV_PROC_WRITE_BUFFER_SIZE))
    {
        nv_free_file_private(nvfp);
        NV_KMEM_CACHE_FREE_STACK(sp);
        NV_SET_FILE_PRIVATE(file, NULL);
        return -ENOMEM;
    }
    os_mem_set((void *)nvfp->data, 0, NV_PROC_WRITE_BUFFER_SIZE);

    return 0;
}

static int
nv_procfs_close_registry(
    struct inode *inode,
    struct file  *file
)
{
    nv_file_private_t *nvfp;
    nv_state_t *nv;
    nv_linux_state_t *nvl = NULL;
    nv_stack_t *sp = NULL;
    char *key_name, *key_value, *registry_keys;
    size_t key_len, len;
    long count;
    RM_STATUS rm_status;
    int rc = 0;

    nvfp = NV_GET_FILE_PRIVATE(file);
    if (nvfp == NULL)
        return 0;

    sp = nvfp->sp;

    if (0 != nvfp->off)
    {
        nv = nvfp->proc_data;
        if (nv != NULL)
            nvl = NV_GET_NVL_FROM_NV_STATE(nv);
        key_value = (char *)nvfp->data;

        key_name = strsep(&key_value, "=");

        if (NULL == key_name || NULL == key_value)
        {
            rc = -EINVAL;
            goto done;
        }

        key_len = (strlen(key_name) + 1);
        count = (nvfp->off - key_len);

        if (count <= 0)
        {
            rc = -EINVAL;
            goto done;
        }

        rm_status = rm_write_registry_binary(sp, nv, "NVreg", key_name,
                key_value, count);
        if (rm_status != RM_OK)
        {
            rc = -EFAULT;
            goto done;
        }

        registry_keys = ((nvl != NULL) ?
                nvl->registry_keys : nv_registry_keys);
        if (strstr(registry_keys, key_name) != NULL)
            goto done;
        len = strlen(registry_keys);

        if ((len + key_len + 2) <= NV_MAX_REGISTRY_KEYS_LENGTH)
        {
            if (len != 0)
                strcat(registry_keys, ", ");
            strcat(registry_keys, key_name);
        }
    }

done:
    if (NULL != nvfp->data)
        os_free_mem(nvfp->data);

    nv_free_file_private(nvfp);
    NV_SET_FILE_PRIVATE(file, NULL);

    NV_KMEM_CACHE_FREE_STACK(sp);

    return rc;
}

static struct file_operations nv_procfs_registry_fops = {
    .open    = nv_procfs_open_registry,
    .release = nv_procfs_close_registry,
};

static int
nv_procfs_read_params(
    char  *page,
    char **start,
    off_t  off,
    int    count,
    int   *eof,
    void  *data
)
{
    unsigned int i;
    int len = 0;
    nv_parm_t *entry;

    *eof = 1;

    for (i = 0; (entry = &nv_parms[i])->name != NULL; i++)
        len += sprintf(page+len, "%s: %u\n", entry->name, *entry->data);

    len += sprintf(page+len, "RegistryDwords: \"%s\"\n",
                (NVreg_RegistryDwords != NULL) ? NVreg_RegistryDwords : "");
    len += sprintf(page+len, "RmMsg: \"%s\"\n",
                (NVreg_RmMsg != NULL) ? NVreg_RmMsg : "");

    return len;
}

static int
nv_procfs_read_registry(
    char  *page,
    char **start,
    off_t  off,
    int    count,
    int   *eof,
    void  *data
)
{
    nv_state_t *nv = data;
    nv_linux_state_t *nvl = NULL;
    char *registry_keys;

    if (nv != NULL)
        nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    registry_keys = ((nvl != NULL) ?
            nvl->registry_keys : nv_registry_keys);

    *eof = 1;
    return sprintf(page, "Binary: \"%s\"\n", registry_keys);
}

static int
nv_procfs_write_registry(
    struct file   *file,
    const char    *buffer,
    unsigned long  count,
    void          *data
)
{
    nv_file_private_t *nvfp = NV_GET_FILE_PRIVATE(file);
    char *proc_buffer;
    unsigned long bytes_left;

    bytes_left = (NV_PROC_WRITE_BUFFER_SIZE - nvfp->off - 1);

    if (count == 0)
        return -EINVAL;
    else if ((bytes_left == 0) || (count > bytes_left))
        return -ENOSPC;

    proc_buffer = &((char *)nvfp->data)[nvfp->off];

    if (copy_from_user(proc_buffer, buffer, count))
    {
        nv_printf(NV_DBG_ERRORS,
                "NVRM: failed to copyin proc data!\n");
        return -EFAULT;
    }

    nvfp->proc_data = data;
    nvfp->off += count;

    return count;
}

static int
nv_procfs_read_text_file(
    char  *page,
    char **start,
    off_t  off,
    int    count,
    int   *eof,
    void  *data
)
{
    *eof = 1;
    return sprintf(page, "%s", (char *)data);
}

static void
nv_procfs_add_text_file(
    struct proc_dir_entry *parent,
    const char *filename,
    const char *text
)
{
    NV_CREATE_PROC_FILE(filename, parent,
            nv_procfs_read_text_file, NULL, NULL, (void *)text);
}

static void nv_procfs_unregister_all(struct proc_dir_entry *entry)
{
    while (entry)
    {
        struct proc_dir_entry *next = entry->next;
        if (entry->subdir)
            nv_procfs_unregister_all(entry->subdir);
        remove_proc_entry(entry->name, entry->parent);
        if (entry == proc_nvidia)
            break;
        entry = next;
    }
}
#endif

void nv_procfs_add_warning(
    const char *filename,
    const char *text
)
{
#if defined(CONFIG_PROC_FS)
    nv_procfs_add_text_file(proc_nvidia_warnings, filename, text);
#endif
}

int nv_register_procfs(void)
{
#if defined(CONFIG_PROC_FS)
    nv_state_t *nv;
    nv_linux_state_t *nvl;
    NvU32 i = 0;
    char name[6];

    struct proc_dir_entry *entry;
    struct proc_dir_entry *proc_nvidia_agp;
    struct proc_dir_entry *proc_nvidia_gpus, *proc_nvidia_gpu;

    proc_nvidia = NV_CREATE_PROC_DIR("driver/nvidia", NULL);
    if (!proc_nvidia)
        goto failed;

    entry = NV_CREATE_PROC_FILE("params", proc_nvidia,
        nv_procfs_read_params, NULL, NULL, NULL);
    if (!entry)
        goto failed;

    /*
     * entry->proc_fops originally points to a constant
     * structure, so to add more methods for the
     * binary registry write path, we need to replace the
     * said entry->proc_fops with a new fops structure.
     * However, in preparation for this, we need to preserve
     * the procfs read() and write() operations.
     */
    nv_procfs_registry_fops.read = entry->proc_fops->read;
    nv_procfs_registry_fops.write = entry->proc_fops->write;

    entry = NV_CREATE_PROC_FILE("registry", proc_nvidia,
        nv_procfs_read_registry,
        nv_procfs_write_registry,
        &nv_procfs_registry_fops, NULL);
    if (!entry)
        goto failed;

    proc_nvidia_warnings = NV_CREATE_PROC_DIR("warnings", proc_nvidia);
    if (!proc_nvidia_warnings)
        goto failed;
    nv_procfs_add_text_file(proc_nvidia_warnings, "README", __README_warning);

    proc_nvidia_patches = NV_CREATE_PROC_DIR("patches", proc_nvidia);
    if (!proc_nvidia_patches)
        goto failed;

    for (i = 0; __nv_patches[i].short_description; i++)
    {
        nv_procfs_add_text_file(proc_nvidia_patches,
            __nv_patches[i].short_description, __nv_patches[i].description);
    }

    nv_procfs_add_text_file(proc_nvidia_patches, "README", __README_patches);

    entry = NV_CREATE_PROC_FILE("version", proc_nvidia,
        nv_procfs_read_version, NULL, NULL, NULL);
    if (!entry)
        goto failed;

    proc_nvidia_gpus = NV_CREATE_PROC_DIR("gpus", proc_nvidia);
    if (!proc_nvidia_gpus)
        goto failed;

    for (nvl = nv_linux_devices; nvl != NULL;  nvl = nvl->next)
    {
        nv = NV_STATE_PTR(nvl);

        snprintf(name, sizeof(name), "%u", i++);
        proc_nvidia_gpu = NV_CREATE_PROC_DIR(name, proc_nvidia_gpus);
        if (!proc_nvidia_gpu)
            goto failed;

        entry = NV_CREATE_PROC_FILE("information", proc_nvidia_gpu,
            nv_procfs_read_gpu_info, NULL, NULL, nv);
        if (!entry)
            goto failed;

        entry = NV_CREATE_PROC_FILE("registry", proc_nvidia_gpu,
            nv_procfs_read_registry,
            nv_procfs_write_registry,
            &nv_procfs_registry_fops, nv);
        if (!entry)
            goto failed;

        if (nv_find_pci_capability(nvl->dev, PCI_CAP_ID_AGP))
        {
            proc_nvidia_agp = NV_CREATE_PROC_DIR("agp", proc_nvidia);
            if (!proc_nvidia_agp)
                goto failed;

            entry = NV_CREATE_PROC_FILE("status", proc_nvidia_agp,
                nv_procfs_read_agp_status, NULL, NULL, nv);
            if (!entry)
                goto failed;

            entry = NV_CREATE_PROC_FILE("host-bridge", proc_nvidia_agp,
                nv_procfs_read_agp_info, NULL, NULL, NULL);
            if (!entry)
                goto failed;

            entry = NV_CREATE_PROC_FILE("gpu", proc_nvidia_agp,
                nv_procfs_read_agp_info, NULL, NULL, nv);
            if (!entry)
                goto failed;
        }
    }
#endif
    return 0;
#if defined(CONFIG_PROC_FS)
failed:
    nv_procfs_unregister_all(proc_nvidia);
    return -1;
#endif
}

void nv_unregister_procfs(void)
{
#if defined(CONFIG_PROC_FS)
    nv_procfs_unregister_all(proc_nvidia);
#endif
}

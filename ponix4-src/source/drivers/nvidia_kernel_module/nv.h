/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 1999-2001 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */


#ifndef _NV_H_
#define _NV_H_

#include <nvtypes.h>
#include <stdarg.h>

#if !defined(NV_MIN)
#define NV_MIN(_a,_b) ((_a) < (_b) ? (_a) : (_b))
#endif
#if !defined(NV_MAX)
#define NV_MAX(_a,_b) ((_a) > (_b) ? (_a) : (_b))
#endif

/* NVIDIA's reserved major character device number (Linux). */
#define NV_MAJOR_DEVICE_NUMBER 195

/* most cards in a single system */
#define NV_MAX_DEVICES 32

/* NOTE: using an ioctl() number > 55 will overflow! */
#define NV_IOCTL_MAGIC      'F'
#define NV_IOCTL_BASE       200
#define NV_ESC_CARD_INFO         (NV_IOCTL_BASE + 0)
#define NV_ESC_ENV_INFO          (NV_IOCTL_BASE + 2)
#define NV_ESC_ALLOC_OS_EVENT    (NV_IOCTL_BASE + 6)
#define NV_ESC_FREE_OS_EVENT     (NV_IOCTL_BASE + 7)
#define NV_ESC_STATUS_CODE       (NV_IOCTL_BASE + 9)
#define NV_ESC_CHECK_VERSION_STR (NV_IOCTL_BASE + 10)
#define NV_ESC_IOCTL_XFER_CMD    (NV_IOCTL_BASE + 11)

#if defined(NV_SUNOS)
#define NV_PLATFORM_MAX_IOCTL_SIZE  255
#elif (defined(NV_LINUX) || defined(NV_BSD) || defined(NV_VMWARE))
#define NV_PLATFORM_MAX_IOCTL_SIZE  4096
#else
#error "The maximum ioctl() argument size is undefined!"
#endif

/*
 * #define an absolute maximum used as a sanity check for the
 * NV_ESC_IOCTL_XFER_CMD ioctl() size argument.
 */
#define NV_ABSOLUTE_MAX_IOCTL_SIZE  4096

/*
 * Solaris provides no more than 8 bits for the argument size in
 * the ioctl() command encoding; make sure we don't exceed this
 * limit.
 */
#define __NV_IOWR_ASSERT(type) ((sizeof(type) <= NV_PLATFORM_MAX_IOCTL_SIZE) ? 1 : -1)
#define __NV_IOWR(nr, type) ({                                        \
    typedef char __NV_IOWR_TYPE_SIZE_ASSERT[__NV_IOWR_ASSERT(type)];  \
    _IOWR(NV_IOCTL_MAGIC, (nr), type);                                \
})

/*
 * ioctl()'s with parameter structures too large for the
 * _IOC cmd layout use the nv_ioctl_xfer_t structure
 * and the NV_ESC_IOCTL_XFER_CMD ioctl() to pass the actual
 * size and user argument pointer into the RM, which
 * will then copy it to/from kernel space in separate steps.
 */
typedef struct nv_ioctl_xfer
{
    NvU32   cmd;
    NvU32   size;
    NvP64   ptr  NV_ALIGN_BYTES(8);
} nv_ioctl_xfer_t;

typedef struct nv_ioctl_card_info
{
    NvU16    flags;               /* see below                   */
    NvU32    domain;              /* PCI domain number           */
    NvU8     bus;                 /* bus number (PCI, AGP, etc)  */
    NvU8     slot;                /* card slot                   */
    NvU16    vendor_id;           /* PCI vendor id               */
    NvU16    device_id;
    NvU32    gpu_id;
    NvU16    interrupt_line;
    NvU64    reg_address    NV_ALIGN_BYTES(8);
    NvU64    reg_size       NV_ALIGN_BYTES(8);
    NvU64    fb_address     NV_ALIGN_BYTES(8);
    NvU64    fb_size        NV_ALIGN_BYTES(8);
} nv_ioctl_card_info_t;

#define NV_IOCTL_CARD_INFO_BUS_TYPE_PCI            0x0001
#define NV_IOCTL_CARD_INFO_BUS_TYPE_AGP            0x0002
#define NV_IOCTL_CARD_INFO_BUS_TYPE_PCI_EXPRESS    0x0003

#define NV_IOCTL_CARD_INFO_FLAG_PRESENT       0x0001

#define SIM_ENV_GPU       0
#define SIM_ENV_IKOS      1
#define SIM_ENV_CSIM      2

#define NV_SLI_DISABLED   0
#define NV_SLI_ENABLED    1

typedef struct nv_ioctl_env_info
{
    NvU32 pat_supported;
} nv_ioctl_env_info_t;

/* old rm api check
 *
 * this used to be used to verify client/rm interaction both ways by
 * overloading the structure passed into the NV_IOCTL_CARD_INFO ioctl.
 * This interface is deprecated and NV_IOCTL_CHECK_VERSION_STR should
 * be used instead.  We keep the structure and defines here so that RM
 * can recognize and handle old clients.
 */
typedef struct nv_ioctl_rm_api_old_version
{
    NvU32 magic;
    NvU32 major;
    NvU32 minor;
    NvU32 patch;
} nv_ioctl_rm_api_old_version_t;

#define NV_RM_API_OLD_VERSION_MAGIC_REQ              0x0197fade
#define NV_RM_API_OLD_VERSION_MAGIC_REP              0xbead2929
#define NV_RM_API_OLD_VERSION_MAGIC_LAX_REQ         (NV_RM_API_OLD_VERSION_MAGIC_REQ ^ '1')
#define NV_RM_API_OLD_VERSION_MAGIC_OVERRIDE_REQ    (NV_RM_API_OLD_VERSION_MAGIC_REQ ^ '2')
#define NV_RM_API_OLD_VERSION_MAGIC_IGNORE           0xffffffff

/* alloc event */
typedef struct nv_ioctl_alloc_os_event
{
    NvU32 hClient;
    NvU32 hDevice;
    NvU32 hOsEvent;
    NvU32 fd;
    NvU32 Status;
} nv_ioctl_alloc_os_event_t;

/* free event */
typedef struct nv_ioctl_free_os_event
{
    NvU32 hClient;
    NvU32 hDevice;
    NvU32 fd;
    NvU32 Status;
} nv_ioctl_free_os_event_t;

/* status code */
typedef struct nv_ioctl_status_code
{
    NvU32 domain;
    NvU32 bus;
    NvU32 slot;
    NvU32 status;
} nv_ioctl_status_code_t;

/* check version string */
#define NV_RM_API_VERSION_STRING_LENGTH 64

typedef struct nv_ioctl_rm_api_version
{
    NvU32 cmd;
    NvU32 reply;
    char versionString[NV_RM_API_VERSION_STRING_LENGTH];
} nv_ioctl_rm_api_version_t;

#define NV_RM_API_VERSION_CMD_STRICT         0
#define NV_RM_API_VERSION_CMD_RELAXED       '1'
#define NV_RM_API_VERSION_CMD_OVERRIDE      '2'

#define NV_RM_API_VERSION_REPLY_UNRECOGNIZED 0
#define NV_RM_API_VERSION_REPLY_RECOGNIZED   1

#ifdef NVRM

extern const char *pNVRM_ID;

/*
 * ptr arithmetic convenience
 */

typedef union
{
    volatile NvV8 Reg008[1];
    volatile NvV16 Reg016[1];
    volatile NvV32 Reg032[1];
} nv_hwreg_t, * nv_phwreg_t;


#define NVRM_PCICFG_NUM_BARS            6
#define NVRM_PCICFG_BAR_OFFSET(i)       (0x10 + (i) * 4)
#define NVRM_PCICFG_BAR_REQTYPE_MASK    0x00000001
#define NVRM_PCICFG_BAR_REQTYPE_MEMORY  0x00000000
#define NVRM_PCICFG_BAR_MEMTYPE_MASK    0x00000006
#define NVRM_PCICFG_BAR_MEMTYPE_64BIT   0x00000004
#define NVRM_PCICFG_BAR_ADDR_MASK       0xfffffff0

#define NV_GPU_NUM_BARS                 3
#define NV_GPU_BAR_INDEX_REGS           0
#define NV_GPU_BAR_INDEX_FB             1
#define NV_GPU_BAR_INDEX_IMEM           2

typedef struct
{
    NvU64 address;
    NvU64 size;
    NvU32 offset;
    NvU32 *map;
    nv_phwreg_t map_u;
} nv_aperture_t;


typedef struct
{
    char *node;
    char *name;
    NvU32 *data;
} nv_parm_t;

#if defined(NVCPU_X86_64)
#define NV_STACK_SIZE (RM_PAGE_SIZE * 3)
#else
#define NV_STACK_SIZE (RM_PAGE_SIZE * 2)
#endif

typedef struct nv_stack_s
{
    NvU32 size;
    void *top;
    NvU8  stack[NV_STACK_SIZE-16] __attribute__ ((aligned(16)));
} nv_stack_t;

/*
 * this is a wrapper for unix events
 * unlike the events that will be returned to clients, this includes
 * kernel-specific data, such as file pointer, etc..
 */
typedef struct nv_event_s
{
    NvU32 hParent;
    NvU32 hObject;
    NvU32 index;
    void  *file;  /* per file-descriptor data pointer */
    NvU32 handle;
    NvU32 fd;
    struct nv_event_s *next;
} nv_event_t;

typedef struct nv_kern_mapping_s
{
    void  *addr;
    NvU64 size;
    struct nv_kern_mapping_s *next;
} nv_kern_mapping_t;

/*
 * per device state
 */

typedef struct
{
    void  *priv;                    /* private data */
    void  *os_state;                /* os-specific device state */

    int    rmInitialized;
    int    flags;

    /* PCI config info */
    NvU32 domain;
    NvU16 bus;
    NvU16 slot;
    NvU16 vendor_id;
    NvU16 device_id;
    NvU16 subsystem_id;
    NvU32 gpu_id;
    void *handle;

    NvU32 pci_cfg_space[16];

    /* physical characteristics */
    nv_aperture_t bars[NV_GPU_NUM_BARS];
    nv_aperture_t *regs;
    nv_aperture_t *fb, ud;
    nv_aperture_t agp;

    NvU32  interrupt_line;

    NvU32 agp_config;
    NvU32 agp_status;

    NvU32 primary_vga;

    NvU32 sim_env;

    NvU32 rc_timer_enabled;

    /* list of events allocated for this device */
    nv_event_t *event_list;

    nv_kern_mapping_t *kern_mappings;

} nv_state_t;

typedef struct
{
    /* PCI config info */
    NvU32 domain;
    NvU16 bus;
    NvU16 slot;
    NvU16 vendor_id;
    NvU16 device_id;
    void *handle;
    nv_aperture_t bar0;
    nv_aperture_t *regs;
} nv_smu_state_t;

/*
 * flags
 */

#define NV_FLAG_OPEN                   0x0001
#define NV_FLAG_WAS_POSTED             0x0002
#define NV_FLAG_CONTROL                0x0004
#define NV_FLAG_MAP_REGS_EARLY         0x0008
#define NV_FLAG_USE_BAR0_CFG           0x0010
#define NV_FLAG_USES_MSI               0x0020
#define NV_FLAG_S4_AVAILABLE           0x0040
#define NV_FLAG_PASSTHRU               0x0080
#define NV_FLAG_GVI_IN_SUSPEND         0x0100
#define NV_FLAG_GVI                    0x0200
#define NV_FLAG_GVI_INTR_EN            0x0400
#define NV_FLAG_PERSISTENT_SW_STATE    0x0800
#define NV_FLAG_IN_RECOVERY            0x1000

#define NV_PM_APM_SUSPEND       0x0001
#define NV_PM_APM_RESUME        0x0002
#define NV_PM_ACPI_HIBERNATE    0x0003
#define NV_PM_ACPI_STANDBY      0x0004
#define NV_PM_ACPI_RESUME       0x0005

#define NV_AGP_STATUS_DISABLED  0x0000
#define NV_AGP_STATUS_ENABLED   0x0001
#define NV_AGP_STATUS_FAILED    0x0002

#define NV_AGP_ENABLED(nv)      ((nv)->agp_status & NV_AGP_STATUS_ENABLED)
#define NV_AGP_FAILED(nv)       ((nv)->agp_status & NV_AGP_STATUS_FAILED)

#define NV_NVAGP_ENABLED(nv)    ((nv)->agp_config & NVOS_AGP_CONFIG_NVAGP)
#define NV_OSAGP_ENABLED(nv)    ((nv)->agp_config & NVOS_AGP_CONFIG_OSAGP)

#define NV_PRIMARY_VGA(nv)      ((nv)->primary_vga)

#define NV_IS_GVI_DEVICE(nv) ((nv)->flags & NV_FLAG_GVI)

/*                                                                                    
 * The ACPI specification defines IDs for various ACPI video                          
 * extension events like display switch events, AC/battery                            
 * events, docking events, etc..                                                      
 * Whenever an ACPI event is received by the corresponding                            
 * event handler installed within the core NVIDIA driver, the                         
 * code can verify the event ID before processing it.                                 
 */
#define ACPI_DISPLAY_DEVICE_CHANGE_EVENT      0x80 
#define NVIF_NOTIFY_DISPLAY_DETECT           0xCB
#define NVIF_DISPLAY_DEVICE_CHANGE_EVENT     NVIF_NOTIFY_DISPLAY_DETECT 
/*                                                                                    
 * NVIDIA ACPI event IDs to be passed into the core NVIDIA                            
 * driver for various events like display switch events,                              
 * AC/battery events, docking events, etc..                                           
 */                                                                                   
#define NV_SYSTEM_ACPI_DISPLAY_SWITCH_EVENT  0x8001                                   
#define NV_SYSTEM_ACPI_BATTERY_POWER_EVENT   0x8002                                   
#define NV_SYSTEM_ACPI_DOCK_EVENT            0x8003                                   

/*
 * Status bit definitions for display switch hotkey events.
 */
#define NV_HOTKEY_STATUS_DISPLAY_ENABLE_LCD 0x01
#define NV_HOTKEY_STATUS_DISPLAY_ENABLE_CRT 0x02
#define NV_HOTKEY_STATUS_DISPLAY_ENABLE_TV  0x04
#define NV_HOTKEY_STATUS_DISPLAY_ENABLE_DFP 0x08

/*                                                                                    
 * NVIDIA ACPI sub-event IDs (event types) to be passed into                          
 * to core NVIDIA driver for ACPI events.                                             
 */                                                                                   
#define NV_SYSTEM_ACPI_EVENT_VALUE_DISPLAY_SWITCH_DEFAULT    0                        
#define NV_SYSTEM_ACPI_EVENT_VALUE_POWER_EVENT_AC            0                        
#define NV_SYSTEM_ACPI_EVENT_VALUE_POWER_EVENT_BATTERY       1                        
#define NV_SYSTEM_ACPI_EVENT_VALUE_DOCK_EVENT_UNDOCKED       0                        
#define NV_SYSTEM_ACPI_EVENT_VALUE_DOCK_EVENT_DOCKED         1                        

#define NV_ACPI_NVIF_HANDLE_PRESENT 0x01
#define NV_ACPI_DSM_HANDLE_PRESENT  0x02
#define NV_ACPI_WMMX_HANDLE_PRESENT 0x04
#define NV_ACPI_MXMI_HANDLE_PRESENT 0x08
#define NV_ACPI_MXMS_HANDLE_PRESENT 0x10

#define NV_ACPI_METHOD_NVIF     0x01
#define NV_ACPI_METHOD_WMMX     0x02
#define NV_ACPI_METHOD_MXMI     0x03
#define NV_ACPI_METHOD_MXMS     0x04

/*
** where we hide our nv_state_t * ...
*/
#define NV_SET_NV_STATE(pgpu,p) ((pgpu)->pOsHwInfo = (p))
#define NV_GET_NV_STATE(pGpu) \
    (nv_state_t *)((pGpu) ? (pGpu)->pOsHwInfo : NULL)

/* mmap(2) offsets */

#define IS_REG_OFFSET(nv, offset, length)                                      \
             (((offset) >= (nv)->regs->address) &&                             \
             (((offset) + ((length)-1)) <= (nv)->regs->address + ((nv)->regs->size-1)))

#define IS_FB_OFFSET(nv, offset, length)                                       \
             (((offset) >= (nv)->fb->address) &&                               \
             (((offset) + ((length)-1)) <= (nv)->fb->address + ((nv)->fb->size-1)))

#define IS_UD_OFFSET(nv, offset, length)                                       \
             (((nv)->ud.address != 0) && ((nv)->ud.size != 0) &&               \
             ((offset) >= (nv)->ud.address) &&                                 \
             (((offset) + ((length)-1)) <= (nv)->ud.address + ((nv)->ud.size-1)))

#define IS_AGP_OFFSET(nv, offset, length)                                      \
             ((NV_AGP_ENABLED(nv)) &&                                          \
             ((offset) >= (nv)->agp.address) &&                                \
             (((offset) + ((length)-1)) <= (nv)->agp.address + ((nv)->agp.size-1)))

/* duplicated from nvos.h for external builds */
#ifndef NVOS_AGP_CONFIG_DISABLE_AGP
#  define NVOS_AGP_CONFIG_DISABLE_AGP (0x00000000)
#endif
#ifndef NVOS_AGP_CONFIG_NVAGP
#  define NVOS_AGP_CONFIG_NVAGP       (0x00000001)
#endif
#ifndef NVOS_AGP_CONFIG_OSAGP
#  define NVOS_AGP_CONFIG_OSAGP       (0x00000002)
#endif
#ifndef NVOS_AGP_CONFIG_ANYAGP
#  define NVOS_AGP_CONFIG_ANYAGP      (0x00000003)
#endif


/* device name length; must be atleast 8 */

#define NV_DEVICE_NAME_LENGTH 40

#define NV_MAX_ISR_DELAY_US           20000
#define NV_MAX_ISR_DELAY_MS           (NV_MAX_ISR_DELAY_US / 1000)

#define NV_TIMERCMP(a, b, CMP)                                              \
    (((a)->tv_sec == (b)->tv_sec) ?                                         \
        ((a)->tv_usec CMP (b)->tv_usec) : ((a)->tv_sec CMP (b)->tv_sec))

#define NV_TIMERADD(a, b, result)                                           \
    {                                                                       \
        (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                       \
        (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                    \
        if ((result)->tv_usec >= 1000000)                                   \
        {                                                                   \
            ++(result)->tv_sec;                                             \
            (result)->tv_usec -= 1000000;                                   \
        }                                                                   \
    }

#define NV_TIMERSUB(a, b, result)                                           \
    {                                                                       \
        (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                       \
        (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                    \
        if ((result)->tv_usec < 0)                                          \
        {                                                                   \
          --(result)->tv_sec;                                               \
          (result)->tv_usec += 1000000;                                     \
        }                                                                   \
    }

#define NV_ALIGN_DOWN(v,g) ((v) & ~((g) - 1))
#define NV_ALIGN_UP(v,g) (((v) + ((g) - 1)) & ~((g) - 1))

/*
 * driver internal interfaces
 */

#ifndef NVWATCH

/*
 * ---------------------------------------------------------------------------
 *
 * Function prototypes for UNIX specific OS interface.
 *
 * ---------------------------------------------------------------------------
 */

/*
 * Make sure that arguments to and from the core resource manager
 * are passed and expected on the stack. define duplicated in os-interface.h
 */
#if !defined(NV_API_CALL)
#if defined(NVCPU_X86)
#if defined(__use_altstack__)
#define NV_API_CALL __attribute__((regparm(0),altstack(false)))
#else
#define NV_API_CALL __attribute__((regparm(0)))
#endif
#elif defined(NVCPU_X86_64) && defined(__use_altstack__)
#define NV_API_CALL __attribute__((altstack(false)))
#else
#define NV_API_CALL
#endif
#endif /* !defined(NV_API_CALL) */


void*  NV_API_CALL  nv_alloc_kernel_mapping      (nv_state_t *, NvU64, NvU64, void **);
RM_STATUS  NV_API_CALL  nv_free_kernel_mapping   (nv_state_t *, void *, void *, NvBool);

RM_STATUS  NV_API_CALL  nv_alloc_user_mapping    (nv_state_t *, NvU64, NvU64, NvU32, NvU64 *, void **);
RM_STATUS  NV_API_CALL  nv_free_user_mapping     (nv_state_t *, NvU64, void *);

NvU64  NV_API_CALL  nv_get_kern_phys_address     (NvU64);
NvU64  NV_API_CALL  nv_get_user_phys_address     (NvU64);
void*  NV_API_CALL  nv_get_adapter_state         (NvU32, NvU16, NvU16);

void   NV_API_CALL  nv_set_dma_address_size      (nv_state_t *, NvU32 );

RM_STATUS  NV_API_CALL  nv_alias_pages           (nv_state_t *, NvU32, NvU32, NvU32, NvU64, NvU64 *, void **);
RM_STATUS  NV_API_CALL  nv_alloc_pages           (nv_state_t *, NvU32, NvBool, NvBool, NvU32, NvBool, NvU64 *, void **);
RM_STATUS  NV_API_CALL  nv_free_pages            (nv_state_t *, NvU32, NvU32, NvU32, NvU32, void *);
RM_STATUS  NV_API_CALL  nv_guest_pfn_list        (nv_state_t *, unsigned int, unsigned int, unsigned int, unsigned int*);

RM_STATUS  NV_API_CALL  nv_agp_init              (nv_state_t *, NvU64 *, NvU64 *, NvU32);
RM_STATUS  NV_API_CALL  nv_agp_teardown          (nv_state_t *);

NvS32  NV_API_CALL  nv_start_rc_timer            (nv_state_t *);
NvS32  NV_API_CALL  nv_stop_rc_timer             (nv_state_t *);

void   NV_API_CALL  nv_post_event                (nv_state_t *, nv_event_t *, NvU32, NvU32);
NvS32  NV_API_CALL  nv_get_event                 (nv_state_t *, void *, nv_event_t *, NvU32 *);

NvS32  NV_API_CALL  nv_no_incoherent_mappings    (void);

void   NV_API_CALL  nv_verify_pci_config         (nv_state_t *, BOOL);

void*  NV_API_CALL  nv_i2c_add_adapter           (nv_state_t *, NvU32);
BOOL   NV_API_CALL  nv_i2c_del_adapter           (nv_state_t *, void *);

void   NV_API_CALL  nv_acpi_methods_init         (NvU32 *);
void   NV_API_CALL  nv_acpi_methods_uninit       (void);

RM_STATUS  NV_API_CALL  nv_acpi_method           (NvU32, NvU32, NvU32, void *, NvU16, NvU32 *, void *, NvU16 *);
RM_STATUS  NV_API_CALL  nv_acpi_dsm_method       (nv_state_t *, NvU8 *, NvU32, NvU32, void *, NvU16, NvU32 *, void *, NvU16 *);
RM_STATUS  NV_API_CALL  nv_acpi_ddc_method       (nv_state_t *, void *, NvU32 *);
void*  NV_API_CALL  nv_get_smu_state             (void);
RM_STATUS  NV_API_CALL  nv_lock_user_pages       (nv_state_t *, void *, NvU64, NvU64 *, void **);
RM_STATUS  NV_API_CALL  nv_unlock_user_pages     (nv_state_t *, NvU64, NvU64 *, void *);
RM_STATUS  NV_API_CALL  nv_acpi_rom_method       (nv_state_t *, NvU32 *, NvU32 *);
RM_STATUS  NV_API_CALL  nv_log_error             (nv_state_t *, NvU32, const char *, va_list);

/*
 * ---------------------------------------------------------------------------
 *
 * Function prototypes for Resource Manager interface.
 *
 * ---------------------------------------------------------------------------
 */

BOOL       NV_API_CALL  rm_init_rm               (nv_stack_t *);
BOOL       NV_API_CALL  rm_shutdown_rm           (nv_stack_t *);
BOOL       NV_API_CALL  rm_init_private_state    (nv_stack_t *, nv_state_t *);
BOOL       NV_API_CALL  rm_free_private_state    (nv_stack_t *, nv_state_t *);
BOOL       NV_API_CALL  rm_init_adapter          (nv_stack_t *, nv_state_t *);
BOOL       NV_API_CALL  rm_disable_adapter       (nv_stack_t *, nv_state_t *);
BOOL       NV_API_CALL  rm_shutdown_adapter      (nv_stack_t *, nv_state_t *);
void       NV_API_CALL  rm_set_interrupts        (nv_stack_t *, BOOL);
BOOL       NV_API_CALL  rm_ioctl                 (nv_stack_t *, nv_state_t *, void *, NvU32, void *);
BOOL       NV_API_CALL  rm_isr                   (nv_stack_t *, nv_state_t *, NvU32 *);
void       NV_API_CALL  rm_isr_bh                (nv_stack_t *, nv_state_t *);
RM_STATUS  NV_API_CALL  rm_power_management      (nv_stack_t *, nv_state_t *, NvU32, NvU32);
RM_STATUS  NV_API_CALL  rm_save_low_res_mode     (nv_stack_t *, nv_state_t *);
NvU32      NV_API_CALL  rm_get_vbios_version     (nv_stack_t *, nv_state_t *, NvU32 *, NvU32 *, NvU32 *, NvU32 *, NvU32 *);
void       NV_API_CALL  rm_free_unused_clients   (nv_stack_t *, nv_state_t *, void *);

void       NV_API_CALL  rm_update_agp_config     (nv_stack_t *, nv_state_t *);
RM_STATUS  NV_API_CALL  rm_init_agp              (nv_stack_t *, nv_state_t *, NvU64 *, NvU64 *);
RM_STATUS  NV_API_CALL  rm_teardown_agp          (nv_stack_t *, nv_state_t *);

RM_STATUS  NV_API_CALL  rm_alloc_agp_pages       (nv_stack_t *, nv_state_t *, NvU32, void **, NvU32 *);
RM_STATUS  NV_API_CALL  rm_free_agp_pages        (nv_stack_t *, nv_state_t *, void *);

RM_STATUS  NV_API_CALL  rm_alloc_agp_bitmap      (nv_stack_t *, nv_state_t *, NvU32, NvU32 *);
RM_STATUS  NV_API_CALL  rm_free_agp_bitmap       (nv_stack_t *, nv_state_t *, NvU32, NvU32);
RM_STATUS  NV_API_CALL  rm_set_agp_bitmap        (nv_stack_t *, nv_state_t *, void *);
RM_STATUS  NV_API_CALL  rm_clear_agp_bitmap      (nv_stack_t *, nv_state_t *, void **);

RM_STATUS  NV_API_CALL  rm_read_registry_dword   (nv_stack_t *, nv_state_t *, NvU8 *, NvU8 *, NvU32 *);
RM_STATUS  NV_API_CALL  rm_write_registry_dword  (nv_stack_t *, nv_state_t *, NvU8 *, NvU8 *, NvU32);
RM_STATUS  NV_API_CALL  rm_read_registry_binary  (nv_stack_t *, nv_state_t *, NvU8 *, NvU8 *, NvU8 *, NvU32 *);
RM_STATUS  NV_API_CALL  rm_write_registry_binary (nv_stack_t *, nv_state_t *, NvU8 *, NvU8 *, NvU8 *, NvU32);
RM_STATUS  NV_API_CALL  rm_write_registry_string (nv_stack_t *, nv_state_t *, NvU8 *, NvU8 *, const char *, NvU32);

RM_STATUS  NV_API_CALL  rm_run_rc_callback       (nv_stack_t *, nv_state_t *);
void       NV_API_CALL  rm_execute_work_item     (nv_stack_t *, void *);
RM_STATUS  NV_API_CALL  rm_get_device_name       (nv_stack_t *, nv_state_t *, NvU16, NvU16, NvU16, NvU32, NvU8 *);

NvU64      NV_API_CALL  nv_rdtsc                 (void);

void       NV_API_CALL  rm_register_compatible_ioctls   (nv_stack_t *);
void       NV_API_CALL  rm_unregister_compatible_ioctls (nv_stack_t *);

BOOL       NV_API_CALL  rm_is_legacy_device      (nv_stack_t *, NvU16, BOOL);
BOOL       NV_API_CALL  rm_is_supported_device   (nv_stack_t *, nv_state_t *);

RM_STATUS  NV_API_CALL  rm_i2c_read_buffer        (nv_stack_t *, nv_state_t *, void *, NvU8, NvU32, NvU8 *);
RM_STATUS  NV_API_CALL  rm_i2c_write_buffer       (nv_stack_t *, nv_state_t *, void *, NvU8, NvU32, NvU8 *);
RM_STATUS  NV_API_CALL  rm_i2c_smbus_write_quick  (nv_stack_t *, nv_state_t *, void *, NvU8, NvU8);
RM_STATUS  NV_API_CALL  rm_i2c_smbus_read_buffer  (nv_stack_t *, nv_state_t *, void *, NvU8, NvU8, NvU32, NvU8 *);
RM_STATUS  NV_API_CALL  rm_i2c_smbus_write_buffer (nv_stack_t *, nv_state_t *, void *, NvU8, NvU8, NvU32, NvU8 *);

void       NV_API_CALL  rm_check_pci_config_space (nv_stack_t *, nv_state_t *nv, BOOL, BOOL, BOOL);

RM_STATUS  NV_API_CALL  rm_i2c_remove_adapters    (nv_stack_t *, nv_state_t *);

RM_STATUS  NV_API_CALL  rm_perform_version_check  (nv_stack_t *, nv_ioctl_rm_api_version_t *);

RM_STATUS  NV_API_CALL  rm_system_event           (nv_stack_t *, NvU32, NvU32);
RM_STATUS  NV_API_CALL  rm_init_smu               (nv_stack_t *, nv_smu_state_t *);
RM_STATUS  NV_API_CALL  rm_shutdown_smu           (nv_stack_t *, nv_smu_state_t *);
RM_STATUS  NV_API_CALL  rm_suspend_smu            (nv_stack_t *);
RM_STATUS  NV_API_CALL  rm_resume_smu             (nv_stack_t *);

void       NV_API_CALL  rm_disable_gpu_state_persistence    (nv_stack_t *sp, nv_state_t *);

RM_STATUS  NV_API_CALL  rm_p2p_init_mapping       (nv_stack_t *, NvU64, NvU64 *, NvU64 *, NvU64 *, NvU64 *, NvU64, NvU64, NvU64, NvU64, void (*)(void *), void *);
RM_STATUS  NV_API_CALL  rm_p2p_destroy_mapping    (nv_stack_t *, NvU64);
RM_STATUS  NV_API_CALL  rm_p2p_get_pages          (nv_stack_t *, NvU64, NvU32, NvU64, NvU64, NvU64 **, NvU32 **, NvU32 **, NvU32 *, void *, void (*)(void *), void *);
RM_STATUS  NV_API_CALL  rm_p2p_put_pages          (nv_stack_t *, NvU64, NvU32, NvU64, void *);

#define rm_disable_interrupts(sp) rm_set_interrupts((sp),FALSE)
#define rm_enable_interrupts(sp)  rm_set_interrupts((sp),TRUE)

#endif /* NVWATCH */

#endif /* NVRM */

static inline int nv_count_bits(NvU64 word)
{
    NvU64 bits;

    bits = (word & 0x5555555555555555ULL) + ((word >>  1) & 0x5555555555555555ULL);
    bits = (bits & 0x3333333333333333ULL) + ((bits >>  2) & 0x3333333333333333ULL);
    bits = (bits & 0x0f0f0f0f0f0f0f0fULL) + ((bits >>  4) & 0x0f0f0f0f0f0f0f0fULL);
    bits = (bits & 0x00ff00ff00ff00ffULL) + ((bits >>  8) & 0x00ff00ff00ff00ffULL);
    bits = (bits & 0x0000ffff0000ffffULL) + ((bits >> 16) & 0x0000ffff0000ffffULL);
    bits = (bits & 0x00000000ffffffffULL) + ((bits >> 32) & 0x00000000ffffffffULL);

    return (int)(bits);
}

#endif

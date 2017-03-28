/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 1999-2001 by NVIDIA Corporation.  All rights reserved.  All
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
#include "nv-reg.h"

#if defined(NV_LINUX_ACPI_EVENTS_SUPPORTED)

static int         nv_acpi_add             (struct acpi_device *);
static int         nv_acpi_remove          (struct acpi_device *, int);
static void        nv_acpi_event           (acpi_handle, u32, void *);
static acpi_status nv_acpi_find_methods    (acpi_handle, u32, void *, void **);
static RM_STATUS   nv_acpi_nvif_method     (NvU32, NvU32, void *, NvU16, NvU32 *, void *, NvU16 *);

static RM_STATUS   nv_acpi_wmmx_method     (NvU32, NvU8 *, NvU16 *);
static RM_STATUS   nv_acpi_mxmi_method     (NvU8 *, NvU16 *);
static RM_STATUS   nv_acpi_mxms_method     (NvU8 *, NvU16 *);

#if defined(NV_ACPI_DEVICE_OPS_HAS_MATCH)
static int         nv_acpi_match           (struct acpi_device *, struct acpi_driver *);
#endif

#if defined(ACPI_VIDEO_HID) && defined(NV_ACPI_DEVICE_ID_HAS_DRIVER_DATA) 
static const struct acpi_device_id nv_video_device_ids[] = {
    { 
        .id          = ACPI_VIDEO_HID, 
        .driver_data = 0, 
    },
    { 
        .id          = "",
        .driver_data = 0, 
    },
};
#endif

static struct acpi_driver *nv_acpi_driver;
static acpi_handle nvif_handle = NULL;
static acpi_handle nvif_parent_gpu_handle  = NULL;
static acpi_handle wmmx_handle = NULL;
static acpi_handle mxmi_handle = NULL;
static acpi_handle mxms_handle = NULL;

static const struct acpi_driver nv_acpi_driver_template = {
    .name = "NVIDIA ACPI Video Driver",
    .class = "video",
#if defined(ACPI_VIDEO_HID)
#if defined(NV_ACPI_DEVICE_ID_HAS_DRIVER_DATA)
    .ids = nv_video_device_ids,
#else
    .ids = ACPI_VIDEO_HID,
#endif
#endif
    .ops = {
        .add = nv_acpi_add,
        .remove = nv_acpi_remove,
#if defined(NV_ACPI_DEVICE_OPS_HAS_MATCH)
        .match = nv_acpi_match,
#endif
    },
};

int nv_acpi_init(void)
{
    /*
     * This function will register the RM with the Linux
     * ACPI subsystem.
     */
    int status;
    nv_stack_t *sp = NULL;
    NvU32 acpi_event_config = 0;
    RM_STATUS rmStatus;
    
    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return -ENOMEM;
    }

    rmStatus = rm_read_registry_dword(sp, NULL, "NVreg",
                   NV_REG_REGISTER_FOR_ACPI_EVENTS, &acpi_event_config);
    NV_KMEM_CACHE_FREE_STACK(sp);
  
    if ((rmStatus == RM_OK) && (acpi_event_config == 0))
        return 0;

    if (nv_acpi_driver != NULL)
        return -EBUSY;

    rmStatus = os_alloc_mem((void **)&nv_acpi_driver,
            sizeof(struct acpi_driver));
    if (rmStatus != RM_OK)
        return -ENOMEM;

    memcpy((void *)nv_acpi_driver, (void *)&nv_acpi_driver_template,
            sizeof(struct acpi_driver));

    status = acpi_bus_register_driver(nv_acpi_driver);
    if (status < 0)
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_init: acpi_bus_register_driver() failed (%d)!\n", status);
        os_free_mem(nv_acpi_driver);
        nv_acpi_driver = NULL;
    }

    return status;
}

int nv_acpi_uninit(void)
{
    nv_stack_t *sp = NULL;
    NvU32 acpi_event_config = 0;
    RM_STATUS rmStatus;
    
    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return -ENOMEM;
    }

    rmStatus = rm_read_registry_dword(sp, NULL, "NVreg",
                   NV_REG_REGISTER_FOR_ACPI_EVENTS, &acpi_event_config);
    NV_KMEM_CACHE_FREE_STACK(sp);
  
    if ((rmStatus == RM_OK) && (acpi_event_config == 0))
        return 0;

    if (nv_acpi_driver == NULL)
        return -ENXIO;

    acpi_bus_unregister_driver(nv_acpi_driver);
    os_free_mem(nv_acpi_driver);

    nv_acpi_driver = NULL;

    return 0;
}

static int nv_acpi_add(struct acpi_device *device)
{
    /*
     * This function will cause RM to initialize the things it needs for acpi interaction
     * on the display device.
     */
    int status = -1;
    RM_STATUS rmStatus = RM_ERROR;
    nv_acpi_t *pNvAcpiObject = NULL;
    union acpi_object control_argument_0 = { ACPI_TYPE_INTEGER };
    struct acpi_object_list control_argument_list = { 0, NULL };
    nv_stack_t *sp = NULL;
    struct list_head *node, *next;
    nv_acpi_integer_t device_id = 0;
    int device_counter = 0;

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return -ENOMEM;
    }

    // allocate data structure we need
    rmStatus = os_alloc_mem((void **) &pNvAcpiObject, sizeof(nv_acpi_t));
    if (rmStatus != RM_OK)
    {
        NV_KMEM_CACHE_FREE_STACK(sp);
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_add: failed to allocate ACPI device management data!\n");
        return -ENOMEM;
    }

    os_mem_set((void *)pNvAcpiObject, 0, sizeof(nv_acpi_t));

    device->driver_data = pNvAcpiObject;
    pNvAcpiObject->device = device;

    pNvAcpiObject->sp = sp;

    // grab handles to all the important nodes representing devices

    list_for_each_safe(node, next, &device->children) 
    {
        struct acpi_device *dev =
            list_entry(node, struct acpi_device, node);

        if (!dev)
            continue;

        if (device_counter == NV_MAXNUM_DISPLAY_DEVICES)
        {
            nv_printf(NV_DBG_ERRORS, 
                      "NVRM: nv_acpi_add: Total number of devices cannot exceed %d\n", 
                      NV_MAXNUM_DISPLAY_DEVICES);
            break;
        }

        status =
            acpi_evaluate_integer(dev->handle, "_ADR", NULL, &device_id);
        if (ACPI_FAILURE(status))
            /* Couldnt query device_id for this device */
            continue;

        device_id = (device_id & 0xffff);

        if ((device_id != 0x100) && /* Not a known CRT device-id */ 
            (device_id != 0x200) && /* Not a known TV device-id */ 
            (device_id != 0x0110) && (device_id != 0x0118) && (device_id != 0x0400) && /* Not an LCD*/
            (device_id != 0x0111) && (device_id != 0x0120) && (device_id != 0x0300)) /* Not a known DVI device-id */ 
        {
            /* This isnt a known device Id. 
               Do default switching on this system. */
            pNvAcpiObject->default_display_mask = 1;
            break;
        }

        pNvAcpiObject->pNvVideo[device_counter].dev_id = device_id;
        pNvAcpiObject->pNvVideo[device_counter].dev_handle = dev->handle;
    
        device_counter++;

    }

    // arg 0, bits 1:0, 0 = enable events
    control_argument_0.integer.type = ACPI_TYPE_INTEGER;
    control_argument_0.integer.value = 0x0;

    // listify it
    control_argument_list.count = 1;
    control_argument_list.pointer = &control_argument_0;

    // _DOS method takes 1 argument and returns nothing
    status = acpi_evaluate_object(device->handle, "_DOS", &control_argument_list, NULL);

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_add: failed to enable display switch events (%d)!\n", status);
    }

    status = acpi_install_notify_handler(device->handle, ACPI_DEVICE_NOTIFY,
                    nv_acpi_event, pNvAcpiObject);

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_add: failed to install event notification handler (%d)!\n", status);
    }
    else
    {
        try_module_get(THIS_MODULE);
        pNvAcpiObject->notify_handler_installed = 1;
    }

    return 0;
}

static int nv_acpi_remove(struct acpi_device *device, int type)
{
    /*
     * This function will cause RM to relinquish control of the VGA ACPI device.
     */
    acpi_status status;
    union acpi_object control_argument_0 = { ACPI_TYPE_INTEGER };
    struct acpi_object_list control_argument_list = { 0, NULL };
    nv_acpi_t *pNvAcpiObject = device->driver_data;


    pNvAcpiObject->default_display_mask = 0;

    // arg 0, bits 1:0, 1 = disable events
    control_argument_0.integer.type = ACPI_TYPE_INTEGER;
    control_argument_0.integer.value = 0x1;

    // listify it
    control_argument_list.count = 1;
    control_argument_list.pointer = &control_argument_0;

    // _DOS method takes 1 argument and returns nothing
    status = acpi_evaluate_object(device->handle, "_DOS", &control_argument_list, NULL);

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_remove: failed to disable display switch events (%d)!\n", status);
    }

    if (pNvAcpiObject->notify_handler_installed)
    {
        // no status returned for this function
        acpi_os_wait_events_complete(NULL);

        // remove event notifier
        status = acpi_remove_notify_handler(device->handle, ACPI_DEVICE_NOTIFY, nv_acpi_event);
    }

    if (pNvAcpiObject->notify_handler_installed &&
        ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_remove: failed to remove event notification handler (%d)!\n", status);
    }
    else
    {
        NV_KMEM_CACHE_FREE_STACK(pNvAcpiObject->sp);
        os_free_mem((void *)pNvAcpiObject);
        module_put(THIS_MODULE);
        device->driver_data = NULL;
    }

    return status;
}

static void nv_acpi_event(acpi_handle handle, u32 event_type, void *data)
{
    /*
     * This function will handle acpi events from the linux kernel, used
     * to detect notifications from the VGA device.
     */
    nv_acpi_t *pNvAcpiObject = data;
    u32 event_val = 0;
    nv_acpi_integer_t state;
    int status = 0;
    int device_counter = 0;

    if (event_type == NVIF_DISPLAY_DEVICE_CHANGE_EVENT)
    {
        /* We are getting NVIF events on this machine. We arent putting a very
           extensive handling in-place to communicate back with SBIOS, know 
           the next enabled devices, and then do the switch. We just 
           pass a default display switch event, so that X-driver decides 
           the switching policy itself. */
        rm_system_event(pNvAcpiObject->sp, NV_SYSTEM_ACPI_DISPLAY_SWITCH_EVENT, 0);
    }
    if (event_type == ACPI_DISPLAY_DEVICE_CHANGE_EVENT)
    {
        if (pNvAcpiObject->default_display_mask != 1) 
        {
            while ((device_counter < NV_MAXNUM_DISPLAY_DEVICES) && 
                   (pNvAcpiObject->pNvVideo[device_counter].dev_handle))
            {
                acpi_handle dev_handle = pNvAcpiObject->pNvVideo[device_counter].dev_handle;
                int dev_id = pNvAcpiObject->pNvVideo[device_counter].dev_id;
    
                status = acpi_evaluate_integer(dev_handle,
                                               "_DGS", 
                                               NULL, 
                                               &state);
                if (ACPI_FAILURE(status)) 
                {
                    nv_printf(NV_DBG_INFO, 
                    "NVRM: nv_acpi_event: failed to query _DGS method for display device 0x%x\n",
                    dev_id);
                }
                else if (state)
                {
                    /* Check if the device is a CRT ...*/
                    if (dev_id == 0x0100)
                    {
                        event_val |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_CRT;
                    }
                    /* device-id for a TV */
                    else if (dev_id == 0x0200)
                    {
                        event_val |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_TV;
                    }
                    else if ((dev_id == 0x0110) ||  /* device id for internal LCD */
                             (dev_id == 0x0118) ||  /* alternate ACPI ID for the
                                                                        internal LCD */
                             (dev_id == 0x0400))    /* ACPI spec 3.0 specified
                                                 device id for a internal LCD*/
                    {
                        event_val |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_LCD;
                    }
                    else if ((dev_id == 0x0111) || /* the set
                                                    of possible device-ids for a DFP */
                             (dev_id == 0x0120) ||
                             (dev_id == 0x0300))   /* ACPI spec 3.0 specified
                                                    device id for non-LVDS DFP */
                    {
                        event_val |= NV_HOTKEY_STATUS_DISPLAY_ENABLE_DFP;
                    }
                }
                device_counter++;
            }
        }

        nv_printf(NV_DBG_INFO,
        "NVRM: nv_acpi_event: Event-type 0x%x, Event-val 0x%x\n", 
        event_type, event_val);

        rm_system_event(pNvAcpiObject->sp, NV_SYSTEM_ACPI_DISPLAY_SWITCH_EVENT, event_val);
    }

    // no unsubscription or re-enable necessary. Once DOD has been set, we are go.
    // once we are subscribed to ACPI events, we don't have to re-subscribe unless
    // unsubscribe.
}

#if defined(NV_ACPI_DEVICE_OPS_HAS_MATCH)
static int nv_acpi_match(struct acpi_device *device, struct acpi_driver *driver)
{
    acpi_handle DOS_method_handler, DOD_method_handler;

    /*
     *
     * attempt to get handles to all of these objects
     * _DOS = enable/disable output switching method
     * _DOD = enumerate all devices attached to the display driver
     * 
     * If handles are available, this is the VGA object.
     *
     */

    if ((!acpi_get_handle(device->handle, "_DOS", &DOS_method_handler)) &&
        (!acpi_get_handle(device->handle, "_DOD", &DOD_method_handler)))
    {
        nv_printf(NV_DBG_INFO, "NVRM: nv_acpi_match: VGA ACPI device detected.\n");

        return 0;
    }
    else
        return (-ENODEV);
}
#endif

/* Do the necessary allocations and install notifier "handler" on the device-node "device" */
static RM_STATUS nv_install_notifier(struct acpi_device *device, acpi_notify_handler handler)
{        
    nv_stack_t *sp = NULL;
    nv_acpi_t *pNvAcpiObject = NULL;
    RM_STATUS rmStatus = RM_ERROR;
    acpi_status status = -1;



    if ((!device) || device->driver_data)
        return RM_ERROR;

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: nv_install_notifier: failed to allocate stack!\n");
        return RM_ERROR;
    }

    rmStatus = os_alloc_mem((void **) &pNvAcpiObject, sizeof(nv_acpi_t));
    if (rmStatus != RM_OK)
        goto return_error;

    os_mem_set((void *)pNvAcpiObject, 0, sizeof(nv_acpi_t));

    // assign driver data structure ptr to this device
    device->driver_data = pNvAcpiObject;

    // store a device reference in our object
    pNvAcpiObject->device = device;
    pNvAcpiObject->sp = sp;

    status = acpi_install_notify_handler(device->handle, ACPI_DEVICE_NOTIFY,
              handler, pNvAcpiObject);
    if (!ACPI_FAILURE(status)) 
    {
        pNvAcpiObject->notify_handler_installed = 1;
        return RM_OK;
    }


return_error:

     NV_KMEM_CACHE_FREE_STACK(sp);
     if (pNvAcpiObject)
         os_free_mem((void *)pNvAcpiObject);
     device->driver_data = NULL;
     return RM_ERROR;

}

/* Tear-down and remove whatever nv_install_notifier did */ 
static void nv_uninstall_notifier(struct acpi_device *device, acpi_notify_handler handler)
{
    acpi_status status;
    nv_acpi_t *pNvAcpiObject = NULL;

    if (!device)
        return;

    pNvAcpiObject = device->driver_data;
    if (pNvAcpiObject && pNvAcpiObject->notify_handler_installed)
    {
        status = acpi_remove_notify_handler(device->handle, ACPI_DEVICE_NOTIFY, handler);
        if (ACPI_FAILURE(status))
        {
            nv_printf(NV_DBG_INFO,
                "NVRM: nv_acpi_methods_uninit: failed to remove event notification handler (%d)!\n", status);
        }
        else
        {
            NV_KMEM_CACHE_FREE_STACK(pNvAcpiObject->sp);
            os_free_mem((void *)pNvAcpiObject);
            device->driver_data = NULL;
        }
    }
   
    return;
}


/*
 * acpi methods init function.
 * check if the NVIF, _DSM and WMMX methods are present in the acpi namespace.
 * store NVIF, _DSM and WMMX handle if found. 
 */

void NV_API_CALL nv_acpi_methods_init(NvU32 *handlesPresent)
{
    struct acpi_device *device = NULL;
    RM_STATUS rmStatus;
    int retVal = -1;


    if (!handlesPresent) // Caller passed us invalid pointer.
        return;


    *handlesPresent = 0;

    NV_ACPI_WALK_NAMESPACE(ACPI_TYPE_DEVICE, ACPI_ROOT_OBJECT,
                        ACPI_UINT32_MAX, nv_acpi_find_methods, NULL, NULL);

    if (nvif_handle)
    {
        *handlesPresent = NV_ACPI_NVIF_HANDLE_PRESENT;
        do
        {
            if (!nvif_parent_gpu_handle) /* unknown error */
                break;
            
            retVal = acpi_bus_get_device(nvif_parent_gpu_handle, &device);

            if (ACPI_FAILURE(retVal) || !device)
                break;

            if (device->driver_data)
            {
                nvif_parent_gpu_handle = NULL;
                break;  /* Someone else has already populated this device 
                           nodes' structures. So nothing more to be done */
            }

            rmStatus = nv_install_notifier(device, nv_acpi_event);
            if (rmStatus != RM_OK)
                nvif_parent_gpu_handle = NULL;

        } while (0);
    }

    if (wmmx_handle)
        *handlesPresent = *handlesPresent | NV_ACPI_WMMX_HANDLE_PRESENT;
    if (mxmi_handle)
        *handlesPresent = *handlesPresent | NV_ACPI_MXMI_HANDLE_PRESENT;
    if (mxms_handle)
        *handlesPresent = *handlesPresent | NV_ACPI_MXMS_HANDLE_PRESENT;

    return;
}
 
acpi_status nv_acpi_find_methods(
    acpi_handle handle,
    u32 nest_level,
    void *dummy1,
    void **dummy2
)
{
    acpi_handle method_handle;

    if (!acpi_get_handle(handle, "NVIF", &method_handle))
    {
        nvif_handle = method_handle;
        nvif_parent_gpu_handle = handle;
    }

    if (!acpi_get_handle(handle, "WMMX", &method_handle))
    {
        wmmx_handle = method_handle;
    }

    if (!acpi_get_handle(handle, "MXMI", &method_handle))
    {
        mxmi_handle = method_handle;
    }

    if (!acpi_get_handle(handle, "MXMS", &method_handle))
    {
        mxms_handle = method_handle;
    }

    return 0;
}

void NV_API_CALL nv_acpi_methods_uninit(void)
{
    struct acpi_device *device = NULL;

    nvif_handle = NULL;
    wmmx_handle = NULL;
    mxmi_handle = NULL;
    mxms_handle = NULL;

    if (nvif_parent_gpu_handle == NULL) 
        return;

    acpi_bus_get_device(nvif_parent_gpu_handle, &device);

    nv_uninstall_notifier(device, nv_acpi_event);
    nvif_parent_gpu_handle = NULL;

    return;
}

RM_STATUS NV_API_CALL nv_acpi_method(
    NvU32 acpi_method,
    NvU32 function,
    NvU32 subFunction,
    void  *inParams,
    NvU16 inParamSize,
    NvU32 *outStatus,
    void  *outData,
    NvU16 *outDataSize
)
{
    RM_STATUS status;

    switch (acpi_method)
    {
        case NV_ACPI_METHOD_NVIF:
            status = nv_acpi_nvif_method(function, 
                                         subFunction, 
                                         inParams, 
                                         inParamSize, 
                                         outStatus, 
                                         outData, 
                                         outDataSize);
            break;

        case NV_ACPI_METHOD_MXMI:
            status = nv_acpi_mxmi_method(outData, outDataSize);
            break;

        case NV_ACPI_METHOD_MXMS:
            status = nv_acpi_mxms_method(outData, outDataSize);
            break;

        case NV_ACPI_METHOD_WMMX:
            status = nv_acpi_wmmx_method(function,  outData, outDataSize);
            break;

        default:
            status = RM_ERR_NOT_SUPPORTED;
    }

    return status;
}  

/*
 * This function executes an MXMI ACPI method.
 */
static RM_STATUS nv_acpi_mxmi_method(
    NvU8  *pInOut,
    NvU16 *pSize
)
{
    acpi_status status;
    struct acpi_object_list input;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *mxmi = NULL;
    union acpi_object mxmi_params[2];

    if (!mxmi_handle)
        return RM_ERR_NOT_SUPPORTED;

    if ((pInOut == NULL) || (pSize == NULL))
    {
        nv_printf(NV_DBG_INFO,
              "NVRM: nv_acpi_mxmi_method: invalid argument(s)!\n");
        return RM_ERR_INVALID_ARGUMENT;
    }

    if (*pSize == 0)
    {
        *pSize = sizeof(NvU8);
        return RM_ERR_BUFFER_TOO_SMALL;
    }

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_mxmi_method: invalid context!\n");
#endif
        return RM_ERR_NOT_SUPPORTED;
    }

    mxmi_params[0].integer.type   = ACPI_TYPE_INTEGER;
    mxmi_params[0].integer.value  = 0;

    mxmi_params[1].buffer.type    = ACPI_TYPE_BUFFER;
    mxmi_params[1].buffer.pointer = pInOut;
    mxmi_params[1].buffer.length  = sizeof(NvU8);

    input.count = 2;
    input.pointer = mxmi_params;

    status = acpi_evaluate_object(mxmi_handle, NULL, &input, &output);
    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_mxmi_method: failed to get MXMI data!\n");
        return RM_ERROR;
    }

    mxmi = output.pointer;
    if ((mxmi != NULL) && (mxmi->type == ACPI_TYPE_INTEGER))
    {
        *pInOut = mxmi->integer.value;
    }
    else
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_mxmi_method: MXMI data invalid!\n");
        kfree(output.pointer);
        return RM_ERROR;
    }

    kfree(output.pointer);
    return RM_OK;
}

/*
 * This function executes an MXMS ACPI method.
 */
static RM_STATUS nv_acpi_mxms_method(
    NvU8  *pInOut,
    NvU16 *pSize
)
{
    acpi_status status;
    struct acpi_object_list input;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *mxms = NULL;
    union acpi_object mxms_params[2];

    if (!mxms_handle)
        return RM_ERR_NOT_SUPPORTED;

    if ((pInOut == NULL) || (pSize == NULL))
    {
        nv_printf(NV_DBG_INFO,
              "NVRM: nv_acpi_mxms_method: invalid argument(s)!\n");
        return RM_ERR_INVALID_ARGUMENT;
    }

    if (*pSize == 0)
    {
        *pSize = sizeof(NvU8);
        return RM_ERR_BUFFER_TOO_SMALL;
    }

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_mxms_method: invalid context!\n");
#endif
        return RM_ERR_NOT_SUPPORTED;
    }

    mxms_params[0].integer.type   = ACPI_TYPE_INTEGER;
    mxms_params[0].integer.value  = 0;

    mxms_params[1].buffer.type    = ACPI_TYPE_BUFFER;
    mxms_params[1].buffer.pointer = pInOut;
    mxms_params[1].buffer.length  = sizeof(NvU8);

    input.count = 2;
    input.pointer = mxms_params;

    status = acpi_evaluate_object(mxms_handle, NULL, &input, &output);
    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_mxms_method: failed to get MXMS data!\n");
        return RM_ERROR;
    }

    mxms = output.pointer;
    if ((mxms != NULL) && (mxms->type == ACPI_TYPE_BUFFER) &&
        (mxms->buffer.length != 0))
    {
        if (*pSize >= mxms->buffer.length)
        {
            *pSize = NV_MIN(*pSize, mxms->buffer.length);
            memcpy(pInOut, mxms->buffer.pointer, mxms->buffer.length);
        }
        else
        {
            *pSize = mxms->buffer.length;
             kfree(output.pointer);
            return RM_ERR_BUFFER_TOO_SMALL;
        }
    }
    else
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_mxms_method: MXMS data invalid!\n");
        kfree(output.pointer);
        return RM_ERROR;
    }

    kfree(output.pointer);
    return RM_OK;
}

/*
 * This function executes an NVIF ACPI method.
 */
static RM_STATUS nv_acpi_nvif_method(
    NvU32 function, 
    NvU32 subFunction, 
    void  *inParams, 
    NvU16 inParamSize, 
    NvU32 *outStatus, 
    void  *outData, 
    NvU16 *outDataSize
)
{
    acpi_status status;
    struct acpi_object_list input;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *nvif = NULL;
    union acpi_object nvif_params[3];
    NvU16 localOutDataSize; 
    NvU8  localInParams[8];

    if (!nvif_handle)
        return RM_ERR_NOT_SUPPORTED;

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_nvif_method: invalid context!\n");
#endif
        return RM_ERR_NOT_SUPPORTED;
    }

    nvif_params[0].integer.type  =  ACPI_TYPE_INTEGER;
    nvif_params[0].integer.value =  function;

    nvif_params[1].integer.type  =  ACPI_TYPE_INTEGER;
    nvif_params[1].integer.value =  subFunction;

    nvif_params[2].buffer.type  =  ACPI_TYPE_BUFFER;
 
    if (inParams && (inParamSize > 0))
    {
        nvif_params[2].buffer.length  = inParamSize;
        nvif_params[2].buffer.pointer = inParams;
    }
    else
    {
        memset(localInParams, 0, 8);  
        nvif_params[2].buffer.length  = 8;
        nvif_params[2].buffer.pointer = localInParams;
    }

    input.count = 3;
    input.pointer = nvif_params;

    status = acpi_evaluate_object(nvif_handle, NULL, &input, &output);
    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_nvif_method: failed to get NVIF data, "
            "status 0x%x, function 0x%x, subFunction 0x%x!\n",
            status, function, subFunction);
        return RM_ERROR;
    }
    
    nvif = output.pointer;
    if (nvif && (nvif->type == ACPI_TYPE_BUFFER) && (nvif->buffer.length >= 4))
    {
        if (outStatus)
        {
            *outStatus = nvif->buffer.pointer[3] << 24 | 
                         nvif->buffer.pointer[2] << 16 |
                         nvif->buffer.pointer[1] << 8  |
                         nvif->buffer.pointer[0];
        }

        if (outData && outDataSize)
        {
            localOutDataSize = nvif->buffer.length - 4;
            if (localOutDataSize <= *outDataSize)
            {
                *outDataSize = NV_MIN(*outDataSize, localOutDataSize);
                memcpy(outData, &nvif->buffer.pointer[4], *outDataSize);
            }
            else
            {
                *outDataSize = localOutDataSize;
                kfree(output.pointer);
                return RM_ERR_BUFFER_TOO_SMALL;
            }
        } 
    }
    else
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_nvif_method: NVIF data invalid, function 0x%x, "
            "subFunction 0x%x!\n", function, subFunction);
        kfree(output.pointer);
        return RM_ERROR;
    }
 
    kfree(output.pointer);
    return RM_OK; 
}

/*
 * This function executes a _DSM ACPI method.
 */
RM_STATUS NV_API_CALL nv_acpi_dsm_method(
    nv_state_t *nv,
    NvU8  *pAcpiDsmGuid,
    NvU32 acpiDsmRev,
    NvU32 acpiDsmSubFunction,
    void  *pInParams,
    NvU16 inParamSize,
    NvU32 *outStatus,
    void  *pOutData,
    NvU16 *pSize
)
{
    acpi_status status;
    struct acpi_object_list input;
    union acpi_object *dsm = NULL;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object dsm_params[4];
    NvU8 argument3[4]; /* For all DSM sub functions, input size is 4 */
    acpi_handle dev_handle  = NULL;
#ifdef DEVICE_ACPI_HANDLE
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv); 
    dev_handle = DEVICE_ACPI_HANDLE(&nvl->dev->dev);
#endif
    if (!dev_handle)
        return RM_ERR_NOT_SUPPORTED;

    if ((!pInParams) || (inParamSize != 4) || (!pOutData) || (!pSize))
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: nv_acpi_dsm_method: Guid (first 4 bytes) %04x DSM subfunction=0x%x,"
                  " invalid argument(s)!\n",
                  (NvU32) *pAcpiDsmGuid,
                  acpiDsmSubFunction);
        return RM_ERR_INVALID_ARGUMENT;
    }

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: nv_acpi_dsm_method: Guid (first 4 bytes) %04x DSM subfunction=0x%x,"
                  " invalid context!\n",
                  (NvU32) *pAcpiDsmGuid,
                  acpiDsmSubFunction);
#endif
        return RM_ERR_NOT_SUPPORTED;
    }

    // 
    // dsm_params[0].buffer.pointer and dsm_params[1].integer.value set in
    // switch below based on acpiDsmFunction
    // 

    dsm_params[0].buffer.type    = ACPI_TYPE_BUFFER;
    dsm_params[0].buffer.length  = 0x10;
    dsm_params[0].buffer.pointer = pAcpiDsmGuid;

    dsm_params[1].integer.type   = ACPI_TYPE_INTEGER;
    dsm_params[1].integer.value  = acpiDsmRev;

    dsm_params[2].integer.type   = ACPI_TYPE_INTEGER;
    dsm_params[2].integer.value  = acpiDsmSubFunction;

    // 3rd dsm argument always a dword. inParamSize checked on entry to this routine.
    dsm_params[3].buffer.type    = ACPI_TYPE_BUFFER;
    dsm_params[3].buffer.length  = inParamSize;
    memcpy(argument3, pInParams, dsm_params[3].buffer.length);
    dsm_params[3].buffer.pointer = argument3;

    // parameters for dsm calls (GUID, rev, subfunction, data)
    input.count = 4;
    input.pointer = dsm_params;

    status = acpi_evaluate_object(dev_handle, "_DSM", &input, &output);
    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: nv_acpi_dsm_method: failed to get"
                  " Guid (first 4 bytes) %04x DSM subfunction 0x%x data, status 0x%x!\n",
                  (NvU32) *pAcpiDsmGuid,
                  acpiDsmSubFunction,
                  status);
        return RM_ERROR;
    }
    else
    {
        dsm = output.pointer;
        if (outStatus)
        {
            *outStatus = dsm->buffer.pointer[3] << 24 | 
                         dsm->buffer.pointer[2] << 16 |
                         dsm->buffer.pointer[1] << 8  |
                         dsm->buffer.pointer[0];
        }

        if (dsm && (dsm->type == ACPI_TYPE_BUFFER) && (dsm->buffer.length > 0))
        {
            if (dsm->buffer.length <= *pSize)
            {
                *pSize = NV_MIN(*pSize, dsm->buffer.length);
                memcpy((void *)pOutData, dsm->buffer.pointer, *pSize);
            } 
            else
            {
                kfree(output.pointer);
                return RM_ERR_BUFFER_TOO_SMALL;
            }
        }
        else if (dsm && (dsm->type == ACPI_TYPE_INTEGER))
        {
            if (*pSize >= 4)
            {
                *pSize = 4;
                 memcpy((void *)pOutData, &dsm->integer.value, *pSize);
            }  
            else
            {
                kfree(output.pointer);
                return RM_ERR_BUFFER_TOO_SMALL;
            }
        }

        // 
        // TBD some ASL returns a package type. Example SPB.
        // This may need to be added in the future.
        // 

        else
        {
            nv_printf(NV_DBG_INFO,
                    "NVRM: nv_acpi_dsm_method: Guid (first 4 bytes) %04x DSM subfunction 0x%x"
                    " data invalid!\n",
                    (NvU32) *pAcpiDsmGuid,
                    acpiDsmSubFunction);
            kfree(output.pointer);
            return RM_ERROR;
        }
    }
 
    kfree(output.pointer);
    return RM_OK;             
}

/*
 * This function executes a _DDC ACPI method.
 */
RM_STATUS NV_API_CALL nv_acpi_ddc_method(
    nv_state_t *nv,
    void *pEdidBuffer,
    NvU32 *pSize
)
{
    acpi_status status;
    struct acpi_device *device = NULL;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *ddc;
    union acpi_object ddc_arg0 = { ACPI_TYPE_INTEGER };
    struct acpi_object_list input = { 1, &ddc_arg0 };
    struct list_head *node, *next;
    nv_acpi_integer_t device_id = 0;
    NvU32 i;
    acpi_handle dev_handle  = NULL;
    acpi_handle lcd_dev_handle  = NULL;
#ifdef DEVICE_ACPI_HANDLE
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv); 
    dev_handle = DEVICE_ACPI_HANDLE(&nvl->dev->dev);
#else
        return RM_ERR_NOT_SUPPORTED;
#endif
    if (!dev_handle)
        return RM_ERR_INVALID_ARGUMENT;

    status = acpi_bus_get_device(dev_handle, &device);

    if (ACPI_FAILURE(status) || !device)
        return RM_ERR_INVALID_ARGUMENT;

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s: invalid context!\n",
                  __FUNCTION__);
#endif
        return RM_ERR_NOT_SUPPORTED;
    }

    list_for_each_safe(node, next, &device->children) 
    {
        struct acpi_device *dev =
            list_entry(node, struct acpi_device, node);

        if (!dev)
            continue;

        status = acpi_evaluate_integer(dev->handle, "_ADR", NULL, &device_id);
        if (ACPI_FAILURE(status))
            /* Couldnt query device_id for this device */
            continue;

        device_id = (device_id & 0xffff);

        if ((device_id == 0x0110) || (device_id == 0x0118) || (device_id == 0x0400)) /* Only for an LCD*/
        {
            lcd_dev_handle = dev->handle;
            nv_printf(NV_DBG_INFO, "NVRM: %s Found LCD: %x\n", __FUNCTION__, device_id);
            break;
        }

    }

    if (lcd_dev_handle == NULL)
    {
        nv_printf(NV_DBG_INFO, "NVRM: %s LCD not found\n", __FUNCTION__);
        return RM_ERROR;
    }

    // 
    // As per ACPI Spec 3.0:
    // ARG0 = 0x1 for 128 bytes edid buffer
    // ARG0 = 0x2 for 256 bytes edid buffer
    // 
    for (i = 1; i <= 2; i++)
    {

        ddc_arg0.integer.value = i;
        status = acpi_evaluate_object(lcd_dev_handle, "_DDC", &input, &output);
        if (ACPI_SUCCESS(status))
            break;
    }

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: %s: failed status: %08x \n",
                  __FUNCTION__,
                  status);
        return RM_ERROR;
    }
    else
    {
        ddc = output.pointer;

        if (ddc && (ddc->type == ACPI_TYPE_BUFFER) && (ddc->buffer.length > 0))
        {
            if (ddc->buffer.length <= *pSize)
            {
                *pSize = NV_MIN(*pSize, ddc->buffer.length);
                memcpy(pEdidBuffer, ddc->buffer.pointer, *pSize);
            } 
            else
            {
                kfree(output.pointer);
                return RM_ERR_BUFFER_TOO_SMALL;
            }
        }
    }
 
    kfree(output.pointer);
    return RM_OK;             
}

/*
 * This function executes a _ROM ACPI method.
 */
RM_STATUS NV_API_CALL nv_acpi_rom_method(
    nv_state_t *nv,
    NvU32 *pInData,
    NvU32 *pOutData
)
{
    acpi_status status;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *rom;
    union acpi_object rom_arg[2];
    struct acpi_object_list input = { 2, rom_arg };
    acpi_handle dev_handle  = NULL;
#ifdef DEVICE_ACPI_HANDLE
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv); 
    dev_handle = DEVICE_ACPI_HANDLE(&nvl->dev->dev);
#else
    return RM_ERR_NOT_SUPPORTED;
#endif
    if (!dev_handle)
        return RM_ERR_INVALID_ARGUMENT;

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s: invalid context!\n",
                  __FUNCTION__);
#endif
        return RM_ERR_NOT_SUPPORTED;
    }

    // 
    // As per ACPI Spec 3.0:
    // ARG0 = Offset of the display device ROM data
    // ARG1 = Size of the buffer to fill in (upto 4K)
    // 
    rom_arg[0].type = ACPI_TYPE_INTEGER;
    rom_arg[0].integer.value = pInData[0];
    rom_arg[1].type = ACPI_TYPE_INTEGER;
    rom_arg[1].integer.value = pInData[1];
    status = acpi_evaluate_object(dev_handle, "_ROM", &input, &output);

    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: %s: failed status: %08x \n",
                  __FUNCTION__,
                  status);
        return RM_ERROR;
    }
    else
    {
        rom = output.pointer;

        if (rom && (rom->type == ACPI_TYPE_BUFFER) &&
            (rom->buffer.length == pInData[1]))
        {
            memcpy(pOutData, rom->buffer.pointer, rom->buffer.length);
        }
        else
        {
            nv_printf(NV_DBG_INFO,
                      "NVRM: %s: Invalid _ROM data \n",
                      __FUNCTION__);
            kfree(output.pointer);
            return RM_ERROR;
        }
    }
 
    kfree(output.pointer);
    return RM_OK;
}


/*
 * This function executes a WMMX ACPI method.
 */
static RM_STATUS  nv_acpi_wmmx_method(
    NvU32 arg2,
    NvU8  *outData,
    NvU16 *outDataSize
)
{
    acpi_status status;
    struct acpi_object_list input;
    struct acpi_buffer output = { ACPI_ALLOCATE_BUFFER, NULL };
    union acpi_object *mmx = NULL;
    union acpi_object mmx_params[3];

    if (!wmmx_handle)
        return RM_ERR_NOT_SUPPORTED;

    if (!NV_MAY_SLEEP())
    {
#if defined(DEBUG)
        nv_printf(NV_DBG_ERRORS,
            "NVRM: nv_acpi_wmmx_method: invalid context!\n");
#endif
        return RM_ERR_NOT_SUPPORTED;
    }

   /* argument 0 and argument 1 are not used in WMMX method, passing 0 */

    mmx_params[0].integer.type  =  ACPI_TYPE_INTEGER;
    mmx_params[0].integer.value =  0;

    mmx_params[1].integer.type  =  ACPI_TYPE_INTEGER;
    mmx_params[1].integer.value =  0;

    mmx_params[2].integer.type  =  ACPI_TYPE_INTEGER;
    mmx_params[2].integer.value =  arg2;

    input.count = 3;
    input.pointer = mmx_params;

    status = acpi_evaluate_object(wmmx_handle, NULL, &input, &output);
    if (ACPI_FAILURE(status))
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: nv_acpi_wmmx_method: failed to get WMMX data, "
            "status 0x%x!\n", status);
        return RM_ERROR;
    }

    mmx = output.pointer;
    if (mmx && (mmx->type == ACPI_TYPE_BUFFER) && (mmx->buffer.length > 0))
    {
        if (outData && outDataSize)
        {
            if (mmx->buffer.length <= *outDataSize)
            {
                *outDataSize = NV_MIN(*outDataSize, mmx->buffer.length);
                memcpy(outData, mmx->buffer.pointer, *outDataSize);
            }
            else
            {
                kfree(output.pointer);
                return RM_ERR_BUFFER_TOO_SMALL;
            }
        }
    }
    else
    {
        nv_printf(NV_DBG_ERRORS,
                "NVRM: nv_acpi_wmmx_method: WMMX data invalid.\n");
        kfree(output.pointer);
        return RM_ERROR;
    }

    kfree(output.pointer);
    return RM_OK;
}

#else // NV_LINUX_ACPI_EVENTS_SUPPORTED

int nv_acpi_init(void)
{
    return 0;
}

int nv_acpi_uninit(void)
{
    return 0;
}

void NV_API_CALL nv_acpi_methods_init(NvU32 *handlePresent)
{
    *handlePresent = 0;
}

void NV_API_CALL nv_acpi_methods_uninit(void)
{
    return;
}

RM_STATUS NV_API_CALL nv_acpi_method(
    NvU32 acpi_method,
    NvU32 function,
    NvU32 subFunction,
    void  *inParams,
    NvU16 inParamSize,
    NvU32 *outStatus,
    void  *outData,
    NvU16 *outDataSize
)
{
    return RM_ERR_NOT_SUPPORTED;
}

RM_STATUS NV_API_CALL nv_acpi_dsm_method(
    nv_state_t *nv,
    NvU8  *pAcpiDsmGuid,
    NvU32 acpiDsmRev,
    NvU32 acpiDsmSubFunction,
    void  *pInParams,
    NvU16 inParamSize,
    NvU32 *outStatus,
    void  *pOutData,
    NvU16 *pSize
)
{
    return RM_ERR_NOT_SUPPORTED;
}

RM_STATUS NV_API_CALL nv_acpi_ddc_method(
    nv_state_t *nv,
    void *pEdidBuffer,
    NvU32 *pSize
)
{
    return RM_ERR_NOT_SUPPORTED;
}

RM_STATUS NV_API_CALL nv_acpi_rom_method(
    nv_state_t *nv,
    NvU32 *pInData,
    NvU32 *pOutData
)
{
    return RM_ERR_NOT_SUPPORTED;
}
#endif

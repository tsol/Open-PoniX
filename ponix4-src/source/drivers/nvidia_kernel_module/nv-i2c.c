/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 2005-2011 by NVIDIA Corporation.  All rights reserved.  All
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

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)

#if defined(KERNEL_2_4)
extern int i2c_add_adapter (struct i2c_adapter *) __attribute__ ((weak));
extern int i2c_del_adapter (struct i2c_adapter *) __attribute__ ((weak));
#define NV_WEAK_SYMBOL_PRESENT(x) (x != NULL)
#endif

static int nv_i2c_algo_master_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
    nv_state_t *nv = (nv_state_t *)adap->algo_data;
    unsigned int i = 0;
    int rc = -EIO;
    RM_STATUS rmStatus = RM_OK;
    nv_stack_t *sp = NULL;

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return -ENOMEM;
    }

    for (i = 0; ((i < (unsigned int)num) && (rmStatus == RM_OK)); i++)
    {
        if (msgs[i].flags & ~I2C_M_RD)
        {
            /* we don't support I2C_FUNC_10BIT_ADDR, I2C_FUNC_PROTOCOL_MANGLING */
            rc = -EINVAL;
            rmStatus = RM_ERR_INVALID_ARGUMENT;
        }
        else if (msgs[i].flags & I2C_M_RD)
        {
            rmStatus = rm_i2c_read_buffer(sp, nv, (void *)adap,
                                          (NvU8)(msgs[i].addr & 0x7f),
                                          (NvU32)(msgs[i].len & 0xffffUL),
                                          (NvU8 *)msgs[i].buf);
        }
        else
        {
            rmStatus = rm_i2c_write_buffer(sp, nv, (void *)adap,
                                           (NvU8)(msgs[i].addr & 0x7f),
                                           (NvU32)(msgs[i].len & 0xffffUL), 
                                           (NvU8 *)msgs[i].buf);
        }
    }

    NV_KMEM_CACHE_FREE_STACK(sp);

    return (rmStatus != RM_OK) ? rc : num;
}

static int nv_i2c_algo_smbus_xfer(
    struct i2c_adapter *adap,
    u16 addr,
    unsigned short flags,
    char read_write,
    u8 command,
    int size,
    union i2c_smbus_data *data
)
{
    nv_state_t *nv = (nv_state_t *)adap->algo_data;
    int rc = -EIO;
    RM_STATUS rmStatus = RM_OK;
    nv_stack_t *sp = NULL;

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return -ENOMEM;
    }

    switch (size)
    {
        case I2C_SMBUS_QUICK:
            rmStatus = rm_i2c_smbus_write_quick(sp, nv, (void *)adap,
                                                (NvU8)(addr & 0x7f),
                                                (read_write == I2C_SMBUS_READ));
            break;

        case I2C_SMBUS_BYTE:
            if (read_write == I2C_SMBUS_READ)
            {
                rmStatus = rm_i2c_read_buffer(sp, nv, (void *)adap,
                                              (NvU8)(addr & 0x7f),
                                              1, /* single byte transfer */
                                              (NvU8 *)&data->byte);
            }
            else
            {
                u8 data = command;
                rmStatus = rm_i2c_write_buffer(sp, nv, (void *)adap,
                                               (NvU8)(addr & 0x7f),
                                               1, /* single byte transfer */
                                               (NvU8 *)&data);
            }
            break;

        case I2C_SMBUS_BYTE_DATA:
            if (read_write == I2C_SMBUS_READ)
            {
                rmStatus = rm_i2c_smbus_read_buffer(sp, nv, (void *)adap,
                                                    (NvU8)(addr & 0x7f),
                                                    (NvU8)command,
                                                    1, /* single byte transfer */
                                                    (NvU8 *)&data->byte);
            }
            else
            {
                rmStatus = rm_i2c_smbus_write_buffer(sp, nv, (void *)adap,
                                                     (NvU8)(addr & 0x7f),
                                                     (NvU8)command,
                                                     1, /* single byte transfer */
                                                     (NvU8 *)&data->byte);
            }
            break;

        case I2C_SMBUS_WORD_DATA:
            if (read_write == I2C_SMBUS_READ)
            {
                rmStatus = rm_i2c_smbus_read_buffer(sp, nv, (void *)adap,
                                                    (NvU8)(addr & 0x7f),
                                                    (NvU8)command,
                                                    2, /* single word transfer */
                                                    (NvU8 *)&data->block[1]);
                data->word = ((NvU16)data->block[1]) |
                             ((NvU16)data->block[2] << 8);
            }
            else
            {
                u16 word = data->word;
                data->block[1] = (word & 0xff);
                data->block[2] = (word >> 8);
                rmStatus = rm_i2c_smbus_write_buffer(sp, nv, (void *)adap,
                                                     (NvU8)(addr & 0x7f),
                                                     (NvU8)command,
                                                     2, /* single word transfer */
                                                     (NvU8 *)&data->block[1]);
            }
            break;

        default:
            rc = -EINVAL;
            rmStatus = RM_ERR_INVALID_ARGUMENT;
    }

    NV_KMEM_CACHE_FREE_STACK(sp);

    return (rmStatus != RM_OK) ? rc : 0;
}

static u32 nv_i2c_algo_functionality(struct i2c_adapter *adap)
{
    return (I2C_FUNC_I2C |
            I2C_FUNC_SMBUS_QUICK |
            I2C_FUNC_SMBUS_BYTE |
            I2C_FUNC_SMBUS_BYTE_DATA |
            I2C_FUNC_SMBUS_WORD_DATA);
}

static struct i2c_algorithm nv_i2c_algo = {
    .master_xfer      = nv_i2c_algo_master_xfer,
    .smbus_xfer       = nv_i2c_algo_smbus_xfer,
    .functionality    = nv_i2c_algo_functionality,
};

#if defined(KERNEL_2_4)

#if defined(NV_I2C_ADAPTER_HAS_INC_USE)
static void nv_i2c_adapter_inc_use(struct i2c_adapter * pAdapter)
{
    MOD_INC_USE_COUNT;

    return;
}

static void nv_i2c_adapter_dec_use(struct i2c_adapter * pAdapter)
{
    MOD_DEC_USE_COUNT;

    return;
}
#endif

#if defined(NV_I2C_ADAPTER_HAS_CLIENT_REGISTER)
static int nv_i2c_adapter_register_client(struct i2c_client * pClient)
{
    MOD_INC_USE_COUNT;

    return 0;
}

static int nv_i2c_adapter_unregister_client(struct i2c_client * pClient)
{
    MOD_DEC_USE_COUNT;

    return 0;
}
#endif

struct i2c_adapter nv_i2c_adapter_prototype = {
    .id                = I2C_HW_B_RIVA,
    .algo              = &nv_i2c_algo,
    .algo_data         = NULL,
#if defined(NV_I2C_ADAPTER_HAS_INC_USE)
    .inc_use           = nv_i2c_adapter_inc_use,
    .dec_use           = nv_i2c_adapter_dec_use,
#endif
#if defined(NV_I2C_ADAPTER_HAS_CLIENT_REGISTER)
    .client_register   = nv_i2c_adapter_register_client,
    .client_unregister = nv_i2c_adapter_unregister_client,
#endif
    .data              = NULL,
};

#else // defined(KERNEL_2_4)

#if defined(NV_I2C_ADAPTER_HAS_CLIENT_REGISTER)
static int nv_i2c_adapter_register_client(struct i2c_client * pClient)
{
    try_module_get(THIS_MODULE);
    return 0;
}

static int nv_i2c_adapter_unregister_client(struct i2c_client * pClient)
{
    module_put(THIS_MODULE);
    return 0;
}
#endif

struct i2c_adapter nv_i2c_adapter_prototype = {
    .owner             = THIS_MODULE,
    .algo              = &nv_i2c_algo,
    .algo_data         = NULL,
#if defined(NV_I2C_ADAPTER_HAS_CLIENT_REGISTER)
    .client_register   = nv_i2c_adapter_register_client,
    .client_unregister = nv_i2c_adapter_unregister_client,
#endif
};

#endif // defined(KERNEL_2_4)

void* NV_API_CALL nv_i2c_add_adapter(nv_state_t *nv, NvU32 port)
{
    RM_STATUS rmStatus;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct i2c_adapter *pI2cAdapter = NULL;
    int osstatus = 0;

#if defined(KERNEL_2_4)
    if (!NV_WEAK_SYMBOL_PRESENT(i2c_add_adapter))
        return NULL;
    if (!NV_WEAK_SYMBOL_PRESENT(i2c_del_adapter))
        return NULL;
#endif

    // get a i2c adapter
    rmStatus = os_alloc_mem((void **)&pI2cAdapter,sizeof(struct i2c_adapter));

    if (rmStatus != RM_OK)
        return NULL;

    // fill in with default structure
    os_mem_copy((void *)pI2cAdapter, (void *)&nv_i2c_adapter_prototype, sizeof(struct i2c_adapter));

#if !defined(KERNEL_2_4)
    pI2cAdapter->dev.parent = &nvl->dev->dev;
#endif

    snprintf(pI2cAdapter->name, sizeof(pI2cAdapter->name),
             "NVIDIA i2c adapter %u at %x:%02x.%u", port, nv->bus,
             nv->slot, PCI_FUNC(nvl->dev->devfn));

    // add our data to the structure
    pI2cAdapter->algo_data = (void *)nv;

    // attempt to register with the kernel
    osstatus = i2c_add_adapter(pI2cAdapter);

    if (osstatus)
    {
        // free the memory and NULL the ptr
        os_free_mem(pI2cAdapter);

        pI2cAdapter = NULL;
    }

    return ((void *)pI2cAdapter);
}

BOOL NV_API_CALL nv_i2c_del_adapter(nv_state_t *nv, void *data)
{
    struct i2c_adapter *pI2cAdapter = (struct i2c_adapter *)data;
    int osstatus = 0;
    BOOL wasReleased = FALSE;

#if defined(KERNEL_2_4)
    if (!NV_WEAK_SYMBOL_PRESENT(i2c_add_adapter))
        return FALSE;
    if (!NV_WEAK_SYMBOL_PRESENT(i2c_del_adapter))
        return FALSE;
#endif

    if (!pI2cAdapter) return FALSE;

    // attempt release with the OS
    osstatus = i2c_del_adapter(pI2cAdapter);

    if (!osstatus)
    {
        os_free_mem(pI2cAdapter);
        wasReleased = TRUE;
    }

    return wasReleased;
}

#else // (defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE))

BOOL NV_API_CALL nv_i2c_del_adapter(nv_state_t *nv, void *data)
{
    return FALSE;
}

void* NV_API_CALL nv_i2c_add_adapter(nv_state_t *nv, NvU32 port)
{
    return NULL;
}

#endif // (defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE))

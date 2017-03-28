/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 2000-2001 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */

#define  __NO_VERSION__
#define NV_DEFINE_REGISTRY_KEY_TABLE
#include "nv-misc.h"
#include "os-interface.h"
#include "nv-linux.h"
#include "nv-reg.h"

static void parse_option_string(nv_stack_t *sp)
{
    unsigned int i, len;
    nv_parm_t *entry;
    char *option_string = NULL;
    char *ptr, *mod;
    char *token;
    char *name, *value;
    NvU32 data;

    if (NVreg_RegistryDwords != NULL)
    {
        len = strlen(NVreg_RegistryDwords) + 1;

        if (os_alloc_mem((void **)&option_string, len) != RM_OK)
            return;

        ptr = NVreg_RegistryDwords;
        mod = option_string;

        while (*ptr != '\0')
        {
            if (!isspace(*ptr)) *mod++ = *ptr;
            ptr++;
        }
        *mod = '\0';

        ptr = option_string;

        while ((token = strsep(&ptr, ";")) != NULL)
        {
            if (!(name = strsep(&token, "=")) || !strlen(name))
                continue;
            if (!(value = strsep(&token, "=")) || !strlen(value))
                continue;
            if (strsep(&token, "=") != NULL)
                continue;

            data = (NvU32)simple_strtoul(value, NULL, 0);

            for (i = 0; (entry = &nv_parms[i])->name != NULL; i++)
            {
                if (strcmp(entry->name, name) == 0)
                    break;
            }

            if (!entry->name)
                rm_write_registry_dword(sp, NULL, "NVreg", name, data);
            else
                *entry->data = data;
        }

        os_free_mem(option_string);
    }
}

RM_STATUS NV_API_CALL os_registry_init(void)
{
    nv_parm_t *entry;
    unsigned int i;
    nv_stack_t *sp = NULL;

    NV_KMEM_CACHE_ALLOC_STACK(sp);
    if (sp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate stack!\n");
        return RM_ERR_NO_FREE_MEM;
    }

    if (NVreg_RmMsg != NULL)
    {
        rm_write_registry_string(sp, NULL, "NVreg",
                "RmMsg", NVreg_RmMsg, strlen(NVreg_RmMsg));
    }

    parse_option_string(sp);

    for (i = 0; (entry = &nv_parms[i])->name != NULL; i++)
    {
        rm_write_registry_dword(sp, NULL, entry->node, entry->name, *entry->data);
    }

    NV_KMEM_CACHE_FREE_STACK(sp);

    return RM_OK;
}

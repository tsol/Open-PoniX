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

#define NV_DEV_NAME "nvidia"

int nv_register_chrdev(int major, struct file_operations *fops)
{
    int status;

    status = register_chrdev(major, NV_DEV_NAME, fops);
    if (status < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: register_chrdev() failed!\n");
        return status;
    }

    return 0;
}

void nv_unregister_chrdev(int major)
{
    unregister_chrdev(major, NV_DEV_NAME);
}

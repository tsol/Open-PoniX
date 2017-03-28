/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 1999-2001 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */

#ifndef _OS_AGP_H_
#define _OS_AGP_H_

#define  __NO_VERSION__
#include "nv-misc.h"
#include "nv-linux.h"

RM_STATUS  KernInitAGP        (nv_stack_t *, nv_state_t *, NvU64 *, NvU64 *);
RM_STATUS  KernTeardownAGP    (nv_stack_t *, nv_state_t *);
RM_STATUS  KernAllocAGPPages  (nv_stack_t *, nv_state_t *, NvU32, void **, NvU32 *);
RM_STATUS  KernFreeAGPPages   (nv_stack_t *, nv_state_t *, void *);
RM_STATUS  KernLoadAGPPages   (nv_state_t *, nv_alloc_t *, void *);

#endif

/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 1993-2011 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */

#ifndef _RMRETVAL_H_
#define _RMRETVAL_H_

#include "nvtypes.h"

/*
 * ---------------------------------------------------------------------------
 *
 * Error codes.
 *
 * ---------------------------------------------------------------------------
 */

typedef NvU32 RM_STATUS;

#define RM_OK                           0x00000000
#define RM_ERROR                        0xFFFFFFFF

#define RM_ERR_CANT_CREATE_CLASS_OBJS   0x00000001
#define RM_ERR_DMA_IN_USE               0x00000002
#define RM_ERR_DMA_MEM_NOT_LOCKED       0x00000003
#define RM_ERR_DMA_MEM_NOT_UNLOCKED     0x00000004
#define RM_ERR_DUAL_LINK_INUSE          0x00000005
#define RM_ERR_FIFO_BAD_ACCESS          0x00000006
#define RM_ERR_GPU_NOT_FULL_POWER       0x00000007
#define RM_ERR_ILLEGAL_ACTION           0x00000008
#define RM_ERR_ILLEGAL_OBJECT           0x00000009
#define RM_ERR_INSERT_DUPLICATE_NAME    0x0000000A
#define RM_ERR_INSUFFICIENT_RESOURCES   0x0000000B
#define RM_ERR_INVALID_ADDRESS          0x0000000C
#define RM_ERR_INVALID_ARGUMENT         0x0000000D
#define RM_ERR_INVALID_BASE             0x0000000E
#define RM_ERR_INVALID_CHANNEL          0x0000000F
#define RM_ERR_INVALID_CLASS            0x00000010
#define RM_ERR_INVALID_CLIENT           0x00000011
#define RM_ERR_INVALID_COMMAND          0x00000012
#define RM_ERR_INVALID_DATA             0x00000013
#define RM_ERR_INVALID_DEVICE           0x00000014
#define RM_ERR_INVALID_DMA_SPECIFIER    0x00000015
#define RM_ERR_INVALID_EVENT            0x00000016
#define RM_ERR_INVALID_FLAGS            0x00000017
#define RM_ERR_INVALID_FUNCTION         0x00000018
#define RM_ERR_INVALID_HEAP             0x00000019
#define RM_ERR_INVALID_INDEX            0x0000001A
#define RM_ERR_INVALID_LIMIT            0x0000001B
#define RM_ERR_INVALID_METHOD           0x0000001C
#define RM_ERR_INVALID_OBJECT           0x0000001D
#define RM_ERR_INVALID_OBJECT_BUFFER    0x0000001E
#define RM_ERR_INVALID_OBJECT_ERROR     0x0000001F
#define RM_ERR_INVALID_OBJECT_HANDLE    0x00000020
#define RM_ERR_INVALID_OBJECT_OLD       0x00000021
#define RM_ERR_INVALID_OBJECT_PARENT    0x00000022
#define RM_ERR_INVALID_OFFSET           0x00000023
#define RM_ERR_INVALID_OWNER            0x00000024
#define RM_ERR_INVALID_PARAM_STRUCT     0x00000025
#define RM_ERR_INVALID_POINTER          0x00000026
#define RM_ERR_INVALID_READ             0x00000027
#define RM_ERR_INVALID_STATE            0x00000028
#define RM_ERR_INVALID_WRITE            0x00000029
#define RM_ERR_INVALID_XLATE            0x0000002A
#define RM_ERR_IRQ_NOT_FIRING           0x0000002B
#define RM_ERR_MULTIPLE_MEMORY_TYPES    0x0000002C
#define RM_ERR_NO_FREE_FIFOS            0x0000002D
#define RM_ERR_NO_FREE_MEM              0x0000002E
#define RM_ERR_NOT_SUPPORTED            0x0000002F
#define RM_ERR_OBJECT_NOT_FOUND         0x00000030
#define RM_ERR_OBJECT_TYPE_MISMATCH     0x00000031
#define RM_ERR_OPERATING_SYSTEM         0x00000032
#define RM_ERR_OTHER_DEVICE_FOUND       0x00000033
#define RM_ERR_CALLBACK_NOT_SCHEDULED   0x00000034
#define RM_ERR_PAGE_TABLE_NOT_AVAIL     0x00000035
#define RM_ERR_PROTECTION_FAULT         0x00000036
#define RM_ERR_STATE_IN_USE             0x00000037
#define RM_ERR_TIMEOUT                  0x00000038
#define RM_ERR_BUFFER_TOO_SMALL         0x00000039
#define RM_ERR_NO_SUCH_DOMAIN           0x0000003A
#define RM_ERR_I2C_ERROR                0x0000003B
#define RM_ERR_FREQ_NOT_SUPPORTED       0x0000003C
#define RM_ERR_INVALID_REQUEST          0x0000003D
#define RM_ERR_MORE_PROCESSING_REQUIRED 0x0000003E
#define RM_ERR_MISSING_TABLE_ENTRY      0x0000003F
#define RM_ERR_NO_INTR_PENDING          0x00000040
#define RM_ERR_INSUFFICIENT_PERMISSIONS 0x00000041
#define RM_ERR_TIMEOUT_RETRY            0x00000042
#define RM_ERR_NOT_READY                0x00000043
#define RM_ERR_BROKEN_FB                0x00000044
#define RM_ERR_GPU_IS_LOST              0x00000045
#define RM_ERR_GPU_IN_FULLCHIP_RESET    0x00000046
#define RM_ERR_INVALID_LOCK_STATE       0x00000047
#define RM_ERR_INSUFFICIENT_POWER       0x00000048
#define RM_ERR_REJECTED_VBIOS           0x00000049
#define RM_ERR_MEMORY_TRAINING_FAILED   0x0000004A
#define RM_ERR_COMPRESSION_ERROR        0x0000004B
#define RM_ERR_I2C_SPEED_TOO_HIGH       0x00000050
#define RM_ERR_INVALID_IRQ_LEVEL        0x00000051
#define RM_ERR_BUSY_RETRY               0x00000052

#define RM_ERR_INVALID_PATH             0x00000060
#define RM_ERR_CANT_HOT_SWITCH          0x00000061
#define RM_ERR_MISMATCHED_TARGET        0x00000062
#define RM_ERR_OUT_OF_RANGE             0x00000063

// Warnings:
#define RM_WARN_NULL_OBJECT             0x00000100
#define RM_WARN_INCORRECT_PERFMON_DATA  0x00000101

#endif /* _RMRETVAL_H_ */

/* _NVRM_COPYRIGHT_BEGIN_
 *
 * Copyright 2011 by NVIDIA Corporation.  All rights reserved.  All
 * information contained herein is proprietary and confidential to NVIDIA
 * Corporation.  Any use, reproduction, or disclosure without the written
 * permission of NVIDIA Corporation is prohibited.
 *
 * _NVRM_COPYRIGHT_END_
 */

#ifndef _NV_ERROR_H_
#define _NV_ERROR_H_

const char *nvidia_error_strings[] = {
    "Unknown Error",
    "DMA Engine Error (FIFO Error 1)",
    "DMA Engine Error (FIFO Error 2)",
    "DMA Engine Error (FIFO Error 3)",
    "DMA Engine Error (FIFO Error 4)",
    "DMA Engine Error (FIFO Error 5)",
    "DMA Engine Error (FIFO Error 6)",
    "DMA Engine Error (FIFO Error 7)",
    "DMA Engine Error (FIFO Error 8)",
    "Graphics Engine Error (GR Error 1)",
    "Graphics Engine Error (GR Error 2)",
    "Graphics Engine Error (GR Error 3)",
    "Graphics Engine Error (GR Error 4)",
    "Graphics Engine Error (GR Error 5)",
    "Fake Error",
    "Display Engine Error (CRTC Error 1)",
    "Display Engine Error (CRTC Error 2)",
    "Display Engine Error (CRTC Error 3)",
    "Bus Interface Error (BIF Error)",
    "Client Reported Error",
    "Video Engine Error (MPEG Error)",
    "Video Engine Error (ME Error)",
    "Video Engine Error (VP Error 1)",
    "Error Reporting Enabled",
    "Graphics Engine Error (GR Error 6)",
    "Graphics Engine Error (GR Error 7)",
    "DMA Engine Error (FIFO Error 9)",
    "Video Engine Error (VP Error 2)",
    "Video Engine Error (VP2 Error)",
    "Video Engine Error (BSP Error)",
    "Access Violation Error (MMU Error 1)",
    "Access Violation Error (MMU Error 2)",
    "DMA Engine Error (PBDMA Error)",
    "Security Engine Error (SEC Error)",
    "Video Engine Error (MSVLD Error)",
    "Video Engine Error (MSPDEC Error)",
    "Video Engine Error (MSPPP Error)",
    "Graphics Engine Error (FECS Error 1)",
    "Graphics Engine Error (FECS Error 2)",
    "DMA Engine Error (CE Error 1)",
    "DMA Engine Error (CE Error 2)",
    "DMA Engine Error (CE Error 3)",
    "Video Engine Error (VIC Error)",
    "Verification Error",
    "Access Violation Error (MMU Error 3)",
    "Operating System Error (OS Error 1)",
    "Operating System Error (OS Error 2)",
    "Video Engine Error (MSENC Error)",
    "ECC Error (DBE Error)",
    "Power State Locked",
    "Power State Event (RC Error)",
    "Power State Event (Stress Test Error)",
    "Power State Event (Thermal Event 1)",
    "Power State Event (Thermal Event 2)",
    "Power State Event (Power Event)",
    "Power State Event (Thermal Event 3)",
    "Display Engine Error (EVO Error)",
    "FB Interface Error (FBPA Error 1)",
    "FB Interface Error (FBPA Error 2)"
};

#endif /* _NV_ERROR_H_ */

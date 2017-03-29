/***************************************************************************
 * Copyright (C) 2003-2006 by XGI Technology, Taiwan.			   *
 *									   *
 * All Rights Reserved.							   *
 *									   *
 * Permission is hereby granted, free of charge, to any person obtaining   *
 * a copy of this software and associated documentation files (the	   *
 * "Software"), to deal in the Software without restriction, including	   *
 * without limitation on the rights to use, copy, modify, merge,	   *
 * publish, distribute, sublicense, and/or sell copies of the Software,	   *
 * and to permit persons to whom the Software is furnished to do so,	   *
 * subject to the following conditions:					   *
 *									   *
 * The above copyright notice and this permission notice (including the	   *
 * next paragraph) shall be included in all copies or substantial	   *
 * portions of the Software.						   *
 *									   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	   *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF	   *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND		   *
 * NON-INFRINGEMENT.  IN NO EVENT SHALL XGI AND/OR			   *
 * ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,	   *
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,	   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER	   *
 * DEALINGS IN THE SOFTWARE.						   *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xgi.h"
#include "xgi_regs.h"
#include "xgi_video.h"
#include "xg47_video.h"
#include "xg47_videohw.h"
#include "xgi_misc.h"

/*
 * HW Regs Setting from blow function
 */
void XG47WaitForSync(ScrnInfoPtr pScrn)
{
    register vgaHWPtr pVgaHW = VGAHWPTR(pScrn);

    while (!(pVgaHW->readST01(pVgaHW) & 0x8)) {};
    while (pVgaHW->readST01(pVgaHW) & 0x8) {};
}

void XG47ResetVideo(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;

    /* Unprotect registers */
    vAcquireRegIOProtect(pXGI);

    XG47HwEnableVideo(pScrn);
    /* Wait vertical blank start signal. */
    XG47WaitForSync(pScrn);

    /* hStart: 0x2414 hEnd: 0x2416 vStart: 0x2418 vEnd: 0x241a */
    OUTW(0x2414, INW(0x2414) & 0xf000);
    OUTW(0x2416, INW(0x2416) & 0xf000);
    OUTW(0x2418, INW(0x2418) & 0xf000);
    OUTW(0x241a, INW(0x241a) & 0xf000);

    /* rowByte: 0x240c lineBufLevel: 0x2410 lineBufThreshold: 0x2412 */
    OUTW(0x240c, INW(0x240c) & 0xf000);
    OUTW(0x2410, INW(0x2410) & 0xfc00);
    OUTB(0x2412, (INB(0x2412) & 0x80) + 8);

    /*
     * 24xx    46-44   R/W Window1 Source Key Lower Bound
     *                  <23:0>:         W1_KEYDATAL<23:0>
     *                                  Window1 Source Key Lower Bound
     */
    OUTW(0x2444, 0);
    OUTW(0x2446, 0xffff);

    /* Color key mask */
    OUTDW(0x243c, 0xff);

    /* make sure vertical mirror is off
     * 24xx    2A  R/W  <7>:            Reserved
     *                  <6:4>:          W1_HDEADJ<2:0>
     *                                      Window1 HDE adjustment
     *                                      * The default value is 0H
     *                  <3>:            W1_VMIRR
     *                                      1: window1 vertical mirror enable
     *                                      0: window1 vertical mirror disable
     *                  <2>:            W1_HMIRR
     *                                      1: window1 vertical mirror enable
     *                                      0: window1 vertical mirror disable
     *                  <1>:            W1_VINTEN
     *                                      Window1 vertical interpolation disable
     *                                      1: Disable
     *                                      *0: Enable
     *                  <0>:           W1_HINTEN
     *                                      Window1 horizontal interpolation disable
     *                                      1: Disable
     *                                      *0: Enable
     */
    OUTB(0x242a, INB(0x242a) & ~0x0c);

    /*
     * 24xx    2B  R/W              Window1 Sharp parameter
     *                  <7:0>:          W1_SHARP_PAR<7:0>
     *                                  Window1 Sharp parameter
     */
    OUTB(0x242b, 0x00);/*4*/
    /*
     * 3D4/3D5 B1  R/W  <7:0>:      LBUF0_MAX_RLEN <7:0>
     *                                  Maximum allowed length (1 ~ 256 DQWORD)
     *                                  for each read request from WIN1.
     */
    OUT3X5B(0xb1, 0x00);
    /*
     * 3CE/3CF 82  R/W              Video BIST Error Output 1
     *                  <5:0>:          Reserved
     *                  <1>:            W2_BIST_ON
     *                                      1: window2 line buffer bits-on enable
     *                                      0: window2 line buffer bits-on disable
     *                  <0>:            W1_BIST_ON
     *                                      1: window1 line buffer bits-on enable
     *                                      0: window1 line buffer bits-on disable
     */
    OUT3CFB(0x82, IN3CFB(0x82) & ~0x01);

    /*
     * frame buffer less mode enable
     */
    OUTB(0x2403, INB(0x2403) & ~0x80);
    /*
     * 24xx 7C-7B   R/W <15>:       LODEN
     *                                  Enable Window1 LCD Overdrive function
     *                                  1: Enable *0: Disable
     *                  <14:12>:    Reserved
     *                  <11>:       LOD_DIV2
     *                                  Overdrive LUT content value divided by 2.
     *                                  i.e. {Content, 2'b0} => {1'b0,Content,1'b0}
     *                  <10>:       LOD_LUT_WEB
     *                                  Overdrive LUT Write Enable
     *                                  1: Read Only *0: Write only
     *                  <9:0>:      LOD_LUT_AIN<9:0>
     *                                  Overdrive LUT Address Write Bus. Higher two bits are used to
     *                                  select one of four 32x32 LUT.
     */
    OUTB(0x247c, INB(0x247c) & ~0x80);

    /*
     * 24xx 50      R/W <7>:    EDGEEN
     *                              Window1 Edge recovery algorithm control
     *                                  1: Enable *0: Disable
     *                  <6:3>:      Reserved
     *                  <2>:        SYNCSEL
     *                  <1>:        LNBFTOG
     *                                  Line toggle for line buffer
     *                                  1: Toggle(Reversed)
     *                                  *0: Normal
     *                  <0>:        CIRDTV
     *                              CCIR-DTV input video data format control
     *                                  1: DTV Format
     *                                  *0: CCIR Format
     */
    OUTB(0x2450, INB(0x2450) | 0x04);

    /*
     * 3CE/3CF 80  R/W  <7>:         Window1 STADD latch
     *                                  1: enable 0: disable
     *                  <6:1>:       Reserved
     *                  <0>:         ENVIDEO
     *                                  1: Window1 enable 0: Window1 disable
     */
    OUT3CFB(0x80, IN3CFB(0x80) | 0x80);

    /*
     * 24xx    29  R/W  <7:5>:      Reserved
     *                  <4>:        W1_CSCPASS
     *                                  1: window1 RGB format
     *                                  *0: window1 YUV format
     *                  <3>:        W1_HSCBEN
     *                                  1: Window1 HSCB enable
     *                                  *0: Window1 HSCB bypass
     *                  <2:0>:      W1_MD<2:0>
     *                                  Window1 window mode
     *                                  *000: YUV non-planar, LBUF mode
     *                                  001: YUV planar but not NV12, LBUF mode
     *                                  010: YUV non-planar or RGB, FIFO_mode
     *                                  011: NV12, LBUF_mode
     *                                  100: RGB LBUF mode
     */
    if (pXGIPort->isEnableHSB)
    {
        /* Enable HSB */
        OUTB(0x2429, INB(0x2429) & ~0x08);
    }
    else
    {
        /* Disable HSB */
        OUTB(0x2429, INB(0x2429) | 0x08);
    }

    XG47HwSetColorKey(pScrn);

    pXGIPort->id = 1;   /* in order for setting Win2 colorkey */
    XG47HwSetColorKey(pScrn);
    pXGIPort->id = 0;

    XG47HwSetVideoContrast(pScrn);
    XG47HwSetVideoParameters(pScrn);
}

Bool XG47HwIsFlipDone(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD8       data = 0;

    if (pXGIPort->id)
    {
        data = IN3CFB(0x81);
    }
    else
    {
        data = IN3CFB(0x80);
    }
    return !((data & 0x1) && (data & 0x80));
}

void XG47HwSetToggle(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;

    /*
     * 3CE/3CF 80  R/W  <7>:         Window1 STADD latch
     *                                  1: enable 0: disable
     *                  <6:1>:       Reserved
     *                  <0>:         ENVIDEO
     *                                  1: Window1 enable 0: Window1 disable
     */

    CARD8  indexToggle = pXGIPort->id > 0 ? 0x81 : 0x80; /* index of the reg 0x3cf */
    OUT3CFB(indexToggle, IN3CFB(indexToggle) | 0x81);
}

void XG47HwDisableDeinterlace(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;

    if (pXGIPort->id)
    {
        return;
    }
    OUTB(0x2458, 0);
}

Bool XG47HwIsDetectionDone(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    if (INDW(0x22e0) & 0x80000000)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

#if DBG
CARD32 XG47HwGetFieldMotion(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    return (INDW(0x22e0) & 0x3ffff);
}

CARD32 XG47HwGetFrameMotion(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    return (INDW(0x22e4) & 0x3ffff);
}
#endif

Bool XG47HwDetectedMovie(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    if (INDW(0x22e0) & 0x200000)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

Bool XG47HwDetectedMovieParity(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    if (INDW(0x22e0) & 0x80000)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

Bool XG47HwDetectedProgressive(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    if (INDW(0x22e0) & 0x100000)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void XG47Hw3To2DetectionDisable(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    if (INDW(0x2550) & 0x4000000)
    {
        OUTDW(0x2550, (INDW(0x2550) & (~0x4000000)));
    }
}

void XG47Hw3To2DetectionEnable(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    if (!(INDW(0x2550) & 0x4000000))
    {
        OUTDW(0x2550, (INDW(0x2550) | 0x4000000));
    }
}

void XG47Hw3To2DetectionSetSTAddr(ScrnInfoPtr pScrn,
                                  unsigned long MVDETCURR,
                                  unsigned long MVDETPFD,
                                  unsigned long MVDETPFR)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    OUTDW(0x2540, ((INDW(0x2540) & 0xfe000000 ) | ((CARD32)MVDETCURR >> 3)));
    OUTDW(0x2544, ((INDW(0x2544) & 0xfe000000 ) | ((CARD32)MVDETPFD  >> 3)));
    OUTDW(0x2548, ((INDW(0x2548) & 0xfe000000 ) | ((CARD32)MVDETPFR  >> 3)));
}
#if 0
void XG47Hw3To2DetectionInitPara(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    const CARD32 FRAME_MOTION_THRESH         = 6;
    const CARD32 FIELD_MOTION_THRESH         = 6;
    const CARD32 MIN_FRAME_THRESH            = 16;
    const CARD32 MAX_FRAME_THRESH            = 50000;
    const CARD32 MIN_FIELD_THRESH            = 16;
    const CARD32 MAX_FIELD_THRESH            = 50000;
    const CARD32 CONTENT_CHANGE_THRESH       = 50000;
    const CARD32 SCENE_CHANGE_THRESH         = 50000;

    const CARD32 FRAME_WEIGHTED_AVERAGE_NUM  = 2;
    const CARD32 FIELD_WEIGHTED_AVERAGE_NUM  = 0; /*2;*/
    const CARD32 FRAME_MOVIE_NUM             = 1; /* for quick movie mode recovery, but "0" will cause pendulum scene error */
    const CARD32 FIELD_MOVIE_NUM             = 3; /*1;*/
    const CARD32 STATIC_PATTERN_NUMBER       = 0;

    const CARD32 FRAME_ADAP_THRESH_FACTOR    = 3;
    const CARD32 FIELD_ADAP_THRESH_FACTOR    = 0; /*6;*/
    const CARD32 FRAME_ENTER_ENABLE          = 1;
    const CARD32 FIELD_ENTER_ENABLE          = 0;
    const CARD32 FRAME_QUIT_ENABLE           = 1;
    const CARD32 FIELD_QUIT_ENABLE           = 1; /* combine with FIELD_QUIT_TIGHTER=0,*/
                                                 /* fix Faroudja 3:2 test error when moving block flips over from bottom to top*/
    const CARD32 FIELD_QUIT_RATIO            = 3; /*0;*/
    const CARD32 FRAME_QUIT_TIGHTER          = 1;
    const CARD32 FIELD_QUIT_TIGHTER          = 0;

    /* ??? */
    const CARD32 DISPLAY_QUIT_ENABLE         = 0;
    const CARD32 CAP_TIMING_INVERT           = 0;
    const CARD32 SEGMENT                     = 0;

    CARD32 dw2554, dw2558, dw255c, dw2560, dw2564;

    dw2554 =    (m_rclSrc.left & 0xfff)             |
                ((m_rclSrc.right & 0xfff) << 12)    |
                (FRAME_MOTION_THRESH << 24)         |
                (FRAME_WEIGHTED_AVERAGE_NUM << 30);

    dw2558 =    ((m_rclSrc.top + 2) & 0xfff)              |
                (((m_rclSrc.bottom - 2) & 0xfff) << 12)   |
                (FIELD_MOTION_THRESH << 24)         |
                (FIELD_WEIGHTED_AVERAGE_NUM << 30);

    dw255c =    (MAX_FRAME_THRESH)              |
                (MIN_FRAME_THRESH << 16)        |
                (FRAME_ADAP_THRESH_FACTOR << 26)|
                (FIELD_ADAP_THRESH_FACTOR << 29);

    dw2560 =    (CONTENT_CHANGE_THRESH)         |
                (MIN_FIELD_THRESH << 18)        |
                (SEGMENT << 28)                 |
                (CAP_TIMING_INVERT << 30);

    dw2564 =    (SCENE_CHANGE_THRESH)           |
                (FRAME_MOVIE_NUM << 18)         |
                (FIELD_MOVIE_NUM << 20)         |
                (FIELD_QUIT_RATIO << 22)        |
                (FRAME_ENTER_ENABLE << 24)      |
                (FIELD_ENTER_ENABLE << 25)      |
                (FRAME_QUIT_ENABLE << 26)       |
                (FIELD_QUIT_ENABLE << 27)       |
                (DISPLAY_QUIT_ENABLE << 28)     |
                (FRAME_QUIT_TIGHTER << 29)      |
                (FIELD_QUIT_TIGHTER << 30);

    OUTDW(0x2554, dw2554);
    OUTDW(0x2558, dw2558);
    OUTDW(0x255c, dw255c);
    OUTDW(0x2560, dw2560);
    OUTDW(0x2564, dw2564);

    OUTB(0x2569, ((INB(0x2569) & 0xf1) | 0xc));
}
#endif
void XG47Hw3To2DetectionSetPara(ScrnInfoPtr pScrn, Bool isTopFirst)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    if (isTopFirst)
    {
        OUTB(0x2567, INB(0x2567) & 0x7f);
    }
    else
    {
        OUTB(0x2567, INB(0x2567) | 0x80);
    }
}

void XG47HwTriggerDetection(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    OUTDW(0x22e0, INDW(0x22e0) | 0x80000000);
}

void XG47HwClearYUV420To422(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    OUTDW(0x2550, INDW(0x2550) & 0xff800000); /* clear [22:0] */
}
#if 0
void XG47HwOverDriveLoadLUT(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    /* Disable Overdrive and disable write to LUT*/
    OUTB(0x247C, ((INB(0x247C) & 0x7f) | 0x4));

    for (CARD16 i = 0; i < 8 * 32 * 4; i++)
    {
        /* Overdrive LUT Data Write Bus, each time fill in 4 cells
         * 2478<23:0>:          LOD_LUT_DIN
	 *
         * Overdrive LUT Address Write Bus. Higher two bits are used to select one of four 32x32 LUT.
         * 247B<9:0>:           LOD_LUT_AIN
	 */

        CARD32 dw2478 =  (OverDriveArray[4*i])           |
                        (OverDriveArray[4*i + 1] << 6)  |
                        (OverDriveArray[4*i + 2] << 12) |
                        (OverDriveArray[4*i + 3] << 18);

        OUTB(0x247B, (CARD8)(i & 0xff));
        OUTB(0x247C, (INB(0x247C) & ~0x3) | (CARD8)(i >> 8));
        OUTB(0x2478, (CARD8)(dw2478 & 0xff));
        OUTB(0x2479, (CARD8)((dw2478 >> 8) & 0xff));
        OUTB(0x247a, (CARD8)((dw2478 >> 16) & 0xff));

        /* Overdrive LUT Write Enable
         * 247B<10>:            LOD_LUT_WEB
         *                      1: Read Only
         *                     *0: Write only
	 */
        OUTB(0x247C, INB(0x247C) & ~0x4);

        /* Disable write to LUT */
        OUTB(0x247C, INB(0x247C) | 0x4);

    }
}

void XG47HwOverDriveSetPara(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    if (!m_pOverDriveMem)
        return;

    /* Window 1 LCD Overdrive Write Back Frame Buffer Start Address
     * 2474<24:0>:          LOD_STADD
     */
    unsigned long LOD_addr = (m_pOverDriveMem->GetLogicAddr() -
	(DWORD_PTR)m_ppdev->m_pbVideoMemBase) >> 4;

    OUTDW(0x2474, (INDW(0x2474) & 0xff000000) | ((CARD32)LOD_addr & 0xffffff));

    /* Overdrive LUT content value divided by 2.
     * 247B<11>:            LOD_DIV2
     *                  i.e. {Content, 2’b0} => {1’b0,Content,1’b0}
     */
    OUTB(0x247C, INB(0x247C) & ~0x8);

    /* 247B<14:12>          Reserved
     *
     * Overdrive Read Buffer Water Mark
     * 247D<3:0>            RBUF_WATERMARK
     * 247D<7:4>            WBUF_WATERMARK
     */
    OUTB(0x247D, 0x8C);
}

/* Enable Window1 LCD Overdrive function
 * 247B<15>:            LODEN
 *                      1: Enable
 *                     *0: Disable
 */
void  XG47HwOverDriveEnable(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    if (!m_Id && !m_bOverDriveEnabled)
    {
        m_ppdev->OUTB(0x247c, m_ppdev->INB(0x247c) | 0x80);
        m_bOverDriveEnabled = TRUE;
    }
}

void  XG47HwOverDriveDisable()
{
    if (!m_Id && m_bOverDriveEnabled)
    {
        CARD16 wVsync1, wVsync2;

        /* Can't turn it off at Vsync Start,
         From Qing Yu: Simulation shows it causes h/w hang */
        if (IsExpansionMode())
        {
            CARD8 save3CF_30 = m_ppdev->IN3CFB(0x30);

            /* Enable shadow */
            m_ppdev->OUT3CFB(0x30, save3CF_30 | 0x40);

            /* Read Vsync Start from register */
            wVsync1 = (CARD16)(m_ppdev->IN3X5B(0x10))                 |
                      (CARD16)((m_ppdev->IN3X5B(0x07) & 0x04) << 6)   |
                      (CARD16)((m_ppdev->IN3X5B(0x07) & 0x80) << 2)   |
                      (CARD16)((m_ppdev->IN3X5B(0x27) & 0x20) << 5);

            /* Restore 3CF.30 */
            m_ppdev->OUT3CFB(0x30, save3CF_30);

            /* Adjust according to LCD size and mode */
            wVsync1 -= ((CARD16)m_ppdev->m_pHWDev->m_DeviceLimits.dwLCDHeight -
		(CARD16)m_ppdev->m_cyScreen);
            wVsync1 += 2;
            wVsync2 = wVsync1 + 1;
        }
        else
        {
            /* Read Vsync Start from register */
            wVsync1 = (CARD16)(m_ppdev->IN3X5B(0x10))                 |
                      (CARD16)((m_ppdev->IN3X5B(0x07) & 0x04) << 6)   |
                      (CARD16)((m_ppdev->IN3X5B(0x07) & 0x80) << 2)   |
                      (CARD16)((m_ppdev->IN3X5B(0x27) & 0x20) << 5);
            wVsync1 += 2;
            wVsync2 = wVsync1;
        }

        CARD16 wVStart = m_ppdev->INW(0x2418) & 0xfff;
        CARD16 wVEnd = m_ppdev->INW(0x241a) & 0xfff;
        CARD16 wScan = m_ppdev->INW(0x24f8) & 0xfff;

        if ((wScan == wVsync1) || (wScan == wVsync2))
        {
            return;
        }

        /* turn it off when  scan line out of W1 VSTART <-> VEND range, otherwise h/w hang. */
        if ((wScan < wVStart) || (wScan > wVEnd))
        {
            m_ppdev->OUTB(0x247c, m_ppdev->INB(0x247c) & 0x7f);
            m_bOverDriveEnabled = FALSE;
        }
        /* hopefully we'll get chance to turn it off soon */
    }
}
#endif
#define VIDEO_INTERPOLATION_H  0x1
#define VIDEO_INTERPOLATION_V  0x2

void XG47HwEnableEdgePatch(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    /* HW algorithm has some problem, disable this function in XG47 */
    OUTB(0x2413, INB(0x2413) | 0x40);
    /*OUTB(0x2413, INB(0x2413) & ~0x40);*/
}

void XG47HwDisableEdgePatch(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/

    OUTB(0x2413, INB(0x2413) | 0x40);
}

void XG47HwEnableInterpolation(ScrnInfoPtr pScrn, CARD32 mask)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD32      index = pXGIPort->id > 0 ? 0x24aa : 0x242a;

    if (mask & VIDEO_INTERPOLATION_H)
    {
        OUTB(index, INB(index) & 0xfe); /*Horizontal*/
    }
    if (mask & VIDEO_INTERPOLATION_V)
    {
        OUTB(index, INB(index) & 0xfd); /*Vertical*/
    }
}

void XG47HwDisableInterpolation(ScrnInfoPtr pScrn, CARD32 mask)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD32      index = pXGIPort->id > 0 ? 0x24aa : 0x242a;

    if (mask & VIDEO_INTERPOLATION_H)
    {
        OUTB(index, INB(index) | 0x1); /*Horizontal*/
    }
    if (mask & VIDEO_INTERPOLATION_V)
    {
        OUTB(index, INB(index) | 0x2); /*Vertical*/
    }
}

int XG47HwGetHStart(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    /*XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;*/
    CARD16      hTotal, hRetraceStart, hRetraceDelay, hDEODelay; /* HDEO for W1.*/
    CARD16      hStart, hSyncStart;
    CARD16      temp;
    Bool        isInW2 = (pXGI->ovlAttr & OVLST_VIDEO_ON_W2) ? TRUE : FALSE;

    /*
     * W2
     * hStart = (W2.hStart - hDEODelay);
     *
     * CRTC, CRTC Shadow, TV
     * hStart = ( hTotal - hRetraceStart - hRetraceDelay ) * 8 - hDEODelay;
     */
    vAcquireRegIOProtect(pXGI);

    hDEODelay = (INB(0x242a) & 0x70) >> 4; /* D2 HDEO delay. */

    /*
     * For DualView, video on W2 case
     */
    if (isInW2)
    {
        /* W2 HStart */
        hStart = INW(0x2494) & 0x0FFF;

        /* HDE adjust for W1 */
        hStart -= hDEODelay;

        /* It seems the w2 HStart is from 9. */
        hStart -= 9;

        return hStart;
    }

    /*
     * Horizon Retrace Delay(CRTC1): 3d4.5.5..6: have effect at both CRTC1 and CRTC2.
     */
    temp = IN3CFB(0x30);
    OUT3CFB(0x30, temp & ~0x40);
    hRetraceDelay = (IN3X5B(0x05) >> 5 ) & 0x03 ;
    OUT3CFB(0x30, (CARD8)temp);

    if ((pXGI->displayDevice & ST_DISP_LCD) || (IN3CFB(0x5B) & ST_DISP_LCD))
    {
        if (temp & 0x81)    /* Enable CRTC Shadow. */
            OUT3CFB(0x30, temp | 0x40);
        else                /* Enable CRTC. */
            OUT3CFB(0x30, temp & ~0x40);
    }

    {
        hTotal = IN3X5B(0x00) + ((IN3X5B(0x2B) & 1) << 8);
        hRetraceStart = IN3X5B(0x04) + ((IN3X5B(0x2B) & 8) << 5);
    }

    if  ((pXGI->displayDevice & ST_DISP_TV) || (IN3CFB(0x5B) & ST_DISP_TV)) /* TV */
    {
        /* TV don't have Horizon Retrace Delay. */
        hRetraceDelay  = 0;
        hTotal = IN3X5B(0xe0);
        hRetraceStart = IN3X5B(0xe4);
    }
    else    /* CRTC or CRTC Shadow */
    {
        /* 3cf.2f.5 == 0 : HSYNC skew, 1 skew at graphic mode */
        if (IN3CFB(0x2f) & 0x20)
            hTotal++;
    }

    hSyncStart = hRetraceStart + hRetraceDelay;

    hDEODelay = 0;
    hStart  = ((hTotal - hRetraceStart - hRetraceDelay ) << 3) - hDEODelay;
    hStart -= 5;   /* seems that CRTC HStart starts from 5 */

    /* shadow restore */
    if ((pXGI->displayDevice & ST_DISP_LCD) || (IN3CFB(0x5B) & ST_DISP_LCD))
    {
        OUT3CFB(0x30, (CARD8)temp);
    }

    return hStart;
}

int XG47HwGetVStart(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    CARD16      vStart, vSyncStart;
    CARD16      vTotal, vRetraceStart;
    CARD16      data3X5_06, data3X5_07, data3X5_10;
    CARD16      data3X5_27, temp;

    Bool        isInW2 = (pXGI->ovlAttr & OVLST_VIDEO_ON_W2) ? TRUE : FALSE;

    vAcquireRegIOProtect(pXGI);
    /*
     *  For DualView, video on LCD case
     */
    if (isInW2)
    {
        vStart = INW(0x2498) & 0x0FFF;
        return vStart;
    }

    /*
     *  For other cases,
     */
    if ((pXGI->displayDevice & ST_DISP_LCD) || (IN3CFB(0x5B) & ST_DISP_LCD))
    {
        temp = IN3CFB (0x30);
        if (temp & 0x81)    /* Enable CRTC Shadow. */
            OUT3CFB(0x30, temp | 0x40);
        else                /* Enable CRTC. */
            OUT3CFB(0x30, temp & ~0x40);
    }

    /* CRTC or CRTC shadow */
    if (!((pXGI->displayDevice & ST_DISP_TV) || (IN3CFB(0x5B) & ST_DISP_TV)))
    {
        /* Vertical total.   0..7 */
        data3X5_06 = IN3X5B (0x06);

        /* Overflow. Bit 2, 7: Vertical Retrace[8..9]; Bit 0, 5: Vertical Total[8..9] */
        data3X5_07 = IN3X5B (0x07);

        /* Vertical Retrace[0..7] */
        data3X5_10 = IN3X5B (0x10);

        /* High Order. Bit 5: Vertical Retrace[10]; Bit 7: Vertical Total[10] */
        data3X5_27  = IN3X5B (0x27);
    }
    else /* TV */
    {
        /* Vertical Total */
        data3X5_06 = IN3X5B (0xE6);

        /* Overflow. Bit 2, 7: Vertical Retrace 8, 9; Bit 0, 5: Vertical Total 8, 9 */
        data3X5_07 = IN3X5B (0xE7);

        /* Vertical Retrace[0..7] */
        data3X5_10 = IN3X5B (0xF0);

        /* TV only have 10 bit for Vertical Total & Retrace. */
        data3X5_27  = 0;
    }

    /* Calculate timing from register values */
    vTotal = ((data3X5_27  & 0x80) << 3)
              | ((data3X5_07 & 0x20) << 4)
              | ((data3X5_07 & 0x01) << 8)
              | (data3X5_06);

    vRetraceStart = ((data3X5_27  & 0x20) << 5)
                      | ((data3X5_07 & 0x80) << 2)
                      | ((data3X5_07 & 0x04) << 6)
                      | data3X5_10;

    vSyncStart = vRetraceStart;
    vStart = vTotal - vRetraceStart;

    /* Horizon retrace select. 3c5.17.2 == 1: Divided by 2. */
    if (IN3X5B (0x17) & 0x4)
        vStart <<= 1;

    vStart -= 4; /* Seems that CRTC VStart starts from 4 */

    /* shadow restore */
    if ((pXGI->displayDevice & ST_DISP_LCD) || (IN3CFB(0x5B) & ST_DISP_LCD))
    {
        OUT3CFB(0x30, (CARD8)temp);
    }

    return vStart;
}

void XG47HwSetBobbing(ScrnInfoPtr pScrn, Bool isOdd)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD8       data_241e = 0, data_2430 = 0, data_2458 = 0;

    if (pXGIPort->id)
    {
        return;
    }

    data_241e = INB(0x241e) & 0xfc; /* Clear movie mode */
    data_2430 = (INB(0x2430) & 0xf8) | 0x1; /* Odd field shift up 1 line */
    data_2458 = 0xc4; /* BOB, double buffer, TOP first, use STARTADD as TOP/BOTTOM select */

    if (isOdd)
    {
        data_2458 |= 0x10; /*A0 Version has bug*/
    }

    if(data_241e != INB(0x241e))
    {
        OUTB(0x241e, data_241e);
    }

    if(data_2458 != INB(0x2458))
    {
        OUTB(0x2458, data_2458);
    }

    if(data_2430 != INB(0x2430))
    {
        OUTB(0x2430, data_2430);
    }
    pXGI->dvdAttr &= ~DVDST_PLAYBACK_MODE_MASK;
    pXGI->dvdAttr |= DVDST_BOB_MODE;
}

void XG47HwSetWeaving(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD8       data_241e = 0;

    if (pXGIPort->id)
    {
        return;
    }

    data_241e = INB(0x241e) & 0xfc; /* Clear movie mode */

    if(data_241e != INB(0x241e))
    {
        OUTB(0x241e, data_241e);
    }

    /* Weave, use STARTADD as TOP/BOTTOM select */
    if(0x88 != INB(0x2458))
    {
        OUTB(0x2458, 0x88);
    }
    pXGI->dvdAttr &= ~DVDST_PLAYBACK_MODE_MASK;
    pXGI->dvdAttr |= DVDST_WEAVE_MODE;
}

void XG47HwSetDeinterlace(ScrnInfoPtr pScrn, CARD32 preAddr)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    CARD32  data_2458 = 0;

    OUTDW(0x2454, (INDW(0x2454) & 0xe0000000) | (preAddr >> 4));
    OUTB(0x2457,  (INB(0x2457)  & 0x1F) | 0xa0);  /* motion threshold */

    OUTB(0x2430, INB(0x2430) & 0xfb);             /* Odd field is shifted up */
    OUTB(0x2430, (INB(0x2430) & 0xfc) | 0x1);     /* One field is 1 line shifted up */

    data_2458 |= 0xc7;  /* 0x87; */
    OUTB(0x2458,  (INB(0x2458) & 0x41) | data_2458);
}
#if 0
void XG47HwSetMovie(ScrnInfoPtr pScrn,
                    Bool isParityFW,
                    Bool isTopFirst,
                    Bool isOdd,
                    unsigned long refAddr)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    CARD8 data_2458 = 0x81;  /* Movie mode, use STARTADD as TOP/BOTTOM select */
    CARD8 data_241e = (INB(0x241e) & 0xfc) | 0x1;   /* Movie mode */

    if (isParityFW)
    {
        data_241e |= 0x2;
    }

    if (isTopFirst)
    {
        data_2458 |= 0x40;
    }

    if (isOdd)
    {
        data_2458 |= 0x10;
    }

    if(data_2458 != INB(0x2458))
    {
        OUTB(0x2458, data_2458);
    }

    if(data_241e != INB(0x241e))
    {
        OUTB(0x241e, data_241e);
    }

    /* Update refAddr according to the current SRC RECT */
    refAddr += m_pSrcSurfaceGbl->lPitch * 2 * m_rclSrc.top + ((m_rclSrc.left + 7) & ~7) * 2;

    OUTDW(0x2454, (CARD32)(INDW(0x2454) & 0xe0000000 | (refAddr >> 4)));
    pXGI->dvdAttr = pXGI->dvdAttr & ~DVDST_PLAYBACK_MODE_MASK | DVDST_MOVIE_MODE;
}
#endif
void XG47HwSetEdge(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;

    if (pXGIPort->id)
    {
        return;     /* WIN2 doesn't support this feature */
    }

    /* detect 7 directions */
    OUTB(0x2451, (INB(0x2451) & 0xfb) | 0x04);

    /* Edge ExistThreshold, HW recommend to set to 0x30 */
    OUTB(0x2452, 0x30);

    /* MOTION_FACTOR, HW recommand to set to 2'b00 */
    OUTB(0x2459, INB(0x2459) & 0xfc);

    /* enable edge */
    OUTB(0x2450, INB(0x2450) | 0x80);
}

void XG47HwInitYUV420To422(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    OUTDW(0x2550, ((INDW(0x2550) & 0xff000000 ) | (1<<23))); /* enable HW update */

    /* A1 and later, A0 has bug */
    OUTB(0x2569, INB(0x2569) | 0x1); /* update status after H/W process is done */

}

void XG47HwSetYUV420To422(ScrnInfoPtr   pScrn,
                          CARD32        srcID,
                          unsigned long targAddr,
                          Bool          isField,
                          Bool          isOdd)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    CARD32 data = 0;
    CARD32 mask = 0xff800000; /* bit[31:23] keeps no change */
    /*CARD32 picW = (m_YV12SrcPitch + 15) >> 4;*/
    /*CARD32 picH = (m_YV12SrcHeight + 3) >> 2;*/

    /* bit [3:0] */
    if (isField)
    {
        data |= 0x8; /* Field pic YUV420->422 */
        if (!isOdd)
        {
            data |= 0x2;
        }
    }
    else
    {
        data |= 0x4; /* Frame pic YUV420->422 */
    }

    /* bit [ 6:4] DVD frame buffer index (0~7) */
    data |= ((CARD8)srcID << 4);
    /* bit [15:7]  pic height */
    /*data |= ((CARD16)picH << 7);*/
    /* bit [22:16] pic width */
    /*data |= ((CARD8)picW << 16);*/

    OUTDW(0x254c, (CARD32)((INDW(0x254c) & 0xfc000000 ) | (CARD32)targAddr >> 3));
    OUTDW(0x2550, (INDW(0x2550) & mask ) | data);
    OUTB(0x2550,  INB(0x2550)  | 0x01); /* enable */
}


void XG47HwResetYUV420To422(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    OUTB(0x2553, INB(0x2553) | 0x80); /* reset HW */
    OUTB(0x2553, INB(0x2553) & 0x7f);
    /* clear busy status */
    OUTB(0x22e3, INB(0x22e3) & 0x7f);
}

void XG47HwSetConvertedSrc(ScrnInfoPtr pScrn, unsigned long srcAddr)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD32      currAddr;

    /* Src Line Buffer Level, Threshold and PageBreak for YUV422 */
    OUTW(0x2410, (INW(0x2410)  & 0xfc00) | pXGIPort->lineBufLevel);
    OUTB(0x2412, (INB(0x2412)  & 0x80) | 0x8); /*m_LineBufferThresh);*/
    /* Src Row Byte */
    OUTW(0x240c, (INW(0x240c) & 0xf000) | (pXGIPort->rowByteY >> 3));
    /* Format */
    OUTB(0x2428, (INB(0x2428) & 0xc0) | 0x10);
    if (pXGIPort->lineBufLevel > 0x60)
    {
        OUTB(0x2429, (INB(0x2429) & 0xe8 ) | 0x2);
    }
    else
    {
        OUTB(0x2429, INB(0x2429) & 0xe8);
    }

    /*currAddr = (CARD32)srcAddr + m_pSrcSurfaceGbl->lPitch * 2 * m_rclSrc.top +
	((m_rclSrc.left + 7) & ~7) * 2;*/
    OUTDW(0x2400, (CARD32)((INDW(0x2400) & 0xe0000000) | (currAddr >> 4)));
}


void XG47HwMinimizeOverlayWindow(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD16      regHStart, regHEnd, regVStart, regVEnd;

    regHStart = pXGIPort->id > 0 ? 0x2494 : 0x2414;
    regHEnd   = pXGIPort->id > 0 ? 0x2496 : 0x2416;
    regVStart = pXGIPort->id > 0 ? 0x2498 : 0x2418;
    regVEnd   = pXGIPort->id > 0 ? 0x249a : 0x241a;

    OUTB(regHEnd, INB(regHStart));
    OUTB(regHEnd + 1, (INB(regHEnd + 1) & 0xf0) | (INB(regHStart + 1) & 0xf));
    OUTB(regVEnd, INB(regVStart));
    OUTB(regVEnd + 1, (INB(regVEnd + 1) & 0xf0) | (INB(regVStart + 1) & 0xf));
}

void XG47HwSetDestRect(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD16      left, right, top, bottom;
    CARD16      regHStart, regHEnd, regVStart, regVEnd;

    regHStart = pXGIPort->id > 0 ? 0x2494 : 0x2414;
    regHEnd   = pXGIPort->id > 0 ? 0x2496 : 0x2416;
    regVStart = pXGIPort->id > 0 ? 0x2498 : 0x2418;
    regVEnd   = pXGIPort->id > 0 ? 0x249a : 0x241a;

    if (pXGIPort->isSpOverlay)
    {
        pXGIPort->dstBox.x1 += 0xe;
        pXGIPort->dstBox.x2 += 0xa;
        pXGIPort->dstBox.y1 -= 4;
        pXGIPort->dstBox.y2 += 7;
    }
    else
    {
        pXGIPort->dstBox.y2 += 4;
    }

    left   = pXGIPort->hStart + (CARD16)pXGIPort->dstBox.x1;
    right  = pXGIPort->hStart + (CARD16)pXGIPort->dstBox.x2;
    top    = pXGIPort->vStart + (CARD16)pXGIPort->dstBox.y1;
    bottom = pXGIPort->vStart + (CARD16)pXGIPort->dstBox.y2;

    OUTW(regHStart, (INW(regHStart) & 0xf000) | left);
    OUTW(regHEnd,   (INW(regHEnd)   & 0xf000) | right);
    OUTW(regVStart, (INW(regVStart) & 0xf000) | top);
    OUTW(regVEnd,   (INW(regVEnd)   & 0xf000) | bottom);
}

void XG47HwEnableVideo(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD8       b3cf80, b3cf81, b3x5be, b3cfda;
    CARD32      frameBufLess = pXGIPort->id ? 0x2483 : 0x2403;

    /* Unprotect registers */
    vAcquireRegIOProtect(pXGI);

    if (pXGIPort->id)
    {
        /*
         * 24xx    EF  R/W  <7>:            W1_ENDBA
         *                  <6:0>:          Reserved
         */
        OUTB(0x24ef, INB(0x24ef) | 0x80);
    }
    else
    {
        /*
         * 24xx    6F-6C   R/W  <31>:           W1_ENDDBA
         *                      <30>:           SOFTOE
         *                      <29:25>:      Reserved
         *                      <24:0>:        W1_ADDY2<24:0>
         */
        OUTB(0x246f, INB(0x246f) | 0x80);
    }

    /*
     * 3CE/3CF 80  R/W  <7>:        Window1 STADD latch
     *                                  1: enable
     *                                  0: disable
     *                  <6:1>:      Reserved
     *                  <0>:        ENVIDEO
     *                                  1: Window1 enable
     *                                  0: Window1 disable
     */
    b3cf80 = IN3CFB(0x80);
    /*
     * 3CE/3CF 81  R/W  <7>:        Window2 STADD latch
     *                                  1: enable
     *                                  *0: disable
     *                  <6:4>:      DEBUG_SEL
     *                                  Video Engine DFT MUX Select Control
     *                  <3:1>:      Reserved
     *                  <0>:        ENVIDEO2
     *                                  1: Window2 enable
     *                                  *0: Window2 disable
     */
    b3cf81 = IN3CFB(0x81);

    /*
     * 3CE/3CF DA  R/W Surface 0 Band Mode Control
     *                  <7:6>:      Surface0 Band Mode Control
     *                                  10: Surface0 Band 64x64
                                        00: Surface0 Disable Band Mode
     *                  others:     Reserved
     *                  <5>:        Reserved
     *                  <4>:        VECLKOFF
     *                              Disable MCK for Display Engine in order to save power
     *                                  1: Disable 0: Enable
     *                  <3>:        V_MIRROR
     *                              Enable Surface 0 Vertical Mirror.
     *                                  1: Enable  0: Disable.
     *                  <2>:        H_MIRROR
     *                              Enable Surface 0 Horizontal Mirror.
     *                                  1: Enable  0: Disable.
     *                  <1:0>:      PITCH<9:8>
     *                              Surface 0 Band Mode Pitch (Total 10 bits) PITCH<7:0> is located at 3CF.D9
     *                              The default value is 0H
     */
    b3cfda = IN3CFB(0xda);
    b3cfda &= 0xef;     /* MClK enable */
    /*
     * 3D4/3D5 BE  RO
     *              R/W <7>:        WBFEMPTYN
     *                  <6:3>:      Reserved
     *                  <2>:        PCLKEN
     *                                  1: PCLK enable
     *                                  0: PCLK disable
     *                  <1:0>:      Reserved
     */
    b3x5be = IN3X5B(0xbe);
    b3x5be |= 0x04;     /* PCLK enable */

    if (pXGIPort->id)
    {
        b3cf81 |= 0x01;
        OUT3C5B(0xbd, (CARD8)(IN3C5B(0xbd)) & ~0x80);       /* Set W2 use CRTC1 */
    }
    else
    {
        b3cf80 |= 0x01;
        if (pXGI->ovlAttr & OVLST_VIDEO_ON_W2)
        {
            OUT3C5B(0xbd, (CARD8)(IN3C5B(0xbd)) | 0x40);    /* Set W1 use CRTC2 */
        }
        else
        {
            OUT3C5B(0xbd, (CARD8)(IN3C5B(0xbd)) & ~0x40);   /* Set W1 use CRTC1 */
        }
    }

    /* DUMA mode support */
    if (IN3X5B(0x20) & 0x01)
    {
        OUTB(frameBufLess, INB(frameBufLess) | 0x80);
    }
    else
    {
        OUTB(frameBufLess, INB(frameBufLess) & 0x7f);
    }

    /* Turn on video mem clock */
    OUT3CFB(0xda, b3cfda);

    /* Turn on PCLK */
    if (b3x5be != IN3X5B(0xbe))
    {
        /*WaitVRetrace(pXGI, VGA_RETRACE);*/
        XG47WaitForSync(pScrn);
        OUT3X5B(0xbe, b3x5be);
    }
    /*
     * There are two new clock need to be turn on for Video playback
     * Consult Zhang ZhanPeng for detail information
     */
    /*
     * 3C4/3C5 52  R/W VCK control
     *              <7>:            PCLKLB2POL
     *                                  0: normal   1: inverted
     *              <6>:            PCLKLB2EN
     *                                  0: disable  1: enable
     *              <5>:            PCLKLB1POL
     *                                  0: normal   1: inverted
     *              <4>:            PCLKLB1EN
     *                                  0: disable  1: enable
     *              <3>:            LCDCLKPOL
     *                                  0: normal   1: inverted
     *              <2>:            LCDCLKENB
     *                                  0: enable   1: disable
     *              <1>:            PIPCKPOL
     *                                  0: normal   1: inverted
     *              <0>:            PIPCKENB
     *                                  0: enable   1: disable
     */
    OUT3C5B(0x52, IN3C5B(0x52) | 0x50);

    /* Turn on video overlay */
    if (pXGIPort->id && (b3cf81 != IN3CFB(0x81)))
    {
        /*WaitVRetrace(pXGI, VGA_RETRACE);*/
        XG47WaitForSync(pScrn);
        OUT3CFB(0x81, b3cf81);
    }
    else if (b3cf80 != IN3CFB(0x80))
    {
        /*WaitVRetrace(pXGI, VGA_RETRACE);*/
        XG47WaitForSync(pScrn);
        OUT3CFB(0x80, b3cf80);
    }
}

void XG47HwDisableVideo(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD8       b3cf80, b3cf81, b3x5be, b3cfda;
    Bool        isDualView = FALSE;

    if (pXGIPort->id)
    {
        OUTB(0x24ef, INB(0x24ef) & 0x7f);
        OUT3CFB(0x81, IN3CFB(0x81) & 0x7f);
    }
    else
    {
        OUTB(0x246f, INB(0x246f) & 0x7f);
        OUT3CFB(0x80, IN3CFB(0x80) & 0x7f);
    }

    b3cf80 = IN3CFB(0x80);
    b3cf81 = IN3CFB(0x81);
    b3cfda = IN3CFB(0xda);
    b3x5be = IN3X5B(0xbe);

    if (!isDualView)
    {
        b3cfda |= 0x10;  /* MClK disable */
        b3x5be &= ~0x04; /* PClK disable */
    }

    if (pXGIPort->id)
    {
        b3cf81 &= 0xfe;
    }
    else
    {
        b3cf80 &= 0xfe;
    }

    /* Turn off video overlay */
    if (pXGIPort->id && (b3cf81 != IN3CFB(0x81)))
    {
        /*WaitVRetrace(pXGI, VGA_RETRACE);*/
        XG47WaitForSync(pScrn);
        OUT3CFB(0x81, b3cf81);
    }
    else if (b3cf80 != IN3CFB(0x80))
    {
        /*WaitVRetrace(pXGI, VGA_RETRACE);*/
        XG47WaitForSync(pScrn);
        OUT3CFB(0x80, b3cf80);
    }

    /* Turn off PCLK */
    if (b3x5be != IN3X5B(0xbe))
    {
        /*WaitVRetrace(pXGI, VGA_RETRACE);*/
        XG47WaitForSync(pScrn);
        OUT3X5B(0xbe, b3x5be);
    }

    /* Turn off video mem clock */
    OUT3CFB(0xda, b3cfda);
}

void XG47HwTurnOffColorKey(ScrnInfoPtr pScrn, CARD32 flag)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;

    if (flag & (1 << 0))
    {
        OUTW(0x3C4, 0x0054);
        OUTW(0x3C4, 0x0055);
        OUTW(0x3C4, 0x0056);

        /* disable color key */
        OUT3CFB(0x62, (CARD8)((IN3CFB(0x62) & ~0x04)));
    }
    if (flag & (1 << 1))
    {
        OUTW(0x3C4, 0x0064);
        OUTW(0x3C4, 0x0065);
        OUTW(0x3C4, 0x0066);

        /* Disable color key */
        OUT3CFB(0x62, (CARD8)((IN3CFB(0x62) & ~0x08)));
    }
}

int XG47HwSetColorKey(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    INT32       colorKey = pXGIPort->colorKey;
    CARD32      colorKeyMask;

    /* Unprotect registers */
    vAcquireRegIOProtect(pXGI);
    if (pXGI->ovlAttr & OVLST_VIDEO_ON_W2)
    {
        switch (pScrn->bitsPerPixel)
        {
        case 8:
            colorKeyMask = 0x000000ff;
            break;
        case 15:
            colorKeyMask = 0x00007fff;
            break;
        case 16:
            colorKeyMask = 0x00003fff;
            colorKey = ((colorKey & 0xF800) >> 2)
                     | ((colorKey & 0x0780) >> 2) /* this is a hardware bug */
                     | (colorKey & 0x001F);
            break;
        case 24:
        case 32:
            colorKeyMask = 0x00ffffff;
            break;
        default:
            break;
        }

        colorKey &= colorKeyMask;
    }
    else
    {
        switch (pScrn->bitsPerPixel)
        {
        case 8:
            colorKeyMask = 0x000003FC;
            break;
        case 15:
            colorKeyMask = 0x7F3FC;
            colorKey = ((colorKey & 0xf80000) >> 9)
                     | ((colorKey & 0x00f800) >> 6)
                     | ((colorKey & 0x0000f8) >> 3);
            break;
        case 16:
            colorKeyMask = 0xFF3FC;
            break;
        case 24:
        case 32:
            colorKeyMask = 0x3FCFF3FC;
        }

        if (pScrn->bitsPerPixel < 30
         || pScrn->bitsPerPixel == 32)
        {
            colorKey = ((colorKey & 0xff0000) << 6)
                     | ((colorKey & 0xff00) << 4)
                     | ((colorKey & 0xff) << 2);
        }
        colorKey &= colorKeyMask;
    }

    if (pXGIPort->id)
    {
        /* Set Color Key Value */
        OUTDW(0x24B8, (INDW(0x24b8) & 0xC0000000) | colorKey);

        /* Set Color Key Mask */
        OUTDW(0x24BC, (INDW(0x24bC) & 0xC0000000) | colorKeyMask);

        /* Overlay Key type */
        if (pXGIPort->isNoColorKey)
        {
            OUTB(0x24C0, 0xCC); /* Playback key only */
        }
        else
        {
            OUTB(0x24C0, 0xC0); /* Colorkey & Playback key */
        }

        /* Eable color key */
        OUTB(0x24C2, INB(0x24C2) | 0x10);
    }
    else
    {
        /* Set Color Key Value */
        OUTDW(0x2438, (INDW(0x2438) & 0xC0000000) | colorKey);

        /* Set Color Key Mask */
        OUTDW(0x243C, (INDW(0x243C) & 0xC0000000) | colorKeyMask);
        OUTDW(0x2444, INDW(0x2444) & 0xFF000000); /* key lower bound */
        OUTDW(0x2446, INDW(0x2446) | 0x00FFFFFF); /* key high bound */

        /* Overlay Key type */
        if (pXGI->ovlAttr & OVLST_VIDEO_ON_W2)
        {
            OUTB(0x2442, INB(0x2442) | 0x20);
        }
        else
        {
            OUTB(0x2442, INB(0x2442) & ~0x20);
        }

        /*
         * 24xx 40  R/W 1.1 Window1 Play Back Key Mode Function Select
         *              <7:0>:  W1_PBKEYSEL<7:0>
         *                      Play back key mode function select control for window1
         *                      *00: VGA only
         *                      FF: Video port only
         *                      F0: Color key only
         *                      CC: Window (playback) key only
         *                      AA: Chroma key only
         *                      88: Window key & Chroma key
         *                      C0: Chroma key & Window key
         *                      80: Color key & Window key & Chroma key
         *                      * The default value is 00H
         */
        if (pXGIPort->isNoColorKey)
        {
            OUTB(0x2440, 0xCC); /* CRT VIEW: Playback key only */
        }
        else
        {
            OUTB(0x2440, 0xC0); /* CRT VIEW: Colorkey & Playback key */
        }

        /* Eable color key */
        OUTB(0x2442, INB(0x2442) | 0x10);
    }

    return Success;
}

void XG47HwSetStartAddress(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD32      regAddrY, regAddrU, regAddrV;

    regAddrY = pXGIPort->id > 0 ? 0x2480 : 0x2400;
    regAddrU = pXGIPort->id > 0 ? 0x2484 : 0x2404;
    regAddrV = pXGIPort->id > 0 ? 0x2488 : 0x2408;

    OUTDW(regAddrY, (INDW(regAddrY) & 0xe0000000) | (pXGIPort->startAddrY >> 4));

    if (pXGIPort->isPlanar)
    {
        OUTDW(regAddrU, (INDW(regAddrU) & 0xe0000000) | (pXGIPort->startAddrU >> 4));
        OUTDW(regAddrV, (INDW(regAddrV) & 0xe0000000) | (pXGIPort->startAddrV >> 4));
    }
}

void XG47HwSetMirror(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD32      regMirror = pXGIPort->id > 0 ? 0x24aa : 0x242a;

    if (pXGIPort->isHMirror)
    {
        OUTB(regMirror, INB(regMirror) | 0x4);
    }
    else
    {
        OUTB(regMirror, INB(regMirror) & ~(0x4));
    }

    if (pXGIPort->isVMirror)
    {
        OUTB(regMirror, INB(regMirror) | 0x8);
    }
    else
    {
        OUTB(regMirror, INB(regMirror) & ~(0x8));
    }
}

void XG47HwSetFormat(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD32      vidFormat = (CARD32)pXGIPort->vidMode;
    CARD32      regFmt, regMode, regHDE;

    regFmt  = pXGIPort->id > 0 ? 0x24a8 : 0x2428;
    regMode = pXGIPort->id > 0 ? 0x24a9 : 0x2429;
    regHDE  = pXGIPort->id > 0 ? 0x24aa : 0x242a;

    /*
     * bit[2:0] :   WINMD
     * bit[4:3] :   PMDY
     * bit[5]   :   PMDX
     * bit[10:8]:   CMD
     */
    OUTB(regFmt,  (INB(regFmt)  & 0xf8) | (CARD8)((vidFormat >> 8) & 0x07)); /* CMD */
    OUTB(regFmt,  (INB(regFmt)  & 0xc7) | (CARD8)((vidFormat & 0x70) >> 1)); /* PMDX, PMDY */
    if (pXGIPort->id)
    {
        CARD8 WINMD = (CARD8)(vidFormat & 0x07);

        /* RGB LBUF mode on W2 should not excess 0x30 */
        if (!pXGIPort->isYUV && pXGIPort->lineBufLevel > 0x30)
        {
            WINMD = 0x2; /* FIFO mode */
        }
        OUTB(regMode, (INB(regMode) & 0xf8) | WINMD); /* WINMD */
    }
    else
    {
        OUTB(regMode, (INB(regMode) & 0xf8) | (CARD8)(vidFormat & 0x07)); /* WINMD */
    }

    if (pXGIPort->isYUV)
    {
        OUTB(regMode, INB(regMode) & 0xef);   /* CSCPASS */
        OUTB(regHDE,  INB(regHDE)  & 0x8f);   /* HDE=0 if only HSCB enabled to compensate hw bias */
    }
    else
    {
        OUTB(regMode, INB(regMode) | 0x10);   /* CSCPASS */
        OUTB(regHDE,  INB(regHDE)  & 0x8f);   /* HDE = 0 default */
    }
}

void XG47HwSetZoomFactor(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD32      regX, regY;

    regX = pXGIPort->id > 0 ? 0x249C : 0x241C;
    regY = pXGIPort->id > 0 ? 0x24A0 : 0x2420;

    OUTW(regX, (INW(regX) & 0xc000) | pXGIPort->zoomX);
    OUTW(regY, (INW(regY) & 0xc000) | pXGIPort->zoomY);
}

void XG47HwSetRowByteandLineBuffer(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD32      regLevel, regThresh, regBreak, regRBY, regRBUV;

    regLevel  = pXGIPort->id > 0 ? 0x2490 : 0x2410;
    regThresh = pXGIPort->id > 0 ? 0x2492 : 0x2412;
    regBreak  = pXGIPort->id > 0 ? 0x2493 : 0x2413;
    regRBY    = pXGIPort->id > 0 ? 0x248c : 0x240c;
    regRBUV   = pXGIPort->id > 0 ? 0x248e : 0x240e;

    /* Src Line Buffer Level, Threshold and PageBreak */
    OUTW(regLevel,  (INW(regLevel)  & 0xfc00) | pXGIPort->lineBufLevel);
    OUTB(regThresh, (INB(regThresh) & 0x80)   | pXGIPort->lineBufThreshold);
    OUTB(regBreak,  (INB(regBreak)  & 0x3f)   | pXGIPort->lineBufBreak << 6);

    /* Src Row Byte */
    OUTW(regRBY, (INW(regRBY) & 0xf000) | pXGIPort->rowByteY >> 4);

    if (pXGIPort->isPlanar)
    {
        OUTW(regRBUV, (INW(regRBUV) & 0xf000) | pXGIPort->rowByteUV >> 4);
    }
}

int XG47LinearConvert(int min1, int max1, int min2, int max2, int pos, int referencePoint)
{
    float   scale, offset;
    int     retPos;

    scale = (float)(max2 - min2)/(float)(max1 - min1);
    offset = min2 - min1 * scale;
    if (pos < referencePoint)
    {
        retPos = (int)(pos * scale + offset - 0.5);
    }
    else
    {
        retPos = (int)(pos * scale + offset + 0.5);
    }

    return retPos;
}

long XG47Convert(long data, long defaultValue, long min, long max)
{
    if(defaultValue != 0)
    {
        if (data < 0)
            data = XG47LinearConvert(min, 0, min, (int)defaultValue, (int)data, 0);
        else if (data > 0)
            data = XG47LinearConvert(0, max, (int)defaultValue, max, (int)data, 0);
        else /* (Hue == 0) */
            data = DEFAULT_HUE;
    }

    return data;
}

void XG47HwSetVideoContrast(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD32      contrast = pXGIPort->id ? 0x24AD : 0x242D;
    /*
     * 24xx    2D/AD  R/W Window1 Contrast adjustment
     *              <7:4>:          Reserved
     *              <3:0>:          W1_CONT<3:0>
                        window1 contrast adjustment
     */
    OUTB(contrast, pXGIPort->contrast & 0xF);
}

void XG47HwSetVideoParameters(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD8       sign;
    double      fSinHue, fCosHue;
    CARD32      sinHue, cosHue;
    long        hwFormatTemp, defaultBrightness;

    /*
     * 24xx    2D   R/W Window1 Contrast adjustment
     *              <7:4>:          Reserved
     *              <3:0>:          W1_CONT<3:0>
     *                  window1 contrast adjustment
     * 24xx    2E  R/W Window1 Hue and Saturation adjustment
     *              <7:5>:          Reserved
     *              <4:0>:          W1_SHUS<4:0>
     *                  Window1 Hue * Sin(H) Adjustment
     * 24xx    2F  R/W Window1 Hue and Saturation adjustment
     *              <7:5>:          Reserved
     *              <4:0>:          W1_CHUS<4:0>
     *                  Window1 Hue * Cos(H) Adjustment
     */
    /* Sin(H) */
    fSinHue = sin((double)pXGIPort->hue / 180.0 * PI) * pXGIPort->saturation / 12.5;

    sign = (fSinHue < 0) ? (1 << 4) : 0;

    sinHue = (int)fabs(fSinHue) & 0xF;

    sinHue |= sign;

    /* Cos(H) */
    fCosHue = cos((double)pXGIPort->hue / 180.0 * PI) * pXGIPort->saturation / 12.5;

    sign = (fCosHue < 0) ? (1 << 4) : 0;

    cosHue = (int)fabs(fCosHue)  & 0xF;

    cosHue |= sign;

    OUTB(0x242e, (INB(0x242e) & ~0x1f) | sinHue);
    OUTB(0x242f, (INB(0x242f) & ~0x1f) | cosHue);

    /* Brightness
     * 24xx    2C  R/W 1.1 Window1 Brightness adjustment
     *              <7:6>:          Reserved
     *              <5:0>:          W1_BRIT<5:0>
                         Window1 Brightness Adjustment
     */
    hwFormatTemp = XG47Convert(pXGIPort->brightness, defaultBrightness, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
    hwFormatTemp = hwFormatTemp * 32 / 100;

    OUTB(0x242c, (INB(0x242c) & ~0x3f) | pXGIPort->brightness);
}

Bool XG47IsExpansionMode(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    Bool    isPanelExpension = FALSE;

    if (pXGI->displayDevice & (ST_DISP_LCD | ST_DISP_LCD_MV))
    {
        /*GetPanelStatus(m_ppdev);*/
        /*if (pXGI->deviceStatus & DEVST_PANEL_EXPANSION_ON)*/ /* Support Expansion/Centering? */
        {
            if (((pXGI->displayDevice & ST_DISP_LCD_MV) && !(pXGI->ovlAttr & OVLST_VIDEO_ON_W2))
                ||
                ((pXGI->displayDevice & ST_DISP_LCD) && (pXGI->ovlAttr & OVLST_VIDEO_ON_W2)))
            {
                isPanelExpension = FALSE ;
            }
            else
            {
                isPanelExpension = (IN3CFB(0x5d) & 0x1) ? TRUE : FALSE; /* Expension/Centering */
            }
        }
    }
    return isPanelExpension;
}

void XG47PanningExpansionAdjust(ScrnInfoPtr pScrn, BoxPtr pDstBox)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    CARD16      left, top, right, bottom;
    CARD16      panningX, panningY, panelSizeX, panelSizeY;
    CARD16      modeSizeX, modeSizeY;
    Bool        isPanelExpension;
    Bool        isInW2 = (pXGI->ovlAttr & OVLST_VIDEO_ON_W2);
    Bool        isOutOfScreen = FALSE;

    left   = (CARD16)pDstBox->x1;
    top    = (CARD16)pDstBox->y1;
    right  = (CARD16)pDstBox->x2;
    bottom = (CARD16)pDstBox->y2;

    /* Expand dest rect too if expansion mode ON */
    isPanelExpension = XG47IsExpansionMode(pScrn);
#if 0
    panelSizeX = pXGI->lcdWidth;
    panelSizeY = pXGI->lcdHeight;
#else
    panelSizeX = 0;
    panelSizeY = 0;
#endif
    panningX = modeSizeX = (CARD16) pScrn->currentMode->HDisplay;
    panningY = modeSizeY = (CARD16) pScrn->currentMode->VDisplay;

    if (isPanelExpension)
    {
        panningX = panelSizeX;
        panningY = panelSizeY;

        left   = (CARD16)((long)left   * (long)panelSizeX / (long)modeSizeX);
        right  = (CARD16)((long)right  * (long)panelSizeX / (long)modeSizeX);
        top    = (CARD16)((long)top    * (long)panelSizeY / (long)modeSizeY);
        bottom = (CARD16)((long)bottom * (long)panelSizeY / (long)modeSizeY);
    }

    /*
     * Set flag to indicate dest out of screen,
     * we won't set h/w registers later for this case
     */
    isOutOfScreen =  ((top >= panningY) || (left   >= panningX)
                     || (bottom <= 0)   || (right  <= 0));

#if 0
    if (m_ppdev->m_pHWDev->m_GraCaps & GRACAPS_PANNING)
    {
        CARD16    LineShifted;
        ULONG   srcTopAdjusted = 0, srcLeftAdjusted = 0;

        wSrcW = (CARD16)(m_rclSrc.right  - m_rclSrc.left);
        wSrcH = (CARD16)(m_rclSrc.bottom - m_rclSrc.top);
        wDstW = wRight - wLeft;
        wDstH = wBottom - wTop;

        CalculateZoomFactor(wSrcW, wSrcH, wDstW, wDstH);

        /* Cut top or left source for panning mode */
        if (wTop < 0 && wBottom > 0)
        {
            LineShifted = -wTop;
            srcTopAdjusted = m_ZoomY ?
                ((((ULONG)(LineShifted - 1) * (ULONG)m_ZoomY) >> 10) + 1) :
                   (ULONG)LineShifted;
            wTop = 0;
        }

        if (wLeft < 0 && wRight > 0)
        {
            LineShifted = -wLeft;

            srcLeftAdjusted = m_ZoomX ?
                ((((ULONG)(LineShifted - 1) * (ULONG)m_ZoomX) >> 10) + 1) :
                   (ULONG)LineShifted;
            wLeft = 0;
        }

        m_rclSrc.top  += (long)srcTopAdjusted;
        m_rclSrc.left += (long)srcLeftAdjusted;
    }
#endif
    if (right - left < 16)  right = left + 16;

    pDstBox->x1 = left;
    pDstBox->y1 = top;
    pDstBox->x2 = right;
    pDstBox->y2 = bottom;
}

void XG47CalculateZoomFactor(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD16      zoomX, zoomY;
    short       srcW, srcH;
    short       dstW, dstH;

    srcW = pXGIPort->srcBox.x2 - pXGIPort->srcBox.x1;
    srcH = pXGIPort->srcBox.y2 - pXGIPort->srcBox.y1;
    dstW = pXGIPort->dstBox.x2 - pXGIPort->dstBox.x1;
    dstH = pXGIPort->dstBox.y2 - pXGIPort->dstBox.y1;
    /*
     * Calculate Horizontal zoom factor.
     */
    if ((srcW > 1) && (dstW > 1))
    {
        /*
         * adjust horizontal start and horizontal end for all format.
         * But IF09 have garbage at right side and I adjust zoom factor
         * for the garbage. I will try to find the better solution.
         */

        dstW += 2;

        zoomX = ((srcW - 1) << 10) / (dstW - 1);
        if (zoomX > 0x3FFF)
            zoomX = 0x3FFF;
    }
    else if ((srcW <= 1) && (srcW < dstW))
        zoomX = 0;
    else if ((dstW <= 1) && (srcW > dstW))
        zoomX = 0x3FFF;
    else
        zoomX = 0x400;

    /*
     * Calculate Vertical zoom factor.
     */
    if ((srcH > 1) && (dstH > 1))
    {

        /*
         * if deinterlacing by PS enabled, treat it as pure weave
         * else make weave behave same as bob to avoid jumping scene with weave
         * and bob mixed title TSB #258 with "Sailor Moon"
         */

        zoomY = ((srcH - 1) << 10)/(dstH - 1);

        if (zoomY > 0x3FFF)
            zoomY = 0x3FFF;
    }
    else if ((srcH <= 1) && (srcH < dstH))
        zoomY = 0;
    else if ((dstH <= 1) && (srcH > dstH))
        zoomY = 0x3FFF;
    else
        zoomY = 0x400;

    pXGIPort->zoomX = zoomX;
    pXGIPort->zoomY = zoomY;
}

/*
 * Enable MC
 */
void XG47HwResetMC(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);

    /* 2281 bit[1:0] set to 2'b10, reset MC and PCIE request */
    OUTB(0x2281, (INB(0x2281) & 0xfc) | 0x2);

    /* 2358 bit 31=1'b1 means PCIE request finished reset */
    while(!(INB(0x235b) & 0x80));

    /* MC Control Register, MC on */
    OUTB(0x2281, 0x1);
}

void XG47HwDisableMC(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    OUTB(0x2281, 0x0);
}

void XG47HwSetIDCTorMC(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    /* This is to XP's new feather of use seperate 4 decode output buffer.
     * 2282[5]: Seperate Display and Decode Command 0: combine 1: seperate
     * 2282[3]: Surface Type 0: MVCA Surface 1: YUV Planner mode
     */
    if (pXGI->dvdAttr & DVDCAPS_HW_DXVA_IDCT)
    {
        if (pXGI->dvdAttr & DVDCAPS_HW_DXVA_USE_NV12)
            OUTB(0x2282, 0xfc); /* Support IDCT & MC using NV12 */
        else
            OUTB(0x2282, 0xf8); /* Support IDCT & MC using YV12 */

        OUTB(0x2283, 0x02 | 0x04); /* Enable 8-uncompressed-buffer mode and disable bits swap */
        pXGI->dvdAttr = ((pXGI->dvdAttr & ~DVDST_DECODE_MASK) | DVDST_IDCT_DXVA_ON);
    }
    else
    {
        if (pXGI->dvdAttr & DVDCAPS_HW_DXVA_USE_NV12)
            OUTB(0x2282, 0xec); /* Support MC only using NV12 */
        else
            OUTB(0x2282, 0xe8); /* Support MC only using YV12 */

        OUTB(0x2283, 0x02); /* Enable 8-uncompressed-buffer mode */
        pXGI->dvdAttr = ((pXGI->dvdAttr & ~DVDST_DECODE_MASK) | DVDST_MC_DXVA_ON);
    }

    OUTB(0x2281, INB(0x2281) | 0x8);
}

/*
 * Subpicture function
 */
void XG47HwEnableSubpicture(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;

    /* Set W2 RGB8 format */
    OUTB(0x24a8, (INB(0x24a8) & 0xf8) | 0x01);

    /* Select SP index in low 4 bits (Bit2) & Enable SubPicture(Bit1) */
    if (pXGIPort->spMode == FOURCC_IA44)
    {
        OUTB(0x2470, INB(0x2470) & ~0x02);
    }
    else
    {
        OUTB(0x2470, INB(0x2470) & ~0x06);
    }
    /* Disable W2 Horizontal interpolation & Bypass W2 CSC */
    OUTB(0x24aa, INB(0x24aa) | 0x01);
    OUTB(0x24a9, INB(0x24a9) | 0x18); /*temp bypass HSCB*/

    /* Clear W2 Sharpness factor. it will cause subtitle hairy
     * low 3 bits is factor, high 5 bits is threshold (Only take effect when factor non-zero),
     */
    OUTB(0x24ab, INB(0x24ab) & ~0x07);

    /* W2 line buffer */
    OUTW(0x2490, (INW(0x2490) & 0xfc00) | (unsigned short)(((pXGIPort->spWidth >> 4) + 7) & ~7));
    /* Line Buffer Threshold as 0 to eliminate subpicture noise */
    OUTB(0x2492, (INB(0x2492) & 0x80) | 0);

    /* W2 Src Row Byte */
    OUTW(0x248c, (INW(0x248c) & 0xf000) | (unsigned short)(pXGIPort->spPitch >> 4));
    OUTB(0x248d, INB(0x248d) & 0xf0);

    /* Set W2 RGB FIFO mode */
    OUTB(0x24a9, (INB(0x24a9) & 0xf8) | 0x02);

    /* Set W2 using Windows Key and Color Key */
    OUTB(0x24c0, 0xc0);

    /* Enable W2 Alpha */
    OUTB(0x24c2, INB(0x24c2) | 0x01);

    /* Enable W2 colorkey */
    OUTB(0x24c2, INB(0x24c2) | 0x10);
}

Bool XG47SubpictureEnable(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;

    XG47SubpictureSetDest(pScrn);

    if (pXGIPort->spStatus != DVD_SP_ENABLED)
    {
        XG47HwEnableSubpicture(pScrn);

        /* Enable Video W2 */
        XG47WaitForSync(pScrn);
        OUT3CFB(0x81, IN3CFB(0x81) | 0x01);
        OUTB(0x2470, INB(0x2470)  | 0x02);
    }
    pXGIPort->spStatus = DVD_SP_ENABLED;

    return TRUE;
}

void XG47SubpictureDisable(ScrnInfoPtr pScrn, Bool isKillOrHide)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;

    if (pXGIPort->spStatus != DVD_SP_DISABLED)
    {
        if (isKillOrHide == TRUE) /* Kill */
        {
            /* Disable Subpicture */
            OUTB(0x2470, INB(0x2470)  & ~0x02);
            /* Disable 2nd VE */
            /*temp*/
            /*WaitForBlankStart;*/
            OUT3CFB(0x81, IN3CFB(0x81) & ~0x01);

            pXGIPort->spStatus = DVD_SP_DISABLED;
        }
        else
        {
            /* Just hide. */
            OUTB(0x2496, INB(0x2494));
            OUTB(0x2497, (INB(0x2497) & 0xf0) | (INB(0x2495) & 0xf));
            OUTB(0x249a, INB(0x2498));
            OUTB(0x249b, (INB(0x249b) & 0xf0) | (INB(0x2499) & 0xf));

            pXGIPort->spStatus = DVD_SP_HIDEN;
        }
    }

    return;
}

void XG47SubpictureAdjustRect(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    BoxRec      spSrcBox, spDstBox, w1SrcBox, w1DstBox;
    CARD16      w1ZoomX, w1ZoomY;

    w1ZoomX  = pXGIPort->zoomX;
    w1ZoomY  = pXGIPort->zoomY;
    /* When change color depth, Zoom is zero since ppdev is re-initailized,
     * make it 0x400 which means no zoom at all.
     */
    if (0 == w1ZoomY)
    {
        w1ZoomY = 0x400;
    }
    if (0 == w1ZoomX)
    {
        w1ZoomX = 0x400;
    }

    w1SrcBox = pXGIPort->srcBox;
    w1DstBox = pXGIPort->dstBox;
    spSrcBox = pXGIPort->spSrcBox;
    spDstBox = pXGIPort->spDstBox;

    if (w1SrcBox.x1 >= spDstBox.x2)
    {
        /* whole subpicture cut out. */
        spSrcBox.x1 = spDstBox.x1 = 0;
        spSrcBox.x2 = spDstBox.x2 = 1;
        spSrcBox.y1 = spDstBox.y1 = 0;
        spSrcBox.y2 = spDstBox.y2 = 1;
    }
    else
    {
        if (spDstBox.x1  > w1SrcBox.x1)
            spDstBox.x1 -= w1SrcBox.x1;
        else
        {
            spSrcBox.x1 += (w1SrcBox.x1 - spDstBox.x1);
            spDstBox.x1 = 0;
        }
        spDstBox.x2 -= w1SrcBox.x1;
    }

    if (w1SrcBox.y1 >= spDstBox.y2)
    {
        /* whole subpicture cut out. */
        spSrcBox.x1 = spDstBox.x1 = 0;
        spSrcBox.x2 = spDstBox.x2 = 1;
        spSrcBox.y1 = spDstBox.y1 = 0;
        spSrcBox.y2 = spDstBox.y2 = 1;
    }
    else
    {
        if (spDstBox.y1  > w1SrcBox.y1)
            spDstBox.y1 -= w1SrcBox.y1;
        else
        {
            spSrcBox.y1 += (w1SrcBox.y1 - spDstBox.y1);
            spDstBox.y1 = 0;
        }
        spDstBox.y2 -=  w1SrcBox.y1;
    }

    /* enlarge dest to screen size, and adjust the destination position (for PowerDVD align issue) */
    if (spDstBox.y1)
        spDstBox.y1 = (spDstBox.y1 - 1) * 1024 / w1ZoomY;
    if (spDstBox.y2)
        spDstBox.y2 = (spDstBox.y2 - 1) * 1024 / w1ZoomY;
    if (spDstBox.x1)
        spDstBox.x1 = (spDstBox.x1 - 1) * 1024 / w1ZoomX;
    if (spDstBox.x2)
        spDstBox.x2 = (spDstBox.x2 - 1) * 1024 / w1ZoomX;

    spDstBox.x1 += w1DstBox.x1;
    spDstBox.x2 += w1DstBox.x1;
    spDstBox.y1 += w1DstBox.y1;
    spDstBox.y2 += w1DstBox.y1;

    pXGIPort->spSrcBox = spSrcBox;
    pXGIPort->spDstBox = spDstBox;
}

void XG47SubpictureSetDest(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    CARD32      startAddr = pXGIPort->spStartAddr;
    BoxRec      spSrcBox, spDstBox;

    XG47SubpictureAdjustRect(pScrn);

    pXGIPort->id = 1;
    pXGIPort->isSpOverlay = TRUE;
    XG47HwSetZoomFactor(pScrn);
    XG47HwSetDestRect(pScrn);
    pXGIPort->isSpOverlay = FALSE;
    pXGIPort->id = 0;

    spSrcBox = pXGIPort->spSrcBox;
    spDstBox = pXGIPort->spDstBox;

    /* Set W2 STARTADDR */
    OUTDW(0x2480, (CARD32)(startAddr >> 4));
    /* Colorkey & Colorkey Mask */
    OUTDW(0x24b8, (INDW(0x24b8) & 0xc0000000) | (INDW(0x2438) & 0x3fffffff));
    OUTDW(0x24bc, (INDW(0x24bc) & 0xc0000000) | (INDW(0x243c) & 0x3fffffff));

    return;
}


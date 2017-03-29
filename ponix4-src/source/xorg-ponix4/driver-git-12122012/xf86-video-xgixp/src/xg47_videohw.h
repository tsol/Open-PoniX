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

#ifndef _XG47_VIDEO_HW_H_
#define _XG47_VIDEO_HW_H_

extern void XG47WaitForSync(ScrnInfoPtr pScrn);
extern void XG47ResetVideo(ScrnInfoPtr pScrn);
extern Bool XG47HwIsFlipDone(ScrnInfoPtr pScrn);
extern void XG47HwSetToggle(ScrnInfoPtr pScrn);
extern void XG47HwDisableDeinterlace(ScrnInfoPtr pScrn);
extern Bool XG47HwIsDetectionDone(ScrnInfoPtr pScrn);

#if DBG
extern CARD32 XG47HwGetFieldMotion(ScrnInfoPtr pScrn);
extern CARD32 XG47HwGetFrameMotion(ScrnInfoPtr pScrn);
#endif

extern Bool XG47HwDetectedMovie(ScrnInfoPtr pScrn);
extern Bool XG47HwDetectedMovieParity(ScrnInfoPtr pScrn);
extern Bool XG47HwDetectedProgressive(ScrnInfoPtr pScrn);
extern void XG47Hw3To2DetectionDisable(ScrnInfoPtr pScrn);
extern void XG47Hw3To2DetectionEnable(ScrnInfoPtr pScrn);
extern void XG47Hw3To2DetectionSetSTAddr(ScrnInfoPtr pScrn,
                                         unsigned long MVDETCURR,
                                         unsigned long MVDETPFD,
                                         unsigned long MVDETPFR);
extern void XG47Hw3To2DetectionInitPara(ScrnInfoPtr pScrn);
extern void XG47Hw3To2DetectionSetPara(ScrnInfoPtr pScrn, Bool isTopFirst);
extern void XG47HwTriggerDetection(ScrnInfoPtr pScrn);
extern void XG47HwClearYUV420To422(ScrnInfoPtr pScrn);

#if 0
extern void XG47HwOverDriveLoadLUT(ScrnInfoPtr pScrn);
extern void XG47HwOverDriveSetPara(ScrnInfoPtr pScrn);
extern void  XG47HwOverDriveEnable(ScrnInfoPtr pScrn);
extern void  XG47HwOverDriveDisable();
#endif

extern void XG47HwEnableEdgePatch(ScrnInfoPtr pScrn);
extern void XG47HwDisableEdgePatch(ScrnInfoPtr pScrn);
extern void XG47HwEnableInterpolation(ScrnInfoPtr pScrn, CARD32 mask);
extern void XG47HwDisableInterpolation(ScrnInfoPtr pScrn, CARD32 mask);
extern int XG47HwGetHStart(ScrnInfoPtr pScrn);
extern int XG47HwGetVStart(ScrnInfoPtr pScrn);
extern void XG47HwSetBobbing(ScrnInfoPtr pScrn, Bool isOdd);
extern void XG47HwSetWeaving(ScrnInfoPtr pScrn);
extern void XG47HwSetDeinterlace(ScrnInfoPtr pScrn, CARD32 preAddr);
extern void XG47HwSetMovie(ScrnInfoPtr  pScrn,
                           Bool         isParityFW,
                           Bool         isTopFirst,
                           Bool         isOdd,
                           unsigned long refAddr);
extern void XG47HwSetEdge(ScrnInfoPtr pScrn);
extern void XG47HwInitYUV420To422(ScrnInfoPtr pScrn);
extern void XG47HwSetYUV420To422(ScrnInfoPtr   pScrn,
                                 CARD32        srcID,
                                 unsigned long targAddr,
                                 Bool          isField,
                                 Bool          isOdd);
extern void XG47HwResetYUV420To422(ScrnInfoPtr pScrn);
extern void XG47HwSetConvertedSrc(ScrnInfoPtr pScrn, unsigned long srcAddr);
extern void XG47HwMinimizeOverlayWindow(ScrnInfoPtr pScrn);
extern void XG47HwSetDestRect(ScrnInfoPtr pScrn);
extern void XG47HwEnableVideo(ScrnInfoPtr pScrn);
extern void XG47HwDisableVideo(ScrnInfoPtr pScrn);
extern void XG47HwTurnOffColorKey(ScrnInfoPtr pScrn, CARD32 flag);
extern int XG47HwSetColorKey(ScrnInfoPtr pScrn);
extern void XG47HwSetStartAddress(ScrnInfoPtr pScrn);
extern void XG47HwSetMirror(ScrnInfoPtr pScrn);
extern void XG47HwSetFormat(ScrnInfoPtr pScrn);
extern void XG47HwSetZoomFactor(ScrnInfoPtr pScrn);
extern void XG47HwSetRowByteandLineBuffer(ScrnInfoPtr pScrn);
extern int XG47LinearConvert(int min1, int max1, int min2, int max2, int pos, int referencePoint);
extern long XG47Convert(long data, long defaultValue, long min, long max);
extern void XG47HwSetVideoContrast(ScrnInfoPtr pScrn);
extern void XG47HwSetVideoParameters(ScrnInfoPtr pScrn);
extern Bool XG47IsExpansionMode(ScrnInfoPtr pScrn);
extern void XG47PanningExpansionAdjust(ScrnInfoPtr pScrn, BoxPtr pDstBox);
extern void XG47CalculateZoomFactor(ScrnInfoPtr pScrn);

extern void XG47HwEnableSubpicture(ScrnInfoPtr pScrn);
extern Bool XG47SubpictureEnable(ScrnInfoPtr pScrn);
extern void XG47SubpictureDisable(ScrnInfoPtr pScrn, Bool isKillOrHide);
extern void XG47SubpictureAdjustRect(ScrnInfoPtr pScrn);
extern void XG47SubpictureSetDest(ScrnInfoPtr pScrn);

#endif

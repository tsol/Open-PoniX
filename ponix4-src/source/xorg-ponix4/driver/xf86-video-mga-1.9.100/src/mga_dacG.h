/*
 * MGA-1064, MGA-G100, MGA-G200, MGA-G400, MGA-G550 RAMDAC driver
 */

#ifndef _MGA_DACG_H_
#define _MGA_DACG_H_

void MGAGRestorePalette(ScrnInfoPtr pScrn, unsigned char* pntr);
void MGAGLoadPalette(ScrnInfoPtr, int, int*, LOCO*, VisualPtr);
void MGAGSavePalette(ScrnInfoPtr pScrn, unsigned char* pntr);

#endif

/*
 * Copyright 1994 by Robin Cutshaw <robin@XFree86.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Robin Cutshaw not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Robin Cutshaw makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ROBIN CUTSHAW DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ROBIN CUTSHAW BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _MGA_DAC3026_H_
#define _MGA_DAC3026_H_

void MGA3026RestorePalette(ScrnInfoPtr pScrn, unsigned char* pntr);
void MGA3026LoadPalette(ScrnInfoPtr, int, int*, LOCO*, VisualPtr);
void MGA3026SavePalette(ScrnInfoPtr pScrn, unsigned char* pntr);

#endif

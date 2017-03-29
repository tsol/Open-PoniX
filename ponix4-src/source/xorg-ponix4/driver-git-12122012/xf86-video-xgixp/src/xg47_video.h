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

#ifndef _XG47_VIDEO_H_
#define _XG47_VIDEO_H_

#define XG47_NUM_FORMATS     4

#ifdef XG47_XV_GAMMA
#define XG47_NUM_ATTRIBUTES 9
#else
#define XG47_NUM_ATTRIBUTES 8
#endif

#define XG47_NUM_IMAGES 4

#define VIDEO_INTERPOLATION_H  0x1
#define VIDEO_INTERPOLATION_V  0x2

#define CHROMA_COLOR_BRIGHTNESS     0x00000001
#define CHROMA_COLOR_CONTRAST       0x00000002
#define CHROMA_COLOR_HUE            0x00000004
#define CHROMA_COLOR_SATURATION     0x00000008
#define CHROMA_COLOR_GAMMA          0x00000010
#define CHROMA_COLOR_SHARPNESS      0x00000020
#define BRIGHTNESS_MASK             0x0000fc00
#define CONTRAST_MASK               0x0000000f
#define H_S_SIN_MASK                0x000003e0
#define H_S_COS_MASK                0x0000001f

#define BRIGHTNESS_MIN              -100
#define BRIGHTNESS_MAX              100
#define SHARPNESS_MIN               -100
#define SHARPNESS_MAX               100
#define CONTRAST_MIN                -100
#define CONTRAST_MAX                100
#define HUE_MIN                     -180
#define HUE_MAX                     180
#define SATURATION_MIN              -100
#define SATURATION_MAX              100

#define DEFAULT_HUE                 0
#define DEFAULT_BRIGHTNESS          0
#define DEFAULT_CONTRAST            0
#define DEFAULT_SHARPNESS           0
#define DEFAULT_SATURATION          0

#if 0
Bool IsInVBlank(XGIPtr pXGI)
{
    if (DISPHEAD_CRTC == pXGI->headID)
    {
        return (INB(0x3da) & 0x08);
    }
    else if (DISPHEAD_WIN2 == pXGI->headID)
    {
        return (IN3C5B(0xdc) & 0x01);
    }
    return FALSE;
}

void WaitForVBlank(XGIPtr pXGI)
{
    int timeOut;
    for(timeOut = 0x7FFFF; timeOut > 0; timeOut--)
    {
        if (IsInVBlank(pXGI)) break;
    }
}

void WaitForNotVBlank(XGIPtr pXGI)
{
    int timeOut;
    for(timeOut = 0x7FFFF; timeOut > 0; timeOut--)
    {
        if (!IsInVBlank(pXGI)) break;
    }
}

void WaitForVBlankStart(XGIPtr pXGI)
{
    WaitForNotVBlank(pXGI);
    WaitForVBlank(pXGI);
}

void WaitForVBlankEnd(XGIPtr pXGI)
{
    WaitForVBlank(pXGI);
    WaitForNotVBlank(pXGI);
}

#define VGA_RETRACE     0x0001

CARD32 WaitVRetrace(XGIPtr pXGI, CARD16 flag)
{
    /*CARD16      timeOut = 1000;*/
    CARD32      vTrace = TRUE;

    if (flag == VGA_RETRACE)
    {
        WaitForVBlank(pXGI);
    }

    return vTrace;
}
#endif

extern void XG47InitVideo(ScreenPtr pScreen);

#endif /*_XG47_VIDEO_H*/

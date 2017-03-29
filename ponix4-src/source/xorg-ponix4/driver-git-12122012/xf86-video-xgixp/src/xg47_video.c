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

/* client libraries expect an encoding */
static XF86VideoEncodingRec XG47Encoding[1] =
{
    { 0,
      "XV_IMAGE",
      XGI_IMAGE_MAX_WIDTH,
      XGI_IMAGE_MAX_HEIGHT,
      {1, 1}
    }
};

static XF86VideoFormatRec XG47Formats[XG47_NUM_FORMATS] =
{
    {8,     PseudoColor},
    {15,    TrueColor},
    {16,    TrueColor},
    {24,    TrueColor}
};

static XF86AttributeRec XG47Attributes[XG47_NUM_ATTRIBUTES] =
{
    {XvSettable | XvGettable,    0, (1 << 24) - 1,  "XV_COLORKEY"},
    {XvSettable | XvGettable, -180, 180,            "XV_HUE"},
    {XvSettable | XvGettable, -200, 200,            "XV_SATURATION"},
    {XvSettable | XvGettable,  -32,  31,            "XV_BRIGHTNESS"},
    {XvSettable | XvGettable,   0,   15,            "XV_CONTRAST"},
    {XvSettable | XvGettable,   0,    2,            "XVMC_MODE"},
    {XvSettable | XvGettable,   0,    1,            "XVMC_SUBPICTURE_INDEX"},
    {XvSettable | XvGettable,   0,    7,            "XVMC_UNCOMPRESS_INDEX"}
};

static XF86ImageRec XG47Images[XG47_NUM_IMAGES] =
{
    {
        0x35315652,
        XvRGB,
        LSBFirst,
        {'R','V','1','5',
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        16,
        XvPacked,
        1,
        15, 0x001F, 0x03E0, 0x7C00,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        {'R','V','B',0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        XvTopToBottom
    },
    {
        0x36315652,
        XvRGB,
        LSBFirst,
        {'R','V','1','6',
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
        16,
        XvPacked,
        1,
        16, 0x001F, 0x07E0, 0xF800,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        {'R','V','B',0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        XvTopToBottom
    },
    XVIMAGE_YV12, /* 4:2:0,12 Bits per Pixel: Y... V... U...plannar */
    XVIMAGE_YUY2, /* 4:2:2,16 Bits per Pixel: Y0 U0 Y1 V0,Y2 U1 Y3 V1... increasing memory address*/
#if 0
    XVIMAGE_UYVY, /* 4:2:2,16 Bits per Pixel: U0 Y0 V0 Y1,U1 Y2 V1 Y3... increasing memory address*/
    XVIMAGE_NV12, /* 4:2:0,12 Bits per Pixel: Y... U0 V0 U1 V1...plannar */
    XVIMAGE_I420  /* 4:2:0,12 Bits per Pixel: Y... U... V...plannar */
#endif
};

static XF86VideoAdaptorPtr XG47SetupImageVideo(ScreenPtr pScreen);
static void XG47InitOffscreenImages(ScreenPtr pScreen);

static void XG47StopVideo(ScrnInfoPtr pScrn,
                          pointer pData, Bool isShutdown);
static int  XG47SetPortAttribute(ScrnInfoPtr pScrn,
                                 Atom attribute,
                                 INT32 value,
                                 pointer pData);
static int  XG47GetPortAttribute(ScrnInfoPtr pScrn,
                                 Atom attribute,
                                 INT32 *pValue,
                                 pointer pData);
static void XG47QueryBestSize(ScrnInfoPtr pScrn,
                              Bool isMotion,
                              short vidW, short vidH,
                              short drwW, short drwH,
                              unsigned int *pWidth,
                              unsigned int *pHeight,
                              pointer pData);
static int  XG47PutImage(ScrnInfoPtr pScrn,
                         short srcX, short srcY,
                         short drwX, short drwY,
                         short srcW, short srcH,
                         short drwW, short drwH,
                         int srcID, unsigned char* buf,
                         short width, short height,
                         Bool isSync, RegionPtr pClipBoxes,
                         pointer pData);
static int  XG47PutVideo(ScrnInfoPtr pScrn,
                         short srcX, short srcY,
                         short drwX, short drwY,
                         short srcW, short srcH,
                         short drwW, short drwH,
                         RegionPtr pClipBoxes, pointer data);
static int XG47QueryImageAttributes(ScrnInfoPtr pScrn,
                                    int id,
                                    unsigned short *w, unsigned short *h,
                                    int *pitches, int *offsets);
static void XG47VideoTimerCallback(ScrnInfoPtr pScrn, Time time);

static int XG47AllocateSurface(ScrnInfoPtr pScrn,
                               int id,
                               unsigned short w,
                               unsigned short h,
                               XF86SurfacePtr pSurface);
static int XG47FreeSurface(XF86SurfacePtr pSurface);
static int XG47StopSurface(XF86SurfacePtr pSurface);
static int XG47GetSurfaceAttribute(ScrnInfoPtr pScrn,
                                   Atom attribute, INT32 *value);
static int XG47SetSurfaceAttribute(ScrnInfoPtr pScrn,
                                   Atom attribute, INT32 value);
static int XG47DisplaySurface(XF86SurfacePtr pSurface,
                              short srcX, short srcY,
                              short drwX, short drwY,
                              short srcW, short srcH,
                              short drwW, short drwH,
                              RegionPtr pClipBoxes);

#define MAKE_ATOM(a) MakeAtom(a, sizeof(a) - 1, TRUE)

static Atom xvColorKey, xvSaturation, xvBrightness, xvHUE, xvContrast;
static Atom xvmcUncompressIndex, xvmcMode, xvmcSubpictureIndex;
static Atom xvmcSubSrcX, xvmcSubSrcY, xvmcSubDstX, xvmcSubDstY, xvmcSubW, xvmcSubH;

void XG47InitVideo(ScreenPtr pScreen)
{
    ScrnInfoPtr         pScrn = xf86ScreenToScrn(pScreen);
    XF86VideoAdaptorPtr *adaptors, *newAdaptors = NULL;
    XF86VideoAdaptorPtr newAdaptor = NULL;
    XGIPtr              pXGI = XGIPTR(pScrn);
    int                 numAdaptors;

	/* Jong 11/28/2006; test */
	ErrorF("Jong-XG47InitVideo()...\n");
	
    newAdaptor = XG47SetupImageVideo(pScreen);
    XG47InitOffscreenImages(pScreen);

    numAdaptors = xf86XVListGenericAdaptors(pScrn, &adaptors);

    if(newAdaptor)
    {
        if(!numAdaptors)
        {
            numAdaptors = 1;
            adaptors = &newAdaptor;
        }
        else
        {
            /* need to free this someplace */
            newAdaptors = malloc((numAdaptors + 1) * sizeof(XF86VideoAdaptorPtr*));
            if(newAdaptors)
            {
                memcpy(newAdaptors, adaptors, numAdaptors * sizeof(XF86VideoAdaptorPtr));
                newAdaptors[numAdaptors] = newAdaptor;
                adaptors = newAdaptors;
                numAdaptors++;
            }
        }
    }

    if(numAdaptors)
        xf86XVScreenInit(pScreen, adaptors, numAdaptors);

    if(newAdaptors)
        free(newAdaptors);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 3, "XGI Video Init Successfully \n");
}

static XF86VideoAdaptorPtr XG47SetupImageVideo(ScreenPtr pScreen)
{
    XF86VideoAdaptorPtr pAdaptor = NULL;
    ScrnInfoPtr         pScrn = xf86ScreenToScrn(pScreen);
    XGIPtr              pXGI = XGIPTR(pScrn);
    XGIPortPtr          pXGIPort = NULL;

    pAdaptor = calloc(1, sizeof(XF86VideoAdaptorRec)
                          + sizeof(XGIPortRec)
                          + sizeof(DevUnion));
    if(!pAdaptor)
    {
        return NULL;
    }

    /*
     * XvInputMask indicates that the adaptor can put video into a drawable.
     * XvOutputMask indicates that the adaptor can get video from a drawable.
     *
     * XvVideoMask XvStillMask XvImageMask indicate that the adaptor supports video,
     * still or image primitives respectively.
     *
     * XvWindowMask XvPixmapMask indicate the types of drawables the adaptor is
     * capable of rendering into. At the moment, Pixmap rendering is not
     * supported and the XvPixmapMask flag is ignored.
     */
    pAdaptor->type = XvWindowMask | XvInputMask | XvImageMask | XvVideoMask;

    /*
     * VIDEO_NO_CLIPPING
     * This indicates that the video adaptor does not support clipping.
     * The driver will never receive ``Put'' requests where less than the entire area
     * determined by drwX, drwY, drwW and drwH is visible. This flag does not
     * apply to ``Get'' requests. Hardware that is incapable of clipping ``Gets'' may
     * punt or get the extents of the clipping region passed to it.
     *
     * VIDEO_INVERT_CLIPLIST
     * This indicates that the video driver requires the clip list to contain
     * the regions which are obscured rather than the regions which are visible.
     *
     * VIDEO_OVERLAID_STILLS
     * Implementing PutStill for hardware that does video as an overlay can be
     * awkward since it's unclear how long to leave the video up for. When this
     * flag is set, StopVideo will be called whenever the destination gets clipped
     * or moved so that the still can be left up until then.
     *
     * VIDEO_OVERLAID_IMAGES
     * Same as VIDEO_OVERLAID_STILLS but for images.
     *
     * VIDEO_CLIP_TO_VIEWPORT
     * Indicates that the clip region passed to the driver functions should be
     * clipped to the visible portion of the screen in the case where the viewport
     * is smaller than the virtual desktop.
     */
    pAdaptor->flags = VIDEO_OVERLAID_IMAGES | VIDEO_CLIP_TO_VIEWPORT;
    pAdaptor->name = "XGI Video Overlay";
    pAdaptor->nEncodings = 1;
    pAdaptor->pEncodings = XG47Encoding;
    pAdaptor->nFormats = XG47_NUM_FORMATS;
    pAdaptor->pFormats = XG47Formats;
    pAdaptor->nPorts = 1;
    pAdaptor->pPortPrivates = (DevUnion*)(&pAdaptor[1]);
    pXGIPort = (XGIPortPtr)(&pAdaptor->pPortPrivates[1]);
    pAdaptor->pPortPrivates[0].ptr = (pointer)(pXGIPort);
    pAdaptor->pAttributes = XG47Attributes;
    pAdaptor->nImages = XG47_NUM_IMAGES;
    pAdaptor->nAttributes = XG47_NUM_ATTRIBUTES;
    pAdaptor->pImages = XG47Images;
    pAdaptor->PutVideo = XG47PutVideo;
    pAdaptor->PutStill = NULL;
    pAdaptor->GetVideo = NULL;
    pAdaptor->GetStill = NULL;
    pAdaptor->StopVideo = XG47StopVideo;
    pAdaptor->SetPortAttribute = XG47SetPortAttribute;
    pAdaptor->GetPortAttribute = XG47GetPortAttribute;
    pAdaptor->QueryBestSize = XG47QueryBestSize;
    pAdaptor->PutImage = XG47PutImage;
    pAdaptor->QueryImageAttributes = XG47QueryImageAttributes;

/* Jong 11/28/2006 */
#ifdef XvExtension
    pXGIPort->colorKey = pXGI->videoKey;
#endif

    pXGIPort->hue = 0;
    pXGIPort->saturation = 80;
    pXGIPort->brightness = 45;
    pXGIPort->contrast = 4;
    pXGIPort->id = 0;
    pXGIPort->uncompressIndex = -1;
    pXGIPort->subpictureIndex = -1;
    pXGIPort->mode = 0;

    pXGIPort->isDoubleBuf = TRUE;
    pXGIPort->currentBuf = 0;
    pXGIPort->isSpOverlay = FALSE;
    pXGIPort->isHMirror = FALSE;
    pXGIPort->isVMirror = FALSE;
    pXGIPort->isNoColorKey = FALSE;
    pXGIPort->isEnableHSB = FALSE;
    pXGIPort->isPlanar = FALSE;
    pXGIPort->isYUV    = TRUE;
    pXGIPort->vidMode  = VIDEOMODE_YUY2;
    pXGIPort->spStatus = DVD_SP_DISABLED;
    pXGIPort->videoStatus = 0;

    /* gotta uninit this someplace */
    REGION_INIT(pScreen, &pXGIPort->clip, NullBox, 0);

/* Jong 11/28/2006 */
#ifdef XvExtension
    pXGI->pAdaptor = pAdaptor;
#endif

    xvColorKey = MAKE_ATOM("XV_COLORKEY");
    xvBrightness = MAKE_ATOM("XV_BRIGHTNESS");
    xvSaturation = MAKE_ATOM("XV_SATURATION");
    xvHUE        = MAKE_ATOM("XV_HUE");
    xvContrast   = MAKE_ATOM("XV_CONTRAST");

    xvmcUncompressIndex = MAKE_ATOM("XVMC_UNCOMPRESS_INDEX");
    xvmcSubpictureIndex = MAKE_ATOM("XVMC_SUBPICTURE_INDEX");
    xvmcMode  = MAKE_ATOM("XVMC_MODE");

    /*xvmcSubSrcX, xvmcSubSrcY, xvmcSubDstX, xvmcSubDstY, xvmcSubW, xvmcSubH;*/

    XG47ResetVideo(pScrn);

#ifdef XGI_DUMP
    XGIDumpRegisterValue(pScrn);
#endif

    return pAdaptor;
}

static void XG47InitOffscreenImages(ScreenPtr pScreen)
{
    XF86OffscreenImagePtr pOffscreenImages;

    /* need to free this someplace */
    if(!(pOffscreenImages = malloc(sizeof(XF86OffscreenImageRec))))
        return;

    pOffscreenImages[0].image = &XG47Images[0];
    pOffscreenImages[0].flags = VIDEO_OVERLAID_IMAGES | VIDEO_CLIP_TO_VIEWPORT;
    pOffscreenImages[0].alloc_surface = XG47AllocateSurface;
    pOffscreenImages[0].free_surface = XG47FreeSurface;
    pOffscreenImages[0].display = XG47DisplaySurface;
    pOffscreenImages[0].stop = XG47StopSurface;
    pOffscreenImages[0].setAttribute = XG47SetSurfaceAttribute;
    pOffscreenImages[0].getAttribute = XG47GetSurfaceAttribute;
    pOffscreenImages[0].max_width = 1024;
    pOffscreenImages[0].max_height = 1024;
    pOffscreenImages[0].num_attributes = XG47_NUM_ATTRIBUTES;
    pOffscreenImages[0].attributes = XG47Attributes;

    xf86XVRegisterOffscreenImages(pScreen, pOffscreenImages, 1);
}

static Bool XG47RegionsEqual(RegionPtr pRegionA, RegionPtr pRegionB)
{
    int *pDataA = NULL , *pDataB = NULL;
    int num = 0;

    num = REGION_NUM_RECTS(pRegionA);
    if(num != REGION_NUM_RECTS(pRegionB))
        return FALSE;

    if((pRegionA->extents.x1 != pRegionB->extents.x1)
        || (pRegionA->extents.x2 != pRegionB->extents.x2)
        || (pRegionA->extents.y1 != pRegionB->extents.y1)
        || (pRegionA->extents.y2 != pRegionB->extents.y2))
        return FALSE;

    pDataA = (int*)REGION_RECTS(pRegionA);
    pDataB = (int*)REGION_RECTS(pRegionB);

    while(num--)
    {
        if((pDataA[0] != pDataB[0]) || (pDataA[1] != pDataB[1]))
            return FALSE;
        pDataA += 2;
        pDataB+= 2;
    }

    return TRUE;
}

static void XG47StopVideo(ScrnInfoPtr pScrn,
                          pointer pData,
                          Bool isShutdown)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = (XGIPortPtr)pData;

    REGION_EMPTY(pScrn->pScreen, &pXGIPort->clip);

    if(isShutdown)
    {
        if(pXGIPort->videoStatus & CLIENT_VIDEO_ON)
        {
            /*XG47DisableVideo(pScrn);*/

            XG47WaitForSync(pScrn);

            /* rowByte: 0x240c lineBufLevel: 0x2410 lineBufThreshold: 0x2412 */
            OUTW(0x240c, INW(0x240c) & 0xf000);
            OUTW(0x2410, INW(0x2410) & 0xfc00);
            OUTB(0x2412, (INB(0x2412) & 0x80) + 8);

            /* Disable Win1 and STADD latch
             * 3CE/3CF 80  R/W  <7>:         Window1 STADD latch
             *                                  1: enable 0: disable
             *                  <6:1>:       Reserved
             *                  <0>:         ENVIDEO
             *                                  1: Window1 enable 0: Window1 disable
             */
            /*
            if (pXGI->id)
            {
                OUT3CFB(0x81, IN3CFB(0x81) & 0x7E);
            }
            else
            {
                OUT3CFB(0x80, IN3CFB(0x80) & 0x7E);
            }
            */
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
            /*OUTB(0x2450, INB(0x2450) & ~0x04);*/
        }

        if(pXGIPort->linear)
        {
            xf86FreeOffscreenLinear(pXGIPort->linear);
            pXGIPort->linear = NULL;
        }
        pXGIPort->videoStatus = 0;
    }
    else
    {
        if(pXGIPort->videoStatus & CLIENT_VIDEO_ON)
        {
            pXGIPort->videoStatus |= OFF_TIMER;
            pXGIPort->offTime = currentTime.milliseconds + OFF_DELAY;

/* Jong 11/28/2006 */
#ifdef XvExtension
            pXGI->VideoTimerCallback = XG47VideoTimerCallback;
#endif

        }
    }
}

static int XG47SetPortAttribute(ScrnInfoPtr pScrn,
                                Atom        attribute,
                                INT32       value,
                                pointer     pData)
{
    XGIPortPtr  pXGIPort = (XGIPortPtr)pData;
    XGIPtr      pXGI = XGIPTR(pScrn);

#ifdef HAVE_XAA_H
    if (!pXGI->noAccel) pXGI->pXaaInfo->Sync(pScrn);
#endif

    if(attribute == xvColorKey)
    {
        pXGIPort->colorKey = value;
        XG47HwSetColorKey(pScrn);
        REGION_EMPTY(pScrn->pScreen, &pXGIPort->clip);
    }
    else if (attribute == xvHUE)
    {
        if ((value < -180) || (value > 180))
            return BadValue;
        pXGIPort->hue = value;
        XG47HwSetVideoParameters(pScrn);
    }
    else if (attribute == xvSaturation)
    {
        if ((value < -200) || (value > 200))
            return BadValue;
        pXGIPort->saturation = value;
        XG47HwSetVideoParameters(pScrn);
    }
    else if (attribute == xvBrightness)
    {
        if ((value < -32) || (value > 31))
            return BadValue;
        pXGIPort->brightness = value;
        XG47HwSetVideoParameters(pScrn);
    }
    else if (attribute == xvContrast)
    {
        if ((value < 0) || (value > 15))
            return BadValue;
        pXGIPort->contrast = value;
        XG47HwSetVideoContrast(pScrn);
    }
/*#ifdef XVMC*/
    else if (attribute == xvmcMode)
    {
        pXGIPort->mode = value;
    }
    else if (attribute == xvmcSubpictureIndex)
    {
        pXGIPort->subpictureIndex = value;
    }
    else if (attribute == xvmcUncompressIndex)
    {
        pXGIPort->uncompressIndex = value;
    }
/*#endif*/
    else
        return BadMatch;

   return Success;
}

static int XG47GetPortAttribute(ScrnInfoPtr pScrn,
                                Atom attribute,
                                INT32 *pValue,
                                pointer pData)
{
    XGIPortPtr pXGIPort = (XGIPortPtr)pData;
    XGIPtr     pXGI = XGIPTR(pScrn);

#ifdef HAVE_XAA_H
    if (!pXGI->noAccel) pXGI->pXaaInfo->Sync(pScrn);
#endif

    if(attribute == xvColorKey)
    {
        *pValue = pXGIPort->colorKey;
    }
    else if (attribute == xvHUE)
    {
        *pValue = pXGIPort->hue;
    }
    else if(attribute == xvSaturation)
    {
        *pValue = pXGIPort->saturation;
    }
    else if(attribute == xvBrightness)
    {
        *pValue = pXGIPort->brightness;
    }
    else if (attribute == xvContrast)
    {
        *pValue = pXGIPort->contrast;
    }
/*#ifdef XVMC*/
    else if (attribute == xvmcMode)
    {
        *pValue = pXGIPort->mode;
    }
    else if (attribute == xvmcSubpictureIndex)
    {
        *pValue = pXGIPort->subpictureIndex;
    }
    else if (attribute == xvmcUncompressIndex)
    {
        *pValue = pXGIPort->uncompressIndex;
    }
/*#endif*/
    else
        return BadMatch;

   return Success;
}

static void XG47QueryBestSize(ScrnInfoPtr   pScrn,
                              Bool          isMotion,
                              short vidW, short vidH,
                              short drwW, short drwH,
                              unsigned int  *pWidth,
                              unsigned int  *pHeight,
                              pointer       pData)
{
     *pWidth = drwW;
     *pHeight = drwH;

     if(*pWidth > 16384)
        *pHeight = 16384;
}

/* copy data from system memory to video memory */
static void XG47CopyDataYUY2(CARD8 *src,
                             CARD8 *dst,
                             int srcPitch,
                             int dstPitch,
                             int left, int top,
                             int w,    int h)
{
    w <<= 1;
    while(h--)
    {
        memcpy(dst, (src + (left << 1)) , w);
        src += srcPitch;
        dst += dstPitch;
    }
}

static void XG47CopyDataYV12(unsigned char *srcY,
                             unsigned char *srcV,
                             unsigned char *srcU,
                             unsigned char *dstY,
                             unsigned char *dstV,
                             unsigned char *dstU,
                             int srcPitchY,
                             int srcPitchUV,
                             int dstPitchY,
                             int dstPitchUV,
                             int w, int h)
{
    int i;

    i = h;
    while(i--)
    {
        memcpy(dstY, srcY, w);
        srcY += srcPitchY;
        dstY += dstPitchY;
    }

    w >>= 1;
    i = (h >> 1);
    while(i--)
    {
        memcpy(dstV, srcV, w);
        srcV += srcPitchUV;
        dstV += dstPitchUV;
    }

    i = (h >> 1);
    while(i--)
    {
        memcpy(dstU, srcU, w);
        srcU += srcPitchUV;
        dstU += dstPitchUV;
    }
}

static void XG47CopyDataYV12ToYUY2(unsigned char *srcY,
                                   unsigned char *srcV,
                                   unsigned char *srcU,
                                   unsigned char *dstY,
                                   int srcPitchY,
                                   int srcPitchUV,
                                   int dstPitchY,
                                   int left,
                                   int top,
                                   int w,
                                   int h)
{
    CARD32 *dst = (CARD32*)dstY;
    int i, j;

    dstPitchY >>= 2;    /* CARD32 copy*/
    w >>= 1;

    for(j = 0; j < h; j++)
    {
        for(i = 0; i < w; i++)
        {
            dst[i] = srcY[(i << 1) + left]
                  | (srcY[(i << 1) + left + 1] << 16)
                  | (srcU[i + (left >> 1)] << 8)
                  | (srcV[i + (left >> 1)] << 24);
        }
        dst += dstPitchY;
        srcY += srcPitchY;
        if(j & 1)
        {
            srcV += srcPitchUV;
            srcU += srcPitchUV;
        }
    }
}

static FBLinearPtr XG47AllocateMemory(ScrnInfoPtr pScrn,
                                      FBLinearPtr pFBLinear,
                                      int size)
{
    ScreenPtr   pScreen;
    FBLinearPtr pNewFBLinear;

    if(pFBLinear)
    {
        if(pFBLinear->size >= size)
            return pFBLinear;

        if(xf86ResizeOffscreenLinear(pFBLinear, size))
            return pFBLinear;

        xf86FreeOffscreenLinear(pFBLinear);
    }

    pScreen = xf86ScrnToScreen(pScrn);

    pNewFBLinear = xf86AllocateOffscreenLinear(pScreen, size, 16, NULL, NULL, NULL);

    if(!pNewFBLinear)
    {
        int maxSize;

        xf86QueryLargestOffscreenLinear(pScreen, &maxSize, 16, PRIORITY_EXTREME);

        if(maxSize < size)
            return NULL;

        xf86PurgeUnlockedOffscreenAreas(pScreen);
        pNewFBLinear = xf86AllocateOffscreenLinear(pScreen, size, 16, NULL, NULL, NULL);
    }

    return pNewFBLinear;
}

static int XG47DisplayVideoWithYV12(ScrnInfoPtr pScrn,
                                    int srcID, unsigned char* buf,
                                    short srcX, short srcY,
                                    short drwX, short drwY,
                                    short srcW, short srcH,
                                    short drwW, short drwH,
                                    short width, short height,
                                    RegionPtr pClipBoxes,
                                    pointer pData)
{
    XGIPortPtr  pXGIPort = (XGIPortPtr)pData;
    XGIPtr      pXGI = XGIPTR(pScrn);
    INT32       x1, x2, y1, y2;
    BoxRec      dstBox;
    CARD8       *pDstStart;
    CARD8       *srcOffsetY, *srcOffsetV, *srcOffsetU;
    int         dstOffsetY, dstOffsetV, dstOffsetU;
    int         screenBpp, screenPitch, videoSize, offset;
    int         srcPitchY, srcPitchUV, dstPitchY, dstPitchUV;
    int         sizePitchY, sizePitchUV;
    int         top, left, npixels, nlines;

    srcPitchY = srcPitchUV = dstPitchY = dstPitchUV = 0;

    screenBpp = pScrn->bitsPerPixel >> 3;
    /* get screen pitch */
    screenPitch = screenBpp * pScrn->displayWidth;

    sizePitchY = (width + 15) & ~15;    /* 128bit aligned */
    sizePitchUV = ((width >> 1) + 15) & ~15;
    videoSize = (sizePitchY * height + sizePitchUV * height
                 + screenBpp - 1) / screenBpp; /* YV12 size */

    if (!(pXGIPort->linear = XG47AllocateMemory(pScrn, pXGIPort->linear,
                                                 pXGIPort->isDoubleBuf ? (videoSize << 1) : videoSize)))
        return BadAlloc;

    pXGIPort->currentBuf ^= 1;

    offset = pXGIPort->linear->offset * screenBpp;
    if (pXGIPort->isDoubleBuf)
    {
        offset += pXGIPort->currentBuf * videoSize * screenBpp;
    }
    pDstStart  = pXGI->fbBase;

    switch(srcID)
    {
    case FOURCC_YV12:
        srcOffsetY = buf;
        srcPitchY  = (srcW + 3) & ~3;
        srcOffsetV = srcOffsetY + srcPitchY * srcH;
        srcPitchUV = ((srcW >> 1) + 3) & ~3;
        srcOffsetU = srcOffsetV + (srcPitchUV * (srcH >> 1));
        break;
    case FOURCC_I420:
        srcOffsetY = buf;
        srcPitchY  = (srcW + 3) & ~3;
        srcOffsetU = srcOffsetY + srcPitchY * srcH;
        srcPitchUV = ((srcW >> 1) + 3) & ~3;
        srcOffsetV = srcOffsetV + (srcPitchUV * (srcH >> 1));
        break;
    case FOURCC_UYVY:
    case FOURCC_YUY2:
    default:
        srcPitchY = (srcW << 1);
        break;
    }

    /* Clip */
    x1 = srcX;
    x2 = srcX + srcW;
    y1 = srcY;
    y2 = srcY + srcH;

    dstBox.x1 = drwX;
    dstBox.x2 = drwX + drwW;
    dstBox.y1 = drwY;
    dstBox.y2 = drwY + drwH;

    if ((drwX < 0) || ((drwX + drwW) > pScrn->currentMode->HDisplay)
     || (drwY < 0) || ((drwY + drwH) > pScrn->currentMode->VDisplay)
     || (pScrn->frameX0 > 0) || (pScrn->frameY0 > 0))
    {
        if(!xf86XVClipVideoHelper(&dstBox, &x1, &x2, &y1, &y2,
                                  pClipBoxes, width, height))
            return Success;

        /* copy data */
        left = (x1 >> 16) & ~1;     /* x1 x2 y1 y2 have been moved left 16 bit */
        top  = (y1 >> 16) & ~1;     /* 2 bytes aligned */
        npixels = ((((x2 + 0xFFFF) >> 16) + 1) & ~1) - left;    /* 2 bytes aligned */
        nlines  = ((((y2 + 0xFFFF) >> 16) + 1) & ~1) - top;
    }
    else
    {
        /* copy data */
        left = x1 & ~1;
        top  = y1 & ~1;
        npixels = ((x2 + 1) & ~1) - left;    /* 2 bytes aligned */
        nlines  = ((y2 + 1) & ~1) - top;
    }

    dstBox.x1 -= pScrn->frameX0;
    dstBox.x2 -= pScrn->frameX0;
    dstBox.y1 -= pScrn->frameY0;
    dstBox.y2 -= pScrn->frameY0;

    switch(srcID)
    {
    case FOURCC_YV12:
        dstOffsetY = offset;

        dstPitchY  = (npixels + 15) & ~15;
        dstOffsetV = dstOffsetY + dstPitchY * nlines;
        dstPitchUV = ((npixels >> 1) + 15) & ~15;
        dstOffsetU = dstOffsetV + (dstPitchUV * (nlines >> 1));

        XG47CopyDataYV12(srcOffsetY + (top * srcPitchY) + left,
                         srcOffsetV + ((top >> 1) * srcPitchUV) + (left >> 1),
                         srcOffsetU + ((top >> 1) * srcPitchUV) + (left >> 1),
                         pDstStart + dstOffsetY,
                         pDstStart + dstOffsetV,
                         pDstStart + dstOffsetU,
                         srcPitchY, srcPitchUV,
                         dstPitchY, dstPitchUV,
                         npixels, nlines);
        break;
    case FOURCC_I420:
        dstOffsetY = offset;

        dstPitchY  = (npixels + 15) & ~15;
        dstOffsetU = dstOffsetY + dstPitchY * nlines;
        dstPitchUV = ((npixels >> 1) + 15) & ~15;
        dstOffsetV = dstOffsetV + (dstPitchUV * (nlines >> 1));

        XG47CopyDataYV12(srcOffsetY + (top * srcPitchY) + left,
                         srcOffsetV + ((top >> 1) * srcPitchUV) + (left >> 1),
                         srcOffsetU + ((top >> 1) * srcPitchUV) + (left >> 1),
                         pDstStart + dstOffsetY,
                         pDstStart + dstOffsetV,
                         pDstStart + dstOffsetU,
                         srcPitchY, srcPitchUV,
                         dstPitchY, dstPitchUV,
                         npixels, nlines);
        break;
    case FOURCC_UYVY:
    case FOURCC_YUY2:
    default:
        left <<= 1;
        pDstStart += left;
        nlines = ((y2 + 0xFFFF) >> 16) - top;
        /*XG47CopyDataYUY2ToYV12();*/
        break;
    }

    /* update cliplist */
    if(!XG47RegionsEqual(&pXGIPort->clip, pClipBoxes))
    {
        /* update cliplist */
        REGION_COPY(pScrn->pScreen, &pXGIPort->clip, pClipBoxes);
        xf86XVFillKeyHelper(pScrn->pScreen, pXGIPort->colorKey, pClipBoxes);
    }

    if ((pXGI->displayDevice & ST_DISP_LCD) || (IN3CFB(0x5B) & ST_DISP_LCD))
    {
        XG47PanningExpansionAdjust(pScrn, &dstBox);
    }

    pXGIPort->hStart = XG47HwGetHStart(pScrn);
    pXGIPort->vStart = XG47HwGetVStart(pScrn);

    XG47WaitForSync(pScrn);

    vAcquireRegIOProtect(pXGI);
    XG47HwSetFormat(pScrn);

    pXGIPort->dstBox.x1 = dstBox.x1;
    pXGIPort->dstBox.x2 = dstBox.x2;
    pXGIPort->dstBox.y1 = dstBox.y1;
    pXGIPort->dstBox.y2 = dstBox.y2;
    XG47HwSetDestRect(pScrn);
    XG47HwSetMirror(pScrn);
    XG47HwSetEdge(pScrn);

    pXGIPort->startAddrY = dstOffsetY;
    pXGIPort->startAddrV = dstOffsetV;
    pXGIPort->startAddrU = dstOffsetU;
    XG47HwSetStartAddress(pScrn);

    /* set Horizontal and Vertical Zoom */
    pXGIPort->srcBox.x1 = left;
    pXGIPort->srcBox.x2 = left + npixels;
    pXGIPort->srcBox.y1 = top;
    pXGIPort->srcBox.y2 = top + nlines;
    XG47CalculateZoomFactor(pScrn);
    XG47HwSetZoomFactor(pScrn);

    /* set Row byte and Line buffer */
    pXGIPort->rowByteY = (npixels + 15) & ~15;
    pXGIPort->rowByteUV = ((npixels >> 1) + 15) & ~15;
    pXGIPort->lineBufLevel = (((npixels + 7) >> 3) + 7) & ~7; /* 8 aligned */
    if (pXGIPort->lineBufLevel < 8)
    {
        pXGIPort->lineBufLevel = 8;
    }
    pXGIPort->lineBufThreshold = 0x38;
    pXGIPort->lineBufBreak = 0;
    XG47HwSetRowByteandLineBuffer(pScrn);

    XG47HwSetToggle(pScrn);
    return 0;
}

static int XG47DisplayVideoWithYUY2(ScrnInfoPtr pScrn,
                                    int srcID, unsigned char* buf,
                                    short srcX, short srcY,
                                    short drwX, short drwY,
                                    short srcW, short srcH,
                                    short drwW, short drwH,
                                    short width, short height,
                                    RegionPtr pClipBoxes,
                                    pointer pData)
{
    XGIPortPtr  pXGIPort = (XGIPortPtr)pData;
    XGIPtr      pXGI = XGIPTR(pScrn);
    INT32       x1, x2, y1, y2;
    BoxRec      dstBox;
    CARD8       *pDstStart;
    CARD8       *srcOffsetY, *srcOffsetV, *srcOffsetU;
    int         dstOffsetY, dstOffsetV, dstOffsetU;
    int         screenBpp, screenPitch, videoSize, offset;
    int         srcPitchY, srcPitchUV, dstPitchY, dstPitchUV;
    int         sizePitchY, sizePitchUV;
    int         top, left, npixels, nlines;

    srcPitchY = srcPitchUV = dstPitchY = dstPitchUV = 0;

    screenBpp = pScrn->bitsPerPixel >> 3;
    /* get screen pitch */
    screenPitch = screenBpp * pScrn->displayWidth;

    sizePitchY = ((width << 1) + 15) & ~15;    /* 128bit aligned */
    sizePitchUV = 0;
    videoSize = ((sizePitchY * height) + screenBpp - 1) / screenBpp; /* YUY2 size */

    if (!(pXGIPort->linear = XG47AllocateMemory(pScrn, pXGIPort->linear,
                                                 pXGIPort->isDoubleBuf ? (videoSize << 1) : videoSize)))
        return BadAlloc;

    pXGIPort->currentBuf ^= 1;

    offset = pXGIPort->linear->offset * screenBpp;
    if (pXGIPort->isDoubleBuf)
    {
        offset += pXGIPort->currentBuf * videoSize * screenBpp;
    }
    pDstStart  = pXGI->fbBase;

    switch(srcID)
    {
    case FOURCC_YV12:
        srcOffsetY = buf;
        srcPitchY  = (srcW + 3) & ~3;
        srcOffsetV = srcOffsetY + srcPitchY * srcH;
        srcPitchUV = ((srcW >> 1) + 3) & ~3;
        srcOffsetU = srcOffsetV + (srcPitchUV * (srcH >> 1));
        break;
    case FOURCC_I420:
        srcOffsetY = buf;
        srcPitchY  = (srcW + 3) & ~3;
        srcOffsetU = srcOffsetY + srcPitchY * srcH;
        srcPitchUV = ((srcW >> 1) + 3) & ~3;
        srcOffsetV = srcOffsetV + (srcPitchUV * (srcH >> 1));
        break;
    case FOURCC_UYVY:
    case FOURCC_YUY2:
    default:
        srcPitchY = (srcW << 1);
        break;
    }

    /* Clip */
    x1 = srcX;
    x2 = srcX + srcW;
    y1 = srcY;
    y2 = srcY + srcH;

    dstBox.x1 = drwX;
    dstBox.x2 = drwX + drwW;
    dstBox.y1 = drwY;
    dstBox.y2 = drwY + drwH;

    if ((drwX < 0) || ((drwX + drwW) > pScrn->currentMode->HDisplay)
     || (drwY < 0) || ((drwY + drwH) > pScrn->currentMode->VDisplay)
     || (pScrn->frameX0 > 0) || (pScrn->frameY0 > 0))
    {
        if(!xf86XVClipVideoHelper(&dstBox, &x1, &x2, &y1, &y2,
                                  pClipBoxes, width, height))
            return Success;

        /* copy data */
        left = (x1 >> 16) & ~1;     /* x1 x2 y1 y2 have been moved left 16 bit */
        top  = (y1 >> 16) & ~1;     /* 2 bytes aligned */
        npixels = ((((x2 + 0xFFFF) >> 16) + 1) & ~1) - left;    /* 2 bytes aligned */
        nlines  = ((((y2 + 0xFFFF) >> 16) + 1) & ~1) - top;
    }
    else
    {
        /* copy data */
        left = x1 & ~1;
        top  = y1 & ~1;
        npixels = ((x2 + 1) & ~1) - left;    /* 2 bytes aligned */
        nlines  = ((y2 + 1) & ~1) - top;
    }

    dstBox.x1 -= pScrn->frameX0;
    dstBox.x2 -= pScrn->frameX0;
    dstBox.y1 -= pScrn->frameY0;
    dstBox.y2 -= pScrn->frameY0;

    switch(srcID)
    {
    case FOURCC_YV12:
    case FOURCC_I420:
        dstOffsetY = offset;
        dstPitchY  = ((npixels << 1) + 15) & ~15;
        dstOffsetV = 0;
        dstPitchUV = 0;
        dstOffsetU = 0;

        XG47CopyDataYV12ToYUY2(srcOffsetY + (top * srcPitchY),
                               srcOffsetV + (top >> 1) * srcPitchUV,
                               srcOffsetU + (top >> 1) * srcPitchUV,
                               pDstStart  + dstOffsetY,
                               srcPitchY, srcPitchUV,
                               dstPitchY,
                               left, top, npixels, nlines);
        break;
    case FOURCC_UYVY:
    case FOURCC_YUY2:
    default:
        dstOffsetY = offset;
        dstPitchY  = ((npixels << 1) + 15) & ~15;
        dstOffsetV = 0;
        dstPitchUV = 0;
        dstOffsetU = 0;

        XG47CopyDataYUY2(srcOffsetY + (top * srcPitchY),
                         pDstStart + dstOffsetY,
                         srcPitchY, dstPitchY,
                         left, top, npixels, nlines);
        break;
    }

    /* update cliplist */
    if(!XG47RegionsEqual(&pXGIPort->clip, pClipBoxes))
    {
        /* update cliplist */
        REGION_COPY(pScrn->pScreen, &pXGIPort->clip, pClipBoxes);
        xf86XVFillKeyHelper(pScrn->pScreen, pXGIPort->colorKey, pClipBoxes);
    }

    if ((pXGI->displayDevice & ST_DISP_LCD) || (IN3CFB(0x5B) & ST_DISP_LCD))
    {
        XG47PanningExpansionAdjust(pScrn, &dstBox);
    }

    pXGIPort->hStart = XG47HwGetHStart(pScrn);
    pXGIPort->vStart = XG47HwGetVStart(pScrn);

    XG47WaitForSync(pScrn);

    vAcquireRegIOProtect(pXGI);
    XG47HwSetFormat(pScrn);

    pXGIPort->dstBox.x1 = dstBox.x1;
    pXGIPort->dstBox.x2 = dstBox.x2;
    pXGIPort->dstBox.y1 = dstBox.y1;
    pXGIPort->dstBox.y2 = dstBox.y2;
    XG47HwSetDestRect(pScrn);
    XG47HwSetMirror(pScrn);
    XG47HwSetEdge(pScrn);

    pXGIPort->startAddrY = dstOffsetY;
    XG47HwSetStartAddress(pScrn);

    /* set Horizontal and Vertical Zoom */
    pXGIPort->srcBox.x1 = left;
    pXGIPort->srcBox.x2 = left + npixels;
    pXGIPort->srcBox.y1 = top;
    pXGIPort->srcBox.y2 = top + nlines;
    XG47CalculateZoomFactor(pScrn);
    XG47HwSetZoomFactor(pScrn);

    /* set Row byte and Line buffer */
    pXGIPort->rowByteY = ((npixels << 1) + 15) & ~15;
    pXGIPort->rowByteUV = 0;
    pXGIPort->lineBufLevel = ((((npixels <<1) + 7) >> 3) + 7) & ~7; /* 8 aligned */
    if (pXGIPort->lineBufLevel < 8)
    {
        pXGIPort->lineBufLevel = 8;
    }
    pXGIPort->lineBufThreshold = 0x8;
    pXGIPort->lineBufBreak = 0;
    XG47HwSetRowByteandLineBuffer(pScrn);

    XG47HwSetToggle(pScrn);

    return 0;
}

static int XG47PutImage(ScrnInfoPtr pScrn,
                        short srcX, short srcY,
                        short drwX, short drwY,
                        short srcW, short srcH,
                        short drwW, short drwH,
                        int srcID, unsigned char* buf,
                        short width, short height,
                        Bool isSync, RegionPtr pClipBoxes,
                        pointer pData)
{
    XGIPortPtr  pXGIPort = (XGIPortPtr)pData;
    XGIPtr      pXGI = XGIPTR(pScrn);

	/* Jong 11/28/2006; test */
	ErrorF("Jong-XG47PutImage()...\n");

    switch(srcID)
    {
    case FOURCC_YV12:
    case FOURCC_I420:
#if 1
        pXGIPort->isHMirror = FALSE;
        pXGIPort->isVMirror = FALSE;
        pXGIPort->isYUV    = TRUE;
        pXGIPort->isPlanar = TRUE;
        pXGIPort->vidMode  = VIDEOMODE_YV12;
        XG47DisplayVideoWithYV12(pScrn, srcID, buf,
                                 srcX, srcY, drwX, drwY,
                                 srcW, srcH, drwW, drwH,
                                 width, height,
                                 pClipBoxes, pData);
#endif
#if 0
        pXGIPort->vidMode  = VIDEOMODE_YUY2;
        pXGIPort->isYUV    = TRUE;
        pXGIPort->isPlanar = FALSE;    /* Because we will use YUY2 format to play */
        pXGIPort->isHMirror = FALSE;
        pXGIPort->isVMirror = FALSE;
        XG47DisplayVideoWithYUY2(pScrn, srcID, buf,
                                 srcX, srcY, drwX, drwY,
                                 srcW, srcH, drwW, drwH,
                                 width, height,
                                 pClipBoxes, pData);
#endif
        break;
    case FOURCC_UYVY:
    case FOURCC_YUY2:
    default:
        pXGIPort->vidMode  = VIDEOMODE_YUY2;
        pXGIPort->isYUV    = TRUE;
        pXGIPort->isPlanar = FALSE;
        pXGIPort->isHMirror = FALSE;
        pXGIPort->isVMirror = FALSE;
        XG47DisplayVideoWithYUY2(pScrn, srcID, buf,
                                 srcX, srcY, drwX, drwY,
                                 srcW, srcH, drwW, drwH,
                                 width, height,
                                 pClipBoxes, pData);
        break;
    }

    if(pXGIPort->videoStatus != CLIENT_VIDEO_ON)
    {
        pXGIPort->videoStatus = CLIENT_VIDEO_ON;
    }

    return Success;
}

static int XG47QueryImageAttributes(ScrnInfoPtr pScrn,
                                    int id,
                                    unsigned short *w,
                                    unsigned short *h,
                                    int *pitches,
                                    int *offsets)
{
    int size, temp;

    if (*w > XGI_IMAGE_MAX_WIDTH) *w = XGI_IMAGE_MAX_WIDTH;
    if (*h > XGI_IMAGE_MAX_HEIGHT) *h = XGI_IMAGE_MAX_HEIGHT;

    *w = (*w + 1) & ~1;
    if (offsets) offsets[0] = 0;

    switch(id)
    {
    case FOURCC_YV12:            /* YV12 */
        *h = (*h + 1) & ~1;
        size = (*w + 3) & ~3;
        if(pitches) pitches[0] = size;
        size *= *h;
        if(offsets) offsets[1] = size;
        temp = ((*w >> 1) + 3) & ~3;
        if(pitches) pitches[1] = pitches[2] = temp;
        temp *= (*h >> 1);
        size += temp;
        if(offsets) offsets[2] = size;
        size += temp;
        break;
    default:                  /* RGB15, RGB16, YUY2 */
        size = *w << 1;
        if(pitches) pitches[0] = size;
        size *= *h;
        break;
    }

    return size;
}

/* Jong 11/28/2006 */
#ifdef XvExtension
static void XG47VideoTimerCallback(ScrnInfoPtr pScrn, Time time)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIPortPtr  pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;

    if(pXGIPort->videoStatus & TIMER_MASK)
    {
        if(pXGIPort->videoStatus & OFF_TIMER)
        {
            if(pXGIPort->offTime < time)
            {
                XG47WaitForSync(pScrn);
                /* rowByte: 0x240c lineBufLevel: 0x2410 lineBufThreshold: 0x2412 */
                OUTW(0x240c, INW(0x240c) & 0xf000);
                OUTW(0x2410, INW(0x2410) & 0xfc00);
                OUTB(0x2412, (INB(0x2412) & 0x80) + 8);

                /* Disable Win1 and STADD latch
                 * 3CE/3CF 80  R/W  <7>:         Window1 STADD latch
                 *                                  1: enable 0: disable
                 *                  <6:1>:       Reserved
                 *                  <0>:         ENVIDEO
                 *                                  1: Window1 enable 0: Window1 disable
                 */
                /*
                if (pXGI->id)
                {
                    OUT3CFB(0x81, IN3CFB(0x81) & 0x7E);
                }
                else
                {
                    OUT3CFB(0x80, IN3CFB(0x80) & 0x7E);
                }
                */
                pXGIPort->videoStatus = FREE_TIMER;
                pXGIPort->freeTime = time + FREE_DELAY;
            }
        }
        else
        {
            /* FREE_TIMER */
            if(pXGIPort->freeTime < time)
            {
                if(pXGIPort->linear)
                {
                    xf86FreeOffscreenLinear(pXGIPort->linear);
                    pXGIPort->linear = NULL;
                }
                pXGIPort->videoStatus = 0;
                pXGI->VideoTimerCallback = NULL;
            }
        }
    }
    else
    {
        /* shouldn't get here */
        pXGI->VideoTimerCallback = NULL;
    }
}
#endif

/*
 * Offscreen stuff
 */
static int XG47AllocateSurface(ScrnInfoPtr pScrn,
                               int id,
                               unsigned short w,
                               unsigned short h,
                               XF86SurfacePtr pSurface)
{
    FBLinearPtr         pFBLinear;
    OffscreenPrivPtr    pOffscreenPriv;
    int                 pitch, fbPitch, size, bpp;

    if((w > 1024) || (h > 1024))
        return BadAlloc;

    w = (w + 1) & ~1;
    pitch = ((w << 1) + 15) & ~15;
    bpp = pScrn->bitsPerPixel >> 3;
    fbPitch = bpp * pScrn->displayWidth;
    size = ((pitch * h) + bpp - 1) / bpp;

    if(!(pFBLinear = XG47AllocateMemory(pScrn, NULL, size)))
    {
        return BadAlloc;
    }

    pSurface->width = w;
    pSurface->height = h;

    if(!(pSurface->pitches = malloc(sizeof(int))))
    {
        xf86FreeOffscreenLinear((FBLinearPtr)pSurface);
        return BadAlloc;
    }
    if(!(pSurface->offsets = malloc(sizeof(int))))
    {
        free(pSurface->pitches);
        xf86FreeOffscreenLinear(pFBLinear);
      return BadAlloc;
    }
    if(!(pOffscreenPriv = malloc(sizeof(OffscreenPrivRec))))
    {
        free(pSurface->pitches);
        free(pSurface->offsets);
        xf86FreeOffscreenLinear(pFBLinear);
        return BadAlloc;
    }

    pOffscreenPriv->pFBLinear = pFBLinear;
    pOffscreenPriv->isOn = FALSE;

    pSurface->pScrn = pScrn;
    pSurface->id = id;
    pSurface->pitches[0] = pitch;
    pSurface->offsets[0] = pFBLinear->offset * bpp;
    pSurface->devPrivate.ptr = (pointer)pOffscreenPriv;

    return Success;
}

static int XG47StopSurface(XF86SurfacePtr pSurface)
{
    OffscreenPrivPtr pOffscreenPriv = (OffscreenPrivPtr)pSurface->devPrivate.ptr;

    if(pOffscreenPriv->isOn)
    {
        XGIPtr  pXGI = XGIPTR(pSurface->pScrn);

        OUTW(0x3D4, 0x0091);
        XG47WaitForSync(pSurface->pScrn);
        OUTW(0x3D4, 0x848E);
        pOffscreenPriv->isOn = FALSE;
    }

    return Success;
}


static int XG47FreeSurface(XF86SurfacePtr pSurface)
{
    OffscreenPrivPtr pOffscreenPriv = (OffscreenPrivPtr)pSurface->devPrivate.ptr;

    if (pOffscreenPriv->isOn)
    {
        XG47StopSurface(pSurface);
    }
    xf86FreeOffscreenLinear(pOffscreenPriv->pFBLinear);
    free(pSurface->pitches);
    free(pSurface->offsets);
    free(pSurface->devPrivate.ptr);

    return Success;
}

static int XG47GetSurfaceAttribute(ScrnInfoPtr pScrn,
                                   Atom attribute, INT32 *value)
{
    return XG47GetPortAttribute(pScrn, attribute, value,
                                (pointer)(GET_PORT_PRIVATE(pScrn)));
}

static int XG47SetSurfaceAttribute(ScrnInfoPtr pScrn,
                                   Atom attribute, INT32 value)
{
    return XG47SetPortAttribute(pScrn, attribute, value,
                                (pointer)(GET_PORT_PRIVATE(pScrn)));
}

static int XG47DisplaySurface(XF86SurfacePtr pSurface,
                              short srcX, short srcY,
                              short drwX, short drwY,
                              short srcW, short srcH,
                              short drwW, short drwH,
                              RegionPtr pClipBoxes)
{
    OffscreenPrivPtr    pOffscreenPriv = (OffscreenPrivPtr)pSurface->devPrivate.ptr;
    ScrnInfoPtr         pScrn = pSurface->pScrn;
    XGIPtr              pXGI = XGIPTR(pScrn);
    XGIPortPtr          pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;

    INT32               x1, y1, x2, y2;
    BoxRec              dstBox;

    x1 = srcX;
    x2 = srcX + srcW;
    y1 = srcY;
    y2 = srcY + srcH;

    dstBox.x1 = drwX;
    dstBox.x2 = drwX + drwW;
    dstBox.y1 = drwY;
    dstBox.y2 = drwY + drwH;

    if(!xf86XVClipVideoHelper(&dstBox, &x1, &x2, &y1, &y2, pClipBoxes,
                  pSurface->width, pSurface->height))
    {
      return Success;
    }

    dstBox.x1 -= pScrn->frameX0;
    dstBox.x2 -= pScrn->frameX0;
    dstBox.y1 -= pScrn->frameY0;
    dstBox.y2 -= pScrn->frameY0;

    XG47HwEnableVideo(pScrn);

    pXGIPort->hStart = XG47HwGetHStart(pScrn);
    pXGIPort->vStart = XG47HwGetVStart(pScrn);

    /*
    XG47DisplayVideo(pScrn, pSurface->id, pSurface->offsets[0],
                   pSurface->width, pSurface->height, pSurface->pitches[0],
                   x1, y1, x2, y2, &dstBox, srcW, srcH, drwW, drwH);
    */

    xf86XVFillKeyHelper(pScrn->pScreen, pXGIPort->colorKey, pClipBoxes);

    pOffscreenPriv->isOn = TRUE;
    /* we've prempted the XvImage stream so set its free timer */
    if(pXGIPort->videoStatus & CLIENT_VIDEO_ON)
    {
        REGION_EMPTY(pScrn->pScreen, &pXGIPort->clip);
        UpdateCurrentTime();
        pXGIPort->videoStatus = FREE_TIMER;
        pXGIPort->freeTime = currentTime.milliseconds + FREE_DELAY;

/* Jong 11/28/2006 */
#ifdef XvExtension
        pXGI->VideoTimerCallback = XG47VideoTimerCallback;
#endif
    }

    return Success;
}

static int XG47DisplayDVDVideo(ScrnInfoPtr pScrn,
                               short srcX, short srcY,
                               short drwX, short drwY,
                               short srcW, short srcH,
                               short drwW, short drwH,
                               RegionPtr pClipBoxes,
                               pointer pData)
{
    XGIPortPtr  pXGIPort = (XGIPortPtr)pData;
    XGIXvMCCreateSurfacePtr pSurf = NULL;
    XGIPtr      pXGI = XGIPTR(pScrn);
    INT32       x1, x2, y1, y2;
    BoxRec      dstBox;
    int         dstOffsetY, dstOffsetV, dstOffsetU;
    int         i = 0, top, left, npixels, nlines;
    short       width, height;

    for(i = 0; i < XGI_MAX_SURFACES; i++)
    {
        if(pXGI->xvmcSurface[i].index == pXGIPort->uncompressIndex)
        {
            pSurf = &(pXGI->xvmcSurface[i]);
            break;
        }
    }

    /* Clip */
    x1 = srcX;
    x2 = srcX + srcW;
    y1 = srcY;
    y2 = srcY + srcH;

    dstBox.x1 = drwX;
    dstBox.x2 = drwX + drwW;
    dstBox.y1 = drwY;
    dstBox.y2 = drwY + drwH;

    if(!xf86XVClipVideoHelper(&dstBox, &x1, &x2, &y1, &y2,
                              pClipBoxes, srcW, srcH))
        return Success;

    /* copy data */
    left = (x1 >> 16) & ~1;     /* x1 x2 y1 y2 have been moved left 16 bit */
    top  = (y1 >> 16) & ~1;     /* 2 bytes aligned */
    npixels = ((((x2 + 0xFFFF) >> 16) + 1) & ~1) - left;    /* 2 bytes aligned */
    nlines  = ((((y2 + 0xFFFF) >> 16) + 1) & ~1) - top;

    dstOffsetY = pSurf->offsetY + pSurf->pitch * top + left;
    dstOffsetV = pSurf->offsetV + (pSurf->pitch >> 1) * (top >> 1) + (left >> 1);
    dstOffsetU = pSurf->offsetU + (pSurf->pitch >> 1) * (top >> 1) + (left >> 1);

    dstBox.x1 -= pScrn->frameX0;
    dstBox.x2 -= pScrn->frameX0;
    dstBox.y1 -= pScrn->frameY0;
    dstBox.y2 -= pScrn->frameY0;

    if ((pXGI->displayDevice & ST_DISP_LCD) || (IN3CFB(0x5B) & ST_DISP_LCD))
    {
        XG47PanningExpansionAdjust(pScrn, &dstBox);
    }

    pXGIPort->hStart = XG47HwGetHStart(pScrn);
    pXGIPort->vStart = XG47HwGetVStart(pScrn);

    vAcquireRegIOProtect(pXGI);
    XG47HwSetFormat(pScrn);

    pXGIPort->dstBox.x1 = dstBox.x1;
    pXGIPort->dstBox.x2 = dstBox.x2;
    pXGIPort->dstBox.y1 = dstBox.y1;
    pXGIPort->dstBox.y2 = dstBox.y2;
    pXGIPort->spDstBox.x1 = dstBox.x1;
    pXGIPort->spDstBox.x2 = dstBox.x2;
    pXGIPort->spDstBox.y1 = dstBox.y1;
    pXGIPort->spDstBox.y2 = dstBox.y2;
    XG47HwSetDestRect(pScrn);
    XG47HwSetMirror(pScrn);
    XG47HwSetEdge(pScrn);

    pXGIPort->startAddrY = dstOffsetY;
    pXGIPort->startAddrV = dstOffsetV;
    pXGIPort->startAddrU = dstOffsetU;
    XG47HwSetStartAddress(pScrn);

    /* set Horizontal and Vertical Zoom */
    pXGIPort->srcBox.x1 = left;
    pXGIPort->srcBox.x2 = left + npixels;
    pXGIPort->srcBox.y1 = top;
    pXGIPort->srcBox.y2 = top + nlines;
    pXGIPort->spSrcBox.x1 = left;
    pXGIPort->spSrcBox.x2 = left + npixels;
    pXGIPort->spSrcBox.y1 = top;
    pXGIPort->spSrcBox.y2 = top + nlines;
    XG47CalculateZoomFactor(pScrn);
    XG47HwSetZoomFactor(pScrn);

    /* set Row byte and Line buffer */
    pXGIPort->rowByteY = pSurf->pitch;
    pXGIPort->rowByteUV = (pSurf->pitch >> 1);
    pXGIPort->lineBufLevel = (((pSurf->pitch + 7) >> 3) + 7) & ~7; /* 8 aligned */
    if (pXGIPort->lineBufLevel < 8)
    {
        pXGIPort->lineBufLevel = 8;
    }
    pXGIPort->lineBufThreshold = 0x38;
    pXGIPort->lineBufBreak = 0;
    XG47HwSetRowByteandLineBuffer(pScrn);

    XG47HwSetToggle(pScrn);
    return 0;
}

static void XG47DisplaySubpicture(ScrnInfoPtr pScrn,
                                  pointer pData)
{
    XGIPtr          pXGI = XGIPTR(pScrn);
    XGIPortPtr      pXGIPort = (XGIPortPtr)pData;
    XGIXvMCSubpicturePtr pSub = &(pXGI->xvmcSubpic);

    pXGIPort->spStartAddr = pSub->hwAddr + (pSub->size >> 1) * pXGIPort->subpictureIndex;
    XG47SubpictureEnable(pScrn);
}
static int XG47PutVideo(ScrnInfoPtr pScrn,
                        short srcX, short srcY,
                        short drwX, short drwY,
                        short srcW, short srcH,
                        short drwW, short drwH,
                        RegionPtr pClipBoxes, pointer pData)
{
    XGIPtr          pXGI = XGIPTR(pScrn);
    XGIPortPtr      pXGIPort = (XGIPortPtr)pData;

	/* Jong 11/28/2006; test */
	ErrorF("Jong-XG47PutVideo()...XvMC...\n");

    /*ErrorF("XVMC index = %x\n", pXGIPort->uncompressIndex);*/
    /*ErrorF("XVMC mode  = %s\n", pXGIPort->mode == WEAVE ? "Weave" : "Bob");*/
    /*ErrorF("XVMC subpictureIndex = %s\n", pXGIPort->subpictureIndex ? "Enable" : "Disable");*/
    /*ErrorF("XVMC subpictureIndex = %x\n", pXGIPort->subpictureIndex);*/

    pXGIPort->videoStatus  = CLIENT_VIDEO_ON;
    pXGIPort->vidMode      = VIDEOMODE_YV12;
    pXGIPort->isYUV        = TRUE;
    pXGIPort->isPlanar     = TRUE;
    pXGIPort->isHMirror    = FALSE;
    pXGIPort->isVMirror    = FALSE;

    XG47DisplayDVDVideo(pScrn,
                        srcX, srcY, drwX, drwY,
                        srcW, srcH, drwW, drwH,
                        pClipBoxes, pData);


    if ((pXGIPort->subpictureIndex == 0) || (pXGIPort->subpictureIndex == 1))
    {
        XG47DisplaySubpicture(pScrn, pData);
    }
    else
    {
        ErrorF("Bad subpictureIndex ! \n");
    }

#if 0
    /* update cliplist */
    if (!XG47RegionsEqual(&pXGIPort->clip, pClipBoxes))
    {
        /* update cliplist */
        REGION_COPY(pScrn->pScreen, &pXGIPort->clip, pClipBoxes);
        /* draw these */
        XAAFillSolidRects(pScrn, pXGIPort->colorKey, GXcopy, ~0,
                          REGION_NUM_RECTS(pClipBoxes),
                          REGION_RECTS(pClipBoxes));
    }
#endif
    return Success;

}

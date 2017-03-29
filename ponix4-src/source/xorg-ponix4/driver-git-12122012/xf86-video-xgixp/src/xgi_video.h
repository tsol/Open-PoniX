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

#ifndef _XGI_VIDEO_H_
#define _XGI_VIDEO_H_

#include <X11/extensions/Xv.h>         /* XvSettable ... */
#include "fourcc.h"         /* XVIMAGE_YV12 ... */
#include "dixstruct.h"      /* currentTime */

#undef PI
#define PI              3.141592653589793

#define OFF_DELAY       800  /* milliseconds */
#define FREE_DELAY      60000

#define OFF_TIMER       0x01
#define FREE_TIMER      0x02
#define CLIENT_VIDEO_ON 0x04

#define TIMER_MASK      (OFF_TIMER | FREE_TIMER)

#define XGI_IMAGE_MIN_WIDTH     32
#define XGI_IMAGE_MIN_HEIGHT    24
#define XGI_IMAGE_MAX_WIDTH     2048
#define XGI_IMAGE_MAX_HEIGHT    2048

/*
 * Video format define
 */

#define XGI_VIDEO_YV12      FOURCC_YV12  /* 0x32315659 */
#define XGI_VIDEO_UYVY      FOURCC_UYVY  /* 0x59565955 */
#define XGI_VIDEO_YUY2      FOURCC_YUY2  /* 0x32595559 */
#define XGI_VIDEO_RGB555    0x35315652
#define XGI_VIDEO_RGB565    0x36315652
#define XGI_VIDEO_YVYU      0x55595659
#define XGI_VIDEO_NV12      0x3231564e
#define XGI_VIDEO_NV21      0x3132564e

/* RGB 555 */
#define XGI_IMAGE_RGB555 \
    { \
        XGI_VIDEO_RGB555, \
        XvRGB, \
        LSBFirst, \
        {'R','V','1','5', \
          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, \
        16, \
        XvPacked, \
        1, \
        15, 0x7C00, 0x03E0, 0x001F, \
        0, 0, 0, \
        0, 0, 0, \
        0, 0, 0, \
        {'R', 'V', 'B',0, \
          0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
        XvTopToBottom \
    }

/* RGB 565 */
#define  XGI_IMAGE_RGB565 \
    { \
        XGI_VIDEO_RGB565, \
        XvRGB, \
        LSBFirst, \
        {'R','V','1','6', \
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, \
        16, \
        XvPacked, \
        1, \
        16, 0xF800, 0x07E0, 0x001F, \
        0, 0, 0, \
        0, 0, 0, \
        0, 0, 0, \
        {'R', 'V', 'B',0, \
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
        XvTopToBottom \
    }

/* YVYU */
#define XGI_IMAGE_YVYU \
    { \
        XGI_VIDEO_YVYU, \
        XvYUV, \
        LSBFirst, \
        {'Y','V','Y','U', \
         0x00,0x00,0x00,0x10,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}, \
        16, \
        XvPacked, \
        1, \
        0, 0, 0, 0 , \
        8, 8, 8, \
        1, 2, 2, \
        1, 1, 1, \
        {'Y','V','Y','U', \
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
        XvTopToBottom \
    }

/* NV12 */
#define XGI_IMAGE_NV12 \
    { \
        XGI_VIDEO_NV12, \
        XvYUV, \
        LSBFirst, \
        {'N','V','1','2', \
         0x00,0x00,0x00,0x10,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}, \
        12, \
        XvPlanar, \
        2, \
        0, 0, 0, 0 , \
        8, 8, 8, \
        1, 2, 2, \
        1, 2, 2, \
        {'Y','U','V',0, \
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
        XvTopToBottom \
    }

/* NV21 */
#define XGI_IMAGE_NV21 \
    { \
        XGI_VIDEO_NV21, \
        XvYUV, \
        LSBFirst, \
        {'N','V','2','1', \
         0x00,0x00,0x00,0x10,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}, \
        12, \
        XvPlanar, \
        2, \
        0, 0, 0, 0, \
        8, 8, 8, \
        1, 2, 2, \
        1, 2, 2, \
        {'Y','V','U',0, \
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
        XvTopToBottom \
    }

/* Define OvlAttr
   Including CAPS and Status for overlay */
typedef unsigned long OVLATTR;

/* CAPS */
#define OVLCAPS_FMT_NONE                0
#define OVLCAPS_FMT_RGB8                0x00000001
#define OVLCAPS_FMT_RGB15               0x00000002
#define OVLCAPS_FMT_RGB16               0x00000004
#define OVLCAPS_FMT_RGB24               0x00000008
#define OVLCAPS_FMT_RGB32               0x00000010
#define OVLCAPS_FMT_IF09                0x00000020
#define OVLCAPS_FMT_YUY2                0x00000040
#define OVLCAPS_FMT_YV12                0x00000080
#define OVLCAPS_FMT_YVU9                0x00000100
#define OVLCAPS_FMT_NV12                0x00000200
#define OVLCAPS_FMT_MVCU                0x00000400
#define OVLCAPS_FMT_TDMC                0x00000800

#define OVLCAPS_INTERLEAVED             0x00001000
#define OVLCAPS_HW_OVERLAY              0x00002000       /* Video Overlay (superset of GRACAPS_DDRAW). */
#define OVLCAPS_MOVIE_CORRECTION        0x00004000       /* Enable/Disable 3:2 detection & correction */
#define OVLCAPS_OVERDRIVE               0x00008000       /* Enable/Disable W1 overdrive */

/* Status */
#define OVLST_VIDEO_ON                  0x00010000       /* overlay running */
#define OVLST_VIDEO_ON_W2               0x00020000
#define OVLST_NODEVSWITCH_ON_VIDEO      0x00040000       /* No Device Switch when Video on */
#define OVLST_LARGE_SRC_MODE_ON         0x00080000
#define OVLST_UPDATE                    0x00100000       /* Updated by DdUpdateOverlay */
#define OVLST_FLIP                      0x00200000       /* Flipped by DdFlip */
#define OVLST_SET_POS                   0x00400000       /* Set position by DdSetOverlayPosition */
#define OVLST_CB_MASK                   (OVLST_UPDATE | OVLST_FLIP | OVLST_SET_POS)
#define OVLST_THM_FULLSCREEN            0x00800000       /* Theater mode enable */
#define OVLST_VIDEO_INT_BY_FULLSCN3D    0x01000000       /* Video playback interrupted by full screen 3D */
#define OVLST_VIDEO_INT_BY_FULLSCNOGL   0x02000000       /* Video playback interrupted by full screen OGL */
/* End OVLATTR */

/* Define DvdAttr
   Including CAPS and Status for DVD */
/*typedef unsigned long DVDATTR;*/

/* CAPS
   SW */
#define DVDCAPS_OVERDRIVE               0x00000001       /* W1 overdrive enable */
#define DVDCAPS_ROTATE                  0x00000002

/* HW */
#define DVDCAPS_HW_DXVA                 0x00000100       /* Enable/Disable DXVA */
#define DVDCAPS_HW_DXVA_IDCT            0x00000200       /* Enable/Disable HW IDCT support */
#define DVDCAPS_HW_DXVA_USE_NV12        0x00000400       /* Use NV12 as video format */
#define DVDCAPS_HW_NEC_DXVA_OFF         0x00000800       /* Disable DXVA on non_LCD device for NEC */
#define DVDCAPS_HW_QUALITYENHANCE       0x00001000       /* DVD quality enhance mode */
#define DVDCAPS_HW_SUB_PICTURE_BY_REG   0x00002000       /* Enable/Disable HW SubPicture */
#define DVDCAPS_HW_HDDVD                0x00004000       /* HDDVD (1280 * 720) supporting */
#define DVDCAPS_HW_HDTV                 0x00008000       /* HDTV (1920 * 1080) supporting */

/* Status
   Set / Use in display driver */
#define DVDST_OVERDRIVE_TEMP_DISABLE    0x00010000

/* Playback mode */
#define DVDST_PLAYBACK_MODE_MASK        0x000e0000
/* [17-18]: 11: deinterlace     10: weave   01: bob
 * [19]:    1: movie
 */
#define DVDST_BOB_MODE                  0x00020000
#define DVDST_WEAVE_MODE                0x00040000
#define DVDST_DEINTERLACE_MODE          0x00060000
#define DVDST_MOVIE_MODE                0x00080000

/* Set / Use in miniport driver */
#define DVDST_DISABLE_VPE_IRQ           0x00100000       /* Disable VPE IRQ */

/* Others */
#define DVDST_APP_FAIL                  0x01000000       /* 1:indicate need clear DVD on flag at DDDestroySurface */
#define DVDST_DISABLE_TDMC_BY_REG       0x02000000       /* Disable TDMC interface if cap present */

/* Decode */
#define DVDST_SW_DECODE                 0x08000000       /* SW Decode */
#define DVDST_MC_MVCU_ON                0x10000000       /* MC & IDCT types */
#define DVDST_MC_TDMC_ON                0x20000000
#define DVDST_MC_DXVA_ON                0x40000000
#define DVDST_IDCT_DXVA_ON              0x80000000
#define DVDST_DECODE_MASK               (DVDST_SW_DECODE | DVDST_MC_MVCU_ON | DVDST_MC_TDMC_ON | DVDST_MC_DXVA_ON | DVDST_IDCT_DXVA_ON)
/* End DVDATTR */

/* DXVA definitions */
#define DVDMC_NO_ERROR                      0
#define DVDMC_ERROR_MEMORY_ALLOCATION       1
#define DVDMC_ERROR_INITIALIZATION          3

#define DVD_CMD_SIZE_HDTV                   0x4200000
#define DVD_CMD_SIZE_HDDVD                  0x2080000
/*#define DVD_CMD_SIZE                        0xd00000*/
#define DVD_CMD_SIZE                        0x200000


#define MAX_NO_OF_DXVA_BUFFERS              8       /* max # of DXVA compressed symbol buffers
                                                       request task could be decomposed to */
#define MAX_DXVA_MBBLOCK_NUMBER             MAX_NO_OF_DXVA_BUFFERS
#define MAX_DXVA_UNCOMPRESSED_BUF_NUM       8
#define MAX_DXVA_DECODE_QUEUE_LENGTH        16

#define DVD_PADDING_LINE_ADJUSTMENT         0x80    /* lines to reserve to prevent garbage on the bottom of the screen */

#define FTYPE_I                             0x0001L
#define FTYPE_P                             0x0002L
#define FTYPE_B                             0x0003L

#define DVD_NOT_DECODED                     0
#define DVD_IN_QUEUE                        1
#define DVD_DECODED                         2
#define DVD_DISPLAYED                       3

/* within 20ms (experimental value), if buffer is not available, we should
   let it go, otherwise, dvd drop frame */
#define BEGINFRAME_TIME_OUT                 16
/* if is 4 frame for example powerdvd, use this timer */
#define BEGINFRAME_TIME_OUT_4frame          16

#define MOTION_ENTERWEAVING_THRESHOLD       0x05
#define MOTION_EXITWEAVING_THRESHOLD        0x09
#define MOTION_COUNT_THRESHOLD              0x20

/* Subpicture related */
#define MAX_SP_BUFFERS                      2
#define DVD_SP_ENABLED                      2
#define DVD_SP_HIDEN                        1
#define DVD_SP_DISABLED                     0
#define DVD_SP_3D_Simu                      0x80000000

/* Below is HW related definitions */
#define MAX_PATTERN             64

/* Decoding Command */
#define PIC_BEGIN               0xa800
#define PIC_END                 0xd000
#define MBLK_HEADER             0xb800
#define EOBLK                   0x0
#define ZIGZAG                  0x0
#define ALTERNATE_ZIGZAG        0x4

/* check mask */
#define BITMASK(n)              (1 << (n))
#define MBLK_TYPE_MASK          (BITMASK(0) |   \
                                 BITMASK(1) |   \
                                 BITMASK(2) |   \
                                 BITMASK(3) |   \
                                 BITMASK(5) |   \
                                 BITMASK(6) |   \
                                 BITMASK(7) |   \
                                 BITMASK(8))

#define MBLK_PICTURE_FIELD      BITMASK(5)
#define MBLK_HALF_PEL_H         BITMASK(30)
#define MBLK_HALF_PEL_V         BITMASK(31)

/* Intra Flag */
#define MBLK_TYPE_INTER         0x00000000 /* Inter MacroBlock */
#define MBLK_TYPE_INTRA         0x00000001 /* Intra MacroBlock */

/* CBP Flag */
#define MBLK_CBP_EXIST          0x00000002 /* Exist Code Block Pattern */

/* Prediction Type */
#define MBLK_PREDICT_BWD        0x00000004 /* Backward Prediction */
#define MBLK_PREDICT_FWD        0x00000008 /* Forward Prediction */
#define MBLK_PREDICT_BD         MBLK_PREDICT_FWD | MBLK_PREDICT_BWD /* Bidirection Prediction */
#define MBLK_PREDICT_FRAME      0x00000000 /* Frame Prediction */
#define MBLK_PREDICT_FIELD      0x00000040 /* Field Prediction */
#define MBLK_PREDICT_16X8       0x00000100 /* 16x8 Prediction */
#define MBLK_PREDICT_DP         0x00000200 /* Dual Prime Prediction */

/* DCT Type */
#define MBLK_DCT_FRAME          0x00000000 /* use Frame DCT for current macroblock */
#define MBLK_DCT_FIELD          0x00000080 /* use Field DCT for current macroblock */

/* Motion Vector Exist */
#define MBLK_MV0_EXIST          0x00001000 /* Exist Motion Vector 0 */
#define MBLK_MV1_EXIST          0x00002000 /* Exist Motion Vector 1 */
#define MBLK_MV2_EXIST          0x00004000 /* Exist Motion Vector 2 */
#define MBLK_MV3_EXIST          0x00008000 /* Exist Motion Vector 3 */

/* Reference Field */
#define MBLK_MV0_REF_TOP        0x00000000 /* Use Fwd top field as Ref for 1st Fwd pred. */
#define MBLK_MV0_REF_BOTTOM     0x00010000 /* Use Fwd bottom field as Ref for 1st Fwd pred. */
#define MBLK_MV1_REF_TOP        0x00000000 /* Use Bkwd top field as Ref for 1st Bkwd pred. */
#define MBLK_MV1_REF_BOTTOM     0x00020000 /* Use Bkwd bottom field as Ref for 1st Bkwd pred. */
#define MBLK_MV2_REF_TOP        0x00000000 /* Use Fwd top field as Ref for 2nd Fwd pred. */
#define MBLK_MV2_REF_BOTTOM     0x00040000 /* Use Fwd bottom field as Ref for 2nd Fwd pred. */
#define MBLK_MV3_REF_TOP        0x00000000 /* Use Bkwd top field as Ref for 2nd Bkwd pred. */
#define MBLK_MV3_REF_BOTTOM     0x00080000 /* Use Bkwd bottom field as Ref for 2nd Bkwd pred. */

/* Software bobbing commands */
#ifdef SW_BOBBING
    #define DXVA_BOBBING_ADD_QUEUE              0
    #define DXVA_BOBBING_ADD_CMD                1
    #define DXVA_BOBBING_ENABLE                 2
    #define DXVA_BOBBING_DISABLE                3
    #define DXVA_BOBBING_STATUS_QUERY           4
    #define DXVA_DECODE_STATUS_QUERY            5
    #define DXVA_UNCOMPBUFFER_USED              6
    #define DXVA_BOBBING_FLUSH_CMD              10
    #define DXVA_DECODE_QUEUELENGTH_QUERY       14
#endif
/* End DXVA */

/*
 * bit[2:0] :   WINMD
 * bit[5:4] :   PMDY
 * bit[6]   :   PMDX
 * bit[10:8]:   CMD
 */
typedef enum
{
    VIDEOMODE_UNKNOWN = 0,
    VIDEOMODE_YV12    = 0x001,
    VIDEOMODE_NV12    = 0x003,
    VIDEOMODE_YUY2    = 0x020,
    VIDEOMODE_IF09    = 0x051,
    VIDEOMODE_RGB8_ZOOM_OUT    = 0x104,
    VIDEOMODE_RGB16_ZOOM_OUT   = 0x204,
    VIDEOMODE_RGB15_ZOOM_OUT   = 0x304,
    VIDEOMODE_RGB32_ZOOM_OUT   = 0x404,
    VIDEOMODE_RGB8_ZOOM_IN     = 0x102,
    VIDEOMODE_RGB16_ZOOM_IN    = 0x202,
    VIDEOMODE_RGB15_ZOOM_IN    = 0x302,
    VIDEOMODE_RGB32_ZOOM_IN    = 0x402
} XGIVideoMode;


typedef struct {
    FBLinearPtr     linear;
    RegionRec       clip;
    INT32           colorKey;

    CARD16          hue;
    CARD8           saturation;
    CARD8           brightness;
    INT8            contrast;
    INT32           uncompressIndex;
    INT32           subpictureIndex;
    INT32           mode;

    Bool            isDoubleBuf;
    unsigned char   currentBuf;

    CARD32          id;

    Bool            isSpOverlay;    /* subpicture overlay */
    Bool            isHMirror;
    Bool            isVMirror;
    Bool            isNoColorKey;
    Bool            isEnableHSB;

    CARD16          hStart;
    CARD16          vStart;

    /* Register Related                         W1                  W2 */
    Bool            isPlanar;
    Bool            isYUV;
    XGIVideoMode    vidMode;            /* 3c5.97 3x5.bf<2:0>   3c5.ce 3x5.bf<6:4>  */
    /* Row byte */
    CARD16          rowByteY;           /* 3x5.90-91            3c5.9c-9d           */
    CARD16          rowByteUV;          /* 3c5.9a-9b                                */
    /* Line buffer */
    CARD16          lineBufLevel;       /* 3c5.96                                   */
    CARD16          lineBufThreshold;   /* 3c5.95                                   */
    CARD16          lineBufBreak;       /* 3x5.8e                                   */

    /* Video data starting address */
    CARD32          startAddrY;         /* 3x5.92-94            3c5.86-88           */
    CARD32          startAddrU;         /* 3c5.80-82                                */
    CARD32          startAddrV;         /* 3c5.83-85                                */
    /* Zoom factor */
    CARD16          zoomX;              /* 3x5.80-81            3c5.89-8a           */
    CARD16          zoomY;              /* 3x5.82-83            3c5.8b-8c           */

    BoxRec          srcBox;
    BoxRec          dstBox;

    BoxRec          spSrcBox;
    BoxRec          spDstBox;

    /* DXVA subpicture alpha blending */
    int             spStatus;
    int             spMode;
    CARD32          spStartAddr;
    CARD32          spWidth;
    CARD32          spHeight;
    CARD32          spPitch;

    Bool            isSpPaletteSet;
    Bool            isSpDataReady;

    CARD32          videoStatus;
    Time            offTime;
    Time            freeTime;
} XGIPortRec, *XGIPortPtr;

typedef struct {
  FBLinearPtr       pFBLinear;
  Bool              isOn;
} OffscreenPrivRec, *OffscreenPrivPtr;

#define GET_PORT_PRIVATE(pScrn) \
    (XGIPortPtr)((XGIPTR(pScrn))->pAdaptor->pPortPrivates[0].ptr)

/* xgi_video.c */
extern void XGIInitVideo(ScreenPtr pScreen);

#endif /*_XGI_VIDEO_H*/

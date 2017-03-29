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

#ifndef _XGI_H_
#define _XGI_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/mman.h>
/*#include <sys/ioctl.h>*/

#include "xf86str.h"        /* ScrnInfoPtr */
#ifdef XSERVER_LIBPCIACCESS
#include <pciaccess.h>
#else
#include "xf86Pci.h"        /* PCI config space */
#endif
#ifdef HAVE_XAA_H
#include "xaa.h"            /* XAA and Cursor Support */
#endif
#include "xf86fbman.h"
#include "xf86Cursor.h"
#include "xf86DDC.h"        /* DDC support */
#include "xf86xv.h"         /* Xv support */
#include "shadowfb.h"       /* RefreshAreaFuncPtr */
#include "xf86RamDac.h"     /* RamDacRecPtr */
#include "xf86int10.h"      /* xf86Int10InfoPtr */
#include "vbe.h"            /* vbeInfoPtr */
#include "shadow.h"         /* shadowBufPtr */
#include "vgaHW.h"          /* VGAHWPTR */
#include "xf86.h"           /* xf86Screens */

#define _XF86DRI_SERVER_
#include <GL/glxint.h>
#include "dri.h"

#include "compat-api.h"
/* Jong 09/06/2006; support dual view */
#define	XGIDUALVIEW

#ifndef PCI_CHIP_0047
#define PCI_CHIP_0047   0x0047
#endif

#ifndef PCI_VENDOR_XGI
#define PCI_VENDOR_XGI  0x18CA

#endif

#define XGI_MMIO_SIZE   0x20000

#define XGI_CURSOR_BUF_SIZE (128 * 128 * 4)

#define MAX_VGA_MODE_NO 13

#define NTSC            14.31818
#define PAL             17.73448

#define __DEBUG_FUNC__	

#ifdef __DEBUG_FUNC__
#define ENABLE_HW_SOLIDLINE  (pXGI->EnableSolidLine)
#define ENABLE_HW_SOLIDFILL  (pXGI->EnableSolidFill)
#define ENABLE_HW_SRC2SRC    (pXGI->EnableSrc2Src)
#define ENABLE_HW_8X8PATTERN (pXGI->Enable8x8Pattern)
#define ENABLE_HW_8X8MONOPAT (pXGI->Enable8x8MonoPat)
#define ENABLE_HW_IMAGEWRITE (pXGI->EnableImageWrite)
#else
#define ENABLE_HW_SOLIDLINE   1
#define ENABLE_HW_SOLIDFILL   1
#define ENABLE_HW_SRC2SRC     1
#define ENABLE_HW_8X8PATTERN  1
#define ENABLE_HW_8X8MONOPAT  1
#define ENABLE_HW_IMAGEWRITE  1
#endif

/* Jong 07/26/2006; enable DPMS */
/* #define DPMSExtension */

/* Jong 09/21/2006; support dual view */
/* #define XGI_DUMP_DUALVIEW */
/* #define CURSOR_DEBUG */

/* #ifndef XGI_DUMP
#define XGI_DUMP
#endif */

/*#ifndef XGI_DEBUG
#define XGI_DEBUG
#endif*/

/* Jong 11/28/2006; enable XvExtension */
/* #define XvExtension */

#ifdef XGI_DEBUG
#define XGITRACE(x) \
    do \
    { \
        ErrorF("(**) %s(%d): ", XGI_NAME, pScrn->scrnIndex); \
        ErrorF x; \
    } while (0);
#else
#define XGITRACE(x)
#endif


/*
 * XGI chipset Definitions
 */

/* Supported chipsets */
typedef enum {
    XG47
} XGIChipType;

/*
 * Child device define
 * MAX device numbers
 */
#define MAX_DEVICE_NUM      4
#define LCD_DEVICE_INDEX    0
#define CRT_DEVICE_INDEX    1
#define TV_DEVICE_INDEX     2
#define DVI_DEVICE_INDEX    3
#define DEVICE_INDEX_FLAG   0x80000000

/* Jong 09/11/2006; use low nible of first byte */
#define ST_DISP_LCD         0x00000001
#define ST_DISP_CRT         0x00000002
#define ST_DISP_TV          0x00000004
#define ST_DISP_DVI         0x00000008

/* Jong 09/11/2006; use high nible of first byte */
#define ST_DISP_LCD_MV      (ST_DISP_LCD << MAX_DEVICE_NUM)
#define ST_DISP_CRT_MV      (ST_DISP_CRT << MAX_DEVICE_NUM)
#define ST_DISP_TV_MV       (ST_DISP_TV << MAX_DEVICE_NUM)
#define ST_DISP_DVI_MV      (ST_DISP_DVI << MAX_DEVICE_NUM)
#define ST_DISP_VID         (1 << (2 * MAX_DEVICE_NUM))

#define ST_DISP_SINGLE      ((1 << MAX_DEVICE_NUM) - 1)
#define ST_DISP_DUALVIEW    (((1 << MAX_DEVICE_NUM) - 1) << MAX_DEVICE_NUM)
#define ST_DISP_MASK        (ST_DISP_DUALVIEW | ST_DISP_SINGLE)

/* Define DevStatus
   Two HWDeviceExtensions have different status. */
typedef unsigned long DEVSTATUS;

#define DEVST_MHS_DUALVIEW_ENABLE       0x00000001         /* DualView(MHS) enabled. */

/* For Set Mode */
#define DEVST_SIMULATE_MODE             0x00000010
#define DEVST_DOS_FULL_SCREEN           0x00000020

/* For Device */
#define DEVST_PANEL_EXPANSION           0x00000040       /*Full screen expansion mode */
#define DEVST_PANEL_CENTERING           0x00000080       /*Screen centering mode */
#define DEVST_PANEL_V_EXPANSION         0x00000100       /*Aspect ratio expansion mode */
#define DEVST_PANEL_EXPANSION_MASK      (DEVST_PANEL_EXPANSION | DEVST_PANEL_CENTERING | DEVST_PANEL_V_EXPANSION)
#define DEVST_LCDTVONDESKTOP            0x00000200       /* LCD+TV when enter DOS full screen */

/* Misc */
#define DEVST_GAMMA_ENABLED             0x00000400
/* End DEVSTATUS */

/* Define HwStatus */
/* The current display adapter status */
typedef unsigned long HWSTATUS;

/*#define HWST_OVERLAY_PDEV_IDX_MASK  0x00000003*/       /* two bits for PdevIdx (0 to 3), support up to  four indices */
/*#define HWST_OVERLAY_USED           0x00000004*/       /* overlay running */

/* Set / Use just in display driver */

/* Set / Use just in miniport */
#define HWST_RESUME_FROM_STANDBY    0x00010000       /* Resume From Standby */
#define HWST_NEED_INITIALIZE_DTV    0x00040000
#define HWST_POWER_BOOTRESUME       0x00100000

#define HWST_GE_DISABLED            0x20000000
/* End HwStatus */
/* Define GraCaps */
/* caps */
typedef unsigned long GRACAPS;

/* For HW      (0  --- 11) */
#define GRACAPS_PANNING                 0x00000001       /* Enable/Disable PANNING ablility */
#define GRACAPS_CURSOR64                0x00000002       /* Is hw support 32*32 bits cursor? Michael for Monterey */
#define GRACAPS_MMX                     0x00000004       /* actually CPU's cap. */
#define GRACAPS_PCI_CMDLIST_OFF_BY_REG  0x00000008       /* PCI CommandList Off */
#define GRACAPS_GAMMA_BY_REG            0x00000010       /* Enable gamma correction */
#define GRACAPS_POWER_SAVING_ON_BY_REG  0x00000020       /* Enable Power Saving Mode */

/* For utility (12 --- 31) */
#define GRACAPS_MODE_FOR_EACH_USER      0x00001000       /* different resolution for each user */
#define GRACAPS_OVERLAY_FULLSCREEN      0x00002000       /* overlay fullscreen in secondary device under contain mode */
#define GRACAPS_HOTKEY_ROTATION         0x00004000       /* enable hotkey loop under rotation mode */
#define GRACAPS_SWAP_HEAD               0x00008000       /* swap head under MHS */
#define GRACAPS_HOTKEY_MHS              0x00020000       /* enable hotkey under MHS */
#define GRACAPS_LCDLID_CLOSED           0x00040000       /* close LCD when LCD lid closes */
#define GRACAPS_DISABLE_PANNING_BY_REG  0x00080000       /* If this bit set, we do not report modes bigger than device physical size on LCD & DVI */
#define GRACAPS_TABLET_PC_BY_REG        0x00100000       /* Tablet PC. */
#define GRACAPS_3D_TEXTURE_CAPS_BY_REG  0x00200000       /* Adjust texture capability */

#ifdef T_ROTATION
#define GRACAPS_ROTATION_BY_REG         0x00400000       /* Adjust texture capability */
#endif

#define GRACAPS_CONTROL_HSCB            0x00800000       /* If the Utility can adjust HSCB */

/* End GRACAPS */

/* Define HwCaps */
typedef unsigned long HWCAPS;

#define HWCAPS_NONE                    0x00000000

/* For PCI configuration */
#define HWCAPS_AGP                     0x00000001       /* AGP caps */
#define HWCAPS_INTERNALBUS_64          0x00000002       /* Force 64 Bits Internal Bus only */
#define HWCAPS_INTERNALBUS_128         0x00000004       /* Using 64/128 Bit Internal Bus */
#define HWCAPS_DUAL_PORT               0x00000008       /* Enable/Disable Dual Port */
#define HWCAPS_LOW_LEVEL_PRODUCT       0x00000010       /* Low level product (<= 32M video memory) */

/* For Device */
#define HWCAPS_CRT                     0x00000100       /* support CRT output */
#define HWCAPS_LCD                     0x00000200       /* LCD chips, we can ask BIOS. */
#define HWCAPS_TV                      0x00000400       /* TV display */
#define HWCAPS_DVI                     0x00000800       /* DVI display */
#define HWCAPS_MUTEX_DVI_CRT           0x00001000       /* We can't turn on DVI+CRT together */
                                                        /* For example, 7116 card */

/* For D3D */
#define HWCAPS_PURE_DEVICE             0x00010000       /* Enable/Disable Pure Device */

/* Others */
#define HWCAPS_CURSOR_IN_DUALVIEW      0x01000000       /* For HW cursor support at LCD view of DUAL VIEW mode. */
#define HWCAPS_ALPHA_HW_MAX            0x02000000       /* Full Hardware Alpha cursor support */
#define HWCAPS_POWER_SAVING            0x04000000       /* Power Saving enable. */
#define HWCAPS_GAMMA                   0x08000000       /* Support Gamma */

/* End DEVCAPS */

/* Define GRASTATUS */
typedef unsigned long GRASTATUS;

/* Graphic Status */
#define GST_NO_ACCELERATOR              0x00000001          /* No accelerator */
#define GST_SW_CURSOR                   0x00000002          /* no hardware pointer. */
#define GST_INTERLACED                  0x00000004
#define GST_DDRAW                       0x00000008          /* directDraw enable. */
#define GST_HWSP_ENABLED                0x00000010          /* h/w subpicture feature enabled */
#define GST_ALPHA_CURSOR                0x00000020          /* Alpha cursor enabled. */
#define GST_ROTATION                    0x00000040          /* Rotation definition */
#define GST_DIB                         0x00000080
#define GST_AGP_ENABLE                  0x00000100          /* AGP version */
#define GST_PCIE_ENABLE                 0x00000200          /* PCI-E version */

/* End GRASTATUS */

typedef struct xgi_regs {
    /**
     * Extended registers at 0x3cf
     */
    uint8_t gra[0x10];


    /**
     * Extended CRTC control registers
     */
    uint8_t crtc[0x100];


    /**
     * Extended sequencer registers
     */
    uint8_t seq[0x29];


    /**
     * Alternate clock select
     * 
     * [7  ] - Enable display memory clock divide by 2
     * [6:5] - Video clock divide
     *     0 0 = divide by 1
     *     0 1 = divide by 2
     *     1 0 = divide by 3
     *     1 1 = divide by 4
     * [4:2] - Reserved
     * [1:0] - Video clock select
     *
     * Stored at 0x3db
     */
    uint8_t alt_clock_select;


    /**
     * SYNDAC command register
     */
    uint8_t syndac_command;
} *XGIRegPtr;

/* modeNo:
 * |--------'--------|
 *  |||||||| ||||||||
 *  |||||||| |'''''''- TVGA video mode number.
 *  |||||||| '-------- flag of clear screen.
 *  |'''''''---------- Base video mode number.
 *  '----------------- reserved.
 *
 * condition:
 * |--------'--------|--------'--------|
 *  |||||||| |||||||| |||||||| ||||||||
 *  |||||||| |||||||| |||||||| |||||||'- support with LCD.
 *  |||||||| |||||||| |||||||| ||||||'-- support with CRT.
 *  |||||||| |||||||| |||||||| |||||'--- support with LCD & CRT.
 *  |||||||| |||||||| |||||||| ||||'---- support with TV.
 *  |||||||| |||||||| |||||||| |||'----- support with LCD & TV.
 *  |||||||| |||||||| |||||||| ||'------ support with TV & CRT.
 *  |||||||| |||||||| |||||||| |'------- support with LCD & CRT & TV.
 *  |||||||| |||||||| |||||||| '-------- Dual view support.
 *  |||||||| |||||||| |||||||'---------- LCD standard (TFT/STN).
 *  |||||||| |||||||| ||||||'----------- Centering/Expansion
 *  |||||||| |||||||| ''''''------------ reserved for LCD.
 *  |||||||| |||||||'------------------- TV standard (NTSC).
 *  |||||||| ||||||'-------------------- TV standard (PAL).
 *  |||||||| |||||'--------------------- TV scanline (underscan).
 *  |||||||| ||||'---------------------- TV scanline (overscan).
 *  |||||||| ''''----------------------- reserved for TV.
 *  |'''''''---------------------------- reserved.
 *  '----------------------------------- Extended/Standard set mode method(1200/mode no).
 */
typedef struct {
    CARD16  width;              /* width of the graphic mode in pixel. */
    CARD16  height;             /* height of the graphic mode in scanline.*/
    CARD16  pixelSize;          /* bits/pixel of the graphic mode. */
    CARD16  refRate;
    CARD16  refSupport;         /* bios will fill this field when call bios */
    CARD16  modeNo;             /* video mode number. */
    CARD32  condition;
} XGIAskModeRec, *XGIAskModePtr;


/*
 * DTV_INFO : Data type for control the TVX
 *          : CARD16, CARD16 - Save the BIOS default value
 *          : CARD8       - Total swing width
 *          : CARD8       - Logical default value
 *          : CARD8       - Current offset (Registery save value)
 *          : CARD8       - Delta
 *          0----------------+----------|----------N
 *      Bios default    defoffset   Curoffset
 *          |<-----------  Swing value ----------->|
 */
typedef struct {
    CARD16      low;                /* BIOS Default */
    CARD16      high;
    CARD8       swingwid;           /* Swing Width */
    CARD8       defoffset;          /* Default Offset */
    CARD8       curoffset;          /* Current Offset */
    CARD8       delta;              /* Delta unit */
} XGIDigitalTVInfoRec, *XGIDigitalTVInfoPtr;

typedef enum
{
    DISPHEAD_CRTC = 1,
    DISPHEAD_WIN2 = 2
} XGIHeadID;


/* HWMC Surfaces */
#define XGI_MAX_SURFACES        8
#define XGI_MAX_SUBPICTURES     2
#define XGI_TOTAL_SURFACES      10

typedef struct {
    /* DVDATTR       dvdAttr; */

    unsigned long IOAddr;
    unsigned long fbAddr;
    unsigned long IOSize;
    unsigned long fbSize;

    /* temp data */
    int           bobFlag;            /* bob or weave mode */

} XGIXvMCCreateContextRec, *XGIXvMCCreateContextPtr;

typedef struct {
    int             index;
    unsigned long   pitch;
    unsigned long   size;
    unsigned long   hwAddr;

    unsigned long   offsetY;
    unsigned long   offsetV;
    unsigned long   offsetU;
} XGIXvMCCreateSurfaceRec, *XGIXvMCCreateSurfacePtr;

/*
typedef struct {
    int             index;
    unsigned int    width;
    unsigned int    pitch;
    unsigned int    size;
    unsigned int    hwAddr;
} XGIXvMCSubpicSurfaceRec, *XGIXvMCSubpicSurfacePtr;
*/
typedef struct {
    unsigned int    pitch;
    unsigned int    size;
    unsigned int    hwAddr;
} XGIXvMCSubpictureRec, *XGIXvMCSubpicturePtr;


typedef unsigned long DVDATTR;

/* Jong 09/11/2006; Private Entity structure for dual view */
#ifdef XGIDUALVIEW
typedef struct {
    int			lastInstance;		/* number of entities */
} XGIEntityRec, *XGIEntityPtr;
#endif

typedef struct {
    ScrnInfoPtr         pScrn;
#ifdef XSERVER_LIBPCIACCESS
    struct pci_device * pPciInfo;
#else
    pciVideoPtr         pPciInfo;
    PCITAG              pciTag;
#endif
    EntityInfoPtr       pEnt;

    int                 chipset;

    int                 dacType;
    int                 dacSpeed;
    int                 pix24bpp;       /* Depth of pixmap for 24bpp framebuffer */
    Bool                isDac8bits;     /* Use 8 bit DAC? */

#ifndef XSERVER_LIBPCIACCESS
    unsigned long       IOAddr;
    unsigned long       fbAddr;
#endif
    unsigned char *     IOBase;
    unsigned char *     fbBase;
    unsigned long       fbSize;
    unsigned long       PIOBase;

    int                 drm_fd;
    DRIInfoPtr          dri_info;

    drm_handle_t        fb_handle;
    drm_handle_t        gart_handle;
    drm_handle_t        mmio_handle;

    uint8_t           * gart_vaddr;
    drmSize             gart_size;

    Bool                (*DRICloseScreen)(int, ScreenPtr);
    Bool                directRenderingEnabled;
    Bool                dri_screen_open;


    CARD32              biosFbSize;     /* bios report frame buffer size */
    CARD32              freeFbSize;
    CARD16              maxBandwidth;
    Bool                isInterpolation;

    Bool                isFBDev;
    Bool                isDDRAM;

    OptionInfoPtr       pOptionInfo;
    Bool                noAccel;
    Bool                noMMIO;
    Bool                isHWCursor;
    Bool                isMmioOnly;
    Bool                isShadowFB;

    CARD32              displayDevice;            /* the current display device */
    DEVSTATUS           deviceStatus;             /* Device status, like DualViewLoop, DualView, ... */
    GRASTATUS           graStatus;                /* Engine status */

    Bool                isGammaBrightnessOn;
    int                 brightness;
    double              gamma;

    Bool                isNeedShadow;
    Bool                isNeedStretch;

    int                 minClock;
    int                 maxClock;

    XGIDigitalTVInfoPtr pDtvInfo;

    int                 currentClock;
    int                 rotate;
    float               frequency;

    CARD8 *             pShadow;
    int                 shadowPitch;
    int                 numDgaModes;
    DGAModePtr          pDgaModes;
    Bool                isDgaActive;
    int                 dgaViewportStatus;

    RefreshAreaFuncPtr  RefreshArea;
    CloseScreenProcPtr  CloseScreen;
    ScreenBlockHandlerProcPtr   BlockHandler;
    xf86PointerMovedProc        *PointerMoved;

    struct xgi_regs           savedReg;
    struct xgi_regs           modeReg;

    I2CBusPtr           pI2C;
    I2CBusPtr           pI2C_dvi;

    CARD16              engineOperation;
    CARD32              bltScanDirection;

    CARD32              patternLocation;
    CARD32              drawFlag;
    CARD16              linePattern;

    RamDacRecPtr        pRamDac;

    /**
     * \name Cursor related data.
     */
    /*@{*/
    Bool                cursor_argb;    /**< Is ARGB cursor in use? */
    xf86CursorInfoPtr   pCursorInfo;    /**< Current core cursor info. */
    uint32_t            cursorStart;    /**< Hardware cursor start addr. */
    uint32_t            cursorEnd;      /**< Hardware cursor end addr. */
    /*@}*/

    xf86Int10InfoPtr    pInt10;
    vbeInfoPtr          pVbe;

    XGIHeadID           headID;
#ifdef HAVE_XAA_H
    XAAInfoRecPtr       pXaaInfo;
#endif
    struct xg47_CmdList *cmdList;

#ifdef XvExtension 
    void                (*VideoTimerCallback)(ScrnInfoPtr, Time);
    XF86VideoAdaptorPtr pAdaptor;
    int                 videoKey;
    CARD32              ovlAttr;    /* overlay attribute */
#endif 

    /* XvMC */
    DVDATTR                     dvdAttr;
    XGIXvMCCreateContextRec     xvmcContext;
    XGIXvMCCreateSurfaceRec     xvmcSurface[XGI_MAX_SURFACES];
    XID                         surfID[XGI_MAX_SURFACES];
    XGIXvMCSubpictureRec        xvmcSubpic;
    XID                         spID;

#ifdef __DEBUG_FUNC__
    Bool              EnableSolidLine;
    Bool              EnableSolidFill;
    Bool              EnableSrc2Src;
    Bool              Enable8x8Pattern;
    Bool              Enable8x8MonoPat;
    Bool              EnableImageWrite;
#endif

#ifdef XGIDUALVIEW
    XGIEntityPtr	  pEntityPrivate;		
	Bool			  FirstView; /* Jong 09/11/2006; support dual view */
	unsigned int	  ScreenIndex; /* 0:first view; 1:second view */
#endif
} XGIRec, *XGIPtr;

struct XGIDRIContext {
    int    dummy;
};

#define XGIPTR(p)   ((XGIPtr)((p)->driverPrivate))

extern Bool XGIDRIScreenInit(ScreenPtr pScreen);
extern Bool XGIDRIFinishScreenInit(ScreenPtr pScreen);
extern void XGIDRICloseScreen(ScreenPtr pScreen);

#endif

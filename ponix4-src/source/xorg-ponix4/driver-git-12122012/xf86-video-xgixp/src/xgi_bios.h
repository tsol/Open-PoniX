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

#ifndef _XGI_BIOS_H_
#define _XGI_BIOS_H_

/*
 *
 */
typedef struct {
    CARD16    xres;
    CARD16    yres;
    CARD16    vref;    /* Hz */
    CARD16    vclk;    /* MHz */
} XGIPixelClockRec, *XGIPixelClockPtr;

/*
 * 3D4/3D5 58  R/W Software Scratch Pad 7
 * 3D4/3D5 59  R/W Software Scratch Pad 14
 * 3D4/3D5 5A  R/W Software Scratch Pad 15
 */
#define SOFT_PAD58_INDEX                0x58
#define SOFT_PAD59_INDEX                0x59
#define SOFT_PAD5A_INDEX                0x5A

/*
 * TV Screen  positioin.
 */
#define     INVALID_VALUE               0xffffffff
#define     CHANGE_ON_HORIZONTAL        0x0001
#define     CHANGE_ON_VERTICAL          0x0002

/*
 * TV IP Core
 */
#define TV_TVX                          0x00000000      /*TVX*/
#define TV_TVX1K                        0x00000001      /*TVX 1K support*/
#define TV_TVX2                         0x00000002      /*TVX2 External*/
#define TV_TVXI                         0x00000003      /*TVX2 Internal*/
#define TV_CHRONTEL_7004                0x10000004      /*CH7003/04*/
#define TV_CHRONTEL_7007                0x10000007      /*CH7007/08*/
#define TV_CHRONTEL_7009                0x10000009      /*CH7009/11*/
#define TV_PHILIPS_7102                 0x20000001      /*PH7102*/
#define TV_NEW_INTERFACE                0x80000000      /*TVX/TVX2 New IF*/
#define TV_INVALID                      0xffffffff
/*
 *  Digital TV Information.
 */
typedef struct {
    CARD16        maxLevel1;
    CARD16        maxLevel2;
    CARD16        delta;
    CARD16        direction;
} XGIDigitalTVRec, *XGIDigitalTVPtr;

typedef struct {
    unsigned long       ret;
    XGIDigitalTVRec     info[3];
} XGIDigitalTVColorRec;

typedef struct {
    unsigned long       ret;
    XGIDigitalTVRec     info[2];
} XGIDigitalTVPositionRec;

typedef struct {
    unsigned long       ret;
    XGIDigitalTVRec     info;
} XGIDigitalTVCallRec;


#define XGI_REG_ARX           0x03C0      /* attribute index register. */
#define XGI_REG_SRX           0x03C4      /* sequencer index register. */
#define XGI_REG_CRX           0x03D4      /* CRT index register.       */
#define XGI_REG_GRX           0x03CE      /* graphics index register.  */
#define XGI_REG_PRX           0x03A4      /* flat panel shadow register. */

/*
 * definition for BIOS_Capability
 */
#define BIOS_LCD_SUPPORT                0x00000001
#define BIOS_CRT_SUPPORT                0x00000002
#define BIOS_TV_SUPPORT                 0x00000004
#define BIOS_DVI_SUPPORT                0x00000008
#define BIOS_VIRTUAL_SCREEN             0x00000010
#define BIOS_DUALCRT_SUPPORT            0x00000040
#define BIOS_DEVICE_SWITCH              0x00000100
#define BIOS_TV_1K_SUPPORT              0x00000200
#define BIOS_SMART_EXPANSION            0x00000400
#define BIOS_TV_CAPABILITY              0x00010000
#define BIOS_MV7_DISABLED               0x00020000

/*
 * definition for Device Support
 */
/* bit[7:0] */
#define SUPPORT_DEV_LCD                0x00000001
#define SUPPORT_DEV_CRT                0x00000002
#define SUPPORT_DEV_TV                 0x00000004
#define SUPPORT_DEV_DVI                0x00000008
/* bit [15:8] LCD Information */
#define SUPPORT_PANEL_EXPANSION         0x00000400
#define SUPPORT_PANEL_CENTERING         0x00000800
#define SUPPORT_PANEL_V_EXPANSION       0x00001000      /*4:3 Expanded Centering*/
#define SUPPORT_PANEL_FULLEXP           0x00001000      /* NEC request */
#define SUPPORT_PANEL_SMARTEXP          0x00002000
/* bit [23:16] TV information */
#define SUPPORT_TV_NTSC                 0x00010000
#define SUPPORT_TV_PAL                  0x00020000
#define SUPPORT_TV_UNDERSCAN            0x00040000
#define SUPPORT_TV_OVERSCAN             0x00080000
#define SUPPORT_TV_NATIVE               0x00100000
#define SUPPORT_CURRENT_NO_TV           0x00200000
#define SUPPORT_TV_PALM                 0x00400000
#define SUPPORT_TV_DIGITAL              0x00800000      /* - XP5/V3 */
#define SUPPORT_TV_NTSCJ                0x00080000      /* XP10/XG47 -*/
/* bit [31:24] Second view information */
#define SUPPORT_DEV2_LCD                0x01000000
#define SUPPORT_DEV2_CRT                0x02000000
#define SUPPORT_DEV2_TV                 0x04000000
#define SUPPORT_DEV2_DVI                0x08000000
#define SUPPORT_W2_CLOSE                0x40000000

/*
 * flat panel defines
 */
#define FPTYPE_UNKNOWN  0
#define FPTYPE_TFT      1
#define FPTYPE_HALF     2

#undef FPSIZE_6X4
#undef FPSIZE_8X4
#undef FPSIZE_8X6
#undef FPSIZE_10X4
#undef FPSIZE_10X6
#undef FPSIZE_10X7
#undef FPSIZE_12X6
#undef FPSIZE_12X10
#undef FPSIZE_14X10
#undef FPSIZE_16X12

#define FPSIZE_UNKNOWN  0
#define FPSIZE_6X4      1
#define FPSIZE_8X4      2
#define FPSIZE_8X6      3
#define FPSIZE_10X4     4
#define FPSIZE_10X6     5
#define FPSIZE_10X7     6
#if defined T_FXP || defined T_CBAi1 || defined T_CARMEL
#define FPSIZE_12X10    7
#define FPSIZE_14X10    8
#define FPSIZE_16X12    9
#else
#define FPSIZE_12X6     7
#define FPSIZE_12X10    8
#define FPSIZE_14X10    9
#define FPSIZE_16X12    0xA
#endif
/* To keep compatibility with KB2.5 old BIOS(KTT 6.0)*/
#define FPSIZE_8X6_KB   2
#define FPSIZE_10X7_KB  3
#define FPSIZE_12X10_KB 4
#define FPSIZE_10X6_KB  5

#define FPTYPE_UNKNOWN  0
#define FPTYPE_TFT      1
#define FPTYPE_HALF     2

#define LCD_LID_OPEN    0
#define LCD_LID_CLOSE   1
#define LCD_LID_STATUS_UNKNOWN  2

/*
 * Following define copied from CMDLIST.H from BIOSDLL file.
 *
 * BIOSDLL call Command definition
 */
#define CLOSE_ALL_DEVICE                20400
#define OPEN_ALL_DEVICE                 20401

/*
 * The following command list is defined for Digital TV
 * and used by all of BIOSDLL & UTILITY & DRIVER
 */
#define INIT_TV_SCREEN                  30000

#define ENABLE_TV_DISPLAY               30014
#define DISABLE_TV_DISPLAY              30015

#define DTV_TVEXPRESS_XP4E  3

/*
 *
 */
#define DEV_SUPPORT_LCD         0x0001
#define DEV_SUPPORT_CRT         0x0002
#define DEV_SUPPORT_TV          0x0004
#define DEV_SUPPORT_DVI         0x0008
#define DEV_SUPPORT_VIRTUAL     0x0010

/* Jong 09/12/2006; device attributes */
#define ZVMX_MASK_LCDCTRL       0x0000FF00  /* mask of LCD control bits. */
#define ZVMX_ATTRIB_TFT         0x00000100  /* TFT flat panel type. */
#define ZVMX_ATTRIB_DSTN        0x00000200  /* DSTN flat panel type. */
#define ZVMX_ATTRIB_EXPANSION   0x00000400  /* Expension. */
#define ZVMX_ATTRIB_CENTERING   0x00000800  /* Centering. */
#define ZVMX_ATTRIB_LCD_ALL     0x00000F00  /* LCD support all. */
#define ZVMX_ATTRIB_V_EXPANSION 0x00001000  /* 4:3 Expanded Centering. */

#define ZVMX_ATTRIB_OVERLAYFOCUS    0x00001000  /* LCD Overlay. */

#define ZVMX_MASK_TVCTRL        0x00FF0000  /* mask of TV control bits. */
#define ZVMX_ATTRIB_NTSC        0x00010000  /* TV standard is NTSC. */
#define ZVMX_ATTRIB_PAL         0x00020000  /* TV standard is PAL. */
#define ZVMX_ATTRIB_PALM        0x00400000  /* TV PAL_M */
#define ZVMX_ATTRIB_NTSCJ       0x00800000  /* TV NTSC-J */
#define ZVMX_ATTRIB_UNDER       0x00040000  /* TV is underscan. */
#define ZVMX_ATTRIB_OVER        0x00080000  /* TV is overscan. */
#define ZVMX_ATTRIB_NATIVE      0x00100000  /* TV is native. */
#define ZVMX_ATTRIB_TV_ALL      0x000F0000  /* TV support all. */
#define TMOD_SUPPORT_NTSCU      0x10        /* MODE SUPPORT NTSC UNDERSCAN. */
#define TMOD_SUPPORT_NTSCO      0x20        /* MODE SUPPORT NTSC OVERSCAN. */
#define TMOD_SUPPORT_PALU       0x40        /* MODE SUPPORT PAL UNDERSCAN. */
#define TMOD_SUPPORT_PALO       0x80        /* MODE SUPPORT PAL OVERSCAN. */

/*
 * 3CF.5D.[6:4,0]
 */
#define TEXT_EXPANSION          0x0001
#define TEXT_V_EXPANSION        0x0010
#define GRAF_EXPANSION          0x0020
#define GRAF_V_EXPANSION        0x0040
#define EXPANSION_MASK          GRAF_EXPANSION+TEXT_EXPANSION
#define V_EXPANSION_MASK        GRAF_V_EXPANSION+TEXT_V_EXPANSION

/* BIOS setmode */
#define BIOS_EXPANSION          0x0010
#define BIOS_V_EXPANSION        0x0020

/*
 * mode refresh defines
 */
#define VMODE_REF_MASK              0x0000FFFF
#define VMODE_REF_RESERVED          0x00000000
#define VMODE_REF_44Hz              0x00000001
#define VMODE_REF_48Hz              0x00000002
#define VMODE_REF_50iHz             0x00000004
#define VMODE_REF_50rHz             0x00000008
#define VMODE_REF_50Hz              0x00000010
#define VMODE_REF_60iHz             0x00000020
#define VMODE_REF_60rHz             0x00000040
#define VMODE_REF_60Hz              0x00000080
#define VMODE_REF_70Hz              0x00000100
#define VMODE_REF_72Hz              0x00000200
#define VMODE_REF_75Hz              0x00000400
#define VMODE_REF_85Hz              0x00000800
#define VMODE_REF_100Hz             0x00001000
#define VMODE_REF_120Hz             0x00002000
#define VREF_MAX_NUMBER             15
#define VREF_RESERVED               (-1)
#define VREF_44Hz                   44
#define VREF_48Hz                   48
#define VREF_50iHz                  0xb2
#define VREF_50rHz                  0x132
#define VREF_50Hz                   50
#define VREF_60iHz                  0xbc
#define VREF_60rHz                  0x13c
#define VREF_60Hz                   60
#define VREF_70Hz                   70
#define VREF_72Hz                   72
#define VREF_75Hz                   75
#define VREF_85Hz                   85
#define VREF_100Hz                  100
#define VREF_120Hz                  120
#define ZVMX_MASK_REFINDEX      0x000F0000  /* mask of refresh rate index. */
#define ZVMX_INDEX_DEFAULT      0x00000000  /* vido BIOS default value. */
#define ZVMX_INDEX_REF44        0x00010000  /* 87 interlaced. */
#define ZVMX_INDEX_REF48        0x00020000  /* 96 interlaced. */
#define ZVMX_INDEX_REF50i       0x00030000  /* 50 interlaced. */
#define ZVMX_INDEX_REF50r       0x00040000  /* 50 reduced blank. */
#define ZVMX_INDEX_REF50        0x00050000  /* 50 Hz. */
#define ZVMX_INDEX_REF60i       0x00060000  /* 60 interlaced. */
#define ZVMX_INDEX_REF60r       0x00070000  /* 60 reduced blank. */
#define ZVMX_INDEX_REF60        0x00080000  /* 60 Hz. */
#define ZVMX_INDEX_REF70        0x00090000  /* 70 Hz. */
#define ZVMX_INDEX_REF72        0x000A0000  /* 72 Hz. */
#define ZVMX_INDEX_REF75        0x000B0000  /* 75 Hz. */
#define ZVMX_INDEX_REF85        0x000C0000  /* 85 Hz. */
#define ZVMX_INDEX_REF100       0x000D0000  /* 100 Hz. */
#define ZVMX_INDEX_REF120       0x000E0000  /* 120 Hz. */


/*
 * monitor defines
 */
#define MONITOR_X320                320
#define MONITOR_Y200                200
#define MONITOR_Y240                240
#define MONITOR_X400                400
#define MONITOR_Y300                300
#define MONITOR_X512                512
#define MONITOR_Y384                384
#define MONITOR_X640                640
#define MONITOR_Y400                400
#define MONITOR_Y420                420
#define MONITOR_Y432                432
#define MONITOR_Y480                480
#define MONITOR_X720                720
#define MONITOR_Y514                514
#define MONITOR_Y576                576
#define MONITOR_Y720                720
#define MONITOR_X800                800
#define MONITOR_Y800                800
#define MONITOR_X848                848
#define MONITOR_X864                864
#define MONITOR_Y600                600
#define MONITOR_X1024               1024
#define MONITOR_Y768                768
#define MONITOR_X1152               1152
#define MONITOR_Y864                864
#define MONITOR_X1280               1280
#define MONITOR_Y1024               1024
#define MONITOR_X1400               1400
#define MONITOR_Y1050               1050
#define MONITOR_Y1080               1080
#define MONITOR_X1440               1440
#define MONITOR_Y900                900
#define MONITOR_Y960                960
#define MONITOR_X1600               1600
#define MONITOR_X1680               1680
#define MONITOR_Y1200               1200
#define MONITOR_X1920               1920
#define MONITOR_Y1440               1440
#define MONITOR_X2048               2048
#define MONITOR_Y1536               1536

/*
 * bpp defines
 */
#define BITSPIXEL_08                8
#define BITSPIXEL_16                16
#define BITSPIXEL_24                24
#define BITSPIXEL_32                32

/*
 * for BiosDllOperationFlag
 */
#define NEED_3CF33_ON_DELAY     0x00000001
#define DEVICE_CLOSED           0x00000002

#endif


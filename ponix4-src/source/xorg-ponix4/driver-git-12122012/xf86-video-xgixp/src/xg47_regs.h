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

#ifndef _XG47_REGS_H_
#define _XG47_REGS_H_


#define BASE_2D_ENG                 0X2100
#define BASE_3D_ENG                 0X2800
#define BASE_MC                     0X2200
#define BASE_AGP                    0X2300

#define ENG_STATUS                  (0x20 + BASE_2D_ENG)
#define ENG_COMMAND                 (0x24 + BASE_2D_ENG)
#define ENG_COLOR_FMT               (0x25 + BASE_2D_ENG)
#define ENG_ROP3                    (0x27 + BASE_2D_ENG)
#define ENG_DRAWINGFLAG             (0x28 + BASE_2D_ENG)
#define ENG_FGCOLOR                 (0x2C + BASE_2D_ENG)
#define ENG_BKCOLOR                 (0x30 + BASE_2D_ENG)
#define ENG_DEST_COLORKEY           (0x34 + BASE_2D_ENG)
#define ENG_DESTXY                  (0x38 + BASE_2D_ENG)
#define ENG_SRCXY                   (0x3C + BASE_2D_ENG)
#define ENG_K1                      (0x3C + BASE_2D_ENG)
#define ENG_K2                      (0x3E + BASE_2D_ENG)
#define ENG_DIMENSION               (0x40 + BASE_2D_ENG)
#define ENG_DIMENSION_WIDTH         (0x42 + BASE_2D_ENG)
#define ENG_LENGTH                  (0x40 + BASE_2D_ENG)
#define ENG_ERRTERM                 (0x42 + BASE_2D_ENG)
#define ENG_PENSTYLEMASK            (0x44 + BASE_2D_ENG)
#define ENG_CLIPSTARTXY             (0x48 + BASE_2D_ENG)
#define ENG_CLIPENDXY               (0x4C + BASE_2D_ENG)
#define ENG_DST_BASE                (0x50 + BASE_2D_ENG)
#define ENG_SRC_BASE                (0x54 + BASE_2D_ENG)
#define ENG_PENCOLOR                (0x58 + BASE_2D_ENG)
#define ENG_PATTERN_FG              (0x58 + BASE_2D_ENG)
#define ENG_PATTERN_BG              (0x5C + BASE_2D_ENG)
#define ENG_BW_MASK                 (0x5C + BASE_2D_ENG)
#define ENG_DISP_BASEADDR1          (0x60 + BASE_2D_ENG)
#define ENG_DISP_BASEADDR2          (0x64 + BASE_2D_ENG)
#define ENG_VERTICAL_SCAN           (0x68 + BASE_2D_ENG)
#define ENG_DISP_BASEADDR           (0x6C + BASE_2D_ENG)
#define ENG_PATTERN                 (0x80 + BASE_2D_ENG)
#define ENG_PATTERN1                (0x84 + BASE_2D_ENG)

#define ENG_BUSY                    0x80
#define VRAM_ACCESS                 0xA0

#define EBP_CMD_NOP                 0x00
#define EBP_CMD_BITBLT              0x01
#define EBP_CMD_BLOCKWRITE          0x02
#define EBP_CMD_SCANLINE            0x03
#define EBP_CMD_LINEDRAW            0x04

#define EBP_ENABLE_DEST_COLORKEY    0x10000
#define EBP_TRANSPARENT_MODE        0x1000
#define EBP_CLIP_ON                 0x40000000
#define EBP_DISP_STARTADDR_ACTIVE   0x2000000   /* Bit25 of GE6C */
#define EBP_DISP_STARTADDR_DEACT    0x4000000   /* Deactive 2d and 3d flip. */
#define EBP_MONO_BYTEPACKING        0x400000    /* byte packed mono source data -- Bit22-23 of GE28 */
#define EBP_ENABLE_2D_FLIP          0x04000000

#define EBP_DIFFERENT_FORMAT        0x80000000  /* source and destination have different format */

#define EBP_SOLID_BRUSH             0x4000
#define EBP_MONO_SOURCE             0x40
#define EBP_MONO_PATTERN            0x20
#define EBP_SOURCE_IN_VRAM          0x04
#define EBP_NOT_HATCH_PATTERN       0x001C0000
#define EBP_PEN_PATTERN             0x8000
#define EBP_OCT_Y_DEC               0x100
#define EBP_OCT_X_DEC               0x200
#define EBP_Y_MAJOR                 0x400
#define EBP_DUAL_PORT               0x03

#define EBP_COLOR_IS_8              0
#define EBP_COLOR_IS_565            1
#define EBP_COLOR_IS_32             2
#define EBP_COLOR_IS_24             3

#define AGP_CHANNEL_CONTROL         (0x68 + BASE_AGP)
#define CMDLIST_STATUS              (0x70 + BASE_AGP)
#define FRAMEBUF_CMDLIST_BUSY       0x01000000
#define AGP_CHANNEL1_BUSY           0x00000100

#define EOP_SD_STRIDE(x)            ((DWORD)(((x)>>4)&0x3ff) << 22)
#define EOP_SD_ADDRESS(x)           ((DWORD)(((x)>>4)&0x3fffff))
#define EOP_XSTART(x)               (((DWORD)(x)) << 16)
#define EOP_YSTART(x)               ((DWORD)(x))
#define EOP_CFORMAT(x)              (CFORMAT[(x >> 3) - 1])

/* 2D engine */
#define FER_CMD_NOP                 0x00
#define FER_CMD_BITBLT              0x01
#define FER_CMD_BWRITE              0x02
#define FER_CMD_LINEDRAW            0x04

#define FER_ENGREG_COMMAND          0x24
#define FER_ENGREG_DRAWINGFLAG0     0x28
#define FER_ENGREG_DMAPX            0x38
#define FER_ENGREG_K1               0x3C
#define FER_ENGREG_ODHEIGHT         0x40
#define FER_ENGREG_LENGTH           0x40

#define FER_CLIP_ON                 0x40000000
#define FER_SRC_IN_AGP              0x20000000
#define FER_DST_IN_AGP              0x10000000
#define FER_SRC_BIAS_MASK           0x07000000
#define FER_SRC_BIAS_SHIFT          24
#define FER_TRANSPARENT_PATTERN     0x00200000
#define FER_DST_KEY_POLARITY        0x00020000
#define FER_ENABLE_DST_COLOR_KEY    0x00010000
#define FER_PEN_PATTERN             0x00008000
#define FER_SOLID_BRUSH             0x00004000
#define FER_TRANSPARENT_POLARITY    0x00002000
#define FER_TRANSPARENT_MODE        0x00001000
#define FER_ENABLE_SRC_COLOR_KEY    0x00001000
#define FER_ENABLE_AGP_BLT          0x00000800
#define FER_Y_MAJOR                 0x00000400
#define FER_OCT_X_DEC               0x00000200
#define FER_OCT_Y_DEC               0x00000100
#define FER_ENABLE_ALPHA            0x00000080
#define FER_MONO_SOURCE             0x00000040
#define FER_MONO_PATTERN            0x00000020
#define FER_SRC_ALPHA               0x00000010
#define FER_CONST_ALPHA             0x00000008
#define FER_SOURCE_IN_VRAM          0x00000004
#define FER_EN_PATTERN_FLIP         0x00000002
#define FER_PATTERN_BANK1           0x00000001

#define FER_ROP_SRCCOPY             0x00CC0020
#define FER_MONO_PATTERN_DPRO       0x001C0020
#define FER_MONO_PATTERN_DD         0x001C0000
#define FER_OCT_XY_DEC              0x00000300

/*Should be defined in some 3d file. */
#define AGP_STATUS         0x2370

/* 3D engine status */
#define WHOLD_GE_STATUS             BASE_3D_ENG
#define IDLE_MASK                   ~0x90200000


/* THIS IS A AUTOMATICALLY GENERATED FILE, DO NOT EDIT BY HAND!!! */

#define ONE_BIT_MASK 		0x1
#define TWO_BIT_MASK 		0x3
#define THREE_BIT_MASK 		0x7
#define FOUR_BIT_MASK 		0xf
#define FIVE_BIT_MASK 		0x1f
#define SIX_BIT_MASK 		0x3f
#define SEVEN_BIT_MASK 		0x7f
#define EIGHT_BIT_MASK 		0xff
#define NINE_BIT_MASK 		0x1ff
#define TEN_BIT_MASK 		0x3ff
#define ELEVEN_BIT_MASK 		0x7ff
#define TWELVE_BIT_MASK 		0xfff
#define THIRTEEN_BIT_MASK 		0x1fff
#define FOURTEEN_BIT_MASK 		0x3fff
#define FIFTEEN_BIT_MASK 		0x7fff
#define SIXTEEN_BIT_MASK 		0xffff
#define SEVENTEEN_BIT_MASK 		0x1ffff
#define EIGHTEEN_BIT_MASK 		0x3ffff
#define NINETEEN_BIT_MASK 		0x7ffff
#define TWENTY_BIT_MASK 		0xfffff
#define TWENTYONE_BIT_MASK 		0x1fffff
#define TWENTYTWO_BIT_MASK 		0x3fffff
#define TWENTYTHREE_BIT_MASK 		0x7fffff
#define TWENTYFOUR_BIT_MASK 		0xffffff
#define TWENTYFIVE_BIT_MASK 		0x1ffffff
#define TWENTYSIX_BIT_MASK 		0x3ffffff
#define TWENTYSEVEN_BIT_MASK 		0x7ffffff
#define TWENTYEIGHT_BIT_MASK 		0xfffffff
#define TWENTYNINE_BIT_MASK 		0x1fffffff
#define THIRTY_BIT_MASK 		0x3fffffff
#define THIRTYONE_BIT_MASK 		0x7fffffff
#define THIRTYTWO_BIT_MASK 		0xffffffff



#define M2REG_FLUSH_ENGINE_ADDRESS  0x0
#define M2REG_FLUSH_ENGINE_COMMAND  0x0
#define 	M2REG_DELAY_CYCLES_RBOFFSET			0x0
#define 	M2REG_DELAY_CYCLES_SHIFT				22
#define 	M2REG_DELAY_CYCLES_MASK				(TWO_BIT_MASK<<22)
typedef enum
{
	DCC_32_clocks = 0x0,	/* default */
	DCC_64_clocks = 0x1,	/* No comment Given */
	DCC_96_clocks = 0x2,	/* No comment Given */
	DCC_128_clocks = 0x3	/* No comment Given */
} DelayClockCount;


#define 	M2REG_FLUSH_FLIP_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_FLIP_ENGINE_SHIFT				21
#define 	M2REG_FLUSH_FLIP_ENGINE_MASK				(ONE_BIT_MASK<<21)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_2D_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_2D_ENGINE_SHIFT				20
#define 	M2REG_FLUSH_2D_ENGINE_MASK				(ONE_BIT_MASK<<20)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_3D_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_3D_ENGINE_SHIFT				0
#define 	M2REG_FLUSH_3D_ENGINE_MASK				TWENTY_BIT_MASK
#define 	M2REG_FLUSH_GE_MI_RBOFFSET			0x0
#define 	M2REG_FLUSH_GE_MI_SHIFT				19
#define 	M2REG_FLUSH_GE_MI_MASK				(ONE_BIT_MASK<<19)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_OUTPUT_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_OUTPUT_ENGINE_SHIFT				18
#define 	M2REG_FLUSH_OUTPUT_ENGINE_MASK				(ONE_BIT_MASK<<18)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_PIXEL_BLENDER_RBOFFSET			0x0
#define 	M2REG_FLUSH_PIXEL_BLENDER_SHIFT				17
#define 	M2REG_FLUSH_PIXEL_BLENDER_MASK				(ONE_BIT_MASK<<17)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_PIXEL_SHADER_RBOFFSET			0x0
#define 	M2REG_FLUSH_PIXEL_SHADER_SHIFT				16
#define 	M2REG_FLUSH_PIXEL_SHADER_MASK				(ONE_BIT_MASK<<16)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_TEXTURE_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_TEXTURE_ENGINE_SHIFT				15
#define 	M2REG_FLUSH_TEXTURE_ENGINE_MASK				(ONE_BIT_MASK<<15)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_FILTER_LOOP_RBOFFSET			0x0
#define 	M2REG_FLUSH_FILTER_LOOP_SHIFT				14
#define 	M2REG_FLUSH_FILTER_LOOP_MASK				(ONE_BIT_MASK<<14)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_LOD_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_LOD_ENGINE_SHIFT				13
#define 	M2REG_FLUSH_LOD_ENGINE_MASK				(ONE_BIT_MASK<<13)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_DEPTH_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_DEPTH_ENGINE_SHIFT				12
#define 	M2REG_FLUSH_DEPTH_ENGINE_MASK				(ONE_BIT_MASK<<12)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_TILING_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_TILING_ENGINE_SHIFT				11
#define 	M2REG_FLUSH_TILING_ENGINE_MASK				(ONE_BIT_MASK<<11)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_BUMP_SHADER_EVALUATOR_RBOFFSET			0x0
#define 	M2REG_FLUSH_BUMP_SHADER_EVALUATOR_SHIFT				10
#define 	M2REG_FLUSH_BUMP_SHADER_EVALUATOR_MASK				(ONE_BIT_MASK<<10)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_RASTERIZATION_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_RASTERIZATION_ENGINE_SHIFT				9
#define 	M2REG_FLUSH_RASTERIZATION_ENGINE_MASK				(ONE_BIT_MASK<<9)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_SETUP_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_SETUP_ENGINE_SHIFT				8
#define 	M2REG_FLUSH_SETUP_ENGINE_MASK				(ONE_BIT_MASK<<8)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_OVERFLOW_BUFFER_RBOFFSET			0x0
#define 	M2REG_FLUSH_OVERFLOW_BUFFER_SHIFT				7
#define 	M2REG_FLUSH_OVERFLOW_BUFFER_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_VIEWPORT_ENGINE_RBOFFSET			0x0
#define 	M2REG_FLUSH_VIEWPORT_ENGINE_SHIFT				6
#define 	M2REG_FLUSH_VIEWPORT_ENGINE_MASK				(ONE_BIT_MASK<<6)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_CLIP_CULL_3DCLIP_RBOFFSET			0x0
#define 	M2REG_FLUSH_CLIP_CULL_3DCLIP_SHIFT				5
#define 	M2REG_FLUSH_CLIP_CULL_3DCLIP_MASK				(ONE_BIT_MASK<<5)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_PRIMITIVE_ASSEMBLY_RBOFFSET			0x0
#define 	M2REG_FLUSH_PRIMITIVE_ASSEMBLY_SHIFT				4
#define 	M2REG_FLUSH_PRIMITIVE_ASSEMBLY_MASK				(ONE_BIT_MASK<<4)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_VERTEX_SHADER_RBOFFSET			0x0
#define 	M2REG_FLUSH_VERTEX_SHADER_SHIFT				3
#define 	M2REG_FLUSH_VERTEX_SHADER_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_PUT_PROCESSOR_RBOFFSET			0x0
#define 	M2REG_FLUSH_PUT_PROCESSOR_SHIFT				2
#define 	M2REG_FLUSH_PUT_PROCESSOR_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_VERTEX_FETCHER_RBOFFSET			0x0
#define 	M2REG_FLUSH_VERTEX_FETCHER_SHIFT				1
#define 	M2REG_FLUSH_VERTEX_FETCHER_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_GP_DECODER_RBOFFSET			0x0
#define 	M2REG_FLUSH_GP_DECODER_SHIFT				0
#define 	M2REG_FLUSH_GP_DECODER_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_RESET_ADDRESS  0x4
#define M2REG_RESET_COMMAND  0x1
#define 	M2REG_RESET_STATUS2_RBOFFSET			0x1
#define 	M2REG_RESET_STATUS2_SHIFT				10
#define 	M2REG_RESET_STATUS2_MASK				(ONE_BIT_MASK<<10)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RESET_STATUS1_RBOFFSET			0x1
#define 	M2REG_RESET_STATUS1_SHIFT				9
#define 	M2REG_RESET_STATUS1_MASK				(ONE_BIT_MASK<<9)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RESET_STATUS0_RBOFFSET			0x1
#define 	M2REG_RESET_STATUS0_SHIFT				8
#define 	M2REG_RESET_STATUS0_MASK				(ONE_BIT_MASK<<8)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RESET_3DENG_RBOFFSET			0x1
#define 	M2REG_RESET_3DENG_SHIFT				4
#define 	M2REG_RESET_3DENG_MASK				(ONE_BIT_MASK<<4)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RESET_2DENG_RBOFFSET			0x1
#define 	M2REG_RESET_2DENG_SHIFT				2
#define 	M2REG_RESET_2DENG_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_MTIME_ADDRESS  0x8
#define M2REG_MTIME_COMMAND  0x2
#define 	M2REG_ENABLE_TIMER_RBOFFSET			0x2
#define 	M2REG_ENABLE_TIMER_SHIFT				23
#define 	M2REG_ENABLE_TIMER_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_TIMER_THRESHOLD_RBOFFSET			0x2
#define 	M2REG_TIMER_THRESHOLD_SHIFT				0
#define 	M2REG_TIMER_THRESHOLD_MASK				TWENTYTHREE_BIT_MASK


#define M2REG_COMMANDSWAP_ADDRESS  0xc
#define M2REG_COMMANDSWAP_COMMAND  0x3
#define 	M2REG_COMMANDSWAPMODE_RBOFFSET			0x3
#define 	M2REG_COMMANDSWAPMODE_SHIFT				0
#define 	M2REG_COMMANDSWAPMODE_MASK				TWO_BIT_MASK
typedef enum
{
	SSM_no_swap = 0x0,	/* unchanged.  default. */
	SSM_half_swap = 0x1,	/* bit[7:0] <--> bit[15:08] and bit[31:24] <--> bit[23:16]. */
	SSM_word_swap = 0x2,	/* bit[7:0] <--> bit[23:16] and bit[15:08] <--> bit[31:24]. */
	SSM_full_swap = 0x3	/* bit[7:0] <--> bit[31:24] and bit[15:08] <--> bit[23:16]. */
} SurfaceSwapMode;




#define M2REG_AUTO_LINK_SETTING_ADDRESS  0x10
#define M2REG_AUTO_LINK_SETTING_COMMAND  0x4
#define 	M2REG_CLEAR_TIMER_INTERRUPT_RBOFFSET			0x4
#define 	M2REG_CLEAR_TIMER_INTERRUPT_SHIFT				11
#define 	M2REG_CLEAR_TIMER_INTERRUPT_MASK				(ONE_BIT_MASK<<11)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CLEAR_INTERRUPT_3_RBOFFSET			0x4
#define 	M2REG_CLEAR_INTERRUPT_3_SHIFT				10
#define 	M2REG_CLEAR_INTERRUPT_3_MASK				(ONE_BIT_MASK<<10)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CLEAR_INTERRUPT_2_RBOFFSET			0x4
#define 	M2REG_CLEAR_INTERRUPT_2_SHIFT				9
#define 	M2REG_CLEAR_INTERRUPT_2_MASK				(ONE_BIT_MASK<<9)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CLEAR_INTERRUPT_0_RBOFFSET			0x4
#define 	M2REG_CLEAR_INTERRUPT_0_SHIFT				8
#define 	M2REG_CLEAR_INTERRUPT_0_MASK				(ONE_BIT_MASK<<8)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CLEAR_COUNTERS_RBOFFSET			0x4
#define 	M2REG_CLEAR_COUNTERS_SHIFT				4
#define 	M2REG_CLEAR_COUNTERS_MASK				(ONE_BIT_MASK<<4)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_GE_INTERRUPT_ENABLE_RBOFFSET			0x4
#define 	M2REG_GE_INTERRUPT_ENABLE_SHIFT				2
#define 	M2REG_GE_INTERRUPT_ENABLE_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_PCI_TRIGGER_MODE_RBOFFSET			0x4
#define 	M2REG_PCI_TRIGGER_MODE_SHIFT				1
#define 	M2REG_PCI_TRIGGER_MODE_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_INVALID_LIST_AUTO_INTERRUPT_RBOFFSET			0x4
#define 	M2REG_INVALID_LIST_AUTO_INTERRUPT_SHIFT				0
#define 	M2REG_INVALID_LIST_AUTO_INTERRUPT_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_AUTO_LINK_STATUS_ADDRESS  0x10
#define M2REG_AUTO_LINK_STATUS_COMMAND  0x4
#define 	M2REG_ACTIVE_TIMER_INTERRUPT_RBOFFSET			0x4
#define 	M2REG_ACTIVE_TIMER_INTERRUPT_SHIFT				11
#define 	M2REG_ACTIVE_TIMER_INTERRUPT_MASK				(ONE_BIT_MASK<<11)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ACTIVE_INTERRUPT_3_RBOFFSET			0x4
#define 	M2REG_ACTIVE_INTERRUPT_3_SHIFT				10
#define 	M2REG_ACTIVE_INTERRUPT_3_MASK				(ONE_BIT_MASK<<10)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ACTIVE_INTERRUPT_2_RBOFFSET			0x4
#define 	M2REG_ACTIVE_INTERRUPT_2_SHIFT				9
#define 	M2REG_ACTIVE_INTERRUPT_2_MASK				(ONE_BIT_MASK<<9)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ACTIVE_INTERRUPT_0_RBOFFSET			0x4
#define 	M2REG_ACTIVE_INTERRUPT_0_SHIFT				8
#define 	M2REG_ACTIVE_INTERRUPT_0_MASK				(ONE_BIT_MASK<<8)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_GE_INTERRUPT_ENABLED_RBOFFSET			0x4
#define 	M2REG_GE_INTERRUPT_ENABLED_SHIFT				2
#define 	M2REG_GE_INTERRUPT_ENABLED_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_PCI_TRIGGERRED_MODE_RBOFFSET			0x4
#define 	M2REG_PCI_TRIGGERRED_MODE_SHIFT				1
#define 	M2REG_PCI_TRIGGERRED_MODE_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_INVALID_LIST_AUTO_INTERRUPTED_MODE_RBOFFSET			0x4
#define 	M2REG_INVALID_LIST_AUTO_INTERRUPTED_MODE_SHIFT				0
#define 	M2REG_INVALID_LIST_AUTO_INTERRUPTED_MODE_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_PCI_TRIGGER_REGISTER_ADDRESS  0x14
#define M2REG_PCI_TRIGGER_REGISTER_COMMAND  0x5
#define 	M2REG_SW_TRIGGER_REGISTER_RBOFFSET			0x5
#define 	M2REG_SW_TRIGGER_REGISTER_SHIFT				0
#define 	M2REG_SW_TRIGGER_REGISTER_MASK				TWENTYFOUR_BIT_MASK


/******************************
* BEGIN INSTRUCTION
******************************/
#define 	BEGIN_DROP_PRIMITIVE_ENABLE_OFFSET			0x0
#define 	BEGIN_DROP_PRIMITIVE_ENABLE_SHIFT				23
#define 	BEGIN_DROP_PRIMITIVE_ENABLE_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	BEGIN_STOP_STORE_CURRENT_POINTER_OFFSET			0x0
#define 	BEGIN_STOP_STORE_CURRENT_POINTER_SHIFT				22
#define 	BEGIN_STOP_STORE_CURRENT_POINTER_MASK				(ONE_BIT_MASK<<22)
/* 0 = Disable */
/* 1 = Enable */
#define 	BEGIN_HOLD_OFFSET			0x0
#define 	BEGIN_HOLD_SHIFT				21
#define 	BEGIN_HOLD_MASK				(ONE_BIT_MASK<<21)
/* 0 = Disable */
/* 1 = Enable */
#define 	BEGIN_VALID_OFFSET			0x0
#define 	BEGIN_VALID_SHIFT				20
#define 	BEGIN_VALID_MASK				(ONE_BIT_MASK<<20)
/* 0 = Disable */
/* 1 = Enable */
#define 	BEGIN_BEGIN_IDENTIFICATION_OFFSET			0x0
#define 	BEGIN_BEGIN_IDENTIFICATION_SHIFT				0
#define 	BEGIN_BEGIN_IDENTIFICATION_MASK				TWENTY_BIT_MASK
#define 	BEGIN_LINK_ENABLE_OFFSET			0x1
#define 	BEGIN_LINK_ENABLE_SHIFT				31
#define 	BEGIN_LINK_ENABLE_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	BEGIN_COMMAND_LIST_LENGTH_OFFSET			0x1
#define 	BEGIN_COMMAND_LIST_LENGTH_SHIFT				0
#define 	BEGIN_COMMAND_LIST_LENGTH_MASK				TWENTYTWO_BIT_MASK
#define 	BEGIN_COMMAND_LIST_START_ADDRESS_OFFSET			0x2
#define 	BEGIN_COMMAND_LIST_START_ADDRESS_SHIFT				0
#define 	BEGIN_COMMAND_LIST_START_ADDRESS_MASK				TWENTYEIGHT_BIT_MASK


/******************************
* CALL INSTRUCTION
******************************/
#define 	CALL_DROP_PRIMITIVE_INCALL_OFFSET			0x0
#define 	CALL_DROP_PRIMITIVE_INCALL_SHIFT				23
#define 	CALL_DROP_PRIMITIVE_INCALL_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	CALL_STOP_STORE_CURRENT_POINTER_OFFSET			0x0
#define 	CALL_STOP_STORE_CURRENT_POINTER_SHIFT				22
#define 	CALL_STOP_STORE_CURRENT_POINTER_MASK				(ONE_BIT_MASK<<22)
/* 0 = Disable */
/* 1 = Enable */
#define 	CALL_HOLD_OFFSET			0x0
#define 	CALL_HOLD_SHIFT				21
#define 	CALL_HOLD_MASK				(ONE_BIT_MASK<<21)
/* 0 = Disable */
/* 1 = Enable */
#define 	CALL_VALID_OFFSET			0x0
#define 	CALL_VALID_SHIFT				20
#define 	CALL_VALID_MASK				(ONE_BIT_MASK<<20)
/* 0 = Disable */
/* 1 = Enable */
#define 	CALL_INSTANT_LENGTH_OFFSET			0x0
#define 	CALL_INSTANT_LENGTH_SHIFT				0
#define 	CALL_INSTANT_LENGTH_MASK				TWENTY_BIT_MASK
#define 	CALL_LINK_ENABLE_OFFSET			0x1
#define 	CALL_LINK_ENABLE_SHIFT				31
#define 	CALL_LINK_ENABLE_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	CALL_NEXT_LIST_BASE_OFFSET			0x1
#define 	CALL_NEXT_LIST_BASE_SHIFT				0
#define 	CALL_NEXT_LIST_BASE_MASK				TWENTYEIGHT_BIT_MASK
#define 	CALL_NEXT_LIST_LENGTH_OFFSET			0x2
#define 	CALL_NEXT_LIST_LENGTH_SHIFT				0
#define 	CALL_NEXT_LIST_LENGTH_MASK				TWENTYTWO_BIT_MASK
#define 	CALL_CALL_IDENTIFICATION_OFFSET			0x2
#define 	CALL_CALL_IDENTIFICATION_SHIFT				22
#define 	CALL_CALL_IDENTIFICATION_MASK				(TEN_BIT_MASK<<22)
#define 	CALL_DROP_SECONDPRIMITIVES_OFFSET			0x3
#define 	CALL_DROP_SECONDPRIMITIVES_SHIFT				31
#define 	CALL_DROP_SECONDPRIMITIVES_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	CALL_INSTANT_BASE_ADDRESS_OFFSET			0x3
#define 	CALL_INSTANT_BASE_ADDRESS_SHIFT				0
#define 	CALL_INSTANT_BASE_ADDRESS_MASK				THIRTYONE_BIT_MASK


/******************************
* CONDITIONAL_END INSTRUCTION
******************************/
#define 	CONDITIONAL_END_HOLD_OFFSET			0x0
#define 	CONDITIONAL_END_HOLD_SHIFT				21
#define 	CONDITIONAL_END_HOLD_MASK				(ONE_BIT_MASK<<21)
/* 0 = Disable */
/* 1 = Enable */
#define 	CONDITIONAL_END_REGISTER_ID_OFFSET			0x0
#define 	CONDITIONAL_END_REGISTER_ID_SHIFT				0
#define 	CONDITIONAL_END_REGISTER_ID_MASK				EIGHT_BIT_MASK
#define 	CONDITIONAL_END_TEST_MASK_OFFSET			0x1
#define 	CONDITIONAL_END_TEST_MASK_SHIFT				0
#define 	CONDITIONAL_END_TEST_MASK_MASK				THIRTYTWO_BIT_MASK


#define M2REG_SURFACE0_CONTROL_MODE_ADDRESS  0x80
#define M2REG_SURFACE0_CONTROL_MODE_COMMAND  0x20
#define 	M2REG_SURFACE0_HW_SWAP_ENABLE_RBOFFSET			0x20
#define 	M2REG_SURFACE0_HW_SWAP_ENABLE_SHIFT				0
#define 	M2REG_SURFACE0_HW_SWAP_ENABLE_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE0_WAIT_DISPLAY_RBOFFSET			0x20
#define 	M2REG_SURFACE0_WAIT_DISPLAY_SHIFT				1
#define 	M2REG_SURFACE0_WAIT_DISPLAY_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE0_FRAMEBUFFER_OVER_DRAW_RBOFFSET			0x20
#define 	M2REG_SURFACE0_FRAMEBUFFER_OVER_DRAW_SHIFT				2
#define 	M2REG_SURFACE0_FRAMEBUFFER_OVER_DRAW_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE0_CHECK_DISPLAY_RBOFFSET			0x20
#define 	M2REG_SURFACE0_CHECK_DISPLAY_SHIFT				3
#define 	M2REG_SURFACE0_CHECK_DISPLAY_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_SURFACE0_BASE_ADDRESS  0x84
#define M2REG_SURFACE0_BASE_COMMAND  0x21
#define 	M2REG_SURFACE0_BASE_ADDRESS_RBOFFSET			0x21
#define 	M2REG_SURFACE0_BASE_ADDRESS_SHIFT				0
#define 	M2REG_SURFACE0_BASE_ADDRESS_MASK				TWENTYTHREE_BIT_MASK
#define 	M2REG_SURFACE0_FLIP_END_OF_FRAME_RBOFFSET			0x21
#define 	M2REG_SURFACE0_FLIP_END_OF_FRAME_SHIFT				23
#define 	M2REG_SURFACE0_FLIP_END_OF_FRAME_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_SURFACE1_CONTROL_MODE_ADDRESS  0x88
#define M2REG_SURFACE1_CONTROL_MODE_COMMAND  0x22
#define 	M2REG_SURFACE1_HW_SWAP_ENABLE_RBOFFSET			0x22
#define 	M2REG_SURFACE1_HW_SWAP_ENABLE_SHIFT				0
#define 	M2REG_SURFACE1_HW_SWAP_ENABLE_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE1_WAIT_DISPLAY_RBOFFSET			0x22
#define 	M2REG_SURFACE1_WAIT_DISPLAY_SHIFT				1
#define 	M2REG_SURFACE1_WAIT_DISPLAY_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE1_FRAMEBUFFER_OVER_DRAW_RBOFFSET			0x22
#define 	M2REG_SURFACE1_FRAMEBUFFER_OVER_DRAW_SHIFT				2
#define 	M2REG_SURFACE1_FRAMEBUFFER_OVER_DRAW_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE1_CHECK_DISPLAY_RBOFFSET			0x22
#define 	M2REG_SURFACE1_CHECK_DISPLAY_SHIFT				3
#define 	M2REG_SURFACE1_CHECK_DISPLAY_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_SURFACE1_BASE_ADDRESS  0x8c
#define M2REG_SURFACE1_BASE_COMMAND  0x23
#define 	M2REG_SURFACE1_BASE_ADDRESS_RBOFFSET			0x23
#define 	M2REG_SURFACE1_BASE_ADDRESS_SHIFT				0
#define 	M2REG_SURFACE1_BASE_ADDRESS_MASK				TWENTYTHREE_BIT_MASK
#define 	M2REG_SURFACE1_FLIP_END_OF_FRAME_RBOFFSET			0x23
#define 	M2REG_SURFACE1_FLIP_END_OF_FRAME_SHIFT				23
#define 	M2REG_SURFACE1_FLIP_END_OF_FRAME_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_SURFACE2_CONTROL_MODE_ADDRESS  0x90
#define M2REG_SURFACE2_CONTROL_MODE_COMMAND  0x24
#define 	M2REG_SURFACE2_HW_SWAP_ENABLE_RBOFFSET			0x24
#define 	M2REG_SURFACE2_HW_SWAP_ENABLE_SHIFT				0
#define 	M2REG_SURFACE2_HW_SWAP_ENABLE_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE2_WAIT_DISPLAY_RBOFFSET			0x24
#define 	M2REG_SURFACE2_WAIT_DISPLAY_SHIFT				1
#define 	M2REG_SURFACE2_WAIT_DISPLAY_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE2_FRAMEBUFFER_OVER_DRAW_RBOFFSET			0x24
#define 	M2REG_SURFACE2_FRAMEBUFFER_OVER_DRAW_SHIFT				2
#define 	M2REG_SURFACE2_FRAMEBUFFER_OVER_DRAW_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE2_CHECK_DISPLAY_RBOFFSET			0x24
#define 	M2REG_SURFACE2_CHECK_DISPLAY_SHIFT				3
#define 	M2REG_SURFACE2_CHECK_DISPLAY_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_SURFACE2_BASE_ADDRESS  0x94
#define M2REG_SURFACE2_BASE_COMMAND  0x25
#define 	M2REG_SURFACE2_BASE_ADDRESS_RBOFFSET			0x25
#define 	M2REG_SURFACE2_BASE_ADDRESS_SHIFT				0
#define 	M2REG_SURFACE2_BASE_ADDRESS_MASK				TWENTYTHREE_BIT_MASK
#define 	M2REG_SURFACE2_FLIP_END_OF_FRAME_RBOFFSET			0x25
#define 	M2REG_SURFACE2_FLIP_END_OF_FRAME_SHIFT				23
#define 	M2REG_SURFACE2_FLIP_END_OF_FRAME_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_EXTENDED_BASE_ADDRESS_ADDRESS  0x98
#define M2REG_EXTENDED_BASE_ADDRESS_COMMAND  0x26
#define 	M2REG_SURFACE0_HIGH_BASE_ADDRESS_RBOFFSET			0x26
#define 	M2REG_SURFACE0_HIGH_BASE_ADDRESS_SHIFT				0
#define 	M2REG_SURFACE0_HIGH_BASE_ADDRESS_MASK				FOUR_BIT_MASK
#define 	M2REG_SURFACE0_HIGH_BASE_ENABLE_RBOFFSET			0x26
#define 	M2REG_SURFACE0_HIGH_BASE_ENABLE_SHIFT				7
#define 	M2REG_SURFACE0_HIGH_BASE_ENABLE_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE1_HIGH_BASE_ADDRESS_RBOFFSET			0x26
#define 	M2REG_SURFACE1_HIGH_BASE_ADDRESS_SHIFT				8
#define 	M2REG_SURFACE1_HIGH_BASE_ADDRESS_MASK				(FOUR_BIT_MASK<<8)
#define 	M2REG_SURFACE1_HIGH_BASE_ENABLE_RBOFFSET			0x26
#define 	M2REG_SURFACE1_HIGH_BASE_ENABLE_SHIFT				15
#define 	M2REG_SURFACE1_HIGH_BASE_ENABLE_MASK				(ONE_BIT_MASK<<15)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SURFACE2_HIGH_BASE_ADDRESS_RBOFFSET			0x26
#define 	M2REG_SURFACE2_HIGH_BASE_ADDRESS_SHIFT				16
#define 	M2REG_SURFACE2_HIGH_BASE_ADDRESS_MASK				(FOUR_BIT_MASK<<16)
#define 	M2REG_SURFACE2_HIGH_BASE_ENABLE_RBOFFSET			0x26
#define 	M2REG_SURFACE2_HIGH_BASE_ENABLE_SHIFT				23
#define 	M2REG_SURFACE2_HIGH_BASE_ENABLE_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_2D_OVERFLOW_BUFFER_BASE_ADDRESS  0xc0
#define M2REG_2D_OVERFLOW_BUFFER_BASE_COMMAND  0x30
#define 	M2REG_OVERFLOW_BUFFER_BASE_ADDRESS_RBOFFSET			0x30
#define 	M2REG_OVERFLOW_BUFFER_BASE_ADDRESS_SHIFT				0
#define 	M2REG_OVERFLOW_BUFFER_BASE_ADDRESS_MASK				TWENTYFOUR_BIT_MASK


#define M2REG_2D_OVERFLOW_BUFFER_OFFSET_ADDRESS  0xc4
#define M2REG_2D_OVERFLOW_BUFFER_OFFSET_COMMAND  0x31
#define 	M2REG_OVERFLOW_ENABLE_RBOFFSET			0x31
#define 	M2REG_OVERFLOW_ENABLE_SHIFT				23
#define 	M2REG_OVERFLOW_ENABLE_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DEBUG_MODE_RBOFFSET			0x31
#define 	M2REG_DEBUG_MODE_SHIFT				22
#define 	M2REG_DEBUG_MODE_MASK				(ONE_BIT_MASK<<22)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_OVERFLOW_BUFFER_OFFSET_RBOFFSET			0x31
#define 	M2REG_OVERFLOW_BUFFER_OFFSET_SHIFT				0
#define 	M2REG_OVERFLOW_BUFFER_OFFSET_MASK				TWENTYTWO_BIT_MASK


#define M2REG_MULTISURFACES_LOAD_COMMAND_ADDRESS  0xd0
#define M2REG_MULTISURFACES_LOAD_COMMAND_COMMAND  0x34
#define 	M2REG_SURFACE_TABLE_ENABLED_RBOFFSET			0x34
#define 	M2REG_SURFACE_TABLE_ENABLED_SHIFT				23
#define 	M2REG_SURFACE_TABLE_ENABLED_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_TOTAL_ENTRY_RBOFFSET			0x34
#define 	M2REG_TOTAL_ENTRY_SHIFT				16
#define 	M2REG_TOTAL_ENTRY_MASK				(SIX_BIT_MASK<<16)
#define 	M2REG_ELEMENT_SIZE_RBOFFSET			0x35
#define 	M2REG_ELEMENT_SIZE_SHIFT				15
#define 	M2REG_ELEMENT_SIZE_MASK				(ONE_BIT_MASK<<15)
typedef enum
{
	ECS_16BPP = 0x0,	/* No comment Given */
	ECS_32BPP = 0x1	/* No comment Given */
} ElementColorSize;


#define 	M2REG_MULTISURFACES_BANDED_TILED_MODE_RBOFFSET			0x34
#define 	M2REG_MULTISURFACES_BANDED_TILED_MODE_SHIFT				12
#define 	M2REG_MULTISURFACES_BANDED_TILED_MODE_MASK				(THREE_BIT_MASK<<12)
typedef enum
{
	MSBTM_Linear = 0x0,	/* No comment Given */
	MSBTM_Tiled = 0x1,	/* 4x4 pixel tiled. */
	MSBTM_Band64 = 0x2,	/* 64*16 pixel banded. */
	MSBTM_Band32_Tiled = 0x3,	/* 64*64 pixel banded + 2x2 32 pixel block+ 4*4 pixel tiled. */
	MSBTM_Band64_InterleavedZ = 0x4,	/* No comment Given */
	MSBTM_BandedCompresson = 0x5,	/* Band32_Tiled_InterleavedZ. */
	MSBTM_Band32_Zbuffer_Only = 0x6,	/* No comment Given */
	MSBTM_Two_Line_Linear = 0x7	/* color compression */
} MultiSurfacesBandedTiledMode;


#define 	M2REG_TOTAL_ELEMENTS_RBOFFSET			0x34
#define 	M2REG_TOTAL_ELEMENTS_SHIFT				10
#define 	M2REG_TOTAL_ELEMENTS_MASK				(TWO_BIT_MASK<<10)
#define 	M2REG_BUFFER_WIDTH_RBOFFSET			0x34
#define 	M2REG_BUFFER_WIDTH_SHIFT				0
#define 	M2REG_BUFFER_WIDTH_MASK				TEN_BIT_MASK
#define 	M2REG_ENABLE_TILED_RBOFFSET			0x35
#define 	M2REG_ENABLE_TILED_SHIFT				26
#define 	M2REG_ENABLE_TILED_MASK				(ONE_BIT_MASK<<26)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUFFER_SWAP_MODE_RBOFFSET			0x35
#define 	M2REG_BUFFER_SWAP_MODE_SHIFT				24
#define 	M2REG_BUFFER_SWAP_MODE_MASK				(TWO_BIT_MASK<<24)
typedef enum
{
	BSM_no_swap = 0x0,	/* unchanged.  default. */
	BSM_half_swap = 0x1,	/* bit[7:0] ?¨¨bit[15:08] and bit[31:24] ?¨¨ bit[23:16]. */
	BSM_word_swap = 0x2,	/* bit[7:0] ?¨¨bit[23:16] and bit[15:08] ?¨¨ bit[31:24]. */
	BSM_full_swap = 0x3	/* bit[7:0] ?¨¨bit[31:24] and bit[15:08] ?¨¨ bit[23:16]. */
} BufferSwapMode;


#define 	M2REG_ONE_OVER_WIDTH_RBOFFSET			0x35
#define 	M2REG_ONE_OVER_WIDTH_SHIFT				0
#define 	M2REG_ONE_OVER_WIDTH_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_BUFFER_BASE_ADDRESS_RBOFFSET			0x36
#define 	M2REG_BUFFER_BASE_ADDRESS_SHIFT				16
#define 	M2REG_BUFFER_BASE_ADDRESS_MASK				(SIXTEEN_BIT_MASK<<16)
#define 	M2REG_BUFFER_END_ADDRESS_RBOFFSET			0x36
#define 	M2REG_BUFFER_END_ADDRESS_SHIFT				0
#define 	M2REG_BUFFER_END_ADDRESS_MASK				SIXTEEN_BIT_MASK


#define M2REG_LOAD_GP_ADDRESS  0x100
#define M2REG_LOAD_GP_COMMAND  0x40
#define 	M2REG_IEEE_TO_IFF_RBOFFSET			0x40
#define 	M2REG_IEEE_TO_IFF_SHIFT				23
#define 	M2REG_IEEE_TO_IFF_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_GP_LOAD_CODE_RBOFFSET			0x40
#define 	M2REG_GP_LOAD_CODE_SHIFT				20
#define 	M2REG_GP_LOAD_CODE_MASK				(THREE_BIT_MASK<<20)
typedef enum
{
	GPLC_NULL = 0x0,	/* default */
	GPLC_Load_Fetch_Instructions = 0x1,	/* to FETCH Array. See chapter 4. */
	GPLC_Load_Put_Instructions = 0x2,	/* to PUT Array. See chapter 4. */
	GPLC_Load_VS_Constants = 0x3,	/* array. */
	GPLC_Load_VS_Instruction = 0x4,	/* array. */
	GPLC_Load_User_Clip_Planes = 0x5	/* Each plane has 4 floats, up to 8 planes. */
} GPLoadCodes;


#define 	M2REG_GPLOAD_START_OFFSET_RBOFFSET			0x40
#define 	M2REG_GPLOAD_START_OFFSET_SHIFT				10
#define 	M2REG_GPLOAD_START_OFFSET_MASK				(NINE_BIT_MASK<<10)
#define 	M2REG_GPLOAD_LENGTH_RBOFFSET			0x40
#define 	M2REG_GPLOAD_LENGTH_SHIFT				0
#define 	M2REG_GPLOAD_LENGTH_MASK				NINE_BIT_MASK
#define 	M2REG_PUT0_RBOFFSET			0x41
#define 	M2REG_PUT0_SHIFT				0
#define 	M2REG_PUT0_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_PUT1_FETCH1_D0_RBOFFSET			0x42
#define 	M2REG_PUT1_FETCH1_D0_SHIFT				0
#define 	M2REG_PUT1_FETCH1_D0_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_PUT2_FETCH1_D1_RBOFFSET			0x43
#define 	M2REG_PUT2_FETCH1_D1_SHIFT				0
#define 	M2REG_PUT2_FETCH1_D1_MASK				THIRTYTWO_BIT_MASK


#define M2REG_OVERFLOW_BUFFER_SETTING_ADDRESS  0x110
#define M2REG_OVERFLOW_BUFFER_SETTING_COMMAND  0x44
#define 	M2REG_OVERFLOW_BUFFER_START_ADDRESS_RBOFFSET			0x44
#define 	M2REG_OVERFLOW_BUFFER_START_ADDRESS_SHIFT				0
#define 	M2REG_OVERFLOW_BUFFER_START_ADDRESS_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_ENABLE_OVERFLOW_BUFFER_RBOFFSET			0x45
#define 	M2REG_ENABLE_OVERFLOW_BUFFER_SHIFT				23
#define 	M2REG_ENABLE_OVERFLOW_BUFFER_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_OVERFLOW_BUFFER_SIZE_RBOFFSET			0x45
#define 	M2REG_OVERFLOW_BUFFER_SIZE_SHIFT				0
#define 	M2REG_OVERFLOW_BUFFER_SIZE_MASK				TWENTYTHREE_BIT_MASK
#define 	M2REG_TOTAL_TRIANGLE_AREA_THRESHOLD_RBOFFSET			0x46
#define 	M2REG_TOTAL_TRIANGLE_AREA_THRESHOLD_SHIFT				0
#define 	M2REG_TOTAL_TRIANGLE_AREA_THRESHOLD_MASK				THIRTYTWO_BIT_MASK


#define M2REG_PRIMITIVE_COMMAND_ADDRESS  0x120
#define M2REG_PRIMITIVE_COMMAND_COMMAND  0x48
#define 	M2REG_SHADING_MODE_RBOFFSET			0x48
#define 	M2REG_SHADING_MODE_SHIFT				21
#define 	M2REG_SHADING_MODE_MASK				(THREE_BIT_MASK<<21)
typedef enum
{
	FSM_Gouraud_Shading = 0x0,	/* nothing changes */
	FSM_Flat_Shading_V0 = 0x1,	/* copy color of v0 to v1 and v2 */
	FSM_Flat_Shading_V1 = 0x2,	/* copy color of v1 to v0 and v2 */
	FSM_Flat_Shading_V2 = 0x3,	/* copy color 0f v2 to v0 and v1 */
	FSM_Flat_Shading_Odd_V1_Even_V2 = 0x4,	/* for odd triangle copy v1 to v0 and v2; for even triangle copy v2 to v0 and v1. */
	FSM_Flat_Shading_Odd_V2_Even_V1 = 0x5	/* for odd triangle copy v2 to v0 and v1; for even triangle copy v1 to v0 and v2. */
} FlatShadingMode;


#define 	M2REG_PRIMITIVE_TYPE_RBOFFSET			0x48
#define 	M2REG_PRIMITIVE_TYPE_SHIFT				16
#define 	M2REG_PRIMITIVE_TYPE_MASK				(FIVE_BIT_MASK<<16)
typedef enum
{
	MPT_NULL = 0x0,	/* as a last one. */
	MPT_IndexedPointList = 0x1,	/* Indexed point list */
	MPT_IndexedLineList = 0x2,	/* No comment Given */
	MPT_IndexedLineStrip = 0x3,	/* No comment Given */
	MPT_IndexedTriangleList = 0x4,	/* No comment Given */
	MPT_IndexedTriangleStrip = 0x5,	/* No comment Given */
	MPT_IndexedTriangleFan = 0x6,	/* No comment Given */
	MPT_IndexedLineloop = 0x7,	/* No comment Given */
	MPT_IndexedQuadList = 0x8,	/* No comment Given */
	MPT_IndexedRectangleList = 0x9,	/* list of rectangles. */
	MPT_ImmediateEOP = 0x10,	/* No comment Given */
	MPT_PointListImmediateMode = 0x11,	/* Immediate mode points. */
	MPT_LineListImmediateMode = 0x12,	/* Immediate mode list */
	MPT_LineStripImmediateMode = 0x13,	/* No comment Given */
	MPT_TriangleListImmediateMode = 0x14,	/* No comment Given */
	MPT_TrianglestripImmediateMode = 0x15,	/* No comment Given */
	MPT_TrianglefanImmediateMode = 0x16,	/* No comment Given */
	MPT_LineloopImmediateMode = 0x17,	/* No comment Given */
	MPT_QuadListImmediateMode = 0x18,	/* No comment Given */
	MPT_RectangleImmediateMode = 0x19,	/* No comment Given */
	MPT_XP_PrimitiveImmediateMode = 0x1a	/* partial/full triangles with flags */
} M2PrimitiveTypes;


#define 	M2REG_EDGE_FLAG_RBOFFSET			0x48
#define 	M2REG_EDGE_FLAG_SHIFT				4
#define 	M2REG_EDGE_FLAG_MASK				(THREE_BIT_MASK<<4)
#define 	M2REG_TRIANGLE_OR_LINE_RBOFFSET			0x48
#define 	M2REG_TRIANGLE_OR_LINE_SHIFT				3
#define 	M2REG_TRIANGLE_OR_LINE_MASK				(ONE_BIT_MASK<<3)
#define 	M2REG_FULL_OR_PARTIAL_RBOFFSET			0x48
#define 	M2REG_FULL_OR_PARTIAL_SHIFT				2
#define 	M2REG_FULL_OR_PARTIAL_MASK				(ONE_BIT_MASK<<2)
#define 	M2REG_REPLACEMENT_RULE_RBOFFSET			0x48
#define 	M2REG_REPLACEMENT_RULE_SHIFT				0
#define 	M2REG_REPLACEMENT_RULE_MASK				TWO_BIT_MASK
#define 	M2REG_PRIMITIVE_COUNT_RBOFFSET			0x48
#define 	M2REG_PRIMITIVE_COUNT_SHIFT				0
#define 	M2REG_PRIMITIVE_COUNT_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_TOTAL_VERTEX_DATA_RBOFFSET			0x49
#define 	M2REG_TOTAL_VERTEX_DATA_SHIFT				10
#define 	M2REG_TOTAL_VERTEX_DATA_MASK				(TWENTYTWO_BIT_MASK<<10)
#define 	M2REG_VERTEX_INDEX_TYPE_RBOFFSET			0x49
#define 	M2REG_VERTEX_INDEX_TYPE_SHIFT				8
#define 	M2REG_VERTEX_INDEX_TYPE_MASK				(TWO_BIT_MASK<<8)
typedef enum
{
	VIT_Index16 = 0x0,	/* two indices are packed to a 32bit. default. */
	VIT_Index24_EdgeFlag3 = 0x1,	/* bit24 = edge01, bit25 = edge12, bit26 = edge20. */
	VIT_Index24 = 0x3	/* default. */
} VertexIndexType;


#define 	M2REG_VERTEX_LENGTH_RBOFFSET			0x49
#define 	M2REG_VERTEX_LENGTH_SHIFT				0
#define 	M2REG_VERTEX_LENGTH_MASK				SIX_BIT_MASK
#define 	M2REG_GP_MODE_RBOFFSET			0x4a
#define 	M2REG_GP_MODE_SHIFT				30
#define 	M2REG_GP_MODE_MASK				(TWO_BIT_MASK<<30)
#define 	M2REG_INDEX_RANGE_LOW_RBOFFSET			0x4a
#define 	M2REG_INDEX_RANGE_LOW_SHIFT				0
#define 	M2REG_INDEX_RANGE_LOW_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_VISIBILITY_BIT_STREAM_FOLLOWED_RBOFFSET			0x4b
#define 	M2REG_VISIBILITY_BIT_STREAM_FOLLOWED_SHIFT				31
#define 	M2REG_VISIBILITY_BIT_STREAM_FOLLOWED_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BOX_SIZE_RBOFFSET			0x4b
#define 	M2REG_BOX_SIZE_SHIFT				28
#define 	M2REG_BOX_SIZE_MASK				(TWO_BIT_MASK<<28)
#define 	M2REG_INDEX_RANGE_HIGH_RBOFFSET			0x4b
#define 	M2REG_INDEX_RANGE_HIGH_SHIFT				0
#define 	M2REG_INDEX_RANGE_HIGH_MASK				TWENTYFOUR_BIT_MASK


#define M2REG_GEOMETRY_ENVIRONMENT_ADDRESS  0x130
#define M2REG_GEOMETRY_ENVIRONMENT_COMMAND  0x4c
#define 	M2REG_VIEWPORT_XFORM_DISABLE_RBOFFSET			0x4c
#define 	M2REG_VIEWPORT_XFORM_DISABLE_SHIFT				23
#define 	M2REG_VIEWPORT_XFORM_DISABLE_MASK				(ONE_BIT_MASK<<23)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_FRUSTUM_GUARDBAND_CLIPCODE_DISABLE_RBOFFSET			0x4c
#define 	M2REG_FRUSTUM_GUARDBAND_CLIPCODE_DISABLE_SHIFT				22
#define 	M2REG_FRUSTUM_GUARDBAND_CLIPCODE_DISABLE_MASK				(ONE_BIT_MASK<<22)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_VERTEXSHADERINROM_RBOFFSET			0x4c
#define 	M2REG_VERTEXSHADERINROM_SHIFT				20
#define 	M2REG_VERTEXSHADERINROM_MASK				(ONE_BIT_MASK<<20)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_VERTEX_CACHE_CLEAR_RBOFFSET			0x4c
#define 	M2REG_VERTEX_CACHE_CLEAR_SHIFT				19
#define 	M2REG_VERTEX_CACHE_CLEAR_MASK				(ONE_BIT_MASK<<19)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_VERTEX_CACHE_BYPASS_RBOFFSET			0x4c
#define 	M2REG_VERTEX_CACHE_BYPASS_SHIFT				18
#define 	M2REG_VERTEX_CACHE_BYPASS_MASK				(ONE_BIT_MASK<<18)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_PUT_INSTRUCTION_OFFSET_RBOFFSET			0x4c
#define 	M2REG_PUT_INSTRUCTION_OFFSET_SHIFT				12
#define 	M2REG_PUT_INSTRUCTION_OFFSET_MASK				(SIX_BIT_MASK<<12)
#define 	M2REG_FETCH_INSTRUCTION_OFFSET_RBOFFSET			0x4c
#define 	M2REG_FETCH_INSTRUCTION_OFFSET_SHIFT				8
#define 	M2REG_FETCH_INSTRUCTION_OFFSET_MASK				(FOUR_BIT_MASK<<8)
#define 	M2REG_VERTEX_SHADER_INSTRUCTIONPAIR_OFFSET_RBOFFSET			0x4c
#define 	M2REG_VERTEX_SHADER_INSTRUCTIONPAIR_OFFSET_SHIFT				0
#define 	M2REG_VERTEX_SHADER_INSTRUCTIONPAIR_OFFSET_MASK				EIGHT_BIT_MASK
#define 	M2REG_VS_BOOLEAN_CONSTANTS_RBOFFSET			0x4d
#define 	M2REG_VS_BOOLEAN_CONSTANTS_SHIFT				16
#define 	M2REG_VS_BOOLEAN_CONSTANTS_MASK				(SIXTEEN_BIT_MASK<<16)
#define 	M2REG_VERTEX_SIZE_IN_VS_RBOFFSET			0x4d
#define 	M2REG_VERTEX_SIZE_IN_VS_SHIFT				12
#define 	M2REG_VERTEX_SIZE_IN_VS_MASK				(FOUR_BIT_MASK<<12)
#define 	M2REG_VERTEX_CACHE_CONFIGURATION_IN_VS_RBOFFSET			0x4d
#define 	M2REG_VERTEX_CACHE_CONFIGURATION_IN_VS_SHIFT				11
#define 	M2REG_VERTEX_CACHE_CONFIGURATION_IN_VS_MASK				(ONE_BIT_MASK<<11)
typedef enum
{
	CESIVS_Vertex32 = 0x0,	/* 32entry*8QW/vertex. default */
	CESIVS_Vertex16 = 0x1	/* 16entry*16QW/vertex. */
} CacheEntrySizeInVS;


#define 	M2REG_RECTANGLE_MODE_RBOFFSET			0x4d
#define 	M2REG_RECTANGLE_MODE_SHIFT				10
#define 	M2REG_RECTANGLE_MODE_MASK				(ONE_BIT_MASK<<10)
typedef enum
{
	RM_Horizontal_Mode = 0x0,	/* default */
	RM_Vertical_Mode = 0x1	/* rotate rendering and vertical gradient fill */
} RectangleMode;


#define 	M2REG_DISABLE_PA_FAST_REJECTION_RBOFFSET			0x4d
#define 	M2REG_DISABLE_PA_FAST_REJECTION_SHIFT				9
#define 	M2REG_DISABLE_PA_FAST_REJECTION_MASK				(ONE_BIT_MASK<<9)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_POINT_MODE_RBOFFSET			0x4d
#define 	M2REG_POINT_MODE_SHIFT				8
#define 	M2REG_POINT_MODE_MASK				(ONE_BIT_MASK<<8)
typedef enum
{
	PST_Simple_Point_List = 0x0,	/* No comment Given */
	PST_MS_Point_Sprite_List = 0x1	/* It will be used by VS, PA and CC.	VS will pack first pair of Uvs to second 3x128bits if this bit is not set.	PA will unpack it also if it is not set. CC decides if it enters point sprite mode. */
} PointSpriteType;


#define 	M2REG_USE_SPECIAL_TEXTURE_COORDINATE_RBOFFSET			0x4d
#define 	M2REG_USE_SPECIAL_TEXTURE_COORDINATE_SHIFT				7
#define 	M2REG_USE_SPECIAL_TEXTURE_COORDINATE_MASK				(ONE_BIT_MASK<<7)
typedef enum
{
	PSTM_Use_Texture_Coordinate = 0x0,	/* defined in vertex. */
	PSTM_Use_Special_Constant_Coordinates = 0x1	/* See detail in architecture spec. */
} PointSpriteTexgenMode;


#define 	M2REG_NUMBER_OF_USER_PLANES_RBOFFSET			0x4d
#define 	M2REG_NUMBER_OF_USER_PLANES_SHIFT				3
#define 	M2REG_NUMBER_OF_USER_PLANES_MASK				(FOUR_BIT_MASK<<3)
#define 	M2REG_USER_CLIP_CODE_VALID_RBOFFSET			0x4d
#define 	M2REG_USER_CLIP_CODE_VALID_SHIFT				2
#define 	M2REG_USER_CLIP_CODE_VALID_MASK				(ONE_BIT_MASK<<2)
#define 	M2REG_W_CLIPPING_ENABLE_RBOFFSET			0x4d
#define 	M2REG_W_CLIPPING_ENABLE_SHIFT				1
#define 	M2REG_W_CLIPPING_ENABLE_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_3D_CLIPPING_ENABLE_RBOFFSET			0x4d
#define 	M2REG_3D_CLIPPING_ENABLE_SHIFT				0
#define 	M2REG_3D_CLIPPING_ENABLE_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_W_EXPONENT_FOR_NORMALIZATION_RBOFFSET			0x4e
#define 	M2REG_W_EXPONENT_FOR_NORMALIZATION_SHIFT				24
#define 	M2REG_W_EXPONENT_FOR_NORMALIZATION_MASK				(EIGHT_BIT_MASK<<24)
#define 	M2REG_TWO_SIDED_LIGHTING_ENABLE_RBOFFSET			0x4e
#define 	M2REG_TWO_SIDED_LIGHTING_ENABLE_SHIFT				22
#define 	M2REG_TWO_SIDED_LIGHTING_ENABLE_MASK				(ONE_BIT_MASK<<22)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FRONT_FACE_TYPE_RBOFFSET			0x4e
#define 	M2REG_FRONT_FACE_TYPE_SHIFT				21
#define 	M2REG_FRONT_FACE_TYPE_MASK				(ONE_BIT_MASK<<21)
typedef enum
{
	FFT_Counter_Clockwise = 0x0,	/* No comment Given */
	FFT_Clockwise = 0x1	/* No comment Given */
} FrontFaceType;


#define 	M2REG_FOG_VALID_INGP_RBOFFSET			0x4e
#define 	M2REG_FOG_VALID_INGP_SHIFT				19
#define 	M2REG_FOG_VALID_INGP_MASK				(ONE_BIT_MASK<<19)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SPECULAR_VALID_INGP_RBOFFSET			0x4e
#define 	M2REG_SPECULAR_VALID_INGP_SHIFT				18
#define 	M2REG_SPECULAR_VALID_INGP_MASK				(ONE_BIT_MASK<<18)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DIFFUSE_COLOR_CLAMP_MODE_RBOFFSET			0x4e
#define 	M2REG_DIFFUSE_COLOR_CLAMP_MODE_SHIFT				16
#define 	M2REG_DIFFUSE_COLOR_CLAMP_MODE_MASK				(TWO_BIT_MASK<<16)
typedef enum
{
	CCM_No_Clamp = 0x0,	/* No comment Given */
	CCM_Clamp_0_1 = 0x1,	/* to [0.0, 1.0]. */
	CCM_Clamp_1_1 = 0x2	/* to [-1.0, 1.0]. */
} ColorClampMode;


#define 	M2REG_SPECULAR_COLOR_CLAMP_MODE_RBOFFSET			0x4e
#define 	M2REG_SPECULAR_COLOR_CLAMP_MODE_SHIFT				14
#define 	M2REG_SPECULAR_COLOR_CLAMP_MODE_MASK				(TWO_BIT_MASK<<14)
/* See earlier definition of ColorClampMode */
#define 	M2REG_FOG_CLAMP_MODE_RBOFFSET			0x4e
#define 	M2REG_FOG_CLAMP_MODE_SHIFT				12
#define 	M2REG_FOG_CLAMP_MODE_MASK				(TWO_BIT_MASK<<12)
/* See earlier definition of ColorClampMode */
#define 	M2REG_PERSPECTIVE_CORRECTION_RBOFFSET			0x4e
#define 	M2REG_PERSPECTIVE_CORRECTION_SHIFT				11
#define 	M2REG_PERSPECTIVE_CORRECTION_MASK				(ONE_BIT_MASK<<11)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_W_BUFFER_ENABLE_RBOFFSET			0x4e
#define 	M2REG_W_BUFFER_ENABLE_SHIFT				10
#define 	M2REG_W_BUFFER_ENABLE_MASK				(ONE_BIT_MASK<<10)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CULLING_MODE_RBOFFSET			0x4e
#define 	M2REG_CULLING_MODE_SHIFT				8
#define 	M2REG_CULLING_MODE_MASK				(TWO_BIT_MASK<<8)
typedef enum
{
	CM_No_Culling = 0x1,	/* default */
	CM_Clockwise_Culling = 0x2,	/* No comment Given */
	CM_Counter_Clockwise_Culling = 0x3	/* No comment Given */
} CullingMode;


#define 	M2REG_BACK_FILL_MODE_RBOFFSET			0x4e
#define 	M2REG_BACK_FILL_MODE_SHIFT				6
#define 	M2REG_BACK_FILL_MODE_MASK				(TWO_BIT_MASK<<6)
typedef enum
{
	FM_Point = 0x1,	/* for triangle. */
	FM_Wireframe = 0x2,	/* for triangle only. */
	FM_Solid = 0x3	/* (default). */
} FillMode;


#define 	M2REG_FRONT_FILL_MODE_RBOFFSET			0x4e
#define 	M2REG_FRONT_FILL_MODE_SHIFT				4
#define 	M2REG_FRONT_FILL_MODE_MASK				(TWO_BIT_MASK<<4)
/* See earlier definition of FillMode */
#define 	M2REG_GP_TOTAL_TEXTURE_COORDINATE_PAIRS_RBOFFSET			0x4e
#define 	M2REG_GP_TOTAL_TEXTURE_COORDINATE_PAIRS_SHIFT				0
#define 	M2REG_GP_TOTAL_TEXTURE_COORDINATE_PAIRS_MASK				FOUR_BIT_MASK
#define 	M2REG_TEXTURE_COORDINATE_WRAP_ENABLES_RBOFFSET			0x4f
#define 	M2REG_TEXTURE_COORDINATE_WRAP_ENABLES_SHIFT				0
#define 	M2REG_TEXTURE_COORDINATE_WRAP_ENABLES_MASK				THIRTYTWO_BIT_MASK


#define M2REG_CULL_THRESHOLD_ADDRESS  0x140
#define M2REG_CULL_THRESHOLD_COMMAND  0x50
#define 	M2REG_CLIP_SPACE_DETERMINANT_THRESHOLD_RBOFFSET			0x50
#define 	M2REG_CLIP_SPACE_DETERMINANT_THRESHOLD_SHIFT				16
#define 	M2REG_CLIP_SPACE_DETERMINANT_THRESHOLD_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_USE_SPECIAL_PER_TEXTURE_COORDINATE_RBOFFSET			0x50
#define 	M2REG_USE_SPECIAL_PER_TEXTURE_COORDINATE_SHIFT				0
#define 	M2REG_USE_SPECIAL_PER_TEXTURE_COORDINATE_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_W_THRESHOLD_RBOFFSET			0x51
#define 	M2REG_W_THRESHOLD_SHIFT				0
#define 	M2REG_W_THRESHOLD_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_ZERO_THRESHOLD_RBOFFSET			0x52
#define 	M2REG_ZERO_THRESHOLD_SHIFT				0
#define 	M2REG_ZERO_THRESHOLD_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_NORMAL_THRESHOLD_RBOFFSET			0x53
#define 	M2REG_NORMAL_THRESHOLD_SHIFT				0
#define 	M2REG_NORMAL_THRESHOLD_MASK				THIRTYTWO_BIT_MASK


#define M2REG_VIEWPORT_CLIP_ADDRESS  0x150
#define M2REG_VIEWPORT_CLIP_COMMAND  0x54
#define 	M2REG_VIEWPORT_CLIP_LEFT_RBOFFSET			0x54
#define 	M2REG_VIEWPORT_CLIP_LEFT_SHIFT				0
#define 	M2REG_VIEWPORT_CLIP_LEFT_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_VIEWPORT_CLIP_RIGHT_RBOFFSET			0x55
#define 	M2REG_VIEWPORT_CLIP_RIGHT_SHIFT				0
#define 	M2REG_VIEWPORT_CLIP_RIGHT_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_VIEWPORT_CLIP_TOP_RBOFFSET			0x56
#define 	M2REG_VIEWPORT_CLIP_TOP_SHIFT				0
#define 	M2REG_VIEWPORT_CLIP_TOP_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_VIEWPORT_CLIP_BOTTOM_RBOFFSET			0x57
#define 	M2REG_VIEWPORT_CLIP_BOTTOM_SHIFT				0
#define 	M2REG_VIEWPORT_CLIP_BOTTOM_MASK				TWENTYFOUR_BIT_MASK


#define M2REG_FAR_NEAR_CLIP_ADDRESS  0x160
#define M2REG_FAR_NEAR_CLIP_COMMAND  0x58
#define 	M2REG_VIEWPORT_CLIP_NEAR_RBOFFSET			0x58
#define 	M2REG_VIEWPORT_CLIP_NEAR_SHIFT				0
#define 	M2REG_VIEWPORT_CLIP_NEAR_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_VIEWPORT_CLIP_FAR_RBOFFSET			0x59
#define 	M2REG_VIEWPORT_CLIP_FAR_SHIFT				0
#define 	M2REG_VIEWPORT_CLIP_FAR_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_GUARDBAND_CLIP_NEAR_RBOFFSET			0x5a
#define 	M2REG_GUARDBAND_CLIP_NEAR_SHIFT				0
#define 	M2REG_GUARDBAND_CLIP_NEAR_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_GUARDBAND_CLIP_FAR_RBOFFSET			0x5b
#define 	M2REG_GUARDBAND_CLIP_FAR_SHIFT				0
#define 	M2REG_GUARDBAND_CLIP_FAR_MASK				TWENTYFOUR_BIT_MASK


#define M2REG_GUARD_BAND_CLIP_ADDRESS  0x170
#define M2REG_GUARD_BAND_CLIP_COMMAND  0x5c
#define 	M2REG_GUARDBAND_CLIP_LEFT_RBOFFSET			0x5c
#define 	M2REG_GUARDBAND_CLIP_LEFT_SHIFT				0
#define 	M2REG_GUARDBAND_CLIP_LEFT_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_GUARDBAND_CLIP_RIGHT_RBOFFSET			0x5d
#define 	M2REG_GUARDBAND_CLIP_RIGHT_SHIFT				0
#define 	M2REG_GUARDBAND_CLIP_RIGHT_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_GUARDBAND_CLIP_TOP_RBOFFSET			0x5e
#define 	M2REG_GUARDBAND_CLIP_TOP_SHIFT				0
#define 	M2REG_GUARDBAND_CLIP_TOP_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_GUARDBAND_CLIP_BOTTOM_RBOFFSET			0x5f
#define 	M2REG_GUARDBAND_CLIP_BOTTOM_SHIFT				0
#define 	M2REG_GUARDBAND_CLIP_BOTTOM_MASK				TWENTYFOUR_BIT_MASK


#define M2REG_GUARD_BAND_SCALE_ADDRESS  0x180
#define M2REG_GUARD_BAND_SCALE_COMMAND  0x60
#define 	M2REG_X_SCALE_GUARDBAND_LEFT_RBOFFSET			0x60
#define 	M2REG_X_SCALE_GUARDBAND_LEFT_SHIFT				0
#define 	M2REG_X_SCALE_GUARDBAND_LEFT_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_X_SCALE_GUARDBAND_RIGHT_RBOFFSET			0x61
#define 	M2REG_X_SCALE_GUARDBAND_RIGHT_SHIFT				0
#define 	M2REG_X_SCALE_GUARDBAND_RIGHT_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_Y_SCALE_GUARDBAND_TOP_RBOFFSET			0x62
#define 	M2REG_Y_SCALE_GUARDBAND_TOP_SHIFT				16
#define 	M2REG_Y_SCALE_GUARDBAND_TOP_MASK				(SIXTEEN_BIT_MASK<<16)
#define 	M2REG_Y_SCALE_GUARDBAND_BOTTOM_RBOFFSET			0x62
#define 	M2REG_Y_SCALE_GUARDBAND_BOTTOM_SHIFT				0
#define 	M2REG_Y_SCALE_GUARDBAND_BOTTOM_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_Z_MINIMUM_SCALE_GUARDBAND_RBOFFSET			0x63
#define 	M2REG_Z_MINIMUM_SCALE_GUARDBAND_SHIFT				16
#define 	M2REG_Z_MINIMUM_SCALE_GUARDBAND_MASK				(SIXTEEN_BIT_MASK<<16)
#define 	M2REG_Z_MAXIMUM_SCALE_GUARDBAND_RBOFFSET			0x63
#define 	M2REG_Z_MAXIMUM_SCALE_GUARDBAND_SHIFT				0
#define 	M2REG_Z_MAXIMUM_SCALE_GUARDBAND_MASK				SIXTEEN_BIT_MASK


#define M2REG_VIEWPORT_SCALE_ADDRESS  0x190
#define M2REG_VIEWPORT_SCALE_COMMAND  0x64
#define 	M2REG_VIEWPORT_X_SCALE_FACTOR_RBOFFSET			0x65
#define 	M2REG_VIEWPORT_X_SCALE_FACTOR_SHIFT				0
#define 	M2REG_VIEWPORT_X_SCALE_FACTOR_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_VIEWPORT_Y_SCALE_FACTOR_RBOFFSET			0x66
#define 	M2REG_VIEWPORT_Y_SCALE_FACTOR_SHIFT				0
#define 	M2REG_VIEWPORT_Y_SCALE_FACTOR_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_VIEWPORT_Z_SCALE_FACTOR_RBOFFSET			0x67
#define 	M2REG_VIEWPORT_Z_SCALE_FACTOR_SHIFT				0
#define 	M2REG_VIEWPORT_Z_SCALE_FACTOR_MASK				TWENTYFOUR_BIT_MASK


#define M2REG_VIEWPORT_OFFSET_ADDRESS  0x1a0
#define M2REG_VIEWPORT_OFFSET_COMMAND  0x68
#define 	M2REG_VIEWPORT_X_OFFSET_RBOFFSET			0x69
#define 	M2REG_VIEWPORT_X_OFFSET_SHIFT				0
#define 	M2REG_VIEWPORT_X_OFFSET_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_VIEWPORT_Y_OFFSET_RBOFFSET			0x6a
#define 	M2REG_VIEWPORT_Y_OFFSET_SHIFT				0
#define 	M2REG_VIEWPORT_Y_OFFSET_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_VIEWPORT_Z_OFFSET_RBOFFSET			0x6b
#define 	M2REG_VIEWPORT_Z_OFFSET_SHIFT				0
#define 	M2REG_VIEWPORT_Z_OFFSET_MASK				THIRTYTWO_BIT_MASK


#define M2REG_GP_WINDOW_CLIP_ADDRESS  0x1b0
#define M2REG_GP_WINDOW_CLIP_COMMAND  0x6c
#define 	M2REG_WINDOW_CLIP_LEFT_RBOFFSET			0x6c
#define 	M2REG_WINDOW_CLIP_LEFT_SHIFT				0
#define 	M2REG_WINDOW_CLIP_LEFT_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_WINDOW_CLIP_RIGHT_RBOFFSET			0x6d
#define 	M2REG_WINDOW_CLIP_RIGHT_SHIFT				0
#define 	M2REG_WINDOW_CLIP_RIGHT_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_WINDOW_CLIP_TOP_RBOFFSET			0x6e
#define 	M2REG_WINDOW_CLIP_TOP_SHIFT				0
#define 	M2REG_WINDOW_CLIP_TOP_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_WINDOW_CLIP_BOTTOM_RBOFFSET			0x6f
#define 	M2REG_WINDOW_CLIP_BOTTOM_SHIFT				0
#define 	M2REG_WINDOW_CLIP_BOTTOM_MASK				SIXTEEN_BIT_MASK


#define M2REG_SW_SCRATCH_REGISTER_ADDRESS  0x1f0
#define M2REG_SW_SCRATCH_REGISTER_COMMAND  0x7c
#define 	M2REG_SCRATCH1_RBOFFSET			0x7c
#define 	M2REG_SCRATCH1_SHIFT				0
#define 	M2REG_SCRATCH1_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_SCRATCH2_RBOFFSET			0x7d
#define 	M2REG_SCRATCH2_SHIFT				0
#define 	M2REG_SCRATCH2_MASK				TWENTYFOUR_BIT_MASK


#define M2REG_DEPTH_OCCLUSION_QUERY_ADDRESS  0x1f8
#define M2REG_DEPTH_OCCLUSION_QUERY_COMMAND  0x7e
#define 	M2REG_TOTAL_VISIBLE_PIXELS_RBOFFSET			0x7e
#define 	M2REG_TOTAL_VISIBLE_PIXELS_SHIFT				0
#define 	M2REG_TOTAL_VISIBLE_PIXELS_MASK				THIRTYTWO_BIT_MASK


#define M2REG_LOAD_RE_ADDRESS  0x200
#define M2REG_LOAD_RE_COMMAND  0x80
#define 	M2REG_RE_LOAD_CODE_RBOFFSET			0x80
#define 	M2REG_RE_LOAD_CODE_SHIFT				20
#define 	M2REG_RE_LOAD_CODE_MASK				(FOUR_BIT_MASK<<20)
typedef enum
{
	RELC_NULL = 0x0,	/* default */
	RELC_Load_Bump_Shader_Instructions = 0x1,	/* RAM size = 64 x 31bit. */
	RELC_Load_Texture_Sampler_States = 0x2,	/* Size = 16x256bit. */
	RELC_Load_DeltaUV_Weights = 0x4,	/* array for 4x4 filtering. */
	RELC_Texture_Base_Address = 0x5,	/* size = 208 x 30bit. */
	RELC_Load_PixelShader20_Constants = 0x6,	/* RAM size = 32 x 128 bits. IEEE 32bit float -> 32bit IFF, 128bits in LOAD. */
	RELC_Load_PixelShader20_Instructions = 0x7,	/* Ram size = 128 x 80bit. */
	RELC_Load_PixelShader1X_Constants = 0x8,	/* RAM size = 10 x 64 bits. */
	RELC_Load_PixelShader1X_Instructions = 0x9,	/* Ram size = 16 x 160bit. */
	RELC_Load_Gamma_Tables = 0xa,	/* See chapter 5. */
	RELC_Load_ROP3_Stipple_Pattern = 0xd,	/* See chapter 5. */
	RELC_Load_Texture_LoadTable = 0xc	/* max size = 64x5bits used in PS2.0 working only mode. */
} RELoadCodes;


#define 	M2REG_RELOAD_START_OFFSET_RBOFFSET			0x80
#define 	M2REG_RELOAD_START_OFFSET_SHIFT				10
#define 	M2REG_RELOAD_START_OFFSET_MASK				(NINE_BIT_MASK<<10)
#define 	M2REG_RELOAD_LENGTH_RBOFFSET			0x80
#define 	M2REG_RELOAD_LENGTH_SHIFT				0
#define 	M2REG_RELOAD_LENGTH_MASK				TEN_BIT_MASK


#define M2REG_RE_WINDOW_CLIP_ADDRESS  0x210
#define M2REG_RE_WINDOW_CLIP_COMMAND  0x84
#define 	M2REG_Z_SCALE_FACTOR_RBOFFSET			0x84
#define 	M2REG_Z_SCALE_FACTOR_SHIFT				0
#define 	M2REG_Z_SCALE_FACTOR_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_PARAMETER_BANK_CONFIGURATION_RBOFFSET			0x85
#define 	M2REG_PARAMETER_BANK_CONFIGURATION_SHIFT				28
#define 	M2REG_PARAMETER_BANK_CONFIGURATION_MASK				(TWO_BIT_MASK<<28)
typedef enum
{
	PBC_32_Primitives = 0x0,	/* 2 colors + 2 Tex + fog. (0~2 tex) */
	PBC_24_Primitives = 0x1,	/* 2 colors + 4 Tex + fog. (3~4 tex) */
	PBC_14_Primitives = 0x2	/* 2 colors + 8 Tex + fog. (5~8 tex) */
} ParameterBankConfig;


#define 	M2REG_X_CLIP_RIGHT_RBOFFSET			0x85
#define 	M2REG_X_CLIP_RIGHT_SHIFT				16
#define 	M2REG_X_CLIP_RIGHT_MASK				(TWELVE_BIT_MASK<<16)
#define 	M2REG_X_CLIP_LEFT_RBOFFSET			0x85
#define 	M2REG_X_CLIP_LEFT_SHIFT				0
#define 	M2REG_X_CLIP_LEFT_MASK				TWELVE_BIT_MASK
#define 	M2REG_PAGE_MAX_PRIM_THRESHOLD_RBOFFSET			0x86
#define 	M2REG_PAGE_MAX_PRIM_THRESHOLD_SHIFT				28
#define 	M2REG_PAGE_MAX_PRIM_THRESHOLD_MASK				(FOUR_BIT_MASK<<28)
#define 	M2REG_Y_CLIP_BOTTOM_RBOFFSET			0x86
#define 	M2REG_Y_CLIP_BOTTOM_SHIFT				16
#define 	M2REG_Y_CLIP_BOTTOM_MASK				(TWELVE_BIT_MASK<<16)
#define 	M2REG_PAGE_MIN_PRIM_THRESHOLD_RBOFFSET			0x86
#define 	M2REG_PAGE_MIN_PRIM_THRESHOLD_SHIFT				12
#define 	M2REG_PAGE_MIN_PRIM_THRESHOLD_MASK				(FOUR_BIT_MASK<<12)
#define 	M2REG_Y_CLIP_TOP_RBOFFSET			0x86
#define 	M2REG_Y_CLIP_TOP_SHIFT				0
#define 	M2REG_Y_CLIP_TOP_MASK				TWELVE_BIT_MASK
#define 	M2REG_LINE_STIPPLE_ENABLE_RBOFFSET			0x87
#define 	M2REG_LINE_STIPPLE_ENABLE_SHIFT				31
#define 	M2REG_LINE_STIPPLE_ENABLE_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_MAX_PAGE_THRESHOLD_RBOFFSET			0x87
#define 	M2REG_MAX_PAGE_THRESHOLD_SHIFT				27
#define 	M2REG_MAX_PAGE_THRESHOLD_MASK				(FOUR_BIT_MASK<<27)
#define 	M2REG_LINE_STIPPLE_MODE_RBOFFSET			0x87
#define 	M2REG_LINE_STIPPLE_MODE_SHIFT				26
#define 	M2REG_LINE_STIPPLE_MODE_MASK				(ONE_BIT_MASK<<26)
typedef enum
{
	LSM_Line_Wireframe_List = 0x0,	/* Reset stipple counts before every single line/wireframe is drawn */
	LSM_Line_Strip_Loop = 0x1	/* Reset once every EOP. Every strip/loop needs an EOP. */
} LineStippleMode;


#define 	M2REG_LINE_STIPPLE_REPEAT_FACTOR_RBOFFSET			0x87
#define 	M2REG_LINE_STIPPLE_REPEAT_FACTOR_SHIFT				16
#define 	M2REG_LINE_STIPPLE_REPEAT_FACTOR_MASK				(TEN_BIT_MASK<<16)
#define 	M2REG_LINE_STIPPLE_PATTERN_RBOFFSET			0x87
#define 	M2REG_LINE_STIPPLE_PATTERN_SHIFT				0
#define 	M2REG_LINE_STIPPLE_PATTERN_MASK				SIXTEEN_BIT_MASK


#define M2REG_RASTER_MODE_ADDRESS  0x220
#define M2REG_RASTER_MODE_COMMAND  0x88
#define 	M2REG_RE_ADVANCED_2D_SHORTCUT_REGISTER_ENABLE_RBOFFSET			0x88
#define 	M2REG_RE_ADVANCED_2D_SHORTCUT_REGISTER_ENABLE_SHIFT				23
#define 	M2REG_RE_ADVANCED_2D_SHORTCUT_REGISTER_ENABLE_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_PAGEBASED_RENDERING_ENABLE_RBOFFSET			0x88
#define 	M2REG_PAGEBASED_RENDERING_ENABLE_SHIFT				22
#define 	M2REG_PAGEBASED_RENDERING_ENABLE_MASK				(ONE_BIT_MASK<<22)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_END_OF_PAGE_AT_END_OF_TRIANGLE_RBOFFSET			0x88
#define 	M2REG_END_OF_PAGE_AT_END_OF_TRIANGLE_SHIFT				21
#define 	M2REG_END_OF_PAGE_AT_END_OF_TRIANGLE_MASK				(ONE_BIT_MASK<<21)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLUSH_NOSORTING_ENABLE_RBOFFSET			0x88
#define 	M2REG_FLUSH_NOSORTING_ENABLE_SHIFT				18
#define 	M2REG_FLUSH_NOSORTING_ENABLE_MASK				(ONE_BIT_MASK<<18)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CONSTANT_COLOR_RENDER_ENABLE_RBOFFSET			0x88
#define 	M2REG_CONSTANT_COLOR_RENDER_ENABLE_SHIFT				17
#define 	M2REG_CONSTANT_COLOR_RENDER_ENABLE_MASK				(ONE_BIT_MASK<<17)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LAST_PIXEL_ON_LINE_RBOFFSET			0x88
#define 	M2REG_LAST_PIXEL_ON_LINE_SHIFT				16
#define 	M2REG_LAST_PIXEL_ON_LINE_MASK				(ONE_BIT_MASK<<16)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_FORCE_ALWAYS_FLUSH_PAGE_RBOFFSET			0x88
#define 	M2REG_FORCE_ALWAYS_FLUSH_PAGE_SHIFT				15
#define 	M2REG_FORCE_ALWAYS_FLUSH_PAGE_MASK				(ONE_BIT_MASK<<15)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ALWAYS_FULLPAGE_ENABLE_RBOFFSET			0x88
#define 	M2REG_ALWAYS_FULLPAGE_ENABLE_SHIFT				14
#define 	M2REG_ALWAYS_FULLPAGE_ENABLE_MASK				(ONE_BIT_MASK<<14)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_HALF_LINE_WIDTH_RBOFFSET			0x88
#define 	M2REG_HALF_LINE_WIDTH_SHIFT				8
#define 	M2REG_HALF_LINE_WIDTH_MASK				(SIX_BIT_MASK<<8)
#define 	M2REG_DISABLE_MULTI_TRIANGLES_FOR_BUMPLOOP_RBOFFSET			0x88
#define 	M2REG_DISABLE_MULTI_TRIANGLES_FOR_BUMPLOOP_SHIFT				7
#define 	M2REG_DISABLE_MULTI_TRIANGLES_FOR_BUMPLOOP_MASK				(ONE_BIT_MASK<<7)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_FOG_VALID_INRE_RBOFFSET			0x88
#define 	M2REG_FOG_VALID_INRE_SHIFT				6
#define 	M2REG_FOG_VALID_INRE_MASK				(ONE_BIT_MASK<<6)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FOG_PERSPECTIVEDISABLE_RBOFFSET			0x88
#define 	M2REG_FOG_PERSPECTIVEDISABLE_SHIFT				5
#define 	M2REG_FOG_PERSPECTIVEDISABLE_MASK				(ONE_BIT_MASK<<5)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_DX9C_DEPTH_CLIP_MODE_RBOFFSET			0x88
#define 	M2REG_DX9C_DEPTH_CLIP_MODE_SHIFT				4
#define 	M2REG_DX9C_DEPTH_CLIP_MODE_MASK				(ONE_BIT_MASK<<4)
#define 	M2REG_DIFFUSE_COLOR_CLAMP_INRE_RBOFFSET			0x88
#define 	M2REG_DIFFUSE_COLOR_CLAMP_INRE_SHIFT				3
#define 	M2REG_DIFFUSE_COLOR_CLAMP_INRE_MASK				(ONE_BIT_MASK<<3)
typedef enum
{
	RECCM_No_Clamp = 0x0,	/* to the range of s.6.15 IFF range */
	RECCM_Clamp_0_1 = 0x1	/* to [0, 1.0]. */
} REColorClampMode;


#define 	M2REG_SPECULAR_COLOR_CLAMP_INRE_RBOFFSET			0x88
#define 	M2REG_SPECULAR_COLOR_CLAMP_INRE_SHIFT				2
#define 	M2REG_SPECULAR_COLOR_CLAMP_INRE_MASK				(ONE_BIT_MASK<<2)
/* See earlier definition of REColorClampMode */
#define 	M2REG_SPECULAR_VALID_INRE_RBOFFSET			0x88
#define 	M2REG_SPECULAR_VALID_INRE_SHIFT				1
#define 	M2REG_SPECULAR_VALID_INRE_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DEPTH_CLIP_ENABLE_RBOFFSET			0x88
#define 	M2REG_DEPTH_CLIP_ENABLE_SHIFT				0
#define 	M2REG_DEPTH_CLIP_ENABLE_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FOG_TABLE_MODE_RBOFFSET			0x89
#define 	M2REG_FOG_TABLE_MODE_SHIFT				22
#define 	M2REG_FOG_TABLE_MODE_MASK				(TWO_BIT_MASK<<22)
typedef enum
{
	GFM_Local_vertex_Fog = 0x0,	/* Disable fog table.	 default. */
	GFM_Exponential_Fog = 0x1,	/* No comment Given */
	GFM_Squared_Exponential_Fog = 0x2,	/* No comment Given */
	GFM_Linear_Fog = 0x3	/* No comment Given */
} GlobalFogMode;


#define 	M2REG_FOG_DENSITY_RBOFFSET			0x89
#define 	M2REG_FOG_DENSITY_SHIFT				0
#define 	M2REG_FOG_DENSITY_MASK				TWENTYTWO_BIT_MASK
#define 	M2REG_ONE_OVER_FOG_RANGE_RBOFFSET			0x89
#define 	M2REG_ONE_OVER_FOG_RANGE_SHIFT				0
#define 	M2REG_ONE_OVER_FOG_RANGE_MASK				TWENTYTWO_BIT_MASK
#define 	M2REG_FOG_END_RBOFFSET			0x8a
#define 	M2REG_FOG_END_SHIFT				0
#define 	M2REG_FOG_END_MASK				TWENTYTWO_BIT_MASK
#define 	M2REG_FOG_COLOR_RED_RBOFFSET			0x8b
#define 	M2REG_FOG_COLOR_RED_SHIFT				16
#define 	M2REG_FOG_COLOR_RED_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_FOG_COLOR_GREEN_RBOFFSET			0x8b
#define 	M2REG_FOG_COLOR_GREEN_SHIFT				8
#define 	M2REG_FOG_COLOR_GREEN_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_FOG_COLOR_BLUE_RBOFFSET			0x8b
#define 	M2REG_FOG_COLOR_BLUE_SHIFT				0
#define 	M2REG_FOG_COLOR_BLUE_MASK				EIGHT_BIT_MASK
#define 	M2REG_LEFTTCI_SC_RBOFFSET			0x89
#define 	M2REG_LEFTTCI_SC_SHIFT				0
#define 	M2REG_LEFTTCI_SC_MASK				FOUR_BIT_MASK
#define 	M2REG_RIGHTTCI_SC_RBOFFSET			0x89
#define 	M2REG_RIGHTTCI_SC_SHIFT				4
#define 	M2REG_RIGHTTCI_SC_MASK				(FOUR_BIT_MASK<<4)
#define 	M2REG_LEFTSAMPLERID_SC_RBOFFSET			0x89
#define 	M2REG_LEFTSAMPLERID_SC_SHIFT				8
#define 	M2REG_LEFTSAMPLERID_SC_MASK				(FOUR_BIT_MASK<<8)
#define 	M2REG_RIGHTSAMPLERID_SC_RBOFFSET			0x89
#define 	M2REG_RIGHTSAMPLERID_SC_SHIFT				12
#define 	M2REG_RIGHTSAMPLERID_SC_MASK				(FOUR_BIT_MASK<<12)
#define 	M2REG_SAMPLINGSTATE_SC_RBOFFSET			0x89
#define 	M2REG_SAMPLINGSTATE_SC_SHIFT				16
#define 	M2REG_SAMPLINGSTATE_SC_MASK				(THREE_BIT_MASK<<16)
typedef enum
{
	SSSC_Invalid_Pair = 0x0,	/* No comment Given */
	SSSC_Valid_Invalid = 0x1,	/* left valid, right invalid */
	SSSC_Invalid_Valid = 0x2,	/* left invalid, right valid */
	SSSC_Valid_Valid = 0x3,	/* both valid */
	SSSC_Coord_NoKill_NoClamp = 0x4,	/* No comment Given */
	SSSC_Coord_NoKill_Clamp = 0x5,	/* No comment Given */
	SSSC_Coord_Kill_NoClamp = 0x6,	/* No comment Given */
	SSSC_Coord_Kill_Clamp = 0x7	/* No comment Given */
} SampleState_SC;


#define 	M2REG_REPEATUFORVENABLE_SC_RBOFFSET			0x89
#define 	M2REG_REPEATUFORVENABLE_SC_SHIFT				19
#define 	M2REG_REPEATUFORVENABLE_SC_MASK				(ONE_BIT_MASK<<19)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LEFTBUMPATTRIBUTE_SC_RBOFFSET			0x89
#define 	M2REG_LEFTBUMPATTRIBUTE_SC_SHIFT				20
#define 	M2REG_LEFTBUMPATTRIBUTE_SC_MASK				(TWO_BIT_MASK<<20)
typedef enum
{
	BASC_No_Bump_Required = 0x0,	/* No comment Given */
	BASC_2D_Bump_Required = 0x1,	/* No comment Given */
	BASC_4D_Bump_Required = 0x2	/* force it no perspective correction. */
} BumpAttribute_SC;


#define 	M2REG_RIGHTBUMPATTRIBUTE_SC_RBOFFSET			0x89
#define 	M2REG_RIGHTBUMPATTRIBUTE_SC_SHIFT				22
#define 	M2REG_RIGHTBUMPATTRIBUTE_SC_MASK				(TWO_BIT_MASK<<22)
/* See earlier definition of BumpAttribute_SC */
#define 	M2REG_TEXTURETYPE_SC_RBOFFSET			0x89
#define 	M2REG_TEXTURETYPE_SC_SHIFT				24
#define 	M2REG_TEXTURETYPE_SC_MASK				(THREE_BIT_MASK<<24)
typedef enum
{
	TTSC_2D_Texture = 0x0,	/* No comment Given */
	TTSC_Depth_Texture = 0x1,	/* No comment Given */
	TTSC_Cube_Map = 0x2,	/* No comment Given */
	TTSC_3D_Texture = 0x3,	/* No comment Given */
	TTSC_Projected_2D_Texture = 0x4,	/* No comment Given */
	TTSC_Projected_Depth_Texture = 0x5,	/* No comment Given */
	TTSC_Projected_TexCoord = 0x6,	/* SamplingAttribute should = 4~7 */
	TTSC_Projected_3D_Texture = 0x7	/* No comment Given */
} TextureType_SC;


#define 	M2REG_TRANSPOSE_EVEN_ODD_SC_RBOFFSET			0x89
#define 	M2REG_TRANSPOSE_EVEN_ODD_SC_SHIFT				27
#define 	M2REG_TRANSPOSE_EVEN_ODD_SC_MASK				(ONE_BIT_MASK<<27)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_TRANSPOSE_ENABLE_SC_RBOFFSET			0x89
#define 	M2REG_TRANSPOSE_ENABLE_SC_SHIFT				28
#define 	M2REG_TRANSPOSE_ENABLE_SC_MASK				(ONE_BIT_MASK<<28)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_END_OF_TEXTURE_SC_RBOFFSET			0x89
#define 	M2REG_END_OF_TEXTURE_SC_SHIFT				29
#define 	M2REG_END_OF_TEXTURE_SC_MASK				(ONE_BIT_MASK<<29)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_END_OF_LOOP_SC_RBOFFSET			0x89
#define 	M2REG_END_OF_LOOP_SC_SHIFT				30
#define 	M2REG_END_OF_LOOP_SC_MASK				(ONE_BIT_MASK<<30)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_REPLACEMENT_ENABLE_SC_RBOFFSET			0x89
#define 	M2REG_REPLACEMENT_ENABLE_SC_SHIFT				31
#define 	M2REG_REPLACEMENT_ENABLE_SC_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_PSROM_OFFSET_RBOFFSET			0x8a
#define 	M2REG_PSROM_OFFSET_SHIFT				31
#define 	M2REG_PSROM_OFFSET_MASK				(ONE_BIT_MASK<<31)
#define 	M2REG_COMPONENT_MASK_SC_RBOFFSET			0x8a
#define 	M2REG_COMPONENT_MASK_SC_SHIFT				24
#define 	M2REG_COMPONENT_MASK_SC_MASK				(FOUR_BIT_MASK<<24)
#define 	M2REG_INVISIBLE_PIXELPAIR_DROP_ENABLE_SC_RBOFFSET			0x8a
#define 	M2REG_INVISIBLE_PIXELPAIR_DROP_ENABLE_SC_SHIFT				15
#define 	M2REG_INVISIBLE_PIXELPAIR_DROP_ENABLE_SC_MASK				(ONE_BIT_MASK<<15)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SHADER_TOTAL_LOOPS_SC_RBOFFSET			0x8a
#define 	M2REG_BUMP_SHADER_TOTAL_LOOPS_SC_SHIFT				12
#define 	M2REG_BUMP_SHADER_TOTAL_LOOPS_SC_MASK				(THREE_BIT_MASK<<12)
#define 	M2REG_DIFFUSE_ONLY_SC_RBOFFSET			0x8a
#define 	M2REG_DIFFUSE_ONLY_SC_SHIFT				11
#define 	M2REG_DIFFUSE_ONLY_SC_MASK				(ONE_BIT_MASK<<11)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SHADER_MODE_SC_RBOFFSET			0x8a
#define 	M2REG_BUMP_SHADER_MODE_SC_SHIFT				9
#define 	M2REG_BUMP_SHADER_MODE_SC_MASK				(TWO_BIT_MASK<<9)
typedef enum
{
	BSMSC_One_Loop_Only = 0x0,	/* no bump loop, consistent to total bump loop = 0. */
	BSMSC_2D_Bump_Loop = 0x1,	/* No comment Given */
	BSMSC_4D_Bump_Loop = 0x2,	/* No comment Given */
	BSMSC_2D_4D_Bump_Mixed = 0x3	/* No comment Given */
} BumpShaderMode_SC;


#define 	M2REG_BUMP_SHADER_MAXIMUM_TILES_FOR_LOOP_SC_RBOFFSET			0x8a
#define 	M2REG_BUMP_SHADER_MAXIMUM_TILES_FOR_LOOP_SC_SHIFT				4
#define 	M2REG_BUMP_SHADER_MAXIMUM_TILES_FOR_LOOP_SC_MASK				(FIVE_BIT_MASK<<4)
#define 	M2REG_BYPASS_L2_CACHE_SC_RBOFFSET			0x8a
#define 	M2REG_BYPASS_L2_CACHE_SC_SHIFT				3
#define 	M2REG_BYPASS_L2_CACHE_SC_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BYPASS_L1_CACHE_SC_RBOFFSET			0x8a
#define 	M2REG_BYPASS_L1_CACHE_SC_SHIFT				2
#define 	M2REG_BYPASS_L1_CACHE_SC_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CLEAR_L2_CACHE_SC_RBOFFSET			0x8a
#define 	M2REG_CLEAR_L2_CACHE_SC_SHIFT				1
#define 	M2REG_CLEAR_L2_CACHE_SC_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CLEAR_L1_CACHE_SC_RBOFFSET			0x8a
#define 	M2REG_CLEAR_L1_CACHE_SC_SHIFT				0
#define 	M2REG_CLEAR_L1_CACHE_SC_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_TEXTURE_BASE_ADDRESS_SC_RBOFFSET			0x8b
#define 	M2REG_TEXTURE_BASE_ADDRESS_SC_SHIFT				0
#define 	M2REG_TEXTURE_BASE_ADDRESS_SC_MASK				TWENTYEIGHT_BIT_MASK
#define 	M2REG_TEXTURE_PORT_SC_RBOFFSET			0x8b
#define 	M2REG_TEXTURE_PORT_SC_SHIFT				28
#define 	M2REG_TEXTURE_PORT_SC_MASK				(TWO_BIT_MASK<<28)


#define M2REG_POLYGON_OFFSET_CLIP_RANGE_ADDRESS  0x230
#define M2REG_POLYGON_OFFSET_CLIP_RANGE_COMMAND  0x8c
#define 	M2REG_POLYGON_OFFSET_FACTOR_RBOFFSET			0x8c
#define 	M2REG_POLYGON_OFFSET_FACTOR_SHIFT				0
#define 	M2REG_POLYGON_OFFSET_FACTOR_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_POLYGON_OFFSET_RBOFFSET			0x8d
#define 	M2REG_POLYGON_OFFSET_SHIFT				0
#define 	M2REG_POLYGON_OFFSET_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_Z_BIASED_CLIP_FAR_RBOFFSET			0x8e
#define 	M2REG_Z_BIASED_CLIP_FAR_SHIFT				0
#define 	M2REG_Z_BIASED_CLIP_FAR_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_Z_BIASED_CLIP_NEAR_RBOFFSET			0x8f
#define 	M2REG_Z_BIASED_CLIP_NEAR_SHIFT				0
#define 	M2REG_Z_BIASED_CLIP_NEAR_MASK				THIRTYTWO_BIT_MASK


#define M2REG_DEPTH_CONSTANT_VALUES_ADDRESS  0x240
#define M2REG_DEPTH_CONSTANT_VALUES_COMMAND  0x90
#define 	M2REG_REFERENCE_STENCIL_CCW_RBOFFSET			0x90
#define 	M2REG_REFERENCE_STENCIL_CCW_SHIFT				16
#define 	M2REG_REFERENCE_STENCIL_CCW_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_STENCIL_WRITE_ENABLES_CCW_RBOFFSET			0x90
#define 	M2REG_STENCIL_WRITE_ENABLES_CCW_SHIFT				8
#define 	M2REG_STENCIL_WRITE_ENABLES_CCW_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_STENCIL_FORMAT_MASK_CCW_RBOFFSET			0x90
#define 	M2REG_STENCIL_FORMAT_MASK_CCW_SHIFT				0
#define 	M2REG_STENCIL_FORMAT_MASK_CCW_MASK				EIGHT_BIT_MASK
#define 	M2REG_CONSTANT_DEPTH_VALUE_RBOFFSET			0x91
#define 	M2REG_CONSTANT_DEPTH_VALUE_SHIFT				0
#define 	M2REG_CONSTANT_DEPTH_VALUE_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_Z_STENCIL_BUFFER_START_OFFSETY_RBOFFSET			0x92
#define 	M2REG_Z_STENCIL_BUFFER_START_OFFSETY_SHIFT				16
#define 	M2REG_Z_STENCIL_BUFFER_START_OFFSETY_MASK				(TWELVE_BIT_MASK<<16)
#define 	M2REG_Z_STENCIL_BUFFER_START_OFFSETX_RBOFFSET			0x92
#define 	M2REG_Z_STENCIL_BUFFER_START_OFFSETX_SHIFT				0
#define 	M2REG_Z_STENCIL_BUFFER_START_OFFSETX_MASK				TWELVE_BIT_MASK
#define 	M2REG_Z_STENCIL_BUFFER_END_OFFSETY_RBOFFSET			0x93
#define 	M2REG_Z_STENCIL_BUFFER_END_OFFSETY_SHIFT				16
#define 	M2REG_Z_STENCIL_BUFFER_END_OFFSETY_MASK				(TWELVE_BIT_MASK<<16)
#define 	M2REG_Z_STENCIL_BUFFER_END_OFFSETX_RBOFFSET			0x93
#define 	M2REG_Z_STENCIL_BUFFER_END_OFFSETX_SHIFT				0
#define 	M2REG_Z_STENCIL_BUFFER_END_OFFSETX_MASK				TWELVE_BIT_MASK


#define M2REG_DEPTH_BUFFER_SETTINGS_ADDRESS  0x250
#define M2REG_DEPTH_BUFFER_SETTINGS_COMMAND  0x94
#define 	M2REG_SYNC_MODE_RBOFFSET			0x94
#define 	M2REG_SYNC_MODE_SHIFT				23
#define 	M2REG_SYNC_MODE_MASK				(ONE_BIT_MASK<<23)
typedef enum
{
	TSM_HiZ_Mask_Sync = 0x0,	/* No comment Given */
	TSM_Cov_Mask_Sync = 0x1	/* No comment Given */
} TileSyncMode;


#define 	M2REG_DEPTH_SOURCE_RBOFFSET			0x94
#define 	M2REG_DEPTH_SOURCE_SHIFT				22
#define 	M2REG_DEPTH_SOURCE_MASK				(ONE_BIT_MASK<<22)
typedef enum
{
	DSM_Source_Z = 0x0,	/* default. */
	DSM_PixelShader_Depth = 0x1	/* No comment Given */
} DepthSourceMode;


#define 	M2REG_DEPTH_TEST_FUNCTION_RBOFFSET			0x94
#define 	M2REG_DEPTH_TEST_FUNCTION_SHIFT				19
#define 	M2REG_DEPTH_TEST_FUNCTION_MASK				(THREE_BIT_MASK<<19)
typedef enum
{
	VTFC_NEVER = 0x0,	/* No comment Given */
	VTFC_LESS = 0x1,	/* No comment Given */
	VTFC_EQUAL = 0x2,	/* No comment Given */
	VTFC_LESSEQUAL = 0x3,	/* No comment Given */
	VTFC_GREATER = 0x4,	/* No comment Given */
	VTFC_NOTEQUAL = 0x5,	/* No comment Given */
	VTFC_GREATEREQUAL = 0x6,	/* No comment Given */
	VTFC_ALWAYS = 0x7	/* default. */
} VisibilityTestFunctionCodes;


#define 	M2REG_STENCIL_TEST_ENABLE_RBOFFSET			0x94
#define 	M2REG_STENCIL_TEST_ENABLE_SHIFT				18
#define 	M2REG_STENCIL_TEST_ENABLE_MASK				(ONE_BIT_MASK<<18)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DEPTH_BUFFER_CONFIGURATION_RBOFFSET			0x94
#define 	M2REG_DEPTH_BUFFER_CONFIGURATION_SHIFT				16
#define 	M2REG_DEPTH_BUFFER_CONFIGURATION_MASK				(TWO_BIT_MASK<<16)
typedef enum
{
	DBF_Band32_OneTile_Z_Only = 0x0,	/* 2x2x32x32Banded + One Tile interleaved. */
	DBF_Band16_OneTile_Z_Only = 0x1,	/* 2x2x16x16Banded + one Tile interleaved. */
	DBF_Band64_OneTile_ZColor = 0x2,	/* (1x2x64x16) Banded + One Tile interleaved. */
	DBF_Band32_OneTile_ZColor = 0x3	/* (2x2x32x16) Banded + One Tile interleaved. */
} DepthBufferFormat;


#define 	M2REG_DEPTH_EXPONENT_BITS_RBOFFSET			0x94
#define 	M2REG_DEPTH_EXPONENT_BITS_SHIFT				14
#define 	M2REG_DEPTH_EXPONENT_BITS_MASK				(TWO_BIT_MASK<<14)
typedef enum
{
	DEB_0_Bit = 0x0,	/* No comment Given */
	DEB_4_Bit = 0x1,	/* default. */
	DEB_5_Bit = 0x2,	/* No comment Given */
	DEB_7_Bit = 0x3	/* No comment Given */
} DepthExponentBits;


#define 	M2REG_ENABLE_DEPTH_WRITE_RBOFFSET			0x94
#define 	M2REG_ENABLE_DEPTH_WRITE_SHIFT				13
#define 	M2REG_ENABLE_DEPTH_WRITE_MASK				(ONE_BIT_MASK<<13)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DEPTH_BUFFER_WIDTH_RBOFFSET			0x94
#define 	M2REG_DEPTH_BUFFER_WIDTH_SHIFT				0
#define 	M2REG_DEPTH_BUFFER_WIDTH_MASK				THIRTEEN_BIT_MASK
#define 	M2REG_DE_INVISIBLE_TILE_DROP_RBOFFSET			0x95
#define 	M2REG_DE_INVISIBLE_TILE_DROP_SHIFT				27
#define 	M2REG_DE_INVISIBLE_TILE_DROP_MASK				(ONE_BIT_MASK<<27)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DEPTH_READ_DISABLE_RBOFFSET			0x95
#define 	M2REG_DEPTH_READ_DISABLE_SHIFT				26
#define 	M2REG_DEPTH_READ_DISABLE_MASK				(ONE_BIT_MASK<<26)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_ZOVERLAY_CONTROL_DISABLE_RBOFFSET			0x95
#define 	M2REG_ZOVERLAY_CONTROL_DISABLE_SHIFT				25
#define 	M2REG_ZOVERLAY_CONTROL_DISABLE_MASK				(ONE_BIT_MASK<<25)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_DEPTH_BUFFER_PORT_RBOFFSET			0x95
#define 	M2REG_DEPTH_BUFFER_PORT_SHIFT				24
#define 	M2REG_DEPTH_BUFFER_PORT_MASK				(ONE_BIT_MASK<<24)
typedef enum
{
	MPC_Buffer_In_FB = 0x0,	/* No comment Given */
	MPC_Buffer_In_PCI_e = 0x1	/* No comment Given */
} MemoryPortConfiguration;


#define 	M2REG_DEPTH_BUFFER_BASE_ADDRESS_RBOFFSET			0x95
#define 	M2REG_DEPTH_BUFFER_BASE_ADDRESS_SHIFT				0
#define 	M2REG_DEPTH_BUFFER_BASE_ADDRESS_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_ENABLE_HIZ_PREFETCH_THRESHOLD_RBOFFSET			0x96
#define 	M2REG_ENABLE_HIZ_PREFETCH_THRESHOLD_SHIFT				31
#define 	M2REG_ENABLE_HIZ_PREFETCH_THRESHOLD_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_HIZ_PREFETCH_THRESHOLD_RBOFFSET			0x96
#define 	M2REG_HIZ_PREFETCH_THRESHOLD_SHIFT				24
#define 	M2REG_HIZ_PREFETCH_THRESHOLD_MASK				(SIX_BIT_MASK<<24)
#define 	M2REG_REFERENCE_STENCIL_RBOFFSET			0x96
#define 	M2REG_REFERENCE_STENCIL_SHIFT				16
#define 	M2REG_REFERENCE_STENCIL_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_DEPTH_BOUND_TEST_ENABLE_RBOFFSET			0x96
#define 	M2REG_DEPTH_BOUND_TEST_ENABLE_SHIFT				15
#define 	M2REG_DEPTH_BOUND_TEST_ENABLE_MASK				(ONE_BIT_MASK<<15)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_STENCIL_READ_DISABLE_RBOFFSET			0x96
#define 	M2REG_STENCIL_READ_DISABLE_SHIFT				14
#define 	M2REG_STENCIL_READ_DISABLE_MASK				(ONE_BIT_MASK<<14)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_USE_STENCIL_TESTMASK_FORBUMPSYNC_ENABLE_RBOFFSET			0x96
#define 	M2REG_USE_STENCIL_TESTMASK_FORBUMPSYNC_ENABLE_SHIFT				13
#define 	M2REG_USE_STENCIL_TESTMASK_FORBUMPSYNC_ENABLE_MASK				(ONE_BIT_MASK<<13)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_TWO_SIDED_STENCIL_ENABLE_RBOFFSET			0x96
#define 	M2REG_TWO_SIDED_STENCIL_ENABLE_SHIFT				12
#define 	M2REG_TWO_SIDED_STENCIL_ENABLE_MASK				(ONE_BIT_MASK<<12)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_STENCIL_OPERATION_SPASS_ZPASS_RBOFFSET			0x96
#define 	M2REG_STENCIL_OPERATION_SPASS_ZPASS_SHIFT				9
#define 	M2REG_STENCIL_OPERATION_SPASS_ZPASS_MASK				(THREE_BIT_MASK<<9)
typedef enum
{
	SOC_KEEP = 0x0,	/* StencilRet = StencilBuf.	 default. */
	SOC_ZERO = 0x1,	/* StencilRet = 0. */
	SOC_REPLACE = 0x2,	/* StencilRet = StencilRef. */
	SOC_INCRSAT = 0x3,	/* StencilRet = (StencilBuf == 0xff) ? 0xff : (StencilBuf++). */
	SOC_DECRSAT = 0x4,	/* StencilRet = (StencilBuf == 0x00) ? 0x00 : (StencilBuf--). */
	SOC_INVERT = 0x5,	/* StencilRet = ~StencilBuf. */
	SOC_INCR = 0x6,	/* StencilRet = StencilBuf++. */
	SOC_DECR = 0x7	/* StencilRet = StencilBuf--. */
} StencilOperationCode;


#define 	M2REG_STENCIL_OPERATION_SPASS_ZFAIL_RBOFFSET			0x96
#define 	M2REG_STENCIL_OPERATION_SPASS_ZFAIL_SHIFT				6
#define 	M2REG_STENCIL_OPERATION_SPASS_ZFAIL_MASK				(THREE_BIT_MASK<<6)
/* See earlier definition of StencilOperationCode */
#define 	M2REG_STENCIL_OPERATION_SFAIL_RBOFFSET			0x96
#define 	M2REG_STENCIL_OPERATION_SFAIL_SHIFT				3
#define 	M2REG_STENCIL_OPERATION_SFAIL_MASK				(THREE_BIT_MASK<<3)
/* See earlier definition of StencilOperationCode */
#define 	M2REG_STENCIL_TEST_FUNCTION_RBOFFSET			0x96
#define 	M2REG_STENCIL_TEST_FUNCTION_SHIFT				0
#define 	M2REG_STENCIL_TEST_FUNCTION_MASK				THREE_BIT_MASK
/* See earlier definition of VisibilityTestFunctionCodes */
#define 	M2REG_Z_16BIT_COMPARISON_ENABLE_RBOFFSET			0x97
#define 	M2REG_Z_16BIT_COMPARISON_ENABLE_SHIFT				31
#define 	M2REG_Z_16BIT_COMPARISON_ENABLE_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DEPTH_BOUND_TEST_ENABLE_PDE_RBOFFSET			0x97
#define 	M2REG_DEPTH_BOUND_TEST_ENABLE_PDE_SHIFT				28
#define 	M2REG_DEPTH_BOUND_TEST_ENABLE_PDE_MASK				(ONE_BIT_MASK<<28)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_STENCIL_TEST_ENABLE_PDE_RBOFFSET			0x97
#define 	M2REG_STENCIL_TEST_ENABLE_PDE_SHIFT				27
#define 	M2REG_STENCIL_TEST_ENABLE_PDE_MASK				(ONE_BIT_MASK<<27)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DEPTH_TEST_FUNCTION_PDE_RBOFFSET			0x97
#define 	M2REG_DEPTH_TEST_FUNCTION_PDE_SHIFT				24
#define 	M2REG_DEPTH_TEST_FUNCTION_PDE_MASK				(THREE_BIT_MASK<<24)
/* See earlier definition of VisibilityTestFunctionCodes */
#define 	M2REG_CONSTANT_STENCIL_RBOFFSET			0x97
#define 	M2REG_CONSTANT_STENCIL_SHIFT				16
#define 	M2REG_CONSTANT_STENCIL_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_STENCIL_WRITE_ENABLES_RBOFFSET			0x97
#define 	M2REG_STENCIL_WRITE_ENABLES_SHIFT				8
#define 	M2REG_STENCIL_WRITE_ENABLES_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_STENCIL_FORMAT_MASK_RBOFFSET			0x97
#define 	M2REG_STENCIL_FORMAT_MASK_SHIFT				0
#define 	M2REG_STENCIL_FORMAT_MASK_MASK				EIGHT_BIT_MASK


#define M2REG_FRAGMENT_OPERATION_ADDRESS  0x260
#define M2REG_FRAGMENT_OPERATION_COMMAND  0x98
#define 	M2REG_REFERENCE_ALPHA_RBOFFSET			0x98
#define 	M2REG_REFERENCE_ALPHA_SHIFT				16
#define 	M2REG_REFERENCE_ALPHA_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_ALPHA_BLEND_OPCODE_RBOFFSET			0x98
#define 	M2REG_ALPHA_BLEND_OPCODE_SHIFT				13
#define 	M2REG_ALPHA_BLEND_OPCODE_MASK				(THREE_BIT_MASK<<13)
typedef enum
{
	ABOC_ADD = 0x1,	/*  SrcFactor*Src + DestFactor*Dest. */
	ABOC_Subtract = 0x2,	/*  SrcFactor*Src - DestFactor*Dest. */
	ABOC_InverseSubtract = 0x3,	/*  DestFactor*Dest - SrcFactor*Src. */
	ABOC_MIN = 0x4,	/* Min(Src,  Dest). */
	ABOC_MAX = 0x5	/* Max(Src,  Dest). */
} AlphaBlendingOperationCodes;


#define 	M2REG_ALPHA_TEST_FUNCTION_RBOFFSET			0x98
#define 	M2REG_ALPHA_TEST_FUNCTION_SHIFT				10
#define 	M2REG_ALPHA_TEST_FUNCTION_MASK				(THREE_BIT_MASK<<10)
/* See earlier definition of VisibilityTestFunctionCodes */
#define 	M2REG_SOURCE_RGB_BLENDING_FACTOR_RBOFFSET			0x98
#define 	M2REG_SOURCE_RGB_BLENDING_FACTOR_SHIFT				6
#define 	M2REG_SOURCE_RGB_BLENDING_FACTOR_MASK				(FOUR_BIT_MASK<<6)
typedef enum
{
	RGBBFC_Inverse_Temp_Alpha = 0x0,	/* No comment Given */
	RGBBFC_Zero = 0x1,	/* SrcColorFactor.R = SrcColorFactor.G = SrcColorFactor.B = 0.0. */
	RGBBFC_One = 0x2,	/* default. SrcColorFactor.R = SrcColorFactor.G = SrcColorFactor.B = 1.0. */
	RGBBFC_SourceColor = 0x3,	/* SrcColorFactor = SrcColor. for RGB */
	RGBBFC_InverseSourceColor = 0x4,	/* SrcColorFactor = 1 - SrcColor.	for RGB */
	RGBBFC_SourceAlpha = 0x5,	/* SrcColorFactor.R = SrcColorFactor.G = SrcColorFactor.B = SrcColor.A. */
	RGBBFC_InverseSourceAlpha = 0x6,	/* SrcColorFactor.R = SrcColorFactor.G = SrcColorFactor.B = 1- SrcColor.A. */
	RGBBFC_DestinationAlpha = 0x7,	/* SrcColorFactor.R = SrcColorFactor.G = SrcColorFactor.B = DestColor.A. */
	RGBBFC_InverseDestinationAlpha = 0x8,	/* SrcColorFactor.R = SrcColorFactor.G = SrcColorFactor.B = 1 - DestColor.A. */
	RGBBFC_DestinationColor = 0x9,	/* SrcColorFactor = DestColor. for RGB */
	RGBBFC_InverseDestinationColor = 0xa,	/* SrcColorFactor = 1 - DestColor. for RGB */
	RGBBFC_SourceAlphaSaturate = 0xb,	/* SrcColorFactor.R = SrcColorFactor.G = SrcColorFactor.B = MIN(SrcColor.A, 1 - DestColor.A). */
	RGBBFC_ConstantColor = 0xc,	/* SrcColorFactor = ConstantColor. for RGB */
	RGBBFC_InverseConstantColor = 0xd,	/* SrcColorFactor = 1 - ConstantColor. for RGB */
	RGBBFC_ConstantAlpha = 0xe,	/* SrcColorFactor = Constant_Alpha; */
	RGBBFC_InverseConstantAlpha = 0xf	/* SrcColorFactor = 1- Constant_Alpha; */
} RGBBlendingFactorCodes;


#define 	M2REG_DESTINATION_RGB_BLENDING_FACTOR_RBOFFSET			0x98
#define 	M2REG_DESTINATION_RGB_BLENDING_FACTOR_SHIFT				2
#define 	M2REG_DESTINATION_RGB_BLENDING_FACTOR_MASK				(FOUR_BIT_MASK<<2)
/* See earlier definition of RGBBlendingFactorCodes */
#define 	M2REG_PREMULTIPLY_SOURCE_ALPHA_MODE_RBOFFSET			0x98
#define 	M2REG_PREMULTIPLY_SOURCE_ALPHA_MODE_SHIFT				0
#define 	M2REG_PREMULTIPLY_SOURCE_ALPHA_MODE_MASK				TWO_BIT_MASK
typedef enum
{
	SAPM_Disable = 0x0,	/* default. */
	SAPM_Line_Alpha_Mask = 0x1,	/* Src.alpha = (alphaMask<<4) | alphaMask;. */
	SAPM_Modulate_LineAlphaMask = 0x2,	/* Src.alpha = (Src.alpha * alphaMask) >> 4;. */
	SAPM_Modulate_ConstantAlpha = 0x3	/* Temp.alpha = Src.alpha * Constant_Alpha. for third alpha blit case. */
} SourceAlphaPremultiplyMode;


#define 	M2REG_SOURCE_ALPHA_BLENDING_FACTOR_RBOFFSET			0x99
#define 	M2REG_SOURCE_ALPHA_BLENDING_FACTOR_SHIFT				28
#define 	M2REG_SOURCE_ALPHA_BLENDING_FACTOR_MASK				(FOUR_BIT_MASK<<28)
typedef enum
{
	ABFC_Inverse_Temp_Alpha = 0x0,	/* No comment Given */
	ABFC_Zero = 0x1,	/* SrcColorFactor.A = 0.0. */
	ABFC_One = 0x2,	/* default. SrcColorFactor.A = 1.0. */
	ABFC_SourceAlpha = 0x5,	/* SrcColorFactor.A = SrcColor.A. */
	ABFC_InverseSourceAlpha = 0x6,	/* SrcColorFactor.A = 1- SrcColor.A. */
	ABFC_DestinationAlpha = 0x7,	/* SrcColorFactor.A = DestColor.A. */
	ABFC_InverseDestinationAlpha = 0x8,	/* SrcColorFactor.A = 1 - DestColor.A. */
	ABFC_ConstantAlpha = 0xe,	/* SrcColorFactor.A = Constant_Alpha; */
	ABFC_InverseConstantAlpha = 0xf	/* SrcColorFactor.A = 1- Constant_Alpha; */
} AlphaBlendingFactorCodes;


#define 	M2REG_DESTINATION_ALPHA_BLENDING_FACTOR_RBOFFSET			0x99
#define 	M2REG_DESTINATION_ALPHA_BLENDING_FACTOR_SHIFT				24
#define 	M2REG_DESTINATION_ALPHA_BLENDING_FACTOR_MASK				(FOUR_BIT_MASK<<24)
/* See earlier definition of AlphaBlendingFactorCodes */
#define 	M2REG_POLYGON_STIPPLE_ENABLE_RBOFFSET			0x99
#define 	M2REG_POLYGON_STIPPLE_ENABLE_SHIFT				23
#define 	M2REG_POLYGON_STIPPLE_ENABLE_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COLOR_DITHER_ENABLE_RBOFFSET			0x99
#define 	M2REG_COLOR_DITHER_ENABLE_SHIFT				22
#define 	M2REG_COLOR_DITHER_ENABLE_MASK				(ONE_BIT_MASK<<22)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DESTINATION_COLOR_KEY_POLARITY_RBOFFSET			0x99
#define 	M2REG_DESTINATION_COLOR_KEY_POLARITY_SHIFT				21
#define 	M2REG_DESTINATION_COLOR_KEY_POLARITY_MASK				(ONE_BIT_MASK<<21)
typedef enum
{
	CKPM_Normal = 0x0,	/* default. */
	CKPM_Invert = 0x1	/* (pixel color == key color) will write the pixel out. */
} ColorKeyPolarityMode;


#define 	M2REG_DESTINATION_COLOR_KEY_ENABLE_RBOFFSET			0x99
#define 	M2REG_DESTINATION_COLOR_KEY_ENABLE_SHIFT				20
#define 	M2REG_DESTINATION_COLOR_KEY_ENABLE_MASK				(ONE_BIT_MASK<<20)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ROP3_CODE_RBOFFSET			0x99
#define 	M2REG_ROP3_CODE_SHIFT				12
#define 	M2REG_ROP3_CODE_MASK				(EIGHT_BIT_MASK<<12)
#define 	M2REG_MONO_PATTERN_EXPANSION_RBOFFSET			0x99
#define 	M2REG_MONO_PATTERN_EXPANSION_SHIFT				11
#define 	M2REG_MONO_PATTERN_EXPANSION_MASK				(ONE_BIT_MASK<<11)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE_RBOFFSET			0x99
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE_SHIFT				10
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE_MASK				(ONE_BIT_MASK<<10)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ENABLE_BIT_MASK_RBOFFSET			0x99
#define 	M2REG_ENABLE_BIT_MASK_SHIFT				9
#define 	M2REG_ENABLE_BIT_MASK_MASK				(ONE_BIT_MASK<<9)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ROP3_ENABLE_RBOFFSET			0x99
#define 	M2REG_ROP3_ENABLE_SHIFT				8
#define 	M2REG_ROP3_ENABLE_MASK				(ONE_BIT_MASK<<8)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ALPHA_BLENDING_ENABLE_RBOFFSET			0x99
#define 	M2REG_ALPHA_BLENDING_ENABLE_SHIFT				7
#define 	M2REG_ALPHA_BLENDING_ENABLE_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ALPHA_TEST_ENABLE_RBOFFSET			0x99
#define 	M2REG_ALPHA_TEST_ENABLE_SHIFT				6
#define 	M2REG_ALPHA_TEST_ENABLE_MASK				(ONE_BIT_MASK<<6)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FOG_BLENDING_ENABLE_RBOFFSET			0x99
#define 	M2REG_FOG_BLENDING_ENABLE_SHIFT				5
#define 	M2REG_FOG_BLENDING_ENABLE_MASK				(ONE_BIT_MASK<<5)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SPECULAR_BLENDING_ENABLE_RBOFFSET			0x99
#define 	M2REG_SPECULAR_BLENDING_ENABLE_SHIFT				4
#define 	M2REG_SPECULAR_BLENDING_ENABLE_MASK				(ONE_BIT_MASK<<4)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ALPHA_POST_BLENDER_RBOFFSET			0x99
#define 	M2REG_ALPHA_POST_BLENDER_SHIFT				2
#define 	M2REG_ALPHA_POST_BLENDER_MASK				(TWO_BIT_MASK<<2)
typedef enum
{
	PBC_Constant = 0x0,	/* Result.A =  a constant alpha defined in 0x0A4. */
	PBC_Diffuse = 0x1,	/* Result.A = Diffuse.A.	default. */
	PBC_Texture = 0x2,	/* Result.A = Texture.A. */
	PBC_Modulate = 0x3	/* Result.A = Diffuse.A * Texture.A/255. */
} PostBlenderCodes;


#define 	M2REG_RGB_POST_BLENDER_RBOFFSET			0x99
#define 	M2REG_RGB_POST_BLENDER_SHIFT				0
#define 	M2REG_RGB_POST_BLENDER_MASK				TWO_BIT_MASK
/* See earlier definition of PostBlenderCodes */
#define 	M2REG_BACKGROUND_COLOR_ALPHA_RBOFFSET			0x9a
#define 	M2REG_BACKGROUND_COLOR_ALPHA_SHIFT				24
#define 	M2REG_BACKGROUND_COLOR_ALPHA_MASK				(EIGHT_BIT_MASK<<24)
#define 	M2REG_BACKGROUND_COLOR_RED_RBOFFSET			0x9a
#define 	M2REG_BACKGROUND_COLOR_RED_SHIFT				16
#define 	M2REG_BACKGROUND_COLOR_RED_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_BACKGROUND_COLOR_GREEN_RBOFFSET			0x9a
#define 	M2REG_BACKGROUND_COLOR_GREEN_SHIFT				8
#define 	M2REG_BACKGROUND_COLOR_GREEN_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_BACKGROUND_COLOR_BLUE_RBOFFSET			0x9a
#define 	M2REG_BACKGROUND_COLOR_BLUE_SHIFT				0
#define 	M2REG_BACKGROUND_COLOR_BLUE_MASK				EIGHT_BIT_MASK
#define 	M2REG_FOREGROUND_COLOR_ALPHA_RBOFFSET			0x9b
#define 	M2REG_FOREGROUND_COLOR_ALPHA_SHIFT				24
#define 	M2REG_FOREGROUND_COLOR_ALPHA_MASK				(EIGHT_BIT_MASK<<24)
#define 	M2REG_FOREGROUND_COLOR_RED_RBOFFSET			0x9b
#define 	M2REG_FOREGROUND_COLOR_RED_SHIFT				16
#define 	M2REG_FOREGROUND_COLOR_RED_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_FOREGROUND_COLOR_GREEN_RBOFFSET			0x9b
#define 	M2REG_FOREGROUND_COLOR_GREEN_SHIFT				8
#define 	M2REG_FOREGROUND_COLOR_GREEN_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_FOREGROUND_COLOR_BLUE_RBOFFSET			0x9b
#define 	M2REG_FOREGROUND_COLOR_BLUE_SHIFT				0
#define 	M2REG_FOREGROUND_COLOR_BLUE_MASK				EIGHT_BIT_MASK


#define M2REG_MISC_OPERATION_ADDRESS  0x270
#define M2REG_MISC_OPERATION_COMMAND  0x9c
#define 	M2REG_READDEPTH_GRANTPARK_THRESHOLD_RBOFFSET			0x9c
#define 	M2REG_READDEPTH_GRANTPARK_THRESHOLD_SHIFT				20
#define 	M2REG_READDEPTH_GRANTPARK_THRESHOLD_MASK				(FOUR_BIT_MASK<<20)
#define 	M2REG_READDEPTH_REQWAIT_THRESHOLD_RBOFFSET			0x9c
#define 	M2REG_READDEPTH_REQWAIT_THRESHOLD_SHIFT				16
#define 	M2REG_READDEPTH_REQWAIT_THRESHOLD_MASK				(FOUR_BIT_MASK<<16)
#define 	M2REG_READCOLOR_GRANTPARK_THRESHOLD_RBOFFSET			0x9c
#define 	M2REG_READCOLOR_GRANTPARK_THRESHOLD_SHIFT				12
#define 	M2REG_READCOLOR_GRANTPARK_THRESHOLD_MASK				(FOUR_BIT_MASK<<12)
#define 	M2REG_READCOLOR_REQWAIT_THRESHOLD_RBOFFSET			0x9c
#define 	M2REG_READCOLOR_REQWAIT_THRESHOLD_SHIFT				8
#define 	M2REG_READCOLOR_REQWAIT_THRESHOLD_MASK				(FOUR_BIT_MASK<<8)
#define 	M2REG_READTEXTURE_GRANTPARK_THRESHOLD_RBOFFSET			0x9c
#define 	M2REG_READTEXTURE_GRANTPARK_THRESHOLD_SHIFT				4
#define 	M2REG_READTEXTURE_GRANTPARK_THRESHOLD_MASK				(FOUR_BIT_MASK<<4)
#define 	M2REG_PAGE_CACHE_MODE_RBOFFSET			0x9c
#define 	M2REG_PAGE_CACHE_MODE_SHIFT				3
#define 	M2REG_PAGE_CACHE_MODE_MASK				(ONE_BIT_MASK<<3)
typedef enum
{
	PCM_OnePagePerCache = 0x0,	/* No comment Given */
	PCM_TwoPagePerCache = 0x1	/* No comment Given */
} PageCacheMode;


#define 	M2REG_COLOR_WRITE_DISABLE_RBOFFSET			0x9c
#define 	M2REG_COLOR_WRITE_DISABLE_SHIFT				2
#define 	M2REG_COLOR_WRITE_DISABLE_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CACHE_FORWARD_ENABLE_RBOFFSET			0x9c
#define 	M2REG_CACHE_FORWARD_ENABLE_SHIFT				1
#define 	M2REG_CACHE_FORWARD_ENABLE_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_PAGE_CACHE_ENABLE_RBOFFSET			0x9c
#define 	M2REG_PAGE_CACHE_ENABLE_SHIFT				0
#define 	M2REG_PAGE_CACHE_ENABLE_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE3_RBOFFSET			0x9d
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE3_SHIFT				31
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE3_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE2_RBOFFSET			0x9d
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE2_SHIFT				30
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE2_MASK				(ONE_BIT_MASK<<30)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE1_RBOFFSET			0x9d
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE1_SHIFT				29
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE1_MASK				(ONE_BIT_MASK<<29)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE0_RBOFFSET			0x9d
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE0_SHIFT				28
#define 	M2REG_CONSTANT_ALPHA_REPLACE_ENABLE0_MASK				(ONE_BIT_MASK<<28)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COLOR_PREFETCH_ENABLE_RBOFFSET			0x9d
#define 	M2REG_COLOR_PREFETCH_ENABLE_SHIFT				27
#define 	M2REG_COLOR_PREFETCH_ENABLE_MASK				(ONE_BIT_MASK<<27)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ALPHA_BLEND_OPCODE_ALPHA_RBOFFSET			0x9d
#define 	M2REG_ALPHA_BLEND_OPCODE_ALPHA_SHIFT				24
#define 	M2REG_ALPHA_BLEND_OPCODE_ALPHA_MASK				(THREE_BIT_MASK<<24)
/* See earlier definition of AlphaBlendingOperationCodes */
#define 	M2REG_DEPTH_PREFETCH_ENABLE_RBOFFSET			0x9d
#define 	M2REG_DEPTH_PREFETCH_ENABLE_SHIFT				23
#define 	M2REG_DEPTH_PREFETCH_ENABLE_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FORCE_ENDOFMERGING_RBOFFSET			0x9d
#define 	M2REG_FORCE_ENDOFMERGING_SHIFT				22
#define 	M2REG_FORCE_ENDOFMERGING_MASK				(ONE_BIT_MASK<<22)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_STOP_WAITING_WHEN_ENDOFMERGING_RBOFFSET			0x9d
#define 	M2REG_STOP_WAITING_WHEN_ENDOFMERGING_SHIFT				21
#define 	M2REG_STOP_WAITING_WHEN_ENDOFMERGING_MASK				(ONE_BIT_MASK<<21)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FORCE_ZMIN_OR_ZMAX_RBOFFSET			0x9d
#define 	M2REG_FORCE_ZMIN_OR_ZMAX_SHIFT				20
#define 	M2REG_FORCE_ZMIN_OR_ZMAX_MASK				(ONE_BIT_MASK<<20)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_PAGE_SIZE_MODE_RBOFFSET			0x9d
#define 	M2REG_PAGE_SIZE_MODE_SHIFT				18
#define 	M2REG_PAGE_SIZE_MODE_MASK				(TWO_BIT_MASK<<18)
typedef enum
{
	PSS_16x16_Pixels = 0x0,	/* single target */
	PSS_16x8_Pixels = 0x1,	/* two render targets or just small page */
	PSS_16x4_Pixels = 0x2	/* three and four render targets or just small page */
} PageSizeSelection;


#define 	M2REG_BYPASS_RAM_CONTROL_ON_RBOFFSET			0x9d
#define 	M2REG_BYPASS_RAM_CONTROL_ON_SHIFT				17
#define 	M2REG_BYPASS_RAM_CONTROL_ON_MASK				(ONE_BIT_MASK<<17)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_PREFETCH_FLUSH_ON_RBOFFSET			0x9d
#define 	M2REG_PREFETCH_FLUSH_ON_SHIFT				16
#define 	M2REG_PREFETCH_FLUSH_ON_MASK				(ONE_BIT_MASK<<16)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COLOR_COMPRESSION_ENABLE_RBOFFSET			0x9d
#define 	M2REG_COLOR_COMPRESSION_ENABLE_SHIFT				15
#define 	M2REG_COLOR_COMPRESSION_ENABLE_MASK				(ONE_BIT_MASK<<15)
#define 	M2REG_STENCIL_WRITE_ENABLE_OE_RBOFFSET			0x9d
#define 	M2REG_STENCIL_WRITE_ENABLE_OE_SHIFT				14
#define 	M2REG_STENCIL_WRITE_ENABLE_OE_MASK				(ONE_BIT_MASK<<14)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COLOR_READBACK_COUNT_LIMIT_RBOFFSET			0x9d
#define 	M2REG_COLOR_READBACK_COUNT_LIMIT_SHIFT				12
#define 	M2REG_COLOR_READBACK_COUNT_LIMIT_MASK				(TWO_BIT_MASK<<12)
#define 	M2REG_L2_REQUEST_FLUSH_THRESHOLD_RBOFFSET			0x9d
#define 	M2REG_L2_REQUEST_FLUSH_THRESHOLD_SHIFT				8
#define 	M2REG_L2_REQUEST_FLUSH_THRESHOLD_MASK				(FOUR_BIT_MASK<<8)
#define 	M2REG_L2_REQUEST_REORDER_ENABLE_RBOFFSET			0x9d
#define 	M2REG_L2_REQUEST_REORDER_ENABLE_SHIFT				7
#define 	M2REG_L2_REQUEST_REORDER_ENABLE_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_L2_REORDER_TIME_THRESHOLD_RBOFFSET			0x9d
#define 	M2REG_L2_REORDER_TIME_THRESHOLD_SHIFT				4
#define 	M2REG_L2_REORDER_TIME_THRESHOLD_MASK				(THREE_BIT_MASK<<4)
#define 	M2REG_ROUND_FACTOR_FOR12_10_RBOFFSET			0x9d
#define 	M2REG_ROUND_FACTOR_FOR12_10_SHIFT				0
#define 	M2REG_ROUND_FACTOR_FOR12_10_MASK				FOUR_BIT_MASK
#define 	M2REG_CONVERSION_BLIT_ENABLE_RBOFFSET			0x9e
#define 	M2REG_CONVERSION_BLIT_ENABLE_SHIFT				29
#define 	M2REG_CONVERSION_BLIT_ENABLE_MASK				(ONE_BIT_MASK<<29)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_ALPHA_BLENDING_ROUNDING_MODE_RBOFFSET			0x9e
#define 	M2REG_ALPHA_BLENDING_ROUNDING_MODE_SHIFT				28
#define 	M2REG_ALPHA_BLENDING_ROUNDING_MODE_MASK				(ONE_BIT_MASK<<28)
typedef enum
{
	ABRM_Add_Round_Clamp = 0x0,	/* default. */
	ABRM_Round_Add_Clamp = 0x1	/* No comment Given */
} AlphaBlendingRoundingMode;


#define 	M2REG_BYPASS_DITHER_RBOFFSET			0x9e
#define 	M2REG_BYPASS_DITHER_SHIFT				27
#define 	M2REG_BYPASS_DITHER_MASK				(ONE_BIT_MASK<<27)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE3_RBOFFSET			0x9e
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE3_SHIFT				26
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE3_MASK				(ONE_BIT_MASK<<26)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE2_RBOFFSET			0x9e
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE2_SHIFT				25
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE2_MASK				(ONE_BIT_MASK<<25)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE1_RBOFFSET			0x9e
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE1_SHIFT				24
#define 	M2REG_DESTINATION_COLOR_BUFFER_READ_ENABLE1_MASK				(ONE_BIT_MASK<<24)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COMPONENT_MASK_RBOFFSET			0x9e
#define 	M2REG_COMPONENT_MASK_SHIFT				20
#define 	M2REG_COMPONENT_MASK_MASK				(FOUR_BIT_MASK<<20)
#define 	M2REG_CONVERSION_BLT_CODE_RBOFFSET			0x9e
#define 	M2REG_CONVERSION_BLT_CODE_SHIFT				18
#define 	M2REG_CONVERSION_BLT_CODE_MASK				(TWO_BIT_MASK<<18)
typedef enum
{
	CBC_CompressColorBuffer = 0x0,	/* from decompressed to compressed */
	CBC_DeCompressColorBuffer = 0x1,	/* from compressed to decompressed */
	CBC_DepthToColor = 0x2,	/* No comment Given */
	CBC_ColorToDepth = 0x3	/* No comment Given */
} ConversionBlitCode;


#define 	M2REG_TIME_STAMP_ON_RBOFFSET			0x9e
#define 	M2REG_TIME_STAMP_ON_SHIFT				17
#define 	M2REG_TIME_STAMP_ON_MASK				(ONE_BIT_MASK<<17)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_REQUEST_COMBINE_RBOFFSET			0x9e
#define 	M2REG_REQUEST_COMBINE_SHIFT				16
#define 	M2REG_REQUEST_COMBINE_MASK				(ONE_BIT_MASK<<16)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_AGP_ARBITRATION_THRESHOLD_RBOFFSET			0x9e
#define 	M2REG_AGP_ARBITRATION_THRESHOLD_SHIFT				13
#define 	M2REG_AGP_ARBITRATION_THRESHOLD_MASK				(THREE_BIT_MASK<<13)
#define 	M2REG_ZPORT_ARBITRATION_THRESHOLD_RBOFFSET			0x9e
#define 	M2REG_ZPORT_ARBITRATION_THRESHOLD_SHIFT				10
#define 	M2REG_ZPORT_ARBITRATION_THRESHOLD_MASK				(THREE_BIT_MASK<<10)
#define 	M2REG_COLORPORT_ARBITRATION_THRESHOLD_RBOFFSET			0x9e
#define 	M2REG_COLORPORT_ARBITRATION_THRESHOLD_SHIFT				7
#define 	M2REG_COLORPORT_ARBITRATION_THRESHOLD_MASK				(THREE_BIT_MASK<<7)
#define 	M2REG_ALPHA_BLENDING_NEED_DESTINATION_COLOR_RBOFFSET			0x9e
#define 	M2REG_ALPHA_BLENDING_NEED_DESTINATION_COLOR_SHIFT				3
#define 	M2REG_ALPHA_BLENDING_NEED_DESTINATION_COLOR_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_EOMERGE_GENERATION_MODE_RBOFFSET			0x9e
#define 	M2REG_EOMERGE_GENERATION_MODE_SHIFT				2
#define 	M2REG_EOMERGE_GENERATION_MODE_MASK				(ONE_BIT_MASK<<2)
typedef enum
{
	EOMGM_Necessarily = 0x0,	/* No comment Given */
	EOMGM_Always = 0x1	/* No comment Given */
} EOMergeGenerationMode;


#define 	M2REG_COLOR_SYNC_DEPTH_DISABLE_RBOFFSET			0x9e
#define 	M2REG_COLOR_SYNC_DEPTH_DISABLE_SHIFT				1
#define 	M2REG_COLOR_SYNC_DEPTH_DISABLE_MASK				(ONE_BIT_MASK<<1)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_COLOR_BUFFER_PORT_RBOFFSET			0x9e
#define 	M2REG_COLOR_BUFFER_PORT_SHIFT				0
#define 	M2REG_COLOR_BUFFER_PORT_MASK				ONE_BIT_MASK
/* See earlier definition of MemoryPortConfiguration */
#define 	M2REG_WRITE_DEPTHCOLOR_GTANTPARK_THRESHOLD_RBOFFSET			0x9f
#define 	M2REG_WRITE_DEPTHCOLOR_GTANTPARK_THRESHOLD_SHIFT				28
#define 	M2REG_WRITE_DEPTHCOLOR_GTANTPARK_THRESHOLD_MASK				(FOUR_BIT_MASK<<28)
#define 	M2REG_WRITEDEPTH_REQWAIT_THRESHOLD_RBOFFSET			0x9f
#define 	M2REG_WRITEDEPTH_REQWAIT_THRESHOLD_SHIFT				24
#define 	M2REG_WRITEDEPTH_REQWAIT_THRESHOLD_MASK				(FOUR_BIT_MASK<<24)
#define 	M2REG_POLYGON_STIPPLE_XOFFSET_RBOFFSET			0x9f
#define 	M2REG_POLYGON_STIPPLE_XOFFSET_SHIFT				18
#define 	M2REG_POLYGON_STIPPLE_XOFFSET_MASK				(FIVE_BIT_MASK<<18)
#define 	M2REG_POLYGON_STIPPLE_YOFFSET_RBOFFSET			0x9f
#define 	M2REG_POLYGON_STIPPLE_YOFFSET_SHIFT				12
#define 	M2REG_POLYGON_STIPPLE_YOFFSET_MASK				(FIVE_BIT_MASK<<12)
#define 	M2REG_STENCIL_OPERATION_SPASS_ZPASS_CCW_RBOFFSET			0x9f
#define 	M2REG_STENCIL_OPERATION_SPASS_ZPASS_CCW_SHIFT				9
#define 	M2REG_STENCIL_OPERATION_SPASS_ZPASS_CCW_MASK				(THREE_BIT_MASK<<9)
/* See earlier definition of StencilOperationCode */
#define 	M2REG_STENCIL_OPERATION_SPASS_ZFAIL_CCW_RBOFFSET			0x9f
#define 	M2REG_STENCIL_OPERATION_SPASS_ZFAIL_CCW_SHIFT				6
#define 	M2REG_STENCIL_OPERATION_SPASS_ZFAIL_CCW_MASK				(THREE_BIT_MASK<<6)
/* See earlier definition of StencilOperationCode */
#define 	M2REG_STENCIL_OPERATION_SFAIL_CCW_RBOFFSET			0x9f
#define 	M2REG_STENCIL_OPERATION_SFAIL_CCW_SHIFT				3
#define 	M2REG_STENCIL_OPERATION_SFAIL_CCW_MASK				(THREE_BIT_MASK<<3)
/* See earlier definition of StencilOperationCode */
#define 	M2REG_STENCIL_TEST_FUNCTION_CCW_RBOFFSET			0x9f
#define 	M2REG_STENCIL_TEST_FUNCTION_CCW_SHIFT				0
#define 	M2REG_STENCIL_TEST_FUNCTION_CCW_MASK				THREE_BIT_MASK
/* See earlier definition of VisibilityTestFunctionCodes */


#define M2REG_COLOR_BUFFER_ADDRESS  0x280
#define M2REG_COLOR_BUFFER_COMMAND  0xa0
#define 	M2REG_DESTINATION_SELECTION_RBOFFSET			0xa0
#define 	M2REG_DESTINATION_SELECTION_SHIFT				22
#define 	M2REG_DESTINATION_SELECTION_MASK				(TWO_BIT_MASK<<22)
typedef enum
{
	DS_Destination_Buffer = 0x0,	/* No comment Given */
	DS_Diffuse = 0x1,	/* No comment Given */
	DS_Specular = 0x2,	/* No comment Given */
	DS_Background_Global = 0x3	/* No comment Given */
} DestinationSelector;


#define 	M2REG_FOG_COLOR_SELECTION_RBOFFSET			0xa0
#define 	M2REG_FOG_COLOR_SELECTION_SHIFT				20
#define 	M2REG_FOG_COLOR_SELECTION_MASK				(TWO_BIT_MASK<<20)
typedef enum
{
	FCS_Fog_Color_Global = 0x0,	/* No comment Given */
	FCS_Diffuse = 0x1,	/* No comment Given */
	FCS_Specular = 0x2,	/* No comment Given */
	FCS_Texture = 0x3	/* No comment Given */
} FogColorSelector;


#define 	M2REG_MRT_MODE_RBOFFSET			0xa0
#define 	M2REG_MRT_MODE_SHIFT				19
#define 	M2REG_MRT_MODE_MASK				(ONE_BIT_MASK<<19)
typedef enum
{
	MRTM_MET_mode = 0x0,	/* default. */
	MRTM_MRT_mode = 0x1	/* D0[23:22] must be non-zero. */
} MultipleRenderTargetMode;


#define 	M2REG_MAIN_TARGET_COLOR_FORMAT_RBOFFSET			0xa0
#define 	M2REG_MAIN_TARGET_COLOR_FORMAT_SHIFT				16
#define 	M2REG_MAIN_TARGET_COLOR_FORMAT_MASK				(THREE_BIT_MASK<<16)
typedef enum
{
	RTF_RGB565 = 0x0,	/* default. 16bpp */
	RTF_ARGB1555 = 0x1,	/* 16 bpp. */
	RTF_ARGB4444 = 0x2,	/* 16 bpp. */
	RTF_ARGB8888 = 0x4,	/* 32 bpp. */
	RTF_ARGB2_10_10_10 = 0x5	/* 32 bpp. */
} RenderTargetFormat;


#define 	M2REG_TOTAL_TARGETS_ELEMENTS_RBOFFSET			0xa0
#define 	M2REG_TOTAL_TARGETS_ELEMENTS_SHIFT				14
#define 	M2REG_TOTAL_TARGETS_ELEMENTS_MASK				(TWO_BIT_MASK<<14)
#define 	M2REG_BANDED_TILED_MODE_RBOFFSET			0xa0
#define 	M2REG_BANDED_TILED_MODE_SHIFT				12
#define 	M2REG_BANDED_TILED_MODE_MASK				(TWO_BIT_MASK<<12)
typedef enum
{
	BTM_Linear = 0x0,	/* when color compression on, it also means 2 scanlines linear. */
	BTM_Tiled = 0x1,	/* 4x4 pixel tiled. */
	BTM_Banded64 = 0x2,	/* 64*16 pixel banded. when color compression on (0x270_D1_15), it means banndedcompression mode */
	BTM_Banded32_Tiled = 0x3	/* 32*32 pixel banded + 4*4 pixel tiled. */
} BandTileMode;


#define 	M2REG_COLOR_INTERLEAVE_WITH_ZBUFFER_RBOFFSET			0xa0
#define 	M2REG_COLOR_INTERLEAVE_WITH_ZBUFFER_SHIFT				11
#define 	M2REG_COLOR_INTERLEAVE_WITH_ZBUFFER_MASK				(ONE_BIT_MASK<<11)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COLOR_DESTINATION_BUFFER_WIDTH_RBOFFSET			0xa0
#define 	M2REG_COLOR_DESTINATION_BUFFER_WIDTH_SHIFT				0
#define 	M2REG_COLOR_DESTINATION_BUFFER_WIDTH_MASK				ELEVEN_BIT_MASK
#define 	M2REG_MRT_READ_ORDER_RBOFFSET			0xa1
#define 	M2REG_MRT_READ_ORDER_SHIFT				26
#define 	M2REG_MRT_READ_ORDER_MASK				(SIX_BIT_MASK<<26)
#define 	M2REG_TILE2X2X4X4_ENABLE_RBOFFSET			0xa1
#define 	M2REG_TILE2X2X4X4_ENABLE_SHIFT				25
#define 	M2REG_TILE2X2X4X4_ENABLE_MASK				(ONE_BIT_MASK<<25)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_GAMMA_CORRECTION_ENABLE_RBOFFSET			0xa1
#define 	M2REG_GAMMA_CORRECTION_ENABLE_SHIFT				24
#define 	M2REG_GAMMA_CORRECTION_ENABLE_MASK				(ONE_BIT_MASK<<24)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS0_RBOFFSET			0xa1
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS0_SHIFT				0
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS0_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_DESTINATION_COLOR_KEY_CONST_COLOR_RBOFFSET			0xa2
#define 	M2REG_DESTINATION_COLOR_KEY_CONST_COLOR_SHIFT				0
#define 	M2REG_DESTINATION_COLOR_KEY_CONST_COLOR_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_COLOR_WRITE_BIT_MASK0_RBOFFSET			0xa3
#define 	M2REG_COLOR_WRITE_BIT_MASK0_SHIFT				0
#define 	M2REG_COLOR_WRITE_BIT_MASK0_MASK				THIRTYTWO_BIT_MASK


#define M2REG_RENDER_TARGETS_ADDRESS  0x290
#define M2REG_RENDER_TARGETS_COMMAND  0xa4
#define 	M2REG_TARGET1_COLOR_FORMAT_RBOFFSET			0xa4
#define 	M2REG_TARGET1_COLOR_FORMAT_SHIFT				0
#define 	M2REG_TARGET1_COLOR_FORMAT_MASK				THREE_BIT_MASK
/* See earlier definition of RenderTargetFormat */
#define 	M2REG_BIT_MASKING_ENABLE1_RBOFFSET			0xa4
#define 	M2REG_BIT_MASKING_ENABLE1_SHIFT				3
#define 	M2REG_BIT_MASKING_ENABLE1_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COMPONENT_MASK1_RBOFFSET			0xa4
#define 	M2REG_COMPONENT_MASK1_SHIFT				4
#define 	M2REG_COMPONENT_MASK1_MASK				(FOUR_BIT_MASK<<4)
#define 	M2REG_TARGET2_COLOR_FORMAT_RBOFFSET			0xa4
#define 	M2REG_TARGET2_COLOR_FORMAT_SHIFT				8
#define 	M2REG_TARGET2_COLOR_FORMAT_MASK				(THREE_BIT_MASK<<8)
/* See earlier definition of RenderTargetFormat */
#define 	M2REG_BIT_MASKING_ENABLE2_RBOFFSET			0xa4
#define 	M2REG_BIT_MASKING_ENABLE2_SHIFT				11
#define 	M2REG_BIT_MASKING_ENABLE2_MASK				(ONE_BIT_MASK<<11)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COMPONENT_MASK2_RBOFFSET			0xa4
#define 	M2REG_COMPONENT_MASK2_SHIFT				12
#define 	M2REG_COMPONENT_MASK2_MASK				(FOUR_BIT_MASK<<12)
#define 	M2REG_TARGET3_COLOR_FORMAT_RBOFFSET			0xa4
#define 	M2REG_TARGET3_COLOR_FORMAT_SHIFT				16
#define 	M2REG_TARGET3_COLOR_FORMAT_MASK				(THREE_BIT_MASK<<16)
/* See earlier definition of RenderTargetFormat */
#define 	M2REG_BIT_MASKING_ENABLE3_RBOFFSET			0xa4
#define 	M2REG_BIT_MASKING_ENABLE3_SHIFT				19
#define 	M2REG_BIT_MASKING_ENABLE3_MASK				(ONE_BIT_MASK<<19)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COMPONENT_MASK3_RBOFFSET			0xa4
#define 	M2REG_COMPONENT_MASK3_SHIFT				20
#define 	M2REG_COMPONENT_MASK3_MASK				(FOUR_BIT_MASK<<20)
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS1_RBOFFSET			0xa5
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS1_SHIFT				0
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS1_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_BIT_MASK1_RBOFFSET			0xa6
#define 	M2REG_BIT_MASK1_SHIFT				0
#define 	M2REG_BIT_MASK1_MASK				THIRTYTWO_BIT_MASK


#define M2REG_RENDER_TARGET23_ADDRESS  0x2a0
#define M2REG_RENDER_TARGET23_COMMAND  0xa8
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS2_RBOFFSET			0xa8
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS2_SHIFT				0
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS2_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS3_RBOFFSET			0xa9
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS3_SHIFT				0
#define 	M2REG_COLOR_BUFFER_BASE_ADDRESS3_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_BIT_MASK2_RBOFFSET			0xaa
#define 	M2REG_BIT_MASK2_SHIFT				0
#define 	M2REG_BIT_MASK2_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_BIT_MASK3_RBOFFSET			0xab
#define 	M2REG_BIT_MASK3_SHIFT				0
#define 	M2REG_BIT_MASK3_MASK				THIRTYTWO_BIT_MASK


#define M2REG_CONVERSION_BLIT_RECTANGLE_ADDRESS  0x2c0
#define M2REG_CONVERSION_BLIT_RECTANGLE_COMMAND  0xb0
#define 	M2REG_SOURCE_YSTART_RBOFFSET			0xb0
#define 	M2REG_SOURCE_YSTART_SHIFT				12
#define 	M2REG_SOURCE_YSTART_MASK				(TWELVE_BIT_MASK<<12)
#define 	M2REG_SOURCE_XSTART_RBOFFSET			0xb0
#define 	M2REG_SOURCE_XSTART_SHIFT				0
#define 	M2REG_SOURCE_XSTART_MASK				TWELVE_BIT_MASK
#define 	M2REG_BLIT_FILL_MODE_ENABLE_RBOFFSET			0xb1
#define 	M2REG_BLIT_FILL_MODE_ENABLE_SHIFT				31
#define 	M2REG_BLIT_FILL_MODE_ENABLE_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RECT_HEIGHT_RBOFFSET			0xb1
#define 	M2REG_RECT_HEIGHT_SHIFT				12
#define 	M2REG_RECT_HEIGHT_MASK				(TWELVE_BIT_MASK<<12)
#define 	M2REG_RECT_WIDTH_RBOFFSET			0xb1
#define 	M2REG_RECT_WIDTH_SHIFT				0
#define 	M2REG_RECT_WIDTH_MASK				TWELVE_BIT_MASK
#define 	M2REG_DEST_YSTART_RBOFFSET			0xb2
#define 	M2REG_DEST_YSTART_SHIFT				12
#define 	M2REG_DEST_YSTART_MASK				(TWELVE_BIT_MASK<<12)
#define 	M2REG_DEST_XSTART_RBOFFSET			0xb2
#define 	M2REG_DEST_XSTART_SHIFT				0
#define 	M2REG_DEST_XSTART_MASK				TWELVE_BIT_MASK


#define M2REG_DEPTH_BOUND_RANGE_ADDRESS  0x2d0
#define M2REG_DEPTH_BOUND_RANGE_COMMAND  0xb4
#define 	M2REG_DEPTH_MIN_RBOFFSET			0xb4
#define 	M2REG_DEPTH_MIN_SHIFT				0
#define 	M2REG_DEPTH_MIN_MASK				TWENTYFOUR_BIT_MASK
#define 	M2REG_DEPTH_MAX_RBOFFSET			0xb5
#define 	M2REG_DEPTH_MAX_SHIFT				0
#define 	M2REG_DEPTH_MAX_MASK				TWENTYFOUR_BIT_MASK


#define M2REG_QUERY_STATISTIC_WRITE_ADDRESS  0x2e0
#define M2REG_QUERY_STATISTIC_WRITE_COMMAND  0xb8
#define 	M2REG_EXTRA_QUERY_TYPE_RBOFFSET			0xb8
#define 	M2REG_EXTRA_QUERY_TYPE_SHIFT				8
#define 	M2REG_EXTRA_QUERY_TYPE_MASK				(SIXTEEN_BIT_MASK<<8)
#define 	M2REG_QUERY_TYPE_RBOFFSET			0xb8
#define 	M2REG_QUERY_TYPE_SHIFT				4
#define 	M2REG_QUERY_TYPE_MASK				(FOUR_BIT_MASK<<4)
#define 	M2REG_CLEAR_COUNTER_AFTER_QUERY_RBOFFSET			0xb8
#define 	M2REG_CLEAR_COUNTER_AFTER_QUERY_SHIFT				3
#define 	M2REG_CLEAR_COUNTER_AFTER_QUERY_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CLEAR_ALL_COUNTERS_RBOFFSET			0xb8
#define 	M2REG_CLEAR_ALL_COUNTERS_SHIFT				2
#define 	M2REG_CLEAR_ALL_COUNTERS_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_QUERY_COMMAND_TYPE_RBOFFSET			0xb8
#define 	M2REG_QUERY_COMMAND_TYPE_SHIFT				0
#define 	M2REG_QUERY_COMMAND_TYPE_MASK				TWO_BIT_MASK
#define 	M2REG_DEST_MEMORY_PORT_RBOFFSET			0xb9
#define 	M2REG_DEST_MEMORY_PORT_SHIFT				31
#define 	M2REG_DEST_MEMORY_PORT_MASK				(ONE_BIT_MASK<<31)
typedef enum
{
	DMP_Local_FB = 0x0,	/* Frame Buffer 1 (Local Frame Buffer) */
	DMP_PCI_E = 0x1	/* Frame Buffer 0 or PCI_E */
} DestMemoryPort;


#define 	M2REG_DEST_MEMORY_ADDRESS_RBOFFSET			0xb9
#define 	M2REG_DEST_MEMORY_ADDRESS_SHIFT				0
#define 	M2REG_DEST_MEMORY_ADDRESS_MASK				THIRTY_BIT_MASK
#define 	M2REG_QUERY_SOURCE0_RBOFFSET			0xba
#define 	M2REG_QUERY_SOURCE0_SHIFT				0
#define 	M2REG_QUERY_SOURCE0_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_QUERY_SOURCE1_RBOFFSET			0xbb
#define 	M2REG_QUERY_SOURCE1_SHIFT				0
#define 	M2REG_QUERY_SOURCE1_MASK				THIRTYTWO_BIT_MASK


#define M2REG_BUMP_SHADER_SETTING_ADDRESS  0x300
#define M2REG_BUMP_SHADER_SETTING_COMMAND  0xc0
#define 	M2REG_BUMP_SYNC_ENABLES_RBOFFSET			0xc0
#define 	M2REG_BUMP_SYNC_ENABLES_SHIFT				16
#define 	M2REG_BUMP_SYNC_ENABLES_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP7_RBOFFSET			0xc0
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP7_SHIFT				23
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP7_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP6_RBOFFSET			0xc0
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP6_SHIFT				22
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP6_MASK				(ONE_BIT_MASK<<22)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP5_RBOFFSET			0xc0
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP5_SHIFT				21
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP5_MASK				(ONE_BIT_MASK<<21)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP4_RBOFFSET			0xc0
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP4_SHIFT				20
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP4_MASK				(ONE_BIT_MASK<<20)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP3_RBOFFSET			0xc0
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP3_SHIFT				19
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP3_MASK				(ONE_BIT_MASK<<19)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP2_RBOFFSET			0xc0
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP2_SHIFT				18
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP2_MASK				(ONE_BIT_MASK<<18)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP1_RBOFFSET			0xc0
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP1_SHIFT				17
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP1_MASK				(ONE_BIT_MASK<<17)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP0_RBOFFSET			0xc0
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP0_SHIFT				16
#define 	M2REG_BUMP_SYNC_ENABLE_LOOP0_MASK				(ONE_BIT_MASK<<16)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_INVISIBLE_PIXELPAIR_DROP_ENABLE_RBOFFSET			0xc0
#define 	M2REG_INVISIBLE_PIXELPAIR_DROP_ENABLE_SHIFT				15
#define 	M2REG_INVISIBLE_PIXELPAIR_DROP_ENABLE_MASK				(ONE_BIT_MASK<<15)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SHADER_TOTAL_LOOPS_RBOFFSET			0xc0
#define 	M2REG_BUMP_SHADER_TOTAL_LOOPS_SHIFT				12
#define 	M2REG_BUMP_SHADER_TOTAL_LOOPS_MASK				(THREE_BIT_MASK<<12)
#define 	M2REG_DIFFUSE_ONLY_RBOFFSET			0xc0
#define 	M2REG_DIFFUSE_ONLY_SHIFT				11
#define 	M2REG_DIFFUSE_ONLY_MASK				(ONE_BIT_MASK<<11)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BUMP_SHADER_MODE_RBOFFSET			0xc0
#define 	M2REG_BUMP_SHADER_MODE_SHIFT				9
#define 	M2REG_BUMP_SHADER_MODE_MASK				(TWO_BIT_MASK<<9)
typedef enum
{
	BSM_One_Loop_Only = 0x0,	/* no bump loop, consistent to total bump loop = 0. */
	BSM_2D_Bump_Loop = 0x1,	/* No comment Given */
	BSM_4D_Bump_Loop = 0x2,	/* No comment Given */
	BSM_2D_4D_Bump_Mixed = 0x3	/* No comment Given */
} BumpShaderMode;


#define 	M2REG_BUMP_SHADER_MAXIMUM_TILES_FOR_LOOP_RBOFFSET			0xc0
#define 	M2REG_BUMP_SHADER_MAXIMUM_TILES_FOR_LOOP_SHIFT				4
#define 	M2REG_BUMP_SHADER_MAXIMUM_TILES_FOR_LOOP_MASK				(FIVE_BIT_MASK<<4)
#define 	M2REG_BYPASS_L2_CACHE_RBOFFSET			0xc0
#define 	M2REG_BYPASS_L2_CACHE_SHIFT				3
#define 	M2REG_BYPASS_L2_CACHE_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BYPASS_L1_CACHE_RBOFFSET			0xc0
#define 	M2REG_BYPASS_L1_CACHE_SHIFT				2
#define 	M2REG_BYPASS_L1_CACHE_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CLEAR_L2_CACHE_RBOFFSET			0xc0
#define 	M2REG_CLEAR_L2_CACHE_SHIFT				1
#define 	M2REG_CLEAR_L2_CACHE_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CLEAR_L1_CACHE_RBOFFSET			0xc0
#define 	M2REG_CLEAR_L1_CACHE_SHIFT				0
#define 	M2REG_CLEAR_L1_CACHE_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LOOP0_BUMP_INSTRUCTION_ENTRY_RBOFFSET			0xc1
#define 	M2REG_LOOP0_BUMP_INSTRUCTION_ENTRY_SHIFT				0
#define 	M2REG_LOOP0_BUMP_INSTRUCTION_ENTRY_MASK				SIX_BIT_MASK
#define 	M2REG_ANISOTROPIC_FILTERING_ENABLED_RBOFFSET			0xc1
#define 	M2REG_ANISOTROPIC_FILTERING_ENABLED_SHIFT				7
#define 	M2REG_ANISOTROPIC_FILTERING_ENABLED_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LOOP1_BUMP_INSTRUCTION_ENTRY_RBOFFSET			0xc1
#define 	M2REG_LOOP1_BUMP_INSTRUCTION_ENTRY_SHIFT				8
#define 	M2REG_LOOP1_BUMP_INSTRUCTION_ENTRY_MASK				(SIX_BIT_MASK<<8)
#define 	M2REG_LOOP2_BUMP_INSTRUCTION_ENTRY_RBOFFSET			0xc1
#define 	M2REG_LOOP2_BUMP_INSTRUCTION_ENTRY_SHIFT				16
#define 	M2REG_LOOP2_BUMP_INSTRUCTION_ENTRY_MASK				(SIX_BIT_MASK<<16)
#define 	M2REG_LOOP3_BUMP_INSTRUCTION_ENTRY_RBOFFSET			0xc1
#define 	M2REG_LOOP3_BUMP_INSTRUCTION_ENTRY_SHIFT				24
#define 	M2REG_LOOP3_BUMP_INSTRUCTION_ENTRY_MASK				(SIX_BIT_MASK<<24)
#define 	M2REG_LOOP4_BUMP_INSTRUCTION_ENTRY_RBOFFSET			0xc2
#define 	M2REG_LOOP4_BUMP_INSTRUCTION_ENTRY_SHIFT				0
#define 	M2REG_LOOP4_BUMP_INSTRUCTION_ENTRY_MASK				SIX_BIT_MASK
#define 	M2REG_LOOP5_BUMP_INSTRUCTION_ENTRY_RBOFFSET			0xc2
#define 	M2REG_LOOP5_BUMP_INSTRUCTION_ENTRY_SHIFT				8
#define 	M2REG_LOOP5_BUMP_INSTRUCTION_ENTRY_MASK				(SIX_BIT_MASK<<8)
#define 	M2REG_LOOP6_BUMP_INSTRUCTION_ENTRY_RBOFFSET			0xc2
#define 	M2REG_LOOP6_BUMP_INSTRUCTION_ENTRY_SHIFT				16
#define 	M2REG_LOOP6_BUMP_INSTRUCTION_ENTRY_MASK				(SIX_BIT_MASK<<16)
#define 	M2REG_LOOP7_BUMP_INSTRUCTION_ENTRY_RBOFFSET			0xc2
#define 	M2REG_LOOP7_BUMP_INSTRUCTION_ENTRY_SHIFT				24
#define 	M2REG_LOOP7_BUMP_INSTRUCTION_ENTRY_MASK				(SIX_BIT_MASK<<24)
#define 	M2REG_LEFTL1_TEXTUREID_THRESHOLD_RBOFFSET			0xc3
#define 	M2REG_LEFTL1_TEXTUREID_THRESHOLD_SHIFT				0
#define 	M2REG_LEFTL1_TEXTUREID_THRESHOLD_MASK				FOUR_BIT_MASK
#define 	M2REG_LEFTL1_TEXIDTHRESHOLD_ENABLE_RBOFFSET			0xc3
#define 	M2REG_LEFTL1_TEXIDTHRESHOLD_ENABLE_SHIFT				4
#define 	M2REG_LEFTL1_TEXIDTHRESHOLD_ENABLE_MASK				(ONE_BIT_MASK<<4)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RIGHTL1_TEXIDTHRESHOLD_ENABLE_RBOFFSET			0xc3
#define 	M2REG_RIGHTL1_TEXIDTHRESHOLD_ENABLE_SHIFT				5
#define 	M2REG_RIGHTL1_TEXIDTHRESHOLD_ENABLE_MASK				(ONE_BIT_MASK<<5)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RIGHTL1_TEXTUREID_THRESHOLD_RBOFFSET			0xc3
#define 	M2REG_RIGHTL1_TEXTUREID_THRESHOLD_SHIFT				8
#define 	M2REG_RIGHTL1_TEXTUREID_THRESHOLD_MASK				(FOUR_BIT_MASK<<8)
#define 	M2REG_FLOAT_TEXCOORD_ENABLE_RBOFFSET			0xc3
#define 	M2REG_FLOAT_TEXCOORD_ENABLE_SHIFT				12
#define 	M2REG_FLOAT_TEXCOORD_ENABLE_MASK				(ONE_BIT_MASK<<12)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_COLOR_REPLACE_ENABLE_RBOFFSET			0xc3
#define 	M2REG_COLOR_REPLACE_ENABLE_SHIFT				16
#define 	M2REG_COLOR_REPLACE_ENABLE_MASK				(SIXTEEN_BIT_MASK<<16)
typedef enum
{
	CRM_No_Replacement = 0x0,	/* No comment Given */
	CRM_Replace_Diffuse = 0x1,	/* No comment Given */
	CRM_Replace_Specular = 0x2,	/* No comment Given */
	CRM_Replace_Diffuse_Specular = 0x3	/* No comment Given */
} ColorReplacementMode;




#define M2REG_PIXELBASED_LODBIAS_TEXTURELOAD_ADDRESS  0x310
#define M2REG_PIXELBASED_LODBIAS_TEXTURELOAD_COMMAND  0xc4
#define 	M2REG_PIXEL_LODBIAS_ENABLES32_RBOFFSET			0xc5
#define 	M2REG_PIXEL_LODBIAS_ENABLES32_SHIFT				0
#define 	M2REG_PIXEL_LODBIAS_ENABLES32_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_PIXEL_LODBIAS_ENABLES64_RBOFFSET			0xc6
#define 	M2REG_PIXEL_LODBIAS_ENABLES64_SHIFT				0
#define 	M2REG_PIXEL_LODBIAS_ENABLES64_MASK				THIRTYTWO_BIT_MASK
#define 	M2REG_RGAA_ENABLES_RBOFFSET			0xc7
#define 	M2REG_RGAA_ENABLES_SHIFT				0
#define 	M2REG_RGAA_ENABLES_MASK				THIRTYTWO_BIT_MASK


#define M2REG_L1_INDEX_ADJUSTMENT_ADDRESS  0x320
#define M2REG_L1_INDEX_ADJUSTMENT_COMMAND  0xc8
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP0_RBOFFSET			0xc8
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP0_SHIFT				0
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP0_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP0_RBOFFSET			0xc8
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP0_SHIFT				1
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP0_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP1_RBOFFSET			0xc8
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP1_SHIFT				2
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP1_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP1_RBOFFSET			0xc8
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP1_SHIFT				3
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP1_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP2_RBOFFSET			0xc8
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP2_SHIFT				4
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP2_MASK				(ONE_BIT_MASK<<4)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP2_RBOFFSET			0xc8
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP2_SHIFT				5
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP2_MASK				(ONE_BIT_MASK<<5)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP3_RBOFFSET			0xc8
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP3_SHIFT				6
#define 	M2REG_LEFTL1TEXIDTHRESHOLDENABLELOOP3_MASK				(ONE_BIT_MASK<<6)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP3_RBOFFSET			0xc8
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP3_SHIFT				7
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDENABLELOOP3_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LOD_COPY_MODE_ENABLE_RBOFFSET			0xc8
#define 	M2REG_LOD_COPY_MODE_ENABLE_SHIFT				8
#define 	M2REG_LOD_COPY_MODE_ENABLE_MASK				(ONE_BIT_MASK<<8)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP0_RBOFFSET			0xc8
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP0_SHIFT				9
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP0_MASK				(ONE_BIT_MASK<<9)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LEFTL1INDEX1REPLACEMENTENABLELOOP1_RBOFFSET			0xc8
#define 	M2REG_LEFTL1INDEX1REPLACEMENTENABLELOOP1_SHIFT				10
#define 	M2REG_LEFTL1INDEX1REPLACEMENTENABLELOOP1_MASK				(ONE_BIT_MASK<<10)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP1_RBOFFSET			0xc8
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP1_SHIFT				11
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP1_MASK				(ONE_BIT_MASK<<11)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LEFTL1INDEX1REPLACEMENTENABLELOOP2_RBOFFSET			0xc8
#define 	M2REG_LEFTL1INDEX1REPLACEMENTENABLELOOP2_SHIFT				12
#define 	M2REG_LEFTL1INDEX1REPLACEMENTENABLELOOP2_MASK				(ONE_BIT_MASK<<12)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP2_RBOFFSET			0xc8
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP2_SHIFT				13
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP2_MASK				(ONE_BIT_MASK<<13)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LEFTL1INDEX1REPLACEMENTENABLELOOP3_RBOFFSET			0xc8
#define 	M2REG_LEFTL1INDEX1REPLACEMENTENABLELOOP3_SHIFT				14
#define 	M2REG_LEFTL1INDEX1REPLACEMENTENABLELOOP3_MASK				(ONE_BIT_MASK<<14)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP3_RBOFFSET			0xc8
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP3_SHIFT				15
#define 	M2REG_RIGHTL1INDEX1REPLACEMENTENABLELOOP3_MASK				(ONE_BIT_MASK<<15)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP0_RBOFFSET			0xc9
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP0_SHIFT				0
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP0_MASK				FOUR_BIT_MASK
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP0_RBOFFSET			0xc9
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP0_SHIFT				4
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP0_MASK				(FOUR_BIT_MASK<<4)
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP1_RBOFFSET			0xc9
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP1_SHIFT				8
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP1_MASK				(FOUR_BIT_MASK<<8)
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP1_RBOFFSET			0xc9
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP1_SHIFT				12
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP1_MASK				(FOUR_BIT_MASK<<12)
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP2_RBOFFSET			0xc9
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP2_SHIFT				16
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP2_MASK				(FOUR_BIT_MASK<<16)
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP2_RBOFFSET			0xc9
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP2_SHIFT				20
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP2_MASK				(FOUR_BIT_MASK<<20)
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP3_RBOFFSET			0xc9
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP3_SHIFT				24
#define 	M2REG_LEFTL1TEXIDTHRESHOLDLOOP3_MASK				(FOUR_BIT_MASK<<24)
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP3_RBOFFSET			0xc9
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP3_SHIFT				28
#define 	M2REG_RIGHTL1TEXIDTHRESHOLDLOOP3_MASK				(FOUR_BIT_MASK<<28)
#define 	M2REG_INDEX1REPLACEMENTLOOP0_RBOFFSET			0xca
#define 	M2REG_INDEX1REPLACEMENTLOOP0_SHIFT				0
#define 	M2REG_INDEX1REPLACEMENTLOOP0_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_INDEX1REPLACEMENTLOOP1_RBOFFSET			0xca
#define 	M2REG_INDEX1REPLACEMENTLOOP1_SHIFT				16
#define 	M2REG_INDEX1REPLACEMENTLOOP1_MASK				(SIXTEEN_BIT_MASK<<16)
#define 	M2REG_INDEX1REPLACEMENTLOOP2_RBOFFSET			0xcb
#define 	M2REG_INDEX1REPLACEMENTLOOP2_SHIFT				0
#define 	M2REG_INDEX1REPLACEMENTLOOP2_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_INDEX1REPLACEMENTLOOP3_RBOFFSET			0xcb
#define 	M2REG_INDEX1REPLACEMENTLOOP3_SHIFT				16
#define 	M2REG_INDEX1REPLACEMENTLOOP3_MASK				(SIXTEEN_BIT_MASK<<16)


#define M2REG_PALETTE_LOAD_ADDRESS  0x330
#define M2REG_PALETTE_LOAD_COMMAND  0xcc
#define 	M2REG_ACTIVATE_PALETTE_TABLE_RBOFFSET			0xcc
#define 	M2REG_ACTIVATE_PALETTE_TABLE_SHIFT				8
#define 	M2REG_ACTIVATE_PALETTE_TABLE_MASK				(ONE_BIT_MASK<<8)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_PORTNO_RBOFFSET			0xcc
#define 	M2REG_PORTNO_SHIFT				6
#define 	M2REG_PORTNO_MASK				(TWO_BIT_MASK<<6)
typedef enum
{
	MPS_AGP = 0x0,	/* No comment Given */
	MPS_Z_Port = 0x1,	/* No comment Given */
	MPS_C_Port = 0x2	/* No comment Given */
} MemoryPortSelection;


#define 	M2REG_PALETTE_TABLE_SIZE_RBOFFSET			0xcc
#define 	M2REG_PALETTE_TABLE_SIZE_SHIFT				0
#define 	M2REG_PALETTE_TABLE_SIZE_MASK				SIX_BIT_MASK
#define 	M2REG_MEMORY_BASE_ADDRESS_RBOFFSET			0xcd
#define 	M2REG_MEMORY_BASE_ADDRESS_SHIFT				0
#define 	M2REG_MEMORY_BASE_ADDRESS_MASK				TWENTYEIGHT_BIT_MASK


#define M2REG_TEXEL_KEY_01_ADDRESS  0x340
#define M2REG_TEXEL_KEY_01_COMMAND  0xd0
#define 	M2REG_TEXEL_KEY_HIGH0_RED_RBOFFSET			0xd0
#define 	M2REG_TEXEL_KEY_HIGH0_RED_SHIFT				16
#define 	M2REG_TEXEL_KEY_HIGH0_RED_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_TEXEL_KEY_HIGH0_GREEN_RBOFFSET			0xd0
#define 	M2REG_TEXEL_KEY_HIGH0_GREEN_SHIFT				8
#define 	M2REG_TEXEL_KEY_HIGH0_GREEN_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_TEXEL_KEY_HIGH0_BLUE_RBOFFSET			0xd0
#define 	M2REG_TEXEL_KEY_HIGH0_BLUE_SHIFT				0
#define 	M2REG_TEXEL_KEY_HIGH0_BLUE_MASK				EIGHT_BIT_MASK
#define 	M2REG_TEXEL_KEY_LOW0_RED_RBOFFSET			0xd1
#define 	M2REG_TEXEL_KEY_LOW0_RED_SHIFT				16
#define 	M2REG_TEXEL_KEY_LOW0_RED_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_TEXEL_KEY_LOW0_GREEN_RBOFFSET			0xd1
#define 	M2REG_TEXEL_KEY_LOW0_GREEN_SHIFT				8
#define 	M2REG_TEXEL_KEY_LOW0_GREEN_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_TEXEL_KEY_LOW0_BLUE_RBOFFSET			0xd1
#define 	M2REG_TEXEL_KEY_LOW0_BLUE_SHIFT				0
#define 	M2REG_TEXEL_KEY_LOW0_BLUE_MASK				EIGHT_BIT_MASK
#define 	M2REG_TEXEL_KEY_HIGH1_RED_RBOFFSET			0xd2
#define 	M2REG_TEXEL_KEY_HIGH1_RED_SHIFT				16
#define 	M2REG_TEXEL_KEY_HIGH1_RED_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_TEXEL_KEY_HIGH1_GREEN_RBOFFSET			0xd2
#define 	M2REG_TEXEL_KEY_HIGH1_GREEN_SHIFT				8
#define 	M2REG_TEXEL_KEY_HIGH1_GREEN_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_TEXEL_KEY_HIGH1_BLUE_RBOFFSET			0xd2
#define 	M2REG_TEXEL_KEY_HIGH1_BLUE_SHIFT				0
#define 	M2REG_TEXEL_KEY_HIGH1_BLUE_MASK				EIGHT_BIT_MASK
#define 	M2REG_TEXEL_KEY_LOW1_RED_RBOFFSET			0xd3
#define 	M2REG_TEXEL_KEY_LOW1_RED_SHIFT				16
#define 	M2REG_TEXEL_KEY_LOW1_RED_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_TEXEL_KEY_LOW1_GREEN_RBOFFSET			0xd3
#define 	M2REG_TEXEL_KEY_LOW1_GREEN_SHIFT				8
#define 	M2REG_TEXEL_KEY_LOW1_GREEN_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_TEXEL_KEY_LOW1_BLUE_RBOFFSET			0xd3
#define 	M2REG_TEXEL_KEY_LOW1_BLUE_SHIFT				0
#define 	M2REG_TEXEL_KEY_LOW1_BLUE_MASK				EIGHT_BIT_MASK


#define M2REG_TEXEL_KEY_2_ADDRESS  0x350
#define M2REG_TEXEL_KEY_2_COMMAND  0xd4
#define 	M2REG_TEXEL_KEY_HIGH2_RED_RBOFFSET			0xd4
#define 	M2REG_TEXEL_KEY_HIGH2_RED_SHIFT				16
#define 	M2REG_TEXEL_KEY_HIGH2_RED_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_TEXEL_KEY_HIGH2_GREEN_RBOFFSET			0xd4
#define 	M2REG_TEXEL_KEY_HIGH2_GREEN_SHIFT				8
#define 	M2REG_TEXEL_KEY_HIGH2_GREEN_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_TEXEL_KEY_HIGH2_BLUE_RBOFFSET			0xd4
#define 	M2REG_TEXEL_KEY_HIGH2_BLUE_SHIFT				0
#define 	M2REG_TEXEL_KEY_HIGH2_BLUE_MASK				EIGHT_BIT_MASK
#define 	M2REG_TEXEL_KEY_LOW2_RED_RBOFFSET			0xd5
#define 	M2REG_TEXEL_KEY_LOW2_RED_SHIFT				16
#define 	M2REG_TEXEL_KEY_LOW2_RED_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_TEXEL_KEY_LOW2_GREEN_RBOFFSET			0xd5
#define 	M2REG_TEXEL_KEY_LOW2_GREEN_SHIFT				8
#define 	M2REG_TEXEL_KEY_LOW2_GREEN_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_TEXEL_KEY_LOW2_BLUE_RBOFFSET			0xd5
#define 	M2REG_TEXEL_KEY_LOW2_BLUE_SHIFT				0
#define 	M2REG_TEXEL_KEY_LOW2_BLUE_MASK				EIGHT_BIT_MASK


#define M2REG_PIXELSHADER_SETTING_ADDRESS  0x380
#define M2REG_PIXELSHADER_SETTING_COMMAND  0xe0
#define 	M2REG_TEXEL_KEY_MASK_OPERATION_CODE_RBOFFSET			0xe0
#define 	M2REG_TEXEL_KEY_MASK_OPERATION_CODE_SHIFT				8
#define 	M2REG_TEXEL_KEY_MASK_OPERATION_CODE_MASK				(SIXTEEN_BIT_MASK<<8)
#define 	M2REG_PIXEL_SHADER_VERSION_RBOFFSET			0xe0
#define 	M2REG_PIXEL_SHADER_VERSION_SHIFT				7
#define 	M2REG_PIXEL_SHADER_VERSION_MASK				(ONE_BIT_MASK<<7)
typedef enum
{
	PSV_PixelShader1X = 0x0,	/* No comment Given */
	PSV_PixelShader20 = 0x1	/* No comment Given */
} PixelShaderVersion;


#define 	M2REG_SHADOW_WORKING_MODE_RBOFFSET			0xe0
#define 	M2REG_SHADOW_WORKING_MODE_SHIFT				4
#define 	M2REG_SHADOW_WORKING_MODE_MASK				(TWO_BIT_MASK<<4)
typedef enum
{
	PSM_8_Texture_Input_Mode = 0x0,	/* 8 texture inputs mode, can set invisible pixel pair drop for single loop. */
	PSM_16_Texture_Input_Mode = 0x1,	/* 16 texture input mode, can not set invisible pixel pair drop any way. */
	PSM_32_Buffer_Working_Only = 0x2	/* 32 buffer working only, can not set invisible pixel pair drop. Will require texture load table. */
} PixelShader2Mode;


#define 	M2REG_SHADOW_WORKING_ENABLE_RBOFFSET			0xe0
#define 	M2REG_SHADOW_WORKING_ENABLE_SHIFT				6
#define 	M2REG_SHADOW_WORKING_ENABLE_MASK				(ONE_BIT_MASK<<6)
/* 1 = Disable */
/* 0 = Enable */
#define 	M2REG_START_PERSISTENT_TEMP_ID_RBOFFSET			0xe0
#define 	M2REG_START_PERSISTENT_TEMP_ID_SHIFT				0
#define 	M2REG_START_PERSISTENT_TEMP_ID_MASK				FIVE_BIT_MASK
#define 	M2REG_PARTIAL_PRECISION_PS2_ENABLE_RBOFFSET			0xe0
#define 	M2REG_PARTIAL_PRECISION_PS2_ENABLE_SHIFT				3
#define 	M2REG_PARTIAL_PRECISION_PS2_ENABLE_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_FLOAT_COLOR_BUFFER_FORMAT_RBOFFSET			0xe0
#define 	M2REG_FLOAT_COLOR_BUFFER_FORMAT_SHIFT				0
#define 	M2REG_FLOAT_COLOR_BUFFER_FORMAT_MASK				THREE_BIT_MASK
typedef enum
{
	FB_ASIS = 0x0,	/* no float buffer */
	FB_L16 = 0x1,	/* 16bits integer buffer */
	FB_G16R16 = 0x2,	/* 32bits integer buffer */
	FB_R16F = 0x3,	/* No comment Given */
	FB_G16R16F = 0x4,	/* No comment Given */
	FB_R32F = 0x5,	/* No comment Given */
	FB_V16U16 = 0x6	/* No comment Given */
} FloatColorBufferFormat;


#define 	M2REG_LOOP0_PSINSTRUCTION_ENTRY_RBOFFSET			0xe1
#define 	M2REG_LOOP0_PSINSTRUCTION_ENTRY_SHIFT				0
#define 	M2REG_LOOP0_PSINSTRUCTION_ENTRY_MASK				SEVEN_BIT_MASK
#define 	M2REG_LOOP1_PSINSTRUCTION_ENTRY_RBOFFSET			0xe1
#define 	M2REG_LOOP1_PSINSTRUCTION_ENTRY_SHIFT				8
#define 	M2REG_LOOP1_PSINSTRUCTION_ENTRY_MASK				(SEVEN_BIT_MASK<<8)
#define 	M2REG_LOOP2_PSINSTRUCTION_ENTRY_RBOFFSET			0xe1
#define 	M2REG_LOOP2_PSINSTRUCTION_ENTRY_SHIFT				16
#define 	M2REG_LOOP2_PSINSTRUCTION_ENTRY_MASK				(SEVEN_BIT_MASK<<16)
#define 	M2REG_LOOP3_PSINSTRUCTION_ENTRY_RBOFFSET			0xe1
#define 	M2REG_LOOP3_PSINSTRUCTION_ENTRY_SHIFT				24
#define 	M2REG_LOOP3_PSINSTRUCTION_ENTRY_MASK				(SEVEN_BIT_MASK<<24)
#define 	M2REG_LOOP4_PSINSTRUCTION_ENTRY_RBOFFSET			0xe2
#define 	M2REG_LOOP4_PSINSTRUCTION_ENTRY_SHIFT				0
#define 	M2REG_LOOP4_PSINSTRUCTION_ENTRY_MASK				SEVEN_BIT_MASK
#define 	M2REG_LOOP5_PSINSTRUCTION_ENTRY_RBOFFSET			0xe2
#define 	M2REG_LOOP5_PSINSTRUCTION_ENTRY_SHIFT				8
#define 	M2REG_LOOP5_PSINSTRUCTION_ENTRY_MASK				(SEVEN_BIT_MASK<<8)
#define 	M2REG_LOOP6_PSINSTRUCTION_ENTRY_RBOFFSET			0xe2
#define 	M2REG_LOOP6_PSINSTRUCTION_ENTRY_SHIFT				16
#define 	M2REG_LOOP6_PSINSTRUCTION_ENTRY_MASK				(SEVEN_BIT_MASK<<16)
#define 	M2REG_LOOP7_PSINSTRUCTION_ENTRY_RBOFFSET			0xe2
#define 	M2REG_LOOP7_PSINSTRUCTION_ENTRY_SHIFT				24
#define 	M2REG_LOOP7_PSINSTRUCTION_ENTRY_MASK				(SEVEN_BIT_MASK<<24)
#define 	M2REG_DIFFUSE_RGB_RESOURCE_RBOFFSET			0xe3
#define 	M2REG_DIFFUSE_RGB_RESOURCE_SHIFT				0
#define 	M2REG_DIFFUSE_RGB_RESOURCE_MASK				THREE_BIT_MASK
typedef enum
{
	DRGBS_Diffuse_RGB = 0x0,	/* No comment Given */
	DRGBS_Specular_RGB = 0x1,	/* No comment Given */
	DRGBS_Texture0_RGB = 0x4,	/* No comment Given */
	DRGBS_Texture1_RGB = 0x5,	/* No comment Given */
	DRGBS_Texture2_RGB = 0x6,	/* No comment Given */
	DRGBS_Texture3_RGB = 0x7	/* No comment Given */
} DiffuseRGBSource;


#define 	M2REG_DIFFUSE_ALPHA_RESOURCE_RBOFFSET			0xe3
#define 	M2REG_DIFFUSE_ALPHA_RESOURCE_SHIFT				4
#define 	M2REG_DIFFUSE_ALPHA_RESOURCE_MASK				(THREE_BIT_MASK<<4)
typedef enum
{
	DAS_Diffuse_Alpha = 0x0,	/* No comment Given */
	DAS_Specular_Alpha = 0x1,	/* No comment Given */
	DAS_Texture0_Alpha = 0x4,	/* No comment Given */
	DAS_Texture1_Alpha = 0x5,	/* No comment Given */
	DAS_Texture2_Alpha = 0x6,	/* No comment Given */
	DAS_Texture3_Alpha = 0x7	/* No comment Given */
} DiffuseAlphaSource;


#define 	M2REG_SPECULAR_RGB_RESOURCE_RBOFFSET			0xe3
#define 	M2REG_SPECULAR_RGB_RESOURCE_SHIFT				8
#define 	M2REG_SPECULAR_RGB_RESOURCE_MASK				(FOUR_BIT_MASK<<8)
typedef enum
{
	SRGBS_Diffuse_RGB = 0x0,	/* No comment Given */
	SRGBS_Specular_RGB = 0x1,	/* No comment Given */
	SRGBS_PixelShader_Output = 0x2,	/* No comment Given */
	SRGBS_Texture0_RGB = 0x4,	/* No comment Given */
	SRGBS_Texture1_RGB = 0x5,	/* No comment Given */
	SRGBS_Texture2_RGB = 0x6,	/* No comment Given */
	SRGBS_Texture3_RGB = 0x7	/* No comment Given */
} SpecularRGBSource;


#define 	M2REG_FOG_RESOURCE_RBOFFSET			0xe3
#define 	M2REG_FOG_RESOURCE_SHIFT				12
#define 	M2REG_FOG_RESOURCE_MASK				(ONE_BIT_MASK<<12)
typedef enum
{
	FS_Fog_Bypass = 0x0,	/* No comment Given */
	FS_PixelShader_Output_Specular_Alpha = 0x1	/* No comment Given */
} FogSource;


#define 	M2REG_FLOAT_COLOR_BUFFER_TARGET3_FORMAT_RBOFFSET			0xe3
#define 	M2REG_FLOAT_COLOR_BUFFER_TARGET3_FORMAT_SHIFT				29
#define 	M2REG_FLOAT_COLOR_BUFFER_TARGET3_FORMAT_MASK				(THREE_BIT_MASK<<29)
/* See earlier definition of FloatcolorBufferformat */
#define 	M2REG_FLOAT_COLOR_BUFFER_TARGET2_FORMAT_RBOFFSET			0xe3
#define 	M2REG_FLOAT_COLOR_BUFFER_TARGET2_FORMAT_SHIFT				26
#define 	M2REG_FLOAT_COLOR_BUFFER_TARGET2_FORMAT_MASK				(THREE_BIT_MASK<<26)
/* See earlier definition of FloatcolorBufferformat */
#define 	M2REG_FLOAT_COLOR_BUFFER_TARGET1_FORMAT_RBOFFSET			0xe3
#define 	M2REG_FLOAT_COLOR_BUFFER_TARGET1_FORMAT_SHIFT				23
#define 	M2REG_FLOAT_COLOR_BUFFER_TARGET1_FORMAT_MASK				(THREE_BIT_MASK<<23)
/* See earlier definition of FloatcolorBufferformat */


/******************************
* FETCH INSTRUCTION
******************************/
#define 	FETCH_MEMORY_PORT_FLAG_OFFSET			0x0
#define 	FETCH_MEMORY_PORT_FLAG_SHIFT				12
#define 	FETCH_MEMORY_PORT_FLAG_MASK				(ONE_BIT_MASK<<12)
/* See earlier definition of MemoryPortSelection */
#define 	FETCH_VERTEX_SIZE_OFFSET			0x0
#define 	FETCH_VERTEX_SIZE_SHIFT				7
#define 	FETCH_VERTEX_SIZE_MASK				(FIVE_BIT_MASK<<7)
#define 	FETCH_VERTEX_STRIDE_OFFSET			0x0
#define 	FETCH_VERTEX_STRIDE_SHIFT				1
#define 	FETCH_VERTEX_STRIDE_MASK				(SIX_BIT_MASK<<1)
#define 	FETCH_LAST_COMMAND_OFFSET			0x0
#define 	FETCH_LAST_COMMAND_SHIFT				0
#define 	FETCH_LAST_COMMAND_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	FETCH_BASE_ADDRESS_OFFSET			0x1
#define 	FETCH_BASE_ADDRESS_SHIFT				0
#define 	FETCH_BASE_ADDRESS_MASK				THIRTY_BIT_MASK


/******************************
* PUT INSTRUCTION
******************************/
#define 	PUT_SWAPMODEHIGH_OFFSET			0x0
#define 	PUT_SWAPMODEHIGH_SHIFT				30
#define 	PUT_SWAPMODEHIGH_MASK				(TWO_BIT_MASK<<30)
/* See earlier definition of SurfaceSwapMode */
#define 	PUT_HIGH_OFFSET_OFFSET			0x0
#define 	PUT_HIGH_OFFSET_SHIFT				24
#define 	PUT_HIGH_OFFSET_MASK				(SIX_BIT_MASK<<24)
#define 	PUT_SWAPMODELOW_OFFSET			0x0
#define 	PUT_SWAPMODELOW_SHIFT				22
#define 	PUT_SWAPMODELOW_MASK				(TWO_BIT_MASK<<22)
/* See earlier definition of SurfaceSwapMode */
#define 	PUT_LOW_OFFSET_OFFSET			0x0
#define 	PUT_LOW_OFFSET_SHIFT				16
#define 	PUT_LOW_OFFSET_MASK				(SIX_BIT_MASK<<16)
#define 	PUT_HIGH_DEFAULT_CODE_OFFSET			0x0
#define 	PUT_HIGH_DEFAULT_CODE_SHIFT				14
#define 	PUT_HIGH_DEFAULT_CODE_MASK				(TWO_BIT_MASK<<14)
typedef enum
{
	PDC_Convert = 0x0,	/* default */
	PDC_Passthrough = 0x1,	/* pass source directly to Special Data Register; */
	PDC_ConvertSetConstants = 0x2,	/* conversion and set (0.0, 0.0, 0.0, 1.0) to components after destOffsetL/H.  Only for first eight types of conversions. */
	PDC_SetConstantOnly = 0x3	/* (0.0, 0.0, 0.0, 1.0) to components from destOffsetL/H. */
} PutDefaultCode;


#define 	PUT_LOW_DEFAULT_CODE_OFFSET			0x0
#define 	PUT_LOW_DEFAULT_CODE_SHIFT				12
#define 	PUT_LOW_DEFAULT_CODE_MASK				(TWO_BIT_MASK<<12)
/* See earlier definition of PutDefaultCode */
#define 	PUT_HIGH_CONVERSIONID_OFFSET			0x0
#define 	PUT_HIGH_CONVERSIONID_SHIFT				8
#define 	PUT_HIGH_CONVERSIONID_MASK				(FOUR_BIT_MASK<<8)
typedef enum
{
	PCC_IEEE32_IFF = 0x0,	/* No comment Given */
	PCC_IEEE16_IFF = 0x1,	/* No comment Given */
	PCC_Int32_IFF = 0x2,	/* No comment Given */
	PCC_DWORD_IFF = 0x3,	/* No comment Given */
	PCC_Short_Normalize_IFF = 0x4,	/* [-1.0, 1.0] = fvalue / (215 - 1). */
	PCC_Short_IFF = 0x5,	/* fvalue */
	PCC_WORD_Normalize_IFF = 0x6,	/* [0.0, 1.0] = fvalue / (216 - 1). */
	PCC_WORD_IFF = 0x7,	/* fvalue. */
	PCC_UBYTE_Normalize_IFF = 0x8,	/* in [0.0, 1.0]; D[7:0] = X, D[15:8] = Y, D[23:16] = Z, D[31:24] = W. */
	PCC_UBYTE_Normalize_IFF_Swap_XZ = 0x9,	/* in [0.0, 1.0]; X <==> Z. */
	PCC_UBYTE_IFF = 0xa,	/* in [0.0, 255.0]; */
	PCC_UBYTE_IFF_Swap_XZ = 0xb,	/* in [0.0, 255.0]; X <==>Z. */
	PCC_Signed10_Normalize_IFF = 0xc,	/* fixed point 10-10-10 format to IFF in [-1.0, 1.0] = fvalue/(29 - 1) in order of (X, Y, Z, 1.0) and set 1.0 to W. D[9:0] = X, D[29:20] = Z. */
	PCC_Signed10_Normalize_IFF_Swap_XZ = 0xd,	/* 10-10-10 format to IFF in [-1.0, 1.0] = fvalue/(29 - 1) in order of (Z, Y, X, 1.0) and set 1.0 to W. */
	PCC_Unsigned10_IFF = 0xe,	/* 10-10-10 format to IFF = fvalue in order of (X, Y, Z, 1.0) and set 1.0 to W. */
	PCC_Unsigned10_IFF_Swap_XZ = 0xf	/* 10-10-10 format to IFF = fvalue in order of (Z, Y, X, 1.0) and set 1.0 to W. X çè Z. */
} PutConversionCodes;


#define 	PUT_LOW_CONVERSIONID_OFFSET			0x0
#define 	PUT_LOW_CONVERSIONID_SHIFT				4
#define 	PUT_LOW_CONVERSIONID_MASK				(FOUR_BIT_MASK<<4)
/* See earlier definition of PutConversionCodes */
#define 	PUT_HOLD_OFFSET			0x0
#define 	PUT_HOLD_SHIFT				3
#define 	PUT_HOLD_MASK				(ONE_BIT_MASK<<3)
/* 0 = Disable */
/* 1 = Enable */
#define 	PUT_HIGH_VALID_OFFSET			0x0
#define 	PUT_HIGH_VALID_SHIFT				2
#define 	PUT_HIGH_VALID_MASK				(ONE_BIT_MASK<<2)
/* 0 = Disable */
/* 1 = Enable */
#define 	PUT_LOW_VALID_OFFSET			0x0
#define 	PUT_LOW_VALID_SHIFT				1
#define 	PUT_LOW_VALID_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	PUT_LAST_COMMAND_OFFSET			0x0
#define 	PUT_LAST_COMMAND_SHIFT				0
#define 	PUT_LAST_COMMAND_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */


/******************************
* BUMPSHADER INSTRUCTION
******************************/
#define 	BUMPSHADER_LEFTTCI_OFFSET			0x0
#define 	BUMPSHADER_LEFTTCI_SHIFT				0
#define 	BUMPSHADER_LEFTTCI_MASK				FOUR_BIT_MASK
#define 	BUMPSHADER_RIGHTTCI_OFFSET			0x0
#define 	BUMPSHADER_RIGHTTCI_SHIFT				4
#define 	BUMPSHADER_RIGHTTCI_MASK				(FOUR_BIT_MASK<<4)
#define 	BUMPSHADER_LEFTSAMPLERID_OFFSET			0x0
#define 	BUMPSHADER_LEFTSAMPLERID_SHIFT				8
#define 	BUMPSHADER_LEFTSAMPLERID_MASK				(FOUR_BIT_MASK<<8)
#define 	BUMPSHADER_RIGHTSAMPLERID_OFFSET			0x0
#define 	BUMPSHADER_RIGHTSAMPLERID_SHIFT				12
#define 	BUMPSHADER_RIGHTSAMPLERID_MASK				(FOUR_BIT_MASK<<12)
#define 	BUMPSHADER_SAMPLINGSTATE_OFFSET			0x0
#define 	BUMPSHADER_SAMPLINGSTATE_SHIFT				16
#define 	BUMPSHADER_SAMPLINGSTATE_MASK				(THREE_BIT_MASK<<16)
typedef enum
{
	SS_Invalid_Pair = 0x0,	/* No comment Given */
	SS_Valid_Invalid = 0x1,	/* left valid, right invalid */
	SS_Invalid_Valid = 0x2,	/* left invalid, right valid */
	SS_Valid_Valid = 0x3,	/* both valid */
	SS_Coord_NoKill_NoClamp = 0x4,	/* No comment Given */
	SS_Coord_NoKill_Clamp = 0x5,	/* No comment Given */
	SS_Coord_Kill_NoClamp = 0x6,	/* No comment Given */
	SS_Coord_Kill_Clamp = 0x7	/* No comment Given */
} SampleState;


#define 	BUMPSHADER_REPEATUFORVENABLE_OFFSET			0x0
#define 	BUMPSHADER_REPEATUFORVENABLE_SHIFT				19
#define 	BUMPSHADER_REPEATUFORVENABLE_MASK				(ONE_BIT_MASK<<19)
/* 0 = Disable */
/* 1 = Enable */
#define 	BUMPSHADER_LEFTBUMPATTRIBUTE_OFFSET			0x0
#define 	BUMPSHADER_LEFTBUMPATTRIBUTE_SHIFT				20
#define 	BUMPSHADER_LEFTBUMPATTRIBUTE_MASK				(TWO_BIT_MASK<<20)
typedef enum
{
	BA_No_Bump_Required = 0x0,	/* No comment Given */
	BA_2D_Bump_Required = 0x1,	/* No comment Given */
	BA_4D_Bump_Required = 0x2	/* force it no perspective correction. */
} BumpAttribute;


#define 	BUMPSHADER_RIGHTBUMPATTRIBUTE_OFFSET			0x0
#define 	BUMPSHADER_RIGHTBUMPATTRIBUTE_SHIFT				22
#define 	BUMPSHADER_RIGHTBUMPATTRIBUTE_MASK				(TWO_BIT_MASK<<22)
/* See earlier definition of BumpAttribute */
#define 	BUMPSHADER_TEXTURETYPE_OFFSET			0x0
#define 	BUMPSHADER_TEXTURETYPE_SHIFT				24
#define 	BUMPSHADER_TEXTURETYPE_MASK				(THREE_BIT_MASK<<24)
typedef enum
{
	TT_2D_Texture = 0x0,	/* No comment Given */
	TT_Depth_Texture = 0x1,	/* No comment Given */
	TT_Cube_Map = 0x2,	/* No comment Given */
	TT_3D_Texture = 0x3,	/* No comment Given */
	TT_Projected_2D_Texture = 0x4,	/* No comment Given */
	TT_Projected_Depth_Texture = 0x5,	/* No comment Given */
	TT_Projected_TexCoord = 0x6,	/* SamplingAttribute should = 4~7 */
	TT_Projected_3D_Texture = 0x7	/* No comment Given */
} TextureType;


#define 	BUMPSHADER_TRANSPOSE_EVEN_ODD_OFFSET			0x0
#define 	BUMPSHADER_TRANSPOSE_EVEN_ODD_SHIFT				27
#define 	BUMPSHADER_TRANSPOSE_EVEN_ODD_MASK				(ONE_BIT_MASK<<27)
/* 0 = Disable */
/* 1 = Enable */
#define 	BUMPSHADER_TRANSPOSE_ENABLE_OFFSET			0x0
#define 	BUMPSHADER_TRANSPOSE_ENABLE_SHIFT				28
#define 	BUMPSHADER_TRANSPOSE_ENABLE_MASK				(ONE_BIT_MASK<<28)
/* 0 = Disable */
/* 1 = Enable */
#define 	BUMPSHADER_END_OF_TEXTURE_OFFSET			0x0
#define 	BUMPSHADER_END_OF_TEXTURE_SHIFT				29
#define 	BUMPSHADER_END_OF_TEXTURE_MASK				(ONE_BIT_MASK<<29)
/* 0 = Disable */
/* 1 = Enable */
#define 	BUMPSHADER_END_OF_LOOP_OFFSET			0x0
#define 	BUMPSHADER_END_OF_LOOP_SHIFT				30
#define 	BUMPSHADER_END_OF_LOOP_MASK				(ONE_BIT_MASK<<30)
/* 0 = Disable */
/* 1 = Enable */
#define 	BUMPSHADER_REPLACEMENT_ENABLE_OFFSET			0x0
#define 	BUMPSHADER_REPLACEMENT_ENABLE_SHIFT				31
#define 	BUMPSHADER_REPLACEMENT_ENABLE_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */


/******************************
* TEXTURESAMPLESTATE INSTRUCTION
******************************/
#define 	TEXTURESAMPLESTATE_TEXTURE_PREFETCH_ENABLE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_TEXTURE_PREFETCH_ENABLE_SHIFT				31
#define 	TEXTURESAMPLESTATE_TEXTURE_PREFETCH_ENABLE_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_HOLD_FORPACK_FORCEDTEXTURE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_HOLD_FORPACK_FORCEDTEXTURE_SHIFT				30
#define 	TEXTURESAMPLESTATE_HOLD_FORPACK_FORCEDTEXTURE_MASK				(ONE_BIT_MASK<<30)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_FORCE_TEXTURE_TORIGHTPIPE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_FORCE_TEXTURE_TORIGHTPIPE_SHIFT				29
#define 	TEXTURESAMPLESTATE_FORCE_TEXTURE_TORIGHTPIPE_MASK				(ONE_BIT_MASK<<29)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_DXTN_FIX4X4_ENABLE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_DXTN_FIX4X4_ENABLE_SHIFT				28
#define 	TEXTURESAMPLESTATE_DXTN_FIX4X4_ENABLE_MASK				(ONE_BIT_MASK<<28)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_YUV2RGB_CONVERSION_ENABLE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_YUV2RGB_CONVERSION_ENABLE_SHIFT				27
#define 	TEXTURESAMPLESTATE_YUV2RGB_CONVERSION_ENABLE_MASK				(ONE_BIT_MASK<<27)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_YUV_DCT_ENABLE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_YUV_DCT_ENABLE_SHIFT				26
#define 	TEXTURESAMPLESTATE_YUV_DCT_ENABLE_MASK				(ONE_BIT_MASK<<26)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_PAIRED_TEXTURE_MODULATE_ALPHA_ENABLE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_PAIRED_TEXTURE_MODULATE_ALPHA_ENABLE_SHIFT				25
#define 	TEXTURESAMPLESTATE_PAIRED_TEXTURE_MODULATE_ALPHA_ENABLE_MASK				(ONE_BIT_MASK<<25)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_PAIRED_TEXTURE_MODULATE_RGB_ENABLE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_PAIRED_TEXTURE_MODULATE_RGB_ENABLE_SHIFT				24
#define 	TEXTURESAMPLESTATE_PAIRED_TEXTURE_MODULATE_RGB_ENABLE_MASK				(ONE_BIT_MASK<<24)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_MIPMAP_LINEAR_FILTER_ENABLE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_MIPMAP_LINEAR_FILTER_ENABLE_SHIFT				23
#define 	TEXTURESAMPLESTATE_MIPMAP_LINEAR_FILTER_ENABLE_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_MAGNIFICATION_FILTER_MODE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_MAGNIFICATION_FILTER_MODE_SHIFT				21
#define 	TEXTURESAMPLESTATE_MAGNIFICATION_FILTER_MODE_MASK				(TWO_BIT_MASK<<21)
typedef enum
{
	TFM_Point_Sampling = 0x1,	/* No comment Given */
	TFM_Linear_Filtering = 0x2,	/* No comment Given */
	TFM_Anisotropic_Filtering = 0x3	/* No comment Given */
} TextureFilterMode;


#define 	TEXTURESAMPLESTATE_MINIFICATION_FILTER_MODE_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_MINIFICATION_FILTER_MODE_SHIFT				19
#define 	TEXTURESAMPLESTATE_MINIFICATION_FILTER_MODE_MASK				(TWO_BIT_MASK<<19)
/* See earlier definition of TextureFilterMode */
#define 	TEXTURESAMPLESTATE_MAX_ANISOTROPIC_RATIO_EXPONENT_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_MAX_ANISOTROPIC_RATIO_EXPONENT_SHIFT				16
#define 	TEXTURESAMPLESTATE_MAX_ANISOTROPIC_RATIO_EXPONENT_MASK				(THREE_BIT_MASK<<16)
typedef enum
{
	AR_OneToOne = 0x0,	/* No comment Given */
	AR_TwoToOne = 0x1,	/* No comment Given */
	AR_FourToOne = 0x2,	/* No comment Given */
	AR_EightToOne = 0x3,	/* No comment Given */
	AR_SixteenToOne = 0x4	/* No comment Given */
} AnisotropicRatio;


#define 	TEXTURESAMPLESTATE_MAXIMUM_LOD_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_MAXIMUM_LOD_SHIFT				12
#define 	TEXTURESAMPLESTATE_MAXIMUM_LOD_MASK				(FOUR_BIT_MASK<<12)
#define 	TEXTURESAMPLESTATE_LOD_BIAS_OFFSET			0x0
#define 	TEXTURESAMPLESTATE_LOD_BIAS_SHIFT				0
#define 	TEXTURESAMPLESTATE_LOD_BIAS_MASK				TEN_BIT_MASK
#define 	TEXTURESAMPLESTATE_FAST_ANISOTROPIC_ENABLE_OFFSET			0x1
#define 	TEXTURESAMPLESTATE_FAST_ANISOTROPIC_ENABLE_SHIFT				31
#define 	TEXTURESAMPLESTATE_FAST_ANISOTROPIC_ENABLE_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_TEXTURE_HEIGHT_OFFSET			0x1
#define 	TEXTURESAMPLESTATE_TEXTURE_HEIGHT_SHIFT				16
#define 	TEXTURESAMPLESTATE_TEXTURE_HEIGHT_MASK				(FIFTEEN_BIT_MASK<<16)
#define 	TEXTURESAMPLESTATE_TEXTURE_HEIGHT_EXPONENT_OFFSET			0x1
#define 	TEXTURESAMPLESTATE_TEXTURE_HEIGHT_EXPONENT_SHIFT				27
#define 	TEXTURESAMPLESTATE_TEXTURE_HEIGHT_EXPONENT_MASK				(FOUR_BIT_MASK<<27)
#define 	TEXTURESAMPLESTATE_TEXTURE_HEIGHT_MANTISSA_OFFSET			0x1
#define 	TEXTURESAMPLESTATE_TEXTURE_HEIGHT_MANTISSA_SHIFT				16
#define 	TEXTURESAMPLESTATE_TEXTURE_HEIGHT_MANTISSA_MASK				(ELEVEN_BIT_MASK<<16)
#define 	TEXTURESAMPLESTATE_TEXTURE_WIDTH_OFFSET			0x1
#define 	TEXTURESAMPLESTATE_TEXTURE_WIDTH_SHIFT				0
#define 	TEXTURESAMPLESTATE_TEXTURE_WIDTH_MASK				FIFTEEN_BIT_MASK
#define 	TEXTURESAMPLESTATE_TEXTURE_WIDTH_EXPONENT_OFFSET			0x1
#define 	TEXTURESAMPLESTATE_TEXTURE_WIDTH_EXPONENT_SHIFT				11
#define 	TEXTURESAMPLESTATE_TEXTURE_WIDTH_EXPONENT_MASK				(FOUR_BIT_MASK<<11)
#define 	TEXTURESAMPLESTATE_TEXTURE_WIDTH_MANTISSA_OFFSET			0x1
#define 	TEXTURESAMPLESTATE_TEXTURE_WIDTH_MANTISSA_SHIFT				0
#define 	TEXTURESAMPLESTATE_TEXTURE_WIDTH_MANTISSA_MASK				ELEVEN_BIT_MASK
#define 	TEXTURESAMPLESTATE_TEX_KILL_ENABLE_OFFSET			0x2
#define 	TEXTURESAMPLESTATE_TEX_KILL_ENABLE_SHIFT				31
#define 	TEXTURESAMPLESTATE_TEX_KILL_ENABLE_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_W_ADDRESSING_MODE_OFFSET			0x2
#define 	TEXTURESAMPLESTATE_W_ADDRESSING_MODE_SHIFT				28
#define 	TEXTURESAMPLESTATE_W_ADDRESSING_MODE_MASK				(THREE_BIT_MASK<<28)
typedef enum
{
	TAM_None = 0x0,	/* Always W = W&0xfff; */
	TAM_Wrap = 0x1,	/* default */
	TAM_Mirror = 0x2,	/* No comment Given */
	TAM_MirrorOnce = 0x3,	/* No comment Given */
	TAM_Clamp = 0x4,	/* No comment Given */
	TAM_Clamp_To_Edge = 0x5,	/* No comment Given */
	TAM_Clamp_To_Border = 0x6	/* No comment Given */
} TexelAddressMode;


#define 	TEXTURESAMPLESTATE_TEXTUREFILTERINGENTRY_OFFSET			0x2
#define 	TEXTURESAMPLESTATE_TEXTUREFILTERINGENTRY_SHIFT				22
#define 	TEXTURESAMPLESTATE_TEXTUREFILTERINGENTRY_MASK				(SIX_BIT_MASK<<22)
#define 	TEXTURESAMPLESTATE_TEXTUREFILTERINGLENGTH_OFFSET			0x2
#define 	TEXTURESAMPLESTATE_TEXTUREFILTERINGLENGTH_SHIFT				16
#define 	TEXTURESAMPLESTATE_TEXTUREFILTERINGLENGTH_MASK				(SIX_BIT_MASK<<16)
#define 	TEXTURESAMPLESTATE_TEXTURE_DEPTH_EXPONENT_OFFSET			0x2
#define 	TEXTURESAMPLESTATE_TEXTURE_DEPTH_EXPONENT_SHIFT				11
#define 	TEXTURESAMPLESTATE_TEXTURE_DEPTH_EXPONENT_MASK				(FOUR_BIT_MASK<<11)
#define 	TEXTURESAMPLESTATE_TEXTURE_DEPTH_MANTISSA_OFFSET			0x2
#define 	TEXTURESAMPLESTATE_TEXTURE_DEPTH_MANTISSA_SHIFT				0
#define 	TEXTURESAMPLESTATE_TEXTURE_DEPTH_MANTISSA_MASK				ELEVEN_BIT_MASK
#define 	TEXTURESAMPLESTATE_TEXTURE_BASE_INDEX_OFFSET			0x3
#define 	TEXTURESAMPLESTATE_TEXTURE_BASE_INDEX_SHIFT				24
#define 	TEXTURESAMPLESTATE_TEXTURE_BASE_INDEX_MASK				(EIGHT_BIT_MASK<<24)
#define 	TEXTURESAMPLESTATE_VERTICAL_SCALE_FACTOR_OFFSET			0x3
#define 	TEXTURESAMPLESTATE_VERTICAL_SCALE_FACTOR_SHIFT				21
#define 	TEXTURESAMPLESTATE_VERTICAL_SCALE_FACTOR_MASK				(THREE_BIT_MASK<<21)
#define 	TEXTURESAMPLESTATE_VERTICAL_SCALED_KERNEL_SIZE_OFFSET			0x3
#define 	TEXTURESAMPLESTATE_VERTICAL_SCALED_KERNEL_SIZE_SHIFT				12
#define 	TEXTURESAMPLESTATE_VERTICAL_SCALED_KERNEL_SIZE_MASK				(NINE_BIT_MASK<<12)
#define 	TEXTURESAMPLESTATE_HORIZONTAL_SCALE_FACTOR_OFFSET			0x3
#define 	TEXTURESAMPLESTATE_HORIZONTAL_SCALE_FACTOR_SHIFT				9
#define 	TEXTURESAMPLESTATE_HORIZONTAL_SCALE_FACTOR_MASK				(THREE_BIT_MASK<<9)
#define 	TEXTURESAMPLESTATE_HORIZONTAL_SCALED_KERNEL_SIZE_OFFSET			0x3
#define 	TEXTURESAMPLESTATE_HORIZONTAL_SCALED_KERNEL_SIZE_SHIFT				0
#define 	TEXTURESAMPLESTATE_HORIZONTAL_SCALED_KERNEL_SIZE_MASK				NINE_BIT_MASK
#define 	TEXTURESAMPLESTATE_OPENGL_ADDRESSING_ENABLE_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_OPENGL_ADDRESSING_ENABLE_SHIFT				31
#define 	TEXTURESAMPLESTATE_OPENGL_ADDRESSING_ENABLE_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_BORDER_MODE_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_BORDER_MODE_SHIFT				30
#define 	TEXTURESAMPLESTATE_BORDER_MODE_MASK				(ONE_BIT_MASK<<30)
typedef enum
{
	BM_Border_Color = 0x0,	/* use constant border color. */
	BM_Border_Texel = 0x1	/* use border texel in texture map. The border width = 1. */
} BorderMode;


#define 	TEXTURESAMPLESTATE_V_ADDRESSING_MODE_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_V_ADDRESSING_MODE_SHIFT				27
#define 	TEXTURESAMPLESTATE_V_ADDRESSING_MODE_MASK				(THREE_BIT_MASK<<27)
/* See earlier definition of TexelAddressMode */
#define 	TEXTURESAMPLESTATE_U_ADDRESSING_MODE_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_U_ADDRESSING_MODE_SHIFT				24
#define 	TEXTURESAMPLESTATE_U_ADDRESSING_MODE_MASK				(THREE_BIT_MASK<<24)
/* See earlier definition of TexelAddressMode */
#define 	TEXTURESAMPLESTATE_TEXTURE_SWAP_MODE_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_TEXTURE_SWAP_MODE_SHIFT				22
#define 	TEXTURESAMPLESTATE_TEXTURE_SWAP_MODE_MASK				(TWO_BIT_MASK<<22)
/* See earlier definition of SurfaceSwapMode */
#define 	TEXTURESAMPLESTATE_TOTALELEMENTS_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_TOTALELEMENTS_SHIFT				20
#define 	TEXTURESAMPLESTATE_TOTALELEMENTS_MASK				(TWO_BIT_MASK<<20)
#define 	TEXTURESAMPLESTATE_ENABLE_2X2XTILE_MODE_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_ENABLE_2X2XTILE_MODE_SHIFT				19
#define 	TEXTURESAMPLESTATE_ENABLE_2X2XTILE_MODE_MASK				(ONE_BIT_MASK<<19)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_ENABLE_COMPRESSEDCOLORBUFFER_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_ENABLE_COMPRESSEDCOLORBUFFER_SHIFT				18
#define 	TEXTURESAMPLESTATE_ENABLE_COMPRESSEDCOLORBUFFER_MASK				(ONE_BIT_MASK<<18)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_TEXTURE_BAND_TILE_MODE_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_TEXTURE_BAND_TILE_MODE_SHIFT				16
#define 	TEXTURESAMPLESTATE_TEXTURE_BAND_TILE_MODE_MASK				(TWO_BIT_MASK<<16)
/* See earlier definition of BandTileMode */
#define 	TEXTURESAMPLESTATE_TEXTURE_FORMAT_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_TEXTURE_FORMAT_SHIFT				11
#define 	TEXTURESAMPLESTATE_TEXTURE_FORMAT_MASK				(FIVE_BIT_MASK<<11)
typedef enum
{
	MTF_NULL = 0x0,	/* default. No map available, Return Border color. */
	MTF_Reserved1 = 0x1,	/* No comment Given */
	MTF_Reserved2 = 0x2,	/* No comment Given */
	MTF_XRGB8888 = 0x3,	/* 32bit, may be tiled. */
	MTF_RGB565 = 0x4,	/* may be tiled. */
	MTF_L16 = 0x5,	/* */
	MTF_G16R16 = 0x6,	/* may be tiled. */
	MTF_Reserved7 = 0x7,	/* may be tiled. */
	MTF_XRGB1555 = 0x8,	/* may be tiled. */
	MTF_G16R16F = 0x9,	/* may be tiled. */
	MTF_ARGB4444 = 0xa,	/* may be tiled. */
	MTF_L8 = 0xb,	/* luminance map */
	MTF_A8L8 = 0xc,	/* may be tiled. */
	MTF_U8V8 = 0xd,	/* for bump mapping; may be tiled. */
	MTF_U5V5L6 = 0xe,	/* may be tiled. */
	MTF_V16U16 = 0xf,	/* may be tiled. */
	MTF_UYVY = 0x10,	/* for video texture. for CYUV, do 1-v for every vertex by S/W Driver. */
	MTF_YUY2 = 0x11,	/* No comment Given */
	MTF_DXT1 = 0x12,	/* for texture compression */
	MTF_DXT2 = 0x13,	/* No comment Given */
	MTF_DXT3 = 0x14,	/* No comment Given */
	MTF_DXT4 = 0x15,	/* No comment Given */
	MTF_DXT5 = 0x16,	/* No comment Given */
	MTF_R16F = 0x17,	/* may be tiled. */
	MTF_DXT7 = 0x18,	/* No comment Given */
	MTF_RGB332 = 0x19,	/* No comment Given */
	MTF_R32F = 0x1a,	/* may be tiled. */
	MTF_A2R10G10B10 = 0x1b,	/* No comment Given */
	MTF_A2W10V10U10 = 0x1c,	/* No comment Given */
	MTF_W11V11U10 = 0x1d,	/* No comment Given */
	MTF_XLVU8888 = 0x1e,	/* 32bit, may be tiled. */
	MTF_QWVU8888 = 0x1f	/* 32bit, may be tiled. */
} M2TextureFormat;


#define 	TEXTURESAMPLESTATE_TEXTURE_PITCH_OFFSET			0x4
#define 	TEXTURESAMPLESTATE_TEXTURE_PITCH_SHIFT				0
#define 	TEXTURESAMPLESTATE_TEXTURE_PITCH_MASK				ELEVEN_BIT_MASK
#define 	TEXTURESAMPLESTATE_VERTICAL_SLICES_EXPONENT_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_VERTICAL_SLICES_EXPONENT_SHIFT				29
#define 	TEXTURESAMPLESTATE_VERTICAL_SLICES_EXPONENT_MASK				(THREE_BIT_MASK<<29)
#define 	TEXTURESAMPLESTATE_HORIZONTAL_SLICES_EXPONENT_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_HORIZONTAL_SLICES_EXPONENT_SHIFT				26
#define 	TEXTURESAMPLESTATE_HORIZONTAL_SLICES_EXPONENT_MASK				(THREE_BIT_MASK<<26)
#define 	TEXTURESAMPLESTATE_HIGH_ORDER_FILTERING_MODE_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_HIGH_ORDER_FILTERING_MODE_SHIFT				24
#define 	TEXTURESAMPLESTATE_HIGH_ORDER_FILTERING_MODE_MASK				(TWO_BIT_MASK<<24)
typedef enum
{
	HOFM_Disable = 0x0,	/* No comment Given */
	HOFM_UVOffset = 0x1,	/* No comment Given */
	HOFM_StagedFiltering = 0x2,	/* load weight table directly. */
	HOFM_PreciseFiltering = 0x3	/* Use weight map in the same pair. */
} HighOrderFilterMode;


#define 	TEXTURESAMPLESTATE_TEXTURE_PAIR_SHARED_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_TEXTURE_PAIR_SHARED_SHIFT				23
#define 	TEXTURESAMPLESTATE_TEXTURE_PAIR_SHARED_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_DEPTH_TEXTURE_TEST_FUNCTION_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_DEPTH_TEXTURE_TEST_FUNCTION_SHIFT				20
#define 	TEXTURESAMPLESTATE_DEPTH_TEXTURE_TEST_FUNCTION_MASK				(THREE_BIT_MASK<<20)
/* See earlier definition of VisibilityTestFunctionCodes */
#define 	TEXTURESAMPLESTATE_UNDOGAMMA_CORRECTION_ENABLE_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_UNDOGAMMA_CORRECTION_ENABLE_SHIFT				18
#define 	TEXTURESAMPLESTATE_UNDOGAMMA_CORRECTION_ENABLE_MASK				(TWO_BIT_MASK<<18)
typedef enum
{
	TUGM_disable = 0x0,	/* No comment Given */
	TUGM_Enable_For_RGB = 0x2,	/* No comment Given */
	TUGM_Enable_For_Blue_Only = 0x3	/* No comment Given */
} TextureUndoGammaMode;


#define 	TEXTURESAMPLESTATE_TEXEL_CLAMP_ENABLE_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_TEXEL_CLAMP_ENABLE_SHIFT				17
#define 	TEXTURESAMPLESTATE_TEXEL_CLAMP_ENABLE_MASK				(ONE_BIT_MASK<<17)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_SWAP_RED_BLUE_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_SWAP_RED_BLUE_SHIFT				16
#define 	TEXTURESAMPLESTATE_SWAP_RED_BLUE_MASK				(ONE_BIT_MASK<<16)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_CONSTANT_TEXTURE_ALPHA_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_CONSTANT_TEXTURE_ALPHA_SHIFT				8
#define 	TEXTURESAMPLESTATE_CONSTANT_TEXTURE_ALPHA_MASK				(EIGHT_BIT_MASK<<8)
#define 	TEXTURESAMPLESTATE_YUV420_ENABLE_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_YUV420_ENABLE_SHIFT				7
#define 	TEXTURESAMPLESTATE_YUV420_ENABLE_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_TEXEL_KEY_POLARITY_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_TEXEL_KEY_POLARITY_SHIFT				6
#define 	TEXTURESAMPLESTATE_TEXEL_KEY_POLARITY_MASK				(ONE_BIT_MASK<<6)
typedef enum
{
	TKP_Normal = 0x0,	/* default. */
	TKP_Invert = 0x1	/* No comment Given */
} TexelKeyPolarity;


#define 	TEXTURESAMPLESTATE_TEXEL_KEY_COLOR_SELECTION_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_TEXEL_KEY_COLOR_SELECTION_SHIFT				4
#define 	TEXTURESAMPLESTATE_TEXEL_KEY_COLOR_SELECTION_MASK				(TWO_BIT_MASK<<4)
typedef enum
{
	TCKS_Disable = 0x0,	/* default. */
	TCKS_TexelColorKey0 = 0x1,	/* No comment Given */
	TCKS_TexelColorKey1 = 0x2,	/* No comment Given */
	TCKS_TexelColorKey2 = 0x3	/* No comment Given */
} TexelColorKeySelection;


#define 	TEXTURESAMPLESTATE_TEXEL_KEY_MODE_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_TEXEL_KEY_MODE_SHIFT				2
#define 	TEXTURESAMPLESTATE_TEXEL_KEY_MODE_MASK				(TWO_BIT_MASK<<2)
typedef enum
{
	TKM_Microsoft_Standard = 0x0,	/* default */
	TKM_Nearest_Key = 0x1,	/* No comment Given */
	TKM_Read_Destination_Key = 0x2	/* second Microsoft key method. */
} TexelKeyMode;


#define 	TEXTURESAMPLESTATE_CONSTANT_ALPHA_REPLACE_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_CONSTANT_ALPHA_REPLACE_SHIFT				1
#define 	TEXTURESAMPLESTATE_CONSTANT_ALPHA_REPLACE_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_TEXEL_BITS_SWAP_OFFSET			0x5
#define 	TEXTURESAMPLESTATE_TEXEL_BITS_SWAP_SHIFT				0
#define 	TEXTURESAMPLESTATE_TEXEL_BITS_SWAP_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */
#define 	TEXTURESAMPLESTATE_DELTAV_OFFSET			0x6
#define 	TEXTURESAMPLESTATE_DELTAV_SHIFT				16
#define 	TEXTURESAMPLESTATE_DELTAV_MASK				(SIXTEEN_BIT_MASK<<16)
#define 	TEXTURESAMPLESTATE_DELTAU_OFFSET			0x6
#define 	TEXTURESAMPLESTATE_DELTAU_SHIFT				0
#define 	TEXTURESAMPLESTATE_DELTAU_MASK				SIXTEEN_BIT_MASK
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_ALPHA_OFFSET			0x7
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_ALPHA_SHIFT				24
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_ALPHA_MASK				(EIGHT_BIT_MASK<<24)
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_RED_OFFSET			0x7
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_RED_SHIFT				16
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_RED_MASK				(EIGHT_BIT_MASK<<16)
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_GREEN_OFFSET			0x7
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_GREEN_SHIFT				8
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_GREEN_MASK				(EIGHT_BIT_MASK<<8)
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_BLUE_OFFSET			0x7
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_BLUE_SHIFT				0
#define 	TEXTURESAMPLESTATE_BORDER_COLOR_BLUE_MASK				EIGHT_BIT_MASK


#define M2REG_SHORTCUT1_TEXTURE_SAMPLER_ADDRESS  0x360
#define M2REG_SHORTCUT1_TEXTURE_SAMPLER_COMMAND  0xd8
#define 	M2REG_MIPMAP_LINEAR_FILTER_ENABLE_SC_RBOFFSET			0xd8
#define 	M2REG_MIPMAP_LINEAR_FILTER_ENABLE_SC_SHIFT				23
#define 	M2REG_MIPMAP_LINEAR_FILTER_ENABLE_SC_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_MAGNIFICATION_FILTER_MODE_SC_RBOFFSET			0xd8
#define 	M2REG_MAGNIFICATION_FILTER_MODE_SC_SHIFT				21
#define 	M2REG_MAGNIFICATION_FILTER_MODE_SC_MASK				(TWO_BIT_MASK<<21)
/* See earlier definition of TextureFilterMode */
#define 	M2REG_MINIFICATION_FILTER_MODE_SC_RBOFFSET			0xd8
#define 	M2REG_MINIFICATION_FILTER_MODE_SC_SHIFT				19
#define 	M2REG_MINIFICATION_FILTER_MODE_SC_MASK				(TWO_BIT_MASK<<19)
/* See earlier definition of TextureFilterMode */
#define 	M2REG_MAX_ANISOTROPIC_RATIO_EXPONENT_SC_RBOFFSET			0xd8
#define 	M2REG_MAX_ANISOTROPIC_RATIO_EXPONENT_SC_SHIFT				16
#define 	M2REG_MAX_ANISOTROPIC_RATIO_EXPONENT_SC_MASK				(THREE_BIT_MASK<<16)
/* See earlier definition of AnisotropicRatio */
#define 	M2REG_MAXIMUM_LOD_SC_RBOFFSET			0xd8
#define 	M2REG_MAXIMUM_LOD_SC_SHIFT				12
#define 	M2REG_MAXIMUM_LOD_SC_MASK				(FOUR_BIT_MASK<<12)
#define 	M2REG_LOD_BIAS_SC_RBOFFSET			0xd8
#define 	M2REG_LOD_BIAS_SC_SHIFT				0
#define 	M2REG_LOD_BIAS_SC_MASK				TEN_BIT_MASK
#define 	M2REG_TEXTURE_HEIGHT_SC_RBOFFSET			0xd9
#define 	M2REG_TEXTURE_HEIGHT_SC_SHIFT				16
#define 	M2REG_TEXTURE_HEIGHT_SC_MASK				(FIFTEEN_BIT_MASK<<16)
#define 	M2REG_TEXTURE_HEIGHT_EXPONENT_SC_RBOFFSET			0xd9
#define 	M2REG_TEXTURE_HEIGHT_EXPONENT_SC_SHIFT				27
#define 	M2REG_TEXTURE_HEIGHT_EXPONENT_SC_MASK				(FOUR_BIT_MASK<<27)
#define 	M2REG_TEXTURE_HEIGHT_MANTISSA_SC_RBOFFSET			0xd9
#define 	M2REG_TEXTURE_HEIGHT_MANTISSA_SC_SHIFT				16
#define 	M2REG_TEXTURE_HEIGHT_MANTISSA_SC_MASK				(ELEVEN_BIT_MASK<<16)
#define 	M2REG_TEXTURE_WIDTH_SC_RBOFFSET			0xd9
#define 	M2REG_TEXTURE_WIDTH_SC_SHIFT				0
#define 	M2REG_TEXTURE_WIDTH_SC_MASK				FIFTEEN_BIT_MASK
#define 	M2REG_TEXTURE_WIDTH_EXPONENT_SC_RBOFFSET			0xd9
#define 	M2REG_TEXTURE_WIDTH_EXPONENT_SC_SHIFT				11
#define 	M2REG_TEXTURE_WIDTH_EXPONENT_SC_MASK				(FOUR_BIT_MASK<<11)
#define 	M2REG_TEXTURE_WIDTH_MANTISSA_SC_RBOFFSET			0xd9
#define 	M2REG_TEXTURE_WIDTH_MANTISSA_SC_SHIFT				0
#define 	M2REG_TEXTURE_WIDTH_MANTISSA_SC_MASK				ELEVEN_BIT_MASK
#define 	M2REG_OPENGL_ADDRESSING_ENABLE_SC_RBOFFSET			0xda
#define 	M2REG_OPENGL_ADDRESSING_ENABLE_SC_SHIFT				31
#define 	M2REG_OPENGL_ADDRESSING_ENABLE_SC_MASK				(ONE_BIT_MASK<<31)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_BORDER_MODE_SC_RBOFFSET			0xda
#define 	M2REG_BORDER_MODE_SC_SHIFT				30
#define 	M2REG_BORDER_MODE_SC_MASK				(ONE_BIT_MASK<<30)
/* See earlier definition of BorderMode */
#define 	M2REG_V_ADDRESSING_MODE_SC_RBOFFSET			0xda
#define 	M2REG_V_ADDRESSING_MODE_SC_SHIFT				27
#define 	M2REG_V_ADDRESSING_MODE_SC_MASK				(THREE_BIT_MASK<<27)
/* See earlier definition of TexelAddressMode */
#define 	M2REG_U_ADDRESSING_MODE_SC_RBOFFSET			0xda
#define 	M2REG_U_ADDRESSING_MODE_SC_SHIFT				24
#define 	M2REG_U_ADDRESSING_MODE_SC_MASK				(THREE_BIT_MASK<<24)
/* See earlier definition of TexelAddressMode */
#define 	M2REG_TEXTURE_SWAP_MODE_SC_RBOFFSET			0xda
#define 	M2REG_TEXTURE_SWAP_MODE_SC_SHIFT				22
#define 	M2REG_TEXTURE_SWAP_MODE_SC_MASK				(TWO_BIT_MASK<<22)
/* See earlier definition of SurfaceSwapMode */
#define 	M2REG_TOTALELEMENTS_SC_RBOFFSET			0xda
#define 	M2REG_TOTALELEMENTS_SC_SHIFT				20
#define 	M2REG_TOTALELEMENTS_SC_MASK				(TWO_BIT_MASK<<20)
#define 	M2REG_CURRENT_MULTIPLE_ELEMENT_NUMBER_SC_RBOFFSET			0xda
#define 	M2REG_CURRENT_MULTIPLE_ELEMENT_NUMBER_SC_SHIFT				18
#define 	M2REG_CURRENT_MULTIPLE_ELEMENT_NUMBER_SC_MASK				(TWO_BIT_MASK<<18)
#define 	M2REG_TEXTURE_BAND_TILE_MODE_SC_RBOFFSET			0xda
#define 	M2REG_TEXTURE_BAND_TILE_MODE_SC_SHIFT				16
#define 	M2REG_TEXTURE_BAND_TILE_MODE_SC_MASK				(TWO_BIT_MASK<<16)
/* See earlier definition of BandTileMode */
#define 	M2REG_TEXTURE_FORMAT_SC_RBOFFSET			0xda
#define 	M2REG_TEXTURE_FORMAT_SC_SHIFT				11
#define 	M2REG_TEXTURE_FORMAT_SC_MASK				(FIVE_BIT_MASK<<11)
/* See earlier definition of M2TextureFormat */
#define 	M2REG_TEXTURE_PITCH_SC_RBOFFSET			0xda
#define 	M2REG_TEXTURE_PITCH_SC_SHIFT				0
#define 	M2REG_TEXTURE_PITCH_SC_MASK				ELEVEN_BIT_MASK
#define 	M2REG_VERTICAL_SLICES_EXPONENT_SC_RBOFFSET			0xdb
#define 	M2REG_VERTICAL_SLICES_EXPONENT_SC_SHIFT				29
#define 	M2REG_VERTICAL_SLICES_EXPONENT_SC_MASK				(THREE_BIT_MASK<<29)
#define 	M2REG_HORIZONTAL_SLICES_EXPONENT_SC_RBOFFSET			0xdb
#define 	M2REG_HORIZONTAL_SLICES_EXPONENT_SC_SHIFT				26
#define 	M2REG_HORIZONTAL_SLICES_EXPONENT_SC_MASK				(THREE_BIT_MASK<<26)
#define 	M2REG_HIGH_ORDER_FILTERING_MODE_SC_RBOFFSET			0xdb
#define 	M2REG_HIGH_ORDER_FILTERING_MODE_SC_SHIFT				24
#define 	M2REG_HIGH_ORDER_FILTERING_MODE_SC_MASK				(TWO_BIT_MASK<<24)
/* See earlier definition of HighOrderFilterMode */
#define 	M2REG_INTERLEAVED_WITH_ZBUFFER_SC_RBOFFSET			0xdb
#define 	M2REG_INTERLEAVED_WITH_ZBUFFER_SC_SHIFT				23
#define 	M2REG_INTERLEAVED_WITH_ZBUFFER_SC_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_DEPTH_TEXTURE_TEST_FUNCTION_SC_RBOFFSET			0xdb
#define 	M2REG_DEPTH_TEXTURE_TEST_FUNCTION_SC_SHIFT				20
#define 	M2REG_DEPTH_TEXTURE_TEST_FUNCTION_SC_MASK				(THREE_BIT_MASK<<20)
/* See earlier definition of VisibilityTestFunctionCodes */
#define 	M2REG_UNDOGAMMA_CORRECTION_ENABLE_SC_RBOFFSET			0xdb
#define 	M2REG_UNDOGAMMA_CORRECTION_ENABLE_SC_SHIFT				18
#define 	M2REG_UNDOGAMMA_CORRECTION_ENABLE_SC_MASK				(TWO_BIT_MASK<<18)
/* See earlier definition of TextureUndoGammaMode */
#define 	M2REG_TEXEL_CLAMP_ENABLE_SC_RBOFFSET			0xdb
#define 	M2REG_TEXEL_CLAMP_ENABLE_SC_SHIFT				17
#define 	M2REG_TEXEL_CLAMP_ENABLE_SC_MASK				(ONE_BIT_MASK<<17)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_SWAP_RED_BLUE_SC_RBOFFSET			0xdb
#define 	M2REG_SWAP_RED_BLUE_SC_SHIFT				16
#define 	M2REG_SWAP_RED_BLUE_SC_MASK				(ONE_BIT_MASK<<16)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_CONSTANT_TEXTURE_ALPHA_SC_RBOFFSET			0xdb
#define 	M2REG_CONSTANT_TEXTURE_ALPHA_SC_SHIFT				8
#define 	M2REG_CONSTANT_TEXTURE_ALPHA_SC_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_YUV420_ENABLE_SC_RBOFFSET			0xdb
#define 	M2REG_YUV420_ENABLE_SC_SHIFT				7
#define 	M2REG_YUV420_ENABLE_SC_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_TEXEL_KEY_POLARITY_SC_RBOFFSET			0xdb
#define 	M2REG_TEXEL_KEY_POLARITY_SC_SHIFT				6
#define 	M2REG_TEXEL_KEY_POLARITY_SC_MASK				(ONE_BIT_MASK<<6)
/* See earlier definition of TexelKeyPolarity */
#define 	M2REG_TEXEL_KEY_COLOR_SELECTION_SC_RBOFFSET			0xdb
#define 	M2REG_TEXEL_KEY_COLOR_SELECTION_SC_SHIFT				4
#define 	M2REG_TEXEL_KEY_COLOR_SELECTION_SC_MASK				(TWO_BIT_MASK<<4)
/* See earlier definition of TexelColorKeySelection */
#define 	M2REG_TEXEL_KEY_MODE_SC_RBOFFSET			0xdb
#define 	M2REG_TEXEL_KEY_MODE_SC_SHIFT				2
#define 	M2REG_TEXEL_KEY_MODE_SC_MASK				(TWO_BIT_MASK<<2)
/* See earlier definition of TexelKeyMode */
#define 	M2REG_CONSTANT_ALPHA_REPLACE_SC_RBOFFSET			0xdb
#define 	M2REG_CONSTANT_ALPHA_REPLACE_SC_SHIFT				1
#define 	M2REG_CONSTANT_ALPHA_REPLACE_SC_MASK				(ONE_BIT_MASK<<1)
/* 0 = Disable */
/* 1 = Enable */
#define 	M2REG_TEXEL_BITS_SWAP_SC_RBOFFSET			0xdb
#define 	M2REG_TEXEL_BITS_SWAP_SC_SHIFT				0
#define 	M2REG_TEXEL_BITS_SWAP_SC_MASK				ONE_BIT_MASK
/* 0 = Disable */
/* 1 = Enable */


#define M2REG_SHORTCUT2_TEXTURE_SAMPLER_ADDRESS  0x370
#define M2REG_SHORTCUT2_TEXTURE_SAMPLER_COMMAND  0xdc
#define 	M2REG_TEXEL_KEY_HIGH0_RED_SC_RBOFFSET			0xdc
#define 	M2REG_TEXEL_KEY_HIGH0_RED_SC_SHIFT				16
#define 	M2REG_TEXEL_KEY_HIGH0_RED_SC_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_TEXEL_KEY_HIGH0_GREEN_SC_RBOFFSET			0xdc
#define 	M2REG_TEXEL_KEY_HIGH0_GREEN_SC_SHIFT				8
#define 	M2REG_TEXEL_KEY_HIGH0_GREEN_SC_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_TEXEL_KEY_HIGH0_BLUE_SC_RBOFFSET			0xdc
#define 	M2REG_TEXEL_KEY_HIGH0_BLUE_SC_SHIFT				0
#define 	M2REG_TEXEL_KEY_HIGH0_BLUE_SC_MASK				EIGHT_BIT_MASK
#define 	M2REG_TEXEL_KEY_LOW0_RED_SC_RBOFFSET			0xdd
#define 	M2REG_TEXEL_KEY_LOW0_RED_SC_SHIFT				16
#define 	M2REG_TEXEL_KEY_LOW0_RED_SC_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_TEXEL_KEY_LOW0_GREEN_SC_RBOFFSET			0xdd
#define 	M2REG_TEXEL_KEY_LOW0_GREEN_SC_SHIFT				8
#define 	M2REG_TEXEL_KEY_LOW0_GREEN_SC_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_TEXEL_KEY_LOW0_BLUE_SC_RBOFFSET			0xdd
#define 	M2REG_TEXEL_KEY_LOW0_BLUE_SC_SHIFT				0
#define 	M2REG_TEXEL_KEY_LOW0_BLUE_SC_MASK				EIGHT_BIT_MASK
#define 	M2REG_DELTAV_SC_RBOFFSET			0xde
#define 	M2REG_DELTAV_SC_SHIFT				16
#define 	M2REG_DELTAV_SC_MASK				(SIXTEEN_BIT_MASK<<16)
#define 	M2REG_DELTAU_SC_RBOFFSET			0xde
#define 	M2REG_DELTAU_SC_SHIFT				0
#define 	M2REG_DELTAU_SC_MASK				SIXTEEN_BIT_MASK
#define 	M2REG_BORDER_COLOR_ALPHA_SC_RBOFFSET			0xdf
#define 	M2REG_BORDER_COLOR_ALPHA_SC_SHIFT				24
#define 	M2REG_BORDER_COLOR_ALPHA_SC_MASK				(EIGHT_BIT_MASK<<24)
#define 	M2REG_BORDER_COLOR_RED_SC_RBOFFSET			0xdf
#define 	M2REG_BORDER_COLOR_RED_SC_SHIFT				16
#define 	M2REG_BORDER_COLOR_RED_SC_MASK				(EIGHT_BIT_MASK<<16)
#define 	M2REG_BORDER_COLOR_GREEN_SC_RBOFFSET			0xdf
#define 	M2REG_BORDER_COLOR_GREEN_SC_SHIFT				8
#define 	M2REG_BORDER_COLOR_GREEN_SC_MASK				(EIGHT_BIT_MASK<<8)
#define 	M2REG_BORDER_COLOR_BLUE_SC_RBOFFSET			0xdf
#define 	M2REG_BORDER_COLOR_BLUE_SC_SHIFT				0
#define 	M2REG_BORDER_COLOR_BLUE_SC_MASK				EIGHT_BIT_MASK


/******************************
* PS20 INSTRUCTION
******************************/
#define 	PS20_OPCODE_OFFSET			0x0
#define 	PS20_OPCODE_SHIFT				0
#define 	PS20_OPCODE_MASK				FIVE_BIT_MASK
typedef enum
{
	MTL_NOP = 0x0,	/* No comment Given */
	MTL_MOV = 0x1,	/* No comment Given */
	MTL_ADD = 0x2,	/* No comment Given */
	MTL_FRCMUL = 0x3,	/* No comment Given */
	MTL_MAD = 0x4,	/* No comment Given */
	MTL_MUL = 0x5,	/* No comment Given */
	MTL_RCP = 0x6,	/* No comment Given */
	MTL_RSQ = 0x7,	/* No comment Given */
	MTL_REFV = 0x8,	/* No comment Given */
	MTL_DP4 = 0x9,	/* No comment Given */
	MTL_MIN = 0xa,	/* No comment Given */
	MTL_MAX = 0xb,	/* No comment Given */
	MTL_SLT = 0xc,	/* No comment Given */
	MTL_SGE = 0xd,	/* No comment Given */
	MTL_EXP = 0xe,	/* No comment Given */
	MTL_LOG = 0xf,	/* No comment Given */
	MTL_RCP_MUL = 0x10,	/* No comment Given */
	MTL_DST = 0x11,	/* No comment Given */
	MTL_LERP = 0x12,	/* No comment Given */
	MTL_FRC = 0x13,	/* No comment Given */
	MTL_EXPMUL = 0x14,	/* No comment Given */
	MTL_LOGMUL = 0x15,	/* No comment Given */
	MTL_SGN = 0x16,	/* No comment Given */
	MTL_RSQMUL = 0x17,	/* No comment Given */
	MTL_SIN = 0x18,	/* No comment Given */
	MTL_COS = 0x19,	/* No comment Given */
	MTL_CND = 0x1a,	/* No comment Given */
	MTL_CMP = 0x1b,	/* No comment Given */
	MTL_DDP4 = 0x1c,	/* No comment Given */
	MTL_DMUL = 0x1d,	/* No comment Given */
	MTL_DADD = 0x1e	/* No comment Given */
} MicroTimLanguage;


#define 	PS20_LASTONE_OFFSET			0x0
#define 	PS20_LASTONE_SHIFT				5
#define 	PS20_LASTONE_MASK				(ONE_BIT_MASK<<5)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_Z_VALID_OFFSET			0x0
#define 	PS20_Z_VALID_SHIFT				6
#define 	PS20_Z_VALID_MASK				(ONE_BIT_MASK<<6)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_W_VALID_OFFSET			0x0
#define 	PS20_W_VALID_SHIFT				7
#define 	PS20_W_VALID_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST0_SHIFT_FACTOR_OFFSET			0x0
#define 	PS20_DEST0_SHIFT_FACTOR_SHIFT				8
#define 	PS20_DEST0_SHIFT_FACTOR_MASK				(FOUR_BIT_MASK<<8)
#define 	PS20_DEST0_CLAMP_ENABLE_OFFSET			0x0
#define 	PS20_DEST0_CLAMP_ENABLE_SHIFT				12
#define 	PS20_DEST0_CLAMP_ENABLE_MASK				(ONE_BIT_MASK<<12)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_SRC0_NEG_ENABLE_OFFSET			0x0
#define 	PS20_SRC0_NEG_ENABLE_SHIFT				13
#define 	PS20_SRC0_NEG_ENABLE_MASK				(ONE_BIT_MASK<<13)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_SRC1_NEG_ENABLE_OFFSET			0x0
#define 	PS20_SRC1_NEG_ENABLE_SHIFT				14
#define 	PS20_SRC1_NEG_ENABLE_MASK				(ONE_BIT_MASK<<14)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_SRC2_NEG_ENABLE_OFFSET			0x0
#define 	PS20_SRC2_NEG_ENABLE_SHIFT				15
#define 	PS20_SRC2_NEG_ENABLE_MASK				(ONE_BIT_MASK<<15)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_SRC0_ID_OFFSET			0x0
#define 	PS20_SRC0_ID_SHIFT				16
#define 	PS20_SRC0_ID_MASK				(SEVEN_BIT_MASK<<16)
typedef enum
{
	PSS_Texture0 = 0x0,	/* No comment Given */
	PSS_Texture1 = 0x1,	/* No comment Given */
	PSS_Texture2 = 0x2,	/* No comment Given */
	PSS_Texture3 = 0x3,	/* No comment Given */
	PSS_Texture4 = 0x4,	/* No comment Given */
	PSS_Texture5 = 0x5,	/* No comment Given */
	PSS_Texture6 = 0x6,	/* No comment Given */
	PSS_Texture7 = 0x7,	/* No comment Given */
	PSS_Texture8 = 0x8,	/* No comment Given */
	PSS_Texture9 = 0x9,	/* No comment Given */
	PSS_Texture10 = 0xa,	/* No comment Given */
	PSS_Texture11 = 0xb,	/* No comment Given */
	PSS_Texture12 = 0xc,	/* No comment Given */
	PSS_Texture13 = 0xd,	/* No comment Given */
	PSS_Texture14 = 0xe,	/* No comment Given */
	PSS_Texture15 = 0xf,	/* No comment Given */
	PSS_Temp0 = 0x10,	/* No comment Given */
	PSS_Temp1 = 0x11,	/* No comment Given */
	PSS_Temp2 = 0x12,	/* No comment Given */
	PSS_Temp3 = 0x13,	/* No comment Given */
	PSS_Temp4 = 0x14,	/* No comment Given */
	PSS_Temp5 = 0x15,	/* No comment Given */
	PSS_Temp6 = 0x16,	/* No comment Given */
	PSS_Temp7 = 0x17,	/* No comment Given */
	PSS_Temp8 = 0x18,	/* No comment Given */
	PSS_Temp9 = 0x19,	/* No comment Given */
	PSS_Temp10 = 0x1a,	/* No comment Given */
	PSS_Temp11 = 0x1b,	/* No comment Given */
	PSS_Temp12 = 0x1c,	/* No comment Given */
	PSS_Temp13 = 0x1d,	/* No comment Given */
	PSS_Temp14 = 0x1e,	/* No comment Given */
	PSS_Temp15 = 0x1f,	/* No comment Given */
	PSS_Const0 = 0x20,	/* No comment Given */
	PSS_Const31 = 0x3f,	/* No comment Given */
	PSS_SConst0 = 0x40,	/* No comment Given */
	PSS_SConst1 = 0x41,	/* No comment Given */
	PSS_SConst2 = 0x42,	/* No comment Given */
	PSS_SConst3 = 0x43,	/* No comment Given */
	PSS_Diffuse = 0x44,	/* No comment Given */
	PSS_Specular = 0x45	/* No comment Given */
} PSSource;


#define 	PS20_SRC0_ABS_ENABLE_OFFSET			0x0
#define 	PS20_SRC0_ABS_ENABLE_SHIFT				23
#define 	PS20_SRC0_ABS_ENABLE_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_SRC0_R_SWIZZLE_OFFSET			0x0
#define 	PS20_SRC0_R_SWIZZLE_SHIFT				24
#define 	PS20_SRC0_R_SWIZZLE_MASK				(TWO_BIT_MASK<<24)
typedef enum
{
	CC_x = 0x0,	/* No comment Given */
	CC_y = 0x1,	/* No comment Given */
	CC_z = 0x2,	/* No comment Given */
	CC_w = 0x3	/* No comment Given */
} ChannelCode;


#define 	PS20_SRC0_G_SWIZZLE_OFFSET			0x0
#define 	PS20_SRC0_G_SWIZZLE_SHIFT				26
#define 	PS20_SRC0_G_SWIZZLE_MASK				(TWO_BIT_MASK<<26)
/* See earlier definition of ChannelCode */
#define 	PS20_SRC0_B_SWIZZLE_OFFSET			0x0
#define 	PS20_SRC0_B_SWIZZLE_SHIFT				28
#define 	PS20_SRC0_B_SWIZZLE_MASK				(TWO_BIT_MASK<<28)
/* See earlier definition of ChannelCode */
#define 	PS20_SRC0_A_SWIZZLE_OFFSET			0x0
#define 	PS20_SRC0_A_SWIZZLE_SHIFT				30
#define 	PS20_SRC0_A_SWIZZLE_MASK				(TWO_BIT_MASK<<30)
/* See earlier definition of ChannelCode */
#define 	PS20_SRC1_ID_OFFSET			0x1
#define 	PS20_SRC1_ID_SHIFT				0
#define 	PS20_SRC1_ID_MASK				SEVEN_BIT_MASK
/* See earlier definition of PSSource */
#define 	PS20_SRC1_ABS_ENABLE_OFFSET			0x1
#define 	PS20_SRC1_ABS_ENABLE_SHIFT				7
#define 	PS20_SRC1_ABS_ENABLE_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_SRC1_R_SWIZZLE_OFFSET			0x1
#define 	PS20_SRC1_R_SWIZZLE_SHIFT				8
#define 	PS20_SRC1_R_SWIZZLE_MASK				(TWO_BIT_MASK<<8)
/* See earlier definition of ChannelCode */
#define 	PS20_SRC1_G_SWIZZLE_OFFSET			0x1
#define 	PS20_SRC1_G_SWIZZLE_SHIFT				10
#define 	PS20_SRC1_G_SWIZZLE_MASK				(TWO_BIT_MASK<<10)
/* See earlier definition of ChannelCode */
#define 	PS20_SRC1_B_SWIZZLE_OFFSET			0x1
#define 	PS20_SRC1_B_SWIZZLE_SHIFT				12
#define 	PS20_SRC1_B_SWIZZLE_MASK				(TWO_BIT_MASK<<12)
/* See earlier definition of ChannelCode */
#define 	PS20_SRC1_A_SWIZZLE_OFFSET			0x1
#define 	PS20_SRC1_A_SWIZZLE_SHIFT				14
#define 	PS20_SRC1_A_SWIZZLE_MASK				(TWO_BIT_MASK<<14)
/* See earlier definition of ChannelCode */
#define 	PS20_SRC2_ID_OFFSET			0x1
#define 	PS20_SRC2_ID_SHIFT				16
#define 	PS20_SRC2_ID_MASK				(SEVEN_BIT_MASK<<16)
/* See earlier definition of PSSource */
#define 	PS20_SRC2_ABS_ENABLE_OFFSET			0x1
#define 	PS20_SRC2_ABS_ENABLE_SHIFT				23
#define 	PS20_SRC2_ABS_ENABLE_MASK				(ONE_BIT_MASK<<23)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_SRC2_R_SWIZZLE_OFFSET			0x1
#define 	PS20_SRC2_R_SWIZZLE_SHIFT				24
#define 	PS20_SRC2_R_SWIZZLE_MASK				(TWO_BIT_MASK<<24)
/* See earlier definition of ChannelCode */
#define 	PS20_SRC2_G_SWIZZLE_OFFSET			0x1
#define 	PS20_SRC2_G_SWIZZLE_SHIFT				26
#define 	PS20_SRC2_G_SWIZZLE_MASK				(TWO_BIT_MASK<<26)
/* See earlier definition of ChannelCode */
#define 	PS20_SRC2_B_SWIZZLE_OFFSET			0x1
#define 	PS20_SRC2_B_SWIZZLE_SHIFT				28
#define 	PS20_SRC2_B_SWIZZLE_MASK				(TWO_BIT_MASK<<28)
/* See earlier definition of ChannelCode */
#define 	PS20_SRC2_A_SWIZZLE_OFFSET			0x1
#define 	PS20_SRC2_A_SWIZZLE_SHIFT				30
#define 	PS20_SRC2_A_SWIZZLE_MASK				(TWO_BIT_MASK<<30)
/* See earlier definition of ChannelCode */
#define 	PS20_DEST0_ID_OFFSET			0x2
#define 	PS20_DEST0_ID_SHIFT				0
#define 	PS20_DEST0_ID_MASK				FIVE_BIT_MASK
typedef enum
{
	PSD_2DBump = 0x0,	/* No comment Given */
	PSD_4DBump = 0x1,	/* No comment Given */
	PSD_Depth = 0x2,	/* No comment Given */
	PSD_Specular = 0x3,	/* No comment Given */
	PSD_Result0 = 0x4,	/* No comment Given */
	PSD_Result1 = 0x5,	/* No comment Given */
	PSD_Result2 = 0x6,	/* No comment Given */
	PSD_Result3 = 0x7,	/* No comment Given */
	PSD_Texture0 = 0x8,	/* No comment Given */
	PSD_Texture1 = 0x9,	/* No comment Given */
	PSD_Texture2 = 0xa,	/* No comment Given */
	PSD_Texture3 = 0xb,	/* No comment Given */
	PSD_Texture4 = 0xc,	/* No comment Given */
	PSD_Texture5 = 0xd,	/* No comment Given */
	PSD_Texture6 = 0xe,	/* No comment Given */
	PSD_Texture7 = 0xf,	/* No comment Given */
	PSD_Temp0 = 0x10,	/* No comment Given */
	PSD_Temp1 = 0x11,	/* No comment Given */
	PSD_Temp2 = 0x12,	/* No comment Given */
	PSD_Temp3 = 0x13,	/* No comment Given */
	PSD_Temp4 = 0x14,	/* No comment Given */
	PSD_Temp5 = 0x15,	/* No comment Given */
	PSD_Temp6 = 0x16,	/* No comment Given */
	PSD_Temp7 = 0x17,	/* No comment Given */
	PSD_Temp8 = 0x18,	/* No comment Given */
	PSD_Temp9 = 0x19,	/* No comment Given */
	PSD_Temp10 = 0x1a,	/* No comment Given */
	PSD_Temp11 = 0x1b,	/* No comment Given */
	PSD_Temp12 = 0x1c,	/* No comment Given */
	PSD_Temp13 = 0x1d,	/* No comment Given */
	PSD_Temp14 = 0x1e,	/* No comment Given */
	PSD_Temp15 = 0x1f	/* No comment Given */
} PSDestination;


#define 	PS20_DEST0_ABS_ENABLE_OFFSET			0x2
#define 	PS20_DEST0_ABS_ENABLE_SHIFT				5
#define 	PS20_DEST0_ABS_ENABLE_MASK				(ONE_BIT_MASK<<5)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST0_A_WRITE_ENABLE_OFFSET			0x2
#define 	PS20_DEST0_A_WRITE_ENABLE_SHIFT				6
#define 	PS20_DEST0_A_WRITE_ENABLE_MASK				(ONE_BIT_MASK<<6)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST0_B_WRITE_ENABLE_OFFSET			0x2
#define 	PS20_DEST0_B_WRITE_ENABLE_SHIFT				7
#define 	PS20_DEST0_B_WRITE_ENABLE_MASK				(ONE_BIT_MASK<<7)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST0_G_WRITE_ENABLE_OFFSET			0x2
#define 	PS20_DEST0_G_WRITE_ENABLE_SHIFT				8
#define 	PS20_DEST0_G_WRITE_ENABLE_MASK				(ONE_BIT_MASK<<8)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST0_R_WRITE_ENABLE_OFFSET			0x2
#define 	PS20_DEST0_R_WRITE_ENABLE_SHIFT				9
#define 	PS20_DEST0_R_WRITE_ENABLE_MASK				(ONE_BIT_MASK<<9)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST1_ID_OFFSET			0x2
#define 	PS20_DEST1_ID_SHIFT				10
#define 	PS20_DEST1_ID_MASK				(FIVE_BIT_MASK<<10)
/* See earlier definition of PSDestination */
#define 	PS20_DEST1_ABS_ENABLE_OFFSET			0x2
#define 	PS20_DEST1_ABS_ENABLE_SHIFT				15
#define 	PS20_DEST1_ABS_ENABLE_MASK				(ONE_BIT_MASK<<15)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST1_A_WRITE_ENABLE_OFFSET			0x2
#define 	PS20_DEST1_A_WRITE_ENABLE_SHIFT				16
#define 	PS20_DEST1_A_WRITE_ENABLE_MASK				(ONE_BIT_MASK<<16)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST1_B_WRITE_ENABLE_OFFSET			0x2
#define 	PS20_DEST1_B_WRITE_ENABLE_SHIFT				17
#define 	PS20_DEST1_B_WRITE_ENABLE_MASK				(ONE_BIT_MASK<<17)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST1_G_WRITE_ENABLE_OFFSET			0x2
#define 	PS20_DEST1_G_WRITE_ENABLE_SHIFT				18
#define 	PS20_DEST1_G_WRITE_ENABLE_MASK				(ONE_BIT_MASK<<18)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST1_R_WRITE_ENABLE_OFFSET			0x2
#define 	PS20_DEST1_R_WRITE_ENABLE_SHIFT				19
#define 	PS20_DEST1_R_WRITE_ENABLE_MASK				(ONE_BIT_MASK<<19)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_COISSUE_MOVE_ENABLE_OFFSET			0x2
#define 	PS20_COISSUE_MOVE_ENABLE_SHIFT				20
#define 	PS20_COISSUE_MOVE_ENABLE_MASK				(ONE_BIT_MASK<<20)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST1_CLAMP_ENABLE_OFFSET			0x2
#define 	PS20_DEST1_CLAMP_ENABLE_SHIFT				21
#define 	PS20_DEST1_CLAMP_ENABLE_MASK				(ONE_BIT_MASK<<21)
/* 0 = Disable */
/* 1 = Enable */
#define 	PS20_DEST1_SHIFT_FACTOR_OFFSET			0x2
#define 	PS20_DEST1_SHIFT_FACTOR_SHIFT				22
#define 	PS20_DEST1_SHIFT_FACTOR_MASK				(FOUR_BIT_MASK<<22)



#endif


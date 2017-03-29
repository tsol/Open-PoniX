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

#ifndef _XG47_CMDLIST_H_
#define _XG47_CMDLIST_H_

struct xg47_CmdList;

/* cmdList size in dword */
extern int  xg47_BeginCmdListType(struct xg47_CmdList *, unsigned size,
    unsigned type);
extern void xg47_EndCmdList(struct xg47_CmdList *);
extern void xg47_SendGECommand(struct xg47_CmdList *, uint32_t addr,
    uint32_t cmd);
extern void xg47_EmitRawCommand(struct xg47_CmdList *pCmdList,
    const uint32_t *cmd);

extern struct xg47_CmdList *xg47_Initialize(ScrnInfoPtr pScrn, 
    unsigned int cmdBufSize, int fd);
extern void xg47_Cleanup(ScrnInfoPtr pScrn, struct xg47_CmdList *s_pCmdList);
extern void xg47_Reset(struct xg47_CmdList *);

#define xg47_BeginCmdList(l, s) xg47_BeginCmdListType(l, s, BTYPE_2D)

typedef enum
{
    FLUSH_2D                        = M2REG_FLUSH_2D_ENGINE_MASK,
    FLUSH_3D                        = M2REG_FLUSH_3D_ENGINE_MASK,
    FLUSH_FLIP                      = M2REG_FLUSH_FLIP_ENGINE_MASK
}FLUSH_CODE;

typedef enum
{
    AGPCMDLIST_SCRATCH_SIZE         = 0x100,
    AGPCMDLIST_BEGIN_SIZE           = 0x004,
    AGPCMDLIST_3D_SCRATCH_CMD_SIZE  = 0x004,
    AGPCMDLIST_2D_SCRATCH_CMD_SIZE  = 0x00c,
    AGPCMDLIST_FLUSH_CMD_LEN        = 0x004,
    AGPCMDLIST_DUMY_END_BATCH_LEN   = AGPCMDLIST_BEGIN_SIZE
}CMD_SIZE;

#endif

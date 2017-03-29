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

#include "xf86.h"
#include "xgi.h"
#include "xgi_regs.h"
#include "xg47_regs.h"
#include "xgi_driver.h"
#include "xg47_cmdlist.h"
#include "xgi_misc.h"
#include "xgi_debug.h"

/**
 * Track a batch of commands to be sent to hardware.
 */
struct xg47_batch {
    enum xgi_batch_type type;   /**< Command engine to receive commands. */
    unsigned int request_size;  /**< Number of 32-bit words requested for this
				 * batch. 
				 */
    uint32_t *  begin;          /**< Pointer to first command of batch. */
    uint32_t *  end;            /**< Pointer for writing next coomand */
};


/**
 * Track buffer allocated from DRM for sending commands to hardware.
 */
struct xg47_buffer {
    uint32_t *    ptr;          /**< CPU pointer to buffer base. */
    uint32_t      hw_addr;      /**< Buffer base address from hardware point
				 * of view. 
				 */
    unsigned long bus_addr;     /**< Handle used to release buffer. */
    unsigned int  size;         /**< Size of buffer in 32-bit words. */
};

struct xg47_CmdList
{
    struct xg47_batch current;  /**< Active command buffer. */
    struct xg47_batch previous; /**< Previous buffer sent to hardware. */
    struct xg47_buffer command; /**< Memory allocated from DRM. */

    /**
     * \name 2D Command bunch
     * 
     * Buffer indiviual 2D command register writes.
     */
    /*@{*/
    unsigned bunch_count;       /**< Number of commands pending in \c bunch. */
    uint32_t bunch[4];          /**< Register write data. */
    /*@}*/


    int		_fd;            /**< DRM file handle. */
    
    /**
     * \name Fence values for the top and bottom halves of the command buffer
     *
     * Each fence will either contain a fence breadcrumb number or zero if
     * not set.
     */
    /*@{*/
    uint32_t top_fence;         /**< Fence protecting the top half */
    uint32_t bottom_fence;      /**< Fence protecting the bottom half */
    /*@}*/
};


struct xg47_CmdList *
xg47_Initialize(ScrnInfoPtr pScrn, unsigned int cmdBufSize, int fd)
{
    struct xg47_CmdList *list = xnfcalloc(sizeof(struct xg47_CmdList), 1);
    int ret;

    list->command.size = cmdBufSize;
    list->_fd = fd;

    if (!XGIPcieMemAllocate(pScrn,
                            list->command.size * sizeof(uint32_t),
                            & list->command.bus_addr,
                            & list->command.hw_addr,
                            (void **) & list->command.ptr)) {
        XGIDebug(DBG_ERROR, "[DBG Error]Allocate CmdList buffer error!\n");
        goto err;
    }

    XGIDebug(DBG_CMDLIST, "cmdBuf VAddr=0x%p  HAddr=0x%p buffsize=0x%x\n",
             list->command.ptr, list->command.hw_addr, list->command.size);

    xg47_Reset(list);

    return list;

err:
    xg47_Cleanup(pScrn, list);
    return NULL;
}

void xg47_Cleanup(ScrnInfoPtr pScrn, struct xg47_CmdList *s_pCmdList)
{
    if (s_pCmdList) {
        if (s_pCmdList->command.bus_addr) {
            XGIDebug(DBG_CMDLIST, "[DBG Free]cmdBuf VAddr=0x%x  HAddr=0x%x\n",
                     s_pCmdList->command.ptr,
                     s_pCmdList->command.hw_addr);

            XGIPcieMemFree(pScrn, s_pCmdList->command.size * sizeof(uint32_t),
                           s_pCmdList->command.bus_addr,
                           s_pCmdList->command.ptr);
        }

        free(s_pCmdList);
    }
}

void xg47_Reset(struct xg47_CmdList *s_pCmdList)
{
    s_pCmdList->previous.begin = s_pCmdList->command.ptr;
    s_pCmdList->previous.end = s_pCmdList->command.ptr;
    s_pCmdList->current.end = 0;
}

/* Implementation Part*/
static void emit_bunch(struct xg47_CmdList *pCmdList);
static void reset_bunch(struct xg47_CmdList *pCmdList);

#ifdef DUMP_COMMAND_BUFFER
static void dumpCommandBuffer(struct xg47_CmdList * pCmdList);
#endif

uint32_t s_emptyBegin[AGPCMDLIST_BEGIN_SIZE] =
{
    0x10000000,     /* 3D Type Begin, Invalid */
    0x80000004,     /* Length = 4;  */
    0x00000000,
    0x00000000
};


static void
xg47_set_fence(int fd, uint32_t *fence)
{
    int ret;
    
    
    ret = drmCommandWriteRead(fd, DRM_XGI_SET_FENCE, fence, sizeof(uint32_t));
    if (ret) {
	/* Do something? */
    }
}

static void
xg47_wait_fence(int fd, uint32_t fence)
{
    uint32_t temp = fence;
    int ret;

    ret = drmCommandWriteRead(fd, DRM_XGI_WAIT_FENCE, &temp, sizeof(uint32_t));
    if (ret) {
	/* Do something? */
    }
}


/**
 * Reserve space in the command buffer
 * 
 * \param pCmdList  pointer to the command list structure
 * \param size      Size, in DWORDS, of the command
 *
 * \returns
 * 1 -- success 0 -- false
 */
int xg47_BeginCmdListType(struct xg47_CmdList *pCmdList, unsigned req_size,
                          unsigned type)
{
    /* Pad the commmand list to 128-bit alignment and add the begin header.
     */
    const unsigned size = ((req_size + 0x3) & ~0x3) + AGPCMDLIST_BEGIN_SIZE;
    const uint32_t *const mid_point =
        pCmdList->command.ptr + (pCmdList->command.size / 2);
    const uint32_t *const end_point = 
        pCmdList->command.ptr + pCmdList->command.size;
    uint32_t * begin_cmd = pCmdList->previous.end;
    uint32_t *const end_cmd = pCmdList->previous.end + size;


    XGIDebug(DBG_CMDLIST, "[DEBUG] Enter beginCmdList.\n");

    if (size >= pCmdList->command.size) {
        return 0;
    }


    /* If the command spills into the bottom half of the command buffer,
     * wait on the bottom half's fence.
     */
    if ((begin_cmd < mid_point) && (end_cmd > mid_point)) {
        if (pCmdList->bottom_fence != 0) {
	    xg47_wait_fence(pCmdList->_fd, pCmdList->bottom_fence);
            pCmdList->bottom_fence = 0;
        }
    } else {
        /* If the command won't fit at the end of the list and we need to wrap
         * back to the top half of the command buffer, wait on the top half's
         * fence.
         *
         * After waiting on the top half's fence, emit the bottom half's
         * fence.
         */
        if (end_cmd > end_point) {
            begin_cmd = pCmdList->command.ptr;

            if (pCmdList->top_fence != 0) {
		xg47_wait_fence(pCmdList->_fd, pCmdList->top_fence);
		pCmdList->top_fence = 0;
            }

	    xg47_set_fence(pCmdList->_fd, & pCmdList->bottom_fence);
        }
    }


    /* Prepare the begin address of next batch. */
    pCmdList->current.begin = begin_cmd;
    pCmdList->current.end = pCmdList->current.begin;
    pCmdList->current.request_size = size;
    pCmdList->current.type = type;


    /* Prepare next begin */
    xg47_EmitRawCommand(pCmdList, s_emptyBegin);
    reset_bunch(pCmdList);


    XGIDebug(DBG_CMDLIST, "[DEBUG] Leave beginCmdList.\n");
    return 1;
}


/**
 * Copy raw command data into the command buffer.
 *
 * \param pCmdList  Command list control structure.
 * \param cmd       Pointer to 4 32-bit commands.
 *
 * \note
 * This function performs byte-swapping on each 32-bit value on big-endian
 * architectures.
 */
void xg47_EmitRawCommand(struct xg47_CmdList *pCmdList, const uint32_t *cmd)
{
    pCmdList->current.end[0] = BE_SWAP32(cmd[0]);
    pCmdList->current.end[1] = BE_SWAP32(cmd[1]);
    pCmdList->current.end[2] = BE_SWAP32(cmd[2]);
    pCmdList->current.end[3] = BE_SWAP32(cmd[3]);
    pCmdList->current.end += 4;
}


/**
 * Reset the 2D register-write data.
 *
 * \param pCmdList  Command list control structure.
 */
void reset_bunch(struct xg47_CmdList *pCmdList)
{
    pCmdList->bunch[0] = 0x7f000000;
    pCmdList->bunch[1] = 0x00000000;
    pCmdList->bunch[2] = 0x00000000;
    pCmdList->bunch[3] = 0x00000000;
    pCmdList->bunch_count = 0;
}


void emit_bunch(struct xg47_CmdList *pCmdList)
{
    /* Copy the commands from bunch to the command buffer and advance the
     * command buffer write pointer.
     */
    xg47_EmitRawCommand(pCmdList, pCmdList->bunch);

    /* Reset bunch.
     */
    reset_bunch(pCmdList);
}


void xg47_SendGECommand(struct xg47_CmdList *pCmdList, uint32_t addr,
                        uint32_t cmd)
{
    /* Encrypt the command for AGP. */
    const unsigned shift = pCmdList->bunch_count;
    const uint32_t reg = (addr & 0x00ff);

    pCmdList->bunch[0] |= (reg | 1) << (shift << 3);
    pCmdList->bunch[shift + 1]  = cmd;

    /* Bunch finished, Send to HW. */
    pCmdList->bunch_count++;
    if (pCmdList->bunch_count == 3) {
        emit_bunch(pCmdList);
    }
}


#ifdef DUMP_COMMAND_BUFFER
void dumpCommandBuffer(struct xg47_CmdList * pCmdList)
{
    const unsigned int count = pCmdList->current.end
        - pCmdList->current.begin;
    unsigned int i;

    XGIDebug(DBG_FUNCTION,"Entering dumpCommandBuffer\n");

    for (i = 0; i < count; i += 4) {
        XGIDebug(DBG_CMD_BUFFER, "%08p: %08x %08x %08x %08x\n",
                 (pCmdList->current.begin + i),
                 pCmdList->current.begin[i + 0],
                 pCmdList->current.begin[i + 1],
                 pCmdList->current.begin[i + 2],
                 pCmdList->current.begin[i + 3]);
    }

    XGIDebug(DBG_FUNCTION,"Leaving dumpCommandBuffer\n");
}
#endif /* DUMP_COMMAND_BUFFER */


void xg47_EndCmdList(struct xg47_CmdList *pCmdList)
{
    uint32_t beginHWAddr;
    struct xgi_cmd_info submitInfo;
    int err;
    size_t data_count;

    XGIDebug(DBG_FUNCTION, "%s: enter\n", __func__);

    /* If there are any pending commands in bunch, emit the whole batch.
     */
    if (pCmdList->bunch_count != 0) {
        emit_bunch(pCmdList);
    }


    /* Calculate data counter *after* flushing the 2D register-write batch as
     * the flush may change the data count.
     */
    data_count = pCmdList->current.end - pCmdList->current.begin;
    if (data_count == 0) {
        return;
    }

    if (data_count > pCmdList->current.request_size) {
        ErrorF("Too much data written written to command buffer! %u vs. %u\n",
	       data_count, pCmdList->current.request_size);
    }


    beginHWAddr = pCmdList->command.hw_addr
	+ ((intptr_t) pCmdList->current.begin
	   - (intptr_t) pCmdList->command.ptr);

    submitInfo.type = pCmdList->current.type;
    submitInfo.hw_addr = beginHWAddr;
    submitInfo.size = data_count;

    XGIDebug(DBG_FUNCTION, "%s: calling ioctl XGI_IOCTL_SUBMIT_CMDLIST\n", 
             __func__);

#ifdef DUMP_COMMAND_BUFFER
    dumpCommandBuffer(pCmdList);
#endif

    err = drmCommandWrite(pCmdList->_fd, DRM_XGI_SUBMIT_CMDLIST,
                          &submitInfo, sizeof(submitInfo));
    if (!err) {
        uint32_t *const begin_cmd = pCmdList->current.begin;
        uint32_t *const end_cmd = pCmdList->current.end;
        const uint32_t *const mid_point =
            pCmdList->command.ptr + (pCmdList->command.size / 2);

        pCmdList->previous = pCmdList->current;

        /* If the command is the last command in the top half, emit the top
         * half's fence.
         */
        if ((begin_cmd < mid_point) && (end_cmd >= mid_point)) {
	    xg47_set_fence(pCmdList->_fd, & pCmdList->top_fence);
        }
    } else {
        ErrorF("[2D] ioctl -- cmdList error (%d, %s)!\n",
               -err, strerror(-err));
    }

    XGIDebug(DBG_FUNCTION, "%s: exit\n", __func__);
    return;
}

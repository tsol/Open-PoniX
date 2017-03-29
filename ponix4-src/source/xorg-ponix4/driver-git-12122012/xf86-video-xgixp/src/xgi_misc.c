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
#include "xgi_driver.h"
#include "xgi_bios.h"

#include "xgi_debug.h"

#if 0
void XGIDumpRegisterValue(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    int i ;
    unsigned long temp ;

    ErrorF("----------------------------------------------------------------------\n") ;
    ErrorF("0x3C4 xx\n") ;
    ErrorF("----------------------------------------------------------------------\n") ;
    for( i = 0 ; i < 0xFF ; i++ )
    {
        OUTB(0x3C4, i);
        temp = INB(0x3C5);
        ErrorF("R%02X = 0x%02X   ", i, temp);
        if ( ((i+1) % 4) == 0 )
        {
            ErrorF("\n") ;
        }
        if ( ((i+1) % 16) == 0 )
        {
            ErrorF("\n") ;
        }
    }
    ErrorF( "\n" ) ;
    ErrorF("----------------------------------------------------------------------\n") ;
    ErrorF("0x3D4 xx\n") ;
    ErrorF("----------------------------------------------------------------------\n") ;
    for( i = 0 ; i < 0xFF ; i++ )
    {
        OUTB(0x3D4, i);
        temp = INB(0x3D5);

        ErrorF("R%02X = 0x%02X   ", i, temp);
        if ( ((i+1) % 4) == 0 )
        {
            ErrorF("\n") ;
        }
        if ( ((i+1) % 16) == 0 )
        {
            ErrorF("\n") ;
        }
    }
    ErrorF( "\n" ) ;
    ErrorF("----------------------------------------------------------------------\n") ;
    ErrorF("0x3CE xx\n") ;
    ErrorF("----------------------------------------------------------------------\n") ;
    for( i = 0 ; i < 0xFF; i++ )
    {
        OUTB(0x3CE, i);
        temp = INB(0x3CF);

        ErrorF("R%02X = 0x%02X   ", i, temp);
        if (((i+1) % 4) == 0)
        {
            ErrorF("\n");
        }
        if (((i+1) % 16) == 0)
        {
            ErrorF("\n");
        }
    }
    ErrorF( "\n" );
}
#endif

/* Jong 07/12/2006 */
void XGIDumpMemory(CARD8 *addr, unsigned long size)
{
    int             i, j;

/* Jong 07/192006 */
#ifndef DUMP_MEMORY
	return;
#endif

	ErrorF("\n==================memory dump at %p, size = %lu ===============\n", addr, size);

    for(i=0; i<0x10; i++)
    {
        if(i == 0)
        {
            ErrorF("%5x", i);
        }
        else
        {
            ErrorF("%3x", i);
        }
    }
    ErrorF("\n");

    for(i=0; i<0x10; i++)
    {
        ErrorF("%1x ", i);

        for(j=0; j<0x10; j++)
        {
            ErrorF("%3x", *((addr+i*16)+j));
        }
        ErrorF("\n");
    }
}

void XGIDumpRegisterValue(ScrnInfoPtr pScrn)
{
    XGIPtr          pXGI = XGIPTR(pScrn);
    int             i, j;
    unsigned char   temp;

    /* 0x3C5 */
    ErrorF("\n==================0x%x===============\n", 0x3C5);

    for(i=0; i<0x10; i++)
    {
        if(i == 0)
        {
            ErrorF("%5x", i);
        }
        else
        {
            ErrorF("%3x", i);
        }
    }
    ErrorF("\n");

    for(i=0; i<0x10; i++)
    {
        ErrorF("%1x ", i);

        for(j=0; j<0x10; j++)
        {
            temp = IN3C5B(i*0x10 + j);
            ErrorF("%3x", temp);
        }
        ErrorF("\n");
    }

    /* 0x3D5 */
    ErrorF("\n==================0x%x===============\n", 0x3D5);
    for(i=0; i<0x10; i++)
    {
        if(i == 0)
        {
            ErrorF("%5x", i);
        }
        else
        {
            ErrorF("%3x", i);
        }
    }
    ErrorF("\n");

    for(i=0; i<0x10; i++)
    {
        ErrorF("%1x ", i);

        for(j=0; j<0x10; j++)
        {
            temp = IN3X5B(i*0x10 + j);
            ErrorF("%3x", temp);
        }
        ErrorF("\n");
    }

    /* 0x3CF */
    ErrorF("\n==================0x%x===============\n", 0x3CF);
    for(i=0; i<0x10; i++)
    {
        if(i == 0)
        {
            ErrorF("%5x", i);
        }
        else
        {
            ErrorF("%3x", i);
        }
    }
    ErrorF("\n");

    for(i=0; i<0x10; i++)
    {
        ErrorF("%1x ", i);

        for(j=0; j<0x10; j++)
        {
            temp = IN3CFB(i*0x10 + j);
            ErrorF("%3x", temp);
        }
        ErrorF("\n");
    }

    ErrorF("\n==================0x%x===============\n", 0xB000);
    for(i=0; i<0x10; i++)
    {
        if(i == 0)
        {
            ErrorF("%5x", i);
        }
        else
        {
            ErrorF("%3x", i);
        }
    }
    ErrorF("\n");

    for(i=0; i<0x5; i++)
    {
        ErrorF("%1x ", i);

        for(j=0; j<0x10; j++)
        {
            temp = INB(0xB000 + i*0x10 + j);
            ErrorF("%3x", temp);
        }
        ErrorF("\n");
    }

    ErrorF("\n==================0x%x===============\n", 0x2300);
    for(i=0; i<0x10; i++)
    {
        if(i == 0)
        {
            ErrorF("%5x", i);
        }
        else
        {
            ErrorF("%3x", i);
        }
    }
    ErrorF("\n");

    for(i=0; i<0x7; i++)
    {
        ErrorF("%1x ", i);

        for(j=0; j<0x10; j++)
        {
            temp = INB(0x2300 + i*0x10 + j);
            ErrorF("%3x", temp);
        }
        ErrorF("\n");
    }

    ErrorF("\n==================0x%x===============\n", 0x2400);
    for(i=0; i<0x10; i++)
    {
        if(i == 0)
        {
            ErrorF("%5x", i);
        }
        else
        {
            ErrorF("%3x", i);
        }
    }
    ErrorF("\n");

    for(i=0; i<0x10; i++)
    {
        ErrorF("%1x ", i);

        for(j=0; j<0x10; j++)
        {
            temp = INB(0x2400 + i*0x10 + j);
            ErrorF("%3x", temp);
        }
        ErrorF("\n");
    }
}

Bool XGIPcieMemAllocate(ScrnInfoPtr pScrn, size_t size,
                        unsigned long *offset,
                        uint32_t *pBufHWAddr, void **pBufVirtAddr)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    struct xgi_mem_alloc  alloc;
    int ret;

    alloc.location = XGI_MEMLOC_NON_LOCAL;
    alloc.size = size;

    ret = drmCommandWriteRead(pXGI->drm_fd, DRM_XGI_ALLOC, &alloc,
			      sizeof(alloc));
    if (ret < 0) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
		   "PCIE memory allocate ioctl failed!\n");
        return FALSE;
    }

    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "alloc.size: 0x%x "
               "alloc.offset: 0x%x alloc.hwAddr: 0x%x\n",
               alloc.size, alloc.offset, alloc.hw_addr);

    *offset = alloc.index;
    *pBufHWAddr = alloc.hw_addr;
    *pBufVirtAddr = pXGI->gart_vaddr + alloc.offset;

    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "pBufVirtAddr: 0x%p\n",
               *pBufVirtAddr);

    return TRUE;
}

Bool XGIPcieMemFree(ScrnInfoPtr pScrn, size_t size, unsigned long offset)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    int ret;


    ret = drmCommandWrite(pXGI->drm_fd, DRM_XGI_FREE, &offset,
			  sizeof(offset));
    if (ret < 0) {
	xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
		   "PCIE memory IOCTL free failed (%d, %s) %p, %d\n",
		   -ret, strerror(-ret), pXGI, pXGI->drm_fd);
    }

    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
               "PCIE memory free size: 0x%x offset: 0x%lx\n",
               (unsigned int) size, offset);

    return ret == 0;
}

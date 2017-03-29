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

#ifndef _XGI_REGS_H_
#define _XGI_REGS_H_

#include "compiler.h"

#if X_BYTE_ORDER == X_BIG_ENDIAN
# define BE_SWAP32(v) (lswapl(v))
# define BE_SWAP16(v) (lswaps(v))
#else
# define BE_SWAP32(v) (v)
# define BE_SWAP16(v) (v)
#endif


static __inline__ uint8_t xinb(XGIPtr pXGI, unsigned index)
{
    const uint8_t reg = (pXGI->noMMIO)
        ? inb(pXGI->PIOBase + index)
        : MMIO_IN8(pXGI->IOBase, index);

    return reg;
}

static __inline__ uint16_t xinw(XGIPtr pXGI, unsigned index)
{
    const uint16_t reg = (pXGI->noMMIO)
        ? inw(pXGI->PIOBase + index)
        : MMIO_IN16(pXGI->IOBase, index);

    return BE_SWAP16(reg);
}

static __inline__ uint32_t xinl(XGIPtr pXGI, unsigned index)
{
    const uint32_t reg = (pXGI->noMMIO)
        ? inl(pXGI->PIOBase + index)
        : MMIO_IN32(pXGI->IOBase, index);

    return BE_SWAP32(reg);
}

static __inline__ void xoutb(XGIPtr pXGI, unsigned index, uint8_t data)
{
    if (pXGI->noMMIO) {
        outb(pXGI->PIOBase + index, data);
    } else {
        MMIO_OUT8(pXGI->IOBase, index, data);
    }
}

static __inline__ void xoutw(XGIPtr pXGI, unsigned index, uint16_t data)
{
    if (pXGI->noMMIO) {
        outw(pXGI->PIOBase + index, BE_SWAP16(data));
    } else {
        MMIO_OUT16(pXGI->IOBase, index, BE_SWAP16(data));
    }
}

static __inline__ void xoutl(XGIPtr pXGI, unsigned index, uint32_t data)
{
    if (pXGI->noMMIO) {
        outl(pXGI->PIOBase + index, BE_SWAP32(data));
    } else {
        MMIO_OUT32(pXGI->IOBase, index, BE_SWAP32(data));
    }
}

static __inline__ uint8_t xinb3x5(XGIPtr pXGI, unsigned index)
{
    xoutb(pXGI, 0x3d4, index);
    return xinb(pXGI, 0x3d5);
}

static __inline__ void xoutb3x5(XGIPtr pXGI, unsigned index, uint8_t data)
{
    xoutb(pXGI, 0x3d4, index);
    xoutb(pXGI, 0x3d5, data);
}

static __inline__ uint8_t xinb3c5(XGIPtr pXGI, unsigned index)
{
    xoutb(pXGI, 0x3c4, index);
    return xinb(pXGI, 0x3c5);
}

static __inline__ void xoutb3c5(XGIPtr pXGI, unsigned index, uint8_t data)
{
    xoutb(pXGI, 0x3c4, index);
    xoutb(pXGI, 0x3c5, data);
}

static __inline__ uint8_t xinb3cf(XGIPtr pXGI, unsigned index)
{
    xoutb(pXGI, 0x3ce, index);
    return xinb(pXGI, 0x3cf);
}

static __inline__ void xoutb3cf(XGIPtr pXGI, unsigned index, uint8_t data)
{
    xoutb(pXGI, 0x3ce, index);
    xoutb(pXGI, 0x3cf, data);
}

static __inline__ void xoutw3x5(XGIPtr pXGI, unsigned index, uint16_t data)
{
    xoutb(pXGI, 0x3d4, index);
    xoutb(pXGI, 0x3d5, (uint8_t)data);
    xoutb(pXGI, 0x3d4, index+1);
    xoutb(pXGI, 0x3d5, (uint8_t)(data>>8));
}

static __inline__ void vAcquireRegIOProtect(XGIPtr pXGI)
{
    /* unprotect all register except which protected by 3c5.0e.7 */
    xoutb3c5(pXGI, 0x11, 0x92);
}

#define INB(port)               xinb(pXGI, port)
#define INW(port)               xinw(pXGI, port)
#define INDW(port)              xinl(pXGI, port)
#define OUTB(port, data)        xoutb(pXGI, port, data)
#define OUTW(port, data)        xoutw(pXGI, port, data)
#define OUTDW(port, data)       xoutl(pXGI, port, data)

#define OUT3X5B(index, data)    xoutb3x5(pXGI, index, data)
#define OUT3X5W(index, data)    xoutw3x5(pXGI, index, data)
#define OUT3C5B(index, data)    xoutb3c5(pXGI, index, data)
#define OUT3CFB(index, data)    xoutb3cf(pXGI, index, data)

#define IN3C5B(index)           xinb3c5(pXGI, index)
#define IN3X5B(index)           xinb3x5(pXGI, index)
#define IN3CFB(index)           xinb3cf(pXGI, index)

#endif

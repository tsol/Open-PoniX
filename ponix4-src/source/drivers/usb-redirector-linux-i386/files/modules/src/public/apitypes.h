/*
 *
 *  Copyright (C) 2007-2014 SimplyCore, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 */

#ifndef IlIIlIlll
#define IlIIlIlll
#include <linux/types.h>
typedef int8_t IIlIIllIl;typedef int16_t llIIllIlI;typedef int32_t llIlIIIII;
typedef int64_t IlIlIIIIl;typedef u_int8_t lllIl;typedef u_int16_t IIIlIl;
typedef u_int32_t IIIlll;typedef u_int64_t lIlIIlI;typedef int BOOL;
#ifndef __KERNEL__
#include <semaphore.h>
typedef sem_t*IllIlllll;
#endif
#ifndef TRUE
#define TRUE (0xa37+2435-0x13b9)
#endif
#ifndef FALSE
#define FALSE (0xf4d+1872-0x169d)
#endif
#ifndef __KERNEL__
#include <endian.h>
#include <byteswap.h>
#if __BYTE_ORDER == __BIG_ENDIAN
#define IlIlIIllI(IlIIlI) llllIllIl((IlIIlI))
#define llIllIIll(IlIIlI) IlIIIllII((IlIIlI))
#define lllIIIllI(IlIIlI) llIlIIllI((IlIIlI))
#define IIllIIlll(IlIIlI) llllIllIl((IlIIlI))
#define IlIIllIIl(IlIIlI) IlIIIllII((IlIIlI))
#define IlIIllIlI(IlIIlI) llIlIIllI((IlIIlI))
#else
#define IlIlIIllI(IlIIlI) (IlIIlI)
#define llIllIIll(IlIIlI) (IlIIlI)
#define lllIIIllI(IlIIlI) (IlIIlI)
#define IIllIIlll(IlIIlI) (IlIIlI)
#define IlIIllIIl(IlIIlI) (IlIIlI)
#define IlIIllIlI(IlIIlI) (IlIIlI)
#endif
# endif
#endif 

